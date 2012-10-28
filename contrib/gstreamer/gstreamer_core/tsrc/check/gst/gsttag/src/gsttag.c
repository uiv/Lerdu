/*
 * Copyright (C) 2003 Benjamin Otte <in7y118@public.uni-hamburg.de>
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
#include <gst/check/gstcheck.h>

#include <string.h>

#define LOG_FILE "c:\\logs\\gsttag_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

//gboolean _gst_check_debug = FALSE;
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


/* multiple artists are possible */
#define UTAG GST_TAG_ARTIST
#define UNFIXED1 "Britney Spears"
#define UNFIXED2 "Evanescence"
#define UNFIXED3 "AC/DC"
#define UNFIXED4 "The Prodigy"

/* license is fixed */
#define FTAG GST_TAG_LICENSE
#define FIXED1 "Lesser General Public License"
#define FIXED2 "Microsoft End User License Agreement"
#define FIXED3 "Mozilla Public License"
#define FIXED4 "Public Domain"

/* checks that a tag contains the given values and not more values */
static void
check_tags (const GstTagList * list, const gchar * tag, gchar * value, ...)
{
  va_list args;
  gchar *str;
  guint i = 0;

  va_start (args, value);
  while (value != NULL) {
    fail_unless (gst_tag_list_get_string_index (list, tag, i, &str));
    fail_unless (strcmp (value, str) == 0);
    g_free (str);

    value = va_arg (args, gchar *);
    i++;
  }
  fail_unless (i == gst_tag_list_get_tag_size (list, tag));
  va_end (args);
}

#define NEW_LIST_FIXED(mode)                                    \
G_STMT_START {                                                  \
  if (list) gst_tag_list_free (list);                           \
  list = gst_tag_list_new ();                                   \
  gst_tag_list_add (list, mode, FTAG, FIXED1, FTAG, FIXED2,     \
                    FTAG, FIXED3, FTAG, FIXED4, NULL);          \
} G_STMT_END;

#define NEW_LIST_UNFIXED(mode)                                  \
G_STMT_START {                                                  \
  if (list) gst_tag_list_free (list);                           \
  list = gst_tag_list_new ();                                   \
  gst_tag_list_add (list, mode, UTAG, UNFIXED1, UTAG, UNFIXED2, \
                    UTAG, UNFIXED3, UTAG, UNFIXED4, NULL);      \
} G_STMT_END;

#define NEW_LISTS_FIXED(mode)                                   \
G_STMT_START {                                                  \
  if (list) gst_tag_list_free (list);                           \
  list = gst_tag_list_new ();                                   \
  gst_tag_list_add (list, GST_TAG_MERGE_APPEND, FTAG, FIXED1,   \
                    FTAG, FIXED2, NULL);                        \
  if (list2) gst_tag_list_free (list2);                         \
  list2 = gst_tag_list_new ();                                  \
  gst_tag_list_add (list2, GST_TAG_MERGE_APPEND, FTAG, FIXED3,  \
                    FTAG, FIXED4, NULL);                        \
  if (merge) gst_tag_list_free (merge);                         \
  merge = gst_tag_list_merge (list, list2, mode);               \
} G_STMT_END;

#define NEW_LISTS_UNFIXED(mode)                                 \
G_STMT_START {                                                  \
  if (list) gst_tag_list_free (list);                           \
  list = gst_tag_list_new ();                                   \
  gst_tag_list_add (list, GST_TAG_MERGE_APPEND, UTAG, UNFIXED1, \
                    UTAG, UNFIXED2, NULL);                      \
  if (list2) gst_tag_list_free (list2);                         \
  list2 = gst_tag_list_new ();                                  \
  gst_tag_list_add (list2, GST_TAG_MERGE_APPEND, UTAG, UNFIXED3,\
                    UTAG, UNFIXED4, NULL);                      \
  if (merge) gst_tag_list_free (merge);                         \
  merge = gst_tag_list_merge (list, list2, mode);               \
} G_STMT_END;


void test_merge()
{
  GstTagList *list = NULL, *list2 = NULL, *merge = NULL;
  
  xmlfile = "gsttag_test_merge";

  /* make sure the assumptions work */
  fail_unless (gst_tag_is_fixed (FTAG));
  fail_unless (!gst_tag_is_fixed (UTAG));
  /* we check string here only */
  fail_unless (gst_tag_get_type (FTAG) == G_TYPE_STRING);
  fail_unless (gst_tag_get_type (UTAG) == G_TYPE_STRING);

  /* check additions */

  /* unfixed */
  NEW_LIST_UNFIXED (GST_TAG_MERGE_REPLACE_ALL);
  check_tags (list, UTAG, UNFIXED4, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_REPLACE);
  check_tags (list, UTAG, UNFIXED4, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_PREPEND);
  check_tags (list, UTAG, UNFIXED4, UNFIXED3, UNFIXED2, UNFIXED1, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_APPEND);
  check_tags (list, UTAG, UNFIXED1, UNFIXED2, UNFIXED3, UNFIXED4, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_KEEP);
  check_tags (list, UTAG, UNFIXED1, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_KEEP_ALL);
  check_tags (list, UTAG, NULL);

  /* fixed */
  NEW_LIST_FIXED (GST_TAG_MERGE_REPLACE_ALL);
  check_tags (list, FTAG, FIXED4, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_REPLACE);
  check_tags (list, FTAG, FIXED4, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_PREPEND);
  check_tags (list, FTAG, FIXED4, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_APPEND);
  check_tags (list, FTAG, FIXED1, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_KEEP);
  check_tags (list, FTAG, FIXED1, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_KEEP_ALL);
  check_tags (list, FTAG, NULL);

  /* check merging */
  /* unfixed */
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_REPLACE_ALL);
  check_tags (merge, UTAG, UNFIXED3, UNFIXED4, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_REPLACE);
  check_tags (merge, UTAG, UNFIXED3, UNFIXED4, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_PREPEND);
  check_tags (merge, UTAG, UNFIXED3, UNFIXED4, UNFIXED1, UNFIXED2, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_APPEND);
  check_tags (merge, UTAG, UNFIXED1, UNFIXED2, UNFIXED3, UNFIXED4, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_KEEP);
  check_tags (merge, UTAG, UNFIXED1, UNFIXED2, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_KEEP_ALL);
  check_tags (merge, UTAG, UNFIXED1, UNFIXED2, NULL);

  /* fixed */
  NEW_LISTS_FIXED (GST_TAG_MERGE_REPLACE_ALL);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_REPLACE);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_PREPEND);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_APPEND);
  check_tags (merge, FTAG, FIXED1, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_KEEP);
  check_tags (merge, FTAG, FIXED1, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_KEEP_ALL);
  check_tags (merge, FTAG, FIXED1, NULL);

  /* clean up */
  if (list)
    gst_tag_list_free (list);
  if (list2)
    gst_tag_list_free (list2);
  if (merge)
    gst_tag_list_free (merge);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

void test_date_tags()
{
  GstTagList *tag_list, *tag_list2;
  GDate *date, *date2;
  gchar *str;
  
  xmlfile = "gsttag_test_date_tags";

  date = g_date_new_dmy (14, 10, 2005);
  tag_list = gst_tag_list_new ();
  gst_tag_list_add (tag_list, GST_TAG_MERGE_APPEND, GST_TAG_DATE, date, NULL);

  str = gst_structure_to_string (tag_list);
  fail_if (str == NULL);
  fail_if (strstr (str, "2005-10-14") == NULL);

  tag_list2 = gst_structure_from_string (str, NULL);
  fail_if (tag_list2 == NULL);
  fail_if (!gst_tag_list_get_date (tag_list2, GST_TAG_DATE, &date2));
  gst_tag_list_free (tag_list2);
  g_free (str);

  fail_if (g_date_compare (date, date2) != 0);
  fail_if (g_date_get_day (date) != 14);
  fail_if (g_date_get_month (date) != 10);
  fail_if (g_date_get_year (date) != 2005);
  fail_if (g_date_get_day (date2) != 14);
  fail_if (g_date_get_month (date2) != 10);
  fail_if (g_date_get_year (date2) != 2005);
  g_date_free (date2);

  gst_tag_list_free (tag_list);
  g_date_free (date);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

void test_type()
{
  GstTagList *taglist;
  
  xmlfile = "gsttag_test_type";

  taglist = gst_tag_list_new ();
  fail_unless (GST_IS_TAG_LIST (taglist));
  fail_unless (gst_is_tag_list (taglist));
  gst_tag_list_free (taglist);

  /* this isn't okay */
  ASSERT_CRITICAL (fail_if (gst_is_tag_list (NULL)));

  /* this however should be fine */
  fail_if (GST_IS_TAG_LIST (NULL));

  /* check gst_tag_list_is_empty */
  ASSERT_CRITICAL (gst_tag_list_is_empty (NULL));
  taglist = gst_tag_list_new ();
  fail_unless (gst_tag_list_is_empty (taglist));
  gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "JD", NULL);
  fail_if (gst_tag_list_is_empty (taglist));
  gst_tag_list_free (taglist);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

void test_set_non_utf8_string()
{
  GstTagList *taglist;
  guint8 foobar[2] = { 0xff, 0x00 };    /* not UTF-8 */
  
  xmlfile = "gsttag_test_set_non_utf8_string";

  taglist = gst_tag_list_new ();
  fail_unless (taglist != NULL);

  ASSERT_WARNING (gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND,
          GST_TAG_ARTIST, (gchar *) foobar, NULL));

  /* That string field with a non-UTF8 string should not have been added */
  fail_unless (gst_tag_list_is_empty (taglist));

  gst_tag_list_free (taglist);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

void (*fn[4]) (void) = {
        test_merge,
        test_date_tags,
        test_type,
        test_set_non_utf8_string
};

char *args[] = {
        "test_merge",
        "test_date_tags",
        "test_type",
        "test_set_non_utf8_string"
};

GST_CHECK_MAIN (gst_tag);

//int main()
//    {
//    gst_check_init(NULL,NULL);
//    test_merge();
//    test_date_tags();
//    test_type();
//    test_set_non_utf8_string();
//    return 0;
//    }
