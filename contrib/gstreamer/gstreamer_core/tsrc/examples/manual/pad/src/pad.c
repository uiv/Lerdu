
/*** block a  from ../../../docs/manual/basics-pads.xml ***/

#define LOG_FILE "c:\\logs\\pad_logs.txt" 

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


#include <gst/gst.h>
GstElement* pipeline;
GMainLoop *loop;

static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        gst_element_set_state (pipeline, GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (pipeline));
        g_main_loop_quit(loop);
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
cb_new_pad (GstElement *element,
	    GstPad     *pad,
	    gpointer    data)
{
  gchar *name;
  GstElement* sink;
  //name = gst_pad_get_name (pad);
  gst_element_set_state (pipeline, GST_STATE_PAUSED);
  sink = gst_element_factory_make ("devsoundsink", "sink");
  gst_bin_add_many (GST_BIN (pipeline),sink, NULL);
  gst_element_link(element,sink);
  gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);
  /* here, you would setup a new pad link for the newly created pad */

/*** block b  from ../../../docs/manual/basics-pads.xml ***/
}

int 
main (int   argc,
      char *argv[]) 
{
  GstElement *source, *demux, *parser;

  xmlfile = "pad";
  std_log(LOG_FILENAME_LINE, "Test Started pad");

  /* init */
  gst_init (&argc, &argv);
  
  if (argc != 2) {
    g_print ("usage: %s <filename>\n", argv[0]);
    std_log(LOG_FILENAME_LINE, "Test Failed wave file as an argument need to be passed");
    create_xml(1); 
    exit (-1);
  }

  loop = g_main_loop_new (NULL, FALSE);
  /* create elements */
  pipeline = gst_pipeline_new ("my_pipeline");
  source = gst_element_factory_make ("filesrc", "source");
  g_object_set (source, "location", argv[1], NULL);
 //demux = gst_element_factory_make ("oggdemux", "demuxer");
  
  parser = gst_element_factory_make ("wavparse", "waveparser");
  /* you would normally check that the elements were created properly */

  /* put together a pipeline */
 //gst_bin_add_many (GST_BIN (pipeline), source, demux, NULL);
  gst_bin_add_many (GST_BIN (pipeline), source, parser, NULL);
  //gst_element_link_pads (source, "src", demux, "sink");
  
  gst_element_link_many (source,parser,NULL);

  /* listen for newly created pads */
 //g_signal_connect (demux, "pad-added", G_CALLBACK (cb_new_pad), NULL);
  gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (pipeline)), bus_call, loop);
  g_signal_connect (parser, "pad-added", G_CALLBACK (cb_new_pad), NULL);
  /* start the pipeline */
  gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);
  
  g_main_loop_run (loop);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  return 0;

/*** block d  from ../../../docs/manual/basics-pads.xml ***/
}
