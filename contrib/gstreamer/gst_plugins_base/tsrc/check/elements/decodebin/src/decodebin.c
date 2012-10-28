/* GStreamer unit tests for decodebin
 *
 * Copyright (C) 2006 Tim-Philipp Müller <tim centricular net>
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
/*
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
*/

//# include <config.h>


#define LOG_FILE "c:\\logs\\decodebin_logs.txt" 

#include <gst/gst_global.h>
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include <gst/check/gstcheck.h>
#include <unistd.h>

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running;
#endif
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
extern GMutex *mutex=NULL;
#endif

//GCond *start_cond;              /* used to notify main thread of thread startups */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
extern GCond *start_cond=NULL;
#endif

//GCond *sync_cond;               /* used to synchronize all threads and main thread */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
extern GCond *sync_cond=NULL;
#endif


#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(check_mutex,gstcheck,GMutex *)
#define check_mutex (*GET_GSTREAMER_WSD_VAR_NAME(check_mutex,gstcheck,g)())
#else 
extern GMutex *check_mutex = NULL;
#endif
//
//extern GCond *check_cond;
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(check_cond,gstcheck,GCond *)
#define check_cond (*GET_GSTREAMER_WSD_VAR_NAME(check_cond,gstcheck,g)())
#else 
extern GCond *check_cond = NULL;
#endif
static const gchar dummytext[] =
    "Quick Brown Fox Jumps over a Lazy Frog Quick Brown "
    "Fox Jumps over a Lazy Frog Quick Brown Fox Jumps over a Lazy Frog Quick "
    "Brown Fox Jumps over a Lazy Frog Quick Brown Fox Jumps over a Lazy Frog "
    "Quick Brown Fox Jumps over a Lazy Frog Quick Brown Fox Jumps over a Lazy "
    "Frog Quick Brown Fox Jumps over a Lazy Frog Quick Brown Fox Jumps over a "
    "Lazy Frog Quick Brown Fox Jumps over a Lazy Frog Quick Brown Fox Jumps "
    "over a Lazy Frog Quick Brown Fox Jumps over a Lazy Frog Quick Brown Fox "
    "jumps over a Lazy Frog Quick Brown Fox Jumps over a Lazy Frog Quick Brown "
    "Fox Jumps over a Lazy Frog Quick Brown Fox Jumps over a Lazy Frog Quick "
    "Brown Fox Jumps over a Lazy Frog Quick Brown Fox Jumps over a Lazy Frog "
    "Quick Brown Fox Jumps over a Lazy Frog Quick Brown Fox Jumps over a Lazy "
    "Frog Quick Brown Fox Jumps over a Lazy Frog Quick Brown Fox Jumps over a "
    "Lazy Frog Quick Brown Fox Jumps over a Lazy Frog Quick Brown Fox Jumps "
    "over a Lazy Frog Quick Brown Fox Jumps over a Lazy Frog Quick Brown Fox ";

static void
src_handoff_cb (GstElement * src, GstBuffer * buf, GstPad * pad, gpointer data)
{
  GST_BUFFER_DATA (buf) = (guint8 *) dummytext;
  GST_BUFFER_SIZE (buf) = sizeof (dummytext);
  GST_BUFFER_OFFSET (buf) = 0;
  GST_BUFFER_FLAG_SET (buf, GST_BUFFER_FLAG_READONLY);
}

static void
decodebin_new_decoded_pad_cb (GstElement * decodebin, GstPad * pad,
    gboolean last, gboolean * p_flag)
{
  /* we should not be reached */
  fail_unless (decodebin == NULL, "new-decoded-pad should not be emitted");
}

/* make sure that decodebin errors out instead of creating a new decoded pad
 * if the entire stream is a plain text file */
void test_text_plain_streams()
{
  GstElement *pipe, *src, *decodebin;
  GstMessage *msg;
  xmlfile = "test_text_plain_streams";
    std_log(LOG_FILENAME_LINE, "Test Started test_text_plain_streams");
  pipe = gst_pipeline_new (NULL);
  fail_unless (pipe != NULL, "failed to create pipeline");
  
  src = gst_element_factory_make ("fakesrc", "src");
  fail_unless (src != NULL, "Failed to create fakesrc element");

  g_object_set (src, "signal-handoffs", TRUE, NULL);
  g_object_set (src, "num-buffers", 1, NULL);
  g_object_set (src, "can-activate-pull", FALSE, NULL);
  g_signal_connect (src, "handoff", G_CALLBACK (src_handoff_cb), NULL);

  decodebin = gst_element_factory_make ("decodebin", "decodebin");
  fail_unless (decodebin != NULL, "Failed to create decodebin element");

  g_signal_connect (decodebin, "new-decoded-pad",
      G_CALLBACK (decodebin_new_decoded_pad_cb), NULL);

  fail_unless (gst_bin_add (GST_BIN (pipe), src));
  fail_unless (gst_bin_add (GST_BIN (pipe), decodebin));
  fail_unless (gst_element_link (src, decodebin), "can't link src<->decodebin");

  fail_unless_equals_int (gst_element_set_state (pipe, GST_STATE_READY),
      GST_STATE_CHANGE_SUCCESS);
  /* it's push-based, so should be async */
  fail_unless_equals_int (gst_element_set_state (pipe, GST_STATE_PAUSED),
      GST_STATE_CHANGE_ASYNC);

  /* it should error out at some point */
  msg = gst_bus_poll (GST_ELEMENT_BUS (pipe), GST_MESSAGE_ERROR, -1);
  fail_unless (msg != NULL);
  fail_unless (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR);
  gst_message_unref (msg);
  

  gst_element_set_state (pipe, GST_STATE_NULL);
  
  gst_object_unref (pipe);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
  
  
}



static void
new_decoded_pad_plug_fakesink_cb (GstElement * decodebin, GstPad * srcpad,
    gboolean last, GstElement * pipeline)
{
  GstElement *sink;
  GstPad *sinkpad;

  sink = gst_element_factory_make ("fakesink", "sink");
  fail_unless (sink != NULL, "Failed to create fakesink element");

  gst_bin_add (GST_BIN (pipeline), sink);

  sinkpad = gst_element_get_static_pad (sink, "sink");
  fail_unless_equals_int (gst_pad_link (srcpad, sinkpad), GST_PAD_LINK_OK);
  gst_object_unref (sinkpad);

  gst_element_set_state (sink, GST_STATE_PLAYING);
  
}

void test_reuse_without_decoders()
{
  GstElement *pipe, *src, *decodebin, *sink;

  pipe = gst_pipeline_new (NULL);
   xmlfile = "test_reuse_without_decoders";
  std_log(LOG_FILENAME_LINE, "Test Started test_reuse_without_decoders");
  fail_unless (pipe != NULL, "failed to create pipeline");

  src = gst_element_factory_make ("audiotestsrc", "src");
  fail_unless (src != NULL, "Failed to create audiotestsrc element");

  decodebin = gst_element_factory_make ("decodebin", "decodebin");
  fail_unless (decodebin != NULL, "Failed to create decodebin element");

  g_signal_connect (decodebin, "new-decoded-pad",
      G_CALLBACK (new_decoded_pad_plug_fakesink_cb), pipe);

  fail_unless (gst_bin_add (GST_BIN (pipe), src));
  fail_unless (gst_bin_add (GST_BIN (pipe), decodebin));
  fail_unless (gst_element_link (src, decodebin), "can't link src<->decodebin");

  fail_unless_equals_int (gst_element_set_state (pipe, GST_STATE_READY),
      GST_STATE_CHANGE_SUCCESS);
  /* it's push-based, so should be async */
  fail_unless_equals_int (gst_element_set_state (pipe, GST_STATE_PAUSED),
      GST_STATE_CHANGE_ASYNC);

  /* wait for state change to complete */
  fail_unless_equals_int (gst_element_get_state (pipe, NULL, NULL, -1),
      GST_STATE_CHANGE_SUCCESS);

  /* there shouldn't be any errors */
  fail_if (gst_bus_poll (GST_ELEMENT_BUS (pipe), GST_MESSAGE_ERROR, 0) != NULL);

  /* reset */
  gst_element_set_state (pipe, GST_STATE_NULL);

  sink = gst_bin_get_by_name (GST_BIN (pipe), "sink");
  gst_bin_remove (GST_BIN (pipe), sink);
  gst_object_unref (sink);

  GST_LOG ("second try");

  fail_unless_equals_int (gst_element_set_state (pipe, GST_STATE_READY),
      GST_STATE_CHANGE_SUCCESS);
  /* it's push-based, so should be async */
  fail_unless_equals_int (gst_element_set_state (pipe, GST_STATE_PAUSED),
      GST_STATE_CHANGE_ASYNC);

  /* wait for state change to complete */
  fail_unless_equals_int (gst_element_get_state (pipe, NULL, NULL, -1),
      GST_STATE_CHANGE_SUCCESS);

  /* there shouldn't be any errors */
  fail_if (gst_bus_poll (GST_ELEMENT_BUS (pipe), GST_MESSAGE_ERROR, 0) != NULL);

  gst_element_set_state (pipe, GST_STATE_NULL);
  gst_object_unref (pipe);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


/*
void
decodebin_suite (void)
{
test_text_plain_streams();
test_reuse_without_decoders();
}

void (*fn[]) (void) = {
test_text_plain_streams,
test_reuse_without_decoders
};

char *args[] = {
"test_text_plain_streams",
"test_reuse_without_decoders"
};
*/
//GST_CHECK_MAIN (decodebin);

int main (int argc, char **argv)
{
//gst_init(&argc, &argv);                
gst_check_init (NULL, NULL); 
test_text_plain_streams();
test_reuse_without_decoders();

}
