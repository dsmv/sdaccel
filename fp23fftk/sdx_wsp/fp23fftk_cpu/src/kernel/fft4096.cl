
#include "fp_op.h"

pipe uint p0_pipe  __attribute__((xcl_reqd_pipe_depth(512)));
pipe uint p1_pipe  __attribute__((xcl_reqd_pipe_depth(32)));
pipe uint p2_pipe  __attribute__((xcl_reqd_pipe_depth(32)));
pipe uint p3_pipe  __attribute__((xcl_reqd_pipe_depth(512)));

/**
 * 	\brief	Get data from global memory
 */
__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void input_stage(
				__global uint    *src
			  )
{
	__attribute__((xcl_pipeline_loop))
    for (int ii = 0 ; ii < N_FFT ; ii++)
	{
		write_pipe_block( p0_pipe,  &src[ii] );
	}
}

/**
 * 	\brief	Forward FFT
 *
 */
__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void fp23_fft4096( uint mode )
{
	if( 0==mode )
	{	// Skip data
		for( int ii=0; ii<N_FFT; ii++ )
		{
			uint val;
			read_pipe_block( &val, p0_pipe );
			write_pipe_block( p1_pipe,  &val );
		}
	} else
	{
		// Calculate data
		for( int ii=0; ii<N_FFT; ii++ )
		{
			uint val=0xA0000+ii;
			write_pipe_block( p1_pipe,  &val );
		}

	}
}

/**
 * 	\brief	Multiplication of image
 */
__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void fp23_mult( uint mode )
{
	if( 0==mode )
	{	// Skip data
		for( int ii=0; ii<N_FFT; ii++ )
		{
			uint val;
			read_pipe_block( &val, p1_pipe );
			write_pipe_block( p2_pipe,  &val );

		}
	} else
	{
		// Calculate data
		for( int ii=0; ii<N_FFT; ii++ )
		{
			uint val=0xB0000+ii;
			write_pipe_block( p2_pipe,  &val );
		}

	}

}

/**
 * 	\brief	Inverse FFT
 */
__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void fp23_inv_fft4096( uint mode )
{
	if( 0==mode )
	{	// Skip data
		for( int ii=0; ii<N_FFT; ii++ )
		{
			uint val;
			read_pipe_block( &val, p2_pipe );
			write_pipe_block( p3_pipe,  &val );
		}
	} else
	{write
		// Calculate data
		for( int ii=0; ii<N_FFT; ii++ )
		{
			uint val=0xC0000+ii;
			write_pipe_block( p3_pipe,  &val );
		}

	}

}


/**
 * 	\brief	Put data to global memory
 */
__kernel
__attribute__ ((reqd_work_group_size(1,1,1)))
void output_stage(
				__global uint    *dst
				)
{
	__attribute__((xcl_pipeline_loop))
    for (int ii = 0 ; ii < N_FFT ; ii++)
	{
		read_pipe_block( &dst[ii], p3_pipe );
	}
}

