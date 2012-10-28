/* GStreamer
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
 *
 * gststructure.c: Unit tests for GstStructure
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
#include <gst/gststructure.h>
#include <gst/check/gstcheck.h>
#include <glib_global.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "libgstreamer_wsd_solution.h" 
#include <libgstreamer_wsd_macros.h>
#ifndef EMULATOR
#define assert_failed *(get_assert_failed())
#define fp_std_log *(get_fp_std_log())
#define xmlfile *(get_xmlfile())
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

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running;
#endif




#define LOG_FILE "c:\\logs\\gststructure_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__



#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers;
#endif


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}




void test_from_string_int()
{
  const char *strings[] = {
    "video/x-raw-rgb, width = (int) 123456",
    "video/x-raw-rgb, stride = (int) -123456",
    "video/x-raw-rgb, red_mask = (int) 0xFFFF",
    "video/x-raw-rgb, red_mask = (int) 0x0000FFFF",
    "video/x-raw-rgb, red_mask = (int) 0x7FFFFFFF",
    "video/x-raw-rgb, red_mask = (int) 0x80000000",
    "video/x-raw-rgb, red_mask = (int) 0xFF000000",
    /* result from
     * gst-launch ... ! "video/x-raw-rgb, red_mask=(int)0xFF000000" ! ... */
    "video/x-raw-rgb,\\ red_mask=(int)0xFF000000",
  };
  gint results[] = {
    123456,
    -123456,
    0xFFFF,
    0xFFFF,
    0x7FFFFFFF,
    (gint) 0x80000000,
    (gint) 0xFF000000,
    (gint) 0xFF000000,
  };
  GstStructure *structure;
  int i;
		xmlfile = "gststructure_test_from_string_int";
		std_log(LOG_FILENAME_LINE, "Test Started test_from_string_int");
  for (i = 0; i < G_N_ELEMENTS (strings); ++i) {
    const char *s;
    const gchar *name;
    gint value;

    s = strings[i];

    structure = gst_structure_from_string (s, NULL);
    fail_if (structure == NULL, "Could not get structure from string %s", s);
    name = gst_structure_nth_field_name (structure, 0);
    fail_unless (gst_structure_get_int (structure, name, &value));
    fail_unless (value == results[i],
        "Value %d is not the expected result %d for string %s",
        value, results[i], s);

    /* cleanup */
    gst_structure_free (structure);
  }

	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
}



/* Test type conversions from string */
void test_from_string()
{
  GstStructure *structure;
  const gchar *s;
  const GValue *val;
	xmlfile = "gststructure_test_from_string";
	std_log(LOG_FILENAME_LINE, "Test Started test_from_string");
  s = "test-string,value=1";
  structure = gst_structure_from_string (s, NULL);
  fail_if (structure == NULL, "Could not get structure from string %s", s);
  fail_unless ((val = gst_structure_get_value (structure, "value")) != NULL);
  fail_unless (G_VALUE_HOLDS_INT (val));
  gst_structure_free (structure);

  s = "test-string,value=1.0";
  structure = gst_structure_from_string (s, NULL);
  fail_if (structure == NULL, "Could not get structure from string %s", s);
  fail_unless ((val = gst_structure_get_value (structure, "value")) != NULL);
  fail_unless (G_VALUE_HOLDS_DOUBLE (val));
  gst_structure_free (structure);

  s = "test-string,value=1/1";
  structure = gst_structure_from_string (s, NULL);
  fail_if (structure == NULL, "Could not get structure from string %s", s);
  fail_unless ((val = gst_structure_get_value (structure, "value")) != NULL);
  fail_unless (GST_VALUE_HOLDS_FRACTION (val));
  gst_structure_free (structure);

  s = "test-string,value=bar";
  structure = gst_structure_from_string (s, NULL);
  fail_if (structure == NULL, "Could not get structure from string %s", s);
  fail_unless ((val = gst_structure_get_value (structure, "value")) != NULL);
  fail_unless (G_VALUE_HOLDS_STRING (val));
  gst_structure_free (structure);

  s = "test-string,value=(boolean)true";
  structure = gst_structure_from_string (s, NULL);
  fail_if (structure == NULL, "Could not get structure from string %s", s);
  fail_unless ((val = gst_structure_get_value (structure, "value")) != NULL);
  fail_unless (G_VALUE_HOLDS_BOOLEAN (val));
  fail_unless_equals_int (g_value_get_boolean (val), TRUE);
  gst_structure_free (structure);

  /* This should still work for now (FIXME: 0.11) */
  s = "0.10:decoder-video/mpeg, abc=(boolean)false";
  structure = gst_structure_from_string (s, NULL);
  fail_if (structure == NULL, "Could not get structure from string %s", s);
  gst_structure_free (structure);

  /* make sure we bail out correctly in case of an error or if parsing fails */
  s = "***foo***, abc=(boolean)false";
  structure = gst_structure_from_string (s, NULL);
  fail_unless (structure == NULL);

  /* assert that we get a warning if the structure wasn't entirely consumed, but
   * we didn't provide an end pointer */
  s = "foo/bar; other random data";
  ASSERT_WARNING (structure = gst_structure_from_string (s, NULL));
  fail_if (structure == NULL, "Could not get structure from string %s", s);
  gst_structure_free (structure);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);

}




void test_to_string()
{
  GstStructure *st1;
  char *p,*m;
	xmlfile = "gststructure_test_to_string";
	std_log(LOG_FILENAME_LINE, "Test Started test_to_string");
  ASSERT_CRITICAL (st1 = gst_structure_new ("Foo\nwith-newline", NULL));
// ASSERT_CRITICAL (st1 = gst_structure_new ("^joo\nba\ndoo^",NULL));
 
 
  fail_unless (st1 == NULL);

  /* FIXME 0.11: re-enable this */
#if 0
  ASSERT_CRITICAL (st1 = gst_structure_new ("Foo with whitespace", NULL));
  fail_unless (st1 == NULL);
  ASSERT_CRITICAL (st1 = gst_structure_new ("1st", NULL));
  fail_unless (st1 == NULL);
#else
  st1 = gst_structure_new ("Foo with whitespace is still allowed", NULL);
  fail_unless (st1 != NULL);
  gst_structure_free (st1);

  /* structure names starting with a number are also still allowed */
  st1 = gst_structure_new ("1st", NULL);
  fail_unless (st1 != NULL);
  gst_structure_free (st1);
#endif
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
}




void test_to_from_string()
{
  GstCaps *caps1, *caps2;
  GstStructure *st1, *st2;
  gchar *str, *res1, *res2;
	xmlfile = "gststructure_test_to_from_string";
	std_log(LOG_FILENAME_LINE, "Test Started test_to_from_string");
  /* test escaping/unescaping */
  st1 = gst_structure_new ("FooBar-123/0_1", "num", G_TYPE_INT, 9173,
      "string", G_TYPE_STRING, "Something Like Face/Off", NULL);
  str = gst_structure_to_string (st1);
  st2 = gst_structure_from_string (str, NULL);
  g_free (str);

  fail_unless (st2 != NULL);

  /* need to put stuctures into caps to compare */
  caps1 = gst_caps_new_empty ();
  gst_caps_append_structure (caps1, st1);
  caps2 = gst_caps_new_empty ();
  gst_caps_append_structure (caps2, st2);
  res1 = gst_caps_to_string (caps1);
  res2 = gst_caps_to_string (caps2);
  fail_unless (gst_caps_is_equal (caps1, caps2),
      "Structures did not match:\n\tStructure 1: %s\n\tStructure 2: %s\n",
      res1, res2);
  gst_caps_unref (caps1);
  gst_caps_unref (caps2);
  g_free (res1);
  g_free (res2);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);

}



void test_complete_structure()
{
  GstStructure *structure;
  const gchar *s;
	xmlfile = "gststructure_test_complete_structure";
	std_log(LOG_FILENAME_LINE, "Test Started test_complete_structure");
  s = "GstEventSeek, rate=(double)1, format=(GstFormat)GST_FORMAT_TIME, flags=(GstSeekFlags)GST_SEEK_FLAGS_NONE, cur_type=(GstSeekType)GST_SEEK_TYPE_SET, cur=(gint64)1000000000, stop_type=(GstSeekType)GST_SEEK_TYPE_NONE, stop=(gint64)0";
  structure = gst_structure_from_string (s, NULL);
  fail_if (structure == NULL, "Could not get structure from string %s", s);
  /* FIXME: TODO: add checks for correct serialization of members ? */
  gst_structure_free (structure);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);

}



void test_structure_new()
{
  GstStructure *s;
  GError *e;
  GQuark domain;
  gboolean bool;
  gint num, den;
  GstClockTime clocktime;
  guint32 fourcc;
	xmlfile = "gststructure_test_structure_new";
	std_log(LOG_FILENAME_LINE, "Test Started test_structure_new");
  s = gst_structure_new ("name",
      "key", G_TYPE_STRING, "value",
      "bool", G_TYPE_BOOLEAN, TRUE,
      "fraction", GST_TYPE_FRACTION, 1, 5,
      "clocktime", GST_TYPE_CLOCK_TIME, GST_CLOCK_TIME_NONE,
      "fourcc", GST_TYPE_FOURCC, GST_MAKE_FOURCC ('f', 'o', 'u', 'r'), NULL);

  fail_unless (gst_structure_get_field_type (s, "unknown") == G_TYPE_INVALID);
  /* test setting a different name */
  gst_structure_set_name (s, "newname");
  fail_unless (strcmp (gst_structure_get_string (s, "key"), "value") == 0);
  fail_unless (gst_structure_has_field (s, "key"));
  fail_unless_equals_int (gst_structure_n_fields (s), 5);
  /* test removing a field */
  gst_structure_remove_field (s, "key");
  fail_if (gst_structure_get_string (s, "key"));
  fail_if (gst_structure_has_field (s, "key"));
  fail_unless_equals_int (gst_structure_n_fields (s), 4);

  fail_unless (gst_structure_get_boolean (s, "bool", &bool));
  fail_unless (bool);

  fail_unless (gst_structure_get_fraction (s, "fraction", &num, &den));
  fail_unless_equals_int (num, 1);
  fail_unless_equals_int (den, 5);

  fail_unless (gst_structure_get_clock_time (s, "clocktime", &clocktime));
  fail_unless_equals_uint64 (clocktime, GST_CLOCK_TIME_NONE);

  fail_unless (gst_structure_get_fourcc (s, "fourcc", &fourcc));

  gst_structure_free (s);

  domain = g_quark_from_static_string ("test");
  e = g_error_new (domain, 0, "a test error");
  s = gst_structure_new ("name", "key", GST_TYPE_G_ERROR, e, NULL);
  g_error_free (e);
  gst_structure_free (s);

  /* This should still work for now (FIXME 0.11) */
  gst_structure_free (gst_structure_new ("0.10:decoder-video/mpeg", NULL));

  /* make sure we bail out correctly in case of an error or if parsing fails */
  ASSERT_CRITICAL (s = gst_structure_new ("^joo\nba\ndoo^",
          "abc", G_TYPE_BOOLEAN, FALSE, NULL));
  fail_unless (s == NULL);

	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
}



void test_fixate()
{
  GstStructure *s;
	xmlfile = "gststructure_test_fixate";
	std_log(LOG_FILENAME_LINE, "Test Started test_fixate");
  s = gst_structure_new ("name",
      "int", G_TYPE_INT, 5,
      "intrange", GST_TYPE_INT_RANGE, 5, 10,
      "intrange2", GST_TYPE_INT_RANGE, 5, 10, NULL);

  fail_if (gst_structure_fixate_field_nearest_int (s, "int", 5));
  fail_unless (gst_structure_fixate_field_nearest_int (s, "intrange", 5));
  fail_if (gst_structure_fixate_field_nearest_int (s, "intrange", 5));
  fail_unless (gst_structure_fixate_field_nearest_int (s, "intrange2", 15));
  fail_if (gst_structure_fixate_field_nearest_int (s, "intrange2", 15));
  gst_structure_free (s);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);

}



void test_fixate_frac_list()
{
  GstStructure *s;
  GValue list = { 0 };
  GValue frac = { 0 };
  gchar *str;
  gint num, denom;
	xmlfile = "gststructure_test_fixate_frac_list";
	std_log(LOG_FILENAME_LINE, "Test Started test_fixate_frac_list");
  g_value_init (&list, GST_TYPE_LIST);
  g_value_init (&frac, GST_TYPE_FRACTION);

  gst_value_set_fraction (&frac, 30, 1);
  gst_value_list_append_value (&list, &frac);
  gst_value_set_fraction (&frac, 15, 1);
  gst_value_list_append_value (&list, &frac);
  gst_value_set_fraction (&frac, 10, 1);
  gst_value_list_append_value (&list, &frac);

  s = gst_structure_new ("name", NULL);
  gst_structure_set_value (s, "frac", &list);
  g_value_unset (&frac);
  g_value_unset (&list);

  str = gst_structure_to_string (s);
  GST_DEBUG ("list %s", str);
  g_free (str);

  /* fixate to the nearest fraction, this should give 15/1 */
  fail_unless (gst_structure_fixate_field_nearest_fraction (s, "frac", 14, 1));

  fail_unless (gst_structure_get_fraction (s, "frac", &num, &denom));
  fail_unless (num == 15);
  fail_unless (denom == 1);

  gst_structure_free (s);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
}



void test_structure_nested()
{
  GstStructure *sp, *sc1, *sc2;
  gchar *str;
	xmlfile = "gststructure_test_structure_nested";
	std_log(LOG_FILENAME_LINE, "Test Started test_structure_nested");
  sc1 =
      gst_structure_new ("Camera", "XResolution", G_TYPE_INT, 72, "YResolution",
      G_TYPE_INT, 73, NULL);
  fail_unless (sc1 != NULL);

  sc2 =
      gst_structure_new ("Image-Data", "Orientation", G_TYPE_STRING, "top-left",
      NULL);
  fail_unless (sc2 != NULL);

  sp = gst_structure_new ("Exif", "Camera", GST_TYPE_STRUCTURE, sc1,
      "Image Data", GST_TYPE_STRUCTURE, sc2, NULL);
  fail_unless (sp != NULL);

  fail_unless (gst_structure_has_field_typed (sp, "Camera",
          GST_TYPE_STRUCTURE));

  str = gst_structure_to_string (sp);
  fail_unless (str != NULL);

  fail_unless (g_str_equal (str,
          "Exif"
          ", Camera=(GstStructure)Camera, XResolution=(int)72, YResolution=(int)73;"
          ", Image Data=(GstStructure)Image-Data, Orientation=(string)top-left;;"));

  g_free (str);
  str = NULL;

  gst_structure_free (sc1);
  gst_structure_free (sc2);
  gst_structure_free (sp);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
}

#if 1
void (*fn[9]) (void) = {
	  		test_from_string_int,
	  		test_from_string,
				test_to_string,
				test_to_from_string,
				test_complete_structure,
				test_structure_new,
				test_fixate,
				test_fixate_frac_list,
				test_structure_nested
};

char *args[] = {
				"test_from_string_int",
				"test_from_string",
				"test_to_string",
				"test_to_from_string",
				"test_complete_structure",
				"test_structure_new",
				"test_fixate",
				"test_fixate_frac_list",
				"test_structure_nested"
				
};



GST_CHECK_MAIN (gst_structure);
#endif


#if 0
int main()
{
    
    
	gst_check_init (NULL, NULL);
	test_from_string_int();
	test_from_string();
	test_to_string();
	test_to_from_string();
	test_complete_structure();
	test_structure_new();
	test_fixate();
	test_fixate_frac_list();
	test_structure_nested();


}
#endif




