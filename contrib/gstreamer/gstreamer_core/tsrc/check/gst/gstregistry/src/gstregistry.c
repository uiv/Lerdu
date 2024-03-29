/* GStreamer unit tests for the plugin registry
 *
 * Copyright (C) 2006 Tim-Philipp Müller <tim centricular net>
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#define LOG_FILE "c:\\logs\\gstregistry_logs.txt" 

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


#include <gst/check/gstcheck.h>
#include <string.h>

static gint
plugin_name_cmp (GstPlugin * a, GstPlugin * b)
{
  const gchar *name_a = gst_plugin_get_name (a);
  const gchar *name_b = gst_plugin_get_name (b);

  return strcmp (name_a, name_b);
}


static void
print_plugin (const gchar * marker, GstRegistry * registry, GstPlugin * plugin)
{
  const gchar *name;
  GList *features, *f;

  name = gst_plugin_get_name (plugin);

  GST_DEBUG ("%s: plugin %p %d %s", marker, plugin,
      GST_OBJECT_REFCOUNT (plugin), name);

  features = gst_registry_get_feature_list_by_plugin (registry, name);
  for (f = features; f != NULL; f = f->next) {
    GstPluginFeature *feature;

    feature = GST_PLUGIN_FEATURE (f->data);

    GST_LOG ("%s:    feature: %p %s", marker, feature,
        gst_plugin_feature_get_name (feature));
  }
  gst_plugin_feature_list_free (features);
}

void test_registry_update()
{
  GstPluginFeature *old_identity, *new_identity;
  GstPluginFeature *old_pipeline, *new_pipeline;
  GstRegistry *registry;
  GList *plugins_before, *plugins_after, *l;
  
    std_log(LOG_FILENAME_LINE, "Test Started test_registry_update");

  registry = gst_registry_get_default ();
  fail_unless (registry != NULL);
  ASSERT_OBJECT_REFCOUNT (registry, "default registry", 1);

  /* refcount should still be 1 the second time */
  registry = gst_registry_get_default ();
  fail_unless (registry != NULL);
  ASSERT_OBJECT_REFCOUNT (registry, "default registry", 1);

  old_identity = gst_registry_lookup_feature (registry, "identity");
  fail_unless (old_identity != NULL, "Can't find plugin feature 'identity'");

  old_pipeline = gst_registry_lookup_feature (registry, "pipeline");
  fail_unless (old_pipeline != NULL, "Can't find plugin feature 'pipeline'");

  /* plugins should have a refcount of 2: the registry holds one reference,
   * and the other one is ours for the list */
  plugins_before = gst_registry_get_plugin_list (registry);
  for (l = plugins_before; l; l = l->next) {
    GstPlugin *plugin;

    plugin = GST_PLUGIN (l->data);

    print_plugin ("before1", registry, plugin);

    ASSERT_OBJECT_REFCOUNT (plugin, "plugin", 2);
  }

  fail_unless (gst_update_registry () != FALSE, "registry update failed");

  GST_LOG (" -----------------------------------");
  GST_LOG ("          registry updated          ");
  GST_LOG (" -----------------------------------");

  /* static plugins should have the same refcount as before (ie. 2), whereas
   * file-based plugins should have been replaced by a newly-created objects
   * (when reading the updated registry.xml file), so there should be only one
   * reference left for those, and that's ours */
  for (l = plugins_before; l; l = l->next) {
    GstPlugin *plugin;

    plugin = GST_PLUGIN (l->data);

    print_plugin ("before2", registry, plugin);

    if (gst_plugin_get_filename (plugin)) {
      /* file-based plugin */
      ASSERT_OBJECT_REFCOUNT (plugin, "plugin", 1);
    } else {
      /* static plugin */
      ASSERT_OBJECT_REFCOUNT (plugin, "plugin", 2);
    }
  }

  GST_LOG (" -----------------------------------");

  plugins_after = gst_registry_get_plugin_list (registry);
  for (l = plugins_after; l; l = l->next) {
    GstPlugin *plugin;

    plugin = GST_PLUGIN (l->data);

    print_plugin ("after  ", registry, plugin);

    /* file-based plugins should have a refcount of 2 (one for the registry,
     * one for us for the list), static plugins should have one of 3 (one for
     * the registry, one for the new list and one for the old list).
     * This implicitly also makes sure that all static plugins are the same
     * objects as they were before and that all non-static ones have been
     * replaced by new objects */
    if (gst_plugin_get_filename (plugin)) {
      ASSERT_OBJECT_REFCOUNT (plugin, "plugin", 2);
    } else {
      ASSERT_OBJECT_REFCOUNT (plugin, "plugin", 3);
    }
  }

  /* check that we still have all plugins in the new list that we had before */
  for (l = plugins_after; l; l = l->next) {
    GstPlugin *plugin;

    plugin = GST_PLUGIN (l->data);

    fail_unless (g_list_find_custom (plugins_before, plugin,
            (GCompareFunc) plugin_name_cmp) != NULL,
        "Plugin %s is in new list but not in old one?!",
        gst_plugin_get_name (plugin));
  }
  for (l = plugins_before; l; l = l->next) {
    GstPlugin *plugin;

    plugin = GST_PLUGIN (l->data);
    fail_unless (g_list_find_custom (plugins_after, plugin,
            (GCompareFunc) plugin_name_cmp) != NULL,
        "Plugin %s is in old list but not in new one?!",
        gst_plugin_get_name (plugin));
  }

  new_identity = gst_registry_lookup_feature (registry, "identity");
  fail_unless (new_identity != NULL, "Can't find plugin feature 'identity'");
  fail_unless (old_identity != new_identity, "Old and new 'identity' feature "
      "should be different but are the same object");

  ASSERT_OBJECT_REFCOUNT (old_identity, "old identity feature after update", 1);

  new_pipeline = gst_registry_lookup_feature (registry, "pipeline");
  fail_unless (new_pipeline != NULL, "Can't find plugin feature 'pipeline'");
  fail_unless (old_pipeline == new_pipeline, "Old and new 'pipeline' feature "
      "objects should be the same, but are different objects");

  gst_plugin_list_free (plugins_before);
  plugins_before = NULL;
  gst_plugin_list_free (plugins_after);
  plugins_after = NULL;
  registry = NULL;

  gst_object_unref (old_identity);
  gst_object_unref (new_identity);
  gst_object_unref (old_pipeline);
  gst_object_unref (new_pipeline);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void (*fn[]) (void) = {
test_registry_update
};

char *args[] = {
"test_registry_update"
};

GST_CHECK_MAIN (registry);
