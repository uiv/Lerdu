/* GStreamer unit test for disabled gst-parse
 * Copyright (C) 2007 Tim-Philipp MÃ¼ller <tim centricular net>
 * *
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
#  include "config.h"
#endif


#define LOG_FILE "c:\\logs\\parse-disabled_logs.txt" 

#include <gst/gst_global.h>
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

#include <gst/gstconfig.h>

void test_parse_launch_errors()
{
  GstElement *pipe;
  GError *err;
  const gchar *arr[] = { "fakesrc", "fakesink", NULL };

  std_log(LOG_FILENAME_LINE, "Test started test_parse_launch_errors");
  
  err = NULL;
  pipe = gst_parse_launch ("fakesrc ! fakesink", &err);
  fail_unless (err != NULL, "expected an error, but did not get one");
  fail_unless (pipe == NULL, "got pipeline, but expected NULL");
  fail_unless (err->domain == GST_CORE_ERROR);
  fail_unless (err->code == GST_CORE_ERROR_DISABLED);
  g_error_free (err);

  err = NULL;
  pipe = gst_parse_bin_from_description ("fakesrc ! fakesink", TRUE, &err);
  fail_unless (err != NULL, "expected an error, but did not get one");
  fail_unless (pipe == NULL, "got pipeline, but expected NULL");
  fail_unless (err->domain == GST_CORE_ERROR);
  fail_unless (err->code == GST_CORE_ERROR_DISABLED);
  g_error_free (err);

  err = NULL;
  pipe = gst_parse_launchv (arr, &err);
  fail_unless (err != NULL, "expected an error, but did not get one");
  fail_unless (pipe == NULL, "got pipeline, but expected NULL");
  fail_unless (err->domain == GST_CORE_ERROR);
  fail_unless (err->code == GST_CORE_ERROR_DISABLED);
  g_error_free (err);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



//void
//parsedisabled_suite (void)
//{
//test_parse_launch_errors();
//}

void (*fn[]) (void) = {
test_parse_launch_errors
};

char *args[] = {
"test_parse_launch_errors"
};

GST_CHECK_MAIN (parsedisabled);
