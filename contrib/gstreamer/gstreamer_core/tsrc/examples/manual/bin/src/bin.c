
/*** block a  from ../../../docs/manual/basics-bins.xml ***/

#define LOG_FILE "c:\\logs\\bin_logs.txt" 

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

int
main (int   argc,
      char *argv[])
{
  GstElement *bin, *pipeline, *source, *sink;
  
  xmlfile = "bin";
  std_log(LOG_FILENAME_LINE, "Test Started bin");

  /* init */
  gst_init (&argc, &argv);

  /* create */
  pipeline = gst_pipeline_new ("my_pipeline");
  bin = gst_pipeline_new ("my_bin");
  source = gst_element_factory_make ("fakesrc", "source");
  sink = gst_element_factory_make ("fakesink", "sink");

  /* set up pipeline */
  gst_bin_add_many (GST_BIN (bin), source, sink, NULL);
  gst_bin_add (GST_BIN (pipeline), bin);
  gst_element_link (source, sink);
  getchar();
/*** block b  from ../../../docs/manual/basics-bins.xml ***/
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
  return 0;
  
  

/*** block c  from ../../../docs/manual/basics-bins.xml ***/
}
