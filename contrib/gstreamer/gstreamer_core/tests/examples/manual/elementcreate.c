
/*** block  from ../../../docs/manual/basics-elements.xml ***/
#include <gst/gst.h>
#include <stdio.h>
int
main (int   argc,
      char *argv[])
{
  GstElement *element;

  /* init GStreamer */
  gst_init (&argc, &argv);

  /* create element */
  element = gst_element_factory_make ("filesrc", "source");
  if (!element) {
    g_print ("Failed to create element of type 'fakesrc'\n");
    return -1;
  }

  gst_object_unref (GST_OBJECT (element));
  printf("Success");
  return 0;
}
