/* GStreamer
 * Copyright (C) 2005 Andy Wingo <wingo@pobox.com>
 *
 * simple_launch_lines.c: Unit test for simple pipelines
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


#define LOG_FILE "c:\\logs\\simple-launch-lines_logs.txt" 

#include <gst/gst_global.h>
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
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers = NULL;
#endif


#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical = FALSE;
#endif
//gboolean _gst_check_raised_warning = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_warning,gstcheck,gboolean)
#define _gst_check_raised_warning (*GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_warning = FALSE;
#endif
//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log = FALSE;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running = FALSE;
#endif

#include <gst/check/gstcheck.h>

#ifndef GST_DISABLE_PARSE

static GstElement *
setup_pipeline (const gchar * pipe_descr)
{
  GstElement *pipeline;

  GST_LOG ("pipeline: %s", pipe_descr);
  pipeline = gst_parse_launch (pipe_descr, NULL);
  g_return_val_if_fail (GST_IS_PIPELINE (pipeline), NULL);
  return pipeline;
}

/* 
 * run_pipeline:
 * @pipe: the pipeline to run
 * @desc: the description for use in messages
 * @events: is a mask of expected events
 * @tevent: is the expected terminal event.
 *
 * the poll call will time out after half a second.
 */
static void
run_pipeline (GstElement * pipe, const gchar * descr,
    GstMessageType events, GstMessageType tevent)
{
  GstBus *bus;
  GstMessage *message;
  GstMessageType revent;
  GstStateChangeReturn ret;

  g_assert (pipe);
  bus = gst_element_get_bus (pipe);
  g_assert (bus);

  fail_if (gst_element_set_state (pipe, GST_STATE_PLAYING) ==
      GST_STATE_CHANGE_FAILURE, "Could not set pipeline %s to playing", descr);
  ret = gst_element_get_state (pipe, NULL, NULL, 10 * GST_SECOND);
  if (ret == GST_STATE_CHANGE_ASYNC) {
    g_critical ("Pipeline '%s' failed to go to PLAYING fast enough", descr);
    goto done;
  } else if (ret != GST_STATE_CHANGE_SUCCESS) {
    g_critical ("Pipeline '%s' failed to go into PLAYING state", descr);
    goto done;
  }

  while (1) {
    message = gst_bus_poll (bus, GST_MESSAGE_ANY, GST_SECOND / 2);


    /* always have to pop the message before getting back into poll */
    if (message) {
      revent = GST_MESSAGE_TYPE (message);
      gst_message_unref (message);
    } else {
      revent = GST_MESSAGE_UNKNOWN;
    }

    if (revent == tevent) {
      break;
    } else if (revent == GST_MESSAGE_UNKNOWN) {
      g_critical ("Unexpected timeout in gst_bus_poll, looking for %d: %s",
          tevent, descr);
      break;
    } else if (revent & events) {
      continue;
    }
    g_critical
        ("Unexpected message received of type %d, '%s', looking for %d: %s",
        revent, gst_message_type_get_name (revent), tevent, descr);
  }

done:
  fail_if (gst_element_set_state (pipe, GST_STATE_NULL) ==
      GST_STATE_CHANGE_FAILURE, "Could not set pipeline %s to NULL", descr);
  gst_element_get_state (pipe, NULL, NULL, GST_CLOCK_TIME_NONE);
  gst_object_unref (pipe);

  gst_bus_set_flushing (bus, TRUE);
  gst_object_unref (bus);
}

void test_element_negotiation()
{
  gchar *s;
  GstElement *pipeline, *fakesrc, *fakesink, *audioconv, *srccapelement, *sinkcapelement;
  GstCaps *srccap, *sinkcap;
  
  std_log(LOG_FILENAME_LINE, "Test Started test_element_negotiation");


  /* Ensures that filtering buffers with unknown caps down to fixed-caps 
   * will apply those caps to the buffers.
   * see http://bugzilla.gnome.org/show_bug.cgi?id=315126 */
  s = "fakesrc num-buffers=2 ! "
      "audio/x-raw-int,width=16,depth=16,rate=22050,channels=1,"
      "signed=(boolean)true,endianness=1234 ! "
      "audioconvert ! audio/x-raw-int,width=16,depth=16,rate=22050,channels=1 "
      "! fakesink";
  //change for parse
 
  //srccap = gst_caps_from_string (srcstr);
  srccap = gst_caps_new_simple ("audio/x-raw-int",
               "width", G_TYPE_INT, 16,
               "depth", G_TYPE_INT, 16,
               "signed",G_TYPE_BOOLEAN, TRUE,
               "endianness",G_TYPE_INT, G_BYTE_ORDER,
               "rate", G_TYPE_INT,  22050,
               "channels", G_TYPE_INT, 1, NULL);
  //sinkcap = gst_caps_from_string (sinkstr);
  sinkcap = gst_caps_new_simple ("audio/x-raw-int",
                 "width", G_TYPE_INT, 16,
                 "depth", G_TYPE_INT, 16,
                 "signed",G_TYPE_BOOLEAN, TRUE,
                 "endianness",G_TYPE_INT, G_BYTE_ORDER,
                 "rate", G_TYPE_INT,  22050,
                 "channels", G_TYPE_INT, 1, NULL);
  
  pipeline = gst_pipeline_new ("audio-player");
  fakesrc = gst_element_factory_make ("fakesrc", NULL);
  g_object_set (G_OBJECT (fakesrc), "num-buffers", 2, NULL);
  audioconv = gst_element_factory_make ("audioconvert", NULL);
  fakesink = gst_element_factory_make ("fakesink", NULL);
   
  gst_bin_add_many (GST_BIN (pipeline), fakesrc, audioconv, fakesink,  NULL);
  
  gst_element_link_filtered(fakesrc, audioconv, srccap);
  gst_element_link_filtered(audioconv, fakesink, sinkcap);
    
 // gst_element_link_many (fakesrc, audioconv, fakesink, NULL);
  
  run_pipeline (pipeline, s,
      GST_MESSAGE_ANY & ~(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING),
      GST_MESSAGE_UNKNOWN);

#ifdef HAVE_LIBVISUAL
  s = "audiotestsrc num-buffers=30 ! tee name=t ! alsasink t. ! audioconvert ! "
      "libvisual_lv_scope ! ffmpegcolorspace ! xvimagesink";
  run_pipeline (setup_pipeline (s), s,
      GST_MESSAGE_ANY & ~(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING),
      GST_MESSAGE_UNKNOWN);
#endif
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);

}



void test_basetransform_based()
{
  /* Each of these tests is to check whether various basetransform based 
   * elements can select output caps when not allowed to do passthrough 
   * and going to a generic sink such as fakesink or filesink */
  const gchar *s;
  std_log(LOG_FILENAME_LINE, "Test Started test_basetransform_based");


  /* Check that videoscale can pick a height given only a width */
  s = "videotestsrc num-buffers=2 ! "
      "video/x-raw-yuv,format=(fourcc)I420,width=320,height=240 ! "
      "videoscale ! video/x-raw-yuv,width=640 ! fakesink";
  run_pipeline (setup_pipeline (s), s,
      GST_MESSAGE_ANY & ~(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING),
      GST_MESSAGE_UNKNOWN);

  /* Test that ffmpegcolorspace can pick an output format that isn't
   * passthrough without completely specified output caps */
  s = "videotestsrc num-buffers=2 ! "
      "video/x-raw-yuv,format=(fourcc)I420,width=320,height=240 ! "
      "ffmpegcolorspace ! video/x-raw-rgb ! fakesink";
  run_pipeline (setup_pipeline (s), s,
      GST_MESSAGE_ANY & ~(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING),
      GST_MESSAGE_UNKNOWN);

  /* Check that audioresample can pick a samplerate to use from a
   * range that doesn't include the input */
  s = "audiotestsrc num-buffers=2 ! "
      "audio/x-raw-int,width=16,depth=16,rate=8000 ! "
      "audioresample ! audio/x-raw-int,rate=[16000,48000] ! fakesink";
  run_pipeline (setup_pipeline (s), s,
      GST_MESSAGE_ANY & ~(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING),
      GST_MESSAGE_UNKNOWN);

  /* Check that audioconvert can pick a depth to use, given a width */
  s = "audiotestsrc num-buffers=30 ! audio/x-raw-int,width=16,depth=16 ! "
      "audioconvert ! " "audio/x-raw-int,width=32 ! fakesink";
  run_pipeline (setup_pipeline (s), s,
      GST_MESSAGE_ANY & ~(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING),
      GST_MESSAGE_UNKNOWN);

  /* Check that videoscale doesn't claim to be able to transform input in
   * formats it can't handle for a given scaling method; ffmpegcolorspace
   * should then make sure a format that can be handled is chosen (4-tap
   * scaling is not implemented for RGB and packed yuv currently) */
  s = "videotestsrc num-buffers=2 ! video/x-raw-rgb,width=64,height=64 ! "
      "ffmpegcolorspace ! videoscale method=4-tap ! ffmpegcolorspace ! "
      "video/x-raw-rgb,width=32,height=32,framerate=(fraction)30/1,"
      "pixel-aspect-ratio=(fraction)1/1,bpp=(int)24,depth=(int)24,"
      "red_mask=(int)16711680,green_mask=(int)65280,blue_mask=(int)255,"
      "endianness=(int)4321 ! fakesink";
  run_pipeline (setup_pipeline (s), s,
      GST_MESSAGE_ANY & ~(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING),
      GST_MESSAGE_UNKNOWN);
  s = "videotestsrc num-buffers=2 ! video/x-raw-yuv,format=(fourcc)AYUV,"
      "width=64,height=64 ! ffmpegcolorspace ! videoscale method=4-tap ! "
      "ffmpegcolorspace ! video/x-raw-yuv,format=(fourcc)AYUV,width=32,"
      "height=32 ! fakesink";
  run_pipeline (setup_pipeline (s), s,
      GST_MESSAGE_ANY & ~(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING),
      GST_MESSAGE_UNKNOWN);
  /* make sure nothing funny happens in passthrough mode (we don't check that
   * passthrough mode is chosen though) */
  s = "videotestsrc num-buffers=2 ! video/x-raw-yuv,format=(fourcc)I420,"
      "width=64,height=64 ! ffmpegcolorspace ! videoscale method=4-tap ! "
      "ffmpegcolorspace ! video/x-raw-yuv,format=(fourcc)I420,width=32,"
      "height=32 ! fakesink";
  run_pipeline (setup_pipeline (s), s,
      GST_MESSAGE_ANY & ~(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING),
      GST_MESSAGE_UNKNOWN);

      std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);

}



#endif /* #ifndef GST_DISABLE_PARSE */

void (*fn[]) (void) = {
test_element_negotiation,
test_basetransform_based      // Dropping test case for Symbian because it includes video operations
};

char *args[] = {
"test_element_negotiation",
"test_basetransform_based"                // Dropping test case for Symbian because it includes video operations
};

GST_CHECK_MAIN (simple-launch-lines);
