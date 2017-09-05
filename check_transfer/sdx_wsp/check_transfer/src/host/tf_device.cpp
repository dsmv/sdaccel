/*
 * tf_device.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: user52
 */

#include <tf_device.h>
#include "parse_cmd.h"

/**
*
*  	\param	argc 	Number of arguments
* 	\parma	argv	Pointer of argumnts
*
* 	Arguments:
*
*	-file 	<path>          : fullpath for xclbin, default "../binary_container_1.xclbin"
*
*/
TF_Device::TF_Device( int argc, char** argv  )
{


	    printf( "Open device\n");
	    std::vector<cl::Device> devices = xcl::get_xil_devices();

	    device = devices[0];
	    deviceName = device.getInfo<CL_DEVICE_NAME>();
	    printf( "Device: %s\n", deviceName.c_str());

	    cl::Context context_i( device );
	    context = context_i;

	    char str[512];
	    GetStrFromCommnadLine( argc, argv, "-file", "../binary_container_1.xclbin", str, 510 );
	    xclbinFileName = str;

	    cl::Program::Binaries bins = xcl::import_binary_file( xclbinFileName.c_str() );
	    devices.resize(1);
	    cl::Program program_i(context, devices, bins);

	    program = program_i;



}

TF_Device::~TF_Device() {

}

