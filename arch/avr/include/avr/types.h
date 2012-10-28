/****************************************************************************
 * arch/avr/include/avr/types.h
 *
 *   Copyright (C) 2011 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/* This file should never be included directed but, rather, only indirectly
 * through stdint.h
 */

#ifndef __ARCH_AVR_INCLUDE_AVR_TYPES_H
#define __ARCH_AVR_INCLUDE_AVR_TYPES_H

#if defined(_KERNEL)
typedef struct label_t {
	unsigned long val[11];
} label_t;
#endif
/****************************************************************************
 * Included Files
 ****************************************************************************/

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Type Declarations
 ****************************************************************************/
#ifndef __ASSEMBLY__

/* These are the sizes of the standard integer types.  NOTE that these type
 * names have a leading underscore character.  This file will be included
 * (indirectly) by include/stdint.h and typedef'ed to the final name without
 * the underscore character.  This roundabout way of doings things allows
 * the stdint.h to be removed from the include/ directory in the event that
 * the user prefers to use the definitions provided by their toolchain header
 * files
 */

typedef signed char        _int8_t;    /* char is 8-bits */
typedef unsigned char      _uint8_t;

typedef signed int         _int16_t;   /* int is 16-bits */
typedef unsigned int       _uint16_t;

typedef signed long        _int32_t;  /* long is 32-bits */
typedef unsigned long      _uint32_t;

typedef signed long long   _int64_t;   /* long long is 64-bits */
typedef unsigned long long _uint64_t;
#define __INT64_DEFINED

/* A (near) pointer is 2 bytes */

typedef signed int         _intptr_t;
typedef unsigned int       _uintptr_t;

/* A FAR pointer is 4 bytes */

typedef signed long        _int_farptr_t;
typedef unsigned long      _uint_farptr_t;

/* This is the size of the interrupt state save returned by irqsave(). */

typedef unsigned char      irqstate_t;

typedef	long		           __key_t;


/*
 * _ALIGN(p) rounds p (pointer or byte index) up to a correctly-aligned
 * value for all data types (int, long, ...).   The result is an
 * unsigned long and must be cast to any desired pointer type.
 *
 * _ALIGNED_POINTER is a boolean macro that checks whether an address
 * is valid to fetch data elements of type t from on this architecture.
 * This does not reflect the optimal alignment, just the possibility
 * (within reasonable limits).
 */
#define	_ALIGNBYTES	(sizeof(int) - 1)
#define	_ALIGN(p)	(((unsigned long)(p) + _ALIGNBYTES) & ~_ALIGNBYTES)
#define	_ALIGNED_POINTER(p,t)	((((unsigned long)(p)) & (sizeof(t) - 1)) == 0)

/* 7.18.1.1 Exact-width integer types */

typedef signed char        __int8_t;    /* char is 8-bits */
typedef unsigned char      __uint8_t;

typedef signed int         __int16_t;   /* int is 16-bits */
typedef unsigned int       __uint16_t;

typedef signed long        __int32_t;  /* long is 32-bits */
typedef unsigned long      __uint32_t;
/* LONGLONG */
typedef	_int64_t		__int64_t;
/* LONGLONG */
typedef	_uint64_t	__uint64_t;

/* 7.18.1.2 Minimum-width integer types */
typedef	__int8_t		__int_least8_t;
typedef	__uint8_t		__uint_least8_t;
typedef	__int16_t		__int_least16_t;
typedef	__uint16_t		__uint_least16_t;
typedef	__int32_t		__int_least32_t;
typedef	__uint32_t		__uint_least32_t;
typedef	__int64_t		__int_least64_t;
typedef	__uint64_t		__uint_least64_t;

/* 7.18.1.3 Fastest minimum-width integer types */
typedef	__int32_t		__int_fast8_t;
typedef	__uint32_t		__uint_fast8_t;
typedef	__int32_t		__int_fast16_t;
typedef	__uint32_t		__uint_fast16_t;
typedef	__int32_t		__int_fast32_t;
typedef	__uint32_t		__uint_fast32_t;
typedef	__int64_t		__int_fast64_t;
typedef	__uint64_t		__uint_fast64_t;
#define	__INT_FAST8_MIN		INT32_MIN
#define	__INT_FAST16_MIN	INT32_MIN
#define	__INT_FAST32_MIN	INT32_MIN
#define	__INT_FAST64_MIN	INT64_MIN
#define	__INT_FAST8_MAX		INT32_MAX
#define	__INT_FAST16_MAX	INT32_MAX
#define	__INT_FAST32_MAX	INT32_MAX
#define	__INT_FAST64_MAX	INT64_MAX
#define	__UINT_FAST8_MAX	UINT32_MAX
#define	__UINT_FAST16_MAX	UINT32_MAX
#define	__UINT_FAST32_MAX	UINT32_MAX
#define	__UINT_FAST64_MAX	UINT64_MAX

/* 7.18.1.4 Integer types capable of holding object pointers */
typedef	long			__intptr_t;
typedef	unsigned long		__uintptr_t;

/* 7.18.1.5 Greatest-width integer types */
typedef	__int64_t		__intmax_t;
typedef	__uint64_t		__uintmax_t;

/* Register size */
typedef __int32_t		__register_t;

/* VM system types */
typedef unsigned long		__vaddr_t;
typedef unsigned long		__paddr_t;
typedef unsigned long		__vsize_t;
typedef unsigned long		__psize_t;

/* Standard system types */
typedef int			__clock_t;
typedef int			__clockid_t;
typedef double			__double_t;
typedef float			__float_t;
typedef long long		__off_t;
typedef long			__ptrdiff_t;
//typedef	unsigned long		__size_t;
typedef	unsigned		__size_t;
typedef	long			__ssize_t;
typedef	int			__time_t;
typedef int			__timer_t;
#if defined(__GNUC__) && __GNUC__ >= 3
typedef	__builtin_va_list	__va_list;
#else
typedef	char *			__va_list;
#endif

/* Wide character support types */
#ifndef __cplusplus
typedef	int			__wchar_t;
#endif
typedef int			__wint_t;
typedef	int			__rune_t;
typedef	void *			__wctrans_t;
typedef	void *			__wctype_t;

#endif /* __ASSEMBLY__ */

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#endif /* __ARCH_AVR_INCLUDE_AVR_TYPES_H */

