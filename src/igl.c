/*
 * igl.c
 *
 * initialization and miscellanous functionality
 *
 *
 * Copyright (c) 2004 Matevz Bradac
 * Copyright (c) 2006-2016 Kai-Uwe Bloem
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "igl.h"
#include "iglcmn.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <dlfcn.h>


/* helper macro for error exits */
#define FCN_EXIT(condition_, ret_, action_) \
{                                           \
    if (condition_)                         \
    {                                       \
        ret = ret_;                         \
        action_;                            \
        goto exit;                          \
    }                                       \
}

/* globals */
igl_globalsT *igl;

#if PLATFORM_X11
#if 0
____________________________ X11 common stuph ____________________________
#endif
/* TODO: better use one of the available extension helpers for this? */
void *dlHandle;

/* helper functions for OpenGL swapcontrol extension */
static void
(*_igl_getOglFunc (const char *fn))()
{
    if (dlHandle == NULL)
        dlHandle = dlopen ("libGL.so", RTLD_NOW);
    return (void (*)())(dlHandle ? dlsym (dlHandle, fn) : NULL);
}

static void
_igl_swapIntervalSGI (Display *dpy, GLXDrawable drw, int val)
{
    /* helper for SGI swap extension which has different arguments */
    igl->swapIntervalSGI (val);
}

static void
_igl_X11swapFunc (void)
{
    const char *cstr = glXQueryExtensionsString (IGL_DISPLAY, IGL_SCREEN);

    /* check which swap extension is available, if any. TODO: replace strstr */
    if (strstr (cstr, "GLX_EXT_swap_control"))
        igl->swapInterval = _igl_getOglFunc ("glXSwapIntervalEXT");
    else if (strstr (cstr, "GLX_SGI_swap_control"))
        igl->swapIntervalSGI = _igl_getOglFunc ("glXSwapIntervalSGI");
    else if (strstr (cstr, "GLX_MESA_swap_control"))
        igl->swapIntervalSGI = _igl_getOglFunc ("glXSwapIntervalMESA");

    if (igl->swapIntervalSGI)
        igl->swapInterval = _igl_swapIntervalSGI;
}

static void
_igl_swapInterval (int val)
{
    /* call swap extension if available */
    if (igl->swapInterval)
        igl->swapInterval (IGL_DISPLAY, IGL_CTX ()->wnd, val);
}


/* attributes for NORMALDRAW layer */
static int glx_attribs_level0[] = {
    GLX_LEVEL, 0,
    GLX_DOUBLEBUFFER,
    GLX_RGBA,
    GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 24,
    GLX_STENCIL_SIZE, 8,
    GLX_ACCUM_RED_SIZE, 16, GLX_ACCUM_GREEN_SIZE, 16, GLX_ACCUM_BLUE_SIZE, 16,
            GLX_ACCUM_ALPHA_SIZE, 16, 
#if 0 /* GLX_SAMPLE_BUFFERS_SGIS */
    GLX_SAMPLE_BUFFERS_SGIS, 1,
    GLX_SAMPLES_SGIS, 4,
#endif
#if 0
    GLX_STEREO,
#endif
    None
};

/* attributes for all other layers */
static int glx_attribs[] = {
    GLX_LEVEL, 0,   /* level to be filled in by visual probing */
    GLX_DOUBLEBUFFER,
    GLX_RGBA,       /* IGL emulates color indexing, hence this must be RGB */
    GLX_RED_SIZE, 4, GLX_GREEN_SIZE, 4, GLX_BLUE_SIZE, 4, GLX_ALPHA_SIZE, 4,
    None
};


/* X11 system initialization */
static int
_igl_systemInit (HINSTANCE disp)
{
    char *font = getenv ("IGL_FONT");
    int ret = 0;
    int tmp[2];
    int i;

    /* open display and check for OpenGL */
    XInitThreads ();
    igl->appInstance = XOpenDisplay (NULL);
    FCN_EXIT (igl->appInstance == NULL, -1,
            printf ("XOpenDisplay() failed!\n"));

    FCN_EXIT (!glXQueryExtension (IGL_DISPLAY, &tmp[0], &tmp[1]), -1,
            printf ("glXQueryExtension() failed!\n"));

    /* TODO: rewrite the visual handling, it's too rigid, unfit and incorrect */

    /* setup OpenGL visuals */
    XMatchVisualInfo (IGL_DISPLAY, DefaultScreen (IGL_DISPLAY), 32, TrueColor, &igl->visualARGB);
    XMatchVisualInfo (IGL_DISPLAY, DefaultScreen (IGL_DISPLAY), 24, DirectColor, &igl->visualCmap);

    /* TODO: emulate layers with pbuffer, draw to texture? */
    for (i = 0; i < IGL_WLAYER_MAX; i++)
    {
        glx_attribs[1] = i - IGL_WLAYER_NORMALDRAW;
        igl->visualInfo[i] = glXChooseVisual (IGL_DISPLAY, DefaultScreen (IGL_DISPLAY),
                (glx_attribs[1] ? glx_attribs : glx_attribs_level0));
        /* if we don't have layer support, try to emulate */
        if (igl->visualInfo[i] == NULL && i != IGL_WLAYER_NORMALDRAW)
            igl->visualInfo[i] = &igl->visualARGB;
    }
    FCN_EXIT (igl->visualInfo[IGL_WLAYER_NORMALDRAW] == NULL, -1,
            printf ("glXChooseVisual () failed!\n"));

    /* setup miscellanous stuff */
    _igl_X11swapFunc ();

    if (igl->debug)
        XSynchronize (igl->appInstance, 1);

    /* setup system font. GL prog.man. says 9x16 px, but that looks quite bad */
    if (font == NULL)
        font = "-*-fixed-medium-r-normal-*-14-*-*-*-*-*-iso8859-1";
    _igl_defineXfont (0, font);
    FCN_EXIT (igl->fontDefs->xfont == NULL, -1,
            printf ("can't load system font %s!\n", font));

    /* cache some often used getgdesc values */
    igl->gdXPMAX = DisplayWidth (IGL_DISPLAY, IGL_SCREEN);
    igl->gdYPMAX = DisplayHeight (IGL_DISPLAY, IGL_SCREEN);
    igl->gdXMMAX = DisplayWidthMM (IGL_DISPLAY, IGL_SCREEN);
    igl->gdYMMAX = DisplayHeightMM (IGL_DISPLAY, IGL_SCREEN);

#ifdef __sgi
{
    /* hack for qemu_irix */
    extern int _getpwent_no_yp;
    _getpwent_no_yp = 1;
}
#endif

exit:
    return ret;
}


static void
_igl_systemExit (void)
{
    if (dlHandle)
        dlclose (dlHandle);
    dlHandle = NULL;

    if (IGL_DISPLAY != NULL)
        XCloseDisplay (IGL_DISPLAY);
}
#endif      /* PLATFORM_X11 */


/* basic exitialization */
static void
_igl_exit (void)
{
    /* nothing to do if not initialized */
    if (igl == NULL)
        return;

    /* cleanup internal structures */
    if (igl->nurbsCurve != NULL)
        gluDeleteNurbsRenderer (igl->nurbsCurve);

    _igl_exitObjectApi ();
    _igl_releaseObjects ();

    /* exit window system */
    _igl_systemExit ();

    /* do away with the global context */
    if (igl != NULL)
        free (igl);
    igl = NULL;
}


/* basic initialization */
void
_igl_init (HINSTANCE app)
{
    int ret = 0, queryWindowId;
    char *env_str;

    /* don't do this more than once */
    if (igl)
        return;

    /* allocate igl context */
    igl = (igl_globalsT *)calloc (1, sizeof (igl_globalsT));
    FCN_EXIT (igl == NULL, -1,
            printf ("malloc() failed!\n"));

    /* check environment options. TODO: real parsing and error checking */
    if (getenv("IGL_VERSION"))
        printf("IGL version %s\n", IGL_VERSION);

    if ((env_str = getenv("IGL_DEBUG")) != NULL)
        igl->debug = strtol(env_str, NULL, 10);

    if ((env_str = getenv("IGL_YIELD")) != NULL)
        igl->yieldTime = strtol(env_str, NULL, 10);
    else
        igl->yieldTime = 1000; /* 1 ms */

    igl->winDefWidth = IGL_WINDOWWIDTH;
    igl->winDefHeight = IGL_WINDOWHEIGHT;
    if ((env_str = getenv("IGL_WINSIZE")) != NULL)
        sscanf(env_str, "%ux%u", &igl->winDefWidth, &igl->winDefHeight);

    if (getenv("IGL_NOLAYERS") != NULL)
        igl->flags |= IGL_IFLAGS_NOLAYERS;

    if ((env_str = getenv("IGL_COMPAT")) != NULL)
    {
        igl->glcSOFTATTACH = (strstr (env_str, "SOFTATTACH") != NULL);
        igl->glcSLOWMAPCOLORS = (strstr (env_str, "SLOWMAPCOLORS") != NULL);
    }

    /* system dependent initialization */
    FCN_EXIT (_igl_systemInit (app), -1,
            printf ("systemInit() failed!\n"));

    /* object-related */
    FCN_EXIT (_igl_initObjectApi () != 0, -1, {});
    FCN_EXIT (_igl_initObjects () != 0, -1, {});

    /* color-related */
    _igl_initColor ();

    /* TODO: move structure init to init functions in modules */

    /* vertex-related */
    igl->nurbsCurve = gluNewNurbsRenderer ();

    /* drawing-related */
    IGL_MAPINIT (igl->lineStyles, IGL_MAXLINESTYLES, 1);
    IGL_MAPINIT (igl->patterns, IGL_MAXPATTERNS, 1);
    IGL_MAPINIT (igl->matrixDefs, IGL_MAXMATRICES, 0);

    /* light-related */
    _igl_setLmDefaults (DEFMATERIAL, igl->materialDefs);
    _igl_setLmDefaults (DEFLIGHT, igl->lightDefs);
    _igl_setLmDefaults (DEFLMODEL, igl->lmodelDefs);
    IGL_MAPINIT (igl->materialDefs, IGL_MAXLMDEFS, 1);
    IGL_MAPINIT (igl->lightDefs, IGL_MAXLMDEFS, 1);
    IGL_MAPINIT (igl->lmodelDefs, IGL_MAXLMDEFS, 1);

    /* pixel related */
    IGL_MAPINIT (igl->fontDefs, IGL_MAXFONTS, 1);

    /* device related */
    igl->deviceQueue[INPUTCHANGE] = igl->deviceQueue[REDRAW] = 1;
    gettimeofday (&igl->lastTime, NULL);
    igl->tiedValuators[CURSORX*2] = MOUSEX;
    igl->tiedValuators[CURSORY*2] = MOUSEY;
    igl->deviceNoise[TIMER0] = igl->deviceNoise[TIMER1] = 
    igl->deviceNoise[TIMER2] = igl->deviceNoise[TIMER3] = 10000;
    IGL_MAPINIT (igl->cursors, IGL_MAXCURSORS, 1);

    /* texture related */
    igl->tevDefs[0].tvMode = GL_MODULATE;
    IGL_MAPINIT (igl->tevDefs, IGL_MAXTEVDEFS, 1);
    IGL_MAPINIT (igl->texDefs, IGL_MAXTEXDEFS, 1);

    /* buffer related */
    igl->gdZBITS = IGL_ZBITS;
    igl->gdZMIN  = 0;
    igl->gdZMAX  = (1 << IGL_ZBITS) - 1;

    /* open the hidden query window - used for getgdesc() */
    igl->flags |= IGL_IFLAGS_NODEBUG;
    noport ();
    queryWindowId = winopen ("");
    igl->flags &= ~IGL_IFLAGS_NODEBUG;
    if (queryWindowId != -1)
        igl->queryWindow = IGL_WINDOW (queryWindowId);
    igl->currentWindow = -1;
    return;

    /* error exit */
exit:
    _igl_exit ();

    exit (ret);
}


#ifndef __sgi
/* IRIX: sginap - timed sleep and processor yield function */
long
sginap (long nap)
{
    glFlush ();
    if (nap)
        usleep (nap*10000);
    else
        sched_yield ();
    /* TODO: "returns the number of unexpired ticks" */
    return 0;
}
#else
unsigned
sleep (unsigned secs)
{
    /* kludge for SGI's GL programming examples */
    glFlush ();
    return sginap (secs*100) / 100;
}
#endif


#define padd(p,x) (short *)((char *)p + (x))
#define pmap(x) padd(map, mapOffset*(x))
/* initialize id mapping for the map */
void
_igl_mapIDinit (short *map, int mapSize, int mapOffset, int nonzero)
{
    int idx;
    short *p;

    /* treat map as hash table with linear collision probing for some speedup */
    for (idx = !!nonzero, p = pmap(idx); idx < mapSize; idx++, p = padd(p,mapOffset))
        *p = -1;
}

/* map IrisGL id to array index. flags bit 0: alloc, flags bit 1: id non-0 */
int
_igl_mapIDalloc (short *map, int mapSize, int mapOffset, short id, int flags)
{
    int idx, empty = -1;
    short *p;

    /* NOTE: this only works as intended if mapSize is a power of 2  */

    if (id < 0 || (id == 0 && (flags & 2)))
        return -1;

    /* treat map as hash table with linear collision probing for some speedup */
    for (idx = id & (mapSize-1), p = pmap(idx); idx < mapSize; idx++, p = padd(p,mapOffset))
    {
        if (*p == id)
            return idx;
        if (empty < 0 && *p < 0)
            empty = idx;
    }
    for (idx = 0, p = pmap(idx); idx < (id & (mapSize-1)); idx++, p = padd(p,mapOffset))
    {
        if (*p == id)
            return idx;
        if (empty < 0 && *p < 0)
            empty = idx;
    }

    /* not found, create new entry if requested */
    if ((flags & 1) && empty >= 0)
    {
        *pmap(empty) = id;
        return empty;
    }

    return -1;
}
#undef padd
#undef pmap


/* if no swap control is available, simulate 60 frames/sec */
static void
_igl_swapTimer (igl_windowT *wptr)
{
    if (wptr->swapInterval && !igl->swapInterval)
    {
        struct timeval next, now;
        int oldCounter = wptr->swapCounter;

        gettimeofday (&now, NULL);

        /* initialize timer on first call */
        if (! wptr->swapTime.tv_sec)
        {
            wptr->swapTime = now;
            return;
        }

        next = wptr->swapTime;

        /* skip lost frames, but update frame counter */
        while (now.tv_sec > next.tv_sec ||
                (now.tv_sec == next.tv_sec && now.tv_usec > next.tv_usec))
        {
            if (next.tv_sec < now.tv_sec-1 - wptr->swapInterval/60)
            {
                /* fast forward timer and counter if we are seconds away */
                wptr->swapCounter +=
                        (now.tv_sec-1 - wptr->swapInterval/60 - next.tv_sec)*60;
                next.tv_sec = now.tv_sec-1 - wptr->swapInterval/60;
            }
            else
            {
                wptr->swapCounter ++;
                next.tv_usec += 1000000/60 * wptr->swapInterval;
                while (next.tv_usec > 1000000)
                    next.tv_sec ++, next.tv_usec -= 1000000;
            }
        }

        if (oldCounter == wptr->swapCounter-1)
        {
            /* no frames lost, wait for start of next frame */
            struct timespec tv;
            tv.tv_sec = next.tv_sec - now.tv_sec;
            tv.tv_nsec = (next.tv_usec - now.tv_usec) * 1000;
            if (next.tv_usec < now.tv_usec)
                tv.tv_sec --, tv.tv_nsec += 1000000000; 
            nanosleep (&tv, &tv);
        }
        else
        {
            /* lost frame. rewind to current frame, no wait */
            wptr->swapCounter --;
            next.tv_usec -= 1000000/60 * wptr->swapInterval;
            while (next.tv_usec < 0)
                next.tv_sec --, next.tv_usec += 1000000;
        }
        wptr->swapTime = next;
    }
    else
    {
        /* there is no frame counter in OpenGL's swapcontrol, so just fake it */
        wptr->swapCounter ++;
    }
}


/* trace function. The braces are needed for the macro overloading. */
void (_igl_trace)(const char *func, int immediate, const char *fmt, ...)
{
    va_list args;
    char objmark[2] = "\0";

    if (!igl)
        _igl_init (NULL);
    if (igl->flags & IGL_IFLAGS_NODEBUG)
        return;

    if (igl->debug & 1)
    {
        /* if function marked as "objectable", check if in object definition */
        if (!immediate && igl->api == igl->objApi)
            objmark[0] = '*';
        printf ("%ld %s%s(",(long)getpid (),objmark,func);
        va_start (args, fmt);
        vprintf (fmt, args);
        va_end (args);
        printf (")\n");
        fflush (stdout);
    } 

    if (igl->debug & 2)
        glFlush ();
}


#if 0
____________________________ misc IrisGL stuph ____________________________
#endif
static void
_igl_resetGL (void)
{
    /* TODO: mummified code... rewrite this... merge with attach in window.c? */
    igl->flags |= IGL_IFLAGS_NODEBUG;

    singlebuffer ();
    onemap ();
    cmode ();
    gconfig ();

    drawmode (NORMALDRAW);

    /* TODO: mmode (MSINGLE)? */
    viewport (0, igl->gdXPMAX-1, 0, igl->gdYPMAX-1);
    ortho2 (-0.5, igl->gdXPMAX-0.5, -0.5, igl->gdYPMAX-0.5);
    picksize (10,10);

    blink (-1, 0, 0, 0, 0);
    setcursor (0, 0, 0);
    curson ();

    wmpack (-1);
    logicop (LO_SRC);

    zbuffer (FALSE);
    lsetdepth (igl->gdZMIN, igl->gdZMAX);
    depthcue (FALSE);
    lshaderange (0, 7, igl->gdZMIN, igl->gdZMAX);

    font (0);

    setlinestyle (0);
    linewidth (1);
    lsrepeat (1);
    lsbackup (FALSE);
    resetls (TRUE);
    setpattern (0);

    backface (FALSE);
    concave (FALSE);
    shademodel (GOURAUD);

    /* TODO: from AIX GL tech.ref.: "removes all button, valuator, and keyboard
     * entries from the event queue", "buttons are set to FALSE and untied".
     * Is this also the case in IrisGL?
     */

    igl->flags &= ~IGL_IFLAGS_NODEBUG;
}


/* GL: greset - resets graphics state */
void
greset (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    igl->flags |= IGL_IFLAGS_NODEBUG;
    _igl_resetGL ();
    _igl_resetColor ();
    igl->flags &= ~IGL_IFLAGS_NODEBUG;
}


/* GL: ginit, gbegin - create a window that occupies the entire screen */
void
gbegin (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    igl->flags |= IGL_IFLAGS_NODEBUG;
    prefsize (igl->gdXPMAX, igl->gdYPMAX);
    winopen ("");
    _igl_resetGL ();
    /* NOTE: gbegin doesn't reset the colormap */
    igl->flags &= ~IGL_IFLAGS_NODEBUG;
}


void
ginit (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    igl->flags |= IGL_IFLAGS_NODEBUG;
    prefsize (igl->gdXPMAX, igl->gdYPMAX);
    winopen ("");
    _igl_resetGL ();
    _igl_resetColor ();
    igl->flags &= ~IGL_IFLAGS_NODEBUG;
}


/* GL: gexit - exits graphics */
void
gexit (void)
{
    int i;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    /* cleanup && destroy all open windows */
    igl->flags |= IGL_IFLAGS_NODEBUG;
    for (i = 0; i < IGL_MAXWINDOWS; i++)
    {
        if (IGL_WINDOW (i)->wnd != IGL_NULLWND)
            winclose (i);
    }
    igl->flags &= ~IGL_IFLAGS_NODEBUG;

    _igl_exit ();
}


/* GL: gversion - returns graphics hardware and library version information */
long
gversion (String v)
{
    char *vers = getenv("IGL_GVERSION");
/*
    NOTE: from gversion man page:
     Graphics Type                            String Returned
     B or G                                   GL4D-m.n
     GT                                       GL4DGT-m.n
     GTX                                      GL4DGTX-m.n
     VGX                                      GL4DVGX-m.n
     VGXT                                     GL4DVGXT-m.
     RealityEngine                            GL4DRE-m.n
     Personal Iris                            GL4DPI2-m.n
     Personal Iris with Turbo Graphics        GL4DPIT-m.n
     Personal Iris (early serial numbers)     GL4DPI-m.n
     Entry                                    GL4DLG-m.n
     XS                                       GL4DXG-m.n
     XS24                                     GL4DXG-m.n
     XZ                                       GL4DXG-m.n
     Elan                                     GL4DXG-m.n
     Extreme                                  GL4DXG-m.n
     Indy                                     GL4DNP-m.n
     Solid IMPACT                             GL4DSLD-m.n
     High IMPACT                              GL4DIMP-m.n
     Maximum IMPACT                           GL4DIMP-m.n
     InfiniteReality                          GL4DINF-m.n
*/

    _igl_trace (__func__,TRUE,"%p",v);
    IGL_CHECKINIT ();

    /* NOTE: return fake strings for Indigo2 High Impact if not set via env */
    if (vers != NULL)
        strcpy(v, vers);
    else
        strcpy(v, "GL4DIMP-0.0");
    return 0;
}


/* GL: glcompat - controls compatibility modes */
void
glcompat (long mode, long value)
{
    long *ptr = (long *)value;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%ld,%ld",mode,value);
    IGL_CHECKINIT ();
    wptr = (igl->currentWindow >= 0 ? IGL_CTX() : IGL_WINDOW(0));

    switch (mode)
    {
        case GLC_OLDPOLYGON:
            /* NOTE: if enabled, old-style drawing methods draw filled polygons
             * outlined (rect, circ, arc, poly). No support for this in OpenGL.
             */
            break;

        case GLC_ZRANGEMAP:
            if (value) {
                igl->gdZBITS = 23;
                igl->gdZMAX  = (1 << 23) - 1;
            } else {
                igl->gdZBITS = IGL_ZBITS;
                igl->gdZMAX  = (1 << IGL_ZBITS) - 1;
            }
            break;

        case GLC_SLOWMAPCOLORS:
            igl->glcSLOWMAPCOLORS = (value != 0);
            break;

        case GLC_SOFTATTACH:
            igl->glcSOFTATTACH = (value != 0);
            break;

        /* the VSYNC stuff is needed for perfly, cimpact et al */
        case GLC_SET_VSYNC:
            wptr->swapCounter = value;
            break;
        case GLC_GET_VSYNC:
            *ptr = wptr->swapCounter;
            break;
        case GLC_VSYNC_SLEEP:
            do
                _igl_swapTimer (wptr);
            while (wptr->swapCounter % ptr[0] != ptr[1]);
            break;
    }
}


/* GL: getgdesc - gets graphics system description */
long
getgdesc (long inquiry)
{
    int buf[4] = { 0 };
    long ret;

    _igl_trace (__func__,TRUE,"%ld",inquiry);
    IGL_CHECKINIT ();

    /* NOTE: getgdesc() needs an open window, meaning that all queries using
     * glGet executed BEFORE a window is created fail miserably. the problem
     * lies in OpenGL, which can only be initialized after the window has been
     * created. to solve the problem, IGL creates a hidden window in
     * WinMain()/igl_X11main() to retrieve info from there...
     */
    /* TODO: do away with the query window; base values on glXGetConfig? */

    switch (inquiry)
    {
        case GD_XPMAX:                  ret = igl->gdXPMAX; break;
        case GD_YPMAX:                  ret = igl->gdYPMAX; break;

        case GD_XMMAX:                  ret = igl->gdXMMAX; break;
        case GD_YMMAX:                  ret = igl->gdYMMAX; break;

        /* GL simulates a 24 bit Z-buffer (see glcompat(GLC_ZRANGEMAP)) */
        case GD_ZMIN:                   ret = igl->gdZMIN; break;
        case GD_ZMAX:                   ret = igl->gdZMAX; break;

        /* this may or may not be true, but is there a way to query OpenGL? */
        case GD_BITS_ACBUF_HW:          ret = (1); break;

        case GD_BITS_ACBUF:
            glGetIntegerv (GL_ACCUM_RED_BITS, &buf[0]);
            glGetIntegerv (GL_ACCUM_GREEN_BITS, &buf[1]);
            glGetIntegerv (GL_ACCUM_BLUE_BITS, &buf[2]);
            glGetIntegerv (GL_ACCUM_ALPHA_BITS, &buf[3]);
            ret = (buf[0] + buf[1] + buf[2] + buf[3]);
            break;

        case GD_BITS_NORM_DBL_RED:
        case GD_BITS_NORM_SNG_RED:
            glGetIntegerv (GL_RED_BITS, buf);
            ret = (buf[0]);
            break;

        case GD_BITS_NORM_DBL_GREEN:
        case GD_BITS_NORM_SNG_GREEN:
            glGetIntegerv (GL_GREEN_BITS, buf);
            ret = (buf[0]);
            break;

        case GD_BITS_NORM_DBL_BLUE:
        case GD_BITS_NORM_SNG_BLUE:
            glGetIntegerv (GL_BLUE_BITS, buf);
            ret = (buf[0]);
            break;

        case GD_BITS_NORM_DBL_ALPHA:
        case GD_BITS_NORM_SNG_ALPHA:
            glGetIntegerv (GL_ALPHA_BITS, buf);
            ret = (buf[0]);
            break;

        case GD_BITS_NORM_ZBUFFER:
#if 0
            glGetIntegerv (GL_DEPTH_BITS, buf);
            ret = (buf[0]);
#else
            /* TODO: real bits, or mapped bits? see glcompat(GLC_ZRANGEMAP) */
            ret = (igl->gdZBITS);
#endif
            break;

        case GD_BITS_STENCIL:
            glGetIntegerv (GL_STENCIL_BITS, buf);
            ret = (buf[0]);
            break;

        /* only cursors with 1 color supported (no X11 support for 3) */
        case GD_BITS_CURSOR:            ret = (1); break;

        /* IGL_MAXCOLORS is 4096 = 2^12 */
        case GD_BITS_NORM_DBL_CMODE:
        case GD_BITS_NORM_SNG_CMODE:
        case GD_BITS_NORM_DBL_MMAP:
        case GD_BITS_NORM_SNG_MMAP:     ret = (12); break;

        /* offer 4 colors in each overlay */
        case GD_BITS_OVER_SNG_CMODE:
        case GD_BITS_PUP_SNG_CMODE:     ret = (4); break;

        /* no underlay support yet */
        case GD_BITS_UNDR_SNG_CMODE:    ret = (0); break;

        /* yes, IGL/OpenGL support alpha functions + they support all modes */
        case GD_AFUNCTION:
        case GD_AFUNCTION_MODES:        ret = (1); break;

        /* TBD */
        case GD_ALPHA_OVERUNDER:        ret = (0); break;

        /* i guess we support blending in all framebuffers */
        case GD_BLEND:                  ret = (1); break;

        case GD_BLENDCOLOR:             ret = (0); break;

        case GD_CIFRACT:                ret = (1); break;

        /* GL limit, OpenGL could do GL_MAX_CLIP_PLANES (>= 6) */
        case GD_CLIPPLANES:             ret = (6); break;

        case GD_CROSSHAIR_CINDEX:       ret = (WHITE); break;

        case GD_MUXPIPES:               ret = (0); break;

        /* dial buttons are not available */
        case GD_DBBOX:                  ret = (0); break;

        case GD_DITHER:                 ret = (1); break;

        case GD_FOGVERTEX:
        case GD_FOGPIXEL:               ret = (1); break;

        /* Live Video Digitizer isn't available */
        case GD_FRAMEGRABBER:           ret = (0); break;

        case GD_LIGHTING_ATT2:
        case GD_LIGHTING_SPOTLIGHT:
        case GD_LIGHTING_TWOSIDE:       ret = (1); break;

        /* in IGL colormap is simulated through RGB, so we say yes to colormap line smooting */
        case GD_LINESMOOTH_CMODE:
        case GD_LINESMOOTH_RGB:         ret = (1); break;

        case GD_LOGICOP:                ret = (1); break;

        /* not yet implemented */
        case GD_MULTISAMPLE:            ret = (0); break;

        case GD_NBLINKS:                ret = (0); break;

        case GD_NMMAPS:
            ret = (IGL_MAXCOLORS / IGL_MAXMULTIMAPCOLORS);
            break;

        /* number of screens on X11 systems is probably more than 1 */
        case GD_NSCRNS:                 ret = (1); break;

        /* is maximum order of nurbs curves/surfaces defined in OpenGL? */
        case GD_NURBS_ORDER:            ret = (10); break;

        /* GL limit, OpenGL could do GD_NOLIMIT */
        case GD_NVERTEX_POLY:           ret = (256); break;

        /* overlay and underlay planes are not shared */
        case GD_OVERUNDER_SHARED:       ret = (0); break;

        /* 64x64 pattern/stipple sizes aren't supported */
        case GD_PATSIZE_64:             ret = (0); break;

        /* point smoothing is available */
        case GD_PNTSMOOTH_CMODE:
        case GD_PNTSMOOTH_RGB:          ret = (1); break;

        case GD_POLYMODE:               ret = (1); break;

        case GD_POLYSMOOTH:             ret = (1); break;

        case GD_PUP_TO_OVERUNDER:       ret = (0); break;

        case GD_READSOURCE:             ret = (1); break;
        case GD_READSOURCE_ZBUFFER:     ret = (1); break;

        case GD_SCRBOX:                 ret = (0); break;

        case GD_SCRNTYPE:               ret = (GD_SCRNTYPE_WM); break;

        case GD_STEREO:                 ret = (0); break;

        /* not yet implemented */
        case GD_STEREO_IN_WINDOW:       ret = (0); break;

        case GD_SUBPIXEL_LINE:
        case GD_SUBPIXEL_PNT:
        case GD_SUBPIXEL_POLY:          ret = (1); break;

        case GD_TEXTPORT:               ret = (0); break;

        case GD_TEXTURE:                ret = (1); break;

        case GD_TEXTURE_3D:             ret = (1); break;

        case GD_TEXTURE_PERSP:
            /* since this is slower, only turn it on if somebody asks for it */
            glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
            ret = (1);
            break;

        case GD_TEXTURE_DETAIL:
        case GD_TEXTURE_LUT:
        case GD_TEXTURE_SHARP:          ret = (0); break;

        /* about 15 ms, says "GL Programming Guide" */
        case GD_TIMERHZ:                ret = (66); break;

        case GD_TRIMCURVE_ORDER:        ret = (3); break;

        case GD_WSYS:                   ret = (GD_WSYS_4S); break;

        case GD_ZDRAW_GEOM:             ret = (0); break;
        case GD_ZDRAW_PIXELS:           ret = (1); break;

        default:                        ret = (0); break;
    }

    return ret;
}


/* GL: gsync -  waits for a vertical retrace period */
void
gsync (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    /* TODO: is there a portable "wait for vsync" in OpenGL? */
    _igl_swapTimer (IGL_CTX ());
}


/* GL: swapinterval - defines a minimum time between buffer swaps */
void
swapinterval (short interval)
{
    _igl_trace (__func__,TRUE,"%d",interval);
    IGL_CHECKWNDV ();

    if (interval >= 0)
    {
        IGL_CTX ()->swapInterval = interval;
        _igl_swapInterval (interval);
    }
}


/* GL: swapbuffers - exchanges the front and back buffers of the normal framebuffer */
void
swapbuffers (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->swapbuffers ();
}

void
iglExec_swapbuffers(void)
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX();

    if ((wptr->flags & IGL_WFLAGS_DOUBLEBUFFER) && wptr->drawMode == NORMALDRAW)
    {
#if PLATFORM_X11
        glXSwapBuffers (igl->appInstance, wptr->wnd);
#endif
        /* IRIS GL resets some states affecting lighting and texturing */
        wptr->state &= ~(IGL_VSTATE_TEXTURED);
        wptr->state &= ~(IGL_VSTATE_N3F | IGL_VSTATE_N3FBGN);

        _igl_swapTimer (wptr);
    }
}


/* GL: mswapbuffers - swap multiple framebuffers simultaneously */
void
mswapbuffers (long fbuf)
{
    _igl_trace (__func__,FALSE,"%ld",fbuf);
    igl->api->mswapbuffers (fbuf);
}

void
iglExec_mswapbuffers (long fbuf)
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

#if PLATFORM_X11
    if (fbuf & UNDERDRAW)
        glXSwapBuffers (igl->appInstance, wptr->layers[IGL_WLAYER_UNDERDRAW].hwnd);
    if (fbuf & OVERDRAW)
        glXSwapBuffers (igl->appInstance, wptr->layers[IGL_WLAYER_OVERDRAW].hwnd);
    if (fbuf & NORMALDRAW)
    {
        glXSwapBuffers (igl->appInstance, wptr->layers[IGL_WLAYER_NORMALDRAW].hwnd);
        _igl_swapTimer (wptr);
    }
#endif

}


/* GL: pushattributes - pushes down the attribute stack */
void
pushattributes (void)
{
    _igl_trace(__func__,FALSE,"");
    igl->api->pushattributes ();
}

void
iglExec_pushattributes (void)
{
    IGL_CHECKWNDV ();

    glPushAttrib (GL_ALL_ATTRIB_BITS);
}


/* GL: popattributes - pops the attribute stack */
void
popattributes (void)
{
    _igl_trace(__func__,FALSE,"");
    igl->api->popattributes ();
}

void
iglExec_popattributes (void)
{
    IGL_CHECKWNDV ();

    glPopAttrib ();
}


/* GL: finish - blocks until the Geometry Pipeline is empty */
void
finish (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    glFinish ();
}


/* GL: gflush - flushs the remote GL client buffer */
void
gflush (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    glFlush ();
}


/* GL: ismex - obsolete routine */
Boolean
ismex (void)
{
    _igl_trace (__func__,TRUE,"");
    return TRUE;
}


#if 0
____________________________ statistic functions ____________________________
#endif
/* GL: minmax - modifies pixel transfers to compute the minimum and maximum pixel values */
void
minmax (unsigned long op, unsigned long val)
{
    _igl_trace (__func__,TRUE,"");

    switch (op)
    {
        case MINMAX_ENABLE:
            glMinmax (GL_MINMAX, GL_RGBA, GL_FALSE);
            glEnable (GL_MINMAX);
        break;

        case MINMAX_DISABLE:
            glDisable (GL_MINMAX);
        break;

        case MINMAX_INIT:
            glResetMinmax (GL_MINMAX);
        break;
    }
}


/* GL: getminmax - gets data computed by minmax */
void
getminmax (float *minmax)
{
    int format, type;

    _igl_trace (__func__,TRUE,"");

    /* pixmode(PM_INPUT_FORMAT) is used to specifiy the min/max pairs to get,
     * although this seems inconsistent. However, the data type is always float.
     */
    _igl_pixInMode(&format, &type);
    glGetMinmax (GL_MINMAX, GL_FALSE, format, GL_FLOAT, minmax);
}


/* GL: hgram - modifies pixel transfers to compute a histogram */
void
hgram (unsigned long op, unsigned long val)
{
    _igl_trace (__func__,TRUE,"");

    switch (op)
    {
        case HIST_ENABLE:
            glHistogram (GL_HISTOGRAM, val, GL_RGBA, GL_FALSE);
            glEnable (GL_HISTOGRAM);
        break;

        case HIST_DISABLE:
            glDisable (GL_HISTOGRAM);
        break;

        case HIST_CLEAR:
            glResetHistogram (GL_HISTOGRAM);
        break;
    }
}


/* GL: gethgram - gets histogram data computed by hgram */
void
gethgram (unsigned long hist[16384])
{
    _igl_trace (__func__,TRUE,"");
    glGetHistogram (GL_HISTOGRAM, GL_FALSE, GL_RGBA, GL_UNSIGNED_BYTE, hist);
}


/* GL: unimplemented stuff */
void
feedback (float buffer[], long size)
{
    _igl_trace (__func__,TRUE,"");
    /* TODO: this is heavily hw dependent in IrisGL! */
    /* glFeedbackBuffer (size, GL_3D_COLOR[_TEXTURE], buffer); */
    /* glRenderMode (GL_FEEDBACK); */
}


void
endfeedback (float buffer [])
{
    _igl_trace (__func__,TRUE,"");
    /* glRenderMode (GL_RENDER); */
}


void
passthrough (short token)
{
    _igl_trace (__func__,TRUE,"");
    /* glPassthrough (token); */
}


void
xfpt(Coord x, Coord y, Coord z)
{
    /* TODO: xfpt* is not available in OpenGL. Draw a point instead? */
    _igl_trace (__func__,TRUE,"");
}


void
xfpti(Icoord x, Icoord y, Icoord z)
{
    _igl_trace (__func__,TRUE,"");
}


void
xfpts(Scoord x, Scoord y, Scoord z)
{
    _igl_trace (__func__,TRUE,"");
}


void
xfpt2(Coord x, Coord y)
{
    _igl_trace (__func__,TRUE,"");
}


void
xfpt2i(Icoord x, Icoord y)
{
    _igl_trace (__func__,TRUE,"");
}


void
xfpt2s(Scoord x, Scoord y)
{
    _igl_trace (__func__,TRUE,"");
}


void
xfpt4(Coord x, Coord y, Coord z, Coord w)
{
    _igl_trace (__func__,TRUE,"");
}


void
xfpt4i(Icoord x, Icoord y, Icoord z, Icoord w)
{
    _igl_trace (__func__,TRUE,"");
}


void
xfpt4s(Scoord x, Scoord y, Scoord z, Scoord w)
{
    _igl_trace (__func__,TRUE,"");
}


void
subpixel (Boolean enable)
{
    _igl_trace (__func__,TRUE,"");
    /* in OpenGL subpixel is always true */
}


long
dglopen (String srvname, long type)
{
    _igl_trace (__func__,TRUE,"");
    return 0;
}


void
dglclose (long srvid)
{
    _igl_trace (__func__,TRUE,"");
}


long
gl_islocal (void)
{
    _igl_trace (__func__,TRUE,"");
    /* no DGL, hence i guess this is true? */
    return TRUE;
}
