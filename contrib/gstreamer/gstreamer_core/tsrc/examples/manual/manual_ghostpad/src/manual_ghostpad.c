
/*** block a  from ../../../docs/manual/basics-pads.xml ***/

#include <gst/gst_global.h>
#include <gst/gst.h>

#define LOG_FILE "c:\\logs\\ghostpad_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__


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
  GstElement *bin, *sink;
  GstPad *pad;
  
  xmlfile = "ghostpad";
  std_log(LOG_FILENAME_LINE, "Test Started ghostpad");
 
  /* init */
  gst_init (&argc, &argv);
  
  /* create element, add to bin */
  sink = gst_element_factory_make ("fakesink", "sink");
  bin = gst_bin_new ("mybin");
  gst_bin_add (GST_BIN (bin), sink);


  /* add ghostpad */
  pad = gst_element_get_pad (sink, "sink");
  gst_element_add_pad (bin, gst_ghost_pad_new ("sink", pad));
  gst_object_unref (GST_OBJECT (pad));
  
    if(!sink || !bin || !pad)
        {
        std_log(LOG_FILENAME_LINE, "Test Failed");
        create_xml(1);
        return -1;
        }

  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
/*** block b  from ../../../docs/manual/basics-pads.xml ***/
  return 0;

/*** block c  from ../../../docs/manual/basics-pads.xml ***/
}
