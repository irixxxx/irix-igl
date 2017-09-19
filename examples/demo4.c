#include <math.h>
#include "igl.h"


#define MAX_POINTS 50


int
main ()
{
    int i, j;
    float xrot, yrot, zrot;
    float camx, camy, camz;
    double angle = 0;
    int windowId1, windowId2;
    float mat[] =
    {
        COLORINDEXES, (float)CYAN, (float)WHITE, (float)WHITE,
        SHININESS, 100.0f,
        LMNULL
    };
    float light1[] =
    {
        POSITION, -3.0f, -3.0f, -3.0f, 0.0f,
        LMNULL
    };
    float light2[] =
    {
        POSITION, 3.0f, 3.0f, 3.0f, 0.0f,
        LMNULL
    };
    float lmodel[] =
    {
        LOCALVIEWER, 1.0f,
        LMNULL
    };
    float nrm[3] = { 0, 0, 1 };


    xrot = yrot = zrot = 0;
    camx = -2.5f, camy = -2.5f, camz = -30.0f;


    prefposition (0, 300, 0, 300);
    windowId1 = winopen ("demo4 - colormap lighting");
    winconstraints ();
    doublebuffer ();
    gconfig ();
    mmode (MVIEWING);
    perspective (600, 320.0f/200.0f, 0, 500);

    lmdef (DEFMATERIAL, 1, 0, mat);
    lmdef (DEFLIGHT, 1, 0, light1);
    lmdef (DEFLIGHT, 2, 0, light2);
    lmdef (DEFLMODEL, 1, 0, NULL);
    lmbind (MATERIAL, 1);
    lmbind (LIGHT1, 1);
    lmbind (LIGHT2, 2);
    lmbind (LMODEL, 1);


    prefposition (307, 600, 0, 300);
    windowId2 = winopen ("demo4 - color");
    winconstraints ();
    doublebuffer ();
    gconfig ();
    perspective (600, 320.0f/200.0f, 0, 500);


    while (1)
    {
        winset (windowId1);
        lmbind (LIGHT1, 1);
        lmbind (LIGHT2, 2);
        lmbind (LMODEL, 1);

        color (BLACK);
        clear ();

        color (GREEN);
        cmov (camx-MAX_POINTS/4, camy-MAX_POINTS/2, camz-MAX_POINTS);
        charstr ("Hello world 1");

        pushmatrix ();
        translate (camx-MAX_POINTS/4, camy-MAX_POINTS/2, camz-MAX_POINTS);
        rot (xrot, 'x');
        rot (yrot, 'y');
        rot (zrot, 'z');
        xrot += 0.5f; yrot += 0.5f; zrot += 0.5f;

        for (i=0; i < MAX_POINTS; i+=2)
        {
            color (YELLOW);

            for (j=0; j < MAX_POINTS; j+=2)
            {
                float sf = (float)sin (angle*M_PI), cf = (float)cos (angle*M_PI);
                unsigned char c = (unsigned char)(2*(i+j));


                rot ((float)angle + sf - cf, 'x');
                rot ((float)angle - sf + cf, 'y');
                rot ((float)angle + sf - cf, 'z');
                angle += 0.0000005;

                pushmatrix ();
                translate ((float)i, (float)j, 0);
                normal (nrm);
                rectf (0, 0, 1, 1);
                popmatrix ();

                color (CYAN);
            }
        }
        popmatrix ();
        swapbuffers ();


        winset (windowId2);
        lmbind (LMODEL, 0);

        color (BLACK);
        clear ();

        color (GREEN);
        cmov (camx-MAX_POINTS/4, camy-MAX_POINTS/2, camz-MAX_POINTS);
        charstr ("Hello world 2");

        pushmatrix ();
        translate (camx-MAX_POINTS/4, camy-MAX_POINTS/2, camz-MAX_POINTS);
        rot (xrot, 'x');
        rot (yrot, 'y');
        rot (zrot, 'z');

        for (i=0; i < MAX_POINTS; i+=2)
        {
            color (MAGENTA);

            for (j=0; j < MAX_POINTS; j+=2)
            {
                float sf = (float)sin (angle*M_PI), cf = (float)cos (angle*M_PI);
                unsigned char c = (unsigned char)(2*(i+j));


                rot ((float)angle + sf - cf, 'x');
                rot ((float)angle - sf + cf, 'y');
                rot ((float)angle + sf - cf, 'z');
                angle += 0.0000005;

                pushmatrix ();
                translate ((float)i, (float)j, 0);
                normal (nrm);
                rectf (0, 0, 1, 1);
                popmatrix ();

                color (RED);
            }
        }
        popmatrix ();
        swapbuffers ();
    }

    return (0);
}




