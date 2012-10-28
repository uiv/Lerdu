#ifndef _GST_ULAW_CONVERSION_H
#define _GST_ULAW_CONVERSION_H

#include <glib.h>
#include <gst/gst_global.h>
#include <gst/riff/riff-read.h>
#include <gst/riff/riff-ids.h>
#include <gst/riff/riff-media.h>
#include <gst/base/gstadapter.h>

void
mulaw_encode(gint16* in, guint8* out, gint numsamples);
void
mulaw_decode(guint8* in,gint16* out,gint numsamples);

#endif /* _GST_ULAW_CONVERSION_H */

