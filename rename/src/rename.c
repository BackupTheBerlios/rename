/*
    rename.c -- file rename tool

    Copyright 1998,1999  Xuming <xuming@users.sourceforge.net>

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
#include <getopt.h>
#include <pwd.h>
#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>


#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#if !HAVE_SETLOCALE
#define setlocale(Category, Locale) /* empty */
#endif

#if ENABLE_NLS
  #include <libintl.h>
  #define _(String) gettext(String)
  #define gettext_noop(String) (String)
  #define N_(String) gettext_noop(String)
#else
  #define _(String) (String)
  #define N_(String) (String)
  #define bindtextdomain(Domain, Directory) /* empty */
  #define textdomain(Domain) /* empty */
#endif

#ifndef errno
extern int errno;
#endif

#ifdef  STDC_HEADERS
#include <stdlib.h>
#else   /* Not STDC_HEADERS */
extern void exit ();
extern char *malloc ();
#endif  /* STDC_HEADERS */

#ifdef  HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#ifdef  __GNUC__
#undef  alloca
#define alloca(n)       __builtin_alloca (n)
#else   /* Not GCC.  */
#ifdef  HAVE_ALLOCA_H
#include <alloca.h>
#else   /* Not HAVE_ALLOCA_H.  */
#ifndef _AIX
extern char *alloca ();
#endif  /* Not _AIX.  */
#endif  /* HAVE_ALLOCA_H.  */
#endif  /* GCC.  */

#ifdef HAVE_DIRENT_H
  #include <dirent.h>
  #define NAMLEN(dirent) strlen((dirent)->d_name)
#else
  #define dirent direct
  #define NAMLEN(dirent) (dirent)->d_namlen
  #ifdef HAVE_SYS_NDIR_H
    #include <sys/ndir.h>
  #endif
  #ifdef HAVE_SYS_DIR_H
    #include <sys/dir.h>
  #endif
  #ifdef HAVE_NDIR_H
    #include <ndir.h>
  #endif
#endif

#include "rename.h"
  
int 	prompt = 0;	/* 0: ask; 1, No to all; other: Yes to all */
int 	oper = 0;	/* the operation: lowcase, upcase, substitute */
int	attr = 0;	/* some attributes of the operation */


regex_t preg[1];	/* see manpage regex(7) */
char	*patnbuf;
char	*pattern;	/* memeory for storing fixed pattern */
int 	pnlen;
char	*subst;
int	stlen;		/* length of substitute and pattern string */

struct passwd *pwd = NULL;

char	cwd[SVRBUF];	/* current working directory */


void recursive(char *path);
int  change_name(char *oldname);
int  match_regexpr(char *str, int n);
int  match_pattern(char *str, int n);
int  match_backward(char *str, int n);
int  setpattern(char *arg);
int  do_rename(char *old, char *new);
void sigbreak(int sig);
void usage(int mode);

char* (*StrStr)(const char *, const char *);
int   (*StrnCmp)(const char *, const char *, size_t);

int main(int argc, char **argv)
{
    int 	recurs = 0, c;
    struct	stat	fs;
    char	*sopt = "hVtluRvs:o:";
    struct	option	lopt[] = {
	{ "lowcase",	0, NULL, 'l' },
	{ "upcase", 	0, NULL, 'u' },
	{ "recursive", 	0, NULL, 'R' },
	{ "owner",      1, NULL, 'o' },
	{ "verbose", 	0, NULL, 'v' },
	{ "test", 	0, NULL, 't' },
	{ "help", 	0, NULL, 'h' },
	{ "version", 	0, NULL, 'V' },
	{ "yes",  	0, NULL, 2 },
	{ "no", 	0, NULL, 1 },
	{ NULL, 0, NULL, 0 }
    };

    StrStr  = strstr;
    StrnCmp = strncmp;

#ifdef HAVE_SETLOCALE
    /* Set locale via LC_ALL.  */
    setlocale(LC_ALL, "");
#endif

#if ENABLE_NLS
    /* Set the text message domain.  */
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
#endif
	
      
    
    while ((c = getopt_long(argc, argv, sopt, lopt, NULL)) != EOF)  {
	switch (c) {
	case 'h':
	    usage(0);
	    return 0;
	    
	case 'V':
	    usage(1);
	    return 0;

	case 1:
	case 2:
	    prompt = c;
	    break;
	    
	case 'l':
	    oper = ACT_LOWCASE;
	    break;
	    
	case 'u':
	    oper = ACT_UPCASE;
	    break;
	    
	case 's':
	    if (setpattern(optarg))  
		return -1;
	    break;
	    
	case 'o':
	    if (geteuid() != 0)  {
		printf(_("Only super user can change file's owner!\n"));
		return 1;
	    }			
	    pwd = getpwnam(optarg);
	    if (pwd == NULL) {
		printf(_("No this user in current system [%s]\n"), optarg);
		return 1;
	    } else
		attr |= MOD_OWNER;
	    break;
	    
	case 't':
	    attr |= MOD_VERBO | MOD_TEST;
	    break;
	    
	case 'v':
	    attr |= MOD_VERBO;
	    break;
	    
	case 'R':
	    recurs = 1;
	    break;
	}
    }
    
    if ((oper == ACT_DEFT) && (attr & MOD_OWNER))
	oper = ACT_OWNER;
	
    if (oper == ACT_DEFT)  {
	    
 	/* just simplely change one filename to another */

	if ((optind + 2) > argc)  {
	    printf(_("%s: missing file arguments.\n"), argv[0]);
	    return 1;
	} else if ((optind + 2) < argc)  {	
	    printf(_("%s: too many arguments.\n"), argv[0]);
	    return 2;
	} else if (strcmp(argv[optind], argv[optind+1])) 
	    do_rename(argv[optind], argv[optind+1]);
	
	return 0;
    }

    if (optind >= argc)  {
	printf(_("%s: missing file arguments.\n"), argv[0]);
	return 1;
    }
	
    if (getcwd(cwd, SVRBUF) == NULL)  {
	printf(_("Out of path!\n"));
	return -1;
    }
    
    signal(SIGINT, sigbreak);
    signal(SIGHUP, sigbreak);
    signal(SIGQUIT, sigbreak);
    signal(SIGTERM, sigbreak);

    for ( ; optind < argc; optind++)  {

	if (recurs)  {
	    if (stat(argv[optind], &fs) < 0)  {
		perror("stat");
		continue;
	    }
	    if (S_ISDIR(fs.st_mode))  {
		recursive(argv[optind]);
		chdir(cwd);
	    }
	}
	change_name(argv[optind]);
    }
    
    if (oper == ACT_REG)  
	regfree(preg);
    
/*    chdir(cwd);*/
    return 0;
}


void recursive(char *path)
{
    DIR 	*dir;
    struct	stat	fs;
    struct	dirent	*de;

    if (attr & MOD_VERBO)  
	printf(_("Entering directory [%s]\n"), path);
    
    if (chdir(path) < 0)  {
	perror("chdir");
	return;
    }
    
    if ((dir = opendir(".")) == NULL)  {
	perror("opendir");
	return;
    }
    while((de = readdir(dir)) != NULL)  {
	    
    	if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))  
	    continue;
    	if (stat(de->d_name, &fs) < 0)  
	    continue; 	/* maybe permission denied */
	
    	if (S_ISDIR(fs.st_mode))  
	    recursive(de->d_name);
    	change_name(de->d_name);
    }
    closedir(dir);
    
    chdir("..");
    if (attr & MOD_VERBO)  
	printf(_("Leaving directory [%s]\n"), path);
}


/* according to the defined ruler, this routine spawns a new filename 
 * and changes old one to this one. */

int change_name(char *oldname)
{
    char	*p, new[SVRBUF];
    int		n;
    
    strncpy(new, oldname, SVRBUF);
    new[SVRBUF-1]='\0';

    /* index the actual filename, not the full path */
    
    if ((p = strrchr(new, '/')) == NULL)  
	p = new;  
    else  
	p++;
    
    if (!strcmp(p, ".") || !strcmp(p, ".."))  
	return 1;
    
    n = new + SVRBUF - p;
    
    switch (oper)  {
    case ACT_LOWCASE:
        while (*p)  {
	    *p = tolower(*p);
    	    p++;
    	}
	break;

    case ACT_UPCASE:
        while (*p)  {
    	    *p = toupper(*p);
    	    p++;
    	}
	break;

    case ACT_SUFFIX:
	n -= strlen(p) - pnlen;
	p += strlen(p) - pnlen;
	if (!StrnCmp(p, pattern, pnlen))
	{
	    strncpy(p, subst, n);
	    p[n-1]='\0';
	}
	break;
	
    case ACT_REG:
        match_regexpr(p, n);
	break;
	
    case ACT_SUBT:
	match_pattern(p, n);
	break;

    case ACT_BACKWD:
	match_backward(p, n);
	break;

    case ACT_OWNER:
	if (attr & MOD_TEST)
	    break;
	if (chown(new, pwd->pw_uid, pwd->pw_gid) < 0)
	    perror("chown");
	break;
    }
    
    if (!strcmp(oldname, new))  
	return 1;
    
    return do_rename(oldname, new);
}


/* to match a null-terminated string against the precompiled pattern buffer.
   When successed, it substitutes matches with the second parameter so
   the original string with enough buffer will be modified.
   Note: precompiled pattern buffer be set to globel.
   If no matches in the string, return 0.
*/

int  match_regexpr(char *str, int n)
{
    int		rs = 0;
    char	tmp[SVRBUF];
    regmatch_t	pmatch[1];

    while (!regexec(preg, str, 1, pmatch, 0))  {
	strncpy(tmp, str + pmatch->rm_eo, SVRBUF);
	tmp[SVRBUF-1]='\0';
	strncpy(str + pmatch->rm_so, subst, n - pmatch->rm_so);
	str[pmatch->rm_so+(n-pmatch->rm_so)-1]='\0';
	n -= strlen(str);
	str += strlen(str);
	strncat(str, tmp, n);
	rs++;
	if ((attr & MOD_REPT) == 0)  
	    break;
    }
    return rs;
}


int  match_pattern(char *str, int n)
{
    char  tmp[SVRBUF], *p;
    int   rs = 0;

    while ((p = StrStr(str, pattern)) != NULL)  {
	n -= p - str;
	strncpy(tmp, p + pnlen, SVRBUF);
	tmp[SVRBUF-1]='\0';
	strncpy(p, subst, n);
	if (n) p[n-1]='\0';

	p += stlen;
	n -= stlen;
	strncpy(p, tmp, n);
	if (n) p[n-1]='\0';

	str = p;

	rs++;
	if ((attr & MOD_REPT) == 0)  
	    break;
    }
    return rs;
}


int match_backward(char *str, int n)
{
    char  tmp[SVRBUF], *p;
    int   rs, room, l;

    /* deal some special situation */
    
    rs = strlen(str) - pnlen;
    if (rs < 0)  
	return 0;	/* the pattern is longer than dest. string */
    
    if (rs == 0)  
    {
	if (StrStr(str, pattern))  
	{
	    l = (stlen < n) ? stlen : n ;
	    strncpy(str, subst, l);
	    if ( l ) str[l-1]='\0';
	    return 1;
	}
	return 0;
    }
    
    room = n - rs;
    for (p = str + rs, rs = 0; p >= str; p--, room++)  {
	    
	if (!StrnCmp(p, pattern, pnlen))  
	{	
	    strncpy(tmp, p + pnlen, SVRBUF);
	    tmp[SVRBUF-1]='\0';
	    strncpy(p, subst, room);
	    if ( room ) p[room-1]='\0';
	    l = room - stlen;
	    strncpy(p + stlen, tmp, l);
	    if ( l ) p[stlen+l-1]='\0';
	    rs++;
	    if ((attr & MOD_REPT) == 0)  
		break;
	}
    }
    return rs;
}    
    

int setpattern(char *arg)
{
    char *idx[4], *p; 
    int  cflags = 0;

    patnbuf = dup_str(arg);
    fixtoken(patnbuf, idx, 4, "/");
    
    pattern = idx[1];
    if (pattern == NULL)  
	return -1;
    else 
	pnlen = strlen(pattern);
    
    subst = idx[2];
    if (subst == NULL)
	return -1;
    else
	stlen = strlen(subst);
    
    oper = ACT_SUBT;
    for (p = idx[3]; p && *p; p++)  {
	switch (*p)  {
	case 'g':
	case 'G':
	    attr |= MOD_REPT;
	    break;
	
	case 'b':
	case 'B':
	    oper = ACT_BACKWD;
	    break;

	case 's':
	case 'S':
	    oper = ACT_SUFFIX;
	    break;

	case 'i':
	case 'I':
	    attr |= MOD_ICASE;
	    StrnCmp = strncasecmp;
	    StrStr = strcasestr;
	    cflags |= REG_ICASE;
	    break;
	
	case 'r':
	case 'R':
	    oper = ACT_REG;
	    break;

	case 'e':
	case 'E':
	    oper = ACT_REG;
	    cflags |= REG_EXTENDED;
	    break;
	}
    }

    if (oper == ACT_REG)  {
	if (regcomp(preg, pattern, cflags))  {
	    printf(_("Compiling regular expression failed. [%s]\n"), pattern);
	    return -2;
	}
    }
    
/*    if (attr & MOD_TEST) 
	printf("pattern: %s subst: %s refs: %s\n", pattern, subst, idx[2]);*/
    
    return 0;
}


/* returns: 0 rename ok, skip or test. others means work failed */

int do_rename(char *old, char *new)
{
    struct stat	fs;
    char *p, buf[64];
    int  rs = 0;

    if (attr & MOD_VERBO)  
	printf(_("rename %-20s => %-20s : "), old, new);
    
    if (!stat(new, &fs))  {
	/* the target file has existed already */

	if (prompt == 1)  {
	    goto skip;
	    
	} else if (prompt == 0)  {
	    fprintf(stderr, _("overwrite %s? ") 
			    "(Yes, No, All, nO_to_all) ",new);
	    
	    tcflush(0, TCIFLUSH);
	    read(0, buf, 64);
	    p = skip_space(buf);
	    
	    switch (*p)  {
	    case 'a':
	    case 'A':
		prompt = 2;

	    case 'y':
	    case 'Y':
		break;
		    
	    case 'o':
	    case 'O':
		prompt = 1;
		
	    case 'n':
	    case 'N':
	    default:
		goto skip;
	    }
	}
    }
    
    if (attr & MOD_TEST) {
	if (attr & MOD_VERBO)
	    printf(_("tested\n"));
    } else {
	rs = rename(old, new);
	if (rs < 0) 
	    perror("rename");
	else if (attr & MOD_VERBO)
	    printf(_("ok\n"));
	
	if (attr & MOD_OWNER)
	    chown(new, pwd->pw_uid, pwd->pw_gid);
    }

    return rs;

skip:
    if (attr & MOD_VERBO) 
	printf(_("skiped\n"));
    return rs;
}

	
void sigbreak(int sig)
{

    if (oper == ACT_REG)  
	regfree(preg);
    
    chdir(cwd);
    exit(sig);
}
    
    
void usage(int mode)
{

    char *help = N_("\
Usage: rename SOURCE DEST\n\
   or: rename [OPTION] file ...\n\
Rename SOURCE to DEST, or substitute characters match the specified pattern\n\
in the filename.\n\
\n\
  -l, --lowcase            lowcase the file names\n\
  -u, --upcase             upcase the file names\n\
  -s/PATTERN/STRING[/sw]   replace matching PATTERN with STRING, [sw] is\n\
                           [g] replace all occurrences in the filename\n\
                           [i] ignore case when searching\n\
                           [b] backward searching and replacing\n\
                           [s] change file's suffix name\n\
                           [r] PATTERN is regular expression\n\
                           [e] PATTERN is extended regular expression\n\
  -R, --recursive          operate on files and directories recursively\n\
  -o, --owner  OWNER       change file's owner (superuser only)\n\
  -v, --verbose            display verbose information\n\
  -t, --test               test only\n\
  -h, --help               display this help and exit\n\
  -V, --version            output version information and exit\n\
      --yes  --no          force to choose YES or NO when target exists\n\
\n\
See man page regex(7) for detail information about extended regular expression.\n\
\n\
Report bugs to <xuming@users.sourceforge.net>.\n");

    char *version = N_("rename tool version %s\n");

    if (mode)  
	printf(_(version), VERSION);  
    else  
	printf(_(help));
}

