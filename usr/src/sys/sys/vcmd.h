/*	vcmd.h	4.4	83/04/05	*/

#ifndef _IOCTL_
#include <sys/ioctl.h>
#endif

#define	VPRINT		0100
#define	VPLOT		0200
#define	VPRINTPLOT	0400

#define	VGETSTATE	_IOR(v, 0, int)
#define	VSETSTATE	_IOW(v, 1, int)
