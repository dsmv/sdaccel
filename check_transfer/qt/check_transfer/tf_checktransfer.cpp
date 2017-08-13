
#include <stdio.h>
#include "tf_checktransfer.h"

#include "table_engine.h"

#include "ipc.h"

struct TF_CheckTransfer_task_data {

    int RowNumber;
    int BlockRd;

    TF_CheckTransfer_task_data()
    {
      BlockRd=0;
      RowNumber=0;
    };
};

TF_CheckTransfer::TF_CheckTransfer( TableEngine  *pTable, int argc, char **argv) : TF_TestThread( pTable, argc, argv )
{
    td = new TF_CheckTransfer_task_data();
}

TF_CheckTransfer::~TF_CheckTransfer()
{
    delete td; td=NULL;
}

void TF_CheckTransfer::PrepareInThread()
{
    printf( "%s\n\n", __FUNCTION__ );

    for( int ii=0; ii<16; ii++ )
    {
        printf( "  %d \n", ii );
    }

}

void TF_CheckTransfer::CleanupInThread()
{
    printf( "%s\n", __FUNCTION__ );

}

void TF_CheckTransfer::GetResultInThread()
{
    printf( "%s\n", __FUNCTION__ );

}

void TF_CheckTransfer::StepTable()
{
    m_pTemplateEngine->SetValueTable( td->RowNumber, 3, td->BlockRd );
}

void TF_CheckTransfer::Run()
{
    td->RowNumber=m_pTemplateEngine->AddRowTable();

    //printf( "%s\n", __FUNCTION__ );

    for( ; ; )
    {
        if( this->m_isTerminate )
            break;

        td->BlockRd++;

        IPC_delay( 200 );
    }

}
