
/*** block  from ../../../docs/manual/basics-elements.xml ***/

#include <gst/gst_global.h>
#include <gst/gst.h>
#include <stdio.h>
#define LOG_FILE "c:\\logs\\getelements_log1.txt"
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
#include <gst/gst.h>

int main (int   argc,
      char *argv[])
{
  GstElement *element;
  gchar *name;
  xmlfile = "test_elementget";
  std_log(LOG_FILENAME_LINE, "Test Started element get");
  /* init GStreamer */
  gst_init (&argc, &argv);

  /* create element */
  element = gst_element_factory_make ("filesrc", "source");

  /* get name */
  g_object_get (G_OBJECT (element), "name", &name, NULL);
//  g_print ("The name of the element is '%s'.\n", name);
  g_free (name);

  gst_object_unref (GST_OBJECT (element));
 // getchar();
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  return 0;
}
