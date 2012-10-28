/* GStreamer GstParamSpec unit tests
 * Copyright (C) 2007 Tim-Philipp Müller <tim centricular net>
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


#define LOG_FILE "c:\\logs\\gstparamspecs_logs.txt" 

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
#include <gst/gst.h>
#include <string.h>

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical;
#endif

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log;
#endif

/* some minimal dummy object */
#define GST_TYPE_DUMMY_OBJ gst_dummy_obj_get_type()

typedef struct
{
  GstElement parent;
  guint num, denom;
} GstDummyObj;

typedef GstElementClass GstDummyObjClass;

GST_BOILERPLATE (GstDummyObj, gst_dummy_obj, GstElement, GST_TYPE_ELEMENT);

static void
gst_dummy_obj_get_property (GObject * obj, guint prop_id, GValue * val,
    GParamSpec * pspec);
static void
gst_dummy_obj_set_property (GObject * obj, guint prop_id, const GValue * val,
    GParamSpec * pspec);

static void
gst_dummy_obj_base_init (gpointer g_class)
{
}

static void
gst_dummy_obj_class_init (GstDummyObjClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = gst_dummy_obj_get_property;
  gobject_class->set_property = gst_dummy_obj_set_property;

  ASSERT_CRITICAL (
      /* default value is out of bounds, should print a warning */
      g_object_class_install_property (gobject_class, 1,
          gst_param_spec_fraction ("ratio", "ratio", "ratio", 0, 1, 2, 1,
              16, 4, G_PARAM_READWRITE)););

  /* should be within bounds */
  g_object_class_install_property (gobject_class, 2,
      gst_param_spec_fraction ("other-ratio", "other ratio", "other ratio",
          0, 1, 2, 1, 16, 9, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, 3,
      g_param_spec_boolean ("foo", "foo", "foo", TRUE, G_PARAM_READWRITE));
}

static void
gst_dummy_obj_init (GstDummyObj * obj, GstDummyObjClass * klass)
{
  /* nothing to do there */
}

static void
gst_dummy_obj_set_property (GObject * obj, guint prop_id, const GValue * val,
    GParamSpec * pspec)
{
  GstDummyObj *dobj = (GstDummyObj *) obj;

  fail_unless_equals_int (prop_id, 2);
  dobj->num = gst_value_get_fraction_numerator (val);
  dobj->denom = gst_value_get_fraction_denominator (val);
}

static void
gst_dummy_obj_get_property (GObject * obj, guint prop_id, GValue * val,
    GParamSpec * pspec)
{
  GstDummyObj *dobj = (GstDummyObj *) obj;

  fail_unless_equals_int (prop_id, 2);
  gst_value_set_fraction (val, dobj->num, dobj->denom);
}

void test_param_spec_fraction()
{
  GObject *obj;
  GValue val = { 0, };
  gint n = 0, d = 0;
  
  std_log(LOG_FILENAME_LINE, "Test Started test_param_spec_fraction");

  obj = g_object_new (GST_TYPE_DUMMY_OBJ, "other-ratio", 15, 8, NULL);

  g_value_init (&val, GST_TYPE_FRACTION);
  g_object_get_property (G_OBJECT (obj), "other-ratio", &val);
  fail_unless_equals_int (gst_value_get_fraction_numerator (&val), 15);
  fail_unless_equals_int (gst_value_get_fraction_denominator (&val), 8);
  g_value_unset (&val);

  g_object_get (obj, "other-ratio", &n, &d, NULL);
  fail_unless_equals_int (n, 15);
  fail_unless_equals_int (d, 8);

  g_object_unref (obj);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


void (*fn[]) (void) = {
test_param_spec_fraction
};

char *args[] = {
"test_param_spec_fraction"
};

GST_CHECK_MAIN (gst_param_spec);
