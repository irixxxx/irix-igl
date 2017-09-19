/*
 * pixmode.c
 *
 * 2D pixel pipeline
 *
 *
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


/* IrisGL pixel pipeline: unpack->shift->expand->add24->zoom->pack
 * copy+read read from fb instead of unpack,
 * copy+write write to fb instead of pack.
 * for read, there is no zoom. Parameters PM_* are set with pixmode().
 *
 * unpack: PM_SIZE, PM_STRIDE, PM_OFFSET, PM_TTOB, PM_RTOL, PM_INPUT_FORMAT/TYPE
 * shift (shifts complete pixel): PM_SHIFT
 * expand (expands LSB to C0 or C1): PM_EXPAND, PM_C0, PM_C1
 * add24 (adds to lower 24 bits of pixel): PM_ADD24
 * pack: PM_SIZE, PM_STRIDE, PM_OFFSET, PM_TTOB, PM_RTOL, PM_OUTPUT_FORMAT/TYPE
 *
 * the internal format is either ABGR (each 8 bits) for a total of 32 bits in
 * RGB mode, or the color index (12 lower bits) in CMAP mode. PM_*_FORMAT/TYPE
 * determine which of the component fields are read or written how.
 * In ZDATA mode, the internal format is z-values (lower 24 bits), and writes/
 * reads go to the Z buffer.
 *
 * Reality Engine has a 64 bit mode where ABGR components are 16 bit each.
 */

/* descriptor for pixel stream to/from memory */
typedef struct igl_pixdataT {
    long format;        /* pixel format */
    long type;          /* component type */
    long size;          /* pixel size */

    uint32_t *img;      /* pixel storage */
    int bit;            /* next bit position to be processed */
    unsigned long long data; /* pixel stream register */

    union {             /* pixel format/type conversion register */
        float f;        /*  ... for type PM_FLOAT */
        unsigned long l;/*  ... everything else */
    } pixel;
} igl_pixdataT;


/* map IrisGL pixel format to OpenGL */
static int
_igl_pixOglFormat(int format, int *num)
{
    switch (format)
    {
        default:
        case PM_ABGR:               *num = 4; return GL_ABGR_EXT;
        case PM_BGR:                *num = 3; return GL_BGR;
        case PM_RGBA:               *num = 4; return GL_RGBA;
        case PM_RGB:                *num = 3; return GL_RGB;
        case PM_LUMINANCE:          *num = 1; return GL_LUMINANCE;
        case PM_LUMINANCEA:         *num = 2; return GL_LUMINANCE_ALPHA;
        case PM_ALPHA:              *num = 1; return GL_ALPHA;
        case PM_BLUE:               *num = 1; return GL_BLUE;
        case PM_GREEN:              *num = 1; return GL_GREEN;
        case PM_RED:                *num = 1; return GL_RED;
        case PM_DEPTH:              *num = 1; return GL_DEPTH_COMPONENT;
    }
}

/* map IrisGL pixel type to OpenGL */
static int
_igl_pixOglType(int type, int *size)
{
    switch (type)
    {
        case PM_BITMAP:             *size =  1; return GL_BITMAP;
        case PM_BYTE:               *size =  8; return GL_BYTE;
	default:
        case PM_UNSIGNED_BYTE:      *size =  8; return GL_UNSIGNED_BYTE;
        case PM_SHORT:              *size = 16; return GL_SHORT;
        case PM_UNSIGNED_SHORT:     *size = 16; return GL_UNSIGNED_SHORT;
        case PM_INT:                *size = 32; return GL_INT;
        case PM_UNSIGNED_INT:       *size = 32; return GL_UNSIGNED_INT;
        case PM_FLOAT:              *size = 32; return GL_FLOAT;
        case PM_SHORT_12:           *size = 16; return GL_SHORT;
        case PM_UNSIGNED_SHORT_12:  *size = 16; return GL_UNSIGNED_SHORT;
    }
}


/* TODO: the pixmode conversion stuff is still in a somewhat experimental state.
 * This means not everything is tested, and it's unoptimized and rather slow.
 * (maybe more pixmodes could be mapped to OpenGL, see comments in pixmode)
 */

/* TODO: the 64 bit pixel size is not supported. The current code only supports
 * PM_SIZE upto 32, and the internal pixel size is 32 bit.
 */


#ifndef GL_EXT_abgr
/* swap bytes (for ABGR->RGBA and vice versa) */
static inline long
_igl_swap32 (long val)
{
    unsigned char b0 = val >> 24, b1 = val >> 16, b2 = val >> 8, b3 = val >> 0;
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}
#endif

/* get one component (applies PM_INPUT_TYPE) */
static unsigned long
_igl_pixInC (igl_pixdataT *pp, int size)
{
    int sz = 0, sg;
    unsigned long ret;

    /* TODO: evaluating this switch for every pixel isn't exactly smart */
    switch (pp->type)
    {
        case PM_BITMAP:             sz =  1, sg = 0; break;
        case PM_BYTE:               sz =  8, sg = 1; break;
	default:
        case PM_UNSIGNED_BYTE:      sz =  8, sg = 0; break;
        case PM_SHORT:              sz = 16, sg = 1; break;
        case PM_UNSIGNED_SHORT:     sz = 16, sg = 0; break;
        case PM_INT:                sz = 32, sg = 1; break;
        case PM_UNSIGNED_INT:       sz = 32, sg = 0; break;
        case PM_SHORT_12:           sz = 12, sg = 1; break;
        case PM_UNSIGNED_SHORT_12:  sz = 12, sg = 0; break;

        case PM_FLOAT:
            /* floating point input; treat all 32 bits as a float number */
            ret = pp->pixel.f * ((1<<size)-1);
            pp->pixel.l = 0; /* consumes all 32 bits */
            return ret & ((1<<size)-1);
    }

    if (sz >= size)
    {
        /* target has more bits; put component in high bits of target */
        ret = pp->pixel.l >> (32-size);
        ret ^= (sg ? 1<<(size-1) : 0); /* remove sign */
        ret &= (1<<(sz-size))-1; /* clear unused low bits */
        pp->pixel.l <<= sz;
        return ret & ((1<<size)-1);
    }
    else
    {
        /* TODO: right or left aligned? */
        ret = (pp->pixel.l >> (32-sz));
        pp->pixel.l <<= sz;
        return ret & ((1<<sz)-1);
    }
}

/* get and convert all compenents of a pixel (applies PM_INPUT_FORMAT) */
static unsigned long
_igl_pixInFormat (igl_pixdataT *pp, unsigned long pixel)
{
    unsigned long ret;

    /* TODO: evaluating this switch for every pixel isn't really smart */
    pp->pixel.l = pixel;
    switch (pp->format)
    {
	default:
        case PM_ABGR:
            ret = (_igl_pixInC (pp, 8)<<24)| (_igl_pixInC (pp, 8)<<16) |
                  (_igl_pixInC (pp, 8)<< 8)| (_igl_pixInC (pp, 8)<< 0);
            break;
        case PM_BGR:
            ret = (_igl_pixInC (pp, 8)<<16)| (_igl_pixInC (pp, 8)<< 8) |
                  (_igl_pixInC (pp, 8)<< 0);
            break;
        case PM_RGBA:
            ret = (_igl_pixInC (pp, 8)<< 0)| (_igl_pixInC (pp, 8)<< 8) |
                  (_igl_pixInC (pp, 8)<<16)| (_igl_pixInC (pp, 8)<<24);
            break;
        case PM_RGB:
            ret = (_igl_pixInC (pp, 8)<< 0)| (_igl_pixInC (pp, 8)<< 8) |
                  (_igl_pixInC (pp, 8)<<16);
            break;
        case PM_LUMINANCEA:
            ret = (_igl_pixInC (pp, 8) * 0x00010101) |
                  (_igl_pixInC (pp, 8) * 0x01000000);
            break;
        case PM_LUMINANCE:
            ret = _igl_pixInC (pp, 8) * 0x00010101;
            break;
        case PM_ALPHA:
            ret = _igl_pixInC (pp, 8) * 0x01000000;
            break;
        case PM_BLUE:
            ret = _igl_pixInC (pp, 8) * 0x00010000;
            break;
        case PM_GREEN:
            ret = _igl_pixInC (pp, 8) * 0x00000100;
            break;
        case PM_RED:
            ret = _igl_pixInC (pp, 8) * 0x00000001;
            break;
        case PM_DEPTH:
            ret = _igl_pixInC (pp, 24);
            break;
    }
    return ret;
}

/* get pixel from incoming data stream (applies PM_SIZE) */
static inline unsigned long
_igl_pixUnpack (igl_pixdataT *pp)
{
    unsigned long pixel;
    if (pp->size == 32 && pp->bit == 32)
    {
        /* shortcut for aligned 32 bit values */
        pixel = pp->data;
        pp->data = *pp->img++;
    }
    else
    {
        /* get more data if there aren't enough bits available */
        if (pp->bit < pp->size)
        {
            pp->data = (pp->data << 32) | *pp->img++;
            pp->bit += 32;
        }
        /* extract bitfield from data */
        pixel = (pp->data >> (pp->bit-pp->size)) & ((1<<pp->size)-1);
        pp->bit -= pp->size;
    }
    return pixel;
}

/* put one component (applies PM_OUTPUT_TYPE) */
static void
_igl_pixOutC (igl_pixdataT *pp, int size, unsigned long comp)
{
    int sz = 0, sg;

    /* mask of any unused pixel bits */
    comp &= ((1<<size)-1);

    /* TODO: evaluating this switch for every pixel isn't exactly smart */
    switch (pp->type)
    {
        case PM_BITMAP:             sz =  1, sg = 0; break;
        case PM_BYTE:               sz =  8, sg = 1; break;
	default:
        case PM_UNSIGNED_BYTE:      sz =  8, sg = 0; break;
        case PM_SHORT:              sz = 16, sg = 1; break;
        case PM_UNSIGNED_SHORT:     sz = 16, sg = 0; break;
        case PM_INT:                sz = 32, sg = 1; break;
        case PM_UNSIGNED_INT:       sz = 32, sg = 0; break;
        case PM_SHORT_12:           sz = 12, sg = 1; break;
        case PM_UNSIGNED_SHORT_12:  sz = 12, sg = 0; break;

        case PM_FLOAT:
            /* TODO: saturation? */
            pp->pixel.f = (float)comp / ((1<<size)-1); return;
    }

    pp->pixel.l <<= sz;
    if (sz >= size)
    {
        /* component has more bits then output type */
        comp ^= (sg ? 1<<(size-1) : 0); /* apply sign */
        pp->pixel.l |= comp << (sz-size);
    }
    else
    {
        /* TODO: right or left aligned? */
        pp->pixel.l |= comp;
    }
}

/* convert pixel and put all components (applies PM_OUTPUT_FORMAT) */
static unsigned long
_igl_pixOutFormat (igl_pixdataT *pp, unsigned long pixel)
{
    unsigned long lum;

    /* TODO: evaluating this switch for every pixel isn't really smart */
    pp->pixel.l = 0;
    switch (pp->format)
    {
	default:
        case PM_ABGR:
            _igl_pixOutC (pp, 8, pixel >> 24);_igl_pixOutC (pp, 8, pixel >> 16);
            _igl_pixOutC (pp, 8, pixel >>  8);_igl_pixOutC (pp, 8, pixel >>  0);
            break;
        case PM_BGR:
            _igl_pixOutC (pp, 8, pixel >> 16);_igl_pixOutC (pp, 8, pixel >>  8);
            _igl_pixOutC (pp, 8, pixel >>  0);
            break;
        case PM_RGBA:
            _igl_pixOutC (pp, 8, pixel >>  0);_igl_pixOutC (pp, 8, pixel >>  8);
            _igl_pixOutC (pp, 8, pixel >> 16);_igl_pixOutC (pp, 8, pixel >> 24);
            break;
        case PM_RGB:
            _igl_pixOutC (pp, 8, pixel >>  0);_igl_pixOutC (pp, 8, pixel >>  8);
            _igl_pixOutC (pp, 8, pixel >> 16);
            break;
        case PM_LUMINANCE:
            lum = 5*GetRValue(pixel) + 10*GetGValue(pixel) + GetBValue(pixel);
            _igl_pixOutC (pp, 8, lum >> 4);
            break;
        case PM_LUMINANCEA:
            lum = 5*GetRValue(pixel) + 10*GetGValue(pixel) + GetBValue(pixel);
            _igl_pixOutC (pp, 8, lum >> 4);
            _igl_pixOutC (pp, 8, pixel >> 24);
            break;
        case PM_ALPHA:
            _igl_pixOutC (pp, 8, pixel >> 24);
            break;
        case PM_BLUE:
            _igl_pixOutC (pp, 8, pixel >> 16);
            break;
        case PM_GREEN:
            _igl_pixOutC (pp, 8, pixel >> 8);
            break;
        case PM_RED:
            _igl_pixOutC (pp, 8, pixel >> 0);
            break;
        case PM_DEPTH:
            _igl_pixOutC (pp, 24, pixel);
            break;
    }
    return pp->pixel.l;
}

/* write pixel to outgoing data stream (applies PM_SIZE) */
static inline void
_igl_pixPack (igl_pixdataT *pp, unsigned long pixel)
{
    if (pp->size == 32 && pp->bit == 64)
    {
        /* shortcut for aligned 32 bit data */
        *pp->img++ = pixel;
    }
    else
    {
        /* if there isn't enough space in data, store some */
        if (pp->bit < pp->size )
        {
            *pp->img++ = pp->data >> 32;
            pp->data = (pp->data << 32);
            pp->bit += 32;
        }
        /* stash bitfield in data */
        pixel &= ((1<<pp->size)-1);
        pp->data |= pixel << (pp->bit - pp->size);
        pp->bit -= pp->size;
    }
}

/* pixel pipe (applies PM_SHIFT, PM_ADD24, PM_EXPAND/C0/C1) */
unsigned long
_igl_pixPipe (unsigned long pixel)
{
    igl_windowT *wptr = IGL_CTX ();

    if (wptr->pixShift < 0)
        pixel >>= -wptr->pixShift;
    else
        pixel <<=  wptr->pixShift;
    if (wptr->pixExpand)
        pixel = wptr->pixCx[pixel & 1];
    if (wptr->pixAdd24)
        pixel = ((pixel + wptr->pixAdd24) & 0x00ffffff) | (pixel & 0xff000000);
    return pixel;
}

/* map IrisGL output mode to OpenGL */
int
_igl_pixInMode(int *format, int *type)
{
    igl_windowT *wptr = IGL_CTX ();
    int cnm, csz;

    *format = _igl_pixOglFormat (wptr->pixInFormat, &cnm);
    *type = _igl_pixOglType (wptr->pixInType, &csz);

    return cnm*csz; /* OpenGL pixel size in bits */
}

/* convert IrisGL output data to OpenGL (applies PM_OFFSET, PM_STRIDE, PM_TTOB, PM_RTOL) */
void *
_igl_pixInConvert(int *format, int *type, int size, void *image, int width, int height)
{
    uint32_t *imgIn = image, *imgOgl = image;
    igl_windowT *wptr = IGL_CTX ();
    int pxsz = wptr->pixSize ? wptr->pixSize : 32;

    /* see if the transfer can be handled by OpenGL pixeltransfer stuff */
    if (size > 0 && (size != pxsz || wptr->pixFlags ||
        (wptr->state & IGL_WSTATE_ZDRAW) || !(wptr->flags & IGL_WFLAGS_RGBA)))
    {
        igl_pixdataT pd;
        uint32_t *iptr;
        int x, y;

        /* no, use software transfer engine to produce GL_RGBA/DEPTH output */
        pd.format = wptr->pixInFormat;
        pd.type = wptr->pixInType;
        pd.size = wptr->pixSize;

        iptr = imgOgl = malloc(width*height * sizeof(*imgOgl));

        /* for TTOB start with top line, for RTOL with last pixel in line */
        if (wptr->pixTtoB)
            iptr += width * (height-1);
        if (wptr->pixRtoL)
            iptr += width - 1;

        for (y = 0; y < height; y++)
        {
            /* prepare pixel stream for this line */
            pd.img = imgIn;
            pd.data = *pd.img++;
            pd.bit = 32-wptr->pixOffset;

            for (x = 0; x < width; x++)
            {
                /* get pixel from input stream */
                unsigned long pixel = _igl_pixUnpack (&pd);

                if (pd.format != PM_ABGR || pd.type != PM_UNSIGNED_BYTE)
                    pixel = _igl_pixInFormat (&pd, pixel);

                /* process, and write OpenGL output */
                pixel = _igl_pixPipe (pixel);

                if (wptr->pixZ || (wptr->state & IGL_WSTATE_ZDRAW))
                    /* OpenGL uses top bits, but IrisGL uses the bottom bits */
                    *iptr = pixel << (32-IGL_ZBITS);
#ifdef GL_EXT_abgr
                else if (wptr->flags & IGL_WFLAGS_RGBA)
                    *iptr = pixel;
                else
                    *iptr = IGL_CMAP2ABGR (wptr, pixel);
#else
                else if (wptr->flags & IGL_WFLAGS_RGBA)
                    *iptr = _igl_swap32 (pixel);
                else
                    *iptr = _igl_swap32 (IGL_CMAP2ABGR (wptr, pixel));
#endif
                /* advance to next output pixel */
                if (wptr->pixRtoL)
                    iptr --;
                else
                    iptr ++;
            }
            /* advance to next stream scanline */
            if (wptr->pixStride)
                imgIn += wptr->pixStride;
            else /* advance to next word boundary, but consider cached data */
                imgIn = pd.img - (pd.bit >= 32);
            /* advance to next output line */
            if (wptr->pixTtoB)
                iptr -= 2*width;
            if (wptr->pixRtoL)
                iptr += 2*width;
        }
        if (wptr->pixZ || (wptr->state & IGL_WSTATE_ZDRAW))
        {
            *format = GL_DEPTH_COMPONENT;
            *type = GL_UNSIGNED_INT;
#ifdef __sgi
            /* TODO: bug in IRIX 6.5.8, remote connection to ubuntu 14? */
            glPixelStorei (GL_UNPACK_SWAP_BYTES, 1);
#endif
            /* OpenGL only allows zdraw access to depth buffer if it is enabled,
             * whereas IrisGL only allows this if it is disabled.
             */
            glPushAttrib(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_ALWAYS);
        }
        else
        {
#ifdef GL_EXT_abgr
            *format = GL_ABGR_EXT;
#else
            *format = GL_RGBA;
#endif
            *type = GL_UNSIGNED_BYTE;
        }
        glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 0);
    }
    else
    {
        /* set stride and row alignment. IrisGL stride is in 32 bit words. */
        glPixelStorei (GL_UNPACK_ROW_LENGTH, wptr->pixStride*32 / pxsz);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 4);
    }

    return imgOgl;
}

void
_igl_pixInEnd(int format, int type, void *image, void *buffer)
{
    if (format == GL_DEPTH_COMPONENT)
    {
        glPopAttrib ();
#ifdef __sgi
        glPixelStorei (GL_UNPACK_SWAP_BYTES, 0);
#endif
    }

    if (image != buffer)
        free (buffer);
}

/* map IrisGL input mode to OpenGL */
void *
_igl_pixOutMode(int *format, int *type, void *image, int width, int height)
{
    igl_windowT *wptr = IGL_CTX ();
    int pxsz = wptr->pixSize ? wptr->pixSize : 32, cnm, csz;
    uint32_t *imgOgl = image;

    *format = _igl_pixOglFormat (wptr->pixOutFormat, &cnm);
    *type = _igl_pixOglType (wptr->pixOutType, &csz);

    /* TODO: man page says, if PM_LUMINANCE[A] is selected, only PM_INPUT_TYPE,
     * PM_OUTPUT_TYPE, PM_OFFSET, and PM_STRIDE are considered. Of these, only
     * PM_OFFSET != 0 would prevent the direct use of GL_LUMINANCE[_ALPHA].
     */
    /* TODO: support for readcomponent() */

    /* see if transfer can be handled directly by OpenGL */
    if (cnm*csz != pxsz || (wptr->state & IGL_WSTATE_RDSOURCEZ) ||
            !(wptr->flags & IGL_WFLAGS_RGBA) || wptr->pixFlags)
    {
        /* no, use software transfer through buffer */
        imgOgl = malloc(width*height * sizeof(*imgOgl));
        if (wptr->pixZ || (wptr->state & IGL_WSTATE_RDSOURCEZ))
        {
            *format = GL_DEPTH_COMPONENT;
            *type = GL_UNSIGNED_INT;
        }
        else
        {
#ifdef GL_EXT_abgr
            *format = GL_ABGR_EXT;
#else
            *format = GL_RGBA;
#endif
            *type = GL_UNSIGNED_BYTE;
        }
        glPixelStorei (GL_PACK_ROW_LENGTH, 0);
        glPixelStorei (GL_PACK_ALIGNMENT, 0);
    }
    else
    {
        /* set stride and row alignment. IrisGL stride is in 32 bit words. */
        glPixelStorei (GL_PACK_ROW_LENGTH, wptr->pixStride*32 / pxsz);
        glPixelStorei (GL_PACK_ALIGNMENT, 4);
    }

    return imgOgl;
}

/* convert OpenGL input data to IrisGL (applies PM_OFFSET, PM_STRIDE, PM_TTOB, PM_RTOL) */
void
_igl_pixOutConvert(int format, int type, void *image, void *buffer, int width, int height)
{
    igl_windowT *wptr = IGL_CTX ();
    uint32_t *iptr = buffer, *imgOut = image;

    if (image != buffer)
    {
        igl_pixdataT pd;
        int x, y;

        /* use software engine to convert GL_RGBA/DEPTH to IrisGL */
        pd.format = wptr->pixOutFormat;
        pd.type = wptr->pixOutType;
        pd.size = wptr->pixSize;

        /* for TTOB start with top line, for RTOL with last pixel in line */
        if (wptr->pixTtoB)
            iptr += width * (height-1);
        if (wptr->pixRtoL)
            iptr += width - 1;

        for (y = 0; y < height; y++)
        {
            /* prepare pixel stream for this line */
            pd.img = imgOut;
            pd.data = 0;
            pd.bit = 64-wptr->pixOffset;

            for (x = 0; x < width; x++)
            {
                unsigned long pixel;

                /* get OpenGL input pixel */
                if (wptr->pixZ || (wptr->state & IGL_WSTATE_RDSOURCEZ))
                    /* OpenGL uses top bits, but IrisGL uses the bottom bits */
                    pixel = *iptr >> (32-IGL_ZBITS);
                else if (wptr->flags & IGL_WFLAGS_RGBA)
#ifdef GL_EXT_abgr
                    pixel = *iptr;
#else
                    pixel = _igl_swap32 (*iptr);
#endif
                else
                    pixel = 0; /* NOTE: no real colormap mode -> BLACK */
                /* advance to next input pixel */
                if (wptr->pixRtoL)
                    iptr --;
                else
                    iptr ++;

                /* process and output pixel stream */
                pixel = _igl_pixPipe (pixel);

                if (pd.format != PM_ABGR || pd.type != PM_UNSIGNED_BYTE)
                    pixel = _igl_pixOutFormat (&pd, pixel);

                _igl_pixPack (&pd, pixel);
            }
            /* flush output buffer */
            if (pd.bit < 64)
                *pd.img++ = pd.data >> 32;
            if (pd.bit < 32)
                *pd.img++ = pd.data;
            /* advance to next stream scanline */
            if (wptr->pixStride)
                imgOut += wptr->pixStride;
            else
                imgOut = pd.img;
            /* advance to next input line */
            if (wptr->pixTtoB)
                iptr -= 2*width;
            if (wptr->pixRtoL)
                iptr += 2*width;
        }
    }
}


void
_igl_pixOutEnd(int format, int type, void *image, void *buffer)
{
    if (image != buffer)
        free (buffer);
}


/* GL: pixmode, pixmodef - specify pixel transfer mode parameters */
void
pixmode(long mode, long value)
{
    igl_windowT *wptr;
    long def = -1;

    _igl_trace (__func__,TRUE,"%ld,%ld",mode,value);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    switch (mode)
    {
        case PM_STRIDE:         wptr->pixStride = value; break;
        case PM_SIZE:           wptr->pixSize = value; break;
        case PM_OFFSET:         wptr->pixOffset = value; def = 0; break;

        case PM_INPUT_FORMAT:   wptr->pixInFormat = value; break;
        case PM_INPUT_TYPE:     wptr->pixInType = value; break;
        case PM_OUTPUT_FORMAT:  wptr->pixOutFormat = value; break;
        case PM_OUTPUT_TYPE:    wptr->pixOutType = value; break;
        case PM_ZDATA:          wptr->pixZ = (value != 0); def = 0; break;

            /* "Use glPixelZoom with negative zoom factors to reflect images" */
        case PM_TTOB:		wptr->pixTtoB = (value != 0); def = 0; break;
        case PM_RTOL:           wptr->pixRtoL = (value != 0); def = 0; break;

            /* "Use LUT to convert bitmap data: glPixelTransfer, glPixelMap" */
        case PM_EXPAND:         wptr->pixExpand = (value != 0); def = 0; break;
        case PM_C0:             wptr->pixCx[0] = value; break;
        case PM_C1:             wptr->pixCx[1] = value; break;

            /* "Use the OpenGL color matrix extension: see glPixelTransfer" */
        case PM_SHIFT:          wptr->pixShift = value; def = 0; break;
        case PM_ADD24:          wptr->pixAdd24 = value; def = 0; break;

        default:
            break;
    }

    /* set flag if this pixmode cannot be directly evaluated by OpenGL */
    if (def >= 0 && def != value)
        wptr->pixFlags |=  (1 << mode);
    else
        wptr->pixFlags &= ~(1 << mode);
}

void
pixmodef(long mode, float value)
{
    _igl_trace (__func__,TRUE,"%ld,%g",mode,value);

    pixmode(mode, (long)value);
}


/* GL: rectzoom - specifies zoom for rectangular pixel copies and writes */
void
rectzoom(float xfactor, float yfactor)
{
    _igl_trace (__func__,TRUE,"%g,%g",xfactor,yfactor);
    IGL_CHECKWNDV ();

    /* NOTE: no fractional factors supported with rectzoom */
    IGL_CTX ()->xzoom = xfactor;
    IGL_CTX ()->yzoom = yfactor;
    glPixelZoom (IGL_CTX ()->xzoom, IGL_CTX ()->yzoom);
}


/* GL:  convolve - modify lrectwrite and rectcopy to convolve pixel data */
void
convolve (long op, long border, long xksize, long yksize, float *kernel, float bias)
{
    int oglborder[1] = { GL_REDUCE };
    float oglkernel[xksize*yksize];
    float oglbias[4];
    int i, j;

    _igl_trace (__func__,TRUE,"%ld,%ld,%ld,%ld,%p,%g",op,border,xksize,yksize,kernel,bias);
    IGL_CHECKWNDV ();

    switch (op)
    {
        case CV_SEPARABLE:
            /* TODO: should use glSeparableFilter2D if extension is available */
            for (i = 0; i < yksize; i++)
                for (j = 0; j < xksize; j++)
                    oglkernel[j + i*xksize] = kernel[j] * kernel[i+xksize];
            glConvolutionFilter2D (GL_CONVOLUTION_2D, GL_LUMINANCE,
                            xksize, yksize, GL_LUMINANCE, GL_FLOAT, oglkernel);
            break;

        case CV_GENERAL:
            glConvolutionFilter2D (GL_CONVOLUTION_2D, GL_LUMINANCE,
                            xksize, yksize, GL_LUMINANCE, GL_FLOAT, kernel);
            break;

        case CV_OFF:
        default:
            glDisable (GL_CONVOLUTION_2D);
            return;
    }

    oglbias[0] = oglbias[1] = oglbias[2] = bias; oglbias[3] = 0;
    glConvolutionParameterfv (GL_CONVOLUTION_2D, GL_CONVOLUTION_FILTER_BIAS, oglbias);
    glConvolutionParameteriv (GL_CONVOLUTION_2D, GL_CONVOLUTION_BORDER_MODE, oglborder);
    glEnable (GL_CONVOLUTION_2D);
}


/* GL: pixelmap - define pixel transfer LUTs */
void
pixelmap (int map, int size, unsigned short *values)
{
    GLenum oglmap = 0;

    _igl_trace (__func__,TRUE,"%d,%d,%p",map,size,values);
    IGL_CHECKWNDV ();

    switch (map)
    {
        case MAP_I_TO_I:        oglmap = GL_PIXEL_MAP_I_TO_I; break;
        case MAP_I_TO_R:        oglmap = GL_PIXEL_MAP_I_TO_R; break;
        case MAP_I_TO_G:        oglmap = GL_PIXEL_MAP_I_TO_G; break;
        case MAP_I_TO_B:        oglmap = GL_PIXEL_MAP_I_TO_B; break;
        case MAP_I_TO_A:        oglmap = GL_PIXEL_MAP_I_TO_A; break;
        case MAP_R_TO_R:        oglmap = GL_PIXEL_MAP_R_TO_R; break;
        case MAP_G_TO_G:        oglmap = GL_PIXEL_MAP_G_TO_G; break;
        case MAP_B_TO_B:        oglmap = GL_PIXEL_MAP_B_TO_B; break;
        case MAP_A_TO_A:        oglmap = GL_PIXEL_MAP_A_TO_A; break;
        default:                return;
    }

    glPixelMapusv (oglmap, size, values);
}


/* GL: pixeltransfer - sets pixel/texel transfer modes */
void
pixeltransfer (int mode, float value)
{
    GLenum oglmap = 0;


    _igl_trace (__func__,TRUE,"%d,%g",mode,value);
    IGL_CHECKWNDV ();

    switch (mode)
    {
        case PT_RED_SCALE:      oglmap = GL_RED_SCALE; break;
        case PT_GREEN_SCALE:    oglmap = GL_GREEN_SCALE; break;
        case PT_BLUE_SCALE:     oglmap = GL_BLUE_SCALE; break;
        case PT_ALPHA_SCALE:    oglmap = GL_ALPHA_SCALE; break;
        case PT_RED_BIAS:       oglmap = GL_RED_BIAS; break;
        case PT_GREEN_BIAS:     oglmap = GL_GREEN_BIAS; break;
        case PT_BLUE_BIAS:      oglmap = GL_BLUE_BIAS; break;
        case PT_ALPHA_BIAS:     oglmap = GL_ALPHA_BIAS; break;
        case PT_MAP_COLOR:      oglmap = GL_MAP_COLOR; break;
        default:                return;
    }

    if (mode == PT_MAP_COLOR)
        glPixelTransferi (oglmap, value != 0);
    else
        glPixelTransferf (oglmap, value);
}


/* GL: readsource - sets the source for pixels that various routines read */
void
readsource (long src)
{
    _igl_trace (__func__,TRUE,"%ld",src);
    IGL_CHECKWNDV ();

    IGL_CTX ()->state &= ~IGL_WSTATE_RDSOURCEZ;
    switch (src)
    {
        case SRC_AUTO:
            glReadBuffer (IGL_CTX ()->flags & IGL_WFLAGS_DOUBLEBUFFER ? GL_BACK : GL_FRONT);
            break;
        case SRC_BACK:
        case SRC_BACKRIGHT:
            if (IGL_CTX ()->flags & IGL_WFLAGS_DOUBLEBUFFER)
                glReadBuffer (src == SRC_BACK ? GL_BACK: GL_BACK_RIGHT);
            break;

        case SRC_FRONT:
        case SRC_FRONTRIGHT:
            glReadBuffer (src == SRC_FRONT ? GL_FRONT: GL_FRONT_RIGHT);
            break;

        case SRC_ZBUFFER:
            /* TODO: how does this interact with pixmode(PM_ZDATA)? */
            IGL_CTX ()->state |= IGL_WSTATE_RDSOURCEZ;
            break;

        case SRC_OVER:
        case SRC_UNDER:
        case SRC_PUP:
        case SRC_ILBUFFER_1: /* upto _16 */
            /* TODO: storing in IGL_CTX () needed. in lrectread, rectcopy, and
             * fbsubtexload switch to drawable and copy through buffer. Is this
             * worth the hassle?
             */
        default:
            break;
    }
}


/* GL: readcomponent - sets the component source within the framebuffer */
void
readcomponent (long mode)
{
    _igl_trace (__func__,TRUE,"");

    /* TODO: still a stub. what exactly is the effect of this with lrectread,
     * rectread, readRGB, fbsubtexload, and how does it correlate with pixmode?
     */
#if 0
    switch (mode)
    {
    default:
    case RC_ABGR:               IGL_CTX ()->readComponent = GL_RGBA; break;
    case RC_ALPHA:              IGL_CTX ()->readComponent = GL_ALPHA; break;
    case RC_BLUE:               IGL_CTX ()->readComponent = GL_BLUE; break;
    case RC_GREEN:              IGL_CTX ()->readComponent = GL_GREEN; break;
    case RC_RED:                IGL_CTX ()->readComponent = GL_RED; break;
    }
#endif
}

