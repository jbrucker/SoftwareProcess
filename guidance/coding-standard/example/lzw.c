/*
Nysiis Solutions Co.,Ltd. (http://www.nysiissolutions.com)

proprietary data
Copyright 2009 Nysiis Solutions Co.,Ltd.
All Rights Reserved

This document contains trade secret data which is the property of Nysiis Solutions Co.,Ltd.
This document is submitted to recipient in confidence.  Information contained here may not be used,
copied or disclosed in whole or in part except as permitted by written agreement signed by
authorized person of Nysiis Solutions Co.,Ltd.
*/
/**
@file			lzw.c

LZW Fundamentals

The original Lempel Ziv approach to data compression was first published in in 1977, followed by an alternate
approach in 1978. Terry Welch's refinements to the 1978 algorithm were published in 1984. The algorithm is
surprisingly simple. In a nutshell, LZW compression replaces strings of characters with single codes.
It does not do any analysis of the incoming text. Instead, it just adds every new string of characters it
sees to a table of strings. Compression occurs when a single code is output instead of a string of characters.

The code that the LZW algorithm outputs can be of any arbitrary length, but it must have more bits in it than
a single character. The first 256 codes (when using eight bit characters) are by default assigned to the
standard character set. The remaining codes are assigned to strings as the algorithm proceeds. The sample
program runs as shown with 12 bit codes. This means codes 0-255 refer to individual bytes, while codes
256-4095 refer to substrings.

PROJECT:		ThaiEngine\n
MODULE CODE:	LZW\n

@version
	1 2009-03-09 Sarayut Chaisuriya Create at version 3.0
		based on http://marknelson.us/attachments/1989/lzw-data-compression/stream_lzw.c\n
@sa
	Ref#1 http://marknelson.us/attachments/1989/lzw-data-compression/stream_lzw.c
*/
#define _LZW_C
#include <limits.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "defs.h"
#include "common.h"

/*************************************************************************
 *                                Local Macro                            *
 *************************************************************************/
#define MODULEID		(202)
#define LZW_SIZE_GROUP	(MAX_MODULE)
/*************************************************************************
 *                            Private Variables                          *
 *************************************************************************/
static LOG_HANDLE		*pLzwLog=(LOG_HANDLE *)NULL;
static pthread_mutex_t	lzwMutex;
static boolean			bInitialized=false;

/*************************************************************************
 *                            Protect Variables                          *
 *************************************************************************/
/*************************************************************************
 *                            Public Variables                           *
 *************************************************************************/
/*************************************************************************
 *                       Private Function Prototype                      *
 *************************************************************************/
static int32_t lzw_getTabSize
	(
		const uint16_t		/*ui16NumBit*/
	);
static int16_t lzw_readCode
	(
		LZW_HANDLE			* /*pLzw*/
	);

static int16_t lzw_addTranslation
	(
		LZW_HANDLE				* /*pLzw*/,
		const STRING_PASCAL		* /*pStrpsNew-new entry to be added */
	);
static int16_t lzw_putString
	(
		LZW_HANDLE			* /*pLzw*/,
		STRING_PASCAL		* /*pStrps*/
	);
static int16_t lzw_clearTable
	(
		LZW_HANDLE			* /*pLzw*/
	);
/*************************************************************************
 *                     1000 - Private Functions                          *
 *************************************************************************/
/**
@brief		BogdanB addon by adapting the original

@retval		>=0		SUCCESS, including EOF\n
@retval		-1		error

@version
	1 2009-04-09 Sarayut migrate\n
*/
//#define debug
//#define debug2
static int16_t
lzw_readCode
	(
		LZW_HANDLE			*pLzw
	)
{
#define FUNCTIONID		(1000)
	uint16_t		ui16RetValue;
	uint32_t		ui32Acc;
	/*
	 * Validation
	 */
#ifdef debug
	printf("lzw_readCode()..");	fflush(stdout);
#endif
	if ( pLzw == (LZW_HANDLE *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pLzw", labErrorExit10)
	if (	( pLzw->sInputIdx >= pLzw->sInputSize )
		&&	( pLzw->ui8InputBitCount == 0 )
		)
		goto labEOFExit10;
#ifdef debug2
	printf("\n\tpLzw->ui8NumBitCurr=%" FMT_I8 "u\n", pLzw->ui8NumBitCurr);
#endif
	/*
	 * prepare accumulator
	 */
	while ( pLzw->ui8InputBitCount <= 24 )
	{
		ui32Acc						=  0x0FF & (uint32_t )pLzw->pui8DataIn[pLzw->sInputIdx++];
		pLzw->ui32InputAcc			= (pLzw->ui32InputAcc<<8) | ui32Acc;
		pLzw->ui8InputBitCount		+=  8;
#ifdef debug2
		printf("\t(ui32Acc=0x%08" FMT_I32 "X.pLzw->ui32InputAcc=0x%08" FMT_I32 "X.pLzw->ui8InputBitCount=%" FMT_I8 "u\n",
			ui32Acc, pLzw->ui32InputAcc, pLzw->ui8InputBitCount);
#endif
		if ( pLzw->sInputIdx >= pLzw->sInputSize )
/*	v<--*/	break;
	}
	/*
	 * generate output value
	 */
	if ( pLzw->ui8InputBitCount < pLzw->ui8NumBitCurr )
	{	/* last data, might be invalid code */
		ui16RetValue	= LZW_EOD;		/* force EOD */
	}
	else
	{
		ui16RetValue	= (uint16_t )(pLzw->ui32InputAcc >> (pLzw->ui8InputBitCount-pLzw->ui8NumBitCurr));
		if ( pLzw->ui8InputBitCount	>= pLzw->ui8NumBitCurr )
			pLzw->ui8InputBitCount	-= pLzw->ui8NumBitCurr;
		else
			pLzw->ui8InputBitCount	= 0;
		pLzw->ui32InputAcc	&= (0x0FFFFFFFF >> (32 - pLzw->ui8InputBitCount));
#ifdef debug2
	printf("\tpLzw->ui32InputAcc=%08" FMT_I32 "X, pLzw->ui8InputBitCount=%" FMT_I8 "u\n", pLzw->ui32InputAcc, pLzw->ui8InputBitCount);
#endif
	}
	/*
	 * Success end
	 */
#ifdef debug
	printf("..success 0x%04" FMT_I16 "X\n", ui16RetValue);	fflush(stdout);
#endif
	return (int16_t )ui16RetValue;
	/*
	 * Error handling
	 */
labErrorExit10:
#ifdef debug
	printf("..Failed\n");	fflush(stdout);
#endif
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
	return -1;
	/*
	 * Eof handling
	 */
labEOFExit10:
#ifdef debug
	printf("..EOF\n");	fflush(stdout);
#endif
	pLzw->ui1Eof	= 1;
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
	return 0;
#undef FUNCTIONID
}
//#undef debug2
//#undef debug
/**
@brief		calculate table size

 The string table size needs to be a prime number that is somewhat larger than 2**BITS.

@retval		0		SUCCESS\n
@retval		-1		FAILED\n

@version
	 1 2009-03-10 Sarayut Chaisuriya Create\n
*/
static int32_t
lzw_getTabSize
	(
		const uint16_t		ui16NumBit
	)
{
#define FUNCTIONID		(1010)
	int32_t	i32RetVal;

	if (	( ui16NumBit>=9 )
		&&	( ui16NumBit<=12 )
		)
		i32RetVal	= (4096-258+1);		/* 4096 = 2^12 */
	else if ( ui16NumBit==13 )
		i32RetVal	= (8192-258+1);		/* 8192 = 2^13 */
	else if ( ui16NumBit==14 )
		i32RetVal	= (16384-258+1);	/* 16384 = 2^14 */
	else
	{
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Num of bit out of scope", labErrorExit10)
		goto labErrorExit10;
	}
	return i32RetVal;
	/*
	 * Error handling
	 */
labErrorExit10:
	return -1;
#undef FUNCTIONID
}
/**
@brief		send a string to output

@retval		0		SUCCESS\n
@retval		-1		FAILED\n

@version
	 1 2009-04-11 Sarayut Chaisuriya Create\n
*/
//#define debug
static int16_t
lzw_putString
	(
		LZW_HANDLE			*pLzw,
		STRING_PASCAL		*pStrps
	)
{
#define FUNCTIONID		(1030)
	/*
	 * Validation
	 */
#ifdef debug
	printf("lzw_putString()-000 Data="); fflush(stdout);
	msc_printHexNoaddr(pStrps->pcData, pStrps->sLen);
	printf("...");
#endif
	if ( pLzw == (LZW_HANDLE *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pLzw", labErrorExit10)
	if ( pStrps == (STRING_PASCAL *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pStrps", labErrorExit10)
	/*
	 * output loop
	 */
	if ( cl_append(pLzw->pclOutBuffer, (void *)pStrps->pcData, (int )pStrps->sLen, MODULEID) != 0 )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "cl_append() failed", labErrorExit10)
	pLzw->sOutputSize	+= pStrps->sLen;
	/*
	 * Success
	 */
#ifdef debug
	printf("lzw_putString()-999 sLen=%" FMT_SIZE "u\n", pStrps->sLen); fflush(stdout);
#endif
	return 0;
	/*
	 * Error handling
	 */
labErrorExit10:
#ifdef debug
	printf("lzw_putString()-1999\n"); fflush(stdout);
#endif
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
	return -1;
#undef FUNCTIONID
}
//#undef debug
/**
@brief		translate the input code into STRING_PASCAL output using translation table

@retval		0		SUCCESS\n
@retval		-1		FAILED\n
@retval		+1		buffer full\n

@version
	 1 2009-03-10 Sarayut Chaisuriya Create\n
*/
//#define debug
static int16_t
lzw_getTranslation
	(
		LZW_HANDLE			*pLzw,
		const int16_t		i16Code,
		STRING_PASCAL		*pStrpsResult		/* result of translation */
	)
{
#define FUNCTIONID		(1040)
#define BASELINE		(258)
	char		c;
	STRING_PASCAL	*pStrpsTmp;
	/*
	 * Validation
	 */
#ifdef debug
	printf("lzw_getTranslation()-Code=%" FMT_I16 "d..", i16Code); fflush(stdout);
#endif
	if ( pLzw == (LZW_HANDLE *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pLzw", labErrorExit10)
	if ( pStrpsResult == (STRING_PASCAL *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pStrpsResult", labErrorExit10)
	/*
	 * translation logic
	 */
	if ( i16Code < 256 )
	{
		c	= (char )i16Code;
		strps_set(pStrpsResult, &c, 1);
#ifdef debug
		printf("%02X.", c);	fflush(stdout);
#endif
	}
	else if (	( i16Code==LZW_CLEAR_TABLE )	/* 256 */
			||	( i16Code==LZW_EOD )			/* 257 */
			)
	{
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Translation failed-found CLEAR_TABLE or EOD", labErrorExit10)
	}
	else if ( (int32_t )(i16Code-BASELINE) < pLzw->i32TableIdx )
	{
		if ( (pStrpsTmp=pLzw->tabTranslation[i16Code-BASELINE])==(STRING_PASCAL *)NULL )
			LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Translation failed-reference to empty entry", labErrorExit10)
		strps_cpy(pStrpsResult, pStrpsTmp);
#ifdef debug
		size_t sIdx;
		printf("[");
		for ( sIdx=0; sIdx<pStrpsResult->sLen; sIdx++ )
			printf("%02X.", pStrpsResult->pcData[sIdx]);	fflush(stdout);
		printf("]."); fflush(stdout);
#endif
	}
	else
	{
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Translation failed-Code exceed table limit", labErrorExit10)
	}
	/*
	 * Success
	 */
#ifdef debug
	printf(".success\n");	fflush(stdout);
#endif
	return 0;
	/*
	 * Error exit
	 */
labErrorExit10:
#ifdef debug
	printf(".error\n");	fflush(stdout);
#endif
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
	return -1;
#undef BASELINE
#undef FUNCTIONID
}
//#undef debug
/**
@brief  add new STRING_PASCAL into translation table

@retval		0		SUCCESS\n
@retval		-1		FAILED\n
@retval		+1		translation table already fulled\n

@version
	 1 2009-04-11 Sarayut Chaisuriya Create\n
*/
//#define debug2
static int16_t
lzw_addTranslation
	(
		LZW_HANDLE				*pLzw,
		const STRING_PASCAL		*pStrpsNew		/**< new entry to be added */
	)
{
#define FUNCTIONID		(1050)
#define FIRST_CODE		(258)
	STRING_PASCAL		*pStrpsDup=(STRING_PASCAL *)NULL;
	/*
	 * Validation
	 */
#ifdef debug
	printf("lzw_addTranslation()-000\n");
#endif
	if ( pLzw == (LZW_HANDLE *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pLzw input", labErrorExit10)
	if ( pStrpsNew == (STRING_PASCAL *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pStrpsNew input", labErrorExit10)
	/*
	 * Add logic
	 */
	if ( pLzw->i32TableIdx >= pLzw->i32TableSize )
		goto labFulledExit10;
	if ( (pStrpsDup=strps_dup(pStrpsNew, MODULEID))==(STRING_PASCAL *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "strps_dup() failed", labErrorExit10)
#ifdef debug2
	printf("lzw_addTranslation: at 0x%04" FMT_I32 "X(%" FMT_I32 "d) len=%" FMT_SIZE "d, value=",
			pLzw->i32TableIdx, pLzw->i32TableIdx, pStrpsDup->sLen);
	msc_printHexNoaddr(pStrpsDup->pcData, (int )pStrpsDup->sLen);
	printf("\n");
#endif
	if  (	( pLzw->ui8NumBitCurr == 9 )
		&&	( pLzw->i32TableIdx == (511-FIRST_CODE-1) )
		)
	{
#ifdef debug2
		printf(">>>EXPAND to 10 bit.\n"); fflush(stdout);
#endif
		pLzw->ui8NumBitCurr++;
	}
	else if  (	( pLzw->ui8NumBitCurr == 10 )
			&&	( pLzw->i32TableIdx == (1023-FIRST_CODE-1) )
			)
	{
#ifdef debug
		printf(">>>EXPAND to 11 bit."); fflush(stdout);
#endif
		pLzw->ui8NumBitCurr++;
	}
	else if  (	( pLzw->ui8NumBitCurr == 11 )
			&&	( pLzw->i32TableIdx == (2047-FIRST_CODE-1) )
			)
	{
#ifdef debug
		printf(">>>EXPAND to 12 bit."); fflush(stdout);
#endif
		pLzw->ui8NumBitCurr++;
	}
	pLzw->tabTranslation[pLzw->i32TableIdx++]	= pStrpsDup;

	/*
	 * Success
	 */
#ifdef debug
	printf("lzw_addTranslation()-999\n");
#endif
	return 0;
	/*
	 * Error exit
	 */
labErrorExit10:
#ifdef debug
	printf("lzw_addTranslation()-1999\n");
#endif
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
	return -1;
	/*
	 * Fulled exit
	 */
labFulledExit10:
#ifdef debug
	printf("lzw_addTranslation()-2999\n");
#endif
	return +1;
#undef FIRST_CODE
#undef FUNCTIONID
}
//#undef debug2
/**
@brief  clear the translation table, reset num bit to 9

@retval		0		SUCCESS\n
@retval		-1		FAILED\n

@version
	 1 2009-04-12 Sarayut Chaisuriya Create\n
*/
static int16_t
lzw_clearTable
	(
		LZW_HANDLE			*pLzw
	)
{
#define FUNCTIONID		(1060)
	int32_t		i32Idx;
	/*
	 * Validation
	 */
#ifdef debug
	printf("lzw_clearTable()-000\n");
#endif
	if ( pLzw == (LZW_HANDLE *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pLzw input", labErrorExit10)
	/*
	 * clear logic
	 */
	for ( i32Idx=0; i32Idx<pLzw->i32TableIdx; i32Idx++ )
	{
		if ( pLzw->tabTranslation[i32Idx] != (STRING_PASCAL *)NULL )
		{
			(void )strps_del(pLzw->tabTranslation[i32Idx], MODULEID);
			pLzw->tabTranslation[i32Idx]	= (STRING_PASCAL *)NULL;
		}
	}
	pLzw->i32TableIdx	= 0;
	pLzw->ui8NumBitCurr	= pLzw->ui8NumBitBeg;
	/*
	 * Success
	 */
#ifdef debug
	printf("lzw_clearTable()-999\n");
#endif
	return 0;
	/*
	 * Error exit
	 */
labErrorExit10:
#ifdef debug
	printf("lzw_clearTable()-1999\n");
#endif
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
	return -1;
#undef FUNCTIONID
}
/*************************************************************************
 *                    2000 - Publish Functions                           *
 *************************************************************************/
/**
@brief  Initialize

Init set#6

@retval		0		SUCCESS\n
@retval		-1		FAILED\n

@version
	 1 2009-03-09 Sarayut Chaisuriya Create\n
*/
int16_t
lzw_init(LOG_HANDLE *pLog)
{
#define FUNCTIONID		(2000)
	pLzwLog		= pLog;

	if ( bInitialized )		/* allow multiple call */
		return 0;
	/*
	 * Dependency test
	 */
	if ( ! mem_isInit() )
		LOG_WRITESYSLOG_SHOWCONSOLE_THENJUMP("not yet call mem_init()", labErrorExit5)
	if ( ! cl_isInit() )
		LOG_WRITESYSLOG_SHOWCONSOLE_THENJUMP("not yet call cl_init()", labErrorExit5)

	trd_initMutexRecursive(&lzwMutex);

	log_addMessage(pLzwLog, "lzw_init() success");
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_INFORMATION, 2);
	bInitialized	= true;
	return 0;
	/*
	 * Error handling
	 */
labErrorExit5:
	return -1;
#undef FUNCTIONID
}
/**
@brief	Terminate function

@retval		0		SUCCESS\n
@retval		-1		FAILED\n

@version
	 1 2009-03-09 Sarayut Chaisuriya Create\n
*/
int16_t
lzw_end(void)
{
#define FUNCTIONID		(2010)
	if ( ! bInitialized )
		return -1;

	trd_destroyMutexRecursive(&lzwMutex);

	log_addMessage(pLzwLog, "lzw_end()");
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_INFORMATION, 2);
	log_flush(pLzwLog);
	bInitialized	= false;
	return 0;
#undef FUNCTIONID
}
/**
@brief	test wether the module is alaready initialized

@retval		true	SUCCESS\n
@retval		false	FAILED\n

@version
	 1 2009-03-09 Sarayut Chaisuriya Create\n
*/
boolean
lzw_isInit(void)
{
#define FUNCTIONID		(2020)
	return  bInitialized;
#undef FUNCTIONID
}
/*************************************************************************
 *           2100 -  constructor/destructor Functions                    *
 *************************************************************************/
/**
@brief	LZW_HANDLE constructor

@retval		NOTNIL		success, pointer to newly create LZW_HANDLE objects\n
@retval		NIL			FAILED\n

@version
	 1 2009-04-08 Sarayut Chaisuriya Create\n
*/
LZW_HANDLE *
lzw_new
	(
		void			*pvdInput,
		const size_t	sInpSize
	)
{
#define FUNCTIONID		(2100)
	size_t			sSize;
	LZW_HANDLE		*pLzw=(LZW_HANDLE *)NULL;
	/*
	 * Parameter validate
	 */
	if ( pvdInput==(char *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "NIL pvdInput", labErrorExit10)
	/*
	 * allocate
	 */
	sSize	= sizeof(LZW_HANDLE);
	if ((pLzw=(LZW_HANDLE *)mem_alloc(sSize, MODULEID))==(LZW_HANDLE *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "mem_alloc()-pLzw failed", labErrorExit10)
	memset((void *)pLzw, 0, sSize);
	SETMEMINFO((void *)pLzw, "LZW_HANDLE object in lzw_new()");

	pLzw->pui8DataIn		= (uint8_t *)pvdInput;
	pLzw->sInputSize		= sInpSize;

	pLzw->ui8NumBitBeg		= 9;		/* according to PDF specification */
	pLzw->ui8NumBitEnd		= 12;		/* according to PDF specification */
	pLzw->ui8NumBitCurr		= pLzw->ui8NumBitBeg;
	/* prepare translation code */
	if ( (pLzw->i32TableSize=lzw_getTabSize(pLzw->ui8NumBitEnd))<0 )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_getTabSize() failed", labErrorExit10)
	sSize		= sizeof(TEXT *) * pLzw->i32TableSize;
	if ((pLzw->tabTranslation=(STRING_PASCAL **)mem_alloc(sSize, MODULEID))==(STRING_PASCAL **)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "mem_alloc()-tabTranslation failed", labErrorExit10)
	memset((void *)pLzw->tabTranslation, 0, sSize);
	SETMEMINFO((void *)pLzw, "pLzw->tabTranslation in lzw_new()");

	if ( (pLzw->pclOutBuffer=cl_getFreeList(MODULEID))==(CLIST *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "cl_getFreeList() failed", labErrorExit10)
	/*
	 * Success End
	 */
	return pLzw;
	/*
	 * Error handling
	 */
labErrorExit10:
	if ( pLzw != (LZW_HANDLE *)NULL )
	{
		if ( pLzw->tabTranslation != (STRING_PASCAL **)NULL )
		{
			sSize		= sizeof(STRING_PASCAL *) * pLzw->i32TableSize;
			(void )mem_free((void *)pLzw->tabTranslation, sSize, MODULEID);
		}
		if ( pLzw->pclOutBuffer != (CLIST *)NULL )
			(void )cl_returnAll(pLzw->pclOutBuffer, MODULEID);
		sSize	= sizeof(LZW_HANDLE);
		(void )mem_free((void *)pLzw, sSize, MODULEID);
	}
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
	return (LZW_HANDLE *)NULL;
#undef FUNCTIONID
}
/**
@brief	LZW_HANDLE destructor

@retval		0		success
@retval		-1		failed

@version
	 1 2009-04-08 Sarayut Chaisuriya Create\n
*/
int16_t
lzw_del
	(
		LZW_HANDLE		*pLzw
	)
{
#define FUNCTIONID		(2110)
	size_t		sSize;
	int32_t		i32Idx;

	if ( pLzw == (LZW_HANDLE *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pLzw", labErrorExit10)
	/*
	 * Free logic
	 */
	if ( pLzw->pclOutBuffer != (CLIST *)NULL )
		(void )cl_returnAll(pLzw->pclOutBuffer, MODULEID);
	if ( pLzw->tabTranslation != (STRING_PASCAL **)NULL )
	{
		for ( i32Idx=0; i32Idx<pLzw->i32TableIdx; i32Idx++ )
			(void )strps_del((STRING_PASCAL *)pLzw->tabTranslation[i32Idx], MODULEID);
		sSize		= sizeof(STRING_PASCAL *) * pLzw->i32TableSize;
		(void )mem_free((void *)pLzw->tabTranslation, sSize, MODULEID);
	}

	sSize	= sizeof(LZW_HANDLE);
	(void )mem_free((void *)pLzw, sSize, MODULEID);
	return 0;
	/*
	 * Error handling
	 */
labErrorExit10:
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
	return -1;
#undef FUNCTIONID
}
/**
@brief		expand process

@algorithm	The Decompression Algorithm

	Read OLD_CODE
	output OLD_CODE
	CHARACTER = OLD_CODE
	WHILE there are still input characters DO
		Read NEW_CODE
		IF NEW_CODE is not in the translation table THEN
			STRING = get translation of OLD_CODE
			STRING = STRING+CHARACTER
		ELSE
			STRING = get translation of NEW_CODE
		END of IF
		output STRING
		CHARACTER = first character in STRING
		add OLD_CODE + CHARACTER to the translation table
		OLD_CODE = NEW_CODE
	END of WHILE

@retval		0		success
@retval		-1		failed

@version
	 1 2009-04-08 Sarayut Chaisuriya Create\n
*/
//#define debug
//#define debug2
int16_t
lzw_expand
	(
		LZW_HANDLE		*pLzw
	)
{
#define FUNCTIONID		(2120)
#define FIRST_CODE		(258)
	int16_t		i16NewCode=0;
	int16_t		i16OldCode;
	int16_t		i16Character;
	char		c;

	STRING_PASCAL	*pStrpsString=(STRING_PASCAL *)NULL;
	STRING_PASCAL	*pStrpsTmp=(STRING_PASCAL *)NULL;

	/*
	 * validation
	 */
	if ( pLzw == (LZW_HANDLE *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "Nil pLzw", labErrorExit10)
	if ( (pStrpsString=strps_new(MODULEID))==(STRING_PASCAL *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "strps_new() failed", labErrorExit10)
	if ( (pStrpsTmp=strps_new(MODULEID))==(STRING_PASCAL *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "strps_new() failed", labErrorExit10)
	/*
	 * Initialize
	 */
#ifdef debug
	printf("lzw_expand()-000\n"); fflush(stdout);
#endif
	if ( pLzw->ui2ExecStatus != 0 )		/* status must be not started */
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "already processed", labErrorExit10)
	pLzw->ui1ProcessType	= 0;	/* 0=expand, 1=compress*/

	/*
	 * Read OLD_CODE
	 * output OLD_CODE
	 * CHARACTER = OLD_CODE
	 */
	if ( (i16OldCode=lzw_readCode(pLzw))<0 )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_readCode()-1 failed", labErrorExit10)
	if ( i16OldCode == LZW_CLEAR_TABLE )
	{
		if ( (i16OldCode=lzw_readCode(pLzw))<0 )
			LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_readCode()-2 failed", labErrorExit10)
	}
	c	= (char )i16OldCode;
	strps_set(pStrpsTmp, &c, 1);
	if ( lzw_putString(pLzw, pStrpsTmp) != 0 )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_putString()-1 failed", labErrorExit10)
	i16Character	= i16OldCode;
#ifdef debug
	printf("lzw_expand-99: NewCode 0x%04X, Character=%04X, OldCode=%04X, String=[", i16NewCode, i16Character, i16OldCode);
	msc_printHexNoaddr(pStrpsString->pcData, (int )pStrpsString->sLen);
	printf("]\n");
#endif
	while ( (i16NewCode=lzw_readCode(pLzw)) >= 0 )
	{
#ifdef debug2
		printf("lzw_expand-100: read 0x%04X, OldCode=%04X, String=[", i16NewCode, i16OldCode);
		msc_printHexNoaddr(pStrpsString->pcData, (int )pStrpsString->sLen);
		printf("], CurrOutSize=%" FMT_SIZE "u, Character=%04X\n", pLzw->sOutputSize, i16Character);
#endif
		if ( pLzw->ui1Eof )
/*	v<--*/	break;
		if ( i16NewCode==LZW_EOD )				/* 257 */
		{
#ifdef debug
			printf("Found EOD\n");
#endif
/*	v<--*/	break;
		}
		/* If NewCode = CLEAR_TABLE
		 *    Read OLD_CODE
		 *    output OLD_CODE
		 *    CHARACTER = OLD_CODE
		 */
		if ( i16NewCode==LZW_CLEAR_TABLE )		/* 256 */
		{
#ifdef debug
			printf("Found Clear Table\n");
#endif
			(void )lzw_clearTable(pLzw);
			if ( (i16OldCode=lzw_readCode(pLzw))<0 )
				LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_readCode()-3 failed", labErrorExit10)
			else if ( i16OldCode == LZW_EOD )
/*	v<------*/	break;
			c	= (char )i16OldCode;
			strps_set(pStrpsTmp, &c, 1);
			if ( lzw_putString(pLzw, pStrpsTmp) != 0 )
				LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_putString()-2 failed", labErrorExit10)
			i16Character	= i16OldCode;
#ifdef debug
			printf("lzw_expand-110: NewCode 0x%04X, Character=%04X, OldCode=%04X, String=[", i16NewCode, i16Character, i16OldCode);
			msc_printHexNoaddr(pStrpsString->pcData, (int )pStrpsString->sLen);
			printf("]\n");
#endif
/*	^<--*/	continue;
		}
		/* IF NEW_CODE is not in the translation table THEN
		 *	 STRING = get translation of OLD_CODE
		 *	 STRING = STRING+CHARACTER
		 * ELSE
		 *	 STRING = get translation of NEW_CODE
		 * END of IF
		 */
		else if ( (int32_t )i16NewCode >= (pLzw->i32TableIdx+FIRST_CODE) )
		{	/* new code is not in translation table */
			if ( lzw_getTranslation(pLzw, i16OldCode, pStrpsString) != 0 )
				LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_getTranslation()-old code failed", labErrorExit10)
			c	= (char )i16Character;
			if ( strps_appendChar(pStrpsString, c) != 0 )
				LOG_ADDMESSAGE_THENJUMP(pLzwLog, "strps_appendChar()-1 failed", labErrorExit10)
		}
		else
		{	/* i16NewCode < 256, also in translation table */
			if ( lzw_getTranslation(pLzw, i16NewCode, pStrpsString) != 0 )
				LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_getTranslation()-new code 2 failed", labErrorExit10)
		}
		/*
		 * output STRING
		 */
		if ( lzw_putString(pLzw, pStrpsString) != 0 )
			LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_putString()-3 failed", labErrorExit10)
		/*
		 * CHARACTER = first character in STRING
		 */
		i16Character	= (int16_t )strps_charAt(pStrpsString, 0);
		/*
		 * add OLD_CODE + CHARACTER to the translation table
		 */
		if ( lzw_getTranslation(pLzw, i16OldCode, pStrpsTmp) != 0 )
			LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_getTranslation()-old code 2 failed", labErrorExit10)
		c	= (char )i16Character;
		if ( strps_appendChar(pStrpsTmp, c) != 0 )
			LOG_ADDMESSAGE_THENJUMP(pLzwLog, "strps_appendChar()-2 failed", labErrorExit10)
		if ( lzw_addTranslation(pLzw, pStrpsTmp) != 0 )
			LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_addTranslation() failed", labErrorExit10)
		/* else noop */

		/* OLD_CODE = NEW_CODE */
#ifdef debug
		printf("lzw_expand-199: read 0x%04X, OldCode=%04X, String=[", i16NewCode, i16OldCode);
		msc_printHexNoaddr(pStrpsString->pcData, (int )pStrpsString->sLen);
		printf("], CurrOutSize=%" FMT_SIZE "u, Character=%04X\n", pLzw->sOutputSize, i16Character);
#endif
		i16OldCode	= i16NewCode;
	}
	if ( i16NewCode < 0 )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_readCode()-4 failed", labErrorExit10)
	/*
	 * Close & Clear
	 */
	pLzw->ui2ExecStatus	= 1;	/* success */
	if ( pStrpsString != (STRING_PASCAL *)NULL )
		(void )strps_del(pStrpsString, MODULEID);
	if ( pStrpsTmp != (STRING_PASCAL *)NULL )
		(void )strps_del(pStrpsTmp, MODULEID);
	/*
	 * Success end
	 */
#ifdef debug
	printf("lzw_expand()-999\n"); fflush(stdout);
#endif
	return 0;
	/*
	 * Error handling
	 */
labErrorExit10:
#ifdef debug
	printf("lzw_expand()-1999\n"); fflush(stdout);
#endif
	if ( pLzw != (LZW_HANDLE *)NULL )
	{
		pLzw->ui2ExecStatus	= 2;	/* failed */
	}
	if ( pStrpsString != (STRING_PASCAL *)NULL )
		(void )strps_del(pStrpsString, MODULEID);
	if ( pStrpsTmp != (STRING_PASCAL *)NULL )
		(void )strps_del(pStrpsTmp, MODULEID);
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
	return -1;
#undef FUNCTIONID
}
//#undef debug2
//#undef debug
/*************************************************************************
 *             2200 -  Generic uncompress Functions                      *
 *************************************************************************/
/**
@brief		uncompress memory data output on preapare memory area

@retval		>=0	success
@retval		-1	failed

@version
	1 2009-03-11 Sarayut migrate from Ref#1\n
*/
//#define debug
ssize_t
lzw_memExpand4PDF
	(
		char			*pcOutput,
		size_t			sOutLimit,
		const char		*pcInput,
		const size_t	sInpSize
	)
{
#define FUNCTIONID		(2200)
	LZW_HANDLE		*pLzw=(LZW_HANDLE *)NULL;
	ssize_t			ssRetSize=0;
#ifdef debug
	printf("lzw_memExpand4PDF()-000\n"); fflush(stdout);
#endif
	/*
	 * Validation
	 */
	if ( ! bInitialized )
		LOG_ADDMESSAGE_THENJUMP(log_pSysLog, "not yet call lzw_init()", labErrorExit5)

	if ( pcInput==(char *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "NIL pcInput input", labErrorExit10)
	if ( pcOutput==(char *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "NIL pcOutput input", labErrorExit10)
	/*
	 * Initialize
	 */
#ifdef debug
	printf("lzw_memExpand4PDF-10-Expanding %" FMT_SIZE "u bytes:\n", sInpSize); fflush(stdout);
	if ( sInpSize > 512 )
		msc_printHex(pcInput, 512, 24);
	else
		msc_printHex(pcInput, (int )sInpSize, 24);
#endif
	if ( (pLzw=lzw_new((void *)pcInput, sInpSize))==(LZW_HANDLE *)NULL )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_new() failed", labErrorExit10)
	/*
	 * Basic flow
	 */
	if ( lzw_expand(pLzw) != 0 )
		LOG_ADDMESSAGE_THENJUMP(pLzwLog, "lzw_expand() failed", labErrorExit10)
#ifdef debug
	printf("sOutputSize=%" FMT_SIZE "u\n", pLzw->sOutputSize);
#endif
	if ( pLzw->sOutputSize > 0 )
	{
		ssRetSize	= (ssize_t )cl_getContent(pLzw->pclOutBuffer, pcOutput, sOutLimit);
#ifdef debug
		printf("after cl_getContent ssRetSize=%" FMT_SIZE "d Result Data=\n", ssRetSize);
		msc_printHex((void *)pcOutput, (int )ssRetSize, 24);
#endif
	}
	/*
	 * Close & Clear
	 */
	if ( pLzw != (LZW_HANDLE *)NULL )
		(void )lzw_del(pLzw);
	/*
	 * success end
	 */
#ifdef debug
	printf("\nlzw_memExpand4PDF()-999\n"); fflush(stdout);
#endif
	return ssRetSize;
	/*
	 * Error handling
	 */
labErrorExit10:
#ifdef debug
	printf("lzw_memExpand4PDF()-1999\n"); fflush(stdout);
#endif
	if ( pLzw!=(LZW_HANDLE *)NULL )
	{
		(void )lzw_del(pLzw);
	}
	log_print(pLzwLog, MODULEID, FUNCTIONID, LOG_SEVERITY_ERROR, 0);
labErrorExit5:
	return (ssize_t )-1;
#undef FUNCTIONID
}
//#undef debug
/*** end of lzw.c ***/
