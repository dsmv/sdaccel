/*
 * TF_TestThread.cpp
 *
 *  Created on: Jan 29, 2017
 *      Author: Dmitry Smekhov
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "exceptinfo.h"

#include "tf_testthread.h"

//-----------------------------------------------------------------------------

TF_TestThread::TF_TestThread( TableEngine  *pTable, int argc, char **argv) : TF_Test( pTable )
{
    // TODO Auto-generated constructor stub
    //fprintf(stderr, "TF_TestThread::%s(): ID - %ld\n", __FUNCTION__, pthread_self());

    m_isPrepareComplete=0;
    m_isComplete=0;
    m_isTerminate=0;
    m_CycleCnt=0;
    m_isException=0;

    pthread_mutex_init(&m_ResultStartMutex, 0);
    pthread_mutex_init(&m_ResultCompleteMutex, 0);
    pthread_mutex_init(&m_StartMutex, 0);
    pthread_mutex_init(&m_ThreadExitMutex, 0);

    // Lock mutex for Start and GetResult
    pthread_mutex_lock(&m_ResultStartMutex);
    pthread_mutex_lock(&m_ResultCompleteMutex);
    pthread_mutex_lock(&m_StartMutex);
    //pthread_mutex_lock(&m_ThreadExitMutex);

}


//-----------------------------------------------------------------------------

TF_TestThread::~TF_TestThread()
{
    //fprintf(stderr, "TF_TestThread::%s(): ID - %ld\n", __FUNCTION__, pthread_self());
    pthread_mutex_lock(&m_ThreadExitMutex);
    //fprintf(stderr, "TF_TestThread::%s(): ID - %ld (COMPLETE)\n", __FUNCTION__, pthread_self());
}

//-----------------------------------------------------------------------------

int TF_TestThread::Prepare(int cnt)
{
    if( 0==cnt )
    {
        //fprintf(stderr, "TF_TestThread::%s(): ID - %ld\n", __FUNCTION__, pthread_self());
        int res = pthread_attr_init(&m_attrThread);
        if(res != 0) {
            fprintf(stderr, "%s\n", "Stream not started");
            throw( "Stream not started" );
        }

        res = pthread_attr_setdetachstate(&m_attrThread, PTHREAD_CREATE_JOINABLE);
        if(res != 0) {
            fprintf(stderr, "%s\n", "Stream not started");
            throw( "Stream not started" );
        }

        res = pthread_create(&m_hThread, &m_attrThread, ThreadFunc, this);
        if(res != 0) {
            fprintf(stderr, "%s\n", "Stream not started");
            throw( "Stream not started" );
        }
    }

    int ret=m_isPrepareComplete;

    if( m_isException )
    {
        throw except_info( "%s - exception during execute", __FUNCTION__ );
    }
    return ret;
}

//-----------------------------------------------------------------------------

void* TF_TestThread::ThreadFunc(void* lpvThreadParm)
{
    TF_TestThread *test=(TF_TestThread*)lpvThreadParm;
    if(!test)
        return 0;
    //fprintf(stderr, "TF_TestThread::%s(): ID - %ld\n", __FUNCTION__, pthread_self());
    return test->Execute();
}

//-----------------------------------------------------------------------------

void* TF_TestThread::Execute()
{
    try
    {
        pthread_mutex_lock(&m_ThreadExitMutex);

        PrepareInThread();


        m_isPrepareComplete = 1;

        // Sleep here while TF_TestThread::Start() will be called in another thread
        pthread_mutex_lock(&m_StartMutex);

        //fprintf(stderr, "TF_TestThread::%s(): ID - %ld (START)\n", __FUNCTION__, pthread_self());

        // Working forever while m_isTerminate == 0
        Run();

        m_isComplete = 1;

        // Wait while TF_TestThread::GetResult() will be called
        pthread_mutex_lock( &m_ResultStartMutex );

        GetResultInThread();

        // Unlock mutex for complete GetResult() execution
        pthread_mutex_unlock(&m_ResultCompleteMutex);

        CleanupInThread();

        // Unlock mutex for complete TF_TestThread::~TF_TestThread() execution
        pthread_mutex_unlock(&m_ThreadExitMutex);

        //fprintf(stderr, "TF_TestThread::%s(): ID - %ld (COMPLETE)\n", __FUNCTION__, pthread_self());

    }
    catch( except_info_t err )
    {
        printf( "\n\n\n\n\n\n\n                \n Error in thread:\n%s\n                 \n", err.info.c_str());
        m_isComplete = 1;
        m_isException = 1;
        pthread_mutex_unlock(&m_ThreadExitMutex);
    }

    catch( ... )
    {
        printf( "\n\n\n                \n Unknow error in thread\n                 \n" );
        m_isComplete = 1;
        m_isException = 1;
        pthread_mutex_unlock(&m_ThreadExitMutex);

    }

    return 0;
}

//-----------------------------------------------------------------------------

void TF_TestThread::Start()
{
    // Unlock mutex for unblock TF_TestThread::Execute() function
    pthread_mutex_unlock(&m_StartMutex);
    //fprintf(stderr, "TF_TestThread::%s(): ID - %ld\n", __FUNCTION__, pthread_self());
}

//-----------------------------------------------------------------------------

void TF_TestThread::Stop()
{
    m_isTerminate = 1;
    //fprintf(stderr, "TF_TestThread::%s(): ID - %ld\n", __FUNCTION__, pthread_self());
}

//-----------------------------------------------------------------------------

int TF_TestThread::isComplete()
{
    //if(m_isComplete > 0)
        //fprintf(stderr, "TF_TestThread::%s(): ID - %ld\n", __FUNCTION__, pthread_self());

    return m_isComplete;
}

//-----------------------------------------------------------------------------

void TF_TestThread::GetResult()
{
    //fprintf(stderr, "TF_TestThread::%s(): ID - %ld (START)\n", __FUNCTION__, pthread_self());

    // Unlock mutex for finish Execute()
    pthread_mutex_unlock(&m_ResultStartMutex);

    //fprintf(stderr, "TF_TestThread::%s(): ID - %ld (Unlock m_ResultStartMutex)\n", __FUNCTION__, pthread_self());

    // Waiting here while GetResultInThread() will be called and complete from Execute();
    pthread_mutex_lock(&m_ResultCompleteMutex);

    //fprintf(stderr, "TF_TestThread::%s(): ID - %ld (COMPLETE)\n", __FUNCTION__, pthread_self());
}

//-----------------------------------------------------------------------------
/*
void TF_TestThread::GetResultInThread()
{
    fprintf(stderr, "TF_TestThread::%s(): ID - %ld (START)\n", __FUNCTION__, pthread_self());
    fprintf(stderr, "TF_TestThread::%s(): ID - %ld (COMPLETE)\n", __FUNCTION__, pthread_self());
}
*/
//-----------------------------------------------------------------------------

/**
 * 	\brief 	Get integer value from command line
 *
 * 	format command line:
 * 	<name1> <value1> <name2> <value2>
 *
 * 	\param	argc		number of argument
 * 	\param	argv		pointers to arguments
 * 	\param	name		key of argument
 * 	\param	defValue	default value for arguments
 *
 * 	\return   value of argument or default value of argument
 */
int TF_TestThread::GetFromCommnadLine(int argc, char **argv, const char* name, int defValue)
{
    int ret=defValue;
    for( int ii=1; ii<argc-1; ii++ )
    {
        if( 0==strcmp( argv[ii], name) )
        {
            ret=atoi( argv[ii+1] );
        }
    }
    return ret;
}


/**
 *  \brief  Get string value from command line
 *
 * 	format command line:
 * 	<name1> <text1> <name2> <text2>
 *
 * 	\param	argc		number of argument
 * 	\param	argv		pointers to arguments
 * 	\param	name		key of argument
 * 	\param	defValue	default value for arguments
 * 	\param	dst			pointer to destination string, NULL - do not copy
 * 	\param  dstLen		max number of chars in the dst
 *
 * 	\return   0 - name not found, 1 - found only name, 2 - found name and text
 */
 int TF_TestThread::GetStrFromCommnadLine(int argc, char **argv, const char* name, char* defValue, char* dst, int dstLen )
{
	int ret=0;
	int index=0;
	for( int ii=1; ii<argc; ii++ )
    {
        if( 0==strcmp( argv[ii], name) )
        {
        	if( ii==(argc-1) )
        		ret=1;
        	else
        	{
        		ret=2; index=ii;
        	}

        	break;
        }
    }
	if( NULL!=dst )
	{
		if( 2==ret )
			strncpy( dst, argv[index+1], dstLen );
		else
			strncpy( dst, defValue, dstLen );
		dst[dstLen-1]=0;
	}
	return ret;
}


//-----------------------------------------------------------------------------


