/*
 * objapi.c
 *
 * object API managment
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
#include <stdarg.h>


#define IGL_CHECKOBJV()                     \
igl_objectT *obj;                           \
igl_objFcnT *fcn;                           \
if (igl->currentObjIdx == -1)               \
    return;                                 \
obj = &igl->objects[igl->currentObjIdx];

#define IGLFCNV(_op)                        \
fcn = _igl_addObjectFcn (obj, _op);         \
if (fcn == NULL)                            \
    return;


void
callfunc (void (*fctn)(), long nargs, ...)
{
    int i;
    va_list ap;
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CALLFUNC);

    fcn->params[0].v = (void *)fctn;
    fcn->params[1].i = (int)nargs;
    fcn->params[2].v = malloc (nargs * sizeof (long));
    if (fcn->params[2].v == NULL)
    {
        fcn->type = 0;
        fcn->opcode = OP_NONE;
        return;
    }

    va_start (ap, nargs);
    for (i=0; i < nargs; i++)
        ((long *)fcn->params[2].v)[i] = va_arg (ap, long);
    va_end (ap);
}


#if 0
_____________________________________________________
#endif
void
iglObj_clear (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CLEAR);
}

void
iglObj_lsetdepth (long near_, long far_)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LSETDEPTH);
    fcn->params[0].i = near_;
    fcn->params[1].i = far_;
}

void
iglObj_swapbuffers (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SWAPBUFFERS);
}

void
iglObj_mswapbuffers (long fbuf)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_MSWAPBUFFERS);
    fcn->params[0].i = fbuf;
}

void
iglObj_backbuffer (Boolean enable)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BACKBUFFER);
    fcn->params[0].i = enable;
}

void
iglObj_frontbuffer (Boolean enable)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_FRONTBUFFER);
    fcn->params[0].i = enable;
}

void
iglObj_leftbuffer (Boolean enable)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LEFTBUFFER);
    fcn->params[0].i = enable;
}

void
iglObj_rightbuffer (Boolean enable)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RIGHTBUFFER);
    fcn->params[0].i = enable;
}

void
iglObj_backface (Boolean enable)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BACKFACE);
    fcn->params[0].i = enable;
}

void
iglObj_frontface (Boolean enable)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_FRONTFACE);
    fcn->params[0].i = enable;
}

void
iglObj_depthcue (Boolean enable)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_DEPTHCUE);
    fcn->params[0].i = enable;
}

void
iglObj_lshaderange (Colorindex lowin, Colorindex highin, long znear, long zfar)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LSHADERANGE);
    fcn->params[0].us = lowin;
    fcn->params[1].us = highin;
    fcn->params[2].i = znear;
    fcn->params[3].i = zfar;
}

void
iglObj_lRGBrange (short rmin, short gmin, short bmin, short rmax, short gmax, short bmax, long znear, long zfar)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LRGBRANGE);
    fcn->params[0].us = rmin;
    fcn->params[1].us = gmin;
    fcn->params[2].us = bmin;
    fcn->params[3].us = rmax;
    fcn->params[4].us = gmax;
    fcn->params[5].us = bmax;
    fcn->params[6].i = znear;
    fcn->params[7].i = zfar;
}

void
iglObj_zbuffer (Boolean enable)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ZBUFFER);
    fcn->params[0].i = enable;
}

void
iglObj_zfunction (long func)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ZFUNCTION);
    fcn->params[0].i = func;
}

void
iglObj_zclear (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ZCLEAR);
}

void
iglObj_czclear (unsigned long cval, long zval)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CZCLEAR);
    fcn->params[0].ui = cval;
    fcn->params[1].i = zval;
}

void
iglObj_sclear (unsigned long sval)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SCLEAR);
    fcn->params[0].ui = sval;
}

void
iglObj_stencil (long enable, unsigned long ref, long func, unsigned long mask, long fail, long pass, long zpass)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_STENCIL);
    fcn->params[0].i = enable;
    fcn->params[1].ui = ref;
    fcn->params[2].i = func;
    fcn->params[3].ui = mask;
    fcn->params[4].i = fail;
    fcn->params[5].i = pass;
    fcn->params[6].i = zpass;
}

void
iglObj_acbuf (long op, float value)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ACBUF);
    fcn->params[0].i = op;
    fcn->params[1].f = value;
}

void
iglObj_shademodel (int mode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SHADEMODEL);
    fcn->params[0].i = mode;
}

void
iglObj_color (Colorindex c)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_COLOR);
    fcn->params[0].us = c;
}

void
iglObj_mapcolor (Colorindex i, short r, short g, short b)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_MAPCOLOR);
    fcn->params[0].us = i;
    fcn->params[1].s = r;
    fcn->params[2].s = g;
    fcn->params[3].s = b;
}

void
iglObj_cpack (unsigned long color)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CPACK);
    fcn->params[0].ui = color;
}

void
iglObj_RGBcolor (short r, short g, short b)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RGBCOLOR);
    fcn->params[0].s = r;
    fcn->params[1].s = g;
    fcn->params[2].s = b;
}

void
iglObj_afunction (long ref, long func)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_AFUNCTION);
    fcn->params[0].i = ref;
    fcn->params[1].i = func;
}

void
iglObj_blendfunction (long sfactor, long dfactor)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BLENDFUNCTION);
    fcn->params[0].i = sfactor;
    fcn->params[1].i = dfactor;
}

void
iglObj_dither (long mode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_DITHER);
    fcn->params[0].i = mode;
}

void
iglObj_fogvertex (long mode, float *params)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_FOGVERTEX);
    fcn->params[0].i = mode;
    fcn->params[1].v = (float *)malloc (5*sizeof (float));
    if (fcn->params[1].v != NULL)
    {
        if (mode == FG_VTX_LIN || mode == FG_PIX_LIN)
            memcpy (fcn->params[1].v, params, 5*sizeof (float));
        else
            memcpy (fcn->params[1].v, params, 4*sizeof (float));
    }
}

void
iglObj_logicop (long opcode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LOGICOP);
    fcn->params[0].i = opcode;
}

void
iglObj_c4f (float cv[4])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_C4F);
    fcn->params[0].v = (float *)malloc (4*sizeof (float));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, cv, 4*sizeof (float));
}

void
iglObj_c4i (int cv[4])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_C4I);
    fcn->params[0].v = (int *)malloc (4*sizeof (int));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, cv, 4*sizeof (int));
}

void
iglObj_c4s (short cv[4])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_C4S);
    fcn->params[0].v = (short *)malloc (4*sizeof (short));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, cv, 4*sizeof (short));
}

void
iglObj_mmode (short mode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_MMODE);
    fcn->params[0].s = mode;
}

void
iglObj_loadmatrix (Matrix m)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LOADMATRIX);
    fcn->params[0].v = (Matrix *)malloc (sizeof (Matrix));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, m, sizeof (Matrix));
}

void
iglObj_multmatrix (Matrix m)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_MULTMATRIX);
    fcn->params[0].v = (Matrix *)malloc (sizeof (Matrix));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, m, sizeof (Matrix));
}

void
iglObj_pushmatrix (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PUSHMATRIX);
}

void
iglObj_popmatrix (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POPMATRIX);
}

void
iglObj_rot (float amount, char angle)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ROT);
    fcn->params[0].f = amount;
    fcn->params[1].c = angle;
}

void
iglObj_rotate (Angle amount, char angle)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ROTATE);
    fcn->params[0].s = amount;
    fcn->params[1].c = angle;
}

void
iglObj_translate (Coord x, Coord y, Coord z)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_TRANSLATE);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = z;
}

void
iglObj_scale (float x, float y, float z)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SCALE);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = z;
}

void
iglObj_polarview (Coord dist, Angle azim, Angle inc, Angle twist)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLARVIEW);
    fcn->params[0].f = dist;
    fcn->params[1].s = azim;
    fcn->params[2].s = inc;
    fcn->params[3].s = twist;
}

void
iglObj_clipplane (long index, long mode, float *params)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CLIPPLANE);
    fcn->params[0].i = index;
    fcn->params[1].i = mode;
    fcn->params[2].v = (float *)malloc (4*sizeof (float));
    if (fcn->params[2].v != NULL)
        memcpy (fcn->params[2].v, params, 4*sizeof (float));
}

void
iglObj_ortho (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ORTHO);
    fcn->params[0].f = left;
    fcn->params[1].f = right;
    fcn->params[2].f = bottom;
    fcn->params[3].f = top;
    fcn->params[4].f = near_;
    fcn->params[5].f = far_;
}

void
iglObj_ortho2 (Coord left, Coord right, Coord bottom, Coord top)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ORTHO2);
    fcn->params[0].f = left;
    fcn->params[1].f = right;
    fcn->params[2].f = bottom;
    fcn->params[3].f = top;
}

void
iglObj_window (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_WINDOW);
    fcn->params[0].f = left;
    fcn->params[1].f = right;
    fcn->params[2].f = bottom;
    fcn->params[3].f = top;
    fcn->params[4].f = near_;
    fcn->params[5].f = far_;
}

void
iglObj_perspective (Angle fovy, float aspect, Coord near_, Coord far_)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PERSPECTIVE);
    fcn->params[0].s = fovy;
    fcn->params[1].f = aspect;
    fcn->params[2].f = near_;
    fcn->params[3].f = far_;
}

void
iglObj_viewport (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_VIEWPORT);
    fcn->params[0].s = left;
    fcn->params[1].s = right;
    fcn->params[2].s = bottom;
    fcn->params[3].s = top;
}

void
iglObj_pushviewport (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PUSHVIEWPORT);
}

void
iglObj_popviewport (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POPVIEWPORT);
}

void
iglObj_lookat (Coord vx, Coord vy, Coord vz, Coord px, Coord py, Coord pz, Angle twist)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LOOKAT);
    fcn->params[0].f = vx;
    fcn->params[1].f = vy;
    fcn->params[2].f = vz;
    fcn->params[3].f = px;
    fcn->params[4].f = py;
    fcn->params[5].f = pz;
    fcn->params[6].s = twist;
}

void
iglObj_scrmask (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SCRMASK);
    fcn->params[0].s = left;
    fcn->params[1].s = right;
    fcn->params[2].s = bottom;
    fcn->params[3].s = top;
}

void
iglObj_polf (long n, const Coord parray[][3])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLF);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Coord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*3*sizeof (Coord));
}

void
iglObj_polfi (long n, const Icoord parray[][3])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLFI);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Icoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*3*sizeof (Icoord));
}

void
iglObj_polfs (long n, const Scoord parray[][3])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLFS);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Scoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*3*sizeof (Scoord));
}

void
iglObj_polf2 (long n, const Coord parray[][2])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLF2);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*2*sizeof (Icoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*2*sizeof (Icoord));
}

void
iglObj_polf2i (long n, const Icoord parray[][2])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLF2I);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*2*sizeof (Icoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*2*sizeof (Icoord));
}

void
iglObj_polf2s (long n, const Scoord parray[][2])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLF2S);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*2*sizeof (Scoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*2*sizeof (Scoord));
}

void
iglObj_poly (long n, const Coord parray[][3])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLY);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Coord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*3*sizeof (Coord));
}

void
iglObj_polyi (long n, const Icoord parray[][3])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLYI);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Icoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*3*sizeof (Icoord));
}

void
iglObj_polys (long n, const Scoord parray[][3])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLYS);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Scoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*3*sizeof (Scoord));
}

void
iglObj_poly2 (long n, const Coord parray[][2])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLY2);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*2*sizeof (Icoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*2*sizeof (Icoord));
}

void
iglObj_poly2i (long n, const Icoord parray[][2])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLY2I);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*2*sizeof (Icoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*2*sizeof (Icoord));
}

void
iglObj_poly2s (long n, const Scoord parray[][2])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLY2S);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*2*sizeof (Scoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*2*sizeof (Scoord));
}

void
iglObj_polymode (long mode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLYMODE);
    fcn->params[0].i = mode;
}

void
iglObj_polysmooth (long mode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POLYSMOOTH);
    fcn->params[0].i = mode;
}

void
iglObj_displacepolygon (float scalefactor)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_DISPLACEPOLYGON);
    fcn->params[0].f = scalefactor;
}

void
iglObj_pmv (Coord x, Coord y, Coord z)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PMV);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = z;
}

void
iglObj_pdr (Coord x, Coord y, Coord z)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PDR);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = z;
}

void
iglObj_pclos (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PCLOS);
}

void
iglObj_pnt (Coord x, Coord y, Coord z)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PNT);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = z;
}

void
iglObj_pntsizef (float n)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PNTSIZEF);
    fcn->params[0].f = n;
}

void
iglObj_pntsmooth (unsigned long mode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PNTSMOOTH);
    fcn->params[0].ui = mode;
}

void
iglObj_rect (Coord a, Coord b, Coord c, Coord d)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RECT);
    fcn->params[0].f = a;
    fcn->params[1].f = b;
    fcn->params[2].f = c;
    fcn->params[3].f = d;
}

void
iglObj_rectf (Coord a, Coord b, Coord c, Coord d)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RECTF);
    fcn->params[0].f = a;
    fcn->params[1].f = b;
    fcn->params[2].f = c;
    fcn->params[3].f = d;
}

void
iglObj_sbox (Coord a, Coord b, Coord c, Coord d)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SBOX);
    fcn->params[0].f = a;
    fcn->params[1].f = b;
    fcn->params[2].f = c;
    fcn->params[3].f = d;
}

void
iglObj_sboxf (Coord a, Coord b, Coord c, Coord d)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SBOXF);
    fcn->params[0].f = a;
    fcn->params[1].f = b;
    fcn->params[2].f = c;
    fcn->params[3].f = d;
}

void
iglObj_v4f (float vector[4])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_V4F);
    fcn->params[0].v = (float *)malloc (4*sizeof (float));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, vector, 4*sizeof (float));
}

void
iglObj_arc (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ARC);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = radius;
    fcn->params[3].s = startAngle;
    fcn->params[4].s = endAngle;
}

void
iglObj_arcf (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ARCF);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = radius;
    fcn->params[3].s = startAngle;
    fcn->params[4].s = endAngle;
}

void
iglObj_circ (Coord x, Coord y, Coord radius)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CIRC);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = radius;
}

void
iglObj_circf (Coord x, Coord y, Coord radius)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CIRCF);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = radius;
}

void
iglObj_bgntmesh (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BGNTMESH);
}

void
iglObj_swaptmesh (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SWAPTMESH);
}

void
iglObj_bgnclosedline (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BGNCLOSEDLINE);
}

void
iglObj_bgncurve (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BGNCURVE);
}

void
iglObj_endcurve (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ENDCURVE);
}

void
iglObj_nurbscurve (long knotCount, const double *knotList, long offset, const double *ctlArray, long order, long type)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_NURBSCURVE);

    /* TODO: store knotList in compact format, minimizing knotCount */
    fcn->params[0].i = knotCount;
    fcn->params[1].v = malloc (knotCount*sizeof (double));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, knotList, knotCount*sizeof (double));

    /* TODO: store ctlArray in compact format, minimizing order */
    fcn->params[2].i = offset;
    fcn->params[3].v = malloc (order*sizeof (double));
    if (fcn->params[3].v != NULL)
        memcpy (fcn->params[3].v, ctlArray, order*sizeof (double));

    fcn->params[4].i = order;
    fcn->params[5].i = type;
}

void
iglObj_bgnsurface (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BGNSURFACE);
}

void
iglObj_endsurface (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ENDSURFACE);
}

void
iglObj_nurbssurface (long scount, const double *sknot, long tcount, const double *tknot, long soffset, long toffset, const double *ctlArray, long sorder, long torder, long type)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_NURBSSURFACE);

    /* TODO: store *knot, in compact format, with minimal *count */
    fcn->params[0].i = scount;
    fcn->params[1].v = malloc (scount*sizeof (double));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, sknot, scount*sizeof (double));

    fcn->params[2].i = tcount;
    fcn->params[3].v = malloc (tcount*sizeof (double));
    if (fcn->params[3].v != NULL)
        memcpy (fcn->params[3].v, tknot, tcount*sizeof (double));

    /* TODO: store ctlArray in compact format, with minimal *order */
    fcn->params[4].i = soffset;
    fcn->params[5].i = toffset;
    fcn->params[6].v = malloc (sorder*sizeof (double));
    if (fcn->params[6].v != NULL)
        memcpy (fcn->params[6].v, ctlArray, sorder*sizeof (double));

    fcn->params[7].i = sorder;
    fcn->params[8].i = torder;
    fcn->params[9].i = type;
}

void
iglObj_bgntrim (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BGNTRIM);
}

void
iglObj_endtrim (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ENDTRIM);
}

void
iglObj_pwlcurve (long n, double *dataArray, long byteSize, long type)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PWLCURVE);

    /* TODO: store dataArray in compact format, using minimal byteSize */
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*sizeof (double));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, dataArray, n*sizeof (double));

    fcn->params[2].i = byteSize;
    fcn->params[3].i = type;
}

void
iglObj_setnurbsproperty (long property, float value)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SETNURBSPROPERTY);
    fcn->params[0].i = property;
    fcn->params[1].f = value;
}

void
iglObj_bgnline (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BGNLINE);
}

void
iglObj_bgnpoint (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BGNPOINT);
}

void
iglObj_endpoint (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_ENDPOINT);
}

void
iglObj_bgnpolygon (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BGNPOLYGON);
}

void
iglObj_bgnqstrip (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_BGNQSTRIP);
}

void
iglObj_curveit (short niter)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CURVEIT);
    fcn->params[0].i = niter;
}

void
iglObj_crvn (long n, Coord points[][3])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CRVN);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Coord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, points, n*3*sizeof (Coord));
}

void
iglObj_rcrvn (long n, Coord points[][4])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RCRVN);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*4*sizeof (Coord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, points, n*4*sizeof (Coord));
}

void
iglObj_curveprecision (short n)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CURVEPRECISION);
    fcn->params[0].i = n;
}

void
iglObj_curvebasis (short n)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CURVEBASIS);
    fcn->params[0].i = n;
}

void
iglObj_patch (Matrix geomx, Matrix geomy, Matrix geomz)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PATCH);
    fcn->params[0].v = malloc (4*4*sizeof (float));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, geomx, 4*4*sizeof (float));
    fcn->params[1].v = malloc (4*4*sizeof (float));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, geomy, 4*4*sizeof (float));
    fcn->params[2].v = malloc (4*4*sizeof (float));
    if (fcn->params[2].v != NULL)
        memcpy (fcn->params[2].v, geomz, 4*4*sizeof (float));
}


void
iglObj_rpatch (Matrix geomx, Matrix geomy, Matrix geomz, Matrix geomw)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RPATCH);
    fcn->params[0].v = malloc (4*4*sizeof (float));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, geomx, 4*4*sizeof (float));
    fcn->params[1].v = malloc (4*4*sizeof (float));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, geomy, 4*4*sizeof (float));
    fcn->params[2].v = malloc (4*4*sizeof (float));
    if (fcn->params[2].v != NULL)
        memcpy (fcn->params[2].v, geomz, 4*4*sizeof (float));
    fcn->params[3].v = malloc (4*4*sizeof (float));
    if (fcn->params[3].v != NULL)
        memcpy (fcn->params[3].v, geomw, 4*4*sizeof (float));
}

void
iglObj_patchcurves (short nu, short nv)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PATCHCURVES);
    fcn->params[0].i = nu;
    fcn->params[1].i = nv;
}

void
iglObj_patchprecision (short nu, short nv)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PATCHPRECISION);
    fcn->params[0].i = nu;
    fcn->params[1].i = nv;
}

void
iglObj_patchbasis (short nu, short nv)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PATCHBASIS);
    fcn->params[0].i = nu;
    fcn->params[1].i = nv;
}

void
iglObj_nmode (long mode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_NMODE);
    fcn->params[0].i = mode;
}

void
iglObj_n3f (float *vector)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_N3F);
    fcn->params[0].v = (float *)malloc (3*sizeof (float));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, vector, 3*sizeof (float));
}

void
iglObj_linesmooth (unsigned long mode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LINESMOOTH);
    fcn->params[0].ui = mode;
}

void
iglObj_linewidthf (float width)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LINEWIDTHF);
    fcn->params[0].f = width;
}

void
iglObj_setlinestyle (short n)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SETLINESTYLE);
    fcn->params[0].s = n;
}

void
iglObj_lsrepeat (long factor)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LSREPEAT);
    fcn->params[0].i = factor;
}

void
iglObj_setpattern (short index)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SETPATTERN);
    fcn->params[0].s = index;
}

void
iglObj_lmbind (short target, short index)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LMBIND);
    fcn->params[0].s = target;
    fcn->params[1].s = index;
}

void
iglObj_lmcolor (long mode)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LMCOLOR);
    fcn->params[0].i = mode;
}

void
iglObj_move (Coord x, Coord y, Coord z)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_MOVE);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = z;
}

void
iglObj_rmv (Coord dx, Coord dy, Coord dz)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RMV);
    fcn->params[0].f = dx;
    fcn->params[1].f = dy;
    fcn->params[2].f = dz;
}

void
iglObj_rpmv (Coord dx, Coord dy, Coord dz)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RPMV);
    fcn->params[0].f = dx;
    fcn->params[1].f = dy;
    fcn->params[2].f = dz;
}

void
iglObj_draw (Coord x, Coord y, Coord z)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_DRAW);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = z;
}

void
iglObj_rdr (Coord dx, Coord dy, Coord dz)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RDR);
    fcn->params[0].f = dx;
    fcn->params[1].f = dy;
    fcn->params[2].f = dz;
}

void
iglObj_rpdr (Coord dx, Coord dy, Coord dz)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_RPDR);
    fcn->params[0].f = dx;
    fcn->params[1].f = dy;
    fcn->params[2].f = dz;
}

void
iglObj_splf (long n, Coord parray[][3], Colorindex iarray[])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SPLF);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Coord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*3*sizeof (Coord));
    fcn->params[2].v = malloc (n*sizeof (Colorindex));
    if (fcn->params[2].v != NULL)
        memcpy (fcn->params[2].v, iarray, n*sizeof (Colorindex));
}

void
iglObj_splfi (long n, Icoord parray[][3], Colorindex iarray[])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SPLFI);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Icoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*3*sizeof (Icoord));
    fcn->params[2].v = malloc (n*sizeof (Colorindex));
    if (fcn->params[2].v != NULL)
        memcpy (fcn->params[2].v, iarray, n*sizeof (Colorindex));
}

void
iglObj_splfs (long n, Scoord parray[][3], Colorindex iarray[])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SPLFS);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*3*sizeof (Scoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*3*sizeof (Scoord));
    fcn->params[2].v = malloc (n*sizeof (Colorindex));
    if (fcn->params[2].v != NULL)
        memcpy (fcn->params[2].v, iarray, n*sizeof (Colorindex));
}

void
iglObj_splf2 (long n, Coord parray[][2], Colorindex iarray[])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SPLF2);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*2*sizeof (Coord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*2*sizeof (Coord));
    fcn->params[2].v = malloc (n*sizeof (Colorindex));
    if (fcn->params[2].v != NULL)
        memcpy (fcn->params[2].v, iarray, n*sizeof (Colorindex));
}

void
iglObj_splf2i (long n, Icoord parray[][2], Colorindex iarray[])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SPLF2I);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*2*sizeof (Icoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*2*sizeof (Icoord));
    fcn->params[2].v = malloc (n*sizeof (Colorindex));
    if (fcn->params[2].v != NULL)
        memcpy (fcn->params[2].v, iarray, n*sizeof (Colorindex));
}

void
iglObj_splf2s (long n, Scoord parray[][2], Colorindex iarray[])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_SPLF2S);
    fcn->params[0].i = n;
    fcn->params[1].v = malloc (n*2*sizeof (Scoord));
    if (fcn->params[1].v != NULL)
        memcpy (fcn->params[1].v, parray, n*2*sizeof (Scoord));
    fcn->params[2].v = malloc (n*sizeof (Colorindex));
    if (fcn->params[2].v != NULL)
        memcpy (fcn->params[2].v, iarray, n*sizeof (Colorindex));
}

void
iglObj_callobj (Object id)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CALLOBJ);
    fcn->params[0].i = id;
}

void
iglObj_tevbind (long target, long index)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_TEVBIND);
    fcn->params[0].i = target;
    fcn->params[1].i = index;
}

void
iglObj_texbind (long target, long index)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_TEXBIND);
    fcn->params[0].i = target;
    fcn->params[1].i = index;
}

void
iglObj_texgen (long coord, long mode, float *params)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_TEXGEN);
    fcn->params[0].i = coord;
    fcn->params[1].i = mode;
    if (params != NULL)
    {
        fcn->params[2].v = (float *)malloc (4*sizeof (float));
        if (fcn->params[2].v != NULL)
            memcpy (fcn->params[2].v, params, 4*sizeof (float));
    }
    else
        fcn->params[2].v = NULL;
}

void
iglObj_t4f (float vector[4])
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_T4F);
    fcn->params[0].v = (float *)malloc (4*sizeof (float));
    if (fcn->params[0].v != NULL)
        memcpy (fcn->params[0].v, vector, 4*sizeof (float));
}

void
iglObj_cmov (Coord x, Coord y, Coord z)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_CMOV);
    fcn->params[0].f = x;
    fcn->params[1].f = y;
    fcn->params[2].f = z;
}

void
iglObj_lcharstr (long type, void *str)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LCHARSTR);
    fcn->params[0].i = type;
    fcn->params[1].v = malloc (strlen ((char *)str) + 1);
    if (fcn->params[1].v != NULL)
        strcpy ((char *)fcn->params[1].v, (char *)str);
}

void
iglObj_font (short fntnum)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_FONT);
    fcn->params[0].s = fntnum;
}

void
iglObj_pushattributes (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PUSHATTRIBUTES);
}

void
iglObj_popattributes (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POPATTRIBUTES);
}

void
iglObj_initnames (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_INITNAMES);
}

void
iglObj_loadname (short name)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_LOADNAME);
    fcn->params[0].s = name;
}

void
iglObj_pushname (short name)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_PUSHNAME);
    fcn->params[0].s = name;
}

void
iglObj_popname (void)
{
    IGL_CHECKOBJV ();
    IGLFCNV (OP_POPNAME);
}


#define API(_fcn)                       \
igl->execApi->_fcn = iglExec_##_fcn;    \
igl->objApi->_fcn = iglObj_##_fcn;


int
_igl_initObjectApi (void)
{
    /* get memory for the function tables */
    igl->execApi = calloc (1, sizeof (igl_apiList));
    igl->objApi = calloc (1, sizeof (igl_apiList));
    if (igl->execApi == NULL || igl->objApi == NULL)
        return -1;

    /* initialize execution function tables */
    API (clear);
    API (lsetdepth);
    API (swapbuffers);
    API (mswapbuffers);
    API (backbuffer);
    API (frontbuffer);
    API (leftbuffer);
    API (rightbuffer);
    API (backface);
    API (frontface);
    API (depthcue);
    API (lshaderange);
    API (lRGBrange);
    API (zbuffer);
    API (zfunction);
    API (zclear);
    API (czclear);
    API (sclear);
    API (stencil);
    API (acbuf);
    API (shademodel);
    API (color);
    API (mapcolor);
    API (cpack);
    API (RGBcolor);
    API (afunction);
    API (blendfunction);
    API (dither);
    API (fogvertex);
    API (logicop);
    API (c4f);
    API (c4i);
    API (c4s);
    API (mmode);
    API (loadmatrix);
    API (multmatrix);
    API (pushmatrix);
    API (popmatrix);
    API (rot);
    API (rotate);
    API (translate);
    API (scale);
    API (polarview);
    API (clipplane);
    API (ortho);
    API (ortho2);
    API (window);
    API (perspective);
    API (viewport);
    API (pushviewport);
    API (popviewport);
    API (lookat);
    API (scrmask);
    API (polf);
    API (polfi);
    API (polfs);
    API (polf2);
    API (polf2i);
    API (polf2s);
    API (poly);
    API (polyi);
    API (polys);
    API (poly2);
    API (poly2i);
    API (poly2s);
    API (polymode);
    API (polysmooth);
    API (displacepolygon);
    API (pmv);
    API (pdr);
    API (pclos);
    API (pnt);
    API (pntsizef);
    API (pntsmooth);
    API (rect);
    API (rectf);
    API (sbox);
    API (sboxf);
    API (v4f);
    API (arc);
    API (arcf);
    API (circ);
    API (circf);
    API (bgntmesh);
    API (swaptmesh);
    API (bgnclosedline);
    API (bgncurve);
    API (endcurve);
    API (nurbscurve);
    API (bgnsurface);
    API (endsurface);
    API (nurbssurface);
    API (bgntrim);
    API (endtrim);
    API (pwlcurve);
    API (setnurbsproperty);
    API (bgnline);
    API (bgnpoint);
    API (endpoint);
    API (bgnpolygon);
    API (bgnqstrip);
    API (curveit);
    API (crvn);
    API (rcrvn);
    API (curveprecision);
    API (curvebasis);
    API (patch);
    API (rpatch);
    API (patchcurves);
    API (patchprecision);
    API (patchbasis);
    API (nmode);
    API (n3f);
    API (linesmooth);
    API (linewidthf);
    API (setlinestyle);
    API (lsrepeat);
    API (setpattern);
    API (lmbind);
    API (lmcolor);
    API (move);
    API (rmv);
    API (rpmv);
    API (draw);
    API (rdr);
    API (rpdr);
    API (splf);
    API (splfi);
    API (splfs);
    API (splf2);
    API (splf2i);
    API (splf2s);
    API (callobj);
    API (tevbind);
    API (texbind);
    API (texgen);
    API (t4f);
    API (cmov);
    API (lcharstr);
    API (font);
    API (pushattributes);
    API (popattributes);
    API (initnames);
    API (loadname);
    API (pushname);
    API (popname);

    /* now in immediate mode */
    igl->api = igl->execApi;
    return 0;
}


void
_igl_exitObjectApi (void)
{
    if (igl->execApi != NULL)
        free (igl->execApi);
    igl->execApi = NULL;
    if (igl->execApi != NULL)
        free (igl->objApi);
    igl->objApi = NULL;
}


/* GL: unimplemented stuff */
void
bbox2 (Screencoord xmin, Screencoord ymin, Coord x1, Coord y1, Coord x2, Coord y2)
{
    _igl_trace (__func__,TRUE,"");
}


void
bbox2i (Screencoord xmin, Screencoord ymin, Icoord x1, Icoord y1, Icoord x2, Icoord y2)
{
    _igl_trace (__func__,TRUE,"");
}


void
bbox2s (Screencoord xmin, Screencoord ymin, Scoord x1, Scoord y1, Scoord x2, Scoord y2)
{
    _igl_trace (__func__,TRUE,"");
}


