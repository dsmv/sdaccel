
#include <stdio.h>
#include "exceptinfo.h"
#include "tf_checktransferout.h"

#include "table_engine.h"

#include "utypes.h"
#include "ipc.h"

//#include "xcl.h"
#include <vector>


//!< Internal data for TF_CheckTransferOut
struct TF_CheckTransferOut_task_data {

	cl_uint RowNumber;      //!< Number of first row in the table
	cl_uint BlockWr;        //!< Count of written blocks
	cl_uint BlockRd;        //!< Count of read blocks
	cl_uint BlockOk;        //!< Count of correct blocks
	cl_uint BlockError;     //!< Count of incorrect blocks
	cl_uint sizeBlock;      //!< Size of block [bytes]
	cl_uint Sig;			//!< Signature for status buffer
    float VelocityCurrent;  //!< current speed (on 4 secund interval)
    float VelocityAverage;  //!< average speed (from test start)

    clock_t startTick;      //!< Number of start clock();
    clock_t lastTick;       //!< Number of last clock()
    cl_uint lastBlock;      //!< Number BlockWr for lastTick
    float   testTime;       //!< Time from test start


    time_t  startTime;		//!< time of start main test cycle
    time_t  currentTime;	//!< current test time

    cl_ulong  dataOut;        //!< current data for output
    cl_ulong  dataExpect;      //!< expect data from input


    char *xclbinFileName;   //!< file name for container
    char *kernelName;       //!< kernel name

    cl::Device  device;
    cl::Context context;
    cl::Program program;
    cl::Kernel  krnl;
    cl::CommandQueue *q;

    std::string deviceName;

    cl_uint *pBufOut[2];				//!< pointers to buffers in the host memory

    cl::Buffer      *pBuffer[2];	//!< pointers to buffers in the device memory

    cl::Buffer		*dpStatus;		//!< pointer to status buffer in the device memory

    cl_uint	*pStatus;					//!< pointer to status buffer in the host memory

    cl_ulong	flagGetStatus;			//!< 1 - request for get status information

    TF_CheckTransferOut_task_data()
    {
      BlockWr=0;
      BlockRd=0;
      BlockOk=0;
      BlockError=0;
      RowNumber=0;

      lastBlock = 0;
      testTime = 0;

      dataOut=dataExpect=0xA0000000;

      xclbinFileName = new char[256];

      kernelName = "check_cnt";


      pBufOut[0]=NULL;
      pBufOut[1]=NULL;

      pBuffer[0]=NULL;
      pBuffer[1]=NULL;

      pStatus = NULL;
      dpStatus = NULL;

      q=NULL;

      flagGetStatus=0;
    };

    ~TF_CheckTransferOut_task_data()
    {
        delete xclbinFileName; xclbinFileName=NULL;
    }


};

//! Constructor
/**
 *
 * 		-mode	<n>				: 0 - Emulation-CPU, 1 - Emulation-HW, 2 - System
 *
 */
TF_CheckTransferOut::TF_CheckTransferOut( TableEngine  *pTable, int argc, char **argv) : TF_TestThread( pTable, argc, argv )
{
    td = new TF_CheckTransferOut_task_data();

    int mode = GetFromCommnadLine( argc, argv, "-mode", 0 );

    //"../sdx_wsp/check_transfer/System/binary_container_1.xclbin";
    //const char *path0 = "../sdx_wsp/check_transfer/";
    const char *path0 = "../";
    const char *name  = "binary_container_1.xclbin";


    switch (mode)
    {
        case 0:
            //sprintf( td->xclbinFileName, "%sEmulation-CPU/%s", path0, name );
        	sprintf( td->xclbinFileName, "%s%s", path0, name );
            break;
        case 1:
            sprintf( td->xclbinFileName, "%sEmulation-HW/%s", path0, name );
            break;
        case 2:
            sprintf( td->xclbinFileName, "%sSystem/%s", path0, name );
            break;

        default:
            throw except_info( "%s - mode=%d - it is not allowed ", __FUNCTION__, mode );
            break;
    }


}

//! Destructor
TF_CheckTransferOut::~TF_CheckTransferOut()
{

    free( td->pBufOut[0] ); td->pBufOut[0]=NULL;
    free( td->pBufOut[1] ); td->pBufOut[1]=NULL;
    free( td->pStatus );	td->pStatus=NULL;


    delete td->pBuffer[0]; td->pBuffer[0]=NULL;
    delete td->pBuffer[1]; td->pBuffer[1]=NULL;
    delete td->dpStatus;  td->dpStatus=NULL;


    delete td; td=NULL;
}

//! Prepare test
/**
 *  This function executed before main body of test.
 *
 *
 */
void TF_CheckTransferOut::PrepareInThread()
{
    printf( "\n\n\n\n"); //FIXME - it is need for output under table
    printf( "%s\n\n", __FUNCTION__ );


    printf( "Open device\n");
    std::vector<cl::Device> devices = xcl::get_xil_devices();

    td->device = devices[0];
    td->deviceName = td->device.getInfo<CL_DEVICE_NAME>();
    printf( "Device: %s\n", td->deviceName.c_str());

    cl::Context context( td->device );
    td->context = context;

    cl::Program::Binaries bins = xcl::import_binary_file( td->xclbinFileName );
    devices.resize(1);
    cl::Program program(td->context, devices, bins);

    td->program = program;

    cl::Kernel krnl( td->program, td->kernelName );
    td->krnl = krnl;


    td->sizeBlock = 16 * 1024 * 1024;

    //td->sizeBlock = 4 * 1024;

    printf( "Alloc host memory 2 x %d ", td->sizeBlock );
    {
        void *ptr=(void*)0xAAAA;
        int err;
        err = posix_memalign( &ptr, 4096, td->sizeBlock );
        if( err )
            throw except_info( "%s - memory allocation error ", __FUNCTION__);
        td->pBufOut[0] = (cl_uint*) ptr;
        printf( "ptr=%p\n", ptr );

        err = posix_memalign( &ptr, 4096, td->sizeBlock );
        if( err )
        	throw except_info( "%s - memory allocation error ", __FUNCTION__);
        td->pBufOut[1] = (cl_uint*) ptr;

        printf( "ptr=%p\n", ptr );



        err = posix_memalign( &ptr, 4096, 16384 );
        if( err )
        	throw except_info( "%s - memory allocation error ", __FUNCTION__);
        td->pStatus = (cl_uint*) ptr;


    }
    printf( " - Ok\n" );

    printf( "Alloc device memory 2 x %d (DDR0 & DDR1) ", td->sizeBlock );
    {
        cl::Buffer *pBuf;

        cl_mem_ext_ptr_t input_buffer_ext;

        input_buffer_ext.flags = XCL_MEM_DDR_BANK0;
        input_buffer_ext.obj = NULL;
        input_buffer_ext.param = 0;

        pBuf = new cl::Buffer( td->context,
                               CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,
                               td->sizeBlock,
                               &input_buffer_ext
                               );
        td->pBuffer[0] = pBuf;
    }

    {
        cl::Buffer *pBuf;

        cl_mem_ext_ptr_t input_buffer_ext;

        input_buffer_ext.flags = XCL_MEM_DDR_BANK0;
        input_buffer_ext.obj = NULL;
        input_buffer_ext.param = 0;

        pBuf = new cl::Buffer( td->context,
                               CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,
                               td->sizeBlock,
                               &input_buffer_ext
                               );
        td->pBuffer[1] = pBuf;
    }

    {
        cl::Buffer *pBuf;

        cl_mem_ext_ptr_t input_buffer_ext;

        input_buffer_ext.flags = XCL_MEM_DDR_BANK0;
        input_buffer_ext.obj = NULL;
        input_buffer_ext.param = 0;

        pBuf = new cl::Buffer( td->context,
                               CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,
                               16384,
                               &input_buffer_ext
                               );
        td->dpStatus = pBuf;


    }

    printf( " - Ok\n" );

}

//! Free any resource
void TF_CheckTransferOut::CleanupInThread()
{
    printf( "%s\n", __FUNCTION__ );
    td->q=NULL;


}

//! Show results of test
void TF_CheckTransferOut::GetResultInThread()
{
    printf( "%s\n", __FUNCTION__ );
    //GetStatus();

    int flag_error=0;

    if( 0xAA55==td->Sig )
    {
    	printf( "Sig=0xAA55 - Ok\n");
    } else
    {
    	printf( "Sig=0x%X - Error, expect 0xAA55\n", td->Sig );
    	flag_error=1;
    }
    printf( "BlockWr    = %d\n", td->BlockWr );
    printf( "BlockRd    = %d\n", td->BlockRd );
    printf( "BlockOK    = %d\n", td->BlockOk );
    printf( "BlockError = %d\n", td->BlockError );

    if( td->BlockOk!=td->BlockRd )
    	flag_error++;

    if( 0 == td->BlockRd )
    	flag_error++;

    if( 0 != td->BlockError )
    	flag_error++;

    if( 0==flag_error )
    	printf( "\nTest finished successfully\n\n" );
    else
    	printf( "\nTest finished with errors\n\n" );

}

//! Show table during test executing
void TF_CheckTransferOut::StepTable()
{
    m_pTemplateEngine->SetValueTable( td->RowNumber, 1, (unsigned)td->BlockWr );
    m_pTemplateEngine->SetValueTable( td->RowNumber, 2, (unsigned)td->BlockRd );

    m_pTemplateEngine->SetValueTable( td->RowNumber, 3, (unsigned)td->BlockOk );
    m_pTemplateEngine->SetValueTable( td->RowNumber, 4, (unsigned)td->BlockError );
    m_pTemplateEngine->SetValueTable( td->RowNumber, 5, td->VelocityCurrent, "%10.1f" );
    m_pTemplateEngine->SetValueTable( td->RowNumber, 6, td->VelocityAverage, "%10.1f" );

    m_pTemplateEngine->SetValueTable( td->RowNumber, 0, td->testTime, "%10.1f" );

    td->flagGetStatus=1;
}

//! Main body of test
void TF_CheckTransferOut::Run()
{
    td->RowNumber=m_pTemplateEngine->AddRowTable();

    printf( "%s\n", __FUNCTION__ );

    td->startTick = td->lastTick = clock();

    td->startTime = time(NULL);

    cl::CommandQueue q(td->context, td->device );
    td->q = &q;

    {
    	for( int ii=0; ii<512; ii++ )
    	{
    		td->pStatus[ii]=0;
    	}
    	td->pStatus[0]=0xBB66;

		cl_int ret=td->q->enqueueWriteBuffer(
				*(td->dpStatus),
				CL_TRUE,
				0,
				4096,
				td->pStatus,
				NULL,
				NULL
				);
		printf( "%s ret=%d \n", __FUNCTION__, ret );
    }



    cl::Event eventCompletionTransfer0;
    cl::Event eventCompletionTransfer1;
    cl::Event eventCompletionExecuting0;
    cl::Event eventCompletionExecuting1;
    cl_int ret;
    cl_int sizeOfuint16 = td->sizeBlock/64;  // count of words by 512 bits
    cl_ulong8	arrayExpect;

    std::vector<cl::Event>  events0;
    std::vector<cl::Event>  events1;

    int flag_wait=0;

    cl::NDRange globalNDR(1,1,1);
    cl::NDRange localNDR(1,1,1);
    cl::NDRange offsetNDR=cl::NullRange;

    //SetBuffer( td->pBufOut[0] );
    //SetBuffer( td->pBufOut[1] );
    for( ; ; )
    {
        if( this->m_isTerminate )
            break;


        if( td->flagGetStatus )
        {
        	GetStatus();
        	td->flagGetStatus=0;
        }

        {

            if( flag_wait)
            {
             ret = eventCompletionExecuting0.wait();

            }

            SetBuffer( td->pBufOut[0] );
            td->BlockWr++;

            ret=q.enqueueWriteBuffer(
                    *(td->pBuffer[0]),
                    CL_FALSE,
                    0,
                    td->sizeBlock,
                    td->pBufOut[0],
                    NULL,
                    &eventCompletionTransfer0
                    );

            {
				ulong expect = td->dataExpect;
				for( int jj=0; jj<8; jj++ )
				{
					arrayExpect.s[jj]=expect++;
				}
				td->dataExpect+=td->sizeBlock/8;
            }

            td->krnl.setArg( 0, *(td->pBuffer[0]) );
            td->krnl.setArg( 1, *(td->dpStatus) );
            td->krnl.setArg( 2, arrayExpect );
            td->krnl.setArg( 3, sizeOfuint16 );

            events0.clear();
            events0.push_back( eventCompletionTransfer0 );



            ret = q.enqueueNDRangeKernel(
            		td->krnl,
					offsetNDR,
					globalNDR,
					localNDR,
					&events0,
					//NULL,
					&eventCompletionExecuting0

					);

					if( CL_SUCCESS != ret )
					{
						throw except_info( "%s - q.enqueueNDRangeKernel() - error  ret=%d ", __FUNCTION__, ret );
					}


            //eventCompletionExecuting0.wait();
//            q.enqueueTask( td->krnl );
//
            if( flag_wait)
             ret = eventCompletionExecuting1.wait();

            SetBuffer( td->pBufOut[1] );
            td->BlockWr++;

            ret=q.enqueueWriteBuffer(
                    *(td->pBuffer[1]),
                    CL_FALSE,
                    0,
                    td->sizeBlock,
                    td->pBufOut[1],
                    NULL,
                    &eventCompletionTransfer1
                    );


            {
				ulong expect = td->dataExpect;
				for( int jj=0; jj<8; jj++ )
				{
					arrayExpect.s[jj]=expect++;
				}
				td->dataExpect+=td->sizeBlock/8;
            }

            td->krnl.setArg( 0, *(td->pBuffer[1]) );
            td->krnl.setArg( 1, *(td->dpStatus) );
            td->krnl.setArg( 2, arrayExpect );
            td->krnl.setArg( 3, sizeOfuint16 );

            events1.clear();
            events1.push_back( eventCompletionTransfer1 );



            ret = q.enqueueNDRangeKernel(
            		td->krnl,
					offsetNDR,
					globalNDR,
					localNDR,
					&events1,
					//NULL,
					&eventCompletionExecuting1

					);

					if( CL_SUCCESS != ret )
					{
						throw except_info( "%s - q.enqueueNDRangeKernel() - error  ret=%d ", __FUNCTION__, ret );
					}




            flag_wait=1;

//            if( CL_SUCCESS !=ret )
//                td->BlockError++;
        }

        //eventCompletionExecuting0.wait();

        clock_t currentTick = clock();
        clock_t diff = currentTick - td->lastTick;
        clock_t interval = 5*CLOCKS_PER_SEC;
        if( diff > interval )
        {
            double currentTime = (currentTick - td->lastTick);
            currentTime /=CLOCKS_PER_SEC;
            double velocity = ((td->BlockWr-td->lastBlock)*td->sizeBlock)/currentTime;

            td->VelocityCurrent = velocity / (1024*1024);


//            currentTime = (currentTick - td->startTick);
//            currentTime /=CLOCKS_PER_SEC;
//            velocity = (td->BlockWr)*td->sizeBlock/currentTime;

            time_t timeFromStart = time(NULL) - td->startTime;

            velocity = (td->BlockWr)*td->sizeBlock/timeFromStart;

            td->VelocityAverage = velocity / (1024*1024);

            td->lastTick = currentTick;
            td->lastBlock = td->BlockWr;
            td->testTime = timeFromStart;
        }
    }

    GetStatus();

}

//! set test data in buffer
void TF_CheckTransferOut::SetBuffer( cl_uint *ptr )
{
	cl_ulong *dst = (cl_ulong*) ptr;
    cl_uint count = td->sizeBlock / sizeof(cl_ulong) / 16;
    cl_ulong val=td->dataOut;
    for( cl_uint ii=0; ii<count; ii++ )
    {
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
        *dst++=val++;
    }
    td->dataOut=val;
}

//! check data in the buffer
void TF_CheckTransferOut::CheckBuffer( cl_uint *ptr )
{
	cl_ulong *src = (cl_ulong*)ptr;
    cl_uint count = td->sizeBlock / sizeof(cl_uint);
    cl_ulong val=td->dataExpect;
    cl_ulong di;
    cl_ulong flag_error=0;
    for( int ii=0; ii<count; ii++ )
    {
        //di = *src++;
        di = ptr[ii];

        if( di!=val )
        {
            flag_error++;
        }
        val++;
    }
    td->dataExpect=val;

    if( 0==flag_error )
        td->BlockOk++;
    else
        td->BlockError++;
}

//! Read status information from device
void TF_CheckTransferOut::GetStatus( void )
{
	if( NULL==td->pStatus )
		return;


    cl_int ret=td->q->enqueueReadBuffer(
            *(td->dpStatus),
            CL_TRUE,
            0,
            512,
            td->pStatus,
            NULL,
            NULL
            );
//    printf( "%s ret=%d \n", __FUNCTION__, ret );

	td->Sig		   = td->pStatus[0];
	td->BlockRd    = td->pStatus[1];
	td->BlockOk    = td->pStatus[2];
	td->BlockError = td->pStatus[3];

}
