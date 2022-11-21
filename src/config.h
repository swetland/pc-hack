/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* config.h - version 1.0.3 */

#ifndef CONFIG	/* make sure the compiler doesnt see the typedefs twice */

#define	CONFIG
#define	CHDIR		/* delete if no chdir() available */

/*
 * Some include files are in a different place under SYSV
 * 	BSD		   SYSV
 * <strings.h>		<string.h>
 * <sys/wait.h>		<wait.h>
 * <sys/time.h>		<time.h>
 * <sgtty.h>		<termio.h>
 * Some routines are called differently
 * index		strchr
 * rindex		strrchr
 * Also, the code for suspend and various ioctls is only given for BSD4.2
 * (I do not have access to a SYSV system.)
 */
/* #define BSD		/* define for BSD */
/* #define SYSV		/* define for System V */
/* #define MSDOS 	/* define for MS-DOS (actually defined by compiler) */

/* #define UNIX		/* delete if no fork(), exec() available */

/* #define STUPID	/* avoid some complicated expressions if
			   your C compiler chokes on them */
/* #define PYRAMID_BUG		/* avoid a bug on the Pyramid */
/* #define NOWAITINCLUDE	/* neither <wait.h> nor <sys/wait.h> exists */
#ifdef MSDOS
#  define NOWAITINCLUDE
#endif


/* #define WIZARD  "kneller"	/* the person allowed to use the -D option */
#define RECORD	"record"/* the file containing the list of topscorers */
#define	HELP	"help"	/* the file containing a description of the commands */
#define	SHELP	"hh"	/* abbreviated form of the same */
#define	FMASK	0660	/* file creation mask */

#ifdef UNIX
#define	HLOCK	"perm"	/* an empty file used for locking purposes */
#define LLOCK	"safelock"	/* link to previous */
/*
 * Define DEF_PAGER as your default pager, e.g. "/bin/cat" or "/usr/ucb/more"
 * If defined, it can be overridden by the environment variable PAGER.
 * Hack will use its internal pager if DEF_PAGER is not defined.
 * (This might be preferable for security reasons.)
 * #define DEF_PAGER	".../mydir/mypager"
 */

/* #define SHELL		/* do not delete the '!' command */

#ifdef BSD
#define	SUSPEND		/* let ^Z suspend the game */
#endif /* BSD /**/
#endif /* UNIX /**/

#ifdef CHDIR
/*
 * If you define HACKDIR, then this will be the default playground;
 * otherwise it will be the current directory.
 */
#define HACKDIR	"/hack"

/*
 * Some system administrators are stupid enough to make Hack suid root
 * or suid daemon, where daemon has other powers besides that of reading or
 * writing Hack files. In such cases one should be careful with chdir's
 * since the user might create files in a directory of his choice.
 * Of course SECURE is meaningful only if HACKDIR is defined.
 */
/* #define SECURE		/* do setuid(getuid()) after chdir() */

/*
 * If it is desirable to limit the number of people that can play Hack
 * simultaneously, define HACKDIR, SECURE and MAX_NR_OF_PLAYERS.
 * #define MAX_NR_OF_PLAYERS	6
 */
#endif /* CHDIR /**/

/* size of terminal screen is (at least) (ROWNO+2) by COLNO */
#define	COLNO	80
#define	ROWNO	22

/*
 * small signed integers (8 bits suffice)
 *	typedef	char	schar;
 * will do when you have signed characters; otherwise use
 *	typedef	short int schar;
 */
typedef	char	schar;

/*
 * small unsigned integers (8 bits suffice - but 7 bits do not)
 * - these are usually object types; be careful with inequalities! -
 *	typedef	unsigned char	uchar;
 * will be satisfactory if you have an "unsigned char" type; otherwise use
 *	typedef unsigned short int uchar;
 */
typedef	unsigned char	uchar;

/*
 * small integers in the range 0 - 127, usually coordinates
 * although they are nonnegative they must not be declared unsigned
 * since otherwise comparisons with signed quantities are done incorrectly
 */
typedef schar	xchar;
typedef	xchar	boolean;		/* 0 or 1 */
#define	TRUE	1
#define	FALSE	0

/*
 * Declaration of bitfields in various structs; if your C compiler
 * doesnt handle bitfields well, e.g., if it is unable to initialize
 * structs containing bitfields, then you might use
 *	#define Bitfield(x,n)	uchar x
 * since the bitfields used never have more than 7 bits. (Most have 1 bit.)
 */
#define	Bitfield(x,n)	unsigned x:n

#define	SIZE(x)	(int)(sizeof(x) / sizeof(x[0]))

# ifdef MSDOS
#   include <fcntl.h>
#   define exit	msexit		/* do chdir first */
#   ifdef getchar
#     undef getchar
#   endif /* getchar /**/
#   define getchar tgetch
#   define DGK			/* enhancements by dgk */
#   ifdef MSC30
#     define REGBUG		/* A bug in register usage under 3.0 */
#   endif

#  ifdef DGK
#    include "msdos.h"		/* contains necessary externs for msdos.c */
#    define REDO		/* support for redoing last command */
#    define SHELL		/* via exec of COMMAND.COM */
#    define PATHLEN	64	/* maximum pathlength */
#    define FILENAME 80		/* maximum filename length (conservative) */
#    define FROMPERM	1	/* for ramdisk use */
#    define TOPERM	2	/* for ramdisk use */
#    define glo(x)	name_file(lock, x)	/* name_file used for bones */
#    define IS_CORNER(x)	((x) == symbol.tlcorn || (x) == symbol.trcorn\
				|| (x) == symbol.blcorn || (x)==symbol.brcorn)
     /* screen symbols for using character graphics.
      */
     struct symbols {
		unsigned char vwall, hwall, tlcorn, trcorn, blcorn, brcorn;
		unsigned char door, room, corr;
     };
     extern struct symbols symbol;
     extern int vminor, vmajor;
     extern char *configfile;

     /* Fake indices into the mons[] array for writing save files.  These
      * monsters appear outside of this array but we have to save them
      * without pointers.
      */
#    define INDEX_LITTLEDOG	-1
#    define INDEX_DOG		-2
#    define INDEX_LARGEDOG	-3
#    define INDEX_HELLHOUND	-4
#  endif /* DGK /**/
# endif /* MSDOS /**/

#endif /* CONFIG /**/
