

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <time.h>
#include <sys/ioctl.h>

//-----------------------------------------------------------------------------

void IPC_delay(int ms)
{
    struct timeval tv = {0, 0};
    tv.tv_usec = 1000*ms;

    select(0,NULL,NULL,NULL,&tv);
}

//-----------------------------------------------------------------------------

int IPC_getch(void)
{
    int ch;

    while(1)
    {
        size_t size = 0;
    if (ioctl(STDIN_FILENO, FIONREAD, &size) == -1)
            size = 0;
        if(size)
    {
        ch = getchar();
        //read(0, &ch, 1);
        //DEBUG_PRINT("%s(): ch = %d\n", __FUNCTION__, ch );
        break;
    }
    }
    return ch;
}

//-----------------------------------------------------------------------------

 int IPC_getche(void)
{
    return IPC_getch();
}

//-----------------------------------------------------------------------------

int IPC_kbhit(void)
{
    size_t size = 0;
    if (ioctl(STDIN_FILENO, FIONREAD, &size) == -1)
        return 0;

    //DEBUG_PRINT("%s(): size = %d\n", __FUNCTION__, size );

    return size;
}

long IPC_getTickCount()
{
   return time(NULL) * 1000;
}
