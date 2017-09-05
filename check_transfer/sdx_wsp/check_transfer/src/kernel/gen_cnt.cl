



__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void gen_cnt(
				__global ulong8    *pOut,
				__global ulong8    *pStatus,
				const 	uint  size
			  )
{


    uint	blockWr;


    ulong8 	temp0, temp1;

    ulong8	checkStatus;

    ulong8	addConst;


    checkStatus = pStatus[0];

    temp1     	= pStatus[1];
    addConst 	= pStatus[2];

    blockWr 	= checkStatus.s0 >> 32;

  __attribute__((xcl_pipeline_loop))
  for ( int ii=0; ii<size; ii++)
    {

	  	pOut[ii] = temp1;

    	temp1.s0 +=addConst.s0;
    	temp1.s1 +=addConst.s1;
    	temp1.s2 +=addConst.s2;
    	temp1.s3 +=addConst.s3;
    	temp1.s4 +=addConst.s4;
    	temp1.s5 +=addConst.s5;
    	temp1.s6 +=addConst.s6;
    	temp1.s7 +=addConst.s7;

    }



    blockWr++;
    checkStatus.s0 = ((ulong)blockWr)<<32     | 0xAA56;

    pStatus[0] = checkStatus;
    pStatus[1] = temp1;

}


