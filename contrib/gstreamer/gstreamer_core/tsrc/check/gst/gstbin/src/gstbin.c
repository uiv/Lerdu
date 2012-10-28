/* GStreamer
 * Copyright (C) 2005 Wim Taymans <wim@fluendo.com>
 * Copyright (C) 2005 Thomas Vander Stichele <thomas at apestaart dot org>
 *
 * gstbin.c: Unit test for GstBin
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
#include <glib_global.h>



#define LOG_FILE "c:\\logs\\gstbin_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

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
//gboolean _gst_check_threads_running = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running;
#endif

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(thread_list,gstcheck,GList*)
#define thread_list (*GET_GSTREAMER_WSD_VAR_NAME(thread_list,gstcheck,g)())
#else 
GList *thread_list;
#endif

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(mutex,gstcheck,GMutex*)
#define mutex (*GET_GSTREAMER_WSD_VAR_NAME(mutex,gstcheck,g)())
#else 
GMutex *mutex;
#endif

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
GCond *start_cond;
#endif

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
GCond *sync_cond;
#endif

static void
pop_async_done (GstBus * bus)
{
  GstMessage *message;

  GST_DEBUG ("popping async-done message");
  message = gst_bus_poll (bus, GST_MESSAGE_ASYNC_DONE, -1);

  fail_unless (message && GST_MESSAGE_TYPE (message)
      == GST_MESSAGE_ASYNC_DONE, "did not get GST_MESSAGE_ASYNC_DONE");

  gst_message_unref (message);
  GST_DEBUG ("popped message");
}

static void
pop_messages (GstBus * bus, int count)
{
  GstMessage *message;

  int i;

  GST_DEBUG ("popping %d messages", count);
  for (i = 0; i < count; ++i) {
    message = gst_bus_poll (bus, GST_MESSAGE_STATE_CHANGED, -1);

    fail_unless (message && GST_MESSAGE_TYPE (message)
        == GST_MESSAGE_STATE_CHANGED, "did not get GST_MESSAGE_STATE_CHANGED");

    gst_message_unref (message);
  }
  GST_DEBUG ("popped %d messages", count);
}

void test_interface()
{
  GstBin *bin, *bin2;
  GstElement *filesrc;
  GstIterator *it;
  gpointer item;

  xmlfile = "test_interface";
  std_log(LOG_FILENAME_LINE, "Test Started test_interface");
  bin = GST_BIN (gst_bin_new (NULL));
  fail_unless (bin != NULL, "Could not create bin");

  filesrc = gst_element_factory_make ("filesrc", NULL);
  fail_unless (filesrc != NULL, "Could not create filesrc");
  fail_unless (GST_IS_URI_HANDLER (filesrc), "Filesrc not a URI handler");
  gst_bin_add (bin, filesrc);

  fail_unless (gst_bin_get_by_interface (bin, GST_TYPE_URI_HANDLER) == filesrc);
  gst_object_unref (filesrc);

  it = gst_bin_iterate_all_by_interface (bin, GST_TYPE_URI_HANDLER);
  fail_unless (it != NULL);
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_OK);
  fail_unless (item == (gpointer) filesrc);
  gst_object_unref (GST_OBJECT (item));
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_DONE);
  gst_iterator_free (it);

  gst_bin_add_many (bin,
      gst_element_factory_make ("identity", NULL),
      gst_element_factory_make ("identity", NULL),
      gst_element_factory_make ("identity", NULL), NULL);
  it = gst_bin_iterate_all_by_interface (bin, GST_TYPE_URI_HANDLER);
  fail_unless (it != NULL);
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_OK);
  fail_unless (item == (gpointer) filesrc);
  gst_object_unref (GST_OBJECT (item));
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_DONE);
  gst_iterator_free (it);

  bin2 = bin;
  bin = GST_BIN (gst_bin_new (NULL));
  fail_unless (bin != NULL);
  gst_bin_add_many (bin,
      gst_element_factory_make ("identity", NULL),
      gst_element_factory_make ("identity", NULL),
      GST_ELEMENT (bin2), gst_element_factory_make ("identity", NULL), NULL);
  it = gst_bin_iterate_all_by_interface (bin, GST_TYPE_URI_HANDLER);
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_OK);
  fail_unless (item == (gpointer) filesrc);
  gst_object_unref (GST_OBJECT (item));
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_DONE);
  gst_iterator_free (it);

  gst_bin_add (bin, gst_element_factory_make ("filesrc", NULL));
  gst_bin_add (bin2, gst_element_factory_make ("filesrc", NULL));
  it = gst_bin_iterate_all_by_interface (bin, GST_TYPE_URI_HANDLER);
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_OK);
  gst_object_unref (GST_OBJECT (item));
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_OK);
  gst_object_unref (GST_OBJECT (item));
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_OK);
  gst_object_unref (GST_OBJECT (item));
  fail_unless (gst_iterator_next (it, &item) == GST_ITERATOR_DONE);
  gst_iterator_free (it);

  gst_object_unref (bin);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_message_state_changed()
{
  GstBin *bin;
  GstBus *bus;
  GstMessage *message;
  GstStateChangeReturn ret;
  
  xmlfile = "test_message_state_changed";
    std_log(LOG_FILENAME_LINE, "Test Started test_message_state_changed"); 

  bin = GST_BIN (gst_bin_new (NULL));
  fail_unless (bin != NULL, "Could not create bin");
  ASSERT_OBJECT_REFCOUNT (bin, "bin", 1);

  bus = g_object_new (gst_bus_get_type (), NULL);
  gst_element_set_bus (GST_ELEMENT_CAST (bin), bus);

  /* change state, spawning a message, causing an incref on the bin */
  ret = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_READY);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);

  ASSERT_OBJECT_REFCOUNT (bin, "bin", 2);

  /* get and unref the message, causing a decref on the bin */
  message = gst_bus_poll (bus, GST_MESSAGE_STATE_CHANGED, -1);

  fail_unless (message && GST_MESSAGE_TYPE (message)
      == GST_MESSAGE_STATE_CHANGED, "did not get GST_MESSAGE_STATE_CHANGED");

  gst_message_unref (message);

  ASSERT_OBJECT_REFCOUNT (bin, "bin", 1);

  /* clean up */
  ret = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);

  gst_object_unref (bus);
  gst_object_unref (bin);
  

  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
    
}



void test_message_state_changed_child()
{
  GstBin *bin;
  GstElement *src;
  GstBus *bus;
  GstMessage *message;
  GstStateChangeReturn ret;
	xmlfile = "test_message_state_changed_child";
    std_log(LOG_FILENAME_LINE, "Test Started test_message_state_changed_child"); 
  bin = GST_BIN (gst_bin_new (NULL));
  fail_unless (bin != NULL, "Could not create bin");
  ASSERT_OBJECT_REFCOUNT (bin, "bin", 1);

  bus = g_object_new (gst_bus_get_type (), NULL);
  gst_element_set_bus (GST_ELEMENT_CAST (bin), bus);

  src = gst_element_factory_make ("fakesrc", NULL);
  fail_if (src == NULL, "Could not create fakesrc");
  gst_bin_add (bin, src);
  ASSERT_OBJECT_REFCOUNT (bin, "bin", 1);
  ASSERT_OBJECT_REFCOUNT (src, "src", 1);

  /* change state, spawning two messages:
   * - first for fakesrc, forwarded to bin's bus, causing incref on fakesrc
   * - second for bin, causing an incref on the bin */
  GST_DEBUG ("setting bin to READY");
  ret = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_READY);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);

  ASSERT_OBJECT_REFCOUNT (src, "src", 2);
  ASSERT_OBJECT_REFCOUNT (bin, "bin", 2);

  /* get and unref the message, causing a decref on the src */
  message = gst_bus_poll (bus, GST_MESSAGE_STATE_CHANGED, -1);
  fail_unless (message && GST_MESSAGE_TYPE (message)
      == GST_MESSAGE_STATE_CHANGED, "did not get GST_MESSAGE_STATE_CHANGED");

  fail_unless (message->src == GST_OBJECT (src));
  gst_message_unref (message);

  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (bin, "bin", 2);

  /* get and unref message 2, causing a decref on the bin */
  message = gst_bus_poll (bus, GST_MESSAGE_STATE_CHANGED, -1);
  fail_unless (message && GST_MESSAGE_TYPE (message)
      == GST_MESSAGE_STATE_CHANGED, "did not get GST_MESSAGE_STATE_CHANGED");

  fail_unless (message->src == GST_OBJECT (bin));
  gst_message_unref (message);

  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (bin, "bin", 1);

  /* clean up */
  ret = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);
  gst_object_unref (bus);
  gst_object_unref (bin);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_message_state_changed_children()
{
  GstPipeline *pipeline;
  GstElement *src, *sink;
  GstBus *bus;
  GstStateChangeReturn ret;
  GstState current, pending;
  xmlfile = "test_message_state_changed_children";
    std_log(LOG_FILENAME_LINE, "Test Started test_message_state_changed_children"); 

  pipeline = GST_PIPELINE (gst_pipeline_new (NULL));
  fail_unless (pipeline != NULL, "Could not create pipeline");
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  src = gst_element_factory_make ("fakesrc", NULL);
  fail_if (src == NULL, "Could not create fakesrc");
  /* need to silence the element as the deep_notify refcounts the
   * parents while running */
  g_object_set (G_OBJECT (src), "silent", TRUE, NULL);
  gst_bin_add (GST_BIN (pipeline), src);

  sink = gst_element_factory_make ("fakesink", NULL);
  /* need to silence the element as the deep_notify refcounts the
   * parents while running */
  g_object_set (G_OBJECT (sink), "silent", TRUE, NULL);
  fail_if (sink == NULL, "Could not create fakesink");
  gst_bin_add (GST_BIN (pipeline), sink);

  fail_unless (gst_element_link (src, sink), "could not link src and sink");

  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);
  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);

  bus = gst_pipeline_get_bus (pipeline);

  /* change state to READY, spawning three messages */
  GST_DEBUG ("setting pipeline to READY");
  ret = gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_READY);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);

  /* each object is referenced by a message */
  ASSERT_OBJECT_REFCOUNT (bus, "bus", 2);
  ASSERT_OBJECT_REFCOUNT (src, "src", 2);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 2);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 2);

  pop_messages (bus, 3);
  fail_if (gst_bus_have_pending (bus), "unexpected pending messages");

  ASSERT_OBJECT_REFCOUNT (bus, "bus", 2);
  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  /* change state to PAUSED, spawning three messages */
  GST_DEBUG ("setting pipeline to PAUSED");
  ret = gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC);
  ret =
      gst_element_get_state (GST_ELEMENT (pipeline), &current, &pending,
      GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);
  fail_unless (current == GST_STATE_PAUSED);
  fail_unless (pending == GST_STATE_VOID_PENDING);

  /* wait for async thread to settle down */
  while (GST_OBJECT_REFCOUNT_VALUE (pipeline) > 3)
    THREAD_SWITCH ();

  /* each object is referenced by a message;
   * base_src is blocked in the push and has an extra refcount.
   * base_sink_chain has taken a refcount on the sink, and is blocked on
   * preroll */
  ASSERT_OBJECT_REFCOUNT (src, "src", 2);
  /* refcount can be 4 if the bin is still processing the async_done message of
   * the sink. */
  ASSERT_OBJECT_REFCOUNT_BETWEEN (sink, "sink", 2, 3);
  /* 2 or 3 is valid, because the pipeline might still be posting 
   * its state_change message */
  ASSERT_OBJECT_REFCOUNT_BETWEEN (pipeline, "pipeline", 2, 3);

  pop_messages (bus, 3);
  pop_async_done (bus);
  fail_if ((gst_bus_pop (bus)) != NULL);

  ASSERT_OBJECT_REFCOUNT (bus, "bus", 2);
  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  /* change state to PLAYING, spawning three messages */
  GST_DEBUG ("setting pipeline to PLAYING");
  ret = gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);
  ret =
      gst_element_get_state (GST_ELEMENT (pipeline), &current, &pending,
      GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);
  fail_unless (current == GST_STATE_PLAYING);
  fail_unless (pending == GST_STATE_VOID_PENDING);

  /* each object is referenced by one message
   * src might have an extra reference if it's still pushing
   * sink might have an extra reference if it's still blocked on preroll
   * pipeline posted a new-clock message too. */
  ASSERT_OBJECT_REFCOUNT_BETWEEN (src, "src", 2, 3);
  ASSERT_OBJECT_REFCOUNT_BETWEEN (sink, "sink", 2, 3);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 3);

  pop_messages (bus, 3);
  fail_if ((gst_bus_pop (bus)) != NULL);

  ASSERT_OBJECT_REFCOUNT (bus, "bus", 2);
  /* src might have an extra reference if it's still pushing */
  ASSERT_OBJECT_REFCOUNT_BETWEEN (src, "src", 1, 2);
  /* sink might have an extra reference if it's still blocked on preroll */
  ASSERT_OBJECT_REFCOUNT_BETWEEN (sink, "sink", 1, 2);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  /* go back to READY, spawning six messages */
  GST_DEBUG ("setting pipeline to READY");
  ret = gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_READY);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);

  /* each object is referenced by two messages */
  ASSERT_OBJECT_REFCOUNT (src, "src", 3);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 3);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 3);

  pop_messages (bus, 6);
  fail_if ((gst_bus_pop (bus)) != NULL);

  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  /* setting pipeline to NULL flushes the bus automatically */
  ret = gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);

  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  /* clean up */
  gst_object_unref (bus);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_watch_for_state_change()
{
  GstElement *src, *sink, *bin;
  GstBus *bus;
  GstStateChangeReturn ret;
  
  xmlfile = "test_watch_for_state_change";
    std_log(LOG_FILENAME_LINE, "Test Started test_watch_for_state_change"); 

  bin = gst_element_factory_make ("bin", NULL);
  fail_unless (bin != NULL, "Could not create bin");

  bus = g_object_new (gst_bus_get_type (), NULL);
  gst_element_set_bus (GST_ELEMENT_CAST (bin), bus);

  src = gst_element_factory_make ("fakesrc", NULL);
  fail_if (src == NULL, "Could not create fakesrc");
  sink = gst_element_factory_make ("fakesink", NULL);
  fail_if (sink == NULL, "Could not create fakesink");

  gst_bin_add (GST_BIN (bin), sink);
  gst_bin_add (GST_BIN (bin), src);

  fail_unless (gst_element_link (src, sink), "could not link src and sink");

  /* change state, spawning two times three messages */
  ret = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_PAUSED);
  fail_unless (ret == GST_STATE_CHANGE_ASYNC);
  ret =
      gst_element_get_state (GST_ELEMENT (bin), NULL, NULL,
      GST_CLOCK_TIME_NONE);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);

  pop_messages (bus, 6);
  pop_async_done (bus);

  fail_unless (gst_bus_have_pending (bus) == FALSE,
      "Unexpected messages on bus");

  ret = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_PLAYING);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);

  pop_messages (bus, 3);

  /* this one might return either SUCCESS or ASYNC, likely SUCCESS */
  ret = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_PAUSED);
  gst_element_get_state (GST_ELEMENT (bin), NULL, NULL, GST_CLOCK_TIME_NONE);

  pop_messages (bus, 3);
  if (ret == GST_STATE_CHANGE_ASYNC)
    pop_async_done (bus);

  fail_unless (gst_bus_have_pending (bus) == FALSE,
      "Unexpected messages on bus");

  /* setting bin to NULL flushes the bus automatically */
  ret = gst_element_set_state (GST_ELEMENT (bin), GST_STATE_NULL);
  fail_unless (ret == GST_STATE_CHANGE_SUCCESS);

  /* clean up */
  gst_object_unref (bus);
  gst_object_unref (bin);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* adding an element with linked pads to a bin unlinks the
 * pads */
void test_add_linked()
{
  GstElement *src, *sink;
  GstPad *srcpad, *sinkpad;
  GstElement *pipeline;
  
  xmlfile = "test_add_linked";
    std_log(LOG_FILENAME_LINE, "Test Started test_add_linked"); 

  pipeline = gst_pipeline_new (NULL);
  fail_unless (pipeline != NULL, "Could not create pipeline");

  src = gst_element_factory_make ("fakesrc", NULL);
  fail_if (src == NULL, "Could not create fakesrc");
  sink = gst_element_factory_make ("fakesink", NULL);
  fail_if (sink == NULL, "Could not create fakesink");

  srcpad = gst_element_get_pad (src, "src");
  fail_unless (srcpad != NULL);
  sinkpad = gst_element_get_pad (sink, "sink");
  fail_unless (sinkpad != NULL);

  fail_unless (gst_pad_link (srcpad, sinkpad) == GST_PAD_LINK_OK);

  /* pads are linked now */
  fail_unless (gst_pad_is_linked (srcpad));
  fail_unless (gst_pad_is_linked (sinkpad));

  /* adding element to bin voids hierarchy so pads are unlinked */
  gst_bin_add (GST_BIN (pipeline), src);

  /* check if pads really are unlinked */
  fail_unless (!gst_pad_is_linked (srcpad));
  fail_unless (!gst_pad_is_linked (sinkpad));

  /* cannot link pads in wrong hierarchy */
  fail_unless (gst_pad_link (srcpad, sinkpad) == GST_PAD_LINK_WRONG_HIERARCHY);

  /* adding other element to bin as well */
  gst_bin_add (GST_BIN (pipeline), sink);

  /* now we can link again */
  fail_unless (gst_pad_link (srcpad, sinkpad) == GST_PAD_LINK_OK);

  /* check if pads really are linked */
  fail_unless (gst_pad_is_linked (srcpad));
  fail_unless (gst_pad_is_linked (sinkpad));

  gst_object_unref (srcpad);
  gst_object_unref (sinkpad);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* adding ourself should fail */
void test_add_self()
{
  GstElement *bin;
  xmlfile = "test_add_self";
    std_log(LOG_FILENAME_LINE, "Test Started test_add_self"); 

  bin = gst_bin_new (NULL);
  fail_unless (bin != NULL, "Could not create bin");

  
  ASSERT_WARNING (gst_bin_add (GST_BIN (bin), bin));

  gst_object_unref (bin);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}




/* g_print ("%10s: %4d => %4d\n", GST_OBJECT_NAME (msg->src), old, new); */

#define ASSERT_STATE_CHANGE_MSG(bus,element,old_state,new_state,num)          \
  {                                                                           \
    GstMessage *msg;                                                          \
    GstState old = 0, new = 0, pending = 0;                                   \
    msg = gst_bus_poll (bus, GST_MESSAGE_STATE_CHANGED, GST_SECOND);          \
    fail_if (msg == NULL, "No state change message within 1 second (#"        \
        G_STRINGIFY (num) ")");                                               \
    gst_message_parse_state_changed (msg, &old, &new, &pending);              \
    fail_if (msg->src != GST_OBJECT (element), G_STRINGIFY(element)           \
        " should have changed state next (#" G_STRINGIFY (num) ")");          \
    fail_if (old != old_state || new != new_state, "state change is not "     \
        G_STRINGIFY (old_state) " => " G_STRINGIFY (new_state));              \
    gst_message_unref (msg);                                                  \
  }

void test_children_state_change_order_flagged_sink()
{
  GstElement *src, *identity, *sink, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstBus *bus;
  
  xmlfile = "test_children_state_change_order_flagged_sink";
    std_log(LOG_FILENAME_LINE, "Test Started test_children_state_change_order_flagged_sink"); 

  pipeline = gst_pipeline_new (NULL);
  fail_unless (pipeline != NULL, "Could not create pipeline");

  bus = gst_element_get_bus (pipeline);
  fail_unless (bus != NULL, "Pipeline has no bus?!");

  src = gst_element_factory_make ("fakesrc", NULL);
  fail_if (src == NULL, "Could not create fakesrc");
  g_object_set (src, "num-buffers", 5, NULL);

  identity = gst_element_factory_make ("identity", NULL);
  fail_if (identity == NULL, "Could not create identity");

  sink = gst_element_factory_make ("fakesink", NULL);
  fail_if (sink == NULL, "Could not create fakesink");

  gst_bin_add_many (GST_BIN (pipeline), src, identity, sink, NULL);

  fail_unless (gst_element_link (src, identity) == TRUE);
  fail_unless (gst_element_link (identity, sink) == TRUE);

  /* (1) Test state change with fakesink being a regular sink */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_if (ret != GST_STATE_CHANGE_ASYNC,
      "State change to PLAYING did not return ASYNC");
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_if (ret != GST_STATE_CHANGE_SUCCESS, "State change to PLAYING failed");
  fail_if (current != GST_STATE_PLAYING, "State change to PLAYING failed");
  fail_if (pending != GST_STATE_VOID_PENDING, "State change to PLAYING failed");

  /* NULL => READY */
  ASSERT_STATE_CHANGE_MSG (bus, sink, GST_STATE_NULL, GST_STATE_READY, 101);
  ASSERT_STATE_CHANGE_MSG (bus, identity, GST_STATE_NULL, GST_STATE_READY, 102);
  ASSERT_STATE_CHANGE_MSG (bus, src, GST_STATE_NULL, GST_STATE_READY, 103);
  ASSERT_STATE_CHANGE_MSG (bus, pipeline, GST_STATE_NULL, GST_STATE_READY, 104);

  /* READY => PAUSED */
  /* because of pre-rolling, sink will return ASYNC on state
   * change and change state later when it has a buffer */
  GST_DEBUG ("popping READY -> PAUSED messages");
  ASSERT_STATE_CHANGE_MSG (bus, identity, GST_STATE_READY, GST_STATE_PAUSED,
      105);
#if 0
  /* From here on, all bets are off. Usually the source changes state next,
   * but it might just as well be that the first buffer produced by the
   * source reaches the sink before the source has finished its state change,
   * in which case the sink will commit its new state before the source ...  */
  ASSERT_STATE_CHANGE_MSG (bus, src, GST_STATE_READY, GST_STATE_PAUSED, 106);
  ASSERT_STATE_CHANGE_MSG (bus, sink, GST_STATE_READY, GST_STATE_PAUSED, 107);
#else

  pop_messages (bus, 2);        /* pop remaining ready => paused messages off the bus */
  ASSERT_STATE_CHANGE_MSG (bus, pipeline, GST_STATE_READY, GST_STATE_PAUSED,
      108);
  pop_async_done (bus);
#endif
  /* PAUSED => PLAYING */
  GST_DEBUG ("popping PAUSED -> PLAYING messages");
  ASSERT_STATE_CHANGE_MSG (bus, sink, GST_STATE_PAUSED, GST_STATE_PLAYING, 109);
  ASSERT_STATE_CHANGE_MSG (bus, identity, GST_STATE_PAUSED, GST_STATE_PLAYING,
      110);
  ASSERT_STATE_CHANGE_MSG (bus, src, GST_STATE_PAUSED, GST_STATE_PLAYING, 111);
  ASSERT_STATE_CHANGE_MSG (bus, pipeline, GST_STATE_PAUSED, GST_STATE_PLAYING,
      112);

  /* don't set to NULL that will set the bus flushing and kill our messages */
  ret = gst_element_set_state (pipeline, GST_STATE_READY);
  fail_if (ret != GST_STATE_CHANGE_SUCCESS, "State change to READY failed");
  ret = gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
  fail_if (ret != GST_STATE_CHANGE_SUCCESS, "State change to READY failed");

  /* TODO: do we need to check downwards state change order as well? */
  pop_messages (bus, 4);        /* pop playing => paused messages off the bus */
  pop_messages (bus, 4);        /* pop paused => ready messages off the bus */

  while (GST_OBJECT_REFCOUNT_VALUE (pipeline) > 1)
    THREAD_SWITCH ();

  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_if (ret != GST_STATE_CHANGE_SUCCESS, "State change to NULL failed");

  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  gst_object_unref (bus);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}




void test_children_state_change_order_semi_sink()
{
  GstElement *src, *identity, *sink, *pipeline;
  GstStateChangeReturn ret;
  GstState current, pending;
  GstBus *bus;
  
  xmlfile = "test_children_state_change_order_semi_sink";
    std_log(LOG_FILENAME_LINE, "Test Started test_children_state_change_order_semi_sink"); 

  /* (2) Now again, but check other code path where we don't have
   *     a proper sink correctly flagged as such, but a 'semi-sink' */
  pipeline = gst_pipeline_new (NULL);
  fail_unless (pipeline != NULL, "Could not create pipeline");

  bus = gst_element_get_bus (pipeline);
  fail_unless (bus != NULL, "Pipeline has no bus?!");

  src = gst_element_factory_make ("fakesrc", NULL);
  fail_if (src == NULL, "Could not create fakesrc");

  identity = gst_element_factory_make ("identity", NULL);
  fail_if (identity == NULL, "Could not create identity");

  sink = gst_element_factory_make ("fakesink", NULL);
  fail_if (sink == NULL, "Could not create fakesink");

  gst_bin_add_many (GST_BIN (pipeline), src, identity, sink, NULL);

  fail_unless (gst_element_link (src, identity) == TRUE);
  fail_unless (gst_element_link (identity, sink) == TRUE);

  /* this is not very nice but should work just fine in this case. */
  GST_OBJECT_FLAG_UNSET (sink, GST_ELEMENT_IS_SINK);    /* <======== */

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  fail_if (ret != GST_STATE_CHANGE_ASYNC, "State change to PLAYING not ASYNC");
  ret =
      gst_element_get_state (pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
  fail_if (ret != GST_STATE_CHANGE_SUCCESS, "State change to PLAYING failed");
  fail_if (current != GST_STATE_PLAYING, "State change to PLAYING failed");
  fail_if (pending != GST_STATE_VOID_PENDING, "State change to PLAYING failed");

  /* NULL => READY */
  ASSERT_STATE_CHANGE_MSG (bus, sink, GST_STATE_NULL, GST_STATE_READY, 201);
  ASSERT_STATE_CHANGE_MSG (bus, identity, GST_STATE_NULL, GST_STATE_READY, 202);
  ASSERT_STATE_CHANGE_MSG (bus, src, GST_STATE_NULL, GST_STATE_READY, 203);
  ASSERT_STATE_CHANGE_MSG (bus, pipeline, GST_STATE_NULL, GST_STATE_READY, 204);

  /* READY => PAUSED */
  /* because of pre-rolling, sink will return ASYNC on state
   * change and change state later when it has a buffer */
  GST_DEBUG ("popping READY -> PAUSED messages");
  ASSERT_STATE_CHANGE_MSG (bus, identity, GST_STATE_READY, GST_STATE_PAUSED,
      205);
#if 0
  /* From here on, all bets are off. Usually the source changes state next,
   * but it might just as well be that the first buffer produced by the
   * source reaches the sink before the source has finished its state change,
   * in which case the sink will commit its new state before the source ...  */
  ASSERT_STATE_CHANGE_MSG (bus, src, GST_STATE_READY, GST_STATE_PAUSED, 206);
  ASSERT_STATE_CHANGE_MSG (bus, sink, GST_STATE_READY, GST_STATE_PAUSED, 207);
#else
  pop_messages (bus, 2);        /* pop remaining ready => paused messages off the bus */
  ASSERT_STATE_CHANGE_MSG (bus, pipeline, GST_STATE_READY, GST_STATE_PAUSED,
      208);
  pop_async_done (bus);

  /* PAUSED => PLAYING */
  GST_DEBUG ("popping PAUSED -> PLAYING messages");
  ASSERT_STATE_CHANGE_MSG (bus, sink, GST_STATE_PAUSED, GST_STATE_PLAYING, 209);
  ASSERT_STATE_CHANGE_MSG (bus, identity, GST_STATE_PAUSED, GST_STATE_PLAYING,
      210);
  ASSERT_STATE_CHANGE_MSG (bus, src, GST_STATE_PAUSED, GST_STATE_PLAYING, 211);
  ASSERT_STATE_CHANGE_MSG (bus, pipeline, GST_STATE_PAUSED, GST_STATE_PLAYING,
      212);
#endif

  /* don't set to NULL that will set the bus flushing and kill our messages */
  ret = gst_element_set_state (pipeline, GST_STATE_READY);
  fail_if (ret != GST_STATE_CHANGE_SUCCESS, "State change to READY failed");

  /* TODO: do we need to check downwards state change order as well? */
  pop_messages (bus, 4);        /* pop playing => paused messages off the bus */
  pop_messages (bus, 4);        /* pop paused => ready messages off the bus */

  GST_DEBUG ("waiting for pipeline to reach refcount 1");
  while (GST_OBJECT_REFCOUNT_VALUE (pipeline) > 1)
    THREAD_SWITCH ();

  GST_DEBUG ("checking refcount");
  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_if (ret != GST_STATE_CHANGE_SUCCESS, "State change to NULL failed");

  GST_DEBUG ("checking refcount");
  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  GST_DEBUG ("cleanup");
  gst_object_unref (bus);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_children_state_change_order_two_sink()
{
  GstElement *src, *tee, *identity, *sink1, *sink2, *pipeline;
  GstStateChangeReturn ret;
  GstBus *bus;
  
  xmlfile = "test_children_state_change_order_two_sink";
    std_log(LOG_FILENAME_LINE, "Test Started test_children_state_change_order_two_sink"); 

  pipeline = gst_pipeline_new (NULL);
  fail_unless (pipeline != NULL, "Could not create pipeline");

  bus = gst_element_get_bus (pipeline);
  fail_unless (bus != NULL, "Pipeline has no bus?!");

  src = gst_element_factory_make ("fakesrc", NULL);
  fail_if (src == NULL, "Could not create fakesrc");

  tee = gst_element_factory_make ("tee", NULL);
  fail_if (tee == NULL, "Could not create tee");

  identity = gst_element_factory_make ("identity", NULL);
  fail_if (identity == NULL, "Could not create identity");

  sink1 = gst_element_factory_make ("fakesink", NULL);
  fail_if (sink1 == NULL, "Could not create fakesink1");

  sink2 = gst_element_factory_make ("fakesink", NULL);
  fail_if (sink2 == NULL, "Could not create fakesink2");

  gst_bin_add_many (GST_BIN (pipeline), src, tee, identity, sink1, sink2, NULL);

  fail_unless (gst_element_link (src, tee) == TRUE);
  fail_unless (gst_element_link (tee, identity) == TRUE);
  fail_unless (gst_element_link (identity, sink1) == TRUE);
  fail_unless (gst_element_link (tee, sink2) == TRUE);

  ret = gst_element_set_state (pipeline, GST_STATE_READY);
  fail_if (ret != GST_STATE_CHANGE_SUCCESS, "State change to READY failed");

  /* NULL => READY */
  {
    GstMessage *msg;
    GstState old = 0, new = 0, pending = 0;
    GstObject *first, *second;

    msg = gst_bus_poll (bus, GST_MESSAGE_STATE_CHANGED, GST_SECOND);
    fail_if (msg == NULL, "No state change message within 1 second (#201)");

    gst_message_parse_state_changed (msg, &old, &new, &pending);
    first = gst_object_ref (msg->src);

    fail_if (first != GST_OBJECT (sink1) && first != GST_OBJECT (sink2),
        "sink1 or sink2 should have changed state next #(202)");
    gst_message_unref (msg);

    msg = gst_bus_poll (bus, GST_MESSAGE_STATE_CHANGED, GST_SECOND);
    fail_if (msg == NULL, "No state change message within 1 second (#201)");

    gst_message_parse_state_changed (msg, &old, &new, &pending);
    second = gst_object_ref (msg->src);

    fail_if (second != GST_OBJECT (sink1) && second != GST_OBJECT (sink2),
        "sink1 or sink2 should have changed state next #(202)");
    gst_message_unref (msg);

    fail_if (second == first, "got state change from same object");

    gst_object_unref (first);
    gst_object_unref (second);
  }
  ASSERT_STATE_CHANGE_MSG (bus, identity, GST_STATE_NULL, GST_STATE_READY, 203);
  ASSERT_STATE_CHANGE_MSG (bus, tee, GST_STATE_NULL, GST_STATE_READY, 204);
  ASSERT_STATE_CHANGE_MSG (bus, src, GST_STATE_NULL, GST_STATE_READY, 205);
  ASSERT_STATE_CHANGE_MSG (bus, pipeline, GST_STATE_NULL, GST_STATE_READY, 206);

  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (tee, "tee", 1);
  ASSERT_OBJECT_REFCOUNT (identity, "identity", 1);
  ASSERT_OBJECT_REFCOUNT (sink1, "sink1", 1);
  ASSERT_OBJECT_REFCOUNT (sink2, "sink2", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  ret = gst_element_set_state (pipeline, GST_STATE_NULL);
  fail_if (ret != GST_STATE_CHANGE_SUCCESS, "State change to NULL failed");

  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (tee, "tee", 1);
  ASSERT_OBJECT_REFCOUNT (identity, "identity", 1);
  ASSERT_OBJECT_REFCOUNT (sink1, "sink1", 1);
  ASSERT_OBJECT_REFCOUNT (sink2, "sink2", 1);
  ASSERT_OBJECT_REFCOUNT (pipeline, "pipeline", 1);

  gst_object_unref (bus);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_iterate_sorted()
{
  GstElement *src, *tee, *identity, *sink1, *sink2, *pipeline, *bin;
  GstIterator *it;
  gpointer elem;
  
  xmlfile = "test_iterate_sorted";
    std_log(LOG_FILENAME_LINE, "Test Started test_iterate_sorted"); 

  pipeline = gst_pipeline_new (NULL);
  fail_unless (pipeline != NULL, "Could not create pipeline");

  bin = gst_bin_new (NULL);
  fail_unless (bin != NULL, "Could not create bin");

  src = gst_element_factory_make ("fakesrc", NULL);
  fail_if (src == NULL, "Could not create fakesrc");

  tee = gst_element_factory_make ("tee", NULL);
  fail_if (tee == NULL, "Could not create tee");

  sink1 = gst_element_factory_make ("fakesink", NULL);
  fail_if (sink1 == NULL, "Could not create fakesink1");

  gst_bin_add_many (GST_BIN (bin), src, tee, sink1, NULL);

  fail_unless (gst_element_link (src, tee) == TRUE);
  fail_unless (gst_element_link (tee, sink1) == TRUE);

  identity = gst_element_factory_make ("identity", NULL);
  fail_if (identity == NULL, "Could not create identity");

  sink2 = gst_element_factory_make ("fakesink", NULL);
  fail_if (sink2 == NULL, "Could not create fakesink2");

  gst_bin_add_many (GST_BIN (pipeline), bin, identity, sink2, NULL);

  fail_unless (gst_element_link (tee, identity) == TRUE);
  fail_unless (gst_element_link (identity, sink2) == TRUE);

  it = gst_bin_iterate_sorted (GST_BIN (pipeline));
  fail_unless (gst_iterator_next (it, &elem) == GST_ITERATOR_OK);
  fail_unless (elem == sink2);
  gst_object_unref (elem);

  fail_unless (gst_iterator_next (it, &elem) == GST_ITERATOR_OK);
  fail_unless (elem == identity);
  gst_object_unref (elem);

  fail_unless (gst_iterator_next (it, &elem) == GST_ITERATOR_OK);
  fail_unless (elem == bin);
  gst_object_unref (elem);

  gst_iterator_free (it);

  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
}

void (*fn[]) (void) = {
test_message_state_changed,
test_interface,
test_message_state_changed_child,
test_message_state_changed_children,
test_watch_for_state_change,
test_add_linked,
test_add_self,
test_iterate_sorted,
test_children_state_change_order_flagged_sink,
test_children_state_change_order_semi_sink,
test_children_state_change_order_two_sink
};

char *args[] = {
"test_message_state_changed",
"test_interface",
"test_message_state_changed_child",
"test_message_state_changed_children",
"test_watch_for_state_change",
"test_add_linked",
"test_add_self",
"test_iterate_sorted",
"test_children_state_change_order_flagged_sink",
"test_children_state_change_order_semi_sink",
"test_children_state_change_order_two_sink"
};

GST_CHECK_MAIN (gst_bin);

#if 0
int main()
{
    gst_check_init (NULL, NULL);
    test_message_state_changed();
    test_interface();
    test_message_state_changed_child();
    test_message_state_changed_children();
    test_watch_for_state_change();
    test_add_linked();
    test_add_self();
    test_iterate_sorted();
    test_children_state_change_order_flagged_sink();
    test_children_state_change_order_semi_sink();
    test_children_state_change_order_two_sink();
    
}
#endif
