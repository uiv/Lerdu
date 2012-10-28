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
#ifndef __G729_ENCODER_INTERFACE__H
#define __G729_ENCODER_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GST_TYPE_G729_ENCODER               (gst_g729_encoder_get_type ())
#define GST_G729_ENCODER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_G729_ENCODER, GstG729Encoder))
#define GST_G729_ENCODER_CLASS(vtable)      (G_TYPE_CHECK_CLASS_CAST ((vtable), GST_TYPE_G729_ENCODER, GstG729EncoderIntfc))
#define GST_IS_G729_ENCODER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_G729_ENCODER))
#define GST_IS_G729_ENCODER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_TYPE ((vtable), GST_TYPE_G729_ENCODER))
#define GST_G729_ENCODER_GET_IFACE(obj)     (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_G729_ENCODER, GstG729EncoderIntfc))

typedef struct _GstG729Encoder     GstG729Encoder;
typedef struct _GstG729EncoderIntfc GstG729EncoderIntfc;


/**
*  Interface for G729 encoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*/

struct _GstG729EncoderIntfc
    {
    GTypeInterface parent;  
    gint (*SetVadMode) (gboolean aVadMode);
    gint (*GetVadMode)(gboolean* aVadMode);
	
    };

IMPORT_C GType gst_g729_encoder_get_type(void);

G_END_DECLS

#endif /* __G711_ENCODER_INTERFACE__ */
