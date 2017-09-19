/*
 * vertex.c
 *
 * bgn/end primitive, vertex coordinate and normal vector handling, NURBS
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


/* NOTE: Lighting is different in IrisGL and OpenGL. While IrisGL only lights
 * primitives if a normal has been specified before end*, OpenGL always lights
 * primitives when lighting is enabled.
 * Texturing is apparently enabled if a t* was given inside bgn*, while OpenGL
 * applies texturing always if is enabled, using the last given coordinate.
 * There is apparently also a dependency whether n3f was called inside bgn*(),
 * or outside. If called inside, the current normal is seen as a vertex normal
 * and is invalidated on end*(), if called outside, it is seen as a surface
 * normal and is kept.
 * It only adds to the nuisance that OpenGL lighting cannot be enabled within
 * glBegin/glEnd :-(
 *
 * To emulate IrisGL behaviour correctly, it would be necessary to store all
 * vertices from bgn* upto end*, then decide upon lighting and push everything
 * to OpenGL, but that's rather slow on those old SGI systems. Instead, store
 * the glBegin primitive and delay the execution until the 3rd vertex is given,
 * which is the most I've seen in the wild. Only enable lighting if a normal
 * was set with n3f by then, and enable texturing if a t* was given.
 *
 * TODO: lmbind/lmdef are allowed within bgn*,end* sequences. Hence,
 * additionally track material changes (lmbind/lmdef) in the vertex stack?
 */

/* NOTE: OpenGL doesn't offer swaptmesh. To emulate this, a 2-vertex stack must
 * be kept, and on swaptmesh the older vertex is pushed to OpenGL, effectively
 * exchanging the two vertices inside the OpenGL renderer, at the cost of
 * rendering a zero-width triangle along the edge (and possible artifacts :-/).
 * Apparently, IrisGL doesn't only swap the vertices, it also swaps the
 * colors, normals, and texture coordinates associated with these vertices.
 * So, the vertex stack must also include these.
 */


/* flush the vertex stack to OpenGL, deciding on lighting on the way */
static void
_igl_flushstack(igl_windowT *wptr, int tidx)
{
    int i;

    /* lighting enabled? */
    if (wptr->state & IGL_WSTATE_LIGHTING)
    {
        if (wptr->state & (IGL_VSTATE_N3F|IGL_VSTATE_N3FBGN)) {
            IGL_ENLIGHT ();
        } else {
            IGL_DISLIGHT ();
        }
    }

    /* texturing enabled? */
    if (wptr->state & IGL_WSTATE_TEXTURING)
    {
        if (wptr->texgenMode || (wptr->state & IGL_VSTATE_TEXTURED))
            glEnable(wptr->textureType);
        else
            glDisable(wptr->textureType);
    }

    /* OpenGL startup */
    glBegin (wptr->bgnPrimitive);
 
    /* if colors are disabled via lmcolor, use stored color */
    if (wptr->state & IGL_WSTATE_LMCNULL)
    {
        if (wptr->state & IGL_VSTATE_COLORED)
        {
            glColor4ubv (wptr->lmcColor);
            wptr->state &= ~IGL_VSTATE_COLORED;
        }
    }

    /* shove stashed data to OpenGL */
    /* TODO: use glDrawElements, or glDrawArrays? */
    for (i = IGL_VERTEXSTACK_SIZE-1; i >= wptr->bgnDelay; i--)
    {
        if (!(wptr->state & IGL_WSTATE_LMCNULL))
            glColor4ubv (wptr->colorStack[i]);
        if (wptr->state & IGL_VSTATE_N3F)
            glNormal3fv (wptr->normalStack[i]);
        if (wptr->state & IGL_VSTATE_TEXTURED)
            glTexCoord4fv (wptr->texcoordStack[i]);
        glVertex4fv (wptr->vertexStack[i]);
    }

    /* must copy data to correct position for swaptmesh if needed */
    if ((wptr->state & IGL_VSTATE_TMESH) && wptr->bgnDelay > tidx)
    {
        for (i = 0; i <= 1; i++)
        {
            IGL_V4COPY (wptr->colorStack[tidx+i]   , wptr->colorStack[wptr->bgnDelay+i]);
            IGL_V3COPY (wptr->normalStack[tidx+i]  , wptr->normalStack[wptr->bgnDelay+i]);
            IGL_V4COPY (wptr->texcoordStack[tidx+i], wptr->texcoordStack[wptr->bgnDelay+i]);
            IGL_V4COPY (wptr->vertexStack[tidx+i]  , wptr->vertexStack[wptr->bgnDelay+i]);
        }
    }

    wptr->stackIdx = 0;
    wptr->bgnDelay = 0;
}

#if 0
____________________________ v functions ____________________________
#endif
/* GL: v[234][dfis] - transfers a 2-D, 3-D, or 4-D vertex to the graphics pipe */
void
v4f (float vector[4])
{
    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(g),vector,IGL_V4PAR(vector));
    igl->api->v4f (vector);
}

void
iglExec_v4f (float vector[4])
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* lighting+texturing state machine at the start of the primitive */
    switch (wptr->bgnDelay)
    {
        default:   /* still delaying, store data */
            wptr->bgnDelay--;
            if (!(wptr->state & IGL_WSTATE_LMCNULL))
                IGL_V4COPY (wptr->colorStack[wptr->bgnDelay], wptr->currentColor);
            if (wptr->state & IGL_VSTATE_N3F)
                IGL_V3COPY (wptr->normalStack[wptr->bgnDelay], wptr->currentNormal);
            if (wptr->state & IGL_VSTATE_TEXTURED)
                IGL_V4COPY (wptr->texcoordStack[wptr->bgnDelay], wptr->currentTexcoord);
            IGL_V4COPY (wptr->vertexStack[wptr->bgnDelay], vector);
            return;
    
        case 1:    /* set lighting/texturing accordingly, then start OpenGL */
            _igl_flushstack (wptr, 1);
            wptr->stackIdx = 0;
    
            /* must resend data for current vertex (overwritten by flush) */
            if (!(wptr->state & IGL_WSTATE_LMCNULL))
                glColor4ubv (wptr->currentColor);
            if (wptr->state & IGL_VSTATE_N3F)
                glNormal3fv (wptr->currentNormal);
            if (wptr->state & IGL_VSTATE_TEXTURED)
                glTexCoord4fv (wptr->currentTexcoord);
            glVertex4fv (vector);
            break;
    
        case 0:     /* pass all subsequent data directly to OpenGL */
            if (wptr->state & IGL_WSTATE_LMCNULL)
            {
                if (wptr->state & IGL_VSTATE_COLORED)
                {
                    /* color set, but disabled by lmcolor. reset it */
                    glColor4ubv (wptr->lmcColor);
                    wptr->state &= ~IGL_VSTATE_COLORED;
                }
            }
            glVertex4fv (vector);
            break;
    }

    if (wptr->state & IGL_VSTATE_TMESH)
    {
	/* must stash data for swaptmesh */
        if (!(wptr->state & IGL_WSTATE_LMCNULL))
            IGL_V4COPY (wptr->colorStack[wptr->stackIdx], wptr->currentColor);
        if (wptr->state & IGL_VSTATE_N3F)
            IGL_V3COPY (wptr->normalStack[wptr->stackIdx], wptr->currentNormal);
        if (wptr->state & IGL_VSTATE_TEXTURED)
            IGL_V4COPY (wptr->texcoordStack[wptr->stackIdx], wptr->currentTexcoord);
        IGL_V4COPY (wptr->vertexStack[wptr->stackIdx], vector);
        wptr->stackIdx ^= 1;
    }
}


void
v4d (double vector[4])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(lg),vector,IGL_V4PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = (float)vector[3];

    igl->api->v4f (vector4);
}


void
v4i (int vector[4])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(d),vector,IGL_V4PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = (float)vector[3];

    igl->api->v4f (vector4);
}


void
v4s (short vector[4])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V4FMT(d),vector,IGL_V4PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = (float)vector[3];

    igl->api->v4f (vector4);
}


void
v3f (float vector[3])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(g),vector,IGL_V3PAR(vector));
    vector4[0] = vector[0];
    vector4[1] = vector[1];
    vector4[2] = vector[2];
    vector4[3] = 1.0f;

    igl->api->v4f (vector4);
}


void
v3d (double vector[3])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(lg),vector,IGL_V3PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = 1.0f;

    igl->api->v4f (vector4);
}


void
v3i (int vector[3])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(d),vector,IGL_V3PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = 1.0f;

    igl->api->v4f (vector4);
}


void
v3s (short vector[3])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(d),vector,IGL_V3PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = (float)vector[2];
    vector4[3] = 1.0f;

    igl->api->v4f (vector4);
}


void
v2f (float vector[2])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V2FMT(g),vector,IGL_V2PAR(vector));
    vector4[0] = vector[0];
    vector4[1] = vector[1];
    vector4[2] = 0.0f;
    vector4[3] = 1.0f;

    igl->api->v4f (vector4);
}


void
v2d (double vector[2])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V2FMT(lg),vector,IGL_V2PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = 0.0f;
    vector4[3] = 1.0f;

    igl->api->v4f (vector4);
}


void
v2i (int vector[2])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V2FMT(d),vector,IGL_V2PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = 0.0f;
    vector4[3] = 1.0f;

    igl->api->v4f (vector4);
}


void
v2s (short vector[2])
{
    float vector4[4];

    _igl_trace (__func__,FALSE,"%p" IGL_V2FMT(d),vector,IGL_V2PAR(vector));
    vector4[0] = (float)vector[0];
    vector4[1] = (float)vector[1];
    vector4[2] = 0.0f;
    vector4[3] = 1.0f;

    igl->api->v4f (vector4);
}


#if 0
____________________________ normal functions ____________________________
#endif
/* GL: nmode - specify renormalization of normals */
void
nmode (long mode)
{
    _igl_trace (__func__,FALSE,"%ld",mode);
    igl->api->nmode (mode);
}

void
iglExec_nmode (long mode)
{
    IGL_CHECKWNDV ();

    if (mode == NAUTO)
    {
        glDisable (GL_NORMALIZE);
        glEnable (GL_RESCALE_NORMAL);
    }
    else /* treat everything else as NNORMALIZE */
    {
        glEnable (GL_NORMALIZE);
        glDisable (GL_RESCALE_NORMAL);
    }
}


/* GL: n3f - specifies a normal */
void
n3f (float *vector)
{
    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(g),vector,IGL_V3PAR(vector));
    igl->api->n3f (vector);
}

void
iglExec_n3f (float *vector)
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* NOTE: IrisGL apparently applies the normal to previous vertices as well
     * if this is the 1st normal inside of bgn*?
     */
    /* TODO: is this also valid for color and tex coordinates? */
    if (wptr->bgnDelay)
    {
        if (!(wptr->state & IGL_VSTATE_N3F))
        {
            int i;
            for (i = IGL_VERTEXSTACK_SIZE-1; i >= wptr->bgnDelay; i--)
                IGL_V3COPY (wptr->normalStack[i], vector);
        }
    } else
        glNormal3fv (vector);

    IGL_V3COPY (wptr->currentNormal, vector);

    /* revoke "surface normal" state and set "vertex normal" */
    wptr->state &= ~IGL_VSTATE_N3FBGN;
    wptr->state |= IGL_VSTATE_N3F;
}


/* GL: normal - obsolete routine */
void
normal (Coord *narray)
{
    _igl_trace (__func__,FALSE,"%p" IGL_V3FMT(g),narray,IGL_V3PAR(narray));
    igl->api->n3f (narray);
}


#if 0
____________________________ bgn functions ____________________________
#endif
void
iglExec_endpoint (void)
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* must emit primitive if there is data in the vertex stack */
    if (wptr->bgnDelay)
    {
        GLfloat (*vertex)[4] = &wptr->vertexStack[wptr->bgnDelay];
        int vCount = IGL_VERTEXSTACK_SIZE - wptr->bgnDelay;

        /* kludge for old stuff drawing a line of length 0 to produce a point */
        if (vCount == 2 && wptr->bgnPrimitive == GL_LINE_STRIP && !memcmp(vertex, vertex+1, sizeof(*vertex)))
        {
            wptr->bgnPrimitive = GL_POINTS;
            wptr->bgnDelay ++;
        }
        _igl_flushstack (wptr, wptr->bgnDelay);
    }

    glEnd ();

    /* reset states valid only inside primitives */
    if (wptr->state & IGL_WSTATE_LMCNULL)
    {
        glColor4ubv (wptr->currentColor);
        wptr->state |= IGL_VSTATE_COLORED;
    }
    wptr->state &= ~(IGL_VSTATE_N3F | IGL_VSTATE_TEXTURED | IGL_VSTATE_TMESH);

    wptr->bgnPrimitive = -1;
    wptr->bgnDelay = 0;
}

static void
bgn (GLenum what)
{
    igl_windowT *wptr = IGL_CTX ();

    wptr->bgnPrimitive = what;
    wptr->bgnDelay = IGL_VERTEXSTACK_SIZE;

    /* new normal since last primitive -> set "surface normal" state */
    if (wptr->state & IGL_VSTATE_N3F)
    {
        wptr->state |= IGL_VSTATE_N3FBGN;
        wptr->state &= ~IGL_VSTATE_N3F;
    }
}

/* GL: bgntmesh, endtmesh - delimit the vertices of a triangle mesh */
void
bgntmesh (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->bgntmesh ();
}

void
iglExec_bgntmesh (void)
{
    IGL_CHECKWNDV ();

    IGL_CTX ()->state |= IGL_VSTATE_TMESH;
    bgn (GL_TRIANGLE_STRIP);
}


/* GL: bgntmesh, endtmesh - delimit the vertices of a triangle mesh */
void
endtmesh (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->endpoint ();
}


/* GL: bgntmesh, endtmesh - delimit the vertices of a triangle mesh */
void
swaptmesh (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->swaptmesh ();
}

/* GL: swaptmesh - toggles the triangle mesh register pointer */
void
iglExec_swaptmesh (void)
{
    igl_windowT *wptr;

    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    /* setup tmesh stack if not yet done */
    if (wptr->bgnDelay)
    {
        _igl_flushstack (wptr, 0);
        wptr->stackIdx = 1;
    }

    /* write out 2nd last vertex again to swap in OpenGL */
    if (wptr->state & IGL_WSTATE_LMCNULL)
    {
        if (wptr->state & IGL_VSTATE_COLORED)
        {
            glColor4ubv (wptr->lmcColor);
            wptr->state &= ~IGL_VSTATE_COLORED;
        }
    }
    else
        glColor4ubv (wptr->colorStack[wptr->stackIdx]);
    if (wptr->state & IGL_VSTATE_N3F)
        glNormal3fv (wptr->normalStack[wptr->stackIdx]);
    if (wptr->state & IGL_VSTATE_TEXTURED)
        glTexCoord4fv (wptr->texcoordStack[wptr->stackIdx]);
    glVertex4fv (wptr->vertexStack[wptr->stackIdx]);

    /* flip stack index to reflect swap */
    wptr->stackIdx ^= 1;
}


/* GL: bgnclosedline, endclosedline - delimit the vertices of a closed line */
void
bgnclosedline (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->bgnclosedline ();
}

void
iglExec_bgnclosedline (void)
{
    IGL_CHECKWNDV ();

    bgn (GL_LINE_LOOP);
}


/* GL: bgnclosedline, endclosedline - delimit the vertices of a closed line */
void
endclosedline (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->endpoint ();
}


/* GL: bgnline, endline - delimit the vertices of a line */
void
bgnline (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->bgnline ();
}

void
iglExec_bgnline (void)
{
    IGL_CHECKWNDV ();

    bgn (GL_LINE_STRIP);
}


/* GL: bgnline, endline - delimit the vertices of a line */
void
endline (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->endpoint ();
}


/* GL: bgnpoint, endpoint - delimit the interpretation of vertex routines as points */
void
bgnpoint (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->bgnpoint ();
}

void
iglExec_bgnpoint (void)
{
    IGL_CHECKWNDV ();

    bgn (GL_POINTS);
}


/* GL: bgnpoint, endpoint - delimit the interpretation of vertex routines as points */
void
endpoint (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->endpoint ();
}


/* GL: bgnpolygon, endpolygon - delimit the vertices of a polygon */
void
bgnpolygon (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->bgnpolygon ();
}

void
iglExec_bgnpolygon (void)
{
    IGL_CHECKWNDV ();

    bgn (GL_POLYGON);
}


/* GL: bgnpolygon, endpolygon - delimit the vertices of a polygon */
void
endpolygon (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->endpoint ();
}


/* GL: bgnqstrip, endqstrip - delimit the vertices of a quadrilateral strip */
void
bgnqstrip (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->bgnqstrip ();
}

void
iglExec_bgnqstrip (void)
{
    IGL_CHECKWNDV ();

    bgn (GL_QUAD_STRIP);
}


/* GL: bgnqstrip, endqstrip - delimit the vertices of a quadrilateral strip */
void
endqstrip (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->endpoint ();
}


/* GL: bgncurve, endcurve - delimit a NURBS curve definition */
void
bgncurve (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->bgncurve ();
}

void
iglExec_bgncurve (void)
{
    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
        gluBeginCurve (igl->nurbsCurve);
}


/* GL: bgncurve, endcurve - delimit a NURBS curve definition */
void
endcurve (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->endcurve ();
}

void
iglExec_endcurve (void)
{
    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
        gluEndCurve (igl->nurbsCurve);
}


/* GL: nurbscurve - controls the shape of a NURBS curve */
void 
nurbscurve (long knotCount, const double *knotList, long offset, const double *ctlArray,
            long order, long type)
{
    _igl_trace (__func__,FALSE,"%ld,%p,%ld,%p,%ld,%ld",knotCount,knotList,offset,ctlArray,order,type);
    igl->api->nurbscurve (knotCount, knotList, offset, ctlArray, order, type);
}

void 
iglExec_nurbscurve (long knotCount, const double *knotList, long offset, const double *ctlArray,
            long order, long type)
{
    GLfloat *knots, *ctls;
    int otype, ptsz, i, j;

    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
    {
        switch (type)
        {
            case N_V3D:     otype = GL_MAP1_VERTEX_3, ptsz = 3; break;
            case N_V3DR:    otype = GL_MAP1_VERTEX_4, ptsz = 4; break;
            case N_P2D:     otype = GLU_MAP1_TRIM_2, ptsz = 2; break;
            case N_P2DR:    otype = GLU_MAP1_TRIM_3, ptsz = 3; break;
            default:        return;
        }

        /* convert input parameters of type double to float */
        knots = malloc (knotCount * sizeof(GLfloat));
        ctls = malloc (ptsz * knotCount * sizeof(GLfloat));

        for (i = 0; i < knotCount; i++)
        {
            knots[i] = *knotList++;
            for (j = 0; j < ptsz; j++)
                ctls[i*ptsz + j] = ctlArray[j];
            ctlArray = (double *)((char *)ctlArray + offset);
        }

        gluNurbsCurve (igl->nurbsCurve, knotCount, knots, ptsz, ctls, order, otype);

        free (ctls);
        free (knots);
    }
}


/* GL: bgnsurface, endsurface - delimit a NURBS surface definition */
void
bgnsurface (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->bgnsurface ();
}

void
iglExec_bgnsurface (void)
{
    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
    {
        /* GL NURBS surfaces have implied normals, thus enable lighting */
        glEnable (GL_AUTO_NORMAL);
        IGL_ENLIGHT ();
        gluBeginSurface (igl->nurbsCurve);
    }
}


/* GL: bgnsurface, endsurface - delimit a NURBS surface definition */
void
endsurface (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->endsurface ();
}

void
iglExec_endsurface (void)
{
    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
    {
        gluEndSurface (igl->nurbsCurve);
        glDisable (GL_AUTO_NORMAL);
    }
}


/* nurbssurface - controls the shape of a NURBS surface */
void
nurbssurface (long scount, const double *sknot, long tcount, const double *tknot, long soffset,
              long toffset, const double *ctlArray, long sorder, long torder, long type)
{
    _igl_trace (__func__,FALSE,"%ld,%p,%ld,%p,%ld,%ld,%p,%ld,%ld,%ld",
         scount,sknot,tcount,tknot,soffset,toffset,ctlArray,sorder,torder,type);
    igl->api->nurbssurface (scount, sknot, tcount, tknot, soffset, toffset, ctlArray, sorder, torder, type);
}

void
iglExec_nurbssurface (long scount, const double *sknot, long tcount, const double *tknot, long soffset,
              long toffset, const double *ctlArray, long sorder, long torder, long type)
{
    GLfloat *sknots, *tknots, *ctls;
    int otype, ptsz, i, j, k;

    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
    {
        switch (type)
        {
            case N_V3D:     otype = GL_MAP2_VERTEX_3, ptsz = 3; break;
            case N_V3DR:    otype = GL_MAP2_VERTEX_4, ptsz = 4; break;
            case N_C4D:     otype = GL_MAP2_COLOR_4, ptsz = 4; break;
            case N_T2D:     otype = GL_MAP2_TEXTURE_COORD_2, ptsz = 2; break;
            case N_T2DR:    otype = GL_MAP2_TEXTURE_COORD_3, ptsz = 3; break;
            default:        return;    /* others (e.g. N_C4DR) not supported */
        }

        /* convert input parameters of type double to float */
        sknots = malloc (scount * sizeof(GLfloat));
        tknots = malloc (tcount * sizeof(GLfloat));
        ctls = malloc (scount * tcount * ptsz * sizeof(GLfloat));

        for (i = 0; i < scount; i++)
            sknots[i] = *sknot++;
        for (i = 0; i < tcount; i++)
            tknots[i] = *tknot++;
        for (i = 0; i < tcount; i++)
        {
            const double *ctlp = ctlArray;
            for (j = 0; j < scount; j++)
            {
                for (k = 0; k < ptsz; k++)
                    ctls[(i*scount + j)*ptsz + k] = ctlp[k];
                ctlp = (double *)((char *)ctlp + soffset);
            }
            ctlArray = (double *)((char *)ctlArray + toffset);
        }

        gluNurbsSurface (igl->nurbsCurve, scount, sknots, tcount, tknots,
            ptsz, scount * ptsz, ctls, sorder, torder, otype);

        free (ctls);
        free (sknots);
        free (tknots);
    }
}


/* GL: bgntrim, endtrim - delimit a NURBS surface trimming loop */
void
bgntrim (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->bgntrim ();
}

void
iglExec_bgntrim (void)
{
    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
        gluBeginTrim (igl->nurbsCurve);
}


/* GL: bgntrim, endtrim - delimit a NURBS surface trimming loop */
void
endtrim (void)
{
    _igl_trace (__func__,FALSE,"");
    igl->api->endtrim ();
}

void
iglExec_endtrim (void)
{
    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
        gluEndTrim (igl->nurbsCurve);
}


/* GL: pwlcurve - describes a piecewise linear trimming curve for NURBS surfaces */
void
pwlcurve (long n, double *dataArray, long byteSize, long type)
{
    _igl_trace (__func__,FALSE,"%ld,%p,%ld,%ld",n,dataArray,byteSize,type);
    igl->api->pwlcurve (n, dataArray, byteSize, type);
}

void
iglExec_pwlcurve (long n, double *dataArray, long byteSize, long type)
{
    GLfloat *knots;
    int i;

    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
    {
        if (type != N_ST)   /* the only one supported by IrisGL */
            return;

        /* convert input parameters of type double to float */
        knots = malloc (2*n * sizeof(GLfloat));
        for (i = 0; i < n; i++)
        {
            knots[2*i+0] = dataArray[0];
            knots[2*i+1] = dataArray[1];
            dataArray = (double *)((char *)dataArray + byteSize);
        }

        gluPwlCurve (igl->nurbsCurve, n, knots, 2, GLU_MAP1_TRIM_2);

        free (knots);
    }
}


/* GL: setnurbsproperty - sets a property for the display of trimmed NURBS surfaces */
void
setnurbsproperty (long property, float value)
{
    _igl_trace (__func__,FALSE,"%ld,%g",property,value);
    igl->api->setnurbsproperty (property, value);
}

void
iglExec_setnurbsproperty (long property, float value)
{
    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
    {
        if (property == N_ERRORCHECKING)
        {
            /* not supported in OpenGL? */
        }
        else if (property == N_PIXEL_TOLERANCE)
            gluNurbsProperty (igl->nurbsCurve, GLU_SAMPLING_TOLERANCE, value);
        else if (property == N_DISPLAY)
        {
            if (value == N_FILL)
                gluNurbsProperty (igl->nurbsCurve, GLU_DISPLAY_MODE, GLU_FILL);
            else if (value == N_OUTLINE_POLY)
                gluNurbsProperty (igl->nurbsCurve, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
            else if (value == N_OUTLINE_PATCH)
                gluNurbsProperty (igl->nurbsCurve, GLU_DISPLAY_MODE, GLU_OUTLINE_PATCH);
        }
        else if (property == N_CULLING)
            gluNurbsProperty (igl->nurbsCurve, GLU_CULLING, value);
    }
}


void
getnurbsproperty (long property, float *value)
{
    _igl_trace (__func__,TRUE,"%ld,%p",property,value);
    IGL_CHECKWNDV ();

    if (igl->nurbsCurve != NULL)
    {
        if (property == N_ERRORCHECKING)
        {
            /* not supported in OpenGL? */
            *value = 0;
        }
        else if (property == N_PIXEL_TOLERANCE)
            gluGetNurbsProperty (igl->nurbsCurve, GLU_SAMPLING_TOLERANCE, value);
        else if (property == N_DISPLAY)
        {
            gluGetNurbsProperty (igl->nurbsCurve, GLU_DISPLAY_MODE, value);
            switch ((long)*value)
            {
                case GLU_FILL:              *value = N_FILL; break;
                case GLU_OUTLINE_POLYGON:   *value = N_OUTLINE_POLY; break;
                case GLU_OUTLINE_PATCH:     *value = N_OUTLINE_PATCH; break;
                default:                    *value = N_FILL; break;
            }
        }
        else if (property == N_CULLING)
            gluGetNurbsProperty (igl->nurbsCurve, GLU_CULLING, value);
    }
}
