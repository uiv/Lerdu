/* GStreamer
 *
 * unit test for GstMiniObject
 *
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
 * Copyright (C) <2005> Tim-Philipp Müller <tim centricular net>
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

#define LOG_FILE "c:\\logs\\gstminiobject_log1.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

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
extern gboolean _gst_check_raised_critical = FALSE;
#endif
//gboolean _gst_check_raised_warning = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_warning,gstcheck,gboolean)
#define _gst_check_raised_warning (*GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_warning = FALSE;
#endif
//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log = FALSE;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running = FALSE;
#endif



#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(thread_list,gstcheck,GList*)
#define thread_list (*GET_GSTREAMER_WSD_VAR_NAME(thread_list,gstcheck,g)())
#else 
extern GList *thread_list = NULL;
#endif

//GMutex *mutex;
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(mutex,gstcheck,GMutex*)
#define mutex (*GET_GSTREAMER_WSD_VAR_NAME(mutex,gstcheck,g)())
#else 
extern GMutex *mutex = NULL;
#endif

//GCond *start_cond;              /* used to notify main thread of thread startups */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
extern GCond *start_cond = NULL;
#endif

//GCond *sync_cond;               /* used to synchronize all threads and main thread */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
extern GCond *sync_cond = NULL;
#endif

void test_copy()
{
  GstBuffer *buffer, *copy;
  
  xmlfile = "gstminiobject_test_copy";
  std_log(LOG_FILENAME_LINE, "Test Started test_copy");

  buffer = gst_buffer_new_and_alloc (4);

  copy = GST_BUFFER (gst_mini_object_copy (GST_MINI_OBJECT (buffer)));

  fail_if (copy == NULL, "Copy of buffer returned NULL");
  fail_unless (GST_BUFFER_SIZE (copy) == 4,
      "Copy of buffer has different size");
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_is_writable()
{
  GstBuffer *buffer;
  GstMiniObject *mobj;
  
  xmlfile = "gstminiobject_test_is_writable";
    std_log(LOG_FILENAME_LINE, "Test Started test_is_writable");

  buffer = gst_buffer_new_and_alloc (4);
  mobj = GST_MINI_OBJECT (buffer);

  fail_unless (gst_mini_object_is_writable (mobj),
      "A buffer with one ref should be writable");

  GST_MINI_OBJECT_FLAG_SET (mobj, GST_MINI_OBJECT_FLAG_READONLY);
  fail_if (gst_mini_object_is_writable (mobj),
      "A buffer with READONLY set should not be writable");
  GST_MINI_OBJECT_FLAG_UNSET (mobj, GST_MINI_OBJECT_FLAG_READONLY);
  fail_unless (gst_mini_object_is_writable (mobj),
      "A buffer with one ref and READONLY not set should be writable");

  fail_if (gst_mini_object_ref (mobj) == NULL, "Could not ref the mobj");

  fail_if (gst_mini_object_is_writable (mobj),
      "A buffer with two refs should not be writable");
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_make_writable()
{
  GstBuffer *buffer;
  GstMiniObject *mobj, *mobj2, *mobj3;
  
  xmlfile = "gstminiobject_test_make_writable";
      std_log(LOG_FILENAME_LINE, "Test Started test_make_writable");

  buffer = gst_buffer_new_and_alloc (4);
  mobj = GST_MINI_OBJECT (buffer);

  mobj2 = gst_mini_object_make_writable (mobj);
  fail_unless (GST_IS_BUFFER (mobj2), "make_writable did not return a buffer");
  fail_unless (mobj == mobj2,
      "make_writable returned a copy for a buffer with refcount 1");

  mobj2 = gst_mini_object_ref (mobj);
  mobj3 = gst_mini_object_make_writable (mobj);
  fail_unless (GST_IS_BUFFER (mobj3), "make_writable did not return a buffer");
  fail_if (mobj == mobj3,
      "make_writable returned same object for a buffer with refcount > 1");

  fail_unless (GST_MINI_OBJECT_REFCOUNT_VALUE (mobj) == 1,
      "refcount of original mobj object should be back to 1");

  mobj2 = gst_mini_object_make_writable (mobj);
  fail_unless (GST_IS_BUFFER (mobj2), "make_writable did not return a buffer");
  fail_unless (mobj == mobj2,
      "make_writable returned a copy for a buffer with refcount 1");
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



static gint num_threads = 10;
static gint refs_per_thread = 10000;

/* test thread-safe refcounting of GstMiniObject */
static void
thread_ref (GstMiniObject * mobj)
{
  int j;

  THREAD_START ();

  for (j = 0; j < refs_per_thread; ++j) {
    gst_mini_object_ref (mobj);

    if (j % num_threads == 0)
      THREAD_SWITCH ();
  }
  GST_DEBUG ("thread stopped");
}

void test_ref_threaded()
{
  GstBuffer *buffer;
  GstMiniObject *mobj;
  gint expected;
  
  xmlfile = "gstminiobject_test_ref_threaded";
  std_log(LOG_FILENAME_LINE, "Test Started test_ref_threaded");

  buffer = gst_buffer_new_and_alloc (4);

  mobj = GST_MINI_OBJECT (buffer);

  MAIN_START_THREADS (num_threads, thread_ref, mobj);

  MAIN_STOP_THREADS ();

  expected = num_threads * refs_per_thread + 1;
  ASSERT_MINI_OBJECT_REFCOUNT (mobj, "miniobject", expected);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



static void
thread_unref (GstMiniObject * mobj)
{
  int j;

  THREAD_START ();

  for (j = 0; j < refs_per_thread; ++j) {
    gst_mini_object_unref (mobj);

    if (j % num_threads == 0)
      THREAD_SWITCH ();
  }
}

void test_unref_threaded()
{
  GstBuffer *buffer;
  GstMiniObject *mobj;
  int i;
  
  xmlfile = "gstminiobject_test_unref_threaded";
    std_log(LOG_FILENAME_LINE, "Test Started test_unref_threaded");

  buffer = gst_buffer_new_and_alloc (4);

  mobj = GST_MINI_OBJECT (buffer);

  for (i = 0; i < num_threads * refs_per_thread; ++i)
    gst_mini_object_ref (mobj);

  MAIN_START_THREADS (num_threads, thread_unref, mobj);

  MAIN_STOP_THREADS ();

  ASSERT_MINI_OBJECT_REFCOUNT (mobj, "miniobject", 1);

  /* final unref */
  gst_mini_object_unref (mobj);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



/* ======== value collection test ======== */
typedef struct _MyFoo
{
  GObject object;
} MyFoo;

typedef struct _MyFooClass
{
  GObjectClass gobject_class;
} MyFooClass;

enum
{
  PROP_BUFFER = 1
};

G_DEFINE_TYPE (MyFoo, my_foo, G_TYPE_OBJECT);

static void
my_foo_init (MyFoo * foo)
{
}

static void
my_foo_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  GstBuffer *new_buf;

  g_assert (prop_id == PROP_BUFFER);

  new_buf = gst_buffer_new_and_alloc (1024);
  gst_value_set_mini_object (value, GST_MINI_OBJECT (new_buf));
  gst_buffer_unref (new_buf);
}

static void
my_foo_set_property (GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  GstMiniObject *mini_obj;

  g_assert (prop_id == PROP_BUFFER);

  mini_obj = gst_value_get_mini_object (value);
  g_assert (GST_IS_MINI_OBJECT (mini_obj));
  g_assert (GST_IS_BUFFER (mini_obj));

#if 0
  /* gst_value_dup_mini_object() does not exist yet */
  mini_obj = gst_value_dup_mini_object (value);
  g_assert (GST_IS_MINI_OBJECT (mini_obj));
  g_assert (GST_IS_BUFFER (mini_obj));
  gst_mini_object_unref (mini_obj);
#endif
}


static void
my_foo_class_init (MyFooClass * klass)
{
  GObjectClass *gobject_klass = G_OBJECT_CLASS (klass);

  gobject_klass->get_property = my_foo_get_property;
  gobject_klass->set_property = my_foo_set_property;

  g_object_class_install_property (gobject_klass, PROP_BUFFER,
      gst_param_spec_mini_object ("buffer", "Buffer",
          "a newly created GstBuffer", GST_TYPE_BUFFER, G_PARAM_READWRITE));
}

void test_value_collection()
{
  GstBuffer *buf = NULL;
  MyFoo *foo;
  
  xmlfile = "gstminiobject_test_value_collection";
  std_log(LOG_FILENAME_LINE, "Test Started test_value_collection");

  foo = (MyFoo *) g_object_new (my_foo_get_type (), NULL);

  /* test g_object_get() refcounting */
  g_object_get (foo, "buffer", &buf, NULL);
  g_assert (GST_IS_BUFFER (buf));
  g_assert (GST_MINI_OBJECT_REFCOUNT_VALUE (GST_MINI_OBJECT_CAST (buf)) == 1);
  gst_buffer_unref (buf);

  /* test g_object_set() refcounting */
  buf = gst_buffer_new_and_alloc (1024);
  g_object_set (foo, "buffer", buf, NULL);
  g_assert (GST_MINI_OBJECT_REFCOUNT_VALUE (GST_MINI_OBJECT_CAST (buf)) == 1);
  gst_buffer_unref (buf);

  g_object_unref (foo);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


void (*fn[]) (void) = {
test_copy,
test_is_writable,
test_make_writable,
test_ref_threaded,
test_unref_threaded,
test_value_collection
};

char *args[] = {
"test_copy",
"test_is_writable",
"test_make_writable",
"test_ref_threaded",
"test_unref_threaded",
"test_value_collection"
};

GST_CHECK_MAIN (gst_mini_object);
