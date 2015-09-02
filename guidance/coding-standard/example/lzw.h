/*
Nysiis Solutions Co.,Ltd. (http://www.nysiissolutions.com)

proprietary data
Copyright 2008 Nysiis Solutions Co.,Ltd.
All Rights Reserved

This document contains trade secret data which is the property of Nysiis Solutions Co.,Ltd.
This document is submitted to recipient in confidence.  Information contained here may not be used,
copied or disclosed in whole or in part except as permitted by written agreement signed by
authorized person of Nysiis Solutions Co.,Ltd.
*/
/**
@file			lzw.h

PROJECT:		ThaiEngine\n
MODULE CODE:	LZW\n

@version
	1 2009-03-09 Sarayut Chaisuriya Create at version 3.0\n
*/
#ifndef _LZW_H
#define _LZW_H
/*
 * public macro
 */
#define LZW_CLEAR_TABLE		(256)
#define LZW_EOD				(257)
/**
@brief		LZW compression handling

@version
	1 2009-04-06 Sarayut create\n
*/
typedef struct
{
	uint8_t		ui2ExecStatus:2;	/**< 0=not start, 1=success, 2=failed */
	uint8_t		ui1ProcessType:1;	/**< 0=expand, 1=compress */
	uint8_t		ui1Eof:1;			/**< input reach EOF? */
	uint8_t		ui1OutputFulled:1;	/**< not enough output space */
	uint8_t		_fillter:3;

	uint8_t		ui8NumBitCurr;		/**< current num encoding bit, for PDF we start at 9 bits */
	uint8_t		ui8NumBitBeg;		/**< num encoding bit - use at beginning */
	uint8_t		ui8NumBitEnd;		/**< num encoding bit - max */

	uint8_t		ui8InputBitCount;	/**< num input bits in ui32InputAcc */
	size_t		sInputSize;
	size_t		sOutputSize;		/**< actual output size in bytes */
	size_t		sInputIdx;			/**< current input index */
	uint32_t	ui32InputAcc;		/**< input accumulator */
	int32_t		i32TableSize;		/**< num entry in tabCode */
	int32_t		i32TableIdx;		/**< current index of tabCode */
	uint8_t		*pui8DataIn;		/**< input data, not free() at destroy */

	CLIST			*pclOutBuffer;		/**< output buffer (unbound) */
	STRING_PASCAL	**tabTranslation;	/**< translation table */
} LZW_HANDLE;
/*
 * protect variables declaration for external modules
 */
#ifndef _LZW_C
#endif

/*
 * public variables declaration for external modules
 */
#ifndef _LZW_C
#endif

/*
 * Public function declarations
 */
/*** init/terminate ***/
int16_t		lzw_end(void);
int16_t		lzw_init(LOG_HANDLE * /*pLog*/);
boolean		lzw_isInit(void);
LZW_HANDLE * lzw_new
	(
		void			* /*pvdInput*/,
		const size_t	/*sInpSize*/
	);
int16_t lzw_del
	(
		LZW_HANDLE		* /*pLzw*/
	);
int16_t lzw_expand
	(
		LZW_HANDLE		* /*pLzw*/
	);
/*** generic function ***/
ssize_t lzw_memExpand4PDF
	(
		char			* /*pcOutput*/,
		size_t			/*sOutSize*/,
		const char		* /*pcInput*/,
		const size_t	/*sInpSize*/
	);

#endif
/*** end of lzw.h ***/
