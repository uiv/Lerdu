/*
 * collectpads.c - GstCollectPads testsuite
 * Copyright (C) 2006 Alessandro Decina <alessandro@nnva.org>
 *
 * Authors:
 *   Alessandro Decina <alessandro@nnva.org>
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


#define LOG_FILE "c:\\logs\\collectpads_logs.txt" 

#include <gst/gst_global.h>
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include "libgstreamer_wsd_solution.h" 

#include <gst/check/gstcheck.h>
#include <gst/base/gstcollectpads.h>

#include <glib_global.h>
#include <gstpad.h>


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

//char* xmlfile = "collectpads";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

#define fail_unless_collected(expected)           \
G_STMT_START {                                    \
  g_mutex_lock (lock);                            \
  while (expected == TRUE && collected == FALSE)  \
    g_cond_wait (cond, lock);                     \
  fail_unless_equals_int (collected, expected);   \
  g_mutex_unlock (lock);                          \
} G_STMT_END;

typedef struct
{
  char foo;
} BadCollectData;

typedef struct
{
  GstCollectData data;
  GstPad *pad;
  GstBuffer *buffer;
  GstEvent *event;
} TestData;

static GstStaticPadTemplate srctemplate = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static GstCollectPads *collect;
static gboolean collected;
static GstPad *srcpad1, *srcpad2;
static GstPad *sinkpad1, *sinkpad2;
static TestData *data1, *data2;

static GMutex *lock;
static GCond *cond;

static GstFlowReturn
collected_cb (GstCollectPads * pads, gpointer user_data)
{
  g_mutex_lock (lock);
  collected = TRUE;
  g_cond_signal (cond);
  g_mutex_unlock (lock);

  return GST_FLOW_OK;
}

static gpointer
push_buffer (gpointer user_data)
{
  TestData *test_data = (TestData *) user_data;

  fail_unless (gst_pad_push (test_data->pad, test_data->buffer)
      == GST_FLOW_OK);
  
  return NULL;
}

static gpointer
push_event (gpointer user_data)
{
  TestData *test_data = (TestData *) user_data;

  fail_unless (gst_pad_push_event (test_data->pad, test_data->event) == TRUE);

  return NULL;
}

static void
setup (void)
{
  collect = gst_collect_pads_new ();
  gst_collect_pads_set_function (collect, collected_cb, NULL);

  srcpad1 = gst_pad_new_from_static_template (&srctemplate, "src1");
  srcpad2 = gst_pad_new_from_static_template (&srctemplate, "src2");
  sinkpad1 = gst_pad_new_from_static_template (&sinktemplate, "sink1");
  sinkpad2 = gst_pad_new_from_static_template (&sinktemplate, "sink2");
//#ifdef __SYMBIAN32__
  GST_PAD_UNSET_FLUSHING(sinkpad1);
  GST_PAD_UNSET_FLUSHING(sinkpad2);
//#endif  
  fail_unless (gst_pad_link (srcpad1, sinkpad1) == GST_PAD_LINK_OK);
  fail_unless (gst_pad_link (srcpad2, sinkpad2) == GST_PAD_LINK_OK);

  cond = g_cond_new ();
  lock = g_mutex_new ();
  data1 = NULL;
  data2 = NULL;
  collected = FALSE;
}

static void
teardown (void)
{
  gst_object_unref (sinkpad1);
  gst_object_unref (sinkpad2);
  gst_object_unref (collect);
  g_cond_free (cond);
  g_mutex_free (lock);
}

void test_pad_add_remove()
{
	xmlfile = "test_pad_add_remove";
  std_log(LOG_FILENAME_LINE, "Test Started test_pad_add_remove");
  
  setup();
  
  ASSERT_CRITICAL (gst_collect_pads_add_pad (collect, sinkpad1,
          sizeof (BadCollectData)));

  data1 = (TestData *) gst_collect_pads_add_pad (collect,
      sinkpad1, sizeof (TestData));
  fail_unless (data1 != NULL);

  fail_unless (gst_collect_pads_remove_pad (collect, sinkpad2) == FALSE);
  fail_unless (gst_collect_pads_remove_pad (collect, sinkpad1) == TRUE);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);  
}



void test_collect()
{
  GstBuffer *buf1, *buf2, *tmp;
  GThread *thread1, *thread2;

	xmlfile = "test_collect";
  std_log(LOG_FILENAME_LINE, "Test Started test_collect");
  
  setup();
  
  data1 = (TestData *) gst_collect_pads_add_pad (collect,
      sinkpad1, sizeof (TestData));
  fail_unless (data1 != NULL);

  data2 = (TestData *) gst_collect_pads_add_pad (collect,
      sinkpad2, sizeof (TestData));
  fail_unless (data2 != NULL);

  buf1 = gst_buffer_new ();
  buf2 = gst_buffer_new ();

  /* start collect pads */
  gst_collect_pads_start (collect);

  /* push buffers on the pads */
  data1->pad = srcpad1;
  data1->buffer = buf1;
  thread1 = g_thread_create (push_buffer, data1, TRUE, NULL);
  /* here thread1 is blocked and srcpad1 has a queued buffer */
  fail_unless_collected (FALSE);
  
  data2->pad = srcpad2;
  data2->buffer = buf2;
  thread2 = g_thread_create (push_buffer, data2, TRUE, NULL);

  fail_unless_collected (FALSE);
  /* now both pads have a buffer */
  /*
  #ifndef __SYMBIAN32__
  fail_unless_collected (TRUE);
  #else
  fail_unless_collected(FALSE);
  #endif
  */
  
  fail_unless_collected (TRUE);
  tmp = gst_collect_pads_pop (collect, (GstCollectData *) data1);
  fail_unless (tmp == buf1);
  tmp = gst_collect_pads_pop (collect, (GstCollectData *) data2);
  fail_unless (tmp == buf2);

  /* these will return immediately as at this point the threads have been
   * unlocked and are finished */
  g_thread_join (thread1);
  g_thread_join (thread2);

  gst_collect_pads_stop (collect);

  gst_buffer_unref (buf1);
  gst_buffer_unref (buf2);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);  
}



void test_collect_eos()
{
  GstBuffer *buf1, *tmp;
  GThread *thread1, *thread2;
  setup();
  
  xmlfile = "test_collect_eos";
  std_log(LOG_FILENAME_LINE, "Test Started test_collect_eos");
  
  data1 = (TestData *) gst_collect_pads_add_pad (collect,
      sinkpad1, sizeof (TestData));
  fail_unless (data1 != NULL);

  data2 = (TestData *) gst_collect_pads_add_pad (collect,
      sinkpad2, sizeof (TestData));
  fail_unless (data2 != NULL);

  buf1 = gst_buffer_new ();

  /* start collect pads */
  gst_collect_pads_start (collect);

  /* push a buffer on srcpad1 and EOS on srcpad2 */
  data1->pad = srcpad1;
  data1->buffer = buf1;
  thread1 = g_thread_create (push_buffer, data1, TRUE, NULL);
  /* here thread1 is blocked and srcpad1 has a queued buffer */
  fail_unless_collected (FALSE);
  
  data2->pad = srcpad2;
  data2->event = gst_event_new_eos ();
  thread2 = g_thread_create (push_event, data2, TRUE, NULL);
  /* now sinkpad1 has a buffer and sinkpad2 has EOS */
  fail_unless_collected (TRUE);
  
  tmp = gst_collect_pads_pop (collect, (GstCollectData *) data1);
  fail_unless (tmp == buf1);
  /* sinkpad2 has EOS so a NULL buffer is returned */
  tmp = gst_collect_pads_pop (collect, (GstCollectData *) data2);
  fail_unless (tmp == NULL);

  /* these will return immediately as when the data is popped the threads are
   * unlocked and will terminate */
  g_thread_join (thread1);
  g_thread_join (thread2);

  gst_collect_pads_stop (collect);

  gst_buffer_unref (buf1);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);  
  
}



void test_collect_twice()
{
  GstBuffer *buf1, *buf2, *tmp;
  GThread *thread1, *thread2;

	xmlfile = "test_collect_twice";
  std_log(LOG_FILENAME_LINE, "Test Started test_collect_twice");
  
	setup();
	
  data1 = (TestData *) gst_collect_pads_add_pad (collect,
      sinkpad1, sizeof (TestData));
  fail_unless (data1 != NULL);

  data2 = (TestData *) gst_collect_pads_add_pad (collect,
      sinkpad2, sizeof (TestData));
  fail_unless (data2 != NULL);

  buf1 = gst_buffer_new ();

  /* start collect pads */
  gst_collect_pads_start (collect);

  /* queue a buffer */
  data1->pad = srcpad1;
  data1->buffer = buf1;
  thread1 = g_thread_create (push_buffer, data1, TRUE, NULL);
  /* here thread1 is blocked and srcpad1 has a queued buffer */
  fail_unless_collected (FALSE);

  /* push EOS on the other pad */
  data2->pad = srcpad2;
  data2->event = gst_event_new_eos ();
  thread2 = g_thread_create (push_event, data2, TRUE, NULL);

  /* one of the pads has a buffer, the other has EOS */
  fail_unless_collected (TRUE);

  tmp = gst_collect_pads_pop (collect, (GstCollectData *) data1);
  fail_unless (tmp == buf1);
  /* there's nothing to pop from the one which received EOS */
  tmp = gst_collect_pads_pop (collect, (GstCollectData *) data2);
  fail_unless (tmp == NULL);

  /* these will return immediately as at this point the threads have been
   * unlocked and are finished */
  g_thread_join (thread1);
  g_thread_join (thread2);

  gst_collect_pads_stop (collect);
  collected = FALSE;

  buf2 = gst_buffer_new ();

  /* start collect pads */
  gst_collect_pads_start (collect);

  /* push buffers on the pads */
  data1->pad = srcpad1;
  data1->buffer = buf1;
  thread1 = g_thread_create (push_buffer, data1, TRUE, NULL);
  /* here thread1 is blocked and srcpad1 has a queued buffer */
  fail_unless_collected (FALSE);

  data2->pad = srcpad2;
  data2->buffer = buf2;
  thread2 = g_thread_create (push_buffer, data2, TRUE, NULL);

  /* now both pads have a buffer */
  fail_unless_collected (TRUE);

  tmp = gst_collect_pads_pop (collect, (GstCollectData *) data1);
  fail_unless (tmp == buf1);
  tmp = gst_collect_pads_pop (collect, (GstCollectData *) data2);
  fail_unless (tmp == buf2);

  /* these will return immediately as at this point the threads have been
   * unlocked and are finished */
  g_thread_join (thread1);
  g_thread_join (thread2);

  gst_collect_pads_stop (collect);

  gst_buffer_unref (buf1);
  gst_buffer_unref (buf2);

  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);  
}



void
gst_collect_pads_suite (void)
{
test_pad_add_remove();
test_collect();
test_collect_eos();
test_collect_twice();
}

void (*fn[]) (void) = {
test_pad_add_remove,
test_collect,
test_collect_eos,
test_collect_twice
};

char *args[] = {
"test_pad_add_remove",
"test_collect",
"test_collect_eos",
"test_collect_twice"
};

GST_CHECK_MAIN (gst_collect_pads);
#if 0 
int main(int argc,char** argv)
{
		int i;
		gst_check_init(NULL,NULL);
		for(i=0;i<4;i++)
			fn[i]();
			
} 
#endif