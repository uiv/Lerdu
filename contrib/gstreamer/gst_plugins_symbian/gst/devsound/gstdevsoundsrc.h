/* GStreamer
 * Copyright (C) 1999,2000 
 *                    2000 
 *
 * gstdevsoundsrc.h: 
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
 
/*Gstreamer Src Plugin for Symbian Devsound. This is the plugin used to 
record sound from the Symbian Devsound.

*/

#ifndef __GST_DEVSOUNDSRC_H__
#define __GST_DEVSOUNDSRC_H__

#include <gst/gst.h>
#include <gst/base/gstpushsrc.h>

G_BEGIN_DECLS

#define GST_TYPE_DEVSOUND_SRC          (gst_devsound_src_get_type())
#define GST_DEVSOUND_SRC(obj)          (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_DEVSOUND_SRC,GstDevsoundSrc))
#define GST_DEVSOUND_SRC_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_DEVSOUND_SRC,GstDevsoundSrcClass))
#define GST_IS_DEVSOUND_SRC(obj)       (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_DEVSOUND_SRC))
#define GST_IS_DEVSOUND_SRC_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_DEVSOUND_SRC))

typedef struct _GstDevsoundSrc GstDevsoundSrc;
typedef struct _GstDevsoundSrcClass GstDevsoundSrcClass;

typedef struct _GstDevsoundUpdate GstDevsoundUpdate;

struct _GstDevsoundUpdate{
gboolean  channelsupdate;
gboolean  rateupdate;
gboolean  gainupdate;
gboolean  leftbalanceupdate;
gboolean  rightbalanceupdate;
gboolean  preferenceupdate;
gboolean  priorityupdate;
};

struct _GstDevsoundSrc {
    GstPushSrc    src;

  void *handle;	
  gchar *device;
  gint   bytes_per_sample;
  GstCaps *probed_caps;
  
  GstDevsoundUpdate pending;
  
  //properties
  gint   priority;
  gint   preference;
  gulong fourcc;
  gint   channels;
  gint   rate;
  gint   gain;
  gint   maxgain;
  gint   leftbalance;
  gint   rightbalance;
  gint   samplesrecorded; 
  GList*  fmt;
  GList*  supportedbitrates;
  
  guint speechbitrate;
  gboolean speechvadmode;
  gint g711encodemode;
  gboolean g711vadmode;
  gboolean g729vadmode;
  gint ilbcencodemode;
  gboolean ilbcvadmode;

  
};

struct _GstDevsoundSrcClass {
GstPushSrcClass parent_class;
};

GType gst_devsound_src_get_type(void);

G_END_DECLS

#endif /* __GST_DEVSOUNDSINK_H__ */
