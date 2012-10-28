#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>

#include <gst/gst.h>
#include <gst/base/gstadapter.h>

#define LOG_FILE "c:\\logs\\adapter_test_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

struct TestParams
{
  gint tot_size;
  gint read_size;
  gint write_size;
};

/* This test pushes 'n' buffers of 'write size' into an adapter, then reads
 * them out in 'read size' sized pieces, using take and then take_buffer, 
 * and prints the timings */

static struct TestParams param_sets[] = {
/* These values put ~40MB in 1MB chunks in an adapter, then reads them out
 * in 250kb blocks */
  {40000000, 250000, 1000000},
/* These values put ~4MB in 1000 byte chunks in an adapter, then reads them 
 * out in 200 byte blocks */
  {4000000, 200, 1000},
/* These values put ~40KB in 200 chunks in an adapter, then reads them out
 * in 1000 byte blocks */
  {400000, 1000, 200}
};
static const gint n_tests = sizeof (param_sets) / sizeof (struct TestParams);

static gint ticks_per_sec;


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

static void
run_test_take (struct TestParams *params)
{
  /* Create an adapter and feed it data of fixed size, then retrieve it in 
   * a different size */
  GstAdapter *adapter = gst_adapter_new ();
  guint8 *data;
  GstBuffer *buf;
  int i;
  gint ntimes = params->tot_size / params->write_size;

  for (i = 0; i < ntimes; i++) {
    buf = gst_buffer_new_and_alloc (params->write_size);
    memset (GST_BUFFER_DATA (buf), 0, params->write_size);

    gst_adapter_push (adapter, buf);
  }

  do {
    data = gst_adapter_take (adapter, params->read_size);
    if (data == NULL)
      break;
    g_free (data);
  } while (TRUE);

  g_object_unref (adapter);
}

static void
run_test_take_buffer (struct TestParams *params)
{
  /* Create an adapter and feed it data of fixed size, then retrieve it in 
   * a different size
   */
  GstAdapter *adapter = gst_adapter_new ();
  GstBuffer *buf;
  int i;
  gint ntimes = params->tot_size / params->write_size;

  for (i = 0; i < ntimes; i++) {
    buf = gst_buffer_new_and_alloc (params->write_size);
    memset (GST_BUFFER_DATA (buf), 0, params->write_size);

    gst_adapter_push (adapter, buf);
  }

  do {
    buf = gst_adapter_take_buffer (adapter, params->read_size);
    if (buf == NULL)
      break;
    gst_buffer_unref (buf);
  } while (TRUE);

  g_object_unref (adapter);
}

static void
run_tests (struct TestParams *params)
{
#ifndef __SYMBIAN32__  
  struct tms time_data;
  double  start;
  double dur;
#endif
  
  g_print ("Running on %d bytes, writing %d bytes/buf, reading %d bytes/buf\n",
      params->tot_size, params->write_size, params->read_size);

#ifndef __SYMBIAN32__
  start = 0.0;
#endif
  
  start_time();
  run_test_take (params);
  end_time();
  
  g_print ("Time for take test: %f secs\n", (float)time_diff()/1000000);
  
#ifndef __SYMBIAN32__ 
  times (&time_data);
  dur = (gdouble) (time_data.tms_utime + time_data.tms_stime) / ticks_per_sec;
  g_print ("Time for take test: %g secs\n", dur - start);
  
  start = dur;
#endif  
  
  start_time();
  run_test_take_buffer (params);
  end_time();
  
  g_print ("Time for TakeBuffer test: %f secs\n", (float)time_diff()/1000000);
      
#ifndef __SYMBIAN32__ 
  times (&time_data);
  dur = (gdouble) (time_data.tms_utime + time_data.tms_stime) / ticks_per_sec;
  g_print ("Time for TakeBuffer test: %g secs\n", dur - start);
#endif  
  
}

int
main (int argc, char **argv)
{
  gint i;

	xmlfile = "adapter_test";
  std_log(LOG_FILENAME_LINE, "Test Started adapter_test");
  ticks_per_sec = sysconf (_SC_CLK_TCK);

  gst_init (&argc, &argv);

  for (i = 0; i < n_tests; i++)
    run_tests (param_sets + i);
  
  g_print ("completed testing controller\n");
	std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  return 0;
}
