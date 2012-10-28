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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "gstg711decoderinterface.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <gobject_global.h>

#endif

static guint g711_decoder_count = 0;
static void gst_g711_decoder_base_init (gpointer g_class);
static void gst_g711_decoder_base_finalize (gpointer g_class);
             
EXPORT_C GType gst_g711_decoder_get_type(void)
{
  static GType g711_decoder_type = 0;

  if (g711_decoder_type == 0) {
    static const GTypeInfo g711_decoder_info = {
      sizeof (GstG711DecoderIntfc), /* class_size */
      gst_g711_decoder_base_init,  /* base_init */
      gst_g711_decoder_base_finalize,  /* base_finalize */
      NULL,
      NULL,                     /* class_finalize */
      NULL,                     /* class_data */
      0,
      0,
      NULL
    };


    g711_decoder_type = g_type_register_static (G_TYPE_INTERFACE, "GstG711DecoderInterface",
        &g711_decoder_info, 0);

  }

  return g711_decoder_type;
}


static void gst_g711_decoder_base_init (gpointer g_class)
    {
    static gboolean initialized = FALSE;
    g711_decoder_count++;

    if (G_UNLIKELY (!initialized))
        {
        initialized = TRUE;
        }
    }

static void gst_g711_decoder_base_finalize (gpointer g_class)
    {
    g711_decoder_count--;

    }


