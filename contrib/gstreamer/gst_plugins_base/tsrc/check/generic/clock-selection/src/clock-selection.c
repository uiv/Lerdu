/* GStreamer
 *
 * unit test for clock selection
 *
 * Copyright (C) <2005> Wim Taymans <wim at fluendo dot com>
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
#include <unistd.h>

#include <gst/check/gstcheck.h>

#define LOG_FILE "c:\\logs\\gstclock-selection_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

//char* xmlfile = "gstsystemclock";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

void test_add()
{
  GstElement *pipeline;
  GstStateChangeReturn ret;
  
  	xmlfile = "test_add";
  std_log(LOG_FILENAME_LINE, "Test Started test_add");

  pipeline = gst_pipeline_new ("pipeline");
  fail_unless (pipeline != NULL, "could not create pipeline");

  ret = gst_element_set_state (pipeline, GST_STATE_READY);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "could not set to READY");

  /* cleanup */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

void (*fn[1]) (void) = {
        test_add
};

char *args[] = {
        "test_add"
};

GST_CHECK_MAIN (clock-selection);

/*static Suite *
volume_suite (void)
{
  Suite *s = suite_create ("clocks");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_add);

  return s;
}*/

/*int
main (int argc, char **argv)
{
  int nf;

  Suite *s = volume_suite ();

  gst_check_init (&argc, &argv);
  test_add();
  
  SRunner *sr = srunner_create (s);
  srunner_run_all (sr, CK_NORMAL);
  nf = srunner_ntests_failed (sr);
  srunner_free (sr);

  return nf;
}*/
