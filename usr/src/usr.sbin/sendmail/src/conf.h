/*
 * Copyright (c) 1983 Eric P. Allman
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)conf.h	6.19 (Berkeley) %G%
 */

/*
**  CONF.H -- All user-configurable parameters for sendmail
*/

# include <sys/param.h>
# include <fcntl.h>

/*
**  Table sizes, etc....
**	There shouldn't be much need to change these....
*/

# define MAXLINE	1024		/* max line length */
# define MAXNAME	256		/* max length of a name */
# define MAXPV		40		/* max # of parms to mailers */
# define MAXATOM	100		/* max atoms per address */
# define MAXMAILERS	25		/* maximum mailers known to system */
# define MAXRWSETS	100		/* max # of sets of rewriting rules */
# define MAXPRIORITIES	25		/* max values for Precedence: field */
# define MAXMXHOSTS	20		/* max # of MX records */
# define SMTPLINELIM	990		/* maximum SMTP line length */
# define MAXKEY		128		/* maximum size of a database key */
# define MEMCHUNKSIZE	1024		/* chunk size for memory allocation */
# define MAXUSERENVIRON	100		/* max envars saved, must be >= 3 */
# define MAXIPADDR	16		/* max # of IP addrs for this host */
# define PSBUFSIZE	(MAXNAME * 4)	/* size of prescan buffer */

# ifndef QUEUESIZE
# define QUEUESIZE	1000		/* max # of jobs per queue run */
# endif

# ifndef FORK
# define FORK		vfork		/* function to call to fork mailer */
# endif

/*
**  Compilation options.
**
**	#define these if they are available; comment them out otherwise.
*/

# define LOG		1	/* enable logging */
# define UGLYUUCP	1	/* output ugly UUCP From lines */
# define NETINET	1	/* include internet support */
# define SETPROCTITLE	1	/* munge argv to display current status */
# define NAMED_BIND	1	/* use Berkeley Internet Domain Server */
# define MATCHGECOS	1	/* match user names from gecos field */

# ifdef NEWDB
# define USERDB		1	/* look in user database (requires NEWDB) */
# define BTREE_MAP	1	/* enable BTREE mapping type (requires NEWDB) */
# define HASH_MAP	1	/* enable HASH mapping type (requires NEWDB) */
# endif

# ifdef NDBM
# define DBM_MAP	1	/* enable DBM mapping type (requires NDBM) */
# endif

/*
**  Operating system configuration.
**
**	Unless you are porting to a new OS, you shouldn't have to
**	change these.
*/

# ifdef hpux
# define SYSTEM5	1
# endif

# ifdef SYSTEM5

# define LOCKF		1	/* use System V lockf instead of flock */
# define SYS5TZ		1	/* use System V style timezones */
# define HASUNAME	1	/* use System V uname system call */

# endif

#ifdef sun
# include <vfork.h>
#endif

/*
**  Due to a "feature" in some operating systems such as Ultrix 4.3 and
**  HPUX 8.0, if you receive a "No route to host" message (ICMP message
**  ICMP_UNREACH_HOST) on _any_ connection, all connections to that host
**  are closed.  Some firewalls return this error if you try to connect
**  to the IDENT port (113), so you can't receive email from these hosts
**  on these systems.  The firewall really should use a more specific
**  message such as ICMP_UNREACH_PROTOCOL or _PORT or _NET_PROHIB.
*/

#if !defined(ultrix) && !defined(hpux)
# define IDENTPROTO	1	/* use IDENT proto (RFC 1413) */
#endif

/*
**  Remaining definitions should never have to be changed.  They are
**  primarily to provide back compatibility for older systems -- for
**  example, it includes some POSIX compatibility definitions */

/*
**  Older systems don't have this error code -- it should be in
**  /usr/include/sysexits.h.
*/

# ifndef EX_CONFIG
# define EX_CONFIG	78	/* configuration error */
# endif

/*
**  Do some required dependencies
*/

#if defined(NETINET) || defined(NETISO)
# define SMTP		1	/* enable user and server SMTP */
# define QUEUE		1	/* enable queueing */
# define DAEMON		1	/* include the daemon (requires IPC & SMTP) */
#endif


/*
**  Arrange to use either varargs or stdargs
*/

# ifdef __STDC__

# include <stdarg.h>

# define VA_LOCAL_DECL	va_list ap;
# define VA_START(f)	va_start(ap, f)
# define VA_END		va_end(ap)

# else

# include <varargs.h>

# define VA_LOCAL_DECL	va_list ap;
# define VA_START(f)	va_start(ap)
# define VA_END		va_end(ap)

# endif

#ifdef HASUNAME
# include <sys/utsname.h>
# ifdef newstr
#  undef newstr
# endif
#else /* ! HASUNAME */
# define NODE_LENGTH 32
struct utsname
{
	char nodename[NODE_LENGTH+1];
};
#endif /* HASUNAME */

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN	256
#endif

#if !defined(SIGCHLD) && defined(SIGCLD)
# define SIGCHLD	SIGCLD
#endif

#ifndef STDIN_FILENO
#define STDIN_FILENO	0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO	1
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO	2
#endif

#ifdef LOCKF
#define LOCK_SH		0x01	/* shared lock */
#define LOCK_EX		0x02	/* exclusive lock */
#define LOCK_NB		0x04	/* non-blocking lock */

#else

# include <sys/file.h>

#endif

/*
**  Size of tobuf (deliver.c)
**	Tweak this to match your syslog implementation.  It will have to
**	allow for the extra information printed.
*/

#ifndef TOBUFSIZE
# define TOBUFSIZE (1024 - 256)
#endif
