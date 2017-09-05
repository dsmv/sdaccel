#ifndef TF_CHECKTRANSFEROUT_H
#define TF_CHECKTRANSFEROUT_H


#include "xcl2.hpp"

#include "tf_testthread.h"


class TF_Device;

struct TF_CheckTransferOut_task_data;

/**
 * 		\brief	check data transfer from device to host
 *
 *
 */
class TF_CheckTransferOut : public TF_TestThread
{
public:

    //!< Internal data for TF_CheckTransferOut
    TF_CheckTransferOut_task_data  *td;

    TF_CheckTransferOut(  TableEngine  *pTable, TF_Device  *pDevice, int argc, char** argv );

    ~TF_CheckTransferOut();

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
    void StartWriteBuf( cl::Buffer *pDevice, cl_uint *pHost, cl::Event &event  );

    //! Start kernel for buffer
    void StartCalculateBuf( cl::Buffer *pDevice, cl::Event &event );

    //! Wait for complete calculate
    void WaitForCalculateComplete( cl::Event &event );


};

#endif // TF_CHECKTRANSFEROUT_H
