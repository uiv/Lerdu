#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif


#include <gst/gst_global.h>
#include <gst/gst.h>
#include <stdlib.h>
#include <string.h>

#define TEST_RUNTIME 120.0      /* how long to run the test, in seconds */


#include <gst/check/gstcheck.h>


#define LOG_FILE "c:\\logs\\stressplaybin_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


void play_file (const gchar * uri)
{
  GstStateChangeReturn sret;
  GstMessage *msg;
  GstElement *play;
  guint wait_nanosecs;
  GstElement  *fakesink1 ;
  GstBus *bus;

  play = gst_element_factory_make ("playbin", "playbin");
  fakesink1 = gst_element_factory_make ("fakesink", NULL);
  fail_unless (fakesink1 != NULL, "Failed to create fakesink element #1");

  /* make them behave like normal sinks, even if not needed for the test */
  g_object_set (fakesink1, "sync", TRUE, NULL);
  g_object_set (play, "audio-sink", fakesink1, NULL);
  
  g_object_set (play, "uri", uri, NULL);
  sret = gst_element_set_state (play, GST_STATE_PLAYING);
  if (sret != GST_STATE_CHANGE_ASYNC) {
    g_printerr ("ERROR: state change failed, sret=%d\n", sret);
    goto next;
  }

  wait_nanosecs = g_random_int_range (0, GST_SECOND / 10);
  bus = gst_element_get_bus (play);
  msg = gst_bus_poll (/*GST_ELEMENT_BUS (play)*/ bus , GST_MESSAGE_ERROR | GST_MESSAGE_EOS, -1);
  if (msg) {
    g_printerr ("Got %s messge\n", GST_MESSAGE_TYPE_NAME (msg));
    gst_message_unref (msg);
    goto next;
  }

  /* on to the next one */
  g_print (".");

next:
  gst_element_set_state (play, GST_STATE_NULL);
}

void check_arg (GPtrArray * files, const gchar * arg)
{
  GDir *dir;

  if ((dir = g_dir_open (arg, 0, NULL))) {
    const gchar *entry;

    while ((entry = g_dir_read_name (dir))) {
      gchar *path;

      path = g_strconcat (arg, G_DIR_SEPARATOR_S, entry, NULL);
      check_arg (files, path);
      g_free (path);
    }

    g_dir_close (dir);
    return;
  } else if (g_file_test (arg, G_FILE_TEST_EXISTS)) {
    /* hack: technically an URI is not just file:// + path, but it'll do here */
    g_ptr_array_add (files, g_strdup_printf ("file://%s", arg));
  }
}


void test_stress_playbin(int argc,char** argv)
{
  GPtrArray *files;
  gchar **args = NULL;
  guint num, i;
  GError *err = NULL;
  GOptionContext *ctx;
 
  GTimer *timer;
  //char* p = argv[1];
  
  GOptionEntry options[] = {
    {G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, NULL , NULL},
    {NULL}
  };
  
  xmlfile = "stress-playbin";
  std_log(LOG_FILENAME_LINE, "Test Started stress-playbin");

  
  args = argv;
  
  options[0].arg_data = (void*)args;

  if (!g_thread_supported ())
    g_thread_init (NULL);

  ctx = g_option_context_new ("FILES OR DIRECTORIES WITH AUDIO FILES");
  g_option_context_add_main_entries (ctx, options, NULL);
  g_option_context_add_group (ctx, gst_init_get_option_group ());
  if (!g_option_context_parse (ctx,  NULL, NULL, &err)) {
    g_print ("Error initializing: %s\n", GST_STR_NULL (err->message));
    exit (1);
  }
  g_option_context_free (ctx);

  if (args == NULL || *args == NULL) {
    g_print ("Please provide one or more directories with audio files\n\n");
    return;
  }

  files = g_ptr_array_new ();

  num = g_strv_length (args);
// Skip first argument which is name of the test case exe
  for (i = 1; i < num; ++i) {
    if (g_path_is_absolute (args[i])) {
      check_arg (files, args[i]);
    } else {
      g_warning ("Argument '%s' is not an absolute file path", args[i]);
    }
  }

  if (files->len == 0) {
    g_print ("Did not find any files\n\n");
    exit(1);
  }

  timer = g_timer_new ();

  while (g_timer_elapsed (timer, NULL) < TEST_RUNTIME) {
    gint32 idx;

    idx = g_random_int_range (0, files->len);
    play_file ((const gchar *) g_ptr_array_index (files, idx));
  }

  //g_strfreev (args);  //assigned to a constant no need to free
  g_timer_destroy (timer);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);

}
/*	
void (*fn[1]) (void) = {
        test_stress_playbin
};

char *args[] = {
        "test_stress_playbin"
};
*/
//GST_CHECK_MAIN (stress-playbin);
int main(int argc,char** argv)
{
	gst_check_init(&argc,&argv);
	test_stress_playbin(argc,argv);
}
