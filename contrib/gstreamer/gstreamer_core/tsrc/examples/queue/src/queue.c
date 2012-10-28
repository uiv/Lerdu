
#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>

/* This example uses the queue element to create a buffer between 2 elements.
 * The scheduler automatically uses 2 threads, 1 to feed and another to consume
 * data from the queue buffer
 */

/* Event loop to listen to events posted on the GstBus from the pipeline. Exits
 * on EOS or ERROR events
 */
#define LOG_FILE "c:\\logs\\queue_logs.txt" 
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
  GstElement *filesrc, *audiosink, *decode, *queue;
  GstElement *pipeline;

	xmlfile = "queue_logs";
  std_log(LOG_FILENAME_LINE, "Test Started queue");
  gst_init (&argc, &argv);

  if (argc != 2) {
    g_print ("usage: %s <filename>\n", argv[0]);
    std_log(LOG_FILENAME_LINE, "Test Failed mp3 file need to be passed as an argument");
    create_xml(1); 
    exit (-1);
  }

  /* create a new pipeline to hold the elements */
  pipeline = gst_pipeline_new ("pipeline");
  g_assert (pipeline != NULL);

  /* create a disk reader */
  filesrc = gst_element_factory_make ("filesrc", "disk_source");
  g_assert (filesrc != NULL);
  g_object_set (G_OBJECT (filesrc), "location", argv[1], NULL);

  decode = gst_element_factory_make ("mad", "decode");
  g_assert (decode != NULL);

  queue = gst_element_factory_make ("queue", "queue");
  g_assert (queue != NULL);

  /* and an audio sink */
  audiosink = gst_element_factory_make ("devsoundsink", "play_audio");
  g_assert (audiosink != NULL);

  /* add objects to the main pipeline */
  gst_bin_add_many (GST_BIN (pipeline), filesrc, decode, queue, audiosink,
      NULL);

  gst_element_link_many (filesrc, decode, queue, audiosink, NULL);

  /* start playing */
  gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);

  /* Listen for EOS */
  event_loop (pipeline);

  gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_NULL);

	std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
  exit (0);
}
