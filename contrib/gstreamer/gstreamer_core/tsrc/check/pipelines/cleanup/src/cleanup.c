/* GStreamer
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
 *
 * cleanup.c: Unit test for cleanup of pipelines
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

#define LOG_FILE "c:\\logs\\cleanup_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
//char* xmlfile = "cleanup";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

/*
#include "libgstreamer_wsd_solution.h" 

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers = NULL;
#endif
*/



static GstElement *
setup_pipeline (gchar * pipe_descr)
{
  GstElement *pipeline;

  pipeline = gst_parse_launch (pipe_descr, NULL);
  g_return_val_if_fail (GST_IS_PIPELINE (pipeline), NULL);
  return pipeline;
}

/* events is a mask of expected events. tevent is the expected terminal event.
   the poll call will time out after half a second.
 */
static void
run_pipeline (GstElement * pipeline, gchar * descr,
    GstMessageType events, GstMessageType tevent)
{
  GstBus *bus;
  GstMessageType revent;

  bus = gst_element_get_bus (pipeline);
  g_assert (bus);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  while (1) {
    GstMessage *message = gst_bus_poll (bus, GST_MESSAGE_ANY, GST_SECOND / 2);

    if (message) {
      revent = GST_MESSAGE_TYPE (message);
      gst_message_unref (message);
    } else {
      revent = GST_MESSAGE_UNKNOWN;
    }

    if (revent == tevent) {
      break;
    } else if (revent == GST_MESSAGE_UNKNOWN) {
      g_critical ("Unexpected timeout in gst_bus_poll, looking for %d: %s",
          tevent, descr);
            TEST_ASSERT_FAIL

      break;
    } else if (revent & events) {
      continue;
    }
    g_critical
        ("Unexpected message received of type %d, '%s', looking for %d: %s",
        revent, gst_message_type_get_name (revent), tevent, descr);
			  TEST_ASSERT_FAIL

  }

  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  gst_object_unref (bus);
}

void test_pipeline_unref()
{
  gchar *s;
  GstElement *pipeline, *src, *sink;
  GstElement *filesrc, *filesink;

  xmlfile = "test_pipeline_unref";
  std_log(LOG_FILENAME_LINE, "Test Started test_pipeline_unref");


  s = "fakesrc name=src num-buffers=20 ! fakesink name=sink";
//  s = "fakesrc name=src";
  
  //changes for parse launch
  //pipeline = setup_pipeline (s);
  filesrc = gst_element_factory_make ("fakesrc", "src");
  g_assert (filesrc);
  g_object_set (G_OBJECT (filesrc), "num-buffers", 20, NULL);
//  g_object_set (G_OBJECT (filesrc), "location", "c:\\warning.wav", NULL);
  
  
  filesink = gst_element_factory_make ("fakesink", "sink");
  g_assert (filesrc);
//  g_object_set (G_OBJECT (filesink), "location", "c:\\warning1.wav", NULL);
  
  //pipeline = gst_parse_launch (pipe_descr, NULL);
  pipeline = gst_pipeline_new ("pipeline");
  g_assert (pipeline);
  
  gst_bin_add_many (GST_BIN (pipeline), filesrc, filesink, NULL);

    /* link the elements */
    gst_element_link_many (filesrc, filesink, NULL);
  
//  g_return_val_if_fail (GST_IS_PIPELINE (pipeline), NULL);
  
  
  /* get_by_name takes a ref */
  src = gst_bin_get_by_name (GST_BIN (pipeline), "src");
  fail_if (src == NULL);
    TEST_ASSERT_FAIL

  sink = gst_bin_get_by_name (GST_BIN (pipeline), "sink");
  fail_if (sink == NULL);
  TEST_ASSERT_FAIL


  run_pipeline (pipeline, s,
      GST_MESSAGE_NEW_CLOCK | GST_MESSAGE_STATE_CHANGED |
      GST_MESSAGE_ASYNC_DONE, GST_MESSAGE_EOS);
  while (GST_OBJECT_REFCOUNT_VALUE (src) > 1)
    THREAD_SWITCH ();
  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);
  gst_object_unref (src);
  gst_object_unref (sink);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);

}

//GST_END_TEST;

//static Suite *
//cleanup_suite (void)
//{
//  Suite *s = suite_create ("Pipeline cleanup");
//  TCase *tc_chain = tcase_create ("linear");

  /* time out after 20s, not the default 3 */
//  tcase_set_timeout (tc_chain, 20);

//  suite_add_tcase (s, tc_chain);
//  tcase_add_test (tc_chain, test_pipeline_unref);
//  return s;
//}

void (*fn[1]) (void) = {
        test_pipeline_unref
       
};

char *args[] = {
        "test_pipeline_unref"
};


GST_CHECK_MAIN (cleanup);

/*
int main (int argc, char **argv)                
{                               
    int i;                  
gst_check_init (NULL, NULL);                    
test_pipeline_unref();
}
*/
