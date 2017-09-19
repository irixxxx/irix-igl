/*
 * texture.c
 *
 * texture and texture coordinate handling
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


#define DBG(x)          //x

#if 0
____________________________ IGL helpers ____________________________
#endif

/*  check if texturing is active */
static void
_igl_checkTexturing (igl_windowT *wptr)
{
    if (wptr->state & IGL_WSTATE_TEXTURING)
    {
        glDisable (GL_TEXTURE_2D);
        glDisable (GL_TEXTURE_3D);
    }

    /* tev and tex must both be non-zero for texturing to be enabled */
    if (wptr->tevIndex != 0 && wptr->texIndex != 0)
    {
        glEnable (wptr->textureType);
        wptr->state |= IGL_WSTATE_TEXTURING;
    }
    else
        wptr->state &= ~(IGL_WSTATE_TEXTURING | IGL_VSTATE_TEXTURED);
}


/* parse IrisGL texture environment definition */
static void
_igl_parseTevDef (long index, long np, float *props)
{
    igl_tevDefT *tvptr;
    int i;


    tvptr = &igl->tevDefs[index];
    tvptr->tvMode = TV_MODULATE;    /* this is the default mode */

    for (i=0; props[i] != TV_NULL && (np == 0 || i < np); i++)
    {
        switch ((int)props[i])
        {
            /* these four are mutually exclusive */
            case TV_BLEND:
            case TV_DECAL:
            case TV_ALPHA:
            case TV_MODULATE:
DBG(printf("mode %f\n",props[i]));
                tvptr->tvMode = (int)props[i];
            break;

            case TV_COLOR:
DBG(printf("color %f %f %f %f\n", props[i+1], props[i+2], props[i+3], props[i+4]));
                tvptr->flags |= IGL_TEVDEFFLAGS_COLOR;
                IGL_V4COPY (tvptr->blendColor, &props[i+1]);
                i += 4;
            break;

            case TV_COMPONENT_SELECT:
DBG(printf("select %f\n", props[i+1]));
                tvptr->flags |= IGL_TEVDEFFLAGS_COMPONENT;
                tvptr->component = props[i+1];
                i ++;
            break;

            default:
            break;
        }
    }
}


/* set OpenGL texture environment */
static void
_igl_setTevDef (long index)
{
    igl_tevDefT *tvptr = &igl->tevDefs[index];

    IGL_CTX ()->tevIndex = index;
    switch (tvptr->tvMode)
    {
        case TV_BLEND:
            glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        break;

        case TV_DECAL:
            glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        break;

        /* TBD: what to do with this one? */
        case TV_ALPHA:
        break;

        /* if none specified, TV_MODULATE is the default */
        case TV_MODULATE:
        default:
            glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        break;
    }

    if (tvptr->flags & IGL_TEVDEFFLAGS_COLOR)
        glTexEnvfv (GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, tvptr->blendColor);
    if (tvptr->flags & IGL_TEVDEFFLAGS_COMPONENT)
    {
        /* TODO: texture select extension
         * glTexParameteri (GL_TEXTURE_2D, GL_{DUAL|QUAD}_TEXTURE_SELECT_SGIS, n);
         * texture must have internalFormat GL_{DUAL|QUAD}_{.*}_SGIS.
         */
    }
}


/* parse IrisGL texture definition */
static void
_igl_parseTexDef (long index, long np, float *props)
{
    int i;
    igl_texDefT *txptr;

    txptr = &igl->texDefs[index];
    if (txptr->glName)
        glDeleteTextures (1, &txptr->glName);

    IGL_CLR_SAVEID (txptr);
    txptr->internalFormat = TX_RGBA_8;
    txptr->externalFormat = TX_PACK_8;

    for (i=0; props[i] != TX_NULL && (np == 0 || i < np); i++)
    {
        switch ((int)props[i])
        {
            case TX_FAST_DEFINE:
DBG(printf("fast\n"));
                break;

DBG(printf("minflt %f\n", props[i+1]));
            case TX_MINFILTER:
                txptr->flags |= IGL_TEXDEFFLAGS_MINFILTER;
                txptr->minFilter = props[i+1];
                i ++;
                break;

            case TX_MAGFILTER:
DBG(printf("magflt %f\n", props[i+1]));
                txptr->flags |= IGL_TEXDEFFLAGS_MAGFILTER;
                txptr->magFilter = props[i+1];
                i ++;
                break;

            case TX_MAGFILTER_ALPHA:
DBG(printf("magalpha %f\n", props[i+1]));
                txptr->flags |= IGL_TEXDEFFLAGS_MAGFILTERALPHA;
                txptr->magFilterAlpha = props[i+1];
                i ++;
                break;

            case TX_MAGFILTER_COLOR:
DBG(printf("magcolor %f\n", props[i+1]));
                txptr->flags |= IGL_TEXDEFFLAGS_MAGFILTERCOLOR;
                txptr->magFilterColor = props[i+1];
                i ++;
                break;

            case TX_WRAP:
DBG(printf("wrap %f\n", props[i+1]));
                txptr->flags |= (IGL_TEXDEFFLAGS_WRAPS | IGL_TEXDEFFLAGS_WRAPT | IGL_TEXDEFFLAGS_WRAPR);
                txptr->wrapS = txptr->wrapT = txptr->wrapR = props[i+1];
                i ++;
                break;

            case TX_WRAP_S:
DBG(printf("wraps %f\n", props[i+1]));
                txptr->flags |= IGL_TEXDEFFLAGS_WRAPS;
                txptr->wrapS = props[i+1];
                i ++;
                break;

            case TX_WRAP_T:
DBG(printf("wrapt %f\n", props[i+1]));
                txptr->flags |= IGL_TEXDEFFLAGS_WRAPT;
                txptr->wrapT = props[i+1];
                i ++;
                break;

            case TX_WRAP_R:
DBG(printf("wrapr %f\n", props[i+1]));
                txptr->flags |= IGL_TEXDEFFLAGS_WRAPR;
                txptr->wrapR = props[i+1];
                i ++;
                break;

            case TX_INTERNAL_FORMAT:
DBG(printf("intfmt %f\n", props[i+1]));
                txptr->internalFormat = (int)props[i+1];
                i ++;
                break;

            case TX_EXTERNAL_FORMAT:
DBG(printf("extfmt %f\n", props[i+1]));
                txptr->externalFormat = (int)props[i+1];
                i ++;
                break;

            case TX_MIPMAP_FILTER_KERNEL:
DBG(printf("mipmap kernel\n"));
                txptr->flags |= IGL_TEXDEFFLAGS_MIPMAPFILTER;
                memcpy (txptr->mipmapFilter, &props[i+1], 8*sizeof(float));
                i += 8;
                break;

            case TX_CONTROL_POINT:
                /* can appear multiple (max 4) times for different lod's */
DBG(printf("control %f %f\n", props[i+1], props[i+2]));
                txptr->flags |= IGL_TEXDEFFLAGS_CONTROLPOINT;
                if (txptr->numCtlPoints < 4)
                {
                    txptr->ctlPoints[2*txptr->numCtlPoints+0] = props[i+1];
                    txptr->ctlPoints[2*txptr->numCtlPoints+1] = props[i+2];
                    txptr->numCtlPoints ++;
                }
                i += 2;
                break;

            case TX_CONTROL_CLAMP:
DBG(printf("clamp %f\n", props[i+1]));
                txptr->flags |= IGL_TEXDEFFLAGS_CLAMP;
                txptr->clamp = props[i+1];
                i ++;
                break;

            case TX_DETAIL:
DBG(printf("detail\n"));
                txptr->flags |= IGL_TEXDEFFLAGS_DETAIL;
                memcpy (txptr->detail, &props[i+1], 5*sizeof(float));
                i += 5;
                break;

            case TX_TILE:
DBG(printf("tile\n"));
                txptr->flags |= IGL_TEXDEFFLAGS_TILE;
                memcpy (txptr->tile, &props[i+1], 4*sizeof(float));
                i += 4;
                break;

            case TX_BICUBIC_FILTER:
DBG(printf("bicubic %f %f\n", props[i+1], props[i+2]));
                txptr->flags |= IGL_TEXDEFFLAGS_BICUBICFILTER;
                memcpy (txptr->bicubicFilter, &props[i+1], 2*sizeof(float));
                i += 2;
                break;

            case TX_FRAMEBUFFER_SRC:
DBG(printf("fbsrc\n"));
                txptr->flags |= IGL_TEXDEFFLAGS_FRAMEBUFFERSRC;
                break;

            default:
DBG(printf("unknown %x\n",(int)props[i]));
                break;
        }
    }
}


/* set OpenGL texture parameter minfilter */
static void
_igl_setTexMinFilter (int texture, igl_texDefT *txptr)
{
    GLuint oglType;

    switch ((int)txptr->minFilter)
    {
        case TX_POINT:
            oglType = GL_NEAREST;
            break;

        case TX_BILINEAR:
        case TX_TRILINEAR:
            oglType = GL_LINEAR;
            break;

        case TX_MIPMAP_POINT:
            oglType = GL_NEAREST_MIPMAP_NEAREST;
            break;

        case TX_MIPMAP_LINEAR:
            oglType = GL_NEAREST_MIPMAP_LINEAR;
            break;

        case TX_MIPMAP_BILINEAR:
            oglType = GL_LINEAR_MIPMAP_NEAREST;
            break;

        case TX_MIPMAP_TRILINEAR:
        case TX_MIPMAP_QUADLINEAR:
            oglType = GL_LINEAR_MIPMAP_LINEAR;
            break;

        case TX_BILINEAR_LEQUAL:
        case TX_BILINEAR_GEQUAL:
            /* TODO: depth mode texture
             * oglType = GL_LINEAR; ???
             * glTexParameteri (texture, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
             * glTexParameteri (texture, GL_TEXTURE_COMPARE_FUNC, GL_[LG]EQUAL);
             * glTexParameteri (texture, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
             * texture must have internalFormat GL_DEPTH_COMPONENT.
             */

        case TX_BICUBIC:         /* no equivalent in OpenGL */
        default:
            oglType = GL_LINEAR;
            break;
    }

    glTexParameteri (texture, GL_TEXTURE_MIN_FILTER, oglType);
}

/* set OpenGL texture parameter magfilter */
static void
_igl_setTexMagFilter (int texture, igl_texDefT *txptr)
{
    GLuint oglType;

    switch ((int)txptr->magFilter)
    {
        case TX_POINT:
            oglType = GL_NEAREST;
            break;

        case TX_BILINEAR:
        case TX_TRILINEAR:
            oglType = GL_LINEAR;
            break;

        case TX_SHARPEN:
            oglType = GL_LINEAR;
            /* TODO: only available with SGIs sharpen extension, TX_CTRL_CLAMP?
             * oglType = GL_LINEAR_SHARPEN_SGIS;
             * glSharpenTexFuncSGIS (texture, txptr->numCtlPoints, txptr->ctlPoints);
             */
            break;

        case TX_ADD_DETAIL:
        case TX_MODULATE_DETAIL:
            oglType = GL_LINEAR;
            /* TODO: only available with SGIs detail extension, TX_CTRL_CLAMP?
             * oglType = GL_LINEAR_DETAIL_SGIS;
             * glDetailTexFuncSGIS (texture, txptr->numCtlPoints, txptr->ctlPoints); 
             * glTexParameteri(texture, GL_DETAIL_TEXTURE_LEVEL_SGIS, lod);
             * glTexParameteri(texture, GL_DETAIL_TEXTURE_MODE_SGIS, GL_ADD/GL_MODULATE);
             * texture must have target GL_DETAIL_TEXTURE_2D_SGIS.
             */
            break;

        case TX_BILINEAR_LEQUAL:
        case TX_BILINEAR_GEQUAL:
            /* TODO: see _igl_setTexMinFilter */

        case TX_BICUBIC:         /* no equivalent in OpenGL */
        default:
            oglType = GL_LINEAR;
            break;
    }

    glTexParameteri (texture, GL_TEXTURE_MAG_FILTER, oglType);
}

/* set OpenGL texture parameter wrap(_S,_R,_T) */
static void
_igl_setTexWrap (int texture, int texParam, float wrap)
{
    GLuint oglType;

    switch ((int)wrap)
    {
        case TX_REPEAT:
            oglType = GL_REPEAT;
            break;

        case TX_CLAMP:
            oglType = GL_CLAMP;
            break;

        case TX_SELECT: /* no equivalent in OpenGL */
        default:
            oglType = GL_CLAMP;
            break;
    }

    glTexParameteri (texture, texParam, oglType);
}

/* set OpenGL texture parameters */
static void
_igl_setTexDef (long index)
{
    igl_texDefT *txptr = &igl->texDefs[index];
    int oglTexture = (txptr->flags & IGL_TEXDEFFLAGS_3D) ? GL_TEXTURE_3D : GL_TEXTURE_2D;

    IGL_CTX ()->texIndex = index;
    if (txptr->flags & IGL_TEXDEFFLAGS_MINFILTER)
        _igl_setTexMinFilter (oglTexture, txptr);

    if (txptr->flags & IGL_TEXDEFFLAGS_MAGFILTER)
        _igl_setTexMagFilter (oglTexture, txptr);

    if (txptr->flags & IGL_TEXDEFFLAGS_WRAPS)
        _igl_setTexWrap (oglTexture, GL_TEXTURE_WRAP_S, txptr->wrapS);

    if (txptr->flags & IGL_TEXDEFFLAGS_WRAPT)
        _igl_setTexWrap (oglTexture, GL_TEXTURE_WRAP_T, txptr->wrapT);

    if (txptr->flags & IGL_TEXDEFFLAGS_WRAPR)
        _igl_setTexWrap (oglTexture, GL_TEXTURE_WRAP_R, txptr->wrapR);

    /* IGL_TEXDEFFLAGS_MAGFILTERALPHA: no equivalent in OpenGL */
    /* IGL_TEXDEFFLAGS_MAGFILTERCOLOR: no equivalent in OpenGL */
    /* IGL_TEXDEFFLAGS_MIPMAPFILTER: no equivalent in OpenGL */
    /* IGL_TEXDEFFLAGS_BICUBICFILTER: no equivalent in OpenGL */
}

/* map IrisGL texture #components to OpenGL format */
static int
_igl_texFormat (int nc)
{
    switch (nc)
    {
        default:
        case 4:         return GL_ABGR_EXT;
        case 3:         return GL_BGR;
        case 2:         return GL_LUMINANCE_ALPHA;
        case 1:         return GL_LUMINANCE;
    }
}

/* map IrisGL texture external format to OpenGL type */
static int
_igl_texType (int extFormat, int *size)
{
    switch (extFormat)
    {
        default:
        case TX_PACK_8:  *size =  8; return GL_UNSIGNED_BYTE;
        case TX_PACK_16: *size = 16; return GL_UNSIGNED_SHORT;
    }
}

/* get OpenGL format and type from texture definition */
static int
_igl_texFormatType (igl_texDefT *txptr, int *oglFormat, int *oglType)
{
    int csz;

    if (txptr->externalFormat == TX_PIXMODE)
        return _igl_pixInMode(oglFormat, oglType);
    else
    {
        *oglFormat = _igl_texFormat (txptr->size);
        *oglType = _igl_texType (txptr->externalFormat, &csz);
        glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 0);
        return 0; /* txptr->size * csz */
    }
}

/* get OpenGL internal format from texture definition */
static int
_igl_texIntFormat (igl_texDefT *txptr)
{
    switch (txptr->internalFormat)
    {
        case TX_I_12A_4: return GL_LUMINANCE12_ALPHA4;
        case TX_IA_8:    return GL_LUMINANCE8_ALPHA8;
        case TX_RGB_5:   return GL_RGB5;
        case TX_RGBA_4:  return GL_RGBA4;
        case TX_IA_12:   return GL_LUMINANCE12_ALPHA12;
        case TX_RGBA_8:  return GL_RGBA8;
        case TX_RGBA_12: return GL_RGBA12;
        case TX_RGB_12:  return GL_RGB12;
        case TX_I_16:    return GL_INTENSITY16;
        default:         return GL_RGBA8;
    }
}


#if 0
____________________________ tev|tex functions ____________________________
#endif
/* GL: tevdef - defines a texture mapping environment */
void
tevdef (long index, long np, float *props)
{
    int idx;

    _igl_trace (__func__,TRUE,"%ld,%ld,%p",index,np,props);
    IGL_CHECKWNDV ();

    if (props == NULL)
        return;

    idx = IGL_MAPID(igl->tevDefs, IGL_MAXTEVDEFS, index, 3);
    if (idx < 0)
        return;

    _igl_parseTevDef (idx, np, props);
}


/* GL: tevbind -  selects a texture environment */
void
tevbind (long target, long index)
{
    _igl_trace (__func__,FALSE,"%ld,%ld",target,index);
    igl->api->tevbind (target, index);
}

void
iglExec_tevbind (long target, long index)
{
    int idx;

    IGL_CHECKWNDV ();

    if (target != TV_ENV0)
        return;

    idx = IGL_MAPID(igl->tevDefs, IGL_MAXTEVDEFS, index, 0);
    if (idx < 0)
        return;

    _igl_setTevDef (idx);
    _igl_checkTexturing (IGL_CTX ());
}


/* GL: texdef2d, texdef3d - convert a 2/3-dimensional image into a texture */
void
texdef2d (long index, long nc, long width, long height, unsigned long *image, long np, float *props)
{
    int idx, type, ointernalFormat, oFormat, osize;
    igl_texDefT *txptr;
    unsigned long *img = NULL;

    _igl_trace (__func__,TRUE,"%ld,%ld,%ld,%ld,%p,%ld,%p",index,nc,width,height,image,np,props);
    IGL_CHECKWNDV ();

    if (props == NULL || width <= 0 || height <= 0 || nc < 1 || nc > 4)
        return;

    idx = IGL_MAPID(igl->texDefs, IGL_MAXTEXDEFS, index, 3);
    if (idx < 0)
        return;

    /* parse */
    _igl_parseTexDef (idx, np, props);
    txptr = &igl->texDefs[idx];
    glGenTextures (1, &txptr->glName);

    txptr->width = width;
    txptr->height = height;
    txptr->size = nc;

    osize = _igl_texFormatType (txptr, &oFormat, &type);
    ointernalFormat = _igl_texIntFormat (txptr);
    if (txptr->flags & IGL_TEXDEFFLAGS_FRAMEBUFFERSRC)
    {
        if (IGL_CTX ()->state & IGL_WSTATE_RDSOURCEZ)
        {
            /* TODO: depth scale for 16 bit depth (lsetdepth(0, 0xffff)) here */
            int scale = 1 << (IGL_ZBITS - 16);
#if 1
            /* scale factor is 1<<(getgdesc (GD_ZMAX) / 0xffff) */
            glPixelTransferf (GL_DEPTH_SCALE, scale);
#else
            /* Shadow mapping test code. Removes the top 3 bits for shad demo */
            glPixelTransferf (GL_DEPTH_SCALE, scale << 3);
            glPixelTransferf (GL_DEPTH_BIAS, -7.0);
#endif
            ointernalFormat = oFormat = GL_DEPTH_COMPONENT;
            type = GL_UNSIGNED_SHORT;
        }
    }
    else if (image)
        img = _igl_pixInConvert (&oFormat, &type, osize, image, width, height);
    DBG(printf("ifmt=%x ofmt=%x type=%x\n", ointernalFormat, oFormat, type));

    /* TODO: is there something to be done for TX_TILE? */

    glBindTexture (GL_TEXTURE_2D, txptr->glName);
    /* TODO: TX_DETAIL for 2D: glTexImage2D (GL_DETAIL_TEXTURE_2D_SGIS, ...)  */
    /* TODO: TX_MIN/MAG_FILTER TX_BILINEAR_{L|G}EQUAL: glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ...)  */
    glTexImage2D (GL_TEXTURE_2D, 0, ointernalFormat, width, height, 0, oFormat, type, img);
    if (img != NULL)
        gluBuild2DMipmaps (GL_TEXTURE_2D, ointernalFormat, width, height, oFormat, type, img);
    else if (txptr->flags & IGL_TEXDEFFLAGS_FRAMEBUFFERSRC)
        /* NOTE: no mip maps in this case? */
        glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);

    if (!(txptr->flags & IGL_TEXDEFFLAGS_FRAMEBUFFERSRC) && image)
        _igl_pixInEnd (oFormat, type, image, img);
}

void
texdef3d (long index, long nc, long width, long height, long depth, unsigned long *image, long np,
          float *props)
{
    int idx, type, ointernalFormat, oFormat, osize;
    igl_texDefT *txptr;
    unsigned long *img = NULL;

    _igl_trace (__func__,TRUE,"%ld,%ld,%ld,%ld,%ld,%p,%ld,%p",index,nc,width,height,depth,image,np,props);
    IGL_CHECKWNDV ();

    if (props == NULL || width <= 0 || height <= 0 || depth <= 0 || nc < 1 || nc > 4)
        return;

    idx = IGL_MAPID(igl->texDefs, IGL_MAXTEXDEFS, index, 3);
    if (idx < 0)
        return;

    /* parse */
    _igl_parseTexDef (idx, np, props);
    txptr = &igl->texDefs[idx];
    txptr->flags |= IGL_TEXDEFFLAGS_3D;
    glGenTextures (1, &txptr->glName);

    txptr->width = width;
    txptr->height = height;
    txptr->size = nc;

    osize = _igl_texFormatType (txptr, &oFormat, &type);
    ointernalFormat = _igl_texIntFormat (txptr);
    DBG(printf("texdef3d: nc=%ld ofmt=%x type=%x\n", nc, oFormat, type));
    if (image)
        img = _igl_pixInConvert (&oFormat, &type, osize, image, width, height*depth);

    glBindTexture (GL_TEXTURE_3D, txptr->glName);
    glTexImage3D (GL_TEXTURE_3D, 0, ointernalFormat, width, height, depth, 0, oFormat, type, img);
    if (img != NULL)
        gluBuild3DMipmaps (GL_TEXTURE_3D, ointernalFormat, width, height, depth, oFormat, type, img);

    if (image)
        _igl_pixInEnd (oFormat, type, image, img);
}


/* GL: texbind -  selects a texture function */
void
texbind (long target, long index)
{
    _igl_trace (__func__,FALSE,"%ld,%ld",target,index);
    igl->api->texbind (target, index);
}

void
iglExec_texbind (long target, long index)
{
    igl_texDefT *txptr;
    int idx;

    IGL_CHECKWNDV ();

    /* TODO: support for DETAIL, IDLE? */
    if (target != TX_TEXTURE_0/* && target != TX_TEXTURE_DETAIL && target != TX_TEXTURE_IDLE*/)
        return;

    idx = IGL_MAPID(igl->texDefs, IGL_MAXTEXDEFS, index, 0);
    if (idx < 0 && index != 0)
        return;
    txptr = &igl->texDefs[idx];

    if (index == 0 || txptr->glName == 0)
    {
        glBindTexture (GL_TEXTURE_2D, 0);
        glBindTexture (GL_TEXTURE_3D, 0);
        IGL_CTX ()->texIndex = 0;
    }
    else
    {
        IGL_CTX ()->textureType =
            (txptr->flags & IGL_TEXDEFFLAGS_3D ? GL_TEXTURE_3D : GL_TEXTURE_2D);
        glBindTexture (IGL_CTX ()->textureType, txptr->glName);
        _igl_setTexDef (idx);
    }
    _igl_checkTexturing (IGL_CTX ());
}


/* GL: texgen - specify automatic generation of texture coordinates */
void
texgen (long coord, long mode, float *params)
{
    _igl_trace (__func__,TRUE,"%ld,%ld,%p",coord,mode,params);
    igl->api->texgen (coord, mode, params);
}

void
iglExec_texgen (long coord, long mode, float *params)
{
    int oglType = -1, oglTex = -1;

    IGL_CHECKWNDV ();

    /* is this pretty or what? */
#define TXGL(_a) if (coord == TX_##_a) { oglType=GL_##_a; oglTex=GL_TEXTURE_GEN_##_a; }
    TXGL(S)
    else TXGL(T)
    else TXGL(R)
    else TXGL(Q)
    else return;
#undef TXGL

    if (mode == TG_ON)
    {
        IGL_CTX ()->texgenMode |=  (1 << coord);
        glEnable (oglTex);
    }
    else if (mode == TG_OFF)
    {
        IGL_CTX ()->texgenMode &= ~(1 << coord);
        glDisable (oglTex);
    }
    else if (mode == TG_LINEAR)
    {
        glTexGenf (oglType, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGenfv (oglType, GL_OBJECT_PLANE, params);
    }
    else if (mode == TG_CONTOUR)
    {
        glTexGenf (oglType, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGenfv (oglType, GL_EYE_PLANE, params);
    }
    else if (mode == TG_SPHEREMAP)
    {
        glTexGenf (oglType, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    }
}


#if 0
____________________________ misc functions ____________________________
#endif
/* GL: subtexload - load part or all of a texture defined with TX_FASTDEFINE */
void
subtexload (long target, long id, float s0, float s1, float t0, float t1,
            long numwords, unsigned long *texture, unsigned long flags)
{
    int idx, type, format, size;
    igl_texDefT *txptr;
    unsigned long *img = texture;
    int tx, ty, width, height;

    _igl_trace (__func__,TRUE,"...");
    IGL_CHECKWNDV ();

    idx = IGL_MAPID(igl->texDefs, IGL_MAXTEXDEFS, id, 0);
    txptr = &igl->texDefs[idx];
    if (target != TX_TEXTURE_0 || idx < 0 || id == 0 || (txptr->flags & IGL_TEXDEFFLAGS_3D))
        return;

    tx = s0 * txptr->width;
    ty = t0 * txptr->height;
    width = (s1-s0) * txptr->width;
    height = (t1-t0) * txptr->height;

    /* TODO: is this implementation of flags OK? */
    if (flags == 1)
    {
        size = _igl_texFormatType (txptr, &format, &type);
        type = GL_UNSIGNED_SHORT;
        /* TODO: left alignment to size (glPixelTransfer(GL_[RGB]_SCALE))? */
    }
    else if (flags == 2)
    {
        format = GL_ABGR_EXT;
        type = GL_UNSIGNED_BYTE;
        glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 0);
    }
    else
    {
        size = _igl_texFormatType (txptr, &format, &type);
        img = _igl_pixInConvert (&format, &type, size, texture, width, height);
    }

    glBindTexture (GL_TEXTURE_2D, txptr->glName);
    glTexSubImage2D (GL_TEXTURE_2D, 0, tx, ty, width, height, format, type, img);

    _igl_pixInEnd (format, type, texture, img);
}


/* GL: fbsubtexload - load part or all of a texture defined with TX_FASTDEFINE */
void
fbsubtexload (long x, long y, long target, long id, float s0, float s1, float t0, float t1,
              unsigned long flags)
{
    int idx;
    igl_texDefT *txptr;
    int tx, ty, width, height;

    _igl_trace (__func__,TRUE,"...");
    IGL_CHECKWNDV ();

    idx = IGL_MAPID(igl->texDefs, IGL_MAXTEXDEFS, id, 0);
    txptr = &igl->texDefs[idx];
    if (target != TX_TEXTURE_0 || idx < 0 || id == 0 || (txptr->flags & IGL_TEXDEFFLAGS_3D))
        return;

    /* TODO: readsource(SRC_ZBUFFER) (really?), readcomponent() support */
    tx = s0 * txptr->width;
    ty = t0 * txptr->height;
    width = (s1-s0) * txptr->width;
    height = (t1-t0) * txptr->height;

    glBindTexture (GL_TEXTURE_2D, txptr->glName);
    glCopyTexSubImage2D (GL_TEXTURE_2D, 0, tx, ty, x, y, width, height);
}


/* GL: istexloaded - returns whether the texture is resident in texture memory */
long
istexloaded (long target, long id)
{
    int idx;
    igl_texDefT *txptr;
    GLuint tex;
    GLboolean res;

    _igl_trace (__func__,TRUE,"%ld,%ld",target,id);
    IGL_CHECKWND (0);

    idx = IGL_MAPID(igl->texDefs, IGL_MAXTEXDEFS, id, 0);
    if (target != TX_TEXTURE_0 || idx < 0 || id == 0)
        return 0;
    txptr = &igl->texDefs[idx];

    tex = txptr->glName;
    glAreTexturesResident (1, &tex, &res);
    return (res != GL_FALSE);
}


#if 0
____________________________ t functions ____________________________
#endif
/* GL: t[234][dfis] - specifies a texture coordinate tuple */
void
t4f (float vector[4])
{
    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(g),vector,IGL_V4PAR(vector));
    igl->api->t4f (vector);
}

void
iglExec_t4f (float vector[4])
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if (!wptr->bgnDelay)
        glTexCoord4fv (vector);
    if (wptr->bgnDelay || (wptr->state & IGL_VSTATE_TMESH))
        IGL_V4COPY (wptr->currentTexcoord, vector);
    wptr->state |= IGL_VSTATE_TEXTURED;
}


void
t4s (short vector[4])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(d),vector,IGL_V4PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = (float)vector[3];

    igl->api->t4f (vector4);
}


void
t4i (long vector[4])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(d),vector,IGL_V4PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = (float)vector[3];

    igl->api->t4f (vector4);
}


void
t4d (double vector[4])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(lg),vector,IGL_V4PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = (float)vector[3];

    igl->api->t4f (vector4);
}


void
t3s (short vector[3])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(d),vector,IGL_V3PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = 1.0f;

    igl->api->t4f (vector4);
}


void
t3i (long vector[3])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(d),vector,IGL_V3PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = 1.0f;

    igl->api->t4f (vector4);
}


void
t3f (float vector[3])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(g),vector,IGL_V3PAR(vector));
    vector4[0] = vector[0];
    vector4[1] = vector[1];
    vector4[2] = vector[2];
    vector4[3] = 1.0f;

    igl->api->t4f (vector4);
}


void
t3d (double vector[3])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(lg),vector,IGL_V3PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = 1.0f;

    igl->api->t4f (vector4);
}


void
t2s (short vector[2])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V2FMT(d),vector,IGL_V2PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = 0.0f;
    vector4[3] = 1.0f;

    igl->api->t4f (vector4);
}


void
t2i (long vector[2])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V2FMT(d),vector,IGL_V2PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = 0.0f;
    vector4[3] = 1.0f;

    igl->api->t4f (vector4);
}


void
t2f (float vector[2])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V2FMT(g),vector,IGL_V2PAR(vector));
    vector4[0] = vector[0];
    vector4[1] = vector[1];
    vector4[2] = 0.0f;
    vector4[3] = 1.0f;

    igl->api->t4f (vector4);
}


void
t2d (double vector[2])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V2FMT(lg),vector,IGL_V2PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = 0.0f;
    vector4[3] = 1.0f;

    igl->api->t4f (vector4);
}


/* GL: unimplemented stuff */
void
tlutdef (long index, long nc, long len, unsigned long *table, long np, float *props)
{
}


void
tlutbind (long target, long index)
{
}
