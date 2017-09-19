/*
 * device.c
 *
 * device handling and device queue management, and mouse cursor handling
 *
 *
 * Copyright (c) 2004 Matevz Bradac
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


/* GL: getbutton - returns the state of a button */
Boolean
getbutton (Device dev)
{
    _igl_trace (__func__,TRUE,"%d",dev);
    IGL_CHECKWND (FALSE);

    if (dev > IGL_MAXDEVICES || !ISBUTTON(dev))
        return (FALSE);

    glFlush ();
    IGL_MSG_PROCESS

    return (igl->deviceVal[dev]);
}


/* GL: getvaluator - returns the current state of a valuator */
long
getvaluator (Device dev)
{
    _igl_trace (__func__,TRUE,"%d",dev);
    IGL_CHECKWND (0);

    if (dev > IGL_MAXDEVICES || !ISVALUATOR(dev))
        return (0);

    glFlush ();
    IGL_MSG_PROCESS

    return (igl->deviceVal[dev]);
}


/* GL: setvaluator - assigns an initial value and a range to a valuator */
void
setvaluator (Device v, short init, short vmin, short vmax)
{
    _igl_trace (__func__,TRUE,"%d,%d,%d,%d",v,init,vmin,vmax);
    IGL_CHECKWNDV ();

    if (!ISVALUATOR(v))
        return;

    igl->deviceVal[v] = init;
    /* what about vmin/vmax? should we count them in? */
}


/* GL: noise - filters valuator (mouse, cursor, and GL timer) motion */
void
noise (Device v, short delta)
{
    _igl_trace (__func__,TRUE,"%d,%d",v,delta);
    IGL_CHECKWNDV ();

    igl->deviceNoise[v] = delta;
    igl->deviceLast[v] = igl->deviceVal[v];
}


/* GL: getdev - reads a list of valuators at one time */
void
getdev (long n, Device devs[], short vals[])
{
    int i;

    _igl_trace (__func__,TRUE,"%ld,%p,%p",n,devs,vals);
    IGL_CHECKWNDV ();

    if (n <= 0 || n > 128)
        return;

    glFlush ();
    IGL_MSG_PROCESS

    for (i=0; i < n; i++)
    {
        if (devs[i] > IGL_MAXDEVICES)
            vals[i] = 0;
        else
            vals[i] = (short)(igl->deviceVal[devs[i]]);
    }
}


/* GL: qdevice - queues a device */
void
qdevice (Device dev)
{
    _igl_trace (__func__,TRUE,"%d",dev);
    IGL_CHECKWNDV ();

    if (dev < IGL_MAXDEVICES)
        igl->deviceQueue[dev] = 1;
}


/* GL: unqdevice - disables the specified device from making entries in the event queue */
void
unqdevice (Device dev)
{
    _igl_trace (__func__,TRUE,"%d",dev);
    IGL_CHECKWNDV ();

    if (dev < IGL_MAXDEVICES)
        igl->deviceQueue[dev] = 0;
}


/* GL: isqueued - returns whether the specified device is enabled for queuing */
Boolean
isqueued (Device dev)
{
    _igl_trace (__func__,TRUE,"%d",dev);
    IGL_CHECKWND (FALSE);

    return ((dev < IGL_MAXDEVICES) ? igl->deviceQueue[dev] : FALSE);
}


/* internal qenter worker, with or without noise filtering */
void
_igl_qenter (Device dev, short val, Boolean filter)
{
    short diff = igl->deviceVal[dev] - igl->deviceLast[dev];
    int qidx = igl->numQueuedEvents;

    /* if filtering is set and count not reached, do nothing */
    if (filter && abs(diff) < igl->deviceNoise[dev])
        return;
    igl->deviceLast[dev] = igl->deviceVal[dev];

    /* try to avoid duplicate REDRAW events */
    if (qidx <= 0 || dev != REDRAW ||
        igl->eventQueue[qidx*2-2] != dev || igl->eventQueue[qidx*2-1] != val)
    {
        if (qidx + 1 < IGL_MAXDEVQENTRIES)
        {
            /* queue not full, create entry */
            igl->eventQueue[qidx*2] = dev;
            igl->eventQueue[qidx*2+1] = val;
            igl->numQueuedEvents = qidx+1;
        }
        else if (igl->deviceQueue[QFULL])
        {
            /* report queue overflow in last entry if qdevice(QFULL) */
            igl->eventQueue[qidx*2-2] = QFULL;
            igl->eventQueue[qidx*2-1] = 1;
        }
    }
}


/* GL timer emulation, called from event handling loop */
void
_igl_glTimers (void)
{
    struct timeval currTime;
    long timeDiff;

    /* calculate how many timer ticks have passed since the last call */
    gettimeofday (&currTime, NULL);
    timeDiff = (currTime.tv_sec  - igl->lastTime.tv_sec ) * 1000 +
               (currTime.tv_usec - igl->lastTime.tv_usec) / 1000;
    /* IrisGL timers have a 15 ms tick */
    while (timeDiff >= 15)
    {
        if (igl->deviceQueue[TIMER0])
        {
            igl->deviceVal[TIMER0]--;
            _igl_qenter (TIMER0, 0, True);
        }
        if (igl->deviceQueue[TIMER1])
        {
            igl->deviceVal[TIMER1]--;
            _igl_qenter (TIMER1, 0, True);
        }
        if (igl->deviceQueue[TIMER2])
        {
            igl->deviceVal[TIMER2]--;
            _igl_qenter (TIMER2, 0, True);
        }
        if (igl->deviceQueue[TIMER3])
        {
            igl->deviceVal[TIMER3]--;
            _igl_qenter (TIMER3, 0, True);
        }
        timeDiff -= 1000/60;
        igl->lastTime.tv_usec += 1000000/60;
    }
    /* normalize timer */
    while (igl->lastTime.tv_usec >= 1000000)
    {
        igl->lastTime.tv_usec -= 1000000;
        igl->lastTime.tv_sec += 1;
    }
}


/* GL: qenter - creates an event queue entry */
void
qenter (Device dev, short val)
{
    _igl_trace (__func__,TRUE,"%d,%d",dev,val);
    IGL_CHECKWNDV ();

    _igl_qenter (dev, val, False);
}


/* GL: qtest - checks the contents of the event queue */
long
qtest (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (0);

    IGL_MSG_PROCESS

    if (igl->currentWindow < 0 || IGL_CTX ()->drawMode != NORMALDRAW ||
                !(IGL_CTX()->flags & IGL_WFLAGS_DOUBLEBUFFER))
        glFlush ();

    /* Some sleeping to avoid high speed polling */
    if (igl->numQueuedEvents == 0)
    {
        usleep (igl->yieldTime);

        IGL_MSG_PROCESS
    }

    return ((igl->numQueuedEvents > 0) ? igl->eventQueue[0] : 0);
}


/* GL: qreset - empties the event queue */
void
qreset (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

    igl->numQueuedEvents = 0;
}


/* GL: blkqread - reads multiple entries from the queue */
long
blkqread (short *data, short n)
{
    int numEvents, pendingEvents;

    _igl_trace (__func__,TRUE,"%p,%d",data,n);
    IGL_CHECKWND (0);

    if (data == NULL || n == 0 || igl->numQueuedEvents == 0)
        return (0);

    numEvents = (n > igl->numQueuedEvents) ? igl->numQueuedEvents : n;
    memcpy (data, igl->eventQueue, numEvents*sizeof(short)*2);

    pendingEvents = igl->numQueuedEvents - numEvents;
    if (pendingEvents > 0)
        memmove (igl->eventQueue, &igl->eventQueue[numEvents*2], pendingEvents*sizeof(short)*2);

    igl->numQueuedEvents = pendingEvents;
    return (numEvents*2);
}


/* GL: qread - reads the first entry in the event queue */
long
qread (short *data)
{
    long ret;
    short queue[2];

    _igl_trace (__func__,TRUE,"%p",data);
    IGL_CHECKWND (0);

    glFlush ();

    /* qread() blocks until something is present in the queue! */
    while (1)
    {
        /*
            we must block, but not window messages (WM_XXX)!
            otherwise, blkqread() would never return anything
            (since we handle device events in WndProc())
        */
        IGL_MSG_PROCESS

        ret = blkqread (queue, 1);
        if (ret != 0)
            break;
        else
        {
            /* wake up every 15ms if timers are running, else wait for event */
#if PLATFORM_X11
            int timers = igl->deviceQueue[TIMER0] | igl->deviceQueue[TIMER1] |
                         igl->deviceQueue[TIMER2] | igl->deviceQueue[TIMER3];
            struct timeval tv = { 0, 15000 };
            int fd = XConnectionNumber (IGL_DISPLAY);
            fd_set fds;

            FD_ZERO (&fds);
            FD_SET (fd, &fds);
            select (fd+1, &fds, NULL, NULL, (timers ? &tv : NULL));
#endif
        }
    }

    *data = queue[1];
    return (queue[0]);
}


/* GL: tie - ties two valuators to a button */
void
tie (Device b, Device v1, Device v2)
{
    _igl_trace (__func__,TRUE,"%d,%d,%d",b,v1,v2);
    IGL_CHECKWNDV ();

    /*
        tie() conditions:
        1. b has to be a button
        2. v1 has to be a valuator OR a NULLDEV
        3. v2 has to be a valuator OR a NULLDEV
        4. if v1 is NULLDEV, v2 must be too (untie)
        5. v2 can be NULLDEV even if v1 is not
    */
    if (!ISBUTTON(b) || (!ISVALUATOR(v1) && v1 != NULLDEV) ||
                (!ISVALUATOR(v2) && v2 != NULLDEV) ||
                (v1 == NULLDEV && v2 != NULLDEV))
        return;

    igl->tiedValuators[b*2] = v1;
    igl->tiedValuators[b*2+1] = v2;
}


/* GL: attachcursor - attaches the cursor to two valuators */
void
attachcursor (Device vx, Device vy)
{
    _igl_trace (__func__,TRUE,"%d,%d",vx,vy);
    IGL_CHECKWNDV ();

    igl->tiedValuators[CURSORX*2] = vx;
    igl->tiedValuators[CURSORY*2] = vy;
}


/* GL: qgetfd - get the read file descriptor associated with the event queue */
long
qgetfd (void)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (-1);

#if PLATFORM_X11
    return XConnectionNumber(IGL_DISPLAY);
#endif
}


/* set the cursor for a GL window */
static void
_igl_setCursor (igl_windowT *wptr, Cursor xid)
{
    int i;

    /* Set cursor for all attached layers, since these have their own cursors */
    for (i = IGL_WLAYER_CURSORDRAW-1; i >= IGL_WLAYER_NORMALDRAW; i--)
    {
#if PLATFORM_X11
        if (wptr->layers[i].hwnd)
            XDefineCursor (IGL_DISPLAY, wptr->layers[i].hwnd, xid);
#endif
    }
    /* TODO: setting cursor for newly attached layers */
}


/* update the color of a cursor from CURSORDRAW colormap */
void
_igl_recolorCursor (igl_windowT *wptr)
{
#if PLATFORM_X11
    XColor fg, bg;
    int idx = wptr->cursorIndex, color;

    if (idx > 0)
    {
        /* CCROSS uses color WHITE (== 3), see getgdesc(GD_CROSSHAIR_CINDEX) */
        color = (igl->cursors[idx].type == CCROSS ? 3 : 1);
        fg.red   = GetRValue(igl->cursorColorPalette[color]) * 256;
        fg.green = GetGValue(igl->cursorColorPalette[color]) * 256;
        fg.blue  = GetBValue(igl->cursorColorPalette[color]) * 256;
        bg.red = bg.green = bg.blue = 65535;
        XRecolorCursor (IGL_DISPLAY, igl->cursors[idx].xid, &fg, &bg);
    }
#endif
}


/* reverse bits in a byte */
static inline unsigned char _igl_byteRev(unsigned char v)
{
    static unsigned char rev[16] = {
        0x00, 0x08, 0x04, 0x0c, 0x02, 0x0a, 0x06, 0x0e,
        0x01, 0x09, 0x05, 0x0d, 0x03, 0x0b, 0x07, 0x0f };
    return rev[v >> 4] | rev[v & 0xf] << 4;
}


/* GL: curson, cursoff - control cursor visibility by window */
void
curson (void)
{
    igl_windowT *wptr;
    int idx;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    idx = wptr->cursorIndex;
    _igl_setCursor (wptr, idx ? igl->cursors[idx].xid : None);
    wptr->state &= ~IGL_WSTATE_CURSOROFF;
}


void
cursoff (void)
{
    static XColor black;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();

#if PLATFORM_X11
    /* X has no capability to switch the cursor off. Instead, use a transparant
     * cursor stored in igl cursor 0, which isn't used otherwise.
     */
    if (! igl->cursors->xid)
    {
        igl->cursors->src = XCreatePixmap(IGL_DISPLAY, IGL_ROOT, 1, 1, 1);
        igl->cursors->xid = XCreatePixmapCursor(IGL_DISPLAY,
                    igl->cursors->src, igl->cursors->src, &black, &black, 0, 0);
    }
#endif

    _igl_setCursor (IGL_CTX (), igl->cursors->xid);
    IGL_CTX ()->state |= IGL_WSTATE_CURSOROFF;
}


/* GL: curstype - defines the type and/or size of cursor */
void
curstype (long type)
{
    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    igl->cursorType = type;
}


/* GL: defcursor - defines a cursor glyph */
void
defcursor (short n, unsigned short *curs)
{
    char src[32*32/8], mask[32*32/8];
    int idx, w, h, i, j;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    /* GL has cursors with 1 or 3 colors, X11 Cursors have 2 colors. Hence,
     * only 1 color GL cursors are supported.
     */
    if (igl->cursorType != C16X1 && igl->cursorType != C32X1 && igl->cursorType != CCROSS)
        return;

    idx = IGL_MAPID(igl->cursors, IGL_MAXCURSORS, n, 3);
    if (idx < 0)
        return;

    /* drop old bitmaps and cursor */
#if PLATFORM_X11
    if (igl->cursors[idx].src)
        XFreePixmap (IGL_DISPLAY, igl->cursors[idx].src);
    if (igl->cursors[idx].mask)
        XFreePixmap (IGL_DISPLAY, igl->cursors[idx].mask);
    if (igl->cursors[idx].xid)
        XFreeCursor(IGL_DISPLAY, igl->cursors[idx].xid);
#endif
    igl->cursors[idx].xid = 0;

    h = (igl->cursorType == C32X1 ? 32 : 16);
    igl->cursors[idx].size = h;
    igl->cursors[idx].type = igl->cursorType;

    /* store bitmaps if this is a user defined cursor */
    if (igl->cursorType != CCROSS)
    {
#if PLATFORM_X11
        /* in GL 1st pixel is lower left, in X11 it's upper left
         * in GL 1st pixel in byte is MSB, in X11 1st pixel is LSB
         * need to mirror in y, and reverse bits in each byte for x :-(
         */
        w = h / (8 * sizeof(*curs));
        for (j = 1; j <= h; j++)
            for (i = 0; i < w; i++)
            {
                src [2*(h-j)*w + 2*i+0] = _igl_byteRev(curs[(j-1)*w + i] >> 8);
                src [2*(h-j)*w + 2*i+1] = _igl_byteRev(curs[(j-1)*w + i]);
                mask[2*(h-j)*w + 2*i+0] = _igl_byteRev(curs[(j-1)*w + i] >> 8);
                mask[2*(h-j)*w + 2*i+1] = _igl_byteRev(curs[(j-1)*w + i]);
            }

        igl->cursors[idx].src  = XCreatePixmapFromBitmapData(IGL_DISPLAY,
                                                IGL_ROOT, src, h, h, 1, 0, 1);
        igl->cursors[idx].mask = XCreatePixmapFromBitmapData(IGL_DISPLAY,
                                                IGL_ROOT, mask, h, h, 1, 0, 1);
#endif
    }
}


/* GL: curorigin - sets the origin of a cursor */
void
curorigin (short n, short xo, short yo)
{
    int idx;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKINIT ();

    idx = IGL_MAPID(igl->cursors, IGL_MAXCURSORS, n, 0);
    if (idx < 0)
        return;

    /* in GL 1st pixel is lower left, in X11 it's upper left - mirror in y */
    igl->cursors[idx].xori = xo;
    igl->cursors[idx].yori = igl->cursors[idx].size - yo;

    /* drop old cursor definition since parameters have changed */
    if (igl->cursors[idx].xid)
    {
#if PLATFORM_X11
        XFreeCursor(IGL_DISPLAY, igl->cursors[idx].xid);
#endif
        igl->cursors[idx].xid = 0;
    }
}


/* GL: setcursor - sets the cursor characteristics */
void
setcursor (short n, Colorindex color, Colorindex wtm)
{
    igl_windowT *wptr;
    int idx;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWNDV ();
    wptr = IGL_CTX ();

    idx = IGL_MAPID(igl->cursors, IGL_MAXCURSORS, n, 0);
    if (idx < 0)
        return;

    wptr->cursorIndex = idx;
    if (idx == 0)
    {
        /* system cursor */
        _igl_setCursor (wptr, None);
    }
    else if (igl->cursors[idx].type == CCROSS)
    {
        /* crosshair cursor */
        if (!igl->cursors[idx].xid)
        {
#if PLATFORM_X11
            /* NOTE: the SGI crosshair lines extend over the whole screen, which
             * isn't supported directly by X. Use 16x16 XC_crosshair instead.
             */
            igl->cursors[idx].xid = XCreateFontCursor (IGL_DISPLAY, XC_crosshair);
#endif
        }
        _igl_setCursor (wptr, igl->cursors[idx].xid);
        _igl_recolorCursor (wptr);
    }
    else
    {
        /* user defined cursor. create the cursor if it doesn't exist */
        if (!igl->cursors[idx].xid)
        {
#if PLATFORM_X11
            XColor fg, bg;
            fg.red   = GetRValue(igl->cursorColorPalette[1]) * 256;
            fg.green = GetGValue(igl->cursorColorPalette[1]) * 256;
            fg.blue  = GetBValue(igl->cursorColorPalette[1]) * 256;
            bg.red = bg.green = bg.blue = 65535;
            igl->cursors[idx].xid = XCreatePixmapCursor(IGL_DISPLAY,
                        igl->cursors[idx].src, igl->cursors[idx].mask, &fg, &bg,
                        igl->cursors[idx].xori, igl->cursors[idx].yori);
#endif
        }
        _igl_setCursor (wptr, igl->cursors[idx].xid);
    }
}


/* GL: getcursor - returns the cursor characteristics */
void
getcursor (short *n, Colorindex *color, Colorindex *wtm, Boolean *vis)
{
    _igl_trace (__func__,TRUE,"");

    *n = igl->cursors[IGL_CTX ()->cursorIndex].id;
    *vis = (IGL_CTX ()->state & IGL_WSTATE_CURSOROFF ? False : True);
    /* TODO: color, wtm? */
}


/* GL: obsolete routines */
void
RGBcursor (short index, short red, short green, short blue, short redm, short greenm, short bluem)
{
    _igl_trace (__func__,TRUE,"");

    igl->cursorColorPalette[1] = RGB (red, green, blue);
    setcursor(index, 1, 0);
    /* TODO: redm,greenm,bluem? */
}


void
gRGBcursor (short *index, short *red, short *green, short *blue, short *redm, short *greenm, short *bluem, Boolean *vis)
{
    Colorindex color, wtm;

    _igl_trace (__func__,TRUE,"");

    *red   = GetRValue(igl->cursorColorPalette[1]);
    *green = GetGValue(igl->cursorColorPalette[1]);
    *blue  = GetBValue(igl->cursorColorPalette[1]);
    getcursor(index, &color, &wtm, vis);
    /* TODO: redm,greenm,bluem? */
}


/* GL: unimplemented stuff */
void
qcontrol (long cmd, long icnt, short idata[], long ocnt, short odata[])
{
    _igl_trace (__func__,TRUE,"");
}


void
gammaramp (short r[256], short g[256], short b[256])
{
    _igl_trace (__func__,TRUE,"");
}


void
blankscreen (Boolean b)
{
    _igl_trace (__func__,TRUE,"");
}


void
blanktime (long count)
{
    _igl_trace (__func__,TRUE,"");
}


void
clkon (void)
{
    _igl_trace (__func__,TRUE,"");
}


void
clkoff (void)
{
    _igl_trace (__func__,TRUE,"");
}


void
lampon (Byte lamps)
{
    _igl_trace (__func__,TRUE,"");
}


void
lampoff (Byte lamps)
{
    _igl_trace (__func__,TRUE,"");
}


void
ringbell (void)
{
    _igl_trace (__func__,TRUE,"");
    /* avoid a gcc compiler warning here */
    if (write (1, "\a", 1)) {}
}


void
setbell (Byte durat)
{
    _igl_trace (__func__,TRUE,"");
}


void
devport (Device dev, long port)
{
    _igl_trace (__func__,TRUE,"");
}


Boolean
setfastcom (void)
{
    _igl_trace (__func__,TRUE,"");
    return 0;
}


Boolean
setslowcom (void)
{
    _igl_trace (__func__,TRUE,"");
    return 0;
}


void
dbtext (String str)
{
    _igl_trace (__func__,TRUE,"");
}


void
setdblights (unsigned long mask)
{
    _igl_trace (__func__,TRUE,"");
}


void
setvideo (long reg, long val)
{
    _igl_trace (__func__,TRUE,"");
}


long
getvideo (long reg)
{
    _igl_trace (__func__,TRUE,"");
    return -1;
}


void
videocmd (long cmd)
{
    _igl_trace (__func__,TRUE,"");
}


void
setmonitor (short mtype)
{
    _igl_trace (__func__,TRUE,"");
}


long
getmonitor (void)
{
    _igl_trace (__func__,TRUE,"");
    return 1; /*HZ60*/
}


long
getothermonitor (void)
{
    _igl_trace (__func__,TRUE,"");
    return 0;
}


void
foreground (void)
{
    _igl_trace (__func__,TRUE,"");
}


long
getwscrn (void)
{
    _igl_trace (__func__,TRUE,"");
    /* we're always on screen 0, i guess */
    return (0);
}


void
scrnselect (long gsnr)
{
    _igl_trace (__func__,TRUE,"");
}


void
scrnattach (long gsnr)
{
    _igl_trace (__func__,TRUE,"");
}


void
textinit (void)
{
    _igl_trace (__func__,TRUE,"");
}


void
textport (Screencoord left, Screencoord right, Screencoord bottom, Screencoord top)
{
    _igl_trace (__func__,TRUE,"");
}


void
gettp (Screencoord *left, Screencoord *right, Screencoord *bottom, Screencoord *top)
{
    _igl_trace (__func__,TRUE,"");
}


void
textcolor (Colorindex tcolor)
{
    _igl_trace (__func__,TRUE,"");
}


void
textwritemask (Colorindex tmask)
{
    _igl_trace (__func__,TRUE,"");
}


void
pagecolor (Colorindex pcolor)
{
    _igl_trace (__func__,TRUE,"");
}


void
pagewritemask (Colorindex pmask)
{
    _igl_trace (__func__,TRUE,"");
}


void
tpon (void)
{
    _igl_trace (__func__,TRUE,"");
}


void
tpoff (void)
{
    _igl_trace (__func__,TRUE,"");
}
