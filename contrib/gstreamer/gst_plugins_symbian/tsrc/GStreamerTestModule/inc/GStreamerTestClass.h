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



#ifndef CGStreamerTestClass_H
#define CGStreamerTestClass_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <e32svr.h>
#include <e32base.h>
#include <e32cons.h>
#include <gst/gst.h>

#include "TimeoutController.h"
#include "gstg711encoderinterface.h"
#include "gstspeechencoderconfig.h"
#include "gsterrorconcealmentinterface.h"
#include "gstg711decoderinterface.h"
#include "gstilbcdecoderinterface.h"
#include "gstilbcencoderinterface.h"
#include "gstg729encoderinterface.h"
#include "gstg729decoderinterface.h"


//global variables
/*static GstElement *iPipeline;
static GMainLoop *iLoop;
static GstElement *iConverter;
static GstElement *iResampler;
static GstElement *iSink;*/

// CONSTANTS
const TInt KErrBadTestParameter= -1000;         // Error on configuration file

const TInt KErrEventPending =-2000;             // Timeout and not callback 
const TInt KErrCallbackErrorCode = -2001;       // Callback return error code
const TInt KErrUnexpectedValue =-2002;          // Unexpected value at setting    
const TInt KErrTimeoutController = -2007;		// Unexpected notification

const TInt KShortTimeout = 1000;
const TInt KMediumTimeout = 4000;
const TInt KLongTimeout = 100000;


_LIT( KMsgBadTestParameters, "[Error] No valid test case parameters");

// General
_LIT( KTagLocation, "location");

// Common Properties
_LIT( KTagRateProperty,"rate");
_LIT( KTagChannelsProperty,"channels");
_LIT( KTagLeftBalanceProperty,"leftbalance");


// Sink Properties
_LIT( KTagVolumeProperty,"volume");
_LIT( KTagMaxVolumeProperty,"maxvolume");
_LIT( KTagSamplesPlayedProperty,"samplesplayed");
_LIT( KTagRightBalanceProperty,"rightbalance");
_LIT( KTagVolumeRampProperty,"volumeramp");
_LIT( KTagPriorityProperty,"priority");
_LIT( KTagPereferenceProperty,"preference");
_LIT( KTagFourCCProperty,"fourcc");
_LIT( KTagMimeTypeProperty,"mimetype");

// Source Properties
_LIT( KTagGainProperty,"gain");
_LIT( KTagMaxGainProperty,"maxgain");
_LIT( KTagSamplesRecordedProperty,"samplesrecorded");



// MACROS
//#define ?macro ?macro_def
// Logging path
#ifdef __WINSCW__
_LIT( KGStreamerTestClassLogPath, "\\logs\\testframework\\" ); 
#else
_LIT( KGStreamerTestClassLogPath, "e:\\testing\\Log\\" );
#endif

// Log file
_LIT( KGStreamerTestClassLogFile, "GStreamerTestClass.txt" ); 


// FORWARD DECLARATIONS
class CGStreamerTestClass;
//typedef struct _GstObjects GstObjects;
class CGlibEventHandler: public CActive
{
    
public:
    static CGlibEventHandler* NewL();
    ~CGlibEventHandler();
    
    void RunL();
    void DoCancel();
    void Start();
    void Stop();
private:
    CGlibEventHandler();
    void ConstructL();  
    
    RTimer iTimer;
};


class CGstObjects : public CBase
    {
public:

    static CGstObjects* NewL(void* aParent);    
    virtual ~CGstObjects();
private:
    void ConstructL();
    CGstObjects(void* aParent);
    
public :
    
    GstElement *iPipeline;
    GMainLoop *iLoop;
    GstElement *iConverter;
    GstElement *iResampler;
    GstElement *iSink;        
    GstElement *iSource;
    GstElement *iWavparse;

    GstElement *iDecoder;
    GstElement *iFakesink;
    GstElement *iFilesink;
    GstElement *iEncoder;
    GstElement *iFilter;
    GstElement *iWavenc;
    GstBus* iBus;
    GstCaps* iCaps;
    GstG711EncoderIntfc *iG711EncoderIntfc;
    GstG729EncoderIntfc *iG729EncoderIntfc;
    GstIlbcEncoderIntfc *iIlbcEncoderIntfc;
    GstSpeechEncoderConfigIntfc *iSpeechEncoderIntfc;
    GstErrorConcealmentIntfc *iErrconcealmentIntfc;
    GstG711DecoderIntfc *iG711DecoderIntfc;
    GstG729DecoderIntfc *iG729DecoderIntfc;
    GstIlbcDecoderIntfc *iIlbcDecoderIntfc;  
    FILE* fp;
    CGlibEventHandler* iCGlibEventHandler;
    void *iParent;
    };

// CLASS DECLARATION

/**
*  CGStreamerTestClass test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series60_3_2
*/
NONSHARABLE_CLASS(CGStreamerTestClass) : public CScriptBase, 
											public MTimeoutObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CGStreamerTestClass* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CGStreamerTestClass();

     
		enum TGSTExpectedEvent
            {
            EEOSReceived
            };

		enum TGStreamerElements
		    {
		    EFILESOURCE = 1,
		    EFILESINK,
		    EFAKESOURCE,
		    EFAKESINK,
		    EWAVPARSE,		    
		    EMP3DECODER,
		    EWAVEENCODER,
		    ERESAMPLER,
		    ECONVERTER,
		    EDEVSOUNDSRC,
		    EDEVSOUNDSINK
		    };
		
      enum TElementCustomInterfaces
            {
            EG711ENCODER = 1,
            EG711DECODER,
            EG729ENCODER,
            EG729DECODER,
            EILBCENCODER,
            EILBCDECODER,
            ESPEECHENCODER,          
            EERRORCONCEALMENT
            };

      enum TG711DecCustomIntfcProp
          {
          EG711SetDecoderMode = 1,
          EG711SetCng,
          EG711GetCng,
          EG711SetPlc
          };

      enum TILBCDecCustomIntfcProp
          {
          EIlbcSetDecoderMode = 1,
          EIlbcSetCng,
          EIlbcGetCng,
          };   
      
      enum TG729DecCustomIntfcProp
          {
          EG729BadLsfNextBuffer = 1
          };
      
      enum TG711EncCustomIntfcProp
          {
          EG711SetEncoderMode = 1,
          EG711SetVadMode,
          EG711GetVadMode          
          };

      enum TILBCEncCustomIntfcProp
          {
          EIlbcSetEncoderMode = 1,
          EIlbcSetVadMode,
          EIlbcGetVadMode          
          };
      
      enum TG729EncCustomIntfcProp
          {
          EG729SetVadMode = 1,
          EG729GetVadMode          
          };
      
      enum TSpEncCustomIntfcProp
          {
          ESPENGetSupportedBitrates = 1,
          ESPENSetBitrate,
          ESPENGetBitrate,
          ESPENSetVadMode,
          ESPENGetVadMode       
          };
      
      enum TErrorConcealCustomIntfcProp
          {
          EECConcealErrorForNextBuffer = 1,
          EECSetFrameMode,
          EECFrameModeRqrdForEC         
          };    
      
    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );


 	
	    // From MTimeoutObserver

	    /**
	    * @since ?Series60_version
	    * @param none
	    * Review if all the expected events have ocurred once the time is over
	    */
	    void HandleTimeout(TInt error);

        /**
        * Verify that the event was expected, removes it from the list
        * Signal the TestScripter with the returned error code
        * @since ?Series60_version
        */
        void ProcessEvent(TGSTExpectedEvent aEvent, TInt aError);


    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CGStreamerTestClass( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */

	    /**
	    * Set an event as expected and set default timeout
	    * @since ?Series60_version
	    */
	    void AddExpectedEvent(TGSTExpectedEvent event, TInt ms);


	    /**
	    * Unset an event as expected
	    * @since ?Series60_version
	    */
	    TBool RemoveExpectedEvent(TGSTExpectedEvent event);


	    /**
	    * @since ?Series60_version
	    * @param none
	    * Removes all expected events
	    */
	    void RemoveAllExpectedEvents();



	    /**
	    * Maps a event with a descriptor with its name
	    * @since ?Series60_version
	    */
	    TPtrC EventName( TInt aKey );

	    /*
	    * Test methods are listed below.
	    */

	    /**
	    * Sets a timeout different since the default
	    * @since Series60_3_2
	    * @param aItem Script line containing parameters.
	    * @return Symbian OS error code.
	    */
	    TInt SetTimeout( CStifItemParser& aItem );

	    /**
	    *
	    * @since Series60_3_2
	    * @param aItem Script line containing parameters.
	    * @return Symbian OS error code.
	    */
	    TInt SetExpectedEvents( CStifItemParser& aItem );

	    /**
	    *
	    * @since Series60_3_2
	    * @param aItem Script line containing parameters.
	    * @return Symbian OS error code.
	    */
	    TInt SetAllowedPanic( CStifItemParser& aItem );
	    
	    /**
	    * Connect to the default client to the server
	    * @since ?Series60_version
	    * @param aItem Script line containing parameters.
	    * @return Symbian OS error code.
	    */

		TInt ExampleL( CStifItemParser& aItem );
			
		TInt InitGStreamer( CStifItemParser& aItem );
		TInt CreatePipeLine( CStifItemParser& aItem );
		TInt CreateElement( CStifItemParser& aItem );
		TInt SetElementProperties( CStifItemParser& aItem );
		TInt GetElementProperties( CStifItemParser& aItem );
		TInt AddElementToPipeline( CStifItemParser& aItem );
		TInt SetPipelineState( CStifItemParser& aItem );
		TInt InitPipeLine( CStifItemParser& aItem );
		TInt LinkElementsInPipeline( CStifItemParser& aItem );
		TInt GetElementsCustomIntfc( CStifItemParser& aItem );
		TInt CustomIntfcProp( CStifItemParser& aItem );
		
		TInt SetCapsInPipeLine( CStifItemParser& aItem );
		
		TInt SetMainLoopRun( CStifItemParser& aItem );
		TInt GetConfigureData();
		TInt SetConfigureData();


    
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
    	// reference to TestModuleIf
    	CTestModuleIf& iTestModuleIf;
   	
    	// Active object with a timer to timeout the test case
    	CSimpleTimeout * iTimeoutController;

    	// Indicates if the test case use a normal exit reason
    	TBool iNormalExitReason;

    	// List of expected events
    	RArray<TGSTExpectedEvent> iExpectedEvents;

    	// List of notification event (by the callback)
    	RArray<TGSTExpectedEvent> iOcurredEvents;
    	

    	/*GstElement *iSource;
    	GstElement *iWavparse;

    	GstElement *iDecoder;
    	GstElement *iFakesink;
    	GstElement *iFilesink;
    	GstElement *iEncoder;
    	GstElement *iFilter;
    	GstElement *iWavenc;
    	GstBus* iBus;
    	GstCaps* iCaps;
    	GstG711EncoderIntfc *iG711EncoderIntfc;
        GstG729EncoderIntfc *iG729EncoderIntfc;
        GstIlbcEncoderIntfc *iIlbcEncoderIntfc;
    	GstSpeechEncoderConfigIntfc *iSpeechEncoderIntfc;
    	GstErrorConcealmentIntfc *iErrconcealmentIntfc;
    	GstG711DecoderIntfc *iG711DecoderIntfc;
        GstG729DecoderIntfc *iG729DecoderIntfc;
        GstIlbcDecoderIntfc *iIlbcDecoderIntfc;*/
        
        CGstObjects* iObjects;
    	
    };

#endif      // CGStreamerTestClass_H

// End of File
