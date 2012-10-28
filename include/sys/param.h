/*	$OpenBSD: param.h,v 1.94 2012/01/11 22:11:34 deraadt Exp $	*/
/*	$NetBSD: param.h,v 1.23 1996/03/17 01:02:29 thorpej Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)param.h	8.2 (Berkeley) 1/21/94
 */

#define	BSD	199306		/* System version (year & month). */
#define BSD4_3	1
#define BSD4_4	1

#define OpenBSD	201205		/* OpenBSD version (year & month). */
#define OpenBSD5_1 1		/* OpenBSD 5.1 */

#ifndef NULL
#ifdef 	__GNUG__
#define	NULL	__null
#else
#define	NULL	((void *)0)
#endif
#endif

#ifndef _LOCORE
#include <sys/types.h>
#endif


#define	MAXTASKS	256		/* max number of tasks in system */
#define	MAXTHREADS	128		/* max number of threads per task */
#define	MAXOBJECTS	32		/* max number of objects per task */
#define	MAXSYNCS	512		/* max number of synch objects per task */
#define MAXMEM		(4*1024*1024)	/* max core per task - first # is Mb */

/* The following name length include a null-terminate character */
#define MAXTASKNAME	12		/* max task name */
#define MAXDEVNAME	12		/* max device name */
#define MAXOBJNAME	16		/* max object name */
#define MAXEVTNAME	12		/* max event name */

#define HZ		CONFIG_HZ	/* ticks per second */
#define MAXIRQS		32		/* max number of irq line */

/*
 * Priorities.
 * Probably should not be altered too much.
 */
#define PRI_TIMER	15	/* priority for timer thread */
#define PRI_IST	 	16	/* top priority for interrupt threads */
#define PRI_DPC	 	33	/* priority for Deferred Procedure Call */
#define PRI_IDLE	255	/* priority for idle thread */
#define PRI_REALTIME	127	/* default priority for real-time thread */
#define PRI_DEFAULT	200	/* default user priority */

#define MINPRI		255
#define NPRI		(MINPRI + 1)	/* number of thread priority */

/* Server priorities */
#define PRI_PROC	124	/* process server */
#define PRI_EXEC	125	/* exec server */
#define PRI_FS		126	/* file system server */
#define PRI_POW		100	/* power server */

#include <machine/limits.h>
#include <machine/memory.h>

#define KSTACKSZ	768		/* kernel stack size */

#define USRSTACK	(0 + PAGE_SIZE)	/* base address of user stack */
#define DFLSTKSZ	4096		/* default size of user stack */

#ifdef CONFIG_MMU
#define user_area(a)	((vaddr_t)(a) <  (vaddr_t)USERLIMIT)
#else
#define user_area(a)	1
#endif

/* Address translation between physical address and kernel viritul address */
#define ptokv(pa)	(void *)((paddr_t)(pa) + KERNBASE)
#define kvtop(va)	((paddr_t)(va) - KERNBASE)

/*
 * Round p (pointer or byte index) up to a correctly-aligned value for all
 * data types (int, long, ...).   The result is u_long and must be cast to
 * any desired pointer type.
 */
#define	_ALIGNBYTES	(sizeof(int) - 1)
#define	ALIGN(p)	(((unsigned)(p) + _ALIGNBYTES) &~ _ALIGNBYTES)

/*
 * Memory page
 */
#define PAGE_MASK	(PAGE_SIZE-1)
#define trunc_page(x)	((x) & ~PAGE_MASK)
#define round_page(x)	(((x) + PAGE_MASK) & ~PAGE_MASK)

#define rounddown(x,y)  (((x)/(y))*(y))

#define	BSIZE	512		/* size of secondary block (bytes) */

/*
 * Macro to convert milliseconds and tick.
 */
#define mstohz(ms)	(((ms) + 0UL) * HZ / 1000)

#define hztoms(tick)	((tick) >= 0x20000 ? \
			 (((tick) + 0u) / HZ) * 1000u : \
			 (((tick) + 0u) * 1000u) / HZ)
			 
			 
/*
 * Machine-independent constants (some used in following include files).
 * Redefined constants are from POSIX 1003.1 limits file.
 *
 * MAXCOMLEN should be >= sizeof(ac_comm) (see <acct.h>)
 * MAXLOGNAME should be >= UT_NAMESIZE (see <utmp.h>)
 */
#include <sys/syslimits.h>

#define	MAXCOMLEN	16		/* max command name remembered */
#define	MAXINTERP	128		/* max interpreter file name length */
#define	MAXLOGNAME	LOGIN_NAME_MAX	/* max login name length w/ NUL */
#define	MAXUPRC		CHILD_MAX	/* max simultaneous processes */
#define	NCARGS		ARG_MAX		/* max bytes for an exec function */
#define	NGROUPS		NGROUPS_MAX	/* max number groups */
#define	NOFILE		OPEN_MAX	/* max open files per process (soft) */
#define	NOFILE_MAX	1024		/* max open files per process (hard) */
#define	NOGROUP		65535		/* marker for empty group set member */
#define MAXHOSTNAMELEN	256		/* max hostname size */


#define	PASSWORD_LEN	4		/* fixed length, not counting NULL */
/* More types and definitions used throughout the kernel. */
#ifdef _KERNEL
#include <sys/cdefs.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/ucred.h>
#include <sys/uio.h>
#endif

/* Signals. */
#include <sys/signal.h>

/* Machine type dependent parameters. */
#include <sys/limits.h>
#include <machine/param.h>

/*
 * Priorities.  Note that with 32 run queues, differences less than 4 are
 * insignificant.
 */
#define	PSWP	0
#define	PVM	4
#define	PINOD	8
#define	PRIBIO	16
#define	PVFS	20
#define	PZERO	22		/* No longer magic, shouldn't be here.  XXX */
#define	PSOCK	24
#define	PWAIT	32
#define	PLOCK	36
#define	PPAUSE	40
#define	PUSER	50
#define	MAXPRI	127		/* Priorities range from 0 through MAXPRI. */

#define	PRIMASK		0x0ff
#define	PCATCH		0x100	/* OR'd with pri for tsleep to check signals */
#define PNORELOCK	0x200	/* OR'd with pri for msleep to not reaquire
				   the mutex */

#define	CMASK	022		/* default file mask: S_IWGRP|S_IWOTH */
#define	NODEV	(dev_t)(-1)	/* non-existent device */
#define NETDEV	(dev_t)(-2)	/* network device (for nfs swap) */
	
#define	CBLOCK	64		/* Clist block size, must be a power of 2. */
#define CBQSIZE	(CBLOCK/NBBY)	/* Quote bytes/cblock - can do better. */
				/* Data chars/clist. */
#define	CBSIZE	(CBLOCK - sizeof(struct cblock *) - CBQSIZE)
#define	CROUND	(CBLOCK - 1)	/* Clist rounding. */

/*
 * Constants related to network buffer management.
 * MCLBYTES must be no larger than NBPG (the software page size), and,
 * on machines that exchange pages of input or output buffers with mbuf
 * clusters (MAPPED_MBUFS), MCLBYTES must also be an integral multiple
 * of the hardware page size.
 */
#define	MSIZE		256		/* size of an mbuf */
#define	MCLSHIFT	11		/* convert bytes to m_buf clusters */
					/* 2K cluster can hold Ether frame */
#define	MCLBYTES	(1 << MCLSHIFT)	/* size of a m_buf cluster */
#define	MCLOFSET	(MCLBYTES - 1)

#define MAXMCLBYTES	(64 * 1024)	/* largest cluster from the stack */

/*
 * File system parameters and macros.
 *
 * The file system is made out of blocks of at most MAXBSIZE units, with
 * smaller units (fragments) only in the last direct block.  MAXBSIZE
 * primarily determines the size of buffers in the buffer pool.  It may be
 * made larger without any effect on existing file systems; however making
 * it smaller makes some file systems unmountable.
 */
#ifndef MAXBSIZE	/* XXX temp until sun3 DMA chaining */
#define	MAXBSIZE	MAXPHYS
#endif
#define MAXFRAG 	8

/*
 * MAXPATHLEN defines the longest permissible path length after expanding
 * symbolic links. It is used to allocate a temporary buffer from the buffer
 * pool in which to do the name expansion, hence should be a power of two,
 * and must be less than or equal to MAXBSIZE.  MAXSYMLINKS defines the
 * maximum number of symbolic links that may be expanded in a path name.
 * It should be set high enough to allow all legitimate uses, but halt
 * infinite loops reasonably quickly.
 */
#define	MAXPATHLEN	PATH_MAX
#define MAXSYMLINKS	SYMLOOP_MAX

/* Macros to set/clear/test flags. */
#ifdef _KERNEL
#define SET(t, f)	((t) |= (f))
#define CLR(t, f)	((t) &= ~(f))
#define ISSET(t, f)	((t) & (f))
#endif

/* Bit map related macros. */
#define	setbit(a,i)	((a)[(i)>>3] |= 1<<((i)&(NBBY-1)))
#define	clrbit(a,i)	((a)[(i)>>3] &= ~(1<<((i)&(NBBY-1))))
#define	isset(a,i)	((a)[(i)>>3] & (1<<((i)&(NBBY-1))))
#define	isclr(a,i)	(((a)[(i)>>3] & (1<<((i)&(NBBY-1)))) == 0)

/* Macros for counting and rounding. */
#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))
#define powerof2(x)	((((x)-1)&(x))==0)

/* Macros for min/max. */
#define	MIN(a,b) (((a)<(b))?(a):(b))
#define	MAX(a,b) (((a)>(b))?(a):(b))

/* Macros for calculating the offset of a field */
#if !defined(offsetof) && defined(_KERNEL)
#define offsetof(s, e) ((size_t)&((s *)0)->e)
#endif

#define nitems(_a)	(sizeof((_a)) / sizeof((_a)[0]))

/*
 * Constants for setting the parameters of the kernel memory allocator.
 *
 * 2 ** MINBUCKET is the smallest unit of memory that will be
 * allocated. It must be at least large enough to hold a pointer.
 *
 * Units of memory less or equal to MAXALLOCSAVE will permanently
 * allocate physical memory; requests for these size pieces of
 * memory are quite fast. Allocations greater than MAXALLOCSAVE must
 * always allocate and free physical memory; requests for these
 * size allocations should be done infrequently as they will be slow.
 *
 * Constraints: PAGE_SIZE <= MAXALLOCSAVE <= 2 ** (MINBUCKET + 14), and
 * MAXALLOCSIZE must be a power of two.
 */
#define MINBUCKET	4		/* 4 => min allocation of 16 bytes */
#define MAXALLOCSAVE	(2 * PAGE_SIZE)

/*
 * Scale factor for scaled integers used to count %cpu time and load avgs.
 *
 * The number of CPU `tick's that map to a unique `%age' can be expressed
 * by the formula (1 / (2 ^ (FSHIFT - 11))).  The maximum load average that
 * can be calculated (assuming 32 bits) can be closely approximated using
 * the formula (2 ^ (2 * (16 - FSHIFT))) for (FSHIFT < 15).
 *
 * For the scheduler to maintain a 1:1 mapping of CPU `tick' to `%age',
 * FSHIFT must be at least 11; this gives us a maximum load avg of ~1024.
 */
#define	FSHIFT	11		/* bits to right of fixed binary point */
#define FSCALE	(1<<FSHIFT)

/*
 * The time for a process to be blocked before being very swappable.
 * This is a number of seconds which the system takes as being a non-trivial
 * amount of real time.  You probably shouldn't change this;
 * it is used in subtle ways (fractions and multiples of it are, that is, like
 * half of a ``long time'', almost a long time, etc.)
 * It is related to human patience and other factors which don't really
 * change over time.
 */
#define	MAXSLP	20

/*
 * rfork() options.
 *
 * XXX currently, operations without RFPROC set are not supported.
 */
#define RFNAMEG		(1<<0)	/* UNIMPL new plan9 `name space' */
#define RFENVG		(1<<1)	/* UNIMPL copy plan9 `env space' */
#define RFFDG		(1<<2)	/* copy fd table */
#define RFNOTEG		(1<<3)	/* UNIMPL create new plan9 `note group' */
#define RFPROC		(1<<4)	/* change child (else changes curproc) */
#define RFMEM		(1<<5)	/* share `address space' */
#define RFNOWAIT	(1<<6)	/* parent need not wait() on child */ 
#define RFCNAMEG	(1<<10) /* UNIMPL zero plan9 `name space' */
#define RFCENVG		(1<<11) /* UNIMPL zero plan9 `env space' */
#define RFCFDG		(1<<12)	/* zero fd table */
#define RFTHREAD	(1<<13)	/* create a thread, not a process */
