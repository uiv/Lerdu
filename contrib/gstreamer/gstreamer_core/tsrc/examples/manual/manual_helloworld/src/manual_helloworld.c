
/*** block  from ../../../docs/manual/basics-helloworld.xml ***/
#include <gst/gst_global.h>
#include <gst/gst.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>             /* for close() */
#endif

#include "libgstreamer_wsd_macros.h"

#ifndef EMULATOR
int failed=0;
FILE* fp_std_log_t=NULL;
#define assert_failed *(get_assert_failed())
#define fp_std_log *(get_fp_std_log())
#endif

#define LOG_FILE "c:\\logs\\helloworld_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

//char* xmlfile = "helloworld";


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

/*
 * Global objects are usually a bad thing. For the purpose of this
 * example, we will use them, however.
 */

GstElement *pipeline, *source, *parser, *decoder, *conv, *sink;

static gboolean
bus_call (GstBus     *bus,
	  GstMessage *msg,
	  gpointer    data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      g_print ("End-of-stream\n");
      g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_ERROR: {
      gchar *debug;
      GError *err;

      gst_message_parse_error (msg, &err, &debug);
      g_free (debug);

      g_print ("Error: %s\n", err->message);
      g_error_free (err);

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}

static void
new_pad (GstElement *element,
	 GstPad     *pad,
	 gpointer    data)
{
  GstPad *sinkpad;
  /* We can now link this pad with the audio decoder */
  g_print ("Dynamic pad created, linking parser/decoder\n");
  gst_element_set_state (pipeline, GST_STATE_PAUSED);
  sink = gst_element_factory_make ("devsoundsink", "devoutput");
  gst_bin_add_many (GST_BIN (pipeline),sink,  NULL);
  // Since gst_element_get_pad is deprecated. Replace with gst_element_get_request_pad
  //sinkpad = gst_element_get_pad (decoder, "sink");
  //sinkpad = gst_element_get_request_pad (decoder, "sink");
  //gst_pad_link (pad, sinkpad);
  gst_element_link (element, sink);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
}

int
main (int   argc,
      char *argv[])
{
  GMainLoop *loop;
  GstBus *bus;
	xmlfile = "helloworld";
  std_log(LOG_FILENAME_LINE, "Test Started helloworld");

  /* initialize GStreamer pt 1*/ 
  gst_init (&argc, &argv);
  loop = g_main_loop_new (NULL, FALSE);
 
  /* check input arguments */
 
  if (argc != 2) {
    g_print ("Usage: %s <wave file filename>\n", argv[0]);
    std_log(LOG_FILENAME_LINE, "Test Failed wave file as an argument need to be passed");
    create_xml(1); 
    exit (-1);
  }
  

  /* create elements  pt2*/
   pipeline = gst_pipeline_new ("audio-player");
  source = gst_element_factory_make ("filesrc", "file-source");
  
  // Path hardcoded need to change
  
 // g_object_set(source,"location",argv[1],NULL);
  
  g_object_set(source,"location",argv[1],NULL);
  
  parser = gst_element_factory_make ("wavparse", "waveparser");
 
 
  
  if (!pipeline || !source || !parser ) {
    g_print ("One element could not be created\n");
    return -1;
  }

  /* set filename property on the file source. Also add a message
   * handler.  pt3*/
  

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  /* put all elements in a bin pt4*/
  gst_bin_add_many (GST_BIN (pipeline),
		    source, parser,  NULL);

  /* link together - note that we cannot link the parser and
   * decoder yet, becuse the parser uses dynamic pads. For that,
   * we set a pad-added signal handler. pt5*/
  gst_element_link (source, parser);
 
  g_signal_connect (parser, "pad-added", G_CALLBACK (new_pad), NULL);

  /* Now set to playing and iterate. pt6*/
  g_print ("Setting to PLAYING\n");
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  g_print ("Running\n");
  g_main_loop_run (loop);

  /* clean up nicely pt7*/
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);
  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));

  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  return 0;
}
