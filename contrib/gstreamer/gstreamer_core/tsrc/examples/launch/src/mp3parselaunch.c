
#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>

#define LOG_FILE "c:\\logs\\mp3parselaunch_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__



void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

static void
event_loop (GstElement * pipe)
{
  GstBus *bus;
  GstMessage *message = NULL;

  bus = gst_element_get_bus (GST_ELEMENT (pipe));

  while (TRUE) {
    message = gst_bus_poll (bus, GST_MESSAGE_ANY, -1);

    g_assert (message != NULL);

    switch (message->type) {
      case GST_MESSAGE_EOS:
        gst_message_unref (message);
        std_log(LOG_FILENAME_LINE, "Test Successful");
        create_xml(0); 
        return;
      case GST_MESSAGE_WARNING:
      case GST_MESSAGE_ERROR:{
        GError *gerror;
        gchar *debug;

        gst_message_parse_error (message, &gerror, &debug);
        gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
        gst_message_unref (message);
        g_error_free (gerror);
        g_free (debug);
        std_log(LOG_FILENAME_LINE, "Test Failed");
        create_xml(1); 
        return;
      }
      default:
        gst_message_unref (message);
        break;
    }
  }
}

int
main (int argc, char *argv[])
{
  GstElement *bin;
  GstElement *filesrc, *mad, *devsink, *conv, *resample;
  GError *error = NULL;

	xmlfile = "mp3parselaunch_logs";
  std_log(LOG_FILENAME_LINE, "Test Started mp3parselaunch");
  gst_init (NULL, NULL);

  if (argc != 2) {
    g_print ("usage: %s <mp3 file>\n", argv[0]);
    std_log(LOG_FILENAME_LINE, "Test Failed");
    create_xml(1); 
    exit (-1);
  }


  bin = (GstElement *)
      gst_parse_launch ("filesrc name=my_filesrc ! mad ! devsoundsink", &error);
  if (!bin) {
    fprintf (stderr, "Parse error: %s", error->message);
    exit (-1);
  }

 /* filesrc = gst_element_factory_make ("filesrc", "my_filesrc");
    mad = gst_element_factory_make ("mad", NULL);
    devsink = gst_element_factory_make ("devsoundsink", NULL);
    
    conv = gst_element_factory_make ("audioconvert", "audioconvert");
    resample = gst_element_factory_make ("audioresample", "audioresample");
    
    g_object_set (G_OBJECT (filesrc), "location", argv[1], NULL);
    
    bin = gst_pipeline_new ("pipeline");
      
    gst_bin_add_many (GST_BIN (bin), filesrc, mad, conv, resample, devsink,  NULL);
    gst_element_link_many (filesrc, mad, conv, resample, devsink, NULL);*/

  filesrc = gst_bin_get_by_name (GST_BIN (bin), "my_filesrc");
  g_object_set (G_OBJECT (filesrc), "location", argv[1], NULL);

  /* start playing */
  gst_element_set_state (bin, GST_STATE_PLAYING);

  /* Run event loop listening for bus messages until EOS or ERROR */
  event_loop (bin);

  /* stop the bin */
  gst_element_set_state (bin, GST_STATE_NULL);

  //std_log(LOG_FILENAME_LINE, "Test Successful");
  //create_xml(0); 

  exit (0);
}
