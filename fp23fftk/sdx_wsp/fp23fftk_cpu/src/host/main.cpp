



#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#include "ipc.h"
#include "exceptinfo.h"
#include "table_engine_console.h"

#include "tf_device.h"
#include "parse_cmd.h"

#include "tf_test.h"

static volatile int exit_flag = 0;

void signal_handler(int signo)
{
    exit_flag = 1;
}

/**
 *
 * 	\brief	Start point for host application
 *
 * 	\param	argc 	Number of arguments
 * 	\parma	argv	Pointer of argumnts
 *
 * 	Arguments:
 *
 *  main():
 *
 *  -mode   <mode>          : 1 - check input, 2 - check output, 3 - check input and output
 * 	-table	<flag_show>	  	: 1 - show table, 0 -	do not show table
 * 	-time   <time> 		  	: execution time [s],
 *
 *
 *  TF_Device:
 *
 *	-file 	<path>          : fullpath for xclbin, default "../binary_container_1.xclbin"
 *
 *  TF_CheckTransferIn & TF_CheckTransferOut:
 *
 *	-size	<n>             : size block of kilobytes, default 64
 *	-metric <n>             : 0 - binary:  1MB=2^10=1024*1024=1048576 bytes,
 *
 *
 */

int main(int argc, char **argv)
{
    int X = 0;
    int Y = 0;
    int tableMode;
    int timeMode;
    long	testStartTime;
    int		testMode;		   // 1 - input, 2 - output, 3 - input and output

    const char *headers[] = {
        "TIME", "BLOCK_WR", "BLOCK_RD", "BLOCK_OK", "BLOCK_ERROR", "SPD_CURRENT", "SPD_AVR",
    };

    TableEngine  *pTable = new TableEngineConsole();



    tableMode = GetFromCommnadLine( argc, argv, "-table", 0 );
    timeMode  = GetFromCommnadLine( argc, argv, "-time", 10 );
    testMode  = GetFromCommnadLine( argc, argv, "-mode",  2 );


    if( tableMode )
    	pTable->CreateTable(headers, sizeof(headers)/sizeof(headers[0]), 0);

    printf( "\n\n\n\n"); //FIXME - it is need for output under table

    signal(SIGINT, signal_handler);

    TF_Device	device( argc, argv );

    TF_Test	*pTest[2];
    int		testCnt=0;

    try {

#if 0
    	if( testMode & 2 )
    	{
          TF_CheckTransferOut *pTestOut = new TF_CheckTransferOut( pTable, &device, argc, argv );
          pTest[testCnt++]=pTestOut;
    	}

    	if( testMode & 1 )
    	{
    	  TF_CheckTransferIn *pTestIn = new TF_CheckTransferIn( pTable, &device, argc, argv );
          pTest[testCnt++]=pTestIn;
    	}
#endif


#if 0   // parallel executing prepare functions
        for( int ii=0; ; ii++)
        {
        	int flagExit=1;
        	for( int jj=0; jj<testCnt; jj++)
        	{
        		flagExit &=pTest[jj]->Prepare(ii);
        	}
			if( flagExit )
				break;
		}
#else	// sequence executing prepare functions


    	for( int jj=0; jj<testCnt; jj++)
    	{
        	int flagExit=1;
            for( int ii=0; ; ii++)
            {
            	flagExit=pTest[jj]->Prepare(ii);
            	if( flagExit )
            		break;
            }
    	}
#endif

        if( tableMode )
        	pTable->GetConsolePos(X,Y);

    	for( int jj=0; jj<testCnt; jj++ )
    	{
    		pTest[jj]->Start();
            IPC_delay(100); // FIXME

    	}

#if defined(IS_STEP_MAIN_THREAD)
        IPC_TIMEVAL start_time;
        IPC_TIMEVAL curr_time;
#endif


        testStartTime = IPC_getTickCount();

        int count = 0;
        while(1) {
#if defined(IS_STEP_MAIN_THREAD)
            IPC_getTime(&start_time);

            for(;;) {
				for( int jj=0; jj<testCnt; jj++ )
				{
					pTest[jj]->StepMainThread();
				}
                IPC_getTime(&curr_time);
                if(IPC_getDiffTime(&start_time, &curr_time) > 100) {
                    break;
                }
                IPC_delay(10);
            }
#else
            IPC_delay(100);
#endif

				long currentTime = IPC_getTickCount();
				if( timeMode>0 )
				{
					if( (currentTime - testStartTime) >= timeMode*1000 )
							exit_flag=2;
				}

                if( exit_flag )
                {
    				for( int jj=0; jj<testCnt; jj++ )
    				{
    					pTest[jj]->Stop();
    				}
                }

                int flagExit=1;
				for( int jj=0; jj<testCnt; jj++ )
				{
					flagExit &= pTest[jj]->isComplete();
				}

				if( flagExit )
					break;


				 if( tableMode )
				 {
					for( int jj=0; jj<testCnt; jj++ )
					{
					 pTest[jj]->StepTable();
					}
				 }

            ++count;

            if( IPC_kbhit() )
            {
            	// \todo - don't work
                int key = IPC_getch();
                if( 27==key )
                    break;
            }
		}

        if( tableMode )
        	pTable->SetConsolePos(X, Y+1);


		for( int jj=0; jj<testCnt; jj++ )
		{
			pTest[jj]->GetResult();
		}

		for( int jj=0; jj<testCnt; jj++ )
		{
			delete pTest[jj]; pTest[jj]=NULL;
		}

    } catch(except_info_t& err) {
        printf( "\n\n\n\n             \n%s\n", err.info.c_str());
    } catch( ... ) {
        printf( "Unknown error in application!\n");
	}

    delete pTable;

    return 0;
}

