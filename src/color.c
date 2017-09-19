/*
 * color.c
 *
 * functions affecting the value of pixels written to a color buffer
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


/* NOTE: while the NORMALDRAW layer can be in RGB, all the other layers are
 * always color indexed, with color 0 normally being "fully transparent".
 * mapcolor(), getmcolor() apparently do work in RGB mode, but color() doesn't.
 */


/* standard colormaps for GL */
static unsigned long stdcolmap[256] = {
    RGB(  0,  0,  0), RGB(255,  0,  0), RGB(  0,255,  0), RGB(255,255,  0),
    RGB(  0,  0,255), RGB(255,  0,255), RGB(  0,255,255), RGB(255,255,255),
    RGB( 85, 85, 85), RGB(198,113,113), RGB(113,198,113), RGB(142,142, 56),
    RGB(113,113,198), RGB(142, 56,142), RGB( 56,142,142), RGB(170,170,170),
    RGB(  0,  0,  0), RGB(  0,  0,  0), RGB(  0,  0,  0), RGB(  0,  0,  0),
    RGB(  0,  0,  0), RGB(  0,  0,  0), RGB(  0,  0,  0), RGB(  0,  0,  0),
    RGB(  0,  0,  0), RGB(  0,  0,  0), RGB(  0,  0,  0), RGB(  0,  0,  0),
    RGB(  0,  0,  0), RGB(  0,  0,  0), RGB(  0,  0,  0), RGB(  0,  0,  0),
    RGB( 10, 10, 10), RGB( 20, 20, 20), RGB( 30, 30, 30), RGB( 40, 40, 40),
    RGB( 51, 51, 51), RGB( 61, 61, 61), RGB( 71, 71, 71), RGB( 81, 81, 81),
    RGB( 91, 91, 91), RGB(102,102,102), RGB(112,112,112), RGB(122,122,122),
    RGB(132,132,132), RGB(142,142,142), RGB(153,153,153), RGB(163,163,163),
    RGB(173,173,173), RGB(183,183,183), RGB(193,193,193), RGB(204,204,204),
    RGB(214,214,214), RGB(224,224,224), RGB(234,234,234), RGB(244,244,244),
    RGB(  0,  0,  0), RGB(  0, 36,  0), RGB(  0, 72,  0), RGB(  0,109,  0),
    RGB(  0,145,  0), RGB(  0,182,  0), RGB(  0,218,  0), RGB(  0,255,  0),
    RGB( 63,  0,  0), RGB( 63, 36,  0), RGB( 63, 72,  0), RGB( 63,109,  0),
    RGB( 63,145,  0), RGB( 63,182,  0), RGB( 63,218,  0), RGB( 63,255,  0),
    RGB(127,  0,  0), RGB(127, 36,  0), RGB(127, 72,  0), RGB(127,109,  0),
    RGB(127,145,  0), RGB(127,182,  0), RGB(127,218,  0), RGB(127,255,  0),
    RGB(191,  0,  0), RGB(191, 36,  0), RGB(191, 72,  0), RGB(191,109,  0),
    RGB(191,145,  0), RGB(191,182,  0), RGB(191,218,  0), RGB(191,255,  0),
    RGB(255,  0,  0), RGB(255, 36,  0), RGB(255, 72,  0), RGB(255,109,  0),
    RGB(255,145,  0), RGB(255,182,  0), RGB(255,218,  0), RGB(255,255,  0),
    RGB(  0,  0, 63), RGB(  0, 36, 63), RGB(  0, 72, 63), RGB(  0,109, 63),
    RGB(  0,145, 63), RGB(  0,182, 63), RGB(  0,218, 63), RGB(  0,255, 63),
    RGB( 63,  0, 63), RGB( 63, 36, 63), RGB( 63, 72, 63), RGB( 63,109, 63),
    RGB( 63,145, 63), RGB( 63,182, 63), RGB( 63,218, 63), RGB( 63,255, 63),
    RGB(127,  0, 63), RGB(127, 36, 63), RGB(127, 72, 63), RGB(127,109, 63),
    RGB(127,145, 63), RGB(127,182, 63), RGB(127,218, 63), RGB(127,255, 63),
    RGB(191,  0, 63), RGB(191, 36, 63), RGB(191, 72, 63), RGB(191,109, 63),
    RGB(191,145, 63), RGB(191,182, 63), RGB(191,218, 63), RGB(191,255, 63),
    RGB(255,  0, 63), RGB(255, 36, 63), RGB(255, 72, 63), RGB(255,109, 63),
    RGB(255,145, 63), RGB(255,182, 63), RGB(255,218, 63), RGB(255,255, 63),
    RGB(  0,  0,127), RGB(  0, 36,127), RGB(  0, 72,127), RGB(  0,109,127),
    RGB(  0,145,127), RGB(  0,182,127), RGB(  0,218,127), RGB(  0,255,127),
    RGB( 63,  0,127), RGB( 63, 36,127), RGB( 63, 72,127), RGB( 63,109,127),
    RGB( 63,145,127), RGB( 63,182,127), RGB( 63,218,127), RGB( 63,255,127),
    RGB(127,  0,127), RGB(127, 36,127), RGB(127, 72,127), RGB(127,109,127),
    RGB(127,145,127), RGB(127,182,127), RGB(127,218,127), RGB(127,255,127),
    RGB(191,  0,127), RGB(191, 36,127), RGB(191, 72,127), RGB(191,109,127),
    RGB(191,145,127), RGB(191,182,127), RGB(191,218,127), RGB(191,255,127),
    RGB(255,  0,127), RGB(255, 36,127), RGB(255, 72,127), RGB(255,109,127),
    RGB(255,145,127), RGB(255,182,127), RGB(255,218,127), RGB(255,255,127),
    RGB(  0,  0,191), RGB(  0, 36,191), RGB(  0, 72,191), RGB(  0,109,191),
    RGB(  0,145,191), RGB(  0,182,191), RGB(  0,218,191), RGB(  0,255,191),
    RGB( 63,  0,191), RGB( 63, 36,191), RGB( 63, 72,191), RGB( 63,109,191),
    RGB( 63,145,191), RGB( 63,182,191), RGB( 63,218,191), RGB( 63,255,191),
    RGB(127,  0,191), RGB(127, 36,191), RGB(127, 72,191), RGB(127,109,191),
    RGB(127,145,191), RGB(127,182,191), RGB(127,218,191), RGB(127,255,191),
    RGB(191,  0,191), RGB(191, 36,191), RGB(191, 72,191), RGB(191,109,191),
    RGB(191,145,191), RGB(191,182,191), RGB(191,218,191), RGB(191,255,191),
    RGB(255,  0,191), RGB(255, 36,191), RGB(255, 72,191), RGB(255,109,191),
    RGB(255,145,191), RGB(255,182,191), RGB(255,218,191), RGB(255,255,191),
    RGB(  0,  0,255), RGB(  0, 36,255), RGB(  0, 72,255), RGB(  0,109,255),
    RGB(  0,145,255), RGB(  0,182,255), RGB(  0,218,255), RGB(  0,255,255),
    RGB( 63,  0,255), RGB( 63, 36,255), RGB( 63, 72,255), RGB( 63,109,255),
    RGB( 63,145,255), RGB( 63,182,255), RGB( 63,218,255), RGB( 63,255,255),
    RGB(127,  0,255), RGB(127, 36,255), RGB(127, 72,255), RGB(127,109,255),
    RGB(127,145,255), RGB(127,182,255), RGB(127,218,255), RGB(127,255,255),
    RGB(191,  0,255), RGB(191, 36,255), RGB(191, 72,255), RGB(191,109,255),
    RGB(191,145,255), RGB(191,182,255), RGB(191,218,255), RGB(191,255,255),
    RGB(255,  0,255), RGB(255, 36,255), RGB(255, 72,255), RGB(255,109,255),
    RGB(255,145,255), RGB(255,182,255), RGB(255,218,255), RGB(255,255,255),
};

/* colormap for overlay/popup, 0:transparent 1:red 2:black 3:white */
static unsigned long ovlcolmap[4] = {
    RGB(  0,  0,  0), RGB(255,  0,  0), RGB(  0,  0,  0), RGB(255,255,255),
};

/* colormap for cursor, 0:transparent 1:red 2:white 3:red */
static unsigned long curscolmap[4] = {
    RGB(  0,  0,  0), RGB(255,  0,  0), RGB(255,255,255), RGB(255,  0,  0),
};

/* initialize color table management */
void
_igl_initColor (void)
{
    int i;

    for(i = 0; i < 256; i++)
        igl->normalColorPalette[i] = stdcolmap[i];
    for(i = 0; i < 4; i++)
        igl->underColorPalette[i] = igl->overColorPalette[i] =
                                    igl->popupColorPalette[i] = ovlcolmap[i];
    for(i = 0; i < 4; i++)
        igl->cursorColorPalette[i] = curscolmap[i];
}

/* reset color tables on greset */
void
_igl_resetColor (void)
{
    int i;

    for(i = 0; i < 8; i++)
        igl->normalColorPalette[i] = stdcolmap[i];
    for(i = 0; i < 4; i++)
        igl->popupColorPalette[i] = ovlcolmap[i];
    for(i = 0; i < 4; i++)
        igl->cursorColorPalette[i] = curscolmap[i];
}

/* set current RGBA color from 8 bit subcolors */
static inline void
_igl_currentColor (long r, long g, long b, long a)
{
    igl_windowT *wptr = IGL_CTX ();

    wptr->currentColor[0] = (r < 0 ? 0 : (r > 255 ? 255 : r));
    wptr->currentColor[1] = (g < 0 ? 0 : (g > 255 ? 255 : g));
    wptr->currentColor[2] = (b < 0 ? 0 : (b > 255 ? 255 : b));
    wptr->currentColor[3] = (a < 0 ? 0 : (a > 255 ? 255 : a));
    wptr->state |= IGL_VSTATE_COLORED;
}


/* GL: color, colorf - sets the color index in the current draw mode */
void
color (Colorindex c)
{
    _igl_trace (__func__,FALSE,"%d",c);
    igl->api->color (c);
}

void
iglExec_color (Colorindex c)
{
    unsigned long clr, mapSize, mapOffs;
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if ((wptr->flags & IGL_WFLAGS_RGBA) && wptr->drawMode == NORMALDRAW)
        return;

    mapOffs = 0;
    mapSize = wptr->colormapSize;
    if ((wptr->flags & IGL_WFLAGS_MULTIMAP) && wptr->drawMode == NORMALDRAW)
    {   /* multimap mode in NORMALDRAW */
        mapOffs = wptr->multimapIndex*IGL_MAXMULTIMAPCOLORS;
        mapSize = IGL_MAXMULTIMAPCOLORS;
    }

    c &= ~wptr->writeMask;
    if (c < mapSize)
    {
        wptr->colorIndex = c;
        clr = wptr->colorPalette[c + mapOffs];
        /* in other layers than NORMALDRAW color index 0 is transparent */
        _igl_currentColor (GetRValue(clr), GetGValue(clr), GetBValue(clr),
                        (c == 0 && wptr->drawMode != NORMALDRAW ? 0x00 : 0xff));
        glColor4ubv (wptr->currentColor);
    }
}


void
colorf (float c)
{
    _igl_trace (__func__,FALSE,"%g",c);
    igl->api->color ((Colorindex)c);
}


/* GL: getcolor - returns the current color */
long
getcolor (void)
{
    long color;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (-1);

   color = IGL_CTX ()->colorIndex;
    _igl_trace (" = ",TRUE,"%ld",color);
    return color;
}


/* GL: mapcolor - changes a color map entry */
void
mapcolor (Colorindex i, short r, short g, short b)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",i,r,g,b);
    igl->api->mapcolor (i, r, g, b);
}

void
iglExec_mapcolor (Colorindex i, short r, short g, short b)
{
    unsigned int mapSize, mapOffs;
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    mapOffs = 0;
    mapSize = wptr->colormapSize;
    if ((wptr->flags & IGL_WFLAGS_MULTIMAP) && wptr->drawMode == NORMALDRAW)
    {   /* multimap mode in NORMALDRAW */
        mapOffs = wptr->multimapIndex*IGL_MAXMULTIMAPCOLORS;
        mapSize = IGL_MAXMULTIMAPCOLORS;
    }

    if (i < mapSize)
    {
        wptr->colorPalette[i + mapOffs] = RGB (r, g, b);

        /* for very old stuff drawing iteratively without qtest/qread */
        if (igl->glcSLOWMAPCOLORS)
            glFlush ();

        /* if the cursor color is modified, recolor the cursor now */
        if (wptr->drawMode == CURSORDRAW && i > 0)
            _igl_recolorCursor (wptr);
    }
}


/* GL: getmcolor - gets a copy of the RGB values for a color map entry */
void
getmcolor (Colorindex i, short *r, short *g, short *b)
{
    unsigned int mapSize, mapOffs;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%d,%p,%p,%p",i,r,g,b);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    mapOffs = 0;
    mapSize = wptr->colormapSize;
    if ((wptr->flags & IGL_WFLAGS_MULTIMAP) && wptr->drawMode == NORMALDRAW)
    {   /* multimap mode in NORMALDRAW */
        mapOffs = wptr->multimapIndex*IGL_MAXMULTIMAPCOLORS;
        mapSize = IGL_MAXMULTIMAPCOLORS;
    }

    if (i < mapSize && r && g && b)
    {
        *r = GetRValue (wptr->colorPalette[i + mapOffs]);
        *g = GetGValue (wptr->colorPalette[i + mapOffs]);
        *b = GetBValue (wptr->colorPalette[i + mapOffs]);
        _igl_trace (" = ",TRUE,"{%d,%d,%d}",*r,*g,*b);
    }
}


/* GL: writemask - grants write permission to bitplanes */
void
writemask (Colorindex wtm)
{
    _igl_trace (__func__,TRUE,"%d",wtm);
    IGL_CHECKWNDV ();

    /* NOTE: no full implementation possible since there is no colormap mode.
     * Only consider wtm when colormap is accessed, and imply all masked bits
     * to be zero, which isn't neccessarily true.
     */
    IGL_CTX ()->writeMask = ~wtm;
}


/* GL: getwritemask - returns the current writemask */
long
getwritemask (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (-1);

    return (~IGL_CTX()->writeMask);
}


/* GL: setshade - obsolete routine */
void
setshade (Colorindex shade)
{
    /* same as color() */
    _igl_trace (__func__,FALSE,"%d",shade);
    igl->api->color (shade);
}


/* GL: getshade - obsolete routine */
long
getshade (void)
{
    /* same as getcolor() */
    _igl_trace (__func__,TRUE,"");
    return (IGL_CTX()->colorIndex);
}


/* GL: RGBcolor - sets the current color in RGB mode */
void
RGBcolor (short r, short g, short b)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d",r,g,b);
    igl->api->RGBcolor (r, g, b);
}

void
iglExec_RGBcolor (short r, short g, short b)
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if ((wptr->flags & IGL_WFLAGS_RGBA) && wptr->drawMode == NORMALDRAW)
    {
        _igl_currentColor (r, g, b, 0xff);
        glColor3ubv (wptr->currentColor);
    }
}


/* GL: gRGBcolor - gets the current RGB color values */
void
gRGBcolor (short *r, short *g, short *b)
{
    _igl_trace (__func__,TRUE,"%p,%p,%p",r,g,b);
    IGL_CHECKWNDV ();

    if ((IGL_CTX()->flags & IGL_WFLAGS_RGBA) && r != NULL && g != NULL && b != NULL)
    {
        *r = IGL_CTX ()->currentColor[0];
        *g = IGL_CTX ()->currentColor[1];
        *b = IGL_CTX ()->currentColor[2];
        _igl_trace (" = ",TRUE,"{%d,%d,%d}",*r,*g,*b);
    }
}


/* GL: cpack - specifies RGBA color with a single packed 32-bit integer */
void
cpack (unsigned long color)
{
    _igl_trace (__func__,FALSE,"%lx",color);
    igl->api->cpack (color);
}

void
iglExec_cpack (unsigned long color)
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if ((wptr->flags & IGL_WFLAGS_RGBA) && wptr->drawMode == NORMALDRAW)
    {
        /* the color is in ABGR mode */
        _igl_currentColor (GetRValue(color), GetGValue(color),
                           GetBValue(color), GetAValue(color));
        glColor4ubv (wptr->currentColor);
    }
}


/* GL: RGBwritemask - grants write access to a subset of available bitplanes */
void
RGBwritemask (short redm, short greenm, short bluem)
{
    wmpack (RGBA(redm,greenm,bluem,0xff));
}


/* GL: gRGBmask - returns the current RGB writemask */
void
gRGBmask (short *redm, short *greenm, short *bluem)
{
    GLint wm[4];

    _igl_trace (__func__,TRUE,"%p,%p,%p",redm,greenm,bluem);
    IGL_CHECKWNDV ();

    glGetIntegerv (GL_COLOR_WRITEMASK, wm);
    *redm = wm[0] == GL_TRUE ? 0xff : 0x00;
    *greenm = wm[1] == GL_TRUE ? 0xff : 0x00;
    *bluem = wm[2] == GL_TRUE ? 0xff : 0x00;
}


/* GL: wmpack - specifies RGBA writemask with a single packed integer */
void
wmpack (unsigned long color)
{
    _igl_trace (__func__,TRUE,"%lx",color);
    IGL_CHECKWNDV ();

    /* TODO: OpenGL doesn't support changes to individual bits - color mask can
     * be either enabled ot disabled with glColorMask(). Could try to mask RGB
     * color when writing to the current color, assuming the masked bits are 0.
     * (could maybe do some trickery with glLogicOp?)
     */
    glColorMask (GetRValue(color) ? GL_TRUE : GL_FALSE,
        GetGValue(color) ? GL_TRUE : GL_FALSE,
        GetBValue(color) ? GL_TRUE : GL_FALSE,
        GetAValue(color) ? GL_TRUE : GL_FALSE);
}


/* GL: afunction - specify alpha test function */
void
afunction (long ref, long func)
{
    _igl_trace (__func__,FALSE,"%ld,%ld",ref,func);
    igl->api->afunction (ref, func);
}

void
iglExec_afunction (long ref, long func)
{
    int oglFunc;

    IGL_CHECKWNDV ();

    switch (func)
    {
        case AF_LESS:               oglFunc = GL_LESS; break;
        case AF_EQUAL:              oglFunc = GL_EQUAL; break;
        case AF_LEQUAL:             oglFunc = GL_LEQUAL; break;
        case AF_GREATER:            oglFunc = GL_GREATER; break;
        case AF_NOTEQUAL:           oglFunc = GL_NOTEQUAL; break;
        case AF_GEQUAL:             oglFunc = GL_GEQUAL; break;
        case AF_ALWAYS:             oglFunc = GL_ALWAYS; break;
        case AF_NEVER:              oglFunc = GL_NEVER; break;

        default:                    return;
    }

    if (func == AF_ALWAYS)
        glDisable (GL_ALPHA_TEST);
    else
    {
        /* ref is 0-255 in IrisGL, 0.0f-1.0f in OpenGL */
        glAlphaFunc (oglFunc, (float)ref/255.0f);
        glEnable (GL_ALPHA_TEST);
    }
}


/* GL: blendcolor - specifies a constant color for blending */
void
blendcolor (float r, float g, float b, float a)
{
    _igl_trace (__func__,TRUE,"%g,%g,%g,%g",r,g,b,a);
    IGL_CHECKWNDV ();

    glBlendColor(r, g, b, a);
}


/* GL: blendfunction - computes a blended color value for a pixel */
void
blendfunction (long sfactor, long dfactor)
{
    _igl_trace (__func__,FALSE,"%ld,%ld",sfactor,dfactor);
    igl->api->blendfunction (sfactor, dfactor);
}

void
iglExec_blendfunction (long sfactor, long dfactor)
{
    int osf, odf;

    IGL_CHECKWNDV ();

    switch (sfactor)
    {
        case BF_ZERO:               osf = GL_ZERO; break;
        case BF_ONE:                osf = GL_ONE; break;
        case BF_DC:                 osf = GL_DST_COLOR; break;
        case BF_MDC:                osf = GL_ONE_MINUS_DST_COLOR; break;
        case BF_SA:                 osf = GL_SRC_ALPHA; break;
        case BF_MSA:                osf = GL_ONE_MINUS_SRC_ALPHA; break;
        case BF_DA:                 osf = GL_DST_ALPHA; break;
        case BF_MDA:                osf = GL_ONE_MINUS_DST_ALPHA; break;
        case BF_MIN_SA_MDA:         osf = GL_SRC_ALPHA_SATURATE; break;

        case BF_CC:                 osf = GL_CONSTANT_COLOR; break;
        case BF_MCC:                osf = GL_ONE_MINUS_CONSTANT_COLOR; break;
        case BF_CA:                 osf = GL_CONSTANT_ALPHA; break;
        case BF_MCA:                osf = GL_ONE_MINUS_CONSTANT_ALPHA; break;
#if 0   /* TODO: glBlendEquation(GL_FUNC_ADD/GL_MIN/GL_MAX) */
        case BF_MIN:                osf = GL_MIN; break;
        case BF_MAX:                osf = GL_MAX; break;
#endif
        default:                    osf = GL_DST_COLOR; break;
    }

    switch (dfactor)
    {
        case BF_ZERO:               odf = GL_ZERO; break;
        case BF_ONE:                odf = GL_ONE; break;
        case BF_SC:                 odf = GL_SRC_COLOR; break;
        case BF_MSC:                odf = GL_ONE_MINUS_SRC_COLOR; break;
        case BF_SA:                 odf = GL_SRC_ALPHA; break;
        case BF_MSA:                odf = GL_ONE_MINUS_SRC_ALPHA; break;
        case BF_DA:                 odf = GL_DST_ALPHA; break;
        case BF_MDA:                odf = GL_ONE_MINUS_DST_ALPHA; break;

        case BF_CC:                 odf = GL_CONSTANT_COLOR; break;
        case BF_MCC:                odf = GL_ONE_MINUS_CONSTANT_COLOR; break;
        case BF_CA:                 odf = GL_CONSTANT_ALPHA; break;
        case BF_MCA:                odf = GL_ONE_MINUS_CONSTANT_ALPHA; break;

        default:                    odf = GL_SRC_COLOR; break;
    }

    if (sfactor == BF_ONE && dfactor == BF_ZERO)
    {
        /* this disables blending with IrisGL */
        glDisable (GL_BLEND);
    }
    else
    {
        glEnable (GL_BLEND);
        glBlendFunc (osf, odf);
        /* man page says to force logicop(LO_SRC) = glLogicOp(GL_COPY) */
        glLogicOp (GL_COPY);
    }
}


/* GL: dither - controls the dithering of pixels */
void
dither (long mode)
{
    _igl_trace (__func__,FALSE,"%ld",mode);
    igl->api->dither (mode);
}

void
iglExec_dither (long mode)
{
    IGL_CHECKWNDV ();

    if (mode == DT_OFF)
        glDisable (GL_DITHER);
    else
        glEnable (GL_DITHER);
}


/* GL: logicop - specifies a logical operation for pixel writes */
void
logicop (long opcode)
{
    _igl_trace (__func__,FALSE,"%ld",opcode);
    igl->api->logicop (opcode);
}

void
iglExec_logicop (long opcode)
{
    int glop;

    IGL_CHECKWNDV ();

    switch (opcode)
    {
        case LO_ZERO:               glop = GL_CLEAR; break;
        case LO_ONE:                glop = GL_SET; break;
        case LO_SRC:                glop = GL_COPY; break;
        case LO_NSRC:               glop = GL_COPY_INVERTED; break;
        case LO_DST:                glop = GL_NOOP; break;
        case LO_NDST:               glop = GL_INVERT; break;
        case LO_AND:                glop = GL_AND; break;
        case LO_NAND:               glop = GL_NAND; break;
        case LO_OR:                 glop = GL_OR; break;
        case LO_NOR:                glop = GL_NOR; break;
        case LO_XOR:                glop = GL_XOR; break;
        case LO_XNOR:               glop = GL_EQUIV; break;
        case LO_ANDR:               glop = GL_AND_REVERSE; break;
        case LO_ANDI:               glop = GL_AND_INVERTED; break;
        case LO_ORR:                glop = GL_OR_REVERSE; break;
        case LO_ORI:                glop = GL_OR_INVERTED; break;

        default: return;
    }

    glLogicOp (glop);
    if (opcode != LO_SRC)
    {
        /* man page says to force blending to (BF_ONE, BF_ZERO), = disabled */
        glDisable (GL_BLEND);
    }
}


/* GL: onemap - organizes the color map as one large map */
void
onemap (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    IGL_CTX ()->newFlags &= ~IGL_WFLAGS_MULTIMAP;
}


/* GL: multimap - organizes the color map as a number of smaller maps */
void
multimap (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    /* TODO: only valid in NORMALDRAW */
    IGL_CTX ()->newFlags |= IGL_WFLAGS_MULTIMAP;
}


/* GL: getcmmode - returns the current color map mode */
Boolean
getcmmode (void)
{
    _igl_trace (__func__,TRUE,"");   /* TRUE means onemap() */
    IGL_CHECKWND (TRUE);

    return ((IGL_CTX ()->flags & IGL_WFLAGS_MULTIMAP) ? FALSE : TRUE);
}


/* GL: setmap - selects one of the small color maps provided by multimap mode */
void
setmap (short mapnum)
{
    _igl_trace (__func__,TRUE,"%d",mapnum);
    IGL_CHECKWNDV ();

    if ((IGL_CTX ()->flags & IGL_WFLAGS_MULTIMAP) && mapnum >= 0 && mapnum < 16)
        IGL_CTX ()->multimapIndex = (unsigned char)mapnum;
}


/* GL: getmap - returns the number of the current color map */
long
getmap (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    return ((IGL_CTX ()->flags & IGL_WFLAGS_MULTIMAP) ? (long)IGL_CTX ()->multimapIndex : 0);
}


/* GL: pupcolor - specifies the current pop-up drawing color */
void
pupcolor (long clr)
{
    Colorindex c = clr;
    igl_layerT *lptr;

    _igl_trace (__func__,TRUE,"%ld",clr);
    IGL_CHECKWNDV ();

    lptr = &IGL_CTX ()->layers[IGL_WLAYER_PUPDRAW];

    if (c < lptr->_colormapSize)
    {
        /* set color index in popup layer */
        lptr->_colorIndex = c;
        clr = lptr->_colorPalette[c];
        /* in PUPDRAW color index 0 is transparent */
        lptr->_currentColor[0] = GetRValue(clr);
        lptr->_currentColor[1] = GetGValue(clr);
        lptr->_currentColor[2] = GetBValue(clr);
        lptr->_currentColor[3] = (c == 0 ? 0x00 : 0xff);

        /* if PUPDRAW is the active layer, color is new current OpenGL color */
        if (IGL_CTX ()->drawMode == PUPDRAW)
            glColor4ubv (lptr->_currentColor);
    }
}


/* AIX GL extensions */
void
mapcolors (short start_idx, short end_idx, short r[], short g[], short b[])
{
    int i;
    
    _igl_trace (__func__,TRUE,"%d,%d,%p,%p,%p",start_idx,end_idx,r,g,b);
    IGL_CHECKWNDV ();

    for (i = start_idx; i <= end_idx; i++)
        mapcolor (i, *r++, *g++, *b++);
}
    
void
getmcolors (short start_idx, short end_idx, short *r, short *g, short *b)
{
    int i;

    _igl_trace (__func__,TRUE,"%d,%d,%p,%p,%p",start_idx,end_idx,r,g,b);
    IGL_CHECKWNDV ();

    for (i = start_idx; i <= end_idx; i++)
        getmcolor (i, r++, g++, b++);
}   


/* GL: unimplemented stuff */
void
blink (short rate, Colorindex i, short red, short green, short blue)
{
    _igl_trace (__func__,TRUE,"");
}


void
cyclemap (short duration, short map, short nxtmap)
{
    _igl_trace (__func__,TRUE,"");
}


#if 0
____________________________ c functions ____________________________
#endif
/* GL: c[34][fis]- sets the RGB (or RGBA) values for the current color vector */
void
c4f (float cv[4])
{
    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(g),cv,IGL_V4PAR(cv));
    igl->api->c4f (cv);
}

void
iglExec_c4f (float cv[4])
{
    IGL_CHECKWNDV ();

    if ((IGL_CTX ()->flags & IGL_WFLAGS_RGBA) && IGL_CTX ()->drawMode == NORMALDRAW)
    {
        _igl_currentColor (cv[0]*255, cv[1]*255, cv[2]*255, cv[3]*255);
        glColor4fv (cv);
    }
}


void
c4i (int cv[4])
{
    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(d),cv,IGL_V4PAR(cv));
    igl->api->c4i (cv);
}

void
iglExec_c4i (int cv[4])
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if ((wptr->flags & IGL_WFLAGS_RGBA) && wptr->drawMode == NORMALDRAW)
    {
        _igl_currentColor (cv[0], cv[1], cv[2], cv[3]);
        glColor4ubv (wptr->currentColor);
    }
}


void
c4s (short cv[4])
{
    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(d),cv,IGL_V4PAR(cv));
    igl->api->c4s (cv);
}

void
iglExec_c4s (short cv[4])
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if ((wptr->flags & IGL_WFLAGS_RGBA) && wptr->drawMode == NORMALDRAW)
    {
        _igl_currentColor (cv[0], cv[1], cv[2], cv[3]);
        glColor4ubv (wptr->currentColor);
    }
}


void
c3f (float cv[3])
{
    float cv4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(g),cv,IGL_V3PAR(cv));
    cv4[0] = cv[0];
    cv4[1] = cv[1];
    cv4[2] = cv[2];
    cv4[3] = 1.0f;
    igl->api->c4f (cv4);
}

void
c3i (int cv[3])
{
    int cv4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(d),cv,IGL_V3PAR(cv));
    cv4[0] = cv[0];
    cv4[1] = cv[1];
    cv4[2] = cv[2];
    cv4[3] = 255;
    igl->api->c4i (cv4);
}


void
c3s (short cv[3])
{
    short cv4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(d),cv,IGL_V3PAR(cv));
    cv4[0] = cv[0];
    cv4[1] = cv[1];
    cv4[2] = cv[2];
    cv4[3] = 255;
    igl->api->c4s (cv4);
}
