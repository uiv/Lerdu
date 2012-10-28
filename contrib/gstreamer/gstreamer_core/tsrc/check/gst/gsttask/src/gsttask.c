/* GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas at apestaart dot org>
 *
 * gsttask.c: Unit test for GstTask
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
#include<gst/check/gstcheck.h>

#include <glib_global.h>
#include <libgstreamer_wsd_solution.h>
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

#ifndef EMULATOR
int failed=0;
FILE* fp_std_log_t=NULL;
#define assert_failed *(get_assert_failed())
#define fp_std_log *(get_fp_std_log())
#endif

#define LOG_FILE "c:\\logs\\gsttask_log.txt" 
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
static GMutex *task_lock ;
static GCond  *task_cond ;

static GStaticRecMutex task_mutex = G_STATIC_REC_MUTEX_INIT;

static void
task_func2 (void *data)
{
  gboolean ret;
  GstTask *t = *((GstTask **) data);
  

  g_mutex_lock (task_lock);
  GST_DEBUG ("signal");
  g_cond_signal (task_cond);
  g_mutex_unlock (task_lock);

  ASSERT_WARNING (ret = gst_task_join (t));
  fail_unless (ret == FALSE);
  TEST_ASSERT_FAIL

}

void test_join()
{
  GstTask *t;
  gboolean ret;
    //xmlfile = "test_join";
 std_log(LOG_FILENAME_LINE, "Test Started test_join");

  t = gst_task_create (task_func2, &t);
  fail_if (t == NULL);
  TEST_ASSERT_FAIL
 

  gst_task_set_lock (t, &task_mutex);

  task_cond = g_cond_new ();
  task_lock = g_mutex_new ();

  g_mutex_lock (task_lock);
  GST_DEBUG ("starting");
  ret = gst_task_start (t);
  fail_unless (ret == TRUE);
  TEST_ASSERT_FAIL
  /* wait for it to spin up */
  GST_DEBUG ("waiting");
  g_cond_wait (task_cond, task_lock);
  GST_DEBUG ("done waiting");
  g_mutex_unlock (task_lock);

  GST_DEBUG ("joining");
  ret = gst_task_join (t);
  fail_unless (ret == TRUE);
  TEST_ASSERT_FAIL

  gst_object_unref (t);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


static void
task_func (void *data)
{
   
  g_mutex_lock (task_lock);
  GST_DEBUG ("signal");
  g_cond_signal (task_cond);
  g_mutex_unlock (task_lock);
   
 
}


void test_lock_start()
{
  GstTask *t;
  gboolean ret;
    //xmlfile = "test_lock_start";
 std_log(LOG_FILENAME_LINE, "Test Started test_lock_start");

  t = gst_task_create (task_func, NULL);
   fail_if (t == NULL);
  TEST_ASSERT_FAIL
   gst_task_set_lock (t, &task_mutex);
  task_cond = g_cond_new ();
  task_lock = g_mutex_new ();
  g_mutex_lock (task_lock);
   GST_DEBUG ("starting");
  ret = gst_task_start (t);
  fail_unless (ret == TRUE);
  TEST_ASSERT_FAIL
  /* wait for it to spin up */
  GST_DEBUG ("waiting");
  g_cond_wait (task_cond, task_lock);
  GST_DEBUG ("done waiting");
  g_mutex_unlock (task_lock);
  /* cannot set mutex now */
  ASSERT_WARNING (gst_task_set_lock (t, &task_mutex));//b failing
  GST_DEBUG ("joining");
  ret = gst_task_join (t);
    fail_unless (ret == TRUE);
  TEST_ASSERT_FAIL
  gst_object_unref (t);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_lock()
{
  GstTask *t;
  gboolean ret;
    //xmlfile = "test_lock";
 std_log(LOG_FILENAME_LINE, "Test Started test_lock");

  t = gst_task_create (task_func, NULL);
  fail_if (t == NULL);
  TEST_ASSERT_FAIL

  gst_task_set_lock (t, &task_mutex);

  GST_DEBUG ("pause");
  ret = gst_task_pause (t);
  fail_unless (ret == TRUE);
  TEST_ASSERT_FAIL

  g_usleep (1 * G_USEC_PER_SEC / 2);

  GST_DEBUG ("joining");
  ret = gst_task_join (t);
  fail_unless (ret == TRUE);
  TEST_ASSERT_FAIL

  g_usleep (1 * G_USEC_PER_SEC / 2);

  gst_object_unref (t);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_no_lock()
{
  GstTask *t;
  gboolean ret;
    //xmlfile = "test_no_lock";
 std_log(LOG_FILENAME_LINE, "Test Started test_no_lock");

  t = gst_task_create (task_func, NULL);
  fail_if (t == NULL);
  TEST_ASSERT_FAIL

  /* stop should be possible without lock */
  gst_task_stop (t);
  /* pause should give a warning */
  ASSERT_WARNING (ret = gst_task_pause (t));   //b failing
   fail_unless (ret == FALSE);
    TEST_ASSERT_FAIL
    /* start should give a warning */
  ASSERT_WARNING (ret = gst_task_start (t));
  fail_unless (ret == FALSE);
   TEST_ASSERT_FAIL
     /* stop should be possible without lock */
  gst_task_stop (t);
  gst_object_unref (t);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}

void test_create()
{
  GstTask *t;
  //xmlfile = "test_create";
 std_log(LOG_FILENAME_LINE, "Test Started test_create");
 
  t = gst_task_create (task_func, NULL);
  fail_if (t == NULL);
  TEST_ASSERT_FAIL
  
   
  gst_object_unref (t);
    
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}     
  
void (*fn[]) (void) = {
test_create,
test_no_lock,
test_lock,
test_lock_start,
test_join
};

char *args[] = {
"test_create",
"test_no_lock",
"test_lock",
"test_lock_start",
"test_join"
};

GST_CHECK_MAIN (gst_task);


