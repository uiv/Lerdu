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
*  Name        : GSTPlayerappview.cpp
*  Part of     : GSTPlayer
*  Interface   : 
*  Description : 
*  Version     : 
*

* ==============================================================================
*/

// INCLUDE FILES
#include <coemain.h>
#include <stdlib.h>
#include <gst/gst.h>
#include "GSTPlayerAppView.h"
#include "GlibEventHandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGSTPlayerAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSTPlayerAppView* CGSTPlayerAppView::NewL( const TRect& aRect )
    {
    CGSTPlayerAppView* self = CGSTPlayerAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CGSTPlayerAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSTPlayerAppView* CGSTPlayerAppView::NewLC( const TRect& aRect )
    {
    CGSTPlayerAppView* self = new ( ELeave ) CGSTPlayerAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CGSTPlayerAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGSTPlayerAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();
    iRte = CRichTextEditorRTE::NewL(*this);
    // Set the windows size
    SetRect( aRect );    
    // Activate the window, which makes it ready to be drawn
    ActivateL();   

    }
 
// -----------------------------------------------------------------------------
// CGSTPlayerAppView::CGSTPlayerAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGSTPlayerAppView::CGSTPlayerAppView()
    {
    // No implementation required
  	gst_init(NULL,NULL);  
  	
    iCGlibEventHandler = CGlibEventHandler::NewL();
	iCGlibEventHandler->Start();
    }


// -----------------------------------------------------------------------------
// CGSTPlayerAppView::~CGSTPlayerAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CGSTPlayerAppView::~CGSTPlayerAppView()
    {
    // No implementation required
    
    delete iRte;
    
    if(iCGlibEventHandler)
    {
    	iCGlibEventHandler->Stop();
        	delete iCGlibEventHandler;
    }
    }


// -----------------------------------------------------------------------------
// CGSTPlayerAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CGSTPlayerAppView::Draw( const TRect& /*aRect*/ ) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect drawRect( Rect());

    // Clears the screen
    gc.Clear( drawRect );
    
  	}
void CGSTPlayerAppView::DrawText(const TDesC& aText, const TRgb aRgb)
    {
    TRAPD(err, iRte->AddTextL(aText, aRgb));
    if (err != KErrNone)
        {
        ; // don't care but makes compiler happy
        }
    }


// -----------------------------------------------------------------------------
// CGSTPlayerAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CGSTPlayerAppView::SizeChanged()
    {  
    iRte->SetExtent(TPoint(0,0), Window().Size());
   DrawNow();
    }
// End of File

// ----------------------------------------------------------------------------
// CGSTPlayerAppView::CountComponentControls
// Called by the framework in compound controls
// ----------------------------------------------------------------------------
TInt CGSTPlayerAppView::CountComponentControls() const
    {
    return 1; // return number of controls inside this container
    }


// ----------------------------------------------------------------------------
// CGSTPlayerAppView::ComponentControl
// Called by the framework in compound controls
// ----------------------------------------------------------------------------
CCoeControl* CGSTPlayerAppView::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
    case 0:
        return iRte;
    default:
        return NULL;
        }
    }


