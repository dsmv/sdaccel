
#define BUFFER_SIZE 1024
#define LOOP_SIZE   1024

static bool	check_data64( ulong data_i, ulong expect );

static void check_data( __global ulong8 *pStatus, uint size );

static void read_input(__global ulong8 *in, uint size);

pipe ulong8 pipe_input __attribute__((xcl_reqd_pipe_depth(512)));


__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void check_read_input(
				__global ulong8    *src,
				const 	uint  size
			  )
{
	   read_input(src, size);
}


__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void check_cnt_m2a(
				__global uint16    *pStatus,
				const 	uint  size
			  )
{
	   check_data( pStatus, size );

}

//__kernel
//__attribute__ ((reqd_work_group_size(1,1,1)))
//__attribute__ ((xcl_dataflow))
//void check_cnt_m2b(
//				__global ulong8    *src,
//				__global uint16    *pStatus,
//				const 	ulong8   expect,
//				const 	uint  size
//			  )
//{
//
//	   read_input(src, size);
//	   check_data( pStatus, expect, size );
//
//}


void read_input(__global ulong8 *in, uint size)
{
	__attribute__((xcl_pipeline_loop))
    for (int ii = 0 ; ii < size ; ii++)
	{
		write_pipe_block( pipe_input,  &in[ii] );
	}
}


//__attribute__ ((xcl_dataflow))
static void check_data(  __global ulong8 *pStatus, uint size )
{

    uint  	flagError=0;
    uint	blockRd;
    uint	blockOk;
    uint	blockError;


    ulong8 	temp0, temp1;

    ulong8	checkStatus;

    bool	flag0=0;
    bool	flag1=0;
    bool	flag2=0;
    bool	flag3=0;
    bool	flag4=0;
    bool	flag5=0;
    bool	flag6=0;
    bool	flag7=0;

    bool 	word_error;
    uint	cnt_error=0;
    ulong8	addConst;

    //__global ulong8	*pStatusUlong = (ulong8*)pStatus;

    checkStatus = pStatus[0];

    temp1     	= pStatus[1];
    addConst 	= pStatus[2];

    blockRd 	= checkStatus.s0 >> 32;
    blockOk 	= checkStatus.s1 & 0xFFFFFFFF;
    blockError	= checkStatus.s1 >> 32;
    //printf( "krnl - %.4x %d %d %d \n", checkStatus.s0, blockRd, blockOk, blockError );



//    printf( "krnl:  input0=%p\n", input0 );
//    printf( "krnl:  pStatus=%p\n", pStatus);
//    printf( "krnl:  size=%d\n", size);
//
//    //for( int ii=0; ii<8; ii++ )
//    	printf( "krnl:  expect(%X)= %lX\n", 0, temp1.s0 );
//    	printf( "krnl:  expect(%X)= %lX\n", 1, temp1.s1 );
//    	printf( "krnl:  expect(%X)= %lX\n", 2, temp1.s2 );
//    	printf( "krnl:  expect(%X)= %lX\n", 3, temp1.s3 );
//    	printf( "krnl:  expect(%X)= %lX\n", 4, temp1.s4 );
//    	printf( "krnl:  expect(%X)= %lX\n", 5, temp1.s5 );
//    	printf( "krnl:  expect(%X)= %lX\n", 6, temp1.s6 );
//    	printf( "krnl:  expect(%X)= %lX\n", 7, temp1.s7 );


  __attribute__((xcl_pipeline_loop))
  for ( int ii=0; ii<size; ii++)
    {

    	//word_error=0;
    	read_pipe_block( pipe_input, &temp0 );

    	{
			flag0 |= check_data64( temp0.s0, temp1.s0 );
			flag1 |= check_data64( temp0.s1, temp1.s1 );
			flag2 |= check_data64( temp0.s2, temp1.s2 );
			flag3 |= check_data64( temp0.s3, temp1.s3 );
			flag4 |= check_data64( temp0.s4, temp1.s4 );
			flag5 |= check_data64( temp0.s5, temp1.s5 );
			flag6 |= check_data64( temp0.s6, temp1.s6 );
			flag7 |= check_data64( temp0.s7, temp1.s7 );
    	}

//    	if( flag0 || flag1 || flag2 || flag3 || flag4 || flag5 || flag6 || flag7 )
//    	{
//    		flagError=1;
//    		word_error=1;
//    		cnt_error++;
//
//    	}


//    	if( word_error && cnt_error<4 )
//    	{
//    		printf( "0: %.4X %.4X %.4X %.4X %.4X %.4X %.4X %.4X ii=%d\n",
//    				temp0.s0,
//    				temp0.s1,
//    				temp0.s2,
//    				temp0.s3,
//    				temp0.s4,
//    				temp0.s5,
//    				temp0.s6,
//    				temp0.s7,
//					ii
//					);
//
//			printf( "1: %.4X %.4X %.4X %.4X %.4X %.4X %.4X %.4X   %d\n\n",
//					temp1.s0,
//					temp1.s1,
//					temp1.s2,
//					temp1.s3,
//					temp1.s4,
//					temp1.s5,
//					temp1.s6,
//					temp1.s7,
//					word_error
//
//    		);
//    	}

    	temp1.s0 +=addConst.s0;
    	temp1.s1 +=addConst.s1;
    	temp1.s2 +=addConst.s2;
    	temp1.s3 +=addConst.s3;
    	temp1.s4 +=addConst.s4;
    	temp1.s5 +=addConst.s5;
    	temp1.s6 +=addConst.s6;
    	temp1.s7 +=addConst.s7;

    }

	if( flag0 || flag1 || flag2 || flag3 || flag4 || flag5 || flag6 || flag7 )
	{
		flagError=1;
	}


    if( flagError )
    	blockError++;
    else
    	blockOk++;

    blockRd++;
    checkStatus.s0 = ((ulong)blockRd)<<32     | 0xAA55;
    checkStatus.s1 = ((ulong)blockError)<<32  | blockOk;

    pStatus[0] = checkStatus;
    pStatus[1] = temp1;

    //printf( "krnl - %.4x %d %d %d - Ok\n", checkStatus.s0, blockRd, blockOk, blockError );
}


__attribute__((xcl_pipeline_loop))
static bool	check_data64( ulong data_i, ulong expect )
{
	bool ret;

	if( data_i==expect )
		ret=0;
	else
		ret=1;

	return ret;
}
