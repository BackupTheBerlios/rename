
#ifndef	RENAME_H
#define RENAME_H
  
#define ACT_DEFT	0	/* directly rename, just like mv(1) does */
#define ACT_SUBT	1	/* search and substitute simplely */
#define ACT_BACKWD	2	/* search and substitute backwardly */
#define ACT_REG		3	/* enable regular expression */
#define ACT_LOWCASE	4	/* lowcase filename */
#define ACT_UPCASE	5	/* upcase filename */
#define	ACT_SUFFIX	6	/* match suffix pattern */
#define ACT_OWNER	7	/* change file owner can work individually */

#define MOD_REPT	1	/* substitute all occurrence of pattern */
#define MOD_ICASE	2	/* ignore cases */
#define	MOD_VERBO	4	/* verbose mode */
#define MOD_OWNER	8	/* change file's owner */
#define	MOD_TEST	0x80	/* just for test only */
  
#define	SVRBUF	512


/* see misc.c */

#ifndef HAVE_STRCASECMP
int strcasecmp(char *sour, char *dest);
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp(char *sour, char *dest, int leng);
#endif

#ifdef  HAVE_STRCASESTR
extern char *strcasestr(const char *, const char *);
#else
char *strcasestr(const char *haystack, const char *needle);
#endif

char *dup_str(char *s);
char *skip_space(char *sour);

/* see fixtoken.c */

int fixtoken(char *sour, char **idx, int ids, char *delim);
int ziptoken(char *sour, char **idx, int ids, char *delim);
  
#endif

