
#include <stdio.h>
#include "exceptinfo.h"
#include "tf_checktransferout.h"

#include "table_engine.h"

#include "utypes.h"
#include "ipc.h"

//#include "xcl.h"
#include <vector>
#include "xcl2.hpp"


//!< Internal data for TF_CheckTransferOut
struct TF_CheckTransferOut_task_data {

    U32 RowNumber;      //!< Number of first row in the table
    U32 BlockWr;        //!< Count of written blocks
    U32 BlockRd;        //!< Count of read blocks
    U32 BlockOk;        //!< Count of correct blocks
    U32 BlockError;     //!< Count of incorrect blocks
    U32 sizeBlock;      //!< Size of block [bytes]
    float VelocityCurrent;  //!< current speed (on 4 secund interval)
    float VelocityAverage;  //!< average spped (from test start)

    clock_t startTick;      //!< Number of start clock();
    clock_t lastTick;       //!< Number of last clock()
    U32     lastBlock;      //!< Number BlockWr for lastTick
    float   testTime;       //!< Time from test start


    time_t  startTime;
    time_t  currentTime;

    U32     dataOut;        //!< current data for output
    U32     dataExpect;      //!< expect data from input

//    xcl_world   xcl;        //!< OpenCL enviropment for device
//    cl_program  program;    //!< OpenCL programm
//    cl_kernel   kernel;     //!< OpenCL kernel

    char *xclbinFileName;   //!< file name for container
    char *kernelName;       //!< kernel name

//    cl::Device  *pDevice;
//    cl::Context *pContext;
//    cl::Program *pProgram;
//    cl::Kernel  *pKrnl;

    cl::Device  device;
    cl::Context context;
    cl::Program program;
    cl::Kernel  krnl;

    std::string deviceName;

    U32 *pBufOut[2];

    cl::Buffer      *pBuffer[2];

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

      kernelName = "bandwidth";


/*      pDevice  = NULL;
      pContext = NULL;
      pProgram = NULL;
      pKrnl     = NULL;
*/

      pBufOut[0]=NULL;
      pBufOut[1]=NULL;

      pBuffer[0]=NULL;
      pBuffer[1]=NULL;
    };

    ~TF_CheckTransferOut_task_data()
    {
        delete xclbinFileName; xclbinFileName=NULL;
    }


};

//! Constructor
TF_CheckTransferOut::TF_CheckTransferOut( TableEngine  *pTable, int argc, char **argv) : TF_TestThread( pTable, argc, argv )
{
    td = new TF_CheckTransferOut_task_data();

    int mode = GetFromCommnadLine( argc, argv, "-mode", 2 );

    //"../sdx_wsp/check_transfer/System/binary_container_1.xclbin";
    const char *path0 = "../sdx_wsp/check_transfer/";
    const char *name  = "binary_container_1.xclbin";


    switch (mode)
    {
        case 0:
            sprintf( td->xclbinFileName, "%sEmulation-CPU/%s", path0, name );
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

    delete td->pBuffer[0]; td->pBuffer[0]=NULL;
    delete td->pBuffer[1]; td->pBuffer[1]=NULL;


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

    //throw except_info( "%s - check ", __FUNCTION__ );

//    printf( "Open device\n");
//    td->xcl = xcl_world_single();
//    printf( "Device: %s\n", td->xcl.device_name );

//    printf( "Start load file \n");
//    td->program = xcl_import_binary_file( td->xcl, td->xclbinFileName );
//    printf( "load Ok \n");

//    printf( "Get kernel: %s\n", td->kernelName );
//    td->kernel = xcl_get_kernel( td->program, td->kernelName );
//    printf( "Get kernel - Ok\n");

 //   td->xcl.command_queue
    //cl::CommandQueue q;

    printf( "Open device\n");
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    //cl::Device device = devices[0];
//    td->pDevice = new  cl::Device();
//    *(td->pDevice) = devices[0];
//    std::string name = td->pDevice->getInfo<CL_DEVICE_NAME>();
//    //td->pDevice->getInfo( , &name );
//    printf( "Device: %s\n", name.c_str());

//    td->pContext = new cl::Context( *(td->pDevice));

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

    //td->sizeBlock = 16 * 1024;

    printf( "Alloc host memory 2 x %d ", td->sizeBlock );
    {
        void *ptr=(void*)0xAAAA;
        int err;
        err = posix_memalign( &ptr, 4096, td->sizeBlock );
        if( err )
            except_info( "%s - ошибка выделения памяти ");
        td->pBufOut[0] = (U32*) ptr;
        printf( "ptr=%p\n", ptr );

        err = posix_memalign( &ptr, 4096, td->sizeBlock );
        if( err )
            except_info( "%s - ошибка выделения памяти ");
        td->pBufOut[1] = (U32*) ptr;

        printf( "ptr=%p\n", ptr );

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
    printf( " - Ok\n" );

}

//! Free any resource
void TF_CheckTransferOut::CleanupInThread()
{
    printf( "%s\n", __FUNCTION__ );



}

//! Show results of test
void TF_CheckTransferOut::GetResultInThread()
{
    printf( "%s\n", __FUNCTION__ );

}

//! Show table durint test executing
void TF_CheckTransferOut::StepTable()
{
    m_pTemplateEngine->SetValueTable( td->RowNumber, 1, (unsigned)td->BlockWr );
    m_pTemplateEngine->SetValueTable( td->RowNumber, 2, (unsigned)td->BlockRd );

    m_pTemplateEngine->SetValueTable( td->RowNumber, 3, (unsigned)td->BlockOk );
    m_pTemplateEngine->SetValueTable( td->RowNumber, 4, (unsigned)td->BlockError );
    m_pTemplateEngine->SetValueTable( td->RowNumber, 5, td->VelocityCurrent, "%10.1f" );
    m_pTemplateEngine->SetValueTable( td->RowNumber, 6, td->VelocityAverage, "%10.1f" );

    m_pTemplateEngine->SetValueTable( td->RowNumber, 0, td->testTime, "%10.1f" );

}

//! Main body of test
void TF_CheckTransferOut::Run()
{
    td->RowNumber=m_pTemplateEngine->AddRowTable();

    //printf( "%s\n", __FUNCTION__ );

    td->startTick = td->lastTick = clock();

    td->startTime = time(NULL);

    cl::CommandQueue q(td->context, td->device );
    cl::Event eventCompletion0;
    cl::Event eventCompletion1;
    cl::Event eventCompletion2;
    cl::Event eventCompletion3;
    cl_int ret;

    int flag_wait=0;

    for( ; ; )
    {
        if( this->m_isTerminate )
            break;

//        td->BlockRd++;

//        IPC_delay( 200 );

        //SetBuffer( td->pBufOut[0] );
        {
            //cl::CommandQueue q(td->context, td->device, CL_QUEUE_PROFILING_ENABLE);

            if( flag_wait)
             ret = eventCompletion0.wait();

            SetBuffer( td->pBufOut[0] );

            ret=q.enqueueWriteBuffer(
                    *(td->pBuffer[0]),
                    CL_FALSE,
                    0,
                    td->sizeBlock,
                    td->pBufOut[0],
                    NULL,
                    &eventCompletion0
                    );

            //ret = eventCompletion0.wait();
//            //q.flush();
//            q.finish();

//        }
//        {
//            cl_int ret;
//            cl::CommandQueue q(td->context, td->device );

            if( flag_wait)
             ret = eventCompletion1.wait();

            SetBuffer( td->pBufOut[1] );

            ret=q.enqueueWriteBuffer(
                    *(td->pBuffer[1]),
                    CL_FALSE,
                    0,
                    td->sizeBlock,
                    td->pBufOut[1],
                    NULL,
                    &eventCompletion1
                    );


            //q.flush();
//            q.finish();

            //CheckBuffer( td->pBufOut[1] );

            flag_wait=1;

            if( CL_SUCCESS !=ret )
                td->BlockError++;
        }
        td->BlockWr++;


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

}

//! set test data in buffer
void TF_CheckTransferOut::SetBuffer( U32 *ptr )
{
    U32 *dst = ptr;
    U32 count = td->sizeBlock / sizeof(U32) / 16;
    U32 val=td->dataOut;
    for( int ii=0; ii<count; ii++ )
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
void TF_CheckTransferOut::CheckBuffer( U32 *ptr )
{
    U32 *src = ptr;
    U32 count = td->sizeBlock / sizeof(U32);
    U32 val=td->dataExpect;
    U32 di;
    U32 flag_error=0;
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
