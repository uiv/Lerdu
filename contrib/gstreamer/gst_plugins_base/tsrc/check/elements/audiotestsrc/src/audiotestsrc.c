/* GStreamer
 *
 * unit test for audiotestsrc
 *
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
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




#define LOG_FILE "c:\\logs\\audiotestsrc_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running;
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
extern GMutex *mutex=NULL;
#endif

//GCond *start_cond;              /* used to notify main thread of thread startups */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
extern GCond *start_cond=NULL;
#endif

//GCond *sync_cond;               /* used to synchronize all threads and main thread */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
extern GCond *sync_cond=NULL;
#endif


#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(check_mutex,gstcheck,GMutex *)
#define check_mutex (*GET_GSTREAMER_WSD_VAR_NAME(check_mutex,gstcheck,g)())
#else 
extern GMutex *check_mutex = NULL;
#endif
//
//extern GCond *check_cond;
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(check_cond,gstcheck,GCond *)
#define check_cond (*GET_GSTREAMER_WSD_VAR_NAME(check_cond,gstcheck,g)())
#else 
extern GCond *check_cond = NULL;
#endif

#include <gst/check/gstcheck.h>
//#include <gstcheck.h>
#include <glib_global.h>

/* For ease of programming we use globals to keep refs for our floating
 * src and sink pads we create; otherwise we always have to do get_pad,
 * get_peer, and then remove references in every test function */
static GstPad *mysinkpad;


#define CAPS_TEMPLATE_STRING            \
    "audio/x-raw-int, "                 \
    "channels = (int) 1, "              \
    "rate = (int) [ 1,  MAX ], "        \
    "endianness = (int) BYTE_ORDER, "   \
    "width = (int) 16, "                \
    "depth = (int) 16, "                \
    "signed = (bool) TRUE"

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (CAPS_TEMPLATE_STRING)
    );

static GstElement *
setup_audiotestsrc (void)
{
  GstElement *audiotestsrc;

  GST_DEBUG ("setup_audiotestsrc");
  audiotestsrc = gst_check_setup_element ("audiotestsrc");
  mysinkpad = gst_check_setup_sink_pad (audiotestsrc, &sinktemplate, NULL);
  gst_pad_set_active (mysinkpad, TRUE);

  return audiotestsrc;
}

static void
cleanup_audiotestsrc (GstElement * audiotestsrc)
{
  GST_DEBUG ("cleanup_audiotestsrc");

  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;

  gst_pad_set_active (mysinkpad, FALSE);
  gst_check_teardown_sink_pad (audiotestsrc);
  gst_check_teardown_element (audiotestsrc);
}

void test_all_waves()
{
  GstElement *audiotestsrc;
  GObjectClass *oclass;
  GParamSpec *property;
  GEnumValue *values;
  guint j = 0;
  xmlfile = "test_all_waves";
  std_log(LOG_FILENAME_LINE, "Test Started test_all_waves");

  audiotestsrc = setup_audiotestsrc ();
  oclass = G_OBJECT_GET_CLASS (audiotestsrc);
  property = g_object_class_find_property (oclass, "wave");
  
  /*
     Since g_param_types is a internal API to libglib
     the below line is commented out.  
  */
  //fail_unless (G_IS_PARAM_SPEC_ENUM (property));
  
  values = G_ENUM_CLASS (g_type_class_ref (property->value_type))->values;


  while (values[j].value_name) {
    GST_DEBUG_OBJECT (audiotestsrc, "testing wave %s", values[j].value_name);

    fail_unless (gst_element_set_state (audiotestsrc,
            GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
        "could not set to playing");

    g_mutex_lock (check_mutex);
    while (g_list_length (buffers) < 10)
      g_cond_wait (check_cond, check_mutex);
    g_mutex_unlock (check_mutex);

    gst_element_set_state (audiotestsrc, GST_STATE_READY);

    g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
    g_list_free (buffers);
    buffers = NULL;
    ++j;
    
  }

  /* cleanup */
  cleanup_audiotestsrc (audiotestsrc);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


/*
void
audiotestsrc_suite (void)
{
test_all_waves();
}

void (*fn[]) (void) = {
test_all_waves
};

char *args[] = {
"test_all_waves"
};
*/
int main (int argc, char **argv)
{
//gst_init(&argc, &argv);                
gst_check_init (NULL, NULL); 
test_all_waves();

}

