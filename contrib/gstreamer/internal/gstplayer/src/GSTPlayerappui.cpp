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
/*
* ==============================================================================
*  Name        : gstplayerappui.cpp
*  Part of     : gstplayer
*  Interface   : 
*  Description : 
*  Version     : 
*

* ==============================================================================
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <GSTPlayer.rsg>
#include <CAknMemorySelectionDialog.h>
#include <f32file.h>
#include <s32file.h>
#include <PathInfo.h>
#include <gst/gst.h>

#include "GSTPlayer.pan"
#include "GSTPlayerAppUi.h"
#include "GSTPlayerAppView.h"
#include "GSTPlayer.hrh"

#include "GlibEventHandler.h"
#include "gstreamer.h"

TFileName fileName;

_LIT( KHelloFileName, "\\private\\A000017F\\Hello.txt" );
_LIT( KHelloText, "HELLO WORLD!");

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CGSTPlayerAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGSTPlayerAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL(CAknAppUi::EAknEnableSkin);
    // Here the Hello.txt file can be created, if it is not copied automatically.
    /*
     RFs fsSession;
     User::LeaveIfError(fsSession.Connect());            

     RFile file;
     
     // Create a file to write the text to       
     if ( file.Replace(fsSession, KHelloFileName, EFileWrite ) != KErrNone )
     {
     return;
     }
     CleanupClosePushL( file );            
     
     RFileWriteStream outputFileStream( file );
     CleanupClosePushL( outputFileStream );
     outputFileStream << KHelloText;

     CleanupStack::PopAndDestroy(2); // file, outputFileStream

     fsSession.Close();	
     */

    // Create view object
    iAppView = CGSTPlayerAppView::NewL(ClientRect() );

    iGstView = iAppView;

    }
// -----------------------------------------------------------------------------
// CGSTPlayerAppUi::CGSTPlayerAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGSTPlayerAppUi::CGSTPlayerAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CGSTPlayerAppUi::~CGSTPlayerAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CGSTPlayerAppUi::~CGSTPlayerAppUi()
    {

    if (iAppView)
        {
        delete iAppView;
        iAppView = NULL;
        }

    }

// -----------------------------------------------------------------------------
// CGSTPlayerAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CGSTPlayerAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                gst_object_unref(GST_OBJECT (pipeline));

                }
            Exit();
            break;

        case EGSTPlayerNewSong:
            {
            /*TDes& aPath;
             TBool aMemoryCardStorageAllowed;*/
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                gst_object_unref(GST_OBJECT (pipeline));

                }
            CAknMemorySelectionDialog* memoryDialog =
                    CAknMemorySelectionDialog::NewL(ECFDDialogTypeSave, 
                    R_MUSICSHOP_MEMORY_SELECTION_DIALOG, EFalse);
            CleanupStack::PushL(memoryDialog);

            CAknMemorySelectionDialog::TMemory
                    mem(CAknMemorySelectionDialog::EPhoneMemory);
            TFileName ignore;
            //TFileName path;
            if (! (memoryDialog->ExecuteL(mem, &fileName, &ignore) ))
                {
                // User dnt select the memory location.
                //	retVal = EFalse;
                }
            else
                {
                //aPath.Copy( path );
                }
            CleanupStack::PopAndDestroy(); // memoryDialog        


            //if user has pressed cancel button, then just return
            if (!ShowDirListL(fileName))
                {
                return;
                } //end if
            if (GstreamerNew(fileName) == -1)
                {
                // Load a string from the resource file and display it
                HBufC* textResource =
                        StringLoader::LoadLC( R_HEWB_COMMAND1_TEXT);
                CAknInformationNote* informationNote;

                informationNote = new ( ELeave ) CAknInformationNote;

                // Show the information Note with
                // textResource loaded with StringLoader.
                informationNote->ExecuteLD( *textResource);

                // Pop HBuf from CleanUpStack and Destroy it.
                CleanupStack::PopAndDestroy(textResource);

                }

            }
            break;
        case EGSTPlayerPlay:
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                //gst_object_unref (GST_OBJECT (pipeline));  			 	

                }
            if (GstreamerNew(fileName) == -1)
                {
                // Load a string from the resource file and display it
                HBufC* textResource =
                        StringLoader::LoadLC( R_HEWB_COMMAND2_TEXT);
                CAknInformationNote* informationNote;

                informationNote = new ( ELeave ) CAknInformationNote;

                // Show the information Note with
                // textResource loaded with StringLoader.
                informationNote->ExecuteLD( *textResource);

                // Pop HBuf from CleanUpStack and Destroy it.
                CleanupStack::PopAndDestroy(textResource);

                }

            break;
        case EGSTPlayerStop:
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                //gst_object_unref (GST_OBJECT (pipeline));  			 	

                }
            break;

        case EGSTPlayerRecordWav:
            {
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                //gst_object_unref (GST_OBJECT (pipeline));                 

                }
            gst_record_wav();
            }
            break;

        case EGSTPlayerRecordRaw:
            {
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                //gst_object_unref (GST_OBJECT (pipeline));                 

                }
            gst_record_raw();
            }
            break;
        
        case EGSTPlayerRecordAmr:
            {
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                //gst_object_unref (GST_OBJECT (pipeline));                 

                }
            gst_record_amr();
            }
            break;

        case EGSTPlayerRecordG711:
            {
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                //gst_object_unref (GST_OBJECT (pipeline));                 

                }
            gst_record_g711();
            }
            break;
        case EGSTPlayerRecordG729:
            {
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                //gst_object_unref (GST_OBJECT (pipeline));                 

                }
            gst_record_g729();
            }
            break;
        case EGSTPlayerRecordIlbc:
            {
            if (pipeline!=NULL && pipeline ->current_state
                    == GST_STATE_PLAYING)
                {

                gst_element_set_state(pipeline, GST_STATE_NULL);
                //gst_object_unref (GST_OBJECT (pipeline));                 

                }
            gst_record_ilbc();
            }
            break;
            
        case EGSTPlayerCurrentVolume:
            gst_current_volume();
            break;
        case EGSTPlayerMaxVolume:
            gst_max_volume();
            break;
        case EGSTPlayerVolumeUp:
            gst_volume_up();
            break;
        case EGSTPlayerVolumeDown:
            gst_volume_down();
            break;
        case EGSTPlayerLeftBalance:
        case EGSTPlayerRightBalance:    
              gst_balance();
              break;
        case EGSTPlayerCurrentGain:
             gst_current_gain();
             break;
         case EGSTPlayerMaxGain:
             gst_max_gain();
             break;
         case EGSTPlayerGainUp:
             gst_gain_up();
             break;
         case EGSTPlayerGainDown:
             gst_gain_down();
             break;      
        case EGSTPlayerSamplesPlayed:
            samplesplayed();
            break;
        case EGSTPlayerSamplesRecorded:
             samplesrecorded();
             break;    
        case EGSTPlayerSinkPadCaps:
            getsinkpadcaps();
            break;
        case EGSTPlayerSrcPadCaps:
            getsrcpadcaps();
            break;
        case EGSTPlayerNegotiatedSinkCaps:
            negotiatedsinkcaps();
            break;
        case EGSTPlayerNegotiatedSrcCaps:
             negotiatedsrccaps();
             break;    
        default:
            Panic(EGSTPlayerUi);
            break;
        }
    }
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CGSTPlayerAppUi::HandleStatusPaneSizeChange()
    {
    iAppView->SetRect(ClientRect() );

    }

TBool CGSTPlayerAppUi::ShowDirListL(TFileName &filePath)
    {

    _LIT(KDialogTitle, "Select File");

    TBool ret = CAknFileSelectionDialog::RunDlgLD(filePath, // on return, contains the selected file's name
            PathInfo::PhoneMemoryRootPath(), // default root path for browsing
            KDialogTitle, // Dialog's title
            0 // Pointer to class implementing 
            // MAknFileSelectionObserver. OkToExitL is called
            // when user has selected an file.
            );

    return ret;

    }

void CGSTPlayerAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
  {  
    
#ifdef __WINSCW__    
     if (aResourceId==R_RECORD_CMD )
      {        
            aMenuPane->SetItemDimmed(EGSTPlayerRecordAmr, ETrue);
            aMenuPane->SetItemDimmed(EGSTPlayerRecordG711, ETrue);
            aMenuPane->SetItemDimmed(EGSTPlayerRecordG729, ETrue);
            aMenuPane->SetItemDimmed(EGSTPlayerRecordIlbc, ETrue);

      }    
#endif     
  }

void ShowNoteL()
    {
    // Load a string from the resource file and display it
    HBufC* textResource = StringLoader::LoadLC( R_HEWB_COMMAND1_TEXT);
    CAknInformationNote* informationNote;

    informationNote = new ( ELeave ) CAknInformationNote;

    // Show the information Note with
    // textResource loaded with StringLoader.
    informationNote->ExecuteLD( *textResource);

    // Pop HBuf from CleanUpStack and Destroy it.
    CleanupStack::PopAndDestroy(textResource);
    }
// End of File

