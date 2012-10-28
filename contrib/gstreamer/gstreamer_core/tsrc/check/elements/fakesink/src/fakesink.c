/* GStreamer
 *
 * unit test for fakesink
 *
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
 *               <2007> Wim Taymans <wim@fluendo.com>
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

#define LOG_FILE "c:\\logs\\fakesink_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
//char* xmlfile = "fakesink";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

typedef struct
{
  GstPad *pad;
  GstBuffer *buffer;
  GThread *thread;
  GstFlowReturn ret;
} ChainData;

static gpointer
chain_async_buffer (gpointer data)
{
  ChainData *chain_data = (ChainData *) data;

  chain_data->ret = gst_pad_chain (chain_data->pad, chain_data->buffer);

  return chain_data;
}

static ChainData *
chain_async (GstPad * pad, GstBuffer * buffer)
{
  GThread *thread;
  ChainData *chain_data;
  GError *error = NULL;

  chain_data = g_new (ChainData, 1);
  chain_data->pad = pad;
  chain_data->buffer = buffer;
  chain_data->ret = GST_FLOW_ERROR;

  thread = g_thread_create (chain_async_buffer, chain_data, TRUE, &error);
  if (error != NULL) {
    g_warning ("could not create thread reason: %s", error->message);
    g_free (chain_data);
    return NULL;
  }
  chain_data->thread = thread;

  return chain_data;
}

static GstFlowReturn
chain_async_return (ChainData * data)
{
  GstFlowReturn ret;

  g_thread_join (data->thread);
  ret = data->ret;
  g_free (data);

  return ret;
}

void test_clipping()
{
  GstElement *sink;
  GstPad *sinkpad;
  GstStateChangeReturn ret;

  //xmlfile = "test_clipping";
  std_log(LOG_FILENAME_LINE, "Test Started test_clipping");
  /* create sink */
  sink = gst_element_factory_make ("fakesink", "sink");
  fail_if (sink == NULL);
  TEST_ASSERT_FAIL

  sinkpad = gst_element_get_pad (sink, "sink");
  fail_if (sinkpad == NULL);
  TEST_ASSERT_FAIL

  /* make element ready to accept data */
  ret = gst_element_set_state (sink, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC);
  TEST_ASSERT_FAIL

  /* send segment */
  {
    GstEvent *segment;
    gboolean eret;

    GST_DEBUG ("sending segment");
    segment = gst_event_new_new_segment (FALSE,
        1.0, GST_FORMAT_TIME, 1 * GST_SECOND, 5 * GST_SECOND, 1 * GST_SECOND);

    eret = gst_pad_send_event (sinkpad, segment);
    fail_if (eret == FALSE);
    TEST_ASSERT_FAIL
  }

  /* new segment should not have finished preroll */
  ret = gst_element_get_state (sink, NULL, NULL, 0);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC);
  TEST_ASSERT_FAIL

  /* send buffer that should be dropped */
  {
    GstBuffer *buffer;
    GstFlowReturn fret;

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 0;
    GST_BUFFER_DURATION (buffer) = 1 * GST_MSECOND;

    GST_DEBUG ("sending buffer to be dropped");
    fret = gst_pad_chain (sinkpad, buffer);
    fail_if (fret != GST_FLOW_OK);
    TEST_ASSERT_FAIL
  }
  /* dropped buffer should not have finished preroll */
  ret = gst_element_get_state (sink, NULL, NULL, 0);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC);
  TEST_ASSERT_FAIL

  /* send buffer that should be dropped */
  {
    GstBuffer *buffer;
    GstFlowReturn fret;

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 5 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 1 * GST_MSECOND;

    GST_DEBUG ("sending buffer to be dropped");
    fret = gst_pad_chain (sinkpad, buffer);
    fail_if (fret != GST_FLOW_OK);
    TEST_ASSERT_FAIL
  }
  /* dropped buffer should not have finished preroll */
  ret = gst_element_get_state (sink, NULL, NULL, 0);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC);
  TEST_ASSERT_FAIL

  /* send buffer that should block and finish preroll */
  {
    GstBuffer *buffer;
    GstFlowReturn fret;
    ChainData *data;
    GstState current, pending;

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 1 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 1 * GST_MSECOND;

    GST_DEBUG ("sending buffer to finish preroll");
    data = chain_async (sinkpad, buffer);
    fail_if (data == NULL);
    TEST_ASSERT_FAIL

    /* state should now eventually change to PAUSED */
    ret = gst_element_get_state (sink, &current, &pending, GST_CLOCK_TIME_NONE);
    fail_unless (ret == GST_STATE_CHANGE_SUCCESS);
    fail_unless (current == GST_STATE_PAUSED);
    fail_unless (pending == GST_STATE_VOID_PENDING);
    TEST_ASSERT_FAIL

    /* playing should render the buffer */
    ret = gst_element_set_state (sink, GST_STATE_PLAYING);
    fail_unless (ret == GST_STATE_CHANGE_SUCCESS);
    TEST_ASSERT_FAIL

    /* and we should get a success return value */
    fret = chain_async_return (data);
    fail_if (fret != GST_FLOW_OK);
    TEST_ASSERT_FAIL
  }

  /* send some buffer that will be dropped or clipped, this can 
   * only be observed in the debug log. */
  {
    GstBuffer *buffer;
    GstFlowReturn fret;

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 6 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 1 * GST_MSECOND;

    /* should be dropped */
    GST_DEBUG ("sending buffer to drop");
    fret = gst_pad_chain (sinkpad, buffer);
    fail_if (fret != GST_FLOW_OK);
    TEST_ASSERT_FAIL

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 0 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 2 * GST_SECOND;

    /* should be clipped */
    GST_DEBUG ("sending buffer to clip");
    fret = gst_pad_chain (sinkpad, buffer);
    fail_if (fret != GST_FLOW_OK);
    TEST_ASSERT_FAIL

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 4 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 2 * GST_SECOND;

    /* should be clipped */
    GST_DEBUG ("sending buffer to clip");
    fret = gst_pad_chain (sinkpad, buffer);
    fail_if (fret != GST_FLOW_OK);
    TEST_ASSERT_FAIL
  }

  gst_element_set_state (sink, GST_STATE_NULL);
  gst_element_get_state (sink, NULL, NULL, GST_CLOCK_TIME_NONE);
  gst_object_unref (sinkpad);
  gst_object_unref (sink);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_preroll_sync()
{
  GstElement *pipeline, *sink;
  GstPad *sinkpad;
  GstStateChangeReturn ret;

  //xmlfile = "test_preroll_sync";
  std_log(LOG_FILENAME_LINE, "Test Started test_preroll_sync");
  /* create sink */
  pipeline = gst_pipeline_new ("pipeline");
  fail_if (pipeline == NULL);
  TEST_ASSERT_FAIL

  sink = gst_element_factory_make ("fakesink", "sink");
  fail_if (sink == NULL);
  TEST_ASSERT_FAIL
  g_object_set (G_OBJECT (sink), "sync", TRUE, NULL);

  gst_bin_add (GST_BIN (pipeline), sink);

  sinkpad = gst_element_get_pad (sink, "sink");
  fail_if (sinkpad == NULL);
  TEST_ASSERT_FAIL

  /* make pipeline and element ready to accept data */
  ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC);
  TEST_ASSERT_FAIL

  /* send segment */
  {
    GstEvent *segment;
    gboolean eret;

    GST_DEBUG ("sending segment");
    segment = gst_event_new_new_segment (FALSE,
        1.0, GST_FORMAT_TIME, 0 * GST_SECOND, 2 * GST_SECOND, 0 * GST_SECOND);

    eret = gst_pad_send_event (sinkpad, segment);
    fail_if (eret == FALSE);
    TEST_ASSERT_FAIL
  }

  /* send buffer that should block and finish preroll */
  {
    GstBuffer *buffer;
    GstFlowReturn fret;
    ChainData *data;
    GstState current, pending;

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 1 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 1 * GST_SECOND;

    GST_DEBUG ("sending buffer to finish preroll");
    data = chain_async (sinkpad, buffer);
    fail_if (data == NULL);
    TEST_ASSERT_FAIL

    /* state should now eventually change to PAUSED */
    ret =
        gst_element_get_state (pipeline, &current, &pending,
        GST_CLOCK_TIME_NONE);
    fail_unless (ret == GST_STATE_CHANGE_SUCCESS);
    fail_unless (current == GST_STATE_PAUSED);
    fail_unless (pending == GST_STATE_VOID_PENDING);
    TEST_ASSERT_FAIL

    /* playing should render the buffer */
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    fail_unless (ret == GST_STATE_CHANGE_SUCCESS);
    TEST_ASSERT_FAIL

    /* and we should get a success return value */
    fret = chain_async_return (data);
    fail_if (fret != GST_FLOW_OK);
    TEST_ASSERT_FAIL
  }
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
  gst_object_unref (sinkpad);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



/* after EOS, we refuse everything */
void test_eos()
{
  GstElement *pipeline, *sink;
  GstPad *sinkpad;
  GstStateChangeReturn ret;
  GstMessage *message;
  GstBus *bus;

  //xmlfile = "test_eos";
  std_log(LOG_FILENAME_LINE, "Test Started test_eos");
  /* create sink */
  pipeline = gst_pipeline_new ("pipeline");
  fail_if (pipeline == NULL);
  TEST_ASSERT_FAIL

  bus = gst_pipeline_get_bus (GST_PIPELINE_CAST (pipeline));
  fail_if (bus == NULL);
  TEST_ASSERT_FAIL

  sink = gst_element_factory_make ("fakesink", "sink");
  fail_if (sink == NULL);
  TEST_ASSERT_FAIL
  g_object_set (G_OBJECT (sink), "sync", TRUE, NULL);

  gst_bin_add (GST_BIN (pipeline), sink);

  sinkpad = gst_element_get_pad (sink, "sink");
  fail_if (sinkpad == NULL);
  TEST_ASSERT_FAIL

  /* make pipeline and element ready to accept data */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC);
  TEST_ASSERT_FAIL

  /* send EOS, this should work fine */
  {
    GstEvent *eos;
    gboolean eret;

    GST_DEBUG ("sending EOS");
    eos = gst_event_new_eos ();

    eret = gst_pad_send_event (sinkpad, eos);
    fail_if (eret == FALSE);
    TEST_ASSERT_FAIL
  }

  /* wait for preroll */
  gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);

  /* EOS should be on the bus at some point */
  while (TRUE) {
    GstMessageType type;

    /* blocking wait for messages */
    message = gst_bus_timed_pop (bus, GST_CLOCK_TIME_NONE);
    type = GST_MESSAGE_TYPE (message);
    gst_message_unref (message);

    GST_DEBUG ("got message %s", gst_message_type_get_name (type));

    if (type == GST_MESSAGE_EOS)
      break;
  }
  gst_object_unref (bus);

  /* send another EOS, this should fail */
  {
    GstEvent *eos;
    gboolean eret;

    GST_DEBUG ("sending second EOS");
    eos = gst_event_new_eos ();

    eret = gst_pad_send_event (sinkpad, eos);
    fail_if (eret == TRUE);
    TEST_ASSERT_FAIL
  }

  /* send segment, this should fail */
  {
    GstEvent *segment;
    gboolean eret;

    GST_DEBUG ("sending segment");
    segment = gst_event_new_new_segment (FALSE,
        1.0, GST_FORMAT_TIME, 0 * GST_SECOND, 2 * GST_SECOND, 0 * GST_SECOND);

    eret = gst_pad_send_event (sinkpad, segment);
    fail_if (eret == TRUE);
    TEST_ASSERT_FAIL
  }

  /* send buffer that should fail after EOS */
  {
    GstBuffer *buffer;
    GstFlowReturn fret;

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 1 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 1 * GST_SECOND;

    GST_DEBUG ("sending buffer");

    /* buffer after EOS is not UNEXPECTED */
    fret = gst_pad_chain (sinkpad, buffer);
    fail_unless (fret == GST_FLOW_UNEXPECTED);
    TEST_ASSERT_FAIL
  }

  /* flush, EOS state is flushed again. */
  {
    GstEvent *event;
    gboolean eret;

    GST_DEBUG ("sending FLUSH_START");
    event = gst_event_new_flush_start ();
    eret = gst_pad_send_event (sinkpad, event);
    fail_unless (eret == TRUE);
    TEST_ASSERT_FAIL

    GST_DEBUG ("sending FLUSH_STOP");
    event = gst_event_new_flush_stop ();
    eret = gst_pad_send_event (sinkpad, event);
    fail_unless (eret == TRUE);
    TEST_ASSERT_FAIL
  }

  /* send segment, this should now work again */
  {
    GstEvent *segment;
    gboolean eret;

    GST_DEBUG ("sending segment");
    segment = gst_event_new_new_segment (FALSE,
        1.0, GST_FORMAT_TIME, 0 * GST_SECOND, 2 * GST_SECOND, 0 * GST_SECOND);

    eret = gst_pad_send_event (sinkpad, segment);
    fail_unless (eret == TRUE);
    TEST_ASSERT_FAIL
  }

  /* send buffer that should work and block */
  {
    GstBuffer *buffer;
    GstFlowReturn fret;

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 1 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 1 * GST_SECOND;

    GST_DEBUG ("sending buffer");

    fret = gst_pad_chain (sinkpad, buffer);
    fail_unless (fret == GST_FLOW_OK);
    TEST_ASSERT_FAIL
  }

  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
  gst_object_unref (sinkpad);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



/* test EOS triggered by the element */
void test_eos2()
{
  GstElement *pipeline, *sink;
  GstPad *sinkpad;
  GstStateChangeReturn ret;

  //xmlfile = "test_eos2";
  std_log(LOG_FILENAME_LINE, "Test Started test_eos2");
  /* create sink */
  pipeline = gst_pipeline_new ("pipeline");
  fail_if (pipeline == NULL);
  TEST_ASSERT_FAIL

  sink = gst_element_factory_make ("fakesink", "sink");
  fail_if (sink == NULL);
  TEST_ASSERT_FAIL
  g_object_set (G_OBJECT (sink), "sync", TRUE, NULL);
  g_object_set (G_OBJECT (sink), "num-buffers", 1, NULL);

  gst_bin_add (GST_BIN (pipeline), sink);

  sinkpad = gst_element_get_pad (sink, "sink");
  fail_if (sinkpad == NULL);
  TEST_ASSERT_FAIL

  /* make pipeline and element ready to accept data */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC);
  TEST_ASSERT_FAIL

  /* send segment, this should work */
  {
    GstEvent *segment;
    gboolean eret;

    GST_DEBUG ("sending segment");
    segment = gst_event_new_new_segment (FALSE,
        1.0, GST_FORMAT_TIME, 0 * GST_SECOND, 2 * GST_SECOND, 0 * GST_SECOND);

    eret = gst_pad_send_event (sinkpad, segment);
    fail_if (eret == FALSE);
    TEST_ASSERT_FAIL
  }

  /* send buffer that should return UNEXPECTED */
  {
    GstBuffer *buffer;
    GstFlowReturn fret;

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 1 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 1 * GST_SECOND;

    GST_DEBUG ("sending buffer");

    /* this buffer will generate UNEXPECTED */
    fret = gst_pad_chain (sinkpad, buffer);
    fail_unless (fret == GST_FLOW_UNEXPECTED);
    TEST_ASSERT_FAIL
  }

  /* send buffer that should return UNEXPECTED */
  {
    GstBuffer *buffer;
    GstFlowReturn fret;

    buffer = gst_buffer_new ();
    GST_BUFFER_TIMESTAMP (buffer) = 1 * GST_SECOND;
    GST_BUFFER_DURATION (buffer) = 1 * GST_SECOND;

    GST_DEBUG ("sending buffer");

    fret = gst_pad_chain (sinkpad, buffer);
    fail_unless (fret == GST_FLOW_UNEXPECTED);
    TEST_ASSERT_FAIL
  }

  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
  gst_object_unref (sinkpad);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


// static Suite *
// fakesink_suite (void)
// {
//   Suite *s = suite_create ("fakesink");
//   TCase *tc_chain = tcase_create ("general");
//
//   suite_add_tcase (s, tc_chain);
//   tcase_add_test (tc_chain, test_clipping);
//   tcase_add_test (tc_chain, test_preroll_sync);
//   tcase_add_test (tc_chain, test_eos);
//   tcase_add_test (tc_chain, test_eos2);
//
//   return s;
// }

void (*fn[5]) (void) = {
        test_clipping,
        test_preroll_sync,
        test_eos,
        test_eos2
};

char *args[] = {
        "test_clipping",
        "test_preroll_sync",
        "test_eos",
        "test_eos2",
};

GST_CHECK_MAIN (fakesink);
