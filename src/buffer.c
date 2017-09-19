/*
 * buffer.c
 *
 * buffer and buffer properties handling, or affecting whole buffer contents
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


#if 0
____________________________ buffer functions ____________________________
#endif
/* GL: clear - clears the viewport */
void
clear (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->clear ();
}

void
iglExec_clear (void)
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    glClearColor (wptr->currentColor[0]/255.0f, wptr->currentColor[1]/255.0f,
                wptr->currentColor[2]/255.0f, wptr->currentColor[3]/255.0f);
    glClear (GL_COLOR_BUFFER_BIT);
}


/* GL: cmode - sets color map mode as the current mode */
void
cmode (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    IGL_CTX()->newFlags &= ~IGL_WFLAGS_RGBA;
}


/* GL: RGBmode - sets a rendering and display mode that bypasses the color map */
void
RGBmode (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    IGL_CTX()->newFlags |= IGL_WFLAGS_RGBA;
}


/* GL: RGBsize - specifies the number of bit planes desired to comprise each single-color component buffer */
void
RGBsize (long planes)
{
    _igl_trace (__func__,TRUE,"%ld",planes);
    IGL_CHECKWNDV ();

    IGL_CTX()->rgbPlanes = (unsigned char)planes;
}


/* GL: getplanes - returns the number of available bitplanes */
long
getplanes (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (-1);

    /* the layers are limited to 4 planes */
    if (IGL_CTX ()->drawMode != NORMALDRAW)
        return 4;

    /* color-index mode has a fixed palette of 4096 (2^12) entries */
    return ((IGL_CTX()->flags & IGL_WFLAGS_RGBA) ? IGL_CTX()->rgbPlanes : 12);
}


/* GL: getdisplaymode - returns the current display mode */
long
getdisplaymode (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (DMRGB);

    if (IGL_CTX ()->flags & IGL_WFLAGS_DOUBLEBUFFER)
        return (IGL_CTX ()->flags & IGL_WFLAGS_RGBA) ? DMRGBDOUBLE : DMDOUBLE;
    else
        return (IGL_CTX ()->flags & IGL_WFLAGS_RGBA) ? DMRGB : DMSINGLE;
}


/* GL: singlebuffer - writes and displays all bitplanes */
void
singlebuffer (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    IGL_CTX()->newFlags &= ~IGL_WFLAGS_DOUBLEBUFFER;
}


/* GL: doublebuffer - sets the display mode to double buffer mode */
void
doublebuffer (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    IGL_CTX()->newFlags |= IGL_WFLAGS_DOUBLEBUFFER;
}


/* GL: backbuffer, frontbuffer - enable and disable drawing to the back or front buffer */
void
backbuffer (Boolean enable)
{
    _igl_trace (__func__,FALSE,"%d",enable);
    igl->api->backbuffer (enable);
}

void
iglExec_backbuffer (Boolean enable)
{
    int cb;

    IGL_CHECKWNDV ();

    if (!(IGL_CTX()->flags & IGL_WFLAGS_DOUBLEBUFFER))
        return;

    /* NOTE: we only support FRONT and BACK buffers!!!! */
    glGetIntegerv (GL_DRAW_BUFFER, &cb);
    if (enable)
    {
        /* backbuffer already set? -> nothing to do */
        if (cb == GL_BACK || cb == GL_FRONT_AND_BACK)
            return;
        else if (cb == GL_FRONT)
            glDrawBuffer (GL_FRONT_AND_BACK);
        else if (cb == GL_NONE)
            glDrawBuffer (GL_BACK);
    }
    else
    {
        if (cb == GL_BACK)
            glDrawBuffer (GL_NONE);
        else if (cb == GL_FRONT_AND_BACK)
            glDrawBuffer (GL_FRONT);
    }
}


/* GL: backbuffer, frontbuffer - enable and disable drawing to the back or front buffer */
void
frontbuffer (Boolean enable)
{
    _igl_trace (__func__,FALSE,"%d",enable);
    igl->api->frontbuffer (enable);
}

void
iglExec_frontbuffer (Boolean enable)
{
    int cb;

    IGL_CHECKWNDV ();

    if (!(IGL_CTX()->flags & IGL_WFLAGS_DOUBLEBUFFER))
        return;

    /* NOTE: we only support FRONT and BACK buffers!!!! */
    glGetIntegerv (GL_DRAW_BUFFER, &cb);
    if (enable)
    {
        /* frontbuffer already set? -> nothing to do */
        if (cb == GL_FRONT || cb == GL_FRONT_AND_BACK)
            return;
        else if (cb == GL_BACK)
            glDrawBuffer (GL_FRONT_AND_BACK);
        else if (cb == GL_NONE)
            glDrawBuffer (GL_FRONT);
    }
    else
    {
        if (cb == GL_FRONT)
            glDrawBuffer (GL_NONE);
        else if (cb == GL_FRONT_AND_BACK)
            glDrawBuffer (GL_BACK);
    }
}


/* GL: leftbuffer, rightbuffer - enables and disables drawing to the left/right buffer */
void
leftbuffer (Boolean enable)
{
    _igl_trace (__func__,FALSE,"%d",enable);
    igl->api->leftbuffer (enable);
}

void
iglExec_leftbuffer (Boolean enable)
{
    int cb;

    IGL_CHECKWNDV ();

    glGetIntegerv (GL_DRAW_BUFFER, &cb);

    if (enable)
    {
        switch (cb)
        {
            case GL_NONE:           cb = GL_LEFT;   break;
            case GL_FRONT_RIGHT:    cb = GL_FRONT; break;
            case GL_BACK_RIGHT:     cb = GL_BACK;   break;
            case GL_RIGHT:          cb = GL_FRONT_AND_BACK; break;

            default:                return;
        }
        glDrawBuffer (cb);
    }
    else
    {
        switch (cb)
        {
            case GL_LEFT:
            case GL_FRONT_LEFT:
            case GL_BACK_LEFT:      cb = GL_NONE; break;

            case GL_FRONT:          cb = GL_FRONT_RIGHT; break;
            case GL_BACK:           cb = GL_BACK_RIGHT; break;
            case GL_FRONT_AND_BACK: cb = GL_RIGHT; break;

            default:                return;
        }
        glDrawBuffer (cb);
    }
}


/* GL: leftbuffer, rightbuffer - enables and disables drawing to the left/right buffer */
void
rightbuffer (Boolean enable)
{
    _igl_trace (__func__,FALSE,"%d",enable);
    igl->api->rightbuffer (enable);
}

void
iglExec_rightbuffer (Boolean enable)
{
    int cb;

    IGL_CHECKWNDV ();

    glGetIntegerv (GL_DRAW_BUFFER, &cb);

    if (enable)
    {
        switch (cb)
        {
            case GL_NONE:           cb = GL_RIGHT; break;
            case GL_FRONT_LEFT:     cb = GL_FRONT; break;
            case GL_BACK_LEFT:      cb = GL_BACK;  break;
            case GL_LEFT:           cb = GL_FRONT_AND_BACK; break;

            default:                return;
        }
        glDrawBuffer (cb);
    }
    else
    {
        switch (cb)
        {
            case GL_RIGHT:
            case GL_FRONT_RIGHT:
            case GL_BACK_RIGHT:      cb = GL_NONE; break;

            case GL_FRONT:           cb = GL_FRONT_LEFT; break;
            case GL_BACK:            cb = GL_BACK_LEFT; break;
            case GL_FRONT_AND_BACK:  cb = GL_LEFT; break;

            default:                 return;
        }
        glDrawBuffer (cb);
    }
}


/* GL: monobuffer - configures the framebuffer for monoscopic viewing */
void
monobuffer (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    IGL_CTX()->newFlags &= ~IGL_WFLAGS_STEREOBUFFER;
}


/* GL: stereobuffer - configures the framebuffer for stereoscopic viewing */
void
stereobuffer (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    IGL_CTX()->newFlags |= IGL_WFLAGS_STEREOBUFFER;
}


/* GL: getbuffer - indicates which buffers are enabled for writing */
long
getbuffer (void)
{
    int cb, zb;
    long ret = 0;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    glGetIntegerv (GL_DEPTH_TEST, &zb);
    glGetIntegerv (GL_DRAW_BUFFER, &cb);

    if (zb == 1)
        ret = DRAWZBUFFER;

    switch (cb)
    {
        case GL_FRONT:              ret |= FRNTBUFFER; break;
        case GL_BACK:               ret |= BCKBUFFER; break;
        case GL_FRONT_AND_BACK:     ret |= (BCKBUFFER | FRNTBUFFER); break;
    }

    return (ret);
}


#if 0
____________________________ Z-buffer functions ____________________________
#endif
/* GL: zbuffer - enable or disable z-buffer operation in the current framebuffer */
void
zbuffer (Boolean enable)
{
    _igl_trace (__func__,FALSE,"%d",enable);
    igl->api->zbuffer (enable);
}

void
iglExec_zbuffer (Boolean enable)
{
    IGL_CHECKWNDV ();

    if (enable)
        glEnable (GL_DEPTH_TEST);
    else
        glDisable (GL_DEPTH_TEST);
}


/* GL: getzbuffer - returns whether z-buffering is on or off */
Boolean
getzbuffer (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (FALSE);

    return (glIsEnabled (GL_DEPTH_TEST));
}


/* GL: zfunction - specifies the function used for z-buffer comparison by the current framebuffer */
void
zfunction (long func)
{
    _igl_trace (__func__,FALSE,"%ld",func);
    igl->api->zfunction (func);
}

void
iglExec_zfunction (long func)
{
    int oglFunc;

    IGL_CHECKWNDV ();

    switch (func)
    {
        case ZF_LESS:                oglFunc = GL_LESS; break;
        case ZF_EQUAL:               oglFunc = GL_EQUAL; break;
        case ZF_LEQUAL:              oglFunc = GL_LEQUAL; break;
        case ZF_GREATER:             oglFunc = GL_GREATER; break;
        case ZF_NOTEQUAL:            oglFunc = GL_NOTEQUAL; break;
        case ZF_GEQUAL:              oglFunc = GL_GEQUAL; break;
        case ZF_ALWAYS:              oglFunc = GL_ALWAYS; break;
        case ZF_NEVER:               oglFunc = GL_NEVER; break;

        default:                     return;
    }
    glDepthFunc (oglFunc);
}


/* GL: zwritemask - specifies a write mask for the z-buffer of the current framebuffer */
void
zwritemask (unsigned long mask)
{
    _igl_trace (__func__,TRUE,"%lx",mask);
    IGL_CHECKWNDV ();

    /* not supported in OpenGL */
    glDepthMask (mask != 0);
}


/* GL: zclear - initializes the z-buffer of the current framebuffer */
void
zclear (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->zclear ();
}

void
iglExec_zclear (void)
{
    IGL_CHECKWNDV ();

    glClearDepth (1.0);
    glClear (GL_DEPTH_BUFFER_BIT);
}


/* GL: czclear - clears the color bitplanes and the z-buffer simultaneously */
void
czclear (unsigned long cval, long zval)
{
    _igl_trace (__func__,FALSE,"%lx,%ld",cval,zval);
    igl->api->czclear (cval, zval);
}

void
iglExec_czclear (unsigned long cval, long zval)
{
    IGL_CHECKWNDV ();

    glClearColor (GetRValue(cval)/255.0f, GetGValue(cval)/255.0f,
                    GetBValue(cval)/255.0f,GetAValue(cval)/255.0f);
    glClearDepth ((double)zval / igl->gdZMAX);

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


/* GL: zbsize - specifies the number of bit planes desired to comprise the Z buffer */
void
zbsize (long planes)
{
    _igl_trace (__func__,TRUE,"%ld",planes);
    IGL_CHECKWNDV ();

    if (planes < 0)
        return;

    /* round up to multiple of 8 */
    planes = (planes+7) & ~7;
    if (planes > 32)
        planes = 32;

    IGL_CTX ()->zbPlanes = (unsigned char)planes;
}


/* GL: zdraw - enables or disables drawing to the z-buffer */
void
zdraw (Boolean enable)
{
    _igl_trace (__func__,TRUE,"%d",enable);
    IGL_CHECKWNDV ();

    if (enable)
        IGL_CTX ()->state |= IGL_WSTATE_ZDRAW;
    else
        IGL_CTX ()->state &= ~IGL_WSTATE_ZDRAW;
}


/* GL: lsetdepth - sets the depth range */
void
lsetdepth (long near_, long far_)
{
    _igl_trace (__func__,FALSE,"%ld,%ld",near_,far_);
    igl->api->lsetdepth (near_, far_);
}

void
iglExec_lsetdepth (long near_, long far_)
{
    IGL_CHECKWNDV ();

    glDepthRange ((double)near_ / igl->gdZMAX, (double)far_ / igl->gdZMAX);
}


/* GL: obsolete routines */
void
setdepth (Screencoord near_, Screencoord far_)
{
    _igl_trace (__func__,FALSE,"%d,%d",near_,far_);
    igl->api->lsetdepth ((long)near_, (long)far_);
}


void
getdepth (Screencoord *near_, Screencoord *far_)
{
    GLfloat d[2];

    _igl_trace (__func__,TRUE,"%p,%p",near_,far_);
    IGL_CHECKWNDV ();

    glGetFloatv (GL_DEPTH_RANGE, d);
    if (near_ != NULL)
        *near_ = (Screencoord)(d[0] * igl->gdZMAX);
    if (far_ != NULL)
        *far_ =  (Screencoord)(d[1] * igl->gdZMAX);
}


#if 0
_________________________ stencil buffer functions _________________________
#endif
/* GL: sclear - clear the stencil planes to a specified value */
void
sclear (unsigned long sval)
{
    _igl_trace (__func__,FALSE,"%lu",sval);
    igl->api->sclear (sval);
}

void
iglExec_sclear (unsigned long sval)
{
    IGL_CHECKWNDV ();

    glClearStencil (sval);
    glClear (GL_STENCIL_BUFFER_BIT);
}


/* GL: stencil - alter the operating parameters of the stencil */
static GLenum
_igl_stencilop (long op)
{
    switch (op)
    {
        case ST_KEEP:               return GL_KEEP;
        case ST_ZERO:               return GL_ZERO;
        case ST_REPLACE:            return GL_REPLACE;
        case ST_INCR:               return GL_INCR;
        case ST_DECR:               return GL_DECR;
        case ST_INVERT:             return GL_INVERT;
    }
    /* Oy vey, what to do now? */
    return GL_NEVER;
}

void
stencil (long enable, unsigned long ref, long func, unsigned long mask, long fail, long pass, long zpass)
{
    _igl_trace (__func__,FALSE,"%ld,...",enable);
    igl->api->stencil (enable, ref, func, mask, fail, pass, zpass);
}

void
iglExec_stencil (long enable, unsigned long ref, long func, unsigned long mask, long fail, long pass, long zpass)
{
    GLenum ofunc, ofail, opass, ozpass;

    IGL_CHECKWNDV ();

    if (enable)
    {
        glEnable (GL_STENCIL_TEST);
        switch (func)
        {
            case SF_NEVER:          ofunc = GL_NEVER; break;
            case SF_LESS:           ofunc = GL_LESS; break;
            case SF_EQUAL:          ofunc = GL_EQUAL; break;
            case SF_LEQUAL:         ofunc = GL_LEQUAL; break;
            case SF_GREATER:        ofunc = GL_GREATER; break;
            case SF_NOTEQUAL:       ofunc = GL_NOTEQUAL; break;
            case SF_GEQUAL:         ofunc = GL_GEQUAL; break;
            case SF_ALWAYS:         ofunc = GL_ALWAYS; break;

            default: return;
        }

        ofail = _igl_stencilop (fail);
        opass = _igl_stencilop (pass);
        ozpass = _igl_stencilop (zpass);

        if (ofail != GL_NEVER && opass != GL_NEVER && ozpass != GL_NEVER)
        {
            glStencilFunc (ofunc, ref, mask);
            glStencilOp (ofail, opass, ozpass);
        }
    }
    else
    {
        glDisable (GL_STENCIL_TEST);
    }
}


/* GL: swritemask - specify which stencil bits can be written */
void
swritemask (unsigned long mask)
{
    glStencilMask (mask);
}


/* GL: stensize - specify the number of bit planes desired to comprise the stencil buffer */
void
stensize (long planes)
{
    _igl_trace (__func__,TRUE,"%ld",planes);
    IGL_CHECKWNDV ();

    if (planes < 0 || planes > 8)
        return;

    IGL_CTX ()->stenPlanes = (unsigned char)planes;
    glStencilMask ((1 << planes) - 1);
}


#if 0
______________________ accumulation buffer functions _______________________
#endif
/* GL: acbuf - operate on the accumulation buffer */
void
acbuf (long op, float value)
{
    _igl_trace (__func__,FALSE,"%ld,%g",op,value);
    igl->api->acbuf (op, value);
}

void
iglExec_acbuf (long op, float value)
{
    IGL_CHECKWNDV ();

    /* NOTE: IrisGL scales color components to unsigned 8 bit values [0..255],
     * thus in a 16 bit acbuf there are 8 "overflow bits" for calculations.
     * OTOH, OpenGL scales components to floats in the range [0..1], and the
     * buffer has a range of [-1..1]. To have the same leeway as in IrisGL, all
     * input must be scaled down by 256, and data rescaled up on read out.
     * AC_CLEAR is 16 bits signed in IrisGL, hence scale factor must be 32768.
     */
    switch (op)
    {
        case AC_CLEAR: /* GL: signed 16 bit integer, OpenGL: [-1,1] */
            glClearAccum (value/32768, value/32768, value/32768, value/32768);
            glClear (GL_ACCUM_BUFFER_BIT);
            break;

        case AC_ACCUMULATE: /* GL: scaled to unsigned 8 bit, OpenGL: [0,1] */
            glAccum (GL_ACCUM, value/256);
            break;

        case AC_CLEAR_ACCUMULATE: /* like AC_ACCUMULATE */
            glAccum (GL_LOAD, value/256);
            break;

        case AC_RETURN: /* GL: scaled to unsigned 8 bit, OpenGL: no scaling */
            glAccum (GL_RETURN, value*256);
            break;

        case AC_MULT:
            glAccum (GL_MULT, value);
            break;

        case AC_ADD:
            glAccum (GL_ADD, value/256);
            break;
    }
}


/* GL: acsize - specify the number of bitplanes per color component in the accumulation buffer */
void
acsize (long planes)
{
    _igl_trace (__func__,TRUE,"%ld",planes);
    IGL_CHECKWNDV ();

    if (planes != 0 && planes != 16)
        return;

    IGL_CTX ()->acPlanes = (unsigned char)planes;
}


#if PLATFORM_X11
/* TODO: experimental code for testing. Implement this (using pbuffer?)! */
static Pixmap pm;
static GLXPixmap gpm;
static GLXContext gc;

/* GL: ilbuffer - allocates buffers for temporary image processing results */
long
ilbuffer (unsigned long buffers)
{
    _igl_trace (__func__,TRUE,"%lu",buffers);

    if (buffers && !gpm)
    {
        int w = DisplayWidth(IGL_DISPLAY, IGL_SCREEN);
        int h = DisplayHeight(IGL_DISPLAY, IGL_SCREEN);
        pm = XCreatePixmap (IGL_DISPLAY, IGL_ROOT, w, h, IGL_VISUAL->depth);
        gpm = glXCreateGLXPixmap (IGL_DISPLAY, IGL_VISUAL, pm);
    }
    else if (!buffers && gpm)
    {
        glXDestroyGLXPixmap (IGL_DISPLAY, gpm);
        XFreePixmap (IGL_DISPLAY, pm);
        gpm = 0;
    }
    return !!buffers;
}


/* GL: ildraw - selects an ilbuffer as the drawing destination */
void
ildraw (unsigned long buffer)
{
    _igl_trace (__func__,TRUE,"%lu",buffer);

    if (buffer)
    {
        if (!gc)
            gc = glXCreateContext (IGL_DISPLAY, IGL_VISUAL, NULL, False);
        glXMakeCurrent (IGL_DISPLAY, gpm, gc);
    }
    else if (gc)
    {
        if (igl->currentWindow < 0)
            glXMakeCurrent (IGL_DISPLAY, IGL_WINDOW (0)->wnd, IGL_WINDOW (0)->layers[IGL_WLAYER_NORMALDRAW].hrc);
        else
            glXMakeCurrent (IGL_DISPLAY, IGL_CTX ()->currentLayer->hwnd, IGL_CTX ()->currentLayer->hrc);
        glXDestroyContext (IGL_DISPLAY, gc);
        gc = NULL;
    }
}
#endif


/* GL: unimplemented stuff */
void
zsource (long src)
{
    _igl_trace (__func__,TRUE,"");
    if (src) /* TODO: valid only for Personal Iris with GL4DPI? */
        glDepthFunc (GL_ALWAYS);
}


void
multisample (Boolean enable)
{
    _igl_trace (__func__,TRUE,"");

    /* in OpenGL 1.2 only available as extension */
    /* a multisampling capable FBconfig must have been choosen as visual */
#ifdef GL_MULTISAMPLE_SGIS
    if (enable)
        glEnable (GL_MULTISAMPLE_SGIS);
    else
        glDisable (GL_MULTISAMPLE_SGIS);
#endif
}


Boolean
getmultisample (void)
{
    GLint rv = FALSE;

    _igl_trace (__func__,TRUE,"");
#ifdef GL_MULTISAMPLE_SGIS
    glGetIntegerv (GL_MULTISAMPLE_SGIS, &rv);
#endif
    return rv;
}


void
mssample (long mode)
{   /* POINT, AREA, CENTER_POINT */
    _igl_trace (__func__,TRUE,"");
}


void
msalpha (long mode)
{   /* MASK, MASK_ONE, ALPHA */
    _igl_trace (__func__,TRUE,"");
}


void
msmask (float mask, Boolean inverse)
{
    _igl_trace (__func__,TRUE,"");
}


void
mspattern (long pattern)
{   /* DEFAULT, 2PASS_[01], 4PASS_[0123] */
    _igl_trace (__func__,TRUE,"");
}


void
mssize (long samples, long zsize, long ssize)
{
    _igl_trace (__func__,TRUE,"");
}
