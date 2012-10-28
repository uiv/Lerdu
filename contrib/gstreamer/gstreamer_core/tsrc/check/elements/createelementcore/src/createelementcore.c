//createelementcore.c
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

#include <gst/gst_global.h>
#include <libgstreamer_wsd_solution.h>
#include <gst/check/gstcheck.h>


#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical ;
#endif

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_warning,gstcheck,gboolean)
#define _gst_check_raised_warning (*GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_warning ;
#endif

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log ;
#endif

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running ;
#endif

#ifndef EMULATOR
int failed=0;
FILE* fp_std_log_t=NULL;
#define assert_failed *(get_assert_failed())
#define fp_std_log *(get_fp_std_log())
#endif

#define LOG_FILE "c:\\logs\\createelementcore.txt" 

#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__


#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers;
#endif


#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(thread_list,gstcheck,GList*)
#define thread_list (*GET_GSTREAMER_WSD_VAR_NAME(thread_list,gstcheck,g)())
#else 
extern GList *thread_list;
#endif

//GMutex *mutex;
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(mutex,gstcheck,GMutex*)
#define mutex (*GET_GSTREAMER_WSD_VAR_NAME(mutex,gstcheck,g)())
#else 
extern GMutex *mutex;
#endif

//GCond *start_cond;              /* used to notify main thread of thread startups */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
extern GCond *start_cond;
#endif

//GCond *sync_cond;               /* used to synchronize all threads and main thread */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
extern GCond *sync_cond;
#endif


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}
   
void test_createelement_core()
{

  GstElement *audiotestsrc,*fakesink,*fakesrc,*filesrc,*identity,*devsoundsink,*wavparse;
  GstElement *bin,*decodebin,*playbin,*capsfilter,*tee,*queue,*pipeline,*audioresample,*audioconvert;
  GstElement *typefind,*multiqueue,*osssink,*mad,*fdsink,*fdsrc,*filesink;
  //xmlfile = "test_elementcreate";
    std_log(LOG_FILENAME_LINE, "Test Started element create core");
   audiotestsrc	= gst_element_factory_make ("audiotestsrc", "audiotestsrc");
	fail_unless (audiotestsrc != NULL, "Could not create audiotestsrc");
fakesink	= gst_element_factory_make ("fakesink", "fakesink");
	fail_unless (fakesink != NULL, "Could not create fakesink");
fakesrc		= gst_element_factory_make ("fakesrc", "fakesrc");
	fail_unless (fakesrc != NULL, "Could not create fakesrc");
filesrc		= gst_element_factory_make ("filesrc", "filesrc");
	fail_unless (filesrc != NULL, "Could not create filesrc");
identity	= gst_element_factory_make ("identity", "identity");//filter
	fail_unless (identity != NULL, "Could not create identity");
devsoundsink	= gst_element_factory_make ("devsoundsink", "devsoundsink");
	fail_unless (devsoundsink != NULL, "Could not create devsoundsink");
wavparse	= gst_element_factory_make ("wavparse", "wavparse");//waveparser
	fail_unless (wavparse != NULL, "Could not create wavparse");
bin		= gst_element_factory_make ("bin", "bin");
	fail_unless (bin != NULL, "Could not create bin");
decodebin	= gst_element_factory_make ("decodebin", "decodebin"); //decoder
	fail_unless (decodebin != NULL, "Could not create decodebin");
playbin		= gst_element_factory_make ("playbin", "playbin");
	fail_unless (playbin != NULL, "Could not create playbin");
capsfilter	= gst_element_factory_make ("capsfilter", "capsfilter");
	fail_unless (capsfilter != NULL, "Could not create capsfilter");
tee		= gst_element_factory_make ("tee", "tee");
	fail_unless (tee != NULL, "Could not create tee");
queue		= gst_element_factory_make ("queue","queue");
	fail_unless (queue != NULL, "Could not create queue");
pipeline	= gst_element_factory_make ("pipeline","pipeline");
	fail_unless (pipeline != NULL, "Could not create pipeline");
audioresample	= gst_element_factory_make ("audioresample", "audioresample");
	fail_unless (audioresample != NULL, "Could not create audioresample");
audioconvert	= gst_element_factory_make ("audioconvert", "audioconvert");
	fail_unless (audioconvert != NULL, "Could not create audioconvert");
typefind	= gst_element_factory_make ("typefind", "typefind");
	fail_unless (typefind != NULL, "Could not create typefind");
multiqueue	= gst_element_factory_make ("multiqueue","multiqueue");
	fail_unless (multiqueue != NULL, "Could not create multiqueue");
	fdsink		= gst_element_factory_make ("fdsink", "fdsink");
	fail_unless (fdsink != NULL, "Could not create fdsink");
	fdsrc		= gst_element_factory_make ("fdsrc", "fdsrc");
	      fail_unless (fdsrc != NULL, "Could not create fdsrc");
	filesink		= gst_element_factory_make ("filesink", "filesink");
	fail_unless (filesink != NULL, "Could not create filesink");
	 
  
  gst_object_unref (audiotestsrc);
  gst_object_unref (fakesink);
  gst_object_unref (fakesrc);
  gst_object_unref (filesrc);
  gst_object_unref (identity);
  gst_object_unref (devsoundsink);
  gst_object_unref (wavparse);
  gst_object_unref (bin);
  gst_object_unref (decodebin);
  gst_object_unref (playbin);
  gst_object_unref (capsfilter);
  gst_object_unref (tee);
  gst_object_unref (queue);
  gst_object_unref (pipeline);
  gst_object_unref (audioresample);
  gst_object_unref (audioconvert);
  gst_object_unref (typefind);
  gst_object_unref (multiqueue);
  gst_object_unref (fdsink);
  gst_object_unref (fdsrc);
  gst_object_unref (filesink);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
   create_xml(0);
 }

void (*fn[]) (void) = {
        test_createelement_core
};

char *args[] = {
        "test_createelement_core"
};

GST_CHECK_MAIN (createelementcore);




