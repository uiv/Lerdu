/*-*- mode:C; -*- */
/*
 * Check: a unit test framework for C
 * Copyright (C) 2001, 2002, Arien Malec
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef CHECK_H
#define CHECK_H

#include <stddef.h>


/* Check: a unit test framework for C

   Check is a unit test framework for C. It features a simple
   interface for defining unit tests, putting little in the way of the
   developer. Tests are run in a separate address space, so Check can
   catch both assertion failures and code errors that cause
   segmentation faults or other signals. The output from unit tests
   can be used within source code editors and IDEs.

   Unit tests are created with the START_TEST/END_TEST macro
   pair. The fail_unless and fail macros are used for creating
   checks within unit tests; the mark_point macro is useful for
   trapping the location of signals and/or early exits.


   Test cases are created with tcase_create, unit tests are added
   with tcase_add_test


   Suites are created with suite_create; test cases are added
   with suite_add_tcase

   Suites are run through an SRunner, which is created with
   srunner_create. Additional suites can be added to an SRunner with
   srunner_add_suite. An SRunner is freed with srunner_free, which also
   frees all suites added to the runner. 

   Use srunner_run_all to run a suite and print results.

*/


#ifdef __cplusplus
#define CK_CPPSTART extern "C" {
CK_CPPSTART
#endif

#include <sys/types.h>



/* Fail the test case unless expr is true */
/* The space before the comma sign before ## is essential to be compatible
   with gcc 2.95.3 and earlier.
*/

#define fail_unless(expr, args...) \
        _fail_unless(expr, __FILE__, __LINE__, \
        "Assertion '"#expr"' failed" , ## args, NULL)

//#define fail_unless 

/* Fail the test case if expr is true */
/* The space before the comma sign before ## is essential to be compatible
   with gcc 2.95.3 and earlier.
*/

/* FIXME: these macros may conflict with C89 if expr is 
   FIXME:   strcmp (str1, str2) due to excessive string length. */
#define fail_if(expr, args...) \
        _fail_unless(!(expr), __FILE__, __LINE__, \
        "Failure '"#expr"' occured" , ## args, NULL)

//#define fail_if 

/* Always fail */
#define fail(args...) _fail_unless(0, __FILE__, __LINE__, "Failed" , ## args, NULL)
//#define fail 

#define TEST_ASSERT_FAIL                    \
    if(assert_failed){                       \
        create_xml(1);                       \
        exit(1);                          \
    }

/* Non macro version of #fail_unless, with more complicated interface */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void _fail_unless (int result, const char *file,
                   int line, const char *expr, ...);

#ifdef __cplusplus 
#define CK_CPPEND }
CK_CPPEND

#endif
#endif /* CHECK_H */
