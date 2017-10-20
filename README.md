# SDAccel examples

## check_transfer

Program for check data transfer between host and Xilinx OpenCL device. 
  
Directory:
* *bin* - executable for QT application
* *qt*  - Qt application
* *sdx_wsp* - SDAccel application


There are two ways for build host application - Qt and SDAccell.
SDAccel used for primary debug OpenCL kernel and create simple console application.
Qt may be used for more complex application but this project used same code for Qt and SDAccel.

There are two test class:
* *TF_CheckTransferOut* - check data transfer from HOST to device
* *TF_CheckTransferIn*  - check data transfer from device to HOST


Program have two work mode:
* with test table	- for direct launch and launch from Qt 
* without test table	- for launch from SDAccel 

Test table is a text table with status information:
* *Time*    - name of test and test time
* *BlockWr* - count of written blocks
* *BlockRd* - count of read blocks
* *BlockOk* - count of correct blocks
* *BlockError* - count of incorrect blocks
* *SPD_CURRENT* - velocity of data transfer for last 4 seconds
* *SPD_AVR* - velocity of data transfer from test start

Arguments:
* -mode   <mode>        : 1 - check input, 2 - check output, 3 - check input and output
* -table  <flag_show>	: 1 - show table, 0 -	do not show table, default 0
* -time   <time> 	: execution time [s], default 10
* -file	  <fullpath>	: fullpath for xclbin 
* -size   <size>	: size block of megabytes, default 64
* -metric <flag>	: 0 - binary:  1MB=2^10=1024*1024=1048576 bytes
                          1 - decimal: 1MB=10^6=1000*1000=1000000 bytes

Doxygen documentation: https://dsmv.github.io/sdaccel/check_transfer/index.html

Project page: https://github.com/dsmv/sdaccel/projects/1

Wiki: https://github.com/dsmv/sdaccel/wiki

Deveopment Notes: https://github.com/dsmv/sdaccel/wiki/Development-Notes


