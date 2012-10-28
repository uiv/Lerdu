
/*** block  from ../../../docs/manual/basics-elements.xml ***/

#include <gst/gst_global.h>
#include <gst/gst.h>
#include <gst/check/gstcheck.h>

#define LOG_FILE "c:\\logs\\elementfactory_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

//char* xmlfile = "gstvalue";

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
  GstElementFactory *factory;
  xmlfile = "elementfactory";
  std_log(LOG_FILENAME_LINE, "Test Started elementfactory");

  /* init GStreamer */
  gst_init (&argc, &argv);

  /* get factory */
  factory = gst_element_factory_find ("fakesrc");
  if (!factory) {
    g_print ("You don't have the 'fakesrc' element installed!\n");
    return -1;
  }

  /* display information */
  g_print ("The '%s' element is a member of the category %s.\n"
           "Description: %s\n",
           gst_plugin_feature_get_name (GST_PLUGIN_FEATURE (factory)),
           gst_element_factory_get_klass (factory),
           gst_element_factory_get_description (factory));
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);         
  
  return 0;
}
