#
#  FILE
#
#	Makefile    build termcap library
#
#  KEY WORDS
#
#	libraries
#	test functions
#
#  SYNOPSIS
#
#	make		compile the library sources
#	make tests	compile sources for tests
#
#  DESCRIPTION
#
#	Standard make file for building the termcap library and tests.
#
#  AUTHOR
#
#	Fred Fish
#

CC	=	msc
MODEL	=	L
CFLAGS	=	-A${MODEL} -DDGK

TSOURCES =	testtcp.c 
LSOURCES =	tgetent.c tgetflag.c tgetnum.c tgetstr.c tgoto.c tputs.c \
		isdigit.c fgetlr.c


TOBJECTS =	testtcp.obj
LOBJECTS =	tgetent.obj tgetflag.obj tgetnum.obj tgetstr.obj tgoto.obj \
		tputs.obj isdigit.obj fgetlr.obj

termlib.lib :	$(LOBJECTS)
		lib termlib.lib -+ $(LOBJECTS);

install: termlib.lib
	mv termlib.lib \lib\${MODEL}termlib.lib

all: library tests

tests :		testtcp

testtcp :	testtcp.obj
		link testtcp.obj, testtcp,, \lib\termcap
#		$(CC) -o testtcp testtcp.obj -ltermcap

testtcp.obj :	testtcp.c
		$(CC) -c $(CFLAGS) testtcp.c

#
#	Clean up the directory.
#

clean:
		rm  *.obj
