
## I borrow this Makefile.am from GNU hello for making life easier ;-)
##
## Copyright (C) 1996, 1997, 2001 Free Software Foundation, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
## USA.

## Process this file with automake to produce Makefile.in.

bin_PROGRAMS = rename
rename_SOURCES = alloca.c fixtoken.c getopt.c getopt.h getopt1.c misc.c \
	regex.c regex.h rename.c rename.h version.c
rename_LDADD = @LIBINTL@ @ALLOCA@

localedir = $(datadir)/locale

INCLUDES = -I. -I$(srcdir) -I.. -I../intl -I$(top_srcdir)/intl
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

## # Automake cannot handle this now (version 1.0).
## $(bin_PROGRAMS): $(LDADD)

## src/Makefile.am ends here
