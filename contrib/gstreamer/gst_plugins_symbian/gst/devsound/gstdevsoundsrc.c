/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *               2000,2005 Wim Taymans <wim@fluendo.com>
 *
 * gstdevsoundsrc.c:
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

#include <gqueue.h>
#include "common.h"
#include "gstdevsoundsrc.h"

#include <glib_global.h>
#include "gstg711encoderinterface.h"
#include "gstspeechencoderconfig.h"
#include "gstg729encoderinterface.h"
#include "gstilbcencoderinterface.h"
#include "string.h"

GST_DEBUG_CATEGORY_EXTERN (devsoundsrc_debug);
#define GST_CAT_DEFAULT devsoundsrc_debug

/* elementfactory information */
static const GstElementDetails gst_devsound_src_details=
GST_ELEMENT_DETAILS ("Audio Src (DEVSOUND)",
        "Src/Audio",
        "Record via Devsound",
        " "
);

static void gst_devsound_src_base_init(gpointer g_class);
static void gst_devsound_src_class_init(GstDevsoundSrcClass * klass);
static void gst_devsound_src_init(GstDevsoundSrc * devsoundsrc);

static void gst_devsound_src_dispose(GObject * object);

static void gst_devsound_src_get_property(GObject * object, guint prop_id,
        GValue * value, GParamSpec * pspec);
static void gst_devsound_src_set_property(GObject * object, guint prop_id,
        const GValue * value, GParamSpec * pspec);

static GstCaps *gst_devsound_src_getcaps(GstBaseSrc * bsrc);
static gboolean gst_devsound_src_setcaps(GstBaseSrc *bsrc, GstCaps *caps);

static gboolean gst_devsound_src_start(GstBaseSrc * bsrc);
static gboolean gst_devsound_src_stop(GstBaseSrc * bsrc);
static gboolean gst_devsound_src_unlock(GstBaseSrc * bsrc);
static gboolean gst_devsound_src_unlock_stop(GstBaseSrc * bsrc);
static gboolean gst_devsound_src_is_seekable(GstBaseSrc * bsrc);
static gboolean gst_devsound_src_get_size(GstBaseSrc * src, guint64 * size);

static GstFlowReturn gst_devsound_src_create(GstBaseSrc *src, guint64 offset,
        guint size, GstBuffer **buf);
static void *StartDevSoundThread(void *threadid);

/*********************************
 * Speech Encoder Config Interface
 * ******************************/
static void gst_speech_encoder_config_handler_init(gpointer g_iface,
        gpointer iface_data);
static gint gst_get_speech_encoder_bit_rate(guint* aBitrate);
static gint gst_get_speech_encoder_supported_bit_rates(
        GList** aSupportedBitrates);
static gint gst_get_speech_encoder_vad_mode(gboolean* aVadMode);
static gint gst_set_speech_encoder_bit_rate(guint aBitrate);
static gint gst_set_speech_encoder_vad_mode(gboolean aVadMode);
static void gst_Apply_SpeechEncoder_Update(GstDevsoundSrc *devsoundsrc );


/*************************
 * G711 Encoder Interface
 * ***********************/
static void gst_g711_encoder_handler_init(gpointer g_iface,
        gpointer iface_data);
static gint gst_get_g711_encoder_vad_mode(gboolean* aVadMode);
static gint gst_set_g711_encoder_mode(enum TG711EncodeMode aEncodeMode);
static gint gst_set_g711_vad_mode(gboolean aVadMode);
static void gst_Apply_G711Encoder_Update(GstDevsoundSrc *devsoundsrc );


/*************************
 * 729 Encoder Interface
 * ***********************/
static void gst_g729_encoder_handler_init(gpointer g_iface,
        gpointer iface_data);
static gint gst_set_g729_vad_mode(TBool aVadMode);
static gint gst_get_g729_vad_mode(TBool* aVadMode);
static void gst_Apply_G729Encoder_Update(GstDevsoundSrc *devsoundsrc );


/*************************
 * ilbc Encoder Interface
 * ***********************/
static void gst_ilbc_encoder_handler_init(gpointer g_iface,
        gpointer iface_data);
static gint gst_set_ilbc_encoder_mode(enum TIlbcEncodeMode aEncodeMode);
static gint gst_set_ilbc_vad_mode(gboolean aVadMode);
static gint gst_get_ilbc_vad_mode(gboolean* aVadMode);
static void gst_Apply_IlbcEncoder_Update(GstDevsoundSrc *devsoundsrc );


static gint gst_devsound_src_get_rate(gint rate);


typedef struct _GstCustomIfaceUpdate GstCustomIfaceUpdate;

struct _GstCustomIfaceUpdate{
gboolean  speechbitrateupdate;
gboolean  speechvadmodeupdate;
gboolean  g711encodermodeupdate;
gboolean  g711vadmodeupdate;
gboolean  g729vadmodeupdate;
gboolean  ilbcencodermodeupdate;
gboolean  ilbcvadmodeupdate;
};
GstCustomIfaceUpdate customInfaceUpdate = {0,0,0,0,0,0,0};

guint speechbitrate;
gboolean speechvadmode;
enum TG711EncodeMode g711encodemode;
gboolean g711vadmode;
GList* supportedbitrates;
gint numofbitrates;
gboolean g729vadmode;
enum TIlbcEncodeMode ilbcencodemode;
gboolean ilbcvadmode;

pthread_t ds_thread;
pthread_mutex_t create_mutex1;
pthread_cond_t create_condition1;

enum consumer_thread_state_enum {
   CONSUMER_THREAD_UNINITIALIZED,
   CONSUMER_THREAD_INITIALIZING,
   CONSUMER_THREAD_INITIALIZED
};
enum consumer_thread_state_enum consumer_thread_state;



GQueue* dataqueue= NULL;
GstBuffer* popBuffer= NULL;
int dataleft = 0;
int dataCopied = 0;

enum command_to_consumer_thread_enum
    {
    OPEN = 2,
    READDATA,
    /*UPDATE,*/
    CLOSE
    };
enum command_to_consumer_thread_enum cmd;

enum
    {
    LAST_SIGNAL
    };

#define DEFAULT_DEVICE  "default"

enum
    {
    PROP_0,
    PROP_DEVICE,
    GAIN,
    MAXGAIN,
    CHANNELS,
    LEFTBALANCE,
    RIGHTBALANCE,
    RATE,
    PRIORITY,
    PREFERENCE,
    SAMPLESRECORDED
    };

static GstStaticPadTemplate devsound_src_factory=
    GST_STATIC_PAD_TEMPLATE ("src",
        GST_PAD_SRC,
        GST_PAD_ALWAYS,
        GST_STATIC_CAPS ("audio/x-raw-int, "
                "endianness = (int) { " G_STRINGIFY (G_BYTE_ORDER) " }, "
                "signed = (boolean) TRUE, "
                "width = (int) 16, "
                "depth = (int) 16, "
                "rate = (int) [ 8000, 48000 ],"
                "channels = (int) [ 1, 2 ]; "
                "audio/amr, "
                "rate = (int) [ 8000, 48000 ], "
                "channels = (int) [ 1, 2 ]; "
                "audio/x-alaw, "
                "rate = (int) [ 8000, 48000 ], "
                "channels = (int) [ 1, 2 ]; "
                "audio/g729, "
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

static void gst_devsound_src_base_init(gpointer g_class)
    {
    GstElementClass *element_class= GST_ELEMENT_CLASS (g_class);

    gst_element_class_set_details(element_class, &gst_devsound_src_details);

    gst_element_class_add_pad_template(element_class,
            gst_static_pad_template_get(&devsound_src_factory));
    }

GType gst_devsound_src_get_type(void)
    {
    static GType devsoundsrc_type = 0;

    if (!devsoundsrc_type)
        {
        static const GTypeInfo devsoundsrc_info =
            {
            sizeof(GstDevsoundSrcClass), gst_devsound_src_base_init, NULL,
                    (GClassInitFunc) gst_devsound_src_class_init, NULL, NULL,
                    sizeof(GstDevsoundSrc), 0,
                    (GInstanceInitFunc) gst_devsound_src_init
,        };

    static const GInterfaceInfo speech_encoder_config_info =
        {
        gst_speech_encoder_config_handler_init,
        NULL,
        NULL
        };

    static const GInterfaceInfo g711_encoder_info =
        {
        gst_g711_encoder_handler_init,
        NULL,
        NULL
        };

    static const GInterfaceInfo g729_encoder_info =
         {
         gst_g729_encoder_handler_init,
         NULL,
         NULL
         };

    static const GInterfaceInfo ilbc_encoder_info =
         {
         gst_ilbc_encoder_handler_init,
         NULL,
         NULL
         };

    devsoundsrc_type =
    g_type_register_static (GST_TYPE_PUSH_SRC, "GstDevsoundSrc",
            &devsoundsrc_info, 0);

    g_type_add_interface_static (devsoundsrc_type, GST_TYPE_SPEECH_ENCODER_CONFIG,
            &speech_encoder_config_info);

    g_type_add_interface_static (devsoundsrc_type, GST_TYPE_G711_ENCODER,
            &g711_encoder_info);

    g_type_add_interface_static (devsoundsrc_type, GST_TYPE_G729_ENCODER,
              &g729_encoder_info);

    g_type_add_interface_static (devsoundsrc_type, GST_TYPE_ILBC_ENCODER,
              &ilbc_encoder_info);

    }

return devsoundsrc_type;
}

static void gst_devsound_src_dispose(GObject * object)
    {
    GstDevsoundSrc *devsoundsrc= GST_DEVSOUND_SRC (object);
    G_OBJECT_CLASS (parent_class)->dispose (object);
    }

static void gst_devsound_src_class_init(GstDevsoundSrcClass * klass)
    {
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;
    GstBaseSrcClass *gstbasesrc_class;
    GstPushSrcClass *gstpushsrc_class;

    gobject_class = (GObjectClass *) klass;
    gstelement_class = (GstElementClass *) klass;
    gstbasesrc_class = (GstBaseSrcClass *) klass;
    gstpushsrc_class = (GstPushSrcClass *) klass;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_devsound_src_dispose);
    gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_devsound_src_get_property);
    gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_devsound_src_set_property);

    g_object_class_install_property(gobject_class, PROP_DEVICE,
            g_param_spec_string("device", "Device", "Devsound device ",
                    DEFAULT_DEVICE, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, GAIN, g_param_spec_int(
            "gain", "Gain", "Devsound src gain", -1, G_MAXINT, -1,
            G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, MAXGAIN, g_param_spec_int(
            "maxgain", "MaxGain", "Devsound src max gain", -1, G_MAXINT, -1,
            G_PARAM_READABLE));

    g_object_class_install_property(gobject_class, LEFTBALANCE,
            g_param_spec_int("leftbalance", "Left Balance", "Left Balance",
                    -1, G_MAXINT, -1, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, RIGHTBALANCE,
            g_param_spec_int("rightbalance", "Right Balance",
                    "Right Balance", -1, G_MAXINT, -1, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, SAMPLESRECORDED,
            g_param_spec_int("samplesrecorded", "Samples Recorded",
                    "Samples Recorded", -1, G_MAXINT, -1, G_PARAM_READABLE));

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
    gstbasesrc_class->start = GST_DEBUG_FUNCPTR (gst_devsound_src_start);
    gstbasesrc_class->stop = GST_DEBUG_FUNCPTR (gst_devsound_src_stop);
    gstbasesrc_class->get_caps = GST_DEBUG_FUNCPTR (gst_devsound_src_getcaps);
    gstbasesrc_class->set_caps = GST_DEBUG_FUNCPTR (gst_devsound_src_setcaps);

    gstbasesrc_class->create = GST_DEBUG_FUNCPTR (gst_devsound_src_create);
    }

static void gst_devsound_src_init(GstDevsoundSrc * devsoundsrc)
    {
    GST_DEBUG_OBJECT(devsoundsrc, "initializing devsoundsrc");
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "gst_devsound_src_init ENTER ",NULL);
    devsoundsrc->device = g_strdup(DEFAULT_DEVICE);
    devsoundsrc->handle=NULL;
    devsoundsrc->preference = 0; //default=>EMdaPriorityPreferenceNone;
    devsoundsrc->priority = 0;   //default=>EMdaPriorityNormal;
    pthread_mutex_init(&create_mutex1, NULL);
    pthread_cond_init(&create_condition1, NULL);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "gst_devsound_src_init EXIT ",NULL);
    }

static void *StartDevSoundThread(void *threadarg)
    {
    GstDevsoundSrc *devsoundsrc= GST_DEVSOUND_SRC (threadarg);
    int buffersize;
    TUint8* gBuffer;
    GstBuffer* pushBuffer= NULL;
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "StartDevSoundThread ",NULL);
    open_devsound(&(devsoundsrc->handle));

    getsupporteddatatypes(devsoundsrc);

// TODO obtain mutex to update variable here???
    consumer_thread_state = CONSUMER_THREAD_INITIALIZED;
	
    //pre_init_setconf(devsoundsrc);

    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "After Record Init ",NULL);

    pthread_mutex_lock(&(create_mutex1));
    pthread_cond_signal(&(create_condition1));
    pthread_mutex_unlock(&(create_mutex1));

    // Wait until we receive a command from the main thread
    // TODO obtain mutex to read variable here???
    while ( cmd == OPEN )
        {
        pthread_mutex_lock(&create_mutex1);
        pthread_cond_wait(&create_condition1, &create_mutex1);
        pthread_mutex_unlock(&create_mutex1);
        }

    // This could happen if client creates sink and deletes it
    // without putting it to play state
    if ( cmd != CLOSE )
        {
        initialize_devsound(devsoundsrc);
    
        recordinit(devsoundsrc->handle);
        initproperties(devsoundsrc);
        }
    //cmd = READDATA;
    while (1)
        {
        //set/get properties
        //***************************************
        pre_init_setconf(devsoundsrc);
        gst_Apply_SpeechEncoder_Update(devsoundsrc);
        gst_Apply_G711Encoder_Update(devsoundsrc);
        gst_Apply_G729Encoder_Update(devsoundsrc );
        gst_Apply_IlbcEncoder_Update(devsoundsrc );

        populateproperties(devsoundsrc);

        supportedbitrates = devsoundsrc->supportedbitrates;
        //numofbitrates = devsoundsrc->numofbitrates;
        speechbitrate = devsoundsrc->speechbitrate;
        speechvadmode = devsoundsrc->speechvadmode;
        g711vadmode = devsoundsrc->g711vadmode;
        g729vadmode  = devsoundsrc->g729vadmode;
        ilbcvadmode = devsoundsrc->ilbcvadmode;


        //****************************************
        //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "Before Buffer Alloc ",NULL);
        buffersize = get_databuffer_size(devsoundsrc->handle);
        get_databuffer(devsoundsrc->handle, &gBuffer);
        pushBuffer = gst_buffer_new_and_alloc(buffersize);
        //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "After Buffer Alloc ",NULL);
        if (GST_BUFFER_DATA(pushBuffer))
            {
            memcpy(GST_BUFFER_DATA(pushBuffer),gBuffer,buffersize);
            }
        else
            {
            //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "Push buffer alloc failed ",NULL);
            }

        if (dataqueue)
            {
            GST_OBJECT_LOCK(devsoundsrc);
            g_queue_push_head (dataqueue,pushBuffer);
            GST_OBJECT_UNLOCK(devsoundsrc);
            //signalmutex_create(devsoundsrc->handle);
            if(dataqueue->length == 1 && (cmd != CLOSE))
                {
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "Before signal in DevSoundt ",NULL);
                pthread_mutex_lock(&(create_mutex1));
                pthread_cond_signal(&(create_condition1));
                pthread_mutex_unlock(&(create_mutex1));
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "After signal in DevSoundt ",NULL);
                }
            //cmd = READDATA;
            //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "Before DevSnd Wait ",NULL);
            //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "After DevSnd Wait ",NULL);
           }
        else
            {
            //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "dataqueue is NULL, CLOSE now ",NULL);
            cmd = CLOSE;
            }

        switch (cmd)
            {
            case READDATA:
                {
                record_data(devsoundsrc->handle);
                }
                break;
            case CLOSE:
                {
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "Before Close DevSound ",NULL);
                close_devsound(devsoundsrc);
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "After Close DevSound ",NULL);
                devsoundsrc->handle= NULL;
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "After handle NULL ",NULL);

                pthread_mutex_lock(&(create_mutex1));
                pthread_cond_signal(&(create_condition1));
                pthread_mutex_unlock(&(create_mutex1));
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

static void gst_devsound_src_set_property(GObject * object, guint prop_id,
        const GValue * value, GParamSpec * pspec)
    {
    GstDevsoundSrc *src;

    src = GST_DEVSOUND_SRC (object);

    switch (prop_id)
        {
        case PROP_DEVICE:
            if (src->device)
                g_free(src->device);
            src->device = g_value_dup_string(value);
            if (src->probed_caps)
                {
                gst_caps_unref(src->probed_caps);
                src->probed_caps = NULL;
                }
            break;
        case CHANNELS:
            src->channels = g_value_get_int(value);
            src->pending.channelsupdate = TRUE;
            break;
        case RATE:
            src->rate = g_value_get_int(value);
            //Convert rate to something devsound understands
            src->rate = gst_devsound_src_get_rate(src->rate);
            src->pending.rateupdate = TRUE;
            break;
        case GAIN:
            src->gain = g_value_get_int(value);
            src->pending.gainupdate = TRUE;
            break;
        case LEFTBALANCE:
            src->leftbalance = g_value_get_int(value);
            src->pending.leftbalanceupdate = TRUE;
            break;
        case RIGHTBALANCE:
            src->rightbalance = g_value_get_int(value);
            src->pending.rightbalanceupdate = TRUE;
            break;
        case PRIORITY:
            src->priority = g_value_get_int(value);
            src->pending.priorityupdate = TRUE;
            break;
        case PREFERENCE:
            src->preference = g_value_get_int(value);
            src->pending.preferenceupdate = TRUE;
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
        }
    }

static void gst_devsound_src_get_property(GObject * object, guint prop_id,
        GValue * value, GParamSpec * pspec)
    {
    GstDevsoundSrc *src;

    src = GST_DEVSOUND_SRC (object);

    switch (prop_id)
        {
        case PROP_DEVICE:
            g_value_set_string(value, src->device);
            break;
        case CHANNELS:
            g_value_set_int(value, src->channels);
            break;
        case RATE:
            g_value_set_int(value, src->rate);
            break;
        case GAIN:
            g_value_set_int(value, src->gain);
            break;
        case MAXGAIN:
            g_value_set_int(value, src->maxgain);
            break;
        case SAMPLESRECORDED:
            g_value_set_int(value, src->samplesrecorded);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
        }
    }

static gboolean gst_devsound_src_setcaps(GstBaseSrc *bsrc, GstCaps *caps)
    {
    GstDevsoundSrc *devsoundsrc;
    GstStructure *structure;
    const gchar *mimetype;
    int rate;
    int channel;

    devsoundsrc = GST_DEVSOUND_SRC (bsrc);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "gst_devsound_src_setcaps ENTER ",NULL);
    structure = gst_caps_get_structure(caps, 0);
    mimetype = gst_structure_get_name(structure);

    gst_structure_get_int(structure, "rate", &rate);
    gst_structure_get_int(structure, "channels", &channel);

    devsoundsrc->rate = gst_devsound_src_get_rate(rate);
    devsoundsrc->channels = channel;

    set_channels(devsoundsrc->handle, devsoundsrc->channels);

    set_rate(devsoundsrc->handle, devsoundsrc->rate);

    if (!strncmp(mimetype, "audio/x-raw-int", 15))
        {
        devsoundsrc->fourcc = 0x36315020; //KMMFFourCCCodePCM16
        }
    else if (!strncmp(mimetype, "audio/amr", 9))
        {
        devsoundsrc->fourcc = 0x524d4120; //KMMFFourCCCodeAMR
        }
    else if (!strncmp(mimetype, "audio/x-mulaw", 13)||
             !strncmp(mimetype,"audio/x-alaw", 12))
        {
        devsoundsrc->fourcc = 0x31313747; //KMccFourCCIdG711
        }
    else if (!strncmp(mimetype, "audio/ilbc", 10))
        {
        devsoundsrc->fourcc = 0x43424c49; //KMccFourCCIdILBC
        }
    else if (!strncmp(mimetype, "audio/g729", 10))
        {
        devsoundsrc->fourcc = 0x39323747; //KMccFourCCIdG729
        }
    else
        {
        devsoundsrc->fourcc = 0x36315020; //KMMFFourCCCodePCM16
        }

    set_fourcc(devsoundsrc->handle, devsoundsrc->fourcc);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "Before Signal DevSound from STOP",NULL);
    pthread_mutex_lock(&(create_mutex1));
    pthread_cond_signal(&(create_condition1));
    pthread_mutex_unlock(&(create_mutex1));
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "After Signal DevSound from STOP ",NULL);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) devsoundsrc, "gst_devsound_src_setcaps EXIT ",NULL);
    return TRUE;
    }

static GstCaps * gst_devsound_src_getcaps(GstBaseSrc * bsrc)
    {
    GstDevsoundSrc *devsoundsrc;
    GstCaps *caps;
    GstPadTemplate *pad_template;

    devsoundsrc = GST_DEVSOUND_SRC (bsrc);

    GST_DEBUG_OBJECT(devsoundsrc, "getcaps called");

    pad_template = gst_static_pad_template_get(&devsound_src_factory);
    caps = gst_caps_copy(gst_pad_template_get_caps(pad_template));

    gst_object_unref(pad_template);

    return caps;
    }

static gboolean gst_devsound_src_start(GstBaseSrc * bsrc)
    {
    GstBuffer *tmp_gstbuffer=NULL;	
    GstDevsoundSrc *src= GST_DEVSOUND_SRC(bsrc);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "gst_devsound_src_start ENTER ",NULL);

    if(dataqueue)
        {
        while (dataqueue->length)
            {
            tmp_gstbuffer = (GstBuffer*)g_queue_pop_tail(dataqueue);
            gst_buffer_unref(tmp_gstbuffer);
            }
        g_queue_free(dataqueue);
        dataqueue = NULL;
        }
    
    if(dataqueue == NULL)
          {
          dataqueue = g_queue_new();
          }

    consumer_thread_state = CONSUMER_THREAD_INITIALIZING;
    cmd = OPEN;
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "Before Thread Create ",NULL);
    pthread_create(&ds_thread, NULL, StartDevSoundThread, (void *)src);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "After Thread Create ",NULL);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "Before Mutex Wait in START ",NULL);
    if (consumer_thread_state == CONSUMER_THREAD_INITIALIZING)
        {
        pthread_mutex_lock(&create_mutex1);
        pthread_cond_wait(&create_condition1, &create_mutex1);
        pthread_mutex_unlock(&create_mutex1);
        }
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "AFter Mutex Wait in START ",NULL);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "gst_devsound_src_start EXIT ",NULL);
    return TRUE;
    /* ERRORS */
    }

static gboolean gst_devsound_src_stop(GstBaseSrc * bsrc)
    {
	
    GstDevsoundSrc *src= GST_DEVSOUND_SRC(bsrc);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "gst_devsound_src_stop ENTER ");
    cmd = CLOSE;

    //GST_OBJECT_LOCK (src);
    pthread_mutex_lock(&(create_mutex1));
    pthread_cond_wait(&(create_condition1), &(create_mutex1));
    pthread_mutex_unlock(&(create_mutex1));
    //GST_OBJECT_UNLOCK (src);

    if (popBuffer)
        {
        gst_buffer_unref(popBuffer);
        popBuffer = NULL;
        dataleft = 0;   //Arun'S changes
        }
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "Before QUEUE Lock in STOP ");
    GST_OBJECT_LOCK(src);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "After QUEUE Lock in STOP ");
    while (dataqueue->length)
        {
        //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "Removing DATAQUEUE elements ENTER ");
        popBuffer = (GstBuffer*)g_queue_pop_tail(dataqueue);
        gst_buffer_unref(popBuffer);
        popBuffer = NULL;
        dataleft = 0; //Arun'S changes
        //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "Removing DATAQUEUE elements EXIT ");
        }
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "Before Queue FREE ");
    g_queue_free(dataqueue);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "After Queue FREE ");
    dataqueue = NULL;
    GST_OBJECT_UNLOCK(src);

    pthread_mutex_destroy(&create_mutex1);
    pthread_cond_destroy(&(create_condition1));


    g_free(src->device);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) src, "gst_devsound_src_stop EXIT ");
    return TRUE;
    }

static gboolean gst_devsound_src_unlock(GstBaseSrc * bsrc)
    {
    GstDevsoundSrc *src= GST_DEVSOUND_SRC(bsrc);

    GST_LOG_OBJECT(src, "Flushing");
    GST_OBJECT_LOCK (src);
    GST_OBJECT_UNLOCK (src);

    return TRUE;
    }

static gboolean gst_devsound_src_unlock_stop(GstBaseSrc * bsrc)
    {
    GstDevsoundSrc *src= GST_DEVSOUND_SRC(bsrc);

    GST_LOG_OBJECT(src, "No longer flushing");
    return TRUE;
    }

static GstFlowReturn gst_devsound_src_create(GstBaseSrc *src, guint64 offset,
        guint size, GstBuffer **buf)
    {
    GstDevsoundSrc *dsrc= GST_DEVSOUND_SRC(src);
    int bufferpos=0;
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "gst_devsound_src_create ENTER ");

    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "Before Buffer Alloc in CREATE ",NULL);
    *buf = gst_buffer_try_new_and_alloc(size);
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "AFter Buffer Alloc in CREATE ",NULL);

    while (size > 0)
        {
        if (dataleft >= size)
            {
            // if there is some data left in the popped buffer previously whose size
            // is more then the buffer which is incoming fresh to get filled, fill it
            //here. and if the data left in the popped buffer is 0, then unref it
            //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "dataleft >=size in CREATE ", NULL);
            memcpy(GST_BUFFER_DATA(*buf)+bufferpos,GST_BUFFER_DATA(popBuffer)+dataCopied,size);
            bufferpos+=size;
            dataCopied += size;
            dataleft = GST_BUFFER_SIZE(popBuffer) - dataCopied;
            size = 0;
            if (dataleft == 0)
                {
                dataCopied = 0;
                gst_buffer_unref(popBuffer);
                popBuffer = NULL;
                }
            }
        else
            {
            // if the dataleft in the popped buffer is greater then 0 and  less then
            // the size of data needed for the fresh buffer. copy the remaining data
            // from the popped buffer and then unref it.
            if (dataleft > 0)
                {
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "dataleft >0 in CREATE ",NULL);
                memcpy(GST_BUFFER_DATA(*buf)+bufferpos,GST_BUFFER_DATA(popBuffer)+dataCopied,dataleft);
                size -= dataleft;
                bufferpos += dataleft;
                dataCopied = 0;
                dataleft = 0;
                gst_buffer_unref(popBuffer);
                popBuffer = NULL;
                }

            // we wait here if the dataqueue length is 0 and we need data
            // to be filled in the queue from the DevSound Thread
            if (!dataqueue->length)
                {
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "Before WAIT in CREATE ",NULL);
                cmd = READDATA;
                pthread_mutex_lock(&(create_mutex1));
                pthread_cond_signal(&(create_condition1));
                pthread_mutex_unlock(&(create_mutex1));
                
                pthread_mutex_lock(&(create_mutex1));
                pthread_cond_wait(&(create_condition1), &(create_mutex1));
                pthread_mutex_unlock(&(create_mutex1));
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "AFTER WAIT in CREATE ",NULL);
                }

            //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "Before POP in CREATE ",NULL);
            GST_OBJECT_LOCK(dsrc);
            popBuffer = (GstBuffer*)g_queue_pop_tail(dataqueue);
            GST_OBJECT_UNLOCK(dsrc);
            //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "AFTER POP in CREATE ",NULL);

            // copy the data from the popped buffer based on how much of the incoming
            //buffer size is left to fill. we might have filled the fresh buffer somewhat
            // where the size of the fresh buffer is more then the data remaining in the
            // popped buffer.
            if (size < GST_BUFFER_SIZE(popBuffer))
                {
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "SIZE < POPBUFFER CREATE ",NULL);
                memcpy(GST_BUFFER_DATA(*buf)+ bufferpos,GST_BUFFER_DATA(popBuffer),size);
                bufferpos+=size;
                dataCopied = size;
                dataleft = GST_BUFFER_SIZE(popBuffer) - dataCopied;
                size = 0;
                }
            else
                {
                //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "SIZE >= POPBUFFER CREATE ",NULL);
                memcpy(GST_BUFFER_DATA(*buf)+ bufferpos,GST_BUFFER_DATA(popBuffer),GST_BUFFER_SIZE(popBuffer));
                bufferpos+=GST_BUFFER_SIZE(popBuffer);
                dataCopied = 0;
                dataleft = 0;
                size = size - GST_BUFFER_SIZE(popBuffer);
                }

            if (!dataleft)
                {
                gst_buffer_unref(popBuffer);
                popBuffer = NULL;
                }
            }
        }
    //gst_debug_log(devsound_debug, GST_LEVEL_LOG, "", "", 0, (GObject *) dsrc, "gst_devsound_src_create EXIT ",NULL);
    return GST_FLOW_OK;
    }

static gboolean gst_devsound_src_is_seekable(GstBaseSrc * bsrc)
    {
    GstDevsoundSrc *src= GST_DEVSOUND_SRC(bsrc);
    return FALSE;
    }

static gboolean gst_devsound_src_get_size(GstBaseSrc * bsrc, guint64 * size)
    {
    GstDevsoundSrc *src= GST_DEVSOUND_SRC(bsrc);
    return TRUE;
    }

static void gst_speech_encoder_config_handler_init(gpointer g_iface,
        gpointer iface_data)
    {
    GstSpeechEncoderConfigIntfc *iface =
            (GstSpeechEncoderConfigIntfc *) g_iface;

    iface->GetBitrate = gst_get_speech_encoder_bit_rate;
    iface->GetSupportedBitrates = gst_get_speech_encoder_supported_bit_rates;
    iface->GetVadMode = gst_get_speech_encoder_vad_mode;
    iface->SetBitrate = gst_set_speech_encoder_bit_rate;
    iface->SetVadMode = gst_set_speech_encoder_vad_mode;

    }

static gint gst_get_speech_encoder_bit_rate(guint* aBitrate)
    {
    *aBitrate = speechbitrate;
    return 0;
    }
static gint gst_get_speech_encoder_supported_bit_rates(
        GList** aSupportedBitrate)
    {
    *aSupportedBitrate = supportedbitrates;
    return 0;
    }
static gint gst_get_speech_encoder_vad_mode(gboolean* aVadMode)
    {
    *aVadMode = speechvadmode;
    return 0;
    }
static gint gst_set_speech_encoder_bit_rate(guint aBitrate)
    {
    speechbitrate = aBitrate;
    customInfaceUpdate.speechbitrateupdate = TRUE;
    return 0;
    }

static gint gst_set_speech_encoder_vad_mode(gboolean aVadMode)
    {
    speechvadmode = aVadMode;
    customInfaceUpdate.speechvadmodeupdate = TRUE;
    return 0;
    }


static void gst_Apply_SpeechEncoder_Update(GstDevsoundSrc *devsoundsrc )
    {
    if(customInfaceUpdate.speechbitrateupdate == TRUE)
        {
        set_speech_encoder_bit_rate(devsoundsrc->handle,speechbitrate);
        customInfaceUpdate.speechbitrateupdate = FALSE;
        }
    if(customInfaceUpdate.speechvadmodeupdate == TRUE)
        {
        set_speech_encoder_vad_mode(devsoundsrc->handle,speechvadmode);
        customInfaceUpdate.speechvadmodeupdate = FALSE;
        }
    }

static void gst_g711_encoder_handler_init(gpointer g_iface,
        gpointer iface_data)
    {
    GstG711EncoderIntfc *iface = (GstG711EncoderIntfc *) g_iface;

    iface->GetVadMode = gst_get_g711_encoder_vad_mode;
    iface->SetEncoderMode = gst_set_g711_encoder_mode;
    iface->SetVadMode = gst_set_g711_vad_mode;

    }

static gint gst_get_g711_encoder_vad_mode(gboolean* aVadMode)
    {
    *aVadMode = g711vadmode;
    return 0;
    }

static gint gst_set_g711_encoder_mode(enum TG711EncodeMode aEncodeMode)
    {
    g711encodemode = aEncodeMode;
    customInfaceUpdate.g711encodermodeupdate = TRUE;
    return 0;
    }
static gint gst_set_g711_vad_mode(gboolean aVadMode)
    {
    g711vadmode = aVadMode;
    customInfaceUpdate.g711vadmodeupdate = TRUE;
    return 0;
    }


static void gst_Apply_G711Encoder_Update(GstDevsoundSrc *devsoundsrc )
    {

    if(customInfaceUpdate.g711encodermodeupdate == TRUE)
        {
        set_g711_encoder_mode(devsoundsrc->handle,g711encodemode);
        customInfaceUpdate.g711encodermodeupdate = FALSE;
        }
    if(customInfaceUpdate.g711vadmodeupdate == TRUE)
        {
        set_g711_vad_mode(devsoundsrc->handle,g711vadmode);
        customInfaceUpdate.g711vadmodeupdate = FALSE;
        }
    }

static void gst_g729_encoder_handler_init(gpointer g_iface,
        gpointer iface_data)
    {

    GstG729EncoderIntfc *iface = (GstG729EncoderIntfc *) g_iface;

    iface->GetVadMode = gst_get_g729_vad_mode;
    iface->SetVadMode = gst_set_g729_vad_mode;

    }

static gint gst_set_g729_vad_mode(TBool aVadMode)
    {
    g729vadmode = aVadMode;
    customInfaceUpdate.g729vadmodeupdate = TRUE;
    return 0;
    }

static gint gst_get_g729_vad_mode(TBool* aVadMode)
    {
    *aVadMode = g729vadmode;
    return 0;
    }
static void gst_Apply_G729Encoder_Update(GstDevsoundSrc *devsoundsrc )
    {
    if(customInfaceUpdate.g729vadmodeupdate)
        {
        set_g729_vad_mode(devsoundsrc->handle,g729vadmode);
        customInfaceUpdate.g729vadmodeupdate = FALSE;
        }

    }

static void gst_ilbc_encoder_handler_init(gpointer g_iface,
        gpointer iface_data)
    {
    GstIlbcEncoderIntfc *iface = (GstIlbcEncoderIntfc *) g_iface;

    iface->GetVadMode = gst_get_ilbc_vad_mode;
    iface->SetEncoderMode = gst_set_ilbc_encoder_mode;
    iface->SetVadMode = gst_set_ilbc_vad_mode;
    }

static gint gst_set_ilbc_encoder_mode(enum TIlbcEncodeMode aEncodeMode)
    {
    ilbcencodemode = aEncodeMode;
    return 0;
    }

static gint gst_set_ilbc_vad_mode(gboolean aVadMode)
    {
    ilbcvadmode = aVadMode;
    return 0;
    }

static gint gst_get_ilbc_vad_mode(gboolean* aVadMode)
    {
    *aVadMode = ilbcvadmode;
    return 0;
    }

static void gst_Apply_IlbcEncoder_Update(GstDevsoundSrc *devsoundsrc )
    {
    if(customInfaceUpdate.ilbcencodermodeupdate)
        {
        set_ilbc_encoder_mode(devsoundsrc->handle,ilbcencodemode);
        customInfaceUpdate.ilbcencodermodeupdate = FALSE;
        }
    if(customInfaceUpdate.ilbcvadmodeupdate)
         {
         set_ilbc_vad_mode(devsoundsrc->handle,ilbcvadmode);
         customInfaceUpdate.ilbcvadmodeupdate = FALSE;
         }

    }

static gint gst_devsound_src_get_rate(gint rate)
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

