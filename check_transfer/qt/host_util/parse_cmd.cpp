/*
 * parse_cmd.cpp
 *
 *  Created on: Sep 4, 2017
 *
 * 		Functions for parse command line
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse_cmd.h"


/**
 * 	\brief 	Get integer value from command line
 *
 * 	format command line:
 * 	<name1> <value1> <name2> <value2>
 *
 * 	\param	argc		number of argument
 * 	\param	argv		pointers to arguments
 * 	\param	name		key of argument
 * 	\param	defValue	default value for arguments
 *
 * 	\return   value of argument or default value of argument
 */
int GetFromCommnadLine(int argc, char **argv, const char* name, int defValue)
{
    int ret=defValue;
    for( int ii=1; ii<argc-1; ii++ )
    {
        if( 0==strcmp( argv[ii], name) )
        {
            ret=atoi( argv[ii+1] );
        }
    }
    return ret;
}


/**
 *  \brief  Get string value from command line
 *
 * 	format command line:
 * 	<name1> <text1> <name2> <text2>
 *
 * 	\param	argc		number of argument
 * 	\param	argv		pointers to arguments
 * 	\param	name		key of argument
 * 	\param	defValue	default value for arguments
 * 	\param	dst			pointer to destination string, NULL - do not copy
 * 	\param  dstLen		max number of chars in the dst
 *
 * 	\return   0 - name not found, 1 - found only name, 2 - found name and text
 */
 int GetStrFromCommnadLine(int argc, char **argv, const char* name, char* defValue, char* dst, int dstLen )
{
	int ret=0;
	int index=0;
	for( int ii=1; ii<argc; ii++ )
    {
        if( 0==strcmp( argv[ii], name) )
        {
        	if( ii==(argc-1) )
        		ret=1;
        	else
        	{
        		ret=2; index=ii;
        	}

        	break;
        }
    }
	if( NULL!=dst )
	{
		if( 2==ret )
			strncpy( dst, argv[index+1], dstLen );
		else
			strncpy( dst, defValue, dstLen );
		dst[dstLen-1]=0;
	}
	return ret;
}
