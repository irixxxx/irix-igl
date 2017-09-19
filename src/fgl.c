/*
 * fgl.c
 *
 * f77 wrappers for the complete GL functionality
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

#include <igl.h>

#include <alloca.h>
#include <string.h>


/* old-style fortran 77 interface to igl */

/* additional data types for values which are passed by reference in f77 */
#define rScreencoord    rShort
#define rColorindex     rShort
#define rDevice         rShort
#define rAngle          rShort
#define rLinestyle      rShort
#define rObject         rLong
#define rTag            rLong
#define rOffset         rLong
#define rBoolean        rLong
#define rByte           rChar

#define rCoord          const Coord *
#define rIcoord         const Icoord *
#define rScoord         const Scoord *

/* NOTE: short values are apparently always passed as ints in libfgl */
#define rChar           const char *
#define rShort          const int *
#define rInt            const int *
#define rLong           const long *
#define rFloat          const float *


/* fortran system dependant symbol mapping */
#define FTN(n)  n##_

/* make a C string out of an F77 string */
#define Cstring(s,a,l)  char *s = alloca((l)+1); memcpy (s,a,l); s[l] = '\0';

/***************************************
    window-related functions (window.c)
****************************************/
void
FTN(prefsi) (rInt width, rInt height)
	{ prefsize (*width, *height); }

void
FTN(maxsiz) (rLong width, rLong height)
	{ maxsize (*width, *height); }

void
FTN(minsiz) (rLong width, rLong height)
	{ minsize (*width, *height); }

void
FTN(prefpo) (rInt x0, rInt x1, rInt y0, rInt y1)
	{ prefposition (*x0, *x1, *y0, *y1); }

long
FTN(winope) (String windowTitle, rInt len)
	{ Cstring (s, windowTitle, *len); return winopen (s); }

long
FTN(swinop) (rLong parent)
	{ return swinopen (*parent); }

void
FTN(winclo) (rLong gwid)
	{ winclose (*gwid); }

void
FTN(wincon) (void)
	{ winconstraints (); }

void
FTN(winpos) (rInt x0, rInt x1, rInt y0, rInt y1)
	{ winposition (*x0, *x1, *y0, *y1); }

long
FTN(winatt) (void)
	{ return winattach (); }

long
FTN(winget) (void)
	{ return winget (); }

void
FTN(winset) (rLong gwid)
	{ winset (*gwid); }

void
FTN(winpus) (void)
	{ winpush (); }

void
FTN(winpop) (void)
	{ winpop (); }

void
FTN(winmov) (rLong orgx, rLong orgy)
	{ winmove (*orgx, *orgy); }

void
FTN(wintit) (String name, rInt len)
	{ Cstring (s, name, *len); wintitle (s); }

long
FTN(winat) (void)
	{ return winat (); }

void
FTN(keepas) (rInt x, rInt y)
	{ keepaspect (*x, *y); }

void
FTN(stepun) (rInt x, rInt y)
	{ stepunit (*x, *y); }

void
FTN(getsiz) (long *x, long *y)
	{ getsize (x, y); }

void
FTN(getori) (long *x, long *y)
	{ getorigin (x, y); }

long
FTN(windep) (rLong gwid)
	{ return windepth (*gwid); }

void
FTN(gconfi) (void)
	{ gconfig (); }

long
FTN(getgco) (rLong buffer)
	{ return getgconfig (*buffer); }

long
FTN(getpla) (void)
	{ return getplanes (); }

void
FTN(fullsc) (void)
	{ fullscrn (); }

void
FTN(endful) (void)
	{ endfullscrn (); }

int 
FTN(getdra) (void)
	{ return getdrawmode (); }

void
FTN(drawmo) (rInt mode)
	{ drawmode (*mode); }

void
FTN(getpor) (String name, rInt len)
	{ Cstring (s, name, *len); getport (s); }

void
FTN(nobord) (void)
	{ noborder (); }

void
FTN(noport) (void)
	{ noport (); }

void
FTN(imakeb) (void)
	{ imakebackground (); }


/*******************************************
     buffer-related functions (buffer.c)
********************************************/
void
FTN(clear) (void)
	{ clear (); }

void
FTN(cmode) (void)
	{ cmode (); }

void
FTN(rgbmod) (void)
	{ RGBmode (); }

void
FTN(rgbsiz) (rLong planes)
	{ RGBsize (*planes); }

long
FTN(getdis) (void)
	{ return getdisplaymode (); }

void
FTN(lsetde) (rLong near_, rLong far_)
	{ lsetdepth (*near_, *far_); }

void
FTN(setdep) (rScreencoord near_, rScreencoord far_)
	{ setdepth (*near_, *far_); }

void
FTN(getdep) (Screencoord *near_, Screencoord *far_)
	{ getdepth (near_, far_); }

void
FTN(single) (void)
	{ singlebuffer (); }

void
FTN(double) (void)
	{ doublebuffer (); }

void
FTN(monobu) (void)
	{ monobuffer (); }

void
FTN(stereo) (void)
	{ stereobuffer (); }

void
FTN(swapbu) (void)
	{ swapbuffers (); }

void
FTN(mswapb) (rLong fbuf)
        { mswapbuffers (*fbuf); }

void
FTN(backbu) (rBoolean enable)
	{ backbuffer (*enable); }

void
FTN(frontb) (rBoolean enable)
	{ frontbuffer (*enable); }

void
FTN(leftbu) (rBoolean enable)
	{ leftbuffer (*enable); }

void
FTN(rightb) (rBoolean enable)
	{ rightbuffer (*enable); }

long
FTN(getbuf) (void)
	{ return getbuffer (); }

void
FTN(backfa) (rBoolean enable)
	{ backface (*enable); }

long
FTN(getbac) (void)
	{ return getbackface (); }

void
FTN(frontf) (rBoolean enable)
	{ frontface (*enable); }

void
FTN(depthc) (rBoolean enable)
	{ depthcue (*enable); }

Boolean
FTN(getdcm) (void)
	{ return getdcm (); }

void
FTN(lshade) (rColorindex lowin, rColorindex highin, rLong znear, rLong zfar)
	{ lshaderange (*lowin, *highin, *znear, *zfar); }

void
FTN(shader) (rColorindex lowin, rColorindex highin, rScreencoord z1, rScreencoord z2)
	{ shaderange (*lowin, *highin, *z1, *z2); }

void
FTN(lrgbra) (rShort rmin, rShort gmin, rShort bmin, rShort rmax, rShort gmax, rShort bmax, rLong znear, rLong zfar)
	{ lRGBrange (*rmin, *gmin, *bmin, *rmax, *gmax, *bmax, *znear, *zfar); }

void
FTN(rgbran) (rShort rmin, rShort bmin, rShort gmin, rShort rmax, rShort gmax, rShort bmax, rScreencoord znear, rScreencoord zfar)
	{ RGBrange (*rmin, *bmin, *gmin, *rmax, *gmax, *bmax, *znear, *zfar); }

void
FTN(zbuffe) (rBoolean enable)
	{ zbuffer (*enable); }

Boolean
FTN(getzbu) (void)
	{ return getzbuffer (); }

void
FTN(zfunct) (rLong func)
	{ zfunction (*func); }

void
FTN(zwrite) (unsigned rLong mask)
	{ zwritemask (*mask); }

void
FTN(zclear) (void)
	{ zclear (); }

void
FTN(czclea) (unsigned rLong cval, rLong zval)
	{ czclear (*cval, *zval); }

void
FTN(zbsize) (rLong planes)
	{ zbsize (*planes); }

void
FTN(sclear) (unsigned rLong sval)
	{ sclear (*sval); }

void
FTN(stenci) (rLong enable, unsigned rLong ref, rLong func, unsigned rLong mask, rLong fail, rLong pass, rLong zpass)
	{ stencil (*enable, *ref, *func, *mask, *fail, *pass, *zpass); }

void
FTN(swrite) (unsigned rLong mask)
        { swritemask (*mask); }

void
FTN(stensi) (rLong planes)
	{ stensize (*planes); }

void
FTN(acbuf) (rLong op, rFloat value)
	{ acbuf (*op, *value); }

void
FTN(acsize) (rLong planes)
	{ acsize (*planes); }

void
FTN(gsync) (void)
	{ gsync (); }

void
FTN(swapin) (rShort interval)
	{ swapinterval (*interval); }

void
FTN(shadem) (rInt mode)
	{ shademodel (*mode); }

long
FTN(getsm) (void)
	{ return getsm (); }


/***************************************
    color-related functions (color.c)
****************************************/
void
FTN(color) (rColorindex c)
	{ color (*c); }

void
FTN(colorf) (rFloat c)
	{ colorf (*c); }

long
FTN(getcol) (void)
	{ return getcolor (); }

void
FTN(mapcol) (rColorindex i, rShort r, rShort g, rShort b)
	{ mapcolor (*i, *r, *g, *b); }

void
FTN(getmco) (rColorindex i, short *r, short *g, short *b)
	{ getmcolor (*i, r, g, b); }

void
FTN(writem) (rColorindex wtm)
        { writemask (*wtm); }

long
FTN(getwri) (void)
        { return getwritemask (); }

void
FTN(setsha) (rColorindex shade)
	{ setshade (*shade); }

long
FTN(getsha) (void)
	{ return getshade (); }

void
FTN(cpack) (unsigned rLong color)
	{ cpack (*color); }

void
FTN(wmpack) (unsigned rLong color)
	{ wmpack (*color); }

void
FTN(rgbcol) (rShort r, rShort g, rShort b)
	{ RGBcolor (*r, *g, *b); }

void
FTN(grgbco) (short *r, short *g, short *b)
	{ gRGBcolor (r, g, b); }

void
FTN(rgbwri) (rShort redm, rShort greenm, rShort bluem)
	{ RGBwritemask (*redm, *greenm, *bluem); }

void
FTN(grgbma) (short *redm, short *greenm, short *bluem)
	{ gRGBmask (redm, greenm, bluem); }

void
FTN(afunct) (rLong ref, rLong func)
	{ afunction (*ref, *func); }

void
FTN(blendc) (rFloat r, rFloat g, rFloat b, rFloat a)
	{ blendcolor (*r, *g, *b, *a); }

void
FTN(blendf) (rLong sfactor, rLong dfactor)
	{ blendfunction (*sfactor, *dfactor); }

void
FTN(dither) (rLong mode)
	{ dither (*mode); }

void
FTN(fogver) (rLong mode, float *params)
	{ fogvertex (*mode, params); }

void
FTN(logico) (rLong opcode)
	{ logicop (*opcode); }

void
FTN(c3f) (float cv[3])
	{ c3f (cv); }

void
FTN(c3i) (int cv[3])
	{ c3i (cv); }

void
FTN(c3s) (short cv[3])
	{ c3s (cv); }

void
FTN(c4f) (float cv[4])
	{ c4f (cv); }

void
FTN(c4i) (int cv[4])
	{ c4i (cv); }

void
FTN(c4s) (short cv[4])
	{ c4s (cv); }

void
FTN(onemap) (void)
	{ onemap (); }

void
FTN(multim) (void)
	{ multimap (); }

Boolean
FTN(getcmm) (void)
	{ return getcmmode (); }

void
FTN(setmap) (rShort mapnum)
	{ setmap (*mapnum); }

long
FTN(getmap) (void)
	{ return getmap (); }


/*******************************************
    matrix-related functions (matrix.c)
********************************************/
void
FTN(mmode) (rShort mode)
	{ mmode (*mode); }

long
FTN(getmmo) (void)
	{ return getmmode (); }

void
FTN(loadma) (Matrix m)
	{ loadmatrix (m); }

void
FTN(getmat) (Matrix m)
	{ getmatrix (m); }

void
FTN(multma) (Matrix m)
	{ multmatrix (m); }

void
FTN(pushma) (void)
	{ pushmatrix (); }

void
FTN(popmat) (void)
	{ popmatrix (); }

void
FTN(rot) (rFloat amount, rChar angle)
	{ rot (*amount, *angle); }

void
FTN(rotate) (rAngle amount, rChar angle)
	{ rotate (*amount, *angle); }

void
FTN(transl) (rCoord x, rCoord y, rCoord z)
	{ translate (*x, *y, *z); }

void
FTN(scale) (rFloat x, rFloat y, rFloat z)
	{ scale (*x, *y, *z); }

void
FTN(polarv) (rCoord dist, rAngle azim, rAngle inc, rAngle twist)
	{ polarview (*dist, *azim, *inc, *twist); }

void
FTN(clippl) (rLong index, rLong mode, float *params)
	{ clipplane (*index, *mode, params); }

void
FTN(ortho) (rCoord left, rCoord right, rCoord bottom, rCoord top, rCoord near_, rCoord far_)
	{ ortho (*left, *right, *bottom, *top, *near_, *far_); }

void
FTN(ortho2) (rCoord left, rCoord right, rCoord bottom, rCoord top)
	{ ortho2 (*left, *right, *bottom, *top); }

void
FTN(window) (rCoord left, rCoord right, rCoord bottom, rCoord top, rCoord near_, rCoord far_)
	{ window (*left, *right, *bottom, *top, *near_, *far_); }

void
FTN(perspe) (rAngle fovy, rFloat aspect, rCoord near_, rCoord far_)
	{ perspective (*fovy, *aspect, *near_, *far_); }

void
FTN(viewpo) (rScreencoord left, rScreencoord right, rScreencoord bottom, rScreencoord top)
	{ viewport (*left, *right, *bottom, *top); }

void
FTN(getvie) (Screencoord *left, Screencoord *right, Screencoord *bottom, Screencoord *top)
	{ getviewport (left, right, bottom, top); }

void
FTN(reshap) (void)
	{ reshapeviewport (); }

void
FTN(pushvi) (void)
	{ pushviewport (); }

void
FTN(popvie) (void)
	{ popviewport (); }

void
FTN(lookat) (rCoord vx, rCoord vy, rCoord vz, rCoord px, rCoord py, rCoord pz, rAngle twist)
	{ lookat (*vx, *vy, *vz, *px, *py, *pz, *twist); }

void
FTN(mapw) (rObject obj, rScreencoord sx, rScreencoord sy, Coord *wx1, Coord *wy1, Coord *wz1,
      Coord *wx2, Coord *wy2, Coord *wz2)
	{ mapw (*obj, *sx, *sy, wx1, wy1, wz1, wx2, wy2, wz2); }

void
FTN(mapw2) (rObject obj, rScreencoord sx, rScreencoord sy, Coord *wx, Coord *wy)
	{ mapw2 (*obj, *sx, *sy, wx, wy); }

void
FTN(screen) (void)
	{ screenspace (); }

void
FTN(scrmas) (rScreencoord left, rScreencoord right, rScreencoord bottom, rScreencoord top)
	{ scrmask (*left, *right, *bottom, *top); }

void
FTN(getscr) (Screencoord *left, Screencoord *right, Screencoord *bottom, Screencoord *top)
	{ getscrmask (left, right, bottom, top); }

void
FTN(pick) (short buffer[], rLong numnam)
	{ pick (buffer, *numnam); }

long
FTN(endpic) (short buffer[])
	{ return endpick (buffer); }

void
FTN(picksi) (rShort deltax, rShort deltay)
	{ picksize (*deltax, *deltay); }

void
FTN(gselec) (short buffer[], rLong numnam)
	{ gselect (buffer, *numnam); }

long
FTN(endsel) (short buffer[])
	{ return endselect (buffer); }

void
FTN(initna) (void)
	{ initnames (); }

void
FTN(loadna) (rShort name)
	{ loadname (*name); }

void
FTN(pushna) (rShort name)
	{ pushname (*name); }

void
FTN(popnam) (void)
	{ popname (); }


/*******************************************
    vertex/polygon-related functions (vertex.c)
********************************************/
void
FTN(v2f) (float vector[2])
	{ v2f (vector); }

void
FTN(v2d) (double vector[2])
	{ v2d (vector); }

void
FTN(v2i) (int vector[2])
	{ v2i (vector); }

void
FTN(v2s) (short vector[2])
	{ v2s (vector); }

void
FTN(v3f) (float vector[3])
	{ v3f (vector); }

void
FTN(v3d) (double vector[3])
	{ v3d (vector); }

void
FTN(v3i) (int vector[3])
	{ v3i (vector); }

void
FTN(v3s) (short vector[3])
	{ v3s (vector); }

void
FTN(v4f) (float vector[4])
	{ v4f (vector); }

void
FTN(v4d) (double vector[4])
	{ v4d (vector); }

void
FTN(v4i) (int vector[4])
	{ v4i (vector); }

void
FTN(v4s) (short vector[4])
	{ v4s (vector); }

void
FTN(arc) (rCoord x, rCoord y, rCoord radius, rAngle startangle, rAngle endangle)
	{ arc (*x, *y, *radius, *startangle, *endangle); }

void
FTN(arci) (rIcoord x, rIcoord y, rIcoord radius, rAngle startangle, rAngle endangle)
	{ arci (*x, *y, *radius, *startangle, *endangle); }

void
FTN(arcs) (rScoord x, rScoord y, rScoord radius, rAngle startangle, rAngle endangle)
	{ arcs (*x, *y, *radius, *startangle, *endangle); }

void
FTN(arcf) (rCoord x, rCoord y, rCoord radius, rAngle startangle, rAngle endangle)
	{ arcf (*x, *y, *radius, *startangle, *endangle); }

void
FTN(arcfi) (rIcoord x, rIcoord y, rIcoord radius, rAngle startangle, rAngle endangle)
	{ arcfi (*x, *y, *radius, *startangle, *endangle); }

void
FTN(arcfs) (rScoord x, rScoord y, rScoord radius, rAngle startangle, rAngle endangle)
	{ arcfs (*x, *y, *radius, *startangle, *endangle); }

void
FTN(circ) (rCoord x, rCoord y, rCoord radius)
	{ circ (*x, *y, *radius); }

void
FTN(circi) (rIcoord x, rIcoord y, rIcoord radius)
	{ circi (*x, *y, *radius); }

void
FTN(circs) (rScoord x, rScoord y, rScoord radius)
	{ circs (*x, *y, *radius); }

void
FTN(circf) (rCoord x, rCoord y, rCoord radius)
	{ circf (*x, *y, *radius); }

void
FTN(circfi) (rIcoord x, rIcoord y, rIcoord radius)
	{ circfi (*x, *y, *radius); }

void
FTN(circfs) (rScoord x, rScoord y, rScoord radius)
	{ circfs (*x, *y, *radius); }

void
FTN(bgntme) (void)
	{ bgntmesh (); }

void
FTN(endtme) (void)
	{ endtmesh (); }

void
FTN(swaptm) (void)
	{ swaptmesh (); }

void
FTN(bgnclo) (void)
	{ bgnclosedline (); }

void
FTN(endclo) (void)
	{ endclosedline (); }

void
FTN(bgncur) (void)
	{ bgncurve (); }

void
FTN(endcur) (void)
	{ endcurve (); }

void 
FTN(nurbsc) (rLong knotCount, const double *knotList, rLong offset, const double *ctlArray,
            rLong order, rLong type)
	{ nurbscurve (*knotCount, knotList, *offset, ctlArray, *order, *type); }

void
FTN(bgnsur) (void)
	{ bgnsurface (); }

void
FTN(endsur) (void)
	{ endsurface (); }

void
FTN(nurbss) (rLong scount, const double *sknot, rLong tcount, const double *tknot, rLong soffset,
              rLong toffset, const double *ctlArray, rLong sorder, rLong torder, rLong type)
	{ nurbssurface (*scount, sknot, *tcount, tknot, *soffset, *toffset, ctlArray, *sorder, *torder, *type); }

void
FTN(bgntri) (void)
	{ bgntrim (); }

void
FTN(endtri) (void)
	{ endtrim (); }

void
FTN(pwlcur) (rLong n, double *dataArray, rLong byteSize, rLong type)
	{ pwlcurve (*n, dataArray, *byteSize, *type); }

void
FTN(setnur) (rLong property, rFloat value)
	{ setnurbsproperty (*property, *value); }

void
FTN(getnur) (rLong property, float *value)
	{ getnurbsproperty (*property, value); }

void
FTN(bgnlin) (void)
	{ bgnline (); }

void
FTN(endlin) (void)
	{ endline (); }

void
FTN(bgnpoi) (void)
	{ bgnpoint (); }

void
FTN(endpoi) (void)
	{ endpoint (); }

void
FTN(bgnpol) (void)
	{ bgnpolygon (); }

void
FTN(endpol) (void)
	{ endpolygon (); }

void
FTN(bgnqst) (void)
	{ bgnqstrip (); }

void
FTN(endqst) (void)
	{ endqstrip (); }

void
FTN(curvei) (rShort niter)
	{ curveit (*niter); }

void
FTN(crv) (Coord points[4][3])
	{ crv (points); }

void
FTN(crvn) (rLong n, Coord points[][3])
	{ crvn (*n, points); }

void
FTN(rcrv) (Coord points[4][4])
	{ rcrv (points); }

void
FTN(rcrvn) (rLong n, Coord points[][4])
	{ rcrvn (*n, points); }

void
FTN(curvep) (rShort n)
	{ curveprecision (*n); }

void
FTN(curveb) (rShort id)
	{ curvebasis (*id); }

void
FTN(patch) (Matrix geomx, Matrix geomy, Matrix geomz)
	{ patch (geomx, geomy, geomz); }

void
FTN(rpatch) (Matrix geomx, Matrix geomy, Matrix geomz, Matrix geomw)
	{ rpatch (geomx, geomy, geomz, geomw); }

void
FTN(patchc) (rShort nu, rShort nv)
	{ patchcurves (*nu, *nv); }

void
FTN(patchp) (rShort nu, rShort nv)
	{ patchprecision (*nu, *nv); }

void
FTN(patchb) (rShort uid, rShort vid)
	{ patchbasis (*uid, *vid); }

void
FTN(defbas) (rShort id, Matrix mat)
	{ defbasis (*id, mat); }

void
FTN(nmode) (rLong mode)
	{ nmode (*mode); }

void
FTN(n3f) (float *vector)
	{ n3f (vector); }

void
FTN(normal) (Coord *narray)
	{ normal (narray); }

void
FTN(linesm) (unsigned rLong mode)
	{ linesmooth (*mode); }

void
FTN(smooth) (rLong mode)
	{ smoothline (*mode); }

void
FTN(linewi) (rShort width)
	{ linewidth (*width); }

void
FTN(linewf) (rFloat width)
	{ linewidthf (*width); }

long
FTN(getlwi) (void)
	{ return getlwidth (); }

void
FTN(deflin) (rShort n, rLinestyle ls)
	{ deflinestyle (*n, *ls); }

void
FTN(setlin) (rShort n)
	{ setlinestyle (*n); }

long
FTN(getlst) (void)
	{ return getlstyle (); }

void
FTN(lsrepe) (rLong factor)
	{ lsrepeat (*factor); }

long
FTN(getlsr) (void)
	{ return getlsrepeat (); }

void
FTN(defpat) (rShort n, rShort size, unsigned short mask[])
	{ defpattern (*n, *size, mask); }

void
FTN(setpat) (rShort index)
	{ setpattern (*index); }

long
FTN(getpat) (void)
	{ return getpattern (); }


/***************************************
    light-related functions (light.c)
****************************************/
void
FTN(lmdef) (rShort deftype, rShort index, rShort np, float *props)
	{ lmdef (*deftype, *index, *np, props); }

void
FTN(lmbind) (rShort target, rShort index)
	{ lmbind (*target, *index); }

void
FTN(lmcolo) (rLong mode)
	{ lmcolor (*mode); }


/***************************************
    device-related functions (device.c)
****************************************/
Boolean
FTN(getbut) (rDevice dev)
	{ return getbutton (*dev); }

long
FTN(getval) (rDevice dev)
	{ return getvaluator (*dev); }

void
FTN(setval) (rDevice v, rShort init, rShort vmin, rShort vmax)
	{ setvaluator (*v, *init, *vmin, *vmax); }

void
FTN(noise) (rDevice v, rShort delta)
	{ noise (*v, *delta); }

void
FTN(getdev) (rLong n, Device devs[], short vals[])
	{ getdev (*n, devs, vals); }

void
FTN(qdevic) (rDevice dev)
	{ qdevice (*dev); }

void
FTN(unqdev) (rDevice dev)
	{ unqdevice (*dev); }

Boolean
FTN(isqueu) (rDevice dev)
	{ return isqueued (*dev); }

void
FTN(qenter) (rDevice dev, rShort val)
	{ qenter (*dev, *val); }

long
FTN(qtest) (void)
	{ return qtest (); }

void
FTN(qreset) (void)
	{ qreset (); }

long
FTN(blkqre) (short *data, rShort n)
	{ return blkqread (data, *n); }

long
FTN(qread) (short *data)
	{ return qread (data); }

void
FTN(tie) (rDevice b, rDevice v1, rDevice v2)
	{ tie (*b, *v1, *v2); }

void
FTN(attach) (rDevice vx, rDevice vy)
	{ attachcursor (*vx, *vy); }

long
FTN(qgetfd) (void)
	{ return qgetfd (); }


/***************************************
    graphics position-related functions (grpos.c)
****************************************/
void
FTN(move) (rCoord x, rCoord y, rCoord z)
	{ move (*x, *y, *z); }

void
FTN(movei) (rIcoord x, rIcoord y, rIcoord z)
	{ movei (*x, *y, *z); }

void
FTN(moves) (rScoord x, rScoord y, rScoord z)
	{ moves (*x, *y, *z); }

void
FTN(move2) (rCoord x, rCoord y)
	{ move2 (*x, *y); }

void
FTN(move2i) (rIcoord x, rIcoord y)
	{ move2i (*x, *y); }

void
FTN(move2s) (rScoord x, rScoord y)
	{ move2s (*x, *y); }

void
FTN(getgpo) (Coord *fx, Coord *fy, Coord *fz, Coord *fw)
	{ getgpos (fx, fy, fz, fw); }

void
FTN(rmv) (rCoord dx, rCoord dy, rCoord dz)
	{ rmv (*dx, *dy, *dz); }

void
FTN(rmvi) (rIcoord dx, rIcoord dy, rIcoord dz)
	{ rmvi (*dx, *dy, *dz); }

void
FTN(rmvs) (rCoord dx, rCoord dy, rCoord dz)
	{ rmvs (*dx, *dy, *dz); }

void
FTN(rmv2) (rCoord dx, rCoord dy)
	{ rmv2 (*dx, *dy); }

void
FTN(rmv2i) (rIcoord dx, rIcoord dy)
	{ rmv2i (*dx, *dy); }

void
FTN(rmv2s) (rScoord dx, rScoord dy)
	{ rmv2s (*dx, *dy); }

void
FTN(rpmv) (rCoord dx, rCoord dy, rCoord dz)
	{ rpmv (*dx, *dy, *dz); }

void
FTN(rpmvi) (rIcoord dx, rIcoord dy, rIcoord dz)
	{ rpmvi (*dx, *dy, *dz); }

void
FTN(rpmvs) (rCoord dx, rCoord dy, rCoord dz)
	{ rpmvs (*dx, *dy, *dz); }

void
FTN(rpmv2) (rCoord dx, rCoord dy)
	{ rpmv2 (*dx, *dy); }

void
FTN(rpmv2i) (rIcoord dx, rIcoord dy)
	{ rpmv2i (*dx, *dy); }

void
FTN(rpmv2s) (rScoord dx, rScoord dy)
	{ rpmv2s (*dx, *dy); }

void
FTN(draw) (rCoord x, rCoord y, rCoord z)
	{ draw (*x, *y, *z); }

void
FTN(drawi) (rIcoord x, rIcoord y, rIcoord z)
	{ drawi (*x, *y, *z); }

void
FTN(draws) (rScoord x, rScoord y, rScoord z)
	{ draws (*x, *y, *z); }

void
FTN(draw2) (rCoord x, rCoord y)
	{ draw2 (*x, *y); }

void
FTN(draw2i) (rIcoord x, rIcoord y)
	{ draw2i (*x, *y); }

void
FTN(draw2s) (rScoord x, rScoord y)
	{ draw2s (*x, *y); }

void
FTN(rdr) (rCoord dx, rCoord dy, rCoord dz)
	{ rdr (*dx, *dy, *dz); }

void
FTN(rdri) (rIcoord dx, rIcoord dy, rIcoord dz)
	{ rdri (*dx, *dy, *dz); }

void
FTN(rdrs) (rScoord dx, rScoord dy, rScoord dz)
	{ rdrs (*dx, *dy, *dz); }

void
FTN(rdr2) (rCoord dx, rCoord dy)
	{ rdr2 (*dx, *dy); }

void
FTN(rdr2i) (rIcoord dx, rIcoord dy)
	{ rdr2i (*dx, *dy); }

void
FTN(rdr2s) (rScoord dx, rScoord dy)
	{ rdr2s (*dx, *dy); }

void
FTN(rpdr) (rCoord dx, rCoord dy, rCoord dz)
	{ rpdr (*dx, *dy, *dz); }

void
FTN(rpdri) (rIcoord dx, rIcoord dy, rIcoord dz)
	{ rpdri (*dx, *dy, *dz); }

void
FTN(rpdrs) (rScoord dx, rScoord dy, rScoord dz)
	{ rpdrs (*dx, *dy, *dz); }

void
FTN(rpdr2) (rCoord dx, rCoord dy)
	{ rpdr2 (*dx, *dy); }

void
FTN(rpdr2i) (rIcoord dx, rIcoord dy)
	{ rpdr2i (*dx, *dy); }

void
FTN(rpdr2s) (rScoord dx, rScoord dy)
	{ rpdr2s (*dx, *dy); }

void
FTN(polf) (rLong n, const Coord parray[][3])
	{ polf (*n, parray); }

void
FTN(polfi) (rLong n, const Icoord parray[][3])
	{ polfi (*n, parray); }

void
FTN(polfs) (rLong n, const Scoord parray[][3])
	{ polfs (*n, parray); }

void
FTN(polf2) (rLong n, const Coord parray[][2])
	{ polf2 (*n, parray); }

void
FTN(polf2i) (rLong n, const Icoord parray[][2])
	{ polf2i (*n, parray); }

void
FTN(polf2s) (rLong n, const Scoord parray[][2])
	{ polf2s (*n, parray); }

void
FTN(poly) (rLong n, const Coord parray[][3])
	{ poly (*n, parray); }

void
FTN(polyi) (rLong n, const Icoord parray[][3])
	{ polyi (*n, parray); }

void
FTN(polys) (rLong n, const Scoord parray[][3])
	{ polys (*n, parray); }

void
FTN(poly2) (rLong n, const Coord parray[][2])
	{ poly2 (*n, parray); }

void
FTN(poly2i) (rLong n, const Icoord parray[][2])
	{ poly2i (*n, parray); }

void
FTN(poly2s) (rLong n, const Scoord parray[][2])
	{ poly2s (*n, parray); }

void
FTN(polymo) (rLong mode)
	{ polymode (*mode); }

void
FTN(polysm) (rLong mode)
	{ polysmooth (*mode); }

void
FTN(pmv) (rCoord x, rCoord y, rCoord z)
	{ pmv (*x, *y, *z); }

void
FTN(pmvi) (rIcoord x, rIcoord y, rIcoord z)
	{ pmvi (*x, *y, *z); }

void
FTN(pmvs) (rScoord x, rScoord y, rScoord z)
	{ pmvs (*x, *y, *z); }

void
FTN(pmv2) (rCoord x, rCoord y)
	{ pmv2 (*x, *y); }

void
FTN(pmv2i) (rIcoord x, rIcoord y)
	{ pmv2i (*x, *y); }

void
FTN(pmv2s) (rScoord x, rScoord y)
	{ pmv2s (*x, *y); }

void
FTN(pdr) (rCoord x, rCoord y, rCoord z)
	{ pdr (*x, *y, *z); }

void
FTN(pdri) (rIcoord x, rIcoord y, rIcoord z)
	{ pdri (*x, *y, *z); }

void
FTN(pdrs) (rScoord x, rScoord y, rScoord z)
	{ pdrs (*x, *y, *z); }

void
FTN(pdr2) (rCoord x, rCoord y)
	{ pdr2 (*x, *y); }

void
FTN(pdr2i) (rCoord x, rCoord y)
	{ pdr2i (*x, *y); }

void
FTN(pdr2s) (rCoord x, rCoord y)
	{ pdr2s (*x, *y); }

void
FTN(pclos) (void)
	{ pclos (); }

void
FTN(spclos) (void)
	{ spclos (); }

void
FTN(pnt) (rCoord x, rCoord y, rCoord z)
	{ pnt (*x, *y, *z); }

void
FTN(pnti) (rIcoord x, rIcoord y, rIcoord z)
	{ pnti (*x, *y, *z); }

void
FTN(pnts) (rScoord x, rScoord y, rScoord z)
	{ pnts (*x, *y, *z); }

void
FTN(pnt2) (rCoord x, rCoord y)
	{ pnt2 (*x, *y); }

void
FTN(pnt2i) (rIcoord x, rIcoord y)
	{ pnt2i (*x, *y); }

void
FTN(pnt2s) (rScoord x, rScoord y)
	{ pnt2s (*x, *y); }

void
FTN(pntsiz) (rShort n)
	{ pntsize (*n); }

void
FTN(pntszf) (rFloat n)
	{ pntsizef (*n); }

void
FTN(pntsmo) (unsigned rLong mode)
        { pntsmooth (*mode); }

void
FTN(rect) (rCoord a, rCoord b, rCoord c, rCoord d)
	{ rect (*a, *b, *c, *d); }

void
FTN(recti) (rIcoord a, rIcoord b, rIcoord c, rIcoord d)
	{ recti (*a, *b, *c, *d); }

void
FTN(rects) (rScoord a, rScoord b, rScoord c, rScoord d)
	{ rects (*a, *b, *c, *d); }

void
FTN(rectf) (rCoord a, rCoord b, rCoord c, rCoord d)
	{ rectf (*a, *b, *c, *d); }

void
FTN(rectfi) (rIcoord a, rIcoord b, rIcoord c, rIcoord d)
	{ rectfi (*a, *b, *c, *d); }

void
FTN(rectfs) (rScoord a, rScoord b, rScoord c, rScoord d)
	{ rectfs (*a, *b, *c, *d); }

void
FTN(sbox) (rCoord a, rCoord b, rCoord c, rCoord d)
	{ sbox (*a, *b, *c, *d); }

void
FTN(sboxi) (rIcoord a, rIcoord b, rIcoord c, rIcoord d)
	{ sboxi (*a, *b, *c, *d); }

void
FTN(sboxs) (rScoord a, rScoord b, rScoord c, rScoord d)
	{ sboxs (*a, *b, *c, *d); }

void
FTN(sboxf) (rCoord a, rCoord b, rCoord c, rCoord d)
	{ sboxf (*a, *b, *c, *d); }

void
FTN(sboxfi) (rIcoord a, rIcoord b, rIcoord c, rIcoord d)
	{ sboxfi (*a, *b, *c, *d); }

void
FTN(sboxfs) (rScoord a, rScoord b, rScoord c, rScoord d)
	{ sboxfs (*a, *b, *c, *d); }

void
FTN(splf) (rLong n, Coord parray[][3], Colorindex iarray[])
	{ splf (*n, parray, iarray); }

void
FTN(splfi) (rLong n, Icoord parray[][3], Colorindex iarray[])
	{ splfi (*n, parray, iarray); }

void
FTN(splfs) (rLong n, Scoord parray[][3], Colorindex iarray[])
	{ splfs (*n, parray, iarray); }

void
FTN(splf2) (rLong n, Coord parray[][2], Colorindex iarray[])
	{ splf2 (*n, parray, iarray); }

void
FTN(splf2i) (rLong n, Icoord parray[][2], Colorindex iarray[])
	{ splf2i (*n, parray, iarray); }

void
FTN(splf2s) (rLong n, Scoord parray[][2], Colorindex iarray[])
	{ splf2s (*n, parray, iarray); }

void
FTN(displa) (rFloat scalefactor)
	{ displacepolygon (*scalefactor); }


/***************************************
    object-related functions (object.c)
****************************************/
Object
FTN(genobj) (void)
	{ return genobj (); }

void
FTN(makeob) (rObject obj)
	{ makeobj (*obj); }

void
FTN(closeo) (void)
	{ closeobj (); }

void
FTN(callob) (rObject obj)
	{ callobj (*obj); }

void
FTN(delobj) (rObject obj)
	{ delobj (*obj); }

Boolean
FTN(isobj) (rObject obj)
	{ return isobj (*obj); }

void
FTN(editob) (rObject obj)
	{ editobj (*obj); }

Object
FTN(getope) (void)
	{ return getopenobj (); }

Tag
FTN(gentag) (void)
	{ return gentag (); }

void
FTN(maketa) (rTag tag)
	{ maketag (*tag); }

void
FTN(deltag) (rTag tag)
	{ deltag (*tag); }

Boolean
FTN(istag) (rTag tag)
	{ return istag (*tag); }

void
FTN(newtag) (rTag newtg, rTag oldtg, rOffset offset)
	{ newtag (*newtg, *oldtg, *offset); }

void
FTN(objins) (rTag t)
	{ objinsert (*t); }

void
FTN(objdel) (rTag tag1, rTag tag2)
	{ objdelete (*tag1, *tag2); }

void
FTN(objrep) (rTag t)
	{ objreplace (*t); }

void
FTN(chunks) (rLong chunk)
	{ chunksize (*chunk); }

void
FTN(compac) (rObject obj)
	{ compactify (*obj); }

#if 0
void
FTN(callfu) (void (*fctn)(), long nargs, ...)
	{ callfunc ((fctn)(), nargs,...); }
#endif


/***************************************
    texture-related functions (texture.c)
****************************************/
void
FTN(tevdef) (rLong index, rLong np, float *props)
	{ tevdef (*index, *np, props); }

void
FTN(tevbin) (rLong target, rLong index)
	{ tevbind (*target, *index); }

void
FTN(texdf2) (rLong index, rLong nc, rLong width, rLong height, unsigned long *image, rLong np, float *props)
	{ texdef2d (*index, *nc, *width, *height, image, *np, props); }

void
FTN(texdf3) (rLong index, rLong nc, rLong width, rLong height, rLong depth, unsigned long *image, rLong np, float *props)
	{ texdef3d (*index, *nc, *width, *height, *depth, image, *np, props); }

void
FTN(texbin) (rLong target, rLong index)
	{ texbind (*target, *index); }

void
FTN(texgen) (rLong coord, rLong mode, float *params)
	{ texgen (*coord, *mode, params); }

void
FTN(subtex) (rLong target, rLong id, rFloat s0, rFloat s1, rFloat t0, rFloat t1, rLong numwords, unsigned long *texture, unsigned rLong flags)
	{ subtexload (*target, *id, *s0, *s1, *t0, *t1, *numwords, texture, *flags); }

void
FTN(t2s) (short vector[2])
	{ t2s (vector); }

void
FTN(t2i) (long vector[2])
	{ t2i (vector); }

void
FTN(t2f) (float vector[2])
	{ t2f (vector); }

void
FTN(t2d) (double vector[2])
	{ t2d (vector); }

void
FTN(t3s) (short vector[3])
	{ t3s (vector); }

void
FTN(t3i) (long vector[3])
	{ t3i (vector); }

void
FTN(t3f) (float vector[3])
	{ t3f (vector); }

void
FTN(t3d) (double vector[3])
	{ t3d (vector); }

void
FTN(t4s) (short vector[4])
	{ t4s (vector); }

void
FTN(t4i) (long vector[4])
	{ t4i (vector); }

void
FTN(t4f) (float vector[4])
	{ t4f (vector); }

void
FTN(t4d) (double vector[4])
	{ t4d (vector); }


/***************************************
    text/character-related functions (text.c)
****************************************/
void
FTN(getcpo) (short *ix, short *iy)
	{ getcpos (ix, iy); }

void
FTN(cmov) (rCoord x, rCoord y, rCoord z)
	{ cmov (*x, *y, *z); }

void
FTN(cmovi) (rIcoord x, rIcoord y, rIcoord z)
	{ cmovi (*x, *y, *z); }

void
FTN(cmovs) (rScoord x, rScoord y, rScoord z)
	{ cmovs (*x, *y, *z); }

void
FTN(cmov2) (rCoord x, rCoord y)
	{ cmov2 (*x, *y); }

void
FTN(cmov2i) (rIcoord x, rIcoord y)
	{ cmov2i (*x, *y); }

void
FTN(cmov2s) (rScoord x, rScoord y)
	{ cmov2s (*x, *y); }

void
FTN(charst) (String str, rInt len)
	{ Cstring (s, str, *len); charstr (s); }

void
FTN(lchstr) (rLong type, void *str, rInt len)
	{ Cstring (s, str, *len); lcharstr (*type, s); }

long
FTN(strwid) (String str, rInt len)
	{ Cstring (s, str, *len); return strwidth (s); }

long
FTN(lstwid) (rLong type, String str, rInt len)
	{ Cstring (s, str, *len); return lstrwidth (*type, s); }

long
FTN(gethei) (void)
	{ return getheight (); }

void
FTN(font) (rShort fntnum)
	{ font (*fntnum); }

long
FTN(getfon) (void)
	{ return getfont (); }

long
FTN(getdes) (void)
	{ return getdescender (); }

void
FTN(defras) (rShort n, rShort ht, rShort nc, Fontchar chars[], rShort nr, unsigned short raster[])
	{ defrasterfont (*n, *ht, *nc, chars, *nr, raster); }

void
FTN(deflfo) (rShort n, rLong nc, Lfontchar chars[], rLong nr, unsigned short raster[])
	{ deflfont (*n, *nc, chars, *nr, raster); }

void
FTN(pixmod) (rLong mode, rLong value)
	{ pixmode(*mode, *value); }

void
FTN(pixmof) (rLong mode, rFloat value)
	{ pixmodef(*mode, *value); }

void
FTN(rectzo) (rFloat xfactor, rFloat yfactor)
	{ rectzoom(*xfactor, *yfactor); }

void 
FTN(writep) (rShort n, Colorindex colors[])
	{ writepixels(*n, colors); }

void
FTN(rectwr) (rScreencoord x1, rScreencoord y1, rScreencoord x2, rScreencoord y2, Colorindex parray[])
	{ rectwrite(*x1, *y1, *x2, *y2, parray); }

long
FTN(readrg) (rShort n, RGBvalue r[], RGBvalue g[], RGBvalue b[])
	{ return readRGB(*n, r, g, b); }

void
FTN(writer) (rShort n, RGBvalue r[], RGBvalue g[], RGBvalue b[])
	{ writeRGB(*n, r, g, b); }

long
FTN(lrectr) (rScreencoord x1, rScreencoord y1, rScreencoord x2, rScreencoord y2, unsigned long parray[])
	{ return lrectread(*x1, *y1, *x2, *y2, parray); }

void
FTN(lrectw) (rScreencoord x1, rScreencoord y1, rScreencoord x2, rScreencoord y2, unsigned long parray[])
	{ lrectwrite(*x1, *y1, *x2, *y2, parray); }

void
FTN(rectco) (rScreencoord x1, rScreencoord y1, rScreencoord x2, rScreencoord y2, rScreencoord newx, rScreencoord newy)
	{ rectcopy(*x1, *y1, *x2, *y2, *newx, *newy); }

void
FTN(minmax) (unsigned rLong op, unsigned rLong val)
	{ minmax (*op, *val); }

void
FTN(getmin) (float *minmax)
	{ getminmax (minmax); }

void
FTN(hgram) (unsigned rLong op, unsigned rLong val)
	{ hgram (*op, *val); }

void
FTN(gethgr) (unsigned long hist[16384])
	{ gethgram (hist); }


/***************************************
    menu-related functions (menu.c)
****************************************/
#if 0   /* not available in f77 */
long
FTN(defpup) (String str, ...)
	{ defpup (str,...); }
#endif

long
FTN(newpup) (void)
	{ return newpup (); }

void
FTN(addtop) (rLong pup, String str, rInt len, rInt arg)
	{ Cstring (s, str, *len); addtopup (*pup, s, *arg); }

long
FTN(dopup) (rLong pup)
	{ return dopup (*pup); }

void
FTN(freepu) (rLong pup)
	{ freepup (*pup); }

void
FTN(setpup) (rLong pup, rLong entry, unsigned rLong mode)
	{ setpup (*pup, *entry, *mode); }

void
FTN(pupmod) (void)
	{ pupmode (); }

void
FTN(endpup) (void)
	{ endpupmode (); }

void
FTN(pupcol) (rLong clr)
	{ pupcolor (*clr); }


/***************************************
    misc functions
****************************************/
void
FTN(pushat) (void)
	{ pushattributes (); }

void
FTN(popatt) (void)
	{ popattributes (); }

long
FTN(getgde) (rLong inquiry)
	{ return getgdesc (*inquiry); }

void
FTN(greset) (void)
	{ greset (); }

void
FTN(gbegin) (void)
	{ gbegin (); }

void
FTN(ginit) (void)
	{ ginit (); }

void
FTN(gexit) (void)
	{ gexit (); }

void
FTN(finish) (void)
	{ finish (); }

void
FTN(gflush) (void)
	{ gflush (); }

long
FTN(gversi) (String v)
	{ return gversion (v); }

Boolean
FTN(ismex) (void)
	{ return ismex (); }


/***************************************
    unimplemented functions
****************************************/
/* some of these should be implemented later */
void
FTN(glcomp) (rLong mode, rLong value)
	{ glcompat (*mode, *value); }

void
FTN(convol) (rLong op, rLong border, rLong xksize, rLong yksize, float *kernel, rFloat bias)
	{ convolve (*op, *border, *xksize, *yksize, kernel, *bias); }

void
FTN(pixelm) (rInt map, rInt size, unsigned short *values)
	{ pixelmap (*map, *size, values); }

void
FTN(pixelt) (rInt mode, rFloat value)
	{ pixeltransfer (*mode, *value); }

void
FTN(readco) (rLong mode)
	{ readcomponent (*mode); }

void
FTN(readso) (rLong src)
	{ readsource (*src); }

long
FTN(ilbuff) (unsigned rLong buffers)
        { return ilbuffer (*buffers); }

void
FTN(ildraw) (unsigned rLong buffer)
        { ildraw (*buffer); }

long 
FTN(readpi) (rShort n, Colorindex colors[])
	{ return readpixels(*n, colors); }

long
FTN(rectre) (rScreencoord x1, rScreencoord y1, rScreencoord x2, rScreencoord y2, Colorindex parray[])
	{ return rectread(*x1, *y1, *x2, *y2, parray); }

long
FTN(readdi) (rScreencoord x1, rScreencoord y1, rScreencoord x2, rScreencoord y2, unsigned long parray[], unsigned rLong hints)
	{ return readdisplay(*x1, *y1, *x2, *y2, parray, *hints); }

void
FTN(fbsubt) (rLong x, rLong y, rLong target, rLong id, rFloat s0, rFloat s1, rFloat t0, rFloat t1, unsigned rLong flags)
	{ fbsubtexload (*x, *y, *target, *id, *s0, *s1, *t0, *t1, *flags); }

long
FTN(istexl) (rLong target, rLong id)
	{ return istexloaded (*target, *id); }

void
FTN(feedba) (float buffer[], rLong size)
	{ feedback (buffer, *size); }

void
FTN(endfee) (float buffer [])
	{ endfeedback (buffer ); }

void
FTN(passth) (rShort token)
	{ passthrough (*token); }

void
FTN(curson) (void)
	{ curson (); }

void
FTN(cursof) (void)
	{ cursoff (); }

void
FTN(cursty) (rLong type)
	{ curstype (*type); }

void
FTN(defcur) (rShort n, unsigned short *curs)
	{ defcursor (*n, curs); }

void
FTN(setcur) (rShort n, rColorindex color, rColorindex wtm)
	{ setcursor (*n, *color, *wtm); }

void
FTN(curori) (rShort n, rShort xo, rShort yo)
	{ curorigin (*n, *xo, *yo); }

void
FTN(getcur) (short *n, Colorindex *color, Colorindex *wtm, Boolean *vis)
	{ getcursor (n, color, wtm, vis); }

void
FTN(rgbcur) (rShort index, rShort red, rShort green, rShort blue, rShort redm, rShort greenm, rShort bluem)
	{ RGBcursor (*index, *red, *green, *blue, *redm, *greenm, *bluem); }

void
FTN(grgbcu) (short *index, short *red, short *green, short *blue, short *redm, short *greenm, short *bluem, Boolean *vis)
	{ gRGBcursor (index, red, green, blue, redm, greenm, bluem, vis); }

void
FTN(fudge) (rLong xfudge, rLong yfudge)
	{ fudge (*xfudge, *yfudge); }


/***************************************
    meaningless/unimplementable functions
****************************************/
void
FTN(foregr) (void)
	{ foreground (); }

long
FTN(getwsc) (void)
	{ return getwscrn (); }

void
FTN(scrnse) (rLong gsnr)
	{ scrnselect (*gsnr); }

void
FTN(scrnat) (rLong gsnr)
	{ scrnattach (*gsnr); }

void
FTN(scrsub) (rLong mode, float param[])
	{ scrsubdivide (*mode, param); }

long
FTN(dglope) (String srvname, rInt len, rLong type)
	{ Cstring (s, srvname, *len); return dglopen (s, *type); }

void
FTN(dglclo) (rLong srvid)
	{ dglclose (*srvid); }

void
FTN(multis) (rBoolean bool)
        { multisample (*bool); }

Boolean
FTN(getmul) (void)
        { return getmultisample (); }

void
FTN(mssamp) (rLong mode)
	{ mssample (*mode); }

void
FTN(msalph) (rLong mode)
	{ msalpha (*mode); }

void
FTN(msmask) (rFloat mask, rBoolean inverse)
	{ msmask (*mask, *inverse); }

void
FTN(mspatt) (rLong pattern)
	{ mspattern (*pattern); }

void
FTN(mssize) (rLong samples, rLong zsize, rLong ssize)
	{ mssize (*samples, *zsize, *ssize); }

void
FTN(overla) (rLong planes)
	{ overlay (*planes); }

void
FTN(underl) (rLong planes)
	{ underlay (*planes); }

void
FTN(iconti) (String name, rInt len)
	{ Cstring (s, name, *len); icontitle (s); }

void
FTN(iconsi) (rLong x, rLong y)
	{ iconsize (*x, *y); }

void
FTN(qcontr) (rLong cmd, rLong icnt, short idata[], rLong ocnt, short odata[])
	{ qcontrol (*cmd, *icnt, idata, *ocnt, odata); }

void
FTN(blanks) (rBoolean b)
	{ blankscreen (*b); }

void
FTN(blankt) (rLong count)
	{ blanktime (*count); }

void
FTN(gammar) (short r[256], short g[256], short b[256])
	{ gammaramp (r, g, b); }

void
FTN(clkon) (void)
	{ clkon (); }

void
FTN(clkoff) (void)
	{ clkoff (); }

void
FTN(lampon) (rByte lamps)
	{ lampon (*lamps); }

void
FTN(lampof) (rByte lamps)
	{ lampoff (*lamps); }

void
FTN(ringbe) (void)
	{ ringbell (); }

void
FTN(setbel) (rByte durat)
	{ setbell (*durat); }

void
FTN(dbtext) (String str)
	{ dbtext (str); }

void
FTN(setdbl) (unsigned rLong mask)
	{ setdblights (*mask); }

void
FTN(setvid) (rLong reg, rLong val)
	{ setvideo (*reg, *val); }

long
FTN(getvid) (rLong reg)
	{ return getvideo (*reg); }

void
FTN(videoc) (rLong cmd)
	{ videocmd (*cmd); }

void
FTN(setmon) (rShort mtype)
	{ setmonitor (*mtype); }

long
FTN(getmon) (void)
	{ return getmonitor (); }

long
FTN(getoth) (void)
	{ return getothermonitor (); }

void
FTN(zdraw) (rBoolean enable)
	{ zdraw (*enable); }

void
FTN(zsourc) (rLong src)
	{ zsource (*src); }

void
FTN(blink) (rShort rate, rColorindex i, rShort red, rShort green, rShort blue)
	{ blink (*rate, *i, *red, *green, *blue); }

void
FTN(cyclem) (rShort duration, rShort map, rShort nxtmap)
	{ cyclemap (*duration, *map, *nxtmap); }

void
FTN(scrbox) (rLong arg)
	{ scrbox (*arg); }

void
FTN(getsb) (long *left, long *right, long *bottom, long *top)
	{ getscrbox (left, right, bottom, top); }

void
FTN(bbox2) (rScreencoord xmin, rScreencoord ymin, rCoord x1, rCoord y1, rCoord x2, rCoord y2)
	{ bbox2 (*xmin, *ymin, *x1, *y1, *x2, *y2); }

void
FTN(bbox2i) (rScreencoord xmin, rScreencoord ymin, rIcoord x1, rIcoord y1, rIcoord x2, rIcoord y2)
	{ bbox2i (*xmin, *ymin, *x1, *y1, *x2, *y2); }

void
FTN(bbox2s) (rScreencoord xmin, rScreencoord ymin, rScoord x1, rScoord y1, rScoord x2, rScoord y2)
	{ bbox2s (*xmin, *ymin, *x1, *y1, *x2, *y2); }

void
FTN(tlutde) (rLong index, rLong nc, rLong len, unsigned long *table, rLong np, float *props)
	{ tlutdef (*index, *nc, *len, table, *np, props); }

void
FTN(tlutbi) (rLong target, rLong index)
	{ tlutbind (*target, *index); }

void
FTN(lsback) (rBoolean b)
	{ lsbackup (*b); }

Boolean
FTN(getlsb) (void)
	{ return getlsbackup (); }

void
FTN(resetl) (rBoolean b)
	{ resetls (*b); }

Boolean
FTN(getres) (void)
	{ return getresetls (); }

void
FTN(concav) (rBoolean enable)
	{ concave (*enable); }

void
FTN(subpix) (rBoolean enable)
	{ subpixel (*enable); }

void
FTN(textin) (void)
	{ textinit (); }

void
FTN(textpo) (rScreencoord left, rScreencoord right, rScreencoord bottom, rScreencoord top)
	{ textport (*left, *right, *bottom, *top); }

void
FTN(textco) (rColorindex tcolor)
	{ textcolor (*tcolor); }

void
FTN(pageco) (rColorindex pcolor)
	{ pagecolor (*pcolor); }

void
FTN(tpon) (void)
	{ tpon (); }

void
FTN(tpoff) (void)
	{ tpoff (); }

void
FTN(xfpt) (rCoord x, rCoord y, rCoord z)
	{ xfpt (*x, *y, *z); }

void
FTN(xfpti) (rIcoord x, rIcoord y, rIcoord z)
	{ xfpti (*x, *y, *z); }

void
FTN(xfpts) (rScoord x, rScoord y, rScoord z)
	{ xfpts (*x, *y, *z); }

void
FTN(xfpt2) (rCoord x, rCoord y)
	{ xfpt2 (*x, *y); }

void
FTN(xfpt2i) (rIcoord x, rIcoord y)
	{ xfpt2i (*x, *y); }

void
FTN(xfpt2s) (rScoord x, rScoord y)
	{ xfpt2s (*x, *y); }

void
FTN(xfpt4) (rCoord x, rCoord y, rCoord z, rCoord w)
	{ xfpt4 (*x, *y, *z, *w); }

void
FTN(xfpt4i) (rIcoord x, rIcoord y, rIcoord z, rIcoord w)
	{ xfpt4i (*x, *y, *z, *w); }

void
FTN(xfpt4s) (rScoord x, rScoord y, rScoord z, rScoord w)
	{ xfpt4s (*x, *y, *z, *w); }
