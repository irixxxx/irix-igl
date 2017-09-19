#include <math.h>
#include "igl.h"


#define MAX_POINTS 50


float xrot, yrot, zrot;
double angle, sf[MAX_POINTS], cf[MAX_POINTS];
float normalColor1[] = {0.0f, 0.0f, 1.0f};
float normalColor2[] = {1.0f, 0.0f, 0.0f};
int showSGIText = PUP_BOX;


void
render ()
{
    int i;


    pushmatrix ();
    translate (-2.5f+MAX_POINTS/8, -2.5f+MAX_POINTS/4, -2*MAX_POINTS);
    rot (xrot, 'x');
    rot (yrot, 'y');
    rot (zrot, 'z');
    xrot += 0.2f; yrot += 0.2f; zrot += 0.2f;

    pushmatrix ();
        for (i=0; i < MAX_POINTS; i++)
        {
            sf[i] += sin (angle*M_PI)/10.0;
            cf[i] += cos (angle*M_PI)/10.0;

            /* first spyro */
            rot ((float)(sf[i] + cf[i]), 'y');
            angle += 0.00000005;

            normalColor1[0] = 1.0f - (float)i/MAX_POINTS;
            normalColor1[1] = 1.0f - (float)i/MAX_POINTS;
            c3f (normalColor1);

            translate ((float)i, 0, 0);
            lcharstr (STR_B, (void *)"IGL");
        }
    popmatrix ();

    if (showSGIText == PUP_CHECK)
    {
        pushmatrix ();
            for (i=0; i < MAX_POINTS; i++)
            {
                /* second spyro */
                rot ((float)(sf[i] - cf[i]), 'z');
                angle += 0.00000005;

                normalColor2[1] = 1.0f - (float)i/MAX_POINTS;
                normalColor2[2] = 1.0f - (float)i/MAX_POINTS;
                c3f (normalColor2);

                translate ((float)i*2, 0, 0);
                lcharstr (STR_B, (void *)"SGI");
            }
        popmatrix ();
    }

    popmatrix ();
}


int
main ()
{
    int n, newn, windowId;
    float t;
    long pup, subpup, dev;
    short val;


    xrot = yrot = zrot = 0;


    prefsize (800, 600);
    maxsize (800, 600);
    minsize (400, 300);
    windowId = winopen ("demo3 - menus");
    winconstraints ();
    doublebuffer ();
    RGBmode ();
    gconfig ();
    perspective (600, 1, 0, 500);

    qdevice (XKEY);
    qdevice (RIGHTMOUSE);
    subpup = defpup ("a submenu %t|Hello subitem1|and subitem2");
    pup = defpup ("demo3 %t|Show SGI text|misc %m|Exit (X)", subpup);
    setpup (pup, 0, showSGIText);


    n = 0;
    t = 0.0f;

    /* enable transparency */
    zbuffer (FALSE);
    blendfunction (BF_SA, BF_ONE);

    while (1)
    {
        while (qtest ())
        {
            dev = qread (&val);
            switch (dev)
            {
                case XKEY:
                    return (0);
                break;

                case REDRAW:
                    reshapeviewport ();
                break;

                case RIGHTMOUSE:
                    if (val == 1)
                    {
                        val = (short)dopup (pup);
                        if (val == 1)
                        {
                            showSGIText = (showSGIText == PUP_BOX ? PUP_CHECK : PUP_BOX);
                            setpup (pup, 0, showSGIText);
                        }
                        else if (val == 3)
                        {
                            return (0);
                        }
                    }
                break;
            }
        }

        t += 0.1f;
        newn = (t >= 1.0f);
        n += (int)floor (t);
        t = (float)fmod (t, 1.0f);
        sf[n % MAX_POINTS] += sin (angle*M_PI);
        cf[n % MAX_POINTS] += cos (angle*M_PI);

        clear ();
        render ();
        swapbuffers ();
    }

    return (0);
}



