/*
 * parse_cmd.h
 *
 *  Created on: Sep 4, 2017
 *      Author: user52
 */

#ifndef SRC_HOST_UTIL_PARSE_CMD_H_
#define SRC_HOST_UTIL_PARSE_CMD_H_


//! Get integer value from command line
int GetFromCommnadLine(int argc, char **argv, const char* name, int defValue);

//! Get string value from command line
int GetStrFromCommnadLine(int argc, char **argv, const char* name, char* defValue, char* dst, int dstLen );



#endif /* SRC_HOST_UTIL_PARSE_CMD_H_ */
