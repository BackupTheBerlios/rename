
/*
   misc.c -- miscellany functions
	
   Copyright (C) 1998, 1999  Xuming <xuming@users.sourceforge.net>
   
   This program is free software; you can redistribute it and/or 
   modify it under the terms of the GNU General Public License as 
   published by the Free Software Foundation; either version 2, or 
   (at your option) any later version.
	   
   This program is distributed in the hope that it will be useful, 
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License, the file COPYING in this directory, for
   more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>



#ifndef	HAVE_STRCASECMP

int strcasecmp(char *sour, char *dest)
{
    char    a, b;

    while (1)  {
	if ((*sour < 'a') || (*sour > 'z'))  
	    a = *sour;
	else  
	    a = *sour - 'a' + 'A';
	
	if ((*dest < 'a') || (*dest > 'z'))  
	    b = *dest;
	else  
	    b = *dest - 'a' + 'A';
	
	if (a != b)  
	    return 1;
	if (a == '\0')  
	    break;
	sour++, dest++;
    }
    return 0;
}

#endif

#ifndef	HAVE_STRNCASECMP

int strncasecmp(char *sour, char *dest, int leng)
{
    int     i;
    char    a, b;

    for (i = 0; i < leng; i++, sour++, dest++)  {
        if ((*sour < 'a') || (*sour > 'z'))  
	    a = *sour;
        else  
	    a = *sour - 'a' + 'A';
	
        if ((*dest < 'a') || (*dest > 'z'))  
	    b = *dest;
        else  
	    b = *dest - 'a' + 'A';
	
        if (a != b)  
	    return 1;
        if (a == '\0')  
	    break;
    }
    return 0;
}

#endif


#ifndef	HAVE_STRCASESTR

char *strcasestr(const char *haystack, const char *needle)
{
    char  *p;
    int   nlen;

    if (!needle || !haystack)  
	return (char*) haystack;

    nlen = strlen(needle);
    for (p = (char*) haystack; *p; p++)  {
        if (!strncasecmp(p, needle, nlen))  
	    return p;
    }
    
    return NULL;
}

#endif

#ifndef	HAVE_STRSTR
#endif


char *dup_str(char *s)
{
    char *d, *tmp;

    if (!s)  
	return NULL;

    d = tmp = (char *) malloc(strlen(s) + 1);
    if ( d )
        while ((*d++ = *s++) != '\0');
    return tmp;
}


char *skip_space(char *sour)
{
    while (*sour && isspace(*sour))  
	sour++;
    return sour;
}



