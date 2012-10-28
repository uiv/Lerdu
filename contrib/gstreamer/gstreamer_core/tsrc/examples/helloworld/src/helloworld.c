
#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/gstelement.h>

#define LOG_FILE "c:\\logs\\ helloworld_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}
GstElement *pipeline, *source, *wavparse,*sink,*conv,*resample,*decoder;
GstBus *bus;
GMainLoop *loop;

static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        gst_element_set_state (pipeline, GST_STATE_NULL);
        g_main_loop_quit(loop);
        gst_object_unref (GST_OBJECT (pipeline));
        std_log(LOG_FILENAME_LINE, "Test Successful");
        create_xml(0); 
      break;
    case GST_MESSAGE_ERROR: {
      gchar *debug;
      GError *err;
      gst_message_parse_error (msg, &err, &debug);
      g_free (debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      std_log(LOG_FILENAME_LINE, "Test Failed");
      create_xml(1); 
      break;
    }
    default:
      break;
  }

  return TRUE;
}

static void 
new_pad_cb (GstElement *wavparse, GstPad *new_pad, gpointer pipeline)
 {
   
   gst_element_set_state (pipeline, GST_STATE_PAUSED);
   
   if (!gst_element_link (wavparse, sink))
      g_error ("link(wavparse, sink) failed!\n");
    
   gst_element_set_state (pipeline, GST_STATE_PLAYING);
 }

int main (int argc, char *argv[])
{
    
    
    xmlfile = " helloworld_logs";
    std_log(LOG_FILENAME_LINE, "Test Started  helloworld");
 
    if (argc != 2) {
      g_print ("usage: %s <wav file>\n", argv[0]);
      std_log(LOG_FILENAME_LINE, "Test Failed");
      create_xml(1); 
      exit (-1);
    }
    
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);
    
    /* create elements */
    pipeline = gst_pipeline_new ("audio-player");
    source = gst_element_factory_make ("filesrc", "file-source");
    decoder = gst_element_factory_make ("wavparse", "wavparse-decoder");
    sink = gst_element_factory_make ("devsoundsink", "sink");
        if (!pipeline || !source || !decoder) {
        g_print ("One element could not be created\n");
        return -1;
        }
    /* set filename property on the file source. Also add a message  handler. */
    g_object_set (G_OBJECT (source), "location", argv[1], NULL);
            /* put all elements in a bin */
    gst_bin_add_many (GST_BIN (pipeline),source, decoder,sink, NULL);
            /* link together - note that we cannot link the parser and  decoder yet, because the parser uses dynamic pads. For that, we set a pad-added signal handler. */
    gst_element_link (source, decoder);
    gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (pipeline)), bus_call, loop);
    g_signal_connect (decoder, "pad-added", G_CALLBACK (new_pad_cb),pipeline);
            /* Now set to playing and iterate. */
    g_print ("Setting to PLAYING\n");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_print ("Running\n");
    g_main_loop_run (loop);
            /* clean up nicely */
    g_print ("Returned, stopping playback\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);
    g_print ("Deleting pipeline\n");
    gst_object_unref (GST_OBJECT (pipeline));
    
    g_print ("completed palying audio\n");
    std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0); 
    return 0;
}

