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
*  Name        : gstplayerdocument.h
*  Part of     : gstplayer
*  Interface   : 
*  Description : 
*  Version     : 
*

* ==============================================================================
*/

#ifndef __GSTPLAYERDOCUMENT_H__
#define __GSTPLAYERDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CGSTPlayerAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CGSTPlayerDocument application class.
* An instance of class CGSTPlayerDocument is the Document part of the
* AVKON application framework for the GSTPlayer example application.
*/
class CGSTPlayerDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CGSTPlayerDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CGSTPlayerDocument.
        */
        static CGSTPlayerDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CGSTPlayerDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CGSTPlayerDocument.
        */
        static CGSTPlayerDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CGSTPlayerDocument
        * Virtual Destructor.
        */
        virtual ~CGSTPlayerDocument();

    public: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CGSTPlayerAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CGSTPlayerDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CGSTPlayerDocument( CEikApplication& aApp );

    };

#endif // __GSTPLAYERDOCUMENT_H__

// End of File

