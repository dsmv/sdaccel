



#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#include "ipc.h"
#include "exceptinfo.h"
#include "table_engine_console.h"

#include "tf_checktransferout.h"

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
 *
 *
 * 	-table	<flag_show>	  	: 1 - show table, 0 -	do not show table
 * 	-time   <time> 		  	: execution time [s],
 *	-mode	<n>				: 0 - Emulation-CPU, 1 - Emulation-HW, 2 - System
 *
 */

int main(int argc, char **argv)
{
    int X = 0;
    int Y = 0;
    int tableMode;
    int timeMode;
    long	testStartTime;

    const char *headers[] = {
        "TIME", "BLOCK_WR", "BLOCK_RD", "BLOCK_OK", "BLOCK_ERROR", "SPD_CURRENT", "SPD_AVR",
    };

    TableEngine  *pTable = new TableEngineConsole();

    tableMode = TF_TestThread::GetFromCommnadLine( argc, argv, "-table", 0 );
    timeMode  = TF_TestThread::GetFromCommnadLine( argc, argv, "-time", 10 );

    if( tableMode )
    	pTable->CreateTable(headers, sizeof(headers)/sizeof(headers[0]), 0);


    signal(SIGINT, signal_handler);

    try {


        TF_CheckTransferOut *pTest = new TF_CheckTransferOut( pTable, argc, argv );

        for( int ii=0; ; ii++) {
			if( pTest->Prepare(ii) )
				break;
		}


        if( tableMode )
        	pTable->GetConsolePos(X,Y);

		pTest->Start();

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
                // Тестирование реализовано в функции StepMainThread()
                pTest->StepMainThread();
                IPC_getTime(&curr_time);
                if(IPC_getDiffTime(&start_time, &curr_time) > 100) {
                    break;
                }
                IPC_delay(10);
            }
#else
            // Тестирование реализовано в отдельном потоке
            IPC_delay(100);
#endif

				long currentTime = IPC_getTickCount();
				if( timeMode>0 )
				{
					if( (currentTime - testStartTime) >= timeMode*1000 )
							exit_flag=2;
				}

                if( exit_flag ) {
                    pTest->Stop();


                if( pTest->isComplete() )
                    break;

            }

             if( tableMode )
            	 pTest->StepTable();

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

        pTest->GetResult();

		delete pTest; pTest=NULL;

    } catch(except_info_t& err) {
        printf( "\n\n\n\n             \n%s\n", err.info.c_str());
    } catch( ... ) {
        printf( "Unknown error in application!\n");
	}

    delete pTable;

    return 0;
}

