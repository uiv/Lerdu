/* GStreamer
 *
 * unit test for GstObject
 *
 * Copyright (C) <2004> Thomas Vander Stichele <thomas at apestaart dot org>
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
#include <glib/gthread.h>
#define LOG_FILE "c:\\logs\\gstobject_log1.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


gboolean _gst_check_debug = FALSE;


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



#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(thread_list,gstcheck,GList*)
#define thread_list (*GET_GSTREAMER_WSD_VAR_NAME(thread_list,gstcheck,g)())
#else 
extern GList *thread_list;
#endif

//GMutex *mutex;
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(mutex,gstcheck,GMutex*)
#define mutex (*GET_GSTREAMER_WSD_VAR_NAME(mutex,gstcheck,g)())
#else 
extern GMutex *mutex;
#endif

//GCond *start_cond;              /* used to notify main thread of thread startups */
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
extern GCond *start_cond;
#endif

//GCond *sync_cond;               /* used to synchronize all threads and main thread */
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
extern GCond *sync_cond;
#endif


/*
  Create a fake subclass
 */
typedef struct _GstFakeObjectClass GstFakeObjectClass;
typedef struct _GstFakeObject GstFakeObject;

struct _GstFakeObject
{
  GstObject object;
};

struct _GstFakeObjectClass
{
  GstObjectClass parent_class;
};

static GType _gst_fake_object_type = 0;

//static GstObjectClass *parent_class = NULL;
//static guint gst_fake_object_signals[LAST_SIGNAL] = { 0 };

static GType
gst_fake_object_get_type (void)
{
  if (!_gst_fake_object_type) {
    static const GTypeInfo fake_object_info = {
      sizeof (GstFakeObjectClass),
      NULL,                     //gst_fake_object_base_class_init,
      NULL,                     //gst_fake_object_base_class_finalize,
      NULL,                     //(GClassInitFunc) gst_fake_object_class_init,
      NULL,
      NULL,
      sizeof (GstFakeObject),
      0,
      NULL,                     //(GInstanceInitFunc) gst_fake_object_init,
      NULL
    };

    _gst_fake_object_type = g_type_register_static (GST_TYPE_OBJECT,
        "GstFakeObject", &fake_object_info, 0);
  }
  return _gst_fake_object_type;
}

/* g_object_new on abstract GstObject should fail */
void test_fail_abstract_new()
{
  GstObject *object;
  
  xmlfile = "gstobject_test_fail_abstract_new";
  std_log(LOG_FILENAME_LINE, "Test Started test_fail_abstract_new");

  ASSERT_CRITICAL (object = g_object_new (gst_object_get_type (), NULL));
  fail_unless (object == NULL, "Created an instance of abstract GstObject");
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
      create_xml(0);
}



/* g_object_new on GstFakeObject should succeed */
void test_fake_object_new()
{
  GstObject *object;
  
  xmlfile = "gstobject_test_fake_object_new";
    std_log(LOG_FILENAME_LINE, "Test Started test_fake_object_new");

  object = g_object_new (gst_fake_object_get_type (), NULL);
  fail_if (object == NULL, "Failed to create instance of GstFakeObject");
  fail_unless (GST_IS_OBJECT (object),
      "GstFakeObject instance is not a GstObject");
  gst_object_unref (object);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
      create_xml(0);
}



/* GstFakeObject name tests */
void test_fake_object_name()
{
  GstObject *object;
  gchar *name;
  gchar *name2;
  
  xmlfile = "gstobject_test_fake_object_name";
      std_log(LOG_FILENAME_LINE, "Test Started test_fake_object_name");

  object = g_object_new (gst_fake_object_get_type (), NULL);

  name = gst_object_get_name (object);
  fail_if (name == NULL, "Newly created object has no name");
  fail_if (strncmp (name, "fakeobject", 10) != 0,
      "Random name %s does not start with Gst", name);
  g_free (name);

  /* give a random name by setting with NULL;
   * GstFakeObject class -> fakeobject%d */
  gst_object_set_name (object, NULL);
  name = gst_object_get_name (object);
  fail_if (name == NULL, "Random name was not assigned");
  fail_if (strncmp (name, "fakeobject", 10) != 0,
      "Random name %s does not start with Gst", name);
  g_free (name);

  gst_object_set_name (object, "fake");
  name = gst_object_get_name (object);
  fail_if (name == NULL, "Failed to get name of GstFakeObject");
  fail_if (strcmp (name, "fake") != 0, "Name of GstFakeObject is not 'fake'");

  /* change the gotten name to see that it's a copy and not the original */
  name[0] = 'm';
  name2 = gst_object_get_name (object);
  fail_if (strcmp (name2, "fake") != 0,
      "Copy of object name affected actual object name");
  g_free (name);
  g_free (name2);

  gst_object_unref (object);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
      create_xml(0);
}



/* thread function for threaded name change test */
static gpointer
thread_name_object (GstObject * object)
{
  gchar *thread_id = g_strdup_printf ("%p", g_thread_self ());

  THREAD_START ();

  /* give main thread a head start */
  g_usleep (100000);

  /* write our name repeatedly */
  g_message ("THREAD %s: starting loop\n", thread_id);
  while (THREAD_TEST_RUNNING ()) {
    gst_object_set_name (object, thread_id);
    /* a minimal sleep invokes a thread switch */
    THREAD_SWITCH ();
  }

  /* thread is done, so let's return */
  g_message ("THREAD %s: set name\n", thread_id);
  g_free (thread_id);

  return NULL;
}

#if 0
void test_fake_object_name_threaded_wrong()
{
  GstObject *object;
  gchar *name;
  gint i;
  gboolean expected_failure = FALSE;

  g_message ("\nTEST: set/get without lock\n");

  object = g_object_new (gst_fake_object_get_type (), NULL);
  gst_object_set_name (object, "main");

  MAIN_START_THREADS (5, thread_name_object, object);

  /* start looping and set/get name repeatedly */
  for (i = 0; i < 1000; ++i) {
    gst_object_set_name (object, "main");
    THREAD_SWITCH ();
    name = gst_object_get_name (object);
    if (strcmp (name, "main") != 0) {
      g_message ("MAIN: expected failure during run %d\n", i);
      expected_failure = TRUE;
      g_free (name);
      break;
    }
    g_free (name);
  }
  MAIN_STOP_THREADS ();

  gst_object_unref (object);

  fail_unless (expected_failure, "name did not get changed");
}


#endif

/*
 * main thread sets and gets name directly on struct inside the object lock
 * succeed because lock is held during set/get, and threads are locked out
 */
void test_fake_object_name_threaded_right()
{
  GstObject *object;
  gchar *name;
  gint i;
  
  xmlfile = "gstobject_test_fake_object_name_threaded_right";
        std_log(LOG_FILENAME_LINE, "Test Started test_fake_object_name_threaded_right");

  g_message ("\nTEST: set/get inside lock\n");

  object = g_object_new (gst_fake_object_get_type (), NULL);
  gst_object_set_name (object, "main");

  MAIN_START_THREADS (5, thread_name_object, object);

  /* start looping and set/get name repeatedly */
  for (i = 0; i < 1000; ++i) {
    GST_OBJECT_LOCK (object);
    g_free (GST_OBJECT_NAME (object));
    GST_OBJECT_NAME (object) = g_strdup ("main");
    THREAD_SWITCH ();
    name = g_strdup (GST_OBJECT_NAME (object));
    GST_OBJECT_UNLOCK (object);

    fail_unless (strcmp (name, "main") == 0,
        "Name got changed while lock held during run %d", i);
    g_free (name);
  }
  MAIN_STOP_THREADS ();
  gst_object_unref (object);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
      create_xml(0);
}


/*
 * main thread creates lots of objects
 * child threads sets default names on objects
 * then main thread checks uniqueness of object names
 */

static GList *object_list = NULL;
static gint num_objects = 1000;
static gint num_threads = 5;

/* thread function for threaded default name change test */
static gpointer
thread_name_object_default (int *i)
{
  int j;

  THREAD_START ();

  for (j = *i; j < num_objects; j += num_threads) {
    GstObject *o = GST_OBJECT (g_list_nth_data (object_list, j));

    /* g_message ("THREAD %p: setting default name on object %d\n",
       g_thread_self (), j); */
    gst_object_set_name (o, NULL);
    THREAD_SWITCH ();
  }

  /* thread is done, so let's return */
  g_message ("THREAD %p: set name\n", g_thread_self ());
  g_free (i);

  return NULL;
}

static gint
gst_object_name_compare (GstObject * o, GstObject * p)
{
  gint result;

  GST_OBJECT_LOCK (o);
  GST_OBJECT_LOCK (p);

  if (o->name == NULL && p->name == NULL) {
    result = 0;
  } else if (o->name == NULL) {
    result = -1;
  } else if (p->name == NULL) {
    result = 1;
  } else {
    result = strcmp (o->name, p->name);
  }

  GST_OBJECT_UNLOCK (p);
  GST_OBJECT_UNLOCK (o);

  return result;
}

void test_fake_object_name_threaded_unique()
{
  GstObject *object;
  gint i;
  gint *ip;
  gchar *name1, *name2;
  GList *l;
  
  xmlfile = "gstobject_test_fake_object_name_threaded_unique";
  std_log(LOG_FILENAME_LINE, "Test Started test_fake_object_name_threaded_unique");

  g_message ("\nTEST: uniqueness of default names\n");

  for (i = 0; i < num_objects; ++i) {
    object = g_object_new (gst_fake_object_get_type (), NULL);
    object_list = g_list_append (object_list, object);
  }

  MAIN_INIT ();

 // mark_point ();
  for (i = 0; i < num_threads; ++i) {
    ip = g_new (gint, 1);
    *ip = i;
    MAIN_START_THREAD_FUNCTION (i, thread_name_object_default, ip);
  }

  //mark_point ();
  MAIN_SYNCHRONIZE ();
  //mark_point ();
  MAIN_STOP_THREADS ();

  /* sort GList based on object name */
  /* FIXME: sort and test */
  object_list =
      g_list_sort (object_list, (GCompareFunc) gst_object_name_compare);

  name1 = gst_object_get_name (GST_OBJECT (object_list->data));
  for (l = object_list->next; l->next; l = l->next) {
    g_message ("object with name %s\n", name1);
    name2 = gst_object_get_name (GST_OBJECT (l->data));
    fail_if (strcmp (name1, name2) == 0, "Two objects with name %s", name2);
    g_free (name1);
    name1 = name2;
  }
  g_free (name1);

  /* free stuff */
  g_list_foreach (object_list, (GFunc) g_object_unref, NULL);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
      create_xml(0);
}



/* parentage test on GstFakeObject */
void test_fake_object_parentage()
{
  GstObject *object1, *object2;
  GstObject *parent;
  gboolean result;
  
  xmlfile = "gstobject_test_fake_object_parentage";
    std_log(LOG_FILENAME_LINE, "Test Started test_fake_object_parentage");

  /* create new object */
  object1 = g_object_new (gst_fake_object_get_type (), NULL);
  fail_if (object1 == NULL, "Failed to create instance of GstFakeObject");
  fail_unless (GST_IS_OBJECT (object1),
      "GstFakeObject instance is not a GstObject");
  fail_unless (GST_OBJECT_IS_FLOATING (object1),
      "GstFakeObject instance is not floating");

  /* check the parent */
  parent = gst_object_get_parent (object1);
  fail_if (parent != NULL, "GstFakeObject has parent");
  /* try to set a NULL parent, this should give a warning */
#ifndef __SYMBIAN32__
  ASSERT_CRITICAL (result = gst_object_set_parent (object1, NULL));
  result = gst_object_set_parent (object1, NULL);
  fail_if (result == TRUE, "GstFakeObject accepted NULL parent");
  /* try to set itself as parent, we expect a warning here */
  //ASSERT_CRITICAL (result = gst_object_set_parent (object1, object1));
  fail_if (result == TRUE, "GstFakeObject accepted itself as parent");
#endif
  
  /* should still be floating */
  fail_unless (GST_OBJECT_IS_FLOATING (object1),
      "GstFakeObject instance is not floating");

  /* create another object */
  object2 = g_object_new (gst_fake_object_get_type (), NULL);
  fail_if (object2 == NULL,
      "Failed to create another instance of GstFakeObject");
  fail_unless (GST_IS_OBJECT (object2),
      "second GstFakeObject instance is not a GstObject");
  fail_unless (GST_OBJECT_IS_FLOATING (object1),
      "GstFakeObject instance is not floating");

  /* try to set other object as parent */
  result = gst_object_set_parent (object1, object2);
  fail_if (result == FALSE,
      "GstFakeObject could not accept other object as parent");

  /* should not be floating anymore */
  fail_if (GST_OBJECT_IS_FLOATING (object1),
      "GstFakeObject instance is still floating");
  /* parent should still be floating */
  fail_unless (GST_OBJECT_IS_FLOATING (object2),
      "GstFakeObject instance is not floating");

  /* check the parent */
  parent = gst_object_get_parent (object1);
  fail_if (parent != object2, "GstFakeObject has wrong parent");
  gst_object_unref (parent);
  /* try to set other object as parent again */
  result = gst_object_set_parent (object1, object2);
  fail_if (result == TRUE, "GstFakeObject could set parent twice");

  /* ref before unparenting */
  gst_object_ref (object1);
  /* clear parent of object */
  gst_object_unparent (object1);

  /* check the parent */
  parent = gst_object_get_parent (object1);
  fail_if (parent != NULL, "GstFakeObject has parent");

  /* object should not be floating */
  fail_if (GST_OBJECT_IS_FLOATING (object1),
      "GstFakeObject instance is floating again");

  gst_object_unref (object1);
  gst_object_unref (object2);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
      create_xml(0);
}



/* parentage test dispose on GstFakeObject, since our testcase
 * does not handle the parent relation completely, the parent does
 * not hold a ref to the child, we cannot dispose the parent to
 * dipose the child as well. This test needs to be run with DEBUG
 * info to check if the finalize methods are called correctly. */
void test_fake_object_parentage_dispose()
{
  GstObject *object1, *object2;
  gboolean result;
  
  xmlfile = "gstobject_test_fake_object_parentage_dispose";
      std_log(LOG_FILENAME_LINE, "Test Started test_fake_object_parentage_dispose");

  object1 = g_object_new (gst_fake_object_get_type (), NULL);
  fail_if (object1 == NULL, "Failed to create instance of GstFakeObject");

  object2 = g_object_new (gst_fake_object_get_type (), NULL);
  fail_if (object2 == NULL, "Failed to create instance of GstFakeObject");

  /* try to set other object as parent */
  result = gst_object_set_parent (object1, object2);
  fail_if (result == FALSE,
      "GstFakeObject could not accept other object as parent");

  /* clear parent of object */
  gst_object_unparent (object1);

  /* now dispose parent */
  gst_object_unref (object2);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
      create_xml(0);
}



/* test: try renaming a parented object, make sure it fails */


void (*fn[]) (void) = {
test_fake_object_new,
test_fake_object_name,
#if 0
test_fake_object_name_threaded_wrong,
#endif
test_fake_object_name_threaded_right,
test_fake_object_name_threaded_unique,
test_fake_object_parentage,
test_fake_object_parentage_dispose,
test_fail_abstract_new
};

char *args[] = {
"test_fake_object_new",//ok
"test_fake_object_name",//ok
#if 0
"test_fake_object_name_threaded_wrong",
#endif
"test_fake_object_name_threaded_right",//ok
"test_fake_object_name_threaded_unique",//ok
"test_fake_object_parentage",
"test_fake_object_parentage_dispose",//ok
"test_fail_abstract_new"
};

GST_CHECK_MAIN (gst_object);

    
