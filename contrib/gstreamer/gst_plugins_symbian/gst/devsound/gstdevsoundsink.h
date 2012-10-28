/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *
 * gstosssink.h: 
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
 
/*Gstreamer Sink Plugin for Symbian Devsound. This is the plugin used to 
playback sound from the Symbian Devsound.

*/

#ifndef __GST_DEVSOUNDSINK_H__
#define __GST_DEVSOUNDSINK_H__


#include <gst/gst.h>
#include <gst/base/gstbasesink.h>


G_BEGIN_DECLS

#define GST_TYPE_DEVSOUND_SINK          (gst_devsound_sink_get_type())
#define GST_DEVSOUND_SINK(obj)          (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_DEVSOUND_SINK,GstDevsoundSink))
#define GST_DEVSOUND_SINK_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_DEVSOUND_SINK,GstDevsoundSinkClass))
#define GST_IS_DEVSOUND_SINK(obj)       (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_DEVSOUND_SINK))
#define GST_IS_DEVSOUND_SINK_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_DEVSOUND_SINK))



typedef struct _GstDevsoundSink GstDevsoundSink;
typedef struct _GstDevsoundSinkClass GstDevsoundSinkClass;

typedef struct _GstDevsoundUpdate GstDevsoundUpdate;

struct _GstDevsoundUpdate{
gboolean  channelsupdate;
gboolean  rateupdate;
gboolean  volumeupdate;
gboolean  volumerampupdate;
gboolean  leftbalanceupdate;
gboolean  rightbalanceupdate;
gboolean  preferenceupdate;
gboolean  priorityupdate;
gboolean  fourccupdate;
gboolean  outputupdate;
};

struct _GstDevsoundSink {
  GstBaseSink    sink;	
  
  void *handle;	
  void *dataptr;
  gchar *device;
  gint   bytes_per_sample;
  GstCaps *probed_caps;

  GstDevsoundUpdate pending;

  //properties
  gint   channels;
  gint   rate;
  gint   volume;
  gint   volumeramp;
  gint   maxvolume;
  gint   leftbalance;
  gint   rightbalance;
  gint   priority;
  gint   preference;
  gint   samplesplayed;
  gint   output;
  gulong fourcc;
  gchar  *mimetype;
  GList   *fmt;
  gboolean framemodereq;
  gboolean g711cng;
  gboolean ilbccng;  
};

struct _GstDevsoundSinkClass {
  GstBaseSinkClass parent_class;
};

GType gst_devsound_sink_get_type(void);

G_END_DECLS

#endif /* __GST_DEVSOUNDSINK_H__ */
