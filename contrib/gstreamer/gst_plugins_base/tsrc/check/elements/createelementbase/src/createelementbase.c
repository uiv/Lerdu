//createelementbase.c
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


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include <gst/gst_global.h>
#include <unistd.h>

#define LOG_FILE "c:\\logs\\createelementbase_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}
#include <gst/check/gstcheck.h>
#include "libgstreamer_wsd_solution.h" 

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers;
#endif


#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical ;
#endif
//gboolean _gst_check_raised_warning = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_warning,gstcheck,gboolean)
#define _gst_check_raised_warning (*GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_warning ;
#endif
//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log ;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running ;
#endif

void test_createelement_base()
{
  GstElement *adder,*capsfilter,*audiorate,*audioconvert,*typefind,*audioresample;
  GstElement *decodebin2,*queue2,*subparse,*gdpdepay,*volume,*gdppay;
  // xmlfile = "create_test_element";
    std_log(LOG_FILENAME_LINE, "Test Started element create base");
   
adder       = gst_element_factory_make ("adder", "adder");
    fail_unless (adder != NULL, "Could not create adder");
 
capsfilter  = gst_element_factory_make ("capsfilter", "capsfilter");
    fail_unless (capsfilter != NULL, "Could not create capsfilter");
audiorate = gst_element_factory_make ("audiorate", "audiorate");
    fail_unless (audiorate  != NULL, "Could not create audiorate ");
audioconvert    = gst_element_factory_make ("audioconvert", "audioconvert");
    fail_unless (audioconvert != NULL, "Could not create audioconvert");
typefind    = gst_element_factory_make ("typefind", "typefind");
    fail_unless (typefind != NULL, "Could not create typefind");
audioresample   = gst_element_factory_make ("audioresample", "audioresample");
    fail_unless (audioresample != NULL, "Could not create audioresample");
decodebin2 = gst_element_factory_make ("decodebin2", "decodebin2");
    fail_unless (decodebin2 != NULL, "Could not create decodebin2");
queue2 = gst_element_factory_make ("queue2","queue2");
    fail_unless (queue2 != NULL, "Could not create queue2");
subparse    = gst_element_factory_make ("subparse", "subparse");
    fail_unless (subparse != NULL, "Could not create subparse");
gdpdepay    = gst_element_factory_make ("gdpdepay", "gdpdepay");
    fail_unless (gdpdepay != NULL, "Could not create gdpdepay");
gdppay      = gst_element_factory_make ("gdppay", "gdppay");
    fail_unless (gdppay != NULL, "Could not create gdppay");

  gst_object_unref (adder);
  gst_object_unref (capsfilter);
  gst_object_unref (audiorate);
  gst_object_unref (audioconvert);
  gst_object_unref (typefind);
  gst_object_unref (audioresample);
  gst_object_unref (decodebin2);
  gst_object_unref (queue2);
  gst_object_unref (subparse);
  gst_object_unref (gdpdepay);
  
  gst_object_unref (gdppay);
   std_log(LOG_FILENAME_LINE, "Test Successful");
   create_xml(0);
 
}

void (*fn[]) (void) = {
        test_createelement_base
};

char *args[] = {
        "test_createelement_base"
};

GST_CHECK_MAIN (createelementbase);
