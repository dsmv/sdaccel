/***************************************************
*
* UTYPES.H - define usefull types.
*
* (C) Instrumental Systems Corp. Ekkore, Dec. 1997-2001
*
****************************************************/


#ifndef _UTYPES_H_
#define _UTYPES_H_




typedef	cl_uchar	UINT08;
typedef	cl_char		SINT08;
typedef	cl_ushort	UINT16;
typedef	cl_short	SINT16;
typedef	cl_uint		UINT32;
typedef	cl_int		SINT32;

typedef	cl_uchar	U08, *PU08;
typedef	cl_char		S08, *PS08;
typedef	cl_ushort	U16, *PU16;
typedef	cl_short	S16, *PS16;
typedef	cl_uint		U32, *PU32;
typedef	cl_int		S32, *PS32;
typedef	cl_ulong	U64, *PU64;
typedef	cl_long		S64, *PS64;

typedef float			REAL32, *PREAL32;
typedef double			REAL64, *PREAL64;

//typedef enum{ FALSE=0, TRUE=1}        BOOL;
typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;
typedef unsigned long   HANDLE;


typedef char			BRDCHAR;
#define _BRDC(x)		x
#define BRDC_strlen		strlen
#define BRDC_strcpy		strcpy
#define BRDC_strncpy	strncpy
#define BRDC_strcmp		strcmp
#define BRDC_stricmp	_stricmp
#define BRDC_strnicmp	_strnicmp
#define BRDC_strcat		strcat
#define BRDC_strchr		strchr
#define BRDC_strstr		strstr
#define BRDC_strtol		strtol
#define BRDC_strtod		strtod
#define BRDC_atol		atol
#define BRDC_atoi		atoi
#define BRDC_atoi64		_atoi64
#define BRDC_atof		atof
#define BRDC_printf		printf
#define BRDC_sprintf	sprintf
#define BRDC_vsprintf	vsprintf
#define BRDC_sscanf		sscanf
#define BRDC_fopen		fopen
#define BRDC_sopen		sopen
#define BRDC_fgets		fgets
#define BRDC_getenv		getenv
#define BRDC_main		main




#endif /* _UTYPES_H_ */

/*
*  End of File
*/



