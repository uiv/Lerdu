/* GStreamer
 *
 * unit test for GstPlugin
 *
 * Copyright 2004 Thomas Vander Stichele <thomas at apestaart dot org>
 * Copyright 2005 David Schleef <ds@schleef.org>
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


#include <gst/gst_global.h>
#include "config.h"


#include <gst/check/gstcheck.h>
#include<gstplugin.h>

#define LOG_FILE "c:\\logs\\gstplugin_logs.txt" 

#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include "libgstreamer_wsd_solution.h" 

#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers = NULL;
#endif

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}



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

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running;
#endif

#ifdef GST_DISABLE_DEPRECATED
void _gst_plugin_register_static (GstPluginDesc * desc);
#endif

/* keep in sync with GST_GNUC_CONSTRUCTOR in gstmacros.h (ideally we'd just
 * do it there, but I don't want to touch that now, and also we really want
 * to deprecate this macro in the long run, I think) */
#if defined (__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4))
#define GST_GNUC_CONSTRUCTOR_DEFINED
#else
#undef GST_GNUC_CONSTRUCTOR_DEFINED
#endif

#ifdef GST_GNUC_CONSTRUCTOR_DEFINED
/* ------------------------------------------------------------------------- */
/* To make sure the old and deprecated GST_PLUGIN_DEFINE_STATIC still works  */

static guint plugin_init_counter;       /* 0 */

static gboolean
plugin1_init (GstPlugin * plugin)
{
  ++plugin_init_counter;
  return TRUE;
}

static gboolean
plugin2_init (GstPlugin * plugin)
{
  ++plugin_init_counter;
  return TRUE;
}

static gboolean
plugin3_init (GstPlugin * plugin)
{
  ++plugin_init_counter;
  return TRUE;
}

GST_PLUGIN_DEFINE_STATIC (GST_VERSION_MAJOR, GST_VERSION_MINOR, "plugin-1",
    "some static elements 1", plugin1_init, VERSION, GST_LICENSE, PACKAGE,
    GST_PACKAGE_ORIGIN);

GST_PLUGIN_DEFINE_STATIC (GST_VERSION_MAJOR, GST_VERSION_MINOR, "plugin-2",
    "some static elements 2", plugin2_init, VERSION, GST_LICENSE, PACKAGE,
    GST_PACKAGE_ORIGIN);

GST_PLUGIN_DEFINE_STATIC (GST_VERSION_MAJOR, GST_VERSION_MINOR, "plugin-3",
    "some static elements 3", plugin3_init, VERSION, GST_LICENSE, PACKAGE,
    GST_PACKAGE_ORIGIN);

void test_old_register_static()
{
  fail_unless (plugin_init_counter == 3);
}



#endif /* GST_GNUC_CONSTRUCTOR_DEFINED */


static gboolean
register_check_elements (GstPlugin * plugin)
{
  return TRUE;
}

static GstPluginDesc plugin_desc = {
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  "check elements",
  "check elements",
  register_check_elements,
  VERSION,
  GST_LICENSE,
  PACKAGE,
  GST_PACKAGE_NAME,
  GST_PACKAGE_ORIGIN,

  GST_PADDING_INIT
};

void test_register_static()
{
  GstPlugin *plugin;
  //xmlfile = "test_register_static";
    std_log(LOG_FILENAME_LINE, "Test Started test_register_static");
  _gst_plugin_register_static (&plugin_desc);
  fail_unless (gst_plugin_register_static (GST_VERSION_MAJOR,
          GST_VERSION_MINOR, "more-elements", "more-elements",
          register_check_elements, VERSION, GST_LICENSE, PACKAGE,
          GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN));

  plugin = g_object_new (GST_TYPE_PLUGIN, NULL);

  gst_object_unref (plugin);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_registry()
{
  GList *g;
  GstRegistry *registry;
  //xmlfile = "test_registry";
    std_log(LOG_FILENAME_LINE, "Test Started test_registry");
  registry = gst_registry_get_default ();

  for (g = registry->plugins; g; g = g->next) {
    GstPlugin *plugin = GST_PLUGIN (g->data);

    ASSERT_OBJECT_REFCOUNT (plugin, "plugin in registry", 1);
    GST_DEBUG ("refcount %d %s", GST_OBJECT_REFCOUNT_VALUE (plugin),
        plugin->desc.name);
  }
  for (g = registry->features; g; g = g->next) {
    GstPluginFeature *feature = GST_PLUGIN_FEATURE (g->data);

    fail_if (GST_OBJECT_REFCOUNT_VALUE (feature) != 1,
        "Feature in registry should have refcount of 1");
    GST_DEBUG ("refcount %d %s", GST_OBJECT_REFCOUNT_VALUE (feature),
        feature->name);
  }
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_load_coreelements()
{
  GstPlugin *unloaded_plugin;
  GstPlugin *loaded_plugin;
  //xmlfile = "test_load_coreelements";
    std_log(LOG_FILENAME_LINE, "Test Started test_load_coreelements");
  unloaded_plugin = gst_default_registry_find_plugin ("coreelements");
  fail_if (unloaded_plugin == NULL, "Failed to find coreelements plugin");
  fail_if (GST_OBJECT_REFCOUNT_VALUE (unloaded_plugin) != 2,
      "Refcount of unloaded plugin in registry initially should be 2");
  GST_DEBUG ("refcount %d", GST_OBJECT_REFCOUNT_VALUE (unloaded_plugin));
  loaded_plugin = gst_plugin_load (unloaded_plugin);
  fail_if (loaded_plugin == NULL, "Failed to load plugin");
  if (loaded_plugin != unloaded_plugin) {
    fail_if (GST_OBJECT_REFCOUNT_VALUE (loaded_plugin) != 2,
        "Refcount of loaded plugin in registry should be 2");
      GST_DEBUG ("refcount %d", GST_OBJECT_REFCOUNT_VALUE (loaded_plugin));
     fail_if (GST_OBJECT_REFCOUNT_VALUE (unloaded_plugin) != 1,
        "Refcount of replaced plugin should be 1");
    GST_DEBUG ("refcount %d", GST_OBJECT_REFCOUNT_VALUE (unloaded_plugin));
  }

  gst_object_unref (unloaded_plugin);
  gst_object_unref (loaded_plugin);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_registry_get_plugin_list()
{
  GList *list;
  GstPlugin *plugin;
  //xmlfile = "test_registry_get_plugin_list";
    std_log(LOG_FILENAME_LINE, "Test Started test_registry_get_plugin_list");
  plugin = gst_default_registry_find_plugin ("coreelements");
  fail_if (GST_OBJECT_REFCOUNT_VALUE (plugin) != 2,
      "Refcount of plugin in registry should be 2");
     list = gst_registry_get_plugin_list (gst_registry_get_default ());
  

  fail_if (GST_OBJECT_REFCOUNT_VALUE (plugin) != 3,
      "Refcount of plugin in registry+list should be 3");
     gst_plugin_list_free (list);

  fail_if (GST_OBJECT_REFCOUNT_VALUE (plugin) != 2,
      "Refcount of plugin in after list free should be 2");
       gst_object_unref (plugin);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_find_plugin()
{
  GstPlugin *plugin;
  //xmlfile = "test_find_plugin";
    std_log(LOG_FILENAME_LINE, "Test Started test_find_plugin");
  plugin = gst_registry_find_plugin (gst_registry_get_default (),
      "coreelements");
  fail_if (plugin == NULL, "Failed to find coreelements plugin");
  ASSERT_OBJECT_REFCOUNT (plugin, "plugin", 2);
  fail_unless_equals_string (plugin->desc.version, VERSION);
  fail_unless_equals_string (plugin->desc.license, "LGPL");
  fail_unless_equals_string (plugin->desc.source, "gstreamer");
  fail_unless_equals_string (plugin->desc.package, GST_PACKAGE_NAME);
  fail_unless_equals_string (plugin->desc.origin, GST_PACKAGE_ORIGIN);
  gst_object_unref (plugin);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}




void test_find_feature()
{
  GstPluginFeature *feature;
  //xmlfile = "test_find_feature";
    std_log(LOG_FILENAME_LINE, "Test Started test_find_feature");
  feature = gst_registry_find_feature (gst_registry_get_default (),
      "identity", GST_TYPE_ELEMENT_FACTORY);
  fail_if (feature == NULL, "Failed to find identity element factory");
  fail_if (strcmp (feature->plugin_name, "coreelements"),
      "Expected identity to be from coreelements plugin");
  fail_if (GST_OBJECT_REFCOUNT_VALUE (feature) != 2,
      "Refcount of feature should be 2");
  GST_DEBUG ("refcount %d", GST_OBJECT_REFCOUNT_VALUE (feature));
  gst_object_unref (feature);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_find_element()
{
  GstElementFactory *element_factory;
  //xmlfile = "test_find_element";
    std_log(LOG_FILENAME_LINE, "Test Started test_find_element");
  element_factory = gst_element_factory_find ("identity");
  fail_if (element_factory == NULL, "Failed to find identity element factory");
  fail_if (GST_OBJECT_REFCOUNT_VALUE (element_factory) != 2,
      "Refcount of plugin in registry+feature should be 2");
  gst_object_unref (element_factory);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}

/*  BKP
//-----------------------------------------------------
guint8 *
peek (gpointer data, gint64 offset, guint size)
{
  return NULL;
}

void
suggest (gpointer data, guint probability, const GstCaps * caps)
{

}

void test_typefind()
{
  GstPlugin *plugin;
  GstPluginFeature *feature;
  GstTypeFind typefind = {
    peek,
    suggest,
    NULL,
    NULL,
    GST_PADDING_INIT
  };
xmlfile = "test_typefind";
    std_log(LOG_FILENAME_LINE, "Test Started test_typefind");
  plugin = gst_default_registry_find_plugin ("typefindfunctions");
  fail_if (plugin == NULL, "Failed to find typefind functions");
  std_log(LOG_FILENAME_LINE, "1");
  fail_if (GST_OBJECT_REFCOUNT_VALUE (plugin) != 2,
      "Refcount of plugin in registry should be 2");
  fail_if (gst_plugin_is_loaded (plugin), "Expected plugin to be unloaded");

  feature = gst_registry_find_feature (gst_registry_get_default (),
      "audio/x-au", GST_TYPE_TYPE_FIND_FACTORY);
  fail_if (feature == NULL, "Failed to find audio/x-aw typefind factory");
 
 
  #ifndef __SYMBIAN32__
  fail_if (feature->plugin != plugin,
      "Expected identity to be from coreelements plugin");
  fail_if (GST_OBJECT_REFCOUNT_VALUE (plugin) != 3,
      "Refcount of plugin in registry+feature should be 3");
  gst_type_find_factory_call_function (GST_TYPE_FIND_FACTORY (feature),
      &typefind);
  gst_object_unref (feature->plugin);
  #endif

  #ifdef __SYMBIAN32__	
  gst_object_unref (plugin);  
  #endif 
  
  fail_if (GST_OBJECT_REFCOUNT_VALUE (plugin) != 1,
      "Refcount of plugin in after list free should be 1");//crash

  #ifndef __SYMBIAN32__	
  gst_object_unref (plugin);  
  #endif 
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


//-----------------------------
*/




guint8 *
peek (gpointer data, gint64 offset, guint size)
{
  return NULL;
}

void
suggest (gpointer data, guint probability, const GstCaps * caps)
{

}

void test_typefind()
{
  GstPlugin *plugin;
  GstPluginFeature *feature;
  GstTypeFind typefind = {
    peek,
    suggest,
    NULL,
    NULL,
    GST_PADDING_INIT
  };
//xmlfile = "test_typefind";
    std_log(LOG_FILENAME_LINE, "Test Started test_typefind");
  plugin = gst_default_registry_find_plugin ("typefindfunctions");
  fail_if (plugin == NULL, "Failed to find typefind functions");
     fail_if (GST_OBJECT_REFCOUNT_VALUE (plugin) != 2,
      "Refcount of plugin in registry should be 2");
     /*
        //commented
          fail_if (gst_plugin_is_loaded (plugin), "Expected plugin to be unloaded");
     */
  feature = gst_registry_find_feature (gst_registry_get_default (),
      "audio/x-au", GST_TYPE_TYPE_FIND_FACTORY);
  fail_if (feature == NULL, "Failed to find audio/x-aw typefind factory");
  
  
#ifndef __SYMBIAN32__
  fail_if (feature->plugin != plugin,
      "Expected identity to be from coreelements plugin");
  fail_if (GST_OBJECT_REFCOUNT_VALUE (plugin) != 3,
      "Refcount of plugin in registry+feature should be 3");
  gst_type_find_factory_call_function (GST_TYPE_FIND_FACTORY (feature),
      &typefind);
  gst_object_unref (feature->plugin);
  

#endif
  
  
  
  
  
  gst_object_unref (plugin);  
 
  fail_if (GST_OBJECT_REFCOUNT_VALUE (plugin) != 1,
      "Refcount of plugin in after list free should be 1");

  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}

void test_version_checks()
{

//xmlfile = "test_version_checks";
  std_log(LOG_FILENAME_LINE, "Test Started test_version_checks");
  fail_if (gst_default_registry_check_feature_version ("identity",
          GST_VERSION_MAJOR, GST_VERSION_MINOR, GST_VERSION_MICRO) == FALSE,
      "Unexpected version check result");

  fail_if (gst_default_registry_check_feature_version ("identity",
          GST_VERSION_MAJOR + 1, GST_VERSION_MINOR, GST_VERSION_MICRO) == TRUE,
      "Unexpected version check result");

  fail_if (gst_default_registry_check_feature_version ("identity",
          GST_VERSION_MAJOR, GST_VERSION_MINOR + 1, GST_VERSION_MICRO) == TRUE,
      "Unexpected version check result");

  fail_if (gst_default_registry_check_feature_version ("identity",
          GST_VERSION_MAJOR, GST_VERSION_MINOR, GST_VERSION_MICRO + 1) == TRUE,
      "Unexpected version check result");

  if (GST_VERSION_MAJOR > 0) {
    fail_if (gst_default_registry_check_feature_version ("identity",
            GST_VERSION_MAJOR - 1, GST_VERSION_MINOR,
            GST_VERSION_MICRO) == FALSE, "Unexpected version check result");
  }

  if (GST_VERSION_MINOR > 0) {
    fail_if (gst_default_registry_check_feature_version ("identity",
            GST_VERSION_MAJOR, GST_VERSION_MINOR - 1,
            GST_VERSION_MICRO) == FALSE, "Unexpected version check result");
  }

  if (GST_VERSION_MICRO > 0) {
    fail_if (gst_default_registry_check_feature_version ("identity",
            GST_VERSION_MAJOR, GST_VERSION_MINOR,
            GST_VERSION_MICRO - 1) == FALSE, "Unexpected version check result");
  }

  fail_if (gst_default_registry_check_feature_version ("entityid",
          GST_VERSION_MAJOR, GST_VERSION_MINOR, GST_VERSION_MICRO) == TRUE,
      "Unexpected version check result");
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void (*fn[]) (void) = {
test_register_static,
test_registry,
test_registry_get_plugin_list,
test_find_plugin,
test_find_feature,
test_find_element,
test_version_checks,
test_load_coreelements,
test_typefind
};

char *args[] = {
"test_register_static",
"test_registry",
"test_registry_get_plugin_list",
"test_find_plugin",
"test_find_feature",
"test_find_element",
"test_version_checks",
"test_load_coreelements",
"test_typefind"
};

GST_CHECK_MAIN (gst_plugin);




