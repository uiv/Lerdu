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
#ifndef __ILBC_DECODER_INTERFACE__H
#define __ILBC_DECODER_INTERFACE__H

#include <glib.h>
#include <gst/gstelement.h>
#include <gst/gstpluginfeature.h>
#include <gst/gst.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GST_TYPE_ILBC_DECODER               (gst_ilbc_decoder_get_type ())
#define GST_ILBC_DECODER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_ILBC_DECODER, GstIlbcDecoder))
#define GST_ILBC_DECODER_CLASS(vtable)      (G_TYPE_CHECK_CLASS_CAST ((vtable), GST_TYPE_ILBC_DECODER, GstIlbcDecoderIntfc))
#define GST_IS_ILBC_DECODER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_ILBC_DECODER))
#define GST_IS_ILBC_DECODER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_TYPE ((vtable), GST_TYPE_ILBC_DECODER))
#define GST_ILBC_DECODER_GET_IFACE(obj)     (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_ILBC_DECODER, GstIlbcDecoderIntfc))

typedef struct _GstIlbcDecoder     GstIlbcDecoder;
typedef struct _GstIlbcDecoderIntfc GstIlbcDecoderIntfc;


/**
*  Interface for Ilbc decoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*/
enum TIlbcDecodeMode
	{
	E20msFrame = 1,
	E30msFrame
	};
	
struct _GstIlbcDecoderIntfc
    {
    GTypeInterface parent;  
    gint (*SetDecoderMode)(enum TIlbcDecodeMode aDecodeMode);
    gint (*SetCng)(gboolean aCng);
    gint (*GetCng)(gboolean* aCng);
    };

IMPORT_C GType gst_ilbc_decoder_get_type(void);

G_END_DECLS

#endif /* __ILBC_DECODER_INTERFACE__ */
