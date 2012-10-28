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



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "GStreamerTestClass.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>


_LIT8(defaultMedia,"audio/x-raw-int");
_LIT8(amrMedia,"audio/amr");
_LIT8(g711Media,"audio/x-alaw");
_LIT8(g729Media,"audio/g729");
_LIT8(iLbcMedia,"audio/ilbc");
_LIT8(mp3Media,"audio/mp3");


CGlibEventHandler* CGlibEventHandler::NewL()
{
    CGlibEventHandler* self = new(ELeave) CGlibEventHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


CGlibEventHandler::~CGlibEventHandler()
{
    iTimer.Close();
}
    

void CGlibEventHandler::Start()
{
    iTimer.After(iStatus, TTimeIntervalMicroSeconds32(1000));
    SetActive();    
}
    
    
void CGlibEventHandler::Stop()
{
    Cancel();
}
    
    
void CGlibEventHandler::RunL()
{
    g_main_context_iteration(NULL, FALSE);
    iTimer.After(iStatus, TTimeIntervalMicroSeconds32(1000));
    SetActive();    
}
    
    
void CGlibEventHandler::DoCancel()
{
    iTimer.Cancel();    
}


CGlibEventHandler::CGlibEventHandler():CActive(EPriorityStandard)
{
    iTimer.CreateLocal();
    CActiveScheduler::Add(this);
}
    
void CGlibEventHandler::ConstructL()
{
    
}

// -----------------------------------------------------------------------------
// CGstObjects::CGstObjects
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGstObjects::CGstObjects(void* aParent): iParent(aParent)
    {
    iPipeline = NULL;
    iLoop = NULL;
    iConverter = NULL;
    iResampler = NULL;
    iSink = NULL;        
    iSource = NULL;
    iWavparse = NULL;
    iDecoder = NULL;
    iFakesink = NULL;
    iFilesink = NULL;
    iEncoder = NULL;
    iFilter = NULL;
    iWavenc = NULL;
    iBus = NULL;
    iCaps = NULL;
    iG711EncoderIntfc = NULL;
    iG729EncoderIntfc = NULL;
    iIlbcEncoderIntfc = NULL;
    iSpeechEncoderIntfc = NULL;
    iErrconcealmentIntfc = NULL;
    iG711DecoderIntfc = NULL;
    iG729DecoderIntfc = NULL;
    iIlbcDecoderIntfc = NULL;  
    fp=NULL;
    iCGlibEventHandler = NULL;
    }

// -----------------------------------------------------------------------------
// CGstObjects::NewL
// Static constructor.
// -----------------------------------------------------------------------------
//
CGstObjects* CGstObjects::NewL(void* aParent)
    {
    CGstObjects* self = new (ELeave) CGstObjects(aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CGstObjects::ConstructL
// Symbian two-phase constructor
// -----------------------------------------------------------------------------
//
void CGstObjects::ConstructL()
    {
    iCGlibEventHandler = CGlibEventHandler::NewL();
    iCGlibEventHandler->Start();
    }

// -----------------------------------------------------------------------------
// CGstObjects::~CGstObjects
// Destructor deallocate memory.
// -----------------------------------------------------------------------------
//
CGstObjects::~CGstObjects()
    {
    }

gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  CGstObjects* objects = (CGstObjects*) data;
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        gst_element_set_state (GST_ELEMENT (objects->iPipeline), GST_STATE_NULL);        
        //g_main_loop_quit(objects->iLoop);
        ((CGStreamerTestClass*)objects->iParent)->Signal(KErrNone);
      break;
    case GST_MESSAGE_ERROR: {
      gchar *debug;
      GError *err;
      gst_message_parse_error (msg, &err, &debug);
      g_free (debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      break;
    }
    default:
      break;
  }

  return TRUE;
}


static void 
new_pad_cb (GstElement *wavparse, GstPad *new_pad, gpointer data)
 {
    CGstObjects* objects = (CGstObjects*) data; 
    gst_element_set_state ((_GstElement *)objects->iPipeline, GST_STATE_PAUSED);
     
    if(! gst_element_link_many (wavparse,objects->iConverter, objects->iResampler, objects->iSink, NULL))
        g_print ("link(wavparse,conv,remaple sink) failed!\n");  
 
    gst_element_set_state ((_GstElement *)objects->iPipeline, GST_STATE_PLAYING);
 }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGStreamerTestClass::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CGStreamerTestClass::Delete() 
{
    
    FTRACE(FPrint(_L("CGStreamerTestClass::Delete")));
    
    iExpectedEvents.Close();
    iOcurredEvents.Close();
    iObjects->iCGlibEventHandler->Stop();
    //fclose(iObjects->fp);
    gst_object_unref(GST_OBJECT (iObjects->iPipeline));
    delete iObjects->iCGlibEventHandler;
    //g_main_loop_unref(iObjects->iLoop);
    delete iObjects;
    iObjects = NULL;
    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));
				
}

// -----------------------------------------------------------------------------
// CGStreamerTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CGStreamerTestClass::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        

		ENTRY( "CreatePipeLine", CGStreamerTestClass::CreatePipeLine ),  	
		ENTRY( "CreateElement", CGStreamerTestClass::CreateElement ),  
		ENTRY( "InitGStreamer", CGStreamerTestClass::InitGStreamer ),
		ENTRY( "InitPipeLine", CGStreamerTestClass::InitPipeLine ),
		ENTRY( "SetElementProperties", CGStreamerTestClass::SetElementProperties ),
		ENTRY( "GetElementProperties", CGStreamerTestClass::GetElementProperties ),
		ENTRY( "AddElementToPipeline", CGStreamerTestClass::AddElementToPipeline ),
		ENTRY( "SetPipelineState", CGStreamerTestClass::SetPipelineState ),
		ENTRY( "LinkElementsInPipeline", CGStreamerTestClass::LinkElementsInPipeline ),
		ENTRY( "GetElementsCustomIntfc", CGStreamerTestClass::GetElementsCustomIntfc ),
		ENTRY( "CustomIntfcProp", CGStreamerTestClass::CustomIntfcProp ),
		
        ENTRY( "SetTimeout", CGStreamerTestClass::SetTimeout ),
        ENTRY( "SetAllowedPanic", CGStreamerTestClass::SetAllowedPanic ),
        ENTRY( "SetExpectedEvents", CGStreamerTestClass::SetExpectedEvents ),
        ENTRY( "SetMainLoopRun", CGStreamerTestClass::SetMainLoopRun ),
        ENTRY( "SetCapsInPipeLine", CGStreamerTestClass::SetCapsInPipeLine ),
		
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CGStreamerTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CGStreamerTestClass::EventName( TInt aKey )
{
	static TText* const badKeyword = (TText*)L"BadKeyword";
	static TText* const keywords[] =
	{

		(TText*)L"EEOSReceived",
		
	};

			

	if( (TUint)aKey >= (sizeof( keywords )/sizeof(TText*)) )
		{
		iLog->Log(_L("Keyword out of bounds"));
		TPtrC keyword( badKeyword );
		return keyword;
		}
	else
		{
		TPtrC keyword( keywords[aKey] );
		return keyword;
		}
}

// -----------------------------------------------------------------------------
// CGStreamerTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CGStreamerTestClass::AddExpectedEvent(TGSTExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CGStreamerTestClass::AddExpectedEvent")));
	iExpectedEvents.Append(event);
	TPtrC eventName = EventName(event);
	iLog->Log(_L("Adding expected event:(0x%02x)%S Total=%d"), event, &eventName, iExpectedEvents.Count() );

	if ( iTimeoutController && !iTimeoutController->IsActive() )
		{
		if (ms > 0)
			{
			iTimeoutController->Start( TTimeIntervalMicroSeconds(ms * 1000) );
			}
		else
			{
			iLog->Log(_L("Timeout with default value (1s)"));
			iTimeoutController->Start( TTimeIntervalMicroSeconds(1000000) );
			}
		}
}

// -----------------------------------------------------------------------------
// CGStreamerTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CGStreamerTestClass::RemoveExpectedEvent(TGSTExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CGStreamerTestClass::RemoveExpectedEvent")));
	TBool match = EFalse;
	for (TUint i=0; i < iExpectedEvents.Count() ; i++)
		{
		if (iExpectedEvents[i] == aEvent)
			{
			iExpectedEvents.Remove(i);
			match = ETrue;
			break;
			}
		}

	return match;
}


// -----------------------------------------------------------------------------
// CGStreamerTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CGStreamerTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CGStreamerTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));
	
	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CGStreamerTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CGStreamerTestClass::ProcessEvent(TGSTExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CGStreamerTestClass::ProcessExpectedEvent")));
	TPtrC nameEvent = EventName(aEvent);

	// Check for error
	if (aError == KErrNone)
		{
		// Remove the event
		if (RemoveExpectedEvent(aEvent))
			{
			iLog->Log(_L("Expected Event: (0x%02x)%S has ocurred Total=%d"), aEvent, &nameEvent,iExpectedEvents.Count());
			}
		else
			{
			iLog->Log(_L("Event: (0x%02x)%S has ocurred"), aEvent, &nameEvent);
			return;
			}

		

		// All expected events have ocurred
		if (iExpectedEvents.Count() == 0 )
			{
			Signal();
			iTimeoutController->Cancel();
			}
		
		
		}
	else
		{
		iLog->Log(_L("[Error] Event: (0x%02x)%S return with error code=%d"), aEvent, &nameEvent, aError);
		if (iExpectedEvents.Count() != 0 )
			{
			RemoveExpectedEvent(aEvent);
			}
		iTimeoutController->Cancel();
		Signal(KErrCallbackErrorCode);
		}
}

// -----------------------------------------------------------------------------
// CGStreamerTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CGStreamerTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CGStreamerTestClass::HandleTimeout")));
	// All expected events have ocurred
	if (aError != KErrNone)
		{
		if (iExpectedEvents.Count() == 0 )
			{
			iLog->Log(_L("Timing out but events have ocurred"));
			Signal();
			}
		else
			{
			RemoveAllExpectedEvents();
			iLog->Log(_L("Timing out and events still pending"));
			Signal(KErrEventPending);
			}
		}
	else
		{
		iLog->Log(_L("Timing out return a error %d"), aError);
		Signal(aError);
		}
}

// -----------------------------------------------------------------------------
// CGStreamerTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CGStreamerTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CGStreamerTestClass::SetTimeout")));
	TInt timeout=0;
	TInt error = aItem.GetNextInt(timeout) ;
	if ( iTimeoutController )
		{
		if ( timeout > 0 )
			{
			iTimeoutController->Start( TTimeIntervalMicroSeconds(timeout*1000) );
			}
		else
			{
			iTimeoutController->Start( TTimeIntervalMicroSeconds(1000000) );
			}
		}
	else
		{
		iLog->Log(_L("Timeout Controller doesn't exist"));
		error = KErrTimeoutController;
		}
	return error;
}


// -----------------------------------------------------------------------------
// Uses the TestModuleBase API to allow a panic as exit reason for a test case
// -----------------------------------------------------------------------------
TInt CGStreamerTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CGStreamerTestClass::SetAllowedPanic")));
	TInt error = KErrNone;
	TInt panicCode;
	TPtrC panicType;
	if (  ( KErrNone == aItem.GetNextString(panicType) ) &&
	( KErrNone == aItem.GetNextInt(panicCode) )  )
		{
		iLog->Log(_L("Allowing panic: %S %d"), &panicType, panicCode);
		iTestModuleIf.SetExitReason( CTestModuleIf::EPanic, panicCode );
		iNormalExitReason = EFalse;
		}
	else
		{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
		}
	return error;
}

// -----------------------------------------------------------------------------
// CGStreamerTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CGStreamerTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CGStreamerTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		TInt timeout=0;
		aItem.GetNextInt(timeout);
		AddExpectedEvent(static_cast<TGSTExpectedEvent>(event), timeout); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CGStreamerTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CGStreamerTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KGStreamerTestClass, "GStreamerTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KGStreamerTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KGStreamerTestClass, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }
    

// -----------------------------------------------------------------------------
// CGStreamerTestClass::CreatePipeLine
// Create Pipeline
// -----------------------------------------------------------------------------
//

TInt CGStreamerTestClass::InitGStreamer( CStifItemParser& /*aItem*/ )
    {   
    FTRACE(FPrint(_L("CGStreamerTestClass::InitGStreamer")));
    iLog->Log(_L(">>CGStreamerTestClass::InitGStreamer 1 "));
    char* initstatus = getenv("GST_INIT_STATUS");
    iLog->Log(_L(">>CGStreamerTestClass::InitGStreamer 1.1 [%d] [%s] "),initstatus,&initstatus);
    if(!initstatus)
        {
        iLog->Log(_L(">>CGStreamerTestClass::InitGStreamer Initializing GST "));
        gst_init(NULL,NULL);    
        setenv("GST_INIT_STATUS","INITIALIZED",1);        
        }
    else
        {
        iLog->Log(_L(">>CGStreamerTestClass::InitGStreamer GST already INIT "));
        }
    iObjects = CGstObjects::NewL(this);
    //iLog->Log(_L(">>CGStreamerTestClass::InitGStreamer 2 [%x]"),iObjects->iLoop);
    //iObjects->iLoop = g_main_loop_new (NULL, FALSE); 
    iLog->Log(_L(">>CGStreamerTestClass::InitGStreamer 3 [%x]"),iObjects->iSink);
    //iObjects->fp = freopen("e:\\out.txt","w",stderr); 

    iLog->Log(_L("<<CGStreamerTestClass::InitGStreamer"));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CGStreamerTestClass::CreatePipeLine
// Create Pipeline
// -----------------------------------------------------------------------------
//

TInt CGStreamerTestClass::InitPipeLine( CStifItemParser& /*aItem*/ )
    {   
    FTRACE(FPrint(_L("CGStreamerTestClass::InitPipeLine")));
    iLog->Log(_L(">>CGStreamerTestClass::InitPipeLine"));   
    iObjects->iBus = gst_pipeline_get_bus (GST_PIPELINE (iObjects->iPipeline));
    gpointer data(iObjects);
    gst_bus_add_watch(iObjects->iBus, bus_call, data);
    gst_object_unref (iObjects->iBus);
    iLog->Log(_L("<<CGStreamerTestClass::InitPipeLine"));   
    return KErrNone;
    }

TInt CGStreamerTestClass::SetMainLoopRun( CStifItemParser& /*aItem*/ )
    {
    TInt error(KErrNone);
    iLog->Log(_L(">>CGStreamerTestClass::SetMainLoopRun"));
    //g_main_loop_run(iObjects->iLoop);
    iLog->Log(_L("<<CGStreamerTestClass::SetMainLoopRun Error[%d]"),error);
    return error;
    }
// -----------------------------------------------------------------------------
// CGStreamerTestClass::CreatePipeLine
// Create Pipeline
// -----------------------------------------------------------------------------
//

TInt CGStreamerTestClass::CreatePipeLine( CStifItemParser& /*aItem*/ )
	{	
	FTRACE(FPrint(_L("CGStreamerTestClass::CreatePipeLine")));
	iLog->Log(_L(">>CGStreamerTestClass::CreatePipeLine"));

	TInt error(KErrNone);
	
	if(!iObjects->iPipeline)
	    {
	    iObjects->iPipeline = gst_pipeline_new ("pipeline");
	    }
	else
	    {
	    error = KErrAlreadyExists;
	    }
	iLog->Log(_L("<<CGStreamerTestClass::CreatePipeLine Error[%d]"),error);
	return error;
	}

TInt CGStreamerTestClass::CreateElement( CStifItemParser& aItem )
	{	
	FTRACE(FPrint(_L("CGStreamerTestClass::CreateElement")));
	iLog->Log(_L(">>CGStreamerTestClass::CreateElement"));
	TInt error = KErrNone;
	TInt element;
	aItem.GetNextInt(element);
	switch(element)
	    {
	    case   EFILESOURCE:
	        {
	        if(iObjects->iSource)
	            {
	            error = KErrAlreadyExists;
	            }
	        else
	            {
	            iObjects->iSource = gst_element_factory_make("filesrc","filesrc");
                if(!iObjects->iSource)
                    {
                    error = KErrNotFound;
                    }
	            }
	        break;
	        }
	    case   EFILESINK:
            {
            if(iObjects->iSink)
                {
                error = KErrAlreadyExists;
                iLog->Log(_L(">>CGStreamerTestClass::CreateElement Creating FileSink Error [%x]"),iObjects->iSink);
                }
            else
                {
                iObjects->iSink = gst_element_factory_make("filesink","filesink");
                if(!iObjects->iSink)
                    {
                    error = KErrNotFound;
                    iLog->Log(_L(">>CGStreamerTestClass::CreateElement NoElement"));
                    }
                }
            break;
            }
	    case   EFAKESOURCE:
            {
            if(iObjects->iSource)
                {
                error = KErrAlreadyExists;
                }
            else
                {
                iObjects->iSource = gst_element_factory_make("fakesrc","fakesrc");
                if(!iObjects->iSource)
                    {
                    error = KErrNotFound;
                    }
                }
            break;
            }
	    case   EFAKESINK:
            {
            if(iObjects->iSink)
                {
                error = KErrAlreadyExists;
                }
            else
                {
                iObjects->iSink = gst_element_factory_make("fakesink","fakesink");
                if(!iObjects->iSink)
                    {
                    error = KErrNotFound;
                    }
                }
            break;
            }
	    case   EWAVPARSE:
            {
            if(iObjects->iWavparse)
                {
                error = KErrAlreadyExists;
                }
            else
                {
                iObjects->iWavparse = gst_element_factory_make("wavparse","wavparse");   
                g_signal_connect (iObjects->iWavparse, "pad-added", G_CALLBACK (new_pad_cb),iObjects);
                }
            break;
            }
	    case   EMP3DECODER:
            {
            if(iObjects->iDecoder)
                {
                error = KErrAlreadyExists;
                }
            else
                {
                iObjects->iDecoder = gst_element_factory_make("mad", "decoder");             
                }
            break;
            }
	    case   EWAVEENCODER:
            {
            if(iObjects->iEncoder)
                {
                error = KErrAlreadyExists;
                }
            else
                {
                iObjects->iEncoder = gst_element_factory_make("wavenc", "wavencoder");             
                }
            break;
            }
	    case   ERESAMPLER:
            {
            if(iObjects->iResampler)
                {
                error = KErrAlreadyExists;
                }
            else
                {
                iObjects->iResampler = gst_element_factory_make("audioresample", "audioresample");             
                }
            break;
            }
	    case   ECONVERTER:
            {
            if(iObjects->iConverter)
                {
                error = KErrAlreadyExists;
                }
            else
                {
                iObjects->iConverter = gst_element_factory_make("audioconvert", "audioconvert");             
                }
            break;
            }
        case   EDEVSOUNDSRC:
            {
            if(iObjects->iSource)
                {
                error = KErrAlreadyExists;
                }
            else
                {
                iObjects->iSource = gst_element_factory_make("devsoundsrc","devsoundsrc");
                if(!iObjects->iSource)
                    {
                    error = KErrNotFound;
                    }          
                }
            break;
            }            
        case   EDEVSOUNDSINK:
            {
            if(iObjects->iSink)
                {
                error = KErrAlreadyExists;
                }
            else
                {
                iObjects->iSink = gst_element_factory_make("devsoundsink","devsoundsink");
                if(!iObjects->iSink)
                    {
                    error = KErrNotFound;
                    }
                }
            break;
            }
        default:
            error = KErrNotFound;
	        break;
	    }
	iLog->Log(_L("<<CGStreamerTestClass::CreateElement Error[%d]"),error);
	return error;
	}

TInt CGStreamerTestClass::GetElementProperties( CStifItemParser& aItem )
    {   
    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties")));
    iLog->Log(_L(">>CGStreamerTestClass::GetElementProperties"));
    TInt error = KErrNone;
    TInt element;
    aItem.GetNextInt(element);
    switch(element)
        {
        case   EFILESOURCE:
            {
            if(!iObjects->iSource)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EFILESINK:
            {
            if(!iObjects->iSink)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EFAKESOURCE:
            {
            if(!iObjects->iSource)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EFAKESINK:
            {
            if(!iObjects->iSink)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EWAVPARSE:
            {
            if(!iObjects->iWavparse)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EMP3DECODER:
            {
            if(!iObjects->iDecoder)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EWAVEENCODER:
            {
            if(!iObjects->iEncoder)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   ERESAMPLER:
            {
            if(!iObjects->iResampler)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   ECONVERTER:
            {
            if(!iObjects->iConverter)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EDEVSOUNDSRC:
            {
            if(!iObjects->iSource)
                {
                error = KErrNotFound;
                }
            else
                {
                TPtrC property;
                TInt value(0);
                aItem.GetNextString(property);
                HBufC8* prop = HBufC8::NewL(property.Length()+ 1);
                TPtr8 des = prop->Des();
                des.Copy(property);
                if(!property.Compare(KTagGainProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), &value, NULL); 
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties Gain[%d]"),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties Gain[%d]"),value);
                    }
                else if(!property.Compare(KTagRateProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), &value, NULL);
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties Rate[%d]"),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties Rate[%d]"),value);
                    }
                else if(!property.Compare(KTagChannelsProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), &value, NULL); 
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties Channels[%d]"),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties channels[%d]"),value);
                    }
                else if(!property.Compare(KTagMaxGainProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), &value, NULL); 
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties MaxGain[%d]"),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties Max Gain[%d]"),value);
                    }
                else if(!property.Compare(KTagSamplesRecordedProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), &value, NULL); 
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties SamplesRecorded[%d]"),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties SamplesRecorded[%d]"),value);
                    }
                delete prop;
                }
            break;
            }            
        case   EDEVSOUNDSINK:
            {
            if(!iObjects->iSink)
                {
                error = KErrNotFound;
                }
            else
                {
                TPtrC property;
                TInt value(0);
                aItem.GetNextString(property);
                HBufC8* prop = HBufC8::NewL(property.Length()+ 1);
                TPtr8 des = prop->Des();
                des.Copy(property);
                if(!property.Compare(KTagVolumeProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), &value, NULL); 
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties Volume[%d]"),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties Volume[%d]"),value);
                    }
                else if(!property.Compare(KTagRateProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), &value, NULL); 
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties rate [%d]"),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties rate[%d]"),value);
                    }
                else if(!property.Compare(KTagChannelsProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), &value, NULL); 
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties channels [%d]"),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties channels[%d]"),value);
                    }
                else if(!property.Compare(KTagMaxVolumeProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), &value, NULL); 
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties max vol[%d] "),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties max vol[%d]"),value);
                    }
                else if(!property.Compare(KTagSamplesPlayedProperty()))
                    {
                    g_object_get (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), &value, NULL); 
                    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementProperties samples played[%d] "),value));
                    iLog->Log(_L("CGStreamerTestClass::GetElementProperties samples played[%d]"),value);
                    }
                delete prop;
                }
            break;
            }
        default:
            error = KErrNotFound;
            break;
        }
    iLog->Log(_L(">>CGStreamerTestClass::GetElementProperties Error[%d]"),error);
    return error;
    }


TInt CGStreamerTestClass::SetElementProperties( CStifItemParser& aItem )
    {   
    FTRACE(FPrint(_L("CGStreamerTestClass::SetElementProperties")));
    iLog->Log(_L(">>CGStreamerTestClass::SetElementProperties"));
    TInt error = KErrNone;
    TInt element;
    aItem.GetNextInt(element);
    switch(element)
        {
        case   EFILESOURCE:
            {
            if(!iObjects->iSource)
                {
                error = KErrNotFound;
                }
            else
                {
                TPtrC location;
                TFileName filename;
                
                char carray[1024];
                aItem.GetNextString(location);
                filename.Copy(location);
                wcstombs(carray, (const wchar_t *)filename.PtrZ(), 1024);
                g_object_set (G_OBJECT (iObjects->iSource), "location", carray, NULL);
                }
            break;
            }
        case   EFILESINK:
            {
            if(!iObjects->iSink)
                {
                error = KErrNotFound;
                }
            else
                {
                TPtrC location;
                TFileName filename;
                
                char carray[1024];
                aItem.GetNextString(location);
                filename.Copy(location);
                wcstombs(carray, (const wchar_t *)filename.PtrZ(), 1024);
                g_object_set (G_OBJECT (iObjects->iSink), "location", carray, NULL);             
                }
            break;
            }
        case   EFAKESOURCE:
            {
            if(!iObjects->iSource)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EFAKESINK:
            {
            if(!iObjects->iSink)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EWAVPARSE:
            {
            if(!iObjects->iWavparse)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EMP3DECODER:
            {
            if(!iObjects->iDecoder)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EWAVEENCODER:
            {
            if(!iObjects->iEncoder)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   ERESAMPLER:
            {
            if(!iObjects->iResampler)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   ECONVERTER:
            {
            if(!iObjects->iConverter)
                {
                error = KErrNotFound;
                }
            else
                {
                }
            break;
            }
        case   EDEVSOUNDSRC:
            {
            if(!iObjects->iSource)
                {
                error = KErrNotFound;
                }
            else
                {
                TPtrC property;
                aItem.GetNextString(property);
                HBufC8* prop = HBufC8::NewL(property.Length()+ 1);
                TPtr8 des = prop->Des();
                des.Copy(property);
                if(!property.Compare(KTagGainProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), value, NULL); 
                        FTRACE(FPrint(_L("CGStreamerTestClass::SetElementProperties Gain [%d]"),value));
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagRateProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagChannelsProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagLeftBalanceProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagRightBalanceProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagPriorityProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagPereferenceProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSource),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                delete prop;
                }
            break;
            }            
        case   EDEVSOUNDSINK:
            {
            if(!iObjects->iSink)
                {
                error = KErrNotFound;
                }
            else
                {
                TPtrC property;
                aItem.GetNextString(property);
                HBufC8* prop = HBufC8::NewL(property.Length()+ 1);
                TPtr8 des = prop->Des();
                des.Copy(property);
                if(!property.Compare(KTagVolumeProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        FTRACE(FPrint(_L("CGStreamerTestClass::SetElementProperties Volume [%d]"),value));
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagRateProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagChannelsProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagLeftBalanceProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagRightBalanceProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagVolumeRampProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagPriorityProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagPereferenceProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagMimeTypeProperty()))
                    {
                    TPtrC mimetype;
                    TFileName mime;
                    char carray[1024];

                    if(!aItem.GetNextString(mimetype))
                        {
                        mime.Copy(mimetype);
                        wcstombs(carray, (const wchar_t *)mime.PtrZ(), 1024);
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), carray, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                else if(!property.Compare(KTagFourCCProperty()))
                    {
                    TInt value;
                    if(!aItem.GetNextInt(value))
                        {
                        g_object_set (G_OBJECT (iObjects->iSink),(const char *)(des.PtrZ()), value, NULL); 
                        }
                    else
                        {
                        error = KErrNotFound;
                        }
                    }
                delete prop;
                }
            break;
            }
        default:
            error = KErrNotFound;
            break;
        }
    iLog->Log(_L("<<CGStreamerTestClass::SetElementProperties Error[%d]"),error);
    return error;
    }

TInt CGStreamerTestClass::AddElementToPipeline( CStifItemParser& aItem )
    {   
    FTRACE(FPrint(_L("CGStreamerTestClass::AddElementToPipeline")));
    iLog->Log(_L(">>CGStreamerTestClass::AddElementToPipeline"));
    TInt element;
    TInt error(KErrNone);
    if(!iObjects->iPipeline)
        {
        return KErrNotReady;
        }
    
    aItem.GetNextInt(element);
    switch(element)
        {
        case   EFILESOURCE:
            {
            if(!iObjects->iSource)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iSource, NULL);
                }
            break;
            }
        case   EFILESINK:
            {
            if(!iObjects->iSink)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iSink, NULL);             
                }
            break;
            }
        case   EFAKESOURCE:
            {
            if(!iObjects->iSource)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iSource, NULL);                             
                }
            break;
            }
        case   EFAKESINK:
            {
            if(!iObjects->iSink)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iSink, NULL);
                }
            break;
            }
        case   EWAVPARSE:
            {
            if(!iObjects->iWavparse)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iWavparse, NULL);
                }
            break;
            }
        case   EMP3DECODER:
            {
            if(!iObjects->iDecoder)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iDecoder, NULL);
                }
            break;
            }
        case   EWAVEENCODER:
            {
            if(!iObjects->iEncoder)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iEncoder, NULL);
                }
            break;
            }
        case   ERESAMPLER:
            {
            if(!iObjects->iResampler)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iResampler, NULL);
                }
            break;
            }
        case   ECONVERTER:
            {
            if(!iObjects->iConverter)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iConverter, NULL);
                }
            break;
            }
        case   EDEVSOUNDSRC:
            {
            if(!iObjects->iSource)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iSource, NULL);
                }
            break;
            }            
        case   EDEVSOUNDSINK:
            {
            if(!iObjects->iSink)
                {
                error = KErrNotFound;
                }
            else
                {
                gst_bin_add_many(GST_BIN (iObjects->iPipeline),iObjects->iSink, NULL);
                }
            break;
            }
        default:
            error = KErrNotFound;
            break;
        }
    iLog->Log(_L("<<CGStreamerTestClass::AddElementToPipeline Error[%d]"),error);
    return error;
    }

TInt CGStreamerTestClass::SetPipelineState( CStifItemParser& aItem )
    {   
    FTRACE(FPrint(_L("CGStreamerTestClass::SetPipelineState")));
    iLog->Log(_L(">>CGStreamerTestClass::SetPipelineState"));
    TInt element;
    TInt error(KErrNone);
    GstStateChangeReturn ret(GST_STATE_CHANGE_SUCCESS);
    if(!iObjects->iPipeline)
        {
        return KErrNotFound;
        }
    
    aItem.GetNextInt(element);
    ret = gst_element_set_state (iObjects->iPipeline, static_cast<GstState>(element) ); 
    iLog->Log(_L("<<CGStreamerTestClass::SetPipelineState Error[%d] ret[%d]"),error,ret);
    return error;
    }

TInt CGStreamerTestClass::SetCapsInPipeLine( CStifItemParser& aItem )
    {
    TInt error(KErrNone);
    TInt width(16);
    TInt depth(16);
    TInt signedProp(1);
    TInt endianess(1234);
    TInt rate(16000);
    TInt channels(1);    
    TPtrC media;

    TInt value(0);
    FTRACE(FPrint(_L("CGStreamerTestClass::SetCapsInPipeLine")));
    iLog->Log(_L(">>CGStreamerTestClass::SetCapsInPipeLine"));
    
    error = aItem.GetNextString(media);
    HBufC8* mediaString = HBufC8::NewL(media.Length()+1);
    TPtr8 des = mediaString->Des();
    
    if(error == KErrNone)
        {
        des.Copy(media);
        
        if(!des.Compare(defaultMedia()) || !des.Compare(mp3Media()))
            {
            width = 16;
            depth = 16;
            signedProp = 1;
            endianess = 1234;
            rate = 16000;
            channels = 1;            
            }
        else if(!des.Compare(amrMedia()) || !des.Compare(g711Media())
                || !des.Compare(g729Media()) || !des.Compare(iLbcMedia()))
            {
            width = 8;
            depth = 8;
            signedProp = 1;
            endianess = 1234;
            rate = 8000;
            channels = 1;  
            }
        }
    else
        {
        des.Copy(defaultMedia());
        }
    
    if(!aItem.GetNextInt(value))
        {
        width = value;
        }
    if(!aItem.GetNextInt(value))
        {
        depth = value;
        }
    if(!aItem.GetNextInt(value))
        {
        signedProp = value;
        }
    if(!aItem.GetNextInt(value))
        {
        endianess = value;
        }
    if(!aItem.GetNextInt(value))
        {
        rate = value;
        }
    if(!aItem.GetNextInt(value))
        {
        channels = value;
        }
    
    iObjects->iCaps = gst_caps_new_simple ((const char *)des.PtrZ(),
                                "width", G_TYPE_INT, width,
                                "depth", G_TYPE_INT, depth,
                                "signed",G_TYPE_BOOLEAN, signedProp,
                                "endianness",G_TYPE_INT, endianess,
                                "rate", G_TYPE_INT, rate,
                                "channels", G_TYPE_INT, channels, NULL);
    
    delete mediaString;
    iLog->Log(_L("<<CGStreamerTestClass::SetCapsInPipeLine Error[%d]"),error);
    return error;
    }


TInt CGStreamerTestClass::CustomIntfcProp( CStifItemParser& aItem )
    {
    FTRACE(FPrint(_L("CGStreamerTestClass::CustomIntfcProp")));
    iLog->Log(_L(">>CGStreamerTestClass::CustomIntfcProp"));
    TInt element;
    TInt error(KErrNone);
    if(!iObjects->iPipeline)
        {
        return KErrNotReady;
        }

    while((aItem.GetNextInt(element) == KErrNone) && (error == KErrNone))
        {
        iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp element[%d]"),element);
        switch(element)
            {
            case   EG711ENCODER:
                {
                if(!iObjects->iG711EncoderIntfc)
                    {
                    error = KErrNotFound;
                    iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp error[%d]"),error);
                    }
                else
                    {
                    TInt ciProp(0);
                    iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp"));
                    aItem.GetNextInt(ciProp);
                    iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp ciprop[%d]"),ciProp);
                    switch(ciProp)
                        {
                        case EG711SetEncoderMode:
                            {
                            TInt encoderMode(0);
                            error = aItem.GetNextInt(encoderMode);
                            if(error == KErrNone)
                                {
                                error = iObjects->iG711EncoderIntfc->SetEncoderMode(TG711EncodeMode(encoderMode));
                                }
                            }
                            break;
                        case EG711SetVadMode:
                            {
                            TInt vadMode(0);
                            error = aItem.GetNextInt(vadMode);
                            if(!error)
                                {
                                error = iObjects->iG711EncoderIntfc->SetVadMode(vadMode);
                                }
                            }
                            break;
                        case EG711GetVadMode:
                            {
                            TInt vadMode(0);
                            iLog->Log(_L("iG711EncoderIntfc vadMode[%d]"),vadMode);
                            error = iObjects->iG711EncoderIntfc->GetVadMode(&vadMode);
                            iLog->Log(_L("iG711EncoderIntfc vadMode[%d]"),vadMode);
                            }
                            break;
                        default:
                            error = KErrArgument;
                            break;
                        }
                    }
                break;
                }            
            case   EG729ENCODER:
                {
                if(!iObjects->iG729EncoderIntfc)
                    {
                    error = KErrNotFound;
                    iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp error[%d]"),error);
                    }
                else
                    {
                    TInt ciProp(0);
                    iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp"));
                    aItem.GetNextInt(ciProp);
                    iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp ciprop[%d]"),ciProp);
                    switch(ciProp)
                        {
                        case EG729SetVadMode:
                            {
                            TInt vadMode(0);
                            error = aItem.GetNextInt(vadMode);
                            if(!error)
                                {
                                error = iObjects->iG729EncoderIntfc->SetVadMode(vadMode);
                                }
                            }
                            break;
                        case EG729GetVadMode:
                            {
                            TInt vadMode(0);
                            iLog->Log(_L("iG729EncoderIntfc vadMode[%d]"),vadMode);
                            error = iObjects->iG729EncoderIntfc->GetVadMode(&vadMode);
                            iLog->Log(_L("iG729EncoderIntfc vadMode[%d]"),vadMode);
                            }
                            break;
                        default:
                            error = KErrArgument;
                            break;
                        }
                    }
                break;
                }            
            case   EILBCENCODER:
                {
                if(!iObjects->iIlbcEncoderIntfc)
                    {
                    error = KErrNotFound;
                    iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp error[%d]"),error);
                    }
                else
                    {
                    TInt ciProp(0);
                    iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp"));
                    aItem.GetNextInt(ciProp);
                    iLog->Log(_L("CGStreamerTestClass::CustomIntfcProp ciprop[%d]"),ciProp);
                    switch(ciProp)
                        {
                        case EIlbcSetEncoderMode:
                            {
                            TInt encoderMode(0);
                            error = aItem.GetNextInt(encoderMode);
                            if(error == KErrNone)
                                {
                                error = iObjects->iIlbcEncoderIntfc->SetEncoderMode(TIlbcEncodeMode(encoderMode));
                                }
                            }
                            break;                        
                        case EIlbcSetVadMode:
                            {
                            TInt vadMode(0);
                            error = aItem.GetNextInt(vadMode);
                            if(!error)
                                {
                                error = iObjects->iIlbcEncoderIntfc->SetVadMode(vadMode);
                                }
                            }
                            break;
                        case EIlbcGetVadMode:
                            {
                            TInt vadMode(1);
                            iLog->Log(_L("iIlbcEncoderIntfc vadMode[%d]"),vadMode);
                            error = iObjects->iIlbcEncoderIntfc->GetVadMode(&vadMode);
                            iLog->Log(_L("iIlbcEncoderIntfc vadMode[%d]"),vadMode);
                            }
                            break;
                        default:
                            error = KErrArgument;
                            break;
                        }
                    }
                break;
                }            
            case   ESPEECHENCODER:
                {
                if(!iObjects->iSpeechEncoderIntfc)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    TInt ciProp(0);
                    aItem.GetNextInt(ciProp);
                    switch(ciProp)
                        {
                        case ESPENGetSupportedBitrates:
                            {
                            //guint* bitRates[];
                            //error = iSpeechEncoderIntfc->GetSupportedBitrates(&bitRates);
                            }
                            break;
                        case ESPENSetVadMode:
                            {
                            gboolean vadMode(0);
                            error = aItem.GetNextInt(vadMode);
                            if(!error)
                                {
                                error = iObjects->iSpeechEncoderIntfc->SetVadMode(vadMode);
                                }
                            }
                            break;
                        case ESPENGetVadMode:
                            {
                            gboolean vadMode(0);
                            error = iObjects->iSpeechEncoderIntfc->GetVadMode(&vadMode);
                            iLog->Log(_L("iSpeechEncoderIntfc vadMode[%d]"),vadMode);
                            }
                            break;
                        case ESPENSetBitrate:
                            {
                            guint bitRate(0);
                            error = aItem.GetNextInt(bitRate);
                            if(!error)
                                {
                                error = iObjects->iSpeechEncoderIntfc->SetBitrate(bitRate);
                                }
                            }
                            break;
                        case ESPENGetBitrate:
                            {
                            guint bitRate(0);
                            error = iObjects->iSpeechEncoderIntfc->GetBitrate(&bitRate);
                            iLog->Log(_L("iSpeechEncoderIntfc bitRate[%d]"),bitRate);
                            }
                            break;
                        default:
                            error = KErrArgument;
                            break;
                        }
                    }
                break;
                }            
            case   EG711DECODER:
                {
                if(!iObjects->iG711DecoderIntfc)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    TInt ciProp(0);
                    aItem.GetNextInt(ciProp);
                    switch(ciProp)
                        {
                        case EG711SetDecoderMode:
                            {
                            TInt decMode(0);
                             error = aItem.GetNextInt(decMode);
                             if(!error)
                                 {
                                 error = iObjects->iG711DecoderIntfc->SetDecoderMode(TG711DecodeMode(decMode));
                                 }
                            }
                            break;
                        case EG711SetCng:
                            {
                            TInt cng(0);
                             error = aItem.GetNextInt(cng);
                             if(!error)
                                 {
                                 error = iObjects->iG711DecoderIntfc->SetCng(cng);
                                 }
                            }
                            break;
                        case EG711GetCng:
                            {
                            TInt cng(0);
                            error = iObjects->iG711DecoderIntfc->GetCng(&cng);
                            iLog->Log(_L("iG711DecoderIntfc cng[%d]"),cng);
                            }
                            break;
                        case EG711SetPlc:
                            {
                            TInt plc(0);
                            error = aItem.GetNextInt(plc);
                            if(!error)
                                {
                                error = iObjects->iG711DecoderIntfc->SetPlc(plc);
                                }
                            }
                            break;
                        default:
                            error = KErrArgument;
                            break;
                        }
                    }
                break;
                }
            case   EILBCDECODER:
                {
                if(!iObjects->iIlbcDecoderIntfc)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    TInt ciProp(0);
                    aItem.GetNextInt(ciProp);
                    switch(ciProp)
                        {
                        case EIlbcSetDecoderMode:
                            {
                            TInt decMode(0);
                             error = aItem.GetNextInt(decMode);
                             if(!error)
                                 {
                                 error = iObjects->iIlbcDecoderIntfc->SetDecoderMode(TIlbcDecodeMode(decMode));
                                 }
                            }
                            break;
                        case EIlbcSetCng:
                            {
                            TInt cng(0);
                             error = aItem.GetNextInt(cng);
                             if(!error)
                                 {
                                 error = iObjects->iIlbcDecoderIntfc->SetCng(cng);
                                 }
                            }
                            break;
                        case EIlbcGetCng:
                            {
                            TInt cng(0);
                            error = iObjects->iIlbcDecoderIntfc->GetCng(&cng);
                            iLog->Log(_L("iIlbcDecoderIntfc cng[%d]"),cng);
                            }
                            break;
                        default:
                            error = KErrArgument;
                            break;
                        }
                    }
                break;
                }
            case   EG729DECODER:
                {
                if(!iObjects->iG729DecoderIntfc)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    TInt ciProp(0);
                    aItem.GetNextInt(ciProp);
                    switch(ciProp)
                        {
                        case EG729BadLsfNextBuffer:
                            {
                            error = iObjects->iG729DecoderIntfc->BadLsfNextBuffer();
                            }
                            break;
                        default:
                            error = KErrArgument;
                            break;
                        }
                    }
                break;
                }
            case   EERRORCONCEALMENT:
                {
                if(!iObjects->iErrconcealmentIntfc)
                    {
                    error = KErrNotFound;
                    iLog->Log(_L("iErrconcealmentIntfc error[%d]"),error);
                    }
                else
                    {
                    TInt ciProp(0);
                    aItem.GetNextInt(ciProp);
                    switch(ciProp)
                        {
                        case EECConcealErrorForNextBuffer:
                            {
                            error = iObjects->iErrconcealmentIntfc->ConcealErrorForNextBuffer();
                            iLog->Log(_L("iErrconcealmentIntfc ConcealErrorForNextBuffer error[%d]"),error);
                            }
                            break;
                        case EECSetFrameMode:
                            {
                            TInt frmMode(0);
                            error = aItem.GetNextInt(frmMode);
                            if(!error)
                                {
                                error = iObjects->iErrconcealmentIntfc->SetFrameMode(frmMode);
                                iLog->Log(_L("iErrconcealmentIntfc SetFrameMode[%d]"),frmMode);
                                }
                            }
                            break;
                        case EECFrameModeRqrdForEC:
                            {
                            TInt frmModeReq(0);
                            error = iObjects->iErrconcealmentIntfc->FrameModeRqrdForEC(&frmModeReq);
                            iLog->Log(_L("iErrconcealmentIntfc frmModeReq[%d]"),frmModeReq);
                            }
                            break;
                        default:
                            error = KErrArgument;
                            break;
                        }
                    }
                break;
                }
            default:
                error = KErrArgument;
                break;
            }
        }
    iLog->Log(_L("<<CGStreamerTestClass::CustomIntfcProp Error[%d]"),error);
    return error;
    }

TInt CGStreamerTestClass::GetElementsCustomIntfc( CStifItemParser& aItem )
    {
    FTRACE(FPrint(_L("CGStreamerTestClass::GetElementsCustomIntfc")));
    iLog->Log(_L(">>CGStreamerTestClass::GetElementsCustomIntfc"));
    TInt element;
    TInt error(KErrNone);
    if(!iObjects->iPipeline)
        {
        return KErrNotReady;
        }

    while((aItem.GetNextInt(element) == KErrNone) && (error == KErrNone))
        {
        switch(element)
            {
            case   EDEVSOUNDSRC:
                {
                if(!iObjects->iSource)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    TInt customInterface(0);
                    aItem.GetNextInt(customInterface);
                    switch(customInterface)
                        {
                        case EG711ENCODER:
                            {
                            iObjects->iG711EncoderIntfc = GST_G711_ENCODER_GET_IFACE(iObjects->iSource);
                            iLog->Log(_L("iG711EncoderIntfc"));
                            }
                            break;
                        case EG729ENCODER:
                            {
                            iObjects->iG729EncoderIntfc = GST_G729_ENCODER_GET_IFACE(iObjects->iSource);
                            }
                            break;
                        case EILBCENCODER:
                            {
                            iObjects->iIlbcEncoderIntfc = GST_ILBC_ENCODER_GET_IFACE(iObjects->iSource);
                            }
                            break;
                        case ESPEECHENCODER:
                            {
                            iObjects->iSpeechEncoderIntfc = GST_SPEECH_ENCODER_CONFIG_GET_IFACE(iObjects->iSource);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                break;
                }            
            case   EDEVSOUNDSINK:
                {
                if(!iObjects->iSink)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    TInt customInterface(0);
                    aItem.GetNextInt(customInterface);
                    switch(customInterface)
                        {
                        case EG711DECODER:
                            {
                            iObjects->iG711DecoderIntfc = GST_G711_DECODER_GET_IFACE(iObjects->iSink);
                            }
                            break;
                        case EG729DECODER:
                            {
                            iObjects->iG729DecoderIntfc = GST_G729_DECODER_GET_IFACE(iObjects->iSink);
                            }
                            break;                            
                        case EILBCDECODER:
                            {
                            iObjects->iIlbcDecoderIntfc = GST_ILBC_DECODER_GET_IFACE(iObjects->iSink);
                            }
                            break;                            
                        case EERRORCONCEALMENT:
                            {
                            iObjects->iErrconcealmentIntfc = GST_ERROR_CONCEALMENT_GET_IFACE(iObjects->iSink);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                break;
                }
            default:
                error = KErrArgument;
                break;
            }
        }
    iLog->Log(_L("<<CGStreamerTestClass::GetElementsCustomIntfc Error[%d]"),error);
    return error;
    }


TInt CGStreamerTestClass::LinkElementsInPipeline( CStifItemParser& aItem )
    {   
    FTRACE(FPrint(_L("CGStreamerTestClass::LinkElementsInPipeline")));
    iLog->Log(_L(">>CGStreamerTestClass::LinkElementsInPipeline"));
    TInt element;
    TInt error(KErrNone);
    if(!iObjects->iPipeline)
        {
        return KErrNotReady;
        }
    RPointerArray<GstElement> array;
    
    while((aItem.GetNextInt(element) == KErrNone) && (error == KErrNone))
        {
        switch(element)
            {
            case   EFILESOURCE:
                {
                if(!iObjects->iSource)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iSource);
                    }
                break;
                }
            case   EFILESINK:
                {
                if(!iObjects->iSink)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iSink);             
                    }
                break;
                }
            case   EFAKESOURCE:
                {
                if(!iObjects->iSource)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iSource);                             
                    }
                break;
                }
            case   EFAKESINK:
                {
                if(!iObjects->iSink)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iSink);
                    }
                break;
                }
            case   EWAVPARSE:
                {
                if(!iObjects->iWavparse)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iWavparse);
                    }
                break;
                }
            case   EMP3DECODER:
                {
                if(!iObjects->iDecoder)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iDecoder);
                    }
                break;
                }
            case   EWAVEENCODER:
                {
                if(!iObjects->iEncoder)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iEncoder);
                    }
                break;
                }
            case   ERESAMPLER:
                {
                if(!iObjects->iResampler)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iResampler);
                    }
                break;
                }
            case   ECONVERTER:
                {
                if(!iObjects->iConverter)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iConverter);
                    }
                break;
                }
            case   EDEVSOUNDSRC:
                {
                if(!iObjects->iSource)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iSource);
                    }
                break;
                }            
            case   EDEVSOUNDSINK:
                {
                if(!iObjects->iSink)
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    array.Append(iObjects->iSink);
                    }
                break;
                }
            default:
                error = KErrNotFound;
                break;
            }
        }
    
    if(error)
        {
        return error;
        }
    
    gboolean linkOK(0);
    TInt count = array.Count();
    if(count < 2)
        {
        return KErrArgument;
        }
    else
        {
        for(TInt i = 1; i < count ; i++)
            {
            if(iObjects->iCaps)
                {
                linkOK = gst_element_link_filtered(array[i-1],array[i],iObjects->iCaps);
                gst_caps_unref(iObjects->iCaps);
                iObjects->iCaps = NULL;
                }
            else
                {
                linkOK = gst_element_link(array[i-1],array[i]);
                }
            }        
        }
    
    array.Close();
    iLog->Log(_L("<<CGStreamerTestClass::LinkElementsInPipeline Error[%d]"),linkOK);
    return error;
    }


//  End of File
