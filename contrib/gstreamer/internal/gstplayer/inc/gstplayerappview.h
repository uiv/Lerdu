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
*  Name        : gstplayerappview.h
*  Part of     : gstplayer
*  Interface   : 
*  Description : 
*  Version     : 
*

* ==============================================================================
*/

#ifndef __GSTPLAYERAPPVIEW_H__
#define __GSTPLAYERAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>
#include "RichTextEditor.h"

// CLASS DECLARATION
class CGSTPlayerAppView : public CCoeControl
    {
    public: // New methods

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CGSTPlayerAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return a pointer to the created instance of CGSTPlayerAppView.
        */
        static CGSTPlayerAppView* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CGSTPlayerAppView object, which will draw itself
        * to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CGSTPlayerAppView.
        */
        static CGSTPlayerAppView* NewLC( const TRect& aRect );

        /**
        * ~CGSTPlayerAppView
        * Virtual Destructor.
        */
        virtual ~CGSTPlayerAppView();

    public:  // Functions from base classes

        /**
        * From CCoeControl, Draw
        * Draw this CGSTPlayerAppView to the screen.
        * @param aRect the rectangle of this view that needs updating
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CoeControl, SizeChanged.
        * Called by framework when the view size is changed.
        */
        virtual void SizeChanged();
        
        void DrawText(const TDesC& aText, const TRgb aRgb = KRgbBlack);

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        * Perform the second phase construction of a
        * CGSTPlayerAppView object.
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL(const TRect& aRect);
        

        /**
        * CGSTPlayerAppView.
        * C++ default constructor.
        */
        CGSTPlayerAppView();
        
        /**
         * From CoeControl,CountComponentControls.
         */
         TInt CountComponentControls() const;

         /**
         * From CCoeControl,ComponentControl.
         */
         CCoeControl* ComponentControl(TInt aIndex) const;
        
        CRichTextEditorRTE* iRte;

    };

#endif // __GSTPLAYERAPPVIEW_H__

// End of File

