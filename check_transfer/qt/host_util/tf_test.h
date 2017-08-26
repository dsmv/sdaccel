/*
 * tf_test.h
 *
 *  Created on: Jan 29, 2017
 *      Author: Dmitry Smekhov
 */

#ifndef TF_TEST_H_
#define TF_TEST_H_

class TableEngine;

/**
 * 	\brief	Base class for testing device
 */
class TF_Test
{

public:

    TableEngine *m_pTemplateEngine;

    TF_Test( TableEngine  *pTable )
    {
        m_pTemplateEngine = pTable;
    }

    virtual int 	Prepare(int cnt) = 0;

    virtual void	Start() = 0;

    virtual void 	Stop() {}

    virtual int		isComplete() { return 0; }

    virtual void	StepTable() {}

    virtual void	StepMainThread() {}

    virtual void	GetResult() {}
};




#endif /* TF_TEST_H_ */
