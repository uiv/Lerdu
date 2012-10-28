// devsoundwrapper.h
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

#include <mmf/server/sounddevice.h>
#include <mmf/server/Mmfbuffer.h>
#include <mmf/server/MmfDatabuffer.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <pthreadtypes.h>
#include <e32def.h>
#include <e32cons.h>
#include <e32std.h>
#include <e32debug.h>
#include <gst/gstbuffer.h>
#include <gst/gst.h>


#include <SpeechEncoderConfig.h>
#include <G711EncoderIntfc.h>
#include <IlbcEncoderIntfc.h>
#include <G729EncoderIntfc.h>

#include "gstdevsoundsrc.h"

#include "gstg711encoderinterface.h"
#include "gstilbcencoderinterface.h"

class CActiveListener : public CActive
	{
public:
CActiveSchedulerWait *asw;
	CActiveListener() : CActive(CActive::EPriorityStandard)
		{
		CActiveScheduler::Add(this);
		}
	void InitialiseActiveListener()
		{
		iStatus = KRequestPending; SetActive();
		}
	// From CActive
	virtual void RunL()
		{
		asw->AsyncStop();
		}
	void StartActiveScheduler()
	{
	asw->Start();
	}
	virtual void DoCancel()
		{
			asw->AsyncStop();
		//iWait->Stop();
		};

	};


class DevSoundWrapperSrc :public MDevSoundObserver
{

public:
	CActiveListener	*AL;
	CActiveScheduler *as;
	TInt init_complete;
	CMMFBuffer *buffer;
	CMMFDevSound    *dev_sound;
	TMMFCapabilities caps;
  	int buffersize;
  	int gain;
  	int dev_count;
  	TInt iCallbackError;
  	TUint32 fourcc;
  	int bufferreadpos;
  	guint* supportedbitrates;
    CSpeechEncoderConfig* iSpeechEncoderConfig;
    CG711EncoderIntfc*    iG711EncoderIntfc;
    CG729EncoderIntfc*    iG729EncoderIntfc;
    CIlbcEncoderIntfc*    iIlbcEncoderIntfc;

public:
	DevSoundWrapperSrc();

	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	void GetDataTypesL(GstDevsoundSrc *ds);
	int GetSupportedBitratesL(GstDevsoundSrc* ds);
	void GetSpeechEncoderProperties(GstDevsoundSrc* ds);

};

#ifdef __cplusplus
extern "C" {
#endif


	int recordinit(DevSoundWrapperSrc *handle);
	int read_data(DevSoundWrapperSrc *handle,void *ptr, int length);
	int record_data(DevSoundWrapperSrc *handle);
	int get_rate(DevSoundWrapperSrc *handle);
	int get_channels(DevSoundWrapperSrc *handle);
	int get_encoding(DevSoundWrapperSrc *handle);
	int get_size(DevSoundWrapperSrc *handle);
	void set_rate(DevSoundWrapperSrc *handle,int rate);
	void set_channels(DevSoundWrapperSrc *handle,int channels);
	void set_encoding(DevSoundWrapperSrc *handle,int encoding);
	void set_size(DevSoundWrapperSrc *handle,int size);
	void set_fourcc(DevSoundWrapperSrc *handle,int fourcc);

	int open_devsound(DevSoundWrapperSrc **handle);
    int open_device(DevSoundWrapperSrc **handle);
    int initialize_devsound(GstDevsoundSrc* ds);
	int close_devsound(GstDevsoundSrc* ds);

	int SetConfigurations(DevSoundWrapperSrc *handle);
	int reset_devsound(DevSoundWrapperSrc *handle);

    int get_databuffer(DevSoundWrapperSrc *handle, const TUint8** buffer);
    int get_databuffer_size(DevSoundWrapperSrc *handle);

    int post_init_setconfig(DevSoundWrapperSrc *handle);
    void populateproperties(GstDevsoundSrc *ds);
    void initproperties(GstDevsoundSrc* ds);
    int pre_init_setconf(GstDevsoundSrc *ds);
    void getsupporteddatatypes(GstDevsoundSrc *ds);

    int get_speech_encoder_bit_rate(DevSoundWrapperSrc *handle,guint* aBitrate);
    int get_speech_encoder_supported_bit_rates(GstDevsoundSrc *ds);
    int get_speech_encoder_vad_mode(DevSoundWrapperSrc *handle,gboolean* aVadMode);
    int set_speech_encoder_bit_rate(DevSoundWrapperSrc *handle,guint aBitrate);
    int set_speech_encoder_vad_mode(DevSoundWrapperSrc *handle,gboolean aVadMode);

    int get_g711_encoder_vad_mode(DevSoundWrapperSrc *handle,gboolean* aVadMode);
    int set_g711_encoder_mode(DevSoundWrapperSrc *handle,enum TG711EncodeMode aEncodeMode);
    int set_g711_vad_mode(DevSoundWrapperSrc *handle,gboolean aVadMode);

    int set_g729_vad_mode(DevSoundWrapperSrc *handle,TBool aVadMode);
    int get_g729_vad_mode(DevSoundWrapperSrc *handle,TBool* aVadMode);

    int set_ilbc_encoder_mode(DevSoundWrapperSrc *handle,enum TIlbcEncodeMode aEncodeMode);
    int set_ilbc_vad_mode(DevSoundWrapperSrc *handle,gboolean aVadMode);
    int get_ilbc_vad_mode(DevSoundWrapperSrc *handle,gboolean* aVadMode);


#ifdef __cplusplus
}//extern c
#endif
