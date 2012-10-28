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
*  Name        : RichTextEditor.h
*  Part of     : VoIP test application.
*  Description : Utility printing formatted text to the display.
*  Version     : %version: 1 %
*
* ============================================================================
*/

#ifndef CRICHTEXTEDITOR_H
#define CRICHTEXTEDITOR_H

// INCLUDES
#include <eikrted.h>  // CCEikRichTextEditor
#include <txtfrmat.h> // TCharFormatMask
#include <gdi.h>

// CLASS DECLARATION

/**
*  CRichTextEditorRTE
*  Editor that outputs formatted text to the display.
*/
class CRichTextEditorRTE : public CEikRichTextEditor
    {
public:

    /**
    * NewL()
    *
    * Create a CRichTextEditorRTE object, which will draw
    * itself to aRect
    */
    static CRichTextEditorRTE* NewL(const CCoeControl& aView);

    /**
    * NewLC()
    *
    */
    static CRichTextEditorRTE* NewLC(const CCoeControl& aView);


public: // from CoeControl

    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

public: // members

    /**
    * AddCarriageReturnL ()
    *
    * discussion Add one carriage return
    *
    */
    void AddCarriageReturnL ();

    /**
    * AddTextL (const TDesC& aText)
    *
    * discussion Print text on the screen.
    * param aText text to be displayed
    *
    */
    void AddTextL (const TDesC& aText, TRgb aRgb = KRgbBlack);

private: // Basic two-phase EPOC constructors

    void ConstructL(const CCoeControl& aView);

    /**
    * CRichTextEditorRTE()
    *
    * Default contructor.
    *
    */
    CRichTextEditorRTE();

private: //data

    // formatting options for the RichText that will be displayed
    TCharFormatMask iCharacterFormatMask;
    TCharFormat iCharacterFormat;
    };

#endif  // #ifndef CRICHTEXTEDITOR_H

// End of file

