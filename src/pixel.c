/*
 * pixel.c
 *
 * 2D pixel handling: fonts and pixel transferring
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

#include <fcntl.h>


/* NOTE: at the moment only STR_B format is supported! */

/* TODO: is IrisGL doing any lighting on pixel transfers? */
/* TODO: may need glDisable(wptr->textureType)? */


/* GL: getcpos - returns the current character position */
void
getcpos (short *ix, short *iy)
{
    igl_windowT *wptr;
    GLfloat rp[4];

    _igl_trace (__func__,TRUE,"%p,%p",ix,iy);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* TODO: IRIX man page says "absolute screen coordinates", AIX man page
     * says "relative to the window". So, add window origin or not?
     */
    glRasterPos3f (wptr->cposx, wptr->cposy, wptr->cposz);
    glGetFloatv (GL_CURRENT_RASTER_POSITION, rp);
    if (ix != NULL)
        *ix = rp[0]+0.5 + wptr->x;
    if (iy != NULL)
        *iy = rp[1]+0.5 + GL_Y(wptr->y + wptr->height - 1);
    _igl_trace (__func__,TRUE,"{%d,%d}",ix?*ix:0,iy?*iy:0);
}


/* GL: cmov, cmov[is], cmov2, cmov2[is] - updates the current character position */
void
cmov (Coord x, Coord y, Coord z)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g",x,y,z);
    igl->api->cmov (x, y, z);
}

void
iglExec_cmov (Coord x, Coord y, Coord z)
{
    IGL_CHECKWNDV ();

    IGL_CTX ()->cposx = x;
    IGL_CTX ()->cposy = y;
    IGL_CTX ()->cposz = z;
}


void
cmovi (Icoord x, Icoord y, Icoord z)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d",x,y,z);
    igl->api->cmov ((Coord)x, (Coord)y, (Coord)z);
}


void
cmovs (Scoord x, Scoord y, Scoord z)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d",x,y,z);
    igl->api->cmov ((Coord)x, (Coord)y, (Coord)z);
}


void
cmov2 (Coord x, Coord y)
{
    _igl_trace (__func__,FALSE,"%g,%g",x,y);
    igl->api->cmov (x, y, 0.0f);
}


void
cmov2i (Icoord x, Icoord y)
{
    _igl_trace (__func__,FALSE,"%d,%d",x,y);
    igl->api->cmov ((Coord)x, (Coord)y, 0.0f);
}


void
cmov2s (Scoord x, Scoord y)
{
    _igl_trace (__func__,FALSE,"%d,%d",x,y);
    igl->api->cmov ((Coord)x, (Coord)y, 0.0f);
}


/* initialize font properties of a window for lcharstr()/charstr() */
void
_igl_fontInit (igl_windowT *wptr, int idx)
{
    igl_layerT *layer = &wptr->layers[idx];

    /* layer context are shared context, sharing the display lists */
    if (idx != IGL_WLAYER_NORMALDRAW) {
        /* layers without a window use the context of the query window */
        if (layer->hrc != IGL_WINDOW (0)->layers[IGL_WLAYER_NORMALDRAW].hrc)
            layer->_fontBases = wptr->layers[IGL_WLAYER_NORMALDRAW]._fontBases;
        else
            layer->_fontBases = IGL_WINDOW (0)->layers[IGL_WLAYER_NORMALDRAW]._fontBases;
        return;
    }

    if (layer->_fontBases == NULL)
        layer->_fontBases = malloc(IGL_MAXFONTS*sizeof(unsigned));
    memset (layer->_fontBases, 0, IGL_MAXFONTS*sizeof(unsigned));

#if PLATFORM_X11
    layer->_fontBases[0] = glGenLists (igl->fontDefs[0].nChars);
    glXUseXFont (igl->fontDefs[0].xfont->fid, igl->fontDefs[0].firstChar,
                    igl->fontDefs[0].nChars, layer->_fontBases[0]);
#endif
}

/* release font property resources */
void
_igl_fontRelease (igl_windowT *wptr, int idx)
{
    igl_layerT *layer = &wptr->layers[idx];
    int j;

    /* Only do this if this is NORMALDRAW, since it destroys the display list */
    if (layer->_fontBases == NULL || idx != IGL_WLAYER_NORMALDRAW)
        return;

    for (j=0; j < IGL_MAXFONTS; j++)
    {
        if (layer->_fontBases[j] != 0)
            glDeleteLists (layer->_fontBases[j], igl->fontDefs[j].nChars);
    }

    free (layer->_fontBases);
    layer->_fontBases = NULL;
}

/* upload a bitmap font to OpenGL */
static void
_igl_loadfont (int n)
{
    igl_windowT *wptr = IGL_CTX ();
    igl_fontT *font;
    int i;

    if (wptr->fontBases[n] != 0 || n == 0)
        return;

    font = &igl->fontDefs[n];
    wptr->fontBases[n] = glGenLists (font->lchars ? 0x100 : font->nChars);

    glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 2);
    if (font->chars != NULL)
    {
        /* defrasterfont. Put each character into an OpenGL list entry */
        Fontchar *fc;
        for (i = 0, fc = font->chars; i < font->nChars; i++, fc++)
        {
            signed char xoff = -fc->xoff, yoff = -fc->yoff;
            if (font->fontDescent < yoff)
                font->fontDescent = yoff;
            glNewList (wptr->fontBases[n] + i, GL_COMPILE);
            glBitmap (fc->w, fc->h, xoff, yoff, fc->width, 0,
                        &font->raster[sizeof(short)*fc->offset]);
            glEndList ();
        }
    }
    else if (font->lchars != NULL)
    {
        /* deflfont. Put each character into an OpenGL list entry */
        Lfontchar *fc;
        for (i = 0, fc = font->lchars; i < font->nChars; i++, fc++)
        {
            short xoff = -fc->xoff, yoff = -fc->yoff;
            /* NOTE: only STR_B supported, hence don't load multibyte chars */
            if (fc->value <= 0xff)
            {
                if (font->fontHeight < fc->h)
                    font->fontHeight = fc->h;
                if (font->fontDescent < yoff)
                    font->fontDescent = yoff;
                glNewList (wptr->fontBases[n] + fc->value, GL_COMPILE);
                glBitmap (fc->w, fc->h, xoff, yoff, fc->xmove, fc->ymove,
                        &font->raster[sizeof(short)*fc->offset]);
                glEndList ();
            }
        }
    }
    else
    {
        /* loadXfont */
#if PLATFORM_X11
        glXUseXFont (igl->fontDefs[n].xfont->fid, igl->fontDefs[n].firstChar, igl->fontDefs[n].nChars, wptr->fontBases[n]);
#endif
    }
}

/* remove a font from OpenGL */
void
_igl_unloadFont (int n)
{
    int i;

    /* throw away old font definition */
    for (i = 0; i < IGL_MAXWINDOWS; i++)
    {
        igl_windowT *wptr = IGL_WINDOW (i);
        if (wptr->wnd != (HWND)0 && wptr->fontBases[n] != 0)
        {
            glDeleteLists (wptr->fontBases[n], igl->fontDefs[n].nChars);
            wptr->fontBases[n] = 0;
        }
    }

    if (igl->fontDefs[n].raster)
        free (igl->fontDefs[n].raster);
    igl->fontDefs[n].raster = NULL;
#if PLATFORM_X11
    if (igl->fontDefs[n].xfont != NULL)
        XFreeFont (IGL_DISPLAY, igl->fontDefs[n].xfont);
    igl->fontDefs[n].xfont = NULL;
#endif
}

/* store an X font description */
void
_igl_defineXfont (int n, char *name)
{
    igl_fontT *font;
    int idx;

    /* NOTE: loadXfont can redefine font 0, while defrasterfont can't */
    idx = IGL_MAPID(igl->fontDefs, IGL_MAXFONTS, n, 1);
    if (idx < 0)
        return;
    font = &igl->fontDefs[idx];

    _igl_unloadFont (idx);
    IGL_CLR_SAVEID(font);
#if PLATFORM_X11
    font->xfont = XLoadQueryFont (IGL_DISPLAY, name);
    if (font->xfont)
    {
        font->firstChar = 32;
        font->nChars = 128 - font->firstChar;
        font->fontHeight = font->xfont->ascent + font->xfont->descent;
        font->fontDescent = font->xfont->descent + 1;
    }
#endif
}

/* store an IrisGL font definition */
static void
_igl_defineFont (short n, short ht, long nc, Fontchar chars[], Lfontchar lchars[], long nr, unsigned short raster[])
{
    igl_fontT *font;
    int idx, i, size;
    int rsz = sizeof(*raster);

    idx = IGL_MAPID(igl->fontDefs, IGL_MAXFONTS, n, 3);
    if (idx < 0)
        return;
    font = &igl->fontDefs[idx];

    if ((chars == NULL && lchars == NULL) || raster == NULL || nc == 0)
        return;

    /* TODO: deflfont can replace chars in a font (defrasterfont cannot). This
     * might require storing bitmaps for each character separately.
     */

    _igl_unloadFont (idx);
    IGL_CLR_SAVEID(font);

    font->chars = chars;
    font->lchars = lchars;
    font->firstChar = 0;
    font->nChars = nc;
    font->fontHeight = ht;
    font->fontDescent = 0;

    /* determine size of raster array */
    for (size = 0, i = 0; i < nc; i++)
    {
        if (chars)
        {
            int w = (chars[i].w+7)/8; /* width in bytes */
            if (size < chars[i].offset*rsz + w*chars[i].h)
                size = chars[i].offset*rsz + w*chars[i].h;
        }
        if (lchars)
        {
            int w = (lchars[i].w+7)/8; /* width in bytes */
            if (size < lchars[i].offset*rsz + w*lchars[i].h)
                size = lchars[i].offset*rsz + w*lchars[i].h;
        }
    }

    /* allocate internal raster array and convert raster for OpenGL */
    size = (size + rsz-1) & ~(rsz-1); /* align to size of raster type */
    font->raster = malloc (size);
    for (i = 0; i < size/rsz; i++)
    {
        font->raster[2*i + 0] = raster[i] >> 8;
        font->raster[2*i + 1] = raster[i];
    }
}


/* GL: strwidth, lstrwidth - returns the width of the specified text string */
long
strwidth (String str)
{
    return (lstrwidth(STR_B, str));
}


long
lstrwidth (long type, String str)
{
    igl_fontT *font;
    int len = strlen(str);
    long width = 0;
    char *p;

    _igl_trace (__func__,TRUE,"%ld,%p(%s)",type,str,str?str:"");
    IGL_CHECKWND (0);

    /* only ASCII supported for now! */
    if (type != STR_B || len == 0)
        return (0);

    font = &igl->fontDefs[IGL_CTX ()->currentFont];
    /* if this is the system font, get width from system */
#if PLATFORM_X11
    if (font->raster == NULL)
        return XTextWidth (font->xfont, str, len);
#endif

    /* sum up the width of all characters in the string */
    for (p = str; *p; p++)
    {
        if (*p >= font->firstChar && *p < font->firstChar + font->nChars)
            width += font->chars[*p - font->firstChar].width;
    }

    return width;
}


/* GL: getheight - returns the maximum character height in the current raster font */
long
getheight (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    return (igl->fontDefs[IGL_CTX ()->currentFont].fontHeight);
}


/* GL: getdescender - returns the character characteristics */
long
getdescender (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    return (igl->fontDefs[IGL_CTX ()->currentFont].fontDescent);
}


/* GL: charstr, lcharstr - draws a string of characters */
void
charstr (String str)
{
    _igl_trace (__func__,FALSE,"%p(%s)",str,str?str:"");
    igl->api->lcharstr (STR_B, str);
}

void
lcharstr (long type, void *str)
{
    _igl_trace (__func__,FALSE,"%ld,%p(%s)",type,str,str?str:"");
    igl->api->lcharstr (type, str);
}

void
iglExec_lcharstr (long type, void *str)
{
    igl_windowT *wptr;
    igl_fontT *font;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* only ASCII supported for now! */
    if (type != STR_B)
        return;

    font = &igl->fontDefs[wptr->currentFont];

    IGL_DISLIGHT ();
    glRasterPos3f (wptr->cposx, wptr->cposy, wptr->cposz);
    glListBase (wptr->fontBases[wptr->currentFont] - font->firstChar);
    glCallLists (strlen(str), GL_UNSIGNED_BYTE, str);

    wptr->cposx += lstrwidth (type, str);
}


/* GL: font - selects a raster font for drawing text strings */
void
font (short fntnum)
{
    _igl_trace (__func__,FALSE,"%d",fntnum);
    igl->api->font (fntnum);
}

void
iglExec_font (short fntnum)
{
    int idx;

    IGL_CHECKWNDV ();

    idx = IGL_MAPID(igl->fontDefs, IGL_MAXFONTS, fntnum, 0);
    if (idx < 0)
        return;

    if (igl->fontDefs[idx].nChars != 0)
    {
        _igl_loadfont (idx);
        IGL_CTX ()->currentFont = idx;
    }
}


/* GL: getfont - returns the current raster font number */
long
getfont (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    return (igl->fontDefs[IGL_CTX ()->currentFont].id);
}


/* GL: defrasterfont, deflfont - defines a raster font */
void
defrasterfont (short n, short ht, short nc, Fontchar chars[], short nr, unsigned short raster[])
{
    _igl_defineFont (n, ht, nc, chars, NULL, nr, raster);
}
 
void
deflfont (short n, long nc, Lfontchar chars[], long nr, unsigned short raster[])
{
    _igl_defineFont (n, 0, nc, NULL, chars, nr, raster);
}


/* AIX GL extensions for X fonts */
void
loadXfont (int id_num, char *name)
{
    _igl_trace (__func__,TRUE,"%d,%p(%s)",id_num,name,name?name:"");
    IGL_CHECKWNDV ();

    _igl_defineXfont (id_num, name);
}

int
getfonttype (void)
{
    return 0;   /* FT_SBCS? */
}

void
getfontencoding (char *enc)
{
    int idx = IGL_CTX ()->currentFont;
    unsigned long ret;

    _igl_trace (__func__,TRUE,"%p",enc);
    IGL_CHECKWNDV ();

    *enc = '\0'; 
#if PLATFORM_X11
    if (idx >= 0 && igl->fontDefs[idx].xfont != NULL &&
                    XGetFontProperty (igl->fontDefs[idx].xfont, XA_FONT, &ret))
    {
        char *name = XGetAtomName (IGL_DISPLAY, (Atom)ret);
        char *p = name;
        int minus;

        /* -fndry-fmly-wght-slant-swid-style-pxl-pt-rx-ry-spc-awid-rgst-enc */
        /* need rgst-enc -> skip 13*'-' */
        for (minus = 0; minus < 13 && name && *p != '\0'; p++)
            minus += (*p == '-');
        if (minus >= 13) 
            strcpy (enc, p);
    }
#endif
}


#if 0
____________________________ pixel functions ____________________________
#endif

#define IGL_PIXBUFSZ     (8*2048)       /* 2048 for IRIS-4D says manpage */

/* NOTE: in IrisGL the (l)rect functions use screen coordinates relative to the
 * window position. This is not easily done in OpenGL, where the only way before
 * version 1.4 was to use glRasterPos, which is in world coordinates.
 * An orthographic projection seems to be the easiest way to go for.
 */

/* set raster position in window coordinates */
static void
_igl_setRasterpos (float x, float y)
{
    _igl_pushTransform ();
    _igl_ortho2D (0);

    /* NOTE: Setting the raster position outside of the window is not directly
     * possible in OpenGL. Moving the raster position with glBitmap is a known
     * way to circumvent this.
     */
    glRasterPos2i (0, 0);
    glBitmap (0, 0, 0, 0, x, y, NULL);

    _igl_popTransform ();
}


/* GL: writeRGB, writepixels - paints a row of pixels on the screen */
void
writeRGB(short n, RGBvalue r[], RGBvalue g[], RGBvalue b[])
{
    long buffer[IGL_PIXBUFSZ], i;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%d,%p,%p,%p",n,r,g,b);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if (wptr->flags & IGL_WFLAGS_RGBA)
    {
        IGL_DISLIGHT ();
        glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 0);
        /* writes the next n RGB pixels of the current row */
        while (n > 0)
        {
            for (i = 0; i < IGL_PIXBUFSZ && n; i++, n--)
            {
                char *p = (char *)&buffer[i];
                p[0] = *r++; p[1] = *g++;
                p[2] = *b++; p[3] = 255;
            }
            _igl_setRasterpos (wptr->cposx, wptr->cposy);
            glDrawPixels (i, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer); 
            wptr->cposx += i;
        }
    }
}


void 
writepixels(short n, Colorindex colors[])
{
    long buffer[IGL_PIXBUFSZ], i;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%d,%p",n,colors);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if (!(wptr->flags & IGL_WFLAGS_RGBA))
    {
        IGL_DISLIGHT ();
        glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 0);
        /* writes the next n cmap pixels of the current row */
        while (n > 0)
        {
            for (i = 0; i < IGL_PIXBUFSZ && n; i++, n--)
            {
                long c = IGL_CMAP2ABGR (wptr, *colors++);
                char *p = (char *)&buffer[i];
                p[0] = GetRValue(c); p[1] = GetGValue(c);
                p[2] = GetBValue(c); p[3] = GetAValue(c);
            }
            _igl_setRasterpos (wptr->cposx, wptr->cposy);
            glDrawPixels (i, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer); 
            wptr->cposx += i;
        }
    }
}


/* GL: readRGB - gets values of specific pixels */
long
readRGB(short n, RGBvalue r[], RGBvalue g[], RGBvalue b[])
{
    long buffer[IGL_PIXBUFSZ], i, l, m = 0;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%d,%p,%p,%p",n,r,g,b);
    IGL_CHECKWND (0);
    wptr = IGL_CTX ();

    /* TODO: readsource(SRC_ZBUFFER), readcomponent() support */
    if (wptr->flags & IGL_WFLAGS_RGBA)
    {
        glPixelStorei (GL_PACK_ROW_LENGTH, 0);
        glPixelStorei (GL_PACK_ALIGNMENT, 0);
        /* reads the next n RGB pixels of the current row */
        while (n > 0)
        {
            l = (n < IGL_PIXBUFSZ ? n : IGL_PIXBUFSZ);
            glReadPixels (wptr->cposx, wptr->cposy, l, 1,
                            GL_RGBA, GL_UNSIGNED_BYTE, buffer); 
            for (i = 0; i < l; i++, n--)
            {
                char *p = (char *)&buffer[i];
                *r++ = p[0];
                *g++ = p[1];
                *b++ = p[2];
            }
            wptr->cposx += l;
            m += l;
        }
    }

    return m;
}


/* GL: rectwrite, lrectwrite - draws a rectangular array of pixels into the frame buffer*/
void
rectwrite(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, Colorindex parray[])
{
    long buffer[IGL_PIXBUFSZ], i, l;
    int w = x2-x1+1, h = y2-y1+1;
    Screencoord x, y;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%d,%d,%d,%d,%p",x1,y1,x2,y2,parray);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if (!(wptr->flags & IGL_WFLAGS_RGBA))
    {
        IGL_DISLIGHT ();
        glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 0);
        /* writes a rectangular area of cmap pixels */
        y = y1;
        while (h > 0)
        {
            /* process a block of lines fitting in the buffer */
            int blines = IGL_PIXBUFSZ / w;
            if (blines > h) blines = h;

            x = x1;
            l = (blines + !blines) * w; /* process at least one line */
            while (l > 0)
            {
                /* transform color indices to RGB pixels */
                for (i = 0; i < IGL_PIXBUFSZ && l; i++, l--)
                {
                    long c = IGL_CMAP2ABGR (wptr, *parray++);
                    char *p = (char *)&buffer[i];
                    p[0] = GetRValue(c); p[1] = GetGValue(c);
                    p[2] = GetBValue(c); p[3] = GetAValue(c);
                }
                /* output line block or part of the line */
                _igl_setRasterpos (x, y);
                if (blines > 0)
                    glDrawPixels (w, blines, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
                else
                {
                    glDrawPixels (i, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer); 
                    x += i * wptr->xzoom;
                }
            }
            /* line housekeeping */
            y += (blines + !blines) * wptr->yzoom;
            h -= blines + !blines;
        }
    }
}


void
lrectwrite(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, unsigned long parray[])
{
    int oglFormat, oglType, oglSize;
    int w = x2-x1+1, h = y2-y1+1;
    unsigned long *image;

    _igl_trace (__func__,TRUE,"%d,%d,%d,%d,%p",x1,y1,x2,y2,parray);
    IGL_CHECKWNDV ();

    /* writes a rectangular area of RGB/cmap pixels */
    oglSize = _igl_pixInMode (&oglFormat, &oglType);

    image = _igl_pixInConvert (&oglFormat, &oglType, oglSize, parray, w, h);
    IGL_DISLIGHT ();
    _igl_setRasterpos (x1, y1);
    glDrawPixels (w, h, oglFormat, oglType, image); 

    _igl_pixInEnd (oglFormat, oglType, parray, image);
}


/* GL:  lrectread - reads a rectangular array of pixels into CPU memory */
long
lrectread(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, unsigned long parray[])
{
    int oglFormat, oglType;
    int w = x2-x1+1, h = y2-y1+1;
    unsigned long *image;

    _igl_trace (__func__,TRUE,"%d,%d,%d,%d,%p",x1,y1,x2,y2,parray);
    IGL_CHECKWND (0);

    /* TODO: (l)rectread coordinates can cover areas outside of the current
     * window. glReadPixels cannot do this. In that case, use XGetImage ()?
     */

    /* reads a rectangular area of RGB/cmap pixels */
    image = _igl_pixOutMode (&oglFormat, &oglType, parray, w, h);
    glReadPixels (x1, y1, w, h, oglFormat, oglType, image);

    _igl_pixOutConvert (oglFormat, oglType, parray, image, w, h);
    _igl_pixOutEnd (oglFormat, oglType, parray, image);
    return (x2-x1+1)*(y2-y1+1);
}


/* GL: rectcopy - copies a rectangle of pixels with an optional zoom */
void
rectcopy(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, Screencoord newx, Screencoord newy)
{
    int w = x2-x1+1, h = y2-y1+1;
    unsigned long *buffer, i;
    igl_windowT *wptr;
    GLenum iformat, itype, ishift, oformat, otype, oshift;
#ifdef GL_EXT_abgr
    GLenum formats[] = { GL_DEPTH_COMPONENT, GL_ABGR_EXT };
#else
    GLenum formats[] = { GL_DEPTH_COMPONENT, GL_RGBA };
#endif
    GLenum types[] = { GL_UNSIGNED_INT, GL_UNSIGNED_BYTE };

    _igl_trace (__func__,TRUE,"%d,%d,%d,%d,%d,%d",x1,y1,x2,y2,newx,newy);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* TODO: rectcopy coordinates can cover areas outside of the current window.
     * glReadPixels/glCopyPixels cannot do this. In that case, use XGetImage ()?
     */

    /* copy using OpenGL if possible */
    if (!(wptr->state & (IGL_WSTATE_RDSOURCEZ|IGL_WSTATE_ZDRAW)) && !wptr->pixFlags)
    {
        IGL_DISLIGHT ();
        _igl_setRasterpos (newx, newy);
        glCopyPixels (x1, y1, w, h, GL_COLOR);
        return;
    }

    /* determine source and destination buffer - either Z or color buffer */
    iformat = formats[!(wptr->state & IGL_WSTATE_RDSOURCEZ)];
    itype = types[!(wptr->state & IGL_WSTATE_RDSOURCEZ)];
    ishift = (wptr->state & IGL_WSTATE_RDSOURCEZ) ? 32-IGL_ZBITS : 0;
    glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 0);
    oformat = formats[!(wptr->state & IGL_WSTATE_ZDRAW)];
    otype = types[!(wptr->state & IGL_WSTATE_ZDRAW)];
    oshift = (wptr->state & IGL_WSTATE_ZDRAW) ? 32-IGL_ZBITS : 0;
    glPixelStorei (GL_PACK_ROW_LENGTH, 0);
    glPixelStorei (GL_PACK_ALIGNMENT, 0);

    /* allocate and read image */
    IGL_DISLIGHT ();
    buffer = malloc (w*h * sizeof(*buffer));
    glReadPixels (x1, y1, w, h, iformat, itype, buffer);
    /* apply pixel pipe */
    for (i = 0; i < w*h; i++)
    {
        unsigned long pixel = buffer[i] >> ishift;
#ifndef GL_EXT_abgr
        if (!(wptr->state & IGL_WSTATE_RDSOURCEZ))
            pixel = _igl_swap32 (pixel);
#endif
        pixel = _igl_pixPipe (pixel);
#ifndef GL_EXT_abgr
        if (!(wptr->state & IGL_WSTATE_ZDRAW))
            pixel = _igl_swap32 (pixel);
#endif
        buffer[i] = pixel << oshift;
    }
    /* output image and destroy buffer */
    _igl_setRasterpos (newx, newy);
    glDrawPixels (w, h, oformat, otype, buffer);
    free (buffer);
}


/* GL: readdisplay - reads a rectangular screen region in a packed RGB format */
long
readdisplay(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, unsigned long parray[], unsigned long hints)
{
    int w = x2-x1+1, h = y2-y1+1;
    int xoff, xlen, yoff, ylen;
    unsigned i, j;
    int x = x1, y = GL_Y(y2);
#if PLATFORM_X11
    XImage *ximg = NULL;
    unsigned long pix, rm, gm, bm, rs, gs, bs;
#endif

    _igl_trace (__func__,TRUE,"%d,%d,%d,%d,%p,%lx",x1,y1,x2,y2,parray,hints);

    /* see if coordinates are swapped */
    if (w <= 0)
        w = x1-x2+1, x = x2;
    if (h <= 0)
        h = y1-y2+1, y = GL_Y(y1);

    /* clip coordinates to screen */
    xoff = 0, xlen = w;
    if (x < 0)
        xoff = -x, xlen -= -x;
    if (x+w > DisplayWidth(IGL_DISPLAY, IGL_SCREEN)-1)
        xlen -= x+w - (DisplayWidth(IGL_DISPLAY, IGL_SCREEN)-1);
    if (xlen < 0)
        xlen = 0;
    yoff = 0, ylen = h;
    if (y < 0)
        yoff = -y, ylen -= -y;
    if (y+h > DisplayHeight(IGL_DISPLAY, IGL_SCREEN)-1)
        ylen -= y+h - (DisplayHeight(IGL_DISPLAY, IGL_SCREEN)-1);
    if (ylen < 0)
        ylen = 0;

    /* clear clipped areas - ouff, rather do memset32 (parray, 0, x*y)? */
    if (yoff == 0)
        for (i = 0; i < h-ylen; i++)
            for (j = 0; j < w; j++)
                parray[i*w + j] = RGB (0, 0, 0);
    else
        for (i = ylen; i < h; i++)
            for (j = 0; j < w; j++)
                parray[i*w + j] = RGB (0, 0, 0);
    if (xoff == 0)
        for (j = xlen; j < w; j++)
            for (i = h-yoff - ylen; i < h-yoff; i++)
                parray[i*w + j] = RGB (0, 0, 0);
    else
        for (j = 0; j < w-xlen; j++)
            for (i = h-yoff - ylen; i < h-yoff; i++)
                parray[i*w + j] = RGB (0, 0, 0);

#if PLATFORM_X11
    /* get the image data */
    if (xlen != 0 && ylen != 0)
        ximg = XGetImage (IGL_DISPLAY, IGL_ROOT, x+xoff, y+yoff, xlen, ylen,
                            AllPlanes, ZPixmap);

    /* need the image r,g,b bits in the upper bits of the GL RGB bytes.
     * first, make sure value < 0x100, then make sure the top bit is bit 7
     */
    for (i = rm = ximg->red_mask  , rs = 0; i > 0xff; i >>= 1, rs ++) ;
    for (i = rm >> rs; i < 0x7f; i <<= 1, rs --) ;
    for (i = gm = ximg->green_mask, gs = 0; i > 0xff; i >>= 1, gs ++) ;
    for (i = gm >> gs; i < 0x7f; i <<= 1, gs --) ;
    for (i = bm = ximg->blue_mask , bs = 0; i > 0xff; i >>= 1, bs ++) ;
    for (i = bm >> bs; i < 0x7f; i <<= 1, bs --) ;

    /* copy pixel data to destination, with reassembly of the RGB components */
    for (i = 0; i < ylen; i++)
        for (j = 0; j < xlen; j++)
        {
            pix = XGetPixel (ximg, j, ylen-i-1);
            parray[(i+ h-ylen-yoff)*w + (j+xoff)] =
                    RGB ((pix & rm) >> rs, (pix & gm) >> gs, (pix & bm) >> bs);
        }

    XDestroyImage (ximg);
#endif
    return w*h;
}


/* GL: unimplemented stuff */
long 
readpixels(short n, Colorindex colors[])
{
    _igl_trace (__func__,TRUE,"%d,%p",n,colors);

    /* cannot be emulated correctly since colormap mode is only simulated */
    /* TODO: readsource(SRC_ZBUFFER) and readcomponent() on RGB windows */
    return 0;
}


long
rectread(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, Colorindex parray[])
{
    _igl_trace (__func__,TRUE,"%d,%d,%d,%d,%p",x1,y1,x2,y2,parray);

    /* cannot be emulated correctly since colormap mode is only simulated */
    /* TODO: readsource(SRC_ZBUFFER) and readcomponent() on RGB windows */
    return 0;
}
