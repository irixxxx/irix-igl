/*
 * ifm.h
 *
 * SGI font manager interface definitions
 */

#ifndef IFM_H
#define IFM_H


#if PLATFORM_X11

#ifdef __cplusplus
extern "C" {
#endif


typedef long *fmfonthandle;

typedef struct fmfontinfo {
    long printermatched;
    long reserved0;
    double matrix00;
    double matrix01;
    double matrix10;
    double matrix11;
    long type;
    long encoding;
    long fixed_width;
    long xorig;
    long yorig;
    long xsize;
    long ysize;
    long height;
    long nglyphs;
    long bitsdeep;
    long width;
    long resolution;
    long weight;
    long firstcol;
    long lastcol;
    long firstrow;
    long lastrow;
    long padding[26];
} fmfontinfo;

#define FM_MAXFONTS 64

/***************************************
    misc functions
****************************************/
void
fminitpagematrix ();

void
fmgetpagematrix (double m[3][2]);

void
fmsetpagematrix (double m[3][2]);

void
fmconcatpagematrix (double m[3][2]);

void
fmscalepagematrix (double x);

void
fmrotatepagematrix (double angle);

void
fminit (void);

void
fmenumerate (void (*cb)(char *));

void
fmsetpath (const char *path);

char *
fmfontpath (void);

fmfonthandle
fmfindfont (const char *face);

fmfonthandle
fmscalefont (fmfonthandle fh, double scale);

void
fmfreefont (fmfonthandle fh);

void
fmsetfont (fmfonthandle fh);

int
fmgetfontname (fmfonthandle fh, int len, char *str);

long
fmgetchrwidth (fmfonthandle fh, const unsigned char chr);

long
fmgetstrwidth (fmfonthandle fh, const char *str);

int
fmprstr (const char *str);

long
fmoutchar (fmfonthandle fh, unsigned int chr);

int
fmfprstr (fmfonthandle fh, unsigned char *str, short len, int x, int y);

int
fmgetfontinfo (fmfonthandle fh, fmfontinfo *info);



#ifdef __cplusplus
}
#endif

#endif  /* X11 */

#endif  /* IFM_H */


