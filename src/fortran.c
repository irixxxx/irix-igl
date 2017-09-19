/*
 * fortran.c
 *
 * interface for some functions not covered by SGI's libfgl.a
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

#include <igl.h>

#include <alloca.h>
#include <string.h>


/* IRIX fortran libfgl.a compatibility module.
 * These functions have a string argument of variable length, which is passed
 * differently in f77. There's an explicit length information, and no zero
 * termination.
 */


/* fortran system dependant symbol mapping */
#define FTN(n)  n

/* make a C string out of an F77 string */
#define Cstring(s,a,l)  char *s = alloca((l)+1); memcpy (s,a,l); s[l] = '\0';


long
FTN(winope) (String windowTitle, int length)
	{ Cstring (s, windowTitle, length); return winopen (s); }

void
FTN(wintit) (String name, int length)
	{ Cstring (s, name, length); wintitle (s); }

void
FTN(getpor) (String name, int length)
	{ Cstring (s, name, length); getport (s); }

void
FTN(iconti) (String name, int length)
	{ Cstring (s, name, length); icontitle (s); }

long
FTN(dglope) (String srvname, int length, long type)
	{ Cstring (s, srvname, length); return dglopen (s, type); }

void
FTN(charst) (String str, int length)
	{ Cstring (s, str, length); charstr (s); }

void
FTN(lchstr) (long type, void *str, int length)
	{ Cstring (s, str, length); lcharstr (type, s); }

long
FTN(strwid) (String str, int length)
	{ Cstring (s, str, length); return strwidth (s); }

long
FTN(lstwid) (long type, String str, int length)
	{ Cstring (s, str, length); return lstrwidth (type, s); }

void
FTN(addtop) (long pup, String str, int length, int arg)
	{ Cstring (s, str, length); addtopup (pup, s, arg); }

void
FTN(getsb) (long *left, long *right, long *bottom, long *top)
        { getscrbox (left, right, bottom, top); }
