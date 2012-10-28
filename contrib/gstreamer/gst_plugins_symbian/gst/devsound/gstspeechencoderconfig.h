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
#ifndef __SPEECH_ENCODER_CONFIG_INTERFACE__H
#define __SPEECH_ENCODER_CONFIG_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_SPEECH_ENCODER_CONFIG     (gst_speech_encoder_config_get_type ())
#define GST_SPEECH_ENCODER_CONFIG(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_SPEECH_ENCODER_CONFIG, GstSpeechEncoderConfig))
#define GST_IS_SPEECH_ENCODER_CONFIG(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_SPEECH_ENCODER_CONFIG))
#define GST_SPEECH_ENCODER_CONFIG_GET_IFACE(obj)   (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_SPEECH_ENCODER_CONFIG, GstSpeechEncoderConfigIntfc))

typedef struct _GstSpeechEncoderConfig      GstSpeechEncoderConfig;
typedef struct _GstSpeechEncoderConfigIntfc GstSpeechEncoderConfigIntfc;

struct _GstSpeechEncoderConfigIntfc
    {
    GTypeInterface parent;
 	gint (*GetSupportedBitrates)(GList** aSupportedBitrates);
	gint (*SetBitrate)(guint aBitrate);
	gint (*GetBitrate)(guint* aBitrate);
	gint (*SetVadMode)(gboolean aVadMode);
	gint (*GetVadMode)(gboolean* aVadMode);

    };

IMPORT_C GType gst_speech_encoder_config_get_type(void);

G_END_DECLS

#endif /* __SPEECH_ENCODER_CONFIG_INTERFACE */
