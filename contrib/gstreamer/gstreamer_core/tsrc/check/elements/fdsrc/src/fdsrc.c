/* GStreamer
 *
 * unit test for fdsrc
 *
 * Copyright (C) <2005> Jan Schmidt <thaytan at mad dot scientist dot com>
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gst/check/gstcheck.h>
#include <libgstreamer_wsd_macros.h>
#define TESTFILE "c:\\data\\warning.wav"



#define LOG_FILE "c:\\logs\\fdsrc_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
//char* xmlfile = "fdsrc";

#include "libgstreamer_wsd_solution.h" 

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

#include "libgstreamer_wsd_solution.h" 


static gboolean have_eos = FALSE;

static GstPad *mysinkpad;

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static gboolean
event_func (GstPad * pad, GstEvent * event)
{
  if (GST_EVENT_TYPE (event) == GST_EVENT_EOS) {
    have_eos = TRUE;
    gst_event_unref (event);
    return TRUE;
  }

  gst_event_unref (event);
  return FALSE;
}

static GstElement *
setup_fdsrc (void)
{
  GstElement *fdsrc;

  GST_DEBUG ("setup_fdsrc");
  fdsrc = gst_check_setup_element ("fdsrc");
  mysinkpad = gst_check_setup_sink_pad (fdsrc, &sinktemplate, NULL);
  gst_pad_set_event_function (mysinkpad, event_func);
  gst_pad_set_active (mysinkpad, TRUE);
  return fdsrc;
}

static void
cleanup_fdsrc (GstElement * fdsrc)
{
  gst_pad_set_active (mysinkpad, FALSE);
  gst_check_teardown_sink_pad (fdsrc);
  gst_check_teardown_element (fdsrc);
}

void test_num_buffers()
{
  GstElement *src;
  gint pipe_fd[2];
  gchar data[4096];

  xmlfile = "fdsrc_test_num_buffers";
  std_log(LOG_FILENAME_LINE, "Test Started test_num_buffers");


  fail_if (pipe (pipe_fd) < 0);

  src = setup_fdsrc ();
  g_object_set (G_OBJECT (src), "num-buffers", 3, NULL);
  g_object_set (G_OBJECT (src), "fd", pipe_fd[0], NULL);
  fail_unless (gst_element_set_state (src,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");
  

  memset (data, 0, 4096);
  while (!have_eos) {
    fail_if (write (pipe_fd[1], data, 512) < 0);
  

    //g_usleep (1000000);
  }

  fail_unless (g_list_length (buffers) == 3);
  

  fail_unless (gst_element_set_state (src,
          GST_STATE_NULL) == GST_STATE_CHANGE_SUCCESS, "could not set to null");
  

  /* cleanup */
  cleanup_fdsrc (src);
  close (pipe_fd[0]);
  close (pipe_fd[1]);
  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  create_xml(0);


}



void test_nonseeking()
{
  GstElement *src;
  GstQuery *seeking_query;
  gint pipe_fd[2];
  gchar data[4096];
  gboolean seekable;

  xmlfile = "fdsrc_test_nonseeking";
  std_log(LOG_FILENAME_LINE, "Test Started test_nonseeking");


  fail_if (pipe (pipe_fd) < 0);

  src = setup_fdsrc ();
  g_object_set (G_OBJECT (src), "num-buffers", 3, NULL);
  g_object_set (G_OBJECT (src), "fd", pipe_fd[0], NULL);
  fail_unless (gst_element_set_state (src,
          GST_STATE_PAUSED) == GST_STATE_CHANGE_SUCCESS,
      "could not set to paused");
        


  memset (data, 0, 4096);
  
  fail_if (write (pipe_fd[1], data, 256) < 0);

  /* Test that fdsrc is non-seekable with a pipe */
  fail_unless ((seeking_query = gst_query_new_seeking (GST_FORMAT_BYTES))
      != NULL);
        

  fail_unless (gst_element_query (src, seeking_query) == TRUE);
  

  gst_query_parse_seeking (seeking_query, NULL, &seekable, NULL, NULL);
  fail_unless (seekable == FALSE);
  

  gst_query_unref (seeking_query);

  fail_unless (gst_element_set_state (src,
          GST_STATE_NULL) == GST_STATE_CHANGE_SUCCESS, "could not set to null");
  

  /* cleanup */
  cleanup_fdsrc (src);
  close (pipe_fd[0]);
  close (pipe_fd[1]);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);


}



void test_seeking()
{
  GstElement *src;
  gint in_fd;
  GstQuery *seeking_query;
  gboolean seekable;

  xmlfile = "fdsrc_test_seeking";
  std_log(LOG_FILENAME_LINE, "Test Started test_seeking");


#ifndef TESTFILE
#error TESTFILE not defined
#endif

in_fd = errno ;
  fail_if ((in_fd = open ("c:\\data\\gstreamer\\warning.wav", O_RDONLY)) < 0);
    

  src = setup_fdsrc ();

  g_object_set (G_OBJECT (src), "fd", in_fd, NULL);
  fail_unless (gst_element_set_state (src,
          GST_STATE_PAUSED) == GST_STATE_CHANGE_SUCCESS,
      "could not set to paused");
  

  /* Test that fdsrc is seekable with a file fd */
  fail_unless ((seeking_query = gst_query_new_seeking (GST_FORMAT_BYTES))
      != NULL);
  

  fail_unless (gst_element_query (src, seeking_query) == TRUE);
  

  gst_query_parse_seeking (seeking_query, NULL, &seekable, NULL, NULL);
  fail_unless (seekable == TRUE);
  

  gst_query_unref (seeking_query);

  fail_unless (gst_element_set_state (src,
          GST_STATE_NULL) == GST_STATE_CHANGE_SUCCESS, "could not set to null");
  

  /* cleanup */
  cleanup_fdsrc (src);
  close (in_fd);
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);

  
}

#if 1
void (*fn[3]) (void) = {
   		  test_num_buffers,
        test_nonseeking,
        test_seeking
};

char *args[] = {
 	    	"test_num_buffers",
        "test_nonseeking",
        "test_seeking"
};



GST_CHECK_MAIN (fdsrc);
#endif


#if 0
int main()
{
    
    
	gst_check_init (NULL, NULL);
	test_num_buffers();
	test_nonseeking();
	test_seeking();

}
#endif
