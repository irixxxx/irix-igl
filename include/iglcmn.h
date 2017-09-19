/*
 * iglcmn.h
 *
 * IGL internal interface definitions
 */

#ifndef IGLCMN_H
#define IGLCMN_H

#define IGL_VERSION      "0.4.2"

#define PLATFORM_X11     1

#if PLATFORM_X11
#   include <X11/X.h>
#   include <X11/Xlib.h>
#   include <X11/Xatom.h>
#   include <X11/cursorfont.h>
#   include <X11/keysym.h>
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glx.h>
#   include <sys/time.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <limits.h>

#include "iglobj.h"

#ifdef __cplusplus
extern "C" {
#endif

/* IGL table sizes, should be powers of 2 */
#define IGL_MAXWINDOWS          256         /* max open windows per process */
#define IGL_MAXCOLORS           4096        /* max colors available for color[f]() palette */
#define IGL_MAXMULTIMAPCOLORS   256         /* max colors available in each multimap palette (16 palettes) */

#define IGL_MAXCURSORS          8           /* max cursors available */
#define IGL_MAXLINESTYLES       64          /* max linestyles available */
#define IGL_MAXPATTERNS         64          /* max polygon patterns available */
#define IGL_MAXMATRICES         16          /* max curve matrices available */
#define IGL_MAXLMDEFS           64          /* max material/light/lightmodel definitions */
#define IGL_MAXTEVDEFS          16          /* max tevdef()'s */
#define IGL_MAXTEXDEFS          128         /* max texdef()'s */
#define IGL_MAXMENUS            32          /* max menus available */
#define IGL_MAXFONTS            16          /* max raster fonts available */

/* other GL constants */
#define IGL_MAXCURVESEGMENTS    256         /* max curve segments for crv() and crvn() */
#define IGL_MAXPOLYVERTICES     256         /* max vertices in a polygon */
#define IGL_DEFOBJCHUNKSIZE     1020
#define IGL_MAXDEVICES          548         /* max devices on IrisGL - the last one is VIDEO */
#define IGL_MAXDEVQENTRIES      101         /* max entries in device queue */
#define IGL_MAXTIEDVALUATORS    IGL_MAXDEVICES /* max tie()'d valuators */

#define IGL_WINDOWWIDTH         100         /* default window width for winopen() */
#define IGL_WINDOWHEIGHT        100         /* default window height for winopen() */
#define IGL_MAXMENUITEMS        100         /* max entries in a menu */

#define IGL_ZBITS               24          /* bits in emulated Z buffer */

/* other IGL constants */
#define IGL_VERTEXSTACK_SIZE    3           /* vertex stack size.
                    Must be at least 2 for swaptmesh, and 3 for perfly demo */


#if PLATFORM_X11
extern void
igl_X11processEvents ();

#   define IGL_MSG_PROCESS  igl_X11processEvents ();

#   define BOOL             int
#   define BYTE             unsigned char
#   define WORD             unsigned short
#   define DWORD            unsigned long

#   define HWND             Window
#   define HDC              GC
#   define HGLRC            GLXContext
#   define HINSTANCE        Display *
#   define HMENU            Window
#endif /* PLATFORM */


/*******************************************
    igl_windowT
********************************************/
#define IGL_WFLAGS_NONE             0x00
/* visual properties*/
#define IGL_WFLAGS_RGBA             0x01
#define IGL_WFLAGS_MULTIMAP         0x02
#define IGL_WFLAGS_DOUBLEBUFFER     0x04
#define IGL_WFLAGS_STEREOBUFFER     0x08
/* window properties */
#define IGL_WFLAGS_NOBORDER         0x10
#define IGL_WFLAGS_NOPORT           0x20
#define IGL_WFLAGS_BACKGROUND       0x40

#define IGL_WSTATE_NONE             0x0000
/* window state */
#define IGL_WSTATE_FULLSCREEN       0x0100
#define IGL_WSTATE_MAPPED           0x0200
#define IGL_WSTATE_LIGHTING         0x0400
#define IGL_WSTATE_TEXTURING        0x0800
#define IGL_WSTATE_PICKING          0x1000
#define IGL_WSTATE_LMCNULL          0x2000
#define IGL_WSTATE_RDSOURCEZ        0x4000
#define IGL_WSTATE_ZDRAW            0x8000

#define IGL_WSTATE_CURSOROFF        0x10000

#define IGL_WSTATE_CURVIEWPORT      0x40000
#define IGL_WSTATE_CURMATRICES      0x80000

/* vertex state */
#define IGL_VSTATE_COLORED          0x040000
#define IGL_VSTATE_LIGHTED          0x080000
#define IGL_VSTATE_TEXTURED         0x100000
#define IGL_VSTATE_N3F              0x200000
#define IGL_VSTATE_N3FBGN           0x400000
#define IGL_VSTATE_TMESH            0x800000

#define IGL_WINDOWCLASS             "IGL"

/* these are sorted in sequence of the stacking. don't change! */
#define IGL_WLAYER_UNDERDRAW        0
#define IGL_WLAYER_NORMALDRAW       1
#define IGL_WLAYER_OVERDRAW         2
#define IGL_WLAYER_PUPDRAW          3
#define IGL_WLAYER_CURSORDRAW       4
#define IGL_WLAYER_MAX              (IGL_WLAYER_CURSORDRAW+1)

/* TODO: according to the manual "each of the 4 logical frame buffers maintains
 * a separate version..." of doublebuffer state, front/backbuffer, RGB/colormap
 * mode, a separate colormap, color writemask.
 */
/* TODO: the manual states "many rendering options are shared among the four GL
 * frame buffers". Which are shared, which are not?
 */
/* per-layer status information */
typedef struct
{
#if PLATFORM_X11
    HWND hwnd;
    Colormap cmap;
#endif
    HGLRC hrc;
    int xoffs, yoffs;

    /* buffer related */
    struct timeval _swapTime;
    int _swapCounter;

    /* graphics position related */
    short _lineStyleIndex;
    short _patternIndex;

    /* character position related */
    unsigned int *_fontBases;        /* OpenGL list bases */
    unsigned short _currentFont;     /* active font */

    /* color related */
    GLubyte _currentColor[4];        /* current RGB color */
    unsigned long *_colorPalette;    /* layer color palette */
    unsigned short _colormapSize;    /* #entries in palette */
    unsigned short _colorIndex;      /* current color index */
    unsigned int _writeMask;         /* current write mask */

    /* vertex related */
    GLenum _bgnPrimitive;            /* stored glBegin primitive */
    GLenum _bgnDelay;                /* glBegin delay Counter */
    float _currentNormal[3];         /* current normal vector */

    int _stackIdx;                   /* stack for bgn* and swaptmesh */
    GLfloat _vertexStack[IGL_VERTEXSTACK_SIZE][4];
    GLubyte _colorStack[IGL_VERTEXSTACK_SIZE][4];
    GLfloat _normalStack[IGL_VERTEXSTACK_SIZE][3];
    GLfloat _texcoordStack[IGL_VERTEXSTACK_SIZE][4];
} igl_layerT;


#define swapTime        currentLayer->_swapTime
#define swapCounter     currentLayer->_swapCounter

#define lineStyleIndex  currentLayer->_lineStyleIndex
#define patternIndex    currentLayer->_patternIndex

#define fontBases       currentLayer->_fontBases
#define currentFont     currentLayer->_currentFont

#define currentColor    currentLayer->_currentColor
#define colorPalette    currentLayer->_colorPalette
#define colormapSize    currentLayer->_colormapSize
#define colorIndex      currentLayer->_colorIndex
#define writeMask       currentLayer->_writeMask

#define bgnDelay        currentLayer->_bgnDelay
#define bgnPrimitive    currentLayer->_bgnPrimitive
#define currentNormal   currentLayer->_currentNormal
#define stackIdx        currentLayer->_stackIdx
#define vertexStack     currentLayer->_vertexStack
#define normalStack     currentLayer->_normalStack
#define colorStack      currentLayer->_colorStack
#define texcoordStack   currentLayer->_texcoordStack

typedef struct
{
    HWND wnd;
    int parent;
    igl_layerT layers[IGL_WLAYER_MAX];
    igl_layerT *currentLayer;
    int drawMode;
    int swapInterval;

    /* window related */
    char title[256];
    int x, y;
    unsigned int width, height;
    unsigned int minWidth, minHeight;
    unsigned int maxWidth, maxHeight;
    unsigned int aspectX, aspectY;
    unsigned int stepX, stepY;

    long flags;       	            /* IGL_WFLAGS_XXX */
    long state;                     /* IGL_[VW]STATE_XXX */

    GLfloat pickMatrix[16];         /* projection matrix used while picking */
    short pickDeltax, pickDeltay;   /* size of picking area */
    unsigned *pickBuffer;           /* local pickbuffer (Ogl->GL conversion) */

    GLint viewport[4];              /* current OpenGL viewport */
    GLdouble projMatrix[16];        /* current OpenGL projection matrix */
    GLdouble modelMatrix[16];       /* current OpenGL viewing matrix */

    /* gconfig() related */
    long newFlags;                  /* IGL_WFLAGS_XXX before gconfig() */
    unsigned char acPlanes;         /* accumulation buffer planes */
    unsigned char stenPlanes;       /* stencil buffer planes */
    unsigned char msPlanes;         /* multisample buffer planes */
    unsigned char zbPlanes;         /* Z-buffer buffer planes */
    unsigned char rgbPlanes;        /* RGB planes */

    /* graphics position related */
    float gposx, gposy, gposz;

    /* character position related */
    float cposx, cposy, cposz;

    /* color related */
    unsigned char multimapIndex;    /* color submap index (NORMALDRAW only!)*/

    /* drawing related */
    short curveBasisIndex;          /* current curve matrixDef */
    short patchBasisIndexU, patchBasisIndexV; /* current patch matrixDefs */
    short curveSegments;            /* #segments in a curve */
    short patchCurvesU, patchCurvesV;       /* #curves in a patch */
    short patchSegmentsU, patchSegmentsV;   /* #segments in a patch curve */

    /* matrix related */
    short matrixMode;

    /* light related */
    short materialIndex;            /* current materialDef */
    short backMaterialIndex;        /* current backmaterialDef */
    short lmodelIndex;              /* current lightmodelDef */
    short lightIndex[8];            /* current light[i]Def */
    short lmcMode;                  /* current lmcolor() mode */
    GLubyte lmcColor[4];

    /* texture related */
    short tevIndex, texIndex;       /* currently bound texture */
    GLenum textureType;             /* GL_TEXTURE_2D/3D */
    short texgenMode;               /* texgen for TX_[STRQ] enabled? */
    GLfloat currentTexcoord[4];     /* current texture coordinate */

    /* text/font related */
    struct fmcontext *fmContexts;   /* font manager contexts */

    /* pixmode and friends */
    long pixStride;                 /* line length in 32 bit words */
    long pixOffset;                 /* offset into 1st word in bits */
    long pixSize;                   /* size of a pixel in bits */
    long pixInFormat, pixInType;    /* format and subfield type for reading */
    long pixOutFormat, pixOutType;  /* format and subfield type for writing */
    long pixShift;                  /* SHIFT: +=left, -=right */
    long pixAdd24;                  /* ADD24 value */
    long pixCx[2];                  /* bit expand values C0, C1 */
    long pixFlags;                  /* need copy through buffer if any is set */
    /* TODO: drop these 4, their meaning is already reflected by pixFlags */
    char pixTtoB, pixRtoL;          /* flags for TTOB, RTOL */
    char pixZ, pixExpand;           /* flags for ZDATA, EXPAND */

    int xzoom, yzoom;               /* rectzoom factors */

    /* device related */
    int cursorIndex;                /* current cursor */
} igl_windowT;


/*******************************************
    lmdefs
********************************************/
#define IGL_LMMATERIALFLAGS_NONE            0x00
#define IGL_LMMATERIALFLAGS_AMBIENT         0x01
#define IGL_LMMATERIALFLAGS_COLORINDEXES    0x02
#define IGL_LMMATERIALFLAGS_DIFFUSE         0x04
#define IGL_LMMATERIALFLAGS_EMISSION        0x08
#define IGL_LMMATERIALFLAGS_SHININESS       0x10
#define IGL_LMMATERIALFLAGS_SPECULAR        0x20
#define IGL_LMMATERIALFLAGS_CREATED         0x80
typedef struct
{
    short id;
    unsigned short flags;
    float ambient[4];
    float colorIndexes[4];
    float diffuse[4];
    float emission[4];
    float shininess;
    float specular[4];
} igl_lmMaterialDefT;

#define IGL_LMLIGHTFLAGS_NONE           0x00
#define IGL_LMLIGHTFLAGS_AMBIENT        0x01
#define IGL_LMLIGHTFLAGS_LCOLOR         0x02
#define IGL_LMLIGHTFLAGS_POSITION       0x04
#define IGL_LMLIGHTFLAGS_SPOTDIRECTION  0x08
#define IGL_LMLIGHTFLAGS_SPOTLIGHT      0x10
#define IGL_LMLIGHTFLAGS_CREATED        0x80
typedef struct
{
    short id;
    unsigned short flags;
    float ambient[4];
    float lcolor[4];
    float position[4];
    float spotDirection[4];
    float spotLight[2];

    short lightTarget;                  /* GL_LIGHTx to which this lightDef is bound */
} igl_lmLightDefT;

#define IGL_LMMODELFLAGS_NONE           0x00
#define IGL_LMMODELFLAGS_AMBIENT        0x01
#define IGL_LMMODELFLAGS_ATTENUATION    0x02
#define IGL_LMMODELFLAGS_ATTENUATION2   0x04
#define IGL_LMMODELFLAGS_LOCALVIEWER    0x08
#define IGL_LMMODELFLAGS_TWOSIDE        0x10
#define IGL_LMMODELFLAGS_CREATED        0x80
typedef struct
{
    short id;
    unsigned short flags;
    float ambient[4];
    float attenuation[2];
    float attenuation2;
    float localViewer;
    float twoSide;
} igl_lmLmodelDefT;


#define IGL_TEVDEFFLAGS_NONE            0x00
#define IGL_TEVDEFFLAGS_COLOR           0x01
#define IGL_TEVDEFFLAGS_COMPONENT       0x02
typedef struct
{
    short id;
    unsigned short flags;
    int tvMode;
    float blendColor[4];    /* TV_COLOR specifies it, used with TV_BLEND */
    float component;
} igl_tevDefT;


#define IGL_TEXDEFFLAGS_NONE            0x0000
#define IGL_TEXDEFFLAGS_MINFILTER       0x0001
#define IGL_TEXDEFFLAGS_MAGFILTER       0x0002
#define IGL_TEXDEFFLAGS_MAGFILTERALPHA  0x0004
#define IGL_TEXDEFFLAGS_MAGFILTERCOLOR  0x0008
#define IGL_TEXDEFFLAGS_WRAPS           0x0010
#define IGL_TEXDEFFLAGS_WRAPT           0x0020
#define IGL_TEXDEFFLAGS_WRAPR           0x0040
#define IGL_TEXDEFFLAGS_MIPMAPFILTER    0x0080
#define IGL_TEXDEFFLAGS_CONTROLPOINT    0x0100  /* LOD and scale */
#define IGL_TEXDEFFLAGS_CLAMP           0x0200
#define IGL_TEXDEFFLAGS_DETAIL          0x0400
#define IGL_TEXDEFFLAGS_TILE            0x0800
#define IGL_TEXDEFFLAGS_BICUBICFILTER   0x1000
#define IGL_TEXDEFFLAGS_FRAMEBUFFERSRC  0x2000
#define IGL_TEXDEFFLAGS_3D              0x8000
typedef struct
{
    short id;
    unsigned short flags;
    unsigned int glName;
    float minFilter;
    float magFilter;
    float magFilterAlpha;
    float magFilterColor;
    float wrapS;
    float wrapT;
    float wrapR;
    int internalFormat;
    int externalFormat;
    float mipmapFilter[8];
    float ctlPoints[4*2];
    int numCtlPoints;
    float clamp;
    float detail[5];
    float tile[4];
    float bicubicFilter[2];
    int width, height, size;  /* of the actual texture loaded */
} igl_texDefT;


typedef struct
{
    short id;
    float matrix[16];
} igl_matrixDefT;


typedef struct
{
    short id;
    Linestyle lineStyle;
} igl_lineStyleDefT;


typedef struct
{
    short id;
    GLubyte pattern[128];  /* patterns of 32x32 in size */
} igl_patternDefT;


#define IGL_MENUITEMFLAGS_NONE          0x00
#define IGL_MENUITEMFLAGS_DELIMITER     0x01
#define IGL_MENUITEMFLAGS_GRAYED        0x02
#define IGL_MENUITEMFLAGS_CHECKED       0x04
#define IGL_MENUITEMFLAGS_UNCHECKED     0x08
typedef struct
{
    char title[256];
    int id;
    short flags;
    short dontExecuteRoutine;
    int (*routine)(int, ...);
    long submenu;
#if PLATFORM_X11
    int y;
#endif
} igl_menuItemT;

typedef struct
{
    HMENU hmenu;
    char title[256];
    int (*routine)(int, ...);
    igl_menuItemT items[IGL_MAXMENUITEMS];
    int numItems;
#if PLATFORM_X11
    int width, height, itemHeight, titlePosX;
    Boolean hasCheckboxes;
    GC textGC;          /* GC for displaying text */
    GC backgroundGC;    /* GC for background color */
    GC borderWhiteGC;   /* GC for menu surrounding border */
    GC borderGrayGC;    /* GC for menu surrounding border */
    Pixmap itemUnchecked, itemChecked;
#endif
} igl_menuT;


typedef struct
{
    Object id;
    long chunkSize;
    char *chunk;
    char *beginPtr, *curPtr, *endPtr;
} igl_objectT;

typedef struct
{
    short id;
    short firstChar, nChars;
    short fontHeight, fontDescent;
    Fontchar *chars;
    Lfontchar *lchars;
#if PLATFORM_X11
    XFontStruct *xfont;
#endif
    GLubyte *raster;
} igl_fontT;

typedef struct
{
    short id;
    Cursor xid;
    int type;
    Pixmap src;
    Pixmap mask;
    int size, xori, yori;
} igl_cursorT;

typedef struct
{
    void (*fullscrn) ();
    void (*endfullscrn) ();
    void (*clear) ();
    void (*lsetdepth) (long near_, long far_);
    void (*swapbuffers) ();
    void (*mswapbuffers) (long fbuf);
    void (*backbuffer) (Boolean enable);
    void (*frontbuffer) (Boolean enable);
    void (*leftbuffer) (Boolean enable);
    void (*rightbuffer) (Boolean enable);
    void (*backface) (Boolean enable);
    void (*frontface) (Boolean enable);
    void (*depthcue) (Boolean enable);
    void (*lshaderange) (Colorindex lowin, Colorindex highin, long znear, long zfar);
    void (*lRGBrange) (short rmin, short gmin, short bmin, short rmax, short gmax, short bmax, long znear, long zfar);
    void (*zbuffer) (Boolean enable);
    void (*zfunction) (long func);
    void (*zclear) ();
    void (*czclear) (unsigned long cval, long zval);
    void (*sclear) (unsigned long sval);
    void (*stencil) (long enable, unsigned long ref, long func, unsigned long mask, long fail, long pass, long zpass);
    void (*acbuf) (long op, float value);
    void (*shademodel) (int mode);
    void (*color) (Colorindex c);
    void (*mapcolor) (Colorindex i, short r, short g, short b);
    void (*cpack) (unsigned long color);
    void (*RGBcolor) (short r, short g, short b);
    void (*afunction) (long ref, long func);
    void (*blendfunction) (long sfactor, long dfactor);
    void (*dither) (long mode);
    void (*fogvertex) (long mode, float *params);
    void (*logicop) (long opcode);
    void (*c4f) (float cv[4]);
    void (*c4i) (int cv[4]);
    void (*c4s) (short cv[4]);
    void (*mmode) (short mode);
    void (*loadmatrix) (Matrix m);
    void (*multmatrix) (Matrix m);
    void (*pushmatrix) ();
    void (*popmatrix) ();
    void (*rot) (float amount, char angle);
    void (*rotate) (Angle amount, char angle);
    void (*translate) (Coord x, Coord y, Coord z);
    void (*scale) (float x, float y, float z);
    void (*polarview) (Coord dist, Angle azim, Angle inc, Angle twist);
    void (*clipplane) (long index, long mode, float *params);
    void (*ortho) (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_);
    void (*ortho2) (Coord left, Coord right, Coord bottom, Coord top);
    void (*window) (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_);
    void (*perspective) (Angle fovy, float aspect, Coord near_, Coord far_);
    void (*viewport) (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);
    void (*reshapeviewport) ();
    void (*pushviewport) ();
    void (*popviewport) ();
    void (*lookat) (Coord vx, Coord vy, Coord vz, Coord px, Coord py, Coord pz, Angle twist);
    void (*scrmask) (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);
    void (*polf) (long n, const Coord parray[][3]);
    void (*polfi) (long n, const Icoord parray[][3]);
    void (*polfs) (long n, const Scoord parray[][3]);
    void (*polf2) (long n, const Coord parray[][2]);
    void (*polf2i) (long n, const Icoord parray[][2]);
    void (*polf2s) (long n, const Scoord parray[][2]);
    void (*poly) (long n, const Coord parray[][3]);
    void (*polyi) (long n, const Icoord parray[][3]);
    void (*polys) (long n, const Scoord parray[][3]);
    void (*poly2) (long n, const Coord parray[][2]);
    void (*poly2i) (long n, const Icoord parray[][2]);
    void (*poly2s) (long n, const Scoord parray[][2]);
    void (*polymode) (long mode);
    void (*polysmooth) (long mode);
    void (*displacepolygon) (float scalefactor);
    void (*pmv) (Coord x, Coord y, Coord z);
    void (*pdr) (Coord x, Coord y, Coord z);
    void (*pclos) ();
    void (*pnt) (Coord x, Coord y, Coord z);
    void (*pntsizef) (float n);
    void (*pntsmooth) (unsigned long mode);
    void (*rect) (Coord a, Coord b, Coord c, Coord d);
    void (*rectf) (Coord a, Coord b, Coord c, Coord d);
    void (*sbox) (Coord a, Coord b, Coord c, Coord d);
    void (*sboxf) (Coord a, Coord b, Coord c, Coord d);
    void (*v4f) (float vector[4]);
    void (*arc) (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle);
    void (*arcf) (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle);
    void (*circ) (Coord x, Coord y, Coord radius);
    void (*circf) (Coord x, Coord y, Coord radius);
    void (*bgntmesh) ();
    void (*swaptmesh) ();
    void (*bgnclosedline) ();
    void (*bgncurve) ();
    void (*endcurve) ();
    void (*nurbscurve) (long knotCount, const double *knotList, long offset, const double *ctlArray, long order, long type);
    void (*bgnsurface) ();
    void (*endsurface) ();
    void (*nurbssurface) (long scount, const double *sknot, long tcount, const double *tknot, long soffset, long toffset, const double *ctlArray, long sorder, long torder, long type);
    void (*bgntrim) ();
    void (*endtrim) ();
    void (*pwlcurve) (long n, double *dataArray, long byteSize, long type);
    void (*setnurbsproperty) (long property, float value);
    void (*bgnline) ();
    void (*bgnpoint) ();
    void (*endpoint) ();
    void (*bgnpolygon) ();
    void (*bgnqstrip) ();
    void (*curveit) (short nitem);
    void (*crvn) (long n, Coord points[][3]);
    void (*rcrvn) (long n, Coord points[][4]);
    void (*curveprecision) (short n);
    void (*curvebasis) (short n);
    void (*patch) (Matrix geomx, Matrix geomy, Matrix geomz);
    void (*rpatch) (Matrix geomx, Matrix geomy, Matrix geomz, Matrix geomw);
    void (*patchcurves) (short nu, short nv);
    void (*patchprecision) (short nu, short nv);
    void (*patchbasis) (short nu, short nv);
    void (*nmode) (long mode);
    void (*n3f) (float *vector);
    void (*linesmooth) (unsigned long mode);
    void (*linewidthf) (float width);
    void (*deflinestyle) (short n, Linestyle ls);
    void (*setlinestyle) (short n);
    void (*lsrepeat) (long factor);
    void (*setpattern) (short index);
    void (*lmbind) (short target, short index);
    void (*lmcolor) (long mode);
    void (*move) (Coord x, Coord y, Coord z);
    void (*rmv) (Coord dx, Coord dy, Coord dz);
    void (*rpmv) (Coord dx, Coord dy, Coord dz);
    void (*draw) (Coord x, Coord y, Coord z);
    void (*rdr) (Coord dx, Coord dy, Coord dz);
    void (*rpdr) (Coord dx, Coord dy, Coord dz);
    void (*splf) (long n, Coord parray[][3], Colorindex iarray[]);
    void (*splfi) (long n, Icoord parray[][3], Colorindex iarray[]);
    void (*splfs) (long n, Scoord parray[][3], Colorindex iarray[]);
    void (*splf2) (long n, Coord parray[][2], Colorindex iarray[]);
    void (*splf2i) (long n, Icoord parray[][2], Colorindex iarray[]);
    void (*splf2s) (long n, Scoord parray[][2], Colorindex iarray[]);
    void (*callobj) (Object id);
    void (*tevbind) (long target, long index);
    void (*texbind) (long target, long index);
    void (*texgen) (long coord, long mode, float *params);
    void (*t4f) (float vector[4]);
    void (*cmov) (Coord x, Coord y, Coord z);
    void (*lcharstr) (long type, void *str);
    void (*font) (short fntnum);
    void (*pushattributes) ();
    void (*popattributes) ();
    void (*defpattern) (short n, short size, unsigned short mask[]);
    void (*initnames) ();
    void (*loadname) (short name);
    void (*pushname) (short name);
    void (*popname) ();
} igl_apiList;

extern int _igl_initObjectApi (void);
extern void _igl_exitObjectApi (void);


/*******************************************
    igl_globalsT
********************************************/
#define IGL_IFLAGS_NOLAYERS     0x0100
#define IGL_IFLAGS_NODEBUG      0x8000
typedef struct
{
    /* app related */
    HINSTANCE appInstance;  /* set by WinMain() on Win32, set by XOpenDisplay() on UNIX */
    int initialized;        /* since some things cannot be inited before WGL, we have a helper */
    int flags;
    int debug;

#if PLATFORM_X11
    XVisualInfo visualARGB;     /* visual supporting transparency */
    XVisualInfo visualCmap;     /* colormap visual */
    XVisualInfo *visualInfo[IGL_WLAYER_MAX];
    Colormap colormaps[IGL_WLAYER_MAX];
    void (*swapInterval)(Display *, GLXDrawable, int);
    void (*swapIntervalSGI)(int);
    int yieldTime;
#endif

    /* callback for releasing window resources (used by libfm) */
    void (*fmReleaseCB) (igl_windowT *);

    /* glcompat settings */
    int glcSOFTATTACH, glcSLOWMAPCOLORS;

    /* cached getgdesc screen parameters */
    long gdXPMAX, gdYPMAX;      /* getgdesc() for XPMAX, YPMAX */
    long gdXMMAX, gdYMMAX;      /* getgdesc() for XMMAX, YMMAX */
    long gdZMIN, gdZMAX;        /* getgdesc() for ZMIN, ZMAX */
    int gdZBITS;                /* bits in emulated Z-Buffer */

    /* window related */
    int winDefWidth, winDefHeight;  /* default width/height of GL windows */
    igl_windowT openWindows[IGL_MAXWINDOWS];
    igl_windowT *queryWindow;   /* needed for getgdesc() */
    int winOffsetX, winOffsetY; /* border surrounding client rectangle */
    int activeWindow, currentWindow;
    int winPosx, winPosy;
    int winWidth, winHeight;
    int winMinWidth, winMinHeight;
    int winMaxWidth, winMaxHeight;
    int winAspectX, winAspectY;
    int winStepX, winStepY;
    long windowFlags; /* reset on winopen() */

    /* color related */
    unsigned long normalColorPalette[IGL_MAXCOLORS];
    unsigned long overColorPalette[16];
    unsigned long underColorPalette[16];
    unsigned long popupColorPalette[16];
    unsigned long cursorColorPalette[4];

    /* vertex related */
    GLUnurbsObj *nurbsCurve;
    igl_lineStyleDefT lineStyles[IGL_MAXLINESTYLES];
    igl_patternDefT patterns[IGL_MAXPATTERNS];
    igl_matrixDefT matrixDefs[IGL_MAXMATRICES];

    /* light related */
    igl_lmMaterialDefT materialDefs[IGL_MAXLMDEFS];
    igl_lmLightDefT lightDefs[IGL_MAXLMDEFS];
    igl_lmLmodelDefT lmodelDefs[IGL_MAXLMDEFS];

    /* device related */
    long deviceVal[IGL_MAXDEVICES];             /* device values */
    short deviceLast[IGL_MAXDEVICES];           /* last reported values */
    short deviceQueue[IGL_MAXDEVICES];          /* queued devices */
    short deviceNoise[IGL_MAXDEVICES];          /* device noise count */
    short eventQueue[IGL_MAXDEVQENTRIES*2];     /* event queue */
    int numQueuedEvents;
    Device tiedValuators[IGL_MAXTIEDVALUATORS*2];
    int numTiedValuators;
    struct timeval lastTime;
    igl_cursorT cursors[IGL_MAXCURSORS];
    int cursorType;

    /* texture related */
    igl_tevDefT tevDefs[IGL_MAXTEVDEFS];
    igl_texDefT texDefs[IGL_MAXTEXDEFS];

    /* menu related */
    igl_menuT menus[IGL_MAXMENUS];

    /* object related */
    igl_objectT *objects;
    long objChunkSize;
    int currentObjIdx;
    long numObjects;
    long numAllocatedObjects;
    igl_apiList *execApi;
    igl_apiList *objApi;
    igl_apiList *api;   /* equals either execApi or objApi */

    /* text related */
    igl_fontT fontDefs[IGL_MAXFONTS];
} igl_globalsT;

extern igl_globalsT *igl;


#if PLATFORM_X11
#   define IGL_NULLWND      0
#   define IGL_NULLCTX      NULL
#   define IGL_NULLMENU     0

#   define IGL_VISUAL       igl->visualInfo[IGL_WLAYER_NORMALDRAW]
#   define IGL_DISPLAY      igl->appInstance
#   define IGL_SCREEN       IGL_VISUAL->screen
#   define IGL_ROOT         RootWindow(IGL_DISPLAY, IGL_SCREEN)
#   define IGL_XWINDOW(w)   (w)->wnd
#   define IGL_XPARENT(w)   ((w)->parent >= 0 ? IGL_PARENT (w)->wnd : IGL_ROOT)

#   define RGB(r,g,b)       ((DWORD)((BYTE)(r)|((BYTE)(g)<<8)|((BYTE)(b)<<16)))
#   define RGBA(r,g,b,a)    ((DWORD)(RGB(r,g,b)|((BYTE)(a)<<24)))
#   define GetRValue(_rgb)  ((BYTE)(_rgb))
#   define GetGValue(_rgb)  ((BYTE)((_rgb) >> 8))
#   define GetBValue(_rgb)  ((BYTE)((_rgb) >> 16))
#   define GetAValue(_rgb)  ((BYTE)((_rgb) >> 24))
#endif

#define IGL_CLR(s)          memset(s, 0, sizeof(*s))
#define IGL_CLR_SAVEID(s)   { short id = (s)->id; memset(s, 0, sizeof(*s)); (s)->id = id; }

/* vector copy, converting types if necessary */
#define IGL_V2COPY(d,s) (d)[0] = (s)[0],(d)[1] = (s)[1]
#define IGL_V3COPY(d,s) (d)[0] = (s)[0],(d)[1] = (s)[1],(d)[2] = (s)[2]
#define IGL_V4COPY(d,s) (d)[0] = (s)[0],(d)[1] = (s)[1],(d)[2] = (s)[2],(d)[3] = (s)[3]

/* maps GL id's to IGL index */
extern void _igl_mapIDinit (short *map, int size, int offset, int flags);
extern int _igl_mapIDalloc (short *map, int size, int offset, short id, int flags);
#define IGL_MAPINIT(t,n,a)  _igl_mapIDinit(&(t)->id, n, sizeof(*t), a)
#define IGL_MAPID(t,n,x,a)  _igl_mapIDalloc(&(t)->id, n, sizeof(*t), x, a)

/* returns current window context (igl_windowT *) */
#define IGL_WINDOW(w)       (&igl->openWindows[w])
#define IGL_CTX()           IGL_WINDOW(igl->currentWindow)
#define IGL_PARENT(p)       ((p)->parent >= 0 ? IGL_WINDOW ((p)->parent) : NULL)

/* returns current HRC of current window */
#define IGL_HRC(_wptr)      ((_wptr)->currentLayer->hrc)

/* converts upper-left coordinates to lower-left Iris/OpenGL and vice versa */
#define GL_Y(y)             (int)(igl->gdYPMAX - (y) - 1)
#define GL_WY(w,y)          (int)((w)->height - (y) - 1)


extern void _igl_initColor (void);
extern void _igl_resetColor (void);

extern void _igl_qenter (Device dev, short val, Boolean filter);
extern void _igl_glTimers (void);
extern void _igl_recolorCursor (igl_windowT *wptr);

extern void _igl_fontInit (igl_windowT *wptr, int idx);
extern void _igl_fontRelease (igl_windowT *wptr, int idx);
extern void _igl_defineXfont (int n, char *name);

extern int _igl_pixInMode(int *format, int *type);
extern void *_igl_pixInConvert(int *format, int *type, int size, void *image, int width, int height);
extern void _igl_pixInEnd(int format, int type, void *buffer, void *image);
extern void *_igl_pixOutMode(int *format, int *type, void *image, int width, int height);
extern void _igl_pixOutConvert(int format, int type, void *image, void *buffer, int width, int height);
extern void _igl_pixOutEnd(int format, int type, void *image, void *buffer);
extern unsigned long _igl_pixPipe (unsigned long pixel);

extern void _igl_pushTransform (void);
extern void _igl_popTransform (void);
extern void _igl_getViewport (void);
extern void _igl_getMatrices (void);
extern void _igl_ortho2D (int reshape);

extern void _igl_setLmDefaults (short deftype, void *ptr);

extern int _igl_initObjects (void);
extern void _igl_releaseObjects (void);
extern igl_objFcnT *_igl_addObjectFcn (igl_objectT *obj, int opcode);


/* IGL initialization check */
#define IGL_CHECKINIT()     if (!igl) _igl_init(NULL)
#define IGL_CHECKWND(_ret) { \
    IGL_CHECKINIT (); \
    if (igl->currentWindow == -1 || igl->openWindows[igl->currentWindow].wnd == IGL_NULLWND) \
        return _ret;                                                  \
}
#define IGL_CHECKWNDV() { \
    IGL_CHECKINIT (); \
    if (igl->currentWindow == -1 || igl->openWindows[igl->currentWindow].wnd == IGL_NULLWND) \
        return;                                                  \
}

/* map colormap index to RGB color (only for NORMALDRAW) */
#define IGL_CMAP2ABGR(w,i)  (((w)->flags & IGL_WFLAGS_MULTIMAP) ? \
		(w)->colorPalette[(w)->multimapIndex*IGL_MAXMULTIMAPCOLORS + \
		((i) & (IGL_MAXMULTIMAPCOLORS-1) & ~(w)->writeMask)] : \
		(w)->colorPalette[(i) & (IGL_MAXCOLORS-1) & ~(w)->writeMask])

/* turn lighting on or off for primitives unlighted in IrisGL */
#define IGL_ENLIGHT()   if ((IGL_CTX ()->state & (IGL_WSTATE_LIGHTING|IGL_VSTATE_LIGHTED)) == IGL_WSTATE_LIGHTING) \
        { IGL_CTX ()->state |= IGL_VSTATE_LIGHTED; glEnable (GL_LIGHTING); }
#define IGL_DISLIGHT()  if (IGL_CTX ()->state & IGL_VSTATE_LIGHTED) \
        { IGL_CTX ()->state &= ~IGL_VSTATE_LIGHTED; glDisable (GL_LIGHTING); }

/* manage current normal */
#define IGL_BGNN3F()    if (IGL_CTX ()->state & IGL_VSTATE_N3F) \
        { IGL_CTX ()->state |= IGL_VSTATE_N3FBGN; \
	  IGL_CTX ()->state &= ~IGL_VSTATE_N3F; }
#define IGL_ENDN3F()    IGL_CTX ()->state &= ~IGL_VSTATE_N3F
#define IGL_CLEARN3F()  IGL_CTX ()->state &= ~(IGL_VSTATE_N3F|IGL_VSTATE_N3FBGN)

/* turn lighting on/off, depending on the current normal */
#define IGL_BGNLIGHT()  IGL_BGNN3F(); \
	if (IGL_CTX ()->state & (IGL_VSTATE_N3F|IGL_VSTATE_N3FBGN)) \
            { IGL_ENLIGHT (); } else { IGL_DISLIGHT (); }
#define IGL_ENDLIGHT()  IGL_ENDN3F()


/* tracing stuff */
extern void _igl_init (HINSTANCE app);
extern void (_igl_trace) (const char *func, int immed, const char *fmt, ...);

#define IGL_V2FMT(f)    "{%" #f ",%" #f "}"
#define IGL_V2PAR(v)    v[0],v[1]
#define IGL_V3FMT(f)    "{%" #f ",%" #f ",%" #f "}"
#define IGL_V3PAR(v)    v[0],v[1],v[2]
#define IGL_V4FMT(f)    "{%" #f ",%" #f ",%" #f ",%" #f "}"
#define IGL_V4PAR(v)    v[0],v[1],v[2],v[3]

#define IGL_MATFMT(f)   "{" IGL_V4FMT(f) "," IGL_V4FMT(f) "," IGL_V4FMT(f) "," IGL_V4FMT(f) "}"
#define IGL_MATPAR(m)   IGL_V4PAR(m[0]),IGL_V4PAR(m[1]),IGL_V4PAR(m[2]),IGL_V4PAR(m[3])

#ifndef DEBUG /* debug output off */
#define __func__ ""
#define _igl_trace(func,immed,fmt...) /**/
#elif defined(__GNUC__) /* gcc version */
#define __func__ __FUNCTION__
/* this provides a significant speedup. It avoids tracing when not debugging. */
#define _igl_trace(func,immed,fmt...) if (!igl || igl->debug) (_igl_trace)(func,immed,fmt);
#else /* mipspro cc doesn't support __FUNCTION__ and the trace macro */
#define _stringify(x) #x
#define stringify(x) _stringify(x)
#define __func__ __FILE__ ":" stringify(__LINE__)
#endif


#ifdef __cplusplus
}
#endif

#endif  /* IGLCMN_H */
