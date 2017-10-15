//#ifndef TF_CHECKFFT_H
#if 0
#define TF_CHECKFFT_H


#include "xcl2.hpp"

#include "tf_testthread.h"


class TF_Device;

struct TF_CheckTransferIn_task_data;

/**
 * 		\brief	check data transfer from host to device
 *
 *
 */
class TF_CheckTransferIn : public TF_TestThread
{
public:

    //!< Internal data for TF_CheckTransferIn
    TF_CheckTransferIn_task_data  *td;

    TF_CheckTransferIn(  TableEngine  *pTable, TF_Device  *pDevice, int argc, char** argv );

    ~TF_CheckTransferIn();

    virtual void PrepareInThread();
    virtual void CleanupInThread();
    virtual void GetResultInThread();
    virtual void StepTable();
    virtual void Run();


    //! set test data in buffer buffer
    void SetBuffer( cl_uint *ptr );

    //! check data in the buffer
    void CheckBuffer( cl_uint *ptr );

    //! Read status information from device
    void GetStatus( void );

    //! Wait for complete data transfer
    void WaitForTransferBufComplete( cl::Event &event );

    //! Start data transfer
    void StartReadBuf( cl::Buffer *pDevice, cl_uint *pHost, cl::Event &event  );

    //! Start kernel for buffer
    void StartCalculateBuf( cl::Buffer *pDevice, cl::Event &event );

    //! Wait for complete calculate
    void WaitForCalculateComplete( cl::Event &event );


};

#endif // TF_CHECKFFT_H
