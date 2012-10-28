
/*** block  from ../../../docs/manual/basics-elements.xml ***/

#include <gst/gst_global.h>
#include <gst/gst.h>
#include <stdio.h>
#define LOG_FILE "c:\\logs\\filesink_log1.txt"
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
int main (int   argc,
      char *argv[])
{
  GstElement *element;
  xmlfile = "test_elementcreate";
  std_log(LOG_FILENAME_LINE, "Test Started element create");
  /* init GStreamer */
  gst_init (&argc, &argv);

  /* create element */
  element = gst_element_factory_make ("filesrc", "source");
  if (!element) {
   // g_print ("Failed to create element of type 'fakesrc'\n");
    std_log(LOG_FILENAME_LINE, "Failed to create element of type 'fakesrc'");
    getchar();
    return -1;
  }

  gst_object_unref (GST_OBJECT (element));
  //printf("Success");
  //getchar();
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  return 0;
}
