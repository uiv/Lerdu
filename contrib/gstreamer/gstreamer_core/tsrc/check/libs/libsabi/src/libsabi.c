/* GStreamer
 * Copyright (C) 2005 Wim Taymans <wim@fluendo.com>
 *
 * libsabi.c: Unit test for ABI compatibility
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gst/gst_global.h>
#include "libgstreamer_wsd_solution.h" 

#define LOG_FILE "c:\\logs\\libsabi_logs.txt" 

#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include <gst/check/gstcheck.h>

#include <gst/base/gstadapter.h>
#include <gst/base/gstbasesink.h>
#include <gst/base/gstbasesrc.h>
#include <gst/base/gstbasetransform.h>
#include <gst/base/gstcollectpads.h>
#include <gst/base/gstpushsrc.h>
#include <gst/base/gsttypefindhelper.h>
#include <gst/controller/gstcontroller.h>
#include <gst/net/gstnet.h>
#include <gst/net/gstnetclientclock.h>
#include <gst/net/gstnettimepacket.h>
#include <gst/net/gstnettimeprovider.h>

#include <glib_global.h>

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(thread_list,gstcheck,GList*)
#define thread_list (*GET_GSTREAMER_WSD_VAR_NAME(thread_list,gstcheck,g)())
#else 
extern GList *thread_list;
#endif

#if EMULATOR
#include "struct_i386.h"
#define HAVE_ABI_SIZES TRUE
#else
#include "struct_hppa.h"
#define HAVE_ABI_SIZES TRUE
#endif

#if 0

#ifdef HAVE_CPU_I386
#include "struct_i386.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef __powerpc64__
#include "struct_ppc64.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef __powerpc__
#include "struct_ppc32.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef HAVE_CPU_X86_64
#include "struct_x86_64.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef HAVE_CPU_HPPA
#include "struct_hppa.h"
#define HAVE_ABI_SIZES TRUE
#else
/* in case someone wants to generate a new arch */
//#include "struct_i386.h"
#define HAVE_ABI_SIZES FALSE
#endif
#endif
#endif
#endif
#endif
#endif

//char* xmlfile = "libsabi";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

void test_ABI()
{
  xmlfile = "test_ABI";
  std_log(LOG_FILENAME_LINE, "Test Started test_ABI");
  gst_check_abi_list (list, HAVE_ABI_SIZES);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
}



//void
//libsabi_suite (void)
//{
//test_ABI();
//}

void (*fn[]) (void) = {
test_ABI
};

char *args[] = {
"test_ABI"
};

GST_CHECK_MAIN (libsabi);
#if 0
int main(int argc,char** argv)
{
		int i;
		gst_init(&argc,&argv);
		for( i=0;i<1;i++)
			fn[i]();
			
} 
#endif