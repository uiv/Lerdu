/* GStreamer unit tests for libgstnetbuffer
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define LOG_FILE "c:\\logs\\netbuffer_logs.txt" 

#include <gst/gst_global.h>
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include "libgstreamer_wsd_solution.h" 
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers;
#endif

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


//#include <gst/check/gstcheck.h>
#include <gst/netbuffer/gstnetbuffer.h>
#include <gstcheck.h>

#define DATA_STRING "Yoho this is a string"

void test_netbuffer_copy()
{
  GstNetBuffer *netbuf, *copy;
  guint8 ipv6_addr[16] = { 0xff, 0x11, 0xee, 0x22, 0xdd, 0x33, 0xcc,
    0x44, 0xbb, 0x55, 0xaa, 0x66, 0x00, 0x77, 0x99, 0x88
  };
  guint8 ipv6_copy[16];
  guint32 ipv4_copy, ipv4_addr = 0xfe12dc34;
  guint16 ipv6_port = 3490;
  guint16 ipv4_port = 5678;
  guint16 port;
    
    xmlfile = "test_netbuffer_copy";
    std_log(LOG_FILENAME_LINE, "Test Started test_netbuffer_copy");

  netbuf = gst_netbuffer_new ();
  fail_unless (netbuf != NULL, "failed to create net buffer");

  gst_netaddress_set_ip4_address (&netbuf->from, ipv4_addr, ipv4_port);
  gst_netaddress_set_ip6_address (&netbuf->to, ipv6_addr, ipv6_port);

  GST_BUFFER_DATA (netbuf) = (guint8 *) DATA_STRING;
  GST_BUFFER_SIZE (netbuf) = strlen (DATA_STRING);
  GST_BUFFER_FLAG_SET (netbuf, GST_BUFFER_FLAG_DISCONT);
  GST_BUFFER_FLAG_SET (netbuf, GST_BUFFER_FLAG_READONLY);

  copy = (GstNetBuffer *) gst_buffer_copy (netbuf);
  fail_unless (copy != NULL, "failed to copy net buffer");
  fail_unless (GST_IS_NETBUFFER (copy), "copied buffer is not a GstNetBuffer!");

  fail_unless_equals_int (GST_MINI_OBJECT_REFCOUNT_VALUE (copy), 1);

  fail_unless_equals_int (GST_BUFFER_SIZE (copy), GST_BUFFER_SIZE (netbuf));
  fail_unless (memcmp (GST_BUFFER_DATA (copy), GST_BUFFER_DATA (netbuf),
          GST_BUFFER_SIZE (copy)) == 0);

  fail_if (GST_BUFFER_FLAG_IS_SET (copy, GST_BUFFER_FLAG_READONLY));
  fail_unless (GST_BUFFER_FLAG_IS_SET (copy, GST_BUFFER_FLAG_DISCONT));

  fail_unless (gst_netaddress_get_ip4_address (&copy->from, &ipv4_copy, &port));
  fail_unless (ipv4_copy == ipv4_addr,
      "Copied buffer has wrong IPV4 from address");
  fail_unless (port == ipv4_port, "Copied buffer has wrong IPV4 from port");

  fail_unless (gst_netaddress_get_ip6_address (&copy->to, ipv6_copy, &port));
  fail_unless (memcmp (ipv6_copy, ipv6_addr, 16) == 0,
      "Copied buffer has wrong IPv6 destination address");
  fail_unless (port == ipv6_port,
      "Copied buffer has wrong IPv6 destination port");

  gst_buffer_unref (netbuf);
  gst_buffer_unref (copy);

    std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);

}
/*
int main (int argc, char **argv)
{
//gst_init(&argc, &argv);
gst_check_init (NULL,NULL );
test_netbuffer_copy();
}

*/
void
netbuffer_suite (void)
{
test_netbuffer_copy();
}

void (*fn[]) (void) = {
test_netbuffer_copy
};

char *args[] = {
"test_netbuffer_copy"
};

GST_CHECK_MAIN (netbuffer);



