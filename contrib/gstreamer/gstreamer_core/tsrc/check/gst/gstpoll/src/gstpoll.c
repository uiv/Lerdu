/* GStreamer
 *
 * unit test for GstPoll
 *
 * Copyright (C) <2007> Peter Kjellerstedt <pkj@axis.com>
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
#include <errno.h>

#define LOG_FILE "c:\\logs\\gstpoll_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include <gst/check/gstcheck.h>
#include <glib_global.h>


#ifdef G_OS_WIN32
#include <winsock2.h>
#define EINPROGRESS WSAEINPROGRESS
#else
#include <sys/socket.h>
#endif

#include "libgstreamer_wsd_solution.h" 
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
extern gboolean _gst_check_expecting_log  ;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running  ;
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
extern GList *thread_list ;
#endif

//GMutex *mutex;
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(mutex,gstcheck,GMutex*)
#define mutex (*GET_GSTREAMER_WSD_VAR_NAME(mutex,gstcheck,g)())
#else 
extern GMutex *mutex ;
#endif

//GCond *start_cond;              /* used to notify main thread of thread startups */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
extern GCond *start_cond ;
#endif

//GCond *sync_cond;               /* used to synchronize all threads and main thread */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
extern GCond *sync_cond  ;
#endif


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}
/*
 //------------------Dropped due to Scket Pair-----------------------
void test_poll_wait()
{
  GstPoll *set;
  GstPollFD rfd = GST_POLL_FD_INIT;
  GstPollFD wfd = GST_POLL_FD_INIT;
  gint socks[2];
  guchar c = 'A';
//xmlfile = "test_poll_wait";
    std_log(LOG_FILENAME_LINE, "Test Started test_poll_wait");
  set = gst_poll_new (FALSE);
  fail_if (set == NULL, "Failed to create a GstPoll");

#ifdef G_OS_WIN32
  fail_if (_pipe (socks, 4096, _O_BINARY) < 0, "Could not create a pipe");
#else
  fail_if (socketpair (PF_UNIX, SOCK_STREAM, 0, socks) < 0,
      "Could not create a pipe");//----------------------------------
#endif
  rfd.fd = socks[0];
  wfd.fd = socks[1];

  fail_unless (gst_poll_add_fd (set, &rfd), "Could not add read descriptor");
  fail_unless (gst_poll_fd_ctl_read (set, &rfd, TRUE),
      "Could not mark the descriptor as readable");

  fail_unless (write (wfd.fd, &c, 1) == 1, "write() failed");

  fail_unless (gst_poll_wait (set, GST_CLOCK_TIME_NONE) == 1,
      "One descriptor should be available");
  fail_unless (gst_poll_fd_can_read (set, &rfd),
      "Read descriptor should be readable");
  fail_if (gst_poll_fd_can_write (set, &rfd),
      "Read descriptor should not be writeable");

  fail_unless (gst_poll_add_fd (set, &wfd), "Could not add write descriptor");
  fail_unless (gst_poll_fd_ctl_write (set, &wfd, TRUE),
      "Could not mark the descriptor as writeable");

  fail_unless (gst_poll_wait (set, GST_CLOCK_TIME_NONE) == 2,
      "Two descriptors should be available");
  fail_unless (gst_poll_fd_can_read (set, &rfd),
      "Read descriptor should be readable");
  fail_if (gst_poll_fd_can_write (set, &rfd),
      "Read descriptor should not be writeable");
  fail_if (gst_poll_fd_can_read (set, &wfd),
      "Write descriptor should not be readable");
  fail_unless (gst_poll_fd_can_write (set, &wfd),
      "Write descriptor should be writeable");

  fail_unless (read (rfd.fd, &c, 1) == 1, "read() failed");

  fail_unless (gst_poll_wait (set, GST_CLOCK_TIME_NONE) == 1,
      "One descriptor should be available");
  fail_if (gst_poll_fd_can_read (set, &rfd),
      "Read descriptor should not be readable");
  fail_if (gst_poll_fd_can_write (set, &rfd),
      "Read descriptor should not be writeable");
  fail_if (gst_poll_fd_can_read (set, &wfd),
      "Write descriptor should not be readable");
  fail_unless (gst_poll_fd_can_write (set, &wfd),
      "Write descriptor should be writeable");

  gst_poll_free (set);
  close (socks[0]);
  close (socks[1]);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}

*/

void test_poll_basic()
{
  GstPoll *set;
  GstPollFD fd = GST_POLL_FD_INIT;
//xmlfile = "test_poll_basic";
    std_log(LOG_FILENAME_LINE, "Test Started test_poll_basic");
  fd.fd = 1;

  set = gst_poll_new (FALSE);
  fail_if (set == NULL, "Failed to create a GstPoll");

  fail_unless (gst_poll_add_fd (set, &fd), "Could not add descriptor");
  fail_unless (gst_poll_fd_ctl_write (set, &fd, TRUE),
      "Could not mark the descriptor as writeable");
  fail_unless (gst_poll_fd_ctl_read (set, &fd, TRUE),
      "Could not mark the descriptor as readable");
  fail_if (gst_poll_fd_has_closed (set, &fd),
      "Descriptor should not be closed");
  fail_if (gst_poll_fd_has_error (set, &fd),
      "Descriptor should not have an error");
  fail_if (gst_poll_fd_can_write (set, &fd),
      "Descriptor should not be writeable");
  fail_if (gst_poll_fd_can_read (set, &fd),
      "Descriptor should not be readable");
  fail_unless (gst_poll_remove_fd (set, &fd), "Could not remove descriptor");

  fail_if (gst_poll_remove_fd (set, &fd),
      "Could remove already removed descriptor");

  fail_unless (gst_poll_wait (set, 50 * GST_MSECOND) == 0,
      "Waiting did not timeout");

  gst_poll_free (set);

  set = gst_poll_new (TRUE);
  fail_if (set == NULL, "Failed to create a GstPoll");
  gst_poll_set_flushing (set, TRUE);
  gst_poll_free (set);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



static gpointer
delayed_stop (gpointer data)
{
  GstPoll *set = data;
  THREAD_START ();

  g_usleep (500000);

  gst_poll_set_flushing (set, TRUE);

  return NULL;
  
}

void test_poll_wait_stop()
{
  GstPoll *set;
//xmlfile = "test_poll_wait_stop";
    std_log(LOG_FILENAME_LINE, "Test Started test_poll_wait_stop");
  set = gst_poll_new (TRUE);
  fail_if (set == NULL, "Failed to create a GstPoll");
 MAIN_START_THREADS (1, delayed_stop, set);
  fail_unless (gst_poll_wait (set, GST_SECOND) != 0, "Waiting timed out");
  MAIN_STOP_THREADS ();
  gst_poll_free (set);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



static gpointer
delayed_restart (gpointer data)
{
  GstPoll *set = data;
  GstPollFD fd = GST_POLL_FD_INIT;

  fd.fd = 1;

  THREAD_START ();

  //g_usleep (500000);                //Arun's changes

  gst_poll_add_fd (set, &fd);
  gst_poll_fd_ctl_write (set, &fd, TRUE);
  gst_poll_restart (set);

  g_usleep (1000000); 
  
  return NULL;
  
}

void test_poll_wait_restart()
{
  GstPoll *set;
  GstPollFD fd = GST_POLL_FD_INIT;
//xmlfile = "test_poll_wait_restart";
    std_log(LOG_FILENAME_LINE, "Test Started test_poll_wait_restart");
  fd.fd = 1;
  set = gst_poll_new (TRUE);
  fail_if (set == NULL, "Failed to create a GstPoll");
   MAIN_START_THREADS (1, delayed_restart, set);

 g_usleep (500000);                //Arun's changes - delay introduced for the callback to add the fd to the set
 
  fail_unless (gst_poll_wait (set, GST_SECOND) > 0, "Waiting was interrupted");
   fail_unless (gst_poll_fd_can_write (set, &fd),
      "Write descriptor should be writeable");
        MAIN_STOP_THREADS ();
  gst_poll_free (set);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



static gpointer
delayed_flush (gpointer data)
{
  GstPoll *set = data;

  THREAD_START ();

  g_usleep (500000);
  gst_poll_set_flushing (set, TRUE);

  return NULL;
  
}

void test_poll_wait_flush()
{
  GstPoll *set;
//xmlfile = "test_poll_wait_flush";
    std_log(LOG_FILENAME_LINE, "Test Started test_poll_wait_flush");
  set = gst_poll_new (TRUE);
  fail_if (set == NULL, "Failed to create a GstPoll");
  gst_poll_set_flushing (set, TRUE);
  fail_unless (gst_poll_wait (set, GST_SECOND) == -1 && errno == EBUSY,
      "Waiting was not flushed");
       fail_unless (gst_poll_wait (set, GST_SECOND) == -1 && errno == EBUSY,
      "Waiting was not flushed");
        gst_poll_set_flushing (set, FALSE);
  fail_unless (gst_poll_wait (set, GST_SECOND) == 0, "Waiting did not timeout");
   MAIN_START_THREADS (1, delayed_flush, set);
  fail_unless (gst_poll_wait (set, GST_SECOND) == -1 && errno == EBUSY,
      "Waiting was not flushed");
  fail_unless (gst_poll_wait (set, GST_SECOND) == -1 && errno == EBUSY,
      "Waiting was not flushed");

  gst_poll_set_flushing (set, FALSE);
  fail_unless (gst_poll_wait (set, GST_SECOND) == 0, "Waiting did not timeout");
  MAIN_STOP_THREADS ();
  gst_poll_free (set);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



static gpointer
delayed_control (gpointer data)
{
  GstPoll *set = data;
  GstPollFD fd = GST_POLL_FD_INIT;

  fd.fd = 1;

  THREAD_START ();

  g_usleep (500000);            
 
  gst_poll_add_fd (set, &fd);
  gst_poll_fd_ctl_write (set, &fd, TRUE);
  gst_poll_restart (set);

  THREAD_SYNCHRONIZE ();

  g_usleep (5000000);

  gst_poll_add_fd (set, &fd);
  gst_poll_fd_ctl_write (set, &fd, TRUE);
  gst_poll_restart (set);

  return NULL;
  
}

void test_poll_controllable()
{
  GstPoll *set;
  int i;
  GstPollFD fd = GST_POLL_FD_INIT;
//xmlfile = "test_poll_controllable";
    std_log(LOG_FILENAME_LINE, "Test Started test_poll_controllable");
  fd.fd = 1;

  set = gst_poll_new (FALSE);
  fail_if (set == NULL, "Failed to create a GstPoll");
  MAIN_START_THREADS (1, delayed_control, set);
  fail_unless (gst_poll_wait (set, GST_SECOND) == 0, "Waiting did not timeout");

  fail_unless (gst_poll_remove_fd (set, &fd), "Could not remove descriptor");
  fail_unless (gst_poll_set_controllable (set, TRUE),
      "Could not make the set controllable");
  MAIN_SYNCHRONIZE ();
  g_usleep (5000000);   //Arun's changes - delay introduced for the callback to add the fd to the set
   
 fail_unless (gst_poll_wait (set, GST_SECOND) > 0, "Waiting was interrupted");
  fail_unless (gst_poll_fd_can_write (set, &fd),
      "Write descriptor should be writeable");
  MAIN_STOP_THREADS ();
  gst_poll_free (set);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


void (*fn[]) (void) = {
test_poll_basic,
test_poll_wait_stop,
test_poll_wait_flush,
test_poll_wait_restart,
test_poll_controllable
};

char *args[] = {
"test_poll_basic",
"test_poll_wait_stop",
"test_poll_wait_flush",
"test_poll_wait_restart",
"test_poll_controllable"
};

GST_CHECK_MAIN (gst_poll);



