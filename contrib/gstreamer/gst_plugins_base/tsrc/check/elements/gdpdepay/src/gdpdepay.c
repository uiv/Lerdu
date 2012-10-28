/* GStreamer
 *
 * Copyright (C) 2006 Thomas Vander Stichele <thomas at apestaart dot org>
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
#include "config.h"
#include <string.h>
#include <unistd.h>


#define LOG_FILE "c:\\logs\\gdpdepay_logs.txt" 
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
static GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers;
#endif

#include <gst/check/gstcheck.h>
#include <gst/dataprotocol/dataprotocol.h>

/* For ease of programming we use globals to keep refs for our floating
 * src and sink pads we create; otherwise we always have to do get_pad,
 * get_peer, and then remove references in every test function */
static GstPad *mysrcpad, *mysinkpad, *myshsinkpad;

#define AUDIO_CAPS_TEMPLATE_STRING \
  "audio/x-raw-int, " \
    "rate = (int) [ 1, MAX ], " \
    "channels = (int) [ 1, 8 ], " \
    "endianness = (int) BYTE_ORDER, " \
    "width = (int) {8, 16}, " \
    "depth = (int) {8, 16}, " \
    "signed = (boolean) true"

#define AUDIO_CAPS_STRING \
  "audio/x-raw-int, " \
    "rate = (int) 1000, " \
    "channels = (int) 2, " \
    "endianness = (int) BYTE_ORDER, " \
    "width = (int) 16, " \
    "depth = (int) 16, " \
    "signed = (boolean) true"


static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (AUDIO_CAPS_TEMPLATE_STRING)
    );
static GstStaticPadTemplate srctemplate = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-gdp")
    );

/* takes over reference for outcaps */
static GstElement *
setup_gdpdepay (void)
{
  GstElement *gdpdepay;

  GST_DEBUG ("setup_gdpdepay");
  gdpdepay = gst_check_setup_element ("gdpdepay");
  mysrcpad = gst_check_setup_src_pad (gdpdepay, &srctemplate, NULL);
  mysinkpad = gst_check_setup_sink_pad (gdpdepay, &sinktemplate, NULL);
  gst_pad_set_active (mysrcpad, TRUE);
  gst_pad_set_active (mysinkpad, TRUE);

  return gdpdepay;
}

static void
cleanup_gdpdepay (GstElement * gdpdepay)
{
  GST_DEBUG ("cleanup_gdpdepay");

  gst_pad_set_active (mysrcpad, FALSE);
  if (mysinkpad)
    gst_pad_set_active (mysinkpad, FALSE);
  if (myshsinkpad)
    gst_pad_set_active (myshsinkpad, FALSE);
  gst_check_teardown_src_pad (gdpdepay);
  gst_check_teardown_sink_pad (gdpdepay);
  gst_check_teardown_element (gdpdepay);
  mysinkpad = NULL;
  myshsinkpad = NULL;
}

static void
gdpdepay_push_per_byte (gchar * reason, guint8 * bytes, guint length)
{
  int i;
  GstBuffer *inbuffer;

  for (i = 0; i < length; ++i) {
    inbuffer = gst_buffer_new_and_alloc (1);
    GST_BUFFER_DATA (inbuffer)[0] = bytes[i];
    fail_unless (gst_pad_push (mysrcpad, inbuffer) == GST_FLOW_OK,
        "%s: failed pushing byte buffer", reason);
  }
}

void test_audio_per_byte()
{
  GstCaps *caps;
  GstPad *srcpad;
  GstElement *gdpdepay;
  GstBuffer *buffer, *outbuffer;
  guint8 *header, *payload;
  guint len;
  GstDPPacketizer *pk;
  
  std_log(LOG_FILENAME_LINE, "Test Started test_audio_per_byte");

  pk = gst_dp_packetizer_new (GST_DP_VERSION_1_0);
  std_log(LOG_FILENAME_LINE, "0");
  gdpdepay = setup_gdpdepay ();
  std_log(LOG_FILENAME_LINE, "1");
  srcpad = gst_element_get_pad (gdpdepay, "src");
  std_log(LOG_FILENAME_LINE, "2");
  fail_unless (gst_element_set_state (gdpdepay,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");
  std_log(LOG_FILENAME_LINE, "3");
  caps = gst_pad_get_caps (srcpad);
  std_log(LOG_FILENAME_LINE, "4");
  fail_unless (gst_caps_is_any (caps));
  std_log(LOG_FILENAME_LINE, "5");
  gst_caps_unref (caps);
  std_log(LOG_FILENAME_LINE, "6");
  fail_if (gst_pad_get_negotiated_caps (srcpad));
  std_log(LOG_FILENAME_LINE, "7");
  /* create caps and buffer packets and push them */
  caps = gst_caps_from_string (AUDIO_CAPS_STRING);
  std_log(LOG_FILENAME_LINE, "8");
  fail_unless (pk->packet_from_caps (caps, 0, &len, &header, &payload));
  std_log(LOG_FILENAME_LINE, "9");
  gst_caps_unref (caps);
  std_log(LOG_FILENAME_LINE, "10");
  gdpdepay_push_per_byte ("caps header", header, len);
  std_log(LOG_FILENAME_LINE, "11");
  fail_unless_equals_int (g_list_length (buffers), 0);
  std_log(LOG_FILENAME_LINE, "12");
  gdpdepay_push_per_byte ("caps payload", payload,          
      gst_dp_header_payload_length (header));
  std_log(LOG_FILENAME_LINE, "13");
  fail_unless_equals_int (g_list_length (buffers), 0);
  std_log(LOG_FILENAME_LINE, "14");
  caps = gst_pad_get_caps (srcpad);
  std_log(LOG_FILENAME_LINE, "15");
  fail_if (gst_caps_is_any (caps));
  std_log(LOG_FILENAME_LINE, "16");
  gst_caps_unref (caps);
  std_log(LOG_FILENAME_LINE, "17");
  g_free (header);
  std_log(LOG_FILENAME_LINE, "18");
  g_free (payload);
  std_log(LOG_FILENAME_LINE, "19");

  buffer = gst_buffer_new_and_alloc (4);
  std_log(LOG_FILENAME_LINE, "20");
  memcpy (GST_BUFFER_DATA (buffer), "f00d", 4);
  std_log(LOG_FILENAME_LINE, "21");
  GST_BUFFER_TIMESTAMP (buffer) = GST_SECOND;
  GST_BUFFER_DURATION (buffer) = GST_SECOND / 10;
  std_log(LOG_FILENAME_LINE, "22");
  fail_unless (pk->header_from_buffer (buffer, 0, &len, &header));
  std_log(LOG_FILENAME_LINE, "23");
  gdpdepay_push_per_byte ("buffer header", header, len);
  std_log(LOG_FILENAME_LINE, "24");
  fail_unless_equals_int (g_list_length (buffers), 0);
  std_log(LOG_FILENAME_LINE, "25");
  gdpdepay_push_per_byte ("buffer payload", GST_BUFFER_DATA (buffer),
      gst_dp_header_payload_length (header));
  std_log(LOG_FILENAME_LINE, "26");
  g_free (header);
  std_log(LOG_FILENAME_LINE, "27");
  gst_buffer_unref (buffer);
  std_log(LOG_FILENAME_LINE, "28");
  fail_unless_equals_int (g_list_length (buffers), 1);
  fail_if ((outbuffer = (GstBuffer *) buffers->data) == NULL);
  fail_unless_equals_uint64 (GST_BUFFER_TIMESTAMP (outbuffer), GST_SECOND);
  std_log(LOG_FILENAME_LINE, "29");
  fail_unless_equals_uint64 (GST_BUFFER_DURATION (outbuffer), GST_SECOND / 10);
  std_log(LOG_FILENAME_LINE, "30");
  buffers = g_list_remove (buffers, outbuffer);
  gst_buffer_unref (outbuffer);
  std_log(LOG_FILENAME_LINE, "31");
  fail_unless (gst_element_set_state (gdpdepay,
          GST_STATE_NULL) == GST_STATE_CHANGE_SUCCESS, "could not set to null");

  ASSERT_OBJECT_REFCOUNT (gdpdepay, "gdpdepay", 1);
  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;
  gst_object_unref (srcpad);
  cleanup_gdpdepay (gdpdepay);

  gst_dp_packetizer_free (pk);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_audio_in_one_buffer()
{
  GstCaps *caps;
  GstPad *srcpad;
  GstElement *gdpdepay;
  GstBuffer *buffer, *inbuffer;
  guint8 *caps_header, *caps_payload, *buf_header;
  guint header_len, payload_len;
  guint i;
  GstDPPacketizer *pk;
  
  std_log(LOG_FILENAME_LINE, "Test Started test_audio_in_one_buffer");

  pk = gst_dp_packetizer_new (GST_DP_VERSION_1_0);

  gdpdepay = setup_gdpdepay ();
  srcpad = gst_element_get_pad (gdpdepay, "src");

  fail_unless (gst_element_set_state (gdpdepay,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  /* make sure no caps are set yet */
  caps = gst_pad_get_caps (srcpad);
  fail_unless (gst_caps_is_any (caps));
  gst_caps_unref (caps);
  fail_if (gst_pad_get_negotiated_caps (srcpad));

  /* create caps and buffer packets and push them as one buffer */
  caps = gst_caps_from_string (AUDIO_CAPS_STRING);
  fail_unless (pk->packet_from_caps (caps, 0, &header_len, &caps_header,
          &caps_payload));

  buffer = gst_buffer_new_and_alloc (4);
  memcpy (GST_BUFFER_DATA (buffer), "f00d", 4);
  fail_unless (pk->header_from_buffer (buffer, 0, &header_len, &buf_header));

  payload_len = gst_dp_header_payload_length (caps_header);

  inbuffer = gst_buffer_new_and_alloc (2 * GST_DP_HEADER_LENGTH +
      payload_len + GST_BUFFER_SIZE (buffer));
  memcpy (GST_BUFFER_DATA (inbuffer), caps_header, GST_DP_HEADER_LENGTH);
  i = GST_DP_HEADER_LENGTH;
  memcpy (GST_BUFFER_DATA (inbuffer) + i, caps_payload, payload_len);
  i += payload_len;
  memcpy (GST_BUFFER_DATA (inbuffer) + i, buf_header, GST_DP_HEADER_LENGTH);
  i += GST_DP_HEADER_LENGTH;
  memcpy (GST_BUFFER_DATA (inbuffer) + i, GST_BUFFER_DATA (buffer),
      GST_BUFFER_SIZE (buffer));

  gst_caps_unref (caps);
  gst_buffer_unref (buffer);

  g_free (caps_header);
  g_free (caps_payload);
  g_free (buf_header);

  /* now push it */
  gst_pad_push (mysrcpad, inbuffer);

  /* the buffer is still queued */
  fail_unless_equals_int (g_list_length (buffers), 1);

  fail_unless (gst_element_set_state (gdpdepay,
          GST_STATE_NULL) == GST_STATE_CHANGE_SUCCESS, "could not set to null");

  gst_object_unref (srcpad);
  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;
  ASSERT_OBJECT_REFCOUNT (gdpdepay, "gdpdepay", 1);
  cleanup_gdpdepay (gdpdepay);

  gst_dp_packetizer_free (pk);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



static GstStaticPadTemplate shsinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-gst-test-streamheader")
    );

static GstElement *
setup_gdpdepay_streamheader (void)
{
  GstElement *gdpdepay;

  GST_DEBUG ("setup_gdpdepay");
  gdpdepay = gst_check_setup_element ("gdpdepay");
  mysrcpad = gst_check_setup_src_pad (gdpdepay, &srctemplate, NULL);
  myshsinkpad = gst_check_setup_sink_pad (gdpdepay, &shsinktemplate, NULL);
  gst_pad_set_active (mysrcpad, TRUE);
  gst_pad_set_active (myshsinkpad, TRUE);

  return gdpdepay;
}

#ifndef HAVE_CPU_PPC64          /* Test known to fail on PPC64. See #348114 */

/* this tests deserialization of a GDP stream where the serialized caps
 * have a streamheader set */
void test_streamheader()
{
  GstCaps *caps, *padcaps;
  GstPad *srcpad;
  GstElement *gdpdepay;
  GstBuffer *buffer, *inbuffer, *outbuffer, *shbuffer;
  guint8 *caps_header, *caps_payload, *buf_header;
  guint header_len, payload_len;
  guint i;
  GstStructure *structure;
  GValue array = { 0 };
  GValue value = { 0 };
  GstDPPacketizer *pk;

  std_log(LOG_FILENAME_LINE, "Test Started test_streamheader");
  
  pk = gst_dp_packetizer_new (GST_DP_VERSION_1_0);

  gdpdepay = setup_gdpdepay_streamheader ();
  srcpad = gst_element_get_pad (gdpdepay, "src");
  ASSERT_OBJECT_REFCOUNT (gdpdepay, "gdpdepay", 1);

  fail_unless (gst_element_set_state (gdpdepay,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  /* make sure no caps are set yet */
  caps = gst_pad_get_caps (srcpad);
  fail_unless (gst_caps_is_any (caps));
  gst_caps_unref (caps);
  fail_if (gst_pad_get_negotiated_caps (srcpad));

  /* create a streamheader buffer and the caps containing it */
  caps = gst_caps_from_string ("application/x-gst-test-streamheader");
  structure = gst_caps_get_structure (caps, 0);
  buffer = gst_buffer_new_and_alloc (4);
  memcpy (GST_BUFFER_DATA (buffer), "f00d", 4);
  GST_BUFFER_FLAG_SET (buffer, GST_BUFFER_FLAG_IN_CAPS);
  g_value_init (&array, GST_TYPE_ARRAY);
  g_value_init (&value, GST_TYPE_BUFFER);
  shbuffer = gst_buffer_copy (buffer);
  gst_value_set_buffer (&value, shbuffer);
  gst_buffer_unref (shbuffer);
  gst_value_array_append_value (&array, &value);
  g_value_unset (&value);
  gst_structure_set_value (structure, "streamheader", &array);
  g_value_unset (&array);

  gst_buffer_set_caps (buffer, caps);

  /* create GDP packets for the caps and the buffer, and put them in one
   * GDP buffer */
  fail_unless (pk->packet_from_caps (caps, 0, &header_len, &caps_header,
          &caps_payload));

  fail_unless (pk->header_from_buffer (buffer, 0, &header_len, &buf_header));

  payload_len = gst_dp_header_payload_length (caps_header);

  inbuffer = gst_buffer_new_and_alloc (2 * GST_DP_HEADER_LENGTH +
      payload_len + GST_BUFFER_SIZE (buffer));
  memcpy (GST_BUFFER_DATA (inbuffer), caps_header, GST_DP_HEADER_LENGTH);
  i = GST_DP_HEADER_LENGTH;
  memcpy (GST_BUFFER_DATA (inbuffer) + i, caps_payload, payload_len);
  i += payload_len;
  memcpy (GST_BUFFER_DATA (inbuffer) + i, buf_header, GST_DP_HEADER_LENGTH);
  i += GST_DP_HEADER_LENGTH;
  memcpy (GST_BUFFER_DATA (inbuffer) + i, GST_BUFFER_DATA (buffer),
      GST_BUFFER_SIZE (buffer));

  gst_caps_unref (caps);
  gst_buffer_unref (buffer);

  g_free (caps_header);
  g_free (caps_payload);
  g_free (buf_header);

  /* now push it */
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);
  gst_pad_push (mysrcpad, inbuffer);

  /* our only output buffer is the streamheader buffer */
  fail_unless_equals_int (g_list_length (buffers), 1);
  fail_if ((outbuffer = (GstBuffer *) buffers->data) == NULL);
  buffers = g_list_remove (buffers, outbuffer);
  ASSERT_BUFFER_REFCOUNT (outbuffer, "outbuffer", 1);
  fail_unless (GST_BUFFER_FLAG_IS_SET (outbuffer, GST_BUFFER_FLAG_IN_CAPS));

  padcaps = gst_pad_get_negotiated_caps (myshsinkpad);
  caps = gst_buffer_get_caps (outbuffer);
  fail_if (caps == NULL);
  fail_if (padcaps == NULL);
  GST_DEBUG ("caps: %" GST_PTR_FORMAT ", padcaps: %" GST_PTR_FORMAT, caps,
      padcaps);
  fail_unless (gst_caps_is_equal (padcaps, caps));

  /* FIXME: get streamheader, compare data with buffer */
  gst_buffer_unref (outbuffer);
  gst_caps_unref (padcaps);
  gst_caps_unref (caps);

  /* clean up */
  fail_unless (gst_element_set_state (gdpdepay,
          GST_STATE_NULL) == GST_STATE_CHANGE_SUCCESS, "could not set to null");

  gst_object_unref (srcpad);
  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;
  ASSERT_OBJECT_REFCOUNT (gdpdepay, "gdpdepay", 1);
  cleanup_gdpdepay (gdpdepay);

  gst_dp_packetizer_free (pk);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



#endif /* ifndef HAVE_CPU_PPC64 */



void (*fn[]) (void) = {
test_audio_per_byte,
test_audio_in_one_buffer,
test_streamheader
};

char *args[] = {
"test_audio_per_byte",
"test_audio_in_one_buffer",
"test_streamheader"
};

GST_CHECK_MAIN(gdpdeplay)
