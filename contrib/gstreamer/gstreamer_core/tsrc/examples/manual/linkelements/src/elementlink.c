
/*** block a  from ../../../docs/manual/basics-elements.xml ***/

#include <gst/gst_global.h>
#include <gst/gst.h>
#include <stdio.h>
#define LOG_FILE "c:\\logs\\linkelements_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

//char* xmlfile = "filesink";


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}
int
main (int   argc,
      char *argv[])
{
  GstElement *pipeline;
  GstElement *source, *filter, *sink;
  xmlfile = "test_elementlink";
  std_log(LOG_FILENAME_LINE, "Test Started element link");
  /* init */
  gst_init (&argc, &argv);

  /* create pipeline */
  pipeline = gst_pipeline_new ("my-pipeline");

  /* create elements */
  source = gst_element_factory_make ("fakesrc", "source");
  filter = gst_element_factory_make ("identity", "filter");
  sink = gst_element_factory_make ("fakesink", "sink");

  /* must add elements to pipeline before linking them */
  gst_bin_add_many (GST_BIN (pipeline), source, filter, sink, NULL);

  /* link */
  if (!gst_element_link_many (source, filter, sink, NULL)) {
    g_warning ("Failed to link elements!");
  }
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);

/*** block b  from ../../../docs/manual/basics-elements.xml ***/
  return 0;

/*** block c  from ../../../docs/manual/basics-elements.xml ***/
}
