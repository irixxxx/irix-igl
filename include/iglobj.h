/*
 * iglobj.h
 *
 * IGL object API definitions
 */

#ifndef IGLOBJ_H
#define IGLOBJ_H


#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    OP_NONE = 0,
    OP_CLEAR,
    OP_LSETDEPTH,
    OP_SWAPBUFFERS,
    OP_MSWAPBUFFERS,
    OP_BACKBUFFER,
    OP_FRONTBUFFER,
    OP_LEFTBUFFER,
    OP_RIGHTBUFFER,
    OP_BACKFACE,
    OP_FRONTFACE,
    OP_DEPTHCUE,
    OP_LSHADERANGE,
    OP_LRGBRANGE,
    OP_ZBUFFER,
    OP_ZFUNCTION,
    OP_ZCLEAR,
    OP_CZCLEAR,
    OP_SCLEAR,
    OP_STENCIL,
    OP_ACBUF,
    OP_SHADEMODEL,
    OP_COLOR,
    OP_MAPCOLOR,
    OP_CPACK,
    OP_RGBCOLOR,
    OP_AFUNCTION,
    OP_BLENDFUNCTION,
    OP_DITHER,
    OP_FOGVERTEX,
    OP_LOGICOP,
    OP_C4F,
    OP_C4I,
    OP_C4S,
    OP_MMODE,
    OP_LOADMATRIX,
    OP_MULTMATRIX,
    OP_PUSHMATRIX,
    OP_POPMATRIX,
    OP_ROT,
    OP_ROTATE,
    OP_TRANSLATE,
    OP_SCALE,
    OP_POLARVIEW,
    OP_CLIPPLANE,
    OP_ORTHO,
    OP_ORTHO2,
    OP_WINDOW,
    OP_PERSPECTIVE,
    OP_VIEWPORT,
    OP_PUSHVIEWPORT,
    OP_POPVIEWPORT,
    OP_LOOKAT,
    OP_SCRMASK,
    OP_POLF,
    OP_POLFI,
    OP_POLFS,
    OP_POLF2,
    OP_POLF2I,
    OP_POLF2S,
    OP_POLY,
    OP_POLYI,
    OP_POLYS,
    OP_POLY2,
    OP_POLY2I,
    OP_POLY2S,
    OP_POLYMODE,
    OP_POLYSMOOTH,
    OP_DISPLACEPOLYGON,
    OP_PMV,
    OP_PDR,
    OP_PCLOS,
    OP_PNT,
    OP_PNTSIZEF,
    OP_PNTSMOOTH,
    OP_RECT,
    OP_RECTF,
    OP_SBOX,
    OP_SBOXF,
    OP_V4F,
    OP_ARC,
    OP_ARCF,
    OP_CIRC,
    OP_CIRCF,
    OP_BGNTMESH,
    OP_SWAPTMESH,
    OP_BGNCLOSEDLINE,
    OP_BGNCURVE,
    OP_ENDCURVE,
    OP_NURBSCURVE,
    OP_BGNSURFACE,
    OP_ENDSURFACE,
    OP_NURBSSURFACE,
    OP_BGNTRIM,
    OP_ENDTRIM,
    OP_PWLCURVE,
    OP_SETNURBSPROPERTY,
    OP_BGNLINE,
    OP_BGNPOINT,
    OP_ENDPOINT,
    OP_BGNPOLYGON,
    OP_BGNQSTRIP,
    OP_CURVEIT,
    OP_CRVN,
    OP_RCRVN,
    OP_CURVEPRECISION,
    OP_CURVEBASIS,
    OP_PATCH,
    OP_RPATCH,
    OP_PATCHCURVES,
    OP_PATCHPRECISION,
    OP_PATCHBASIS,
    OP_NMODE,
    OP_N3F,
    OP_LINESMOOTH,
    OP_LINEWIDTHF,
    OP_SETLINESTYLE,
    OP_LSREPEAT,
    OP_SETPATTERN,
    OP_LMBIND,
    OP_LMCOLOR,
    OP_MOVE,
    OP_RMV,
    OP_RPMV,
    OP_DRAW,
    OP_RDR,
    OP_RPDR,
    OP_SPLF,
    OP_SPLFI,
    OP_SPLFS,
    OP_SPLF2,
    OP_SPLF2I,
    OP_SPLF2S,
    OP_CALLOBJ,
    OP_CALLFUNC,        /* NOTE: this one is special. it can only be called from the object */
    OP_TEVBIND,
    OP_TEXBIND,
    OP_TEXGEN,
    OP_T4F,
    OP_CMOV,
    OP_LCHARSTR,
    OP_FONT,
    OP_PUSHATTRIBUTES,
    OP_POPATTRIBUTES,
    OP_INITNAMES,
    OP_LOADNAME,
    OP_PUSHNAME,
    OP_POPNAME,
} igl_opcodeT;

typedef union
{
    char c;
    unsigned char uc;
    short s;
    unsigned short us;
    int i;
    unsigned int ui;
    float f;
    void *v;
} _igl_objFcnParamU;

typedef struct
{
    char type;
    int opcode;
    _igl_objFcnParamU params[10];
} igl_objFcnT;


void
iglExec_clear ();

void
iglObj_clear ();

void
iglExec_lsetdepth (long near_, long far_);

void
iglObj_lsetdepth (long near_, long far_);

void
iglExec_swapbuffers ();

void
iglObj_swapbuffers ();

void
iglExec_mswapbuffers (long fbuf);

void
iglObj_mswapbuffers (long fbuf);

void
iglExec_backbuffer (Boolean enable);

void
iglObj_backbuffer (Boolean enable);

void
iglExec_frontbuffer (Boolean enable);

void
iglObj_frontbuffer (Boolean enable);

void
iglExec_leftbuffer (Boolean enable);

void
iglObj_leftbuffer (Boolean enable);

void
iglExec_rightbuffer (Boolean enable);

void
iglObj_rightbuffer (Boolean enable);

void
iglExec_backface (Boolean enable);

void
iglObj_backface (Boolean enable);

void
iglExec_frontface (Boolean enable);

void
iglObj_frontface (Boolean enable);

void
iglExec_depthcue (Boolean enable);

void
iglObj_depthcue (Boolean enable);

void
iglExec_lshaderange (Colorindex lowin, Colorindex highin, long znear, long zfar);

void
iglObj_lshaderange (Colorindex lowin, Colorindex highin, long znear, long zfar);

void
iglExec_lRGBrange (short rmin, short gmin, short bmin, short rmax, short gmax, short bmax, long znear, long zfar);

void
iglObj_lRGBrange (short rmin, short gmin, short bmin, short rmax, short gmax, short bmax, long znear, long zfar);

void
iglExec_zbuffer (Boolean enable);

void
iglObj_zbuffer (Boolean enable);

void
iglExec_zfunction (long func);

void
iglObj_zfunction (long func);

void
iglExec_zclear ();

void
iglObj_zclear ();

void
iglExec_czclear (unsigned long cval, long zval);

void
iglObj_czclear (unsigned long cval, long zval);

void
iglExec_sclear (unsigned long sval);

void
iglObj_sclear (unsigned long sval);

void
iglExec_stencil (long enable, unsigned long ref, long func, unsigned long mask, long fail, long pass, long zpass);

void
iglObj_stencil (long enable, unsigned long ref, long func, unsigned long mask, long fail, long pass, long zpass);

void
iglExec_acbuf (long op, float value);

void
iglObj_acbuf (long op, float value);

void
iglExec_shademodel (int mode);

void
iglObj_shademodel (int mode);

void
iglExec_color (Colorindex c);

void
iglObj_color (Colorindex c);

void
iglExec_mapcolor (Colorindex i, short r, short g, short b);

void
iglObj_mapcolor (Colorindex i, short r, short g, short b);

void
iglExec_cpack (unsigned long color);

void
iglObj_cpack (unsigned long color);

void
iglExec_RGBcolor (short r, short g, short b);

void
iglObj_RGBcolor (short r, short g, short b);

void
iglExec_afunction (long ref, long func);

void
iglObj_afunction (long ref, long func);

void
iglExec_blendfunction (long sfactor, long dfactor);

void
iglObj_blendfunction (long sfactor, long dfactor);

void
iglExec_dither (long mode);

void
iglObj_dither (long mode);

void
iglExec_fogvertex (long mode, float *params);

void
iglObj_fogvertex (long mode, float *params);

void
iglExec_logicop (long opcode);

void
iglObj_logicop (long opcode);

void
iglExec_c4f (float cv[4]);

void
iglObj_c4f (float cv[4]);

void
iglExec_c4i (int cv[4]);

void
iglObj_c4i (int cv[4]);

void
iglExec_c4s (short cv[4]);

void
iglObj_c4s (short cv[4]);

void
iglExec_mmode (short mode);

void
iglObj_mmode (short mode);

void
iglExec_loadmatrix (Matrix m);

void
iglObj_loadmatrix (Matrix m);

void
iglExec_multmatrix (Matrix m);

void
iglObj_multmatrix (Matrix m);

void
iglExec_pushmatrix ();

void
iglObj_pushmatrix ();

void
iglExec_popmatrix ();

void
iglObj_popmatrix ();

void
iglExec_rot (float amount, char angle);

void
iglObj_rot (float amount, char angle);

void
iglExec_rotate (Angle amount, char angle);

void
iglObj_rotate (Angle amount, char angle);

void
iglExec_translate (Coord x, Coord y, Coord z);

void
iglObj_translate (Coord x, Coord y, Coord z);

void
iglExec_scale (float x, float y, float z);

void
iglObj_scale (float x, float y, float z);

void
iglExec_polarview (Coord dist, Angle azim, Angle inc, Angle twist);

void
iglObj_polarview (Coord dist, Angle azim, Angle inc, Angle twist);

void
iglExec_clipplane (long index, long mode, float *params);

void
iglObj_clipplane (long index, long mode, float *params);

void
iglExec_ortho (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_);

void
iglObj_ortho (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_);

void
iglExec_ortho2 (Coord left, Coord right, Coord bottom, Coord top);

void
iglObj_ortho2 (Coord left, Coord right, Coord bottom, Coord top);

void
iglExec_window (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_);

void
iglObj_window (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_);

void
iglExec_perspective (Angle fovy, float aspect, Coord near_, Coord far_);

void
iglObj_perspective (Angle fovy, float aspect, Coord near_, Coord far_);

void
iglExec_viewport (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);

void
iglObj_viewport (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);

void
iglExec_pushviewport ();

void
iglObj_pushviewport ();

void
iglExec_popviewport ();

void
iglObj_popviewport ();

void
iglExec_lookat (Coord vx, Coord vy, Coord vz, Coord px, Coord py, Coord pz, Angle twist);

void
iglObj_lookat (Coord vx, Coord vy, Coord vz, Coord px, Coord py, Coord pz, Angle twist);

void
iglExec_scrmask (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);

void
iglObj_scrmask (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);

void
iglExec_polf (long n, const Coord parray[][3]);

void
iglObj_polf (long n, const Coord parray[][3]);

void
iglExec_polfi (long n, const Icoord parray[][3]);

void
iglObj_polfi (long n, const Icoord parray[][3]);

void
iglExec_polfs (long n, const Scoord parray[][3]);

void
iglObj_polfs (long n, const Scoord parray[][3]);

void
iglExec_polf2 (long n, const Coord parray[][2]);

void
iglObj_polf2 (long n, const Coord parray[][2]);

void
iglExec_polf2i (long n, const Icoord parray[][2]);

void
iglObj_polf2i (long n, const Icoord parray[][2]);

void
iglExec_polf2s (long n, const Scoord parray[][2]);

void
iglObj_polf2s (long n, const Scoord parray[][2]);

void
iglExec_poly (long n, const Coord parray[][3]);

void
iglObj_poly (long n, const Coord parray[][3]);

void
iglExec_polyi (long n, const Icoord parray[][3]);

void
iglObj_polyi (long n, const Icoord parray[][3]);

void
iglExec_polys (long n, const Scoord parray[][3]);

void
iglObj_polys (long n, const Scoord parray[][3]);

void
iglExec_poly2 (long n, const Coord parray[][2]);

void
iglObj_poly2 (long n, const Coord parray[][2]);

void
iglExec_poly2i (long n, const Icoord parray[][2]);

void
iglObj_poly2i (long n, const Icoord parray[][2]);

void
iglExec_poly2s (long n, const Scoord parray[][2]);

void
iglObj_poly2s (long n, const Scoord parray[][2]);

void
iglExec_polymode (long mode);

void
iglObj_polymode (long mode);

void
iglExec_polysmooth (long mode);

void
iglObj_polysmooth (long mode);

void
iglExec_displacepolygon (float scalefactor);

void
iglObj_displacepolygon (float scalefactor);

void
iglExec_pmv (Coord x, Coord y, Coord z);

void
iglObj_pmv (Coord x, Coord y, Coord z);

void
iglExec_pdr (Coord x, Coord y, Coord z);

void
iglObj_pdr (Coord x, Coord y, Coord z);

void
iglExec_pclos ();

void
iglObj_pclos ();

void
iglExec_pnt (Coord x, Coord y, Coord z);

void
iglObj_pnt (Coord x, Coord y, Coord z);

void
iglExec_pntsizef (float n);

void
iglObj_pntsizef (float n);

void
iglExec_pntsmooth (unsigned long mode);

void
iglObj_pntsmooth (unsigned long mode);

void
iglExec_rect (Coord a, Coord b, Coord c, Coord d);

void
iglObj_rect (Coord a, Coord b, Coord c, Coord d);

void
iglExec_rectf (Coord a, Coord b, Coord c, Coord d);

void
iglObj_rectf (Coord a, Coord b, Coord c, Coord d);

void
iglExec_sbox (Coord a, Coord b, Coord c, Coord d);

void
iglObj_sbox (Coord a, Coord b, Coord c, Coord d);

void
iglExec_sboxf (Coord a, Coord b, Coord c, Coord d);

void
iglObj_sboxf (Coord a, Coord b, Coord c, Coord d);

void
iglExec_v4f (float vector[4]);

void
iglObj_v4f (float vector[4]);

void
iglExec_arc (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle);

void
iglObj_arc (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle);

void
iglExec_arcf (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle);

void
iglObj_arcf (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle);

void
iglExec_circ (Coord x, Coord y, Coord radius);

void
iglObj_circ (Coord x, Coord y, Coord radius);

void
iglExec_circf (Coord x, Coord y, Coord radius);

void
iglObj_circf (Coord x, Coord y, Coord radius);

void
iglExec_bgntmesh ();

void
iglObj_bgntmesh ();

void
iglExec_swaptmesh ();

void
iglObj_swaptmesh ();

void
iglExec_bgnclosedline ();

void
iglObj_bgnclosedline ();

void
iglExec_bgncurve ();

void
iglObj_bgncurve ();

void
iglExec_endcurve ();

void
iglObj_endcurve ();

void 
iglExec_nurbscurve (long knotCount, const double *knotList, long offset, const double *ctlArray, long order, long type);

void 
iglObj_nurbscurve (long knotCount, const double *knotList, long offset, const double *ctlArray, long order, long type);

void
iglExec_bgnsurface ();

void
iglObj_bgnsurface ();

void
iglExec_endsurface ();

void
iglObj_endsurface ();

void
iglExec_nurbssurface (long scount, const double *sknot, long tcount, const double *tknot, long soffset, long toffset, const double *ctlArray, long sorder, long torder, long type);

void
iglObj_nurbssurface (long scount, const double *sknot, long tcount, const double *tknot, long soffset, long toffset, const double *ctlArray, long sorder, long torder, long type);

void
iglExec_bgntrim ();

void
iglObj_bgntrim ();

void
iglExec_endtrim ();

void
iglObj_endtrim ();

void
iglExec_pwlcurve (long n, double *dataArray, long byteSize, long type);

void
iglObj_pwlcurve (long n, double *dataArray, long byteSize, long type);

void
iglExec_setnurbsproperty (long property, float value);

void
iglObj_setnurbsproperty (long property, float value);

void
iglExec_bgnline ();

void
iglObj_bgnline ();

void
iglExec_bgnpoint ();

void
iglObj_bgnpoint ();

void
iglExec_endpoint ();

void
iglObj_endpoint ();

void
iglExec_bgnpolygon ();

void
iglObj_bgnpolygon ();

void
iglExec_bgnqstrip ();

void
iglObj_bgnqstrip ();

void
iglExec_curveit (short niter);

void
iglObj_curveit (short niter);

void
iglExec_crvn (long n, Coord points[][3]);

void
iglObj_crvn (long n, Coord points[][3]);

void
iglExec_rcrvn (long n, Coord points[][4]);

void
iglObj_rcrvn (long n, Coord points[][4]);

void
iglExec_curveprecision (short n);

void
iglObj_curveprecision (short n);

void
iglExec_curvebasis (short n);

void
iglObj_curvebasis (short n);

void
iglExec_patch(Matrix geomx, Matrix geomy, Matrix geomz);

void
iglObj_patch(Matrix geomx, Matrix geomy, Matrix geomz);

void
iglExec_rpatch(Matrix geomx, Matrix geomy, Matrix geomz, Matrix geomw);

void
iglObj_rpatch(Matrix geomx, Matrix geomy, Matrix geomz, Matrix geomw);

void
iglExec_patchcurves (short nu, short nv);

void
iglObj_patchcurves (short nu, short nv);

void
iglExec_patchprecision (short nu, short nv);

void
iglObj_patchprecision (short nu, short nv);

void
iglExec_patchbasis (short nu, short nv);

void
iglObj_patchbasis (short nu, short nv);

void
iglExec_nmode (long mode);

void
iglObj_nmode (long mode);

void
iglExec_n3f (float *vector);

void
iglObj_n3f (float *vector);

void
iglExec_linesmooth (unsigned long mode);

void
iglObj_linesmooth (unsigned long mode);

void
iglExec_linewidthf (float width);

void
iglObj_linewidthf (float width);

void
iglExec_setlinestyle (short n);

void
iglObj_setlinestyle (short n);

void
iglExec_lsrepeat (long factor);

void
iglObj_lsrepeat (long factor);

void
iglExec_setpattern (short index);

void
iglObj_setpattern (short index);

void
iglExec_lmbind (short target, short index);

void
iglObj_lmbind (short target, short index);

void
iglExec_lmcolor (long mode);

void
iglObj_lmcolor (long mode);

void
iglExec_move (Coord x, Coord y, Coord z);

void
iglObj_move (Coord x, Coord y, Coord z);

void
iglExec_rmv (Coord dx, Coord dy, Coord dz);

void
iglObj_rmv (Coord dx, Coord dy, Coord dz);

void
iglExec_rpmv (Coord dx, Coord dy, Coord dz);

void
iglObj_rpmv (Coord dx, Coord dy, Coord dz);

void
iglExec_draw (Coord x, Coord y, Coord z);

void
iglObj_draw (Coord x, Coord y, Coord z);

void
iglExec_rdr (Coord dx, Coord dy, Coord dz);

void
iglObj_rdr (Coord dx, Coord dy, Coord dz);

void
iglExec_rpdr (Coord dx, Coord dy, Coord dz);

void
iglObj_rpdr (Coord dx, Coord dy, Coord dz);

void
iglExec_splf (long n, Coord parray[][3], Colorindex iarray[]);

void
iglObj_splf (long n, Coord parray[][3], Colorindex iarray[]);

void
iglExec_splfi (long n, Icoord parray[][3], Colorindex iarray[]);

void
iglObj_splfi (long n, Icoord parray[][3], Colorindex iarray[]);

void
iglExec_splfs (long n, Scoord parray[][3], Colorindex iarray[]);

void
iglObj_splfs (long n, Scoord parray[][3], Colorindex iarray[]);

void
iglExec_splf2 (long n, Coord parray[][2], Colorindex iarray[]);

void
iglObj_splf2 (long n, Coord parray[][2], Colorindex iarray[]);

void
iglExec_splf2i (long n, Icoord parray[][2], Colorindex iarray[]);

void
iglObj_splf2i (long n, Icoord parray[][2], Colorindex iarray[]);

void
iglExec_splf2s (long n, Scoord parray[][2], Colorindex iarray[]);

void
iglObj_splf2s (long n, Scoord parray[][2], Colorindex iarray[]);

void
iglExec_callobj (Object id);

void
iglObj_callobj (Object id);

void
iglExec_tevbind (long target, long index);

void
iglObj_tevbind (long target, long index);

void
iglExec_texbind (long target, long index);

void
iglObj_texbind (long target, long index);

void
iglExec_texgen (long coord, long mode, float *params);

void
iglObj_texgen (long coord, long mode, float *params);

void
iglExec_t4f (float vector[4]);

void
iglObj_t4f (float vector[4]);

void
iglExec_cmov (Coord x, Coord y, Coord z);

void
iglObj_cmov (Coord x, Coord y, Coord z);

void
iglExec_lcharstr (long type, void *str);

void
iglObj_lcharstr (long type, void *str);

void
iglExec_font (short fntnum);

void
iglObj_font (short fntnum);

void
iglExec_pushattributes ();

void
iglObj_pushattributes ();

void
iglExec_popattributes ();

void
iglObj_popattributes ();

void
iglExec_initnames ();

void
iglObj_initnames ();

void
iglExec_loadname (short name);

void
iglObj_loadname (short name);

void
iglExec_pushname (short name);

void
iglObj_pushname (short name);

void
iglExec_popname ();

void
iglObj_popname ();

#ifdef __cplusplus
}
#endif

#endif  /* IGLOBJ_H */

