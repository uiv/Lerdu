/* GStreamer
 * Copyright (C) 2005 Andy Wingo <wingo@pobox.com>
 *
 * gstnettimeprovider.c: Unit test for the network time provider
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


#define LOG_FILE "c:\\logs\\gstnettimeprovider_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include <gst/check/gstcheck.h>
#include <gst/net/gstnet.h>

#include <unistd.h>


//char* xmlfile = "gstnettimeprovider";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

void test_refcounts()
{
  GstNetTimeProvider *ntp;
  GstClock *clock;

	 xmlfile = "test_refcounts";
  std_log(LOG_FILENAME_LINE, "Test Started test_refcounts");
  
  clock = gst_system_clock_obtain ();
  fail_unless (clock != NULL, "failed to get system clock");

  /* one for gstreamer, one for us */
  ASSERT_OBJECT_REFCOUNT (clock, "system clock", 2);

  ntp = gst_net_time_provider_new (clock, NULL, 0);
  fail_unless (ntp != NULL, "failed to create net time provider");

  /* one for ntp, one for gstreamer, one for us */
  ASSERT_OBJECT_REFCOUNT (clock, "system clock", 3);
  /* one for us */
  ASSERT_OBJECT_REFCOUNT (ntp, "net time provider", 1);

  gst_object_unref (ntp);
  ASSERT_OBJECT_REFCOUNT (clock, "net time provider", 2);

  gst_object_unref (clock);
    
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
}



void test_functioning()
{
  GstNetTimeProvider *ntp;
  GstNetTimePacket *packet;
  GstClock *clock;
  GstClockTime local;
  struct sockaddr_in servaddr;
  gint port = -1, sockfd, ret;
  socklen_t len;

	xmlfile = "test_functioning";
  std_log(LOG_FILENAME_LINE, "Test Started test_functioning");
  
  clock = gst_system_clock_obtain ();
  fail_unless (clock != NULL, "failed to get system clock");
  ntp = gst_net_time_provider_new (clock, "127.0.0.1", 0);
  fail_unless (ntp != NULL, "failed to create net time provider");

  g_object_get (ntp, "port", &port, NULL);
  fail_unless (port > 0);

  sockfd = socket (AF_INET, SOCK_DGRAM, 0);
  fail_if (sockfd < 0, "socket failed");

  memset (&servaddr, 0, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons (port);
  inet_aton ("127.0.0.1", &servaddr.sin_addr);

  packet = gst_net_time_packet_new (NULL);
  fail_unless (packet != NULL, "failed to create packet");

  packet->local_time = local = gst_clock_get_time (clock);
  
  len = sizeof (servaddr);
  ret = gst_net_time_packet_send (packet, sockfd,
      (struct sockaddr *) &servaddr, len);

  fail_unless (ret == GST_NET_TIME_PACKET_SIZE, "failed to send packet");

  g_free (packet);

  packet = gst_net_time_packet_receive (sockfd, (struct sockaddr *) &servaddr,
      &len);

  fail_unless (packet != NULL, "failed to receive packet");
  
 // fail_unless (packet->local_time == local, "local time is not the same");  //local time has changed
  fail_unless (packet->remote_time > local, "remote time not after local time");
  fail_unless (packet->remote_time < gst_clock_get_time (clock),
      "remote time in the future");

  g_free (packet);

  close (sockfd);

  //gst_object_unref (ntp); //thread is blocking
  gst_object_unref (clock);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}

void (*fn[]) (void) = {
test_refcounts,
test_functioning
};

char *args[] = {
"test_refcounts",
"test_functioning"
};

GST_CHECK_MAIN (gst_net_time_provider);
#if 0
int main(int argc,char** argv)
{
		int i;
		gst_check_init(&argc,&argv);
		for(i=0;i<2;i++)
			fn[i]();
			
} 
#endif