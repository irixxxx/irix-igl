/*
 * grpos.c
 *
 * old-style drawing functions and graphics position handling
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


/* NOTE: OpenGL generally handles line pixels by the diamond rule, which leads
 * to nothing being drawn for a line of length zero. IrisGL however produces at
 * least one pixel. To emulate this, draw a point for lines of length 0.
 * TODO: set pointsize temporarily to linewidth in this case?
 */


/* TODO: IrisGL shades flatshaded polygons with the color of the last vertex,
 * while OpenGL uses the color of the first vertex. To emulate IrisGL correctly
 * all vertices of a polygon must be stored and only sent to Opengl after the
 * pclos/endpolygon primitive (using glDrawElements/glDrawArrays?).
 */

/* TODO: IrisGL handles concave polygons if concave has been called in advance.
 * OpenGL has no such built-in functionality. The gluTess* functions might be
 * used to do the job, however handling of color, normals and texture coords
 * must be considered.
 * (might look at the angles. They must be all smaller or larger then 180)
 */

/* TODO: IrisGL allows for matrix changes between calls referring to the
 * graphical position. To accomodate for this, the projected grpos needs to be
 * stored, and unprojected to the changed matrices for drawing. Was this really
 * used anywhere?
 */


static inline void
_igl_setgpos (Coord x, Coord y, Coord z)
{
    IGL_CTX ()->gposx = x;
    IGL_CTX ()->gposy = y;
    IGL_CTX ()->gposz = z;
}

static inline void
_igl_addgpos (Coord x, Coord y, Coord z)
{
    IGL_CTX ()->gposx += x;
    IGL_CTX ()->gposy += y;
    IGL_CTX ()->gposz += z;
}


/* GL: getgpos - gets the current graphics position */
void
getgpos (Coord *fx, Coord *fy, Coord *fz, Coord *fw)
{
    GLdouble *model = IGL_CTX ()->modelMatrix, *proj = IGL_CTX ()->projMatrix;
    float gx = IGL_CTX ()->gposx, gy = IGL_CTX ()->gposy, gz = IGL_CTX ()->gposz;
    GLdouble tmp[4];

    _igl_trace (__func__,TRUE,"%p,%p,%p,%p",fx,fy,fz,fw);
    IGL_CHECKWNDV ();

    /* NOTE: returns the position after transformation (GPOS * MODEL * PROJ) */
    _igl_getMatrices ();

    /* TODO: this, or like gluProject with viewport 0,0,1,1? */
    tmp[0] = model[0]*gx + model[4]*gy + model[8]*gz + model[12];
    tmp[1] = model[1]*gx + model[5]*gy + model[9]*gz + model[13];
    tmp[2] = model[2]*gx + model[6]*gy + model[10]*gz + model[14];
    tmp[3] = model[3]*gx + model[7]*gy + model[11]*gz + model[15];

    *fx = proj[0]*tmp[0] + proj[4]*tmp[1] + proj[8]*tmp[2] + proj[12]*tmp[3];
    *fy = proj[1]*tmp[0] + proj[5]*tmp[1] + proj[9]*tmp[2] + proj[13]*tmp[3];
    *fz = proj[2]*tmp[0] + proj[6]*tmp[1] + proj[10]*tmp[2] + proj[14]*tmp[3];
    *fw = proj[3]*tmp[0] + proj[7]*tmp[1] + proj[11]*tmp[2] + proj[15]*tmp[3];
}


/* Helper macro to define func for 2 and 3 args, and for float, int and short */
#define DEF3(func, x, y, z, block) \
	void func (Coord x, Coord y, Coord z) \
            { _igl_trace (__func__,FALSE,"%g,%g,%g",x,y,z); block; } \
	void func##i (Icoord x, Icoord y, Icoord z) \
            { _igl_trace (__func__,FALSE,"%d,%d,%d",x,y,z); block; } \
	void func##s (Scoord x, Scoord y, Scoord z) \
            { _igl_trace (__func__,FALSE,"%d,%d,%d",x,y,z); block; }
#define DEF2(func, x, y, block) \
	void func (Coord x, Coord y) \
            { _igl_trace (__func__,FALSE,"%g,%g",x,y); block; } \
	void func##i (Icoord x, Icoord y) \
            { _igl_trace (__func__,FALSE,"%d,%d",x,y); block; } \
	void func##s (Scoord x, Scoord y) \
            { _igl_trace (__func__,FALSE,"%d,%d",x,y); block; }


#if 0
____________________________ move functions ____________________________
#endif
/* GL: move, movei, moves, move2, move2i, move2s - moves the current graphics
         position to a specified point */
DEF3( move, x, y, z,
    igl->api->move ((Coord)x, (Coord)y, (Coord)z)
)

void
iglExec_move (Coord x, Coord y, Coord z)
{
    IGL_CHECKWNDV ();

    _igl_setgpos (x, y, z);
}


DEF2( move2, x, y,
    igl->api->move ((Coord)x, (Coord)y, 0.0f)
)


#if 0
____________________________ rmv functions ____________________________
#endif
/* GL: rmv, rmvi, rmvs, rmv2, rmv2i, rmv2s - relative move */
DEF3( rmv, dx, dy, dz,
    igl->api->rmv ((Coord)dx, (Coord)dy, (Coord)dz)
)

void
iglExec_rmv (Coord dx, Coord dy, Coord dz)
{
    IGL_CHECKWNDV ();

    _igl_addgpos (dx, dy, dz);
}


DEF2( rmv2, dx, dy,
    igl->api->rmv ((Coord)dx, (Coord)dy, 0.0f)
)


#if 0
____________________________ draw functions ____________________________
#endif
/* GL: draw, drawi, draws, draw2, draw2i, draw2s - draws a line */
DEF3( draw, x, y, z,
    igl->api->draw ((Coord)x, (Coord)y, (Coord)z)
)

void
iglExec_draw (Coord x, Coord y, Coord z)
{
    float gx = IGL_CTX ()->gposx, gy = IGL_CTX ()->gposy, gz = IGL_CTX ()->gposz;

    IGL_CHECKWNDV ();

    IGL_BGNLIGHT ();
    /* special case: line of length 0, draw a point instead */
    if (gx == x && gy == y && gz == z)
    {
        glBegin (GL_POINTS);
            glVertex3f (gx, gy, gz);
        glEnd ();
    }
    else
    {
        glBegin (GL_LINES);
            glVertex3f (gx, gy, gz);
            glVertex3f (x, y, z);
        glEnd ();
    }
    IGL_ENDLIGHT ();

    _igl_setgpos (x, y, z);
}


DEF2( draw2, dx, dy,
    igl->api->draw ((Coord)dx, (Coord)dy, 0.0f)
)



#if 0
____________________________ rdr functions ____________________________
#endif
/* GL: rdr, rdri, rdrs, rdr2, rdr2i, rdr2s - relative draw */
DEF3( rdr, dx, dy, dz,
    igl->api->rdr ((Coord)dx, (Coord)dy, (Coord)dz)
)

void
iglExec_rdr (Coord dx, Coord dy, Coord dz)
{
    float gx = IGL_CTX ()->gposx, gy = IGL_CTX ()->gposy, gz = IGL_CTX ()->gposz;

    IGL_CHECKWNDV ();

    IGL_BGNLIGHT ();
    /* special case: line of length 0 draws at least one pixel */
    if (dx == 0 && dy == 0 && dz == 0)
    {
        glBegin (GL_POINTS);
            glVertex3f (gx, gy, gz);
        glEnd ();
    }
    else
    {
        glBegin (GL_LINES);
            glVertex3f (gx, gy, gz);
            glVertex3f (gx+dx, gy+dy, gz+dz);
        glEnd ();
    }
    IGL_ENDLIGHT ();

    _igl_addgpos (dx, dy, dz);
}


DEF2( rdr2, dx, dy,
    igl->api->rdr ((Coord)dx, (Coord)dy, 0.0f);
)


#if 0
____________________________ pnt functions ____________________________
#endif
/* GL: pnt, pnti, pnts, pnt2, pnt2i, pnt2s - draws a point */
DEF3( pnt, x, y, z,
    igl->api->pnt ((Coord)x, (Coord)y, (Coord)z)
)

void
iglExec_pnt (Coord x, Coord y, Coord z)
{
    IGL_CHECKWNDV ();

    IGL_BGNLIGHT ();
    glBegin (GL_POINTS);
        glVertex3f (x, y, z);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (x, y, z);
}


DEF2( pnt2, x, y,
    igl->api->pnt ((Coord)x, (Coord)y, 0.0f)
)


#if 0
____________________________ rpmv functions ____________________________
#endif
/* GL: rpmv, rpmvi, rpmvs, rpmv2, rpmv2i, rpmv2s - relative polygon move */
DEF3( rpmv, dx, dy, dz,
    igl->api->rpmv ((Coord)dx, (Coord)dy, (Coord)dz)
)

void
iglExec_rpmv (Coord dx, Coord dy, Coord dz)
{
    IGL_CHECKWNDV ();

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        glVertex3f (IGL_CTX ()->gposx, IGL_CTX ()->gposy, IGL_CTX ()->gposz);

    _igl_addgpos (dx, dy, dz);
}


DEF2( rpmv2, dx, dy,
    igl->api->rpmv ((Coord)dx, (Coord)dy, 0.0f)
)


#if 0
____________________________ rpdr functions ____________________________
#endif
/* GL: rpdr, rpdri, rpdrs, rpdr2, rpdr2i, rpdr2s - relative polygon draw */
DEF3( rpdr, dx, dy, dz,
    igl->api->rpdr ((Coord)dx, (Coord)dy, (Coord)dz)
)

void
iglExec_rpdr (Coord dx, Coord dy, Coord dz)
{
    IGL_CHECKWNDV ();

    glVertex3f (IGL_CTX ()->gposx, IGL_CTX ()->gposy, IGL_CTX ()->gposz);

    _igl_addgpos (dx, dy, dz);
}


DEF2( rpdr2, dx, dy,
    igl->api->rpdr ((Coord)dx, (Coord)dy, 0.0f)
)


#if 0
____________________________ pmv functions ____________________________
#endif
/* GL: pmv, pmvi, pmvs, pmv2, pmv2i, pmv2s - specifies the first point of a polygon */
DEF3( pmv, x, y, z,
    igl->api->pmv ((Coord)x, (Coord)y, (Coord)z)
)

void
iglExec_pmv (Coord x, Coord y, Coord z)
{
    IGL_CHECKWNDV ();

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        glVertex3f (x, y, z);

    _igl_setgpos (x, y, z);
}


DEF2( pmv2, x, y,
    igl->api->pmv ((Coord)x, (Coord)y, 0.0f)
)


#if 0
____________________________ pdr functions ____________________________
#endif
/* GL: pdr, pdri, pdrs, pdr2, pdr2i, pdr2s - specifies the next point of a polygon */
DEF3( pdr, x, y, z,
    igl->api->pdr ((Coord)x, (Coord)y, (Coord)z)
)

void
iglExec_pdr (Coord x, Coord y, Coord z)
{
    IGL_CHECKWNDV ();

    glVertex3f (x, y, z);

    _igl_setgpos (x, y, z);
}


DEF2( pdr2, x, y,
    igl->api->pdr ((Coord)x, (Coord)y, 0.0f)
)


/* GL: pclos - closes a filled polygon */
void
pclos (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->pclos ();
}

void
iglExec_pclos (void)
{
    IGL_CHECKWNDV ();

    glEnd ();
    IGL_ENDLIGHT ();
}


/* GL: spclos - obsolete routine */
void
spclos (void)
{
    igl->api->pclos ();
}


#if 0
____________________________ splf functions ____________________________
#endif
/* GL: splf, splfi, splfs, splf2, splf2i, splf2s - draws a shaded filled polygon */
void
splf (long n, Coord parray[][3], Colorindex iarray[])
{
    _igl_trace (__func__,FALSE,"%ld,%p,%p",n,parray,iarray);
    igl->api->splf (n, parray, iarray);
}

void
iglExec_splf (long n, Coord parray[][3], Colorindex iarray[])
{
    int i;

    IGL_CHECKWNDV ();

    /* "must be used in colormap mode" */
    if ((IGL_CTX ()->flags & IGL_WFLAGS_RGBA) && IGL_CTX ()->drawMode == NORMALDRAW)
        return;

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
        {
            color (iarray[i]);
            glVertex3fv (&parray[i][0]);
        }
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], parray[0][2]);
}


void
splfi (long n, Icoord parray[][3], Colorindex iarray[])
{
    _igl_trace (__func__,FALSE,"%ld,%p,%p",n,parray,iarray);
    igl->api->splfi (n, parray, iarray);
}

void
iglExec_splfi (long n, Icoord parray[][3], Colorindex iarray[])
{
    int i;

    IGL_CHECKWNDV ();

    /* "must be used in colormap mode" */
    if ((IGL_CTX ()->flags & IGL_WFLAGS_RGBA) && IGL_CTX ()->drawMode == NORMALDRAW)
        return;

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
        {
            color (iarray[i]);
            glVertex3iv ((GLint *)&parray[i][0]);
        }
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], parray[0][2]);
}


void
splfs (long n, Scoord parray[][3], Colorindex iarray[])
{
    _igl_trace (__func__,FALSE,"%ld,%p,%p",n,parray,iarray);
    igl->api->splfs (n, parray, iarray);
}

void
iglExec_splfs (long n, Scoord parray[][3], Colorindex iarray[])
{
    int i;

    IGL_CHECKWNDV ();

    /* "must be used in colormap mode" */
    if ((IGL_CTX ()->flags & IGL_WFLAGS_RGBA) && IGL_CTX ()->drawMode == NORMALDRAW)
        return;

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
        {
            color (iarray[i]);
            glVertex3sv (&parray[i][0]);
        }
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], parray[0][2]);
}


void
splf2 (long n, Coord parray[][2], Colorindex iarray[])
{
    _igl_trace (__func__,FALSE,"%ld,%p,%p",n,parray,iarray);
    igl->api->splf2 (n, parray, iarray);
}

void
iglExec_splf2 (long n, Coord parray[][2], Colorindex iarray[])
{
    int i;

    IGL_CHECKWNDV ();

    /* "must be used in colormap mode" */
    if ((IGL_CTX ()->flags & IGL_WFLAGS_RGBA) && IGL_CTX ()->drawMode == NORMALDRAW)
        return;

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
        {
            color (iarray[i]);
            glVertex2fv (&parray[i][0]);
        }
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], 0);
}


void
splf2i (long n, Icoord parray[][2], Colorindex iarray[])
{
    _igl_trace (__func__,FALSE,"%ld,%p,%p",n,parray,iarray);
    igl->api->splf2i (n, parray, iarray);
}

void
iglExec_splf2i (long n, Icoord parray[][2], Colorindex iarray[])
{
    int i;

    IGL_CHECKWNDV ();

    /* "must be used in colormap mode" */
    if ((IGL_CTX ()->flags & IGL_WFLAGS_RGBA) && IGL_CTX ()->drawMode == NORMALDRAW)
        return;

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
        {
            color (iarray[i]);
            glVertex2iv ((GLint *)&parray[i][0]);
        }
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], 0);
}


void
splf2s (long n, Scoord parray[][2], Colorindex iarray[])
{
    _igl_trace (__func__,FALSE,"%ld,%p,%p",n,parray,iarray);
    igl->api->splf2s (n, parray, iarray);
}

void
iglExec_splf2s (long n, Scoord parray[][2], Colorindex iarray[])
{
    int i;

    IGL_CHECKWNDV ();

    /* "must be used in colormap mode" */
    if ((IGL_CTX ()->flags & IGL_WFLAGS_RGBA) && IGL_CTX ()->drawMode == NORMALDRAW)
        return;

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
        {
            color (iarray[i]);
            glVertex2sv (&parray[i][0]);
        }
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], 0);
}


#if 0
____________________________ polf functions ____________________________
#endif
/* GL: polf, polfi, polfs, polf2, polf2i, polf2s - draws a filled polygon */
void
polf (long n, const Coord parray[][3])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->polf (n, parray);
}

void
iglExec_polf (long n, const Coord parray[][3])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
            glVertex3fv (&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], parray[0][2]);
}


void
polfi (long n, const Icoord parray[][3])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->polfi (n, parray);
}

void
iglExec_polfi (long n, const Icoord parray[][3])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
            glVertex3iv ((GLint *)&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], parray[0][2]);
}


void
polfs (long n, const Scoord parray[][3])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->polfs (n, parray);
}

void
iglExec_polfs (long n, const Scoord parray[][3])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
            glVertex3sv (&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], parray[0][2]);
}


void
polf2 (long n, const Coord parray[][2])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->polf2 (n, parray);
}

void
iglExec_polf2 (long n, const Coord parray[][2])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
            glVertex2fv (&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], 0);
}


void
polf2i (long n, const Icoord parray[][2])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->polf2i (n, parray);
}

void
iglExec_polf2i (long n, const Icoord parray[][2])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
            glVertex2iv ((GLint *)&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], 0);
}


void
polf2s (long n, const Scoord parray[][2])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->polf2s (n, parray);
}

void
iglExec_polf2s (long n, const Scoord parray[][2])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_POLYGON);
        for (i=0; i < n; i ++)
            glVertex2sv (&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], 0);
}


#if 0
____________________________ poly functions ____________________________
#endif
/* GL: poly, polyi, polys, poly2, poly2i, poly2s - outlines a polygon */
void
poly (long n, const Coord parray[][3])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->poly (n, parray);
}

void
iglExec_poly (long n, const Coord parray[][3])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_LINE_LOOP);
        for (i=0; i < n; i ++)
            glVertex3fv (&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], parray[0][2]);
}


void
polyi (long n, const Icoord parray[][3])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->polyi (n, parray);
}

void
iglExec_polyi (long n, const Icoord parray[][3])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_LINE_LOOP);
        for (i=0; i < n; i ++)
            glVertex3iv ((GLint *)&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], parray[0][2]);
}


void
polys (long n, const Scoord parray[][3])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->polys (n, parray);
}

void
iglExec_polys (long n, const Scoord parray[][3])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_LINE_LOOP);
        for (i=0; i < n; i ++)
            glVertex3sv (&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], parray[0][2]);
}


void
poly2 (long n, const Coord parray[][2])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->poly2 (n, parray);
}

void
iglExec_poly2 (long n, const Coord parray[][2])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_LINE_LOOP);
        for (i=0; i < n; i ++)
            glVertex2fv (&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], 0);
}


void
poly2i (long n, const Icoord parray[][2])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->poly2i (n, parray);
}

void
iglExec_poly2i (long n, const Icoord parray[][2])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_LINE_LOOP);
        for (i=0; i < n; i ++)
            glVertex2iv ((GLint *)&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], 0);
}


void
poly2s (long n, const Scoord parray[][2])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,parray);
    igl->api->poly2s (n, parray);
}

void
iglExec_poly2s (long n, const Scoord parray[][2])
{
    int i;

    IGL_CHECKWNDV ();

    if (n < 2 || n > IGL_MAXPOLYVERTICES)
        return;

    IGL_BGNLIGHT ();
    glBegin (GL_LINE_LOOP);
        for (i=0; i < n; i ++)
            glVertex2sv (&parray[i][0]);
    glEnd ();
    IGL_ENDLIGHT ();

    _igl_setgpos (parray[0][0], parray[0][1], 0);
}


#if 0
____________________________ misc functions ____________________________
#endif
/* GL: pntsize, pntsizef - specifies size of points */
void
pntsizef (float n)
{
    _igl_trace (__func__,FALSE,"%g",n);
    igl->api->pntsizef (n);
}

void
iglExec_pntsizef (float n)
{
    IGL_CHECKWNDV ();

    glPointSize (n);
}


void
pntsize (short n)
{
    igl->api->pntsizef ((float)n);
}


/* GL: pntsmooth - specify antialiasing of points */
void
pntsmooth (unsigned long mode)
{
    _igl_trace (__func__,FALSE,"%lu",mode);
    igl->api->pntsmooth (mode);
}

void
iglExec_pntsmooth (unsigned long mode)
{
    IGL_CHECKWNDV ();

    if (mode == SMP_OFF)
        glDisable (GL_POINT_SMOOTH);
    else if (mode & SMP_ON)
    {
        glEnable (GL_POINT_SMOOTH);

        if (mode & SMP_SMOOTHER)
            glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);
        else
            glHint (GL_POINT_SMOOTH_HINT, GL_FASTEST);
    }
}


/* GL: linewidth, linewidthf - specifies width of lines */
void
linewidthf (float width)
{
    _igl_trace (__func__,FALSE,"%g",width);
    igl->api->linewidthf (width);
}

void
iglExec_linewidthf (float width)
{
    IGL_CHECKWNDV ();

    glLineWidth (width);
}


void
linewidth (short width)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->linewidthf ((float)width);
}


/* GL: getlwidth - returns the current linewidth */
long
getlwidth (void)
{
    int lw;

    IGL_CHECKWND (1);

    glGetIntegerv (GL_LINE_WIDTH, &lw);
    return (lw);
}


/* GL: linesmooth - specify antialiasing of lines */
void
linesmooth (unsigned long mode)
{
    _igl_trace (__func__,FALSE,"%lu",mode);
    igl->api->linesmooth (mode);
}

void
iglExec_linesmooth (unsigned long mode)
{
    IGL_CHECKWNDV ();

    if (mode == SML_OFF)
        glDisable (GL_LINE_SMOOTH);
    else if (mode & SML_ON)
    {
        glEnable (GL_LINE_SMOOTH);

        /* is this a good approximation? */
        if ((mode & SML_SMOOTHER) || (mode & SML_END_CORRECT))
            glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
        else
            glHint (GL_LINE_SMOOTH_HINT, GL_FASTEST);
    }
}


/* GL: smoothline - obsolete routine */
void
smoothline (long mode)
{
    _igl_trace (__func__,TRUE,"%ld",mode);
    igl->api->linesmooth ((unsigned long)mode);
}


/* GL: deflinestyle - defines a linestyle */
void
deflinestyle (short n, Linestyle ls)
{
    int idx;

    _igl_trace (__func__,TRUE,"%d,%u",n,ls);
    IGL_CHECKINIT ();

    idx = IGL_MAPID(igl->lineStyles, IGL_MAXLINESTYLES, n, 3);
    if (idx < 0)
        return;

    igl->lineStyles[idx].lineStyle = ls;
}


/* GL: setlinestyle - selects a linestyle pattern */
void
setlinestyle (short n)
{
    _igl_trace (__func__,FALSE,"%d",n);
    igl->api->setlinestyle (n);
}

void
iglExec_setlinestyle (short n)
{
    int idx;

    IGL_CHECKWNDV ();

    idx = IGL_MAPID(igl->lineStyles, IGL_MAXLINESTYLES, n, 0);
    if (idx < 0)
        return;

    if (n == 0)
    {
        /* solid line */
        glDisable (GL_LINE_STIPPLE);
    }
    else
    {
        glEnable (GL_LINE_STIPPLE);
        glLineStipple (1, igl->lineStyles[idx].lineStyle);
    }
    IGL_CTX ()->lineStyleIndex = idx;
}


/* GL: getlstyle - returns the current linestyle */
long
getlstyle (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    return (igl->lineStyles[IGL_CTX ()->lineStyleIndex].id);
}


/* GL: lsrepeat - sets a repeat factor for the current linestyle */
void
lsrepeat (long factor)
{
    _igl_trace (__func__,FALSE,"%ld",factor);
    igl->api->lsrepeat (factor);
}

void
iglExec_lsrepeat (long factor)
{
    IGL_CHECKWNDV ();

    if (factor > 0 && factor < 256)
        glLineStipple (factor, igl->lineStyles[IGL_CTX ()->lineStyleIndex].lineStyle);
}


/* GL: getlsrepeat - returns the linestyle repeat count */
long
getlsrepeat (void)
{
    int lsr;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (1);

    glGetIntegerv (GL_LINE_STIPPLE_REPEAT, &lsr);
    return (lsr);
}


/* GL: polymode - control the rendering of polygons */
void
polymode (long mode)
{
    _igl_trace (__func__,FALSE,"%ld",mode);
    igl->api->polymode (mode);
}

void
iglExec_polymode (long mode)
{
    IGL_CHECKWNDV ();

    if (mode == PYM_FILL)
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    else if (mode == PYM_POINT)
        glPolygonMode (GL_FRONT_AND_BACK, GL_POINT);
    /* PYM_HOLLOW not supported by OpenGL */
    else if (mode == PYM_LINE || mode == PYM_HOLLOW)
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
}


/* GL: polysmooth - specify antialiasing of polygons */
void
polysmooth (long mode)
{
    _igl_trace (__func__,FALSE,"%ld",mode);
    igl->api->polysmooth (mode);
}

void
iglExec_polysmooth (long mode)
{
    IGL_CHECKWNDV ();

    if (mode == PYSM_OFF)
        glDisable (GL_POLYGON_SMOOTH);
    /* PYSM_SHRINK not supported in OpenGL */
    else if (mode == PYSM_ON || mode == PYSM_SHRINK)
    {
        glEnable (GL_POLYGON_SMOOTH);
        glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    }
}


/* GL: displacepolygon - specifies a displacement for the z values of rendered polygons */
void
displacepolygon (float scalefactor)
{
    _igl_trace (__func__,TRUE,"%g",scalefactor);
    igl->api->displacepolygon (scalefactor);
}

void
iglExec_displacepolygon (float scalefactor)
{
    IGL_CHECKWNDV ();

    /* TODO: really set all off these offsets? */
    if (scalefactor != 0)
    {
        glEnable (GL_POLYGON_OFFSET_FILL);
        glEnable (GL_POLYGON_OFFSET_LINE);
        glEnable (GL_POLYGON_OFFSET_POINT);
    }
    else
    {
        glDisable (GL_POLYGON_OFFSET_FILL);
        glDisable (GL_POLYGON_OFFSET_LINE);
        glDisable (GL_POLYGON_OFFSET_POINT);
    }
    glPolygonOffset (scalefactor, 0);
}


/* GL: backface - turns backfacing polygon removal on and off */
void
backface (Boolean enable)
{
    _igl_trace (__func__,FALSE,"%d",enable);
    igl->api->backface (enable);
}

void
iglExec_backface (Boolean enable)
{
    int cm;

    IGL_CHECKWNDV ();

    glGetIntegerv (GL_CULL_FACE_MODE, &cm);
    if (enable)
    {
        glEnable (GL_CULL_FACE);
        if (cm == GL_FRONT)
            glCullFace (GL_FRONT_AND_BACK);
    }
    else
    {
        if (cm == GL_FRONT_AND_BACK)
            glCullFace (GL_FRONT);
        else if (cm == GL_BACK)
            glDisable (GL_CULL_FACE);
    }
}


/* GL: getbackface - returns whether backfacing polygons will appear */
long
getbackface (void)
{
    int cm;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    glGetIntegerv (GL_CULL_FACE_MODE, &cm);

    return (cm == GL_BACK || cm == GL_FRONT_AND_BACK);
}


/* GL: frontface - turns frontfacing polygon removal on and off */
void
frontface (Boolean enable)
{
    _igl_trace (__func__,FALSE,"%d",enable);
    igl->api->frontface (enable);
}

void
iglExec_frontface (Boolean enable)
{
    int cm;

    IGL_CHECKWNDV ();

    glGetIntegerv (GL_CULL_FACE_MODE, &cm);
    if (enable)
    {
        glEnable (GL_CULL_FACE);
        if (cm == GL_BACK)
            glCullFace (GL_FRONT_AND_BACK);
    }
    else
    {
        if (cm == GL_FRONT_AND_BACK)
            glCullFace (GL_BACK);
        else if (cm == GL_FRONT)
            glDisable (GL_CULL_FACE);
    }
}


/* GL: defpattern - defines patterns */
void
defpattern (short n, short size, unsigned short mask[])
{
    int i, idx;
    GLubyte *omask;
     
    _igl_trace (__func__,TRUE,"%d,%d,%p",n,size,mask);
    IGL_CHECKINIT ();

    idx = IGL_MAPID(igl->patterns, IGL_MAXPATTERNS, n, 3);
    if (idx < 0)
        return;

    /* OpenGL only allows one pattern size (32x32 pixels), so we resize */
    omask = igl->patterns[idx].pattern;
    switch (size)
    {
        case 16:
            for (i=0; i < 16; i++)
            {
                omask[4*i + 0] = mask[i] >> 8;
                omask[4*i + 1] = mask[i];
                omask[4*i + 2] = mask[i] >> 8;
                omask[4*i + 3] = mask[i];
                omask[4*i + 64] = mask[i] >> 8;
                omask[4*i + 65] = mask[i];
                omask[4*i + 66] = mask[i] >> 8;
                omask[4*i + 67] = mask[i];
            }
        break;

        case 32:
            for (i=0; i < 64; i++)
            {
                omask[2*i + 0] = mask[i] >> 8;
                omask[2*i + 1] = mask[i];
            }
        break;

        default:
            memset (omask, 0, sizeof(igl->patterns[idx].pattern));
        break;
    }
}


/* GL: setpattern - selects a pattern for filling polygons and rectangles */
void
setpattern (short index)
{
    _igl_trace (__func__,FALSE,"%d",index);
    igl->api->setpattern (index);
}

void
iglExec_setpattern (short index)
{
    int idx;

    IGL_CHECKWNDV ();

    idx = IGL_MAPID(igl->patterns, IGL_MAXPATTERNS, index, 0);
    if (idx < 0)
        return;

    if (index == 0)
        glDisable (GL_POLYGON_STIPPLE);
    else
    {
        glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 0);
        glPolygonStipple (igl->patterns[idx].pattern);
        glEnable (GL_POLYGON_STIPPLE);
    }
    IGL_CTX ()->patternIndex = idx;
}


/* GL: getpattern - returns the index of the current pattern */
long
getpattern (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    return (igl->patterns[IGL_CTX ()->patternIndex].id);
}


/* GL: unimplemented stuff */
void
lsbackup (Boolean b)
{
    _igl_trace (__func__,TRUE,"%d",b);
}


Boolean
getlsbackup (void)
{
    _igl_trace (__func__,TRUE,"");
    return (FALSE);
}


void
resetls (Boolean b)
{
    _igl_trace (__func__,TRUE,"%d",b);
}


Boolean
getresetls (void)
{
    _igl_trace (__func__,TRUE,"");
    return (FALSE);
}


void
scrsubdivide (long mode, float param[])
{
    _igl_trace (__func__,TRUE,"");
}


void
concave (Boolean enable)
{
    _igl_trace (__func__,TRUE,"");
}
