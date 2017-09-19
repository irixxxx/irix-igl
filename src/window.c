/*
 * window.c
 *
 * window management, event processing
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


#if PLATFORM_X11
static void _igl_X11ConfigureLayers (igl_windowT *wptr);
static void _igl_X11MoveResize (igl_windowT *wptr);
#endif


#if 0
____________________________ helpers ____________________________
#endif


/* find a space for a new window in igl->openWindows. return -1 on error */
static int
_igl_findWindowPlaceholder (void)
{
    int i;

    for (i=0; i < IGL_MAXWINDOWS; i++)
    {
        if (igl->openWindows[i].wnd == IGL_NULLWND)
        {
            IGL_CLR (&igl->openWindows[i]);
            return (i);
        }
    }

    return (-1);
}


/* find ID of the window by HWND. searches only for main (decorated) window */
static short
_igl_findWindowByHandle (HWND wnd)
{
    short i;

    if (wnd != IGL_NULLWND)
    {
        for (i=0; i < IGL_MAXWINDOWS; i++)
        {
            if (igl->openWindows[i].wnd == wnd)
                return (i);
        }
    }

    return (-1);
}


/* find ID of the layer by HWND */
static short
_igl_findLayerByHandle (HWND wnd, short *layer)
{
    igl_layerT *lptr;
    short i, j;

    if (wnd != IGL_NULLWND)
    {
        for (i=0; i < IGL_MAXWINDOWS; i++)
        {
            if (igl->openWindows[i].wnd != IGL_NULLWND)
            {
                lptr = igl->openWindows[i].layers;
                for (j = 0; layer && j < IGL_WLAYER_CURSORDRAW; j++)
                {
                    if (lptr->hwnd == wnd)
                    {
                        *layer = j;
                        return (i);
                    }
                    lptr ++;
                }
            }
        }
    }

    *layer = -1;
    return (-1);
}


/* set the window parameters using the user settings stored in global context */
static void
_igl_setWinParams (igl_windowT *wptr)
{
    /* take over user settings */
    if (igl->winPosx != 0 || igl->winPosy != 0)
    {
        wptr->x = igl->winPosx;
        wptr->y = igl->winPosy;
    }
    if (igl->winWidth != 0 && igl->winHeight != 0)
    {
        wptr->width = igl->winWidth;
        wptr->height = igl->winHeight;
    }
    if (igl->winAspectX != 0 && igl->winAspectY != 0)
    {
        wptr->aspectX = igl->winAspectX;
        wptr->aspectY = igl->winAspectY;
    }
    if (igl->winStepX != 0 && igl->winStepY != 0)
    {
        wptr->stepX = igl->winStepX;
        wptr->stepY = igl->winStepY;
    }
    if (igl->winMinWidth != 0 || igl->winMinHeight != 0)
    {
        wptr->minWidth = igl->winMinWidth;
        wptr->minHeight = igl->winMinHeight;
    }
    if (igl->winMaxWidth != 0 || igl->winMaxHeight != 0)
    {
        wptr->maxWidth = igl->winMaxWidth;
        wptr->maxHeight = igl->winMaxHeight;
    }

    /* the window parameters can be inconsistent. How does GL deal with it? */
    /* anyway, for getsize to work correctly, w/h should be known here */
    if (wptr->width == 0)
        wptr->width = (wptr->minWidth ? wptr->minWidth : IGL_WINDOWWIDTH);
    if (wptr->height == 0)
        wptr->height = (wptr->minHeight ? wptr->minHeight : IGL_WINDOWHEIGHT);
    if (wptr->maxWidth > 0 && wptr->width > wptr->maxWidth)
        wptr->width = wptr->maxWidth;
    if (wptr->minWidth > 0 && wptr->width < wptr->minWidth)
        wptr->width = wptr->minWidth;
    if (wptr->maxHeight > 0 && wptr->height > wptr->maxHeight)
        wptr->height = wptr->maxHeight;
    if (wptr->minHeight > 0 && wptr->height < wptr->minHeight)
        wptr->height = wptr->minHeight;
    if (wptr->height > igl->gdYPMAX)
        wptr->height = igl->gdYPMAX;
    if (wptr->width > igl->gdXPMAX)
        wptr->width = igl->gdXPMAX;
    /* check if the parameter set fulfills aspect ratio settings */
    if (wptr->aspectX > 0 && wptr->aspectY > 0)
    {
        long height = wptr->width * wptr->aspectY / wptr->aspectX;
        long width = wptr->height * wptr->aspectX / wptr->aspectY;
        int hok = (height >= wptr->minHeight && height <= igl->gdYPMAX &&
                    (wptr->maxHeight <= 0 || height <= wptr->maxHeight));
        int wok = (width >= wptr->minWidth && width <= igl->gdXPMAX &&
                    (wptr->maxWidth <= 0 || width <= wptr->maxWidth));

        /* try to keep the calculated parameter between min and max, if set */
        if (wok)
            wptr->width = width;
        else if (hok)
            wptr->height = height;
        /* error case, try to rectify the min/max limits */
        else
        {
            wptr->height = height;
            if (wptr->maxHeight > 0 && wptr->height > wptr->maxHeight)
                wptr->maxHeight = wptr->height;
            if (wptr->minHeight > 0 && wptr->height < wptr->minHeight)
                wptr->minHeight = wptr->height;
        }
    }
}


/* attach a layer to the GL window */
static void
_igl_attachLayer (igl_windowT *wptr, int idx, long win)
{
#if PLATFORM_X11
    int x, y, xp, yp;
    unsigned int wp, hp, dummy;
    Window root, parent, child, *childs;
    XSetWindowAttributes wa;

    /* TODO: It's difficult to get a transparent window without using the
     * XRender extension. Use the window manager to achieve this, by reparenting
     * the layer to the root window. Does this work on a real SGI?
     */

    /* get the layer's position relative to the NORMALDRAW window */
    XTranslateCoordinates (IGL_DISPLAY, win, wptr->wnd, 0, 0,
                    &wptr->layers[idx].xoffs, &wptr->layers[idx].yoffs, &child);

    /* get the dimensions of the parent to get the real width/height */
    XQueryTree (IGL_DISPLAY, win,
                    &root, &parent, &childs, &dummy);
    if (childs)
        XFree (childs);
    XGetGeometry (IGL_DISPLAY, parent,
                    &root, &xp, &yp, &wp, &hp, &dummy, &dummy);

    /* get the absolute position of the NORMALDRAW window */
    XTranslateCoordinates (IGL_DISPLAY, wptr->wnd, IGL_ROOT, 0, 0,
                    &x, &y, &child);

    /* set override_redirect. no border, positioned above NORMALDRAW by us */
    wa.override_redirect = True;
    XChangeWindowAttributes (IGL_DISPLAY, win, CWOverrideRedirect, &wa);

    /* reparent to root and position above NORMALDRAW */
    x += wptr->layers[idx].xoffs, y += wptr->layers[idx].yoffs;
    XReparentWindow (IGL_DISPLAY, win, IGL_ROOT, x, y);

    wptr->layers[idx].hwnd = win;
    _igl_X11ConfigureLayers (wptr);

    /* TODO: IrisGL shares lots of stuff across the layers. This is rather
     * difficult to emulate with different contexts for every layer. However
     * using the same context for all layers imply all layers having the same
     * visual type, which might not be the case.
     * Shareable contexts share only the display lists, which isn't enough.
     * E.g. in IrisGL layers share the matrix stack, and all def* stuff.
     * Is there more? If so, what exactly?
     */

#if 0
    /* create the OpenGL context for the layer, sharing with NORMALDRAW */
    wptr->layers[idx].hrc = glXCreateContext (IGL_DISPLAY, igl->visualInfo[idx],
                                wptr->layers[IGL_WLAYER_NORMALDRAW].hrc, False);
#else
    /* share the OpenGL context among all layers */
    wptr->layers[idx].hrc = wptr->layers[IGL_WLAYER_NORMALDRAW].hrc;
#endif
#endif

    _igl_fontInit (wptr, idx);
}


/* detach a layer from the GL window */
static void
_igl_detachLayer (igl_windowT *wptr, int idx)
{
    unsigned int normal = IGL_WLAYER_NORMALDRAW;

    if (wptr->layers[idx].hwnd == IGL_NULLWND)
        return;

    /* if the current layer is detached, switch to NORMALDRAW layer */
    if (wptr->currentLayer == &wptr->layers[idx])
    {
        if (wptr == IGL_CTX ())
        {
#if PLATFORM_X11
            glXMakeCurrent (IGL_DISPLAY, wptr->layers[normal].hwnd, wptr->layers[normal].hrc);
#endif
        }
    }

    _igl_fontRelease (wptr, idx);

    /* destroy the OpenGL context for this layer */
#if PLATFORM_X11
    if (wptr->layers[idx].hrc != IGL_WINDOW (0)->layers[normal].hrc &&
                    wptr->layers[idx].hrc != wptr->layers[normal].hrc)
        glXDestroyContext (IGL_DISPLAY, wptr->layers[idx].hrc);
#endif

    wptr->layers[idx].hrc = IGL_NULLCTX;
    wptr->layers[idx].hwnd = IGL_NULLWND;
}


/* create a layer for the GL window */
static void
_igl_createLayer (igl_windowT *wptr, int idx)
{
#if PLATFORM_X11
    XSetWindowAttributes wattr;
    XVisualInfo *vi = igl->visualInfo[idx];
    Window win;
    int wflags;

    /* The CURSORDRAW layer doesn't really exist. Use the query window for it */
    if (idx == IGL_WLAYER_CURSORDRAW || (igl->flags & IGL_IFLAGS_NOLAYERS)) {
        wptr->layers[idx].hrc = IGL_WINDOW (0)->layers[IGL_WLAYER_NORMALDRAW].hrc;
        wptr->layers[idx].hwnd = IGL_WINDOW (0)->layers[IGL_WLAYER_NORMALDRAW].hwnd;
        wptr->layers[idx].cmap = IGL_WINDOW (0)->layers[IGL_WLAYER_NORMALDRAW].cmap;
        return;
    }
    wptr->layers[idx].cmap = XCreateColormap (IGL_DISPLAY, IGL_ROOT, vi->visual, AllocNone);

    /* create the layer window as subwindow of the OpenGL window */
    wattr.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
        ExposureMask | StructureNotifyMask | FocusChangeMask | PointerMotionMask;
    wattr.colormap = wptr->layers[idx].cmap;
    wattr.border_pixel = 0;
    wattr.background_pixel = 0;
    /* NOTE: need CWBorderPixel regardless of borderwidth 0 */
    wflags = CWEventMask | CWColormap| CWBorderPixel | CWBackPixel;

    win = XCreateWindow (IGL_DISPLAY, wptr->wnd, 0, 0, 2000, 2000, 0, vi->depth,
                    InputOutput, vi->visual, wflags, &wattr);

    /* attach the layer to the OpenGL window */
    _igl_attachLayer (wptr, idx, win);
    XMapWindow (IGL_DISPLAY, win);

    glXMakeCurrent (IGL_DISPLAY, wptr->layers[idx].hwnd, wptr->layers[idx].hrc);
#endif

    /* make the layer transparent */
    glPushAttrib (GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
    glDrawBuffer (GL_FRONT_AND_BACK);
    glClearColor (0, 0, 0, 0);
    glClear (GL_COLOR_BUFFER_BIT);
    glPopAttrib ();

#if PLATFORM_X11
    glXWaitGL ();
    /* back to the previous context */
    glXMakeCurrent (IGL_DISPLAY, wptr->currentLayer->hwnd, wptr->currentLayer->hrc);
#endif
}


/* destroy a layer of the GL window */
static void
_igl_destroyLayer (igl_windowT *wptr, int idx)
{
    _igl_detachLayer (wptr, idx);

    /* Don't do this if there's no window for this layer */
    if (wptr->layers[idx].hrc == IGL_WINDOW (0)->layers[IGL_WLAYER_NORMALDRAW].hrc) 
        return;

#if PLATFORM_X11
    if (wptr->layers[idx].hwnd == IGL_NULLWND)
        return;

    glXWaitGL ();
    XDestroyWindow (IGL_DISPLAY, wptr->layers[idx].hwnd);
    XFreeColormap (IGL_DISPLAY, wptr->layers[idx].cmap);
#endif
}


/* attach a system window as a GL window's NORMALDRAW layer */
static long
_igl_attachWindow (long idx, long win)
{
    igl_windowT *wptr;

    wptr = IGL_WINDOW (idx);
    wptr->wnd = win;

    /* init + set default window params */
    wptr->acPlanes = 64;    /* Win32 default i guess */
    wptr->stenPlanes = 0;   /* IrisGL default */
    wptr->zbPlanes = 24;    /* IrisGL default */
    wptr->rgbPlanes = 32;

#if PLATFORM_X11
    /* Create shareable context to share between the layers */
    wptr->layers[IGL_WLAYER_NORMALDRAW].hrc = glXCreateContext (IGL_DISPLAY,
                    igl->visualInfo[IGL_WLAYER_NORMALDRAW], NULL, False);
    if (wptr->layers[IGL_WLAYER_NORMALDRAW].hrc == IGL_NULLCTX)
        return (-1);

    glXMakeCurrent (IGL_DISPLAY, wptr->wnd, wptr->layers[IGL_WLAYER_NORMALDRAW].hrc);
#endif

    /* initialize layer management */
    wptr->drawMode = NORMALDRAW;
    wptr->layers[IGL_WLAYER_NORMALDRAW]._colorPalette = igl->normalColorPalette;
    wptr->layers[IGL_WLAYER_NORMALDRAW]._colormapSize = IGL_MAXCOLORS;
    wptr->layers[IGL_WLAYER_OVERDRAW]._colorPalette = igl->overColorPalette;
    wptr->layers[IGL_WLAYER_OVERDRAW]._colormapSize = 16;
    wptr->layers[IGL_WLAYER_PUPDRAW]._colorPalette = igl->popupColorPalette;
    wptr->layers[IGL_WLAYER_PUPDRAW]._colormapSize = 16;
    wptr->layers[IGL_WLAYER_UNDERDRAW]._colorPalette = igl->underColorPalette;
    wptr->layers[IGL_WLAYER_UNDERDRAW]._colormapSize = 16;
    wptr->layers[IGL_WLAYER_CURSORDRAW]._colorPalette = igl->cursorColorPalette;
    wptr->layers[IGL_WLAYER_CURSORDRAW]._colormapSize = 4;

    wptr->currentLayer = &wptr->layers[IGL_WLAYER_NORMALDRAW];
    wptr->currentLayer->hwnd = wptr->wnd;

    _igl_fontInit (wptr, IGL_WLAYER_NORMALDRAW);

    /* OpenGL heaven begins... set IrisGL defaults */
    glDrawBuffer (GL_FRONT);
    glDepthFunc (GL_LEQUAL);

    wptr->xzoom = wptr->yzoom = 1;
    wptr->pixFlags = 0;
    wptr->pixStride = 0;
    wptr->pixSize = 32;
    wptr->pixInFormat = wptr->pixOutFormat = PM_ABGR;
    wptr->pixInType = wptr->pixOutType = PM_UNSIGNED_BYTE;

    wptr->matrixMode = MSINGLE;
    glMatrixMode (GL_PROJECTION);

    gluOrtho2D(-0.5, wptr->width-0.5, -0.5, wptr->height-0.5);
    wptr->pickDeltax = wptr->pickDeltay = 10;

    wptr->patchCurvesU = wptr->patchCurvesV = 10;
    wptr->patchSegmentsU = wptr->patchSegmentsV = 10;
    wptr->curveSegments = 20;

    wptr->colorIndex = 7;
    wptr->currentColor[0] = wptr->currentColor[1] = wptr->currentColor[2] = 255;
    glColor4ubv (wptr->currentColor);

    glEnable (GL_RESCALE_NORMAL);

    return 0;
}


/* detach system window from a GL window */
static void
_igl_detachWindow(long gwid)
{
    igl_windowT *wptr;
    int i;

    /* find the new current window if the current one is to be closed */
    if (gwid == igl->currentWindow)
    {
        igl->currentWindow = -1;    /* in case this is the last window */
        for (i = 1, wptr = IGL_WINDOW (i); i < IGL_MAXWINDOWS; i++, wptr++)
        {
            if (wptr->wnd != IGL_NULLWND && i != gwid)
            {
                igl->currentWindow = i;
#if PLATFORM_X11
                glXMakeCurrent (IGL_DISPLAY, wptr->wnd, wptr->currentLayer->hrc);
#endif
                break;
            }
        }
        if (igl->currentWindow == -1)
            glXMakeCurrent (IGL_DISPLAY, None, NULL);
    }

    wptr = IGL_WINDOW (gwid);
    _igl_fontRelease (wptr, IGL_WLAYER_NORMALDRAW);
    if (igl->fmReleaseCB)
        igl->fmReleaseCB (wptr);

    /* destroy the OpenGL context associated with the window */
#if PLATFORM_X11
    glXDestroyContext (IGL_DISPLAY, wptr->layers[IGL_WLAYER_NORMALDRAW].hrc);
#endif

    wptr->wnd = IGL_NULLWND;
}


/* create a system window and assign a GL window to it */
static long
_igl_createWindow (char *winTitle, long parentId)
{
    int idx, wflags;
    unsigned int format;
    igl_windowT *wptr, *pptr;
#if PLATFORM_X11
    char *scrsave = getenv ("XSCREENSAVER_WINDOW");
    XSetWindowAttributes swAttribs;
    XSizeHints sizeHints;
#endif

    if (!winTitle) winTitle = "";

    /* get the GL window */
    idx = _igl_findWindowPlaceholder ();
    if (idx == -1)
        return (-1);
    wptr = IGL_WINDOW (idx);

    /* basic GL window initialization */
    wptr->parent = parentId;
    pptr = (parentId != -1) ? IGL_WINDOW (parentId) : NULL;
    if (pptr == NULL)
        strncpy (wptr->title, winTitle, 255);
    else
        strcpy (wptr->title, pptr->title);

    /* default window size, or if given use the size from environment */
    wptr->width = igl->winDefWidth;
    wptr->height = igl->winDefHeight;

    /* set window parameters from user settings */
    _igl_setWinParams (wptr);

#if PLATFORM_X11
    if (scrsave && parentId == -1 && idx == 1)
    {
        Window root, child;

        /* if this is running as screensaver, the window is already created.
         * get the window id from the environment, and its dimensions.
         */
        sscanf(scrsave, "0x%lx", &wptr->wnd);
        XGetGeometry (IGL_DISPLAY, wptr->wnd, &root, &wptr->x, &wptr->y,
                        &wptr->width, &wptr->height, &format, &format);
        XTranslateCoordinates (IGL_DISPLAY, wptr->wnd, IGL_ROOT, 0, 0,
                        &wptr->x, &wptr->y, &child);
    }
    else
    {
        /* create the new system window */
        wptr->layers[IGL_WLAYER_NORMALDRAW].cmap = XCreateColormap(IGL_DISPLAY,
            IGL_ROOT, IGL_VISUAL->visual, AllocNone);
        IGL_CLR (&swAttribs);
        swAttribs.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
            ExposureMask | StructureNotifyMask | FocusChangeMask | PointerMotionMask | PropertyChangeMask ;
        swAttribs.colormap = wptr->layers[IGL_WLAYER_NORMALDRAW].cmap;
        swAttribs.border_pixel = 0;
        swAttribs.background_pixel = 0;
        wflags = CWEventMask | CWColormap | CWBorderPixel | CWBackPixel;

        wptr->wnd = XCreateWindow (IGL_DISPLAY, IGL_XPARENT(wptr),
            wptr->x, wptr->y, wptr->width, wptr->height, 0, IGL_VISUAL->depth,
            InputOutput, IGL_VISUAL->visual, wflags, &swAttribs);
        if (wptr->wnd == IGL_NULLWND)
            return (-1);

        XStoreName (IGL_DISPLAY, wptr->wnd, wptr->title);

        /* inform the WM about the size equirements of this window */
        IGL_CLR (&sizeHints);

        if (igl->winPosx || igl->winPosy || igl->winWidth || igl->winHeight)
        {
            /* non-default windows aren't resizeable until winconstraints() is called */
            sizeHints.flags |= PMinSize | PMaxSize;
            sizeHints.min_width = sizeHints.max_width = wptr->width;
            sizeHints.min_height = sizeHints.max_height = wptr->height;
        }

        if (wptr->aspectX != 0 && wptr->aspectY != 0)
        {
            /* keepaspect() affects winopen() and winconstraints() */
            sizeHints.flags |= PAspect;
            sizeHints.min_aspect.x = sizeHints.max_aspect.x = wptr->aspectX;
            sizeHints.min_aspect.y = sizeHints.max_aspect.y = wptr->aspectY;
        }

        XSetWMNormalHints (IGL_DISPLAY, wptr->wnd, &sizeHints);
    }

    if (igl->windowFlags & IGL_WFLAGS_NOPORT)
    {
        /* reset for each window */
        wptr->flags |= IGL_WFLAGS_NOPORT;
        igl->windowFlags &= ~IGL_WFLAGS_NOPORT;
    }
    else if (!scrsave)
    {
        /* the xscreensaver window is already positioned */
        wptr->flags |= (igl->windowFlags & IGL_WFLAGS_BACKGROUND);
        igl->windowFlags &= ~IGL_WFLAGS_BACKGROUND;
        _igl_X11MoveResize (wptr);
    }
#endif

    /* attach system window to GL window and make it the active window */
    _igl_attachWindow (idx, wptr->wnd);

    igl->currentWindow = idx;
    swapinterval (1);

    /* reset the user setting to defaults since they have been consumed */
    igl->winPosx = igl->winPosy = 0;
    igl->winWidth = igl->winHeight = 0;
    igl->winMinWidth = igl->winMinHeight = igl->winMaxWidth = igl->winMaxHeight = 0;
    igl->winAspectX = igl->winAspectY = igl->winStepX = igl->winStepY = 0;

    return idx;
}


/* destroy a GL window */
static void
_igl_destroyWindow(long gwid)
{
    igl_windowT *wptr = IGL_WINDOW (gwid);
    HWND wnd = wptr->wnd;
    int i;

    /* TODO: close all (grand...)child windows opened by swinopen() */

    if (igl == NULL || wptr->wnd == IGL_NULLWND)
        return;

    for (i = 0; i < IGL_WLAYER_CURSORDRAW; i++)
    {
        if (i != IGL_WLAYER_NORMALDRAW)
            _igl_destroyLayer (wptr, i);
    }
    _igl_detachWindow (gwid);

#if PLATFORM_X11
    glXWaitGL ();
    XDestroyWindow (IGL_DISPLAY, wnd);
    XFreeColormap (IGL_DISPLAY, wptr->layers[IGL_WLAYER_NORMALDRAW].cmap);
#endif
}

#if PLATFORM_X11
#if 0
____________________________ X11 stuph ____________________________
#endif
#ifndef _NET_WM_STATE_ADD
#define _NET_WM_STATE_ADD       1
#define _NET_WM_STATE_REMOVE    0
#endif

/* send a NET_WM_STATE message to the WM */
static void
_igl_X11setwmstate (Window win, int on, char *prop1, char *prop2)
{
    XEvent xev;

    IGL_CLR(&xev);
    xev.xclient.type = ClientMessage;
    xev.xclient.display = IGL_DISPLAY;
    xev.xclient.window = win;
    xev.xclient.format = 32;
    xev.xclient.message_type = XInternAtom(IGL_DISPLAY, "_NET_WM_STATE", False);
    xev.xclient.data.l[0] = on ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
    xev.xclient.data.l[1] = XInternAtom(IGL_DISPLAY, prop1, False);
    if (prop2 != NULL)
        xev.xclient.data.l[2] = XInternAtom(IGL_DISPLAY, prop2, False);
    XSendEvent(IGL_DISPLAY, IGL_ROOT,
           False, SubstructureNotifyMask | SubstructureRedirectMask, &xev);
}

/* map or unmap all layers. called if visibility of main window changes */
static void
_igl_X11mapLayers (igl_windowT *wptr, int visible)
{
    int i;

    for (i = 0; i < IGL_WLAYER_CURSORDRAW; i++)
    {
        HWND win = wptr->layers[i].hwnd;
        if (i != IGL_WLAYER_NORMALDRAW && win &&
                    win != IGL_WINDOW (0)->layers[IGL_WLAYER_NORMALDRAW].hwnd)
        {
            if (visible)
                XMapWindow (IGL_DISPLAY, wptr->layers[i].hwnd);
            else
                XUnmapWindow (IGL_DISPLAY, wptr->layers[i].hwnd);
        }
    }
}

/* (re)position and (re)stack all layers to the main window */
static void
_igl_X11ConfigureLayers (igl_windowT *wptr)
{
    int x = 0, y = 0;
    igl_windowT *pptr;
    int i;
    Window bottom = IGL_NULLWND;

    /* calculate screen position of this (sub)window */
    for (pptr = wptr; pptr != NULL; pptr = IGL_PARENT (pptr))
        x += pptr->x, y += pptr->y;

    /* grab the server, else this may interfere with the WM */
    XGrabServer (IGL_DISPLAY);
    for (i = 0; i < IGL_WLAYER_CURSORDRAW; i++)
    {
        if (wptr->layers[i].hwnd != IGL_NULLWND &&
            wptr->layers[i].hwnd != IGL_WINDOW (0)->layers[IGL_WLAYER_NORMALDRAW].hwnd)
        {
            Window top, root, parent, *children;
            unsigned int nChild;
            XWindowChanges xwc;

            /* reposition this window */
            if (i != IGL_WLAYER_NORMALDRAW)
            {
                int xo = wptr->layers[i].xoffs, yo = wptr->layers[i].yoffs;
                XMoveResizeWindow (IGL_DISPLAY, wptr->layers[i].hwnd,
                        x + xo, y + yo, wptr->width - xo, wptr->height - yo);
            }

            /* can only stack siblings, get the topmost parent (WM supplied) */
            parent = wptr->layers[i].hwnd;
            do
            {
                top = parent;
                if (! XQueryTree (IGL_DISPLAY, top, &root, &parent, &children, &nChild))
                    break;
                if (children)
                    XFree (children);
            }
            while (parent != root);

            /* restack the layers directly above each other */
            if (bottom != IGL_NULLWND && bottom != top)
            {
                /* don't restack the decorated main window (managed by WM) */
                if (i == IGL_WLAYER_NORMALDRAW)
                {
                    xwc.sibling = top;
                    xwc.stack_mode = Below;
                    XConfigureWindow (IGL_DISPLAY, bottom, CWSibling|CWStackMode, &xwc);
                }
                else
                {
                    xwc.sibling = bottom;
                    xwc.stack_mode = Above;
                    XConfigureWindow (IGL_DISPLAY, top, CWSibling|CWStackMode, &xwc);
                }
            }
            bottom = top;
        }
    }
    XUngrabServer (IGL_DISPLAY);
}

/* move or resize the main window, setting the WM state accordingly */
static void
_igl_X11MoveResize (igl_windowT *wptr)
{
    XEvent xev;

    /* set window type to DESKTOP if imakebackground. Is there something better? */
    if ((wptr->flags & IGL_WFLAGS_BACKGROUND) && wptr->wnd != IGL_ROOT)
    {
        *xev.xclient.data.l = XInternAtom(IGL_DISPLAY, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
        XChangeProperty (IGL_DISPLAY, wptr->wnd,
                XInternAtom(IGL_DISPLAY, "_NET_WM_WINDOW_TYPE", False),
                XA_ATOM, 32, PropModeReplace, (unsigned char *)xev.xclient.data.l, 1);
    }

    if (wptr->x == 0 && wptr->y == 0 &&
        wptr->width >= igl->gdXPMAX-1 && wptr->height >= igl->gdYPMAX-1)
    {
        /* window covers root window, hence set fullscreen state */
        _igl_X11setwmstate (wptr->wnd, 1, "_NET_WM_STATE_FULLSCREEN", NULL);
    }
    else
    {
        /* otherwise remove fullscreen state */
        _igl_X11setwmstate (wptr->wnd, 0, "_NET_WM_STATE_MAXIMIZED_HORZ", "_NET_WM_STATE_MAXIMIZED_VERT");
        _igl_X11setwmstate (wptr->wnd, 0, "_NET_WM_STATE_FULLSCREEN", NULL);
    }

    /* NOTE: Unmap/Map needed, else WM will not perform the operation? */
    if (wptr->parent < 0)
        XUnmapWindow (IGL_DISPLAY, wptr->wnd);

    /* position and map the window and its layers */
    XMoveResizeWindow (IGL_DISPLAY, wptr->wnd, wptr->x, wptr->y, wptr->width, wptr->height);
    XMapWindow (IGL_DISPLAY, wptr->wnd);
    _igl_X11ConfigureLayers (wptr);
}


/* convert X11 msgs (key/mouse events) to IrisGL devices */
/* NOTE: X11 differentiates between XK_Q and XK_q (which sucks i guess). IGL doens't */
#define KEYTR(_key,_skey) case XK_##_key: case XK_##_skey: ret=_key##KEY; break
static int
_igl_X11deviceToIris (XEvent *event, short *state, char *key)
{
    KeySym ks;
    int ret = -1;

    *state = *key = 0;
    if (event->type == KeyPress || event->type == KeyRelease)
    {
        *state = (event->type == KeyPress);

        XLookupString (&event->xkey, key, 1, &ks, NULL);
        switch (ks)
        /* switch (XLookupKeysym (&event->xkey, 0)) */
        {
            KEYTR(A, a);
            KEYTR(B, b);
            KEYTR(C, c);
            KEYTR(D, d);
            KEYTR(E, e);
            KEYTR(F, f);
            KEYTR(G, g);
            KEYTR(H, h);
            KEYTR(I, i);
            KEYTR(J, j);
            KEYTR(K, k);
            KEYTR(L, l);
            KEYTR(M, m);
            KEYTR(N, n);
            KEYTR(O, o);
            KEYTR(P, p);
            KEYTR(Q, q);
            KEYTR(R, r);
            KEYTR(S, s);
            KEYTR(T, t);
            KEYTR(U, u);
            KEYTR(V, v);
            KEYTR(W, w);
            KEYTR(X, x);
            KEYTR(Y, y);
            KEYTR(Z, z);

            case XK_semicolon: ret = SEMICOLONKEY; break;
            case XK_period: ret = PERIODKEY; break;
            case XK_comma: ret = COMMAKEY; break;
            case XK_apostrophe: ret = QUOTEKEY; break;
            case XK_minus: ret = MINUSKEY; break;
            case XK_backslash: ret = BACKSLASHKEY; break;
            case XK_equal: ret = EQUALKEY; break;
            case XK_bracketleft: ret = LEFTBRACKETKEY; break;
            case XK_bracketright: ret = RIGHTBRACKETKEY; break;

            case XK_0: ret = ZEROKEY; break;
            case XK_1: ret = ONEKEY; break;
            case XK_2: ret = TWOKEY; break;
            case XK_3: ret = THREEKEY; break;
            case XK_4: ret = FOURKEY; break;
            case XK_5: ret = FIVEKEY; break;
            case XK_6: ret = SIXKEY; break;
            case XK_7: ret = SEVENKEY; break;
            case XK_8: ret = EIGHTKEY; break;
            case XK_9: ret = NINEKEY; break;

            case XK_KP_0: ret = PAD0; break;
            case XK_KP_1: ret = PAD1; break;
            case XK_KP_2: ret = PAD2; break;
            case XK_KP_3: ret = PAD3; break;
            case XK_KP_4: ret = PAD4; break;
            case XK_KP_5: ret = PAD5; break;
            case XK_KP_6: ret = PAD6; break;
            case XK_KP_7: ret = PAD7; break;
            case XK_KP_8: ret = PAD8; break;
            case XK_KP_9: ret = PAD9; break;
            case XK_KP_Decimal: ret = PADPERIOD; break;
            case XK_KP_Subtract: ret = PADMINUS; break;
            case XK_KP_Separator: ret = PADCOMMA; break;
            case XK_KP_Enter: ret = PADENTER; break;

            case XK_F1: ret = F1KEY; break;
            case XK_F2: ret = F2KEY; break;
            case XK_F3: ret = F3KEY; break;
            case XK_F4: ret = F4KEY; break;
            case XK_F5: ret = F5KEY; break;
            case XK_F6: ret = F6KEY; break;
            case XK_F7: ret = F7KEY; break;
            case XK_F8: ret = F8KEY; break;
            case XK_F9: ret = F9KEY; break;
            case XK_F10: ret = F10KEY; break;
            case XK_F11: ret = F11KEY; break;
            case XK_F12: ret = F12KEY; break;

            case XK_Escape: ret = ESCKEY; break;
            case XK_Tab: ret = TABKEY; break;
            case XK_Return: ret = RETKEY; break;
            case XK_space: ret = SPACEKEY; break;
            case XK_BackSpace: ret = BACKSPACEKEY; break;
            case XK_Left: ret = LEFTARROWKEY; break;
            case XK_Down: ret = DOWNARROWKEY; break;
            case XK_Right: ret = RIGHTARROWKEY; break;
            case XK_Up: ret = UPARROWKEY; break;
            case XK_Print: ret = PRINTSCREENKEY; break;
            case XK_Scroll_Lock: ret = SCROLLLOCKKEY; break;
            case XK_Pause: ret = PAUSEKEY; break;
            case XK_Insert: ret = INSERTKEY; break;
            case XK_Home: ret = HOMEKEY; break;
            case XK_End: ret = ENDKEY; break;
            case XK_Num_Lock: ret = NUMLOCKKEY; break;

            case XK_Alt_L: ret = LEFTALTKEY; break;
            case XK_Alt_R: ret = RIGHTALTKEY; break;
            case XK_Shift_L: ret = LEFTSHIFTKEY; break;
            case XK_Shift_R: ret = RIGHTSHIFTKEY; break;
            case XK_Control_L: ret = LEFTCTRLKEY; break;
            case XK_Control_R: ret = RIGHTCTRLKEY; break;
            case XK_Caps_Lock: ret = CAPSLOCKKEY; break;
        }
    }
    else if (event->type == ButtonPress || event->type == ButtonRelease)
    {
        *state = (event->type == ButtonPress);
        switch (event->xbutton.button)
        {
            case Button1:
                ret = LEFTMOUSE;
            break;

            case Button2:
                ret = MIDDLEMOUSE;
            break;

            case Button3:
            default:
                ret = RIGHTMOUSE;
            break;
        }
    }

    return (ret);
}
#undef KEYTR


void
_igl_X11processDevice (XEvent *event)
{
    int dev, devTie;
    short state;
    char key;

    dev = _igl_X11deviceToIris (event, &state, &key);
    if (ISBUTTON(dev))
        igl->deviceVal[dev] = state;
    /* if it's a KEYBD device and it's 'down', qenter KEYBD as well */
    if (/*ISKEYBD(dev)*/ key && state == 1 && igl->deviceQueue[KEYBD])
        _igl_qenter (KEYBD, (short)key, True);

    if (dev != -1 && igl->deviceQueue[dev])
    {
        /* if device is queued, we have to qenter() it */
        /* NOTE: state indicates whether a device is 'up' or 'down' */
        _igl_qenter ((Device)dev, state, True);

        /* if device is tied, we have to qenter() its ties */
        devTie = igl->tiedValuators[dev*2];
        if (devTie != NULLDEV)
        {
            _igl_qenter ((Device)devTie, (short)igl->deviceVal[devTie], True);

            devTie = igl->tiedValuators[dev*2+1];
            if (devTie != NULLDEV)
                _igl_qenter ((Device)devTie, (short)igl->deviceVal[devTie], True);
        }
    }
}


void
igl_X11processEvents (void)
{
    short wid = -1, lid;
    XEvent event;
    igl_windowT *wptr;

    _igl_glTimers ();
    /* XEvent processing */
    while (XPending (IGL_DISPLAY) > 0)
    {
        XNextEvent (IGL_DISPLAY, &event);
        _igl_trace (__func__,TRUE,"event %d", event.type);
        switch (event.type)
        {
            case ButtonPress:       /* mouse buttons */
            case ButtonRelease:
                igl->deviceVal[igl->tiedValuators[2*CURSORX]] =
                        igl->deviceVal[CURSORX] = event.xbutton.x_root;
                igl->deviceVal[igl->tiedValuators[2*CURSORY]] =
                        igl->deviceVal[CURSORY] = GL_Y(event.xbutton.y_root);

                /* If this event happens in one of the layer windows which igl
                 * controls for itself, the WM will not set the focus to it.
                 * Set focus to the window to activate it, else it would only
                 * be possible to activate the window by clicking on the border.
                 */
                wid = _igl_findLayerByHandle (event.xfocus.window, &lid);
                if (wid != -1 && wid != igl->activeWindow)
                    XSetInputFocus(IGL_DISPLAY, IGL_WINDOW (wid)->wnd,
                                        RevertToPointerRoot, CurrentTime);
                else
                    _igl_X11processDevice (&event);
                break;

            case KeyPress:          /* keyboard keys */
            case KeyRelease:
                _igl_X11processDevice (&event);

                /* The 4Sight Programmers Guide states to keep input focus
                 * as long as any key is pressed (called "follow focus"). So,
                 * grab the pointer here, as long as any key is pressed :-/
                 */
                if (igl->glcSOFTATTACH && event.type == KeyPress)
                    XGrabPointer(IGL_DISPLAY, IGL_CTX ()->wnd, False,
                            ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
                            GrabModeAsync, IGL_ROOT, None, CurrentTime);
                else if (event.type == KeyRelease)
                    XUngrabPointer(IGL_DISPLAY, CurrentTime);
                break;

            case MotionNotify:      /* mouse movements */
                igl->deviceVal[igl->tiedValuators[2*CURSORX]] =
                        igl->deviceVal[CURSORX] = event.xbutton.x_root;
                igl->deviceVal[igl->tiedValuators[2*CURSORY]] =
                        igl->deviceVal[CURSORY] = GL_Y(event.xbutton.y_root);
                if (event.xmotion.is_hint == NotifyNormal || event.xmotion.is_hint == NotifyHint)
                {
                    int devx = igl->tiedValuators[2*CURSORX];
                    int devy = igl->tiedValuators[2*CURSORY];
                    if (igl->deviceQueue[CURSORX])
                        _igl_qenter (CURSORX, (short)igl->deviceVal[CURSORX], True);
                    if (igl->deviceQueue[CURSORY])
                        _igl_qenter (CURSORY, (short)igl->deviceVal[CURSORY], True);
                    if (igl->deviceQueue[devx])
                        _igl_qenter (devx, (short)igl->deviceVal[devx], True);
                    if (igl->deviceQueue[devy])
                        _igl_qenter (devy, (short)igl->deviceVal[devy], True);
                }
                break;

            case Expose:            /* (part of) window exposed */
                if (event.xexpose.count == 0)
                {
                    wid = _igl_findLayerByHandle (event.xexpose.window, &lid);
                    if (wid != -1 && igl->deviceQueue[REDRAW])
                        _igl_qenter (REDRAW, wid, False);
                }
                break;

            case UnmapNotify:       /* window (un)mapped */
            case MapNotify:
                if (!event.xmap.override_redirect)
                {
                    /* main window (un)mapped - layers must follow */
                    wid = _igl_findWindowByHandle (event.xmap.window);
                    if (wid != -1)
                    {
                        wptr = IGL_WINDOW (wid);
                        wptr->state &= ~IGL_WSTATE_MAPPED;
                        if (event.type == MapNotify)
                            wptr->state |= IGL_WSTATE_MAPPED;
                        _igl_X11mapLayers (wptr, event.type == MapNotify);
                    }
                }
                break;

            case ConfigureNotify:   /* window position and/or size changed */
                if (!event.xconfigure.override_redirect)
                {
                    /* main window changed - get the new window position/size */
                    wid = _igl_findWindowByHandle (event.xconfigure.window);
                    if (wid != -1)
                    {
                        Window child;

                        wptr = IGL_WINDOW (wid);
                        /* absolute coordinates needed for igl */
                        XTranslateCoordinates (IGL_DISPLAY, wptr->wnd, IGL_XPARENT(wptr),
                                0, 0, &wptr->x, &wptr->y, &child);
                        wptr->width = event.xconfigure.width;
                        wptr->height = event.xconfigure.height;

                        /* must reposition the layers as well */
                        _igl_X11ConfigureLayers (wptr);

                        /* this probably has to be forced here? */
                        _igl_qenter (REDRAW, wid, False);
                    }
                }
                break;

            case FocusIn:           /* got mouse focus */
                wid = _igl_findLayerByHandle (event.xfocus.window, &lid);
                if (wid != -1)
                {
                    if (igl->deviceQueue[INPUTCHANGE])
                        _igl_qenter (INPUTCHANGE, wid, False);
                    if (igl->deviceQueue[REDRAW])
                        _igl_qenter (REDRAW, wid, False);
                    igl->activeWindow = wid;
                    /* restack here since the WM may have changed stacking */
                    _igl_X11ConfigureLayers (IGL_WINDOW(wid));
                }
                break;

            case FocusOut:          /* lost mouse focus */
                wid = _igl_findLayerByHandle (event.xfocus.window, &lid);
                if (igl->activeWindow == wid)
                    igl->activeWindow = -1;
                break;

            case PropertyNotify:    /* window property changed */
                wid = _igl_findWindowByHandle (event.xproperty.window);
                if (wid != -1 && igl->activeWindow == wid)
                {
                    Atom wmstate = XInternAtom (IGL_DISPLAY, "_NET_WM_STATE", False);
                    Atom wmhidden = XInternAtom (IGL_DISPLAY, "_NET_WM_STATE_HIDDEN", False);
                    int visible = True;

                    /* the only info needed is if the window is minimized */
                    if (event.xproperty.atom == wmstate)
                    {
                        Atom type;
                        unsigned long nitems, after, *data;
                        int format, i, e;

                        /* check if WM_STATE_HIDDEN is in WM_STATE */
                        wptr = IGL_WINDOW (wid);
                        e = XGetWindowProperty(IGL_DISPLAY, wptr->wnd, wmstate, 0, 1, False,
                                XA_ATOM, &type, &format, &nitems, &after, (unsigned char **)&data);
                        if (e == Success && format == 32)
                        {
                            for (i = 0; i < nitems && visible; i++)
                                visible = visible && !(data[i] == wmhidden);
                        }
                        if (e == Success)
                            XFree(data);

                        /* layer visibility follows window visibility */
                        _igl_X11mapLayers (wptr, visible);
                    }
                }
                break;

#if 0
            case ReparentNotify:
            {
                Window rootWnd, parentWnd, *clientWnds;
                XWindowAttributes wa;
                int frameSize;
                unsigned int clientCount;

                /*
                    this is a bit tricky:
                    X11 window managers usually reparent each client
                    window - add its own window as a parent and add a
                    title frame/border to the window.
                    when you then say XMoveWindow(0, 0), the window manager
                    repositions OUR window to (0, 0) and the frame is positioned
                    beyond (maybe i got this wrong, but this seems to be the case).
                    so to overcome this problem, we calculate the delta between
                    parent's and our own Y-coord position and move the window
                    to that location...
                */
                if (!event.xreparent.override_redirect)
                {
                    wid = _igl_findWindowByHandle (event.xreparent.window);
                    if (wid != -1)
                    {
                        wptr = IGL_WINDOW (wid);
                        XQueryTree (IGL_DISPLAY, wptr->wnd, &rootWnd, &parentWnd, &clientWnds, &clientCount);
                        if (parentWnd != IGL_ROOT)
                        {
                            XGetWindowAttributes (IGL_DISPLAY, parentWnd, &wa);
                            frameSize = wptr->y - wa.y;
                            wptr->y = (wptr->y < frameSize ? frameSize : wptr->y);
                            frameSize = wptr->x - wa.x;
                            wptr->x = (wptr->x < frameSize ? frameSize : wptr->x);
                            XMoveWindow (IGL_DISPLAY, wptr->wnd, wptr->x, wptr->y);
                        }
                    }
                }
                break;
            }
#endif

            default:
                break;
        }
    }
}


#if defined(__sgi)
/* Pre-OpenGL SGI stuff to attach GL contexts to X11 visuals */


/* GL: GLXgetconfig - Get configuration information for GL rendering into an X window */
GLXconfig *
GLXgetconfig (void *disp, long screen, GLXconfig *desc)
{
    GLXconfig *glxc;
    int count, layer;
    long flags = 0;

    _igl_trace (__func__,TRUE,"%p,%d,%p",disp,screen,desc);
    /* TODO: It would be better if the Display from the calling application
     * could be used. However, that is crashing on IRIX with preloading?
     */
    IGL_CHECKINIT ();

    /* allocate the return array: plus 3 entries each for 3 supported layers */
    for (count = 0; desc[count].buffer && count < 64; count++)
        ;
    if (! (glxc = calloc(count + 10, sizeof(GLXconfig))))
        return glxc;

    /* loop through the descriptor array, filling the return array on the way */
    count = 0;
    igl->windowFlags = 0;
    while (desc && desc->buffer && count < 64) {
        glxc[count] = *desc;
        switch (desc->mode) {
        case GLXC_RGB:
            if (glxc[count].buffer == GLXC_NORMAL && glxc[count].arg)
                flags |= IGL_WFLAGS_RGBA;
            break;
        case GLXC_DOUBLE:
            if (glxc[count].buffer == GLXC_NORMAL && glxc[count].arg)
                flags |= IGL_WFLAGS_DOUBLEBUFFER;
            break;
        case GLXC_STEREOBUF:
            if (glxc[count].buffer == GLXC_NORMAL && glxc[count].arg)
                flags |= IGL_WFLAGS_STEREOBUFFER;
            break;
        case GLXC_ZSIZE:
            glxc[count].arg = getgdesc(GD_BITS_NORM_ZBUFFER);
            break;
        case GLXC_RGBSIZE:
            glxc[count].arg = getgdesc(GD_BITS_NORM_DBL_RED);
            break;
        case GLXC_BUFSIZE:
            if (glxc[count].buffer == GLXC_UNDERLAY)
                glxc[count].arg = 0;
            else if (glxc[count].buffer == GLXC_NORMAL)
                glxc[count].arg = 32;
            else
                glxc[count].arg = 4;
            break;
        case GLXC_STENSIZE:
            glxc[count].arg = getgdesc(GD_BITS_STENCIL);
            break;
        case GLXC_ACSIZE:
            glxc[count].arg = getgdesc(GD_BITS_ACBUF);
            break;

        case GLXC_MSSAMPLE:
        case GLXC_MSZSIZE:
        case GLXC_MSSSIZE:
            glxc[count].arg = 0;
            break;
        default:    /* unknown, ignore this in output */
            count--;
        }
        desc ++, count++;
    }

    /* Deliver visuals and colormaps for the 3 top layers. No underlay.
     * Sequence is important here, some old SGI stuff cuts the config after the
     * GLXC_NORMAL/GLXC_WINDOW field.
     */
    /* TODO: although the man page explicitly states that the colormap is
     * read-only, there are programs that manipulate it, which produces an
     * X error if the map is TrueColor. Colormap isn't really supported in IGL.
     */
    layer = IGL_WLAYER_PUPDRAW;
    if (igl->colormaps[layer] == 0)
        igl->colormaps[layer] = XCreateColormap (disp, DefaultRootWindow(disp),
                igl->visualInfo[layer]->visual, AllocNone);
    glxc[count].buffer = GLXC_POPUP; glxc[count].mode = GLXC_VISUAL;
        glxc[count++].arg = igl->visualInfo[layer]->visualid;
    glxc[count].buffer = GLXC_POPUP; glxc[count].mode = GLXC_COLORMAP;
        glxc[count++].arg = igl->colormaps[layer];
    glxc[count].buffer = GLXC_POPUP; glxc[count].mode = GLXC_WINDOW;
        glxc[count++].arg = 0;

    layer = IGL_WLAYER_OVERDRAW;
    if (igl->colormaps[layer] == 0)
        igl->colormaps[layer] = XCreateColormap (disp, DefaultRootWindow(disp),
                igl->visualInfo[layer]->visual, AllocNone);
    glxc[count].buffer = GLXC_OVERLAY; glxc[count].mode = GLXC_VISUAL;
        glxc[count++].arg = igl->visualInfo[layer]->visualid;
    glxc[count].buffer = GLXC_OVERLAY; glxc[count].mode = GLXC_COLORMAP;
        glxc[count++].arg = igl->colormaps[layer];
    glxc[count].buffer = GLXC_OVERLAY; glxc[count].mode = GLXC_WINDOW;
        glxc[count++].arg = 0;

    layer = IGL_WLAYER_NORMALDRAW;
    if (igl->colormaps[layer] == 0)
        igl->colormaps[layer] = XCreateColormap (disp, DefaultRootWindow(disp),
                igl->visualInfo[layer]->visual, AllocNone);
    glxc[count].buffer = GLXC_NORMAL; glxc[count].mode = GLXC_VISUAL;
        glxc[count++].arg = igl->visualInfo[layer]->visualid;
    glxc[count].buffer = GLXC_NORMAL; glxc[count].mode = GLXC_COLORMAP;
        glxc[count++].arg = igl->colormaps[layer];
    glxc[count].buffer = GLXC_NORMAL; glxc[count].mode = GLXC_WINDOW;
        glxc[count++].arg = 0;

    /* mark this as last element */
    IGL_CLR (&glxc[count]);

    return glxc;
}


/* GL: GLXlink - Configure X window for GL rendering */
long
GLXlink (void *disp, GLXconfig *cfg)
{
    igl_windowT *wptr;
    Window layers[IGL_WLAYER_MAX];
    long idx, flags = 0;

    _igl_trace (__func__,TRUE,"%p,%p",disp,cfg);
    IGL_CHECKINIT ();

    /* flush a possible CreateWindow and wait some for the X Server */
    XSync(disp, False);

    /* get a GL window for this */
    idx = _igl_findWindowPlaceholder ();
    if (idx == -1)
        return (-1);
    wptr = IGL_WINDOW (idx);

    /* parse the descriptor list */
    memset (layers, IGL_NULLWND, sizeof(layers));
    while (cfg->buffer)
    {
        switch (cfg->mode)
        {
        case GLXC_RGB:
            if (cfg->buffer == GLXC_NORMAL && cfg->arg)
                flags |= IGL_WFLAGS_RGBA;
            break;
        case GLXC_DOUBLE:
            if (cfg->buffer == GLXC_NORMAL && cfg->arg)
                flags |= IGL_WFLAGS_DOUBLEBUFFER;
            break;
        case GLXC_STEREOBUF:
            if (cfg->buffer == GLXC_NORMAL && cfg->arg)
                flags |= IGL_WFLAGS_STEREOBUFFER;
            break;
        case GLXC_WINDOW:
            switch (cfg->buffer)
            {
            case GLXC_NORMAL:   layers[IGL_WLAYER_NORMALDRAW] = cfg->arg; break;
            case GLXC_OVERLAY:  layers[IGL_WLAYER_OVERDRAW] = cfg->arg;   break;
            case GLXC_POPUP:    layers[IGL_WLAYER_PUPDRAW] = cfg->arg;    break;
            }
            break;
        }
        cfg ++;
    }

    /* attach to the NORMALDRAW window */
    if (layers[IGL_WLAYER_NORMALDRAW] != 0)
    {
        Window root, child, this = layers[IGL_WLAYER_NORMALDRAW];
        unsigned int dummy;

        /* set window flags as set by GLXconfig */
        wptr->flags = flags;

        /* get absolute position and size of the window */
        XGetGeometry (IGL_DISPLAY, this, &root, &wptr->x, &wptr->y,
                                &wptr->width, &wptr->height, &dummy, &dummy);
        XTranslateCoordinates (IGL_DISPLAY, this, IGL_ROOT, 0, 0,
                                &wptr->x, &wptr->y, &child);

        /* attach to the IGL window context */
        _igl_attachWindow (idx, this);

        /* attach the layers in the correct stacking sequence */
        for (idx = 0; idx < IGL_WLAYER_MAX && wptr->wnd != IGL_NULLWND; idx++)
        {
            if (layers[idx] != IGL_NULLWND && idx != IGL_WLAYER_NORMALDRAW)
                _igl_attachLayer (wptr, idx, layers[idx]);
        }

        if (igl->currentWindow >= 0)
            glXMakeCurrent (IGL_DISPLAY, IGL_CTX ()->currentLayer->hwnd, IGL_CTX ()->currentLayer->hrc);
    }
    else
    {
        return GLWS_NOWINDOW;
    }

    /* flush our attach operations */
    XSync (IGL_DISPLAY, False);

    return (wptr->wnd != IGL_NULLWND ? GLWS_NOERROR : GLWS_NOCONTEXT);
}


/* GL: GLXunlink - End GL rendering in an X window */
long
GLXunlink (void *disp, unsigned long win)
{
    short wnd = _igl_findWindowByHandle (win);

    _igl_trace (__func__,TRUE,"%p,%lx",disp,win);
    IGL_CHECKWND (-1);

    if (wnd <= 0)
    {
        int i;
        igl_windowT *wptr = IGL_CTX ();
        for (i = 0; i < IGL_WLAYER_CURSORDRAW; i++)
            if (wptr->layers[i].hwnd == win)
            {
                _igl_detachLayer (wptr, i);
                return GLWS_NOERROR;
            }
        return GLWS_NOCONTEXT;
    }

    _igl_detachWindow (wnd);

    return GLWS_NOERROR;
}


/* GL: GLXwinset - Begin GL rendering in an X window */
long
GLXwinset (void *disp, unsigned long win)
{
    short layer = IGL_WLAYER_NORMALDRAW;
    short wnd = _igl_findLayerByHandle (win, &layer);
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%p,%lx",disp,win);
    IGL_CHECKINIT ();

    glFlush();

    if (wnd > 0)
    {
        igl->currentWindow = wnd;
        wptr = IGL_CTX ();
        wptr->currentLayer = &wptr->layers[layer];

        /* set OpenGL context to the layer */
        glXMakeCurrent (IGL_DISPLAY, win, wptr->currentLayer->hrc);

        if (layer != IGL_WLAYER_NORMALDRAW)
        {
            int xo = wptr->currentLayer->xoffs, yo = wptr->currentLayer->yoffs;
            Window child;

            /* this is a useful place where the layer position and size can
             * be adjusted to the NORMALDRAW plane, since IGL's event handling
             * isn't used with GLX.
             */
            XTranslateCoordinates (IGL_DISPLAY, wptr->wnd, IGL_ROOT, 0, 0,
                                    &wptr->x, &wptr->y, &child);
            XMoveWindow (IGL_DISPLAY, wptr->currentLayer->hwnd,
                                    wptr->x + xo, wptr->y + yo);

            glDrawBuffer (GL_FRONT);
        }
        else
            glDrawBuffer ((wptr->flags & IGL_WFLAGS_DOUBLEBUFFER) ? GL_BACK : GL_FRONT);

        switch (layer)
        {
            case IGL_WLAYER_UNDERDRAW:  wptr->drawMode = UNDERDRAW; break;
            case IGL_WLAYER_NORMALDRAW: wptr->drawMode = NORMALDRAW; break;
            case IGL_WLAYER_OVERDRAW:   wptr->drawMode = OVERDRAW; break;
            case IGL_WLAYER_PUPDRAW:    wptr->drawMode = PUPDRAW; break;
            case IGL_WLAYER_CURSORDRAW: wptr->drawMode = CURSORDRAW; break;
            default:                    return GLWS_NOWINDOW;
        }

        /* TODO: imgworks adds a layer subwindow totally obscuring us. It is
         * transparent on an SGI, but it's not in a "normal" X11. How can a
         * subwindow be made transparent without using the Xrender extension?
         */
#if 0
        XUnmapSubwindows(disp,win);
#endif
#if 0
        /* If there are subwindows, it must be layers. Attach to them. */
        Window root, parent, *children;
        unsigned int nChild;

        if (XQueryTree (IGL_DISPLAY, win, &root, &parent, &children, &nChild))
        {
            int i;
            for (i = 0; i < (nChild < 2 ? nChild : 2); i++)
                _igl_attachLayer (IGL_CTX (), IGL_WLAYER_OVERDRAW+i, children[i]);
            XFree (children);
        }
#endif

        return GLWS_NOERROR;
    }

    return GLWS_NOWINDOW;
}
#endif
#if defined(__aix)
/* TODO: AIX GL extensions. experimental code as of now */
Display *
getXdpy(void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    return IGL_DISPLAY;
}

Window
getXwid(void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (IGL_NULLWND);

    return IGL_CTX ()->wnd;
}

short
winX (Display *dpy, Window xid)
{
    igl_windowT *wptr;
    Window root, child;
    unsigned int dummy;
    short idx;

    _igl_trace (__func__,TRUE,"%p,%x",dpy,xid);
    IGL_CHECKINIT ();

    /* TODO: untested; this won't work, since AIX requires a PseudoColor visual
     * while IGL requires a TrueColor one. I think this would require having
     * an IGL window as a subwindow on top of the AIX window.
     */

    /* flush a possible CreateWindow and wait some for the X Server */
    XSync(dpy, False);

    /* get a GL window for this */
    idx = _igl_findWindowPlaceholder ();
    if (idx == -1)
        return (-1);
    wptr = IGL_WINDOW (idx);

    /* TODO: window flags? */

    /* get absolute position and size of the window */
    XGetGeometry (IGL_DISPLAY, xid, &root, &wptr->x, &wptr->y,
                            &wptr->width, &wptr->height, &dummy, &dummy);
    XTranslateCoordinates (IGL_DISPLAY, xid, IGL_ROOT, 0, 0,
                            &wptr->x, &wptr->y, &child);

    /* attach to the IGL window context */
    _igl_attachWindow (idx, xid);

    /* flush our attach operations */
    XSync (IGL_DISPLAY, False);

    return idx;
}
#endif
#endif  /* PLATFORM_X11 */


#if 0
____________________________ window functions ____________________________
#endif
/* GL: prefsize - specifies the preferred size of a graphics window */
void
prefsize (int width, int height)
{
    _igl_trace (__func__,TRUE,"%d,%d",width,height);
    IGL_CHECKINIT ();

    if (width > 0)
        igl->winWidth = width;
    if (height > 0)
        igl->winHeight = height;
}


/* GL: maxsize - specifies the maximum size of a graphics window */
void
maxsize (long width, long height)
{
    _igl_trace (__func__,TRUE,"%ld,%ld",width,height);
    IGL_CHECKINIT ();

    if (width > 0 && (igl->winMinWidth == 0 || width >= igl->winMinWidth))
        igl->winMaxWidth = width;
    if (height > 0 && (igl->winMinHeight == 0 || width >= igl->winMinHeight))
        igl->winMaxHeight = height;
}


/* GL: minsize - specifies the minimum size of a graphics window */
void
minsize (long width, long height)
{
    _igl_trace (__func__,TRUE,"%ld,%ld",width,height);
    IGL_CHECKINIT ();

    if (width > 0 && (igl->winMaxWidth == 0 || width <= igl->winMaxWidth))
        igl->winMinWidth = width;
    if (height > 0 && (igl->winMaxHeight == 0 || height <= igl->winMaxHeight))
        igl->winMinHeight = height;
}


/* GL: prefposition - specifies the preferred location and size of a graphics window */
void
prefposition (int x0, int x1, int y0, int y1)
{
    int x, y, w, h;

    _igl_trace (__func__,TRUE,"%d,%d,%d,%d",x0,x1,y0,y1);
    IGL_CHECKINIT ();

    /* corners may be swapped */
    if (x1 < x0)
    {
        x = x1;
        w = x0-x1+1;
    } else {
        x = x0;
        w = x1-x0+1;
    }
    if (y1 < y0)
    {
        y = GL_Y(y0);
        h = y0-y1+1;
    } else {
        y = GL_Y(y1);
        h = y1-y0+1;
    }

    igl->winPosx = x;
    igl->winPosy = y;
    igl->winWidth = w;
    igl->winHeight = h;
}


/* GL: keepaspect - specifies the aspect ratio of a graphics window */
void
keepaspect (int x, int y)
{
    _igl_trace (__func__,TRUE,"%d,%d",x,y);
    IGL_CHECKINIT ();

    if (x > 0 && x <= 32767 && y > 0 && y <= 32767)
    {
        igl->winAspectX = x;
        igl->winAspectY = y;
    }
}


/* GL: stepunit - specifies the window size step unit of a graphics window */
void
stepunit (int x, int y)
{
    _igl_trace (__func__,TRUE,"%d,%d",x,y);
    IGL_CHECKINIT ();

    if (x > 0 && x <= 32767 && y > 0 && y <= 32767)
    {
        igl->winStepX = x;
        igl->winStepY = y;
    }
}


/* GL: noborder - specifies a window without any borders */
void
noborder (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    igl->windowFlags |= IGL_WFLAGS_NOBORDER;
}


/* GL: noport - specifies that a program does not need screen space */
void
noport (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    igl->windowFlags |= IGL_WFLAGS_NOPORT;
}


/* GL: imakebackground - registers the screen background process */
void
imakebackground (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    prefposition (0, igl->gdXPMAX-1, 0, igl->gdYPMAX-1);
    igl->windowFlags |= IGL_WFLAGS_BACKGROUND;
}


/* GL: winopen - creates a graphics window */
long
winopen (String winTitle)
{
    long ret;

    _igl_trace (__func__,TRUE,"%p(%s)",winTitle,winTitle?winTitle:"");
    IGL_CHECKINIT ();

    ret = _igl_createWindow (winTitle, -1);
    return ret;
}


/* GL: swinopen - creates a graphics subwindow */
long
swinopen (long parent)
{
    long ret;

    _igl_trace (__func__,TRUE,"%ld",parent);
    IGL_CHECKWND (-1);

    if (parent <= 0 || parent >= IGL_MAXWINDOWS || IGL_WINDOW (parent)->wnd == IGL_NULLWND)
        return (-1);

    ret = _igl_createWindow (NULL, parent);
    return ret;
}

/* GL: winclose -  closes the identified graphics window */
void
winclose (long gwid)
{
    _igl_trace (__func__,TRUE,"%ld",gwid);
    IGL_CHECKWNDV ();

    /* avoid closing the query window */
    if (gwid <= 0 || gwid >= IGL_MAXWINDOWS || IGL_WINDOW (gwid)->wnd == IGL_NULLWND)
        return;

    _igl_destroyWindow (gwid);
}


/* GL: winconstraints - binds window constraints to the current window */
void
winconstraints (void)
{
    igl_windowT *wptr;
#if PLATFORM_X11
    XSizeHints sizeHints;
#endif

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* take over window parameters from user settings */
    _igl_setWinParams (wptr);

#if PLATFORM_X11
    /* inform WM of the change */
    sizeHints.flags = 0;
    if (wptr->aspectX != 0 && wptr->aspectY != 0)
    {
        sizeHints.flags |= PAspect;
        sizeHints.min_aspect.x = sizeHints.max_aspect.x = wptr->aspectX;
        sizeHints.min_aspect.y = sizeHints.max_aspect.y = wptr->aspectY;
    }
    if (wptr->stepX != 0 && wptr->stepY != 0)
    {
        sizeHints.flags |= PResizeInc;
        sizeHints.width_inc = wptr->stepX;
        sizeHints.height_inc = wptr->stepY;
    }
    if (wptr->minWidth != 0 && wptr->minHeight != 0)
    {
        sizeHints.flags |= PMinSize;
        sizeHints.min_width = wptr->minWidth;
        sizeHints.min_height = wptr->minHeight;
    }
    if (wptr->maxWidth != 0 && wptr->maxHeight != 0)
    {
        sizeHints.flags |= PMaxSize;
        sizeHints.max_width = wptr->maxWidth;
        sizeHints.max_height = wptr->maxHeight;
    }
    XSetWMNormalHints (IGL_DISPLAY, wptr->wnd, &sizeHints);

    /* make changes current */
    _igl_X11MoveResize (wptr);
#endif

    /*
        winconstraints() man page says:
        "After binding constraints to a window, winconstraints resets the window
         constraints to their default values, if any."
        the question is, what are the default values, if any =)
    */
    igl->winPosx = igl->winPosy = 0;
    igl->winWidth = igl->winHeight = 0;
    igl->winMinWidth = igl->winMinHeight = igl->winMaxWidth = igl->winMaxHeight = 0;
    igl->winAspectX = igl->winAspectY = igl->winStepX = igl->winStepY = 0;
}


/* GL: winposition - changes the size and position of the current graphics window */
void
winposition (int x0, int x1, int y0, int y1)
{
    igl_windowT *wptr;
    int x, y, w, h;

    _igl_trace (__func__,TRUE,"%d,%d,%d,%d",x0,x1,y0,y1);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* corners may be swapped */
    if (x1 < x0)
    {
        x = x1;
        w = x0-x1+1;
    } else {
        x = x0;
        w = x1-x0+1;
    }
    if (y1 < y0)
    {
        y = y0;
        h = y0-y1+1;
    } else {
        y = y1;
        h = y1-y0+1;
    }

    wptr->x = x;
    wptr->y = wptr->parent >= 0 ? GL_WY (IGL_PARENT (wptr), y) : GL_Y(y);
    wptr->width = w;
    wptr->height = h;

    /* check against constraints */
    if (wptr->maxWidth > 0 && wptr->width > wptr->maxWidth)
        wptr->width = wptr->maxWidth;
    else if (wptr->minWidth > 0 && wptr->width < wptr->minWidth)
        wptr->width = wptr->minWidth;
    if (wptr->maxHeight > 0 && wptr->height > wptr->maxHeight)
        wptr->height = wptr->maxHeight;
    else if (wptr->minHeight > 0 && wptr->height < wptr->minHeight)
        wptr->height = wptr->minHeight;

#if PLATFORM_X11
    _igl_X11MoveResize (wptr);
#endif
}


/* GL: winget - returns the identifier of the current graphics window */
long
winget (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    return (igl->currentWindow);
}


/* GL: winset - sets the current graphics window */
void
winset (long gwid)
{
    _igl_trace (__func__,TRUE,"%ld",gwid);
    IGL_CHECKWNDV ();

    glFlush ();
    if (gwid >= 0 && gwid < IGL_MAXWINDOWS && gwid != igl->currentWindow)
    {
        igl->currentWindow = gwid;
#if PLATFORM_X11
        glXMakeCurrent (IGL_DISPLAY, IGL_WINDOW (gwid)->wnd, IGL_WINDOW (gwid)->currentLayer->hrc);
#endif
    }
}


/* GL: winpush - places the current graphics window behind all other windows */
void
winpush (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

#if PLATFORM_X11
    {
        XWindowChanges xwc;

        _igl_trace (__func__,TRUE,"");
        IGL_CHECKWNDV ();

        xwc.stack_mode = BottomIf;
        XConfigureWindow (IGL_DISPLAY, IGL_CTX ()->wnd, CWStackMode, &xwc);
    }
#endif
}


/* GL: winpop - moves the current graphics window in front of all other windows */
void
winpop (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

#if PLATFORM_X11
    {
        XWindowChanges xwc;

        _igl_trace (__func__,TRUE,"");
        IGL_CHECKWNDV ();

        xwc.stack_mode = TopIf;
        XConfigureWindow (IGL_DISPLAY, IGL_CTX ()->wnd, CWStackMode, &xwc);
    }
#endif
}


/* GL: windepth - measures how deep a window is in the window stack */
long
windepth (long gwid)
{
    int depth = 0, i;

    _igl_trace (__func__,TRUE,"%ld",gwid);
    IGL_CHECKWND (-1);

#if PLATFORM_X11
    {
        Window win, current, root, parent = 0, *children;
        unsigned int nChild;

        /* Since the window manager reparents all windows it's not possible to
         * simply look up the childs of the root window. Instead, go down the
         * tree and sum up all the order information.
         */
        XGrabServer (IGL_DISPLAY);
        parent = win = IGL_WINDOW (gwid)->wnd;
        do
        {
            current = parent;
            if (! XQueryTree (IGL_DISPLAY, current, &root, &parent, &children, &nChild))
                break;

            for (i = 0; i < nChild; i++)
            {
                if (children[i] == win)
                {
                    depth += i;
                    win = current;
                    break;
                }
            }
            XFree (children);
        }
        while (win != root);
        XUngrabServer (IGL_DISPLAY);
    }
#endif

    return depth;
}


/* GL: winmove - moves the current graphics window by its lower-left corner */
void
winmove (long orgx, long orgy)
{
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%ld,%ld",orgx,orgy);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* convert lower-left (IrisGL/OpenGL) to upper-left (Win32/X11) coords */
    orgy  += wptr->height - 1;
    wptr->x = orgx;
    wptr->y = wptr->parent >= 0 ? GL_WY (IGL_PARENT (wptr), orgy) : GL_Y(orgy);

#if PLATFORM_X11
    _igl_X11MoveResize (wptr);
#endif
}


/* GL: wintitle - adds a title bar to the current graphics window */
void
wintitle (String name)
{
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%p(%s)",name,name?name:"");
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if (name == NULL)
        return;
    strcpy (wptr->title, name);

#if PLATFORM_X11
    XStoreName (IGL_DISPLAY, wptr->wnd, wptr->title);
#endif
}


/* GL: winat - return identifier of graphics window beneath cursor */
long
winat (void)
{
    return gl_winat (igl->deviceVal[CURSORX], igl->deviceVal[CURSORY]);
}


/* GL: gl_winat - ??? (gets the window id at specified coordinates?) */
long
gl_winat (short x, short y)
{
    igl_windowT *wptr;
    int i;

    _igl_trace (__func__,TRUE,"%d,%d",x,y);
    IGL_CHECKWND (-1);

    /* TODO: consider stacking order */
    y = GL_Y(y);

    /* first, process active window */
    i = igl->activeWindow;
    if (i != -1)
    {
        wptr = IGL_WINDOW (i);
        if (wptr->wnd != IGL_NULLWND && (wptr->state & IGL_WSTATE_MAPPED) &&
            wptr->parent < 0 && x >= wptr->x && y >= wptr->y &&
            x < (wptr->x + wptr->width) && y < (wptr->y + wptr->height))
        {
            return (i);
        }
    }

    /* not found, try inactive */
    for (i=0; i < IGL_MAXWINDOWS; i++)
    {
        wptr = IGL_WINDOW (i);
        if (wptr->wnd != IGL_NULLWND && (wptr->state & IGL_WSTATE_MAPPED) &&
            wptr->parent < 0 && x >= wptr->x && y >= wptr->y &&
            x < (wptr->x + wptr->width) && y < (wptr->y + wptr->height))
        {
            return (i);
        }
    }

    return (-1);
}


/* GL: getsize - returns the size of a graphics window */
void
getsize (long *x, long *y)
{
    _igl_trace (__func__,TRUE,"%p,%p",x,y);
    IGL_CHECKWNDV ();

    IGL_MSG_PROCESS

    if (x != NULL)
        *x = IGL_CTX()->width;
    if (y != NULL)
        *y = IGL_CTX()->height;
    _igl_trace (" = ",TRUE,"(%d,%d)",x?*x:0,y?*y:0);
}


/* GL: getorigin - returns the position of a graphics window */
void
getorigin (long *x, long *y)
{
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%p,%p",x,y);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    IGL_MSG_PROCESS

    if (x != NULL)
        *x = wptr->x;
    if (y != NULL)
    {
        *y = wptr->y + wptr->height - 1;
        *y = wptr->parent >= 0 ? GL_WY (IGL_PARENT (wptr), *y) : GL_Y(*y);
    }
    _igl_trace (" = ",TRUE,"(%d,%d)",x?*x:0,y?*y:0);
}


/* GL: gconfig - reconfigures the GL modes of the current window */
void
gconfig (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

#if PLATFORM_X11
    reshapeviewport ();

    if (IGL_CTX ()->flags != IGL_CTX ()->newFlags)
        IGL_CTX ()->flags = IGL_CTX ()->newFlags;
    glDrawBuffer ((IGL_CTX ()->flags & IGL_WFLAGS_DOUBLEBUFFER) ? GL_BACK : GL_FRONT);
#endif

    /* this has to be done here to get the first redraw properly (?) */
    _igl_qenter (REDRAW, (short)igl->currentWindow, True);
}


/* GL: getgconfig - gets the size of a buffer or a state in the current buffer configuration */
long
getgconfig (long buffer)
{
    int buf[2];
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%ld",buffer);
    IGL_CHECKWND (0);
    wptr = IGL_CTX ();

    switch (buffer)
    {
        case GC_BITS_CMODE:
            return ((wptr->flags & IGL_WFLAGS_RGBA) ? 0 : IGL_MAXCOLORS);

        case GC_BITS_RED:
            glGetIntegerv (GL_RED_BITS, buf);
            return (buf[0]);

        case GC_BITS_GREEN:
            glGetIntegerv (GL_GREEN_BITS, buf);
            return (buf[0]);

        case GC_BITS_BLUE:
            glGetIntegerv (GL_BLUE_BITS, buf);
            return (buf[0]);

        case GC_BITS_ALPHA:
            glGetIntegerv (GL_ALPHA_BITS, buf);
            return (buf[0]);

        case GC_BITS_ZBUFFER:
            glGetIntegerv (GL_DEPTH_BITS, buf);
            return (buf[0]);

        case GC_ZMIN:
            return (0);
        case GC_ZMAX:
            return (0xffffff);

        case GC_BITS_STENCIL:
            glGetIntegerv (GL_STENCIL_BITS, buf);
            return (buf[0]);

        case GC_BITS_ACBUF:
            return (wptr->acPlanes);

        case GC_MS_SAMPLES:
        case GC_BITS_MS_ZBUFFER:
        case GC_MS_ZMIN:
        case GC_MS_ZMAX:
        case GC_BITS_MS_STENCIL:
            /* multisample mode not yet supported */
            return (0);

        case GC_STEREO:
            return (wptr->flags & IGL_WFLAGS_STEREOBUFFER);

        case GC_DOUBLE:
            return (wptr->flags & IGL_WFLAGS_DOUBLEBUFFER);

        default:
            return (0);
    }
}


/* TODO: fullscrn might be simulated in some layers by extending the layer over
 * the whole screen. This has some serious drawbacks, though, as events won't
 * propagate to windows "behind" the layer window. This might even lead to
 * situations where the whole screen is locked up. The whole approach appears
 * to be a bit dangerous.
 * For now, create an ortho projection for the part of the screen covered by
 * the window and hope for a better idea later :-(
 */

/* GL: fullscrn - allows a program write to the entire screen */
void
fullscrn (void)
{
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if (!(wptr->state & IGL_WSTATE_FULLSCREEN))
    {
        pushviewport();
        viewport (0, wptr->width-1, 0, wptr->height-1);
        ortho2 (wptr->x-0.5, wptr->x+wptr->width-0.5, GL_Y(wptr->y+wptr->height-1)-0.5, GL_Y(wptr->y)-0.5);
    }
    wptr->state |= IGL_WSTATE_FULLSCREEN;
}


/* GL: endfullscrn - ends full-screen mode */
void
endfullscrn (void)
{
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    wptr = IGL_CTX ();
    if (wptr->state & IGL_WSTATE_FULLSCREEN)
        popviewport();
    wptr->state &= ~IGL_WSTATE_FULLSCREEN;
}


/* GL: screenspace - map world space to absolute screen coordinates */
void
screenspace (void)
{
    long xmin, ymin, xmax, ymax;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    getorigin (&xmin, &ymin);
    xmax = igl->gdXPMAX;
    ymax = igl->gdYPMAX;

    viewport ((short)-xmin, (short)(xmax-xmin), (short)-ymin, (short)(ymax-ymin));
    ortho2 (-0.5f, xmax+0.5f, -0.5f, ymax+0.5f);
}


/* GL: getdrawmode - returns the current drawing mode */
int
getdrawmode (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (NORMALDRAW);

    return IGL_CTX()->drawMode;
}


/* GL: drawmode - selects which GL framebuffer is drawable */
void
drawmode (int mode)
{
    igl_windowT *wptr;
    int layer;

    _igl_trace (__func__,TRUE,"%d",mode);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* map mode to our index */
    switch (mode)
    {
    case UNDERDRAW:         layer = IGL_WLAYER_UNDERDRAW; break;
    case NORMALDRAW:        layer = IGL_WLAYER_NORMALDRAW; break;
    case OVERDRAW:          layer = IGL_WLAYER_OVERDRAW; break;
    case PUPDRAW:           layer = IGL_WLAYER_PUPDRAW; break;
    case CURSORDRAW:        layer = IGL_WLAYER_CURSORDRAW; break;
    default:                return;
    }
    wptr->currentLayer = &wptr->layers[layer];
    wptr->drawMode = mode;

#if PLATFORM_X11
    /* create the layer window if it doesn't yet exist */
    if (wptr->currentLayer->hwnd == IGL_NULLWND)
        _igl_createLayer (wptr, layer);

    glXMakeCurrent (IGL_DISPLAY, wptr->currentLayer->hwnd, wptr->currentLayer->hrc);
#endif

    if (mode != NORMALDRAW)
        glDrawBuffer (GL_FRONT);
    else
        glDrawBuffer ((wptr->flags & IGL_WFLAGS_DOUBLEBUFFER) ? GL_BACK : GL_FRONT);
    glColor4ubv (wptr->currentColor);
}


/* GL: obsolete routines */
void
pupmode (void)
{
    drawmode (PUPDRAW);
}


void
endpupmode (void)
{
    drawmode (NORMALDRAW);
}


void
getport (String name)
{
    winopen (name);
}


long
winattach (void)
{
    return (0);
}


/* GL: unimplemented stuff */
void
overlay (long planes)
{
    _igl_trace (__func__,TRUE,"%ld",planes);
}


void
underlay (long planes)
{
    _igl_trace (__func__,TRUE,"%ld",planes);
}


void
icontitle (String name)
{
    _igl_trace (__func__,TRUE,"%s",name);
}


void
iconsize (long x, long y)
{
    _igl_trace (__func__,TRUE,"%ld,%ld",x,y);
}


void
fudge (long xfudge, long yfudge)
{
    _igl_trace (__func__,TRUE,"%ld,%ld",xfudge,yfudge);
}
