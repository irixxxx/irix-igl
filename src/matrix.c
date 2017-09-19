/*
 * matrix.c
 *
 * matrix handling, picking and selecting
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


/* TODO: the OpenGL porting guide says that MSINGLE mode should be replaced by
 * glMatrixMode (GL_PROJECTION). However, only GL_MODELVIEW has a guaranteed
 * decent stack size, for GL_PROJECTION the min stack size is only 2. OTOH,
 * there are functions only using either of the matrices, hence simply replacing
 * GL_PROJECTION by GL_MODELVIEW wouldn't work. What to do?
 */

/* TODO: all layers of the same window share the matrix stack. The easiest way
 * to achieve this is to replicate all matrix operations on all layers.
 */


/* set matrix mode */
static void
_igl_setmmode (void)
{
    switch (IGL_CTX ()->matrixMode)
    {
        case MVIEWING:
            glMatrixMode (GL_MODELVIEW);
            break;

        case MSINGLE:
        case MPROJECTION:
            glMatrixMode (GL_PROJECTION);
            break;

        case MTEXTURE:
            glMatrixMode (GL_TEXTURE);
            break;
    }
}


/* push transformations onto their respective stacks */
void
_igl_pushTransform (void)
{
    glPushAttrib (GL_VIEWPORT_BIT | GL_SCISSOR_BIT);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glMatrixMode (GL_TEXTURE);
    glPushMatrix ();
}


/* pop transformations from their respective stacks */
void
_igl_popTransform (void)
{
    glMatrixMode (GL_TEXTURE);
    glPopMatrix ();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
    glPopAttrib ();
    _igl_setmmode ();
}


/* get the current OpenGL viewport */
void
_igl_getViewport (void)
{
    igl_windowT *wptr = IGL_CTX ();

    if (! (wptr->flags & IGL_WSTATE_CURVIEWPORT))
    {
        glGetIntegerv (GL_VIEWPORT, wptr->viewport);
        wptr->flags |= IGL_WSTATE_CURVIEWPORT;
    }
}


/* get the current OpenGL vertex matrices */
void
_igl_getMatrices (void)
{
    igl_windowT *wptr = IGL_CTX ();

    if (! (wptr->flags & IGL_WSTATE_CURMATRICES))
    {
        glGetDoublev (GL_MODELVIEW_MATRIX, wptr->modelMatrix);
        glGetDoublev (GL_PROJECTION_MATRIX, wptr->projMatrix);
        wptr->flags |= IGL_WSTATE_CURMATRICES;
    }
}


/* load base projection matrix (either identitiy or pick matrix) */
static void
_igl_loadProjBase (void)
{
    if (! (IGL_CTX ()->state & IGL_WSTATE_PICKING))
        glLoadIdentity ();
    else
        glLoadMatrixf (IGL_CTX ()->pickMatrix);
}


/* GL: mmode - sets the current matrix mode */
void
mmode (short mode)
{
    _igl_trace (__func__,FALSE,"%d",mode);
    igl->api->mmode (mode);
}

void
iglExec_mmode (short mode)
{
    IGL_CHECKWNDV ();

    if (mode != MSINGLE && mode != MVIEWING && mode != MPROJECTION && mode != MTEXTURE)
        return;

    /* when entering/leaving MSINGLE, all matrix stacks are reset */
    if ((IGL_CTX ()->matrixMode == MSINGLE) != (mode == MSINGLE))
    {
        glMatrixMode (GL_MODELVIEW);
        while (glGetError () == GL_NO_ERROR)
            glPopMatrix ();
        glLoadIdentity ();

        glMatrixMode (GL_PROJECTION);
        while (glGetError () == GL_NO_ERROR)
            glPopMatrix ();
        glLoadIdentity ();

        glMatrixMode (GL_TEXTURE);
        while (glGetError () == GL_NO_ERROR)
            glPopMatrix ();
        glLoadIdentity ();

        IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
    }

    IGL_CTX ()->matrixMode = mode;
    _igl_setmmode ();
}


/* GL: getmmode - returns the current matrix mode */
long
getmmode (void)
{
    long mode;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (MSINGLE);

    mode = IGL_CTX ()->matrixMode;
    _igl_trace (" = ",TRUE,"%ld",mode);
    return (mode);
}


/* GL: loadmatrix - loads a transformation matrix */
void
loadmatrix (Matrix m)
{
    _igl_trace (__func__,FALSE,IGL_MATFMT(g),IGL_MATPAR(m));
    igl->api->loadmatrix (m);
}

void
iglExec_loadmatrix (Matrix m)
{
    IGL_CHECKWNDV ();

    if (IGL_CTX ()->matrixMode == MSINGLE || IGL_CTX ()->matrixMode == MPROJECTION)
    {
        _igl_loadProjBase ();
        glMultMatrixf ((GLfloat *)m);
    }
    else
        glLoadMatrixf ((GLfloat *)m);
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: getmatrix - returns a copy of a transformation matrix */
void
getmatrix (Matrix m)
{
    IGL_CHECKWNDV ();

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

    _igl_trace (__func__,TRUE,IGL_MATFMT(g),IGL_MATPAR(m));
}


/* GL: multmatrix - premultiplies the current transformation matrix */
void
multmatrix (Matrix m)
{
    _igl_trace (__func__,FALSE,IGL_MATFMT(g),IGL_MATPAR(m));
    igl->api->multmatrix (m);
}

void
iglExec_multmatrix (Matrix m)
{
    IGL_CHECKWNDV ();

    glMultMatrixf ((GLfloat *)m);
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: pushmatrix - pushes down the transformation matrix stack */
void
pushmatrix (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->pushmatrix ();
}

void
iglExec_pushmatrix (void)
{
    IGL_CHECKWNDV ();

    /* TBD: man page says: "should not be called when mmode is MPROJECTION or MTEXTURE" */
    glPushMatrix ();
}


/* GL: pushmatrix - pops the transformation matrix stack */
void
popmatrix (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->popmatrix ();
}

void
iglExec_popmatrix (void)
{
    IGL_CHECKWNDV ();

    /* TBD: man page says: "should not be called when mmode is MPROJECTION or MTEXTURE" */
    glPopMatrix ();
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: rotate, rot - rotates the current matrix */
void
rot (float amount, char angle)
{
    _igl_trace (__func__,FALSE,"%g,'%c'",amount,angle);
    igl->api->rot (amount, angle);
}

void
iglExec_rot (float amount, char angle)
{
    IGL_CHECKWNDV ();

    if (angle == 'x' || angle == 'X')
        glRotatef (amount, 1, 0, 0);
    else if (angle == 'y' || angle == 'Y')
        glRotatef (amount, 0, 1, 0);
    else if (angle == 'z' || angle == 'Z')
        glRotatef (amount, 0, 0, 1);
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: rotate, rot - rotates the current matrix */
void
rotate (Angle amount, char angle)
{
    _igl_trace (__func__,FALSE,"%d,'%c'",amount,angle);
    igl->api->rotate (amount, angle);
}

void
iglExec_rotate (Angle amount, char angle)
{
    IGL_CHECKWNDV ();

    /* NOTE: amount is an integer specified in tenths of degrees */
    if (angle == 'x' || angle == 'X')
        glRotatef (amount*0.1f, 1, 0, 0);
    else if (angle == 'y' || angle == 'Y')
        glRotatef (amount*0.1f, 0, 1, 0);
    else if (angle == 'z' || angle == 'Z')
        glRotatef (amount*0.1f, 0, 0, 1);
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: translate - translates the current matrix */
void
translate (Coord x, Coord y, Coord z)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g",x,y,z);
    igl->api->translate (x, y, z);
}

void
iglExec_translate (Coord x, Coord y, Coord z)
{
    IGL_CHECKWNDV ();

    glTranslatef (x, y, z);
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: scale - scales and mirrors the current matrix */
void
scale (float x, float y, float z)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g",x,y,z);
    igl->api->scale (x, y, z);
}

void
iglExec_scale (float x, float y, float z)
{
    IGL_CHECKWNDV ();

    glScalef (x, y, z);
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: polarview - defines the viewer's position in polar coordinates */
void
polarview (Coord dist, Angle azim, Angle inc, Angle twist)
{
    _igl_trace (__func__,FALSE,"%g,%d,%d,%d",dist,azim,inc,twist);
    igl->api->polarview (dist, azim, inc, twist);
}

void
iglExec_polarview (Coord dist, Angle azim, Angle inc, Angle twist)
{
    IGL_CHECKWNDV ();

    glTranslatef (0, 0, -dist);
    glRotatef (-twist*0.1f, 0, 0, 1);
    glRotatef (-inc*0.1f, 1, 0, 0);
    glRotatef (-azim*0.1f, 0, 0, 1);
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: clipplane - specify a plane against which all geometry is clipped */
void
clipplane (long index, long mode, float *params)
{
    _igl_trace (__func__,FALSE,"%ld,%ld,%p",index,mode,params);
    igl->api->clipplane (index, mode, params);
}

void
iglExec_clipplane (long index, long mode, float *params)
{
    GLdouble dparams[4];

    IGL_CHECKWNDV ();

    if (index < 0 || index > 5 || (mode == CP_DEFINE && params == NULL) || IGL_CTX ()->matrixMode == MSINGLE)
        return;

    /* IrisGL supports 6 clipplanes, OpenGL at least 6 - we're in the clear */
    if (mode == CP_OFF)
        glDisable (GL_CLIP_PLANE0+index);
    else if (mode == CP_ON)
        glEnable (GL_CLIP_PLANE0+index);
    else if (mode == CP_DEFINE)
    {
        /* convert the float input paramter to type double */
        dparams[0] = params[0]; dparams[1] = params[1];
        dparams[2] = params[2]; dparams[3] = params[3];
        glClipPlane (GL_CLIP_PLANE0+index, dparams);
    }
}


#if 0
________________________ projection/viewport functions ________________________
#endif
/* load 2D orthographic projection into modelview and projection */
void
_igl_ortho2D (int reshape)
{
    igl_windowT *wptr = IGL_CTX ();

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glMatrixMode (GL_PROJECTION);
    _igl_loadProjBase ();
    wptr->flags &= ~IGL_WSTATE_CURMATRICES;

    if (reshape) {
        wptr->viewport[0] = 0;
        wptr->viewport[1] = 0;
        wptr->viewport[2] = wptr->width;
        wptr->viewport[3] = wptr->height;
        wptr->flags |= IGL_WSTATE_CURVIEWPORT;
        glViewport (0, 0, wptr->width, wptr->height);
        glScissor (0, 0, wptr->width, wptr->height);
        gluOrtho2D (-0.5, wptr->width-0.5, -0.5, wptr->height-0.5);
    }
    else
    {
        _igl_getViewport ();
        gluOrtho2D (wptr->viewport[0]-0.5, wptr->viewport[0]+wptr->viewport[2]-0.5,
                    wptr->viewport[1]-0.5, wptr->viewport[1]+wptr->viewport[3]-0.5);
    }
}


/* GL: ortho, ortho2 - define an orthographic projection transformation */
void
ortho (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%g,%g,%g",left,right,bottom,top,near_,far_);
    igl->api->ortho (left, right, bottom, top, near_, far_);
}

void
iglExec_ortho (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_)
{
    IGL_CHECKWNDV ();

    /*
        in MSINGLE, ortho() loads the matrix onto the matrix stack, otherwise
        it changes the projection matrix
    */
    if (IGL_CTX ()->matrixMode == MSINGLE || IGL_CTX ()->matrixMode == MPROJECTION)
    {
        _igl_loadProjBase ();
        glOrtho ((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top,
            (GLdouble)near_, (GLdouble)far_);
    }
    else
    {
        /* in other modes, only MPROJECTION matrix is touched! */
        glMatrixMode (GL_PROJECTION);

        _igl_loadProjBase ();
        glOrtho ((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top,
            (GLdouble)near_, (GLdouble)far_);

        _igl_setmmode ();
    }
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: ortho, ortho2 - define an orthographic projection transformation */
void
ortho2 (Coord left, Coord right, Coord bottom, Coord top)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%g",left,right,bottom,top);
    igl->api->ortho2 (left, right, bottom, top);
}

void
iglExec_ortho2 (Coord left, Coord right, Coord bottom, Coord top)
{
    IGL_CHECKWNDV ();

    if (IGL_CTX ()->matrixMode == MSINGLE || IGL_CTX ()->matrixMode == MPROJECTION)
    {
        _igl_loadProjBase ();
        gluOrtho2D ((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top);
    }
    else
    {
        /* in other modes, only MPROJECTION matrix is touched! */
        glMatrixMode (GL_PROJECTION);

        _igl_loadProjBase ();
        gluOrtho2D ((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top);

        _igl_setmmode ();
    }
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;

    /* TODO: disable lighting fixes powerflip backgrounds, but is it correct? */
    glDisable (GL_LIGHTING);
    IGL_CTX ()->state &= ~(IGL_WSTATE_LIGHTING | IGL_VSTATE_LIGHTED);

    /* TODO: disable color material fixes perfly GUI, but is it correct? */
    glDisable (GL_COLOR_MATERIAL);
    IGL_CTX ()->state &= ~(IGL_WSTATE_LMCNULL);
}


/* GL: window - defines a perspective projection transformation */
void
window (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%g,%g,%g",left,right,bottom,top,near_,far_);
    igl->api->window (left, right, bottom, top, near_, far_);
}

void
iglExec_window (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_)
{
    IGL_CHECKWNDV ();

    if (IGL_CTX ()->matrixMode == MSINGLE || IGL_CTX ()->matrixMode == MPROJECTION)
    {
        _igl_loadProjBase ();
        glFrustum ((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top,
            (GLdouble)near_, (GLdouble)far_);
    }
    else
    {
        /* in other modes, only MPROJECTION matrix is touched! */
        glMatrixMode (GL_PROJECTION);

        _igl_loadProjBase ();
        glFrustum ((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top,
            (GLdouble)near_, (GLdouble)far_);

        _igl_setmmode ();
    }
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: perspective - defines a perspective projection transformation */
void
perspective (Angle fovy, float aspect, Coord near_, Coord far_)
{
    _igl_trace (__func__,FALSE,"%d,%g,%g,%g",fovy,aspect,near_,far_);
    igl->api->perspective (fovy, aspect, near_, far_);
}

void
iglExec_perspective (Angle fovy, float aspect, Coord near_, Coord far_)
{
    IGL_CHECKWNDV ();

    /* man page says it's invalid */
    if (fovy < 2)
        return;

    if (IGL_CTX ()->matrixMode == MSINGLE || IGL_CTX ()->matrixMode == MPROJECTION)
    {
        _igl_loadProjBase ();
        gluPerspective ((GLdouble)0.1*fovy, (GLdouble)aspect, (GLdouble)near_, (GLdouble)far_);
    }
    else
    {
        /* in other modes, only MPROJECTION matrix is touched! */
        glMatrixMode (GL_PROJECTION);

        _igl_loadProjBase ();
        gluPerspective ((GLdouble)0.1*fovy, (GLdouble)aspect, (GLdouble)near_, (GLdouble)far_);

        _igl_setmmode ();
    }
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: lookat - defines a viewing transformation */
void
lookat (Coord vx, Coord vy, Coord vz, Coord px, Coord py, Coord pz, Angle twist)
{
    _igl_trace (__func__,FALSE,"%g,%g,%g,%g,%g,%g,%d",vx,vy,vz,px,py,pz,twist);
    igl->api->lookat (vx, vy, vz, px, py, pz, twist);
}

void
iglExec_lookat (Coord vx, Coord vy, Coord vz, Coord px, Coord py, Coord pz, Angle twist)
{
    GLfloat dx = px - vx;
    GLfloat dy = py - vy;
    GLfloat dz = pz - vz;
    int yup = (dx == 0 && dz == 0);

    IGL_CHECKWNDV ();

    if (yup && dy == 0)
        return;

    glRotatef (-twist*0.1f, 0, 0, 1 );
    gluLookAt (vx, vy, vz, px, py, pz, 0, !yup, -yup);
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURMATRICES;
}


/* GL: viewport - allocates a rectangular area of the window for an image */
void
viewport (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",left,right,bottom,top);
    igl->api->viewport (left, right, bottom, top);
}

void
iglExec_viewport (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top)
{
    long width, height;

    IGL_CHECKWNDV ();

    width = right-left + 1;
    height = top-bottom + 1;
    if (width <= 0 || height <= 0)
        return;

    IGL_CTX ()->viewport[0] = left;
    IGL_CTX ()->viewport[1] = bottom;
    IGL_CTX ()->viewport[2] = width;
    IGL_CTX ()->viewport[3] = height;
    glViewport (left, bottom, width, height);
    /* viewport() sets scrmask */
    glScissor (left, bottom, width, height);
    glEnable (GL_SCISSOR_TEST);
    IGL_CTX ()->flags |= IGL_WSTATE_CURVIEWPORT;
}


/* GL: getviewport - gets a copy of the dimensions of the current viewport */
void
getviewport (Screencoord *left, Screencoord *right, Screencoord *bottom, Screencoord *top)
{
    _igl_trace (__func__,TRUE,"%p,%p,%p,%p",left,right,bottom,top);
    IGL_CHECKWNDV ();

    if (left == NULL || right == NULL || bottom == NULL || top == NULL)
        return;

    _igl_getViewport ();

    *left = IGL_CTX ()->viewport[0];
    *right = IGL_CTX ()->viewport[0] + IGL_CTX ()->viewport[2] - 1;
    *bottom = IGL_CTX ()->viewport[1];
    *top = IGL_CTX ()->viewport[1] + IGL_CTX ()->viewport[3] - 1;
}


/* GL: reshapeviewport - sets the viewport to the dimensions of the current graphics window */
void
reshapeviewport (void)
{
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"{%d,%d}",IGL_CTX()->width,IGL_CTX()->height);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    wptr->viewport[0] = 0;
    wptr->viewport[1] = 0;
    wptr->viewport[2] = wptr->width;
    wptr->viewport[3] = wptr->height;
    glViewport (0, 0, wptr->width, wptr->height);
    /* viewport() sets scrmask */
    glScissor (0, 0, wptr->width, wptr->height);
    glEnable (GL_SCISSOR_TEST);
    wptr->flags |= IGL_WSTATE_CURVIEWPORT;
}


/* GL: pushviewport - pushes down the viewport stack */
void
pushviewport (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->pushviewport ();
}

void
iglExec_pushviewport (void)
{
    IGL_CHECKWNDV ();

    glPushAttrib (GL_VIEWPORT_BIT | GL_SCISSOR_BIT);
}


/* GL: popviewport - pops the viewport stack */
void
popviewport (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->popviewport ();
}

void
iglExec_popviewport (void)
{
    IGL_CHECKWNDV ();

    glPopAttrib ();
    IGL_CTX ()->flags &= ~IGL_WSTATE_CURVIEWPORT;
}


/* GL: scrmask - defines a rectangular screen clipping mask */
void
scrmask (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top)
{
    _igl_trace (__func__,FALSE,"%d,%d,%d,%d",left,right,bottom,top);
    igl->api->scrmask (left, right, bottom, top);
}

void
iglExec_scrmask (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top)
{
    long width, height;

    IGL_CHECKWNDV ();

    width = right-left+1;
    height = top-bottom+1;
    if (width <= 0 || height <= 0)
        return;

    glScissor (left, bottom, width, height);
    glEnable (GL_SCISSOR_TEST);
}


/* GL: getscrmask - returns the current screen mask */
void
getscrmask (Screencoord *left, Screencoord *right, Screencoord *bottom, Screencoord *top)
{
    int cscrmask[4];

    _igl_trace (__func__,TRUE,"%p,%p,%p,%p",left,right,bottom,top);
    IGL_CHECKWNDV ();

    if (left == NULL || right == NULL || bottom == NULL || top == NULL)
        return;

    /* if GL_SCISSOR_TEST is disabled, we get the entire window box */
    glGetIntegerv (GL_SCISSOR_BOX, cscrmask);

    *left = cscrmask[0];
    *right = cscrmask[0] + cscrmask[2] - 1;
    *bottom = cscrmask[1];
    *top = cscrmask[1] + cscrmask[3] - 1;
    _igl_trace (" = ",TRUE,"{%ld,%ld,%ld,%ld}",*left,*right,*bottom,*top);
}


/* GL: mapw - maps a point on the screen into a line in 3-D world coordinates */
void
mapw (Object obj, Screencoord x, Screencoord y, Coord *wx1, Coord *wy1, Coord *wz1, 
      Coord *wx2, Coord *wy2, Coord *wz2)
{
    GLdouble tx, ty, tz;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%ld,%d,%d,%p,%p,%p,%p,%p,%p",obj,x,y,wx1,wy1,wz1,wx2,wy2,wz2);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if (wx1 == NULL || wy1 == NULL || wz1 == NULL || wx2 == NULL || wy2 == NULL || wz2 == NULL)
        return;

    _igl_pushTransform ();
    _igl_setmmode ();

    callobj (obj);
    _igl_getViewport ();
    _igl_getMatrices ();

    wptr->flags &= ~(IGL_WSTATE_CURMATRICES | IGL_WSTATE_CURVIEWPORT);
    _igl_popTransform ();

    /* NOTE: in NDC, z should be [0..1]. For safety, use a small epsilon */
    gluUnProject (x, y, 0.00001, wptr->modelMatrix, wptr->projMatrix, wptr->viewport, &tx, &ty, &tz);
    *wx1 = (GLfloat)tx;
    *wy1 = (GLfloat)ty;
    *wz1 = (GLfloat)tz;
    gluUnProject (x, y, 0.99999, wptr->modelMatrix, wptr->projMatrix, wptr->viewport, &tx, &ty, &tz);
    *wx2 = (GLfloat)tx;
    *wy2 = (GLfloat)ty;
    *wz2 = (GLfloat)tz;
    /* TODO: need or need not to negate z?  The resulting line should be
     * perpendicular to the window, so ostensibly it doesn't matter.
     */
    _igl_trace (" = ",TRUE,"{%g,%g,%g},{%g,%g,%g}",*wx1,*wy1,*wz1,*wx2,*wy2,*wz2);
}


/* GL: mapw2 - maps a point on the screen into 2-D world coordinates */
void
mapw2 (Object obj, Screencoord x, Screencoord y, Coord *wx, Coord *wy)
{
    GLdouble tx, ty, tz;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%ld,%d,%d,%p,%p",obj,x,y,wx,wy);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    if (wx == NULL || wy == NULL)
        return;

    _igl_pushTransform ();
    _igl_setmmode ();

    callobj (obj);
    _igl_getViewport ();
    _igl_getMatrices ();

    wptr->flags &= ~(IGL_WSTATE_CURMATRICES | IGL_WSTATE_CURVIEWPORT);
    _igl_popTransform ();

    /* NOTE: this only works with orthogonal projections - z doesn't matter */
    gluUnProject (x, y, 0, wptr->modelMatrix, wptr->projMatrix, wptr->viewport, &tx, &ty, &tz);
    *wx = (GLfloat)tx;
    *wy = (GLfloat)ty;
    _igl_trace (" = ",TRUE,"{%g,%g}",*wx,*wy);
}


/* GL: gl_invertmat - invert a matrix??? */
/* NOTE: ripped from Mesa */
void
gl_invertmat (Matrix m, Matrix out)
{
    GLdouble *_tmp;
    GLdouble wtmp[4][8];
    GLdouble m0, m1, m2, m3, s;
    GLdouble *r0, *r1, *r2, *r3;
#define SWAP_ROWS(a, b) { _tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m, r, c) (m)[(c)][(r)]

    r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

    r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
    r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
    r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
    r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
    r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
    r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
    r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
    r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
    r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
    r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
    r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
    r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

    /* choose pivot - or die */
    if (fabs(r3[0]) > fabs(r2[0]))
        SWAP_ROWS (r3, r2);
    if (fabs(r2[0]) > fabs(r1[0]))
        SWAP_ROWS (r2, r1);
    if (fabs(r1[0]) > fabs(r0[0]))
        SWAP_ROWS (r1, r0);
    if (r0[0] == 0)
        return;

    /* eliminate first variable     */
    m1 = r1[0] / r0[0];
    m2 = r2[0] / r0[0];
    m3 = r3[0] / r0[0];
    s = r0[1];
    r1[1] -= m1 * s;
    r2[1] -= m2 * s;
    r3[1] -= m3 * s;
    s = r0[2];
    r1[2] -= m1 * s;
    r2[2] -= m2 * s;
    r3[2] -= m3 * s;
    s = r0[3];
    r1[3] -= m1 * s;
    r2[3] -= m2 * s;
    r3[3] -= m3 * s;
    s = r0[4];
    if (s != 0.0)
    {
        r1[4] -= m1 * s;
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
    }
    s = r0[5];
    if (s != 0.0)
    {
        r1[5] -= m1 * s;
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
    }
    s = r0[6];
    if (s != 0.0)
    {
        r1[6] -= m1 * s;
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
    }
    s = r0[7];
    if (s != 0.0)
    {
        r1[7] -= m1 * s;
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
    }

    /* choose pivot - or die */
    if (fabs(r3[1]) > fabs(r2[1]))
        SWAP_ROWS (r3, r2);
    if (fabs(r2[1]) > fabs(r1[1]))
        SWAP_ROWS (r2, r1);
    if (r1[1] == 0)
        return;

    /* eliminate second variable */
    m2 = r2[1] / r1[1];
    m3 = r3[1] / r1[1];
    r2[2] -= m2 * r1[2];
    r3[2] -= m3 * r1[2];
    r2[3] -= m2 * r1[3];
    r3[3] -= m3 * r1[3];
    s = r1[4];
    if (0.0 != s)
    {
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
    }
    s = r1[5];
    if (0.0 != s)
    {
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
    }
    s = r1[6];
    if (0.0 != s)
    {
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
    }
    s = r1[7];
    if (0.0 != s)
    {
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
    }

    /* choose pivot - or die */
    if (fabs(r3[2]) > fabs(r2[2]))
        SWAP_ROWS (r3, r2);
    if (r2[2] == 0)
        return;

    /* eliminate third variable */
    m3 = r3[2] / r2[2];
    r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
        r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];

    /* last check */
    if (r3[3] == 0)
        return;

    s = 1.0 / r3[3];        /* now back substitute row 3 */
    r3[4] *= s;
    r3[5] *= s;
    r3[6] *= s;
    r3[7] *= s;

    m2 = r2[3];             /* now back substitute row 2 */
    s = 1.0 / r2[2];
    r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
        r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
    m1 = r1[3];
    r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
        r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
    m0 = r0[3];
    r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
        r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

    m1 = r1[2];             /* now back substitute row 1 */
    s = 1.0 / r1[1];
    r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
        r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
    m0 = r0[2];
    r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
        r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

    m0 = r0[1];             /* now back substitute row 0 */
    s = 1.0 / r0[0];
    r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
        r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

    MAT(out, 0, 0) = (float)r0[4];
    MAT(out, 0, 1) = (float)r0[5], MAT(out, 0, 2) = (float)r0[6];
    MAT(out, 0, 3) = (float)r0[7], MAT(out, 1, 0) = (float)r1[4];
    MAT(out, 1, 1) = (float)r1[5], MAT(out, 1, 2) = (float)r1[6];
    MAT(out, 1, 3) = (float)r1[7], MAT(out, 2, 0) = (float)r2[4];
    MAT(out, 2, 1) = (float)r2[5], MAT(out, 2, 2) = (float)r2[6];
    MAT(out, 2, 3) = (float)r2[7], MAT(out, 3, 0) = (float)r3[4];
    MAT(out, 3, 1) = (float)r3[5], MAT(out, 3, 2) = (float)r3[6];
    MAT(out, 3, 3) = (float)r3[7];

#undef MAT
#undef SWAP_ROWS
}


/* GL: gl_invertmat - invert a matrix??? */
void
gl_invert4d (Matrix out, Matrix m)
{
    gl_invertmat (m, out);
}


/* GL: gl_sincos - calculate sin and cos??? */
void
gl_sincos (int a, float *b, float *c)
{
    *b = sinf (a * M_PI / 1800);
    *c = cosf (a * M_PI / 1800);
}


#if 0
______________________________ pick functions ______________________________
#endif
/* GL: pick - puts the system in picking mode ^*/
void
pick (short buffer[], long numnam)
{
    int x = 0, y = 0;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%p,%d",buffer,numnam);
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* update and get the cursor position in the window in GL coordinates */
    IGL_MSG_PROCESS
    x = igl->deviceVal[CURSORX] - wptr->x;
    y = igl->deviceVal[CURSORY] - GL_Y(wptr->y + wptr->height - 1);
    _igl_getViewport ();

    /* NOTE: man page states "places a special viewing matrix on the stack".
     * I think that's from times where there was only the MSINGLE mode.
     * The GL programming manual says "loads a projection matrix", and
     * gluPickMatrix() defines a projection matrix too.
     * Because in IrisGL the projection matrix can be reloaded after picking
     * starts, the OpenGL pick matrix must be stored, and reloaded if the
     * projection matrix gets overwritten.
     */

    /* load the picking matrix onto the projection stack and save it */
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    gluPickMatrix (x, y, wptr->pickDeltax, wptr->pickDeltay, wptr->viewport);
    glGetFloatv (GL_PROJECTION_MATRIX, wptr->pickMatrix);
    glPopMatrix ();
    _igl_setmmode ();

    /* since OpenGL puts extra information into the pick buffer and uses a
     * different type, a (bigger?) temporary buffer for conversion is needed.
     */
    wptr->pickBuffer = calloc(1, numnam * sizeof(GLuint));
    glSelectBuffer (numnam, wptr->pickBuffer);
    glRenderMode (GL_SELECT);
    glPushName (0);     /* glLoadName will fail for an empty name stack */

    wptr->state |= IGL_WSTATE_PICKING;
}


/* GL: endpick - turns off picking mode */
long
endpick (short buffer[])
{
    int i, j, o, p, num = glRenderMode (GL_RENDER);
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%p",buffer);
    IGL_CHECKWND (0);
    wptr = IGL_CTX ();

    /* convert the hit records */
    for (i = 0, o = 0, p = 0; i < num; i++)
    {
        int l = wptr->pickBuffer[o++];
        buffer[p++] = l;
        o += 2; /* skip Z information */
        for  (j = 0; j < l; j++)
            buffer[p++] = wptr->pickBuffer[o++];
    }

    free (wptr->pickBuffer);
    wptr->pickBuffer = NULL;
    wptr->state &= ~IGL_WSTATE_PICKING;
    _igl_trace (" = ",TRUE,"%d",num);
    return num;
}


/* GL: gselect - puts the system in selecting mode */
void
gselect (short buffer[], long numnam)
{
    _igl_trace (__func__,TRUE,"%p,%d",buffer,numnam);
    IGL_CHECKWNDV ();

    /* since OpenGL puts extra information into the select buffer and uses a
     * different type, a (bigger?) temporary buffer for conversion is needed.
     */
    IGL_CTX ()->pickBuffer = calloc(1, numnam * sizeof(GLuint));
    glSelectBuffer (numnam, IGL_CTX ()->pickBuffer);
    glRenderMode (GL_SELECT);
    glPushName (0);     /* glLoadName will fail for an empty name stack */
}


/* GL: endselect - turns off selecting mode */
long
endselect (short buffer[])
{
    _igl_trace (__func__,TRUE,"%p",buffer);
    IGL_CHECKWND (0);

    return endpick (buffer);
}


/* GL: picksize - sets the dimensions of the picking region */
void
picksize (short deltax, short deltay)
{
    _igl_trace (__func__,TRUE,"%d,%d",deltax, deltay);
    IGL_CHECKWNDV ();

    IGL_CTX ()->pickDeltax = deltax;
    IGL_CTX ()->pickDeltay = deltay;
}


/* GL: initnames - initializes the name stack */
void
initnames (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->initnames ();
}

void
iglExec_initnames (void)
{
    IGL_CHECKWNDV ();

    glInitNames ();
    glPushName (0);     /* glLoadName will fail for an empty name stack */
}


/* GL: loadname - loads a name onto the name stack */
void
loadname (short name)
{
    _igl_trace (__func__,FALSE,"%d",name);
    igl->api->loadname (name);
}

void
iglExec_loadname (short name)
{
    IGL_CHECKWNDV ();

    glLoadName (name);
}


/* GL: pushname - pushes a new name on the name stack */
void
pushname (short name)
{
    _igl_trace (__func__,FALSE,"%d",name);
    igl->api->pushname (name);
}

void
iglExec_pushname (short name)
{
    IGL_CHECKWNDV ();

    glPushName (name);
}


/* GL: popname -  pops a name off the name stack */
void
popname (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->popname ();
}

void
iglExec_popname (void)
{
    IGL_CHECKWNDV ();

    glPopName ();
}



/* GL: unimplemented stuff */
void
scrbox (long arg)
{
    _igl_trace (__func__,TRUE,"%d",arg);
    /* not supported in OpenGL */
}


void
getscrbox (long *left, long *right, long *bottom, long *top)
{
    char *sb = getenv ("IGL_SCRBOX");

    _igl_trace (__func__,TRUE,"%p,%p,%p,%p",left,right,bottom,top);
    IGL_CHECKWNDV ();

    /* NOTE: not supported by OpenGL. Return either values from environment, or
     * the viewport limits as the maximum */
    if (!sb || sscanf(sb, "%ld,%ld,%ld,%ld", left, right, bottom, top) != 4)
    {
        _igl_getViewport ();
        *left = IGL_CTX ()->viewport[0];
        *right = IGL_CTX ()->viewport[0] + IGL_CTX ()->viewport[2] - 1;
        *bottom = IGL_CTX ()->viewport[1];
        *top = IGL_CTX ()->viewport[1] + IGL_CTX ()->viewport[3] - 1;
    }
    _igl_trace (" = ",TRUE,"{%ld,%ld,%ld,%ld}",*left,*right,*bottom,*top);
}

long
gethitcode (void)
{
    _igl_trace (__func__,TRUE,"");
    /* not supported in OpenGL */
    return 0;
}

void
clearhitcode (void)
{
    _igl_trace (__func__,TRUE,"");
    /* not supported in OpenGL */
}
