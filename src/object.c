/*
 * object.c
 *
 * GL object managment
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


#define IGL_OBJTAG_RANGE    2147483647      /* INT_MAX or MAX_INT */


/*********************************************************************
 to add a function named MYFUNC, change the following:

 iglobj.h:
	- add an enum to igl_opcodeT named OP_MYFUNC
	- declare functions iglExec_myfunc () and iglObj_myfunc (void)

 iglcmn.h:
	- add myfunc to igl_apiList struct

 object.c:
	- add case OP_MYFUNC to _igl_callFunc () and optional case OP_MYFUNC to _igl_deleteObjectFcn (void)

 objapi.c:
	- define function iglObj_myfunc (), which adds to the object (see e.g. iglObj_clear())
	- add API(myfunc) to _igl_initObjectApi.

 <appropriate .c file>:
	- define function myfunc (), which runs igl->api->myfunc
	- define function iglExec_myfunc (), which does the job
 *********************************************************************/


#define IGL_OBJDEF_TAG      1
#define IGL_OBJDEF_FCN      2


typedef struct
{
    char type;
    int id;
} igl_objTagT;


#   define _igl_printf printf

#if 0
static void
_igl_dumpObject (igl_objectT *obj)
{
    char *ptr;
    igl_objTagT *tagPtr;
    igl_objFcnT *fcnPtr;
    int idx;

    _igl_printf ("obj ptr=%p id=%ld chunkSize=%ld chunk=%p beginPtr=%p curPtr=%p endPtr=%p\n",
        obj, obj->id, obj->chunkSize, obj->chunk, obj->beginPtr, obj->curPtr, obj->endPtr);
    if (obj->chunk == NULL)
    {
        _igl_printf ("\n");
        return;
    }

    idx = 0;
    ptr = obj->chunk;
    while (1)
    {
        switch (*ptr)
        {
            case IGL_OBJDEF_TAG:
                tagPtr = (igl_objTagT *)ptr;
                _igl_printf ("\t[%d]TAG id=%d\n", idx, tagPtr->id);
                idx ++;
                ptr += sizeof (igl_objTagT);
            break;

            case IGL_OBJDEF_FCN:
                fcnPtr = (igl_objFcnT *)ptr;
                _igl_printf ("\t[%d]FCN opcode=%d\n", idx, fcnPtr->opcode);
                idx ++;
                ptr += sizeof (igl_objFcnT);
            break;
        }

        if (ptr > obj->endPtr)
        {
            _igl_printf ("\n");
            break;
        }
    }
}
#endif


#define FREEPARAM(_idx) if (fcn->params[_idx].v != NULL) free (fcn->params[_idx].v);
static void
_igl_deleteObjectFcn (igl_objFcnT *fcn)
{
    switch (fcn->opcode)
    {
        case OP_CALLFUNC:
        case OP_CLIPPLANE:
        case OP_TEXGEN:
            FREEPARAM (2);
        break;

        case OP_FOGVERTEX:
        case OP_LCHARSTR:
        case OP_POLF:
        case OP_POLFI:
        case OP_POLFS:
        case OP_POLF2:
        case OP_POLF2I:
        case OP_POLF2S:
        case OP_POLY:
        case OP_POLYI:
        case OP_POLYS:
        case OP_POLY2:
        case OP_POLY2I:
        case OP_POLY2S:
        case OP_PWLCURVE:
        case OP_CRVN:
        case OP_RCRVN:
            FREEPARAM (1);
        break;

        case OP_C4F:
        case OP_C4I:
        case OP_C4S:
        case OP_V4F:
        case OP_N3F:
        case OP_T4F:
        case OP_LOADMATRIX:
        case OP_MULTMATRIX:
            FREEPARAM (0);
        break;

        case OP_NURBSCURVE:
            FREEPARAM (1);
            FREEPARAM (3);
        break;

        case OP_NURBSSURFACE:
            FREEPARAM (1);
            FREEPARAM (3);
            FREEPARAM (6);
        break;

        case OP_RPATCH:
            FREEPARAM (3);
        case OP_PATCH:
            FREEPARAM (0);
            FREEPARAM (1);
            FREEPARAM (2);
        break;

        case OP_SPLF:
        case OP_SPLFI:
        case OP_SPLFS:
        case OP_SPLF2:
        case OP_SPLF2I:
        case OP_SPLF2S:
            FREEPARAM (1);
            FREEPARAM (2);
        break;

        default:
        break;
    }
}


static igl_objectT *
_igl_findObject (Object id, int *idx)
{
    int i;

    for (i=0; i < igl->numAllocatedObjects; i++)
    {
        if (igl->objects[i].id == id)
        {
            if (idx != NULL)
                *idx = i;
            return (&igl->objects[i]);
        }
    }

    if (idx != NULL)
        *idx = -1;

    return (NULL);
}


static igl_objectT *
_igl_allocObject (int *idx)
{
    igl_objectT *objects;

    /* try to find an "empty" object in the list */
    _igl_findObject (0, idx);
    if (*idx == -1)
    {
        /* if we don't have room, realloc the list */
        if (igl->numObjects + 1 >= igl->numAllocatedObjects)
        {
            /* remember old ptr in case of realloc() failure */
            objects = igl->objects;

            igl->objects = (igl_objectT *)realloc (igl->objects,
                (igl->numAllocatedObjects + 100)*sizeof (igl_objectT));
            if (igl->objects == NULL)
            {
                igl->objects = objects;
                return (NULL);
            }

            igl->numAllocatedObjects += 100;
        }

        igl->numObjects ++;
        *idx = igl->numObjects-1;
    }
    else
    {
        igl->numObjects ++;
    }

    return (&igl->objects[*idx]);
}


static void
_igl_deleteObject (igl_objectT *obj)
{
    char *ptr;

    if (obj->chunk != NULL)
    {
        ptr = obj->beginPtr + sizeof (igl_objTagT);
        while (ptr != obj->endPtr)
        {
            switch (*ptr)
            {
                case IGL_OBJDEF_TAG:
                    ptr += sizeof (igl_objTagT);
                break;

                case IGL_OBJDEF_FCN:
                    _igl_deleteObjectFcn ((igl_objFcnT *)ptr);
                    ptr += sizeof (igl_objFcnT);
                break;
            }
        }

        free (obj->chunk);
        obj->chunk = NULL;
    }
    obj->id = 0;
    obj->chunkSize = 0;
    obj->beginPtr = obj->curPtr = obj->endPtr = NULL;
    igl->numObjects --;
}


int
_igl_initObjects (void)
{
    igl->objects = (igl_objectT *)calloc (100, sizeof (igl_objectT));
    if (igl->objects == NULL)
        return -1;

    igl->numAllocatedObjects = 100;
    igl->numObjects = 0;
    igl->currentObjIdx = -1;
    igl->objChunkSize = IGL_DEFOBJCHUNKSIZE;    /* 1020 bytes was IRIX default */

    return 0;
}


void
_igl_releaseObjects (void)
{
    int i;

    for (i=0; i < igl->numAllocatedObjects; i++)
    {
        if (igl->objects[i].id != 0)
            _igl_deleteObject (&igl->objects[i]);
    }

    if (igl->objects != NULL)
        free (igl->objects);
    igl->objects = NULL;
    igl->numAllocatedObjects = 0;
}


static int
_igl_reallocObjectChunk (igl_objectT *obj, int newSize)
{
    int curDiff, endDiff;
    char *chunk = obj->chunk;

    curDiff = obj->curPtr - obj->beginPtr;
    endDiff = obj->endPtr - obj->beginPtr;

    obj->chunk = realloc (obj->chunk, newSize);
    if (obj->chunk == NULL)
    {
        obj->chunk = chunk;
        return (-1);
    }
    obj->beginPtr = obj->chunk;
    obj->curPtr = obj->beginPtr + curDiff;
    obj->endPtr = obj->beginPtr + endDiff;
    obj->chunkSize = newSize;

    return (0);
}


static igl_objTagT *
_igl_findObjectTag (igl_objectT *obj, Tag t)
{
    igl_objTagT *tagPtr;
    char *ptr;


    if (t == STARTTAG)
        return ((igl_objTagT *)obj->beginPtr);
    else if (t == ENDTAG)
        return ((igl_objTagT *)obj->endPtr);

    ptr = obj->beginPtr + sizeof (igl_objTagT);
    while (ptr != obj->endPtr)
    {
        switch (*ptr)
        {
            case IGL_OBJDEF_TAG:
                tagPtr = (igl_objTagT *)ptr;
                if (tagPtr->id == t)
                    return (tagPtr);
                ptr += sizeof (igl_objTagT);
            break;

            case IGL_OBJDEF_FCN:
                ptr += sizeof (igl_objFcnT);
            break;
        }
    }

    return (NULL);
}


static void
_igl_addObjectTag (igl_objectT *obj, Tag t, Boolean addCurrent)
{
    igl_objTagT *tag;

    /* check whether we need to resize object's chunk (as we must insert a tag) */
    if ((obj->endPtr + sizeof (igl_objTagT)) >= (obj->chunk + obj->chunkSize))
    {
        if (_igl_reallocObjectChunk (obj, obj->chunkSize + igl->objChunkSize) != 0)
            return;
    }

    memmove (obj->curPtr + sizeof (igl_objTagT), obj->curPtr, obj->endPtr + sizeof (igl_objTagT) - obj->curPtr);
    obj->endPtr += sizeof (igl_objTagT);
    /* create a new tag in the middle */
    tag = (igl_objTagT *)obj->curPtr;
    tag->type = IGL_OBJDEF_TAG;
    tag->id = t;

    obj->curPtr += sizeof (igl_objTagT);

#if 0
    _igl_printf ("_igl_addObjectTag (%d)\n", t);
    _igl_dumpObject (obj);
#endif
}


static void
_igl_deleteObjectTag (igl_objectT *obj, igl_objTagT *tag)
{
    char *srcPtr;

    srcPtr = (char *)tag + sizeof (igl_objTagT);
    memmove ((char *)tag, srcPtr, obj->endPtr + sizeof (igl_objTagT) - srcPtr);
    obj->endPtr -= sizeof (igl_objTagT);
    obj->curPtr = obj->endPtr;

#if 0
    _igl_printf ("_igl_deleteObjectTag (%d)\n", tag->id);
    _igl_dumpObject (obj);
#endif
}


igl_objFcnT *
_igl_addObjectFcn (igl_objectT *obj, int opcode)
{
    igl_objFcnT *fcn;

    /* check whether we need to resize object's chunk (as we must insert a tag) */
    if ((obj->endPtr + sizeof (igl_objFcnT)) >= (obj->chunk + obj->chunkSize))
    {
        if (_igl_reallocObjectChunk (obj, obj->chunkSize + igl->objChunkSize) != 0)
            return (NULL);
    }

    memmove (obj->curPtr + sizeof (igl_objFcnT), obj->curPtr, obj->endPtr + sizeof (igl_objTagT) - obj->curPtr);
    obj->endPtr += sizeof (igl_objFcnT);
    /* create a new tag in the middle */
    fcn = (igl_objFcnT *)obj->curPtr;
    fcn->type = IGL_OBJDEF_FCN;
    fcn->opcode = opcode;

    obj->curPtr += sizeof (igl_objFcnT);

#if 0
    _igl_printf ("_igl_addObjectFcn (%d)\n", opcode);
    _igl_dumpObject (obj);
#endif

    return (fcn);
}


static void
_igl_callFunc (igl_objFcnT *fcn)
{
    switch (fcn->opcode)
    {
        case OP_CLEAR:
            iglExec_clear ();
        break;

        case OP_LSETDEPTH:
            iglExec_lsetdepth (fcn->params[0].i, fcn->params[1].i);
        break;

        case OP_SWAPBUFFERS:
            iglExec_swapbuffers ();
        break;

        case OP_MSWAPBUFFERS:
            iglExec_mswapbuffers (fcn->params[0].i);
        break;

        case OP_BACKBUFFER:
            iglExec_backbuffer (fcn->params[0].i);
        break;

        case OP_FRONTBUFFER:
            iglExec_frontbuffer (fcn->params[0].i);
        break;

        case OP_LEFTBUFFER:
            iglExec_leftbuffer (fcn->params[0].i);
        break;

        case OP_RIGHTBUFFER:
            iglExec_rightbuffer (fcn->params[0].i);
        break;

        case OP_BACKFACE:
            iglExec_backface (fcn->params[0].i);
        break;

        case OP_FRONTFACE:
            iglExec_frontface (fcn->params[0].i);
        break;

        case OP_DEPTHCUE:
            iglExec_depthcue (fcn->params[0].i);
        break;

        case OP_LSHADERANGE:
            iglExec_lshaderange (fcn->params[0].us, fcn->params[1].us, fcn->params[2].i, fcn->params[3].i);
        break;

        case OP_LRGBRANGE:
            iglExec_lRGBrange (fcn->params[0].us, fcn->params[1].us, fcn->params[2].us, fcn->params[3].us, fcn->params[4].us, fcn->params[5].us, fcn->params[6].i, fcn->params[7].i);
        break;

        case OP_ZBUFFER:
            iglExec_zbuffer (fcn->params[0].i);
        break;

        case OP_ZFUNCTION:
            iglExec_zfunction (fcn->params[0].i);
        break;

        case OP_ZCLEAR:
            iglExec_zclear ();
        break;

        case OP_CZCLEAR:
            iglExec_czclear (fcn->params[0].ui, fcn->params[1].i);
        break;

        case OP_SCLEAR:
            iglExec_sclear (fcn->params[0].ui);
        break;

        case OP_STENCIL:
            iglExec_stencil (fcn->params[0].i, fcn->params[1].ui, fcn->params[2].i, fcn->params[3].ui,
                fcn->params[4].i, fcn->params[5].i, fcn->params[6].i);
        break;

        case OP_ACBUF:
            iglExec_acbuf (fcn->params[0].i, fcn->params[1].f);
        break;

        case OP_SHADEMODEL:
            iglExec_shademodel (fcn->params[0].i);
        break;

        case OP_COLOR:
            iglExec_color (fcn->params[0].us);
        break;

        case OP_MAPCOLOR:
            iglExec_mapcolor (fcn->params[0].us, fcn->params[1].s, fcn->params[2].s, fcn->params[3].s);
        break;

        case OP_CPACK:
            iglExec_cpack (fcn->params[0].ui);
        break;

        case OP_RGBCOLOR:
            iglExec_RGBcolor (fcn->params[0].s, fcn->params[1].s, fcn->params[2].s);
        break;

        case OP_AFUNCTION:
            iglExec_afunction (fcn->params[0].i, fcn->params[1].i);
        break;

        case OP_BLENDFUNCTION:
            iglExec_blendfunction (fcn->params[0].i, fcn->params[1].i);
        break;

        case OP_DITHER:
            iglExec_dither (fcn->params[0].i);
        break;

        case OP_FOGVERTEX:
            iglExec_fogvertex (fcn->params[0].i, (float *)fcn->params[0].v);
        break;

        case OP_LOGICOP:
            iglExec_logicop (fcn->params[0].i);
        break;

        case OP_C4F:
            iglExec_c4f ((float *)fcn->params[0].v);
        break;

        case OP_C4I:
            iglExec_c4i ((int *)fcn->params[0].v);
        break;

        case OP_C4S:
            iglExec_c4s ((short *)fcn->params[0].v);
        break;

        case OP_MMODE:
            iglExec_mmode (fcn->params[0].s);
        break;

        case OP_LOADMATRIX:
            iglExec_loadmatrix (fcn->params[0].v);
        break;

        case OP_MULTMATRIX:
            iglExec_multmatrix (fcn->params[0].v);
        break;

        case OP_PUSHMATRIX:
            iglExec_pushmatrix ();
        break;

        case OP_POPMATRIX:
            iglExec_popmatrix ();
        break;

        case OP_ROT:
            iglExec_rot (fcn->params[0].f, fcn->params[1].c);
        break;

        case OP_ROTATE:
            iglExec_rotate (fcn->params[0].s, fcn->params[1].c);
        break;

        case OP_TRANSLATE:
            iglExec_translate (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_SCALE:
            iglExec_scale (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_POLARVIEW:
            iglExec_polarview (fcn->params[0].f, fcn->params[1].s, fcn->params[2].s, fcn->params[3].s);
        break;

        case OP_CLIPPLANE:
            iglExec_clipplane (fcn->params[0].i, fcn->params[1].i, (float *)fcn->params[2].v);
        break;

        case OP_ORTHO:
            iglExec_ortho (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].f,
                fcn->params[4].f, fcn->params[5].f);
        break;

        case OP_ORTHO2:
            iglExec_ortho2 (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].f);
        break;

        case OP_WINDOW:
            iglExec_window (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].f,
                fcn->params[4].f, fcn->params[5].f);
        break;

        case OP_PERSPECTIVE:
            iglExec_perspective (fcn->params[0].s, fcn->params[1].f, fcn->params[2].f, fcn->params[3].f);
        break;

        case OP_VIEWPORT:
            iglExec_viewport (fcn->params[0].s, fcn->params[1].s, fcn->params[2].s, fcn->params[3].s);
        break;

        case OP_PUSHVIEWPORT:
            iglExec_pushviewport ();
        break;

        case OP_POPVIEWPORT:
            iglExec_popviewport ();
        break;

        case OP_LOOKAT:
            iglExec_lookat (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].f,
                fcn->params[4].f, fcn->params[5].f, fcn->params[6].s);
        break;

        case OP_SCRMASK:
            iglExec_scrmask (fcn->params[0].s, fcn->params[1].s, fcn->params[2].s, fcn->params[3].s);
        break;

        case OP_POLF:
            iglExec_polf (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLFI:
            iglExec_polfi (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLFS:
            iglExec_polfs (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLF2:
            iglExec_polf2 (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLF2I:
            iglExec_polf2i (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLF2S:
            iglExec_polf2s (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLY:
            iglExec_poly (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLYI:
            iglExec_polyi (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLYS:
            iglExec_polys (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLY2:
            iglExec_poly2 (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLY2I:
            iglExec_poly2i (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLY2S:
            iglExec_poly2s (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_POLYMODE:
            iglExec_polymode (fcn->params[0].i);
        break;

        case OP_POLYSMOOTH:
            iglExec_polysmooth (fcn->params[0].i);
        break;

        case OP_DISPLACEPOLYGON:
            iglExec_displacepolygon (fcn->params[0].f);
        break;

        case OP_PMV:
            iglExec_pmv (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_PDR:
            iglExec_pdr (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_PCLOS:
            iglExec_pclos ();
        break;

        case OP_PNT:
            iglExec_pnt (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_PNTSIZEF:
            iglExec_pntsizef (fcn->params[0].f);
        break;

        case OP_PNTSMOOTH:
            iglExec_pntsmooth (fcn->params[0].ui);
        break;

        case OP_RECT:
            iglExec_rect (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].f);
        break;

        case OP_RECTF:
            iglExec_rectf (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].f);
        break;

        case OP_SBOX:
            iglExec_sbox (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].f);
        break;

        case OP_SBOXF:
            iglExec_sboxf (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].f);
        break;

        case OP_V4F:
            iglExec_v4f ((float *)fcn->params[0].v);
        break;

        case OP_ARC:
            iglExec_arc (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].s,
                fcn->params[4].s);
        break;

        case OP_ARCF:
            iglExec_arcf (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f, fcn->params[3].s,
                fcn->params[4].s);
        break;

        case OP_CIRC:
            iglExec_circ (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_CIRCF:
            iglExec_circf (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_BGNTMESH:
            iglExec_bgntmesh ();
        break;

        case OP_SWAPTMESH:
            iglExec_swaptmesh ();
        break;

        case OP_BGNCLOSEDLINE:
            iglExec_bgnclosedline ();
        break;

        case OP_BGNCURVE:
            iglExec_bgncurve ();
        break;

        case OP_ENDCURVE:
            iglExec_endcurve ();
        break;

        case OP_NURBSCURVE:
            iglExec_nurbscurve (fcn->params[0].i, fcn->params[1].v, fcn->params[2].i, fcn->params[3].v,
                fcn->params[4].i, fcn->params[5].i);
        break;

        case OP_BGNSURFACE:
            iglExec_bgnsurface ();
        break;

        case OP_ENDSURFACE:
            iglExec_endsurface ();
        break;

        case OP_NURBSSURFACE:
            iglExec_nurbssurface (fcn->params[0].i, fcn->params[1].v, fcn->params[2].i, fcn->params[3].v,
                fcn->params[4].i, fcn->params[5].i, fcn->params[6].v, fcn->params[7].i, fcn->params[8].i,
                fcn->params[9].i);
        break;

        case OP_BGNTRIM:
            iglExec_bgntrim ();
        break;

        case OP_ENDTRIM:
            iglExec_endtrim ();
        break;

        case OP_PWLCURVE:
            iglExec_pwlcurve (fcn->params[0].i, fcn->params[1].v, fcn->params[2].i, fcn->params[3].i);
        break;

        case OP_SETNURBSPROPERTY:
            iglExec_setnurbsproperty (fcn->params[0].i, fcn->params[1].f);
        break;

        case OP_BGNLINE:
            iglExec_bgnline ();
        break;

        case OP_BGNPOINT:
            iglExec_bgnpoint ();
        break;

        case OP_ENDPOINT:
            iglExec_endpoint ();
        break;

        case OP_BGNPOLYGON:
            iglExec_bgnpolygon ();
        break;

        case OP_BGNQSTRIP:
            iglExec_bgnqstrip ();
        break;

        case OP_CURVEIT:
            iglExec_curveit (fcn->params[0].i);
        break;

        case OP_CRVN:
            iglExec_crvn (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_RCRVN:
            iglExec_rcrvn (fcn->params[0].i, fcn->params[1].v);
        break;

        case OP_CURVEPRECISION:
            iglExec_curveprecision (fcn->params[0].i);
        break;

        case OP_CURVEBASIS:
            iglExec_curvebasis (fcn->params[0].i);
        break;

        case OP_PATCH:
            iglExec_patch (fcn->params[0].v, fcn->params[1].v, fcn->params[2].v);
        break;

        case OP_RPATCH:
            iglExec_rpatch (fcn->params[0].v, fcn->params[1].v, fcn->params[2].v,fcn->params[3].v);
        break;

        case OP_PATCHCURVES:
            iglExec_patchcurves (fcn->params[0].i, fcn->params[1].i);
        break;

        case OP_PATCHPRECISION:
            iglExec_patchprecision (fcn->params[0].i, fcn->params[1].i);
        break;

        case OP_PATCHBASIS:
            iglExec_patchbasis (fcn->params[0].i, fcn->params[1].i);
        break;

        case OP_NMODE:
            iglExec_nmode (fcn->params[0].i);
        break;

        case OP_N3F:
            iglExec_n3f ((float *)fcn->params[0].v);
        break;

        case OP_LINESMOOTH:
            iglExec_linesmooth (fcn->params[0].ui);
        break;

        case OP_LINEWIDTHF:
            iglExec_linewidthf (fcn->params[0].f);
        break;

        case OP_SETLINESTYLE:
            iglExec_setlinestyle (fcn->params[0].s);
        break;

        case OP_LSREPEAT:
            iglExec_lsrepeat (fcn->params[0].i);
        break;

        case OP_SETPATTERN:
            iglExec_setpattern (fcn->params[0].s);
        break;

        case OP_LMBIND:
            iglExec_lmbind (fcn->params[0].s, fcn->params[1].s);
        break;

        case OP_LMCOLOR:
            iglExec_lmcolor (fcn->params[0].i);
        break;

        case OP_MOVE:
            iglExec_move (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_RMV:
            iglExec_rmv (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_RPMV:
            iglExec_rpmv (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_DRAW:
            iglExec_draw (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_RDR:
            iglExec_rdr (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_RPDR:
            iglExec_rpdr (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_SPLF:
            iglExec_splf (fcn->params[0].i, fcn->params[1].v, fcn->params[2].v);
        break;

        case OP_SPLFI:
            iglExec_splfi (fcn->params[0].i, fcn->params[1].v, fcn->params[2].v);
        break;

        case OP_SPLFS:
            iglExec_splfs (fcn->params[0].i, fcn->params[1].v, fcn->params[2].v);
        break;

        case OP_SPLF2:
            iglExec_splf2 (fcn->params[0].i, fcn->params[1].v, fcn->params[2].v);
        break;

        case OP_SPLF2I:
            iglExec_splf2i (fcn->params[0].i, fcn->params[1].v, fcn->params[2].v);
        break;

        case OP_SPLF2S:
            iglExec_splf2s (fcn->params[0].i, fcn->params[1].v, fcn->params[2].v);
        break;

        case OP_CALLOBJ:
            iglExec_callobj (fcn->params[0].i);
        break;

        case OP_CALLFUNC:
        {
            long *args = (long *)fcn->params[2].v;
            void (*func)() = (void (*)())fcn->params[0].v;

            /* NOTE: IrisGL limits this to 10 parameters */
            switch ((long)fcn->params[1].i)
            {
            case 0: func(0); break;
            case 1: func(1, args[0]); break;
            case 2: func(2, args[0], args[1]); break;
            case 3: func(3, args[0], args[1], args[2]); break;
            case 4: func(4, args[0], args[1], args[2], args[3]); break;
            case 5: func(5, args[0], args[1], args[2], args[3], args[4]); break;
            case 6: func(6, args[0], args[1], args[2], args[3], args[4],
                            args[5]); break;
            case 7: func(7, args[0], args[1], args[2], args[3], args[4],
                            args[5], args[6]); break;
            case 8: func(8, args[0], args[1], args[2], args[3], args[4],
                            args[5], args[6], args[7]); break;
            case 9: func(9, args[0], args[1], args[2], args[3], args[4],
                            args[5], args[6], args[7], args[8]); break;
            case 10:func(10,args[0], args[1], args[2], args[3], args[4],
                            args[5], args[6], args[7], args[8], args[9]); break;
            default: break;
            }
        }
        break;

        case OP_TEVBIND:
            iglExec_tevbind (fcn->params[0].i, fcn->params[1].i);
        break;

        case OP_TEXBIND:
            iglExec_texbind (fcn->params[0].i, fcn->params[1].i);
        break;

        case OP_TEXGEN:
            iglExec_texgen (fcn->params[0].i, fcn->params[1].i, fcn->params[2].v);
        break;

        case OP_T4F:
            iglExec_t4f ((float *)fcn->params[0].v);
        break;

        case OP_CMOV:
            iglExec_cmov (fcn->params[0].f, fcn->params[1].f, fcn->params[2].f);
        break;

        case OP_LCHARSTR:
            iglExec_lcharstr (fcn->params[0].i, (char *)fcn->params[1].v);
        break;

        case OP_FONT:
            iglExec_font (fcn->params[0].s);
        break;

        case OP_PUSHATTRIBUTES:
            iglExec_pushattributes ();
        break;

        case OP_POPATTRIBUTES:
            iglExec_popattributes ();
        break;

        case OP_INITNAMES:
            iglExec_initnames ();
        break;

        case OP_LOADNAME:
            iglExec_loadname (fcn->params[0].s);
        break;

        case OP_PUSHNAME:
            iglExec_pushname (fcn->params[0].s);
        break;

        case OP_POPNAME:
            iglExec_popname ();
        break;

        default:
        break;
    }
}


#if 0
_____________________________________________________
#endif
Boolean
isobj (Object id)
{
    return ((id > 0 && _igl_findObject (id, NULL) != NULL) ? TRUE : FALSE);
}


Object
genobj (void)
{
    Object id = 1;  /* is 0 a valid object id? */

    while (1)
    {
        if (_igl_findObject (id, NULL) == NULL)
        {
            /* not found in the list, this will be ok */
            return (id);
        }

        id ++;
        if (id >= IGL_OBJTAG_RANGE)
            break;
    }

    return (-1);
}


void
makeobj (Object id)
{
    igl_objectT *obj;
    igl_objTagT *tag;

    /* should we allow makeobj() if no windows are created yet? */
    _igl_trace (__func__,TRUE,"%ld",id);
    IGL_CHECKINIT ();

    /* makeobj() cannot be used within makeobj() */
    if (id <= 0 || igl->currentObjIdx != -1)
        return;

    obj = _igl_findObject (id, &igl->currentObjIdx);
    /* if obj already exists it will be overwritten */
    if (obj != NULL)
        _igl_deleteObject (obj);
    else
        obj = _igl_allocObject (&igl->currentObjIdx);

    if (obj == NULL || igl->currentObjIdx == -1)
        return;

    obj->id = id;

    /* allocate the first chunk for this object */
    obj->chunk = (char *)malloc (igl->objChunkSize);
    memset (obj->chunk, 0, igl->objChunkSize);
    obj->chunkSize = igl->objChunkSize;
    igl->api = igl->objApi;

    /* add start and end tags to the object */
    obj->beginPtr = obj->chunk;
    tag = (igl_objTagT *)obj->beginPtr;
    tag->type = IGL_OBJDEF_TAG;
    tag->id = STARTTAG;

    obj->endPtr = obj->beginPtr + sizeof (igl_objTagT);
    tag = (igl_objTagT *)obj->endPtr;
    tag->type = IGL_OBJDEF_TAG;
    tag->id = ENDTAG;

    obj->curPtr = obj->endPtr;
#if 0
    _igl_printf ("makeobj (%d)\n", id);
    _igl_dumpObject (obj);
#endif
}


void
closeobj (void)
{
    igl_objectT *obj;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    if (igl->currentObjIdx != -1)
    {
        obj = &igl->objects[igl->currentObjIdx];
        obj->curPtr = obj->beginPtr + sizeof (igl_objTagT);
        igl->currentObjIdx = -1;
        igl->api = igl->execApi;
    }
}


void
delobj (Object id)
{
    igl_objectT *obj;

    _igl_trace (__func__,TRUE,"%ld",id);
    IGL_CHECKINIT();

    if (id <= 0)
        return;

    obj = _igl_findObject (id, NULL);
    if (obj != NULL)
        _igl_deleteObject (obj);
}


void
editobj (Object id)
{
    igl_objectT *obj;

    _igl_trace (__func__,TRUE,"%ld",id);
    IGL_CHECKINIT();

    if (igl->currentObjIdx != -1 || id <= 0)
        return;

    obj = _igl_findObject (id, &igl->currentObjIdx);
    if (obj != NULL)
    {
        obj->curPtr = obj->endPtr;
        igl->api = igl->objApi;
    }
}


Object
getopenobj (void)
{
    return (igl->currentObjIdx != -1 ? igl->objects[igl->currentObjIdx].id : -1);
}


void
callobj (Object id)
{
    _igl_trace (__func__,FALSE,"%ld",id);
    igl->api->callobj (id);
}


void
iglExec_callobj (Object id)
{
    char *ptr;
    igl_objectT *obj;

    IGL_CHECKWNDV ();

    if (id <= 0)
        return;

    obj = _igl_findObject (id, NULL);
    if (obj == NULL)
        return;

    ptr = obj->beginPtr + sizeof (igl_objTagT);
    while (ptr != obj->endPtr)
    {
        switch (*ptr)
        {
            case IGL_OBJDEF_TAG:
                ptr += sizeof (igl_objTagT);
            break;

            case IGL_OBJDEF_FCN:
                _igl_callFunc ((igl_objFcnT *)ptr);
                ptr += sizeof (igl_objFcnT);
            break;
        }
    }
}


#if 0
_____________________________________________________
#endif
Tag
gentag (void)
{
    int id = 1;

    if (igl->currentObjIdx == -1)
        return (-1);

    while (1)
    {
        if (_igl_findObjectTag (&igl->objects[igl->currentObjIdx], id) == NULL)
            return (id);

        id ++;
        if (id >= IGL_OBJTAG_RANGE)
            break;
    }

    return (-1);
}


void
maketag (Tag t)
{
    igl_objectT *obj;


    if (igl->currentObjIdx == -1 || t == STARTTAG || t == ENDTAG)
        return;

    obj = &igl->objects[igl->currentObjIdx];
    if (_igl_findObjectTag (obj, t) != NULL)
    {
        /* tag exists within current object, nothing to do here */
        return;
    }
    _igl_addObjectTag (obj, t, TRUE);
}


void
deltag (Tag t)
{
    igl_objTagT *tag;
    igl_objectT *obj;


    if (igl->currentObjIdx == -1 || t == STARTTAG || t == ENDTAG)
        return;

    obj = &igl->objects[igl->currentObjIdx];
    tag = _igl_findObjectTag (obj, t);
    if (tag != NULL)
        _igl_deleteObjectTag (obj, tag);
}


Boolean
istag (Tag t)
{
    if (igl->currentObjIdx == -1)
        return (FALSE);
    else if (t == STARTTAG || t == ENDTAG ||
            _igl_findObjectTag (&igl->objects[igl->currentObjIdx], t) != NULL)
        return (TRUE);

    return (FALSE);
}


void
newtag (Tag newtg, Tag oldtg, Offset offset)
{
    int count;
    igl_objectT *obj;
    igl_objTagT *tag;
    char *ptr;

    /* note: oldtg can be STARTTAG */
    if (igl->currentObjIdx == -1 || newtg == STARTTAG || newtg == ENDTAG || oldtg == ENDTAG)
        return;

    obj = &igl->objects[igl->currentObjIdx];
    if (_igl_findObjectTag (obj, newtg) != NULL)
    {
        /* newtg already exists, bail out */
        return;
    }
    tag = _igl_findObjectTag (obj, oldtg);
    if (tag == NULL)
    {
        /* oldtg doesn't exist, bail out */
        return;
    }

    count = 0;
    ptr = (char *)tag + sizeof (igl_objTagT);
    while (ptr != obj->endPtr && count != offset)
    {
        switch (*ptr)
        {
            case IGL_OBJDEF_TAG:
                ptr += sizeof (igl_objTagT);
            break;

            case IGL_OBJDEF_FCN:
                count ++;
                if (count != offset)
                {
                    ptr += sizeof (igl_objFcnT);
                }
            break;
        }
    }

    /* new tag placeholder wasn't found */
    if (count != offset)
        return;

    obj->curPtr = ptr;
    _igl_addObjectTag (obj, newtg, FALSE);
}


#if 0
_____________________________________________________
#endif
void
objinsert (Tag t)
{
    igl_objectT *obj;
    igl_objTagT *tag;

    if (igl->currentObjIdx == -1 || t == ENDTAG)
        return;

    obj = &igl->objects[igl->currentObjIdx];
    tag = _igl_findObjectTag (obj, t);
    if (tag == NULL)
        return;

    obj->curPtr = ((char *)tag + sizeof (igl_objTagT));
}


void
objdelete (Tag tag1, Tag tag2)
{
    igl_objectT *obj;
    igl_objTagT *t1, *t2;
    int size;

    if (igl->currentObjIdx == -1)
        return;
    obj = &igl->objects[igl->currentObjIdx];

    t1 = _igl_findObjectTag (obj, tag1);
    if (t1 == NULL)
        return;

    t2 = _igl_findObjectTag (obj, tag2);
    if (t2 == NULL || ((char *)t2 < (char *)t1) || ((char *)t2 - (char *)t1 == sizeof (igl_objTagT)))
        return;

    size = obj->endPtr + sizeof (igl_objTagT) - (char *)t2;
    memmove ((char *)t1 + sizeof (igl_objTagT), t2, size);
    /*
        (char *)t2 - (char *)t1 - sizeof (igl_objTagT) is the amount
        of bytes for which we moved t2 towards t1. so we must move endPtr
        for the same amount
    */
    obj->endPtr -= ((char *)t2 - (char *)t1 - sizeof (igl_objTagT));
    obj->curPtr = obj->endPtr;
#if 0
    _igl_printf ("objdelete (%d, %d)\n", tag1, tag2);
    _igl_dumpObject (obj);
#endif
}


void
objreplace (Tag t)
{
    igl_objectT *obj;
    igl_objTagT *tag1, *tag2;
    Tag t1, t2;
    char *ptr;

    if (igl->currentObjIdx == -1 || t == ENDTAG)
        return;
    obj = &igl->objects[igl->currentObjIdx];

    tag1 = _igl_findObjectTag (obj, t);
    if (tag1 == NULL)
        return;

    ptr = (char *)tag1 + sizeof (igl_objTagT);
    tag2 = NULL;
    while (tag2 == NULL)
    {
        switch (*ptr)
        {
            case IGL_OBJDEF_TAG:
                tag2 = (igl_objTagT *)ptr;
            break;

            case IGL_OBJDEF_FCN:
                ptr += sizeof (igl_objFcnT);
            break;
        }
    }

    t1 = tag1->id;
    t2 = tag2->id;
    objdelete (t1, t2);
    objinsert (t1);
}


void
chunksize (long chunk)
{
    if (chunk > 0)
        igl->objChunkSize = chunk;
}


void
compactify (Object id)
{
    int newSize;
    igl_objectT *obj;

    if (id <= 0)
        return;
    obj = _igl_findObject (id, NULL);
    if (obj == NULL || obj->chunkSize <= 0)
        return;

    /* compact the chunk to fit tags/ops from STARTTAG to ENDTAG */
    newSize = obj->endPtr + sizeof (igl_objTagT) - obj->beginPtr;
    if (obj->chunkSize > newSize)
        _igl_reallocObjectChunk (obj, newSize);
}
