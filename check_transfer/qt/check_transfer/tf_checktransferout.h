#ifndef TF_CHECKTRANSFER_H
#define TF_CHECKTRANSFER_H

#include "utypes.h"

#include "tf_testthread.h"


struct TF_CheckTransferOut_task_data;

class TF_CheckTransferOut : public TF_TestThread
{
public:

    //!< Internal data for TF_CheckTransferOut
    TF_CheckTransferOut_task_data  *td;

    TF_CheckTransferOut(  TableEngine  *pTable, int argc, char** argv );

    ~TF_CheckTransferOut();

    virtual void PrepareInThread();
    virtual void CleanupInThread();
    virtual void GetResultInThread();
    virtual void StepTable();
    virtual void Run();


    //! set test data in buffer buffer
    void SetBuffer( U32 *ptr );

    //! check data in the buffer
    void CheckBuffer( U32 *ptr );
};

#endif // TF_CHECKTRANSFER_H
