#ifndef TF_CHECKTRANSFER_H
#define TF_CHECKTRANSFER_H

#include "tf_testthread.h"

struct TF_CheckTransfer_task_data;

class TF_CheckTransfer : public TF_TestThread
{
public:

    TF_CheckTransfer_task_data  *td;

    TF_CheckTransfer(  TableEngine  *pTable, int argc, char** argv );

    ~TF_CheckTransfer();

    virtual void PrepareInThread();
    virtual void CleanupInThread();
    virtual void GetResultInThread();
    virtual void StepTable();
    virtual void Run();

};

#endif // TF_CHECKTRANSFER_H
