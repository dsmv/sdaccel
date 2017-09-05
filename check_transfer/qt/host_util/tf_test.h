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

    //! Prepare test
    /**
     * \param cnt   number of call function
     * \return      1 - prepare complete, 0 - prepare don't complete
     *
     *  Function is called during test preparation.
     *  Preparation of the test can be very long.
     *  When preparation is complete function must be return 1.
     *
     *  Preparation of the all tests is complete when all tests return 1
     *
     */
    virtual int 	Prepare(int cnt) = 0;

    //! Start of test
    virtual void	Start() = 0;

    //! Stop of test
    virtual void 	Stop() {}

    //! Return 1 when test is complete
    virtual int		isComplete() { return 0; }

    //! Show table
    /**
     *
     * Function is called with 100 ms period.
     * Test must put status information in the test table into this function.
     *
     */
    virtual void	StepTable() {}

    //! Don't use. Reserve for future
    virtual void	StepMainThread() {}


    //! Show result of test
    virtual void	GetResult() {}
};




#endif /* TF_TEST_H_ */
