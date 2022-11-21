/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* wizard.c - version 1.0.3 */

/* wizard code - inspired by rogue code from Merlyn Leroy (digi-g!brian) */

#include "hack.h"
extern struct permonst pm_wizard;
extern struct monst *makemon();

#ifdef DGK
#define	WIZSHOT	    2	/* one chance in WIZSHOT that wizard will try magic */
#else
#define	WIZSHOT	    6	/* one chance in WIZSHOT that wizard will try magic */
#endif
#define	BOLT_LIM    8	/* from this distance D and 1 will try to hit you */

char wizapp[] = "@DNPTUVXcemntx";

#ifdef DGK
#define URETREATING(x,y) (movedist(u.ux,u.uy,x,y) > movedist(u.ux0,u.uy0,x,y))
extern char mlarge[];

movedist(x0, x1, y0, y1)
{
	register int absdx, absdy;

	absdx = abs(x1 - x0);
	absdy = abs(y1 - y0);

	return (absdx + absdy - min(absdx, absdy));
}
#endif

/* If he has found the Amulet, make the wizard appear after some time */
amulet(){
	register struct obj *otmp;
	register struct monst *mtmp;

	if(!flags.made_amulet || !flags.no_of_wizards)
		return;
	/* find wizard, and wake him if necessary */
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if(mtmp->data->mlet == '1' && mtmp->msleep && !rn2(40))
		for(otmp = invent; otmp; otmp = otmp->nobj)
		    if(otmp->olet == AMULET_SYM && !otmp->spe) {
			mtmp->msleep = 0;
			if(dist(mtmp->mx,mtmp->my) > 2)
			    pline(
    "You get the creepy feeling that somebody noticed your taking the Amulet."
			    );
			return;
		    }
}

wiz_hit(mtmp)
register struct monst *mtmp;
{
	/* if we have stolen or found the amulet, we disappear */
	if(mtmp->minvent && mtmp->minvent->olet == AMULET_SYM &&
	    mtmp->minvent->spe == 0) {
		/* vanish -- very primitive */
		fall_down(mtmp);
		return(1);
	}

	/* if it is lying around someplace, we teleport to it */
	if(!carrying(AMULET_OF_YENDOR)) {
	    register struct obj *otmp;

	    for(otmp = fobj; otmp; otmp = otmp->nobj)
		if(otmp->olet == AMULET_SYM && !otmp->spe) {
		    if((u.ux != otmp->ox || u.uy != otmp->oy) &&
		       !m_at(otmp->ox, otmp->oy)) {

			/* teleport to it and pick it up */
			mtmp->mx = otmp->ox;
			mtmp->my = otmp->oy;
			freeobj(otmp);
			mpickobj(mtmp, otmp);
			pmon(mtmp);
			return(0);
		    }
		    goto hithim;
		}
	    return(0);				/* we don't know where it is */
	}
hithim:
	if(rn2(2)) {				/* hit - perhaps steal */

	    /* if hit 1/20 chance of stealing amulet & vanish
		- amulet is on level 26 again. */
	    if(hitu(mtmp, d(mtmp->data->damn,mtmp->data->damd))
		&& !rn2(20) && stealamulet(mtmp))
		;
	}
	else
	    inrange(mtmp);			/* try magic */
	return(0);
}

#ifdef DGK
/* Check if a monster is carrying a particular item.
 */
struct obj *
m_carrying(mtmp, type)
struct monst *mtmp;
int type;
{
	register struct obj *otmp;

	for(otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == type)
			return(otmp);
	return((struct obj *) 0);
}

/* Remove an item from the monster's inventory.
 */
m_useup(mon, obj)
struct monst *mon;
struct obj *obj;
{
	struct obj *otmp, *prev;

	prev = ((struct obj *) 0);
	for (otmp = mon->minvent; otmp; otmp = otmp->nobj) {
		if (otmp == obj) {
			if (prev)
				prev->nobj = obj->nobj;
			else
				mon->minvent = obj->nobj;
			free((char *) obj);
			break;
		}
		prev = otmp;
	}
}

m_throw(x, y, dx, dy, range, obj)
register int x,y,dx,dy,range;		/* direction and range */
register struct obj *obj;
{
	register struct monst *mtmp;
	struct objclass *oclass = &objects[obj->otyp];
	char sym = obj->olet;
	int damage;
	extern char *exclam();

	bhitpos.x = x;
	bhitpos.y = y;

	if(sym) tmp_at(-1, sym);	/* open call */
	while(range-- > 0) {
		bhitpos.x += dx;
		bhitpos.y += dy;
		if(mtmp = m_at(bhitpos.x,bhitpos.y)) {
			damage = index(mlarge, mtmp->data->mlet)
				? oclass->wldam
				: oclass->wsdam;
			hit(oclass->oc_name, mtmp, exclam(damage));
			mtmp->mhp -= damage;
			if(mtmp->mhp < 1) {
				pline("%s is killed!", Monnam(mtmp));
				mondied(mtmp);
			}
			range = 0;
		}
		if (bhitpos.x == u.ux && bhitpos.y == u.uy) {
			if (multi)
				nomul(0);
			if (!thitu(8, rnd(oclass->wldam), oclass->oc_name)) {
				mksobj_at(obj->otyp, u.ux, u.uy);
				fobj->quan = 1;
				stackobj(fobj);
			}
			range = 0;
		}
		tmp_at(bhitpos.x, bhitpos.y);
	}
	tmp_at(-1, -1);
}
#endif
			
/* Return 1 if it's OK for the monster to move as well as (throw,
 * zap, etc).
 */
inrange(mtmp)
register struct monst *mtmp;
{
	register schar tx,ty;
#ifdef DGK
	struct obj *otmp;
	register xchar x, y;
#endif

	/* do nothing if cancelled (but make '1' say something) */
	if(mtmp->data->mlet != '1' && mtmp->mcan)
		return 1;

	/* spit fire only when both in a room or both in a corridor */
	if(inroom(u.ux,u.uy) != inroom(mtmp->mx,mtmp->my)) return 1;
	tx = u.ux - mtmp->mx;
	ty = u.uy - mtmp->my;
#ifdef DGK
	if ((!tx || !ty || abs(tx) == abs(ty))	/* straight line or diagonal */
		&& movedist(tx, 0,  ty, 0) < BOLT_LIM) {
		/* Check if there are any dead squares between.  If so,
		 * it won't be possible to shoot.
		 */
		for (x = mtmp->mx, y = mtmp->my; x != u.ux || y != u.uy;
				x += sgn(tx), y += sgn(ty))
			if (!ACCESSIBLE(levl[x][y].typ))
				return 1;

		switch(mtmp->data->mlet) {
		case 'K':
		case 'C':
		/* If you're coming toward the monster, the monster
		 * should try to soften you up with arrows.  If you're
		 * going away, you are probably hurt or running.  Give
		 * chase, but if you're getting too far away, throw.
		 */
		x = mtmp->mx;
		y = mtmp->my;
		otmp = (mtmp->data->mlet == 'K') ? m_carrying(mtmp, DART)
			: m_carrying(mtmp, CROSSBOW_BOLT);
		if (otmp && (!URETREATING(x,y)
			|| !rn2(BOLT_LIM - movedist(x, u.ux, y, u.uy)))) {
				m_throw(mtmp->mx, mtmp->my, sgn(tx), sgn(ty),
					BOLT_LIM,otmp);
				if (!--otmp->quan)
					m_useup(mtmp, otmp);
				return 0;
			}
		break;
#else
	if((!tx && abs(ty) < BOLT_LIM) || (!ty && abs(tx) < BOLT_LIM)
	    || (abs(tx) == abs(ty) && abs(tx) < BOLT_LIM)){
	    switch(mtmp->data->mlet) {
#endif
	    case 'D':
		/* spit fire in the direction of @ (not nec. hitting) */
		buzz(-1,mtmp->mx,mtmp->my,sgn(tx),sgn(ty));
		break;
	    case '1':
		if(rn2(WIZSHOT)) break;
		/* if you zapped wizard with wand of cancellation,
		he has to shake off the effects before he can throw
		spells successfully.  1/2 the time they fail anyway */
		if(mtmp->mcan || rn2(2)) {
		    if(canseemon(mtmp))
			pline("%s makes a gesture, then curses.",
			    Monnam(mtmp));
		    else
			pline("You hear mumbled cursing.");
		    if(!rn2(3)) {
			mtmp->mspeed = 0;
			mtmp->minvis = 0;
		    }
		    if(!rn2(3))
			mtmp->mcan = 0;
		} else {
		    if(canseemon(mtmp)){
			if(!rn2(6) && !Invis) {
			    pline("%s hypnotizes you.", Monnam(mtmp));
#ifdef DGK
	/* bug fix by ab@unido
	 */
			    nomul(-(rn2(3) + 3));
#else
			    nomul(rn2(3) + 3);
#endif
			    break;
			} else
			    pline("%s chants an incantation.",
				Monnam(mtmp));
		    } else
			    pline("You hear a mumbled incantation.");
		    switch(rn2(Invis ? 5 : 6)) {
		    case 0:
			/* create a nasty monster from a deep level */
			/* (for the moment, 'nasty' is not implemented) */
			(void) makemon((struct permonst *)0, u.ux, u.uy);
			break;
		    case 1:
			pline("\"Destroy the thief, my pets!\"");
			aggravate();	/* aggravate all the monsters */
			/* fall into next case */
		    case 2:
			if (flags.no_of_wizards == 1 && rnd(5) == 0)
			    /* if only 1 wizard, clone himself */
			    clonewiz(mtmp);
			break;
		    case 3:
			if(mtmp->mspeed == MSLOW)
				mtmp->mspeed = 0;
			else
				mtmp->mspeed = MFAST;
			break;
		    case 4:
			mtmp->minvis = 1;
			break;
		    case 5:
			/* Only if not Invisible */
			pline("You hear a clap of thunder!");
			/* shoot a bolt of fire or cold, or a sleep ray */
			buzz(-rnd(3),mtmp->mx,mtmp->my,sgn(tx),sgn(ty));
			break;
		    }
		}
	    }
	    if(u.uhp < 1) done_in_by(mtmp);
	}
	return 1;
}

aggravate()
{
	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		mtmp->msleep = 0;
		if(mtmp->mfroz && !rn2(5))
			mtmp->mfroz = 0;
	}
}

clonewiz(mtmp)
register struct monst *mtmp;
{
	register struct monst *mtmp2;

	if(mtmp2 = makemon(PM_WIZARD, mtmp->mx, mtmp->my)) {
		flags.no_of_wizards = 2;
		unpmon(mtmp2);
		mtmp2->mappearance = wizapp[rn2(sizeof(wizapp)-1)];
		pmon(mtmp);
	}
}
