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
* ============================================================================
*  Name        : RichTextEditorRTE.cpp
*  Part of     : VoIP test application.
*  Description : Utility for outputting formatted text to the display.
*  Version     : %version: 1 %
* ============================================================================
*/

// INCLUDES
#include <barsread.h>   // TResourceReader
#include <eikrted.h>    // CEikRichTextEditor
#include <txtrich.h>    // CRichText
#include <GSTPlayer.rsg>
#include "RichTextEditor.h"


// ----------------------------------------------------------------------------
// CRichTextEditorRTE::NewL
// Symbian constructor.
// ----------------------------------------------------------------------------
//
CRichTextEditorRTE* CRichTextEditorRTE::NewL(const CCoeControl& aView)
    {
    CRichTextEditorRTE* self = CRichTextEditorRTE::NewLC(aView);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CRichTextEditorRTE::NewLC
// Symbian constructor with self pointer pushed into cleanup stack.
// ----------------------------------------------------------------------------
//
CRichTextEditorRTE* CRichTextEditorRTE::NewLC(const CCoeControl& aView)
    {
    CRichTextEditorRTE* self = new (ELeave) CRichTextEditorRTE;
    CleanupStack::PushL(self);
    self->ConstructL(aView);
    return self;
    }

// -----------------------------------------------------------------------------
// CRichTextEditorRTE::ConstructL
// Standard EPOC 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CRichTextEditorRTE::ConstructL(const CCoeControl& aView)
    {
    TResourceReader reader;
    // Construct RichTextEditor from resource
    iCoeEnv->CreateResourceReaderLC(reader, R_RICHTEXTEDITOR_RICH_TEXT_EDITOR);
    SetContainerWindowL(aView);
    ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); //reader
    // Sets that the control has keyboard focus
    SetFocus(ETrue);
    }

// ----------------------------------------------------------------------------
// CRichTextEditorRTE::CRichTextEditorRTE
// constructor
// ----------------------------------------------------------------------------
//
CRichTextEditorRTE::CRichTextEditorRTE(void)
    {
    }

// ----------------------------------------------------------------------------
// CRichTextEditorRTE::AddCarriageReturnL
// Insert one line break at the end of the text.
// ----------------------------------------------------------------------------
//
void CRichTextEditorRTE::AddCarriageReturnL()
    {
    CRichText* text = RichText();
    TInt len = text->DocumentLength();
    text->InsertL(len, CEditableText::ELineBreak);
    }

// ----------------------------------------------------------------------------
// CRichTextEditorRTE::AddTextL
// Draws text using selected color.
// ----------------------------------------------------------------------------
//
void CRichTextEditorRTE::AddTextL(const TDesC& aText, TRgb aRgb)
    {
    CRichText* text = RichText();
    TInt len = text->DocumentLength();

    // Interested in color
    iCharacterFormatMask.SetAttrib(EAttColor);
    // Set it to Black
    iCharacterFormat.iFontPresentation.iTextColor = aRgb;
    text->InsertL(len, aText);
    // Apply formatting
    text->ApplyCharFormatL(iCharacterFormat,
                           iCharacterFormatMask,
                           len,
                           aText.Length());
    AddCarriageReturnL();
    HandleTextChangedL();

    // Scroll pages to always display at the bottom of the screen
    for (TInt i = len + aText.Length(); (i / 50) > 1; i -= 50)
        {
        MoveCursorL(TCursorPosition::EFPageDown, EFalse);
        }
    }

// ----------------------------------------------------------------------------
// TKeyResponse CRichTextEditorRTE::OfferKeyEventL(const TKeyEvent& aKeyEvent,
//      TEventCode aType)
//
// Called by the framework whenever a key event occurs. Handles scrolling
// events.
// ----------------------------------------------------------------------------
TKeyResponse CRichTextEditorRTE::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                                TEventCode aType)
    {
    if (aType == EEventKey)
        {
        if (aKeyEvent.iCode == EKeyDownArrow)
            {
            MoveCursorL (TCursorPosition::EFPageDown, EFalse);

            return EKeyWasConsumed;
            }
        else if (aKeyEvent.iCode == EKeyUpArrow)
            {
            MoveCursorL (TCursorPosition::EFPageUp, EFalse);

            return EKeyWasConsumed;
            }
        else
            {
            return CEikRichTextEditor::OfferKeyEventL(aKeyEvent, aType);
            }
        }

    return EKeyWasNotConsumed;
    }

// End of file
