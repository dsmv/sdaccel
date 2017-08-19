

__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void check_cnt(__global uint  * __restrict input0,
               //__global uint16  * __restrict pErrorData,
               __global uint    * __restrict pStatus,
			   ulong expect,
               ulong size
			  )
{

    ulong ii;
    uint  	flagError=0;
    uint	blockRd		= pStatus[0];
    uint	blockOk 	= pStatus[1];
    uint	blockError	= pStatus[2];

    uint temp0, temp1;


    //
    ii = 0;
    temp1=expect;

    __attribute__((xcl_pipeline_loop))

    for (ii=0; ii<size; ii++) {

    	temp0 = input0[ii];

    	if( temp0!=temp1 )
        	flagError=1;

    	temp1++;

    }

    if( flagError )
    	blockError++;
    else
    	blockOk++;

    blockRd++;
    pStatus[0]=blockRd;
    pStatus[1]=blockOk;
    pStatus[1]=blockError;
}
