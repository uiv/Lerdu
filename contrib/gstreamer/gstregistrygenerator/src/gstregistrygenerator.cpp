#include <gst/gst_global.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>

int
main (int argc, char *argv[])
{
  //This will update registry
  gst_init (&argc, &argv);  
  return 0;
}
