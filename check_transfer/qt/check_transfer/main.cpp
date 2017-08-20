
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

int main(int argc, char **argv)
{
    int X = 0;
    int Y = 0;

    const char *headers[] = {
        //"NAME", "BLOCK_WR", "BLOCK_RD", "BLOCK_OK", "BLOCK_ERROR", "SPD_CURRENT", "SPD_AVR", "STATUS", "OTHER",
        "NAME", "BLOCK_WR", "BLOCK_RD", "BLOCK_OK", "BLOCK_ERROR", "SPD_CURRENT", "SPD_AVR",
    };

    TableEngine  *pTable = new TableEngineConsole();
    pTable->CreateTable(headers, sizeof(headers)/sizeof(headers[0]), 0);

    // Создадим две строки в таблице (для примера использования)
    //pTable->AddRowTable();
    //pTable->AddRowTable();

    signal(SIGINT, signal_handler);

    try {


        TF_CheckTransferOut *pTest = new TF_CheckTransferOut( pTable, argc, argv );

        for( int ii=0; ; ii++) {
			if( pTest->Prepare(ii) )
				break;
		}


        pTable->GetConsolePos(X,Y);

		pTest->Start();

#if defined(IS_STEP_MAIN_THREAD)
        IPC_TIMEVAL start_time;
        IPC_TIMEVAL curr_time;
#endif

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
                if( exit_flag ) {
                    pTest->Stop();


                if( pTest->isComplete() )
                    break;

            }

			pTest->StepTable();

            // Сохраним координаты курсора, перед первым выводом в таблицу
            // для последующего вывода информации с нужной строки
            //if(count == 0) {
                //pTable->GetConsolePos(X,Y);
            //}

            // Пример зарполнения информации в таблице
            //pTable->SetValueTable(1,1,"%d : %d", 1,count);
            //pTable->SetValueTable(2,2,"%d : %d", 2,count);
            ++count;

            if( IPC_kbhit() )
            {
                int key = IPC_getch();
                if( 27==key )
                    break;
            }
		}

        // Восстановим координаты курсора для того, чтобы
        // печатаемый текст не накладывался на уже выведенный
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

