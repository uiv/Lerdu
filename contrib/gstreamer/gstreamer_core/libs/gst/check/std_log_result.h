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


#ifndef _STD_LOG_FILE_H__
#define _STD_LOG_FILE_H__

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#ifdef __SYMBIAN32__
#include "libgstreamer_wsd_solution.h" 

#ifndef EMULATOR
#define assert_failed *(get_assert_failed())
#define fp_std_log *(get_fp_std_log())
#define xmlfile *(get_xmlfile())
#endif
//#define LOG_FILE "c:\\logs\\std_test_log.txt"
#define LOG_DIR "c:\\logs\\"
#define LOG_FILE_EXT "xml"
#else
#define LOG_DIR ""
#define LOG_FILE_EXT "xml"
#define LOG_FILE "std_test_log.txt"
#endif

//FILE *fp_std_log=NULL;
#if EMULATOR
static	GET_GLOBAL_VAR_FROM_TLS(fp_std_log,std_log_result,FILE *)
#define fp_std_log (*GET_GSTREAMER_WSD_VAR_NAME(fp_std_log,std_log_result,g)())
#else 
extern FILE *fp_std_log;
#endif

//int assert_failed = 0;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(assert_failed,std_log_result,int)
#define assert_failed (*GET_GSTREAMER_WSD_VAR_NAME(assert_failed,std_log_result,g)())
#else 
extern int assert_failed;
#endif

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(xmlfile,std_log_result,char*)
#define xmlfile (*GET_GSTREAMER_WSD_VAR_NAME(xmlfile,std_log_result,g)())
#else 
extern char* xmlfile;
#endif

#ifdef __SYMBIAN32__
EXPORT_C
#endif
int gnutest = 1;

# define VERIFY(fn) gnutest &= (fn)

#ifdef __SYMBIAN32__
EXPORT_C
#endif
void std_log(const char *filename,const int lineno,const char* aformat,...)
{
	va_list va;
	if(fp_std_log==NULL)
	{
		fp_std_log = fopen(LOG_FILE,"a");
	}
	
	va_start(va,aformat);    
    {
		fprintf(fp_std_log,"%s - [%d] : ",filename,lineno);
		vfprintf(fp_std_log,aformat,va);
		fprintf(fp_std_log,"\n");
		fflush(fp_std_log);
	}
	va_end(va);
}

#ifdef __SYMBIAN32__
EXPORT_C
#endif
void init_log_file()
{
	if(fp_std_log == NULL)
	{
		fp_std_log = fopen(LOG_FILE, "a");
	}
}

#ifdef __SYMBIAN32__
EXPORT_C
#endif
void close_log_file()
{
   fclose(fp_std_log);
   fp_std_log = NULL;
}

// This function is used to generate the xml file used bt ATS
#ifdef __SYMBIAN32__
EXPORT_C
#endif
void testResultXml(char *filename)
{
	char time_buf[50];

	char result[10];

	char xmlfilename[256];

	time_t t = time(NULL);

	struct tm *tm1 = localtime(&t);

	char *atsinitmsg        =   "<test-report>\n\t<test-batch>";

	char *atsbatchinit1     =   \
								"\n\t\t<batch-init>\
								\n\t\t\t<description></description>\
								\n\t\t\t<date>";
	char *atsbatchinit2 =   "</date>\
							 \n\t\t\t<factory>NA</factory>\
							 \n\t\t\t<component>\
							 \n\t\t\t\t<name>NA</name>\
							 \n\t\t\t\t<version>NA</version>\
							 \n\t\t\t</component>\
							 \n\t\t</batch-init>";

	char *atsbatchresult=   \
							"\n\t\t<batch-result>\
							\n\t\t\t<run-time>00:00:00</run-time>\
							\n\t\t</batch-result>";

	char *atsclosemsg       =   \
								"\n\t</test-batch>\
								\n</test-report>\n ";

	char *atstestinit       =       "\n\t\t<test-case time-stamp=\"00:00:00\">";


	char *atscaseinit1      =       \
									"\n\t\t\t<case-init>\
									\n\t\t\t\t<version></version>\
									\n\t\t\t\t<id>";

	char *atscaseinit2 =    "</id>\
							 \n\t\t\t\t<expected-result description=\"\">0</expected-result>\
							 \n\t\t\t</case-init>";

	char *atscaseresult1=   \
							"\n\t\t\t<case-result status=\"";

	char *atscaseresult2=   "\">\
							 \n\t\t\t\t<actual-result>0</actual-result>\
							 \n\t\t\t\t<run-time>00:00:00</run-time>\
							 \n\t\t\t</case-result>";

	char *atstestclose      =       "\n\t\t</test-case>";

	// create the xml file name
	FILE *fp_result;
	sprintf(xmlfilename, "%s%s.%s", LOG_DIR, xmlfile, LOG_FILE_EXT);
	strftime(time_buf,50,"%c",tm1);

	if(assert_failed )
		strcpy(result,"FAILED");
	else
		strcpy(result,"PASSED");

	fp_result = fopen(xmlfilename,"w");

	if(fp_result)
	{
		fprintf(fp_result,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s",atsinitmsg,atsbatchinit1,time_buf,atsbatchinit2,atstestinit,
				atscaseinit1,xmlfile,atscaseinit2,atscaseresult1,result,atscaseresult2,
				atstestclose,atsbatchresult,atsclosemsg);

		fclose(fp_result);
	}
}

#endif

