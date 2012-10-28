/* GStreamer GstIndex unit test
 * Copyright (C) 2008 Tim-Philipp Müller <tim centricular net>
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


#define LOG_FILE "c:\\logs\\gstindex_log1.txt" 

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

#include <gst/gst.h>
#include <string.h>

void test_index_entries()
{
  GstIndexEntry *entry;
  GstElement *pipe;
  GstIndex *index;
  gint id = -1;
  
  std_log(LOG_FILENAME_LINE, "Test Started test_index_entries");

  pipe = gst_pipeline_new ("pipeline");

  index = gst_index_new ();
  fail_unless (gst_index_get_writer_id (index, GST_OBJECT (pipe), &id));

  entry = gst_index_add_format (index, id, GST_FORMAT_BYTES);
  gst_index_entry_free (entry);

  gst_object_unref (index);
  gst_object_unref (pipe);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


void (*fn[]) (void) = {
test_index_entries
};

char *args[] = {
"test_index_entries"
};

GST_CHECK_MAIN (gst_index);
