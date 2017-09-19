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
    int obj, windowId;
    long dev;
    short val;
    Boolean objRectf = TRUE;


    xrot = yrot = zrot = 0;
    camx = -2.5f, camy = -2.5f, camz = -30.0f;

    prefposition (0, 300, 0, 300);
    keepaspect (4, 3);
    windowId = winopen ("demo5 - editable objects");
    winconstraints ();
    doublebuffer ();
    mmode (MVIEWING);
    gconfig ();
    perspective (600, 320.0f/200.0f, 0, 500);
    cmov (camx-MAX_POINTS-17, camy-3*MAX_POINTS/4, camz-MAX_POINTS);

    qdevice (SPACEKEY);


    /* object definition (default = yellow filled rectangle, size=1) */
    obj = 1;
    makeobj (obj);
    maketag (1);
    color (YELLOW);
    rectf (0, 0, 1, 1);
    closeobj ();

    while (1)
    {
        while (qtest ())
        {
            dev = qread (&val);
            if (dev == SPACEKEY && val == 1)
            {
                objRectf = !objRectf;

                /* edit the object and replace with new definition */
                editobj (1);
                objreplace (1);
                if (objRectf)
                {
                    /* yellow filled rectangle, size=1 */
                    color (YELLOW);
                    rectf (0, 0, 1, 1);
                }
                else
                {
                    /* cyan outlined rectangle, size=2 */
                    color (CYAN);
                    rect (-1.0f, -1.0f, 1.0f, 1.0f);
                }
                closeobj ();
            }
        }


        color (BLACK);
        clear ();


        pushmatrix ();
        translate (camx-MAX_POINTS/4, camy-MAX_POINTS/2, camz-MAX_POINTS);
        rot (xrot, 'x');
        rot (yrot, 'y');
        rot (zrot, 'z');
        xrot += 0.5f; yrot += 0.5f; zrot += 0.5f;

        for (i=0; i < MAX_POINTS; i+=2)
        {
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
                callobj (1);
                popmatrix ();
            }
        }
        popmatrix ();

        color (GREEN);
        charstr ("demo5 - editable objects (SPACE to change)");

        swapbuffers ();
    }

    return (0);
}




