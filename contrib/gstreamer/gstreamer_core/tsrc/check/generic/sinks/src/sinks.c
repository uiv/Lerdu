/* GStreamer
 *
 * unit test for sinks
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
#include <gst/check/gstcheck.h>
#include <libgstreamer_wsd_solution.h>
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

#define LOG_FILE "c:\\logs\\sinks_log.txt" 

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




#if 0
static void
pop_state_change_message (GstBus * bus, GstElement * src, GstState old,
    GstState new, GstState pending)
{
  GstMessage *message = NULL;
  GstState _old, _new, _pending;
    //xmlfile = "test_init";
 std_log(LOG_FILENAME_LINE, "Test Started test_init");

  message = gst_bus_poll (bus, GST_MESSAGE_STATE_CHANGED, GST_SECOND);
  fail_unless (message != NULL,
      "Expected state change message, but got nothing");

  gst_message_parse_state_changed (message, &_old, &_new, &_pending);

  fail_unless (GST_MESSAGE_SRC (message) == (GstObject *) src,
      "Unexpected state change order");
  fail_unless (old == _old, "Unexpected old state");
  fail_unless (new == _new, "Unexpected new state");
  fail_unless (pending == _pending, "Unexpected pending state");

  gst_message_unref (message);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}
#endif

/* a sink should go ASYNC to PAUSE. forcing PLAYING is possible */
void test_sink()
{
  GstElement *sink;
  GstStateChangeReturn ret;
  GstState current, pending;
    //xmlfile = "test_sink";
  std_log(LOG_FILENAME_LINE, "Test Started test_sink");
  

  sink = gst_element_factory_make ("fakesink", "sink");

  ret = gst_element_set_state (sink, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no async state return");

  ret = gst_element_set_state (sink, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no forced async state change");

  ret = gst_element_get_state (sink, &current, &pending, 0);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "not changing state async");
  fail_unless (current == GST_STATE_READY, "bad current state");
  fail_unless (pending == GST_STATE_PLAYING, "bad pending state");

  ret = gst_element_set_state (sink, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no async going back to paused");

  ret = gst_element_set_state (sink, GST_STATE_READY);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "failed to go to ready");

  ret = gst_element_set_state (sink, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "failed to go to null");

  gst_object_unref (sink);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* a sink should go ASYNC to PAUSE and PLAYING, when linking a src, it
 * should complete the state change. */
void test_sink_completion()
{
  GstElement *sink, *src;
  GstStateChangeReturn ret;
  GstState current, pending;
    //xmlfile = "test_sink_completion";
  std_log(LOG_FILENAME_LINE, "Test Started test_sink_completion");
  sink = gst_element_factory_make ("fakesink", "sink");
  g_object_set (G_OBJECT (sink), "sync", TRUE, NULL);

  ret = gst_element_set_state (sink, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no async state return");

  ret = gst_element_get_state (sink, &current, &pending, 0);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "not changing state async");
  fail_unless (current == GST_STATE_READY, "bad current state");
  fail_unless (pending == GST_STATE_PLAYING, "bad pending state");

  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "datarate", 200, "sizetype", 2, NULL);
  gst_element_link (src, sink);

  ret = gst_element_set_state (src, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no success state return");

  /* now wait for final state */
  ret = gst_element_get_state (sink, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "failed to change state");
  fail_unless (current == GST_STATE_PLAYING, "bad current state");
  fail_unless (pending == GST_STATE_VOID_PENDING, "bad pending state");

  ret = gst_element_set_state (sink, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "failed to go to null");

  ret = gst_element_set_state (src, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "failed to go to null");

  gst_object_unref (sink);
  gst_object_unref (src);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* a sink should go ASYNC to PAUSE. PAUSE should complete when
 * prerolled. */
void test_src_sink()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
    //xmlfile = "test_src_sink";
  std_log(LOG_FILENAME_LINE, "Test Started test_src_sink");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  sink = gst_element_factory_make ("fakesink", "sink");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), sink);

  srcpad = gst_element_get_pad (src, "src");
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no async state return");
  ret = gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no success state return");

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "cannot start play");

  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not playing");
  fail_unless (current == GST_STATE_PLAYING, "not playing");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not playing");
  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "cannot null pipeline");

  gst_object_unref (pipeline);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);

}


/* a pipeline with live source should return NO_PREROLL in
 * PAUSE. When removing the live source it should return ASYNC
 * from the sink */
void test_livesrc_remove()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
    //xmlfile = "test_livesrc_remove";
  std_log(LOG_FILENAME_LINE, "Test Started test_livesrc_remove");
 
  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  gst_object_ref (src);
  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), sink);

  srcpad = gst_element_get_pad (src, "src");
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no no_preroll state return");

  ret = gst_element_get_state (src, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not paused");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not playing");

  gst_bin_remove (GST_BIN (pipeline), src);

  ret = gst_element_set_state (src, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "async going to null");
  gst_object_unref (src);

  ret = gst_element_get_state (pipeline, &current, &pending, 0);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "not async");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_PAUSED, "not paused");

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "async going to null");
  gst_object_unref (pipeline);
   
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}
#if 0

/* the sink should go ASYNC to PAUSE. The live source should go
 * NO_PREROLL to PAUSE. the pipeline returns NO_PREROLL. An
 * attempt to go to PLAYING will return ASYNC. polling state
 * completion should return SUCCESS when the sink is gone to
 * PLAYING. */
void test_livesrc_sink()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
  GstBus *bus;
   // xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), sink);

  srcpad = gst_element_get_pad (src, "src");
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  bus = gst_element_get_bus (pipeline);

  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no no_preroll state return");

  pop_state_change_message (bus, sink, GST_STATE_NULL, GST_STATE_READY,
      GST_STATE_VOID_PENDING);
  pop_state_change_message (bus, src, GST_STATE_NULL, GST_STATE_READY,
      GST_STATE_VOID_PENDING);
  pop_state_change_message (bus, pipeline, GST_STATE_NULL, GST_STATE_READY,
      GST_STATE_PAUSED);

  /* this order only holds true for live sources because they do not push
     buffers in PAUSED */
  pop_state_change_message (bus, src, GST_STATE_READY, GST_STATE_PAUSED,
      GST_STATE_VOID_PENDING);
  pop_state_change_message (bus, pipeline, GST_STATE_READY, GST_STATE_PAUSED,
      GST_STATE_VOID_PENDING);

  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no no_preroll state return the second time");

  ret = gst_element_get_state (src, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not paused");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not playing");

  /* don't block here */
  ret = gst_element_get_state (sink, &current, &pending, 0);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "not async");
  fail_unless (current == GST_STATE_READY, "not ready");
  fail_unless (pending == GST_STATE_PAUSED, "not paused");

  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not paused");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not playing");

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "not async");
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not playing");
  fail_unless (current == GST_STATE_PLAYING, "not playing");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not playing");

  /* now we have four messages on the bus: src from paused to playing, sink from
     ready to paused and paused to playing, and pipeline from paused to playing.
     the pipeline message should be last, and the sink messages should go in
     order, but the src message can be interleaved with the sink one. */
  {
    GstMessage *m;
    GstState old, new, pending;
    gint n_src = 1, n_sink = 2;

    while (n_src + n_sink > 0) {
      m = gst_bus_poll (bus, GST_MESSAGE_STATE_CHANGED, GST_SECOND);
      fail_unless (m != NULL, "expected state change message");
      gst_message_parse_state_changed (m, &old, &new, &pending);
      if (GST_MESSAGE_SRC (m) == (GstObject *) src) {
        fail_unless (n_src == 1, "already got one message from the src");
        n_src--;
        fail_unless (old == GST_STATE_PAUSED, "unexpected old");
        fail_unless (new == GST_STATE_PLAYING, "unexpected new (got %d)", new);
        fail_unless (pending == GST_STATE_VOID_PENDING, "unexpected pending");
      } else if (GST_MESSAGE_SRC (m) == (GstObject *) sink) {
        if (n_sink == 2) {
          fail_unless (old == GST_STATE_READY, "unexpected old");
          fail_unless (new == GST_STATE_PAUSED, "unexpected new");
          fail_unless (pending == GST_STATE_VOID_PENDING, "unexpected pending");
        } else if (n_sink == 1) {
          fail_unless (old == GST_STATE_PAUSED, "unexpected old");
          fail_unless (new == GST_STATE_PLAYING, "unexpected new");
          fail_unless (pending == GST_STATE_VOID_PENDING, "unexpected pending");
        } else {
          g_assert_not_reached ();
        }
        n_sink--;
      } else {
        g_critical
            ("Unexpected state change message src %s (%d src %d sink pending)",
            GST_OBJECT_NAME (GST_MESSAGE_SRC (m)), n_src, n_sink);
      }
      gst_message_unref (m);
    }
  }

  pop_state_change_message (bus, pipeline, GST_STATE_PAUSED, GST_STATE_PLAYING,
      GST_STATE_VOID_PENDING);

  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* The sink should go ASYNC to PLAYING. The source should go
 * to PLAYING with SUCCESS. The pipeline returns ASYNC. */
void test_livesrc2_sink()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), sink);

  srcpad = gst_element_get_pad (src, "src");
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no async state return");

  ret = gst_element_get_state (src, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not playing");
  fail_unless (current == GST_STATE_PLAYING, "not playing");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not playing");

  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not playing");
  fail_unless (current == GST_STATE_PLAYING, "not playing");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not playing");

  /* and back down */
  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no no_preroll state return");

  ret = gst_element_get_state (src, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not no_preroll");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not paused");

  /* sink state is not known.. it might be prerolled or not */

  /* and to READY */
  ret = gst_element_set_state (pipeline, GST_STATE_READY);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no success state return");

  ret = gst_element_get_state (src, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not success");
  fail_unless (current == GST_STATE_READY, "not ready");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not ready");

  ret = gst_element_get_state (sink, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not success");
  fail_unless (current == GST_STATE_READY, "not ready");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not ready");

  /* And destroy. Must be NULL */
  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no success state return");
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_livesrc3_sink()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), sink);

  srcpad = gst_element_get_pad (src, "src");
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no async state return");

  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not playing");
  fail_unless (current == GST_STATE_PLAYING, "not playing");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not playing");

  /* and back down */
  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no success state return");
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

#endif

void test_locked_sink()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
    //xmlfile = "test_locked_sink";
  std_log(LOG_FILENAME_LINE, "Test Started test_locked_sink");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), sink);

  /* we don't link the elements */

  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no NO_PREROLL state return");

  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not no_preroll");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "have pending");

  /* the sink is now async going from ready to paused */
  ret = gst_element_get_state (sink, &current, &pending, 0);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "not async");
  fail_unless (current == GST_STATE_READY, "not ready");
  fail_unless (pending == GST_STATE_PAUSED, "not paused");

  /* lock the sink */
  gst_element_set_locked_state (sink, TRUE);

  /* move to PlAYING, the sink should remain ASYNC. The pipeline
   * returns ASYNC */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* back to PAUSED, we should get NO_PREROLL again */
  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no NO_PREROLL state return");

  /* unlock the sink */
  gst_element_set_locked_state (sink, FALSE);

  /* and now everything back down */
  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no success state return");
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_unlinked_live()
{
  GstElement *sink, *src, *lsrc, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
   // xmlfile = "test_unlinked_live";
  std_log(LOG_FILENAME_LINE, "Test Started test_unlinked_live");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  lsrc = gst_element_factory_make ("fakesrc", "lsrc");
  g_object_set (G_OBJECT (lsrc), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  gst_bin_add (GST_BIN (pipeline), src);
  gst_bin_add (GST_BIN (pipeline), lsrc);
  gst_bin_add (GST_BIN (pipeline), sink);

  /* link non live source to sink */
  srcpad = gst_element_get_pad (src, "src");
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  /* we don't link the srcpad of the live source, it will not contribute to the
   * NO_PREROLL. */

  /* set state to PAUSED, this should return NO_PREROLL because there is a live
   * source. since the only sink in this pipeline is linked to a non-live
   * source, it will preroll eventually. */
  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no NO_PREROLL state return");

  /* wait till the sink is prerolled */
  ret = gst_element_get_state (sink, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not success");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "have playing");

  /* the pipeline should still return NO_PREROLL */
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not no_preroll");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "have playing");

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not SUCCESS");

  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_delayed_async()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
   // xmlfile = "test_delayed_async";
  std_log(LOG_FILENAME_LINE, "Test Started test_delayed_async");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  /* add source, don't add sink yet */
  gst_bin_add (GST_BIN (pipeline), src);

  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no NO_PREROLL state return");

  /* add sink now and set to PAUSED */
  gst_bin_add (GST_BIN (pipeline), sink);

  /* This will make the bin notice an ASYNC element. */
  ret = gst_element_set_state (sink, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* we should still be NO_PREROLL now although there is an async element in the
   * pipeline. */
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not NO_PREROLL");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "have pending");

  /* link live source to sink */
  srcpad = gst_element_get_pad (src, "src");
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* we should get SUCCESS now */
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not NO_PREROLL");
  fail_unless (current == GST_STATE_PLAYING, "not PLAYING");
  fail_unless (pending == GST_STATE_VOID_PENDING, "have pending");

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not SUCCESS");

  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_added_async()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
    //xmlfile = "test_added_async";
  std_log(LOG_FILENAME_LINE, "Test Started test_added_async");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  /* add source, don't add sink yet */
  gst_bin_add (GST_BIN (pipeline), src);

  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no NO_PREROLL state return");

  /* set sink to PAUSED without adding it to the pipeline */
  ret = gst_element_set_state (sink, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* add sink now, pipeline should notice the async element */
  gst_bin_add (GST_BIN (pipeline), sink);

  /* we should still be NO_PREROLL now although there is an async element in the
   * pipeline. */
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not NO_PREROLL");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "have pending");

  /* link live source to sink */
  srcpad = gst_element_get_pad (src, "src");
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* we should get SUCCESS now */
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not NO_PREROLL");
  fail_unless (current == GST_STATE_PLAYING, "not PLAYING");
  fail_unless (pending == GST_STATE_VOID_PENDING, "have pending");

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not SUCCESS");

  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_added_async2()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
    //xmlfile = "test_added_async2";
  std_log(LOG_FILENAME_LINE, "Test Started test_added_async2");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  sink = gst_element_factory_make ("fakesink", "sink");

  /* add source, don't add sink yet */
  gst_bin_add (GST_BIN (pipeline), src);

  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no SUCCESS state return");

  /* set sink to PAUSED without adding it to the pipeline */
  ret = gst_element_set_state (sink, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* add sink now, pipeline should notice the async element */
  gst_bin_add (GST_BIN (pipeline), sink);

  /* we should be ASYNC now because there is an async element in the
   * pipeline. */
  ret = gst_element_get_state (pipeline, &current, &pending, 0);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "not ASYNC");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_PAUSED, "not paused");

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not SUCCESS");

  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_add_live()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
   // xmlfile = "test_add_live";
  std_log(LOG_FILENAME_LINE, "Test Started test_add_live");

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  /* add sink, don't add sourc3 yet */
  gst_bin_add (GST_BIN (pipeline), sink);

  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* set source to PAUSED without adding it to the pipeline */
  ret = gst_element_set_state (src, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no NO_PREROLL state return");

  /* add source now, pipeline should notice the NO_PREROLL element */
  gst_bin_add (GST_BIN (pipeline), src);

  /* we should be NO_PREROLL now because there is a NO_PREROLL element in the
   * pipeline. */
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not NO_PREROLL");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "have pending");

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not SUCCESS");

  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

#if 0

static GMutex *blocked_lock;
static GCond *blocked_cond;

static void
pad_blocked_cb (GstPad * pad, gboolean blocked, gpointer user_data)
{
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");
  g_mutex_lock (blocked_lock);
  GST_DEBUG ("srcpad blocked: %d, sending signal", blocked);
  g_cond_signal (blocked_cond);
  g_mutex_unlock (blocked_lock);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

void test_add_live2()
{
  GstElement *sink, *src, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
   // xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");

  blocked_lock = g_mutex_new ();
  blocked_cond = g_cond_new ();

  pipeline = gst_pipeline_new ("pipeline");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  /* add sink, don't add source yet */
  gst_bin_add (GST_BIN (pipeline), sink);

  /* set the pipeline to PLAYING. This will return ASYNC on READY->PAUSED */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  g_mutex_lock (blocked_lock);

  GST_DEBUG ("blocking srcpad");
  /* block source pad */
  srcpad = gst_element_get_pad (src, "src");
  gst_pad_set_blocked_async (srcpad, TRUE, pad_blocked_cb, NULL);

  /* set source to PAUSED without adding it to the pipeline */
  ret = gst_element_set_state (src, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no NO_PREROLL state return");

  /* add source now, pipeline should notice the NO_PREROLL element. This
   * should trigger as commit of the ASYNC pipeline and make it continue
   * to PLAYING. We blocked the source pad so that we don't get an unlinked
   * error. */
  gst_bin_add (GST_BIN (pipeline), src);

  /* wait for pad blocked, this means the source is now PLAYING. */
  g_cond_wait (blocked_cond, blocked_lock);
  g_mutex_unlock (blocked_lock);

  GST_DEBUG ("linking pads");

  /* link to sink */
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  GST_DEBUG ("unblocking srcpad");

  /* and unblock */
  gst_pad_set_blocked_async (srcpad, FALSE, pad_blocked_cb, NULL);

  GST_DEBUG ("getting state");

  /* we should be SUCCESS now and PLAYING */
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not SUCCESS");
  fail_unless (current == GST_STATE_PLAYING, "not PLAYING");
  fail_unless (pending == GST_STATE_VOID_PENDING, "have pending");

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not SUCCESS");

  g_cond_free (blocked_cond);
  g_mutex_free (blocked_lock);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

#endif

void test_bin_live()
{
  GstElement *sink, *src, *pipeline, *bin;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstPad *srcpad, *sinkpad;
    //xmlfile = "test_bin_live";
  std_log(LOG_FILENAME_LINE, "Test Started test_bin_live");

  pipeline = gst_pipeline_new ("pipeline");
  bin = gst_bin_new ("bin");
  src = gst_element_factory_make ("fakesrc", "src");
  g_object_set (G_OBJECT (src), "is-live", TRUE, NULL);
  sink = gst_element_factory_make ("fakesink", "sink");

  gst_bin_add (GST_BIN (bin), src);
  gst_bin_add (GST_BIN (bin), sink);
  gst_bin_add (GST_BIN (pipeline), bin);

  srcpad = gst_element_get_pad (src, "src");
  sinkpad = gst_element_get_pad (sink, "sink");
  gst_pad_link (srcpad, sinkpad);
  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);

  /* PAUSED returns NO_PREROLL because of the live source */
  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL,
      "no NO_PREROLL state return");
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_NO_PREROLL, "not NO_PREROLL");
  fail_unless (current == GST_STATE_PAUSED, "not paused");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not void pending");

  /* when going to PLAYING, the sink should go to PLAYING ASYNC */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "not ASYNC");

  /* now wait for PLAYING to complete */
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "not playing");
  fail_unless (current == GST_STATE_PLAYING, "not playing");
  fail_unless (pending == GST_STATE_VOID_PENDING, "not void pending");

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "cannot null pipeline");

  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}
#if 0

send_eos (GstPad * sinkpad)
{
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");
  gst_pad_send_event (sinkpad, gst_event_new_eos ());
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

/* push a buffer with a very long duration in a fakesink, then push an EOS
 * event. fakesink should emit EOS after the duration of the buffer expired.
 * Going to PAUSED, however should not return ASYNC while processing the
 * buffer. */
void test_fake_eos()
{
  GstElement *sink, *pipeline;
  GstBuffer *buffer;
  GstStateChangeReturn ret;
  GstPad *sinkpad;
  GstFlowReturn res;
  GThread *thread;
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");

  pipeline = gst_pipeline_new ("pipeline");

  sink = gst_element_factory_make ("fakesink", "sink");
  g_object_set (G_OBJECT (sink), "sync", TRUE, NULL);

  sinkpad = gst_element_get_pad (sink, "sink");

  gst_bin_add (GST_BIN_CAST (pipeline), sink);

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* push buffer of 100 seconds, since it has a timestamp of 0, it should be
   * rendered immediatly and the chain function should return immediatly */
  buffer = gst_buffer_new_and_alloc (10);
  GST_BUFFER_TIMESTAMP (buffer) = 0;
  GST_BUFFER_DURATION (buffer) = 100 * GST_SECOND;
  res = gst_pad_chain (sinkpad, buffer);
  fail_unless (res == GST_FLOW_OK, "no OK flow return");

  /* wait for preroll, this should happen really soon. */
  ret = gst_element_get_state (pipeline, NULL, NULL, -1);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no SUCCESS state return");

  /* push EOS, this will block for up to 100 seconds, until the previous
   * buffer has finished. We therefore push it in another thread so we can do
   * something else while it blocks. */
  thread = g_thread_create ((GThreadFunc) send_eos, sinkpad, TRUE, NULL);
  fail_if (thread == NULL, "no thread");

  /* wait a while so that the thread manages to start and push the EOS */
  g_usleep (G_USEC_PER_SEC);

  /* this should cancel rendering of the EOS event and should return SUCCESS
   * because the sink is now prerolled on the EOS. */
  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no SUCCESS state return");

  /* we can unref the sinkpad now, we're done with it */
  gst_object_unref (sinkpad);

  /* wait for a second, use the debug log to see that basesink does not discard
   * the EOS */
  g_usleep (G_USEC_PER_SEC);

  /* go back to PLAYING, which means waiting some more in EOS, check debug log
   * to see this happen. */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no SUCCESS state return");
  g_usleep (G_USEC_PER_SEC);

  /* teardown and cleanup */
  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS, "no SUCCESS state return");

  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* this variable is updated in the same thread, first it is set by the
 * handoff-preroll signal, then it is checked when the ASYNC_DONE is posted on
 * the bus */
static gboolean have_preroll = FALSE;

static void
async_done_handoff (GstElement * element, GstBuffer * buf, GstPad * pad,
    GstElement * sink)
{
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");
  GST_DEBUG ("we have the preroll buffer");
  have_preroll = TRUE;
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

/* when we get the ASYNC_DONE, query the position */
static GstBusSyncReply
async_done_func (GstBus * bus, GstMessage * msg, GstElement * sink)
{
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");
  if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ASYNC_DONE) {
    GstFormat format;
    gint64 position;

    GST_DEBUG ("we have ASYNC_DONE now");
    fail_unless (have_preroll == TRUE, "no preroll buffer received");

    /* get the position now */
    format = GST_FORMAT_TIME;
    gst_element_query_position (sink, &format, &position);

    GST_DEBUG ("we have position %" GST_TIME_FORMAT, GST_TIME_ARGS (position));

    fail_unless (position == 10 * GST_SECOND, "position is wrong");
  }

  /* we must unref the message if we return DROP */
  gst_message_unref (msg);

  /* we can drop the message, nothing is listening for it. */
  return GST_BUS_DROP;
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

static void
send_buffer (GstPad * sinkpad)
{
  GstBuffer *buffer;
  GstFlowReturn ret;
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");

  /* push a second buffer */
  GST_DEBUG ("pushing last buffer");
  buffer = gst_buffer_new_and_alloc (10);
  GST_BUFFER_TIMESTAMP (buffer) = 200 * GST_SECOND;
  GST_BUFFER_DURATION (buffer) = 100 * GST_SECOND;

  /* this function will initially block */
  ret = gst_pad_chain (sinkpad, buffer);
  fail_unless (ret == GST_FLOW_OK, "no OK flow return");
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

/* when we get the ASYNC_DONE message from a sink, we want the sink to be able
 * to report the duration and position. The sink should also have called the
 * render method. */
void test_async_done()
{
  GstElement *sink;
  GstBuffer *buffer;
  GstEvent *event;
  GstStateChangeReturn ret;
  GstPad *sinkpad;
  GstFlowReturn res;
  GstBus *bus;
  GThread *thread;
  GstFormat format;
  gint64 position;
  gboolean qret;
   // xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");

  sink = gst_element_factory_make ("fakesink", "sink");
  g_object_set (G_OBJECT (sink), "sync", TRUE, NULL);
  g_object_set (G_OBJECT (sink), "preroll-queue-len", 2, NULL);
  g_object_set (G_OBJECT (sink), "signal-handoffs", TRUE, NULL);

  g_signal_connect (sink, "preroll-handoff", (GCallback) async_done_handoff,
      sink);

  sinkpad = gst_element_get_pad (sink, "sink");

  ret = gst_element_set_state (sink, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* set bus on element synchronously listen for ASYNC_DONE */
  bus = gst_bus_new ();
  gst_element_set_bus (sink, bus);
  gst_bus_set_sync_handler (bus, (GstBusSyncHandler) async_done_func, sink);

  /* make newsegment, this sets the position to 10sec when the buffer prerolls */
  GST_DEBUG ("sending segment");
  event =
      gst_event_new_new_segment (FALSE, 1.0, GST_FORMAT_TIME, 0, -1,
      10 * GST_SECOND);
  res = gst_pad_send_event (sinkpad, event);

  /* We have not yet received any buffers so we are still in the READY state,
   * the position is therefore still not queryable. */
  format = GST_FORMAT_TIME;
  position = -1;
  qret = gst_element_query_position (sink, &format, &position);
  fail_unless (qret == FALSE, "position wrong");
  fail_unless (position == -1, "position is wrong");

  /* Since we are paused and the preroll queue has a length of 2, this function
   * will return immediatly, the preroll handoff will be called and the stream
   * position should now be 10 seconds. */
  GST_DEBUG ("pushing first buffer");
  buffer = gst_buffer_new_and_alloc (10);
  GST_BUFFER_TIMESTAMP (buffer) = 1 * GST_SECOND;
  GST_BUFFER_DURATION (buffer) = 100 * GST_SECOND;
  res = gst_pad_chain (sinkpad, buffer);
  fail_unless (res == GST_FLOW_OK, "no OK flow return");

  /* scond buffer, will not block either but position should still be 10
   * seconds */
  GST_DEBUG ("pushing second buffer");
  buffer = gst_buffer_new_and_alloc (10);
  GST_BUFFER_TIMESTAMP (buffer) = 100 * GST_SECOND;
  GST_BUFFER_DURATION (buffer) = 100 * GST_SECOND;
  res = gst_pad_chain (sinkpad, buffer);
  fail_unless (res == GST_FLOW_OK, "no OK flow return");

  /* check if position is still 10 seconds */
  format = GST_FORMAT_TIME;
  gst_element_query_position (sink, &format, &position);
  GST_DEBUG ("first buffer position %" GST_TIME_FORMAT,
      GST_TIME_ARGS (position));
  fail_unless (position == 10 * GST_SECOND, "position is wrong");

  /* last buffer, blocks because preroll queue is filled. Start the push in a
   * new thread so that we can check the position */
  GST_DEBUG ("starting thread");
  thread = g_thread_create ((GThreadFunc) send_buffer, sinkpad, TRUE, NULL);
  fail_if (thread == NULL, "no thread");

  GST_DEBUG ("waiting 1 second");
  g_usleep (G_USEC_PER_SEC);
  GST_DEBUG ("waiting done");

  /* check if position is still 10 seconds. This is racy because  the above
   * thread might not yet have started the push, because of the above sleep,
   * this is very unlikely, though. */
  format = GST_FORMAT_TIME;
  gst_element_query_position (sink, &format, &position);
  GST_DEBUG ("second buffer position %" GST_TIME_FORMAT,
      GST_TIME_ARGS (position));
  fail_unless (position == 10 * GST_SECOND, "position is wrong");

  /* now we go to playing. This should unlock and stop the above thread. */
  GST_DEBUG ("going to PLAYING");
  ret = gst_element_set_state (sink, GST_STATE_PLAYING);

  /* join the thread. At this point we know the sink processed the last buffer
   * and the position should now be 210 seconds; the time of the last buffer we
   * pushed */
  GST_DEBUG ("joining thread");
  g_thread_join (thread);

  format = GST_FORMAT_TIME;
  gst_element_query_position (sink, &format, &position);
  GST_DEBUG ("last buffer position %" GST_TIME_FORMAT,
      GST_TIME_ARGS (position));
  fail_unless (position == 210 * GST_SECOND, "position is wrong");

  gst_object_unref (sinkpad);

  gst_element_set_state (sink, GST_STATE_NULL);
  gst_object_unref (sink);
  gst_object_unref (bus);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* when we get the ASYNC_DONE, query the position */
static GstBusSyncReply
async_done_eos_func (GstBus * bus, GstMessage * msg, GstElement * sink)
{
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");
  if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ASYNC_DONE) {
    GstFormat format;
    gint64 position;

    GST_DEBUG ("we have ASYNC_DONE now");

    /* get the position now */
    format = GST_FORMAT_TIME;
    gst_element_query_position (sink, &format, &position);

    GST_DEBUG ("we have position %" GST_TIME_FORMAT, GST_TIME_ARGS (position));

    fail_unless (position == 10 * GST_SECOND, "position is wrong");
  }
  /* we must unref the message if we return DROP */
  gst_message_unref (msg);
  /* we can drop the message, nothing is listening for it. */
  return GST_BUS_DROP;
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

/* when we get the ASYNC_DONE message from a sink, we want the sink to be able
 * to report the duration and position. The sink should also have called the
 * render method. */
void test_async_done_eos()
{
  GstElement *sink;
  GstEvent *event;
  GstStateChangeReturn ret;
  GstPad *sinkpad;
  gboolean res; 
  GstBus *bus;
  GstFormat format;
  gint64 position;
  gboolean qret;
    //xmlfile = "test_deinit";
  std_log(LOG_FILENAME_LINE, "Test Started test_deinit");

  sink = gst_element_factory_make ("fakesink", "sink");
  g_object_set (G_OBJECT (sink), "sync", TRUE, NULL);
  g_object_set (G_OBJECT (sink), "preroll-queue-len", 1, NULL);

  sinkpad = gst_element_get_pad (sink, "sink");

  ret = gst_element_set_state (sink, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC, "no ASYNC state return");

  /* set bus on element synchronously listen for ASYNC_DONE */
  bus = gst_bus_new ();
  gst_element_set_bus (sink, bus);
  gst_bus_set_sync_handler (bus, (GstBusSyncHandler) async_done_eos_func, sink);

  /* make newsegment, this sets the position to 10sec when the buffer prerolls */
  GST_DEBUG ("sending segment");
  event =
      gst_event_new_new_segment (FALSE, 1.0, GST_FORMAT_TIME, 0, -1,
      10 * GST_SECOND);
  res = gst_pad_send_event (sinkpad, event);

  /* We have not yet received any buffers so we are still in the READY state,
   * the position is therefore still not queryable. */
  format = GST_FORMAT_TIME;
  position = -1;
  qret = gst_element_query_position (sink, &format, &position);
  fail_unless (qret == FALSE, "position wrong");
  fail_unless (position == -1, "position is wrong");

  /* Since we are paused and the preroll queue has a length of 1, this function
   * will return immediatly. The EOS will complete the preroll and the  
   * position should now be 10 seconds. */
  GST_DEBUG ("pushing EOS");
  event = gst_event_new_eos ();
  res = gst_pad_send_event (sinkpad, event);
  fail_unless (res == TRUE, "no TRUE return");

  /* check if position is still 10 seconds */
  format = GST_FORMAT_TIME;
  gst_element_query_position (sink, &format, &position);
  GST_DEBUG ("EOS position %" GST_TIME_FORMAT, GST_TIME_ARGS (position));
  fail_unless (position == 10 * GST_SECOND, "position is wrong");

  gst_object_unref (sinkpad);

  gst_element_set_state (sink, GST_STATE_NULL);
  gst_object_unref (sink);
  gst_object_unref (bus);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}
#endif




void (*fn[]) (void) = {
test_sink,
test_sink_completion,
test_src_sink,
test_livesrc_remove,
test_locked_sink,
test_unlinked_live,
test_delayed_async,
test_added_async,
test_added_async2,
test_add_live,
test_bin_live
};

char *args[] = {
"test_sink",
"test_sink_completion",
"test_src_sink",
"test_livesrc_remove",
"test_locked_sink",
"test_unlinked_live",
"test_delayed_async",
"test_added_async",
"test_added_async2",
"test_add_live",
"test_bin_live"
};

GST_CHECK_MAIN (gst_sinks);










