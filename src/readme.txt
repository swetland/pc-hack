
	Welcome to the sources for PC HACK (version 3.6).

Introduction
------------
This is a version of the public domain program HACK 1.03 (copyright
Stichting Mathematisch Centrum, Amsterdam, 1984, 1985.) implemented
under MSDOS with the Microsoft(tm) C v4.0 compiler.

You may copy this version of PC HACK and make any changes you want to
it.  You may give it away, but you may not sell it.

The only major change from version 3.51 is to now use termcap, so there
is a termcap.arc file which has the sources for the Fred Fish termcap
library.  You should compile these separately and combine the .obj files
into a library called LTERMCAP.LIB which gets linked in with the HACK obj
files.  There is a sample TERMCAP file with a termcap entry for the
IBM monochrome monitor.


The sources are in ARC format in HACK351S.ARC.  The commands:

	C> arc51 e hack351s * *.*

will unpack the files.

With a hard disk system, you should be able to type `make' and the sources
will start to be compiled.  This takes a long time.  A floppy disk system
does not really have enough storage.


Compiling
---------
The LARGE compiler model is used.  To add WIZARD mode, add a -DWIZARD
to the MAKEFILE, or a #define WIZARD to the CONFIG.H file.

The MAKEFILE included with PC HACK 3.6 sources is for my version of MAKE.
It is very similar to UNIX(tm) `make'.  See MAKE.DOC for details.

Linking
-------
I used the Microsoft 8086 Linker version 3.51

To link the *.obj files by hand, the command is:
	link @linkfile

Where the contents of the linkfile (not supplied) should be:

decl.obj apply.obj bones.obj cmd.obj do.obj +
do_name.obj do_wear.obj dog.obj eat.obj end.obj +
engrave.obj fight.obj hack.obj invent.obj lev.obj +
main.obj makemon.obj mhitu.obj mklev.obj +
mkmaze.obj mkobj.obj mkshop.obj mon.obj monst.obj +
o_init.obj objnam.obj options.obj pager.obj +
potion.obj pri.obj read.obj rip.obj rumors.obj +
save.obj search.obj shk.obj shknam.obj steal.obj +
termcap.obj timeout.obj topl.obj track.obj +
trap.obj tty.obj unix.obj u_init.obj vault.obj +
wield.obj wizard.obj worm.obj worn.obj zap.obj +
version.obj rnd.obj alloc.obj msdos.obj 
hack.exe 

ltermcap /NOIG /STACK:4000 /CP:1; 


Differences from UNIX HACK
--------------------------
Changes that were introduced to port UNIX HACK to the MSDOS environment
are surrounded with `#ifdef MSDOS', `#endif' directives.

Other changes I have made are surrounded by `#ifdef DGK', `#endif'
directives.  It should be possible to compile these sources without
any of my changes by removing the `#define DGK' line from CONFIG.H.

Also, functions I have added are mainly restricted to the file msdos.c,
although some of them are in other places (ie. wizard.c)


Finally
-------
If you have any questions, contact me at one of:

	Don Kneller
	UUCP:	...ucbvax!ucsfcgl!kneller
	ARPA:	kneller@ucsfcgl.ARPA
	BITNET:	kneller@ucsfcgl.BITNET
	USMAIL:	D. G. Kneller
		2 Panoramic Way #204
		Berkeley, CA 94704
