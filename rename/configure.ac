# Process this file with autoconf to produce a configure script.
AC_INIT
AC_CONFIG_SRCDIR([src/rename.c])

# Gettext require these
PACKAGE=rename
VERSION=2.0
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_DEFINE_UNQUOTED(VERSION, "$VERSION")
AC_SUBST(PACKAGE)
AC_SUBST(VERSION)


# temporaryly set just for autoheader
#AC_CONFIG_HEADER([config.h])

# for automake
AM_INIT_AUTOMAKE(rename, 2.0)
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CC
AC_ISC_POSIX
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_YACC
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_CHECK_HEADERS([alloca.h argz.h errno.h fcntl.h langinfo.h libintl.h limits.h locale.h malloc.h stddef.h stdlib.h string.h strings.h sys/param.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_FUNC_CHOWN
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify bzero dcgettext getcwd getwd mempcpy munmap nl_langinfo re_comp regcomp setlocale stpcpy strcasecmp strchr strcspn strdup strncasecmp strrchr strstr strtoul])

# internationalization macros
AM_GNU_GETTEXT 
#ALL_LINGUAS="zh_CN"

AC_CONFIG_FILES([Makefile intl/Makefile src/Makefile man/Makefile \
	po/Makefile.in])

AC_OUTPUT

