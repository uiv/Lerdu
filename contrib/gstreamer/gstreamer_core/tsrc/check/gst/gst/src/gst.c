/* GStreamer
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
 *
 * gst.c: Unit test for gst.c
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




#include <gst/gst_global.h>
#include <gst/check/gstcheck.h>
#include <gst/gstversion.h>

#define LOG_FILE "c:\\logs\\gstmessage_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
//char* xmlfile = "gstmessage";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


void test_init()
{
    xmlfile = "gst_test_init";
 std_log(LOG_FILENAME_LINE, "Test Started test_init");
  /* don't segfault with NULL, NULL */
  gst_init (NULL, NULL);
  /* allow calling twice. well, actually, thrice. */
  gst_init (NULL, NULL);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_deinit()
{
    xmlfile = "gst_test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");
  gst_init (NULL, NULL);

  gst_deinit ();
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_deinit_sysclock()
{
  GstClock *clock;
  
  xmlfile = "gst_test_deinit_sysclock";
    std_log(LOG_FILENAME_LINE, "Test Started test_deinit_sysclock");

  gst_init (NULL, NULL);

  clock = gst_system_clock_obtain ();
  gst_object_unref (clock);

  gst_deinit ();
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



/* tests if we can create an element from a compiled-in plugin */
void test_new_pipeline()
{
  GstElement *pipeline;
  
  xmlfile = "gst_test_new_pipeline";
  std_log(LOG_FILENAME_LINE, "Test Started test_new_pipeline");

  pipeline = gst_pipeline_new ("pipeline");
  gst_object_unref (pipeline);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* tests if we can load an element from a plugin */
void test_new_fakesrc()
{
  GstElement *element;
  
  xmlfile = "gst_test_new_fakesrc";
  std_log(LOG_FILENAME_LINE, "Test Started test_new_fakesrc");

  element = gst_element_factory_make ("fakesrc", NULL);
  gst_object_unref (element);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_version()
{
  guint major, minor, micro, nano;
  gchar *version;
  
  xmlfile = "gst_test_version";
    std_log(LOG_FILENAME_LINE, "Test Started test_version");

  gst_version (&major, &minor, &micro, &nano);
  assert_equals_int (major, GST_VERSION_MAJOR);

  version = gst_version_string ();
  fail_if (version == NULL);
  g_free (version);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


/*
void
//void gst_suite (void)
{
test_init();
test_deinit();
test_deinit_sysclock();
test_new_pipeline();
test_new_fakesrc();
test_version();
}
*/

void (*fn[]) (void) = {
test_init,
test_deinit,
test_deinit_sysclock,
test_new_pipeline,
test_new_fakesrc,
test_version
};

char *args[] = {
"test_init",
"test_deinit",
"test_deinit_sysclock",
"test_new_pipeline",
"test_new_fakesrc",
"test_version"
};

GST_CHECK_MAIN (gst);
