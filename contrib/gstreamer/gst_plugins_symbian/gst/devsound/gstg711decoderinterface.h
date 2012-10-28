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

#ifndef __G711_DECODER_INTERFACE__H
#define __G711_DECODER_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GST_TYPE_G711_DECODER               (gst_g711_decoder_get_type ())
#define GST_G711_DECODER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_G711_DECODER, GstG711Decoder))
#define GST_G711_DECODER_CLASS(vtable)      (G_TYPE_CHECK_CLASS_CAST ((vtable), GST_TYPE_G711_DECODER, GstG711DecoderIntfc))
#define GST_IS_G711_DECODER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_G711_DECODER))
#define GST_IS_G711_DECODER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_TYPE ((vtable), GST_TYPE_G711_DECODER))
#define GST_G711_DECODER_GET_IFACE(obj)     (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_G711_DECODER, GstG711DecoderIntfc))

typedef struct _GstG711Decoder     GstG711Decoder;
typedef struct _GstG711DecoderIntfc GstG711DecoderIntfc;


/**
*  Interface for G711 decoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*/
enum TG711DecodeMode
    {
    EDecALaw,
    EDecULaw
     };
struct _GstG711DecoderIntfc
    {
    GTypeInterface parent;   
    gint (*SetDecoderMode)(enum TG711DecodeMode aDecodeMode);
    gint (*SetCng)(gboolean aCng);
	gint (*GetCng)(gboolean* aCng);
	gint (*SetPlc)(gboolean aPlc);
	
    };

IMPORT_C GType gst_g711_decoder_get_type(void);

G_END_DECLS

#endif /* __G711_DECODER_INTERFACE__ */
