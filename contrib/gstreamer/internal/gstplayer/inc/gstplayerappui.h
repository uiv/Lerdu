
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
*  Name        : gstplayerappui.h
*  Part of     : gstplayer
*  Interface   : 
*  Description : 
*  Version     : 
*

* ==============================================================================
*/

#ifndef __GSTPLAYERAPPUI_H__
#define __GSTPLAYERAPPUI_H__

// INCLUDES
#include <aknappui.h>
#include <CAknFileSelectionDialog.h>
#include <aknselectionlist.h> 


// FORWARD DECLARATIONS
class CGSTPlayerAppView;


// CLASS DECLARATION
/**
* CGSTPlayerAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CGSTPlayerAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CGSTPlayerAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CGSTPlayerAppUi();

        /**
        * ~CGSTPlayerAppUi.
        * Virtual Destructor.
        */
        virtual ~CGSTPlayerAppUi();

    private:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

        /**
        *  HandleStatusPaneSizeChange.
        *  Called by the framework when the application status pane
 		*  size is changed.
        */

		void HandleStatusPaneSizeChange();
		TBool ShowDirListL(TFileName &filePath); 
    protected:		
		void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        
    private: // Data

        /**
        * The application view
        * Owned by CGSTPlayerAppUi
        */
        CGSTPlayerAppView* iAppView;       
        
    };

#endif // __gstplayerAPPUI_H__

// End of File

