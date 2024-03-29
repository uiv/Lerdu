/* GStreamer
 * Copyright (C) <2005> Wim Taymans <wim@fluendo.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more 
 */

/**
 * SECTION:gstbasertppayload
 * @short_description: Base class for RTP payloader
 *
 * <refsect2>
 * <para>
 * Provides a base class for RTP payloaders
 * </para>
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <string.h>

#include <gst/rtp/gstrtpbuffer.h>

#include "gstbasertppayload.h"

GST_DEBUG_CATEGORY_STATIC (basertppayload_debug);
#define GST_CAT_DEFAULT (basertppayload_debug)

#define GST_BASE_RTP_PAYLOAD_GET_PRIVATE(obj)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GST_TYPE_BASE_RTP_PAYLOAD, GstBaseRTPPayloadPrivate))

struct _GstBaseRTPPayloadPrivate
{
  gboolean ts_offset_random;
  gboolean seqnum_offset_random;
  gboolean ssrc_random;
  guint16 next_seqnum;

  GstClockTime rt_base;
};

/* BaseRTPPayload signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

/* FIXME 0.11, a better default is the Ethernet MTU of 
 * 1500 - sizeof(headers) as pointed out by marcelm in IRC:
 * So an Ethernet MTU of 1500, minus 60 for the max IP, minus 8 for UDP, gives
 * 1432 bytes or so.  And that should be adjusted downward further for other
 * encapsulations like PPPoE, so 1400 at most.
 */
#define DEFAULT_MTU                     1400
#define DEFAULT_PT                      96
#define DEFAULT_SSRC                    -1
#define DEFAULT_TIMESTAMP_OFFSET        -1
#define DEFAULT_SEQNUM_OFFSET           -1
#define DEFAULT_MAX_PTIME               -1
#define DEFAULT_MIN_PTIME               0

enum
{
  PROP_0,
  PROP_MTU,
  PROP_PT,
  PROP_SSRC,
  PROP_TIMESTAMP_OFFSET,
  PROP_SEQNUM_OFFSET,
  PROP_MAX_PTIME,
  PROP_MIN_PTIME,
  PROP_TIMESTAMP,
  PROP_SEQNUM
};

static void gst_basertppayload_class_init (GstBaseRTPPayloadClass * klass);
static void gst_basertppayload_base_init (GstBaseRTPPayloadClass * klass);
static void gst_basertppayload_init (GstBaseRTPPayload * basertppayload,
    gpointer g_class);
static void gst_basertppayload_finalize (GObject * object);

static gboolean gst_basertppayload_setcaps (GstPad * pad, GstCaps * caps);
static GstCaps *gst_basertppayload_getcaps (GstPad * pad);
static gboolean gst_basertppayload_event (GstPad * pad, GstEvent * event);
static GstFlowReturn gst_basertppayload_chain (GstPad * pad,
    GstBuffer * buffer);

static void gst_basertppayload_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_basertppayload_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstStateChangeReturn gst_basertppayload_change_state (GstElement *
    element, GstStateChange transition);

static GstElementClass *parent_class = NULL;

/* FIXME 0.11: API should be changed to gst_base_typ_payload_xyz */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GType
gst_basertppayload_get_type (void)
{
  static GType basertppayload_type = 0;

  if (!basertppayload_type) {
    static const GTypeInfo basertppayload_info = {
      sizeof (GstBaseRTPPayloadClass),
      (GBaseInitFunc) gst_basertppayload_base_init,
      NULL,
      (GClassInitFunc) gst_basertppayload_class_init,
      NULL,
      NULL,
      sizeof (GstBaseRTPPayload),
      0,
      (GInstanceInitFunc) gst_basertppayload_init,
    };

    basertppayload_type =
        g_type_register_static (GST_TYPE_ELEMENT, "GstBaseRTPPayload",
        &basertppayload_info, G_TYPE_FLAG_ABSTRACT);
  }
  return basertppayload_type;
}

static void
gst_basertppayload_base_init (GstBaseRTPPayloadClass * klass)
{
}

static void
gst_basertppayload_class_init (GstBaseRTPPayloadClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  g_type_class_add_private (klass, sizeof (GstBaseRTPPayloadPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->finalize = gst_basertppayload_finalize;

  gobject_class->set_property = gst_basertppayload_set_property;
  gobject_class->get_property = gst_basertppayload_get_property;

  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_MTU,
      g_param_spec_uint ("mtu", "MTU",
          "Maximum size of one packet",
          28, G_MAXUINT, DEFAULT_MTU, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_PT,
      g_param_spec_uint ("pt", "payload type",
          "The payload type of the packets",
          0, 0x80, DEFAULT_PT, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_SSRC,
      g_param_spec_uint ("ssrc", "SSRC",
          "The SSRC of the packets (default == random)",
          0, G_MAXUINT32, DEFAULT_SSRC, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass),
      PROP_TIMESTAMP_OFFSET, g_param_spec_uint ("timestamp-offset",
          "Timestamp Offset",
          "Offset to add to all outgoing timestamps (default = random)", 0,
          G_MAXUINT32, DEFAULT_TIMESTAMP_OFFSET, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_SEQNUM_OFFSET,
      g_param_spec_int ("seqnum-offset", "Sequence number Offset",
          "Offset to add to all outgoing seqnum (-1 = random)", -1, G_MAXUINT16,
          DEFAULT_SEQNUM_OFFSET, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_MAX_PTIME,
      g_param_spec_int64 ("max-ptime", "Max packet time",
          "Maximum duration of the packet data in ns (-1 = unlimited up to MTU)",
          -1, G_MAXINT64, DEFAULT_MAX_PTIME, G_PARAM_READWRITE));
  /**
   * GstBaseRTPAudioPayload:min-ptime:
   *
   * Minimum duration of the packet data in ns (can't go above MTU)
   *
   * Since: 0.10.13
   **/
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_MIN_PTIME,
      g_param_spec_int64 ("min-ptime", "Min packet time",
          "Minimum duration of the packet data in ns (can't go above MTU)",
          0, G_MAXINT64, DEFAULT_MIN_PTIME, G_PARAM_READWRITE));

  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_TIMESTAMP,
      g_param_spec_uint ("timestamp", "Timestamp",
          "The RTP timestamp of the last processed packet",
          0, G_MAXUINT32, 0, G_PARAM_READABLE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_SEQNUM,
      g_param_spec_uint ("seqnum", "Sequence number",
          "The RTP sequence number of the last processed packet",
          0, G_MAXUINT16, 0, G_PARAM_READABLE));

  gstelement_class->change_state = gst_basertppayload_change_state;

  GST_DEBUG_CATEGORY_INIT (basertppayload_debug, "basertppayload", 0,
      "Base class for RTP Payloaders");
}

static void
gst_basertppayload_init (GstBaseRTPPayload * basertppayload, gpointer g_class)
{
  GstPadTemplate *templ;
  GstBaseRTPPayloadPrivate *priv;

  basertppayload->priv = priv =
      GST_BASE_RTP_PAYLOAD_GET_PRIVATE (basertppayload);

  templ =
      gst_element_class_get_pad_template (GST_ELEMENT_CLASS (g_class), "src");
  g_return_if_fail (templ != NULL);

  basertppayload->srcpad = gst_pad_new_from_template (templ, "src");
  gst_element_add_pad (GST_ELEMENT (basertppayload), basertppayload->srcpad);

  templ =
      gst_element_class_get_pad_template (GST_ELEMENT_CLASS (g_class), "sink");
  g_return_if_fail (templ != NULL);

  basertppayload->sinkpad = gst_pad_new_from_template (templ, "sink");
  gst_pad_set_setcaps_function (basertppayload->sinkpad,
      gst_basertppayload_setcaps);
  gst_pad_set_getcaps_function (basertppayload->sinkpad,
      gst_basertppayload_getcaps);
  gst_pad_set_event_function (basertppayload->sinkpad,
      gst_basertppayload_event);
  gst_pad_set_chain_function (basertppayload->sinkpad,
      gst_basertppayload_chain);
  gst_element_add_pad (GST_ELEMENT (basertppayload), basertppayload->sinkpad);

  basertppayload->seq_rand = g_rand_new ();
  basertppayload->ssrc_rand = g_rand_new ();
  basertppayload->ts_rand = g_rand_new ();

  basertppayload->mtu = DEFAULT_MTU;
  basertppayload->pt = DEFAULT_PT;
  basertppayload->seqnum_offset = DEFAULT_SEQNUM_OFFSET;
  basertppayload->ssrc = DEFAULT_SSRC;
  basertppayload->ts_offset = DEFAULT_TIMESTAMP_OFFSET;
  priv->seqnum_offset_random = (basertppayload->seqnum_offset == -1);
  priv->ts_offset_random = (basertppayload->ts_offset == -1);
  priv->ssrc_random = (basertppayload->ssrc == -1);

  basertppayload->max_ptime = DEFAULT_MAX_PTIME;
  basertppayload->min_ptime = DEFAULT_MIN_PTIME;

  basertppayload->media = NULL;
  basertppayload->encoding_name = NULL;

  basertppayload->clock_rate = 0;
}

static void
gst_basertppayload_finalize (GObject * object)
{
  GstBaseRTPPayload *basertppayload;

  basertppayload = GST_BASE_RTP_PAYLOAD (object);

  g_rand_free (basertppayload->seq_rand);
  basertppayload->seq_rand = NULL;
  g_rand_free (basertppayload->ssrc_rand);
  basertppayload->ssrc_rand = NULL;
  g_rand_free (basertppayload->ts_rand);
  basertppayload->ts_rand = NULL;

  g_free (basertppayload->media);
  basertppayload->media = NULL;
  g_free (basertppayload->encoding_name);
  basertppayload->encoding_name = NULL;

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
gst_basertppayload_setcaps (GstPad * pad, GstCaps * caps)
{
  GstBaseRTPPayload *basertppayload;
  GstBaseRTPPayloadClass *basertppayload_class;
  gboolean ret = TRUE;

  GST_DEBUG_OBJECT (pad, "setting caps %" GST_PTR_FORMAT, caps);
  basertppayload = GST_BASE_RTP_PAYLOAD (gst_pad_get_parent (pad));
  basertppayload_class = GST_BASE_RTP_PAYLOAD_GET_CLASS (basertppayload);

  if (basertppayload_class->set_caps)
    ret = basertppayload_class->set_caps (basertppayload, caps);

  gst_object_unref (basertppayload);

  return ret;
}

static GstCaps *
gst_basertppayload_getcaps (GstPad * pad)
{
  GstBaseRTPPayload *basertppayload;
  GstBaseRTPPayloadClass *basertppayload_class;
  GstCaps *caps = NULL;

  GST_DEBUG_OBJECT (pad, "getting caps");

  basertppayload = GST_BASE_RTP_PAYLOAD (gst_pad_get_parent (pad));
  basertppayload_class = GST_BASE_RTP_PAYLOAD_GET_CLASS (basertppayload);

  if (basertppayload_class->get_caps)
    caps = basertppayload_class->get_caps (basertppayload, pad);

  if (!caps) {
    caps = GST_PAD_TEMPLATE_CAPS (GST_PAD_PAD_TEMPLATE (pad));
    GST_DEBUG_OBJECT (pad,
        "using pad template %p with caps %p %" GST_PTR_FORMAT,
        GST_PAD_PAD_TEMPLATE (pad), caps, caps);

    caps = gst_caps_ref (caps);
  }

  gst_object_unref (basertppayload);

  return caps;
}

static gboolean
gst_basertppayload_event (GstPad * pad, GstEvent * event)
{
  GstBaseRTPPayload *basertppayload;
  GstBaseRTPPayloadClass *basertppayload_class;
  gboolean res;

  basertppayload = GST_BASE_RTP_PAYLOAD (gst_pad_get_parent (pad));
  basertppayload_class = GST_BASE_RTP_PAYLOAD_GET_CLASS (basertppayload);

  if (basertppayload_class->handle_event) {
    res = basertppayload_class->handle_event (pad, event);
    if (res)
      goto done;
  }

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_FLUSH_START:
      res = gst_pad_event_default (pad, event);
      break;
    case GST_EVENT_FLUSH_STOP:
      res = gst_pad_event_default (pad, event);
      gst_segment_init (&basertppayload->segment, GST_FORMAT_UNDEFINED);
      break;
    case GST_EVENT_NEWSEGMENT:
    {
      gboolean update;
      gdouble rate;
      GstFormat fmt;
      gint64 start, stop, position;

      gst_event_parse_new_segment (event, &update, &rate, &fmt, &start, &stop,
          &position);
      gst_segment_set_newsegment (&basertppayload->segment, update, rate, fmt,
          start, stop, position);

      /* fallthrough */
    }
    default:
      res = gst_pad_event_default (pad, event);
      break;
  }

done:
  gst_object_unref (basertppayload);

  return res;
}


static GstFlowReturn
gst_basertppayload_chain (GstPad * pad, GstBuffer * buffer)
{
  GstBaseRTPPayload *basertppayload;
  GstBaseRTPPayloadClass *basertppayload_class;
  GstFlowReturn ret;

  basertppayload = GST_BASE_RTP_PAYLOAD (gst_pad_get_parent (pad));
  basertppayload_class = GST_BASE_RTP_PAYLOAD_GET_CLASS (basertppayload);

  if (!basertppayload_class->handle_buffer)
    goto no_function;

  ret = basertppayload_class->handle_buffer (basertppayload, buffer);

  gst_object_unref (basertppayload);

  return ret;

  /* ERRORS */
no_function:
  {
    GST_ELEMENT_ERROR (basertppayload, STREAM, NOT_IMPLEMENTED, (NULL),
        ("subclass did not implement handle_buffer function"));
    gst_object_unref (basertppayload);
    gst_buffer_unref (buffer);
    return GST_FLOW_ERROR;
  }
}

/**
 * gst_basertppayload_set_options:
 * @payload: a #GstBaseRTPPayload
 * @media: the media type (typically "audio" or "video")
 * @dynamic: if the payload type is dynamic
 * @encoding_name: the encoding name 
 * @clock_rate: the clock rate of the media
 *
 * Set the rtp options of the payloader. These options will be set in the caps
 * of the payloader. Subclasses must call this method before calling
 * gst_basertppayload_push() or gst_basertppayload_set_outcaps().
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_basertppayload_set_options (GstBaseRTPPayload * payload,
    gchar * media, gboolean dynamic, gchar * encoding_name, guint32 clock_rate)
{
  g_return_if_fail (payload != NULL);
  g_return_if_fail (clock_rate != 0);

  g_free (payload->media);
  payload->media = g_strdup (media);
  payload->dynamic = dynamic;
  g_free (payload->encoding_name);
  payload->encoding_name = g_strdup (encoding_name);
  payload->clock_rate = clock_rate;
}

static gboolean
copy_fixed (GQuark field_id, const GValue * value, GstStructure * dest)
{
  if (gst_value_is_fixed (value)) {
    gst_structure_id_set_value (dest, field_id, value);
  }
  return TRUE;
}

/**
 * gst_basertppayload_set_outcaps:
 * @payload: a #GstBaseRTPPayload
 * @fieldname: the first field name or %NULL
 * @...: field values
 *
 * Configure the output caps with the optional parameters.
 *
 * Variable arguments should be in the form field name, field type
 * (as a GType), value(s).  The last variable argument should be NULL.
 *
 * Returns: %TRUE if the caps could be set.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_basertppayload_set_outcaps (GstBaseRTPPayload * payload, gchar * fieldname,
    ...)
{
  GstCaps *srccaps, *peercaps;

  /* fill in the defaults, there properties cannot be negotiated. */
  srccaps = gst_caps_new_simple ("application/x-rtp",
      "media", G_TYPE_STRING, payload->media,
      "clock-rate", G_TYPE_INT, payload->clock_rate,
      "encoding-name", G_TYPE_STRING, payload->encoding_name, NULL);

  GST_DEBUG_OBJECT (payload, "defaults: %" GST_PTR_FORMAT, srccaps);

  if (fieldname) {
    va_list varargs;

    /* override with custom properties */
    va_start (varargs, fieldname);
    gst_caps_set_simple_valist (srccaps, fieldname, varargs);
    va_end (varargs);

    GST_DEBUG_OBJECT (payload, "custom added: %" GST_PTR_FORMAT, srccaps);
  }

  /* the peer caps can override some of the defaults */
  peercaps = gst_pad_peer_get_caps (payload->srcpad);
  if (peercaps == NULL) {
    /* no peer caps, just add the other properties */
    gst_caps_set_simple (srccaps,
        "payload", G_TYPE_INT, GST_BASE_RTP_PAYLOAD_PT (payload),
        "ssrc", G_TYPE_UINT, payload->current_ssrc,
        "clock-base", G_TYPE_UINT, payload->ts_base,
        "seqnum-base", G_TYPE_UINT, payload->seqnum_base, NULL);

    GST_DEBUG_OBJECT (payload, "no peer caps: %" GST_PTR_FORMAT, srccaps);
  } else {
    GstCaps *temp;
    GstStructure *s, *d;
    const GValue *value;
    gint pt;

    /* peer provides caps we can use to fixate, intersect. This always returns a
     * writable caps. */
    temp = gst_caps_intersect (srccaps, peercaps);
    gst_caps_unref (srccaps);
    gst_caps_unref (peercaps);

    /* now fixate, start by taking the first caps */
    gst_caps_truncate (temp);

    /* get first structure */
    s = gst_caps_get_structure (temp, 0);

    if (gst_structure_get_int (s, "payload", &pt)) {
      /* use peer pt */
      GST_BASE_RTP_PAYLOAD_PT (payload) = pt;
      GST_LOG_OBJECT (payload, "using peer pt %d", pt);
    } else {
      if (gst_structure_has_field (s, "payload")) {
        /* can only fixate if there is a field */
        gst_structure_fixate_field_nearest_int (s, "payload",
            GST_BASE_RTP_PAYLOAD_PT (payload));
        gst_structure_get_int (s, "payload", &pt);
        GST_LOG_OBJECT (payload, "using peer pt %d", pt);
      } else {
        /* no pt field, use the internal pt */
        pt = GST_BASE_RTP_PAYLOAD_PT (payload);
        gst_structure_set (s, "payload", G_TYPE_INT, pt, NULL);
        GST_LOG_OBJECT (payload, "using internal pt %d", pt);
      }
    }

    if (gst_structure_has_field_typed (s, "ssrc", G_TYPE_UINT)) {
      value = gst_structure_get_value (s, "ssrc");
      payload->current_ssrc = g_value_get_uint (value);
      GST_LOG_OBJECT (payload, "using peer ssrc %08x", payload->current_ssrc);
    } else {
      /* FIXME, fixate_nearest_uint would be even better */
      gst_structure_set (s, "ssrc", G_TYPE_UINT, payload->current_ssrc, NULL);
      GST_LOG_OBJECT (payload, "using internal ssrc %08x",
          payload->current_ssrc);
    }

    if (gst_structure_has_field_typed (s, "clock-base", G_TYPE_UINT)) {
      value = gst_structure_get_value (s, "clock-base");
      payload->ts_base = g_value_get_uint (value);
      GST_LOG_OBJECT (payload, "using peer clock-base %u", payload->ts_base);
    } else {
      /* FIXME, fixate_nearest_uint would be even better */
      gst_structure_set (s, "clock-base", G_TYPE_UINT, payload->ts_base, NULL);
      GST_LOG_OBJECT (payload, "using internal clock-base %u",
          payload->ts_base);
    }
    if (gst_structure_has_field_typed (s, "seqnum-base", G_TYPE_UINT)) {
      value = gst_structure_get_value (s, "seqnum-base");
      payload->seqnum_base = g_value_get_uint (value);
      GST_LOG_OBJECT (payload, "using peer seqnum-base %u",
          payload->seqnum_base);
    } else {
      /* FIXME, fixate_nearest_uint would be even better */
      gst_structure_set (s, "seqnum-base", G_TYPE_UINT, payload->seqnum_base,
          NULL);
      GST_LOG_OBJECT (payload, "using internal seqnum-base %u",
          payload->seqnum_base);
    }

    /* make the target caps by copying over all the fixed caps, removing the
     * unfixed caps. */
    srccaps = gst_caps_new_simple (gst_structure_get_name (s), NULL);
    d = gst_caps_get_structure (srccaps, 0);

    gst_structure_foreach (s, (GstStructureForeachFunc) copy_fixed, d);

    gst_caps_unref (temp);

    GST_DEBUG_OBJECT (payload, "with peer caps: %" GST_PTR_FORMAT, srccaps);
  }

  gst_pad_set_caps (GST_BASE_RTP_PAYLOAD_SRCPAD (payload), srccaps);
  gst_caps_unref (srccaps);

  return TRUE;
}

/**
 * gst_basertppayload_is_filled:
 * @payload: a #GstBaseRTPPayload
 * @size: the size of the packet
 * @duration: the duration of the packet
 *
 * Check if the packet with @size and @duration would exceed the configure
 * maximum size.
 *
 * Returns: %TRUE if the packet of @size and @duration would exceed the
 * configured MTU or max_ptime.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_basertppayload_is_filled (GstBaseRTPPayload * payload,
    guint size, GstClockTime duration)
{
  if (size > payload->mtu)
    return TRUE;

  if (payload->max_ptime != -1 && duration >= payload->max_ptime)
    return TRUE;

  return FALSE;
}

/**
 * gst_basertppayload_push:
 * @payload: a #GstBaseRTPPayload
 * @buffer: a #GstBuffer
 *
 * Push @buffer to the peer element of the payloader. The SSRC, payload type,
 * seqnum and timestamp of the RTP buffer will be updated first.
 * 
 * This function takes ownership of @buffer.
 *
 * Returns: a #GstFlowReturn.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstFlowReturn
gst_basertppayload_push (GstBaseRTPPayload * payload, GstBuffer * buffer)
{
  GstFlowReturn res;
  GstClockTime timestamp;
  guint32 rtptime;
  GstBaseRTPPayloadPrivate *priv;

  if (payload->clock_rate == 0)
    goto no_rate;

  priv = payload->priv;

  gst_rtp_buffer_set_ssrc (buffer, payload->current_ssrc);

  gst_rtp_buffer_set_payload_type (buffer, payload->pt);

  /* update first, so that the property is set to the last
   * seqnum pushed */
  payload->seqnum = priv->next_seqnum;
  gst_rtp_buffer_set_seq (buffer, payload->seqnum);

  /* can wrap around, which is perfectly fine */
  priv->next_seqnum++;

  /* add our random offset to the timestamp */
  rtptime = payload->ts_base;

  timestamp = GST_BUFFER_TIMESTAMP (buffer);
  if (GST_CLOCK_TIME_IS_VALID (timestamp)) {
    gint64 rtime;

    rtime = gst_segment_to_running_time (&payload->segment, GST_FORMAT_TIME,
        timestamp);

    /* take first timestamp as base, we want to calculate the RTP timestamp
     * starting from the ts_base */
    if (priv->rt_base == -1) {
      priv->rt_base = rtime;
      GST_LOG_OBJECT (payload, "first timestamp %" GST_TIME_FORMAT,
          GST_TIME_ARGS (rtime));
    }
    rtime -= priv->rt_base;

    rtime = gst_util_uint64_scale_int (rtime, payload->clock_rate, GST_SECOND);

    /* add running_time in clock-rate units to the base timestamp */
    rtptime += rtime;
  } else {
    /* no timestamp to convert, take previous timestamp */
    rtptime = payload->timestamp;
  }
  gst_rtp_buffer_set_timestamp (buffer, rtptime);

  payload->timestamp = rtptime;

  /* set caps */
  gst_buffer_set_caps (buffer, GST_PAD_CAPS (payload->srcpad));

  GST_LOG_OBJECT (payload,
      "Pushing packet size %d, seq=%d, rtptime=%u, timestamp %" GST_TIME_FORMAT,
      GST_BUFFER_SIZE (buffer), payload->seqnum, rtptime,
      GST_TIME_ARGS (timestamp));

  res = gst_pad_push (payload->srcpad, buffer);

  return res;

  /* ERRORS */
no_rate:
  {
    GST_ELEMENT_ERROR (payload, STREAM, NOT_IMPLEMENTED, (NULL),
        ("subclass did not specify clock-rate"));
    gst_buffer_unref (buffer);
    return GST_FLOW_ERROR;
  }
}

static void
gst_basertppayload_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstBaseRTPPayload *basertppayload;
  GstBaseRTPPayloadPrivate *priv;
  gint64 val;

  basertppayload = GST_BASE_RTP_PAYLOAD (object);
  priv = basertppayload->priv;

  switch (prop_id) {
    case PROP_MTU:
      basertppayload->mtu = g_value_get_uint (value);
      break;
    case PROP_PT:
      basertppayload->pt = g_value_get_uint (value);
      break;
    case PROP_SSRC:
      val = g_value_get_uint (value);
      basertppayload->ssrc = val;
      priv->ssrc_random = FALSE;
      break;
    case PROP_TIMESTAMP_OFFSET:
      val = g_value_get_uint (value);
      basertppayload->ts_offset = val;
      priv->ts_offset_random = FALSE;
      break;
    case PROP_SEQNUM_OFFSET:
      val = g_value_get_int (value);
      basertppayload->seqnum_offset = val;
      priv->seqnum_offset_random = (val == -1);
      GST_DEBUG_OBJECT (basertppayload, "seqnum offset 0x%04x, random %d",
          basertppayload->seqnum_offset, priv->seqnum_offset_random);
      break;
    case PROP_MAX_PTIME:
      basertppayload->max_ptime = g_value_get_int64 (value);
      break;
    case PROP_MIN_PTIME:
      basertppayload->min_ptime = g_value_get_int64 (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_basertppayload_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstBaseRTPPayload *basertppayload;
  GstBaseRTPPayloadPrivate *priv;

  basertppayload = GST_BASE_RTP_PAYLOAD (object);
  priv = basertppayload->priv;

  switch (prop_id) {
    case PROP_MTU:
      g_value_set_uint (value, basertppayload->mtu);
      break;
    case PROP_PT:
      g_value_set_uint (value, basertppayload->pt);
      break;
    case PROP_SSRC:
      if (priv->ssrc_random)
        g_value_set_uint (value, -1);
      else
        g_value_set_uint (value, basertppayload->ssrc);
      break;
    case PROP_TIMESTAMP_OFFSET:
      if (priv->ts_offset_random)
        g_value_set_uint (value, -1);
      else
        g_value_set_uint (value, (guint32) basertppayload->ts_offset);
      break;
    case PROP_SEQNUM_OFFSET:
      if (priv->seqnum_offset_random)
        g_value_set_int (value, -1);
      else
        g_value_set_int (value, (guint16) basertppayload->seqnum_offset);
      break;
    case PROP_MAX_PTIME:
      g_value_set_int64 (value, basertppayload->max_ptime);
      break;
    case PROP_MIN_PTIME:
      g_value_set_int64 (value, basertppayload->min_ptime);
      break;
    case PROP_TIMESTAMP:
      g_value_set_uint (value, basertppayload->timestamp);
      break;
    case PROP_SEQNUM:
      g_value_set_uint (value, basertppayload->seqnum);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstStateChangeReturn
gst_basertppayload_change_state (GstElement * element,
    GstStateChange transition)
{
  GstBaseRTPPayload *basertppayload;
  GstBaseRTPPayloadPrivate *priv;
  GstStateChangeReturn ret;

  basertppayload = GST_BASE_RTP_PAYLOAD (element);
  priv = basertppayload->priv;

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      gst_segment_init (&basertppayload->segment, GST_FORMAT_UNDEFINED);

      if (priv->seqnum_offset_random)
        basertppayload->seqnum_base =
            g_rand_int_range (basertppayload->seq_rand, 0, G_MAXUINT16);
      else
        basertppayload->seqnum_base = basertppayload->seqnum_offset;
      priv->next_seqnum = basertppayload->seqnum_base;

      if (priv->ssrc_random)
        basertppayload->current_ssrc = g_rand_int (basertppayload->ssrc_rand);
      else
        basertppayload->current_ssrc = basertppayload->ssrc;

      if (priv->ts_offset_random)
        basertppayload->ts_base = g_rand_int (basertppayload->ts_rand);
      else
        basertppayload->ts_base = basertppayload->ts_offset;

      priv->rt_base = -1;
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }
  return ret;
}
