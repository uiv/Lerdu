/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *               2000,2005 Wim Taymans <wim@fluendo.com>
 *
 * gstdevsoundsink.c:
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common.h"
#include "gstdevsoundsink.h"
#include "gsterrorconcealmentinterface.h"
#include "gstg711decoderinterface.h"
#include "gstg729decoderinterface.h"
#include "gstilbcdecoderinterface.h"
#include "string.h"
#include <glib_global.h>

GST_DEBUG_CATEGORY_EXTERN (devsound_debug);
#define GST_CAT_DEFAULT devsound_debug

/* elementfactory information */
static const GstElementDetails gst_devsound_sink_details=
GST_ELEMENT_DETAILS ("Audio Sink (DEVSOUND)",
        "Sink/Audio",
        "Output to a speaker via Devsound",
        " "
);

static void gst_devsound_sink_base_init(gpointer g_class);
static void gst_devsound_sink_class_init(GstDevsoundSinkClass * klass);
static void gst_devsound_sink_init(GstDevsoundSink * devsoundsink);

static void gst_devsound_sink_dispose(GObject * object);
static void gst_devsound_sink_finalise(GObject * object);

static void gst_devsound_sink_get_property(GObject * object, guint prop_id,
        GValue * value, GParamSpec * pspec);
static void gst_devsound_sink_set_property(GObject * object, guint prop_id,
        const GValue * value, GParamSpec * pspec);

static GstCaps *gst_devsound_sink_getcaps(GstBaseSink * bsink);
static gboolean gst_devsound_sink_setcaps(GstBaseSink *bsink, GstCaps *caps);


static gboolean gst_devsound_sink_event(GstBaseSink * asink, GstEvent * event);

static void *StartDevSoundThread(void *threadid);


//Error concealment interface impl
static void gst_error_concealment_handler_init (gpointer g_iface,
    gpointer iface_data);
static gint gst_ConcealErrorForNextBuffer();
static gint gst_SetFrameMode(gboolean aFrameMode);
static gint gst_FrameModeRqrdForEC(gboolean* aFrameModeRqrd);
static void gst_Apply_ErrorConcealment_Update(GstDevsoundSink* dssink);

//G711 interface impl
static void gst_g711_decoder_handler_init (gpointer g_iface,
    gpointer iface_data);
static gint gst_SetDecoderMode(enum TG711DecodeMode aDecodeMode);
static gint gst_SetCng(gboolean aCng);
static gint gst_GetCng(gboolean* aCng);
static gint gst_SetPlc(gboolean aPlc);
static void gst_Apply_G711_Decoder_Update(GstDevsoundSink* dssink );


//G729 interface impl
static void gst_g729_decoder_handler_init (gpointer g_iface,
    gpointer iface_data);
static gint gst_BadLsfNextBuffer();
static void gst_Apply_G729_Decoder_Update(GstDevsoundSink* dssink );

//Ilbc interface impl
static void gst_ilbc_decoder_handler_init (gpointer g_iface,
    gpointer iface_data);
static gint gst_GetIlbcCng(gboolean* aCng);
static gint gst_SetIlbcCng(gboolean aCng);
static gint gst_SetIlbcDecoderMode(enum TIlbcDecodeMode aDecodeMode);
static void  gst_Apply_Ilbc_Decoder_Update(GstDevsoundSink* dssink );


static gboolean gst_sink_start (GstBaseSink * sink);
static gboolean gst_sink_stop (GstBaseSink * sink);
static GstFlowReturn gst_sink_render (GstBaseSink * sink,
    GstBuffer * buffer);

pthread_t ds_thread;
pthread_mutex_t ds_mutex;
pthread_cond_t ds_condition;

enum consumer_thread_state_enum {
   CONSUMER_THREAD_UNINITIALIZED,
   CONSUMER_THREAD_INITIALIZING,
   CONSUMER_THREAD_INITIALIZED
};
enum consumer_thread_state_enum consumer_thread_state;



GQueue *   buffer_queue = NULL;

gboolean framemode;
gboolean framemodereq;
gboolean concealerror = FALSE;

gboolean g711decodemode;
gboolean g711cng;
gboolean g711plc;
gboolean ilbccng;
enum TIlbcDecodeMode ilbcdecodemode;
gint output;

enum
    {
    LAST_SIGNAL
    };


typedef struct _GstCustomIfaceUpdate GstCustomIfaceUpdate;

struct _GstCustomIfaceUpdate{
gboolean  framemodeupdate;
gboolean  framemoderequpdate;
gboolean  concealerrorupate;
gboolean  g711decodemodeupdate;
gboolean  g711setcngupdate;
gboolean  g711setplcupdate;
gboolean  g729badlsfnextbufferupdate;
gboolean  ilbccngupdate;
gboolean  ilbcdecodermodeupdate;
};

GstCustomIfaceUpdate customInfaceUpdate = {0,0,0,0,0,0,0,0,0};

#define DEFAULT_DEVICE  "default"

enum
    {
    PROP_0,
    PROP_DEVICE,
    VOLUME,
    MAXVOLUME,
    VOLUMERAMP,
    CHANNELS,
    LEFTBALANCE,
    RIGHTBALANCE,
    RATE,
    PRIORITY,
    PREFERENCE,
    SAMPLESPLAYED,
    FOURCC, //FOURCC is not needed
    MIMETYPE,
    OUTPUTDEVICE
    };

enum command_to_consumer_thread_enum
    {
    OPEN = 2,
    WRITEDATA,
    /*UPDATE,*/
    CLOSE
    };
enum command_to_consumer_thread_enum cmd;

static GstStaticPadTemplate devsoundsink_sink_factory=
    GST_STATIC_PAD_TEMPLATE ("sink",
        GST_PAD_SINK,
        GST_PAD_ALWAYS,
        GST_STATIC_CAPS ("audio/x-raw-int, "
                "endianness = (int) { " G_STRINGIFY (G_BYTE_ORDER) " }, "
                "signed = (boolean) TRUE, "
                "width = (int) 16, "
                "depth = (int) 16, "
                "rate = (int) [ 8000, 48000 ],"
                "channels = (int) [ 1, 2 ]; "
                "audio/amr, "
                //"width = (int) 8, "
                //"depth = (int) 8, "
                "rate = (int) 8000, "
                "channels = (int) 1 ; "
                "audio/x-alaw, "
                "rate = (int) [ 8000, 48000 ], "
                "channels = (int) [ 1, 2 ]; "
                "audio/g729, "
                "rate = (int) [ 8000, 48000 ], "
                "channels = (int) [ 1, 2 ]; "
                "audio/mp3, "
                "rate = (int) [ 8000, 48000 ], "
                "channels = (int) [ 1, 2 ]; "                
                "audio/ilbc, "
                "rate = (int) [ 8000, 48000 ], "
                "channels = (int) [ 1, 2 ]; "
                "audio/x-mulaw, "
                "rate = (int) [ 8000, 48000 ], "
                "channels = (int) [ 1, 2 ]")
                );

static GstElementClass *parent_class= NULL;

GType gst_devsound_sink_get_type(void)
    {
    static GType devsoundsink_type = 0;

    if (!devsoundsink_type)
        {
        static const GTypeInfo devsoundsink_info =
            {
                    sizeof(GstDevsoundSinkClass),
                    gst_devsound_sink_base_init,
                    NULL,
                    (GClassInitFunc) gst_devsound_sink_class_init,
                    NULL,
                    NULL,
                    sizeof(GstDevsoundSink),
                    0,
                    (GInstanceInitFunc) gst_devsound_sink_init
,        };


        static const GInterfaceInfo error_concealment_info = {
            gst_error_concealment_handler_init,
           NULL,
           NULL
             };

        static const GInterfaceInfo g711_decoder_info = {
             gst_g711_decoder_handler_init,
            NULL,
            NULL
            };

        static const GInterfaceInfo g729_decoder_info = {
                gst_g729_decoder_handler_init,
               NULL,
               NULL
               };

        static const GInterfaceInfo ilbc_decoder_info = {
                gst_ilbc_decoder_handler_init,
               NULL,
               NULL
               };

        devsoundsink_type =
        g_type_register_static (GST_TYPE_BASE_SINK, "GstDevsoundSink",
            &devsoundsink_info, 0);


        g_type_add_interface_static (devsoundsink_type, GST_TYPE_ERROR_CONCEALMENT,
         &error_concealment_info);

        g_type_add_interface_static (devsoundsink_type, GST_TYPE_G711_DECODER,
             &g711_decoder_info);

        g_type_add_interface_static (devsoundsink_type, GST_TYPE_G729_DECODER,
               &g729_decoder_info);

        g_type_add_interface_static (devsoundsink_type, GST_TYPE_ILBC_DECODER,
               &ilbc_decoder_info);

        }

    return devsoundsink_type;
    }

static void gst_devsound_sink_dispose(GObject * object)
    {
    GstDevsoundSink *devsoundsink= GST_DEVSOUND_SINK (object);

    if (devsoundsink->probed_caps)
        {
        gst_caps_unref(devsoundsink->probed_caps);
        devsoundsink->probed_caps = NULL;
        }

    G_OBJECT_CLASS (parent_class)->dispose (object);
    }

static void gst_devsound_sink_base_init(gpointer g_class)
    {
    GstElementClass *element_class= GST_ELEMENT_CLASS (g_class);

    gst_element_class_set_details(element_class, &gst_devsound_sink_details);

    gst_element_class_add_pad_template(element_class,
            gst_static_pad_template_get(&devsoundsink_sink_factory));


    }
static void gst_devsound_sink_class_init(GstDevsoundSinkClass * klass)
    {
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;
    GstBaseSinkClass *gstbasesink_class;


    gobject_class = (GObjectClass *) klass;
    gstelement_class = (GstElementClass *) klass;
    gstbasesink_class = (GstBaseSinkClass *) klass;


    parent_class = g_type_class_peek_parent(klass);

    gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_devsound_sink_dispose);
    gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_devsound_sink_finalise);
    gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_devsound_sink_get_property);
    gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_devsound_sink_set_property);

    g_object_class_install_property(gobject_class, PROP_DEVICE,
            g_param_spec_string("device", "Device", "Devsound device ",
            DEFAULT_DEVICE, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, VOLUME,
                g_param_spec_int("volume", "Volume", "Devsound volume",
                        -1, G_MAXINT, -1, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, VOLUMERAMP,
              g_param_spec_int("volumeramp", "VolumeRamp", "Devsound volume ramp",
                      -1, G_MAXINT, -1, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, MAXVOLUME,
            g_param_spec_int("maxvolume", "MaxVolume", "Devsound max volume",
                    -1, G_MAXINT, -1, G_PARAM_READABLE));

    g_object_class_install_property(gobject_class, LEFTBALANCE,
                  g_param_spec_int("leftbalance", "Left Balance", "Left Balance",
                          -1, G_MAXINT, -1, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, RIGHTBALANCE,
                   g_param_spec_int("rightbalance", "Right Balance", "Right Balance",
                           -1, G_MAXINT, -1, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, SAMPLESPLAYED,
                      g_param_spec_int("samplesplayed", "Samples Played", "Samples Played",
                              -1, G_MAXINT, -1, G_PARAM_READABLE));

    g_object_class_install_property(gobject_class, PRIORITY,
            g_param_spec_int("priority", "Priority", "Priority ", -1,
                    G_MAXINT, -1,
                    G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PREFERENCE,
            g_param_spec_int("preference", "Preference", "Preference ", -1,
                    G_MAXINT, -1,
                    G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, RATE,
            g_param_spec_int("rate", "Rate", "Rate ", -1,
                    G_MAXINT, -1,
                    G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, CHANNELS,
            g_param_spec_int("channels", "Channels", "Channels ", -1,
                    G_MAXINT, -1,
                    G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, OUTPUTDEVICE,
             g_param_spec_int("outputdevice", "Output Device", "Output Device ", -1,
                     G_MAXINT, -1,
                     G_PARAM_READWRITE));
    
    gstbasesink_class->start = GST_DEBUG_FUNCPTR (gst_sink_start);
    gstbasesink_class->stop = GST_DEBUG_FUNCPTR (gst_sink_stop);
    gstbasesink_class->render = GST_DEBUG_FUNCPTR (gst_sink_render);

    gstbasesink_class->get_caps = GST_DEBUG_FUNCPTR (gst_devsound_sink_getcaps);
    gstbasesink_class->set_caps = GST_DEBUG_FUNCPTR (gst_devsound_sink_setcaps);
    gstbasesink_class->event    = GST_DEBUG_FUNCPTR (gst_devsound_sink_event);
    }

static void gst_devsound_sink_init(GstDevsoundSink * devsoundsink)
    {
    GST_DEBUG_OBJECT(devsoundsink, "initializing devsoundsink");
    devsoundsink->device = g_strdup(DEFAULT_DEVICE);
    devsoundsink->handle = NULL;
    devsoundsink->preference = 0; //default=>EMdaPriorityPreferenceNone;
    devsoundsink->priority = 0;   //default=>EMdaPriorityNormal;
    pthread_mutex_init(&ds_mutex, NULL);
    pthread_cond_init(&ds_condition, NULL);
    }

static void *StartDevSoundThread(void *threadarg)
    {

    GstDevsoundSink *devsound;

    gint remainingDataLen = 0;
    GstBuffer *buffer = NULL;
    gboolean lastBufferSet=FALSE;
    devsound = (GstDevsoundSink*) threadarg;

    open_devsound(&(devsound->handle));


    //get supported (in/out)put datatypes
    //from devsound to build caps
    getsupporteddatatypes(devsound);

    // TODO obtain mutex to update variable here???
    consumer_thread_state = CONSUMER_THREAD_INITIALIZED;

    // Signal any waiting threads that consumer thread creation was successful.
    pthread_mutex_lock(&ds_mutex);
    pthread_cond_signal(&ds_condition);
    pthread_mutex_unlock(&ds_mutex);

    // Wait until we receive a command from the main thread
    // TODO obtain mutex to read variable here???
    while ( cmd == OPEN )
        {
        pthread_mutex_lock(&ds_mutex);
        pthread_cond_wait(&ds_condition, &ds_mutex);
        pthread_mutex_unlock(&ds_mutex);
        }

    // This could happen if client creates sink and deletes it
    // without putting it to play state
    if ( cmd != CLOSE )
        {
        //TODO if there is preemption we have to somehow signal
        //the pipeline in the render
        initialize_devsound(devsound);

        playinit(devsound->handle);
        initproperties(devsound);
        }
    while (1)
        {
        switch (cmd)
            {
            case WRITEDATA:
                {
                pre_init_setconf(devsound);
                gst_Apply_ErrorConcealment_Update(devsound);
                gst_Apply_G711_Decoder_Update(devsound);
                gst_Apply_G729_Decoder_Update(devsound);
                gst_Apply_Ilbc_Decoder_Update(devsound);

                // TODO we could do this in BTBF callback
                populateproperties(devsound);

                framemodereq = devsound->framemodereq;
                g711cng = devsound->g711cng;
                ilbccng = devsound->ilbccng;
                output = devsound->output;

                if(buffer_queue->length > 0)
                    {
                    if (remainingDataLen == 0)
                        {
                        // TODO enable lock and unlock
                        GST_OBJECT_LOCK (devsound);
                        buffer = GST_BUFFER_CAST(g_queue_peek_head(buffer_queue));
                        GST_OBJECT_UNLOCK(devsound);
                        remainingDataLen = GST_BUFFER_SIZE(buffer);
                        }

                    lastBufferSet =  GST_BUFFER_FLAG_IS_SET(buffer,GST_BUFFER_FLAG_LAST);
                    remainingDataLen = write_data(devsound->handle,
                            GST_BUFFER_DATA(buffer) + (GST_BUFFER_SIZE(buffer) - remainingDataLen),
                            remainingDataLen,
                            lastBufferSet);

                    if (remainingDataLen == 0)
                        {
                        GST_OBJECT_LOCK (devsound);
                        buffer = GST_BUFFER_CAST(g_queue_pop_head(buffer_queue));
                        GST_OBJECT_UNLOCK(devsound);
                        gst_buffer_unref(buffer);
                        buffer = NULL;
                        }

                    if (lastBufferSet && remainingDataLen == 0)
                        {
                        // Last Buffer is already sent to DevSound
                        // and we have received PlayError so now we exit
                        // from the big loop next time
/*
                        pthread_mutex_lock(&ds_mutex);
                        pthread_cond_signal(&ds_condition);
                        pthread_mutex_unlock(&ds_mutex);
*/
                        cmd = CLOSE;
                       }
                    }
                else
                    {
                    pthread_mutex_lock(&ds_mutex);
                    pthread_cond_wait(&ds_condition, &ds_mutex);
                    pthread_mutex_unlock(&ds_mutex);
                    }
                }
                break;
            case CLOSE:
                {
                close_devsound(devsound);
                devsound->handle= NULL;
                pthread_mutex_lock(&ds_mutex);
                pthread_cond_signal(&ds_condition);
                pthread_mutex_unlock(&ds_mutex);
                // TODO obtain mutex here
                consumer_thread_state = CONSUMER_THREAD_UNINITIALIZED;
                pthread_exit(NULL);
                }
                break;
            default:
                // TODO obtain mutex here
                consumer_thread_state = CONSUMER_THREAD_UNINITIALIZED;
                pthread_exit(NULL);
                break;
            }
        }
    // TODO obtain mutex here
    consumer_thread_state = CONSUMER_THREAD_UNINITIALIZED;
    pthread_exit(NULL);
    }

static gboolean gst_sink_start (GstBaseSink * sink)
    {
    GstBuffer *tmp_gstbuffer=NULL;
    GstDevsoundSink *devsound = GST_DEVSOUND_SINK(sink);

    if(buffer_queue)
        {
        while (buffer_queue->length)
            {
            tmp_gstbuffer = (GstBuffer*)g_queue_pop_tail(buffer_queue);
            gst_buffer_unref(tmp_gstbuffer);
            }
        g_queue_free(buffer_queue);
        buffer_queue = NULL;
        }
    
    if(buffer_queue == NULL)
          {
          buffer_queue = g_queue_new();
          }

    consumer_thread_state = CONSUMER_THREAD_INITIALIZING;
    cmd = OPEN;
    pthread_create(&ds_thread,  NULL, StartDevSoundThread, (void *)devsound);

    // Wait until consumer thread is created
    // TODO : obtain mutex to retreive thread state?
    if (consumer_thread_state == CONSUMER_THREAD_INITIALIZING)
        {
        pthread_mutex_lock(&ds_mutex);
        pthread_cond_wait(&ds_condition, &ds_mutex);
        pthread_mutex_unlock(&ds_mutex);
        }

    return TRUE;
    }

static gboolean gst_sink_stop (GstBaseSink * sink)
    {
    GstBuffer *tmp_gstbuffer=NULL;
    GstDevsoundSink *devsound = GST_DEVSOUND_SINK(sink);

    cmd = CLOSE;

    pthread_mutex_lock(&ds_mutex);
    pthread_cond_signal(&ds_condition);
    pthread_mutex_unlock(&ds_mutex);

    GST_OBJECT_LOCK(devsound);
    while (buffer_queue->length)
        {
        tmp_gstbuffer = (GstBuffer*)g_queue_pop_tail(buffer_queue);
        gst_buffer_unref(tmp_gstbuffer);
        }
    g_queue_free(buffer_queue);
    buffer_queue = NULL;
    GST_OBJECT_UNLOCK(devsound);

    return TRUE;
    }

static GstFlowReturn gst_sink_render (GstBaseSink * sink,
    GstBuffer * buffer)
    {
    GstDevsoundSink *devsound = GST_DEVSOUND_SINK(sink);
    GstBuffer* tmp;

    if (get_ds_cb_error(devsound->handle))
        {
        return GST_FLOW_CUSTOM_ERROR;
        }
        
    tmp = gst_buffer_copy(buffer);
 
    GST_OBJECT_LOCK (devsound);
    g_queue_push_tail (buffer_queue, tmp);
    GST_OBJECT_UNLOCK (devsound);

    cmd = WRITEDATA;
    pthread_mutex_lock(&ds_mutex);
    pthread_cond_signal(&ds_condition);
    pthread_mutex_unlock(&ds_mutex);

    return GST_FLOW_OK;
    }

static void gst_devsound_sink_finalise(GObject * object)
    {
    GstDevsoundSink *devsoundsink= GST_DEVSOUND_SINK (object);
    g_free(devsoundsink->device);

    }

static void gst_devsound_sink_set_property(GObject * object, guint prop_id,
        const GValue * value, GParamSpec * pspec)
    {
    GstDevsoundSink *sink;

    sink = GST_DEVSOUND_SINK (object);

    switch (prop_id)
        {
        case PROP_DEVICE:
            if (sink->device)
                g_free(sink->device);
            sink->device = g_value_dup_string(value);
            if (sink->probed_caps)
                {
                gst_caps_unref(sink->probed_caps);
                sink->probed_caps = NULL;
                }
            break;
        case CHANNELS:
            sink->channels = g_value_get_int(value);
            sink->pending.channelsupdate = TRUE;
            break;
        case RATE:
            sink->rate = g_value_get_int(value);
            //Convert rate to something devsound understands
            sink->rate = gst_devsound_sink_get_rate(sink->rate);
            sink->pending.rateupdate = TRUE;
            break;
        case VOLUME:
            sink->volume = g_value_get_int(value);
            sink->pending.volumeupdate = TRUE;
            break;
        case LEFTBALANCE:
            sink->leftbalance = g_value_get_int(value);
            sink->pending.leftbalanceupdate = TRUE;
            break;
        case RIGHTBALANCE:
            sink->rightbalance = g_value_get_int(value);
            sink->pending.rightbalanceupdate = TRUE;
            break;
        case VOLUMERAMP:
            sink->volumeramp = g_value_get_int(value);
            sink->pending.volumerampupdate = TRUE;
            break;
        case PRIORITY:
            sink->priority = g_value_get_int(value);
            sink->pending.priorityupdate = TRUE;
            break;
        case PREFERENCE:
            sink->preference = g_value_get_int(value);
            sink->pending.preferenceupdate = TRUE;
            break;
        case FOURCC: //FOURCC is not needed
            sink->fourcc = g_value_get_int(value);
            sink->pending.fourccupdate = TRUE;
            break;

        case MIMETYPE:
            sink->mimetype = g_value_dup_string(value);
            break;
        case OUTPUTDEVICE:
            sink->output = g_value_get_int(value);
            sink->pending.outputupdate = TRUE;
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
        }

    }

static void gst_devsound_sink_get_property(GObject * object, guint prop_id,
        GValue * value, GParamSpec * pspec)
    {
    GstDevsoundSink *sink;
    sink = GST_DEVSOUND_SINK (object);

    switch (prop_id)
        {
        case PROP_DEVICE:
            g_value_set_string(value, sink->device);
            break;
        case CHANNELS:
            g_value_set_int(value, sink->channels);
            break;
        case RATE:
            g_value_set_int(value, sink->rate);
            break;
        case VOLUME:
            g_value_set_int(value, sink->volume);
            break;
        case MAXVOLUME:
            g_value_set_int(value, sink->maxvolume);
            break;
        case SAMPLESPLAYED:
              g_value_set_int(value, sink->samplesplayed);
              break;
        case OUTPUTDEVICE:
              g_value_set_int(value, sink->output);
              break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
        }
    }

static gboolean gst_devsound_sink_setcaps(GstBaseSink *bsink, GstCaps *caps)
    {
    GstDevsoundSink *devsoundsink;
    GstStructure *structure;
    const gchar *mimetype;
    int width,depth,endianness,sign,channels,rate;

    devsoundsink = GST_DEVSOUND_SINK (bsink);

    structure = gst_caps_get_structure (caps, 0);
    mimetype = gst_structure_get_name (structure);

    gst_structure_get_int (structure, "width", &width);
    gst_structure_get_int (structure, "depth", &depth);
    gst_structure_get_int (structure, "endianness", &endianness);
    gst_structure_get_int (structure, "rate", &rate);
    gst_structure_get_int (structure, "channels",&channels);
    gst_structure_get_boolean (structure, "signed", &sign);


    devsoundsink->rate = gst_devsound_sink_get_rate(rate);
    devsoundsink->channels = channels;

    set_channels(devsoundsink->handle,devsoundsink->channels);

    set_rate(devsoundsink->handle,devsoundsink->rate);

    if (!strncmp (mimetype, "audio/x-raw-int", 15))
        {
        devsoundsink->fourcc = 0x36315020; //KMMFFourCCCodePCM16
        }
    else if (!strncmp(mimetype,"audio/amr",9))
        {
        devsoundsink->fourcc = 0x524d4120; //KMMFFourCCCodeAMR
        }
    else if (!strncmp (mimetype, "audio/x-mulaw", 13)||
             !strncmp (mimetype, "audio/x-alaw", 12))
        {
        devsoundsink->fourcc = 0x31313747; //KMccFourCCIdG711
        }
    else if (!strncmp(mimetype, "audio/ilbc", 10))
        {
        devsoundsink->fourcc = 0x43424c49; //KMccFourCCIdILBC
        }
    else if (!strncmp(mimetype, "audio/g729", 10))
        {
        devsoundsink->fourcc = 0x39323747; //KMccFourCCIdG729   
        }
    else if (!strncmp(mimetype, "audio/mp3", 9))
        {
        devsoundsink->fourcc = 0x33504d20; //KMMFFourCCCodeMP3    
        }
    else
        {
        devsoundsink->fourcc = 0x36315020; //KMMFFourCCCodePCM16
        }

    set_fourcc(devsoundsink->handle,devsoundsink->fourcc);

    pthread_mutex_lock(&ds_mutex);
    pthread_cond_signal(&ds_condition);
    pthread_mutex_unlock(&ds_mutex);

    return TRUE;
    }
static GstCaps * gst_devsound_sink_getcaps(GstBaseSink * bsink)
    {
    GstDevsoundSink *devsoundsink;
    GstCaps *caps;
    GstPadTemplate *pad_template;

   // GstStructure *str;
   //GstCaps *caps2;
   //GstCaps *caps3;

    devsoundsink = GST_DEVSOUND_SINK (bsink);

    GST_DEBUG_OBJECT(devsoundsink, "getcaps called");

    pad_template = gst_static_pad_template_get(&devsoundsink_sink_factory);
    caps = gst_caps_copy(gst_pad_template_get_caps(pad_template));

    return caps;
    //************
 /*   caps2 = gst_caps_new_empty();
    total = g_list_length(devsoundsink->fmt);

    for (;devsoundsink->fmt; devsoundsink->fmt = g_list_next(devsoundsink->fmt))
        {
        data = (guint*)devsoundsink->fmt->data;
        switch ((gint)data)
            {
            case 0x36315020: //KMMFFourCCCodePCM16
                str = gst_structure_new("audio/x-raw-int",
                        "signed",G_TYPE_BOOLEAN, TRUE,
                        "width", G_TYPE_INT, 16,
                        "depth", G_TYPE_INT, 16,
                        "channels", GST_TYPE_INT_RANGE,1, 2,
                        "rate", GST_TYPE_INT_RANGE,8000, 48000, NULL);
                break;

            case 0x524d4120: //KMMFFourCCCodeAMR
                str = gst_structure_new("audio/amr",
                        "width", G_TYPE_INT, 8,
                        "depth", G_TYPE_INT, 8,
                        "channels", G_TYPE_INT, 1,
                        "rate", G_TYPE_INT, 8000,
                        NULL);
                break;
            case 0x31313747: //KMccFourCCIdG711
                str = gst_structure_new("audio/g711",
                        "width", G_TYPE_INT, 8,
                        "depth", G_TYPE_INT, 8,
                        "channels", G_TYPE_INT, 1,
                        "rate", G_TYPE_INT, 8000,
                        NULL);
                break;
            default:
                str = NULL;
                break;
            }

        if(str!=NULL)
            gst_caps_append_structure(caps2, str);
        }

    caps3 = gst_caps_union(caps,caps2);
    //  gst_object_unref(pad_template);

    return caps3;
    */
    }


static gint gst_devsound_sink_get_rate(gint rate)
    {
    gint result;

    switch (rate)
        {
        case 8000:
            result=0x00000001;//EMMFSampleRate8000Hz;
            break;
        case 11025:
            result=0x00000002;//EMMFSampleRate11025Hz;
            break;
        case 16000:
            result=0x00000004;//EMMFSampleRate16000Hz;
            break;
        case 22050:
            result=0x00000008;//EMMFSampleRate22050Hz;
            break;
        case 32000:
            result=0x00000010;//EMMFSampleRate32000Hz;
            break;
        case 44100:
            result=0x00000020;//EMMFSampleRate44100Hz;
            break;
        case 48000:
            result=0x00000040;//EMMFSampleRate48000Hz;
            break;
        case 88200:
            result=0x00000080;//EMMFSampleRate88200Hz;
            break;
        case 96000:
            result= 0x00000100;//EMMFSampleRate96000Hz;
            break;
        case 12000:
            result=0x00000200;//EMMFSampleRate12000Hz;
            break;
        case 24000:
            result=0x00000400;//EMMFSampleRate24000Hz;
            break;
        case 64000:
            result=0x00000800; //EMMFSampleRate64000Hz;
            break;
        default:
            result=0x00000001;//EMMFSampleRate8000Hz;
            break;

        }
    return result;

    }



static gboolean gst_devsound_sink_event(GstBaseSink *asink, GstEvent *event)
    {
    GstDevsoundSink *sink= GST_DEVSOUND_SINK (asink);
    GstBuffer* lastBuffer = NULL;
    switch (GST_EVENT_TYPE (event))
        {
        case GST_EVENT_EOS:
            // end-of-stream, we should close down all stream leftovers here
            //reset_devsound(sink->handle);

            if(buffer_queue->length)
                {
                GST_OBJECT_LOCK(sink);
                lastBuffer = GST_BUFFER(g_queue_peek_tail(buffer_queue));
                GST_BUFFER_FLAG_SET(lastBuffer,GST_BUFFER_FLAG_LAST);
                GST_OBJECT_UNLOCK(sink);
                }
            else
                {
                lastBuffer = gst_buffer_new();
                GST_BUFFER_FLAG_SET(lastBuffer,GST_BUFFER_FLAG_LAST);
                GST_OBJECT_LOCK(sink);
                g_queue_push_tail(buffer_queue,lastBuffer);
                GST_OBJECT_UNLOCK(sink);
                cmd = WRITEDATA;
                pthread_mutex_lock(&ds_mutex);
                pthread_cond_signal(&ds_condition);
                pthread_mutex_unlock(&ds_mutex);
                }
            pthread_mutex_lock(&ds_mutex);
            pthread_cond_wait(&ds_condition, &ds_mutex);
            pthread_mutex_unlock(&ds_mutex);

            break;
        default:
            break;
        }

    return TRUE;
    }


/************************************
* Error Concealment Interface begins
* **********************************/
static void gst_error_concealment_handler_init (gpointer g_iface,
    gpointer iface_data)
    {
    GstErrorConcealmentIntfc *iface = (GstErrorConcealmentIntfc *) g_iface;

    iface->ConcealErrorForNextBuffer = gst_ConcealErrorForNextBuffer;
    iface->SetFrameMode = gst_SetFrameMode;
    iface->FrameModeRqrdForEC = gst_FrameModeRqrdForEC;

    }


static gint gst_ConcealErrorForNextBuffer()
    {
    customInfaceUpdate.concealerrorupate = TRUE;
    return 0;
    }

static gint gst_SetFrameMode(gboolean aFrameMode)
    {
    customInfaceUpdate.framemodeupdate = TRUE;
    framemode = aFrameMode;
    return 0;
    }

static gint gst_FrameModeRqrdForEC(gboolean* aFrameModeRqrd)
    {
    *aFrameModeRqrd = framemodereq;
    return 0;
    }

static void gst_Apply_ErrorConcealment_Update(GstDevsoundSink* dssink)
    {

    if(customInfaceUpdate.concealerrorupate == TRUE)
        {
        conceal_error_for_next_buffer(dssink->handle);
        customInfaceUpdate.concealerrorupate = FALSE;
        }
    if(customInfaceUpdate.framemodeupdate == TRUE)
        {
        set_framemode(dssink->handle,framemode);
        customInfaceUpdate.framemodeupdate = FALSE;
        }
    }

/***********************************************************
 * G711 Decoder interface begins
 * *********************************************************/
static void gst_g711_decoder_handler_init (gpointer g_iface,
    gpointer iface_data)
    {
    GstG711DecoderIntfc *iface = (GstG711DecoderIntfc *) g_iface;

    iface->GetCng         = gst_GetCng;
    iface->SetCng         = gst_SetCng;
    iface->SetDecoderMode = gst_SetDecoderMode;
    iface->SetPlc         = gst_SetPlc;

    }

static gint gst_SetDecoderMode(enum TG711DecodeMode aDecodeMode)
    {
    customInfaceUpdate.g711decodemodeupdate = TRUE;
    g711decodemode  = aDecodeMode;
    return 0;
    }
static gint gst_SetCng(gboolean aCng)
    {
    customInfaceUpdate.g711setcngupdate = TRUE;
    g711cng = aCng;
    return 0;
    }
static gint gst_GetCng(gboolean* aCng)
    {
    *aCng = g711cng;
    return 0;
    }
static gint gst_SetPlc(gboolean aPlc)
    {
    customInfaceUpdate.g711setplcupdate = TRUE;
    g711plc = aPlc;
    return 0;
    }

static void gst_Apply_G711_Decoder_Update(GstDevsoundSink* dssink )
    {

    if(customInfaceUpdate.g711decodemodeupdate == TRUE)
        {
        set_decodermode(dssink->handle,g711decodemode);
        customInfaceUpdate.g711decodemodeupdate = FALSE;
        }
    if(customInfaceUpdate.g711setcngupdate == TRUE)
        {
        set_cng(dssink->handle,g711cng);
        customInfaceUpdate.g711setcngupdate = FALSE;
        }
    if(customInfaceUpdate.g711setplcupdate == TRUE)
        {
        set_plc(dssink->handle,g711plc);
        customInfaceUpdate.g711setplcupdate = FALSE;
        }

    }

//G729 interface impl
static void gst_g729_decoder_handler_init (gpointer g_iface,
    gpointer iface_data)
    {
    GstG729DecoderIntfc *iface = (GstG729DecoderIntfc *) g_iface;

    iface->BadLsfNextBuffer = gst_BadLsfNextBuffer;

    }

static gint gst_BadLsfNextBuffer()
    {
    customInfaceUpdate.g729badlsfnextbufferupdate = TRUE;
    }

static void gst_Apply_G729_Decoder_Update(GstDevsoundSink* dssink )
    {
    if(customInfaceUpdate.g729badlsfnextbufferupdate)
        {
        badlsfnextbuffer(dssink->handle);
        customInfaceUpdate.g729badlsfnextbufferupdate = FALSE;
        }

    }

//Ilbc interface impl
static void gst_ilbc_decoder_handler_init (gpointer g_iface,
    gpointer iface_data)
    {
    GstIlbcDecoderIntfc *iface = (GstIlbcDecoderIntfc *) g_iface;

    iface->GetCng = gst_GetIlbcCng;
    iface->SetCng = gst_SetIlbcCng;
    iface->SetDecoderMode = gst_SetIlbcDecoderMode;

    }

static gint gst_GetIlbcCng(gboolean* aCng)
    {
    *aCng = ilbccng;
    return 0;
    }

static gint gst_SetIlbcCng(gboolean aCng)
    {
    customInfaceUpdate.ilbccngupdate = TRUE;
    ilbccng = aCng;
    return 0;
    }
static gint gst_SetIlbcDecoderMode(enum TIlbcDecodeMode aDecodeMode)
    {
    customInfaceUpdate.ilbcdecodermodeupdate = TRUE;
    ilbcdecodemode = aDecodeMode;
    return 0;
    }

static void gst_Apply_Ilbc_Decoder_Update(GstDevsoundSink* dssink )
    {

    if(customInfaceUpdate.ilbccngupdate)
        {
        set_ilbccng(dssink->handle,ilbccng);
        customInfaceUpdate.ilbccngupdate = FALSE;
        }
    if(customInfaceUpdate.ilbcdecodermodeupdate)
        {
        set_ilbcdecodermode(dssink->handle,ilbcdecodemode);
        customInfaceUpdate.ilbcdecodermodeupdate = FALSE;
        }
    }
