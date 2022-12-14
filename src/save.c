/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* save.c - version 1.0.3 */

#include "hack.h"
extern char genocided[60];	/* defined in Decl.c */
extern char fut_geno[60];	/* idem */
#include <signal.h>

extern char nul[], pl_character[PL_CSIZ], SAVEF[];
extern long lseek();
extern struct obj *restobjchn();
extern struct monst *restmonchn();

dosave(){
	if(dosave0(0)) {
		clear_screen();
		settty("Be seeing you ...\n");
		exit(0);
	}
}

/* returns 1 if save successful */
dosave0(hu) int hu; {
	register fd, ofd;
	int tmp;		/* not register ! */
#ifdef DGK
	long		fds, needed;
	int		mode;
	extern long	bytes_counted;
	extern int	saveprompt;
#endif

	(void) signal(SIGINT, SIG_IGN);
#ifdef DGK
	if (!saveDiskPrompt(0))
		return 0;
	fd = open(SAVEF, O_WRONLY | O_BINARY | O_CREAT, FMASK);
#else
	fd = creat(SAVEF, FMASK);
#endif
	if(fd < 0) {
		if(!hu) pline("Cannot open save file. (Continue or Quit)");
		(void) unlink(SAVEF);		/* ab@unido */
		return(0);
	}
	if(flags.moonphase == FULL_MOON)	/* ut-sally!fletcher */
		u.uluck--;			/* and unido!ab */
#ifdef DGK
	home();
	cl_end();
	msmsg("Saving: ");
#endif
#ifdef DGK
	mode = COUNT;
again:
	savelev(fd, dlevel, mode);
	/* count_only will be set properly by savelev */
#else
	savelev(fd,dlevel);
#endif
	saveobjchn(fd, invent);
	saveobjchn(fd, fcobj);
	savemonchn(fd, fallen_down);
	tmp = getuid();
	bwrite(fd, (char *) &tmp, sizeof tmp);
	bwrite(fd, (char *) &flags, sizeof(struct flag));
	bwrite(fd, (char *) &dlevel, sizeof dlevel);
	bwrite(fd, (char *) &maxdlevel, sizeof maxdlevel);
	bwrite(fd, (char *) &moves, sizeof moves);
	bwrite(fd, (char *) &u, sizeof(struct you));
	if(u.ustuck)
		bwrite(fd, (char *) &(u.ustuck->m_id), sizeof u.ustuck->m_id);
	bwrite(fd, (char *) pl_character, sizeof pl_character);
	bwrite(fd, (char *) genocided, sizeof genocided);
	bwrite(fd, (char *) fut_geno, sizeof fut_geno);
	savenames(fd);
#ifdef DGK
	if (mode == COUNT) {
		/* make sure there is enough disk space */
		needed = bytes_counted;
		for (tmp = 1; tmp <= maxdlevel; tmp++)
			if (tmp != dlevel && fileinfo[tmp].where)
				needed += fileinfo[tmp].size + (sizeof tmp);
		fds = freediskspace(SAVEF);
		if (needed > fds) {
			pline("There is insufficient space on SAVE disk.");
			pline("Require %ld bytes but only have %ld.", needed,
				fds);
			flushout();
			(void) close(fd);
			(void) unlink(SAVEF);
			saveprompt = 0;		/* allow a disk change */
			return 0;
		}
		mode = WRITE;
		goto again;
	}
#endif
	for(tmp = 1; tmp <= maxdlevel; tmp++) {
		extern int hackpid;
#ifdef DGK
		if (tmp == dlevel || !fileinfo[tmp].where) continue;
		if (fileinfo[tmp].where != ACTIVE)
			swapin_file(tmp);
#else
		extern boolean level_exists[];
		if(tmp == dlevel || !level_exists[tmp]) continue;
#endif
		glo(tmp);
#ifdef DGK
		msmsg(".");
#endif
		if((ofd = open(lock, 0)) < 0) {
		    if(!hu) pline("Error while saving: cannot read %s.", lock);
		    (void) close(fd);
		    (void) unlink(SAVEF);
		    if(!hu) done("tricked");
		    return(0);
		}
		getlev(ofd, hackpid, tmp);
		(void) close(ofd);
		bwrite(fd, (char *) &tmp, sizeof tmp);	/* level number */
#ifdef DGK
		savelev(fd,tmp,WRITE);			/* actual level */
#else
		savelev(fd,tmp);			/* actual level */
#endif
		(void) unlink(lock);
	}
	(void) close(fd);
	glo(dlevel);
	(void) unlink(lock);	/* get rid of current level --jgm */
	glo(0);
	(void) unlink(lock);
	return(1);
}

dorecover(fd)
register fd;
{
	register nfd;
	int tmp;		/* not a register ! */
	unsigned mid;		/* idem */
	struct obj *otmp;
	extern boolean restoring;
#ifdef DGK
	struct flag oldflags;

	oldflags = flags;	/* Save flags set in the config file */
#endif
	restoring = TRUE;
	getlev(fd, 0, 0);
	invent = restobjchn(fd);
	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(otmp->owornmask)
			setworn(otmp, otmp->owornmask);
	fcobj = restobjchn(fd);
	fallen_down = restmonchn(fd);
	mread(fd, (char *) &tmp, sizeof tmp);
	if(tmp != getuid()) {		/* strange ... */
		(void) close(fd);
		(void) unlink(SAVEF);
		puts("Saved game was not yours.");
		restoring = FALSE;
		return(0);
	}
	mread(fd, (char *) &flags, sizeof(struct flag));
#ifdef DGK
	/* Some config file OPTIONS take precedence over those in save file.
	 */
	flags.rawio = oldflags.rawio;
	flags.DECRainbow = oldflags.DECRainbow;
	flags.IBMBIOS = oldflags.IBMBIOS;
#endif
	mread(fd, (char *) &dlevel, sizeof dlevel);
	mread(fd, (char *) &maxdlevel, sizeof maxdlevel);
	mread(fd, (char *) &moves, sizeof moves);
	mread(fd, (char *) &u, sizeof(struct you));
	if(u.ustuck)
		mread(fd, (char *) &mid, sizeof mid);
	mread(fd, (char *) pl_character, sizeof pl_character);
	mread(fd, (char *) genocided, sizeof genocided);
	mread(fd, (char *) fut_geno, sizeof fut_geno);
	restnames(fd);
#ifdef DGK
	msmsg("\n");
	cl_end();
	msmsg("You got as far as level %d%s.\n", maxdlevel,
		flags.debug ? " in WIZARD mode" : "");
	cl_end();
	msmsg("Restoring: ");
#endif
	while(1) {
		if(read(fd, (char *) &tmp, sizeof tmp) != sizeof tmp)
			break;
		getlev(fd, 0, tmp);
		glo(tmp);
#ifdef DGK
		msmsg(".");
		nfd = open(lock, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, FMASK);
#else
		nfd = creat(lock, FMASK);
#endif
		if (nfd < 0)
			panic("Cannot open level file %s!\n", lock);
#ifdef DGK
		if (!savelev(nfd, tmp, COUNT | WRITE)) {

			/* The savelev can't proceed because the size required
			 * is greater than the available disk space.
			 */
			msmsg("\nNot enough space on `%s' to restore your game.\n",
				levels);

			/* Remove levels and bones that may have been created.
			 */
			(void) close(nfd);
			eraseall(levels, alllevels);
			eraseall(levels, allbones);

			/* Perhaps the person would like to play without a
			 * RAMdisk.
			 */
			if (ramdisk) {
				/* PlaywoRAMdisk may not return, but if it does
				 * it is certain that ramdisk will be 0.
				 */
				playwoRAMdisk();
				(void) lseek(fd, 0L, 0); /* Rewind save file */
				return dorecover(fd);	 /* and try again */
			} else {
				msmsg("Be seeing you ...\n");
				exit(0);
			}
		}
#else
		savelev(nfd,tmp);
#endif
		(void) close(nfd);
	}
	(void) lseek(fd, 0L, 0);
	getlev(fd, 0, 0);
	(void) close(fd);
	(void) unlink(SAVEF);
	if(Punished) {
		for(otmp = fobj; otmp; otmp = otmp->nobj)
			if(otmp->olet == CHAIN_SYM) goto chainfnd;
		panic("Cannot find the iron chain?");
	chainfnd:
		uchain = otmp;
		if(!uball){
			for(otmp = fobj; otmp; otmp = otmp->nobj)
				if(otmp->olet == BALL_SYM && otmp->spe)
					goto ballfnd;
			panic("Cannot find the iron ball?");
		ballfnd:
			uball = otmp;
		}
	}
	if(u.ustuck) {
		register struct monst *mtmp;

		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
			if(mtmp->m_id == mid) goto monfnd;
		panic("Cannot find the monster ustuck.");
	monfnd:
		u.ustuck = mtmp;
	}
	setsee();  /* only to recompute seelx etc. - these weren't saved */
#ifdef DGK
	gameDiskPrompt();
#endif
	docrt();
	restoring = FALSE;
	return(1);
}

struct obj *
restobjchn(fd)
register fd;
{
	register struct obj *otmp, *otmp2;
	register struct obj *first = 0;
	int xl;
#ifdef lint
	/* suppress "used before set" warning from lint */
	otmp2 = 0;
#endif /* lint /**/
	while(1) {
		mread(fd, (char *) &xl, sizeof(xl));
		if(xl == -1) break;
		otmp = newobj(xl);
		if(!first) first = otmp;
		else otmp2->nobj = otmp;
		mread(fd, (char *) otmp, (unsigned) xl + sizeof(struct obj));
		if(!otmp->o_id) otmp->o_id = flags.ident++;
		otmp2 = otmp;
	}
	if(first && otmp2->nobj){
		impossible("Restobjchn: error reading objchn.");
		otmp2->nobj = 0;
	}
	return(first);
}

struct monst *
restmonchn(fd)
register fd;
{
	register struct monst *mtmp, *mtmp2;
	register struct monst *first = 0;
	int xl;
	int monsindex;
	extern struct permonst li_dog, dog, la_dog, hell_hound, pm_guard;

#ifdef lint
	/* suppress "used before set" warning from lint */
	mtmp2 = 0;
#endif /* lint /**/
	while(1) {
		mread(fd, (char *) &xl, sizeof(xl));
		if(xl == -1) break;
		mtmp = newmonst(xl);
		if(!first) first = mtmp;
		else mtmp2->nmon = mtmp;
		mread(fd, (char *) mtmp, (unsigned) xl + sizeof(struct monst));
		if(!mtmp->m_id)
			mtmp->m_id = flags.ident++;
		monsindex = *((int *)&mtmp->data);
		if (monsindex == INDEX_LITTLEDOG)
			mtmp->data = &li_dog;
		else if (monsindex == INDEX_DOG)
			mtmp->data = &dog;
		else if (monsindex == INDEX_LARGEDOG)
			mtmp->data = &la_dog;
		else if (monsindex == INDEX_HELLHOUND)
			mtmp->data = &hell_hound;
		else if (monsindex == INDEX_GUARD)
			mtmp->data = &pm_guard;
		else if (monsindex < 0) {
			msmsg("Monster index %d in restmonchn\n", monsindex);
			goto next;
		} else
			mtmp->data = &mons[monsindex];
		if(mtmp->minvent)
			mtmp->minvent = restobjchn(fd);
next:
		mtmp2 = mtmp;
	}
	if(first && mtmp2->nmon){
		impossible("Restmonchn: error reading monchn.");
		mtmp2->nmon = 0;
	}
	return(first);
}
