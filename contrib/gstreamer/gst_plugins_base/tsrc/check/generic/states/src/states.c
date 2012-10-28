/* GStreamer
 *
 * unit test for state changes on all elements
 *
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/*
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
*/

#include <gst/gst_global.h>
#include "config.h"
#include <gst/check/gstcheck.h>
#include <unistd.h>
#include <libgstreamer_wsd_solution.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <unistd.h>
//#include <stdio.h>
//#include <sys/select.h>

#define LOG_FILE "c:\\logs\\states_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__
//char* xmlfile = "states";
int fd_new;
void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running;
#endif
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical;
#endif
//gboolean _gst_check_raised_warning = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_warning,gstcheck,gboolean)
#define _gst_check_raised_warning (*GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_warning;
#endif
//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log;
#endif

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers;
#endif


static GList *elements = NULL;

static void
setup (void)
{
  GList *features, *f;
  GList *plugins, *p;
  gchar **ignorelist = NULL;
  const gchar *STATE_IGNORE_ELEMENTS = NULL;
    
  GST_DEBUG("getting elements for package %s", PACKAGE);
   STATE_IGNORE_ELEMENTS = g_getenv ("STATE_IGNORE_ELEMENTS");
  if (STATE_IGNORE_ELEMENTS) {
    GST_DEBUG ("Will ignore element factories: '%s'", STATE_IGNORE_ELEMENTS);
    ignorelist = g_strsplit (STATE_IGNORE_ELEMENTS, " ", 0);
  }

  plugins = gst_registry_get_plugin_list (gst_registry_get_default ());

  for (p = plugins; p; p = p->next) {
    GstPlugin *plugin = p->data;
  if (strcmp (gst_plugin_get_source (plugin), PACKAGE) != 0)
       continue;

    features =
        gst_registry_get_feature_list_by_plugin (gst_registry_get_default (),
        gst_plugin_get_name (plugin));

    for (f = features; f; f = f->next) {
      GstPluginFeature *feature = f->data;
      const gchar *name = gst_plugin_feature_get_name (feature);
      gboolean ignore = FALSE;

      if (!GST_IS_ELEMENT_FACTORY (feature))
        continue;

      if (ignorelist) {
        gchar **s;

        for (s = ignorelist; s && *s; ++s) {
          if (g_str_has_prefix (name, *s)) {
            GST_DEBUG ("ignoring element %s", name);
            ignore = TRUE;
          }
        }
        if (ignore)
          continue;
      }

      GST_DEBUG ("adding element %s", name);
      elements = g_list_prepend (elements, (gpointer) g_strdup (name));
    }
    gst_plugin_feature_list_free (features);
  }
  gst_plugin_list_free (plugins);
  g_strfreev (ignorelist);
 
  
}

 static void
teardown (void)
{
  GList *e;
    
   for (e = elements; e; e = e->next) {
    g_free (e->data);
  }
  g_list_free (elements);
  elements = NULL;
  
}
 
static void create_file()
    {
    fd_new = open("c:\\data\\testdata_states.txt",O_CREAT|O_RDWR , 0666);
    write(fd_new, "hello",5);
    close(fd_new);
    }

static void change_fd(GstElement *element)
    {
    create_file();
    fd_new = open("c:\\data\\testdata_states.txt",O_RDONLY , 0666);
     g_object_set (G_OBJECT (element), "fd", fd_new, NULL);
    }

void test_state_changes_up_and_down_seq()
{
  GstElement *element;
  GList *e;
  
    //xmlfile = "test_state_changes_up_and_down_seq";
   std_log(LOG_FILENAME_LINE, "Test Started test_state_changes_up_and_down_seq");
   
   setup();
   for (e = elements; e; e = e->next) {
    const gchar *name = e->data;

    GST_DEBUG ("testing element %s", name);
    std_log(LOG_FILENAME_LINE, ">> testing element %s", name); 
    element = gst_element_factory_make (name, name);
    fail_if (element == NULL, "Could not make element from factory %s", name);
    TEST_ASSERT_FAIL
    if (GST_IS_PIPELINE (element)) {
      GST_DEBUG ("element %s is a pipeline", name);
    }
    if(!strcmp(name,"fdsrc"))
        {
      change_fd(element);
        }
    std_log(LOG_FILENAME_LINE, ">> setting state"); 
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_READY %s", name);
    gst_element_set_state (element, GST_STATE_READY);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_PAUSED %s", name);
    gst_element_set_state (element, GST_STATE_PAUSED);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_PLAYING %s", name);
    gst_element_set_state (element, GST_STATE_PLAYING);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_PAUSED %s", name);
    gst_element_set_state (element, GST_STATE_PAUSED);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_READY %s", name);
    gst_element_set_state (element, GST_STATE_READY);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_NULL %s", name);
    gst_element_set_state (element, GST_STATE_NULL);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_PAUSED %s", name);
    gst_element_set_state (element, GST_STATE_PAUSED);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_READY %s", name);
    gst_element_set_state (element, GST_STATE_READY);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_PLAYING %s", name);
    gst_element_set_state (element, GST_STATE_PLAYING);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_PAUSED %s", name);
    gst_element_set_state (element, GST_STATE_PAUSED);
    std_log(LOG_FILENAME_LINE, ">> setting state GST_STATE_NULL %s", name);
    gst_element_set_state (element, GST_STATE_NULL);
    if(!strcmp(name,"fdsrc"))
        {
      close(fd_new);
        }
    gst_object_unref (GST_OBJECT (element));
  }
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
    }



void test_state_changes_up_seq()
{
  GstElement *element;
  GList *e;

    //xmlfile = "test_state_changes_up_seq";
 std_log(LOG_FILENAME_LINE, "Test Started test_state_changes_up_seq");
     setup();
   for (e = elements; e; e = e->next) {
    const gchar *name = e->data;

    GST_DEBUG ("testing element %s", name);
    element = gst_element_factory_make (name, name);
    fail_if (element == NULL, "Could not make element from factory %s", name);
    TEST_ASSERT_FAIL

    if (GST_IS_PIPELINE (element)) {
      GST_DEBUG ("element %s is a pipeline", name);
    }
    if(!strcmp(name,"fdsrc"))
        {
      change_fd(element);
        }
    gst_element_set_state (element, GST_STATE_READY);

    gst_element_set_state (element, GST_STATE_PAUSED);
    gst_element_set_state (element, GST_STATE_READY);

    gst_element_set_state (element, GST_STATE_PAUSED);
    gst_element_set_state (element, GST_STATE_PLAYING);
    gst_element_set_state (element, GST_STATE_PAUSED);
    gst_element_set_state (element, GST_STATE_READY);

    gst_element_set_state (element, GST_STATE_NULL);
    if(!strcmp(name,"fdsrc"))
        {
      close(fd_new);
        }
    gst_object_unref (GST_OBJECT (element));
  }
 
    std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
    
}



void test_state_changes_down_seq()
{
  GstElement *element;
  GList *e;
 
    //xmlfile = "test_state_changes_down_seq";
 std_log(LOG_FILENAME_LINE, "test_state_changes_down_seq");
 setup();
  for (e = elements; e; e = e->next) {
    const gchar *name = e->data;

    GST_DEBUG ("testing element %s", name);
    element = gst_element_factory_make (name, name);
    fail_if (element == NULL, "Could not make element from factory %s", name);
    TEST_ASSERT_FAIL

    if (GST_IS_PIPELINE (element)) {
      GST_DEBUG ("element %s is a pipeline", name);
    }

    if(!strcmp(name,"fdsrc"))
        {
      change_fd(element);
        }
    gst_element_set_state (element, GST_STATE_READY);
    gst_element_set_state (element, GST_STATE_PAUSED);
    gst_element_set_state (element, GST_STATE_PLAYING);

    gst_element_set_state (element, GST_STATE_PAUSED);
    gst_element_set_state (element, GST_STATE_PLAYING);

    gst_element_set_state (element, GST_STATE_PAUSED);
    gst_element_set_state (element, GST_STATE_READY);
    gst_element_set_state (element, GST_STATE_PAUSED);
    gst_element_set_state (element, GST_STATE_PLAYING);

    gst_element_set_state (element, GST_STATE_PAUSED);
    gst_element_set_state (element, GST_STATE_READY);
    gst_element_set_state (element, GST_STATE_NULL);
    
    if(!strcmp(name,"fdsrc"))
        {
      close(fd_new);
        }
    gst_object_unref (GST_OBJECT (element));
  }

  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
   
}



void (*fn[]) (void) = {
test_state_changes_up_and_down_seq,
test_state_changes_up_seq,
test_state_changes_down_seq
};

char *args[] = {
"test_state_changes_up_and_down_seq",
"test_state_changes_up_seq",
"test_state_changes_down_seq"
};
GST_CHECK_MAIN (states);
