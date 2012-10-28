/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
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
*
* Description:
*
*/


 
#ifndef __LIBGSTREAMER_WSD_SOLUTION_H__
#define __LIBGSTREAMER_WSD_SOLUTION_H__
#include "libgstreamer_wsd_macros.h"
#include <glib.h>
#include <stdio.h>
//#include <gst/gst.h>


#if EMULATOR
#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct ShutdownClosure ShutdownClosure;

/**
 * This struct represents a function to be called on shutdown.
 */
struct ShutdownClosure
{
  ShutdownClosure *next;     /**< Next ShutdownClosure */
//  DBusShutdownFunction func; /**< Function to call */
  void *data;                /**< Data for function */
};

struct libgstreamer_global_struct
{
//#ifdef GST_TESTCASE_COMPILATION 
	  VARIABLE_DECL(buffers,g,gstcheck,GList*)
	  VARIABLE_DECL(check_mutex,g,gstcheck,GMutex *)
	  VARIABLE_DECL(check_cond,g,gstcheck,GCond *)
	  
	  VARIABLE_DECL(fp_std_log,g,std_log_result,FILE *)
	  VARIABLE_DECL(assert_failed,g,std_log_result,int)
	  VARIABLE_DECL(xmlfile,g,std_log_result,char*)

	  VARIABLE_DECL(raised_critical,g,gstcheck,gboolean)
	  VARIABLE_DECL(raised_warning,g,gstcheck,gboolean)
	  VARIABLE_DECL(expecting_log,g,gstcheck,gboolean)
	  VARIABLE_DECL(threads_running,g,gstcheck,gboolean)
	  
	  VARIABLE_DECL(thread_list,g,gstcheck,GList*)
	  VARIABLE_DECL(mutex,g,gstcheck,GMutex*)
	  VARIABLE_DECL(start_cond,g,gstcheck,GCond*)
	  VARIABLE_DECL(sync_cond,g,gstcheck,GCond*)
//#endif  /* GST_TESTCASE_COMPILATION */

	  /*END-global vars*/ 
    };

struct libgstreamer_global_struct *libgstreamer_ImpurePtr();
int libgstreamer_Init(struct libgstreamer_global_struct *);

#ifdef __cplusplus
}
#endif

#endif //EMULATOR
#endif //__LIBGSTREAMER_WSD_SOLUTION_H__

