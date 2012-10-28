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


#include "libgstreamer_wsd_solution.h" 

#ifdef EMULATOR

#include <e32std.h> 
#include <pls.h> // For emulator WSD API 
#include <glib.h>

const TUid KLibGStreamerUid3 = {0x0AB229CB};     // This is the UID of the library

extern "C" EXPORT_C	struct libgstreamer_global_struct *libgstreamer_ImpurePtr()
{
	  
#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct libgstreamer_global_struct* p  = Pls<struct libgstreamer_global_struct>(KLibGStreamerUid3, &libgstreamer_Init);
  	return p;

#else

	return NULL;	

#endif

	  
	  
}

int libgstreamer_Init(libgstreamer_global_struct *g)
{
#if defined(__WINSCW__) || defined(__WINS__)
	if(g)
	{
//#if GST_TESTCASE_COMPILATION 
    	g->GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g) = NULL;
    	g->GET_GSTREAMER_WSD_VAR_NAME(check_mutex,gstcheck,g) = NULL;
    	g->GET_GSTREAMER_WSD_VAR_NAME(check_cond,gstcheck,g) = NULL;    	
 	
    	g->GET_GSTREAMER_WSD_VAR_NAME(fp_std_log,std_log_result,g) = NULL;
    	g->GET_GSTREAMER_WSD_VAR_NAME(assert_failed,std_log_result,g) = 0;
    	g->GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g) = FALSE;
			g->GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g) = FALSE;
			g->GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g) = FALSE;
			
		g->GET_GSTREAMER_WSD_VAR_NAME(thread_list,gstcheck,g) = NULL;
//#endif  /* GST_TESTCASE_COMPILATION */		
	

		return 0;
	  }
	  return 1;
#else

	return 1;

#endif		  
}

extern "C" EXPORT_C void _gstreamer_wsd_reset()
{
	struct libgstreamer_global_struct* p  = Pls<struct libgstreamer_global_struct>(KLibGStreamerUid3, &libgstreamer_Init);	
	if(p)
	{
		memset(p,0,sizeof(struct libgstreamer_global_struct));
		libgstreamer_Init(p);
	}
}

#endif // EMULATOR


