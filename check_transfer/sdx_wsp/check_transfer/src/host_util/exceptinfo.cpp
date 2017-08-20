
#include "exceptinfo.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <getopt.h>
#endif

#include <fcntl.h>
#include <signal.h>

//-----------------------------------------------------------------------------

using namespace std;

//-----------------------------------------------------------------------------

except_info_t except_info(const char *fmt, ...)
{
    except_info_t err;
    va_list argptr;
    va_start(argptr, fmt);
    char msg[256];
    vsprintf(msg, fmt, argptr);
    err.info = msg;
    return err;
}

//-----------------------------------------------------------------------------
