

bool	check_data64( ulong data_i, ulong expect );


__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void check_cnt(__global ulong8    * __restrict input0,
               __global uint16    * __restrict pStatus,
			   const 	ulong8   expect,
			   const 	uint  size
			  )
{

//	   const 	ulong expect=1;
//	   const 	uint  size=1024;

    ulong 	ii;
    uint  	flagError=0;
    uint	blockRd;
    uint	blockOk;
    uint	blockError;

    ulong8 	temp0, temp1;
    //ulong	temp_expect=expect;

    uint16	checkStatus;

    bool	flag0;
    bool	flag1;
    bool	flag2;
    bool	flag3;
    bool	flag4;
    bool	flag5;
    bool	flag6;
    bool	flag7;


    checkStatus = *pStatus;

    blockRd 	= checkStatus.s1;
    blockOk 	= checkStatus.s2;
    blockError	= checkStatus.s3;


    temp1 = expect;

    printf( "krnl:  input0=%p\n", input0 );
    printf( "krnl:  pStatus=%p\n", pStatus);
    printf( "krnl:  size=%d\n", size);

    //for( int ii=0; ii<8; ii++ )
    	printf( "krnl:  expect(%d)= %ld\n", 0, temp1.s0 );
    	printf( "krnl:  expect(%d)= %ld\n", 1, temp1.s1 );
    	printf( "krnl:  expect(%d)= %ld\n", 2, temp1.s2 );
    	printf( "krnl:  expect(%d)= %ld\n", 3, temp1.s3 );
    	printf( "krnl:  expect(%d)= %ld\n", 4, temp1.s4 );
    	printf( "krnl:  expect(%d)= %ld\n", 5, temp1.s5 );
    	printf( "krnl:  expect(%d)= %ld\n", 6, temp1.s6 );
    	printf( "krnl:  expect(%d)= %ld\n", 7, temp1.s7 );


//    __attribute__((xcl_pipeline_loop))
//
    for (ii=0; ii<size; ii++)
    {
//
    	temp0 = input0[ii];

    	flag0 = check_data64( temp0.s0, temp1.s0 );
    	flag1 = check_data64( temp0.s1, temp1.s1 );
    	flag2 = check_data64( temp0.s2, temp1.s2 );
    	flag3 = check_data64( temp0.s3, temp1.s3 );
    	flag4 = check_data64( temp0.s4, temp1.s4 );
    	flag5 = check_data64( temp0.s5, temp1.s5 );
    	flag6 = check_data64( temp0.s6, temp1.s6 );
    	flag7 = check_data64( temp0.s7, temp1.s7 );

    	if( flag0 || flag1 || flag2 || flag3 || flag4 || flag5 || flag6 || flag7 )
    		flagError=1;

    	temp1.s0 +=8;
    	temp1.s1 +=8;
    	temp1.s2 +=8;
    	temp1.s3 +=8;
    	temp1.s4 +=8;
    	temp1.s5 +=8;
    	temp1.s6 +=8;
    	temp1.s7 +=8;
    }

    if( flagError )
    	blockError++;
    else
    	blockOk++;

    blockRd++;
    checkStatus.s0 = 0xAA55;
    checkStatus.s1 = blockRd;
    checkStatus.s2 = blockOk;
    checkStatus.s3 = blockError;

    *pStatus = checkStatus;
    printf( "krnl - Ok\n" );
}



bool	check_data64( ulong data_i, ulong expect )
{
	bool ret;

	if( data_i==expect )
		ret=0;
	else
		ret=1;

	return ret;
}
