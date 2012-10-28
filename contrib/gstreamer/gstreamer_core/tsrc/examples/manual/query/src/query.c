
/*** block a  from ../../../docs/manual/advanced-position.xml ***/

#include <gst/gst_global.h>
#include <gst/gst.h>
#include <glib.h>
#include <gst/check/gstcheck.h>

#define LOG_FILE "c:\\logs\\query_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

//char* xmlfile = "gstvalue";
GstElement *pipeline;

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

/*** block b  from ../../../docs/manual/advanced-position.xml ***/
static void
my_bus_message_cb (GstBus     *bus,
                   GstMessage *message,
                   gpointer    data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      g_free (debug);

      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_main_loop_quit (loop);
      break;
    default:
      break;
  }
}

/*** block c  from ../../../docs/manual/advanced-position.xml ***/
static gboolean
cb_print_position (GstElement *pipeline)
{
  GstFormat fmt = GST_FORMAT_TIME;
  gint64 pos, len;

  if (gst_element_query_position (pipeline, &fmt, &pos)
    && gst_element_query_duration (pipeline, &fmt, &len)) {
    g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
	     GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
  }

  /* call me again */
  return TRUE;
}


static void
new_pad (GstElement *element,
     GstPad     *pad,
     gpointer    data)
{
  GstPad *sinkpad;
  GstElement *convert, *resample, *devsink;
  /* We can now link this pad with the audio decoder */
  g_print ("Dynamic pad created, linking parser/decoder\n");
  gst_element_set_state (pipeline, GST_STATE_PAUSED);
  //sink = gst_element_factory_make ("devsoundsink", "devoutput");
 // gst_bin_add_many (GST_BIN (pipeline),sink,  NULL);
  
  convert = gst_element_factory_make ("audioconvert", "audio-convert");
   resample= gst_element_factory_make ("audioresample", "audio-resample");
   devsink =  gst_element_factory_make ("devsoundsink", "devsoundsink");
   
   gst_bin_add_many (GST_BIN (pipeline),convert, resample, devsink,  NULL);
   
  
  // Since gst_element_get_pad is deprecated. Replace with gst_element_get_request_pad
  //sinkpad = gst_element_get_pad (decoder, "sink");
  //sinkpad = gst_element_get_request_pad (decoder, "sink");
  //gst_pad_link (pad, sinkpad);
   gst_element_link_many (element, convert, resample, devsink, NULL);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
}


gint
main (gint   argc,
      gchar *argv[])
{
  //GstElement *pipeline;
  GstElement *source, *parser, *convert, *resample, *demux;
/*** block d  from ../../../docs/manual/advanced-position.xml ***/
  GstStateChangeReturn ret;
  GMainLoop *loop;
  GError *err = NULL;
  GstBus *bus;
  gchar *l;
  
  xmlfile = "query";
  std_log(LOG_FILENAME_LINE, "Test Started query");


  /* init */
  gst_init (&argc, &argv);

  /* check input arguments */
  if (argc != 2) {
    g_print ("Usage: %s <filename>\n", argv[0]);
    std_log(LOG_FILENAME_LINE, "Test Failed wave file need to be passed as an arguments");
    create_xml(1); 
    exit (-1);
  }

  loop = g_main_loop_new (NULL, FALSE);

  /* build pipeline, the easy way */
/*  l = g_strdup_printf ("filesrc location=\"%s\" ",
		       "c:\\data\\gstreamer\\khuda1.wav");*/
  //g_print("file location =%s\n",argv[1]);
  l = g_strdup_printf ("filesrc location=\"%s\" ! wavparse ! audioconvert ! audioresample ! devsoundsink",argv[1]);
    
  //pipeline = gst_parse_launch (l, &err);
  
  pipeline = gst_pipeline_new ("audio-player");
    source = gst_element_factory_make ("filesrc", "file-source");
    g_object_set (G_OBJECT (source), "location", argv[1], NULL);
    parser = gst_element_factory_make ("wavparse", "wavparse");
   
    gst_bin_add_many (GST_BIN (pipeline), source, parser,  NULL);
    gst_element_link_many (source, parser, NULL);
    
    g_signal_connect (parser, "pad-added", G_CALLBACK (new_pad), NULL);
  
  
  /* create elements  pt2*/
   //pipeline = gst_pipeline_new ("audio-player");
   //source = gst_element_factory_make ("filesrc", "file-source");
   //convert = gst_element_factory_make ("audioconvert", "audio-convert");
   //resample= gst_element_factory_make ("audioresample", "audio-resample");
   //demux= gst_element_factory_make ("oggdemux", "demuxer");
       
   // Path hardcoded need to change
   /*g_object_set(source,"location","c:\\data\\gstreamer\\khuda1.wav",NULL);
   
   if (!pipeline) {
     g_print ("Pipeline element could not be created\n");
     getchar();
     return -1;
   }
   if (!source) {
        g_print ("Source element could not be created\n");
        getchar();
        return -1;
      }
  if (!demux ) {
        g_print ("Resample element could not be created\n");
        getchar();
        return -1;
      }*/

  if (pipeline == NULL || err != NULL) {
  //if (pipeline == NULL) {
    g_printerr ("Cannot build pipeline\n");
    g_error_free (err);
    g_free (l);
    if (pipeline)
      gst_object_unref (pipeline);
    
    return -1;
  }
  g_free (l);

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_signal_watch (bus);
  g_signal_connect (bus, "message", G_CALLBACK (my_bus_message_cb), loop);
  gst_object_unref (bus);

  /* put all elements in a bin pt4*/
  //gst_bin_add_many (GST_BIN (pipeline),source,NULL);
  //gst_element_link_many(source,NULL);
  /* play */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE){

  g_error ("Failed to set pipeline to PLAYING.\n");
   
  }
/*** block e  from ../../../docs/manual/advanced-position.xml ***/
  /* run pipeline */
  g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);
  g_main_loop_run (loop);

/*** block f  from ../../../docs/manual/advanced-position.xml ***/
  /* clean up */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (pipeline));
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 

  return 0;

/*** block g  from ../../../docs/manual/advanced-position.xml ***/
}
