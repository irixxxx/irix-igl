/*
 * draw.c
 *
 * old-style drawing functions (not determining the graphics position since
 * they calculate their own vertices): rect, sbox, arc, circ, curves, patches
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
____________________________ rect functions ____________________________
#endif
/* GL: rect, recti, rects - outlines a rectangular region */
void
rect (Coord a, Coord b, Coord c, Coord d)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%g",a,b,c,d);
    igl->api->rect (a, b, c, d);
}

void
iglExec_rect (Coord a, Coord b, Coord c, Coord d)
{
    IGL_CHECKWNDV ();

    IGL_BGNLIGHT ();
    glBegin (GL_LINE_LOOP);
        glVertex2f (a, b);
        glVertex2f (c, b);
        glVertex2f (c, d);
        glVertex2f (a, d);
    glEnd ();
    IGL_ENDLIGHT ();
}


/* NOTE: rendering polygons with identical corners leads to different results
 * in IrisGL (1x1 pixels) and OpenGL (no pixels), due to OpenGL using the
 * diamond rule. For rectangles add some epsilon in integer cases which
 * normally render to an ortho projection.
 */
void
recti (Icoord a, Icoord b, Icoord c, Icoord d)
{
    Coord af = (Coord)a + (a < c ? -0.5 : +0.5);
    Coord bf = (Coord)b + (b < d ? -0.5 : +0.5);
    Coord cf = (Coord)c + (a < c ? +0.5 : -0.5);
    Coord df = (Coord)d + (b < d ? +0.5 : -0.5);
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",a,b,c,d);
    igl->api->rect ((Coord)af, (Coord)bf, (Coord)cf, (Coord)df);
}


void
rects (Scoord a, Scoord b, Scoord c, Scoord d)
{
    Coord af = (Coord)a + (a < c ? -0.5 : +0.5);
    Coord bf = (Coord)b + (b < d ? -0.5 : +0.5);
    Coord cf = (Coord)c + (a < c ? +0.5 : -0.5);
    Coord df = (Coord)d + (b < d ? +0.5 : -0.5);
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",a,b,c,d);
    igl->api->rect ((Coord)af, (Coord)bf, (Coord)cf, (Coord)df);
}


/* GL: rectf, rectfi, rectfs - fills a rectangular area */
void
rectf (Coord a, Coord b, Coord c, Coord d)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%g",a,b,c,d);
    igl->api->rectf (a, b, c, d);
}


void
iglExec_rectf (Coord a, Coord b, Coord c, Coord d)
{
    IGL_CHECKWNDV ();

    IGL_BGNLIGHT ();
    glRectf (a, b, c, d);
    IGL_ENDLIGHT ();
}


void
rectfi (Icoord a, Icoord b, Icoord c, Icoord d)
{
    Coord af = (Coord)a + (a < c ? -0.5 : +0.5);
    Coord bf = (Coord)b + (b < d ? -0.5 : +0.5);
    Coord cf = (Coord)c + (a < c ? +0.5 : -0.5);
    Coord df = (Coord)d + (b < d ? +0.5 : -0.5);
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",a,b,c,d);
    igl->api->rectf ((Coord)af, (Coord)bf, (Coord)cf, (Coord)df);
}


void
rectfs (Scoord a, Scoord b, Scoord c, Scoord d)
{
    Coord af = (Coord)a + (a < c ? -0.5 : +0.5);
    Coord bf = (Coord)b + (b < d ? -0.5 : +0.5);
    Coord cf = (Coord)c + (a < c ? +0.5 : -0.5);
    Coord df = (Coord)d + (b < d ? +0.5 : -0.5);
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",a,b,c,d);
    igl->api->rectf ((Coord)af, (Coord)bf, (Coord)cf, (Coord)df);
}


/* NOTE: sbox edge coordinates are projected, but the rectangle is then drawn
 * orthogonally to the screen. Project the edges to window coordinates, then use
 * an ortho projection to draw the actual rectangle. Better ideas very welcome!
 */

/* GL: sbox, sboxi, sboxs - draw a screen-aligned rectangle */
void
sbox (Coord a, Coord b, Coord c, Coord d)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%g",a,b,c,d);
    igl->api->sbox (a, b, c, d);
}

void
iglExec_sbox (Coord a, Coord b, Coord c, Coord d)
{
    GLdouble sx, sy, sz, tx, ty, tz;
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* get coordinates of corners */
    _igl_getViewport ();
    _igl_getMatrices ();
    gluProject (a, b, 0, wptr->modelMatrix, wptr->projMatrix, wptr->viewport, &sx, &sy, &sz);
    gluProject (c, d, 0, wptr->modelMatrix, wptr->projMatrix, wptr->viewport, &tx, &ty, &tz);

    /* save status and load ortho projection */
    _igl_pushTransform ();
    _igl_ortho2D (0);

    /* this is always unlighted */
    IGL_DISLIGHT ();
    glBegin (GL_LINE_LOOP);
        glVertex2f (sx, sy);
        glVertex2f (tx, sy);
        glVertex2f (tx, ty);
        glVertex2f (sx, ty);
    glEnd ();

    /* restore status */
    _igl_popTransform ();
}


void
sboxi (Icoord a, Icoord b, Icoord c, Icoord d)
{
    Coord af = (Coord)a + (a < c ? -0.5 : +0.5);
    Coord bf = (Coord)b + (b < d ? -0.5 : +0.5);
    Coord cf = (Coord)c + (a < c ? +0.5 : -0.5);
    Coord df = (Coord)d + (b < d ? +0.5 : -0.5);
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",a,b,c,d);
    igl->api->sbox ((Coord)af, (Coord)bf, (Coord)cf, (Coord)df);
}

void
sboxs (Scoord a, Scoord b, Scoord c, Scoord d)
{
    Coord af = (Coord)a + (a < c ? -0.5 : +0.5);
    Coord bf = (Coord)b + (b < d ? -0.5 : +0.5);
    Coord cf = (Coord)c + (a < c ? +0.5 : -0.5);
    Coord df = (Coord)d + (b < d ? +0.5 : -0.5);
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",a,b,c,d);
    igl->api->sbox ((Coord)af, (Coord)bf, (Coord)cf, (Coord)df);
}


/* GL: sboxf, sboxfi, sboxfs - draw a filled screen-aligned rectangle */
void
sboxf (Coord a, Coord b, Coord c, Coord d)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%g",a,b,c,d);
    igl->api->sboxf (a, b, c, d);
}

void
iglExec_sboxf (Coord a, Coord b, Coord c, Coord d)
{
    GLdouble sx, sy, sz, tx, ty, tz;
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* get coordinates of corners */
    _igl_getViewport ();
    _igl_getMatrices ();
    gluProject (a, b, 0, wptr->modelMatrix, wptr->projMatrix, wptr->viewport, &sx, &sy, &sz);
    gluProject (c, d, 0, wptr->modelMatrix, wptr->projMatrix, wptr->viewport, &tx, &ty, &tz);

    /* save status and load ortho projection */
    _igl_pushTransform ();
    _igl_ortho2D (0);

    /* this is always unlighted */
    IGL_DISLIGHT ();
    glRectf (sx, sy, tx, ty);

    /* restore status */
    _igl_popTransform ();
}


void
sboxfi (Icoord a, Icoord b, Icoord c, Icoord d)
{
    Coord af = (Coord)a + (a < c ? -0.5 : +0.5);
    Coord bf = (Coord)b + (b < d ? -0.5 : +0.5);
    Coord cf = (Coord)c + (a < c ? +0.5 : -0.5);
    Coord df = (Coord)d + (b < d ? +0.5 : -0.5);
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",a,b,c,d);
    igl->api->sboxf ((Coord)af, (Coord)bf, (Coord)cf, (Coord)df);
}

void
sboxfs (Scoord a, Scoord b, Scoord c, Scoord d)
{
    Coord af = (Coord)a + (a < c ? -0.5 : +0.5);
    Coord bf = (Coord)b + (b < d ? -0.5 : +0.5);
    Coord cf = (Coord)c + (a < c ? +0.5 : -0.5);
    Coord df = (Coord)d + (b < d ? +0.5 : -0.5);
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",a,b,c,d);
    igl->api->sboxf ((Coord)af, (Coord)bf, (Coord)cf, (Coord)df);
}


#if 0
____________________________ arc functions ____________________________
#endif
/*
    NOTE on arc()'s in OpenGL:
    1. angles are in degrees (IrisGL - tenths of degrees)
    2. start angle is measured from positive y-axis (IrisGL - positive x-axis)
    3. arc is drawn clockwise (IrisGL - counterclockwise)
*/

/* draw a filled or unfilled arc */
static void
_igl_drawarc (float ox, float oy, float radius, int segs, int filled, Angle start, Angle end) 
{ 
    Angle diff = (end - start) % 3600;  /* limit width to full circle */
    float delta = 2 * M_PI / segs; 
    float c = cosf(delta), s = sinf(delta);
    float dx, dy;

    /* map IrisGL angles to OpenGL by mirroring at f(x)=x (45 degrees) */
    start = -start + 900;
    end   = -end + 900;
    /* scale #segs down to width of arc */
    diff += (diff < 0 ? 3600 : 0);
    segs = segs * diff / 3600.0;
    /* vector of length radius, pointing at start angle */
    dx = sinf(start / 1800.0 * M_PI) * radius;
    dy = cosf(start / 1800.0 * M_PI) * radius;
    /* startpoint of arc (center) */
    glBegin(filled ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
    glVertex2f (ox, oy);
    /* draw the arc around (ox,oy) by applying a 2D rotation matrix to dx/dy */
    while (segs-- > 0)
    { 
        float tx = dx;
        glVertex2f (ox + dx, oy + dy);
        dx = c*dx - s*dy;
        dy = s*tx + c*dy;
    } 
    /* endpoint of arc */
    dx = sinf(end / 1800.0 * M_PI) * radius;
    dy = cosf(end / 1800.0 * M_PI) * radius;
    glVertex2f (ox + dx, oy + dy);
    glEnd(); 
}

/* GL: arc, arci, arcs - draw a circular arc */
void
arc (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%d,%d",x,y,radius,startAngle,endAngle);
    igl->api->arc (x, y, radius, startAngle, endAngle);
}

void
iglExec_arc (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle)
{
    IGL_CHECKWNDV ();

    if (radius <= 0)
        return;

    IGL_BGNLIGHT ();
    _igl_drawarc (x, y, radius, 64, 0, startAngle, endAngle);
    IGL_ENDLIGHT ();
}


void
arci (Icoord x, Icoord y, Icoord radius, Angle startAngle, Angle endAngle)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d,%d",x,y,radius,startAngle,endAngle);
    igl->api->arc ((Coord)x, (Coord)y, (Coord)radius+0.5, startAngle, endAngle);
}


void
arcs (Scoord x, Scoord y, Scoord radius, Angle startAngle, Angle endAngle)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d,%d",x,y,radius,startAngle,endAngle);
    igl->api->arc ((Coord)x, (Coord)y, (Coord)radius+0.5, startAngle, endAngle);
}


/* GL: arcf, arcfi, arcfs - draw a filled circular arc */
void
arcf (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%d,%d",x,y,radius,startAngle,endAngle);
    igl->api->arcf (x, y, radius, startAngle, endAngle);
}

void
iglExec_arcf (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle)
{
    IGL_CHECKWNDV ();

    if (radius <= 0)
        return;

    IGL_BGNLIGHT ();
    _igl_drawarc (x, y, radius, 64, 1, startAngle, endAngle);
    IGL_ENDLIGHT ();
}


void
arcfi (Icoord x, Icoord y, Icoord radius, Angle startAngle, Angle endAngle)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d,%d",x,y,radius,startAngle,endAngle);
    igl->api->arcf ((Coord)x, (Coord)y, (Coord)radius+0.5, startAngle, endAngle);
}


void
arcfs (Scoord x, Scoord y, Scoord radius, Angle startAngle, Angle endAngle)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d,%d",x,y,radius,startAngle,endAngle);
    igl->api->arcf ((Coord)x, (Coord)y, (Coord)radius+0.5, startAngle, endAngle);
}


#if 0
____________________________ circ functions ____________________________
#endif
/* draw a filled or unfilled circle */
static void
_igl_drawcircle (float ox, float oy, float radius, int segs, int filled) 
{ 
    float ang = 2 * M_PI / segs; 
    float c = cosf(ang), s = sinf(ang);
    float dx = 0, dy = radius; /* vector of length radius */

    /* draw a circle around (ox,oy) by applying a 2D rotation matrix to dx/dy */
    glBegin(filled ? GL_POLYGON : GL_LINE_LOOP); 
    while (--segs > 0)
    { 
        float tx = dx;
        glVertex2f (ox + dx, oy + dy);
        dx = c*dx - s*dy;
        dy = s*tx + c*dy;
    } 
    glEnd(); 
}

/* GL: circ, circi, circs - outlines a circle */
void
circ (Coord x, Coord y, Coord radius)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g",x,y,radius);
    igl->api->circ (x, y, radius);
}

void
iglExec_circ (Coord x, Coord y, Coord radius)
{
    IGL_CHECKWNDV ();

    if (radius <= 0)
        return;

    IGL_BGNLIGHT ();
    _igl_drawcircle (x, y, radius, 64, 0);
    IGL_ENDLIGHT ();
}


void
circi (Icoord x, Icoord y, Icoord radius)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d",x,y,radius);
    igl->api->circ ((float)x, (float)y, (float)radius+0.5);
}


void
circs (Scoord x, Scoord y, Scoord radius)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d",x,y,radius);
    igl->api->circ ((float)x, (float)y, (float)radius+0.5);
}


/* GL: circf, circfi, circfs - draws a filled circle */
void
circf (Coord x, Coord y, Coord radius)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g",x,y,radius);
    igl->api->circf (x, y, radius);
}

void
iglExec_circf (Coord x, Coord y, Coord radius)
{
    IGL_CHECKWNDV ();

    if (radius <= 0)
        return;

    IGL_BGNLIGHT ();
    _igl_drawcircle (x, y, radius, 64, 1);
    IGL_ENDLIGHT ();
}


void
circfi (Icoord x, Icoord y, Icoord radius)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d",x,y,radius);
    igl->api->circf ((float)x, (float)y, (float)radius+0.5);
}


void
circfs (Scoord x, Scoord y, Scoord radius)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d",x,y,radius);
    igl->api->circf ((float)x, (float)y, (float)radius+0.5);
}


#if 0
____________________________ curve/patch functions ____________________________
#endif
/* TODO: IrisGL has parametric curves using basis matrices transforming the
 * power basis, OpenGL has a fixed Bernstein polynomials basis. Let's think
 * about this some:
 *
 * Bernstein basis matrix B:| -1   3  -3   1 |, inverse B":  | 0   0   0   1 |
 *                          |  3  -6   3   0 |               | 0   0  1/3  1 |
 *                          | -3   3   0   0 |               | 0  1/3 2/3  1 |
 *                          |  1   0   0   0 |               | 1   1   1   1 |
 * for curves:
 * IrisGl:                              OpenGL:
 * Px(t) = T' M X,                      Px(t) = T' B X,         y,z,w likewise.
 * with X=vector of control point x values (4x1), M=selected basis matrix (4x4),
 *      T = vector of t^3..t^0 (4x1), Px(t) = x value of curve point
 * Px(t) = T' M X = T' B B" M X = T' B (B" M X)     
 *
 * for patches:
 * Px(u,v) = V Mv X Mu' U'              Px(u,v) = V B X B' U'   y,z,w likewise
 * with X=matrix of control point x values (4x4), M=selected basis matrix (4x4),
 *      U = vector of u^3..u^0, V = vector of v^3..v^0, Px(u,v) = x value
 * Px(u,v) = V B B" Mv X Mu' B"' B' U' = V B (B" Mv X Mu' B"') B' U'
 *
 * So, transforming the control points by multiplying with the basis matrices
 * and the inverse Bernstein matrix according to the above equations should do
 * the trick for both curves and patches.
 *
 * A working test implementation of the concept is #ifdef'ed with TEST_EVAL.
 *
 * Meanwhile, this is done by hand as documented in SGIs and IBMs books.
 */
#define TEST_EVAL

/* multXnYm - curve/patch helper for matrix/vector multiplying */
static void
multV4M3 (float *r, float *v, float *m)
{   /* [r0..r2]' = [v0..v3]' * [m00..m32] */
    int i;

    for (i = 0; i < 3; i++, r++, m++)
        r[0] = v[0]*m[0] + v[1]*m[3] + v[2]*m[6] + v[3]*m[9];
}

static void
multM4M3 (float *r, float *m, float *p)
{   /* [r00..r32] = [m00..m33] * [p00..p32] */
    int i;

    for (i = 0; i < 4; i++, r += 3, m += 4)
        multV4M3 (r, m, p);
}

static void
multV4M4 (float *r, float *v, float *m)
{   /* [r0..r3]' = [v0..v3]' * [m00..m33] */
    int i;

    for (i = 0; i < 4; i++, r++, m++)
        r[0] = v[0]*m[0] + v[1]*m[4] + v[2]*m[8] + v[3]*m[12];
}

static void
multM4M4 (float *r, float *m, float *p)
{   /* [r00..r33] = [m00..m33] * [p00..p33] */
    int i;

    for (i = 0; i < 4; i++, r += 4, m += 4)
        multV4M4 (r, m, p);
}

#ifdef TEST_EVAL
static void
transposeM4 (float *r, float *m)
{
    int i;

    for (i = 0; i < 4; i++, r += 4, m += 1) {
        r[0] = m[0];
        r[1] = m[4];
        r[2] = m[8];
        r[3] = m[12];
    }
}

static GLfloat Binv[16] = { 0.0,   0.0,   0.0, 1.0,
                            0.0,   0.0, 1/3.0, 1.0,
                            0.0, 1/3.0, 2/3.0, 1.0,
                            1.0,   1.0,   1.0, 1.0 };
#else
static void
multV4V4 (float *r, float *v, float *w)
{   /* r = [v0..v3]' * [w0..w3] */
    r[0] = v[0]*w[0] + v[1]*w[1] + v[2]*w[2] + v[3]*w[3];
}

static void
initV4 (float *v, float t)
{   /* [v0..v3] = [t^3..t^0] */
    v[3] = 1, v[2] = t, v[1] = t*t, v[0] = t*t*t;
}
#endif

/* GL: crvn - draws a series of curve segments */
void
crvn (long n, Coord points[][3])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,points);
    igl->api->crvn (n, points);
}

void
iglExec_crvn (long n, Coord points[][3])
{
    igl_windowT *wptr;
    float *B;
    int i;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    B = igl->matrixDefs[wptr->curveBasisIndex].matrix;
    IGL_BGNLIGHT ();
    for (i = 0; i <= n-4; i++, points++)
    {
#ifdef TEST_EVAL
        GLfloat control[12], Bp[12];

        /* control = B" * M * X */
        multM4M3 (Bp, B, *points);
        multM4M3 (control, Binv, Bp );

        glEnable (GL_MAP1_VERTEX_3);
        glMap1f (GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, control);
        glMapGrid1f (wptr->curveSegments, 0.0, 1.0);
        glEvalMesh1 (GL_LINE, 0, wptr->curveSegments);
        glDisable (GL_MAP1_VERTEX_3);
#else
        float M[12];    /* holds B*P */
        int j;

        /* C = T' * B * P */
        multM4M3 (M, B, *points);
        glBegin (GL_LINE_STRIP);
        for (j = 0; j <= wptr->curveSegments; j++)
        {
            float t = (float)j / wptr->curveSegments;
            float T[4], C[3]; /* holds T'*M = T'*(B*P) */
            initV4(T, t);
            multV4M3(C, T, M);
            glVertex3fv (C);
        }
        glEnd ();
#endif
    }
    IGL_ENDLIGHT ();
}


/* GL: crv - draws a curve */
void
crv (Coord points[4][3])
{
    _igl_trace (__func__,FALSE,"%p",points);
    igl->api->crvn (4, points);
}


/* GL: rcrvn - draws a series of curve segments */
void
rcrvn (long n, Coord points[][4])
{
    _igl_trace (__func__,FALSE,"%ld,%p",n,points);
    igl->api->rcrvn (n, points);
}

void
iglExec_rcrvn (long n, Coord points[][4])
{
    igl_windowT *wptr;
    float *B;
    int i;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    IGL_BGNLIGHT ();
    B = igl->matrixDefs[wptr->curveBasisIndex].matrix;
    for (i = 0; i <= n-4; i++, points++)
    {
#ifdef TEST_EVAL
        GLfloat control[16], Bp[16];

        /* control = B" * M * X */
        multM4M3 (Bp, B, *points);
        multM4M3 (control, Binv, Bp );

        glEnable (GL_MAP1_VERTEX_4);
        glMap1f (GL_MAP1_VERTEX_4, 0.0, 1.0, 4, 4, control);
        glMapGrid1f (wptr->curveSegments, 0.0, 1.0);
        glEvalMesh1 (GL_LINE, 0, wptr->curveSegments);
        glDisable (GL_MAP1_VERTEX_4);
#else
        float M[16];    /* holds B*P */
        int j;

        /* C = T' * B * P */
        multM4M4 (M, B, *points);
        glBegin (GL_LINE_STRIP);
        for (j = 0; j <= wptr->curveSegments; j++)
        {
            float t = (float)j / wptr->curveSegments;
            float T[4], C[4]; /* holds T'*M = T'(B*P) */
            initV4(T, t);
            multV4M4(C, T, M);
            glVertex4fv (C);
        }
        glEnd ();
#endif
    }
    IGL_ENDLIGHT ();
}


/* GL: rcrv - draws a rational curve */
void
rcrv (Coord points[4][4])
{
    _igl_trace (__func__,FALSE,"%p",points);
    igl->api->rcrvn (4, points);
}


/* GL: curveprecision - sets number of line segments used to draw a curve segment */
void
curveprecision (short n)
{
    _igl_trace (__func__,FALSE,"%d",n);
    igl->api->curveprecision (n);
}

void
iglExec_curveprecision (short n)
{
    IGL_CHECKWNDV ();

    if (n < 0 || n > IGL_MAXCURVESEGMENTS)
        return;

    IGL_CTX ()->curveSegments = n;
}


/* GL: curvebasis - selects current basis matrix */
void
curvebasis (short id)
{
    _igl_trace (__func__,FALSE,"%d",id);
    igl->api->curvebasis (id);
}

void
iglExec_curvebasis (short id)
{
    int idx;

    IGL_CHECKWNDV ();

    idx = IGL_MAPID(igl->matrixDefs, IGL_MAXMATRICES, id, 0);
    if (idx < 0)
        return;

    IGL_CTX ()->curveBasisIndex = idx;
}


#ifndef TEST_EVAL
/* create vertex for a patch */
static void
_igl_patchvertex (float *UBu, float *VBv, Matrix gx, Matrix gy, Matrix gz, Matrix gw)
{
    float C[4], D[4];

    /* C[x,y,z,w] = (V * Bv) * [X,Y,Z,W] * (U * Bu)' = VBv * [X,Y,Z,W] * UBu' */
    multV4M4 (D, VBv, (float *)gx);
    multV4V4 (&C[0], D, UBu);
    multV4M4 (D, VBv, (float *)gy);
    multV4V4 (&C[1], D, UBu);
    multV4M4 (D, VBv, (float *)gz);
    multV4V4 (&C[2], D, UBu);
    if (gw)
    {
        multV4M4 (D, VBv, (float *)gw);
        multV4V4 (&C[3], D, UBu);
        glVertex4fv (C);
    }
    else
    {
        glVertex3fv (C);
    }
}
#endif

/* draw a patch */
static void
_igl_drawpatch (Matrix gx, Matrix gy, Matrix gz, Matrix gw)
{
    igl_windowT *wptr = IGL_CTX ();
    float *Bu = igl->matrixDefs[wptr->patchBasisIndexU].matrix;
    float *Bv = igl->matrixDefs[wptr->patchBasisIndexV].matrix;
    int i;

#ifdef TEST_EVAL
    GLfloat control[4*16], BMu[16], BMv[16], tmp1[16], tmp2[16];
    GLint oglmode = (gw ? GL_MAP2_VERTEX_4 : GL_MAP2_VERTEX_3);
    GLint oglsize = (gw ? 4 : 3);

    /* BMv = B" * Mv, BMu = Mu' * B"', (note that B"' = B") */
    transposeM4 (tmp1, Bu);
    multM4M4 (BMu, tmp1, Binv);
    multM4M4 (BMv, Binv, Bv);

    /* controlx = B" * Mv * X * Mu' * B"' = BMv * X * BMu, y,z,w likewise */
    multM4M4 (tmp1, *gx, BMu);
    multM4M4 (tmp2, BMv, tmp1);
    for (i = 0; i < 16; i++)
        control[oglsize*i+0] = tmp2[i];

    multM4M4 (tmp1, *gy, BMu);
    multM4M4 (tmp2, BMv, tmp1);
    for (i = 0; i < 16; i++)
        control[oglsize*i+1] = tmp2[i];

    multM4M4 (tmp1, *gz, BMu);
    multM4M4 (tmp2, BMv, tmp1);
    for (i = 0; i < 16; i++)
        control[oglsize*i+2] = tmp2[i];

    if (gw) {
        multM4M4 (tmp1, *gw, BMu);
        multM4M4 (tmp2, BMv, tmp1);
        for (i = 0; i < 16; i++)
            control[oglsize*i+3] = tmp2[i];
    }

    IGL_BGNLIGHT ();
    glEnable (oglmode);
    glMap2f (oglmode, 0.0, 1.0, oglsize, 4, 0.0, 1.0, oglsize * 4, 4, control);
    glMapGrid2f (wptr->patchCurvesU, 0.0, 1.0, wptr->patchCurvesV, 0.0, 1.0);
    glEvalMesh2 (GL_LINE, 0, wptr->patchCurvesU, 0, wptr->patchCurvesV);
    glDisable (oglmode);
    IGL_ENDLIGHT ();
#else
    int j;

    /* C[x] = V * Bv * X * Bu' * U' = (V * Bv) * X * (U * Bu)', yzw likewise */
    IGL_BGNLIGHT ();
    /* draw curves in V direction */
    for (i = 0; i <= wptr->patchCurvesU; i++)
    {
        float u = (float)i / wptr->patchCurvesU;
        float U[4], UBu[4];
        initV4 (U, u);
        multV4M4 (UBu, U, Bu);

        glBegin (GL_LINE_STRIP);
        for (j = 0; j <= wptr->patchSegmentsV; j++)
        {
            float v = (float)j / wptr->patchSegmentsV;
            float V[4], VBv[4];
            initV4 (V, v);
            multV4M4 (VBv, V, Bv);

            _igl_patchvertex (UBu, VBv, gx, gy, gz, gw);
        }
        glEnd ();
    }
    /* draw curves in U direction */
    for (j = 0; j <= wptr->patchCurvesV; j++)
    {
        float v = (float)j / wptr->patchCurvesV;
        float V[4], VBv[4];
        initV4 (V, v);
        multV4M4 (VBv, V, Bv);

        glBegin (GL_LINE_STRIP);
        for (i = 0; i <= wptr->patchSegmentsU; i++)
        {
            float u = (float)i / wptr->patchSegmentsU;
            float U[4], UBu[4];
            initV4 (U, u);
            multV4M4 (UBu, U, Bu);

            _igl_patchvertex (UBu, VBv, gx, gy, gz, gw);
        }
        glEnd ();
    }
    IGL_ENDLIGHT ();
#endif
}


/* GL: patch - draws a surface patch */
void
patch (Matrix geomx, Matrix geomy, Matrix geomz)
{
    _igl_trace (__func__,FALSE,"%p,%p,%p",geomx,geomy,geomz);
    igl->api->patch (geomx, geomy, geomz);
}

void
iglExec_patch (Matrix geomx, Matrix geomy, Matrix geomz)
{
    IGL_CHECKWNDV ();
 
    _igl_drawpatch (geomx, geomy, geomz, NULL);
}


/* GL: rpatch - draws a rational surface patch */
void
rpatch (Matrix geomx, Matrix geomy, Matrix geomz, Matrix geomw)
{
    _igl_trace (__func__,FALSE,"%p,%p,%p,%p",geomx,geomy,geomz,geomw);
    igl->api->rpatch (geomx, geomy, geomz, geomw);
}

void
iglExec_rpatch (Matrix geomx, Matrix geomy, Matrix geomz, Matrix geomw)
{
    IGL_CHECKWNDV ();
 
    _igl_drawpatch (geomx, geomy, geomz, geomw);
}


/* GL: patchcurves - sets the number of curves used to represent a patch */
void
patchcurves (short nu, short nv)
{
    _igl_trace (__func__,FALSE,"%d,%d",nu,nv);
    igl->api->patchcurves (nu, nv);
}

void
iglExec_patchcurves (short nu, short nv)
{
    IGL_CHECKWNDV ();

    if (nu < 2 || nu > IGL_MAXCURVESEGMENTS || nv < 2 || nv > IGL_MAXCURVESEGMENTS)
        return;

    IGL_CTX ()->patchCurvesU = nu;
    IGL_CTX ()->patchCurvesV = nv;
}


/* GL: patchprecision - sets the precision at which curves are drawn in a patch */
void
patchprecision (short nu, short nv)
{
    _igl_trace (__func__,FALSE,"%d,%d",nu,nv);
    igl->api->patchprecision (nu, nv);
}

void
iglExec_patchprecision (short nu, short nv)
{
    IGL_CHECKWNDV ();

    if (nu < 0 || nu > IGL_MAXCURVESEGMENTS || nv < 0 || nv > IGL_MAXCURVESEGMENTS)
        return;

    IGL_CTX ()->patchSegmentsU = nu;
    IGL_CTX ()->patchSegmentsV = nv;
}


/* GL: patchbasis - selects current basis matrix */
void
patchbasis (short uid, short vid)
{
    _igl_trace (__func__,FALSE,"%d,%d",uid,vid);
    igl->api->patchbasis (uid, vid);
}

void
iglExec_patchbasis (short uid, short vid)
{
    int uidx, vidx;

    IGL_CHECKWNDV ();

    uidx = IGL_MAPID(igl->matrixDefs, IGL_MAXMATRICES, uid, 0);
    vidx = IGL_MAPID(igl->matrixDefs, IGL_MAXMATRICES, vid, 0);
    if (uidx < 0 || vidx < 0)
        return;

    IGL_CTX ()->patchBasisIndexU = uidx;
    IGL_CTX ()->patchBasisIndexV = vidx;
}


/* GL: defbasis - defines a basis matrix */
void
defbasis (short id, Matrix mat)
{
    int idx;

    _igl_trace (__func__,TRUE,"%d,%p",id,mat);
    IGL_CHECKWNDV ();

    idx = IGL_MAPID(igl->matrixDefs, IGL_MAXMATRICES, id, 1);
    if (idx < 0)
        return;

    memcpy (igl->matrixDefs[idx].matrix, mat, 16*sizeof(float));
}


/* GL: curveit - draws a curve segment */
void
curveit (short niter)
{
    _igl_trace (__func__,FALSE,"%d",niter);
    igl->api->curveit (niter);
}

void
iglExec_curveit (short niter)
{
    float m[16];
    int i, j;

    IGL_CHECKWNDV ();

    /* get the matrix to operate on */
    switch (IGL_CTX ()->matrixMode)
    {
        case MVIEWING:
            glGetFloatv (GL_MODELVIEW_MATRIX, (GLfloat *)m);
            break;

        case MSINGLE:
        case MPROJECTION:
            glGetFloatv (GL_PROJECTION_MATRIX, (GLfloat *)m);
            break;

        case MTEXTURE:
            glGetFloatv (GL_TEXTURE_MATRIX, (GLfloat *)m);
            break;
    }
    /* replace with identity, as transformation is already included in matrix */
    glLoadIdentity ();

    /* forward difference algorithm */
    IGL_BGNLIGHT ();
    glBegin (GL_LINE_STRIP);
    glVertex3f (m[12]/m[15],m[13]/m[15],m[14]/m[15]);

    while (niter-- > 0)
    {
        for (j = 12; j > 0; j -= 4)
            for (i = 0; i < 4; i++)
                m[j+i] += m[j+i-4];
        glVertex3f (m[12]/m[15],m[13]/m[15],m[14]/m[15]);
    }
    glEnd ();
    IGL_ENDLIGHT ();

    /* replace identity with the resulting matrix */
    glLoadMatrixf (m);
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}
