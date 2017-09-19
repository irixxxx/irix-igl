/*
 * fontmgr.c
 *
 * SGI IRIS font manager replication
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

#include "igl.h"
#include "iglcmn.h"
#include "ifm.h"


#if PLATFORM_X11
#if 0
____________________________ font manager functions ____________________________
#endif


/* X11 names for selected fontmanager font names. */
/* TODO: complete the list, and X names should come from settings to allow mapping! */
static struct fmfontdef {
    char *oldfmname;    /* pre-IRIX4 name */
    char *fmname;       /* post-IRIX4 name */
    const char *xname;  /* X11 name (family-weight-slant-setwidth) */
    const char *encoding; /* X11 encoding (registry-encoding) */
    XFontStruct *font;  /* X11 font descriptor */
} fmfontdefs[] = {
    { "AvGrBk", "AvantGarde-Book", "itc avant garde gothic-book-r-normal", "iso8859-1" },
    { "AvGBOe", "AvantGarde-BookOblique", "itc avant garde gothic-book-o-normal", "iso8859-1" },
    { "AvGrDi", "AvantGarde-Demi", "itc avant garde gothic-demi-r-normal", "iso8859-1" },
    { "AvGDOe", "AvantGarde-DemiOblique", "itc avant garde gothic-demi-o-normal", "iso8859-1" },

    { "BkmDmi", "Bookman-Demi", "itc bookman-demi-r-normal", "iso8859-1" },
    { "BkDmIc", "Bookman-DemiItalic", "itc bookman-demi-i-normal", "iso8859-1" },
    { "BkmLgt", "Bookman-Light", "itc bookman-light-r-normal", "iso8859-1" },
    { "BkLgIc", "Bookman-LightItalic", "itc bookman-light-i-normal", "iso8859-1" },

    { "ChrBlk", "CharterBT-Black", "charter-bold-r-normal", "iso8859-1" },
    { "ChBlIc", "CharterBT-BlackItalic", "charter-bold-i-normal", "iso8859-1" },
    { "ChrtrI", "CharterBT-Italic", "charter-medium-i-normal", "iso8859-1" },
    { "ChrtrR", "CharterBT-Roman", "charter-medium-r-normal", "iso8859-1" },

    { "Crr",    "Courier", "courier-medium-r-normal", "iso8859-1" },
    { "CrrB",   "Courier-Bold", "courier-bold-r-normal", "iso8859-1" },
    { "CrrBO",  "Courier-BoldOblique", "courier-bold-o-normal", "iso8859-1" },
    { "CrrO",   "Courier-Oblique", "courier-medium-o-normal", "iso8859-1" },

    { "Hlv",    "Helvetica", "helvetica-medium-r-normal", "iso8859-1" },
    { "HlvB",   "Helvetica-Bold", "helvetica-bold-r-normal", "iso8859-1" },
    { "HlvBO",  "Helvetica-BoldOblique", "helvetica-bold-o-normal", "iso8859-1" },
    { "HlvO",   "Helvetica-Oblique", "helvetica-medium-o-normal", "iso8859-1" },

    { "HlvNrw", "Helvetica-Narrow", "helvetica-medium-r-narrow", "iso8859-1" },
    { "HlNrBd", "Helvetica-Narrow-Bold", "helvetica-bold-r-narrow", "iso8859-1" },
    { "HlNBOe", "Helvetica-Narrow-BoldOblique", "helvetica-bold-o-narrow", "iso8859-1" },
    { "HlNrOe", "Helvetica-Narrow-Oblique", "helvetica-medium-o-narrow", "iso8859-1" },

    { "NwCnSB", "NewCenturySchlbk-Bold", "new century schoolbook-bold-r-normal", "iso8859-1" },
    { "NwCSBI", "NewCenturySchlbk-BoldItalic", "new century schoolbook-bold-i-normal", "iso8859-1" },
    { "NwCnSI", "NewCenturySchlbk-Italic", "new century schoolbook-medium-i-normal", "iso8859-1" },
    { "NwCnSR", "NewCenturySchlbk-Roman", "new century schoolbook-medium-r-normal", "iso8859-1" },

    { "PaltnB", "Palatino-Bold", "palatino-bold-r-normal", "iso8859-1" },
    { "PltnBI", "Palatino-BoldItalic", "palatino-bold-i-normal", "iso8859-1" },
    { "PaltnI", "Palatino-Italic", "palatino-medium-i-normal", "iso8859-1" },
    { "PaltnR", "Palatino-Roman", "Palatino-medium-r-normal", "iso8859-1" },

    { "TmsB",   "Times-Bold", "times-bold-r-normal", "iso8859-1" },
    { "TmsBI",  "Times-BoldItalic", "times-bold-i-normal", "iso8859-1" },
    { "TmsI",   "Times-Italic", "times-medium-i-normal", "iso8859-1" },
    { "TmsR",   "Times-Roman", "times-medium-r-normal", "iso8859-1" },

    { NULL,     "Utopia-Bold", "utopia-bold-r-normal", "iso8859-1" },
    { NULL,     "Utopia-BoldItalic", "utopia-bold-i-normal", "iso8859-1" },
    { NULL,     "Utopia-Italic", "utopia-medium-i-normal", "iso8859-1" },
    { NULL,     "Utopia-Regular", "utopia-medium-r-normal", "iso8859-1" },

    { "ZpCMIc", "ZapfChancery-MediumItalic", "itc zapf chancery-medium-i-normal", "iso8859-1" },
    { "ZpfDns", "ZapfDingbats", "itc zapf dingbats-medium-r-normal", "adobe-fontspecific" },
    { "Smb",    "Symbol", "symbol-medium-r-normal", "adobe-fontspecific" },

    { "Iris",   "Iris", "fixed-medium-r-normal", "iso8859-1" },
    { "Scn",    "Screen", "fixed-medium-r-normal", "iso8859-1" },
    { "ScnB",   "Screen-Bold", "fixed-bold-r-normal", "iso8859-1" },

    { NULL,     NULL, NULL, NULL }
};

/* font description */
static struct fmfont {
    int index;          /* index in fontdefs */
    int size;           /* pixel size of font */
    XFontStruct *font;
    struct fmfontdata *data;
    /* TODO: font matrix (for fmmakefont), how? */
} fmfonts[FM_MAXFONTS];

/* currently set font */
static struct fmfont *fmCurrFont;

/* display DPI for pt<->pixel calculations */
static int fmxdpi, fmydpi;

/* the page matrix */
static GLfloat fmpage_mat[16];


/* font context for rendering */
struct fmcontext {
    int tex_width, tex_height;      /* size of each texture */

    int grid_mag;                   /* 1,  2,  4, or 8 */
    int ntextures;                  /* 1,  4, 16, or 64 (grid_mag ^ 2) */
    GLuint texid[64];               /* texture IDs */
};

/* bitmap data for a single character */
struct fmfontchar {
    short w, h;
    char *bitmap;
};

/* font descriptor */
struct fmfontdata {
    int cell_width, cell_height;    /* maximal charcell */
    int first_char, num_chars;
    struct fmfontchar *chars;
};


/* NOTE: the core of the textured text output was taken from xscreensaver,
 * but was heavily modified for use in this SGI fontmanager emulation.
 */

/* texfonts, Copyright (c) 2005-2014 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * Renders X11 fonts into textures for use with OpenGL.
 */

/* return the next larger power of 2. */
static unsigned int
to_pow2 (unsigned int i)
{
    --i;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    return ++i;
}

/* helper macro for per-char properties, if present */
#define GETPERCHAR(f,i,b,m) ((f)->per_char && (i) >= 0 ? (f)->per_char[i].m : f->b##_bounds.m)

/* fetch the character bitmaps for the font from the X server */
static void
_ifm_fmfetchfontdata(struct fmfont *fp)
{
    Display *dpy = IGL_DISPLAY;
    XFontStruct *f = fp->font;
    XGCValues gcv;
    GC gc;
    Pixmap p;
    XImage *img;
    struct fmfontdata *data;
    struct fmfontchar *fchar;
    int min_chr = f->min_char_or_byte2;
    /* NOTE: this SGI fontmanager supports only characters with 1 Byte length */
    int max_chr = (f->min_byte1 == f->max_byte1 ? f->max_char_or_byte2 : 255);
    int num_chr = (f->min_byte1 == 0 ? max_chr - min_chr + 1 : 0);
    int n_grab, chr, w, h;

    /* allocate font description and set basic font properties */
    data = calloc (1, sizeof(*data) + num_chr * sizeof(*fchar));
    data->chars = (struct fmfontchar *)(data + 1);
    fp->data = data;

    data->first_char = min_chr;
    data->num_chars = (num_chr < 0 ? 0 : num_chr);
    data->cell_width  = f->max_bounds.rbearing - f->min_bounds.lbearing;
    data->cell_height = f->max_bounds.ascent   + f->max_bounds.descent;

    if (num_chr <= 0)   /* no bitmaps to load */
        return;

    /* limit size of pixmap to 128KB if possible */
    n_grab = 131072 / (data->cell_height * data->cell_width);
    n_grab += !n_grab;  /* must be at least 1 */
    n_grab = (n_grab > num_chr ? num_chr : n_grab);

    /* create the pixmap and the gc used for grabbing */
    w = data->cell_width * n_grab;
    h = data->cell_height;
    p = XCreatePixmap (dpy, IGL_WINDOW(0)->wnd, w, h, IGL_VISUAL->depth);
    gcv.font = f->fid;
    gcv.foreground = WhitePixel(dpy, IGL_SCREEN);
    gcv.background = BlackPixel(dpy, IGL_SCREEN);
    gc = XCreateGC (dpy, p, (GCFont|GCForeground|GCBackground), &gcv);

    for (chr = min_chr; chr <= max_chr; chr += n_grab)
    {
        int grab = (max_chr - chr + 1 > n_grab ? n_grab : max_chr - chr + 1);
        int idx = chr - min_chr;
        int i;

        /* draw chars into the pixmap */
        XSetForeground (dpy, gc, BlackPixel (dpy, IGL_SCREEN));
        XFillRectangle (dpy, p, gc, 0, 0, w, h);
        XSetForeground (dpy, gc, WhitePixel (dpy, IGL_SCREEN));
        for (i = 0; i < grab; i++)
        {
            char c = chr + i;
            int x = i * data->cell_width;
            int y = 0;

            int lbearing = GETPERCHAR(f, idx+i, min, lbearing);
            int ascent   = GETPERCHAR(f, idx+i, max, ascent);
            int width    = GETPERCHAR(f, idx+i, max, width);

            if (width == 0) continue;
            XDrawString (dpy, p, gc, x - lbearing, y + ascent, &c, 1);
        }

        /* get the bitmap and store each char into its char struct */
        img = XGetImage (dpy, p, 0, 0, w, h, ~0L, ZPixmap);
        for (i = 0; i < grab; i++)
        {
            int cw = GETPERCHAR(f, idx+i, max, rbearing) - GETPERCHAR(f, idx+i, min, lbearing);
            int ch = GETPERCHAR(f, idx+i, max, ascent) + GETPERCHAR(f, idx+i, max, descent);
            char *bm = malloc (cw * ch);
            int x, y;

            fchar = &data->chars[idx+i];
            for (y = 0; y < ch; y++)
                for (x = 0; x < cw; x++)
                {
                    unsigned long pix = XGetPixel (img, x + i * data->cell_width, y);
                    unsigned long r = pix & IGL_VISUAL->visual->red_mask;
                    bm[y * cw + x] = (r >> 24 | r >> 16 | r >> 8 | r) & 0xff;
                }
            fchar->bitmap = bm;

            fchar->w = cw;
            fchar->h = ch;
        }
        XDestroyImage (img);
    }
    XFreeGC (dpy, gc);
    XFreePixmap (dpy, p);
}


/* prepare the context for text output with the font */
static void
_ifm_fmusefontdata (struct fmfont *fp, struct fmcontext *ctx)
{
    struct fmfontdata *desc = fp->data;
    unsigned char *bm;
    int grid_size;
    int which;

    /* calculate texture size. rounded to 2^n for OpenGL */
    int tw = to_pow2 (16 * desc->cell_width);
    int th = to_pow2 (16 * desc->cell_height);

    /* Figure out how many textures to use. Try to limit size to 512x512 */
    int i = (tw > th ? tw : th);

    if      (i <= 512)  ctx->grid_mag = 1;  /*  1 tex of 16x16 chars */
    else if (i <= 1024) ctx->grid_mag = 2;  /*  4 tex of 8x8 chars */
    else if (i <= 2048) ctx->grid_mag = 4;  /* 16 tex of 4x4 chars */
    else                ctx->grid_mag = 8;  /* 64 tex of 2x2 chars */
    grid_size = (16 / ctx->grid_mag);
    tw /= ctx->grid_mag;
    th /= ctx->grid_mag;

    ctx->ntextures = ctx->grid_mag * ctx->grid_mag;
    ctx->tex_width  = tw;
    ctx->tex_height = th;

    bm = malloc(tw * th);

    /* for each texture */
    for (which = 0; which < ctx->ntextures; which++)
    {
        GLuint iformat = GL_INTENSITY;
        GLuint format = GL_LUMINANCE;
        GLuint type = GL_UNSIGNED_BYTE;
        GLuint filter = (desc->cell_height > 20 ? GL_LINEAR : GL_NEAREST);

        /* fill characters in texture */
        memset (bm, 0, tw * th);
        for (i = 0; i < 256 / ctx->ntextures; i++)
        {
            char c = (i + (which * 256 / ctx->ntextures));

            if (c >= desc->first_char && c < desc->first_char + desc->num_chars)
            {
                int idx = c - desc->first_char;
                int tx = (i % grid_size) * desc->cell_width;
                int ty = (i / grid_size) * desc->cell_height;
                int cy, cw = desc->chars[idx].w;

                for (cy = 0; cy < desc->chars[idx].h; cy++)
                {
                    memcpy (&bm[(ty+cy)*tw + tx], &desc->chars[idx].bitmap[cy*cw], cw);
                }
            }
        }

        /* create texture and store tex id in context */
        glGenTextures (1, &ctx->texid[which]);
        glBindTexture (GL_TEXTURE_2D, ctx->texid[which]);

        glTexImage2D (GL_TEXTURE_2D, 0, iformat, tw, th, 0, format, type, bm);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   }

   free (bm);
}

/* load font context */
static void
_ifm_fmloadcontext (int n)
{
    igl_windowT *wptr = IGL_CTX ();
    struct fmfont *fp = &fmfonts[n];

    if (wptr->fmContexts[n].ntextures != 0)
        return;

    if (fp->data == NULL)
        _ifm_fmfetchfontdata (fp);
    _ifm_fmusefontdata (fp, &wptr->fmContexts[n]);
}

/* unload font context */
static void
_ifm_fmfreecontext (struct fmfont *fp, struct fmcontext *ctx)
{
    int i;

    for (i = 0; i < ctx->ntextures; i++)
    {
        if (ctx->texid[i])
            glDeleteTextures (1, &ctx->texid[i]);
    }
    ctx->ntextures = 0;
}

/* load a font into OpenGL */
static void
_ifm_fmloadfont (int n)
{
    igl_windowT *wptr = IGL_CTX ();
    struct fmfont *fp = &fmfonts[n];

    if (wptr->fmContexts == NULL)
        wptr->fmContexts = calloc (FM_MAXFONTS, sizeof(struct fmcontext));

    if (fp->font == NULL)
        return;

    _ifm_fmloadcontext (n);
}

/* release font manager resources */
static void
_ifm_fmfontRelease (igl_windowT *wptr)
{
    int i;

    if (wptr->fmContexts == NULL)
        return;
    for (i = 0; i < FM_MAXFONTS; i++)
        _ifm_fmfreecontext (&fmfonts[i], &wptr->fmContexts[i]);

    free (wptr->fmContexts);
    wptr->fmContexts = NULL;
}


/* draws the string in the scene at the origin */
static void
_ifm_fmprintstring(struct fmfont *fp, struct fmcontext *ctx, const char *string, int len)
{
    struct fmfontdata *data = fp->data;
    XFontStruct *f = fp->font;
    int x, y;
    unsigned int i;

    x = 0;
    y = 0;
    for (i = 0; i < len; i++)
    {
        unsigned char c = string[i];
        /* Within each texture:

           [A]----------------------------
            |     |           |   |      |
            |   l |         w |   | r    |
            |   b |         i |   | b    |
            |   e |         d |   | e    |
            |   a |         t |   | a    |
            |   r |         h |   | r    |
            |   i |           |   | i    |
            |   n |           |   | n    |
            |   g |           |   | g    |
            |     |           |   |      |
            |----[B]----------|---|      |
            |     |   ascent  |   |      |
            |     |           |   |      |
            |     |           |   |      |
            |--------------------[C]     |
            |         descent            |
            |                            | cell_width,
            ------------------------------ cell_height

           We want to make a quad from point A to point C.
           We want to position that quad so that point B lies at x,y.
         */
        int lbearing = GETPERCHAR(f, c - f->min_char_or_byte2, min, lbearing);
        int rbearing = GETPERCHAR(f, c - f->min_char_or_byte2, max, rbearing);
        int ascent   = GETPERCHAR(f, c - f->min_char_or_byte2, max, ascent);
        int descent  = GETPERCHAR(f, c - f->min_char_or_byte2, max, descent);
        int cwidth   = GETPERCHAR(f, c - f->min_char_or_byte2, max, width);

        unsigned char cc = c % (256 / ctx->ntextures);

        int gs = (16 / ctx->grid_mag);                  /* grid size */

        int ax = ((int) cc % gs) * data->cell_width;    /* point A */
        int ay = ((int) cc / gs) * data->cell_height;

        int bx = ax - lbearing;                         /* point B */
        int by = ay + ascent;

        int cx = bx + rbearing;                         /* point C */
        int cy = by + descent;

        GLfloat tax = (GLfloat) ax / ctx->tex_width;    /* tex coords of A */
        GLfloat tay = (GLfloat) ay / ctx->tex_height;

        GLfloat tcx = (GLfloat) cx / ctx->tex_width;    /* tex coords of C */
        GLfloat tcy = (GLfloat) cy / ctx->tex_height;

        GLfloat qx0 = x + lbearing;                     /* quad top left */
        GLfloat qy0 = y + ascent;
        GLfloat qx1 = qx0 + rbearing - lbearing;        /* quad bot right */
        GLfloat qy1 = qy0 - (ascent + descent);

        if (cwidth > 0 && c != ' ')
        {
            int which = c / (256 / ctx->ntextures);
            if (which >= ctx->ntextures) abort();
            glBindTexture (GL_TEXTURE_2D, ctx->texid[which]);

            glBegin (GL_QUADS);
            glTexCoord2f (tax, tay); glVertex3f (qx0, qy0, 0);
            glTexCoord2f (tcx, tay); glVertex3f (qx1, qy0, 0);
            glTexCoord2f (tcx, tcy); glVertex3f (qx1, qy1, 0);
            glTexCoord2f (tax, tcy); glVertex3f (qx0, qy1, 0);
            glEnd();
        }

        x += cwidth;
    }
}


/* output string at world coordinates (x,y,z) */
static void
_ifm_fmoutstr (struct fmfont *fp, float x, float y, float z, const char *str, int len)
{
    igl_windowT *wptr = IGL_CTX ();
    GLdouble tx, ty, tz;

    _ifm_fmloadfont (fp-fmfonts);

    /* get raster cordinates of position */
    _igl_getViewport ();
    _igl_getMatrices ();
    gluProject (x, y, z, wptr->modelMatrix, wptr->projMatrix, wptr->viewport, &tx, &ty, &tz);

    /* save status and load ortho projection */
    glPushAttrib (GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT);
    _igl_pushTransform ();
    _igl_ortho2D (0);

    /* move to raster coordinates */
    glTranslatef (tx, ty /*- fp->font->max_bounds.descent*/, 0);
    /* apply page matrix */
    glMultMatrixf (fmpage_mat);

    /* set drawing environment */
    glDisable (GL_CULL_FACE);
    glDisable (GL_DEPTH_TEST);
    glColor3ubv (wptr->currentColor);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_TEXTURE_2D);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    /* draw text TODO: lighting? */
    IGL_DISLIGHT ();
    _ifm_fmprintstring (fp, &wptr->fmContexts[fp-fmfonts], str, len);

    /* restore status */
    _igl_popTransform ();
    glPopAttrib ();
} 


/* FM: font manager page matrix operations */
void
fmgetpagematrix (double m[3][2])
{
    int i, j;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
            m[j][i] = fmpage_mat[j*4 + i];
    m[3][0] = m[3][1] = 0;
}


void
fmsetpagematrix (double m[3][2])
{
    int i, j;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
            fmpage_mat[j*4 + i] = m[j][i];
}


void
fmconcatpagematrix (double m[3][2])
{
    double new_mat[3][2];
    int i, j;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
            new_mat[j][i] = fmpage_mat[j*4 + 0] * m[0][i] +
                            fmpage_mat[j*4 + 1] * m[1][i];

    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
            fmpage_mat[j*4 + i] = new_mat[j][i];
}
 

void
fmscalepagematrix (double x)
{
    int i, j;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
            fmpage_mat[j*4 + i] *= x;
}


void
fmrotatepagematrix (double angle)
{
    double angrad = -angle * M_PI / 180;
    double sinang = sin(angrad), cosang = cos(angrad);
    double m[3][2] = {{ 0 }};

    m[0][0] =  cosang, m[0][1] = -sinang;
    m[1][0] =  sinang, m[1][1] =  cosang;
    fmconcatpagematrix (m);
}


/* FM: intialize page matrix */
void
fminitpagematrix (void)
{
    int i;

    for (i = 0; i < 16; i++)
        fmpage_mat[i] = 0;
    for (i = 0; i < 4; i++)
        fmpage_mat[i*4 + i] = 1;
}


/* find a font by its index. allocate it if it doesn't exist */
static int
_ifm_fmfindslot (int idx, int size)
{
    int i;
    int f = -1;

    for (i = 0; i < FM_MAXFONTS; i++)
    {
        if (fmfonts[i].index == idx && fmfonts[i].size == size)
            return i;
        if (f < 0 && fmfonts[i].index < 0)
            f = i;
    }

    if (f >= 0)
    {
        fmfonts[f].index = idx;
        fmfonts[f].size = size;
        fmfonts[f].font = NULL;
    }
    return f;
}


/* FM: initialize the Font Manager */
void
fminit (void)
{
    struct fmfontdef *fp;
    char name[200];
    int count = 0, i;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    /* calculate DPI in x and y */
    fmxdpi = DisplayWidth (IGL_DISPLAY, IGL_SCREEN) * 254 /
                            (DisplayWidthMM (IGL_DISPLAY, IGL_SCREEN) * 10);
    fmydpi = DisplayHeight (IGL_DISPLAY, IGL_SCREEN) * 254 /
                            (DisplayHeightMM (IGL_DISPLAY, IGL_SCREEN) * 10);

    /* check which of the fonts is available */
    for (i = 0, fp = fmfontdefs; fp->fmname; i++, fp++)
    {
        snprintf(name, sizeof(name), "-*-%s-*-*-10-%d-%d-*-*-%s",
                    fp->xname, fmxdpi, fmydpi, fp->encoding);
        fp->font = XLoadQueryFont (IGL_DISPLAY, name);
        count += (fp->font != NULL);
    }
    if (count != i)
        fprintf (stderr, "fminit: %d of %d fonts not found\n", i-count, i);

    for (i = 0; i < FM_MAXFONTS; i++)
        fmfonts[i].index = -1;

    fminitpagematrix ();

    igl->fmReleaseCB = _ifm_fmfontRelease;
}


/* FM: enumerate the font faces available through the font manager */
void
fmenumerate (void (*cb)(char *))
{
    struct fmfontdef *fp;

    _igl_trace (__func__,TRUE,"%p",cb);
    IGL_CHECKINIT ();

    for (fp = fmfontdefs; fp->fmname; fp++)
    {
        if (fp->font != NULL)
            cb(fp->fmname);
    }
}


/* FM: select a font face */
fmfonthandle
fmfindfont (const char *face)
{
    struct fmfontdef *fp;

    _igl_trace (__func__,TRUE,"%p(%s)",face,face?face:"");
    IGL_CHECKINIT ();

    for (fp = fmfontdefs; fp->fmname; fp++)
    {
        if (fp->font != NULL && (!strcmp (fp->fmname, face) ||
            (fp->oldfmname && !strcmp(fp->oldfmname, face))))
        {
            /* font found, create and return the handle */
            int idx = _ifm_fmfindslot (fp-fmfontdefs, 10);
            if (idx >= 0)
            {
                struct fmfont *p = &fmfonts[idx];
                p->font = fp->font;
                return (fmfonthandle) p;
            }
        }
    }

    return NULL;
}


/* FM: scale a font face */
fmfonthandle
fmscalefont (fmfonthandle fh, double scale)
{
    struct fmfont *fp = (struct fmfont *)fh;
    struct fmfont *fn = NULL;
    struct fmfontdef *fd;
    int idx, scale10 = scale * 10;
    char name[200] = {0};

    _igl_trace (__func__,TRUE,"%p,%f",fh,scale);
    IGL_CHECKINIT ();

    idx = _ifm_fmfindslot(fp->index, scale10);
    if (idx >= 0)
    {
        fn = &fmfonts[idx];
        fd = &fmfontdefs[fp->index];

        if (fn->font == NULL)
        {
            snprintf(name, sizeof(name), "-*-%s-*-*-%d-%d-%d-*-*-%s",
                        fd->xname, scale10, fmxdpi, fmydpi, fd->encoding);
            fn->font = XLoadQueryFont (IGL_DISPLAY, name);
        }
        if (fn->font == NULL)
            fn = NULL;
    }

    return (fmfonthandle)fn;
}


/* FM: free the storage for a font */
void
fmfreefont (fmfonthandle fh)
{
    struct fmfont *fp = (struct fmfont *)fh;
    igl_windowT *wptr;
    int idx = fp-fmfonts;
    int i;

    _igl_trace (__func__,TRUE,"%p",fh);
    IGL_CHECKINIT ();

    /* throw away cached font for all IGL windows */
    for (i = 0, wptr = igl->openWindows; i < IGL_MAXWINDOWS; i++, wptr++)
    {
        if (wptr->fmContexts != NULL)
            _ifm_fmfreecontext(fp, &wptr->fmContexts[idx]);
    }

    if (fmCurrFont == (void *)fh)
        fmCurrFont = NULL;

    if (fp->font != NULL)
    {
        XFreeFont (IGL_DISPLAY, fp->font);
        fp->font = NULL;
    }

    fp->index = -1;
}


/* FM: set the current font */
void
fmsetfont (fmfonthandle fh)
{
    _igl_trace (__func__,TRUE,"%p",fh);
    IGL_CHECKINIT ();

    fmCurrFont = (void *)fh;
}


/* FM: return the name associated with a font */
int
fmgetfontname (fmfonthandle fh, int len, char *str)
{
    struct fmfont *fp = (struct fmfont *)fh;

    _igl_trace (__func__,TRUE,"%p,%d,%p",fh,len,str);
    IGL_CHECKINIT ();

    str[len-1] = '\0';
    strncpy (str, fmfontdefs[fp->index].fmname, len-1);
    return strlen(str);
}


/* FM: return the width of a character */
long
fmgetchrwidth (fmfonthandle fh, const unsigned char chr)
{
    struct fmfont *fp = (struct fmfont *)fh;

    _igl_trace (__func__,TRUE,"%p,%d",fh,chr);
    IGL_CHECKINIT ();

    return XTextWidth (fp->font, (const char *)&chr, 1);
}


/* FM: return the width of a string in pixels */
long
fmgetstrwidth (fmfonthandle fh, const char *str)
{
    struct fmfont *fp = (struct fmfont *)fh;

    _igl_trace (__func__,TRUE,"%p,%p(%s)",fh,str,str?str:"");
    IGL_CHECKINIT ();

    return XTextWidth (fp->font, str, strlen(str));
}


/* FM: render a string in the current font */
int
fmprstr (const char *str)
{
    struct fmfont *fp = fmCurrFont;
    long len = strlen(str), width;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%p(%s)",str,str?str:"");
    IGL_CHECKWND (-1);
    wptr = IGL_CTX ();

    if (fmCurrFont == NULL)
        return -1;

    _ifm_fmoutstr (fp, wptr->cposx, wptr->cposy, wptr->cposz, str, len);

    /* NOTE: [dx,dy] = fmpage_mat * [width,0] */
    width = XTextWidth (fp->font, str, len);
    wptr->cposx += fmpage_mat[0] * width;
    wptr->cposy += fmpage_mat[4] * width;
    return 0;
}


/* FM: render a single glyph */
long
fmoutchar (fmfonthandle fh, unsigned int chr)
{
    struct fmfont *fp = (struct fmfont *)fh;
    char str = chr;
    long width;
    igl_windowT *wptr;

    _igl_trace (__func__,TRUE,"%p,%d",fh,chr);
    IGL_CHECKWND (-1);
    wptr = IGL_CTX ();

    _ifm_fmoutstr (fp, wptr->cposx, wptr->cposy, wptr->cposz, &str, 1);

    /* NOTE: [dx,dy] = fmpage_mat * [width,0] */
    width = XTextWidth (fp->font, &str, 1);
    wptr->cposx += fmpage_mat[0] * width;
    wptr->cposy += fmpage_mat[4] * width;
    return width;
}


/* FM: render a character string in a specified font */
int
fmfprstr (fmfonthandle fh, unsigned char *str, short len, int x, int y)
{
    struct fmfont *fp = (struct fmfont *)fh;
    long width;

    _igl_trace (__func__,TRUE,"%p,%p(%s),%d,%d,%d",fh,str,str?(char *)str:"",len,x,y);
    IGL_CHECKWND (-1);

    fmsetfont (fh);

    _ifm_fmoutstr (fp, x, y, 0, (char *)str, len);

    /* NOTE: [dx,dy] = fmpage_mat * [width,0] */
    width = XTextWidth (fp->font, (char *)str, len);
    IGL_CTX ()->cposx += fmpage_mat[0] * width;
    IGL_CTX ()->cposy += fmpage_mat[4] * width;
    return width;
}


/* FM: return information about the overall font */
int
fmgetfontinfo (fmfonthandle fh, fmfontinfo *info)
{
    struct fmfont *fp = (struct fmfont *)fh;

    _igl_trace (__func__,TRUE,"%p,%p",fh,info);
    IGL_CHECKINIT ();

    if (fp->data == NULL)
        _ifm_fmfetchfontdata (fp);

    /* TODO: this seems to be incorrect. What's really in info? */
    IGL_CLR(info);
    info->matrix00 = info->matrix11 = fp->size / 10.0;
    info->xorig = -fp->font->min_bounds.lbearing;
    info->yorig = fp->font->max_bounds.descent;
    info->xsize = fp->data->cell_width;
    info->ysize = fp->data->cell_height;
    info->width  = info->xsize;
    info->height = info->ysize;
    info->nglyphs = fp->data->first_char + fp->data->num_chars;
    info->resolution = fp->size;
    return 0;
}


/* FM: What exactly is this doing? */
float
fmpointsize_to_pixels(void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    if (fmxdpi == 0.0)  /* might be called before fminit? */
        return 96.0 / 72.0;
    else
        return fmxdpi / 72.0; /* ? */
}


float
fmpixels_to_pointsize(void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    if (fmxdpi == 0.0)  /* might be called before fminit? */
        return 72.0 / 96.0;
    else
        return 72.0 / fmxdpi; /* ? */
}


/* TODO: implement these... */
fmfonthandle
fmmakefont (fmfonthandle fh, double matrix[3][2])
{
    return NULL;
}


long
fmgetwholemetrics (fmfonthandle fh, /*fmglyphinfo*/ void *gi)
{
    return -1;
}


/* these are unimplemented */
void
fmsetpath (const char *path)
{
}


char *
fmfontpath (void)
{
    return "";
}


void
fmsetcachelimit (int limit)
{
}


int
fmcachelimit (void)
{
    return 0;
}


int
fmgetcacheused (void)
{
    return 0;
}


void
fmcachedisable (void)
{
}


void
fmcacheenable (void)
{
}


void
fmprintermatch (int set)
{
}


int
fmgetcomment (fmfonthandle fh, int len, char *buf)
{
    *buf = '\0';
    return -1;
}


/* TODO: precursor of libfl, used by libInventor.so.1. non-working guesswork! */
static fmfonthandle fontmap[4];

void
wfm_init (void)
{
    fminit ();
}

void
wfm_hint (long mode, long val)
{
}

void
wfm_makefontmat (float *mat, float angle, float scale)
{
    *mat = scale; /* store scale only, no rotation */
}

long
wfm_makefont(long id, char *name, float *mat)
{
    fontmap[id] = fmscalefont(fmfindfont (name), *mat);
    return (fontmap[id] != NULL);
}

void
wfm_font(long id)
{
    fmsetfont(fontmap[id]);
}

void
wfm_cmov(float x, float y, float z)
{
    cmov(x, y, z);
}

void
wfm_rpos(float p1, float p2)
{
    /*?*/
}

void
wfm_charstr(char *str)
{
    fmprstr(str);
}

long
wfm_strwidth(char *str)
{
    return fmgetstrwidth ((void *)fmCurrFont, str);
}

void
wfm_getbbox(long chr, float *llx, float *lly, float *urx, float *ury)
{
    if (fmCurrFont->data == NULL)
        _ifm_fmfetchfontdata (fmCurrFont);
    /* should get this for the given chr... */
    *llx = -fmCurrFont->font->min_bounds.lbearing;
    *lly = fmCurrFont->font->max_bounds.descent;
    *urx = fmCurrFont->font->max_bounds.rbearing;
    *ury = fmCurrFont->font->max_bounds.ascent;
}

void *
wfm_getoutline(long chr, float p2 /*?*/)
{
    return NULL; /* can't obtain the outline from X :-( */
}

void
wfm_freeoutline(long *ptr)
{
}
#endif
