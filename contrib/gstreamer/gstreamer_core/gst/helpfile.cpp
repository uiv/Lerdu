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

#include "helpfile.h" 
#include <f32file.h>
#include <e32std.h> 
#include <pls.h> // For emulator WSD API 
#include <glib.h>

extern "C" char* libgstreamer_get_dll_path(char* dllName)
{
    TUint ch = TUint('A');
    char* fullPath = dllName;
    #ifdef __WINSCW__ 
    char* dllPath = "z:\\sys\\bin\\plugins";
    fullPath = g_strjoin ("\\", dllPath,dllName, NULL);
    #else
    char* dllPath = "sys\\bin";
    
    char* filename;
    TInt i; 
    RFs fs;
    TDriveList driveList;
    //err = fs.Connect();
    if( !(fs.Connect()) )
    {
        //err = fs.DriveList(driveList);
        if( !(fs.DriveList(driveList)) )
        {
            for( i = 0; i< KMaxDrives; i++ )
            {
            
                if( driveList[i] != 0 )
                {
                    char temp[3];
                    temp[0]= char(ch);
                    temp[1]= ':';
                    temp[2]= '\0';
                    
                    filename = g_strjoin ("\\", temp,dllPath,dllName, NULL);
                    if (g_file_test (filename, G_FILE_TEST_EXISTS))
                    {
                        fullPath = filename;
                        break;
                    }
                }
                ch++;
            }
        }
        fs.Close();
    }
    #endif
    return fullPath;
}

