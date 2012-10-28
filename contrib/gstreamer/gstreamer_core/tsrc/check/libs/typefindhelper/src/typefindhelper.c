/* GStreamer
 *
 * unit test for typefind helper
 *
 * Copyright (C) 2006 Tim-Philipp Müller  <tim centricular net>
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
#ifdef HAVE_CONFIG_H
#include "config.h"
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

#define LOG_FILE "c:\\logs\\typefindhelper_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include <gst/check/gstcheck.h>

#include <gst/base/gsttypefindhelper.h>

static const guint8 vorbisid[30] = { 0x01, 0x76, 0x6f, 0x72, 0x62, 0x69, 0x73,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x44, 0xac, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0xf4, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb8, 0x01
};



static GstStaticCaps foobar_caps = GST_STATIC_CAPS ("foo/x-bar");

#define FOOBAR_CAPS (gst_static_caps_get (&foobar_caps))

static void
foobar_typefind (GstTypeFind * tf, gpointer unused)
{
  guint8 *data;

  data = gst_type_find_peek (tf, 0, 10);
  fail_unless (data != NULL);
  fail_unless (memcmp (data, vorbisid, 10) == 0);

  data = gst_type_find_peek (tf, 0, 20);
  fail_unless (data != NULL);
  fail_unless (memcmp (data, vorbisid, 20) == 0);

  data = gst_type_find_peek (tf, 0, 30);
  fail_unless (data != NULL);
  fail_unless (memcmp (data, vorbisid, 30) == 0);

  fail_unless (gst_type_find_peek (tf, 0, 31) == NULL);
  fail_unless (gst_type_find_peek (tf, 1, 30) == NULL);
  fail_unless (gst_type_find_peek (tf, 25, 6) == NULL);

  data = gst_type_find_peek (tf, 1, 29);
  fail_unless (data != NULL);
  fail_unless (memcmp (data, vorbisid + 1, 29) == 0);

  data = gst_type_find_peek (tf, 25, 4);
  fail_unless (data != NULL);
  fail_unless (memcmp (data, vorbisid + 25, 4) == 0);

  fail_unless (gst_type_find_peek (tf, -1, 29) == NULL);
  fail_unless (gst_type_find_peek (tf, -1, 1) == NULL);
  fail_unless (gst_type_find_peek (tf, -1, 0) == NULL);

  gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, FOOBAR_CAPS);
}


//char* xmlfile = "typefindhelper";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

/* make sure the entire data in the buffer is available for peeking */
void test_buffer_range()
{
  static gchar *foobar_exts[] = { "foobar", NULL };

  GstStructure *s;
  GstBuffer *buf;
  GstCaps *caps;
	
	xmlfile = "test_buffer_range";
  std_log(LOG_FILENAME_LINE, "Test Started test_buffer_range");
  
  fail_unless (gst_type_find_register (NULL, "foo/x-bar",
          GST_RANK_PRIMARY + 50, foobar_typefind, foobar_exts, FOOBAR_CAPS,
          NULL, NULL));

  buf = gst_buffer_new ();
  fail_unless (buf != NULL);
  GST_BUFFER_DATA (buf) = (guint8 *) vorbisid;
  GST_BUFFER_SIZE (buf) = 30;
  GST_BUFFER_FLAG_SET (buf, GST_BUFFER_FLAG_READONLY);

  caps = gst_type_find_helper_for_buffer (NULL, buf, NULL);
  fail_unless (caps != NULL);
  fail_unless (GST_CAPS_IS_SIMPLE (caps));
  fail_unless (gst_caps_is_fixed (caps));

  s = gst_caps_get_structure (caps, 0);
  fail_unless (s != NULL);
  fail_unless (gst_structure_has_name (s, "foo/x-bar"));

  gst_caps_unref (caps);
  gst_buffer_unref (buf);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



void
gst_typefindhelper_suite (void)
{
test_buffer_range();
}

void (*fn[]) (void) = {
test_buffer_range
};

char *args[] = {
"test_buffer_range"
};

GST_CHECK_MAIN (gst_typefindhelper);
#if 0
int main(int argc,char** argv)
{
		int i;
		gst_check_init(&argc,&argv);
		for(i=0;i<1;i++)
			fn[i]();
			
} 
#endif