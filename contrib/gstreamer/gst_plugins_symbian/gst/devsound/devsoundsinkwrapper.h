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
#include <semaphore.h>

#include <e32def.h>
#include <e32cons.h>
#include <e32std.h>



#include "gstdevsoundsink.h"

#include <IlbcDecoderIntfc.h>
#include <G711DecoderIntfc.h>
#include <G729DecoderIntfc.h>
#include <ErrorConcealmentIntfc.h>
#include <AudioOutput.h>

#include "gstg729decoderinterface.h"
#include "gstilbcdecoderinterface.h"

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
		};

	};


class DevSoundWrapper:public MDevSoundObserver
{

public:
    DevSoundWrapper();

	CActiveListener	*AL;
	CActiveScheduler *as;
	TInt init_complete;
	CMMFBuffer *buffer;
	CMMFDevSound    *dev_sound;
	TMMFCapabilities caps;
  	int buffersize;
  	int volume;
  	int dev_count;
  	TInt iCallbackError;
  	TUint32 fourcc;
  	bool eosReceived;
  	//sem_t mutex;
  	//RArray<TFourCC> supportedtypes;
    CErrorConcealmentIntfc* iErrConcealmentIntfc;
    CG711DecoderIntfc*      iG711DecoderIntfc;
    CG729DecoderIntfc*      iG729DecoderIntfc;
    CIlbcDecoderIntfc*      iIlbcDecoderIntfc;
    CAudioOutput*           iAudioOutput;


public:
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	void GetDataTypesL(GstDevsoundSink *ds);
};

#ifdef __cplusplus
extern "C" {
#endif

	int playinit(DevSoundWrapper *handle);

	int write_data(DevSoundWrapper *handle,TUint8 *ptr, int length, gboolean lastBuffer);
	int play_data(DevSoundWrapper *handle);
	int get_rate(DevSoundWrapper *handle);
	int get_channels(DevSoundWrapper *handle);
	int get_encoding(DevSoundWrapper *handle);
	int get_size(DevSoundWrapper *handle);
	void set_rate(DevSoundWrapper *handle,int rate);
	void set_channels(DevSoundWrapper *handle,int channels);
	void set_encoding(DevSoundWrapper *handle,int encoding);
	void set_size(DevSoundWrapper *handle,int size);
	void set_fourcc(DevSoundWrapper *handle,int fourcc);

	void set_volume(DevSoundWrapper* handle,int volume);

	int open_devsound(DevSoundWrapper **handle);
	int open_device(DevSoundWrapper **handle);
    int initialize_devsound(GstDevsoundSink* sink);
    int close_devsound(GstDevsoundSink *ds);
    int check_if_device_open(DevSoundWrapper *handle) ;

	int get_ds_cb_error(DevSoundWrapper *handle);

	//Error Concealment custom interface
    void conceal_error_for_next_buffer(DevSoundWrapper *handle);
    void set_framemode(DevSoundWrapper *handle,gboolean mode);
    int framemode_rqrd_for_ec(DevSoundWrapper *handle,gboolean* modereq);

    //G711 custom interface
    int set_decodermode(DevSoundWrapper* handle,int aDecodeMode);
    int set_cng(DevSoundWrapper* handle,gboolean aCng);
    int get_cng(DevSoundWrapper* handle,gboolean* aCng);
    int set_plc(DevSoundWrapper* handle,gboolean aPlc);

    //G729 custom interface
    gint badlsfnextbuffer(DevSoundWrapper *handle);

    //Ilbc custom interace
    gint get_ilbccng(DevSoundWrapper *handle,gboolean* aCng);
    gint set_ilbccng(DevSoundWrapper *handle,gboolean aCng);
    gint set_ilbcdecodermode(DevSoundWrapper *handle,enum TIlbcDecodeMode aDecodeMode);


	int post_init_setconfig(DevSoundWrapper *handle);
	int reset_devsound(DevSoundWrapper *handle);

	void populateproperties(GstDevsoundSink *ds);
    void initproperties(GstDevsoundSink* ds);
	int pre_init_setconf(GstDevsoundSink *ds);
	void getsupporteddatatypes(GstDevsoundSink *ds);


#ifdef __cplusplus
}//extern c
#endif
