/* GStreamer
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
 *
 * gstpad.c: Unit test for GstPad
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



#define LOG_FILE "c:\\logs\\gstpad_logs.txt" 
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

//gboolean _gst_check_expecting_log = FALSE;
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
static GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
extern GCond *start_cond;
#endif

//GCond *sync_cond;               /* used to synchronize all threads and main thread */
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
extern GCond *sync_cond;
#endif

void test_link()
{
  GstPad *src, *sink;
  GstPadTemplate *srct;

  GstPadLinkReturn ret;
  gchar *name;

  xmlfile = "test_link";
  std_log(LOG_FILENAME_LINE, "Test Started test_link");

  src = gst_pad_new ("source", GST_PAD_SRC);
  fail_if (src == NULL);
  ASSERT_OBJECT_REFCOUNT (src, "source pad", 1);

  name = gst_pad_get_name (src);
  fail_unless (strcmp (name, "source") == 0);
  ASSERT_OBJECT_REFCOUNT (src, "source pad", 1);
  g_free (name);

  sink = gst_pad_new ("sink", GST_PAD_SINK);
  fail_if (sink == NULL);

  /* linking without templates or caps should fail */
  ret = gst_pad_link (src, sink);
  ASSERT_OBJECT_REFCOUNT (src, "source pad", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink pad", 1);
  fail_unless (ret == GST_PAD_LINK_NOFORMAT);

  /* Fix Me- Issue here
  ASSERT_CRITICAL (gst_pad_get_pad_template (NULL));
  */

  srct = gst_pad_get_pad_template (src);
  fail_unless (srct == NULL);
  ASSERT_OBJECT_REFCOUNT (src, "source pad", 1);

  /* clean up */
  ASSERT_OBJECT_REFCOUNT (src, "source pad", 1);
  gst_object_unref (src);
  gst_object_unref (sink);

  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



/* threaded link/unlink */
/* use globals */
static GstPad *src, *sink;

static void
thread_link_unlink (gpointer data)
{
  THREAD_START ();

  while (THREAD_TEST_RUNNING ()) {
    gst_pad_link (src, sink);
    gst_pad_unlink (src, sink);
    THREAD_SWITCH ();
  }
}

void test_link_unlink_threaded()
{
  GstCaps *caps;
  int i;

  xmlfile = "test_link_unlink_threaded";
  std_log(LOG_FILENAME_LINE, "Test Started test_link_unlink_threaded");

  src = gst_pad_new ("source", GST_PAD_SRC);
  fail_if (src == NULL);
  sink = gst_pad_new ("sink", GST_PAD_SINK);
  fail_if (sink == NULL);

  caps = gst_caps_from_string ("foo/bar");
  gst_pad_set_caps (src, caps);
  gst_pad_set_caps (sink, caps);
  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);

  MAIN_START_THREADS (5, thread_link_unlink, NULL);
  for (i = 0; i < 1000; ++i) {
    gst_pad_is_linked (src);
    gst_pad_is_linked (sink);
    THREAD_SWITCH ();
  }
  MAIN_STOP_THREADS ();

  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);
  gst_caps_unref (caps);

  ASSERT_CAPS_REFCOUNT (caps, "caps", 2);
  gst_object_unref (src);
  gst_object_unref (sink);

  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_refcount()
{
  GstPad *src, *sink;
  GstCaps *caps;
  GstPadLinkReturn plr;

  xmlfile = "test_refcount";
  std_log(LOG_FILENAME_LINE, "Test Started test_refcount");

  sink = gst_pad_new ("sink", GST_PAD_SINK);
  fail_if (sink == NULL);

  src = gst_pad_new ("src", GST_PAD_SRC);
  fail_if (src == NULL);

  caps = gst_caps_from_string ("foo/bar");
  /* one for me */
  ASSERT_CAPS_REFCOUNT (caps, "caps", 1);

  gst_pad_set_caps (src, caps);
  gst_pad_set_caps (sink, caps);
  /* one for me and one for each set_caps */
  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);

  plr = gst_pad_link (src, sink);
  fail_unless (GST_PAD_LINK_SUCCESSFUL (plr));
  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);

  gst_pad_unlink (src, sink);
  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);

  /* cleanup */
  gst_object_unref (src);
  gst_object_unref (sink);
  ASSERT_CAPS_REFCOUNT (caps, "caps", 1);

  gst_caps_unref (caps);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_get_allowed_caps()
{
  GstPad *src, *sink;
  GstCaps *caps, *gotcaps;
  GstBuffer *buffer;
  GstPadLinkReturn plr;

  xmlfile = "test_get_allowed_caps";
  std_log(LOG_FILENAME_LINE, "Test Started test_get_allowed_caps");

  ASSERT_CRITICAL (gst_pad_get_allowed_caps (NULL));


  buffer = gst_buffer_new ();
  ASSERT_CRITICAL (gst_pad_get_allowed_caps ((GstPad *) buffer));
  gst_buffer_unref (buffer);

  src = gst_pad_new ("src", GST_PAD_SRC);
  fail_if (src == NULL);
  caps = gst_pad_get_allowed_caps (src);
  fail_unless (caps == NULL);

  caps = gst_caps_from_string ("foo/bar");

  sink = gst_pad_new ("sink", GST_PAD_SINK);
  gst_pad_set_caps (src, caps);
  gst_pad_set_caps (sink, caps);
  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);

  plr = gst_pad_link (src, sink);
  fail_unless (GST_PAD_LINK_SUCCESSFUL (plr));

  gotcaps = gst_pad_get_allowed_caps (src);
  fail_if (gotcaps == NULL);
  fail_unless (gst_caps_is_equal (gotcaps, caps));

  ASSERT_CAPS_REFCOUNT (gotcaps, "gotcaps", 1);
  gst_caps_unref (gotcaps);

  gst_pad_unlink (src, sink);

  /* cleanup */
  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);
  ASSERT_OBJECT_REFCOUNT (src, "src", 1);
  ASSERT_OBJECT_REFCOUNT (sink, "sink", 1);

  gst_object_unref (src);
  gst_object_unref (sink);

  ASSERT_CAPS_REFCOUNT (caps, "caps", 1);
  gst_caps_unref (caps);

  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



static gboolean
name_is_valid (const gchar * name, GstPadPresence presence)
{
  GstPadTemplate *new;

  new = gst_pad_template_new (name, GST_PAD_SRC, presence, GST_CAPS_ANY);
  if (new) {
    gst_object_unref (GST_OBJECT (new));
    return TRUE;
  }
  return FALSE;
}

void test_name_is_valid()
{
  gboolean result = FALSE;

  xmlfile = "test_name_is_valid";
  std_log(LOG_FILENAME_LINE, "Test Started test_name_is_valid");
  fail_unless (name_is_valid ("src", GST_PAD_ALWAYS));
  

   
  ASSERT_WARNING (name_is_valid ("src%", GST_PAD_ALWAYS));
  ASSERT_WARNING (result = name_is_valid ("src%d", GST_PAD_ALWAYS));
  fail_if (result);

  fail_unless (name_is_valid ("src", GST_PAD_REQUEST));
  ASSERT_WARNING (name_is_valid ("src%s%s", GST_PAD_REQUEST));
  ASSERT_WARNING (name_is_valid ("src%c", GST_PAD_REQUEST));
  ASSERT_WARNING (name_is_valid ("src%", GST_PAD_REQUEST));
  ASSERT_WARNING (name_is_valid ("src%dsrc", GST_PAD_REQUEST));

  fail_unless (name_is_valid ("src", GST_PAD_SOMETIMES));
  fail_unless (name_is_valid ("src%c", GST_PAD_SOMETIMES));
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



static gboolean
_probe_handler (GstPad * pad, GstBuffer * buffer, gpointer userdata)
{
  gint ret = GPOINTER_TO_INT (userdata);

  if (ret == 1)
    return TRUE;
  return FALSE;
}

void test_push_unlinked()
{
  GstPad *src;
  GstCaps *caps;
  GstBuffer *buffer;
  gulong id;

  xmlfile = "test_push_unlinked";
  std_log(LOG_FILENAME_LINE, "Test Started test_push_unlinked");
  src = gst_pad_new ("src", GST_PAD_SRC);
  fail_if (src == NULL);
  caps = gst_pad_get_allowed_caps (src);
  fail_unless (caps == NULL);

  caps = gst_caps_from_string ("foo/bar");

  gst_pad_set_caps (src, caps);
  ASSERT_CAPS_REFCOUNT (caps, "caps", 2);

  /* pushing on an unlinked pad will drop the buffer */
  buffer = gst_buffer_new ();
  gst_buffer_ref (buffer);
  fail_unless (gst_pad_push (src, buffer) == GST_FLOW_NOT_LINKED);
  ASSERT_MINI_OBJECT_REFCOUNT (buffer, "buffer", 1);
  gst_buffer_unref (buffer);

  /* adding a probe that returns FALSE will drop the buffer without trying
   * to chain */
  id = gst_pad_add_buffer_probe (src, (GCallback) _probe_handler,
      GINT_TO_POINTER (0));
  buffer = gst_buffer_new ();
  gst_buffer_ref (buffer);
  fail_unless (gst_pad_push (src, buffer) == GST_FLOW_OK);
  ASSERT_MINI_OBJECT_REFCOUNT (buffer, "buffer", 1);
  gst_buffer_unref (buffer);
  gst_pad_remove_buffer_probe (src, id);

  /* adding a probe that returns TRUE will still chain the buffer,
   * and hence drop because pad is unlinked */
  id = gst_pad_add_buffer_probe (src, (GCallback) _probe_handler,
      GINT_TO_POINTER (1));
  buffer = gst_buffer_new ();
  gst_buffer_ref (buffer);
  fail_unless (gst_pad_push (src, buffer) == GST_FLOW_NOT_LINKED);
  ASSERT_MINI_OBJECT_REFCOUNT (buffer, "buffer", 1);
  gst_buffer_unref (buffer);
  gst_pad_remove_buffer_probe (src, id);


  /* cleanup */
  ASSERT_CAPS_REFCOUNT (caps, "caps", 2);
  ASSERT_OBJECT_REFCOUNT (src, "src", 1);

  gst_object_unref (src);

  ASSERT_CAPS_REFCOUNT (caps, "caps", 1);
  gst_caps_unref (caps);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_push_linked()
{
  GstPad *src, *sink;
  GstPadLinkReturn plr;
  GstCaps *caps;
  GstBuffer *buffer;
  gulong id;

  xmlfile = "test_push_linked";
  std_log(LOG_FILENAME_LINE, "Test Started test_push_linked");
  /* setup */
  sink = gst_pad_new ("sink", GST_PAD_SINK);
  fail_if (sink == NULL);
  gst_pad_set_chain_function (sink, gst_check_chain_func);

  src = gst_pad_new ("src", GST_PAD_SRC);
  fail_if (src == NULL);

  caps = gst_caps_from_string ("foo/bar");
  /* one for me */
  ASSERT_CAPS_REFCOUNT (caps, "caps", 1);

  gst_pad_set_caps (src, caps);
  gst_pad_set_caps (sink, caps);
  /* one for me and one for each set_caps */
  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);

  plr = gst_pad_link (src, sink);
  fail_unless (GST_PAD_LINK_SUCCESSFUL (plr));
  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);

  buffer = gst_buffer_new ();
#if 0
  /* FIXME, new pad should be flushing */
  gst_buffer_ref (buffer);
  fail_unless (gst_pad_push (src, buffer) == GST_FLOW_WRONG_STATE);
  gst_buffer_ref (buffer);
  fail_unless (gst_pad_chain (sink, buffer) == GST_FLOW_WRONG_STATE);
#endif

  /* activate pads */
  gst_pad_set_active (src, TRUE);
  gst_pad_set_active (sink, TRUE);

  /* test */
  /* pushing on a linked pad will drop the ref to the buffer */
  gst_buffer_ref (buffer);
  fail_unless (gst_pad_push (src, buffer) == GST_FLOW_OK);
  ASSERT_MINI_OBJECT_REFCOUNT (buffer, "buffer", 2);
  gst_buffer_unref (buffer);
  fail_unless_equals_int (g_list_length (buffers), 1);
  buffer = GST_BUFFER (buffers->data);
  ASSERT_MINI_OBJECT_REFCOUNT (buffer, "buffer", 1);
  gst_buffer_unref (buffer);
  g_list_free (buffers);
  buffers = NULL;

  /* adding a probe that returns FALSE will drop the buffer without trying
   * to chain */
  id = gst_pad_add_buffer_probe (src, (GCallback) _probe_handler,
      GINT_TO_POINTER (0));
  buffer = gst_buffer_new ();
  gst_buffer_ref (buffer);
  fail_unless (gst_pad_push (src, buffer) == GST_FLOW_OK);
  ASSERT_MINI_OBJECT_REFCOUNT (buffer, "buffer", 1);
  gst_buffer_unref (buffer);
  gst_pad_remove_buffer_probe (src, id);
  fail_unless_equals_int (g_list_length (buffers), 0);

  /* adding a probe that returns TRUE will still chain the buffer */
  id = gst_pad_add_buffer_probe (src, (GCallback) _probe_handler,
      GINT_TO_POINTER (1));
  buffer = gst_buffer_new ();
  gst_buffer_ref (buffer);
  fail_unless (gst_pad_push (src, buffer) == GST_FLOW_OK);
  gst_pad_remove_buffer_probe (src, id);

  ASSERT_MINI_OBJECT_REFCOUNT (buffer, "buffer", 2);
  gst_buffer_unref (buffer);
  fail_unless_equals_int (g_list_length (buffers), 1);
  buffer = GST_BUFFER (buffers->data);
  ASSERT_MINI_OBJECT_REFCOUNT (buffer, "buffer", 1);
  gst_buffer_unref (buffer);
  g_list_free (buffers);
  buffers = NULL;

  /* teardown */
  gst_pad_unlink (src, sink);
  ASSERT_CAPS_REFCOUNT (caps, "caps", 3);
  gst_object_unref (src);
  gst_object_unref (sink);
  ASSERT_CAPS_REFCOUNT (caps, "caps", 1);

  gst_caps_unref (caps);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_flowreturn()
{
  GstFlowReturn ret;
  GQuark quark;

  xmlfile = "test_flowreturn";
  std_log(LOG_FILENAME_LINE, "Test Started test_flowreturn");
  /* test some of the macros */
  ret = GST_FLOW_UNEXPECTED;
  fail_unless (GST_FLOW_IS_FATAL (ret));
  fail_if (GST_FLOW_IS_SUCCESS (ret));
  fail_if (strcmp (gst_flow_get_name (ret), "unexpected"));
  quark = gst_flow_to_quark (ret);
  fail_if (strcmp (g_quark_to_string (quark), "unexpected"));

  ret = GST_FLOW_RESEND;
  fail_if (GST_FLOW_IS_FATAL (ret));
  fail_unless (GST_FLOW_IS_SUCCESS (ret));
  fail_if (strcmp (gst_flow_get_name (ret), "resend"));
  quark = gst_flow_to_quark (ret);
  fail_if (strcmp (g_quark_to_string (quark), "resend"));

  /* custom returns */
  ret = GST_FLOW_CUSTOM_SUCCESS;
  fail_if (GST_FLOW_IS_FATAL (ret));
  fail_unless (GST_FLOW_IS_SUCCESS (ret));
  fail_if (strcmp (gst_flow_get_name (ret), "custom-success"));
  quark = gst_flow_to_quark (ret);
  fail_if (strcmp (g_quark_to_string (quark), "custom-success"));

  ret = GST_FLOW_CUSTOM_ERROR;
  fail_unless (GST_FLOW_IS_FATAL (ret));
  fail_if (GST_FLOW_IS_SUCCESS (ret));
  fail_if (strcmp (gst_flow_get_name (ret), "custom-error"));
  quark = gst_flow_to_quark (ret);
  fail_if (strcmp (g_quark_to_string (quark), "custom-error"));

  /* custom returns clamping */
  ret = GST_FLOW_CUSTOM_SUCCESS + 2;
  fail_if (GST_FLOW_IS_FATAL (ret));
  fail_unless (GST_FLOW_IS_SUCCESS (ret));
  fail_if (strcmp (gst_flow_get_name (ret), "custom-success"));
  quark = gst_flow_to_quark (ret);
  fail_if (strcmp (g_quark_to_string (quark), "custom-success"));

  ret = GST_FLOW_CUSTOM_ERROR - 2;
  fail_unless (GST_FLOW_IS_FATAL (ret));
  fail_if (GST_FLOW_IS_SUCCESS (ret));
  fail_if (strcmp (gst_flow_get_name (ret), "custom-error"));
  quark = gst_flow_to_quark (ret);
  fail_if (strcmp (g_quark_to_string (quark), "custom-error"));

  /* unknown values */
  ret = GST_FLOW_CUSTOM_ERROR + 2;
  fail_unless (GST_FLOW_IS_FATAL (ret));
  fail_if (GST_FLOW_IS_SUCCESS (ret));
  fail_if (strcmp (gst_flow_get_name (ret), "unknown"));
  quark = gst_flow_to_quark (ret);
  fail_unless (quark == 0);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



void test_push_negotiation()
{
  GstPad *src, *sink;
  GstPadLinkReturn plr;
  GstPadTemplate *src_template = gst_pad_template_new ("src", GST_PAD_SRC,
      GST_PAD_ALWAYS,
      gst_caps_from_string ("audio/x-raw-int,width={16,32},depth={16,32}"));
  GstPadTemplate *sink_template = gst_pad_template_new ("sink", GST_PAD_SINK,
      GST_PAD_ALWAYS,
      gst_caps_from_string ("audio/x-raw-int,width=32,depth={16,32}"));
  GstCaps *caps;
  GstBuffer *buffer;
  xmlfile = "test_push_negotiation";
  std_log(LOG_FILENAME_LINE, "Test Started test_push_negotiation");

  /* setup */
  sink = gst_pad_new_from_template (sink_template, "sink");
  fail_if (sink == NULL);
  gst_pad_set_chain_function (sink, gst_check_chain_func);

  src = gst_pad_new_from_template (src_template, "src");
  fail_if (src == NULL);

  plr = gst_pad_link (src, sink);
  fail_unless (GST_PAD_LINK_SUCCESSFUL (plr));

  buffer = gst_buffer_new ();

  /* activate pads */
  gst_pad_set_active (src, TRUE);
  gst_pad_set_active (sink, TRUE);

  caps = gst_caps_from_string ("audio/x-raw-int,width=16,depth=16");

  /* Should fail if src pad caps are incompatible with sink pad caps */
  gst_pad_set_caps (src, caps);
  gst_buffer_ref (buffer);
  gst_buffer_set_caps (buffer, caps);
  fail_unless (gst_pad_push (src, buffer) == GST_FLOW_NOT_NEGOTIATED);
  ASSERT_MINI_OBJECT_REFCOUNT (buffer, "buffer", 1);
  gst_buffer_unref (buffer);

  /* teardown */
  gst_pad_unlink (src, sink);
  gst_object_unref (src);
  gst_object_unref (sink);
  gst_caps_unref (caps);
  gst_object_unref (sink_template);
  gst_object_unref (src_template);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* see that an unref also unlinks the pads */
void test_src_unref_unlink()
{
  GstPad *src, *sink;
  GstCaps *caps;
  GstPadLinkReturn plr;

  xmlfile = "test_src_unref_unlink";
  std_log(LOG_FILENAME_LINE, "Test Started test_src_unref_unlink");
  sink = gst_pad_new ("sink", GST_PAD_SINK);
  fail_if (sink == NULL);

  src = gst_pad_new ("src", GST_PAD_SRC);
  fail_if (src == NULL);

  caps = gst_caps_from_string ("foo/bar");

  gst_pad_set_caps (src, caps);
  gst_pad_set_caps (sink, caps);

  plr = gst_pad_link (src, sink);
  fail_unless (GST_PAD_LINK_SUCCESSFUL (plr));

  /* unref the srcpad */
  gst_object_unref (src);

  /* sink should be unlinked now */
  fail_if (gst_pad_is_linked (sink));

  /* cleanup */
  gst_object_unref (sink);
  gst_caps_unref (caps);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* see that an unref also unlinks the pads */
void test_sink_unref_unlink()
{
  GstPad *src, *sink;
  GstCaps *caps;
  GstPadLinkReturn plr;
  xmlfile = "test_sink_unref_unlink";
  std_log(LOG_FILENAME_LINE, "Test Started test_sink_unref_unlink");

  sink = gst_pad_new ("sink", GST_PAD_SINK);
  fail_if (sink == NULL);

  src = gst_pad_new ("src", GST_PAD_SRC);
  fail_if (src == NULL);

  caps = gst_caps_from_string ("foo/bar");

  gst_pad_set_caps (src, caps);
  gst_pad_set_caps (sink, caps);

  plr = gst_pad_link (src, sink);
  fail_unless (GST_PAD_LINK_SUCCESSFUL (plr));

  /* unref the sinkpad */
  gst_object_unref (sink);

  /* src should be unlinked now */
  fail_if (gst_pad_is_linked (src));

  /* cleanup */
  gst_object_unref (src);
  gst_caps_unref (caps);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* gst_pad_get_caps should return a copy of the caps */
void test_get_caps_must_be_copy()
{
  GstPad *pad;
  GstCaps *caps;
  GstPadTemplate *templ;
  xmlfile = "test_get_caps_must_be_copy";
  std_log(LOG_FILENAME_LINE, "Test Started test_get_caps_must_be_copy");

  caps = gst_caps_new_any ();
  templ =
      gst_pad_template_new ("test_templ", GST_PAD_SRC, GST_PAD_ALWAYS, caps);
  pad = gst_pad_new_from_template (templ, NULL);
  fail_unless (GST_PAD_CAPS (pad) == NULL, "caps present on pad");
  caps = gst_pad_get_caps (pad);

  /* we must own the caps */
  ASSERT_OBJECT_REFCOUNT (caps, "caps", 1);

  /* cleanup */
  gst_caps_unref (caps);
  gst_object_unref (pad);
  gst_object_unref (templ);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

#if 1
void (*fn[12]) (void) = {
    test_link,
    test_refcount,
    test_get_allowed_caps,
    test_link_unlink_threaded,
    test_name_is_valid,        
    test_push_unlinked,
    test_push_linked,  
    test_flowreturn,
    test_push_negotiation,
    test_src_unref_unlink,
    test_sink_unref_unlink,
    test_get_caps_must_be_copy
};

char *args[] = {
    "test_link",
    "test_refcount",
    "test_get_allowed_caps",
    "test_link_unlink_threaded",
    "test_name_is_valid",        
    "test_push_unlinked",
    "test_push_linked",  
    "test_flowreturn",
    "test_push_negotiation",
    "test_src_unref_unlink",
    "test_sink_unref_unlink",
    "test_get_caps_must_be_copy"
};
#endif

#if 0
int main()
{

    gst_check_init (NULL, NULL);
//	test_link();
//    test_refcount();
//    test_get_allowed_caps();   
  //  test_link_unlink_threaded();  
  //  test_name_is_valid();         
   // test_push_unlinked();
  //  test_push_linked();  
    test_flowreturn();
  //  test_push_negotiation();
  //  test_src_unref_unlink();
  //  test_sink_unref_unlink();
  //  test_get_caps_must_be_copy();
}
#endif

#if 0
void
gst_pad_suite (void)
{
}

void (*fn[]) (void) = {
};

char *args[] = {
};
#endif
GST_CHECK_MAIN (gst_pad);

