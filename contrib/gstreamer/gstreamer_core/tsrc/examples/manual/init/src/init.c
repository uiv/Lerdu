
/*** block  from ../../../docs/manual/basics-init.xml ***/

#define LOG_FILE "c:\\logs\\init_logs.txt" 

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
#include <stdio.h>
int
main (int   argc,
      char *argv[])
{
  const gchar *nano_str;
  guint major, minor, micro, nano;

  gst_init (&argc, &argv);
  
  xmlfile = "init";
  std_log(LOG_FILENAME_LINE, "Test Started init");
  

  gst_version (&major, &minor, &micro, &nano);

  if (nano == 1)
    nano_str = "(CVS)";
  else if (nano == 2)
    nano_str = "(Prerelease)";
  else
    nano_str = "";

  getchar();
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 

  return 0;
}
