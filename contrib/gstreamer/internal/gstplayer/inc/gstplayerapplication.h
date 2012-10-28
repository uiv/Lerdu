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
*  Name        : gstplayerapplication.h
*  Part of     : gstplayer
*  Interface   : 
*  Description : 
*  Version     : 
*

* ==============================================================================
*/

#ifndef __GSTPLAYERAPPLICATION_H__
#define __GSTPLAYERAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CGSTPlayerApplication application class.
* Provides factory to create concrete document object.
* An instance of CGSTPlayerApplication is the application part of the
* AVKON application framework for the GSTPlayer example application.
*/
class CGSTPlayerApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidGSTPlayerApp).
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CGSTPlayerDocument document object. The returned
        * pointer in not owned by the CGSTPlayerApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __gstplayerAPPLICATION_H__

// End of File

