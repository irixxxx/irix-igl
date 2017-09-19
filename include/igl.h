/*
 * igl.h
 *
 * IGL version of SGI's GL public interface
 */

#ifndef IGL_H
#define IGL_H


#ifdef _WIN32
#   define PLATFORM_WIN32   1
#   define PLATFORM_X11     0

#else
#   define PLATFORM_X11     1
#   define PLATFORM_WIN32   0

#   define NULL             0
#   define FALSE            0
#   define TRUE             1
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char Byte;
typedef long Boolean;
typedef char *String;
typedef void *Lstring;

typedef short Angle;
typedef short Screencoord;
typedef short Scoord;
typedef long Icoord;
typedef float Coord;
typedef float Matrix[4][4];

typedef unsigned short Colorindex;
typedef unsigned char RGBvalue;

typedef unsigned short Device;

#define PATTERN_16		16
#define PATTERN_32		32
#define PATTERN_64		64

#define PATTERN_16_SIZE		16
#define PATTERN_32_SIZE		64
#define PATTERN_64_SIZE		256
typedef unsigned short Pattern16[PATTERN_16_SIZE];
typedef unsigned short Pattern32[PATTERN_32_SIZE];
typedef unsigned short Pattern64[PATTERN_64_SIZE];

typedef unsigned short Linestyle;

typedef struct
{
    unsigned long value;
    unsigned long offset;
    short w, h;
    short xoff, yoff;
    short xmove, ymove;
} Lfontchar;

typedef struct
{
    unsigned short offset;
    Byte w, h;
    char xoff, yoff;
    short width;
} Fontchar;

typedef long Object;
typedef long Tag;
typedef long Offset;


/* names for colors in color map loaded by greset() */
#define BLACK           0
#define RED             1
#define GREEN           2
#define YELLOW          3
#define BLUE            4
#define MAGENTA         5
#define CYAN            6
#define WHITE           7

/* values returned by getdisplaymode() */
#define DMRGB           0    /* RGB single buffer */
#define DMSINGLE        1    /* color map single buffer */
#define DMDOUBLE        2    /* color map double buffer */
#define DMRGBDOUBLE     5    /* RGB double buffer */

/* defines for acbuf() */
#define AC_CLEAR                0
#define AC_ACCUMULATE           1
#define AC_CLEAR_ACCUMULATE     2
#define AC_RETURN               3
#define AC_MULT                 4
#define AC_ADD                  5

/* bits in value returned by getbuffer() */
#define BCKBUFFER       0x1
#define FRNTBUFFER      0x2
#define DRAWZBUFFER     0x4

/* defines for pntsmooth() */
#define SMP_OFF             0x0
#define SMP_ON              0x1
#define SMP_SMOOTHER        0x2

/* defines for linesmooth() */
#define SML_OFF             0x0
#define SML_ON              0x1
#define SML_SMOOTHER        0x2
#define SML_END_CORRECT     0x4

/* defines for polysmooth() */
#define PYSM_OFF            0
#define PYSM_ON             1
#define PYSM_SHRINK         2

/* defines for blendfunction() */
#define BF_ZERO             0
#define BF_ONE              1
#define BF_DC               2
#define BF_SC               2
#define BF_MDC              3
#define BF_MSC              3
#define BF_SA               4
#define BF_MSA              5
#define BF_DA               6
#define BF_MDA              7
#define BF_MIN_SA_MDA       8
#define BF_CA               9
#define BF_MCA              10
#define BF_CC               11
#define BF_MCC              12
#define BF_MIN              13
#define BF_MAX              14

/* defines for afunction() */
#define AF_NEVER        0
#define AF_LESS         1
#define AF_EQUAL        2
#define AF_LEQUAL       3
#define AF_GREATER      4
#define AF_NOTEQUAL     5
#define AF_GEQUAL       6
#define AF_ALWAYS       7

/* defines for zfunction() */
#define ZF_NEVER        0
#define ZF_LESS         1
#define ZF_EQUAL        2
#define ZF_LEQUAL       3
#define ZF_GREATER      4
#define ZF_NOTEQUAL     5
#define ZF_GEQUAL       6
#define ZF_ALWAYS       7

/* defines for dither() */
#define DT_OFF  0
#define DT_ON   1

/* defines for polymode() */
#define PYM_FILL        1
#define PYM_POINT       2
#define PYM_LINE        3
#define PYM_HOLLOW      4
#define PYM_LINE_FAST   5

/* defines for fogvertex() */
#define FG_OFF          0
#define FG_ON           1
#define FG_DEFINE       2
#define FG_VTX_EXP      2
#define FG_VTX_LIN      3
#define FG_PIX_EXP      4
#define FG_PIX_LIN      5
#define FG_VTX_EXP2     6 
#define FG_PIX_EXP2     7

/* defines for shademodel() */
#define FLAT            0
#define GOURAUD         1

/* defines for logicop() */
#define LO_ZERO         0x0
#define LO_AND          0x1
#define LO_ANDR         0x2
#define LO_SRC          0x3
#define LO_ANDI         0x4
#define LO_DST          0x5
#define LO_XOR          0x6
#define LO_OR           0x7
#define LO_NOR          0x8
#define LO_XNOR         0x9
#define LO_NDST         0xa
#define LO_ORR          0xb
#define LO_NSRC         0xc
#define LO_ORI          0xd
#define LO_NAND         0xe
#define LO_ONE          0xf

/* defines for pwlcurve() */
#define N_ST        0x8
#define N_STW       0xd
#define N_XYZ       0x4c
#define N_XYZW      0x51
#define N_TEX       0x88
#define N_TEXW      0x8d
#define N_RGBA      0xd0
#define N_RGBAW     0xd5

/* defines for nurbscurve() */
#define N_P2D       0x8
#define N_P2DR      0xd
#define N_V3D       0x4c
#define N_V3DR      0x51
#define N_T2D       0x88
#define N_T2DR      0x8d
#define N_C4D       0xd0
#define N_C4DR      0xd5

/* matrix modes */
#define MSINGLE         0
#define MPROJECTION     1
#define MVIEWING        2
#define MTEXTURE        3

/* defines for clipplane() */
#define CP_OFF          0
#define CP_ON           1
#define CP_DEFINE       2

/* lmdef() MATERIAL properties */
#define DEFMATERIAL     0
#define EMISSION        1
#define AMBIENT         2
#define DIFFUSE         3
#define SPECULAR        4
#define SHININESS       5
#define COLORINDEXES    6
#define ALPHA           7
#define LMNULL          0.0

/* lmdef() LIGHT properties */
#define DEFLIGHT        100
#define LCOLOR          101
#define POSITION        102
#define SPOTDIRECTION   103
#define SPOTLIGHT       104

/* lmdef() LIGHTINGMODEL properties */
#define DEFLMODEL       200
#define LOCALVIEWER     201
#define ATTENUATION     202
#define ATTENUATION2    203
#define TWOSIDE         204

/* lmbind() TARGET constants */
#define MATERIAL        1000
#define BACKMATERIAL    1001
#define LIGHT0          1100
#define LIGHT1          1101
#define LIGHT2          1102
#define LIGHT3          1103
#define LIGHT4          1104
#define LIGHT5          1105
#define LIGHT6          1106
#define LIGHT7          1107
#define LMODEL          1200

/* lmcolor() modes */
#define LMC_COLOR       0
#define LMC_EMISSION    1
#define LMC_AMBIENT     2
#define LMC_DIFFUSE     3
#define LMC_SPECULAR    4
#define LMC_AD          5
#define LMC_NULL        6

/* LIGHT constants */
#define MAXLIGHTS        8
#define MAXRESTRICTIONS  4

/* NURBS properties */
#define N_PIXEL_TOLERANCE   1
#define N_CULLING           2
#define N_DISPLAY           3
#define N_ERRORCHECKING     4
#define N_SUBDIVISIONS      5
#define N_S_STEPS           6
#define N_T_STEPS           7
#define N_TILES             8
#define N_TMP1              9
#define N_TMP2              10
#define N_TMP3              11
#define N_TMP4              12
#define N_TMP5              13
#define N_TMP6              14

#define N_FILL              1.0
#define N_OUTLINE_POLY      2.0
#define N_OUTLINE_PATCH     5.0
#define N_ISOLINE_S         12.0

/* defines for stencil() */
#define ST_KEEP         0
#define ST_ZERO         1
#define ST_REPLACE      2
#define ST_INCR         3
#define ST_DECR         4
#define ST_INVERT       5
#define SF_NEVER        0
#define SF_LESS         1
#define SF_EQUAL        2
#define SF_LEQUAL       3
#define SF_GREATER      4
#define SF_NOTEQUAL     5
#define SF_GEQUAL       6
#define SF_ALWAYS       7

/* defines for nmode() */
#define NAUTO           0
#define NNORMALIZE      1

/* defines for tevdef() */
#define TV_MODULATE             0x101
#define TV_BLEND                0x102
#define TV_DECAL                0x103
#define TV_COLOR                0x200
#define TV_SHADOW               0x104
#define TV_ALPHA                0x105
#define TV_COMPONENT_SELECT     0x300
#define TV_I_GETS_R             0x310
#define TV_I_GETS_G             0x320
#define TV_I_GETS_B             0x330
#define TV_I_GETS_A             0x340
#define TV_IA_GETS_RG           0x350
#define TV_IA_GETS_BA           0x360
#define TV_I_GETS_I             0x370
#define TV_NULL                 0x000
#define TV_ENV0                 0

/* defines for texdef() */
#define TX_MINFILTER                0x100
#define TX_MAGFILTER                0x200
#define TX_MAGFILTER_COLOR          0xa00
#define TX_MAGFILTER_ALPHA          0xb00
#define TX_WRAP                     0x300
#define TX_WRAP_S                   0x310
#define TX_WRAP_T                   0x320
#define TX_WRAP_R                   0x330
#define TX_TILE                     0x400
#define TX_BORDER                   0x500
#define TX_DETAIL                   0xe00
#define TX_FAST_DEFINE              0x0f00
#define TX_NOCOPY                   TX_FAST_DEFINE /* obsolete */
#define TX_FRAMEBUFFER_SRC          0x1000 /* obsolete */
#define TX_SUBTEXLOAD               0x2000 /* obsolete */
#define TX_FORMATTED_16bit_64x64    0x2000
#define TX_FORMATTED_ABGR_8_128x128 0x2010
#define TX_CONTROL_POINT            0xc00
#define TX_CONTROL_CLAMP            0xd00
#define TX_NULL                     0x000

#define TX_EXTERNAL_FORMAT      0x700
#define TX_PACK_8               0x710   /* just like STAPUFT 4.0 */
#define TX_PACK_16              0x720   /* */
#define TX_NOPACK_12            0x730   /* 12 bits in 12 lsb bits of short */
#define TX_PIXMODE              0x3000
#define TX_BICUBIC_FILTER       0x4000    /* user bicubic kernel parameters */

#define TX_INTERNAL_FORMAT      0x600
/* choices for TX_INTERNAL_FORMAT */
#define TX_I_12                 0x610   /* 1-comp                      full    speed */
#define TX_I_12A_4              0x610   /*        2-comp               full    speed */
#define TX_I_8                  0x620   /* 1-comp                      full    speed */
#define TX_IA_8                 0x620   /*        2-comp               full    speed */
#define TX_RGB_5                0x630   /*               3-comp        full    speed */
#define TX_RGBA_4               0x640   /*                      4-comp full    speed */
#define TX_IA_12                0x650   /*        2-comp               half    speed */
#define TX_RGBA_8               0x660   /*                      4-comp half    speed */
#define TX_RGB_8                0x660   /*               3-comp        half    speed */
#define TX_RGB_12               0x680   /*               3-comp        third   speed */
#define TX_RGBA_12              0x670   /*                      4-comp quarter speed */
#define TX_I_16                 0x690    /* should only be used in SHADOW mode */


#define TX_MIPMAP_FILTER_KERNEL 0x900   /* seperable 8x8x8 filter kernel */

/* texture filter choices */
#define TX_POINT                0x110
#define TX_BILINEAR             0x220
#define TX_MIPMAP               0x120
#define TX_MIPMAP_POINT         0x121
#define TX_MIPMAP_LINEAR        0x122
#define TX_MIPMAP_BILINEAR      0x123
#define TX_MIPMAP_TRILINEAR     0x124
#define TX_BICUBIC              0x230
#define TX_SHARPEN              0x240
#define TX_MODULATE_DETAIL      0x250
#define TX_ADD_DETAIL           0x260
#define TX_TRILINEAR            0x270
#define TX_MIPMAP_QUADLINEAR    0x280
#define TX_BICUBIC_GEQUAL       0x290
#define TX_BICUBIC_LEQUAL       0x2a0
#define TX_BILINEAR_GEQUAL      0x2b0
#define TX_BILINEAR_LEQUAL      0x2c0

/* texture wrapping access choices */
#define TX_REPEAT       0x301
#define TX_CLAMP        0x302
#define TX_SELECT       0x303

/* texture targets */
#define TX_TEXTURE_0        0
#define TX_TEXTURE_DETAIL   1 /* This binds a texture to the DETAIL texture resource */
#define TX_TEXTURE_IDLE     2

/* defines for texgen() */
#define TX_S            0
#define TX_T            1
#define TX_R            2
#define TX_Q            3
#define TG_OFF          0
#define TG_ON           1
#define TG_CONTOUR      2
#define TG_LINEAR       3
#define TG_SPHEREMAP    4
#define TG_REFRACTMAP   5

/* defines for minmax() */
#define MINMAX_ENABLE           0
#define MINMAX_DISABLE          1
#define MINMAX_INIT             2

/* defines for hgram() */
#define HIST_ENABLE             0
#define HIST_DISABLE            1
#define HIST_CLEAR              2

/* defines for getgdesc() */
#define GD_XPMAX                0
#define GD_YPMAX                1
#define GD_XMMAX                2
#define GD_YMMAX                3
#define GD_ZMIN                 4
#define GD_ZMAX                 5
#define GD_BITS_NORM_SNG_RED    6
#define GD_BITS_NORM_SNG_GREEN  7
#define GD_BITS_NORM_SNG_BLUE   8
#define GD_BITS_NORM_DBL_RED    9
#define GD_BITS_NORM_DBL_GREEN  10
#define GD_BITS_NORM_DBL_BLUE   11
#define GD_BITS_NORM_SNG_CMODE  12
#define GD_BITS_NORM_DBL_CMODE  13
#define GD_BITS_NORM_SNG_MMAP   14
#define GD_BITS_NORM_DBL_MMAP   15
#define GD_BITS_NORM_ZBUFFER    16
#define GD_BITS_OVER_SNG_CMODE  17
#define GD_BITS_UNDR_SNG_CMODE  18
#define GD_BITS_PUP_SNG_CMODE   19
#define GD_BITS_NORM_SNG_ALPHA  21 
#define GD_BITS_NORM_DBL_ALPHA  22
#define GD_BITS_CURSOR          23
#define GD_OVERUNDER_SHARED     24
#define GD_BLEND                25
#define GD_CIFRACT              26
#define GD_CROSSHAIR_CINDEX     27
#define GD_DITHER               28
#define GD_LINESMOOTH_CMODE     30
#define GD_LINESMOOTH_RGB       31
#define GD_LOGICOP              33
#define GD_NSCRNS               35
#define GD_NURBS_ORDER          36
#define GD_NBLINKS              37
#define GD_NVERTEX_POLY         39
#define GD_PATSIZE_64           40
#define GD_PNTSMOOTH_CMODE      41
#define GD_PNTSMOOTH_RGB        42
#define GD_PUP_TO_OVERUNDER     43
#define GD_READSOURCE           44
#define GD_READSOURCE_ZBUFFER   48
#define GD_STEREO               50
#define GD_SUBPIXEL_LINE        51
#define GD_SUBPIXEL_PNT         52
#define GD_SUBPIXEL_POLY        53
#define GD_TRIMCURVE_ORDER      54
#define GD_WSYS                 55
#define GD_ZDRAW_GEOM           57
#define GD_ZDRAW_PIXELS         58
#define GD_SCRNTYPE             61
#define GD_TEXTPORT             62
#define GD_NMMAPS               63
#define GD_FRAMEGRABBER         64
#define GD_TIMERHZ              66
#define GD_DBBOX                67
#define GD_AFUNCTION            68
#define GD_ALPHA_OVERUNDER      69
#define GD_BITS_ACBUF           70
#define GD_BITS_ACBUF_HW        71
#define GD_BITS_STENCIL         72
#define GD_CLIPPLANES           73
#define GD_FOGVERTEX            74
#define GD_LIGHTING_TWOSIDE     76
#define GD_POLYMODE             77
#define GD_POLYSMOOTH           78
#define GD_SCRBOX               79
#define GD_TEXTURE              80
#define GD_FOGPIXEL             81
#define GD_TEXTURE_PERSP        82
#define GD_MUXPIPES             83
#define GD_MULTISAMPLE          84
#define GD_TEXTURE_3D           85
#define GD_TEXTURE_LUT          86
#define GD_TEXTURE_SHARP        87
#define GD_TEXTURE_DETAIL       88
#define GD_STEREO_IN_WINDOW     89
#define GD_BLENDCOLOR           90
#define GD_LIGHTING_SPOTLIGHT   91
#define GD_LIGHTING_ATT2        92
#define GD_AFUNCTION_MODES      93
#define GD_TEXTURE_MEMORY_SIZE  94
#define GD_IS_IGLOO             95
#define GD_NOLIMIT              -2
#define GD_WSYS_NONE            0
#define GD_WSYS_4S              1
#define GD_SCRNTYPE_WM          0
#define GD_SCRNTYPE_NOWM        1

/* defines for lcharstr()/lstrwidth() */
#define STR_B   0x000
#define STR_2B  0x001
#define STR_3B  0x010
#define STR_4B  0x011
#define STR_16  0x100
#define STR_32  0x101

/* defines for pixmode() */
#define PM_SHIFT		0
#define PM_EXPAND		1
#define PM_C0			2
#define PM_C1			3
#define PM_ADD24		4
#define PM_SIZE			5
#define PM_OFFSET		6
#define PM_STRIDE		7
#define PM_TTOB			8
#define PM_RTOL			9
#define PM_ZDATA		10
#define PM_INPUT_FORMAT		11
#define PM_INPUT_TYPE		12
#define PM_OUTPUT_FORMAT	13
#define PM_OUTPUT_TYPE		14

#define PM_ABGR			0
#define PM_BGR			1
#define PM_RGBA			2
#define PM_RGB			3
#define PM_LUMINANCE		4
#define PM_LUMINANCEA		5
#define PM_ALPHA		6
#define PM_COLOR_INDEX		7
#define PM_STENCIL_INDEX	8
#define PM_DEPTH		9
#define PM_RED			10
#define PM_GREEN		11
#define PM_BLUE			12

#define PM_BITMAP		0
#define PM_BYTE			1
#define PM_UNSIGNED_BYTE	2
#define PM_SHORT_12		3
#define PM_UNSIGNED_SHORT_12	4
#define PM_SHORT		5
#define PM_UNSIGNED_SHORT	6
#define PM_INT			7
#define PM_UNSIGNED_INT		8
#define PM_FLOAT		9

/* defines for convolve() */
#define CV_OFF			0
#define CV_GENERAL		1
#define CV_SEPARABLE		2

#define CV_REDUCE		1

/* defines for pixelmap() */
#define MAP_I_TO_I		0
#define MAP_I_TO_R		1
#define MAP_I_TO_G		2
#define MAP_I_TO_B		3
#define MAP_I_TO_A		4
#define MAP_R_TO_R		5
#define MAP_G_TO_G		6
#define MAP_B_TO_B		7
#define MAP_A_TO_A		8

/* defines for pixeltransfer() */
#define PT_MAP_COLOR		0
#define PT_RED_SCALE		1
#define PT_RED_BIAS		2
#define PT_GREEN_SCALE		3
#define PT_GREEN_BIAS		4
#define PT_BLUE_SCALE		5
#define PT_BLUE_BIAS		6
#define PT_ALPHA_SCALE		7
#define PT_ALPHA_BIAS		8

#define PT_MAP_DISABLE		0
#define PT_MAP_CLAMP		1
#define PT_MAP_ENABLE		2

/* defines for readcomponent() */
#define RC_ABGR			4
#define RC_ALPHA		0
#define RC_BLUE			1
#define RC_GREEN		2
#define RC_RED			3

/* defines for readsource() */
#define SRC_AUTO		0
#define SRC_FRONT		1
#define SRC_FRONTLEFT	 	1
#define SRC_BACK		2
#define SRC_BACKLEFT	 	2
#define SRC_ZBUFFER		3
#define SRC_PUP			4
#define SRC_OVER		5
#define SRC_UNDER		6
#define SRC_FRAMEGRABBER 	7
#define SRC_FRONTRIGHT	 	8
#define SRC_BACKRIGHT	 	9
#define SRC_ILBUFFER_1		1000

/* defines for curstype() */
#define C16X1			0
#define C16X2			1
#define C32X1			2
#define C32X2			3
#define CCROSS			4

/* special pre-defined tags */
#define STARTTAG    -2
#define ENDTAG      -3


/****************************
|* buttons, valuators etc. *|
****************************/
#define NULLDEV         0
#define BUTOFFSET       1
#define VALOFFSET       256        /* 0x0100 */
#define PSEUDOFFSET     512        /* 0x0200 */
#define BUT2OFFSET      3840        /* 0x0f00 */

#define TIMOFFSET       515
#define XKBDOFFSET      143

#define BUTCOUNT        255
#define VALCOUNT        256

#define TIMCOUNT        4
#define XKBDCOUNT       28

#define USERBUTOFFSET   4096        /* 0x1000 */
#define USERVALOFFSET   12288        /* 0x3000 */
#define USERPSEUDOFFSET 16384        /* 0x4000 */

#define BUT0        1    /* 0+BUTOFFSET,   0, "break" (83-key)    */
#define BUT1        2    /* 1+BUTOFFSET,   1, "setup" (83-key)    */
#define BUT2        3    /* 2+BUTOFFSET,   2, "left ctrl"     */
#define BUT3        4    /* 3+BUTOFFSET,   3, "caps lock"     */
#define BUT4        5    /* 4+BUTOFFSET,   4, "right shift"     */
#define BUT5        6    /* 5+BUTOFFSET,   5, "left shift"     */
#define BUT6        7    /* 6+BUTOFFSET,   6, "escape"        */
#define BUT7        8    /* 7+BUTOFFSET,   7, "1"        */
#define BUT8        9    /* 8+BUTOFFSET,   8, "tab"        */
#define BUT9        10    /* 9+BUTOFFSET,   9, "Q"        */
#define BUT10       11    /* 10+BUTOFFSET,  A, "A"        */
#define BUT11       12    /* 11+BUTOFFSET,  B, "S"        */
#define BUT12       13    /* 12+BUTOFFSET,  C, "no scroll" (83-key) */
#define BUT13       14    /* 13+BUTOFFSET,  D, "2"        */
#define BUT14       15    /* 14+BUTOFFSET,  E, "3"        */
#define BUT15       16    /* 15+BUTOFFSET,  F, "W"        */
#define BUT16       17    /* 16+BUTOFFSET, 10, "E"        */
#define BUT17       18    /* 17+BUTOFFSET, 11, "D"        */
#define BUT18       19    /* 18+BUTOFFSET, 12, "F"        */
#define BUT19       20    /* 19+BUTOFFSET, 13, "Z"        */
#define BUT20       21    /* 20+BUTOFFSET, 14, "X"        */
#define BUT21       22    /* 21+BUTOFFSET, 15, "4"        */
#define BUT22       23    /* 22+BUTOFFSET, 16, "5"        */
#define BUT23       24    /* 23+BUTOFFSET, 17, "R"        */
#define BUT24       25    /* 24+BUTOFFSET, 18, "T"        */
#define BUT25       26    /* 25+BUTOFFSET, 19, "G"        */
#define BUT26       27    /* 26+BUTOFFSET, 1A, "H"        */
#define BUT27       28    /* 27+BUTOFFSET, 1B, "C"        */
#define BUT28       29    /* 28+BUTOFFSET, 1C, "V"        */
#define BUT29       30    /* 29+BUTOFFSET, 1D, "6"        */
#define BUT30       31    /* 30+BUTOFFSET, 1E, "7"        */
#define BUT31       32    /* 31+BUTOFFSET, 1F, "Y"        */
#define BUT32       33    /* 32+BUTOFFSET, 20, "U"        */
#define BUT33       34    /* 33+BUTOFFSET, 21, "J"        */
#define BUT34       35    /* 34+BUTOFFSET, 22, "K"        */
#define BUT35       36    /* 35+BUTOFFSET, 23, "B"        */
#define BUT36       37    /* 36+BUTOFFSET, 24, "N"        */
#define BUT37       38    /* 37+BUTOFFSET, 25, "8"        */
#define BUT38       39    /* 38+BUTOFFSET, 26, "9"        */
#define BUT39       40    /* 39+BUTOFFSET, 27, "I"        */
#define BUT40       41    /* 40+BUTOFFSET, 28, "O"        */
#define BUT41       42    /* 41+BUTOFFSET, 29, "L"        */
#define BUT42       43    /* 42+BUTOFFSET, 2A, ";"        */
#define BUT43       44    /* 43+BUTOFFSET, 2B, "M"        */
#define BUT44       45    /* 44+BUTOFFSET, 2C, ","        */
#define BUT45       46    /* 45+BUTOFFSET, 2D, "0"        */
#define BUT46       47    /* 46+BUTOFFSET, 2E, "-"        */
#define BUT47       48    /* 47+BUTOFFSET, 2F, "P"        */
#define BUT48       49    /* 48+BUTOFFSET, 30, "["        */
#define BUT49       50    /* 49+BUTOFFSET, 31, "'"        */
#define BUT50       51    /* 50+BUTOFFSET, 32, "return"        */
#define BUT51       52    /* 51+BUTOFFSET, 33, "."        */
#define BUT52       53    /* 52+BUTOFFSET, 34, "/"        */
#define BUT53       54    /* 53+BUTOFFSET, 35, "="        */
#define BUT54       55    /* 54+BUTOFFSET, 36, "`"        */
#define BUT55       56    /* 55+BUTOFFSET, 37, "]"        */
#define BUT56       57    /* 56+BUTOFFSET, 38, "\"        */
#define BUT57       58    /* 57+BUTOFFSET, 39, num pad "1"    */
#define BUT58       59    /* 58+BUTOFFSET, 3A, num pad "0"    */
#define BUT59       60    /* 59+BUTOFFSET, 3B, "line feed" (83-key) */
#define BUT60       61    /* 60+BUTOFFSET, 3C, "back space"    */
#define BUT61       62    /* 61+BUTOFFSET, 3D, "delete"        */
#define BUT62       63    /* 62+BUTOFFSET, 3E, num pad "4"    */
#define BUT63       64    /* 63+BUTOFFSET, 3F, num pad "2"    */
#define BUT64       65    /* 64+BUTOFFSET, 40, num pad "3"    */
#define BUT65       66    /* 65+BUTOFFSET, 41, num pad "."    */
#define BUT66       67    /* 66+BUTOFFSET, 42, num pad "7"    */
#define BUT67       68    /* 67+BUTOFFSET, 43, num pad "8"    */
#define BUT68       69    /* 68+BUTOFFSET, 44, num pad "5"    */
#define BUT69       70    /* 69+BUTOFFSET, 45, num pad "6"    */
#define BUT70       71    /* 70+BUTOFFSET, 46, num pad "pf2" (83-key) */
#define BUT71       72    /* 71+BUTOFFSET, 47, num pad "pf1" (83-key) */
#define BUT72       73    /* 72+BUTOFFSET, 48, "left arrow"    */
#define BUT73       74    /* 73+BUTOFFSET, 49, "down arrow"    */
#define BUT74       75    /* 74+BUTOFFSET, 4A, num pad "9"    */
#define BUT75       76    /* 75+BUTOFFSET, 4B, num pad "-"    */
#define BUT76       77    /* 76+BUTOFFSET, 4C, num pad "," (83-key) */
#define BUT77       78    /* 77+BUTOFFSET, 4D, num pad "pf4" (83-key) */
#define BUT78       79    /* 78+BUTOFFSET, 4E, num pad "pf3" (83-key) */
#define BUT79       80    /* 79+BUTOFFSET, 4F, "right arrow"    */
#define BUT80       81    /* 80+BUTOFFSET, 50, "up arrow"        */
#define BUT81       82    /* 81+BUTOFFSET, 51, num pad "enter"    */
#define BUT82       83    /* 82+BUTOFFSET, 52, "space"        */
#define MAXKBDBUT   83    /* BUT82 */

/* Mouse buttons, etc. */
#define BUT100      101    /* 100+BUTOFFSET, Mouse button 1    */
#define BUT101      102    /* 101+BUTOFFSET, Mouse button 2    */
#define BUT102      103    /* 102+BUTOFFSET, Mouse button 3    */
#define BUT103      104    /*          Light Pen Button    */
#define BUT104      105    /*          Bitpad Button 0    */
#define BUT105      106    /*          Bitpad Button 1    */
#define BUT106      107    /*          Bitpad Button 2    */
#define BUT107      108    /*          Bitpad Button 3    */
#define BUT108      109    /*          Light Pen Valid    */
#define BUT109      110    /*          UNUSED        */

/* Button box definitions */
#define BUT110      111    /* 110+BUTOFFSET, Button box switch 0    */
#define BUT111      112    /* 111+BUTOFFSET, Button box switch 1    */
#define BUT112      113    /* 112+BUTOFFSET, Button box switch 2    */
#define BUT113      114    /* 113+BUTOFFSET, Button box switch 3    */
#define BUT114      115    /* 114+BUTOFFSET, Button box switch 4    */
#define BUT115      116    /* 115+BUTOFFSET, Button box switch 5    */
#define BUT116      117    /* 116+BUTOFFSET, Button box switch 6    */
#define BUT117      118    /* 117+BUTOFFSET, Button box switch 7    */
#define BUT118      119    /* 118+BUTOFFSET, Button box switch 8    */
#define BUT119      120    /* 119+BUTOFFSET, Button box switch 9    */
#define BUT120      121    /* 120+BUTOFFSET, Button box switch 10    */
#define BUT121      122    /* 121+BUTOFFSET, Button box switch 11    */
#define BUT122      123    /* 122+BUTOFFSET, Button box switch 12    */
#define BUT123      124    /* 123+BUTOFFSET, Button box switch 13    */
#define BUT124      125    /* 124+BUTOFFSET, Button box switch 14    */
#define BUT125      126    /* 125+BUTOFFSET, Button box switch 15    */
#define BUT126      127    /* 126+BUTOFFSET, Button box switch 16    */
#define BUT127      128    /* 127+BUTOFFSET, Button box switch 17    */
#define BUT128      129    /* 128+BUTOFFSET, Button box switch 18    */
#define BUT129      130    /* 129+BUTOFFSET, Button box switch 19    */
#define BUT130      131    /* 130+BUTOFFSET, Button box switch 20    */
#define BUT131      132    /* 131+BUTOFFSET, Button box switch 21    */
#define BUT132      133    /* 132+BUTOFFSET, Button box switch 22    */
#define BUT133      134    /* 133+BUTOFFSET, Button box switch 23    */
#define BUT134      135    /* 134+BUTOFFSET, Button box switch 24    */
#define BUT135      136    /* 135+BUTOFFSET, Button box switch 25    */
#define BUT136      137    /* 136+BUTOFFSET, Button box switch 26    */
#define BUT137      138    /* 137+BUTOFFSET, Button box switch 27    */
#define BUT138      139    /* 138+BUTOFFSET, Button box switch 28    */
#define BUT139      140    /* 139+BUTOFFSET, Button box switch 29    */
#define BUT140      141    /* 140+BUTOFFSET, Button box switch 30    */
#define BUT141      142    /* 141+BUTOFFSET, Button box switch 31    */

/* Button definitions for the extended keyboard.  Although current keyboards
 * are 101 or 102 keys, there are 112 positions and so that many values are
 * reserved.
 *
 *                    button         button      kbd
 *                    number         offset      hex  key
 *                    ======       ===========   ===  ===== */
#define BUT142      143    /* 142+BUTOFFSET, 53 "left ALT"        */
#define BUT143      144    /* 143+BUTOFFSET, 54 "right ALT"    */
#define BUT144      145    /* 144+BUTOFFSET, 55 "right ctrl"    */
#define BUT145      146    /* 145+BUTOFFSET, 56 "F1"        */
#define BUT146      147    /* 146+BUTOFFSET, 57 "F2"        */
#define BUT147      148    /* 147+BUTOFFSET, 58 "F3"        */
#define BUT148      149    /* 148+BUTOFFSET, 59 "F4"        */
#define BUT149      150    /* 149+BUTOFFSET, 5A "F5"        */
#define BUT150      151    /* 150+BUTOFFSET, 5B "F6"        */
#define BUT151      152    /* 151+BUTOFFSET, 5C "F7"        */
#define BUT152      153    /* 152+BUTOFFSET, 5D "F8"        */
#define BUT153      154    /* 153+BUTOFFSET, 5E "F9"        */
#define BUT154      155    /* 154+BUTOFFSET, 5F "F10"        */
#define BUT155      156     /* 155+BUTOFFSET, 60 "F11"        */
#define BUT156      157    /* 156+BUTOFFSET, 61 "F12"        */
#define BUT157      158    /* 157+BUTOFFSET, 62 "print screen"    */
#define BUT158      159    /* 158+BUTOFFSET, 63 "scroll lock"    */
#define BUT159      160    /* 159+BUTOFFSET, 64 "pause"        */
#define BUT160      161    /* 160+BUTOFFSET, 65 "insert"        */
#define BUT161      162    /* 161+BUTOFFSET, 66 "home"        */
#define BUT162      163    /* 162+BUTOFFSET, 67 "page up"        */
#define BUT163      164    /* 163+BUTOFFSET, 68 "end"        */
#define BUT164      165    /* 164+BUTOFFSET, 69 "page down"    */
#define BUT165      166    /* 165+BUTOFFSET, 6A "num lock"        */
#define BUT166      167    /* 166+BUTOFFSET, 6B num pad "/"    */
#define BUT167      168    /* 167+BUTOFFSET, 6C num pad "*"    */
#define BUT168      169    /* 168+BUTOFFSET, 6D num pad "+"    */

/*
 * BUT169 (=170) through BUT179 (=180) are reserved for the remainder of
 * the 112 key positions.
 */

/*
 * BUT181 through BUT189 are used for the nine buttons of the Space Ball.
 *
 * Codes through 255 inclusive are reserved for future use by SGI.
 */
#define BUT181      182    /* 181+BUTOFFSET, space ball button 0    */
#define BUT182      183    /* 182+BUTOFFSET, space ball button 1    */
#define BUT183      184    /* 183+BUTOFFSET, space ball button 2    */
#define BUT184      185    /* 184+BUTOFFSET, space ball button 3    */
#define BUT185      186    /* 185+BUTOFFSET, space ball button 4    */
#define BUT186      187    /* 186+BUTOFFSET, space ball button 5    */
#define BUT187      188    /* 187+BUTOFFSET, space ball button 6    */
#define BUT188      189    /* 188+BUTOFFSET, space ball button 7    */
#define BUT189      190    /* 189+BUTOFFSET, space ball button 8    */

/* other buttons */

#define MOUSE1      101    /* BUT100 */
#define MOUSE2      102    /* BUT101 */
#define MOUSE3      103    /* BUT102 */
#define LEFTMOUSE   103    /* BUT102 */
#define MIDDLEMOUSE 102    /* BUT101 */
#define RIGHTMOUSE  101    /* BUT100 */
#define LPENBUT     104    /* LIGHT PEN BUTTON */
#define BPAD0       105    /* BITPAD BUTTON 0 */
#define BPAD1       106    /* BITPAD BUTTON 1 */
#define BPAD2       107    /* BITPAD BUTTON 2 */
#define BPAD3       108    /* BITPAD BUTTON 3 */
#define LPENVALID   109    /* LIGHT PEN VALID */

/* button box */

#define SWBASE  111    /* BUT110 */
#define SW0     111    /* SWBASE */
#define SW1     112    /* SWBASE+1 */
#define SW2     113    /* SWBASE+2 */
#define SW3     114    /* SWBASE+3 */
#define SW4     115    /* SWBASE+4 */
#define SW5     116    /* SWBASE+5 */
#define SW6     117    /* SWBASE+6 */
#define SW7     118    /* SWBASE+7 */
#define SW8     119    /* SWBASE+8 */
#define SW9     120    /* SWBASE+9 */
#define SW10    121    /* SWBASE+10 */
#define SW11    122    /* SWBASE+11 */
#define SW12    123    /* SWBASE+12 */
#define SW13    124    /* SWBASE+13 */
#define SW14    125    /* SWBASE+14 */
#define SW15    126    /* SWBASE+15 */
#define SW16    127    /* SWBASE+16 */
#define SW17    128    /* SWBASE+17 */
#define SW18    129    /* SWBASE+18 */
#define SW19    130    /* SWBASE+19 */
#define SW20    131    /* SWBASE+20 */
#define SW21    132    /* SWBASE+21 */
#define SW22    133    /* SWBASE+22 */
#define SW23    134    /* SWBASE+23 */
#define SW24    135    /* SWBASE+24 */
#define SW25    136    /* SWBASE+25 */
#define SW26    137    /* SWBASE+26 */
#define SW27    138    /* SWBASE+27 */
#define SW28    139    /* SWBASE+28 */
#define SW29    140    /* SWBASE+29 */
#define SW30    141    /* SWBASE+30 */
#define SW31    142    /* SWBASE+31 */

/* space ball buttons */

#define SBBASE      182    /* BUT181 */
#define SBPICK      182    /* SBBASE */
#define SBBUT1      183    /* SBBASE+1 */
#define SBBUT2      184    /* SBBASE+2 */
#define SBBUT3      185    /* SBBASE+3 */
#define SBBUT4      186    /* SBBASE+4 */
#define SBBUT5      187    /* SBBASE+5 */
#define SBBUT6      188    /* SBBASE+6 */
#define SBBUT7      189    /* SBBASE+7 */
#define SBBUT8      190    /* SBBASE+8 */

/* standard keyboard */

#define AKEY        11    /* BUT10 */
#define BKEY        36    /* BUT35 */
#define CKEY        28    /* BUT27 */
#define DKEY        18    /* BUT17 */
#define EKEY        17    /* BUT16 */
#define FKEY        19    /* BUT18 */
#define GKEY        26    /* BUT25 */
#define HKEY        27    /* BUT26 */
#define IKEY        40    /* BUT39 */
#define JKEY        34    /* BUT33 */
#define KKEY        35    /* BUT34 */
#define LKEY        42    /* BUT41 */
#define MKEY        44    /* BUT43 */
#define NKEY        37    /* BUT36 */
#define OKEY        41    /* BUT40 */
#define PKEY        48    /* BUT47 */
#define QKEY        10    /* BUT9 */
#define RKEY        24    /* BUT23 */
#define SKEY        12    /* BUT11 */
#define TKEY        25    /* BUT24 */
#define UKEY        33    /* BUT32 */
#define VKEY        29    /* BUT28 */
#define WKEY        16    /* BUT15 */
#define XKEY        21    /* BUT20 */
#define YKEY        32    /* BUT31 */
#define ZKEY        20    /* BUT19 */
#define ZEROKEY     46    /* BUT45 */
#define ONEKEY      8    /* BUT7 */
#define TWOKEY      14    /* BUT13 */
#define THREEKEY    15    /* BUT14 */
#define FOURKEY     22    /* BUT21 */
#define FIVEKEY     23    /* BUT22 */
#define SIXKEY      30    /* BUT29 */
#define SEVENKEY    31    /* BUT30 */
#define EIGHTKEY    38    /* BUT37 */
#define NINEKEY     39    /* BUT38 */
#define BREAKKEY    1    /* BUT0 */
#define SETUPKEY    2    /* BUT1 */
#define CTRLKEY     3    /* BUT2 */
#define LEFTCTRLKEY    CTRLKEY    /* BUT2 */
#define CAPSLOCKKEY     4    /* BUT3 */
#define RIGHTSHIFTKEY   5    /* BUT4 */
#define LEFTSHIFTKEY    6    /* BUT5 */
#define NOSCRLKEY       13    /* BUT12 */
#define ESCKEY          7    /* BUT6 */
#define TABKEY          9    /* BUT8 */
#define RETKEY          51    /* BUT50 */
#define SPACEKEY        83    /* BUT82 */
#define LINEFEEDKEY     60    /* BUT59 */
#define BACKSPACEKEY    61    /* BUT60 */
#define DELKEY          62    /* BUT61 */
#define SEMICOLONKEY    43    /* BUT42 */
#define PERIODKEY       52    /* BUT51 */
#define COMMAKEY        45    /* BUT44 */
#define QUOTEKEY        50    /* BUT49 */
#define ACCENTGRAVEKEY  55    /* BUT54 */
#define MINUSKEY        47    /* BUT46 */
#define VIRGULEKEY      53    /* BUT52 */
#define BACKSLASHKEY    57    /* BUT56 */
#define EQUALKEY        54    /* BUT53 */
#define LEFTBRACKETKEY  49    /* BUT48 */
#define RIGHTBRACKETKEY 56    /* BUT55 */
#define LEFTARROWKEY    73    /* BUT72 */
#define DOWNARROWKEY    74    /* BUT73 */
#define RIGHTARROWKEY   80    /* BUT79 */
#define UPARROWKEY      81    /* BUT80 */
#define PAD0        59    /* BUT58 */
#define PAD1        58    /* BUT57 */
#define PAD2        64    /* BUT63 */
#define PAD3        65    /* BUT64 */
#define PAD4        63    /* BUT62 */
#define PAD5        69    /* BUT68 */
#define PAD6        70    /* BUT69 */
#define PAD7        67    /* BUT66 */
#define PAD8        68    /* BUT67 */
#define PAD9        75    /* BUT74 */
#define PADPF1      72    /* BUT71 */
#define PADPF2      71    /* BUT70 */
#define PADPF3      79    /* BUT78 */
#define PADPF4      78    /* BUT77 */
#define PADPERIOD   66    /* BUT65 */
#define PADMINUS    76    /* BUT75 */
#define PADCOMMA    77    /* BUT76 */
#define PADENTER    82    /* BUT81 */

/* the extended keyboard */

#define LEFTALTKEY      143
#define RIGHTALTKEY     144
#define RIGHTCTRLKEY    145
#define F1KEY           146
#define F2KEY           147
#define F3KEY           148
#define F4KEY           149
#define F5KEY           150
#define F6KEY           151
#define F7KEY           152
#define F8KEY           153
#define F9KEY           154
#define F10KEY          155
#define F11KEY          156
#define F12KEY          157
#define PRINTSCREENKEY  158
#define SCROLLLOCKKEY   159
#define PAUSEKEY        160
#define INSERTKEY       161
#define HOMEKEY         162
#define PAGEUPKEY       163
#define ENDKEY          164
#define PAGEDOWNKEY     165
#define NUMLOCKKEY      166
#define PADVIRGULEKEY   167
#define PADASTERKEY     168
#define PADPLUSKEY      169

#define SGIRESERVED 256
#define DIAL0       257
#define DIAL1       258
#define DIAL2       259
#define DIAL3       260
#define DIAL4       261
#define DIAL5       262
#define DIAL6       263
#define DIAL7       264
#define DIAL8       265
#define MOUSEX      266
#define MOUSEY      267
#define LPENX       268
#define LPENY       269
#define BPADX       270
#define BPADY       271
#define CURSORX     272
#define CURSORY     273
#define GHOSTX      274
#define GHOSTY      275

/* Space Ball valuators */
#define SBTX        276    /* 20+VALOFFSET */
#define SBTY        277    /* 21+VALOFFSET */
#define SBTZ        278    /* 22+VALOFFSET */
#define SBRX        279    /* 23+VALOFFSET */
#define SBRY        280    /* 24+VALOFFSET */
#define SBRZ        281    /* 25+VALOFFSET */
#define SBPERIOD    282    /* 26+VALOFFSET */

/* timers */

#define TIMER0      515    /* 0+TIMOFFSET */
#define TIMER1      516    /* 1+TIMOFFSET */
#define TIMER2      517    /* 2+TIMOFFSET */
#define TIMER3      518    /* 3+TIMOFFSET */

/* misc devices */

#define KEYBD           513    /* keyboard */
#define RAWKEYBD        514    /* raw keyboard for keyboard manager */
#define VALMARK         523    /* valuator mark */
#define REDRAW          528    /* used by port manager to signal redraws */
#define INPUTCHANGE     534    /* input connected or disconnected */
#define QFULL           535    /* queue was filled */
#define QREADERROR      538    /* qread error */
#define WINFREEZE       539    /* user wants process in this win to shut up */
#define WINTHAW         540    /* user wants process in this win to go again */
#define REDRAWICONIC    541    /* used to signal redraw as an icon */
#define WINQUIT         542    /* signal from user that app is to go away */
#define DEPTHCHANGE     543    /* window stacking order changed */
#define WINSHUT         546    /* window shutdown */
#define DRAWOVERLAY     547    /* overlay planes have been damaged */
#define VIDEO           548    /* video notification */

#define MENUBUTTON      RIGHTMOUSE /* the button used by dopup */


#define ISBUTTON(b)    ((((b) >= BUTOFFSET) && ((b) < VALOFFSET)) || \
             (((b) >= BUT2OFFSET) && ((b) < USERVALOFFSET)))
#define ISVALUATOR(b)    ((((b) >= VALOFFSET) && ((b) < PSEUDOFFSET)) || \
             (((b) >= USERVALOFFSET) && ((b) < USERPSEUDOFFSET)))

#define ISTIMER(t)    (((t) >= TIMOFFSET) && ((t) < (TIMCOUNT+TIMOFFSET)))
#define ISDIAL(t)    (((t) >= DIAL0) && ((t) <= DIAL8))
#define ISLPEN(t)    (((t) == LPENX) || ((t) == LPENY))
#define ISLPENBUT(t)    ((t) == LPENBUT)
#define ISBPADBUT(t)    (((t) >= BPAD0) && ((t) <= BPAD3))
#define ISSW(t)        (((t) >= SW0) && ((t) <= SW31))
#define ISSTDKEYBD(t)    (((t) >= BUT0) && ((t) <= MAXKBDBUT))
#define ISXKEYBD(t)    (((t) >= XKBDOFFSET) && ((t) < (XKBDCOUNT+XKBDOFFSET)))
#define ISKEYBD(t)    (ISSTDKEYBD(t) || ISXKEYBD(t))
#define ISSBALL(t)    (((t) >= SBTX) && ((t) <= SBPERIOD))
#define ISSBALLBUT(t)    (((t) >= SBPICK) && ((t) <= SBBUT8))
#define ISVIDEO(t)    ((t) == VIDEO)



#if 1
#   define XMAXSCREEN      1279
#   define YMAXSCREEN      1023
#else
#   define XMAXSCREEN      1023
#   define YMAXSCREEN      767
#endif

/* popup colors */
#define PUP_CLEAR       0
#define PUP_COLOR       1
#define PUP_BLACK       2
#define PUP_WHITE       3
#define PUP_CURSOR      PUP_COLOR

/* defines for setpup */
#define PUP_NONE        0
#define PUP_GREY        0x1
#define PUP_BOX         0x2
#define PUP_CHECK       0x4

/* defines for drawmode and mswapbuffers*/
#define NORMALDRAW      0x010
#define PUPDRAW         0x020
#define OVERDRAW        0x040
#define UNDERDRAW       0x080
#define CURSORDRAW      0x100
#define DUALDRAW        0x200    /* obsolete skywriter only */
#define GANGDRAW        0x200    /* not available on skywriter */
#define HYPER0          0x10000
#define HYPER1          0x20000
#define HYPER2          0x40000

/* defines for glcompat */
#define GLC_OLDPOLYGON          0
#define GLC_ZRANGEMAP           1
#define GLC_MQUEUERATE          2
#define GLC_SOFTATTACH          3
#define GLC_MANAGEBG            4
#define GLC_SLOWMAPCOLORS       5
#define GLC_INPUTCHANGEBUG      6
#define GLC_NOBORDERBUG         7
#define GLC_SET_VSYNC           8
#define GLC_GET_VSYNC           9
#define GLC_VSYNC_SLEEP         10
#define GLC_FORCECIMAP          11
#define GLC_TABLETSCALE         12
#define GLC_NODESTALPHA         13
#define GLC_COMPATRATE          15


/* defines for getgconfig */
#define GC_BITS_CMODE       0
#define GC_BITS_RED         1
#define GC_BITS_GREEN       2
#define GC_BITS_BLUE        3
#define GC_BITS_ALPHA       4
#define GC_BITS_ZBUFFER     5
#define GC_ZMIN             6
#define GC_ZMAX             7
#define GC_BITS_STENCIL     8
#define GC_BITS_ACBUF       9
#define GC_MS_SAMPLES       10
#define GC_BITS_MS_ZBUFFER  11
#define GC_MS_ZMIN          12
#define GC_MS_ZMAX          13
#define GC_BITS_MS_STENCIL  14
#define GC_STEREO           15
#define GC_DOUBLE           16


/* Pre-OpenGL SGI stuff to attach GL contexts to X11 visuals */
#define	GLWS_NOERROR    0
#define GLWS_NOCONTEXT  -1
#define GLWS_NODISPLAY  -2
#define GLWS_NOWINDOW   -3
#define GLWS_NOGRAPHICS -4
#define GLWS_NOTTOP     -5
#define GLWS_NOVISUAL   -6
#define GLWS_BUFSIZE    -7
#define GLWS_BADWINDOW  -8
#define GLWS_ALREADYBOUND -100
#define GLWS_BINDFAILED -101
#define GLWS_SETFAILED  -102

#define GLXC_NORMAL     0x1000
#define GLXC_OVERLAY    0x2000
#define GLXC_POPUP      0x3000
#define GLXC_UNDERLAY   0x4000

#define GLXC_RGB        0x0001
#define GLXC_DOUBLE     0x0002
#define GLXC_ZSIZE      0x0003
#define GLXC_BUFSIZE    0x0004
#define GLXC_STENSIZE   0x0005
#define GLXC_ACSIZE     0x0006
#define GLXC_VISUAL     0x0007
#define GLXC_COLORMAP   0x0008
#define GLXC_WINDOW     0x0009
#define GLXC_MSSAMPLE   0x000a
#define GLXC_MSZSIZE    0x000b
#define GLXC_MSSSIZE    0x000c
#define GLXC_STEREOBUF  0x000d
#define GLXC_RGBSIZE    0x000e

#define GLX_NONE        0
#define GLX_NOCONFIG    0x7fff
typedef struct _GLXconfig {
    int   buffer;
    int   mode;
    int   arg;
} GLXconfig;

GLXconfig *
GLXgetconfig (void *disp, long screen, GLXconfig *desc);

long
GLXlink (void *disp, GLXconfig *cfg);

long
GLXunlink (void *disp, unsigned long win);

long
GLXwinset (void *disp, unsigned long win);


/***************************************
    device-related functions (device.c)
****************************************/
Boolean
getbutton (Device dev);

long
getvaluator (Device dev);

void
setvaluator (Device v, short init, short vmin, short vmax);

void
noise (Device v, short delta);

void
getdev (long n, Device devs[], short vals[]);

void
qdevice (Device dev);

void
unqdevice (Device dev);

Boolean
isqueued (Device dev);

void
qenter (Device dev, short val);

long
qtest (void);

void
qreset (void);

long
blkqread (short *data, short n);

long
qread (short *data);

void
tie (Device b, Device v1, Device v2);

void
attachcursor (Device vx, Device vy);

long
qgetfd (void);

void
curson (void);

void
cursoff (void);

void
curstype (long type);

void
defcursor (short n, unsigned short *curs);

void
setcursor (short n, Colorindex color, Colorindex wtm);

void
curorigin (short n, short xo, short yo);

void
getcursor (short *n, Colorindex *color, Colorindex *wtm, Boolean *vis);

void
RGBcursor (short index, short red, short green, short blue, short redm, short greenm, short bluem);

void
gRGBcursor (short *index, short *red, short *green, short *blue, short *redm, short *greenm, short *bluem, Boolean *vis);


/***************************************
    window-related functions (window.c)
****************************************/
void
prefsize (int width, int height);

void
maxsize (long width, long height);

void
minsize (long width, long height);

void
prefposition (int x0, int x1, int y0, int y1);

long
winopen (String windowTitle);

long
swinopen (long parent);

void
winclose (long gwid);

void
winconstraints (void);

void
winposition (int x0, int x1, int y0, int y1);

long
winattach (void);

long
winget (void);

void
winset (long gwid);

void
winpush (void);

void
winpop (void);

void
winmove (long orgx, long orgy);

void
wintitle (String name);

long
winat (void);

long
gl_winat (short x, short y);

void
keepaspect (int x, int y);

void
stepunit (int x, int y);

void
getsize (long *x, long *y);

void
getorigin (long *x, long *y);

long
windepth (long gwid);

void
gconfig (void);

long
getgconfig (long buffer);

void
fullscrn (void);

void
endfullscrn (void);

int
getdrawmode (void);

void
drawmode (int mode);

void
getport (String name);

void
noborder (void);

void
noport (void);

void
imakebackground (void);


/*******************************************
    buffer-related functions (buffer.c)
********************************************/
void
clear (void);

void
cmode (void);

void
RGBmode (void);

void
RGBsize (long planes);

long
getplanes (void);

long
getdisplaymode (void);

void
lsetdepth (long near_, long far_);

void
setdepth (Screencoord near_, Screencoord far_);

void
getdepth (Screencoord *near_, Screencoord *far_);

void
singlebuffer (void);

void
doublebuffer (void);

void
monobuffer (void);

void
stereobuffer (void);

void
swapbuffers (void);

void
mswapbuffers (long fbuf);

void
backbuffer (Boolean enable);

void
frontbuffer (Boolean enable);

void
leftbuffer (Boolean enable);

void
rightbuffer (Boolean enable);

long
getbuffer (void);

void
backface (Boolean enable);

long
getbackface (void);

void
frontface (Boolean enable);

void
zbuffer (Boolean enable);

Boolean
getzbuffer (void);

void
zfunction (long func);

void
zwritemask (unsigned long);

void
zclear (void);

void
czclear (unsigned long cval, long zval);

void
zbsize (long planes);

void
zdraw (Boolean enable);

void
sclear (unsigned long sval);

void
stencil (long enable, unsigned long ref, long func, unsigned long mask, long fail, long pass, long zpass);

void
swritemask (unsigned long mask);

void
stensize (long planes);

void
acbuf (long op, float value);

void
acsize (long planes);


/*******************************************
    matrix-related functions (matrix.c)
********************************************/
void
mmode (short mode);

long
getmmode (void);

void
loadmatrix (Matrix m);

void
getmatrix (Matrix m);

void
multmatrix (Matrix m);

void
pushmatrix (void);

void
popmatrix (void);

void
rot (float amount, char angle);

void
rotate (Angle amount, char angle);

void
translate (Coord x, Coord y, Coord z);

void
scale (float x, float y, float z);

void
polarview (Coord dist, Angle azim, Angle inc, Angle twist);

void
clipplane (long index, long mode, float *params);

void
ortho (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_);

void
ortho2 (Coord left, Coord right, Coord bottom, Coord top);

void
window (Coord left, Coord right, Coord bottom, Coord top, Coord near_, Coord far_);

void
perspective (Angle fovy, float aspect, Coord near_, Coord far_);

void
viewport (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);

void
getviewport (Screencoord *left, Screencoord *right, Screencoord *bottom, Screencoord *top);

void
reshapeviewport (void);

void
pushviewport (void);

void
popviewport (void);

void
lookat (Coord vx, Coord vy, Coord vz, Coord px, Coord py, Coord pz, Angle twist);

void
mapw (Object obj, Screencoord sx, Screencoord sy, Coord *wx1, Coord *wy1, Coord *wz1,
      Coord *wx2, Coord *wy2, Coord *wz2);

void
mapw2 (Object obj, Screencoord sx, Screencoord sy, Coord *wx, Coord *wy);

void
gl_invertmat (Matrix m, Matrix out);

void
gl_invert4d (Matrix out, Matrix m);

void
gl_sincos (int a, float *b, float *c);

void
screenspace (void);

void
scrmask (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);

void
getscrmask (Screencoord *left, Screencoord *right, Screencoord *bottom, Screencoord *top);

void
pick (short buffer[], long numnam);

long
endpick (short buffer[]);

void
picksize (short deltax, short deltay);

void
gselect (short buffer[], long numnam);

long
endselect (short buffer[]);

void
initnames (void);

void
loadname (short name);

void
pushname (short name);

void
popname (void);


/***************************************
    color-related functions (color.c)
****************************************/
void
color (Colorindex c);

void
colorf (float c);

long
getcolor (void);

void
mapcolor (Colorindex i, short r, short g, short b);

void
getmcolor (Colorindex i, short *r, short *g, short *b);

void
writemask (Colorindex wtm);

long
getwritemask (void);

void
setshade (Colorindex shade);

long
getshade (void);

void
cpack (unsigned long color);

void
wmpack (unsigned long color);

void
RGBcolor (short r, short g, short b);

void
gRGBcolor (short *r, short *g, short *b);

void
RGBwritemask (short redm, short greenm, short bluem);

void
gRGBmask (short *redm, short *greenm, short *bluem);

void
afunction (long ref, long func);

void
blendcolor (float r, float g, float b, float a);

void
blendfunction (long sfactor, long dfactor);

void
dither (long mode);

void
logicop (long opcode);

void
c3f (float cv[3]);

void
c3i (int cv[3]);

void
c3s (short cv[3]);

void
c4f (float cv[4]);

void
c4i (int cv[4]);

void
c4s (short cv[4]);

void
onemap (void);

void
multimap (void);

Boolean
getcmmode (void);

void
setmap (short mapnum);

long
getmap (void);


/*******************************************
    vertex/polygon-related functions (vertex.c)
********************************************/
void
v2f (float vector[2]);

void
v2d (double vector[2]);

void
v2i (int vector[2]);

void
v2s (short vector[2]);

void
v3f (float vector[3]);

void
v3d (double vector[3]);

void
v3i (int vector[3]);

void
v3s (short vector[3]);

void
v4f (float vector[4]);

void
v4d (double vector[4]);

void
v4i (int vector[4]);

void
v4s (short vector[4]);

void
bgntmesh (void);

void
endtmesh (void);

void
swaptmesh (void);

void
bgnclosedline (void);

void
endclosedline (void);

void
bgncurve (void);

void
endcurve (void);

void 
nurbscurve (long knotCount, const double *knotList, long offset, const double *ctlArray,
            long order, long type);

void
bgnsurface (void);

void
endsurface (void);

void
nurbssurface (long scount, const double *sknot, long tcount, const double *tknot, long soffset,
              long toffset, const double *ctlArray, long sorder, long torder, long type);

void
bgntrim (void);

void
endtrim (void);

void
pwlcurve (long n, double *dataArray, long byteSize, long type);

void
setnurbsproperty (long property, float value);

void
getnurbsproperty (long property, float *value);

void
bgnline (void);

void
endline (void);

void
bgnpoint (void);

void
endpoint (void);

void
bgnpolygon (void);

void
endpolygon (void);

void
bgnqstrip (void);

void
endqstrip (void);

void
nmode (long mode);

void
n3f (float *vector);

void
normal (Coord *narray);


/***************************************
    graphics position-related functions (grpos.c)
****************************************/
void
move (Coord x, Coord y, Coord z);

void
movei (Icoord x, Icoord y, Icoord z);

void
moves (Scoord x, Scoord y, Scoord z);

void
move2 (Coord x, Coord y);

void
move2i (Icoord x, Icoord y);

void
move2s (Scoord x, Scoord y);

void
getgpos (Coord *fx, Coord *fy, Coord *fz, Coord *fw);

void
rmv (Coord dx, Coord dy, Coord dz);

void
rmvi (Icoord dx, Icoord dy, Icoord dz);

void
rmvs (Scoord dx, Scoord dy, Scoord dz);

void
rmv2 (Coord dx, Coord dy);

void
rmv2i (Icoord dx, Icoord dy);

void
rmv2s (Scoord dx, Scoord dy);

void
rpmv (Coord dx, Coord dy, Coord dz);

void
rpmvi (Icoord dx, Icoord dy, Icoord dz);

void
rpmvs (Scoord dx, Scoord dy, Scoord dz);

void
rpmv2 (Coord dx, Coord dy);

void
rpmv2i (Icoord dx, Icoord dy);

void
rpmv2s (Scoord dx, Scoord dy);

void
draw (Coord x, Coord y, Coord z);

void
drawi (Icoord x, Icoord y, Icoord z);

void
draws (Scoord x, Scoord y, Scoord z);

void
draw2 (Coord x, Coord y);

void
draw2i (Icoord x, Icoord y);

void
draw2s (Scoord x, Scoord y);

void
rdr (Coord dx, Coord dy, Coord dz);

void
rdri (Icoord dx, Icoord dy, Icoord dz);

void
rdrs (Scoord dx, Scoord dy, Scoord dz);

void
rdr2 (Coord dx, Coord dy);

void
rdr2i (Icoord dx, Icoord dy);

void
rdr2s (Scoord dx, Scoord dy);

void
rpdr (Coord dx, Coord dy, Coord dz);

void
rpdri (Icoord dx, Icoord dy, Icoord dz);

void
rpdrs (Scoord dx, Scoord dy, Scoord dz);

void
rpdr2 (Coord dx, Coord dy);

void
rpdr2i (Icoord dx, Icoord dy);

void
rpdr2s (Scoord dx, Scoord dy);

void
polf (long n, const Coord parray[][3]);

void
polfi (long n, const Icoord parray[][3]);

void
polfs (long n, const Scoord parray[][3]);

void
polf2 (long n, const Coord parray[][2]);

void
polf2i (long n, const Icoord parray[][2]);

void
polf2s (long n, const Scoord parray[][2]);

void
poly (long n, const Coord parray[][3]);

void
polyi (long n, const Icoord parray[][3]);

void
polys (long n, const Scoord parray[][3]);

void
poly2 (long n, const Coord parray[][2]);

void
poly2i (long n, const Icoord parray[][2]);

void
poly2s (long n, const Scoord parray[][2]);

void
polymode (long mode);

void
polysmooth (long mode);

void
pmv (Coord x, Coord y, Coord z);

void
pmvi (Icoord x, Icoord y, Icoord z);

void
pmvs (Scoord x, Scoord y, Scoord z);

void
pmv2 (Coord x, Coord y);

void
pmv2i (Icoord x, Icoord y);

void
pmv2s (Scoord x, Scoord y);

void
pdr (Coord x, Coord y, Coord z);

void
pdri (Icoord x, Icoord y, Icoord z);

void
pdrs (Scoord x, Scoord y, Scoord z);

void
pdr2 (Coord x, Coord y);

void
pdr2i (Icoord x, Icoord y);

void
pdr2s (Scoord x, Scoord y);

void
pclos (void);

void
spclos (void);

void
pnt (Coord x, Coord y, Coord z);

void
pnti (Icoord x, Icoord y, Icoord z);

void
pnts (Scoord x, Scoord y, Scoord z);

void
pnt2 (Coord x, Coord y);

void
pnt2i (Icoord x, Icoord y);

void
pnt2s (Scoord x, Scoord y);

void
pntsizef (float n);

void
pntsize (short n);

void
pntsmooth (unsigned long mode);

void
splf (long n, Coord parray[][3], Colorindex iarray[]);

void
splfi (long n, Icoord parray[][3], Colorindex iarray[]);

void
splfs (long n, Scoord parray[][3], Colorindex iarray[]);

void
splf2 (long n, Coord parray[][2], Colorindex iarray[]);

void
splf2i (long n, Icoord parray[][2], Colorindex iarray[]);

void
splf2s (long n, Scoord parray[][2], Colorindex iarray[]);

void
displacepolygon (float scalefactor);

void
linesmooth (unsigned long mode);

void
smoothline (long mode);

void
linewidth (short width);

void
linewidthf (float width);

long
getlwidth (void);

void
deflinestyle (short n, Linestyle ls);

void
setlinestyle (short n);

long
getlstyle (void);

void
lsrepeat (long factor);

long
getlsrepeat (void);

void
defpattern (short n, short size, unsigned short mask[]);

void
setpattern (short index);

long
getpattern (void);


/***************************************
    draw-related functions (draw.c)
****************************************/
void
rect (Coord a, Coord b, Coord c, Coord d);

void
recti (Icoord a, Icoord b, Icoord c, Icoord d);

void
rects (Scoord a, Scoord b, Scoord c, Scoord d);

void
rectf (Coord a, Coord b, Coord c, Coord d);

void
rectfi (Icoord a, Icoord b, Icoord c, Icoord d);

void
rectfs (Scoord a, Scoord b, Scoord c, Scoord d);

void
sbox (Coord a, Coord b, Coord c, Coord d);

void
sboxi (Icoord a, Icoord b, Icoord c, Icoord d);

void
sboxs (Scoord a, Scoord b, Scoord c, Scoord d);

void
sboxf (Coord a, Coord b, Coord c, Coord d);

void
sboxfi (Icoord a, Icoord b, Icoord c, Icoord d);

void
sboxfs (Scoord a, Scoord b, Scoord c, Scoord d);

void
arc (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle);

void
arci (Icoord x, Icoord y, Icoord radius, Angle startAngle, Angle endAngle);

void
arcs (Scoord x, Scoord y, Scoord radius, Angle startAngle, Angle endAngle);

void
arcf (Coord x, Coord y, Coord radius, Angle startAngle, Angle endAngle);

void
arcfi (Icoord x, Icoord y, Icoord radius, Angle startAngle, Angle endAngle);

void
arcfs (Scoord x, Scoord y, Scoord radius, Angle startAngle, Angle endAngle);

void
circ (Coord x, Coord y, Coord radius);

void
circi (Icoord x, Icoord y, Icoord radius);

void
circs (Scoord x, Scoord y, Scoord radius);

void
circf (Coord x, Coord y, Coord radius);

void
circfi (Icoord x, Icoord y, Icoord radius);

void
circfs (Scoord x, Scoord y, Scoord radius);

void
curveit (short niter);

void
crv (Coord points[4][3]);

void
crvn (long n, Coord points[][3]);

void
rcrv (Coord points[4][4]);

void
rcrvn (long n, Coord points[][4]);

void
curveprecision (short n);

void
curvebasis (short id);

void
patch (Matrix geomx, Matrix geomy, Matrix geomz);

void
rpatch (Matrix geomx, Matrix geomy, Matrix geomz, Matrix geomw);

void
patchcurves (short nu, short nv);

void
patchprecision (short nu, short nv);

void
patchbasis (short uid, short vid);

void
defbasis (short id, Matrix mat);


/***************************************
    light-related functions (light.c)
****************************************/
void
lmdef (short deftype, short index, short np, float *props);

void
lmbind (short target, short index);

void
lmcolor (long mode);

void
fogvertex (long mode, float *params);

void
depthcue (Boolean enable);

Boolean
getdcm (void);

void
lshaderange (Colorindex lowin, Colorindex highin, long znear, long zfar);

void
shaderange (Colorindex lowin, Colorindex highin, Screencoord z1, Screencoord z2);

void
lRGBrange (short rmin, short gmin, short bmin, short rmax, short gmax, short bmax, long znear, long zfar);

void
RGBrange (short rmin, short bmin, short gmin, short rmax, short gmax, short bmax, Screencoord znear, Screencoord zfar);

void
shademodel (int mode);

long
getsm (void);


/***************************************
    texture-related functions (texture.c)
****************************************/
void
tevdef (long index, long np, float *props);

void
tevbind (long target, long index);

void
texdef2d (long index, long nc, long width, long height, unsigned long *image, long np, float *props);

void
texdef3d (long index, long nc, long width, long height, long depth, unsigned long *image, long np, float *props);

void
texbind (long target, long index);

void
texgen (long coord, long mode, float *params);

void
subtexload (long target, long id, float s0, float s1, float t0, float t1, long numwords, unsigned long * texture, unsigned long flags);

void
fbsubtexload (long x, long y, long target, long id, float s0, float s1, float t0, float t1, unsigned long flags);

long
istexloaded (long target, long id);

void
t2s (short vector[2]);

void
t2i (long vector[2]);

void
t2f (float vector[2]);

void
t2d (double vector[2]);

void
t3s (short vector[3]);

void
t3i (long vector[3]);

void
t3f (float vector[3]);

void
t3d (double vector[3]);

void
t4s (short vector[4]);

void
t4i (long vector[4]);

void
t4f (float vector[4]);

void
t4d (double vector[4]);


/***************************************
    pixel-related functions (pixel.c)
****************************************/
void
getcpos (short *ix, short *iy);

void
cmov (Coord x, Coord y, Coord z);

void
cmovi (Icoord x, Icoord y, Icoord z);

void
cmovs (Scoord x, Scoord y, Scoord z);

void
cmov2 (Coord x, Coord y);

void
cmov2i (Icoord x, Icoord y);

void
cmov2s (Scoord x, Scoord y);

void
charstr (String str);

void
lcharstr (long type, void *str);

long
strwidth (String str);

long
lstrwidth (long type, String str);

long
getheight (void);

void
font (short fntnum);

long
getfont (void);

long
getdescender (void);

void
defrasterfont (short n, short ht, short nc, Fontchar chars[], short nr, unsigned short raster[]);

void
deflfont (short n, long nc, Lfontchar chars[], long nr, unsigned short raster[]);

void
pixmode(long mode, long value);

void
pixmodef(long mode, float value);

void
rectzoom(float xfactor, float yfactor);

void
convolve (long op, long border, long xksize, long yksize, float *kernel, float bias);

void
pixelmap (int map, int size, unsigned short *values);

void
pixeltransfer (int mode, float value);

void
readsource (long src);

void 
writepixels(short n, Colorindex colors[]);

void
rectwrite(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, Colorindex parray[]);

long
readRGB(short n, RGBvalue r[], RGBvalue g[], RGBvalue b[]);

void
writeRGB(short n, RGBvalue r[], RGBvalue g[], RGBvalue b[]);

long
lrectread(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, unsigned long parray[]);

void
lrectwrite(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, unsigned long parray[]);

void
rectcopy(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, Screencoord newx, Screencoord newy);

void
readcomponent (long mode);

long
readdisplay(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, unsigned long parray[], unsigned long hints);

void
minmax (unsigned long op, unsigned long val);

void
getminmax (float *minmax);

void
hgram (unsigned long op, unsigned long val);

void
gethgram (unsigned long hist[16384]);


/***************************************
    menu-related functions (menu.c)
****************************************/
long
defpup (String str, ...);

long
newpup (void);

void
addtopup (long pup, String str, ...);

long
dopup (long pup);

void
freepup (long pup);

void
setpup (long pup, long entry, unsigned long mode);

void
pupmode (void);

void
endpupmode (void);

void
pupcolor (long clr);


/***************************************
    object-related functions (object.c)
****************************************/
Object
genobj (void);

void
makeobj (Object obj);

void
closeobj (void);

void
callobj (Object obj);

void
delobj (Object obj);

Boolean
isobj (Object obj);

void
editobj (Object obj);

Object
getopenobj (void);

Tag
gentag (void);

void
maketag (Tag tag);

void
deltag (Tag tag);

Boolean
istag (Tag tag);

void
newtag (Tag newtg, Tag oldtg, Offset offset);

void
objinsert (Tag t);

void
objdelete (Tag tag1, Tag tag2);

void
objreplace (Tag t);

void
chunksize (long chunk);

void
compactify (Object obj);

void
callfunc (void (*fctn)(), long nargs, ...);


/***************************************
    misc functions
****************************************/
void
swapinterval (short interval);

void
pushattributes (void);

void
popattributes (void);

long
getgdesc (long inquiry);

void
glcompat (long mode, long value);

void
greset (void);

void
gbegin (void);

void
ginit (void);

void
gexit (void);

void
finish (void);

void
gflush (void);

long
gversion (String v);

void
subpixel (Boolean enable);


/***************************************
    AIX extensions
****************************************/
void
mapcolors (short start_idx, short end_idx, short r[], short g[], short b[]);

void
getmcolors (short start_idx, short end_idx, short *r, short *g, short *b);

void
loadXfont (int id_num, char *name);

int
getfonttype (void);

void
getfontencoding (char *enc);


/***************************************
    unimplemented functions
****************************************/
/* may get support in a later version */
long
ilbuffer (unsigned long buffers);

void
ildraw (unsigned long buffer);

void
multisample (Boolean bool);

Boolean
getmultisample (void);

void
mssample (long mode);

void
msalpha (long mode);

void
msmask (float mask, Boolean inverse);

void
mspattern (long pattern);

void
mssize (long samples, long zsize, long ssize);

void
feedback (float buffer[], long size);

void
endfeedback (float buffer []);

void
passthrough (short token);

void
xfpt(Coord x, Coord y, Coord z);

void
xfpti(Icoord x, Icoord y, Icoord z);

void
xfpts(Scoord x, Scoord y, Scoord z);

void
xfpt2(Coord x, Coord y);

void
xfpt2i(Icoord x, Icoord y);

void
xfpt2s(Scoord x, Scoord y);

void
xfpt4(Coord x, Coord y, Coord z, Coord w);

void
xfpt4i(Icoord x, Icoord y, Icoord z, Icoord w);

void
xfpt4s(Scoord x, Scoord y, Scoord z, Scoord w);

long
dglopen (String srvname, long type);

void
dglclose (long srvid);

void
overlay (long planes);

void
underlay (long planes);

void
fudge (long xfudge, long yfudge);

void
icontitle (String name);

void
iconsize (long x, long y);

void
qcontrol (long cmd, long icnt, short idata[], long ocnt, short odata[]);

/* unsupport by IGL (no real colormaps) */
long 
readpixels(short n, Colorindex colors[]);

long
rectread(Screencoord x1, Screencoord y1, Screencoord x2, Screencoord y2, Colorindex parray[]);

void
blink (short rate, Colorindex i, short red, short green, short blue);

void
cyclemap (short duration, short map, short nxtmap);

/* unsupported in OpenGL */
void
gsync (void);

void
scrsubdivide (long mode, float param[]);

void
gammaramp (short r[256], short g[256], short b[256]);

void
zsource (long src);

void
scrbox (long arg);

void
getscrbox (long *left, long *right, long *bottom, long *top);

long
gethitcode (void);

void
clearhitcode (void);

void
bbox2 (Screencoord xmin, Screencoord ymin, Coord x1, Coord y1, Coord x2, Coord y2);

void
bbox2i (Screencoord xmin, Screencoord ymin, Icoord x1, Icoord y1, Icoord x2, Icoord y2);

void
bbox2s (Screencoord xmin, Screencoord ymin, Scoord x1, Scoord y1, Scoord x2, Scoord y2);

void
tlutdef (long index, long nc, long len, unsigned long *table, long np, float *props);

void
tlutbind (long target, long index);

void
lsbackup (Boolean b);

Boolean
getlsbackup (void);

void
resetls (Boolean b);

Boolean
getresetls (void);

void
concave (Boolean enable);

/* unsupported archaic stuff (IRIX 3, Iris3000 and earlier) */
long
getwscrn (void);

void
scrnselect (long gsnr);

void
scrnattach (long gsnr);

void
blankscreen (Boolean b);

void
blanktime (long count);

void
setvideo (long reg, long val);

long
getvideo (long reg);

void
videocmd (long cmd);

void
setmonitor (short mtype);

long
getmonitor (void);

long
getothermonitor (void);

void
clkon (void);

void
clkoff (void);

void
lampon (Byte lamps);

void
lampoff (Byte lamps);

void
ringbell (void);

void
setbell (Byte durat);

void
devport (Device dev, long port);

Boolean
setfastcom (void);

Boolean
setslowcom (void);

void
dbtext (String str);

void
setdblights (unsigned long mask);

void
foreground (void);

Boolean
ismex (void);

void
textinit (void);

void
textport (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);

void
gettp (Screencoord *left, Screencoord *right, Screencoord *bottom, Screencoord *top);

void
textcolor (Colorindex tcolor);

void
textwritemask (Colorindex tmask);

void
pagecolor (Colorindex pcolor);

void
pagewritemask (Colorindex pmask);

void
tpon (void);

void
tpoff (void);

/* additional symbols in libgl.so whose function and protoype is unkown:
 * icallfunc, idefpup, gewrite
 */

#ifdef __cplusplus
}
#endif

#endif  /* IGL_H */

