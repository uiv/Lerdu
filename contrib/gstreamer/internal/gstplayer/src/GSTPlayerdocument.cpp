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
*  Name        : gstplayerdocument.cpp
*  Part of     : gstplayer
*  Interface   : 
*  Description : 
*  Version     : 
*

* ==============================================================================
*/

// INCLUDE FILES
#include "GSTPlayerAppUi.h"
#include "GSTPlayerDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGSTPlayerDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSTPlayerDocument* CGSTPlayerDocument::NewL( CEikApplication&
                                                          aApp )
    {
    CGSTPlayerDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CGSTPlayerDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSTPlayerDocument* CGSTPlayerDocument::NewLC( CEikApplication&
                                                           aApp )
    {
    CGSTPlayerDocument* self =
        new ( ELeave ) CGSTPlayerDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CGSTPlayerDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGSTPlayerDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CGSTPlayerDocument::CGSTPlayerDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGSTPlayerDocument::CGSTPlayerDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CGSTPlayerDocument::~CGSTPlayerDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CGSTPlayerDocument::~CGSTPlayerDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CGSTPlayerDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CGSTPlayerDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave )
                                        CGSTPlayerAppUi ) );
    }

// End of File

