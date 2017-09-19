/*
 * light.c
 *
 * lighting control
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


#define DBG(x)          //x

#if 0
____________________________ IGL helpers ____________________________
#endif
/* set material, light, or model entry to default values */
void
_igl_setLmDefaults (short deftype, void *ptr)
{
    static igl_lmMaterialDefT defaultMaterialDef =
    {
        -1,
        IGL_LMMATERIALFLAGS_CREATED,
        { 0.2f, 0.2f, 0.2f, 1.0f },     /* ambient */
        { 0.0f, 127.5f, 255.0f, 1.0f }, /* colorIndexes */
        { 0.8f, 0.8f, 0.8f, 1.0f },     /* diffuse (+ alpha) */
        { 0.0f, 0.0f, 0.0f, 1.0f },     /* emission */
        0.0f,                           /* shininess */
        { 0.0f, 0.0f, 0.0f, 1.0f }      /* specular */
    };
    static igl_lmLightDefT defaultLightDef =
    {
        -1,
        IGL_LMLIGHTFLAGS_CREATED,
        { 0.0f, 0.0f, 0.0f, 1.0f },     /* ambient */
        { 1.0f, 1.0f, 1.0f, 1.0f },     /* lcolor */
        { 0.0f, 0.0f, 1.0f, 0.0f },     /* position */
        { 0.0f, 0.0f, -1.0f, 0.0f },    /* spotDirection */
        { 0.0f, 180.0f },               /* spotLight */
        -1
    };
    static igl_lmLmodelDefT defaultLmodelDef =
    {
        -1,
        IGL_LMMODELFLAGS_CREATED,
        { 0.2f, 0.2f, 0.2f, 1.0f },     /* ambient */
        { 1.0f, 0.0f },                 /* attenuation */
        0.0f,                           /* attenuation2 */
        0.0f,                           /* localViewer */
        0.0f                            /* twoSide */
    };
    short id;

    switch (deftype)
    {
        case DEFMATERIAL:
            id = ((igl_lmMaterialDefT *)ptr)->id;
            memcpy (ptr, &defaultMaterialDef, sizeof (defaultMaterialDef));
            ((igl_lmMaterialDefT *)ptr)->id = id;
        break;

        case DEFLIGHT:
            id = ((igl_lmLightDefT *)ptr)->id;
            memcpy (ptr, &defaultLightDef, sizeof (defaultLightDef));
            ((igl_lmLightDefT *)ptr)->id = id;
        break;

        case DEFLMODEL:
            id = ((igl_lmLmodelDefT *)ptr)->id;
            memcpy (ptr, &defaultLmodelDef, sizeof (defaultLmodelDef));
            ((igl_lmLmodelDefT *)ptr)->id = id;
        break;
    }
}


/* parse IrisGL material definition */
static void
_igl_parseLmMaterial (short index, short np, float *lmdefs)
{
    int i;
    igl_lmMaterialDefT *lmptr;

    lmptr = &igl->materialDefs[index];
    if (!(lmptr->flags & IGL_LMMATERIALFLAGS_CREATED))
        _igl_setLmDefaults (DEFMATERIAL, lmptr);

    if (lmdefs == NULL)
        return;

    for (i=0; lmdefs[i] != LMNULL && (np == 0 || i < np); i++)
    {
        switch ((int)lmdefs[i])
        {
            case ALPHA: /* = 4th parameter to diffuse */
DBG(printf("alpha %f\n", lmdefs[i+1]));
                lmptr->flags |= IGL_LMMATERIALFLAGS_DIFFUSE;
                /* TODO: toogl suggests alpha for all of these? */
                lmptr->ambient[3] = lmptr->diffuse[3] = lmdefs[i+1];
                lmptr->emission[3] = lmptr->specular[3] = lmdefs[i+1];
                i ++;
            break;

            case AMBIENT:
DBG(printf("ambient %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3]));
                lmptr->flags |= IGL_LMMATERIALFLAGS_AMBIENT;
                IGL_V3COPY (&lmptr->ambient[0], &lmdefs[i+1]);
                i += 3;
            break;

            case COLORINDEXES:
DBG(printf("colidx %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3]));
                lmptr->flags |= IGL_LMMATERIALFLAGS_COLORINDEXES;
                /* ignored when in RGBA mode! */
                IGL_V3COPY (&lmptr->colorIndexes[0], &lmdefs[i+1]);
                i += 3;
            break;

            case DIFFUSE:
DBG(printf("diffuse %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3]));
                lmptr->flags |= IGL_LMMATERIALFLAGS_DIFFUSE;
                IGL_V3COPY (&lmptr->diffuse[0], &lmdefs[i+1]);
                i += 3;
            break;

            case EMISSION:
DBG(printf("emission %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3]));
                lmptr->flags |= IGL_LMMATERIALFLAGS_EMISSION;
                IGL_V3COPY (&lmptr->emission[0], &lmdefs[i+1]);
                i += 3;
            break;

            case SHININESS:
DBG(printf("shininess %f\n", lmdefs[i+1]));
                lmptr->flags |= IGL_LMMATERIALFLAGS_SHININESS;
                lmptr->shininess = lmdefs[i+1];
                i ++;
            break;

            case SPECULAR:
DBG(printf("specular %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3]));
                lmptr->flags |= IGL_LMMATERIALFLAGS_SPECULAR;
                IGL_V3COPY (&lmptr->specular[0], &lmdefs[i+1]);
                i += 3;
            break;
        }
    }
}


/* set OpenGL material definition */
static void
_igl_setLmMaterial (short target, short index)
{
    igl_lmMaterialDefT *lmptr = &igl->materialDefs[index];
    int ogltype = (target == MATERIAL) ? GL_FRONT : GL_BACK;
    GLfloat props[4];
    unsigned long rgb;

    if (index <= 0 || index >= IGL_MAXLMDEFS)
        return;

    if (IGL_CTX ()->flags & IGL_WFLAGS_RGBA)
    {
        glMaterialfv (ogltype, GL_AMBIENT, lmptr->ambient);
        glMaterialfv (ogltype, GL_DIFFUSE, lmptr->diffuse);
        glMaterialfv (ogltype, GL_EMISSION, lmptr->emission);
        if (lmptr->shininess >= 1)
            glMaterialfv (ogltype, GL_SPECULAR, lmptr->specular);
        else
        {
            props[0] = props[1] = props[2] = props[3] = 0;
            glMaterialfv (ogltype, GL_SPECULAR, props);
        }
    }
    else
    {
        /* IGL has fake colormap mode, so we adjust RGB ambient/diffuse/specular */

#define BTF(_b) (((BYTE)(_b))/255.0f)        /* byte to float (0..255 to 0..+1.0) */
        rgb = IGL_CMAP2ABGR (IGL_CTX (), (long)lmptr->colorIndexes[0]);
        props[0] = BTF (GetRValue (rgb));
        props[1] = BTF (GetGValue (rgb));
        props[2] = BTF (GetBValue (rgb));
        props[3] = lmptr->ambient[3];
        glMaterialfv (ogltype, GL_AMBIENT, props);

        rgb = IGL_CMAP2ABGR (IGL_CTX (), (long)lmptr->colorIndexes[1]);
        props[0] = BTF (GetRValue (rgb));
        props[1] = BTF (GetGValue (rgb));
        props[2] = BTF (GetBValue (rgb));
        props[3] = lmptr->diffuse[3];
        glMaterialfv (ogltype, GL_DIFFUSE, props);

        if (lmptr->shininess >= 1)
        {
            rgb = IGL_CMAP2ABGR (IGL_CTX (), (long)lmptr->colorIndexes[2]);
            props[0] = BTF (GetRValue (rgb));
            props[1] = BTF (GetGValue (rgb));
            props[2] = BTF (GetBValue (rgb));
            props[3] = lmptr->specular[3];
        }
        else
            props[0] = props[1] = props[2] = props[3] = 0;
        glMaterialfv (ogltype, GL_SPECULAR, props);
#undef BTF
    }

    glMaterialf (ogltype, GL_SHININESS, lmptr->shininess);
}


/* parse IrisGL light definition */
static void
_igl_parseLmLight (short index, short np, float *lmdefs)
{
    igl_lmLightDefT *lmptr;
    int i;

    lmptr = &igl->lightDefs[index];
    if (!(lmptr->flags & IGL_LMLIGHTFLAGS_CREATED))
        _igl_setLmDefaults (DEFLIGHT, lmptr);

    if (lmdefs == NULL)
        return;

    for (i=0; lmdefs[i] != LMNULL && (np == 0 || i < np); i++)
    {
        switch ((int)lmdefs[i])
        {
            case AMBIENT:
DBG(printf("ambient %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3]));
                lmptr->flags |= IGL_LMLIGHTFLAGS_AMBIENT;
                IGL_V3COPY (&lmptr->ambient[0], &lmdefs[i+1]);
                i += 3;
            break;

            case LCOLOR:
DBG(printf("lcolor %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3]));
                lmptr->flags |= IGL_LMLIGHTFLAGS_LCOLOR;
                IGL_V3COPY (&lmptr->lcolor[0], &lmdefs[i+1]);
                i += 3;
            break;

            case POSITION:
DBG(printf("position %f %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3], lmdefs[i+4]));
                lmptr->flags |= IGL_LMLIGHTFLAGS_POSITION;
                IGL_V4COPY (&lmptr->position[0], &lmdefs[i+1]);
                i += 4;
            break;

            case SPOTDIRECTION:
DBG(printf("spotdir %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3]));
                lmptr->flags |= IGL_LMLIGHTFLAGS_SPOTDIRECTION;
                IGL_V3COPY (&lmptr->spotDirection[0], &lmdefs[i+1]);
                i += 3;
            break;

            case SPOTLIGHT:
DBG(printf("spotlight %f %f\n", lmdefs[i+1], lmdefs[i+2]));
                lmptr->flags |= IGL_LMLIGHTFLAGS_SPOTLIGHT;
                IGL_V2COPY (&lmptr->spotLight[0], &lmdefs[i+1]);
                i += 2;
            break;
        }
    }
}


/* set OpenGL light definition */
static void
_igl_setLmLight (short target, short index)
{
    igl_lmLightDefT *lmptr = &igl->lightDefs[index];
    int oglLight;
    GLfloat props[4];

    if (index < 0 || index >= IGL_MAXLMDEFS)
        return;

    oglLight = GL_LIGHT0 + (target - LIGHT0);

    if (index)
    {
        if (IGL_CTX ()->flags & IGL_WFLAGS_RGBA)
        {
            glLightfv (oglLight, GL_SPECULAR, lmptr->lcolor);
            glLightfv (oglLight, GL_DIFFUSE, lmptr->lcolor);
            glLightfv (oglLight, GL_AMBIENT, lmptr->ambient);
            glLightfv (oglLight, GL_SPOT_DIRECTION, lmptr->spotDirection);
            glLightf (oglLight, GL_SPOT_EXPONENT, lmptr->spotLight[0]);
            glLightf (oglLight, GL_SPOT_CUTOFF, lmptr->spotLight[1]);
        }
        else
        {
            /* "each light source contributes its full intensity" */
            props[0] = props[1] = props[2] = 1.0f; props[3] = 1.0f;
            glLightfv (oglLight, GL_SPECULAR, props);
            glLightfv (oglLight, GL_DIFFUSE, props);
            glLightfv (oglLight, GL_AMBIENT, props);
            glLightf (oglLight, GL_SPOT_CUTOFF, 180.0f);
        }
        glLightfv (oglLight, GL_POSITION, lmptr->position);

        glEnable (oglLight);
    }
    else
        glDisable (oglLight);
}


/* parse IrisGL light model definition */
static void
_igl_parseLmLightModel (short index, short np, float *lmdefs)
{
    igl_lmLmodelDefT *lmptr;
    int i;

    /* if new entry, initialize with the defaults */
    lmptr = &igl->lmodelDefs[index];
    if (!(lmptr->flags & IGL_LMMODELFLAGS_CREATED))
        _igl_setLmDefaults (DEFLMODEL, lmptr);

    if (lmdefs == NULL)
        return;

    for (i=0; lmdefs[i] != LMNULL && (np == 0 || i < np); i++)
    {
        switch ((int)lmdefs[i])
        {
            case AMBIENT:
DBG(printf("ambient %f %f %f\n", lmdefs[i+1], lmdefs[i+2], lmdefs[i+3]));
                lmptr->flags |= IGL_LMMODELFLAGS_AMBIENT;
                IGL_V3COPY (&lmptr->ambient[0], &lmdefs[i+1]);
                i += 3;
            break;

            case ATTENUATION:
DBG(printf("attenuation %f %f\n", lmdefs[i+1], lmdefs[i+2]));
                lmptr->flags |= IGL_LMMODELFLAGS_ATTENUATION;
                IGL_V2COPY (&lmptr->attenuation[0], &lmdefs[i+1]);
                i += 2;
            break;

            case ATTENUATION2:
DBG(printf("attenuation2 %f\n", lmdefs[i+1]));
                lmptr->flags |= IGL_LMMODELFLAGS_ATTENUATION2;
                lmptr->attenuation2 = lmdefs[i+1];
                i ++;
            break;

            case LOCALVIEWER:
DBG(printf("localview %f\n", lmdefs[i+1]));
                lmptr->flags |= IGL_LMMODELFLAGS_LOCALVIEWER;
                lmptr->localViewer = (lmdefs[i+1] != 0.0f) ? 1.0f : 0.0f;
                i ++;
            break;

            case TWOSIDE:
DBG(printf("twoside %f\n", lmdefs[i+1]));
                lmptr->flags |= IGL_LMMODELFLAGS_TWOSIDE;
                lmptr->twoSide = (lmdefs[i+1] != 0.0f) ? 1.0f : 0.0f;
                i ++;
            break;
        }
    }
}


/* set OpenGL light model definition */
static void
_igl_setLmLightModel (short target, short index)
{
    igl_lmLmodelDefT *lmptr = &igl->lmodelDefs[index];
    GLfloat props[4];
    int i;

    if (index <= 0 || index >= IGL_MAXLMDEFS)
        return;

    /*
        NOTE: ATTENUATION[2] specifies the attenuation factors for ALL
        non-infinite light sources - those whose w-coordinate differs from 0.0.
        as OpenGL doesn't support this with one API call, we change all lights.
    */
    if (IGL_CTX ()->flags & IGL_WFLAGS_RGBA)
    {
        glLightModelfv (GL_LIGHT_MODEL_AMBIENT, lmptr->ambient);
        glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, lmptr->twoSide);

        for (i=0; i < MAXLIGHTS; i++)
        {
            glLightf (GL_LIGHT0+i, GL_CONSTANT_ATTENUATION, lmptr->attenuation[0]);
            glLightf (GL_LIGHT0+i, GL_LINEAR_ATTENUATION, lmptr->attenuation[1]);
            glLightf (GL_LIGHT0+i, GL_QUADRATIC_ATTENUATION, lmptr->attenuation2);
        }
    }
    else
    {
        props[0] = props[1] = props[2] = 1.0f; props[3] = 1.0f;
        glLightModelfv (GL_LIGHT_MODEL_AMBIENT, props);
        glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, 0);

        for (i=0; i < MAXLIGHTS; i++)
        {
            glLightf (GL_LIGHT0+i, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf (GL_LIGHT0+i, GL_LINEAR_ATTENUATION, 0.0f);
            glLightf (GL_LIGHT0+i, GL_QUADRATIC_ATTENUATION, 0.0f);
        }
    }

    glLightModelf (GL_LIGHT_MODEL_LOCAL_VIEWER, lmptr->localViewer);
}


/* check if lighting is active */
static void
_igl_checkLighting (igl_windowT *wptr)
{
    /* if both MATERIAL and LMODEL indexes are non-zero, we are enlightened */
    if (wptr->materialIndex != 0 && wptr->lmodelIndex != 0)
    {
        glEnable (GL_LIGHTING);
        wptr->state |= IGL_WSTATE_LIGHTING | IGL_VSTATE_LIGHTED;
    }
    else
    {
        glDisable (GL_LIGHTING);
        wptr->state &= ~(IGL_WSTATE_LIGHTING | IGL_VSTATE_LIGHTED);
        wptr->state &= ~(IGL_VSTATE_N3F|IGL_VSTATE_N3FBGN);
    }
}


#if 0
____________________________ light functions ____________________________
#endif
/* GL: lmdef - defines or modifies a material, light source, or lighting model */
void
lmdef (short deftype, short index, short np, float *props)
{
    igl_windowT *wptr;
    int idx;

    _igl_trace (__func__,TRUE,"%d,%d,%d,%p",deftype,index,np,props);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* NOTE: if we're changing a currently bound definition, changes take effect immediately (lmbind!) */
    switch (deftype)
    {
        case DEFMATERIAL:
            idx = IGL_MAPID(igl->materialDefs, IGL_MAXLMDEFS, index, 3);
            if (idx < 0)
                return;

            _igl_parseLmMaterial (idx, np, props);
            if (wptr->materialIndex == idx)
                _igl_setLmMaterial (MATERIAL, idx);
            if (wptr->backMaterialIndex == idx)
                _igl_setLmMaterial (BACKMATERIAL, idx);
        break;

        case DEFLIGHT:
            idx = IGL_MAPID(igl->lightDefs, IGL_MAXLMDEFS, index, 3);
            if (idx < 0)
                return;

            _igl_parseLmLight (idx, np, props);
            if (igl->lightDefs[idx].lightTarget > 0)
                _igl_setLmLight (igl->lightDefs[idx].lightTarget, idx);
        break;

        case DEFLMODEL:
            idx = IGL_MAPID(igl->lmodelDefs, IGL_MAXLMDEFS, index, 3);
            if (idx < 0)
                return;

            _igl_parseLmLightModel (idx, np, props);
            if (wptr->lmodelIndex == idx)
                _igl_setLmLightModel (LMODEL, idx);
        break;
    }

    _igl_checkLighting (wptr);
}


/* GL: lmbind - selects a new material, light source, or lighting model */
void
lmbind (short target, short index)
{
    _igl_trace (__func__,FALSE,"%d,%d",target,index);
    igl->api->lmbind (target, index);
}

void
iglExec_lmbind (short target, short index)
{
    igl_windowT *wptr;
    int idx;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    switch (target)
    {
        case MATERIAL:
            idx = IGL_MAPID(igl->materialDefs, IGL_MAXLMDEFS, index, 0);
            if (idx < 0)
                return;

            _igl_setLmMaterial (target, idx);
            wptr->materialIndex = idx;
        break;
        case BACKMATERIAL:
            idx = IGL_MAPID(igl->materialDefs, IGL_MAXLMDEFS, index, 0);
            if (idx < 0)
                return;

            _igl_setLmMaterial (target, idx);
            wptr->backMaterialIndex = idx;
        break;

        case LIGHT0:
        case LIGHT1:
        case LIGHT2:
        case LIGHT3:
        case LIGHT4:
        case LIGHT5:
        case LIGHT6:
        case LIGHT7:
            idx = IGL_MAPID(igl->lightDefs, IGL_MAXLMDEFS, index, 0);
            if (idx < 0)
                return;

            igl->lightDefs[wptr->lightIndex[target - LIGHT0]].lightTarget = 0;
            _igl_setLmLight (target, idx);
            wptr->lightIndex[target - LIGHT0] = idx;
            igl->lightDefs[idx].lightTarget = (idx ? target : 0);
        break;

        case LMODEL:
            idx = IGL_MAPID(igl->lmodelDefs, IGL_MAXLMDEFS, index, 0);
            if (idx < 0)
                return;

            _igl_setLmLightModel (target, idx);
            wptr->lmodelIndex = idx;
        break;
    }

    _igl_checkLighting (wptr);
}


/* GL: lmcolor - change the effect of color commands while lighting is active */
void
lmcolor (long mode)
{
    _igl_trace (__func__,FALSE,"%ld",mode);
    igl->api->lmcolor (mode);
}

void
iglExec_lmcolor (long mode)
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* TODO: manual states it affects c()/cpack()/RGBcolor() functions only? */
    if (!(wptr->flags & IGL_WFLAGS_RGBA))
        return;

    /* lmcolor() doesn't change properties to BACKMATERIAL */
    wptr->state &= ~IGL_WSTATE_LMCNULL;
    switch (mode)
    {
        case LMC_NULL:
            /* NOTE: OpenGL does not support ignoring color operations on
             * vertices. Store current color and use that in v* operations.
             */
            IGL_V4COPY (wptr->lmcColor, wptr->currentColor);
            wptr->state |= IGL_WSTATE_LMCNULL;

        case LMC_COLOR:
            glDisable (GL_COLOR_MATERIAL);
            wptr->lmcMode = 0;
            return;
        break;

        case LMC_EMISSION:
            glColorMaterial (GL_FRONT, GL_EMISSION);
        break;

        case LMC_AMBIENT:
            glColorMaterial (GL_FRONT, GL_AMBIENT);
        break;

        case LMC_DIFFUSE:
            glColorMaterial (GL_FRONT, GL_DIFFUSE);
        break;

        case LMC_SPECULAR:
            glColorMaterial (GL_FRONT, GL_SPECULAR);
        break;

        case LMC_AD:
            glColorMaterial (GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        break;
    }    

    wptr->lmcMode = mode;
    glEnable (GL_COLOR_MATERIAL);
}


/* GL: fogvertex - specify atmospheric fogging effects */
void
fogvertex (long mode, float *params)
{
    _igl_trace (__func__,FALSE,"%ld,%p",mode,params);
    igl->api->fogvertex (mode, params);
}

void
iglExec_fogvertex (long mode, float *params)
{
    float fogClr[4];
    float odensity;
    float factor = 0;

    IGL_CHECKWNDV ();

    if (params == NULL && mode != FG_OFF && mode != FG_ON)
        return;

    switch (mode)
    {
        case FG_OFF:
            glDisable (GL_FOG);
            break;

        case FG_ON:
            glEnable (GL_FOG);
            break;

        case FG_VTX_EXP:
        case FG_PIX_EXP:
            factor = 5.54F;  /* ln(1/255) */
            glFogi (GL_FOG_MODE, GL_EXP);
            glHint (GL_FOG_HINT, (mode == FG_VTX_EXP) ? GL_FASTEST : GL_NICEST);
            break;

        case FG_VTX_LIN:
        case FG_PIX_LIN:
            glFogi (GL_FOG_MODE, GL_LINEAR);
            glHint (GL_FOG_HINT, (mode == FG_VTX_LIN) ? GL_FASTEST : GL_NICEST);
            break;

        case FG_VTX_EXP2:
        case FG_PIX_EXP2:
            factor = 2.35F;  /* sqrt(ln(1/255)) */
            glFogi (GL_FOG_MODE, GL_EXP2);
            glHint (GL_FOG_HINT, (mode == FG_VTX_EXP2) ? GL_FASTEST : GL_NICEST);
            break;
    }

    /* FG_{VTX|PIX}_EXP[2] expects four params[] - density, r, g, b */
    if (factor)
    {
        odensity = params[0] * factor;
        glFogf (GL_FOG_DENSITY, odensity);

        /* GL_FOG_COLOR requires RGBA color */
        IGL_V3COPY (fogClr, &params[1]);
        fogClr[3] = 1.0;
        glFogfv (GL_FOG_COLOR, fogClr);
    }
    /* and these five - near, far, r, g, b */
    else if (mode == FG_VTX_LIN || mode == FG_PIX_LIN)
    {
        glFogf (GL_FOG_START, params[0]);
        glFogf (GL_FOG_END, params[1]);

        /* GL_FOG_COLOR requires RGBA color */
        IGL_V3COPY (fogClr, &params[2]);
        fogClr[3] = 1.0;
        glFogfv (GL_FOG_COLOR, fogClr);
    }
}


/* GL: depthcue - turns depth-cue mode on and off */
void
depthcue (Boolean enable)
{
    _igl_trace (__func__,FALSE,"%d",enable);
    igl->api->depthcue (enable);
}

void
iglExec_depthcue (Boolean enable)
{
    IGL_CHECKWNDV ();

    /* (un)set the linear fog (used for depth-cueing) */
    if (enable)
    {
        glFogi (GL_FOG_MODE, GL_LINEAR);
        glHint (GL_FOG_HINT, GL_NICEST);
        glEnable (GL_FOG);
    }
    else
        glDisable (GL_FOG);
}


/* GL: getdcm - indicates whether depth-cue mode is on or off */
Boolean
getdcm (void)
{
    int cm;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (FALSE);

    glGetIntegerv (GL_FOG_MODE, &cm);
    return (cm == GL_LINEAR && glIsEnabled(GL_FOG));
}


/* GL: lshaderange - sets range of color indices used for depth-cueing */
void
lshaderange (Colorindex lowin, Colorindex highin, long znear, long zfar)
{
    _igl_trace (__func__,FALSE,"%d,%d,%ld,%ld",lowin,highin,znear,zfar);
    igl->api->lshaderange (lowin, highin, znear, zfar);
}

void
iglExec_lshaderange (Colorindex lowin, Colorindex highin, long znear, long zfar)
{
    int clr, hlum, llum, shift = 8*(sizeof(long)-sizeof(Byte)) - 1;
    static GLdouble ident[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    GLdouble tznear = (double)znear / igl->gdZMAX, tzfar = (double)zfar / igl->gdZMAX;
    GLdouble x, y, tx, ty;
    GLint fogClr[4];
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* OpenGL linear fog operates in eye space coordinates, while GL depthcue
     * uses screen z. Unproject z values using identity as modelview matrix.
     * NOTE: not working in MSINGLE, since there's no accessible eye space :-/
     */
    _igl_getViewport ();
    _igl_getMatrices ();
    x = (double)wptr->viewport[0] + wptr->viewport[2] / 2;
    y = (double)wptr->viewport[1] + wptr->viewport[3] / 2;
    gluUnProject (x, y, tznear, ident, wptr->projMatrix, wptr->viewport, &tx, &ty, &tznear);
    gluUnProject (x, y, tzfar,  ident, wptr->projMatrix, wptr->viewport, &tx, &ty, &tzfar);

    /* NOTE: OpenGL only has a single fog color. Choose whatever color is
     * brighter as the fog color, and swap near and far accordingly.
     */
    hlum = IGL_CMAP2ABGR (wptr, highin);
    llum = IGL_CMAP2ABGR (wptr, lowin);
    hlum = 3*GetRValue(hlum) + 6*GetGValue(hlum) + 1*GetBValue(hlum);
    llum = 3*GetRValue(llum) + 6*GetGValue(llum) + 1*GetBValue(llum);
    if (hlum >= llum)
    {
        /* NOTE: gluUnProject returns -z */
        glFogf (GL_FOG_START, -tzfar);
        glFogf (GL_FOG_END, -tznear);
        clr = IGL_CMAP2ABGR (wptr, highin);
    }
    else
    {
        glFogf (GL_FOG_START, -tznear);
        glFogf (GL_FOG_END, -tzfar);
        clr = IGL_CMAP2ABGR (wptr, lowin);
    }

    fogClr[0] = GetRValue (clr) << shift;
    fogClr[1] = GetGValue (clr) << shift;
    fogClr[2] = GetBValue (clr) << shift;
    fogClr[3] = INT_MAX;
    glFogiv (GL_FOG_COLOR, fogClr);
}


/* GL: shaderange - obsolete routine */
void
shaderange (Colorindex lowin, Colorindex highin, Screencoord z1, Screencoord z2)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",lowin,highin,z1,z2);
    igl->api->lshaderange (lowin, highin, (long)z1, (long)z2);
}


/* GL: lRGBrange - sets the range of RGB colors used for depth-cueing */
void
lRGBrange (short rmin, short gmin, short bmin, short rmax, short gmax, short bmax, long znear, long zfar)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d,%d,%d,%ld,%ld",rmin,bmin,gmin,rmax,gmax,bmax,znear,zfar);
    igl->api->lRGBrange (rmin, gmin, bmin, rmax, gmax, bmax, znear, zfar);
}

void
iglExec_lRGBrange (short rmin, short gmin, short bmin, short rmax, short gmax, short bmax, long znear, long zfar)
{
    int hlum, llum, shift = 8*(sizeof(long)-sizeof(Byte)) - 1;
    static GLdouble ident[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    GLdouble tznear = (double)znear / igl->gdZMAX, tzfar = (double)zfar / igl->gdZMAX;
    GLdouble x, y, tx, ty;
    GLint fogClr[4];
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* OpenGL linear fog operates in eye space coordinates, while GL depthcue
     * uses screen z. Unproject z values using identity as modelview matrix.
     * NOTE: not working in MSINGLE, since there's no accessible eye space :-/
     */
    _igl_getViewport ();
    _igl_getMatrices ();
    x = (double)wptr->viewport[0] + wptr->viewport[2] / 2;
    y = (double)wptr->viewport[1] + wptr->viewport[3] / 2;
    gluUnProject (x, y, tznear, ident, wptr->projMatrix, wptr->viewport, &tx, &ty, &tznear);
    gluUnProject (x, y, tzfar,  ident, wptr->projMatrix, wptr->viewport, &tx, &ty, &tzfar);

    /* NOTE: OpenGL only has a single fog color. Choose whatever color is
     * brighter as the fog color, and swap near and far accordingly.
     */
    hlum = 3*rmax + 6*gmax + 1*bmax;
    llum = 3*rmin + 6*gmin + 1*bmin;
    if (hlum >= llum)
    {
        /* NOTE: gluUnProject returns -z */
        glFogf (GL_FOG_START, -tzfar);
        glFogf (GL_FOG_END, -tznear);
        fogClr[0] = rmax << shift;
        fogClr[1] = gmax << shift;
        fogClr[2] = bmax << shift;
    }
    else
    {
        glFogf (GL_FOG_START, -tznear);
        glFogf (GL_FOG_END, -tzfar);
        fogClr[0] = rmin << shift;
        fogClr[1] = gmin << shift;
        fogClr[2] = bmin << shift;
    }

    fogClr[3] = INT_MAX;
    glFogiv (GL_FOG_COLOR, fogClr);
}


/* GL: RGBrange - obsolete routine */
void
RGBrange (short rmin, short bmin, short gmin, short rmax, short gmax, short bmax, Screencoord znear, Screencoord zfar)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d,%d,%d,%d,%d",rmin,bmin,gmin,rmax,gmax,bmax,znear,zfar);
    igl->api->lRGBrange (rmin, gmin, bmin, rmax, gmax, bmax, znear, zfar);
}


/* GL: shademodel - selects the shading model */
void
shademodel (int mode)
{
    _igl_trace (__func__,FALSE,"%d",mode);
    igl->api->shademodel (mode);
}

void
iglExec_shademodel (int mode)
{
    IGL_CHECKWNDV ();

    glShadeModel (mode == FLAT ? GL_FLAT : GL_SMOOTH);
}


/* GL: getsm - returns the current shading model */
long
getsm (void)
{
    int sm;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (GOURAUD);

    glGetIntegerv (GL_SHADE_MODEL, &sm);
    return (sm == GL_FLAT ? FLAT : GOURAUD);
}
