#include <stdio.h>
#include <glib.h>
#include "check.h"

#include "libgstreamer_wsd_solution.h"


#if !EMULATOR
#ifdef __SYMBIAN32__
EXPORT_C
#endif
int failed=0;
#ifdef __SYMBIAN32__
EXPORT_C
#endif
FILE* fp_std_log_t=NULL;
#ifdef __SYMBIAN32__
EXPORT_C
#endif
char* xmlfile_t=NULL;
#define assert_failed *(get_assert_failed())
#define fp_std_log *(get_fp_std_log())
#define xmlfile *(get_xmlfile())
#endif

#define LOG_FILE "c:\\logs\\common_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#if !EMULATOR
#ifdef __SYMBIAN32__
EXPORT_C
#endif
int* get_assert_failed()
{
	return &failed;	
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

FILE** get_fp_std_log()
{
	return &fp_std_log_t;	
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

char** get_xmlfile()
{
    return &xmlfile_t;   
}
#endif

#ifdef __SYMBIAN32__
EXPORT_C
#endif
void send_failure_info(const char* msg, const char* file, int line)
{
       
    std_log(LOG_FILENAME_LINE, "FILE : %s, LINE : %d, MSG : %s", file, line, msg);
    assert_failed = 1;
    testResultXml("dummy");
    close_log_file();
    exit(1);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void _fail_unless (int result, const char *file,
                   int line, const char *expr, ...)
{
  const char *msg;
    
  
  if (!result) {
    va_list ap;
    char buf[BUFSIZ];
    
    va_start(ap,expr);
    msg = (const char*)va_arg(ap, char *);
    if (msg == NULL)
      msg = expr;
    vsnprintf(buf, BUFSIZ, msg, ap);
    va_end(ap);
    send_failure_info (buf, file, line);
    
  }
}
