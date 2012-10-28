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

#include "devsoundsrcwrapper.h"
#include <e32base.h>
#include <mmcccodecinformation.h>
#include "debugtracemacros.h"

DevSoundWrapperSrc::DevSoundWrapperSrc()
    {
    init_complete = 0;
    dev_sound = NULL;
    buffersize = 0;
    dev_count = 0;
    caps.iRate = EMMFSampleRate8000Hz;
    caps.iEncoding = EMMFSoundEncoding16BitPCM;
    caps.iChannels = EMMFMono;
    fourcc = KMMFFourCCCodePCM16;
    iCallbackError = KErrNone;
    iSpeechEncoderConfig = NULL;
    iG711EncoderIntfc = NULL;
    iG729EncoderIntfc = NULL;
    iIlbcEncoderIntfc = NULL;
    }

/*********************************************************/
void DevSoundWrapperSrc::InitializeComplete(TInt aError)
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
    TRACE_PRN_FN_EXT;
    }
/************************************************************/

void DevSoundWrapperSrc::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
    {
    }

/***********************************************************/
void DevSoundWrapperSrc::PlayError(TInt /*aError*/)
    {
    }
/*******************************************************/

void DevSoundWrapperSrc::BufferToBeEmptied(CMMFBuffer* aBuffer)
    {
//    TRACE_PRN_FN_ENT;

    buffer = aBuffer;
    bufferreadpos = 0;
    CMMFDataBuffer* buf = STATIC_CAST (CMMFDataBuffer*, buffer);
    buffersize = buf->Data().Length();
    TRACE_PRN_N1(_L("DevSoundWrapperSrc::BufferToBeEmptied->buffersize [%d]"), buffersize);

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
/********************************************************/

void DevSoundWrapperSrc::RecordError(TInt aError)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_N1(_L("DevSoundWrapperSrc::RecordError %d"),aError);
    iCallbackError = aError;
    TRACE_PRN_FN_EXT;
    }
/**********************************************************/

void DevSoundWrapperSrc::ConvertError(TInt /*aError*/)
    {

    }
/***********************************************************/
void DevSoundWrapperSrc::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
    {

    }
/***********************************************************/
void DevSoundWrapperSrc::SendEventToClient(const TMMFEvent& /*aEvent*/)
    {

    }
/************************************************************/
void DevSoundWrapperSrc::ToneFinished(TInt /*aError*/)
    {

    }

/***********************************************************
 *********C interface functions******************************
 ************************************************************/

/******************************************************/
int get_databuffer(DevSoundWrapperSrc *handle, const TUint8** buffer)
    {
    int ret = 0;
    CMMFDataBuffer* buf = STATIC_CAST (CMMFDataBuffer*, handle->buffer);
    *buffer = buf->Data().Ptr();
    if (buffer)
        {
        ret = KErrNone;
        }
    else
        {
        ret = KErrNotFound;
        }
    return ret;
    }

/*********************************************************/
int get_databuffer_size(DevSoundWrapperSrc *handle)
    {
    CMMFDataBuffer* buf = STATIC_CAST (CMMFDataBuffer*, handle->buffer);
    return buf->Data().Length();
    }

/*********************************************************/
int open_devsound(DevSoundWrapperSrc **handle)
    {
    TRACE_PRN_FN_ENT;
    int ret = 0;
    *handle = new DevSoundWrapperSrc();
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

/*******************************************************/
int initialize_devsound(GstDevsoundSrc* ds)
    {
    TRACE_PRN_FN_ENT;
    int ret = 0;
    DevSoundWrapperSrc* handle = (DevSoundWrapperSrc*) ds->handle;

    handle->AL->InitialiseActiveListener();

        TRAP(ret, handle->dev_sound->InitializeL(*handle, handle->fourcc, EMMFStateRecording));

    if (ret)
        {
        TRACE_PRN_FN_EXT;
        return ret;
        }

    handle->AL->StartActiveScheduler();

    if (handle->init_complete == 1)
        {
        TMMFPrioritySettings temp;
        temp.iPref = (TMdaPriorityPreference) ds->preference;
        temp.iPriority = ds->priority;
        handle->dev_sound->SetPrioritySettings(temp);

        SetConfigurations(handle);
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
/*********************************************************/

int open_device(DevSoundWrapperSrc **handle)
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

/*********************************************************/

int close_devsound(GstDevsoundSrc *ds)
    {
    TRACE_PRN_FN_ENT;
    (STATIC_CAST(DevSoundWrapperSrc*, ds->handle))->dev_sound->Stop();
    g_list_foreach(ds->supportedbitrates, (GFunc) g_free, NULL);
    g_list_free(ds->supportedbitrates);

    g_list_foreach(ds->fmt, (GFunc) g_free, NULL);
    g_list_free(ds->fmt);

    delete ds->handle;
    TRACE_PRN_FN_EXT;
    return 0;
    }
/************************************************************/

int SetConfigurations(DevSoundWrapperSrc *handle)
    {
    TRACE_PRN_FN_ENT;
    int ret = 0;

    TMMFCapabilities temp_caps;
    temp_caps = (handle->dev_sound)->Capabilities();

    handle->gain = (handle->dev_sound)->MaxGain();
    (handle->dev_sound)->SetGain(handle->gain);
    handle->caps.iBufferSize = temp_caps.iBufferSize;

        TRAP(ret, (handle->dev_sound)->SetConfigL(handle->caps) );
    if (ret)
        {
        return ret;
        }

    (handle->caps) = (handle->dev_sound)->Config();

    switch (handle->fourcc)
        {
        case KMccFourCCIdG711:
            {
            if (!handle->iG711EncoderIntfc)
                {
                    TRAP( ret, handle->iG711EncoderIntfc
                            = CG711EncoderIntfc::NewL(*handle->dev_sound) );
                }
            break;
            }
        case KMccFourCCIdG729:
            {
            if (!handle->iG729EncoderIntfc)
                {
                    TRAP( ret, handle->iG729EncoderIntfc
                            = CG729EncoderIntfc::NewL(*handle->dev_sound));
                }

            break;
            }
        case KMccFourCCIdILBC:
            {

            if (!handle->iIlbcEncoderIntfc)
                {
                    TRAP( ret, handle->iIlbcEncoderIntfc
                            = CIlbcEncoderIntfc::NewL(*handle->dev_sound));
                }

            break;
            }
        default:
            {
            break;
            }
        }

    if (!handle->iSpeechEncoderConfig && handle->fourcc
            != KMMFFourCCCodePCM16)
        {

            TRAP(ret, handle->iSpeechEncoderConfig
                    = CSpeechEncoderConfig::NewL(*handle->dev_sound));
        }

    TRACE_PRN_FN_EXT;
    return ret;
    }

void DevSoundWrapperSrc::GetSpeechEncoderProperties(GstDevsoundSrc* ds)
    {
    DevSoundWrapperSrc* dsPtr = STATIC_CAST(DevSoundWrapperSrc*, ds->handle);
    get_speech_encoder_supported_bit_rates(ds);
    get_speech_encoder_bit_rate(dsPtr, &ds->speechbitrate);
    get_speech_encoder_vad_mode(dsPtr, &ds->speechvadmode);

    }

/*********************************************************/
int get_speech_encoder_bit_rate(DevSoundWrapperSrc *handle, guint* aBitrate)
    {
    int err = 0;
    if (handle->iSpeechEncoderConfig)
        {
        err = (handle->iSpeechEncoderConfig)->GetBitrate(*aBitrate);
        }
    return err;
    }
/*********************************************************/
int get_speech_encoder_supported_bit_rates(GstDevsoundSrc *ds)
    {
    int err = 0;
    DevSoundWrapperSrc* dsPtr = STATIC_CAST(DevSoundWrapperSrc*, ds->handle);
        TRAP(err, err = dsPtr->GetSupportedBitratesL(ds));

    return err;
    }

int DevSoundWrapperSrc::GetSupportedBitratesL(GstDevsoundSrc* ds)
    {
    int err = 0;
    DevSoundWrapperSrc* dsPtr = STATIC_CAST(DevSoundWrapperSrc*, ds->handle);
    if (dsPtr->iSpeechEncoderConfig)
        {
        RArray<TUint> bitrates;

        CleanupClosePushL(bitrates);
        err = (dsPtr->iSpeechEncoderConfig)->GetSupportedBitrates(bitrates);
        for (TInt i = 0; i < bitrates.Count(); i++)
            {
            guint *ip = g_new (guint, 1);
            *ip = bitrates[i];
            ds->supportedbitrates = g_list_append(ds->supportedbitrates, ip);
            }
        CleanupStack::PopAndDestroy(&bitrates);
        }
    return err;
    }

/*********************************************************/
int get_speech_encoder_vad_mode(DevSoundWrapperSrc *handle,
        gboolean* aVadMode)
    {
    int err = 0;
    if (handle->iSpeechEncoderConfig)
        {
        err = (handle->iSpeechEncoderConfig)->GetVadMode(*aVadMode);
        }
    return err;
    }

/*********************************************************/
int set_speech_encoder_bit_rate(DevSoundWrapperSrc *handle, guint aBitrate)
    {
    int err = 0;
    if (handle->iSpeechEncoderConfig)
        {
        err = (handle->iSpeechEncoderConfig)->SetBitrate(aBitrate);
        }
    return err;
    }

/*********************************************************/
int set_speech_encoder_vad_mode(DevSoundWrapperSrc *handle, gboolean aVadMode)
    {
    int err = 0;
    if (handle->iSpeechEncoderConfig)
        {
        err = (handle->iSpeechEncoderConfig)->SetVadMode(aVadMode);
        }
    return err;
    }

/*********************************************************/
int get_g711_encoder_vad_mode(DevSoundWrapperSrc *handle, gboolean* aVadMode)
    {
    int err = 0;
    if (handle->iG711EncoderIntfc)
        {
        err = (handle->iG711EncoderIntfc)->GetVadMode(*aVadMode);
        }
    return err;
    }

/*********************************************************/
int set_g711_encoder_mode(DevSoundWrapperSrc *handle,
        enum TG711EncodeMode aEncodeMode)
    {
    int err = 0;
    if (handle->iG711EncoderIntfc)
        {
        err = (handle->iG711EncoderIntfc)->SetEncoderMode(
                (CG711EncoderIntfc::TEncodeMode) aEncodeMode);
        }
    return err;
    }

/*********************************************************/
int set_g711_vad_mode(DevSoundWrapperSrc *handle, gboolean aVadMode)
    {
    int err = 0;
    if (handle->iG711EncoderIntfc)
        {
        err = (handle->iG711EncoderIntfc)->SetVadMode(aVadMode);
        }
    return err;
    }

/**************************************************************/
int set_g729_vad_mode(DevSoundWrapperSrc *handle, TBool aVadMode)
    {
    int err = 0;
    if (handle->iG729EncoderIntfc)
        {
        err = (handle->iG729EncoderIntfc)->SetVadMode(aVadMode);
        }
    return err;
    }
/**************************************************************/
int get_g729_vad_mode(DevSoundWrapperSrc *handle, TBool* aVadMode)
    {
    int err = 0;
    if (handle->iG729EncoderIntfc)
        {
        err = (handle->iG729EncoderIntfc)->GetVadMode(*aVadMode);
        }
    return err;
    }
/**************************************************************/
int set_ilbc_encoder_mode(DevSoundWrapperSrc *handle,
        enum TIlbcEncodeMode aEncodeMode)
    {
    int err = 0;
    if (handle->iIlbcEncoderIntfc)
        {
        err = (handle->iIlbcEncoderIntfc)->SetEncoderMode(
                (CIlbcEncoderIntfc::TEncodeMode) aEncodeMode);
        }
    return err;
    }
/**************************************************************/
int set_ilbc_vad_mode(DevSoundWrapperSrc *handle, gboolean aVadMode)
    {
    int err = 0;
    if (handle->iIlbcEncoderIntfc)
        {
        err = (handle->iIlbcEncoderIntfc)->SetVadMode(aVadMode);
        }
    return err;
    }
/**************************************************************/
int get_ilbc_vad_mode(DevSoundWrapperSrc *handle, gboolean* aVadMode)
    {
    int err = 0;
    if (handle->iIlbcEncoderIntfc)
        {
        err = (handle->iIlbcEncoderIntfc)->GetVadMode(*aVadMode);
        }
    return err;
    }

/**************************************************************/
int reset_devsound(DevSoundWrapperSrc */*handle*/)
    {
    return 0;
    }
/*****************************************************************/
int get_rate(DevSoundWrapperSrc *handle)
    {
    return handle->caps.iRate;

    }
/*****************************************************************/
int get_channels(DevSoundWrapperSrc *handle)
    {
    return handle->caps.iChannels;

    }
/********************************************************************/
int get_encoding(DevSoundWrapperSrc *handle)
    {
    return handle->caps.iEncoding;
    }
/*******************************************************************/
int get_size(DevSoundWrapperSrc *handle)
    {
    return handle->caps.iBufferSize;
    }
/******************************************************************/

void set_rate(DevSoundWrapperSrc *handle, int rate)
    {
    handle->caps.iRate = rate;
    }
/******************************************************************/
void set_channels(DevSoundWrapperSrc *handle, int channels)
    {
    handle->caps.iChannels = channels;
    }
/****************************************************************/
void set_encoding(DevSoundWrapperSrc *handle, int encoding)
    {
    handle->caps.iEncoding = encoding;
    }
/*****************************************************************/
void set_size(DevSoundWrapperSrc *handle, int size)
    {
    handle->caps.iBufferSize = size;
    }
/*****************************************************************/
void set_fourcc(DevSoundWrapperSrc *handle, int fourcc)
    {
    handle->fourcc = fourcc;
    }

/*******************************************************************/

int recordinit(DevSoundWrapperSrc *handle)
    {
    TRACE_PRN_FN_ENT;
    int ret = 0;
    ((handle)->AL)->InitialiseActiveListener();
    handle->iCallbackError = KErrNone;

        TRAP(ret, (handle->dev_sound)->RecordInitL() );

    if (ret)
        {
        TRACE_PRN_FN_EXT;
        return ret;
        }
    ((handle)->AL)->StartActiveScheduler();

    if ((handle->iCallbackError) != KErrNone)
        {
        TRACE_PRN_FN_EXT;
        return (handle->iCallbackError);
        }
    TRACE_PRN_FN_EXT;
    return KErrNone;

    }

/*******************************************************************/
int record_data(DevSoundWrapperSrc *handle)
    {
    TRACE_PRN_FN_ENT;

    (handle->AL)->InitialiseActiveListener();
    (handle->dev_sound)->RecordData();
    ((handle)->AL)->StartActiveScheduler();

    TRACE_PRN_FN_EXT;
    return KErrNone;
    }

/*********************************************************************/

int pre_init_setconf(GstDevsoundSrc *ds)
    {
    TRACE_PRN_FN_ENT;
    int ret = 0;
    DevSoundWrapperSrc* dsPtr = STATIC_CAST(DevSoundWrapperSrc*, ds->handle);

    // NOTE: it is too late for setting prio/pref here
    if (ds->pending.preferenceupdate == 1 || ds->pending.priorityupdate == 1)
        {
        ds->pending.preferenceupdate = FALSE;
        ds->pending.priorityupdate = FALSE;
        }
    if (ds->pending.gainupdate == 1)
        {
        (dsPtr->dev_sound)->SetGain(ds->gain);
        ds->pending.gainupdate = FALSE;
        }

    if (ds->pending.leftbalanceupdate == 1 || ds->pending.rightbalanceupdate
            == 1)
        {

            TRAP( ret, (dsPtr->dev_sound)->SetRecordBalanceL(ds->leftbalance,
                            ds->rightbalance) );
        ds->pending.leftbalanceupdate = FALSE;
        ds->pending.rightbalanceupdate = FALSE;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }
/*********************************************************/
void getsupporteddatatypes(GstDevsoundSrc *ds)
    {
    TRACE_PRN_FN_ENT;
    DevSoundWrapperSrc* dsPtr = STATIC_CAST(DevSoundWrapperSrc*, ds->handle);
        TRAP_IGNORE(dsPtr->GetDataTypesL(ds));

    TRACE_PRN_FN_EXT;
    }
/*********************************************************/
void DevSoundWrapperSrc::GetDataTypesL(GstDevsoundSrc *ds)
    {
    TRACE_PRN_FN_ENT;

    DevSoundWrapperSrc* dsPtr = STATIC_CAST(DevSoundWrapperSrc*, ds->handle);
    RArray<TFourCC> inputdatatypes;
    TMMFPrioritySettings prioritysettings;

    CleanupClosePushL(inputdatatypes);

    (dsPtr->dev_sound)->GetSupportedInputDataTypesL(inputdatatypes,
            prioritysettings);

    for (TInt i = 0; i < inputdatatypes.Count(); i++)
        {
        TRACE_PRN_N2(_L("GstDevsoundSrc supported Codec[%d]==[0x%x]"), i+1, inputdatatypes[i].FourCC());
        guint *ip = g_new (guint, 1);
        *ip = inputdatatypes[i].FourCC();
        ds->fmt = g_list_append(ds->fmt, ip);
        }

    CleanupStack::PopAndDestroy(&inputdatatypes);
    TRACE_PRN_FN_EXT;
    }
/*********************************************************/
void populateproperties(GstDevsoundSrc *ds)
    {
    TRACE_PRN_FN_ENT;
    DevSoundWrapperSrc* dsPtr = STATIC_CAST(DevSoundWrapperSrc*, ds->handle);
    ds->samplesrecorded = (dsPtr->dev_sound)->SamplesRecorded();
    TRACE_PRN_FN_EXT;
    }

void initproperties(GstDevsoundSrc* ds)
    {
    TRACE_PRN_FN_ENT;
    DevSoundWrapperSrc* dsPtr=  STATIC_CAST(DevSoundWrapperSrc*, ds->handle);    
    ds->gain = (dsPtr->dev_sound)->Gain();
    ds->maxgain = (dsPtr->dev_sound)->MaxGain();
    dsPtr->GetSpeechEncoderProperties(ds);
    get_g711_encoder_vad_mode(dsPtr, &ds->g711vadmode);
    get_g729_vad_mode(dsPtr, &ds->g729vadmode);
    get_ilbc_vad_mode(dsPtr, &ds->ilbcvadmode);
    TRACE_PRN_FN_EXT;
    }

