/* GStreamer
 *
 * unit test for adapter
 *
 * Copyright (C) <2005> Wim Taymans <wim at fluendo dot com>
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


#define LOG_FILE "c:\\logs\\adapter_logs.txt" 

#include <gst/gst_global.h>
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include <gst/check/gstcheck.h>

#include <gst/base/gstadapter.h>



void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

/* does some implementation dependent checking that should 
 * also be optimal 
 */

/*
 * Start peeking on an adapter with 1 buffer pushed. 
 */
void test_peek1()
{
  GstAdapter *adapter;
  GstBuffer *buffer;
  guint avail;
  const guint8 *bufdata, *data1, *data2;


	xmlfile = "test_peek1";
  std_log(LOG_FILENAME_LINE, "Test Started test_peek1");


  adapter = gst_adapter_new ();
  fail_if (adapter == NULL);
  
  /* push single buffer in adapter */
  buffer = gst_buffer_new_and_alloc (512);
  bufdata = GST_BUFFER_DATA (buffer);

  fail_if (buffer == NULL);
  
  gst_adapter_push (adapter, buffer);

  /* available and available_fast should return the size of the 
   * buffer */
  avail = gst_adapter_available (adapter);
  fail_if (avail != 512);
  
  avail = gst_adapter_available_fast (adapter);
  fail_if (avail != 512);
  
  
  /* should g_critical with NULL as result */
#ifdef SYMBIAN32
  ASSERT_CRITICAL (data1 = gst_adapter_peek (adapter, 0));
  fail_if ( data1!= NULL);
#endif 
  
  /* should return NULL as result */
  data1 = gst_adapter_peek (adapter, 513);
  fail_if (data1 != NULL);
  
  
  /* this should work */
  data1 = gst_adapter_peek (adapter, 512);
  fail_if (data1 == NULL);
  
  
  /* it should point to the buffer data as well */
  fail_if (data1 != bufdata);
  data2 = gst_adapter_peek (adapter, 512);
  fail_if (data2 == NULL);
  
  /* second peek should return the same pointer */
  fail_if (data2 != data1);
  

#ifdef SYMBIAN32
  /* this should fail since we don't have that many bytes */
  ASSERT_CRITICAL (gst_adapter_flush (adapter, 513));
#endif  
  /* this should work fine */
  gst_adapter_flush (adapter, 10);

  /* see if we have 10 bytes less available */
  avail = gst_adapter_available (adapter);
  fail_if (avail != 502);
  
  avail = gst_adapter_available_fast (adapter);
  fail_if (avail != 502);
  
  /* should return NULL as result */
  data2 = gst_adapter_peek (adapter, 503);
  fail_if (data2 != NULL);
  
  /* should work fine */
  data2 = gst_adapter_peek (adapter, 502);
  fail_if (data2 == NULL);
  
  /* peek should return the same old pointer + 10 */
  fail_if (data2 != data1 + 10);
  
  fail_if (data2 != bufdata + 10);
  
  /* flush some more */
  gst_adapter_flush (adapter, 500);

  /* see if we have 2 bytes available */
  avail = gst_adapter_available (adapter);
  fail_if (avail != 2);
  
  avail = gst_adapter_available_fast (adapter);
  fail_if (avail != 2);
  
  data2 = gst_adapter_peek (adapter, 2);
  
  fail_if (data2 == NULL);
  
  fail_if (data2 != data1 + 510);
  
  fail_if (data2 != bufdata + 510);
  
  /* flush some more */
  gst_adapter_flush (adapter, 2);

  /* see if we have 0 bytes available */
  avail = gst_adapter_available (adapter);
  fail_if (avail != 0);
  
  avail = gst_adapter_available_fast (adapter);
  fail_if (avail != 0);
  
  /* silly clear just for fun */
  gst_adapter_clear (adapter);

  g_object_unref (adapter);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* Start peeking on an adapter with 2 non-mergeable buffers 
 * pushed. 
 */
void test_peek2()
{
}



/* Start peeking on an adapter with 2 mergeable buffers 
 * pushed. 
 */
void test_peek3()
{
}



/* take data from an adapter with 1 buffer pushed.
 */
void test_take1()
{
}



/* take data from an adapter with 2 non-mergeable buffers 
 * pushed.
 */
void test_take2()
{
}



/* take data from an adapter with 2 mergeable buffers 
 * pushed.
 */
void test_take3()
{
}



static GstAdapter *
create_and_fill_adapter (void)
{
  GstAdapter *adapter;
  gint i, j;

//	xmlfile = "create_and_fill_adapter";
//  std_log(LOG_FILENAME_LINE, "Test Started create_and_fill_adapter");
  
  adapter = gst_adapter_new ();
  fail_unless (adapter != NULL);

  for (i = 0; i < 10000; i += 4) {
    GstBuffer *buf = gst_buffer_new_and_alloc (sizeof (guint32) * 4);
    guint8 *data;

    fail_unless (buf != NULL);
    data = GST_BUFFER_DATA (buf);

    for (j = 0; j < 4; j++) {
      GST_WRITE_UINT32_LE (data, i + j);
      data += sizeof (guint32);
    }
    gst_adapter_push (adapter, buf);
  }

//	std_log(LOG_FILENAME_LINE, "Test Successful");
//  create_xml(0); 
  
  return adapter;
}

/* Fill a buffer with a sequence of 32 bit ints and read them back out,
 * checking that they're still in the right order */
void test_take_order()
{
  GstAdapter *adapter;
  int i = 0;

		xmlfile = "test_take_order";
  std_log(LOG_FILENAME_LINE, "Test Started test_take_order");
  
  adapter = create_and_fill_adapter ();
  while (gst_adapter_available (adapter) >= sizeof (guint32)) {
    guint8 *data = gst_adapter_take (adapter, sizeof (guint32));

    fail_unless (GST_READ_UINT32_LE (data) == i);
    i++;
    g_free (data);
  }
  fail_unless (gst_adapter_available (adapter) == 0,
      "Data was left in the adapter");

  g_object_unref (adapter);

	std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
}



/* Fill a buffer with a sequence of 32 bit ints and read them back out
 * using take_buffer, checking that they're still in the right order */
void test_take_buf_order()
{
  GstAdapter *adapter;
  int i = 0;

		xmlfile = "test_take_buf_order";
  std_log(LOG_FILENAME_LINE, "Test Started test_take_buf_order");

  adapter = create_and_fill_adapter ();
  while (gst_adapter_available (adapter) >= sizeof (guint32)) {
    GstBuffer *buf = gst_adapter_take_buffer (adapter, sizeof (guint32));

    fail_unless (GST_READ_UINT32_LE (GST_BUFFER_DATA (buf)) == i);
    i++;

    gst_buffer_unref (buf);
  }
  fail_unless (gst_adapter_available (adapter) == 0,
      "Data was left in the adapter");

  g_object_unref (adapter);
  
  	std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



void
gst_adapter_suite (void)
{
test_peek1();
test_take_order();
test_take_buf_order();
}

void (*fn[]) (void) = {
test_peek1,
test_take_order,
test_take_buf_order
};

char *args[] = {
"test_peek1",
"test_take_order",
"test_take_buf_order"
};

GST_CHECK_MAIN (gst_adapter);
#if 0 
int main(int argc,char** argv)
{	
		int i;
		gst_init(&argc,&argv);
		for(i=0;i<3;i++)
		    {
			fn[i]();
		    }
} 
#endif