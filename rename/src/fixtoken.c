
/* fixtoken - extract tokens from a string
   Version 1.1
   Version 1.2 2002-6-6 specify some GNU style macro definations

   Copyright (C) 1998-2002  Xuming <xuming@users.sourceforge.net>
   
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

/* AIX requires this to be the first thing in the file.  */
#if defined (_AIX) && !defined (__GNUC__)
 #pragma alloca
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdio.h>


static int isdelim(char *delim, int ch)
{
    while (*delim) {
	if (*delim == (char) ch)
	    return 1;
	else if ((*delim == ' ') && isspace(ch))
	    return 1;
	delim++;
    }
    return 0;
}


/* This function splits the string into tokens. Unlike strtok(), the 
   number of expected tokens has a fixed limitation. 
   This function splits everything between delimiter.
 
   sour  - the input string that is going to deconstruct to tokens
   idx   - the string array for storing tokens
   ids   - the maximem number of tokens, the size of "idx"
   delim - the delimiter array, each character in the array takes effect.

   It returns the number of token extracted.

   For example, fixtoken("#abc  wdc:have:::#:debug", idx, 16, "# :") returns
   10 tokens "", "abc", "", "wdc", "have", "", "", "", "" and "debug".
   
   BUGS:  'sour' is changed.
*/

int fixtoken(char *sour, char **idx, int ids, char *delim)
{
    int  i;

    for (i = 0; i < ids; idx[i++] = NULL);
    
    i = 0;
    for (idx[i++] = sour; *sour && (i < ids); sour++)  {

	if (isdelim(delim, *sour))  {
	    *sour = 0;
	    idx[i++] = sour + 1;
	}
    }
    return i;
}

/* This function splits the string into tokens. Unlike strtok(), the 
   number of expected tokens has a fixed limitation. 
   This function doesn't work like fixtoken(), it looks continent delimiters
   as one single delimter.
 
   sour  - the input string that is going to deconstruct to tokens
   idx   - the string array for storing tokens
   ids   - the maximem number of tokens, the size of "idx"
   delim - the delimiter array, each character in the array takes effect.

   It returns the number of token extracted.

   For example, fixtoken("#abc  wdc:have:::#:debug", idx, 16, "# :") returns
   4 tokens "abc", "wdc", "have" and "debug".
   
   BUGS:  'sour' is changed.
*/

int ziptoken(char *sour, char **idx, int ids, char *delim)
{
    int  i, ss;

    for (i = 0; i < ids; idx[i++] = NULL);

    for (i = ss = 0; *sour && (i < ids); sour++)  {
	
	if (isdelim(delim, *sour))  {
	    ss = 0;
	    *sour = 0;
	} else if (ss == 0) {
	    ss = 1;
	    idx[i++] = sour;
	}
    }
    
    return i;
}
    

#ifdef	EXECUTABLE

#ifdef  HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

int main(int argc, char **argv)
{
    char buf[256], *idx[32], *delim = " ";
    int  rs, i;

    if (argc > 1)  {
	if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
	    printf("Usage: %s [--help] [DELIM]\n", argv[0]);
	    return 0;
	} else
	    delim = argv[1];
    }
    
    while (fgets(buf, 256, stdin)) {

	buf[strlen(buf) - 1] = 0;
	
	rs = fixtoken(buf, idx, 32, delim);
	printf("%d items: ", rs);
	for (i = 0; i < rs; i++) 
	    printf("\"%s\", ", idx[i]);
	printf("\n");
    }
    return 0;
}

#endif

