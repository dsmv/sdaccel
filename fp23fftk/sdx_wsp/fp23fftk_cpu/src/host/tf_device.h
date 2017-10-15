/*
 * tf_device.h
 *
 *  Created on: Sep 4, 2017
 *      Author: user52
 */

#ifndef SRC_HOST_TF_DEVICE_H_
#define SRC_HOST_TF_DEVICE_H_

#include "xcl2.hpp"

/**
*    \brief common data for OpenCL device
*
*/
class TF_Device {
public:
	TF_Device( int argc, char** argv );
	virtual ~TF_Device();

    std::string deviceName;		//!< OpenCL device name

    std::string xclbinFileName;  //!< file name for container

    cl::Device  device;			//!< OpenCL device
    cl::Context context;		//!< OpenCL context
    cl::Program program;		//!< OpenCL program

};

#endif /* SRC_HOST_TF_DEVICE_H_ */
