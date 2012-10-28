#ifndef	_ARCH_MACHINE_STDIO_H_
#define	_ARCH_MACHINE_STDIO_H_

#define _FDEV_SETUP_READ  __SRD	/**< fdev_setup_stream() with read intent */
#define _FDEV_SETUP_WRITE __SWR	/**< fdev_setup_stream() with write intent */
#define _FDEV_SETUP_RW    (__SRD|__SWR)	/**< fdev_setup_stream() with read/write intent */

#if defined(__DOXYGEN__)
/**
   \brief Initializer for a user-supplied stdio stream

   This macro acts similar to fdev_setup_stream(), but it is to be
   used as the initializer of a variable of type FILE.

   The remaining arguments are to be used as explained in
   fdev_setup_stream().
 */
#define FDEV_SETUP_STREAM(put, get, rwflag)
#else  /* !DOXYGEN */
#define FDEV_SETUP_STREAM(p, g, f) \
	{ \
		.put = p, \
		.get = g, \
		.flags = f, \
		.udata = 0, \
	}
#endif /* DOXYGEN */

/**
   Stream that will be used as an input stream by the simplified
   functions that don't take a \c stream argument.

   The first stream opened with read intent using \c fdevopen()
   will be assigned to \c stdin.
*/
#define stdin (__iob[0])

/**
   Stream that will be used as an output stream by the simplified
   functions that don't take a \c stream argument.

   The first stream opened with write intent using \c fdevopen()
   will be assigned to both, \c stdin, and \c stderr.
*/
#define stdout (__iob[1])

/**
   Stream destined for error output.  Unless specifically assigned,
   identical to \c stdout.

   If \c stderr should point to another stream, the result of
   another \c fdevopen() must be explicitly assigned to it without
   closing the previous \c stderr (since this would also close
   \c stdout).
*/
#define stderr (__iob[2])
#endif /* _ARCH_MACHINE_STDIO_H_ */
