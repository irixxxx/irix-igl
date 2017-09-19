#include <stdio.h>
#include <string.h>
#include <math.h>
#include "igl.h"


#define MAX_POINTS      50


float xrot, yrot, zrot;
float camx, camy, camz;
double angle, sf[MAX_POINTS], cf[MAX_POINTS];
Boolean lightingMode = -1;


float mat[] =
{
    AMBIENT, 0.2f, 0.2f, 0.2f,
    DIFFUSE, 0.8f, 0.8f, 0.8f,
    SPECULAR, 0.9f, 0.9f, 0.9f,
    SHININESS, 50.0f,
    LMNULL
};

float light1[] =
{
    LCOLOR, 1.0f, 0.5f, 0.0f,
    POSITION, -3.0f, -3.0f, -3.0f, 0.0f,
    LMNULL
};

float light2[] =
{
    LCOLOR, 0.0f, 0.5f, 1.0f,
    POSITION, 3.0f, 3.0f, 3.0f, 0.0f,
    LMNULL
};

float lmodel[] =
{
    LOCALVIEWER, 1.0f,
    TWOSIDE, 1.0f,
    LMNULL
};


void
render ()
{
    int i;
    char buf[256];
    Coord nrm[3] = {0, 0, 1};


    pushmatrix ();
    translate (camx+MAX_POINTS/8, camy+MAX_POINTS/4, camz-MAX_POINTS);
    rot (xrot, 'x');
    rot (yrot, 'y');
    rot (zrot, 'z');
    xrot += 0.5f; yrot += 0.5f; zrot += 0.5f;

    cpack (0x407fffff);
    for (i=0; i < MAX_POINTS; i++)
    {
        sf[i] += sin (angle*M_PI) / 1.5f;
        cf[i] += cos (angle*M_PI) / 1.5f;

        rot ((float)(sf[i] - cf[i]), 'x');
        rot ((float)(sf[i] + cf[i]), 'y');
        rot ((float)(sf[i] + cf[i]), 'z');
        angle += 0.00000005;

        pushmatrix ();
            translate ((float)i, 0, -5.0f);
            normal (nrm);
            rectf (0, 0, 1, 1);
        popmatrix ();
    }
    popmatrix ();

    cpack (0xff7fffff);
    if (lightingMode != -1)
    {
        cmov (camx-MAX_POINTS/3, camy-3*MAX_POINTS/4, camz-MAX_POINTS);
        sprintf (buf, "demo2 - fullscreen RGB lighting, LMC mode=%d (<- and -> to change)", lightingMode);
    }
    else
    {
        cmov (camx-MAX_POINTS/4, camy-3*MAX_POINTS/4, camz-MAX_POINTS);
        strcpy (buf, "demo2 - fullscreen RGB color (<- and -> to change)");
    }
    charstr (buf);
}


int
main ()
{
    int n, newn, windowId;
    long dev;
    short val;
    float t;


    xrot = yrot = zrot = 0;
    camx = -2.5f, camy = -2.5f, camz = -30.0f;

    noborder ();
    prefsize (1024, 768);
    windowId = winopen ("demo2 - fullscreen (RGB lighting)");
    winconstraints ();
    doublebuffer ();
    RGBmode ();
    gconfig ();
    fullscrn ();
    mmode (MVIEWING);
    perspective (600, 1, 0, 500);

    qdevice (LEFTARROWKEY);
    qdevice (RIGHTARROWKEY);

    cursoff ();


    lmdef (DEFMATERIAL, 1, 0, mat);
    lmdef (DEFLIGHT, 1, 0, light1);
    lmdef (DEFLIGHT, 2, 0, light2);
    lmdef (DEFLMODEL, 1, 0, lmodel);
    lmbind (MATERIAL, 1);
    lmbind (BACKMATERIAL, 1);
    lmbind (LIGHT1, 1);
    lmbind (LIGHT2, 2);

    n = 0;
    t = 0.0f;

    /* enable transparency */
    zbuffer (FALSE);
    blendfunction (BF_SA, BF_MSA);

    while (1)
    {
        while (qtest ())
        {
            dev = qread (&val);
            if (dev == LEFTARROWKEY && val == 1)
            {
                lightingMode --;
                if (lightingMode < -1)
                {
                    lightingMode = LMC_NULL;
                }
            }
            else if (dev == RIGHTARROWKEY && val == 1)
            {
                lightingMode ++;
                if (lightingMode > LMC_NULL)
                {
                    lightingMode = -1;
                }
            }
        }

        switch (lightingMode)
        {
            case -1:
                lmbind (LMODEL, 0);     /* lighting off */
            break;

            default:
                lmbind (MATERIAL, 1);
                lmbind (LMODEL, 1);     /* lighting on + LMC mode change */
                lmcolor (LMC_COLOR + lightingMode);
            break;
        }

        t += 0.1f;
        newn = (t >= 1.0f);
        n += (int)floor (t);
        t = (float)fmod (t, 1.0f);
        sf[n % MAX_POINTS] += sin (angle*M_PI);
        cf[n % MAX_POINTS] += cos (angle*M_PI);

        cpack (0x000000ff);
        clear ();

        render ();
        swapbuffers ();
    }

    curson ();

    return (0);
}



