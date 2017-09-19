/*
 * menu.c
 *
 * popup menu handling
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
#include <stdarg.h>
#include <ctype.h>


#if PLATFORM_X11
/* bitmaps for checkboxes. fukken X11 reversed bytes/bits, LSB/MSB this... */
static char menuBitmapUnchecked[] =
{
    0xff, 0xff, 0xff, 0xff, 0x03, 0xc0, 0x03, 0xc0,
    0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0,
    0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0,
    0x03, 0xc0, 0x03, 0xc0, 0xff, 0xff, 0xff, 0xff
};

static char menuBitmapChecked[] =
{
    0xff, 0xff, 0xff, 0xff, 0x03, 0xc0, 0x03, 0xc0,
    0x03, 0xc0, 0x03, 0xc8, 0x03, 0xcc, 0x23, 0xce,
    0x63, 0xc7, 0xe3, 0xc3, 0xc3, 0xc1, 0x83, 0xc0,
    0x03, 0xc0, 0x03, 0xc0, 0xff, 0xff, 0xff, 0xff
};


#if 0
____________________________ X11 helpers ____________________________
#endif
#define MENU_BORDERWIDTH        2       /* width of the border frame */
#define MENU_LRPADDING          12      /* left/right padding of menu item */
#define MENU_VPADDING           5       /* vertical padding of menu item */

static void
_igl_X11createPopupMenu (igl_menuT *menu)
{
    int wflags, fontIdx;
    XSetWindowAttributes swAttribs;
    XGCValues gcv;
    XColor color;
    XFontStruct *fontStruct;
    char *fontNames[] =
    {
        "-*-*-*-i-*-sans-12-*-*-*-*-*-*-*",
        "-*-fixed-*-*-*-*-*-*-*-*-*-*-iso8859-1",
        "-*-*-*-*-*-*-*-*-*-*-*-*-*-*"
    };

    /* menu background color is light gray */
    color.red = color.green = color.blue = 49152;
    XAllocColor (IGL_DISPLAY, DefaultColormap (IGL_DISPLAY, IGL_SCREEN), &color);

    wflags = CWBorderPixel | CWBackPixel | CWEventMask | CWOverrideRedirect | CWColormap; /* | CWCursor; */
    IGL_CLR (&swAttribs);
    swAttribs.border_pixel = 0;
    swAttribs.background_pixel = color.pixel;
    swAttribs.event_mask = KeyPressMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
        EnterWindowMask | LeaveWindowMask; /* | ExposureMask; */
    swAttribs.override_redirect = True;
    swAttribs.colormap = XCreateColormap(IGL_DISPLAY, IGL_ROOT, IGL_VISUAL->visual, AllocNone);

    /* create the menu window */
    menu->width = 0;
    menu->height = 0;
    menu->hmenu = XCreateWindow (IGL_DISPLAY, IGL_ROOT,
        10, 10, 10, 10, 0, IGL_VISUAL->depth, InputOutput, IGL_VISUAL->visual, wflags, &swAttribs);

    if (menu->hmenu == IGL_NULLMENU)
        return;


    /* allocate menu GCs */
    /* text */
    wflags = GCFont;

    /*
        now this is utterly stupid. X returns XFontStruct, which we don't want to keep
        in memory, so we free it. but if it's free'd, we have to load the font again.
        if, on the other hand, only XLoadFont() is used to load the font and the font
        doesn't exist, X returns an 'error' and the program quits. aargh, API designers...
    */
    fontIdx = 0;
    while (1)
    {
        fontStruct = XLoadQueryFont (IGL_DISPLAY, fontNames[fontIdx]);
        if (fontStruct != NULL)
            break;
        fontIdx ++;
    }
    XFreeFont (IGL_DISPLAY, fontStruct);

    gcv.font = XLoadFont (IGL_DISPLAY, fontNames[fontIdx]);
    menu->textGC = XCreateGC (IGL_DISPLAY, menu->hmenu, wflags, &gcv);

    wflags = GCForeground;
    /* same as window background_pixel - light gray */
    gcv.foreground = color.pixel;
    menu->backgroundGC = XCreateGC (IGL_DISPLAY, menu->hmenu, wflags, &gcv);

    /* white border */
    gcv.foreground = WhitePixel (IGL_DISPLAY, IGL_SCREEN);
    menu->borderWhiteGC = XCreateGC (IGL_DISPLAY, menu->hmenu, wflags, &gcv);

    /* dark gray border + font for the title */
    wflags |= GCFont;
    color.red = color.green = color.blue = 32768;
    XAllocColor (IGL_DISPLAY, DefaultColormap (IGL_DISPLAY, IGL_SCREEN), &color);
    gcv.foreground = color.pixel;
    menu->borderGrayGC = XCreateGC (IGL_DISPLAY, menu->hmenu, wflags, &gcv);

    /* allocate pixmaps for (un)checked menu items */
    XGetGCValues (IGL_DISPLAY, menu->backgroundGC, GCForeground, &gcv);
    menu->itemUnchecked = XCreatePixmapFromBitmapData (IGL_DISPLAY, menu->hmenu, menuBitmapUnchecked,
        16, 16, BlackPixel (IGL_DISPLAY, IGL_SCREEN),
        gcv.foreground, DefaultDepth (IGL_DISPLAY, IGL_SCREEN));
    menu->itemChecked = XCreatePixmapFromBitmapData (IGL_DISPLAY, menu->hmenu, menuBitmapChecked,
        16, 16, BlackPixel (IGL_DISPLAY, IGL_SCREEN),
        gcv.foreground, DefaultDepth (IGL_DISPLAY, IGL_SCREEN));
}


static void
_igl_X11calculatePopupMenuSize (igl_menuT *menu)
{
    int i, maxItemWidth, maxItemHeight, titleWidth, direction, ascent, descent;
    XCharStruct charStruct;
    GContext gcontext;
    igl_menuItemT *item;

    gcontext = XGContextFromGC (menu->textGC);

    /* first pass: calculate max item width and height */
    maxItemWidth = maxItemHeight = 0;
    titleWidth = 0;
    menu->hasCheckboxes = False;
    if (menu->title[0] != '\0')
    {
        XQueryTextExtents (IGL_DISPLAY, gcontext, menu->title, strlen (menu->title), &direction,
            &ascent, &descent, &charStruct);
        titleWidth = maxItemWidth = charStruct.width;
        maxItemHeight = charStruct.ascent + charStruct.descent;
    }
    for (i=0; i < menu->numItems; i++)
    {
        item = &menu->items[i];

        if (item->title[0] != '\0')
        {
            XQueryTextExtents (IGL_DISPLAY, gcontext, item->title, strlen (item->title), &direction,
                &ascent, &descent, &charStruct);

            if (charStruct.width > maxItemWidth)
                maxItemWidth = charStruct.width;
            if (charStruct.ascent + charStruct.descent > maxItemHeight)
                maxItemHeight = charStruct.ascent + charStruct.descent;

            if (item->submenu != -1)
                maxItemWidth += MENU_LRPADDING;
            /* these are mutually exclusive with submenu and delimiter */
            else if ((item->flags & IGL_MENUITEMFLAGS_UNCHECKED) || (item->flags & IGL_MENUITEMFLAGS_CHECKED))
                menu->hasCheckboxes = True;
        }
    }

    if (maxItemWidth != 0)
        menu->width = maxItemWidth;
    menu->width += 2*MENU_BORDERWIDTH + 2*MENU_LRPADDING + (menu->hasCheckboxes ? 20 : 0);  /* 16 for pixmap + 4 for padding */
    menu->itemHeight = maxItemHeight + 2*MENU_BORDERWIDTH + 2*MENU_VPADDING;  /* 2 for selected item border */


    /* second pass: calculate item positions and menu height */
    menu->height = MENU_BORDERWIDTH;
    if (menu->title[0] != '\0')
    {
        menu->height += menu->itemHeight;
        /* menu title should be centered */
        menu->titlePosX = (int)((menu->width - titleWidth)/2.0f);
    }
    for (i=0; i < menu->numItems; i++)
    {
        item = &menu->items[i];
        item->y = menu->height;
        if (item->title[0] != '\0')
            menu->height += menu->itemHeight;
        else if (item->flags & IGL_MENUITEMFLAGS_DELIMITER)
            menu->height += 2;  /* 1 for shadow */
    }
    menu->height += MENU_BORDERWIDTH;
}


static void
_igl_X11drawPopupMenu (igl_menuT *menu, int selectedItem)
{
    int i, x0, x1, y0, y1;
    igl_menuItemT *item;

    /* 1. draw menu border/frame - 4 white and 4 gray lines for 3D effect */
    y0 = menu->height - 1;
    x1 = menu->width - 1;
    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, 0, 0, x1, 0);
    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, 1, 1, x1-1, 1);
    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, 0, 0, 0, y0);
    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, 1, 1, 1, y0-1);
    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x1, 0, x1, y0);
    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x1-1, 1, x1-1, y0-1);
    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, 0, y0, x1, y0);
    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, 1, y0-1, x1-1, y0-1);

    /* 2. draw menu title */
    if (menu->title[0] != '\0')
    {
        y0 = MENU_BORDERWIDTH + menu->itemHeight - 2*MENU_VPADDING;
        XDrawString (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, menu->titlePosX, y0, menu->title,
            strlen (menu->title));

        y0 += MENU_VPADDING + 2;
        /* double separator */
        x0 = MENU_BORDERWIDTH+1;
        x1 = menu->width - MENU_BORDERWIDTH - 2;
        XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x0, y0, x1, y0);
        XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x0, y0+1, x1, y0+1);
        XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, x0, y0+2, x1, y0+2);
    }
    /* 3. draw menu items */
    for (i=0; i < menu->numItems; i++)
    {
        item = &menu->items[i];
        if (item->title[0] != '\0')
        {
            if (i == selectedItem && !(item->flags & IGL_MENUITEMFLAGS_GRAYED))
            {
                /* draw a selection frame around our selected item, much like the menu window frame */
                x0 = MENU_BORDERWIDTH + 1;
                x1 = menu->width - MENU_BORDERWIDTH - 2;
                y0 = item->y + 2;
                y1 = y0 + menu->itemHeight - 4;

                XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, x0, y0, x1, y0);
                XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, x0+1, y0+1, x1-1, y0+1);
                XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, x0, y0, x0, y1);
                XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, x0+1, y0+1, x0+1, y1-1);
                XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x1, y0, x1, y1);
                XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x1-1, y0+1, x1-1, y1-1);
                XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x1, y1, x0, y1);
                XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x1-1, y1-1, x0+1, y1-1);
            }
            else
            {
                XFillRectangle (IGL_DISPLAY, menu->hmenu, menu->backgroundGC, MENU_BORDERWIDTH, item->y,
                    menu->width - MENU_BORDERWIDTH - 2, menu->itemHeight);
            }

            XDrawString (IGL_DISPLAY, menu->hmenu, (item->flags & IGL_MENUITEMFLAGS_GRAYED ? menu->borderGrayGC : menu->textGC),
                MENU_BORDERWIDTH + MENU_LRPADDING + (menu->hasCheckboxes ? 20 : 0),
                item->y + menu->itemHeight - MENU_BORDERWIDTH - MENU_VPADDING, item->title, strlen (item->title));

            /* draw the arrow for the submenu */
            if (item->submenu != -1)
            {
                x0 = menu->width - MENU_LRPADDING - 4;
                y0 = item->y + MENU_VPADDING + 4;
                y1 = y0 + 10;

                if (i == selectedItem && !(item->flags & IGL_MENUITEMFLAGS_GRAYED))
                {
                    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x0, y0, x0, y1);
                    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x0, y0, x0 + 7, y0 + 5);
                    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, x0, y1, x0 + 7, y0 + 5);
                }
                else
                {
                    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, x0, y0, x0, y1);
                    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, x0, y0, x0 + 7, y0 + 5);
                    XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, x0, y1, x0 + 7, y0 + 5);
                }
            }
            else if (item->flags & IGL_MENUITEMFLAGS_UNCHECKED)
            {
                XCopyArea (IGL_DISPLAY, menu->itemUnchecked, menu->hmenu, menu->backgroundGC,
                    0, 0, 16, 16, MENU_LRPADDING, item->y + MENU_VPADDING);
            }
            else if (item->flags & IGL_MENUITEMFLAGS_CHECKED)
            {
                XCopyArea (IGL_DISPLAY, menu->itemChecked, menu->hmenu, menu->backgroundGC,
                    0, 0, 16, 16, MENU_LRPADDING, item->y + MENU_VPADDING);
            }
        }
        else if (item->flags & IGL_MENUITEMFLAGS_DELIMITER)
        {
            XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderGrayGC, MENU_BORDERWIDTH+1, item->y, menu->width-MENU_BORDERWIDTH-1, item->y);
            XDrawLine (IGL_DISPLAY, menu->hmenu, menu->borderWhiteGC, MENU_BORDERWIDTH+1, item->y+1, menu->width-MENU_BORDERWIDTH-1, item->y+1);
        }
    }
}


static int
_igl_X11getMenuItemId (igl_menuT *menu, int x, int y)
{
    int i;
    igl_menuItemT *item;

    for (i=0; i < menu->numItems; i++)
    {
        item = &menu->items[i];
        if (item->title[0] != '\0' && !(item->flags & IGL_MENUITEMFLAGS_GRAYED) && x >= 0 && x <= menu->width && y >= item->y && y <= (item->y + menu->itemHeight))
            return (i);
    }

    return (-1);
}
#endif


#if 0
____________________________ IGL helpers ____________________________
#endif
static int
_igl_findMenuPlaceholder (void)
{
    int i;

    for (i=0; i < IGL_MAXMENUS; i++)
    {
        if (igl->menus[i].hmenu == IGL_NULLMENU)
        {
            IGL_CLR(&igl->menus[i]);
            return (i);
        }
    }

    return (-1);
}


static int
_igl_parseMenuItem (char *startPtr, char *endPtr, va_list *mlist, igl_menuT *menu, igl_menuItemT *item)
{
    int i;
    char *s;


    /* item 'title' needs special handling as it's not marked with %t as menu title is */
    if (item != NULL)
    {
        IGL_CLR(item);
        item->submenu = -1;
        item->id = -1;

        strncpy (item->title, startPtr, 255);
        s = strchr (item->title, '%');
        if (s != NULL)
        {
            *s = '\0';
            if (s-1 > item->title && *(s-1) == ' ')
                *(s-1) = '\0';
        }
    }

    i = 0;
    s = startPtr;
    while (s != NULL && s <= endPtr)
    {
        if (i)
        {
            switch (*s)
            {
            case 't':
#if 1
                strcpy (menu->title, "<- ");
                strncat (menu->title, startPtr, s-startPtr-1);
                strcat (menu->title, " ->");
#else
                strncpy (menu->title, startPtr, s-startPtr-1);
                menu->title[s-startPtr-1] = 0;
#endif
                item = NULL;
                break;

            case 'F':
                menu->routine = (int(*)(int, ...))va_arg (*mlist, void *);
                break;

            case 'f':
                if (item != NULL)
                    item->routine = (int(*)(int, ...))va_arg (*mlist, void *);
                break;

            case 'l':
                if (item != NULL)
                    item->flags |= IGL_MENUITEMFLAGS_DELIMITER;
                break;

            case 'm':
                if (item != NULL)
                {
                    item->submenu = (long)va_arg (*mlist, long);
                    if (item->submenu < 0 || item->submenu >= IGL_MAXMENUS || igl->menus[item->submenu].hmenu == IGL_NULLMENU)
                    {
                        /* invalid menu specified */
                        item->submenu = -1;
                    }
                }
                break;

            case 'n':
                if (item != NULL)
                    item->dontExecuteRoutine = 1;
                break;

            case 'x':
                if (item != NULL && isdigit(*(unsigned char *)(s+1)))
                {
                    item->id = atoi (s+1);
                    if (item->id < 0)
                        item->id = -1;
                }
                break;
            }
            i = 0;
        }
        else
            i = (*s == '%');

        s ++;
    }

    return (item != NULL);
}


static int
_igl_parseMenu (int pup, String str, va_list args)
{
    int id;
    char *startPtr, *endPtr, tmpStr[256];
    igl_menuT *menu;
    igl_menuItemT *item;

    /* if pup is specified, we're called from addtopup(), else from defpup() */
    if (pup >= 0 && pup < IGL_MAXMENUS)
        id = pup;
    else
    {
        id = _igl_findMenuPlaceholder ();
        if (id == -1)
            return (-1);
    }
    menu = &igl->menus[id];


    strncpy (tmpStr, str, 255);

    /*
        chop menu defines into pieces
    */
    startPtr = strtok (tmpStr, "|");
    endPtr = startPtr+strlen(startPtr)-1;

    /* parse menu and items */
    item = &menu->items[menu->numItems];
    while (startPtr != NULL && menu->numItems < IGL_MAXMENUITEMS)
    {
        if (_igl_parseMenuItem (startPtr, endPtr, &args, menu, item))
        {
            menu->numItems ++;
            item ++;
        }

        startPtr = strtok (NULL, "|");
        if (startPtr == NULL)
            break;
        endPtr = startPtr+strlen(startPtr)-1;
    }


#if PLATFORM_X11
    if (id != pup) /* if (id == pup) we were called from addtopup() */
    {
        /* create the menu w/items if any */
        _igl_X11createPopupMenu (menu);
    }

    _igl_X11calculatePopupMenuSize (menu);
#endif

    return (id);
}


#if 0
____________________________ menu functions ____________________________
#endif
/* GL: defpup - defines a menu */
long
defpup (String str, ...)
{
    int id;
    va_list mlist;

    _igl_trace (__func__,TRUE,"%p(%s),...",str,str?str:"");
    IGL_CHECKWND (-1);

    va_start (mlist, str);
    id = _igl_parseMenu (-1, str, mlist);
    va_end (mlist);

    return (id);
}


/* GL: newpup - allocates and initializes a structure for a new menu */
long
newpup (void)
{
    long id;

    _igl_trace (__func__,TRUE,"");
    IGL_CHECKWND (-1);

    id = _igl_findMenuPlaceholder ();
    if (id != -1)
#if PLATFORM_X11
        _igl_X11createPopupMenu (&igl->menus[id]);
#endif

    return (id);
}


/* GL: addtopup - adds items to an existing pop-up menu */
void
addtopup (long pup, String str, ...)
{
    va_list mlist;

    _igl_trace (__func__,TRUE,"%ld,%p(%s),...",pup,str,str?str:"");
    IGL_CHECKWNDV ();

    if (pup < 0 || pup >= IGL_MAXMENUS || igl->menus[pup].hmenu == IGL_NULLMENU)
        return;

    /* TBD: check if the menu was created with defpup() or newpup() */
    va_start (mlist, str);
    _igl_parseMenu (pup, str, mlist);
    va_end (mlist);
}


/* GL: dopup - displays the specified pop-up menu */
long
dopup (long pup)
{
    long ret = -1, id = -1;
    igl_menuT *menu;
    igl_menuItemT *item;
#if PLATFORM_X11
    XEvent event;
    Cursor cursor;
    /* <sarcasm>luckily newer compilers provide about a meg of stack.</sarcasm> */
    int i, mousex, mousey, rx, ry, x, y, selectedItem, newSelectedItem;
    unsigned int width, height, border_width, depth;
    Boolean processMenu;
    Window rootWnd, childWnd;
    unsigned int maskRet;
    igl_menuT *currentMenu, *childMenu;
#endif

    _igl_trace (__func__,TRUE,"%ld",pup);
    IGL_CHECKWND (-1);

    if (pup < 0 || pup >= IGL_MAXMENUS || igl->menus[pup].hmenu == IGL_NULLMENU)
        return (-1);

    menu = &igl->menus[pup];

#if PLATFORM_X11
    /* grab the mouse pointer */
    cursor = XCreateFontCursor (IGL_DISPLAY, XC_arrow);
    if (XGrabPointer (IGL_DISPLAY, IGL_ROOT,
            True, ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync,
            IGL_ROOT, cursor, CurrentTime) != GrabSuccess)
    {
        /* the cursor is grabbed by someone else, so bail out */
        XFreeCursor (IGL_DISPLAY, cursor);
        return (-1);
    }


    /* show the topmost menu window */
    XQueryPointer (IGL_DISPLAY, menu->hmenu, &rootWnd, &childWnd, &mousex, &mousey, &x, &y, &maskRet);
    x = DisplayWidth (IGL_DISPLAY, IGL_SCREEN) - menu->width;
    if (x > mousex) x = mousex;
    y = DisplayHeight (IGL_DISPLAY, IGL_SCREEN) - menu->height;
    if (y > mousey) y = mousey;
    XMoveResizeWindow (IGL_DISPLAY, menu->hmenu, x, y, menu->width, menu->height);
    XMapRaised (IGL_DISPLAY, menu->hmenu);
    _igl_X11drawPopupMenu (menu, -1);

    /* event processing loop */
    selectedItem = -1;
    processMenu = True;
    currentMenu = menu;
    while (processMenu)
    {
        if (XQueryPointer (IGL_DISPLAY, currentMenu->hmenu, &rootWnd, &childWnd, &rx, &ry, &x, &y, &maskRet) &&
            (rx != mousex || ry != mousey))
        {
            /* if mouse moved, update everything necessary on the screen */
            mousex = rx;
            mousey = ry;

            newSelectedItem = _igl_X11getMenuItemId (currentMenu, x, y);
            if (newSelectedItem != selectedItem)
            {
                /* if previous selected item had a submenu, unmap/hide it */
                if (selectedItem != -1 && currentMenu->items[selectedItem].submenu != -1)
                {
                    childMenu = &igl->menus[currentMenu->items[selectedItem].submenu];
                    XUnmapWindow (IGL_DISPLAY, childMenu->hmenu);
                }
                /* if new selected item has a submenu, map/show it */
                if (newSelectedItem != -1 && currentMenu->items[newSelectedItem].submenu != -1)
                {
                    childMenu = &igl->menus[currentMenu->items[newSelectedItem].submenu];
                    /*
                        yes, one must love API calls with at least 5 params. especially since none can
                        be NULL or we dump core. X11 must be a cult of non-NULL worshipers or something.
                        no, they don't redirect to /dev/null either... =)
                    */
                    XGetGeometry (IGL_DISPLAY, currentMenu->hmenu, &rootWnd, &rx, &ry, &width, &height,
                        &border_width, &depth);

                    /* position the 'child' menu beside parent's selected item and draw it */
                    x = DisplayWidth (IGL_DISPLAY, IGL_SCREEN) - childMenu->width;
                    if (x > rx + currentMenu->width)
                        x = rx + currentMenu->width;
                    y = DisplayHeight (IGL_DISPLAY, IGL_SCREEN) - childMenu->height;
                    if (y > ry + currentMenu->items[newSelectedItem].y)
                        y = ry + currentMenu->items[newSelectedItem].y;
                    XMoveResizeWindow (IGL_DISPLAY, childMenu->hmenu, x, y,
                        childMenu->width, childMenu->height);
                    XMapRaised (IGL_DISPLAY, childMenu->hmenu);
                    _igl_X11drawPopupMenu (childMenu, -1);
                }

                selectedItem = newSelectedItem;
                _igl_X11drawPopupMenu (currentMenu, selectedItem);
            }
        }

        
        XNextEvent (IGL_DISPLAY, &event);
        switch (event.type)
        {
            case KeyRelease:
            case KeyPress:
            break;

/*
    XQueryPointer() seems to be in sync only when a button is pressed,
    hence we use ButtonRelease and track the menu only when a button is held!
*/
#if 0
            case ButtonPress:
#else
            case ButtonRelease:
#endif
                /* we may see a stray old event here, verify button press */
                if (maskRet & event.xbutton.state)
                {
                    id = _igl_X11getMenuItemId (currentMenu, event.xbutton.x, event.xbutton.y);
                    /* hmm... if we release the mouse over an item with a submenu, should we return its id? */
                    if (id != -1 && currentMenu->items[id].submenu != -1)
                        id = -1;
                    processMenu = False;
                    maskRet &= ~event.xbutton.state;
                }
            break;

            case EnterNotify:
                if (event.xcrossing.window != None)
                {
                    for (i=0; i < IGL_MAXMENUS; i++)
                    {
                        if (igl->menus[i].hmenu == event.xcrossing.window)
                        {
                            for (id=0; id<igl->menus[i].numItems; id++)
                            {
                                childMenu = &igl->menus[igl->menus[i].items[id].submenu];
                                if (childMenu == currentMenu && childMenu->hmenu != IGL_NULLMENU)
                                    XUnmapWindow (IGL_DISPLAY, childMenu->hmenu);
                            }
                            selectedItem = -1;
                            currentMenu = &igl->menus[i];
                            break;
                        }
                    }
                }
            break;

            case LeaveNotify:
                selectedItem = -1;
            break;

            default:
            break;
        }
    }

    /* release the mouse pointer and hide the menu window (plus any children!) */
    XUngrabPointer (IGL_DISPLAY, CurrentTime);
    for (i=0; i < IGL_MAXMENUS; i++)
    {
        if (igl->menus[i].hmenu != IGL_NULLMENU)
            XUnmapWindow (IGL_DISPLAY, igl->menus[i].hmenu);
    }
    XFreeCursor (IGL_DISPLAY, cursor);
    XFlush (IGL_DISPLAY);

    /* update GL mouse devices to current state */
    igl->deviceVal[LEFTMOUSE] = !!(maskRet & Button1Mask);
    igl->deviceVal[MIDDLEMOUSE] = !!(maskRet & Button2Mask);
    igl->deviceVal[RIGHTMOUSE] = !!(maskRet & Button3Mask);

    menu = currentMenu;
#endif

    if (id < 0 || id >= IGL_MAXMENUITEMS)
        return (-1);

    item = &menu->items[id];

    /* 0. get value of menu item */
    if (item->id >= 0)
        ret = item->id;
    else
        ret = id+1;

    /* 1. menu item has a routine() defined -> call it */
    if (item->routine != NULL)
        ret = (*item->routine)(ret);

    /* 2. menu has a routine() defined -> call it, unless the item should not execute it */
    /* NOTE: if 1. is true, return value is passed as an argument to this routine */
    if (menu->routine != NULL && !item->dontExecuteRoutine)
        ret = (*menu->routine)(ret);

    return (ret);
}


/* GL: freepup - deallocates a menu */
void
freepup (long pup)
{
    _igl_trace (__func__,TRUE,"%ld",pup);
    IGL_CHECKWNDV ();

    if (pup < 0 || pup >= IGL_MAXMENUS || igl->menus[pup].hmenu == IGL_NULLMENU)
        return;

    /* TBD: if pup is a submenu, update its 'parent', too!!! */
#if PLATFORM_X11
    XFreeGC (IGL_DISPLAY, igl->menus[pup].textGC);
    XFreeGC (IGL_DISPLAY, igl->menus[pup].backgroundGC);
    XFreeGC (IGL_DISPLAY, igl->menus[pup].borderWhiteGC);
    XFreeGC (IGL_DISPLAY, igl->menus[pup].borderGrayGC);
    XFreePixmap (IGL_DISPLAY, igl->menus[pup].itemUnchecked);
    XFreePixmap (IGL_DISPLAY, igl->menus[pup].itemChecked);
    XDestroyWindow (IGL_DISPLAY, igl->menus[pup].hmenu);
#endif
    igl->menus[pup].hmenu = IGL_NULLMENU;
}


/* GL: setpup - sets the display characteristics of a given pop up menu entry */
void
setpup (long pup, long entry, unsigned long mode)
{
    igl_menuT *menu;
    igl_menuItemT *item;
#if PLATFORM_X11
    Boolean recalculateMenu;
#endif

    _igl_trace (__func__,TRUE,"%ld,%ld,%lu",pup,entry,mode);
    IGL_CHECKWNDV ();

    if (pup < 0 || pup >= IGL_MAXMENUS || igl->menus[pup].hmenu == IGL_NULLMENU)
        return;

    menu = &igl->menus[pup];
    if (entry <= 0 || entry > menu->numItems)
        return;

    item = &menu->items[entry-1];

#if PLATFORM_X11
    if (item->submenu != -1 && (mode == PUP_BOX || mode == PUP_CHECK))
        return;

    recalculateMenu = False;
    switch (mode)
    {
        case PUP_NONE:
            if ((item->flags & IGL_MENUITEMFLAGS_UNCHECKED) || (item->flags & IGL_MENUITEMFLAGS_CHECKED))
                recalculateMenu = True;
            item->flags &= ~IGL_MENUITEMFLAGS_UNCHECKED;
            item->flags &= ~IGL_MENUITEMFLAGS_CHECKED;
            item->flags &= ~IGL_MENUITEMFLAGS_GRAYED;
        break;

        case PUP_GREY:
            item->flags |= IGL_MENUITEMFLAGS_GRAYED;
        break;

        case PUP_BOX:
            if (!(item->flags & IGL_MENUITEMFLAGS_UNCHECKED) && !(item->flags & IGL_MENUITEMFLAGS_CHECKED))
                recalculateMenu = True;
            item->flags &= ~IGL_MENUITEMFLAGS_CHECKED;
            item->flags |= IGL_MENUITEMFLAGS_UNCHECKED;
        break;

        case PUP_CHECK:
            if (!(item->flags & IGL_MENUITEMFLAGS_UNCHECKED) && !(item->flags & IGL_MENUITEMFLAGS_CHECKED))
                recalculateMenu = True;
            item->flags &= ~IGL_MENUITEMFLAGS_UNCHECKED;
            item->flags |= IGL_MENUITEMFLAGS_CHECKED;
        break;
    }

    if (recalculateMenu)
        _igl_X11calculatePopupMenuSize (menu);
#endif
}

