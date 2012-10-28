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
*  Name        : gstplayer.pan
*  Part of     : gstplayer
*  Interface   : 
*  Description : 
*  Version     : 
*

* ==============================================================================
*/

#ifndef __GSTPLAYER_PAN__
#define __GSTPLAYER_PAN__

/** GSTPlayer application panic codes */
enum TGSTPlayerPanics
    {
    EGSTPlayerUi = 1
    // add further panics here
    };

inline void Panic(TGSTPlayerPanics aReason)
    {
    _LIT(applicationName,"GSTPlayer");
    User::Panic(applicationName, aReason);
    }

#endif // __gstplayer_PAN__
