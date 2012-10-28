/* GStreamer
 *
 * unit test for adder
 *
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
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
#include <config.h>


/*
#ifdef HAVE_VALGRIND
# include <valgrind/valgrind.h>
#endif
*/
#include <gst/gst_global.h>
#include <unistd.h>

#define LOG_FILE "c:\\logs\\adder_logs.txt" 
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

static GMainLoop *main_loop;

static void
message_received (GstBus * bus, GstMessage * message, GstPipeline * bin)
{
  GST_INFO ("bus message from \"%" GST_PTR_FORMAT "\": %" GST_PTR_FORMAT,
      GST_MESSAGE_SRC (message), message);

  switch (message->type) {
    case GST_MESSAGE_EOS:
      g_main_loop_quit (main_loop);
      break;
    case GST_MESSAGE_WARNING:
    case GST_MESSAGE_ERROR:{
      GError *gerror;
      gchar *debug;

      gst_message_parse_error (message, &gerror, &debug);
      gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
      g_error_free (gerror);
      g_free (debug);
      g_main_loop_quit (main_loop);
    }
    default:
      break;
  }
}


static GstFormat format = GST_FORMAT_UNDEFINED;
static gint64 position = -1;

static void
test_event_message_received (GstBus * bus, GstMessage * message,
    GstPipeline * bin)
{
  GST_INFO ("bus message from \"%" GST_PTR_FORMAT "\": %" GST_PTR_FORMAT,
      GST_MESSAGE_SRC (message), message);

  switch (message->type) {
    case GST_MESSAGE_SEGMENT_DONE:
      gst_message_parse_segment_done (message, &format, &position);
      GST_INFO ("received segment_done : %" G_GINT64_FORMAT, position);
      g_main_loop_quit (main_loop);
      break;
    default:
      g_assert_not_reached ();
      break;
  }
}

void test_event()
{
  GstElement *bin, *src1, *src2, *adder, *sink;
  GstBus *bus;
  GstEvent *seek_event;
  gboolean res;
  //xmlfile = "test_event";
   std_log(LOG_FILENAME_LINE, "Test Started test_event");

  GST_INFO ("preparing test");
  /* build pipeline */
  bin = gst_pipeline_new ("pipeline");
  bus = gst_element_get_bus (bin);
  gst_bus_add_signal_watch_full (bus, G_PRIORITY_HIGH);
  
  /* FIXME, fakesrc with default setting will produce 0 sized
   * buffers and incompatible caps for adder that will make
   * adder EOS and error out */
  src1 = gst_element_factory_make ("audiotestsrc", "src1");
  g_object_set (src1, "wave", 4, NULL); /* silence */
  src2 = gst_element_factory_make ("audiotestsrc", "src2");
  
  g_object_set (src2, "wave", 4, NULL); /* silence */
  adder = gst_element_factory_make ("adder", "adder");
  sink = gst_element_factory_make ("fakesink", "sink");
  gst_bin_add_many (GST_BIN (bin), src1, src2, adder, sink, NULL);
  res = gst_element_link (src1, adder);
  fail_unless (res == TRUE, NULL);
  res = gst_element_link (src2, adder);
  fail_unless (res == TRUE, NULL);
  res = gst_element_link (adder, sink);
  fail_unless (res == TRUE, NULL);
    seek_event = gst_event_new_seek (1.0, GST_FORMAT_TIME,
      GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
      GST_SEEK_TYPE_SET, (GstClockTime) 0,
      GST_SEEK_TYPE_SET, (GstClockTime) 2 * GST_SECOND);
      

  format = GST_FORMAT_UNDEFINED;
  position = -1;
  main_loop = g_main_loop_new (NULL, FALSE);
   
  g_signal_connect (bus, "message::segment-done",
      (GCallback) test_event_message_received, bin);
       
  g_signal_connect (bus, "message::error", (GCallback) message_received, bin);
   
  g_signal_connect (bus, "message::warning", (GCallback) message_received, bin);
   
  g_signal_connect (bus, "message::eos", (GCallback) message_received, bin);

  GST_INFO ("starting test");
 
  /* prepare playing */
  res = gst_element_set_state (bin, GST_STATE_PAUSED);
  
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  /* wait for completion */
  res = gst_element_get_state (bin, NULL, NULL, GST_CLOCK_TIME_NONE);
  
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  res = gst_element_send_event (bin, seek_event);
  fail_unless (res == TRUE, NULL);

  /* run pipeline */
  res = gst_element_set_state (bin, GST_STATE_PLAYING);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);
  g_main_loop_run (main_loop);

  res = gst_element_set_state (bin, GST_STATE_NULL);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  fail_unless (position == 2 * GST_SECOND, NULL);

  /* cleanup */
  g_main_loop_unref (main_loop);
  gst_object_unref (G_OBJECT (bus));
  gst_object_unref (G_OBJECT (bin));
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



static guint play_count = 0;
static GstEvent *play_seek_event = NULL;

static void
test_play_twice_message_received (GstBus * bus, GstMessage * message,
    GstPipeline * bin)
{
  gboolean res;

  GST_INFO ("bus message from \"%" GST_PTR_FORMAT "\": %" GST_PTR_FORMAT,
      GST_MESSAGE_SRC (message), message);

  switch (message->type) {
    case GST_MESSAGE_SEGMENT_DONE:
      play_count++;
      if (play_count == 1) {
        res = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_READY);
        fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

        /* prepare playing again */
        res = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_PAUSED);
        fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

        /* wait for completion */
        res =
            gst_element_get_state (GST_ELEMENT (bin), NULL, NULL,
            GST_CLOCK_TIME_NONE);
        fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

        res = gst_element_send_event (GST_ELEMENT (bin), play_seek_event);
        fail_unless (res == TRUE, NULL);

        /* event is now gone */
        play_seek_event = NULL;

        res = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_PLAYING);
        fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);
      } else {
        g_main_loop_quit (main_loop);
      }
      break;
    default:
      g_assert_not_reached ();
      break;
  }
}


void test_play_twice()
{
  GstElement *bin, *src1, *src2, *adder, *sink;
  GstBus *bus;
  gboolean res;
  //xmlfile = "test_play_twice";
    std_log(LOG_FILENAME_LINE, "Test Started test_play_twice");
  GST_INFO ("preparing test");

  /* build pipeline */
  bin = gst_pipeline_new ("pipeline");
  bus = gst_element_get_bus (bin);
  gst_bus_add_signal_watch_full (bus, G_PRIORITY_HIGH);

  src1 = gst_element_factory_make ("audiotestsrc", "src1");
  g_object_set (src1, "wave", 4, NULL); /* silence */
  src2 = gst_element_factory_make ("audiotestsrc", "src2");
  g_object_set (src2, "wave", 4, NULL); /* silence */
  adder = gst_element_factory_make ("adder", "adder");
  sink = gst_element_factory_make ("fakesink", "sink");
  gst_bin_add_many (GST_BIN (bin), src1, src2, adder, sink, NULL);

  res = gst_element_link (src1, adder);
  fail_unless (res == TRUE, NULL);
  res = gst_element_link (src2, adder);
  fail_unless (res == TRUE, NULL);
  res = gst_element_link (adder, sink);
  fail_unless (res == TRUE, NULL);

  play_seek_event = gst_event_new_seek (1.0, GST_FORMAT_TIME,
      GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
      GST_SEEK_TYPE_SET, (GstClockTime) 0,
      GST_SEEK_TYPE_SET, (GstClockTime) 2 * GST_SECOND);

  play_count = 0;

  main_loop = g_main_loop_new (NULL, FALSE);
  g_signal_connect (bus, "message::segment-done",
      (GCallback) test_play_twice_message_received, bin);
  g_signal_connect (bus, "message::error", (GCallback) message_received, bin);
  g_signal_connect (bus, "message::warning", (GCallback) message_received, bin);
  g_signal_connect (bus, "message::eos", (GCallback) message_received, bin);

  GST_INFO ("starting test");

  /* prepare playing */
  res = gst_element_set_state (bin, GST_STATE_PAUSED);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  /* wait for completion */
  res =
      gst_element_get_state (GST_ELEMENT (bin), NULL, NULL,
      GST_CLOCK_TIME_NONE);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  res = gst_element_send_event (bin, gst_event_ref (play_seek_event));
  fail_unless (res == TRUE, NULL);

  /* run pipeline */
  res = gst_element_set_state (bin, GST_STATE_PLAYING);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  g_main_loop_run (main_loop);

  res = gst_element_set_state (bin, GST_STATE_NULL);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  fail_unless (play_count == 2, NULL);

  /* cleanup */
  g_main_loop_unref (main_loop);
  gst_object_unref (G_OBJECT (bus));
  gst_object_unref (G_OBJECT (bin));
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



/* check if adding pads work as expected */
void test_add_pad()
{
  GstElement *bin, *src1, *src2, *adder, *sink;
  GstBus *bus;
  gboolean res;
  //xmlfile = "test_add_pad";
    std_log(LOG_FILENAME_LINE, "Test Started test_add_pad");
  GST_INFO ("preparing test");

  /* build pipeline */
  bin = gst_pipeline_new ("pipeline");
  bus = gst_element_get_bus (bin);
  gst_bus_add_signal_watch_full (bus, G_PRIORITY_HIGH);

  src1 = gst_element_factory_make ("audiotestsrc", "src1");
  g_object_set (src1, "num-buffers", 4, NULL);
  g_object_set (src1, "wave", 4, NULL); /* silence */
  src2 = gst_element_factory_make ("audiotestsrc", "src2");
  /* one buffer less, we connect with 1 buffer of delay */
  g_object_set (src2, "num-buffers", 3, NULL);
  g_object_set (src2, "wave", 4, NULL); /* silence */
  adder = gst_element_factory_make ("adder", "adder");
  sink = gst_element_factory_make ("fakesink", "sink");
  gst_bin_add_many (GST_BIN (bin), src1, adder, sink, NULL);

  res = gst_element_link (src1, adder);
  fail_unless (res == TRUE, NULL);
  res = gst_element_link (adder, sink);
  fail_unless (res == TRUE, NULL);

  main_loop = g_main_loop_new (NULL, FALSE);
  g_signal_connect (bus, "message::segment-done", (GCallback) message_received,
      bin);
  g_signal_connect (bus, "message::error", (GCallback) message_received, bin);
  g_signal_connect (bus, "message::warning", (GCallback) message_received, bin);
  g_signal_connect (bus, "message::eos", (GCallback) message_received, bin);

  GST_INFO ("starting test");

  /* prepare playing */
  res = gst_element_set_state (bin, GST_STATE_PAUSED);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  /* wait for completion */
  res =
      gst_element_get_state (GST_ELEMENT (bin), NULL, NULL,
      GST_CLOCK_TIME_NONE);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  /* add other element */
  gst_bin_add_many (GST_BIN (bin), src2, NULL);

  /* now link the second element */
  res = gst_element_link (src2, adder);
  fail_unless (res == TRUE, NULL);

  /* set to PAUSED as well */
  res = gst_element_set_state (src2, GST_STATE_PAUSED);

  /* now play all */
  res = gst_element_set_state (bin, GST_STATE_PLAYING);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  g_main_loop_run (main_loop);

  res = gst_element_set_state (bin, GST_STATE_NULL);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  /* cleanup */
  g_main_loop_unref (main_loop);
  gst_object_unref (G_OBJECT (bus));
  gst_object_unref (G_OBJECT (bin));
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



/* check if removing pads work as expected */
void test_remove_pad()
{
  GstElement *bin, *src, *adder, *sink;
  GstBus *bus;
  GstPad *pad;
  gboolean res;
  //xmlfile = "test_remove_pad";
    std_log(LOG_FILENAME_LINE, "Test Started test_remove_pad");
  GST_INFO ("preparing test");

  /* build pipeline */
  bin = gst_pipeline_new ("pipeline");
  bus = gst_element_get_bus (bin);
  gst_bus_add_signal_watch_full (bus, G_PRIORITY_HIGH);

  src = gst_element_factory_make ("audiotestsrc", "src");
  g_object_set (src, "num-buffers", 4, NULL);
  g_object_set (src, "wave", 4, NULL);
  adder = gst_element_factory_make ("adder", "adder");
  sink = gst_element_factory_make ("fakesink", "sink");
  gst_bin_add_many (GST_BIN (bin), src, adder, sink, NULL);

  res = gst_element_link (src, adder);
  fail_unless (res == TRUE, NULL);
  res = gst_element_link (adder, sink);
  fail_unless (res == TRUE, NULL);

  /* create an unconnected sinkpad in adder */
  pad = gst_element_get_request_pad (adder, "sink%d");
  fail_if (pad == NULL, NULL);

  main_loop = g_main_loop_new (NULL, FALSE);
  g_signal_connect (bus, "message::segment-done", (GCallback) message_received,
      bin);
  g_signal_connect (bus, "message::error", (GCallback) message_received, bin);
  g_signal_connect (bus, "message::warning", (GCallback) message_received, bin);
  g_signal_connect (bus, "message::eos", (GCallback) message_received, bin);

  GST_INFO ("starting test");

  /* prepare playing, this will not preroll as adder is waiting
   * on the unconnected sinkpad. */
  res = gst_element_set_state (bin, GST_STATE_PAUSED);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  /* wait for completion for one second, will return ASYNC */
  res = gst_element_get_state (GST_ELEMENT (bin), NULL, NULL, GST_SECOND);
  fail_unless (res == GST_STATE_CHANGE_ASYNC, NULL);

  /* get rid of the pad now, adder should stop waiting on it and
   * continue the preroll */
  gst_element_release_request_pad (adder, pad);
  gst_object_unref (pad);

  /* wait for completion, should work now */
  res =
      gst_element_get_state (GST_ELEMENT (bin), NULL, NULL,
      GST_CLOCK_TIME_NONE);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  /* now play all */
  res = gst_element_set_state (bin, GST_STATE_PLAYING);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  g_main_loop_run (main_loop);

  res = gst_element_set_state (bin, GST_STATE_NULL);
  fail_unless (res != GST_STATE_CHANGE_FAILURE, NULL);

  /* cleanup */
  g_main_loop_unref (main_loop);
  gst_object_unref (G_OBJECT (bus));
  gst_object_unref (G_OBJECT (bin));
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


void (*fn[]) (void) = {
test_event,
test_play_twice,
test_add_pad,
test_remove_pad
};

char *args[] = {
"test_event",
"test_play_twice",
"test_add_pad",
"test_remove_pad"
};
GST_CHECK_MAIN (adder);





