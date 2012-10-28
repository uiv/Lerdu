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

#ifndef __G729_DECODER_INTERFACE__H
#define __G729_DECODER_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GST_TYPE_G729_DECODER               (gst_g729_decoder_get_type ())
#define GST_G729_DECODER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_G729_DECODER, GstG729Decoder))
#define GST_G729_DECODER_CLASS(vtable)      (G_TYPE_CHECK_CLASS_CAST ((vtable), GST_TYPE_G729_DECODER, GstG729DecoderIntfc))
#define GST_IS_G729_DECODER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_G729_DECODER))
#define GST_IS_G729_DECODER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_TYPE ((vtable), GST_TYPE_G729_DECODER))
#define GST_G729_DECODER_GET_IFACE(obj)     (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_G729_DECODER, GstG729DecoderIntfc))

typedef struct _GstG729Decoder     GstG729Decoder;
typedef struct _GstG729DecoderIntfc GstG729DecoderIntfc;


/**
*  Interface for G729 decoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*/

struct _GstG729DecoderIntfc
    {
    GTypeInterface parent;       
    gint (*BadLsfNextBuffer)();   	
    };

IMPORT_C GType gst_g729_decoder_get_type(void);

G_END_DECLS

#endif /* __G711_DECODER_INTERFACE__ */
