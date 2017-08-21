/*
 * TF_TestThread.h
 *
 *  Created on: Jan 29, 2017
 *      Author: Dmitry Smekhov
 */

#ifndef TF_TestThread_H_
#define TF_TestThread_H_

#include <pthread.h>
#include "tf_test.h"

class TableEngine;

/**
 *	\brief	Base class for application with thread
 *
 *
 *
 */
class TF_TestThread: public TF_Test
{
public:
    TF_TestThread( TableEngine  *pTable, int argc, char **argv);

	virtual ~TF_TestThread();

    virtual int 	Prepare(int cnt);
    virtual void	Start();
    virtual void 	Stop();
    virtual int		isComplete();
    virtual void    GetResult();

    static void*    ThreadFunc(void* lpvThreadParm);

    void* Execute();

    virtual void PrepareInThread() = 0;
    virtual void CleanupInThread() = 0;
    virtual void GetResultInThread() = 0;
    virtual void StepTable() = 0;
    virtual void Run() = 0;

	int	m_isPrepareComplete;
	int	m_isComplete;
	int m_isTerminate;
	int	m_CycleCnt;
    int m_isException;

    pthread_mutex_t		m_ResultStartMutex;
    pthread_mutex_t		m_ResultCompleteMutex;
	pthread_mutex_t		m_StartMutex;
    pthread_mutex_t		m_ThreadExitMutex;

    pthread_t 			m_hThread;
    pthread_attr_t  	m_attrThread;

    static int GetFromCommnadLine(int argc, char **argv, const char* name, int defValue);
};

#endif /* TF_TestThread_H_ */
