/* GStreamer unit tests for the RTP support library
 *
 * Copyright (C) 2007 Tim-Philipp Müller <tim centricular net>
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

#include <gst/rtp/gstrtpbuffer.h>
#include <string.h>

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical;
#endif

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log;
#endif


#define RTP_HEADER_LEN 12

void test_rtp_buffer()
{
  GstBuffer *buf;
  guint8 *data;

    xmlfile = "gst_rtp_test_rtp_buffer";
    std_log(LOG_FILENAME_LINE, "Test Started test_rtp_buffer");

  /* check GstRTPHeader structure alignment and packing */
  buf = (GstBuffer *) gst_rtp_buffer_new_allocate ((guint)16, (guint8)4, (guint8)0);
  fail_unless (buf != NULL);
  fail_unless_equals_int (GST_BUFFER_SIZE (buf), RTP_HEADER_LEN + 16 + 4);
  data = GST_BUFFER_DATA (buf);

  /* check defaults */
  fail_unless_equals_int (gst_rtp_buffer_get_version (buf), 2);
  fail_unless (gst_rtp_buffer_get_padding (buf) == FALSE);
  fail_unless (gst_rtp_buffer_get_extension (buf) == FALSE);
  fail_unless_equals_int (gst_rtp_buffer_get_csrc_count (buf), 0);
  fail_unless (gst_rtp_buffer_get_marker (buf) == FALSE);
  fail_unless (gst_rtp_buffer_get_payload_type (buf) == 0);
  fail_unless_equals_int (GST_READ_UINT16_BE (data), 0x8000);

  /* check version in bitfield */
  gst_rtp_buffer_set_version (buf, 3);
  fail_unless_equals_int (gst_rtp_buffer_get_version (buf), 3);
  fail_unless_equals_int ((data[0] & 0xC0) >> 6, 3);
  gst_rtp_buffer_set_version (buf, 2);
  fail_unless_equals_int (gst_rtp_buffer_get_version (buf), 2);
  fail_unless_equals_int ((data[0] & 0xC0) >> 6, 2);

  /* check padding bit */
  gst_rtp_buffer_set_padding (buf, TRUE);
  fail_unless (gst_rtp_buffer_get_padding (buf) == TRUE);
  fail_unless_equals_int ((data[0] & 0x20) >> 5, 1);
  gst_rtp_buffer_set_padding (buf, FALSE);
  fail_unless (gst_rtp_buffer_get_padding (buf) == FALSE);
  fail_unless_equals_int ((data[0] & 0x20) >> 5, 0);

  /* check marker bit */
  gst_rtp_buffer_set_marker (buf, TRUE);
  fail_unless (gst_rtp_buffer_get_marker (buf) == TRUE);
  fail_unless_equals_int ((data[1] & 0x80) >> 7, 1);
  gst_rtp_buffer_set_marker (buf, FALSE);
  fail_unless (gst_rtp_buffer_get_marker (buf) == FALSE);
  fail_unless_equals_int ((data[1] & 0x80) >> 7, 0);

  /* check sequence offset */
  gst_rtp_buffer_set_seq (buf, 0xF2C9);
  fail_unless_equals_int (gst_rtp_buffer_get_seq (buf), 0xF2C9);
  fail_unless_equals_int (GST_READ_UINT16_BE (data + 2), 0xF2C9);
  gst_rtp_buffer_set_seq (buf, 0);
  fail_unless_equals_int (gst_rtp_buffer_get_seq (buf), 0);
  fail_unless_equals_int (GST_READ_UINT16_BE (data + 2), 0);

  /* check timestamp offset */
  gst_rtp_buffer_set_timestamp (buf, 432191);
  fail_unless_equals_int (GST_READ_UINT32_BE (data + 4), 432191);
  fail_unless_equals_int (gst_rtp_buffer_get_timestamp (buf), 432191);
  gst_rtp_buffer_set_timestamp (buf, 0);
  fail_unless_equals_int (gst_rtp_buffer_get_timestamp (buf), 0);
  fail_unless_equals_int (GST_READ_UINT32_BE (data + 4), 0);

  /* check ssrc offset */
  gst_rtp_buffer_set_ssrc (buf, 0xf04043C2);
  fail_unless_equals_int (gst_rtp_buffer_get_ssrc (buf), (gint) 0xf04043c2);
  fail_unless_equals_int (GST_READ_UINT32_BE (data + 4 + 4), (gint) 0xf04043c2);
  gst_rtp_buffer_set_ssrc (buf, 0);
  fail_unless_equals_int (gst_rtp_buffer_get_ssrc (buf), 0);
  fail_unless_equals_int (GST_READ_UINT32_BE (data + 4 + 4), 0);

  /* check csrc bits */
  fail_unless_equals_int (gst_rtp_buffer_get_csrc_count (buf), 0);
  ASSERT_CRITICAL (gst_rtp_buffer_get_csrc (buf, 0));
  fail_unless_equals_int (data[0] & 0xf, 0);
  gst_buffer_unref (buf);

  /* and again, this time with CSRCs */
  buf = (GstBuffer *) gst_rtp_buffer_new_allocate ((guint)16, (guint8)4, (guint8)3);
  fail_unless (buf != NULL);
  fail_unless_equals_int (GST_BUFFER_SIZE (buf),
      RTP_HEADER_LEN + 16 + 4 + 4 * 3);

  data = GST_BUFFER_DATA (buf);

  fail_unless_equals_int (gst_rtp_buffer_get_csrc_count (buf), 3);
  ASSERT_CRITICAL (gst_rtp_buffer_get_csrc (buf, 3));
  fail_unless_equals_int (data[0] & 0xf, 3);
  fail_unless_equals_int (gst_rtp_buffer_get_csrc (buf, 0), 0);
  fail_unless_equals_int (gst_rtp_buffer_get_csrc (buf, 1), 0);
  fail_unless_equals_int (gst_rtp_buffer_get_csrc (buf, 2), 0);

  data += RTP_HEADER_LEN;       /* skip the other header stuff */
  gst_rtp_buffer_set_csrc (buf, 0, 0xf7c0);
  fail_unless_equals_int (GST_READ_UINT32_BE (data + 0 * 4), 0xf7c0);
  gst_rtp_buffer_set_csrc (buf, 1, 0xf7c1);
  fail_unless_equals_int (GST_READ_UINT32_BE (data + 1 * 4), 0xf7c1);
  gst_rtp_buffer_set_csrc (buf, 2, 0xf7c2);
  fail_unless_equals_int (GST_READ_UINT32_BE (data + 2 * 4), 0xf7c2);
  ASSERT_CRITICAL (gst_rtp_buffer_set_csrc (buf, 3, 0xf123));
  gst_buffer_unref (buf);
    std_log(LOG_FILENAME_LINE, "Test Successful");
      create_xml(0);
}



void test_rtp_buffer_set_extension_data()
{
  GstBuffer *buf;
  guint8 *data;
  guint16 bits;
  guint size;
  gpointer pointer;
  
      xmlfile = "gst_rtp_test_rtp_buffer_set_extension_data";
    std_log(LOG_FILENAME_LINE, "Test Started test_rtp_buffer_set_extension_data");

  /* check GstRTPHeader structure alignment and packing */
  buf = (GstBuffer *) gst_rtp_buffer_new_allocate ((guint)4, (guint8)0, (guint8)0);
  data = GST_BUFFER_DATA (buf);

  /* should be impossible to set the extension data */
  fail_unless (gst_rtp_buffer_set_extension_data (buf, 0, 4) == FALSE);
  fail_unless (gst_rtp_buffer_get_extension (buf) == TRUE);

  /* should be possible to set the extension data */
  fail_unless (gst_rtp_buffer_set_extension_data (buf, 270, 0) == TRUE);
  fail_unless (gst_rtp_buffer_get_extension (buf) == TRUE);
  gst_rtp_buffer_get_extension_data (buf, &bits, &pointer, &size);
  fail_unless (bits == 270);
  fail_unless (size == 0);
  fail_unless (pointer == GST_BUFFER_DATA (buf) + 16);
  pointer = (gpointer)gst_rtp_buffer_get_payload ((GstBuffer *) buf);
  fail_unless (pointer == GST_BUFFER_DATA (buf) + 16);
  gst_buffer_unref (buf);

  buf = (GstBuffer *)gst_rtp_buffer_new_allocate ((guint)20, (guint8)0, (guint8)0);
  data = GST_BUFFER_DATA (buf);
  fail_unless (gst_rtp_buffer_get_extension (buf) == FALSE);
  fail_unless (gst_rtp_buffer_set_extension_data (buf, 333, 2) == TRUE);
  fail_unless (gst_rtp_buffer_get_extension (buf) == TRUE);
  gst_rtp_buffer_get_extension_data (buf, &bits, &pointer, &size);
  fail_unless (bits == 333);
  fail_unless (size == 2);
  fail_unless (pointer == GST_BUFFER_DATA (buf) + 16);
  pointer = (gpointer)gst_rtp_buffer_get_payload ((GstBuffer *)buf);
  fail_unless (pointer == GST_BUFFER_DATA (buf) + 24);
  gst_buffer_unref (buf);
    std_log(LOG_FILENAME_LINE, "Test Successful");
      create_xml(0);
}



void
rtp_suite (void)
{
test_rtp_buffer();
test_rtp_buffer_set_extension_data();
}

void (*fn[]) (void) = {
test_rtp_buffer,
test_rtp_buffer_set_extension_data
};

char *args[] = {
"test_rtp_buffer",
"test_rtp_buffer_set_extension_data"
};

GST_CHECK_MAIN (gst_rtp);

#if 0
void main()
    {
    gst_check_init (NULL, NULL);
    test_rtp_buffer();
    test_rtp_buffer_set_extension_data();
    }
    
#endif 


