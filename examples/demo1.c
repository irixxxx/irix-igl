#include <math.h>
#include "igl.h"

#define MAX_POINTS 50

#define TWO_WINDOWS     1   /* define to 1 if you'd like to see two windows in action */
#define DO_MENUS        1   /* define to 1 for right-click menu */
#define DO_PATTERN      0   /* define to 1 for pattern'ed squares in window 1 */


int
main ()
{
    int i, j;
    float xrot, yrot, zrot;
    float camx, camy, camz;
    double angle = 0;
    int windowId1;
#if TWO_WINDOWS    
    int windowId2;
#endif
#if DO_MENUS
    long pup1, pup2, pup3, dev;
    short val;
#endif
#if DO_PATTERN
    unsigned short pattern[1024];
#endif


    xrot = yrot = zrot = 0;
    camx = -2.5f, camy = -2.5f, camz = -30.0f;


    prefposition (0, 300, 0, 300);
    windowId1 = winopen ("demo1 - doublebuffer + pattern");
    winconstraints ();
    doublebuffer ();
    gconfig ();
    perspective (600, 320.0f/200.0f, 0, 500);
    cmov (camx-MAX_POINTS/4, camy-MAX_POINTS/2, camz-MAX_POINTS);
#if TWO_WINDOWS
    prefposition (307, 600, 0, 300);
    windowId2 = winopen ("demo1 - singlebuffer");
    winconstraints ();
    /* NOTE: no doublebuffer() */
    gconfig ();
    perspective (600, 320.0f/200.0f, 0, 500);
    cmov (camx-MAX_POINTS/4, camy-MAX_POINTS/2, camz-MAX_POINTS);

    winset (windowId1);
#endif

#if DO_MENUS
    qdevice (RIGHTMOUSE);
    pup3 = defpup ("submenu 2 %t|Nothing here|Here neither");
    pup2 = defpup ("submenu 1 %t|Hello submenu 2 %m|Nothing here", pup3);
    pup1 = defpup ("IGL menu %t|Hello submenu 1 %m|%l|Exit", pup2);
    setpup(pup2, 1, PUP_GREY);
#endif

#if DO_PATTERN
    for (i=0; i < 1024; )
    {
        pattern[i] = GREEN;
        pattern[i+1] = BLUE;

        i += 2;
    }
    defpattern (1, 32, pattern);
#endif

    while (1)
    {

#if DO_MENUS
        if (qtest())
        {
            dev = qread (&val);
            if (dev == RIGHTMOUSE && val == 1 && dopup(pup1) == 3)
            {
                break;
            }
        }
#endif

        color (BLACK);
        clear ();

        color (GREEN);
        charstr ("Hello world 1");

        pushmatrix ();
        translate (camx-MAX_POINTS/4, camy+MAX_POINTS/2, camz+2);
        rot (xrot, 'x');
        rot (yrot, 'y');
        rot (zrot, 'z');
        xrot += 0.5f; yrot += 0.5f; zrot += 0.5f;

#if DO_PATTERN
        setpattern (1);
#endif

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
                rectf (0, 0, 1, 1);
                popmatrix ();

                color (CYAN);
            }
        }
        popmatrix ();

        swapbuffers ();


#if TWO_WINDOWS
        winset (windowId2);
        color (BLACK);
        clear ();

        color (GREEN);
        charstr ("Hello world 2");

        pushmatrix ();
        translate (camx-MAX_POINTS/4, camy-MAX_POINTS/2, camz-MAX_POINTS);
        rot (xrot, 'x');
        rot (yrot, 'y');
        rot (zrot, 'z');

#if DO_PATTERN
        setpattern (0);
#endif

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
                rectf (0, 0, 1, 1);
                popmatrix ();

                color (RED);
            }
        }

        popmatrix ();

        winset (windowId1);
#endif
    }

    return (0);
}



