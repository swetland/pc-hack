/* version.c - version 1.0.3 */

#include "hack.h"

doversion()
{
	pline("PC HACK %d.%d - Oct 26, 1986", vmajor, vminor);
	return (0);
}

doMSCversion()
{
	pline("PC HACK %d.%d is the MSDOS(tm) version of UNIX(tm) HACK 1.03.",
		vmajor, vminor);
	pline("PC implementation in Microsoft(tm) C by D.Kneller, Berkeley, CA");
	return (0);
}
