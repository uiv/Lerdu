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


#ifndef DEBUGTRACEMACROS_H
#define DEBUGTRACEMACROS_H

#define RET_ERR_IF_ERR(err) if ( err != KErrNone) return err;

#ifdef _DEBUG
#include "e32debug.h"

#define TRACE_PRN_FN_ENT        RDebug::Printf("%s>ENTER", __PRETTY_FUNCTION__)
#define TRACE_PRN_FN_EXT        RDebug::Printf("%s>EXIT", __PRETTY_FUNCTION__)
#define TRACE_PRN_IF_ERR(err) if (err != KErrNone) RDebug::Printf( "%s>BREAK[%d]", __PRETTY_FUNCTION__, err )

#define TRACE_PRN_0(str)              RDebug::Print(str, this)
#define TRACE_PRN_1(str, val1)        RDebug::Print(str, this, val1)
#define TRACE_PRN_2(str, val1, val2)  RDebug::Print(str, this, val1, val2)

#define TRACE_PRN_N(str)              RDebug::Print(str)
#define TRACE_PRN_N1(str, val1)       RDebug::Print(str, val1)
#define TRACE_PRN_N2(str, val1, val2) RDebug::Print(str, val1, val2)

#else

#define TRACE_PRN_FN_ENT
#define TRACE_PRN_FN_EXT
#define TRACE_PRN_IF_ERR(err)
#define TRACE_PRN_0(str)
#define TRACE_PRN_1(str, val1)
#define TRACE_PRN_2(str, val1, val2)
#define TRACE_PRN_N(str)
#define TRACE_PRN_N1(str, val1)
#define TRACE_PRN_N2(str, val1, val2)
#endif //_DEBUG

#endif // DEBUGTRACEMACROS_H

// End of file
