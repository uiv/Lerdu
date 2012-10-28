/* GStreamer
 * Copyright (C) 2005 Andy Wingo <wingo@pobox.com>
 *
 * gstiterator.c: Unit test for iterators
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
#include <glib_global.h>
#define LOG_FILE "c:\\logs\\gstiterator_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

//#ifndef EMULATOR
//#define assert_failed *(get_assert_failed())
//#define fp_std_log *(get_fp_std_log())
////#define xmlfile *(get_xmlfile())
//#endif

//char* xmlfile = "gstiterator_logs";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

static GList *
make_list_of_ints (gint n)
{
  GList *ret = NULL;
  gint i;

  for (i = 0; i < n; i++)
    ret = g_list_prepend (ret, GINT_TO_POINTER (i));

  return g_list_reverse (ret);
}

#define NUM_ELEMENTS 10

void test_manual_iteration()
{
  GList *l;
  guint32 cookie = 0;
  GMutex *m;
  GstIterator *iter;
  GstIteratorResult res;
  gpointer item;
  gint i = 0;

  xmlfile = "gstiterator_test_manual_iteration";
  std_log(LOG_FILENAME_LINE, "Test Started gstiterator_test_manual_iteration");
  
  l = make_list_of_ints (NUM_ELEMENTS);
  m = g_mutex_new ();

  iter = gst_iterator_new_list (G_TYPE_INT, m, &cookie, &l, NULL, NULL, NULL);

  fail_unless (iter != NULL);

  while (1) {
    res = gst_iterator_next (iter, &item);
    if (i < NUM_ELEMENTS) {
      fail_unless (res == GST_ITERATOR_OK);
      fail_unless (GPOINTER_TO_INT (item) == i);
      i++;
      continue;
    } else {
      fail_unless (res == GST_ITERATOR_DONE);
      break;
    }
  }

  /* clean up */
  gst_iterator_free (iter);
  g_mutex_free (m);
  g_list_free (l);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

void test_resync()
{
  GList *l;
  guint32 cookie = 0;
  GMutex *m;
  GstIterator *iter;
  GstIteratorResult res;
  gpointer item;
  gint i = 0;
  gboolean hacked_list = FALSE;
  
  xmlfile = "gstiterator_test_resync";
  std_log(LOG_FILENAME_LINE, "Test Started gstiterator_test_resync");

  l = make_list_of_ints (NUM_ELEMENTS);
  m = g_mutex_new ();

  iter = gst_iterator_new_list (G_TYPE_INT, m, &cookie, &l, NULL, NULL, NULL);

  fail_unless (iter != NULL);

  while (1) {
    res = gst_iterator_next (iter, &item);
    if (i < NUM_ELEMENTS / 2) {
      fail_unless (res == GST_ITERATOR_OK);
      fail_unless (GPOINTER_TO_INT (item) == i);
      i++;
      continue;
    } else if (!hacked_list) {
      /* here's where we test resync */
      fail_unless (res == GST_ITERATOR_OK);
      l = g_list_prepend (l, GINT_TO_POINTER (-1));
      cookie++;
      hacked_list = TRUE;
      continue;
    } else {
      fail_unless (res == GST_ITERATOR_RESYNC);
      gst_iterator_resync (iter);
      res = gst_iterator_next (iter, &item);
      fail_unless (res == GST_ITERATOR_OK);
      fail_unless (GPOINTER_TO_INT (item) == -1);
      break;
    }
  }

  /* clean up */
  gst_iterator_free (iter);
  g_mutex_free (m);
  g_list_free (l);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


static gboolean
add_fold_func (gpointer item, GValue * ret, gpointer user_data)
{
  g_value_set_int (ret, g_value_get_int (ret) + GPOINTER_TO_INT (item));
  return TRUE;
}

void test_fold()
{
  GList *l;
  guint32 cookie = 0;
  GMutex *m;
  GstIterator *iter;
  GstIteratorResult res;
  gint i, expected;
  GValue ret = { 0, };
  
  xmlfile = "gstiterator_test_fold";
  std_log(LOG_FILENAME_LINE, "Test Started gstiterator_test_fold");

  l = make_list_of_ints (NUM_ELEMENTS);
  m = g_mutex_new ();
  iter = gst_iterator_new_list (G_TYPE_INT, m, &cookie, &l, NULL, NULL, NULL);
  fail_unless (iter != NULL);

  expected = 0;
  for (i = 0; i < NUM_ELEMENTS; i++)
    expected += i;

  g_value_init (&ret, G_TYPE_INT);
  g_value_set_int (&ret, 0);

  res = gst_iterator_fold (iter, add_fold_func, &ret, NULL);

  fail_unless (res == GST_ITERATOR_DONE);
  fail_unless (g_value_get_int (&ret) == expected);

  /* clean up */
  gst_iterator_free (iter);
  g_mutex_free (m);
  g_list_free (l);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

void (*fn[3]) (void) = {
        test_manual_iteration,
        test_resync,
        test_fold
};

char *args[] = {
        "test_manual_iteration",
        "test_resync",
        "test_fold"
};

GST_CHECK_MAIN (gst_iterator);
//int main()
//    {
//    gst_check_init(NULL,NULL);
//    test_manual_iteration();
//    test_resync();
//    test_fold();
//    
//    }
