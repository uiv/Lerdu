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

#ifndef __GST_GLOBAL_H__
#define __GST_GLOBAL_H__

#define GST_CAT_QOS (*_GST_CAT_QOS())
#define	GST_CAT_DEFAULT	(*_GST_CAT_DEFAULT())
#define GST_CAT_EVENT (*_GST_CAT_EVENT())
#define	__gst_debug_enabled	(*gst_debug_enabled())

#endif /* __GST_GLOBAL_H__ */
