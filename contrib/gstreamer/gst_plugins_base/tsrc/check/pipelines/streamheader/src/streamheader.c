/* GStreamer
 *
 * unit test for streamheader handling
 *
 * Copyright (C) 2007 Thomas Vander Stichele <thomas at apestaart dot org>
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
#include <unistd.h>
#include <gst/check/gstcheck.h>

#include <fcntl.h>

#define LOG_FILE "c:\\logs\\streamheader_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical;
#endif
//gboolean _gst_check_raised_warning = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_warning,gstcheck,gboolean)
#define _gst_check_raised_warning (*GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_warning;
#endif
//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running;
#endif



#include <gst/check/gstbufferstraw.h>

#ifndef GST_DISABLE_PARSE

/* this tests a gdp-serialized tag from audiotestsrc being sent only once
 * to clients of multifdsink */

static int n_tags = 0;

static gboolean
tag_event_probe_cb (GstPad * pad, GstEvent * event, GMainLoop * loop)
{
  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_TAG:
    {
      ++n_tags;
      fail_if (n_tags > 1, "More than 1 tag received");
      break;
    }
    case GST_EVENT_EOS:
    {
      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}

#if 0 
void test_multifdsink_gdp_tag()
{
  GstElement *p1, *p2;
  GstElement *src, *sink, *depay;
  GstPad *pad;
  GMainLoop *loop;
  int pfd[2];
  
  std_log(LOG_FILENAME_LINE, "Test Started test_multifdsink_gdp_tag");
  
  loop = g_main_loop_new (NULL, FALSE);
  
  p1 = gst_parse_launch ("audiotestsrc num-buffers=10 ! gdppay"
      " ! multifdsink name=p1sink", NULL);
  
/*      
  p1 = gst_parse_launch ("audiotestsrc  ! gdppay"
      " ! multifdsink name=p1sink", NULL);
*/ 
  
  fail_if (p1 == NULL);
  p2 = gst_parse_launch ("fdsrc name=p2src ! gdpdepay name=depay"
      " ! fakesink name=p2sink signal-handoffs=True", NULL);
  fail_if (p2 == NULL);

   
  fail_if (pipe (pfd) == -1);


  gst_element_set_state (p1, GST_STATE_READY);

  sink = gst_bin_get_by_name (GST_BIN (p1), "p1sink");
  g_signal_emit_by_name (sink, "add", pfd[1], NULL);
  gst_object_unref (sink);
  
  src = gst_bin_get_by_name (GST_BIN (p2), "p2src");
  g_object_set (G_OBJECT (src), "fd", pfd[0], NULL);
  gst_object_unref (src);

  depay = gst_bin_get_by_name (GST_BIN (p2), "depay");
  fail_if (depay == NULL);

  pad = gst_element_get_pad (depay, "src");
  fail_unless (pad != NULL, "Could not get pad out of depay");
  gst_object_unref (depay);

  gst_pad_add_event_probe (pad, G_CALLBACK (tag_event_probe_cb), loop);

  gst_element_set_state (p1, GST_STATE_PLAYING);
  gst_element_set_state (p2, GST_STATE_PLAYING);

  g_main_loop_run (loop);

  assert_equals_int (n_tags, 1);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}
#endif

void test_multifdsink_gdp_tag()
{
  GstElement *p1, *p2, *audiotest,*depay1,*depay2,*multifd,*fakesink,*fdsrc;
  GstElement *src, *sink, *depay;
  GstPad *pad;
  GMainLoop *loop;
  int pfd[2];
  
  std_log(LOG_FILENAME_LINE, "Test Started test_multifdsink_gdp_tag");
  
  loop = g_main_loop_new (NULL, FALSE);
  
  p1 = gst_pipeline_new("first");
  fail_if (p1 == NULL);
  audiotest = gst_element_factory_make("audiotestsrc","audiotest");
  g_object_set(audiotest,"num-buffers",10,NULL);
  depay1 = gst_element_factory_make("gdppay","gdppay");
  multifd= gst_element_factory_make("multifdsink","p1sink");
  gst_bin_add_many(GST_BIN(p1),audiotest,depay1,multifd,NULL);
  gst_element_link_many(audiotest,depay1,multifd,NULL);

  
/*  
  p1 = gst_parse_launch ("audiotestsrc num-buffers=10 ! gdppay"
      " ! multifdsink name=p1sink", NULL);
*/
  
/*      
  p1 = gst_parse_launch ("audiotestsrc  ! gdppay"
      " ! multifdsink name=p1sink", NULL);
*/ 
  
  
  /*
  
  p2 = gst_parse_launch ("fdsrc name=p2src ! gdpdepay name=depay"
      " ! fakesink name=p2sink signal-handoffs=True", NULL);
  */
  p2 = gst_pipeline_new("second");
  fail_if (p2 == NULL);
  fdsrc = gst_element_factory_make("fdsrc","p2src");
  depay2 = gst_element_factory_make("gdpdepay","depay");
  fakesink= gst_element_factory_make("fakesink","p2sink");
  g_object_set(fakesink,"signal-handoffs",1,NULL);
  gst_bin_add_many(GST_BIN(p2),fdsrc,depay2,fakesink,NULL);
  gst_element_link_many(fdsrc,depay2,fakesink,NULL);

  fail_if (pipe (pfd) == -1);


  gst_element_set_state (p1, GST_STATE_READY);

  sink = gst_bin_get_by_name (GST_BIN (p1), "p1sink");
  g_signal_emit_by_name (sink, "add", pfd[1], NULL);
  gst_object_unref (sink);
  
  src = gst_bin_get_by_name (GST_BIN (p2), "p2src");
  g_object_set (G_OBJECT (src), "fd", pfd[0], NULL);
  gst_object_unref (src);

  depay = gst_bin_get_by_name (GST_BIN (p2), "depay");
  fail_if (depay == NULL);

  pad = gst_element_get_pad (depay, "src");
  fail_unless (pad != NULL, "Could not get pad out of depay");
  gst_object_unref (depay);

  gst_pad_add_event_probe (pad, G_CALLBACK (tag_event_probe_cb), loop);

  gst_element_set_state (p1, GST_STATE_PLAYING);
  gst_element_set_state (p2, GST_STATE_PLAYING);

  g_main_loop_run (loop);

  assert_equals_int (n_tags, 1);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

/* this tests gdp-serialized Vorbis header pages being sent only once
 * to clients of multifdsink; the gdp depayloader should deserialize
 * exactly three in_caps buffers for the three header packets */

#if 0 

static int n_in_caps = 0;

static gboolean
buffer_probe_cb (GstPad * pad, GstBuffer * buffer)
{
  if (GST_BUFFER_FLAG_IS_SET (buffer, GST_BUFFER_FLAG_IN_CAPS)) {
    GstCaps *caps;
    GstStructure *s;
    const GValue *sh;
    GArray *buffers;
    GstBuffer *buf;
    int i;
    gboolean found = FALSE;

    n_in_caps++;

    caps = gst_buffer_get_caps (buffer);
    s = gst_caps_get_structure (caps, 0);
    fail_unless (gst_structure_has_field (s, "streamheader"));
    sh = gst_structure_get_value (s, "streamheader");
    buffers = g_value_peek_pointer (sh);
    assert_equals_int (buffers->len, 3);


    for (i = 0; i < 3; ++i) {
      GValue *val;

      val = &g_array_index (buffers, GValue, i);
      buf = g_value_peek_pointer (val);
      fail_unless (GST_IS_BUFFER (buf));
      if (GST_BUFFER_SIZE (buf) == GST_BUFFER_SIZE (buffer)) {
        if (memcmp (GST_BUFFER_DATA (buf), GST_BUFFER_DATA (buffer),
                GST_BUFFER_SIZE (buffer)) == 0) {
          found = TRUE;
        }
      }
    }
    fail_unless (found, "Did not find incoming IN_CAPS buffer %p on caps",
        buffer);
  }

  return TRUE;
}

void test_multifdsink_gdp_vorbisenc()
{
  GstElement *p1, *p2;
  GstElement *src, *sink, *depay;
  GstPad *pad;
  GMainLoop *loop;
  int pfd[2];

  loop = g_main_loop_new (NULL, FALSE);

  p1 = gst_parse_launch ("audiotestsrc num-buffers=1 ! audioconvert "
      " ! vorbisenc ! gdppay ! multifdsink name=p1sink", NULL);
  fail_if (p1 == NULL);
  p2 = gst_parse_launch ("fdsrc name=p2src ! gdpdepay name=depay"
      " ! fakesink name=p2sink signal-handoffs=True", NULL);
  fail_if (p2 == NULL);

  fail_if (pipe (pfd) == -1);


  gst_element_set_state (p1, GST_STATE_READY);

  sink = gst_bin_get_by_name (GST_BIN (p1), "p1sink");
  g_signal_emit_by_name (sink, "add", pfd[1], NULL);
  gst_object_unref (sink);

  src = gst_bin_get_by_name (GST_BIN (p2), "p2src");
  g_object_set (G_OBJECT (src), "fd", pfd[0], NULL);
  gst_object_unref (src);

  depay = gst_bin_get_by_name (GST_BIN (p2), "depay");
  fail_if (depay == NULL);

  pad = gst_element_get_pad (depay, "src");
  fail_unless (pad != NULL, "Could not get pad out of depay");
  gst_object_unref (depay);

  gst_pad_add_event_probe (pad, G_CALLBACK (tag_event_probe_cb), loop);
  gst_pad_add_buffer_probe (pad, G_CALLBACK (buffer_probe_cb), NULL);

  gst_element_set_state (p1, GST_STATE_PLAYING);
  gst_element_set_state (p2, GST_STATE_PLAYING);

  g_main_loop_run (loop);

  assert_equals_int (n_in_caps, 3);
}
#endif



#endif /* #ifndef GST_DISABLE_PARSE */

/*void
streamheader_suite (void)
{
test_multifdsink_gdp_tag();
test_multifdsink_gdp_vorbisenc();
}*/

void (*fn[]) (void) = {
test_multifdsink_gdp_tag,
//test_multifdsink_gdp_vorbisenc    // Droping test case Because it involves "vorbisenc", which is not supported on Symbian 
};

char *args[] = {
"test_multifdsink_gdp_tag",
//"test_multifdsink_gdp_vorbisenc"      // Droping test case Because it involves "vorbisenc", which is not supported on Symbian 
};

GST_CHECK_MAIN (streamheader);

#if 0
int main(int argc,char** argv)
{
	gst_check_init(&argc,&argv);
	test_multifdsink_gdp_tag();
}
#endif
