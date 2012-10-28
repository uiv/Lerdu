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

#include "devsoundsinkwrapper.h"
#include <mmcccodecinformation.h>
#include "debugtracemacros.h"

DevSoundWrapper::DevSoundWrapper()
    {
    init_complete = 0;
    dev_sound = NULL;

    buffersize = 0;
    dev_count = 0;
    caps.iRate = EMMFSampleRate8000Hz;
    caps.iEncoding = EMMFSoundEncoding16BitPCM;
    caps.iChannels = EMMFMono;
    fourcc = KMMFFourCCCodePCM16;
    eosReceived = false;
    iErrConcealmentIntfc = NULL;
    iG711DecoderIntfc = NULL;
    iG729DecoderIntfc = NULL;
    iIlbcDecoderIntfc = NULL;
    iCallbackError = KErrNone;
    iAudioOutput = NULL;
    }

/*********************************************************/
void DevSoundWrapper::InitializeComplete(TInt aError)
    {
    TRACE_PRN_FN_ENT;
    TRequestStatus* stat = &(AL->iStatus);

    if (aError == KErrNone)
        {
        init_complete = 1;
        }
    else
        {
        init_complete = 0;
        }

    User::RequestComplete(stat, aError);
    TRACE_PRN_IF_ERR(aError);
    TRACE_PRN_FN_EXT;
    }
/************************************************************/

void DevSoundWrapper::BufferToBeFilled(CMMFBuffer* aBuffer)
    {
//    TRACE_PRN_FN_ENT;
    buffer = aBuffer;
    buffersize = aBuffer->RequestSize();
    TRACE_PRN_N1(_L("DevSoundWrapper::BufferToBeFilled->buffersize [%d]"), buffersize);

    TRequestStatus* stat = &(AL->iStatus);

    if (aBuffer)
        {
        User::RequestComplete(stat, KErrNone);
        iCallbackError = KErrNone;
        }
    else
        {
        User::RequestComplete(stat, KErrNotFound);
        iCallbackError = KErrNotFound;
        }

//    TRACE_PRN_FN_EXT;
    }

/***********************************************************/
void DevSoundWrapper::PlayError(TInt aError)
    {
    TRequestStatus* stat = &(AL->iStatus);
    User::RequestComplete(stat, aError);
    iCallbackError = aError;
    }
/*******************************************************/
void DevSoundWrapper::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
    {

    }
/********************************************************/
void DevSoundWrapper::RecordError(TInt /*aError*/)
    {

    }
/**********************************************************/
void DevSoundWrapper::ConvertError(TInt /*aError*/)
    {

    }
/***********************************************************/
void DevSoundWrapper::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
    {

    }
/***********************************************************/
void DevSoundWrapper::SendEventToClient(const TMMFEvent& /*aEvent*/)
    {

    }
/************************************************************/
void DevSoundWrapper::ToneFinished(TInt /*aError*/)
    {

    }

/***********************************************************
 *********C interface functions******************************
 ************************************************************/

/******************************************************/
int open_devsound(DevSoundWrapper **handle)
    {
    TRACE_PRN_FN_ENT;
    int ret = 0;
    *handle = new DevSoundWrapper();
    if (handle)
        {
        ret = open_device(handle);
        }
    else
        {
        ret = KErrNoMemory;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

/*********************************************************/

int check_if_device_open(DevSoundWrapper *handle)
    {
    TRACE_PRN_FN_ENT;

    int ret;
    if ((handle->dev_count) == 1)
        {
        ret = 1;
        }
    else
        {
        ret = 0;
        }

    TRACE_PRN_FN_EXT;
    return ret;
    }

/*********************************************************/
int open_device(DevSoundWrapper **handle)
    {
    int retcode = KErrNone;
    TRACE_PRN_FN_ENT;

    (*handle)->dev_count++;

    (*handle)->as = CActiveScheduler::Current();

    if (!((*handle)->as))
        {
        (*handle)->as = new CActiveScheduler();
        if ((*handle)->as)
            {
            CActiveScheduler::Install((*handle)->as);
            }

        }
    (*handle)->AL = new CActiveListener;
    ((*handle)->AL)->asw = new CActiveSchedulerWait();

        TRAP( retcode,(*handle)->dev_sound = CMMFDevSound::NewL() );

    if (!(*handle)->AL || !((*handle)->AL)->asw || !(*handle)->dev_sound
            || !(*handle)->as)
        {
        retcode = KErrNoMemory;
        }

    TRACE_PRN_FN_EXT;
    return retcode;
    }

/*******************************************************/
int initialize_devsound(GstDevsoundSink* ds)
    {
    TRACE_PRN_FN_ENT;
    int ret = 0;
    DevSoundWrapper* handle = (DevSoundWrapper*) ds->handle;

    handle->AL->InitialiseActiveListener();

        TRAP (ret, handle->dev_sound->InitializeL(*handle, handle->fourcc, EMMFStatePlaying));

    if (ret)
        {
        TRACE_PRN_IF_ERR(ret);
        return ret;
        }

    handle->AL->StartActiveScheduler();

    if (handle->init_complete==1)
        {
        TMMFPrioritySettings temp;
        temp.iPref = (TMdaPriorityPreference)ds->preference;
        temp.iPriority = ds->priority;
        handle->dev_sound->SetPrioritySettings(temp);

        //set post initialize properties
        post_init_setconfig(handle);
        ret = KErrNone;
        }
    else
        {
        ret = KErrNotFound;
        }

    TRACE_PRN_IF_ERR(ret);
    TRACE_PRN_FN_EXT;
    return ret;
    }

/************************************************************/

int close_devsound(GstDevsoundSink *ds)
    {
    TRACE_PRN_FN_ENT;

    g_list_foreach(ds->fmt, (GFunc) g_free, NULL);
    g_list_free(ds->fmt);
    ds->fmt = NULL;
    delete ds->handle;
    TRACE_PRN_FN_EXT;
    return 0;
    }

/************************************************************/

int post_init_setconfig(DevSoundWrapper *handle)
    {
    TRACE_PRN_FN_ENT;
    int ret = 0;

        TRAP(ret, (handle->dev_sound)->SetConfigL(handle->caps) );

    if (ret)
        {
        TRACE_PRN_IF_ERR(ret);
        return ret;
        }

    (handle->caps)= (handle->dev_sound)->Config();

    if (handle->fourcc == KMccFourCCIdG711 ||
            handle->fourcc == KMccFourCCIdG729 ||
            handle->fourcc == KMccFourCCIdILBC ||
            handle->fourcc == KMccFourCCIdAMRNB)
        {
        if (!handle->iErrConcealmentIntfc)
            {

            TRAP(ret, handle->iErrConcealmentIntfc = CErrorConcealmentIntfc::NewL(*handle->dev_sound) );
            }
        }
    switch (handle->fourcc)
        {
        case KMccFourCCIdG711:
            {
            if (!handle->iG711DecoderIntfc)
                {

                TRAP(ret, handle->iG711DecoderIntfc = CG711DecoderIntfc::NewL(*handle->dev_sound) );
                }
            break;
            }
        case KMccFourCCIdG729:
            {
            if (!handle->iG729DecoderIntfc)
                {

                TRAP(ret, handle->iG729DecoderIntfc = CG729DecoderIntfc::NewL(*handle->dev_sound) );
                }
            break;
            }
        case KMccFourCCIdILBC:
            {
            if (!handle->iIlbcDecoderIntfc)
                {

                TRAP(ret, handle->iIlbcDecoderIntfc = CIlbcDecoderIntfc::NewL(*handle->dev_sound) );
                }
            break;
            }
        case KMccFourCCIdAMRNB:
        case KMMFFourCCCodePCM16:
        default:
            {
            break;
            }
        }
   
    if (!handle->iAudioOutput)
        {
        TRAP(ret, handle->iAudioOutput = CAudioOutput::NewL(*handle->dev_sound) );
        }
    
    TRACE_PRN_IF_ERR(ret);
    TRACE_PRN_FN_EXT;
    return ret;

    }

/*********************************************************/

void conceal_error_for_next_buffer(DevSoundWrapper *handle)
    {
    TRACE_PRN_FN_ENT;
    if (handle->iErrConcealmentIntfc)
        {
        (handle->iErrConcealmentIntfc)->ConcealErrorForNextBuffer();
        }
    TRACE_PRN_FN_EXT;
    }

/*********************************************************/
void set_framemode(DevSoundWrapper *handle, gboolean mode)
    {
    TRACE_PRN_FN_ENT;
    if (handle->iErrConcealmentIntfc)
        {
        (handle->iErrConcealmentIntfc)->SetFrameMode(mode);
        }
    TRACE_PRN_FN_EXT;
    }

/*********************************************************/
int framemode_rqrd_for_ec(DevSoundWrapper *handle, gboolean* modereq)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iErrConcealmentIntfc)
        {
        err = (handle->iErrConcealmentIntfc)->FrameModeRqrdForEC(*modereq);
        }
    TRACE_PRN_FN_EXT;
    return err;
    }

/*********************************************************/
int set_decodermode(DevSoundWrapper *handle, int aDecodeMode)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iG711DecoderIntfc)
        {
        err = (handle->iG711DecoderIntfc)->SetDecoderMode(
                (CG711DecoderIntfc::TDecodeMode) aDecodeMode);
        }
    TRACE_PRN_FN_EXT;
    return err;
    }

/*********************************************************/

int set_cng(DevSoundWrapper *handle, gboolean aCng)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iG711DecoderIntfc)
        {
        err = (handle->iG711DecoderIntfc)->SetCng(aCng);
        }
    TRACE_PRN_FN_EXT;
    return err;
    }

/*********************************************************/
int get_cng(DevSoundWrapper *handle, gboolean* aCng)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iG711DecoderIntfc)
        {
        err = (handle->iG711DecoderIntfc)->GetCng(*aCng);
        }
    TRACE_PRN_FN_EXT;
    return err;
    }

/*********************************************************/
int set_plc(DevSoundWrapper *handle, gboolean aPlc)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iG711DecoderIntfc)
        {
        err = (handle->iG711DecoderIntfc)->SetPlc(aPlc);
        }
    TRACE_PRN_FN_EXT;
    return err;
    }
/*********************************************************/
//G729 custom interface
gint badlsfnextbuffer(DevSoundWrapper *handle)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iG729DecoderIntfc)
        {
        err = (handle->iG729DecoderIntfc)->BadLsfNextBuffer();
        }
    TRACE_PRN_FN_EXT;
    return err;
    }
/*********************************************************/
//Ilbc custom interace
gint get_ilbccng(DevSoundWrapper *handle, gboolean* aCng)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iIlbcDecoderIntfc)
        {
        err = (handle->iIlbcDecoderIntfc)->GetCng(*aCng);
        }

    TRACE_PRN_FN_EXT;
    return err;
    }
/*********************************************************/
gint set_ilbccng(DevSoundWrapper *handle, gboolean aCng)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iIlbcDecoderIntfc)
        {
        err = (handle->iIlbcDecoderIntfc)->SetCng(aCng);
        }
    TRACE_PRN_FN_EXT;
    return err;
    }
/*********************************************************/
gint set_ilbcdecodermode(DevSoundWrapper *handle,
        enum TIlbcDecodeMode aDecodeMode)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iIlbcDecoderIntfc)
        {
        err = (handle->iIlbcDecoderIntfc)->SetDecoderMode(
                (CIlbcDecoderIntfc::TDecodeMode) aDecodeMode);
        }
    TRACE_PRN_FN_EXT;
    return err;
    }

/*********************************************************/
gint set_outputdevice(DevSoundWrapper *handle, gint outputDev)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iAudioOutput)
        {
        (handle->iAudioOutput)->SetAudioOutputL(
                (CAudioOutput::TAudioOutputPreference) outputDev);
        }
    TRACE_PRN_FN_EXT;
    return err;
    }

/*********************************************************/
gint get_outputdevice(DevSoundWrapper *handle, gint* outputDev)
    {
    TRACE_PRN_FN_ENT;
    int err = 0;
    if (handle->iAudioOutput)
        {
        *outputDev = (handle->iAudioOutput)->AudioOutput();
        }
    TRACE_PRN_FN_EXT;
    return err;
    }

/**************************************************************/
int reset_devsound(DevSoundWrapper* /*handle*/)
    {
    return 0;
    }
/*****************************************************************/
int get_rate(DevSoundWrapper *handle)
    {
    return handle->caps.iRate;

    }
/*****************************************************************/
int get_channels(DevSoundWrapper *handle)
    {
    return handle->caps.iChannels;
    }
/********************************************************************/
int get_encoding(DevSoundWrapper *handle)
    {
    return handle->caps.iEncoding;

    }
/*******************************************************************/
int get_size(DevSoundWrapper *handle)
    {
    return handle->caps.iBufferSize;
    }
/******************************************************************/

void set_rate(DevSoundWrapper *handle, int rate)
    {
    handle->caps.iRate = rate;
    }
/******************************************************************/
void set_channels(DevSoundWrapper *handle, int channels)
    {
    handle->caps.iChannels = channels;
    }
/****************************************************************/
void set_encoding(DevSoundWrapper *handle, int encoding)
    {
    handle->caps.iEncoding = encoding;
    }
/*****************************************************************/
void set_size(DevSoundWrapper *handle, int size)
    {
    handle->caps.iBufferSize = size;
    }
/*****************************************************************/
void set_fourcc(DevSoundWrapper *handle, int fourcc)
    {
    handle->fourcc = fourcc;
    }

int get_ds_cb_error(DevSoundWrapper *handle)
    {
    return handle->iCallbackError;
    }
/*******************************************************************/

int playinit(DevSoundWrapper *handle)
    {
    TRACE_PRN_FN_ENT;

    ((handle)->AL)->InitialiseActiveListener();
    handle->eosReceived = false;

        TRAP(handle->iCallbackError,(handle->dev_sound)->PlayInitL());
    if (handle->iCallbackError == KErrNone)
        {
        ((handle)->AL)->StartActiveScheduler();
        }

    TRACE_PRN_IF_ERR(handle->iCallbackError);
    TRACE_PRN_FN_EXT;
    return handle->iCallbackError;
    }
/******************************************************************/
int write_data(DevSoundWrapper *handle, TUint8* gstPtr, int gstBufferSize,
        gboolean lastBuffer)
    {
    TRACE_PRN_FN_ENT;

    CMMFDataBuffer* buf = STATIC_CAST (CMMFDataBuffer*, handle->buffer);

    TRACE_PRN_N2(_L("***gstBufferSize:[%d], handle->buffersize:[%d]"), gstBufferSize, handle->buffersize);

    if ((lastBuffer == TRUE) && (gstBufferSize <= handle->buffersize))
        {
        buf->SetLastBuffer(ETrue);
        }
    
    TInt copySize = Min(gstBufferSize, handle->buffersize);

    if (copySize <= 0)
        {
        //will crash thread if not handled here
        return gstBufferSize;
        }

    TPtr8 srcPtr(gstPtr, copySize, copySize);
    TUint8* p = (TUint8*) (buf->Data().Ptr());
    TPtr8 destPtr(p, buf->Data().Length(), buf->Data().MaxLength());

    destPtr.Append(srcPtr);
    TRACE_PRN_N(_L("write_data->Append"));
    buf->Data().SetLength(buf->Data().Length() + copySize);

    gstBufferSize -= copySize;
    handle->buffersize -= copySize;

    if ((handle->buffersize == 0) || (buf->LastBuffer()))
        {
        play_data(handle);
        }

    TRACE_PRN_FN_EXT;
    return gstBufferSize;
    }

/*******************************************************************/
int play_data(DevSoundWrapper *handle)
    {
    TRACE_PRN_FN_ENT;
    (handle->AL)->InitialiseActiveListener();

    (handle->dev_sound)->PlayData();

    ((handle)->AL)->StartActiveScheduler();

    TRACE_PRN_FN_EXT;
    return KErrNone;
    }

/*********************************************************************/

int pre_init_setconf(GstDevsoundSink *ds)
    {
    TRACE_PRN_FN_ENT;
    int ret = 0;

    DevSoundWrapper* dsPtr = STATIC_CAST(DevSoundWrapper*, ds->handle);

    // NOTE: it is too late for setting prio/pref here
    if (ds->pending.preferenceupdate == 1 || ds->pending.priorityupdate == 1)
        {
        ds->pending.preferenceupdate = FALSE;
        ds->pending.priorityupdate = FALSE;
        }
    if (ds->pending.volumeupdate == 1)
        {
        (dsPtr->dev_sound)->SetVolume(ds->volume);
        ds->pending.volumeupdate = FALSE;
        }
    if (ds->pending.volumerampupdate == 1)
        {
        (dsPtr->dev_sound)->SetVolumeRamp(ds->volumeramp);
        ds->pending.volumerampupdate = FALSE;
        }
    if (ds->pending.leftbalanceupdate == 1 || ds->pending.rightbalanceupdate
            == 1)
        {
            TRAP( ret, (dsPtr->dev_sound)->SetPlayBalanceL(ds->leftbalance,ds->rightbalance) );
        ds->pending.leftbalanceupdate = FALSE;
        ds->pending.rightbalanceupdate = FALSE;
        }
    if(ds->pending.outputupdate == 1)
        {
        set_outputdevice(dsPtr,ds->output);
        ds->pending.outputupdate = FALSE;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }
/*********************************************************/
void getsupporteddatatypes(GstDevsoundSink *ds)
    {
    TRACE_PRN_FN_ENT;
    DevSoundWrapper* dsPtr = STATIC_CAST(DevSoundWrapper*, ds->handle);
        TRAP_IGNORE(dsPtr->GetDataTypesL(ds));

    TRACE_PRN_FN_EXT;
    }

/*********************************************************/
void DevSoundWrapper::GetDataTypesL(GstDevsoundSink *ds)
    {
    TRACE_PRN_FN_ENT;

    RArray<TFourCC> outputdatatypes;
    TMMFPrioritySettings prioritysettings;
    DevSoundWrapper* dsPtr = STATIC_CAST(DevSoundWrapper*, ds->handle);

    CleanupClosePushL(outputdatatypes);

    TRAP_IGNORE( (dsPtr->dev_sound)->GetSupportedOutputDataTypesL(outputdatatypes,prioritysettings) );

    for (TInt i = 0; i < outputdatatypes.Count(); i++)
        {
        TRACE_PRN_N2(_L("GstDevsoundSink supported Codec[%d]==[0x%x]"), i+1, outputdatatypes[i].FourCC());
        guint *ip = g_new (guint, 1);
        *ip = outputdatatypes[i].FourCC();
        ds->fmt = g_list_append(ds->fmt, ip);
        }
    CleanupStack::PopAndDestroy(&outputdatatypes);
    TRACE_PRN_FN_EXT;
    }

/*********************************************************/
void populateproperties(GstDevsoundSink *ds)
    {
    TRACE_PRN_FN_ENT;
    DevSoundWrapper* dsPtr = STATIC_CAST(DevSoundWrapper*, ds->handle);
    ds->samplesplayed = (dsPtr->dev_sound)->SamplesPlayed();
    get_outputdevice(dsPtr,&ds->output);
    TRACE_PRN_FN_EXT;
    }

void initproperties(GstDevsoundSink* ds)
    {
    TRACE_PRN_FN_ENT;
    DevSoundWrapper* dsPtr=  STATIC_CAST(DevSoundWrapper*, ds->handle);
    ds->maxvolume     = (dsPtr->dev_sound)->MaxVolume();
    ds->volume        = (dsPtr->dev_sound)->Volume();
    framemode_rqrd_for_ec(dsPtr,&ds->framemodereq);   
    get_cng(dsPtr,&ds->g711cng);
    get_ilbccng(dsPtr,&ds->ilbccng);
    TRACE_PRN_FN_EXT;    
    }
