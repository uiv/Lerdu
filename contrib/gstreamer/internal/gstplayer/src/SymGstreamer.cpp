/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/

#include "gstreamer.h"
#include "GlibEventHandler.h"
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <GSTPlayer.rsg>
#include <CAknMemorySelectionDialog.h>
#include <e32base.h>
#include <stdlib.h>
//#include <mmf/server/sounddevice.h>

#include "gstplayerappview.h"

GstElement *pipeline, *source, *wavparse,*sink,*decoder,*conv,*resample,*record,*fakesink,*filesink,*encoder,*filter,*wavenc, *auparse, *mulawdec;
GstBus *bus;
GstCaps* caps;
GstState current,pending;
char carray[1024];

CGSTPlayerAppView *iGstView;

GstPad *dssinkpad;
GstCaps *dssinkcap;
GstPad *dssrcpad;
GstCaps *dssrccap;
GstStructure *str;

gboolean negcaps = FALSE;

static gboolean print_field (GQuark field, const GValue *value, gpointer pfx);
static void print_caps (const GstCaps *caps, const gchar *pfx);



static void
cb_raw_playback_handoff (GstElement *src, GstBuffer *buffer, GstPad *pad,
        gpointer user_data)
    {
    static gint readbytes = 0;
    static gboolean eofReached = FALSE;

    size_t readsize;
    gint size;
    FILE *f;
    GstCaps *bufCaps;
    if ( eofReached == TRUE )
        {
        //gst_element_set_state (pipeline, GST_STATE_NULL);
       // gst_object_unref (GST_OBJECT (pipeline));
        if ( gst_element_send_event (src, gst_event_new_eos ()) == TRUE )
            {
            g_print ("posted eos");
           }
     
       else
           {
            //g_print ("unable to post eos");
          }
       return;
       }

    readsize = 0;
    f = fopen (carray, "r");
    eofReached = TRUE;
    if ( fseek(f, readbytes, 0) == 0 )
        {
        readsize = fread (GST_BUFFER_DATA (buffer),1,GST_BUFFER_SIZE (buffer),f);
        eofReached = FALSE;
        GST_BUFFER_SIZE (buffer) = readsize;
        }
    fclose(f);

    size = GST_BUFFER_SIZE (buffer);
    readbytes += readsize;
    if ( (readsize < size) || (readsize == 0) )
        {
        eofReached = TRUE;
        }
    
    bufCaps = gst_caps_new_simple ("audio/x-raw-int",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT, 8000,
              "channels", G_TYPE_INT, 1, NULL);
    
    gst_buffer_set_caps(buffer,bufCaps);

    }

static void
cb_play_mp3_handoff (GstElement *fakesrc,
        GstBuffer  *buffer,
        GstPad     *pad,
        gpointer    user_data)
{
GstCaps *bufCaps;
  static gint readbytes = 0;
  size_t readsize = 0;
  int size = GST_BUFFER_SIZE (buffer);
  
  FILE *f = fopen (carray, "r");
  fseek(f,readbytes,0);
  readsize = fread(GST_BUFFER_DATA (buffer),1,GST_BUFFER_SIZE (buffer),f);
  readbytes += readsize;
  GST_BUFFER_SIZE (buffer) = readsize;
  if(readsize == 0)
      {
      gst_element_send_event(fakesrc,gst_event_new_eos());
      }
  fclose(f);
 
}
static void
cb_record_raw_handoff (GstElement *fakesrc,
        GstBuffer  *buffer,
        GstPad     *pad,
        gpointer    user_data)
{
  int size = GST_BUFFER_SIZE (buffer);
  //g_print ("[%u]", size);
  FILE *f = fopen (carray, "a");
  fwrite(GST_BUFFER_DATA (buffer),1,GST_BUFFER_SIZE (buffer),f);
  fclose(f);
}


static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        gst_element_set_state (pipeline, GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (pipeline));
      break;
    case GST_MESSAGE_ERROR: {
      gchar *debug;
      GError *err;
      gst_message_parse_error (msg, &err, &debug);
      g_free (debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      break;
    }
    default:
      break;
  }

  return TRUE;
}


static void 
new_pad_cb (GstElement *wavparse, GstPad *new_pad, gpointer pipeline)
 {
   //GstElement *sink,*conv,*resample;
   gst_element_set_state ((_GstElement *)pipeline, GST_STATE_PAUSED);
   sink = gst_element_factory_make ("devsoundsink", "sink");
   conv = gst_element_factory_make ("audioconvert", "audioconvert");
  	if (!conv) {
    	g_print ("could not create \"audioconvert\" element!");
    	return;
  		}
   resample = gst_element_factory_make ("audioresample", "audioresample");
  	if (!resample) {
    	g_print ("could not create \"audioresample\" element!");
    	return ;
  		}
   gst_bin_add_many(GST_BIN (pipeline), conv, resample, sink, NULL);
   
  // if (!gst_element_link (wavparse, sink))
     // g_error ("link(wavparse, sink) failed!\n");
     
 	if(! gst_element_link_many (wavparse,conv, resample, sink, NULL))
  		g_print ("link(wavparse,conv,remaple sink) failed!\n");  
 
   	gst_element_set_state ((_GstElement *)pipeline, GST_STATE_PLAYING);
 }

 #define FILENAME 1024
 int GstreamerNew(TFileName filename)
 {
  size_t ret;
  
  ret = wcstombs(carray, (const wchar_t *)filename.PtrZ(), FILENAME);
  int file_type=0;
  
  
  
  	char *p;
	p = strrchr(carray, '.');

	if ((p != NULL) && (strcmp(p, ".mp3") == 0))
	{
	file_type=2;	
	}
	else if ((p != NULL) && (strcmp(p, ".wav") == 0))
	{
	file_type=1;	
	}
    else if ((p != NULL) && (strcmp(p, ".raw") == 0))
    {
    file_type=3;    
    }
    else if ((p != NULL) && (strcmp(p, ".amr") == 0))
    {
    file_type=4;    
    }
    else if ((p != NULL) && (strcmp(p, ".g711") == 0))
    {
    file_type=5;    
    }
    else if ((p != NULL) && (strcmp(p, ".g729") == 0))
    {
    file_type=6;    
    }
    else if ((p != NULL) && (strcmp(p, ".lbc") == 0))
    {
    file_type=7;    
    }
    else if ((p != NULL) && (strcmp(p, ".au") == 0))
    {
    file_type=8;    
    }	
    else if ((p != NULL) && (strcmp(p, ".aac") == 0))
    {
    file_type=9;    
    }	
	else
		return -1;
	
	if(file_type==1)
	{
		gst_play_wave();
	}
	else if(file_type==2)
	{
		gst_play_mp3();
	}
    else if(file_type==3)
    {
        gst_play_raw();
    }
    else if(file_type==4)
    {
        gst_play_amr();
    }
    else if(file_type==5)
    {
        gst_play_g711();
    }
    else if(file_type==6)
    {
        gst_play_g729();
    }
    else if(file_type==7)
    {
        gst_play_ilbc();
    }
    else if(file_type==8)
    {
        gst_play_au();
    }	

	else
	{
		return 0;
	}
 } 
 
 
int gst_play_mp3()
{
 /* create elements */
 gboolean link_ok;
 
 pipeline = gst_pipeline_new ("pipeline");
 source = gst_element_factory_make ("filesrc", "filesrc");
 //sink = gst_element_factory_make ("fakesink", "sink");
 sink = gst_element_factory_make ("devsoundsink", "sink");

 caps = gst_caps_new_simple ("audio/mp3",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT, 16000,
              "channels", G_TYPE_INT, 2, NULL);
             
 bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
 gst_bus_add_watch( bus, bus_call, NULL);
 gst_object_unref (bus);

 g_object_set (G_OBJECT (source), "location", carray, NULL);

 gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
 
 //gst_element_link (source, sink);
 link_ok = gst_element_link_filtered (source, sink, caps);

 gst_element_set_state (pipeline, GST_STATE_PLAYING);

 return 0;  
}
 
 int gst_play_wave()
 {
    /* create elements */
  pipeline = gst_pipeline_new ("pipeline");
  source = gst_element_factory_make ("filesrc", "pavsrc");
  wavparse = gst_element_factory_make ("wavparse", "parse");
 
  /* set filename property on the file source */
  g_object_set (G_OBJECT (source), "location", carray, NULL);
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, NULL);
  gst_object_unref (bus);
  gst_bin_add_many (GST_BIN (pipeline), source, wavparse, NULL);
  if (!gst_element_link (source, wavparse))
     g_error ("link(src, wavparse) failed!\n");

  
  g_signal_connect (wavparse, "pad-added", G_CALLBACK (new_pad_cb),pipeline);
    
  gst_element_set_state (pipeline, GST_STATE_PLAYING); 
  return 0; 
 }
 
 static void 
 new_pad_cb_for_auplayback (GstElement *wavparse, GstPad *new_pad, gpointer pipeline)
  {
    //GstElement *sink,*conv,*resample;
    gst_element_set_state ((_GstElement *)pipeline, GST_STATE_PAUSED);
    sink = gst_element_factory_make ("devsoundsink", "sink");
    if (!sink) {
        g_print ("could not create \"devsoundsink\" element!");
        return;
        }
    
    mulawdec = gst_element_factory_make ("mulawdec", "mulawdec");
    if (!mulawdec) {
        g_print ("could not create \"mulawdec\" element!");
        return;
        }    
    conv = gst_element_factory_make ("audioconvert", "audioconvert");
     if (!conv) {
         g_print ("could not create \"audioconvert\" element!");
         return;
         }
    resample = gst_element_factory_make ("audioresample", "audioresample");
     if (!resample) {
         g_print ("could not create \"audioresample\" element!");
         return ;
         }
    gst_bin_add_many(GST_BIN (pipeline), mulawdec, conv, resample, sink, NULL);
    
   // if (!gst_element_link (wavparse, sink))
      // g_error ("link(wavparse, sink) failed!\n");
      
     if(! gst_element_link_many (auparse,mulawdec,conv, resample, sink, NULL))
         g_print ("link(wavparse,conv,remaple sink) failed!\n");  
  
     gst_element_set_state ((_GstElement *)pipeline, GST_STATE_PLAYING);
  }
 
 int gst_play_au()
 {
    /* create elements */
  pipeline = gst_pipeline_new ("pipeline");
  source = gst_element_factory_make ("filesrc", "ausrc");
  auparse = gst_element_factory_make ("auparse", "parse");
 
  /* set filename property on the file source */
  g_object_set (G_OBJECT (source), "location", carray, NULL);
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, NULL);
  gst_object_unref (bus);
  gst_bin_add_many (GST_BIN (pipeline), source, auparse, NULL);
  if (!gst_element_link (source, auparse))
     g_error ("link(src, auparse) failed!\n");

  
  g_signal_connect (auparse, "pad-added", G_CALLBACK (new_pad_cb_for_auplayback),pipeline);
    
  gst_element_set_state (pipeline, GST_STATE_PLAYING); 
  return 0; 
 }
 
 int gst_record_wav()
    {

    /* create a new bin to hold the elements */
    pipeline = gst_pipeline_new("pipeline");

    record = gst_element_factory_make("devsoundsrc", "record_audio");
    if (!record)
        {
        g_print("could not create \"record\" element!");
        return -1;
        }
    filesink = gst_element_factory_make("filesink", "filesink");
    if (!filesink)
        {
        g_print("could not create \"filesink\" element!");
        return -1;
        }

    wavenc = gst_element_factory_make("wavenc", "wavencoder");
    if (!wavenc)
        {
        g_print("could not create \"wavenc\" element!");
        return -1;
        }

    _LIT(KFILENAME,"c:\\data\\test.wav");
    TFileName fn;
    fn.Append(KFILENAME);
    TInt ret;
    // char carray[FILENAME];
    ret = wcstombs(carray, (const wchar_t *)fn.PtrZ(), FILENAME);

    g_object_set(G_OBJECT (filesink), "location", carray, NULL);
    bus = gst_pipeline_get_bus(GST_PIPELINE (pipeline));
    gst_bus_add_watch(bus, bus_call, NULL);
    gst_object_unref(bus);

    /* add objects to the main pipeline */
    gst_bin_add_many(GST_BIN (pipeline),record,wavenc,filesink, NULL);

    /* link the elements */
    //gst_element_link_many(record, wavenc,filesink, NULL);
    caps = gst_caps_new_simple ("audio/x-raw-int",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT, 16000,
              "channels", G_TYPE_INT, 1, NULL);
    
    gst_element_link_filtered (record, wavenc, caps);
    gst_element_link (wavenc, filesink);
    gst_caps_unref (caps);

    /* start recording */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    return 0;
    }
 
 int gst_record_amr()
 {
 iGstView->DrawText(_L("Recording AMR"),KRgbRed);

   /* create a new bin to hold the elements */
  pipeline = gst_pipeline_new ("pipeline");
  
  //g_print ("pipeline created");
  record = gst_element_factory_make ("devsoundsrc", "record_audio"); 
 // encoder = gst_element_factory_make ("wavenc", NULL); 
  if (!record) {
    g_print ("could not create \"record\" element!");
    iGstView->DrawText(_L("Devsound src not available"),KRgbRed);
    return -1;
  }

  filesink = gst_element_factory_make("filesink", "filesink");
      if (!filesink)
          {
          g_print("could not create \"filesink\" element!");
          return -1;
          }

  caps = gst_caps_new_simple ("audio/amr",
             "width", G_TYPE_INT, 8,
             "depth", G_TYPE_INT, 8,
             "signed",G_TYPE_BOOLEAN, TRUE,
             "endianness",G_TYPE_INT, G_BYTE_ORDER,
             "rate", G_TYPE_INT,  8000,
             "channels", G_TYPE_INT, 1, NULL);
  
  
  g_object_set (G_OBJECT (record),
            "blocksize", 1280,
             NULL);
  
  _LIT(KFILENAME,"c:\\data\\recordtest.amr");
  TFileName fn;
  fn.Append(KFILENAME);
  TInt ret;
  ret = wcstombs(carray, (const wchar_t *)fn.PtrZ(), FILENAME);
  g_object_set(G_OBJECT (filesink), "location", carray, NULL);
   
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, NULL);
  gst_object_unref (bus);
  

  /* add objects to the main pipeline */
  gst_bin_add_many(GST_BIN (pipeline),record,filesink , NULL);
  /* link the elements */
  gst_element_link_filtered (record, filesink, caps);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  
  return 0;
 }
 
 int gst_record_g711()
  {
    /* create a new bin to hold the elements */
   pipeline = gst_pipeline_new ("pipeline");
   
   //g_print ("pipeline created");
   record = gst_element_factory_make ("devsoundsrc", "record_audio"); 
  // encoder = gst_element_factory_make ("wavenc", NULL); 
   if (!record) {
     g_print ("could not create \"record\" element!");
     return -1;
   }

   filesink = gst_element_factory_make("filesink", "filesink");
       if (!filesink)
           {
           g_print("could not create \"filesink\" element!");
           return -1;
           }

   caps = gst_caps_new_simple ("audio/x-alaw",
              "width", G_TYPE_INT, 8,
              "depth", G_TYPE_INT, 8,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT,  8000,
              "channels", G_TYPE_INT, 1, NULL);
   
   g_object_set (G_OBJECT (record),
             "blocksize", 1280,
              NULL);
   
   _LIT(KFILENAME,"c:\\data\\recordtest.g711");
   TFileName fn;
   fn.Append(KFILENAME);
   TInt ret;
   ret = wcstombs(carray, (const wchar_t *)fn.PtrZ(), FILENAME);
   g_object_set(G_OBJECT (filesink), "location", carray, NULL);
    
   bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
   gst_bus_add_watch (bus, bus_call, NULL);
   gst_object_unref (bus);
   

   /* add objects to the main pipeline */
   gst_bin_add_many(GST_BIN (pipeline),record,filesink , NULL);
   /* link the elements */
   gst_element_link_filtered (record, filesink, caps);
   gst_element_set_state (pipeline, GST_STATE_PLAYING);
   
   return 0;
  }
 
 int gst_record_g729()
  {
    /* create a new bin to hold the elements */
   pipeline = gst_pipeline_new ("pipeline");
   
   record = gst_element_factory_make ("devsoundsrc", "record_audio"); 
   if (!record) {
     g_print ("could not create \"record\" element!");
     return -1;
   }

   filesink = gst_element_factory_make("filesink", "filesink");
       if (!filesink)
           {
           g_print("could not create \"filesink\" element!");
           return -1;
           }

   caps = gst_caps_new_simple ("audio/g729",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT,  8000,
              "channels", G_TYPE_INT, 1, NULL);
   
   g_object_set (G_OBJECT (record),
             "blocksize", 1280,
              NULL);
   
   _LIT(KFILENAME,"c:\\data\\recordtest.g729");
   TFileName fn;
   fn.Append(KFILENAME);
   TInt ret;
   ret = wcstombs(carray, (const wchar_t *)fn.PtrZ(), FILENAME);
   g_object_set(G_OBJECT (filesink), "location", carray, NULL);
    
   bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
   gst_bus_add_watch (bus, bus_call, NULL);
   gst_object_unref (bus);
   

   /* add objects to the main pipeline */
   gst_bin_add_many(GST_BIN (pipeline),record,filesink , NULL);
   /* link the elements */
   gst_element_link_filtered (record, filesink, caps);
   gst_element_set_state (pipeline, GST_STATE_PLAYING);
   
   return 0;
  }
 
 int gst_record_ilbc()
  {
    /* create a new bin to hold the elements */
   pipeline = gst_pipeline_new ("pipeline");
   
   record = gst_element_factory_make ("devsoundsrc", "record_audio"); 
   if (!record) {
     g_print ("could not create \"record\" element!");
     return -1;
   }

   filesink = gst_element_factory_make("filesink", "filesink");
       if (!filesink)
           {
           g_print("could not create \"filesink\" element!");
           return -1;
           }

   caps = gst_caps_new_simple ("audio/ilbc",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT,  8000,
              "channels", G_TYPE_INT, 1, NULL);
   
   g_object_set (G_OBJECT (record),
             "blocksize", 1280,
              NULL);
   
   _LIT(KFILENAME,"c:\\data\\recordtest.lbc");
   TFileName fn;
   fn.Append(KFILENAME);
   TInt ret;
   ret = wcstombs(carray, (const wchar_t *)fn.PtrZ(), FILENAME);
   g_object_set(G_OBJECT (filesink), "location", carray, NULL);
    
   bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
   gst_bus_add_watch (bus, bus_call, NULL);
   gst_object_unref (bus);
   

   /* add objects to the main pipeline */
   gst_bin_add_many(GST_BIN (pipeline),record,filesink , NULL);
   /* link the elements */
   gst_element_link_filtered (record, filesink, caps);
   gst_element_set_state (pipeline, GST_STATE_PLAYING);
   
   return 0;
  }

 int gst_record_raw()
 {
 
   /* create a new bin to hold the elements */
  pipeline = gst_pipeline_new ("pipeline");
  
  //g_print ("pipeline created");
  record = gst_element_factory_make ("devsoundsrc", "record_audio"); 
 // encoder = gst_element_factory_make ("wavenc", NULL); 
  if (!record) {
    g_print ("could not create \"record\" element!");
    return -1;
  }
  //g_print ("record created");
  filesink = gst_element_factory_make("filesink", "filesink");
      if (!filesink)
          {
          g_print("could not create \"filesink\" element!");
          return -1;
          }
  //GstRingBufferSpec
  //g_print ("sink created");
  caps = gst_caps_new_simple ("audio/x-raw-int",
             "width", G_TYPE_INT, 16,
             "depth", G_TYPE_INT, 16,
             "signed",G_TYPE_BOOLEAN, TRUE,
             "endianness",G_TYPE_INT, G_BYTE_ORDER,
             "rate", G_TYPE_INT,  8000,
             "channels", G_TYPE_INT, 1, NULL);
  
  //g_print ("caps created");
   g_object_set (G_OBJECT (record),
             //"signal-handoffs", TRUE,
             "blocksize", 1280,
          // "gain",10000,
              NULL);
  
  /*g_object_set (G_OBJECT (fakesink),
             "signal-handoffs", TRUE,
             "sizemax", 4096,
             "sizetype", 2, NULL);*/
  
  _LIT(KFILENAME,"c:\\data\\test.raw");
  TFileName fn;
  fn.Append(KFILENAME);
  TInt ret;
  //char carray[FILENAME];
  carray[0]='\0';
  ret = wcstombs(carray, (const wchar_t *)fn.PtrZ(), FILENAME);
   
   
  g_object_set (G_OBJECT (filesink), "location", carray,"buffer-size",1280, NULL);
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, NULL);
  gst_object_unref (bus);
  

  /* add objects to the main pipeline */
  gst_bin_add_many(GST_BIN (pipeline),record,filesink , NULL);
  //g_print ("added to pipe");
  /* link the elements */
  //gst_element_link(record,/*encoder,*/ fakesink/*audiosink*/);
  gst_element_link_filtered (record, filesink, caps);
  //g_signal_connect (fakesink, "handoff", G_CALLBACK (cb_record_raw_handoff), NULL);
  /* start recording */
 // g_print ("start pipe");
 
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  
  return 0;
 }
     
 
 int gst_play_raw()
 {
 /* create elements */
 gboolean link_ok;
 
 pipeline = gst_pipeline_new ("pipeline");
 source = gst_element_factory_make ("filesrc", "filesrc");
 //sink = gst_element_factory_make ("fakesink", "sink");
 sink = gst_element_factory_make ("devsoundsink", "sink");

caps = gst_caps_new_simple ("audio/x-raw-int",
             "width", G_TYPE_INT, 16,
             "depth", G_TYPE_INT, 16,
             "signed",G_TYPE_BOOLEAN, TRUE,
             "endianness",G_TYPE_INT, G_BYTE_ORDER,
             "rate", G_TYPE_INT,  8000,
             "channels", G_TYPE_INT, 1, NULL);
             
 bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
 gst_bus_add_watch( bus, bus_call, NULL);
 gst_object_unref (bus);

 /*g_object_set (G_OBJECT(source),
         "signal-handoffs", TRUE,
         //"num-buffers", 4,
         "sizemax", 4096,
         "sizetype", 2,
         NULL);*/
 
 g_object_set (G_OBJECT (source), "location", carray, NULL);
// g_signal_connect (source, "handoff", G_CALLBACK (cb_raw_playback_handoff), NULL);

 gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
 
 //gst_element_link (source, sink);
 link_ok = gst_element_link_filtered (source, sink, caps);

 gst_element_set_state (pipeline, GST_STATE_PLAYING);

while (caps->refcount > 1)
     {
     gst_caps_unref (caps);
     }
 gst_caps_unref (caps);

 
 return 0; 
 }

 int gst_play_amr()
  {
  /* create elements */
  gboolean link_ok;
  
  pipeline = gst_pipeline_new ("pipeline");
  source = gst_element_factory_make ("filesrc", "filesrc");
  //sink = gst_element_factory_make ("fakesink", "sink");
  sink = gst_element_factory_make ("devsoundsink", "sink");

 caps = gst_caps_new_simple ("audio/amr",
              "width", G_TYPE_INT, 8,
              "depth", G_TYPE_INT, 8,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT,  8000,
              "channels", G_TYPE_INT, 1, NULL);
              
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch( bus, bus_call, NULL);
  gst_object_unref (bus);

  g_object_set (G_OBJECT (source), "location", carray, NULL);

  gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
  
  link_ok = gst_element_link_filtered (source, sink, caps);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  return 0; 
  }

 int gst_play_g711()
  {
  /* create elements */
  gboolean link_ok;
  
  pipeline = gst_pipeline_new ("pipeline");
  source = gst_element_factory_make ("filesrc", "filesrc");
  sink = gst_element_factory_make ("devsoundsink", "sink");

 caps = gst_caps_new_simple ("audio/x-alaw",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT,  8000,
              "channels", G_TYPE_INT, 1, NULL);
              
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch( bus, bus_call, NULL);
  gst_object_unref (bus);

  g_object_set (G_OBJECT (source), "location", carray, NULL);
  gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
  
  link_ok = gst_element_link_filtered (source, sink, caps);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  return 0; 
  }
 int gst_play_g729()
  {
  /* create elements */
  gboolean link_ok;
  
  pipeline = gst_pipeline_new ("pipeline");
  source = gst_element_factory_make ("filesrc", "filesrc");
  sink = gst_element_factory_make ("devsoundsink", "sink");

 caps = gst_caps_new_simple ("audio/g729",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT,  8000,
              "channels", G_TYPE_INT, 1, NULL);
              
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch( bus, bus_call, NULL);
  gst_object_unref (bus);

  g_object_set (G_OBJECT (source), "location", carray, NULL);

  gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
  
  link_ok = gst_element_link_filtered (source, sink, caps);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  return 0; 
  }
 int gst_play_ilbc()
  {
  /* create elements */
  gboolean link_ok;
  
  pipeline = gst_pipeline_new ("pipeline");
  source = gst_element_factory_make ("filesrc", "filesrc");
  sink = gst_element_factory_make ("devsoundsink", "sink");

 caps = gst_caps_new_simple ("audio/ilbc",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT,  8000,
              "channels", G_TYPE_INT, 1, NULL);
              
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch( bus, bus_call, NULL);
  gst_object_unref (bus);

  g_object_set (G_OBJECT (source), "location", carray, NULL);

  gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
  
  link_ok = gst_element_link_filtered (source, sink, caps);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  return 0; 
  }

 
 //******
 int gst_current_volume()
     {     
     if(!sink)
         {
         iGstView->DrawText(_L("Devsound sink not available"),KRgbRed);
         return -1;
         }
     int vol;
     TBuf<25> currentvolume(_L("current volume "));    
     g_object_get(G_OBJECT(sink),
                    "volume",&vol,NULL);
     
     currentvolume.AppendNum(vol);
     
     iGstView->DrawText(currentvolume,KRgbBlack);
     return 0;
     }
 
 int gst_max_volume()
      {    
      if(!sink)
           {
           iGstView->DrawText(_L("Devsound sink not available"),KRgbRed);
           return -1;
           }
      
      int maxvol;
      TBuf<25> maxvolume(_L("max volume "));   
      
      g_object_get(G_OBJECT(sink),
                     "maxvolume",&maxvol,NULL);
      
      maxvolume.AppendNum(maxvol);
      
      iGstView->DrawText(maxvolume,KRgbBlack);
      return 0;
      
      }
 int gst_volume_up()
     {
     
     if(!sink)
          {
          iGstView->DrawText(_L("Devsound sink not available"),KRgbRed);
          return -1;
          }
     iGstView->DrawText(_L("volume up"),KRgbBlack);
     int maxvol;
     g_object_get(G_OBJECT(sink),
                       "maxvolume",&maxvol,NULL);
     g_object_set (G_OBJECT (sink), 
                        "volume", maxvol, NULL);
     return 0;
     }

 int gst_volume_down()
     {
     if(!sink)
          {
          iGstView->DrawText(_L("Devsound sink not available"),KRgbRed);
          return -1;
          }
     
     iGstView->DrawText(_L("volume down"),KRgbBlack);
     int maxvol;
     g_object_get(G_OBJECT(sink),
                        "maxvolume",&maxvol,NULL);
     g_object_set (G_OBJECT (sink), 
                        "volume", maxvol/2, NULL);
     return 0;
     }
 
 int gst_current_gain()
     {     
     if(!record)
         {
         iGstView->DrawText(_L("Devsound source not available"),KRgbRed);
         return -1;
         }
     int gain;
     TBuf<25> currentgain(_L("current gain "));    
     g_object_get(G_OBJECT(record),
                    "gain",&gain,NULL);
     
     currentgain.AppendNum(gain);
     
     iGstView->DrawText(currentgain,KRgbBlack);
     return 0;
     }
 
 int gst_max_gain()
      {    
      if(!record)
           {
           iGstView->DrawText(_L("Devsound source not available"),KRgbRed);
           return -1;
           }
      
      int max;
      TBuf<25> maxgain(_L("max gain "));   
      
      g_object_get(G_OBJECT(record),
                     "maxgain",&max,NULL);
      
      maxgain.AppendNum(max);
      
      iGstView->DrawText(maxgain,KRgbBlack);
      return 0;
      
      }
 int gst_gain_up()
     {
     
     if(!record)
          {
          iGstView->DrawText(_L("Devsound source not available"),KRgbRed);
          return -1;
          }
     int max;
     g_object_get(G_OBJECT(record),
                      "maxgain",&max,NULL);
     
     iGstView->DrawText(_L("gain up"),KRgbBlack);
     g_object_set (G_OBJECT (record), 
                        "gain", max, NULL);
     return 0;
     }

 int gst_gain_down()
     {
     if(!record)
          {
          iGstView->DrawText(_L("Devsound source not available"),KRgbRed);
          return -1;
          }
     int max;
     g_object_get(G_OBJECT(record),
                      "maxgain",&max,NULL);
     iGstView->DrawText(_L("gain down"),KRgbBlack);
     g_object_set (G_OBJECT (sink), 
                        "gain", max/2, NULL);
     return 0;
     }
 
 
 
 int gst_balance()
     {
     if(!sink)
           {
           iGstView->DrawText(_L("Devsound sink not available"),KRgbRed);
           return -1;
           }
      
      iGstView->DrawText(_L("balance"),KRgbBlack);
    /*  g_object_set (G_OBJECT (sink), 
                         "left balance", 5000,
                         "right balance",5000,NULL);
                         */
     return 0;
     }
 
 void samplesplayed()
      {
      if(!sink)
           {
           iGstView->DrawText(_L("Devsound sink not available"),KRgbRed);      
           }
      int samples;
      TBuf<25> samplesplayed(_L("samples played "));    
      g_object_get (G_OBJECT (sink), 
                   "samples played", &samples, NULL);
      
      samplesplayed.AppendNum(samples);
      iGstView->DrawText(samplesplayed,KRgbBlack);   
      }

 void samplesrecorded()
       {
       if(!record)
            {
            iGstView->DrawText(_L("Devsound src not available"),KRgbRed);      
            }
       int samples;
       TBuf<25> samplesrecorded(_L("samples recorded "));    
       g_object_get (G_OBJECT (record), 
                    "samples recorded", &samples, NULL);
       
       samplesrecorded.AppendNum(samples);
       iGstView->DrawText(samplesrecorded,KRgbBlack);   
       } 
 
 static gboolean print_field (GQuark field, const GValue *value, gpointer pfx)
     {
     gchar *str = gst_value_serialize (value);
     
     const gchar* c;

     RDebug::Printf("%s  %15s: %s\n", (gchar *) pfx, c = g_quark_to_string (field), str);
     
     if(negcaps)
         {
         TPtrC8 property((const TText8*)c);
         TPtrC8 val((const TText8*)str);
         TBuf<10> appdval;
         appdval.Copy(val);
      
         TBuf<25> name;
         name.Copy(property);
         name.Append(':');
         name.Append(appdval);
       
         iGstView->DrawText(name,KRgbBlack);
         }    
     g_free (str);
     return TRUE;
     }

 static void print_caps (const GstCaps *caps, const gchar *pfx)
     {
     guint i;

     g_return_if_fail (caps != NULL);

     if (gst_caps_is_any (caps)) {
     RDebug::Printf("%sANY\n", pfx);
     return;
     }
     if (gst_caps_is_empty (caps)) {
     RDebug::Printf("%sEMPTY\n", pfx);
     return;
     }
     const gchar *c;
     for (i = 0; i < gst_caps_get_size (caps); i++) {
     GstStructure *structure = gst_caps_get_structure (caps, i);
     
     RDebug::Printf("%s%s\n", pfx,c = gst_structure_get_name (structure));
     
     TPtrC8 fmt((const TText8*)c);
   
     TBuf<25> name;
     name.Copy(fmt);
    
     iGstView->DrawText(name,KRgbBlack);
         
     gst_structure_foreach (structure, print_field, (gpointer) pfx);
     }
     }
 
 void getsinkpadcaps()
     {
     RDebug::Print(_L("Devsound Pad Caps"));
     if(!sink)
          {
          iGstView->DrawText(_L("Devsound sink not available"),KRgbRed);
          return;
          }
     negcaps = FALSE;
     dssinkpad = gst_element_get_pad (sink, "sink");   
     dssinkcap = gst_pad_get_caps (dssinkpad);
     
     print_caps (dssinkcap, "     ");
     }
 
 void negotiatedsinkcaps()
     {
     RDebug::Print(_L("Negotiated caps"));
      if(!sink)
           {
           iGstView->DrawText(_L("Devsound sink not available"),KRgbRed);
           return;
           }
      negcaps = TRUE;
      dssinkpad = gst_element_get_pad (sink, "sink");   
      dssinkcap = gst_pad_get_negotiated_caps (dssinkpad);
      
      print_caps (dssinkcap, "     ");    
     }
 
 void getsrcpadcaps()
      {
      RDebug::Print(_L("Devsound Source Pad Caps"));
      if(!record)
           {
           iGstView->DrawText(_L("Devsound src not available"),KRgbRed);
           return;
           }
      negcaps = FALSE;
      dssrcpad = gst_element_get_pad (record, "src");   
      dssrccap = gst_pad_get_caps (dssrcpad);
      
      print_caps (dssrccap, "     ");
      }
  
  void negotiatedsrccaps()
      {
      RDebug::Print(_L("Negotiated  src caps"));
       if(!record)
            {
            iGstView->DrawText(_L("Devsound src not available"),KRgbRed);
            return;
            }
       negcaps = TRUE;
       dssrcpad = gst_element_get_pad (record, "src");   
       dssrccap = gst_pad_get_negotiated_caps (dssrcpad);
   
       print_caps (dssrccap, "     ");
          
      }
