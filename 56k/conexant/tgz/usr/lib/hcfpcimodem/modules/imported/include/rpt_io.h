/*
 * Copyright (c) 2003-2010 Linuxant inc.
 * Copyright (c) 2001-2010 Conexant Systems, Inc.
 * 
 * 1.   Permitted use. Redistribution and use in source and binary forms,
 * without modification, are only permitted under the terms set forth herein.
 * 
 * 2.   Disclaimer of Warranties. LINUXANT, ITS SUPPLIERS, AND OTHER CONTRIBUTORS
 * MAKE NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.
 * IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTIES OF ANY KIND.
 * LINUXANT AND OTHER CONTRIBUTORS DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, GOOD TITLE AND AGAINST INFRINGEMENT.
 * 
 * This software has not been formally tested, and there is no guarantee that
 * it is free of errors including, but not limited to, bugs, defects,
 * interrupted operation, or unexpected results. Any use of this software is
 * at user's own risk.
 * 
 * 3.   No Liability.
 * 
 * (a) Linuxant, its suppliers, or contributors shall not be responsible for
 * any loss or damage to users, customers, or any third parties for any reason
 * whatsoever, and LINUXANT, ITS SUPPLIERS OR CONTRIBUTORS SHALL NOT BE LIABLE
 * FOR ANY ACTUAL, DIRECT, INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL, OR
 * CONSEQUENTIAL (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED, WHETHER IN CONTRACT, STRICT OR OTHER LEGAL THEORY OF
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 * 
 * (b) User agrees to hold Linuxant, its suppliers, and contributors harmless
 * from any liability, loss, cost, damage or expense, including attorney's fees,
 * as a result of any claims which may be made by any person, including
 * but not limited to User, its agents and employees, its customers, or
 * any third parties that arise out of or result from the manufacture,
 * delivery, actual or alleged ownership, performance, use, operation
 * or possession of the software furnished hereunder, whether such claims
 * are based on negligence, breach of contract, absolute liability or any
 * other legal theory.
 * 
 * 4.   Notices. User hereby agrees not to remove, alter or destroy any
 * copyright, trademark, credits, other proprietary notices or confidential
 * legends placed upon, contained within or associated with the Software,
 * and shall include all such unaltered copyright, trademark, credits,
 * other proprietary notices or confidential legends on or in every copy of
 * the Software.
 * 
 * 5.   Reverse-engineering. User hereby agrees not to reverse engineer,
 * decompile, or disassemble the portions of this software provided solely
 * in object form, nor attempt in any manner to obtain their source-code.
 * 
 * 6.   Redistribution. Redistribution of this software is only permitted
 * for exact copies (without modification) of versions explicitly marked
 * and officially released by Linuxant with the word "free" in their name.
 * Redistribution or disclosure of other versions, derivatives or license key
 * information is expressly prohibited without explicit written approval signed
 * by an authorized Linuxant officer.
 * 
 * 7.   Performance. V.92 modems are designed to be capable of receiving data at
 * up to 56Kbps with compatible phone line and server equipment, and transmitting
 * data at up to 31.2Kbps. V.90 modems are designed to be capable of receiving
 * data at up to 56 Kbps from a compatible service provider and transmitting data
 * at up to about 28.8 Kbps. Public networks currently limit download speeds to
 * about 53Kbps. The free version of the drivers is limited to 14.4Kbps.
 * Actual speeds vary and are often less than the maximum possible.
 * 
 * 
 */

/****************************************************************************************
 *                     Version Control Information										*
 *                                                                                      *
 * $Header:   P:/d874/Background_Logger/blam/rtrace/libsrc/shared/rpt_io.h_v   1.7   11 Jul 2003 17:03:12   rayskymy  $
 * 
*****************************************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/* rpt_io.h - interface for BLAM tracing report functions */
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

#if !defined __RPT_IO_H__
#define __RPT_IO_H__

#ifdef __cplusplus
extern "C"
{ 
#endif

#if defined BLAM22

/* General definitions */
/*#include "my_typedef.h" */
/*
#ifndef FALSE
#define FALSE				0
#endif

#ifndef TRUE
#define TRUE				1
#endif

#ifndef WORD
typedef unsigned short		WORD;
#endif

#ifndef UINT32
typedef unsigned long		UINT32;
#endif

#ifndef DWORD
typedef unsigned long		DWORD;
#endif

#ifndef	PDWORD
typedef DWORD				*PDWORD;
#endif

#ifndef BOOL
typedef int					BOOL;
#endif

#ifndef BYTE
typedef unsigned char		BYTE;
#endif

#ifndef PBYTE
typedef BYTE				*PBYTE;
#endif
*/


/*Definitions added for new filtering mechanism */

/* dwGlobalSeverity will contain a bitmask received from the ring 3 app that specifies which */
/* severities of messages should be logged regardless of type. It is assumed shifted left into the */
/* high order byte for easy comparison. */

/* The dwGlobalLoggingThread component */
/* will contain a value set by the ring 3 app that specifies a single programmer defined "loggingthread" */
/* is to be logged regardless of message type or severity.   It is assumed shifted left into the 3rd */
/* byte (bits 16-23).  */

/* If neither severity or global thread causes */
/* the message to get logged, the ReportWrite routine checks the contents of RptMask -- the */
/* index into which IS the message type.  A 00 means DON'T LOG,  a FF means always LOG, a value */
/* between 1 and FE means LOG if and only if the Logging Thread in the RTRACE parameters match. */



#define LOGGING_THREAD_MASK     ((UINT32) 0x00FF0000)
#define GLOBAL_SEVERITY_MASK	((UINT32) 0xFF000000)
#define MESSAGE_TYPE_MASK	((UINT32) 0x000003FF)
#define YES_LOG_MESSAGE		((UINT32) 0x000000FF)


/*These Mask values identify special types of packets to be constructed by stealing the upper 48 values of the 11 */
/*bit "FileID" field in what is supplied to RPTWRTIE from macro calls.   */

/*SYNC Packet is special case, we are putting in all 5s (alternating 1s and 0s) everywhere in the  */
/*2 DWORDS except for the code (0x7FA) in bits 13-23 of the 1st DWORD.  This pattern can then be */
/*searched for by the BLAM app. */
#define BLAMSYNC_CODE 0x7FA
#define BLAMSYNC_MASK    ((UINT32)( (UINT32) BLAMSYNC_CODE << 13))
#define BLAMSYNC_PACKET_DW1 (BLAMSYNC_MASK | (UINT32) 0x15001555)
#define BLAMSYNC_PACKET_DW2  ((UINT32) 0x55555555)

#define STATPACK_CODE 0x7FB
#define STATPACK_MASK    ( (4<<29) | ((UINT32)( STATPACK_CODE << 13)) )

#define RTRACE_DUMP_CODE 0x7FC
#define RTRACE_DUMP_MASK  ((UINT32)( (UINT32) RTRACE_DUMP_CODE << 13))

#define RDUMP_EOS_CODE 0x7FD
#define RDUMP_EOS_MASK  ((UINT32)( (UINT32) RDUMP_EOS_CODE << 13))

#define RTRACE_EOS_CODE 0x7FE
#define RTRACE_EOS_MASK  ((UINT32)( (UINT32) RTRACE_EOS_CODE << 13))

#define RDUMP_CODE 0x7FF
#define RDUMP_MASK  ((UINT32)( (UINT32) RDUMP_CODE << 13))





/*For initialization of new BIT oriented types array */
#define BlamClientDW(type)			((type)/32)
#define BlamClientBit(type)			((type) - (BlamClientDW(type)*32))
#define DO_LOG_MSG_INIT(type)			((UINT32) (1<<(BlamClientBit(type))))
#define NO_LOG_MSG_INIT(type)			((UINT32) 0)   /*(0<<(BlamClientBit(type)))) */


#define	LINE_NUM_MASK			((UINT32) 0x00001FFF)	/*DH	BITS 0-12 */
#define	DUMP_SIZE_MASK			((UINT32) 0x00001FFF)	/*DH	BITS 0-12 */
#define FNUM_MASK			((UINT32) 0x00FFE000)	/*DH	BITS 13-23 */
#define PACKET_TYPE_MASK		FNUM_MASK  /*same field used for special packets, bits 13-23 */

/*#define VERSION_INFO_TIMECODE_MASK	((UINT32) 0x1F000000)	//DH	BITS 24-28 */
#define NUM_ARGS_MASK			((UINT32) 0xE0000000)	/*DH	BITS 29-31 */

#define RPT_MAX_ARGS 7

#define MAX_NUM_THREADS 255	/*current maximum number of logging threads */
#define SET_SPECIFIC_FILTER 3	/*Command to set a value for a particula type */
								/*00=OFF, FF==ON, else its a thread num */
#define SET_GLOBAL_THREAD   2	/*cmd to set the logging global logging thread */
#define SET_GLOBAL_SEVERITY 1	/*cmd to set the severity mask value */

#define NUM_MASK_DWORDS  32     /*size of RptMask array of bits (in DWORDS -- so 32 means 1024 bits). */

#define NUM_RPT_READS_PER_STATPACK  20  /*every 20 times, RPT_READ sends statpack */
#define NUM_BYTES_PER_SYNCPACKET 2048   /*after every 2K bytes, send SYNC packet */

#define MAX_DIR_PATH 100	/*DH */

typedef struct tagReportDescriptor
{
   UINT32 lnum			: 14;  /* 0 to 16K representing source code __LINE__ */
   UINT32 fnum			: 10;  /* 0 to 1023 File number */
   UINT32 version		: 3;   /* allows for varying report packet structure */
   UINT32 timefmt		: 2;   /* timecode format */
   UINT32 payload		: 3;   /* extra DWORDs after base report packet */
} REPORTDESCRIPTOR;


typedef struct tagReportRecord       /*TMW */
{
	UINT32 descriptor;
	UINT32 timestamp;
} REPORT_RECORD, *PREPORT_RECORD;

/*
typedef void (*RPT_Q_CRITSECTION)(BOOL);

#define RPT_RECORD_VERSION 0 // timecode ms, payload 0,1,2,3 UINT32

typedef struct tagMsgQueue
{
	UINT32  read;
	UINT32  write;
	UINT32  size;
	PUINT8 q;
   RPT_Q_CRITSECTION CriticalSection;
} MSGQUEUE, *PMSGQUEUE;

//
//typedef enum 
//{
//    RPT_STATUS_NOT_ASSIGNED,
//    RPT_STATUS_IS_ASSIGNED,
//} RPT_CLIENT_STATUS;
	
*/

typedef UINT32 (*RPT_CLIENT_TIME)(void);


#ifdef __GNUC__
#define _stdcall
#endif



typedef struct tagClientstruct
{
    void*  hQueue;                 /*handle to Octopus FIFO used for BLAM */
    BOOL   bActiveQueue;           /*true if FIFO is supposedly usable */
    RPT_CLIENT_TIME RPT_ClientGetElapsedTime;
    UINT32 dwBaseTime;
    UINT32 dwGlobalSeverity;
    UINT32 dwGlobalLoggingThread;
    UINT32 dwLostPacketCount;     /*Number of BLAM messages that would not fit into FIFO */
    UINT32 dwQueuedPacketCount;   /*Number of BLAM messages put into FIFO */
    UINT32 dwReadPacketCount;     /*Number of BLAM messages read from FIFO for transport to client */
    UINT32 dwNumBLAMBytesWritten; /*TOTAL number of bytes written to FIFO */
    UINT32 dwNumSyncBytesWritten; /*bytes written to FIFO since last SYNC packet queued */
    UINT32 dwStatPackCounter;     /*Keeps track of number of RPT_Read calls between sending StatPacks */
    UINT32 RptMask[NUM_MASK_DWORDS];
} RPT_CLIENT, *PRPT_CLIENT;

typedef struct tagStatPackstruct
{
    REPORT_RECORD rr;            /*header */
    UINT32 dwLostPacketCount;    /*Number of BLAM messages that would not fit into FIFO */
    UINT32 dwQueuedPacketCount;  /*Number of BLAM messages put into FIFO */
    UINT32 dwReadPacketCount;    /*Number of BLAM messages read from FIFO for transport to client */
    UINT32 dwNumBLAMBytesWritten;/*TOTAL number of bytes written to FIFO */
} STATPACK, *PSTATPACK;



/*******************************************************************/
/* prototypes */
/*******************************************************************/
BOOL   RPT_Init			(void* hq, PUINT32 FilterInitArray,UINT32 dwNumMessages, UINT32 SeverityDefault, RPT_CLIENT_TIME fn1);
void   RPT_Shutdown		(void);
BOOL   RPT_Write		(UINT32 TypeSev, UINT32 FormatStr, ...);
BOOL   RPT_Read			(PUINT8 pBuffer, UINT32 requestedSize, PUINT32 actualSize);
BOOL   RPT_SetMask		(UINT32 Cmd, PUINT32 pDataBuf);
UINT32 RPT_GetMask		(PUINT32 pDataBuf);

PRPT_CLIENT    GetIOClient(void);

#else  /* defined BLAM22 */

/* General definitions */

/*
#ifndef FALSE
#define FALSE				0
#endif

#ifndef TRUE
#define TRUE				1
#endif

#ifndef WORD
typedef unsigned short		WORD;
#endif

#ifndef UINT32
typedef unsigned long		UINT32;
#endif

#ifndef DWORD
typedef unsigned long		DWORD;
#endif

#ifndef	PDWORD
typedef DWORD				*PDWORD;
#endif

#ifndef BOOL
typedef int					BOOL;
#endif

#ifndef BYTE
typedef unsigned char		BYTE;
#endif

#ifndef PBYTE
typedef BYTE				*PBYTE;
#endif
*/

/*Definitions added for new filtering mechanism */

/* dwGlobalSeverity will contain a bitmask received from the ring 3 app that specifies which */
/* severities of messages should be logged regardless of type. It is assumed shifted left into the */
/* high order byte for easy comparison. */

/* The dwGlobalLoggingThread component */
/* will contain a value set by the ring 3 app that specifies a single programmer defined "loggingthread" */
/* is to be logged regardless of message type or severity.   It is assumed shifted left into the 3rd */
/* byte (bits 16-23).  */

/* If neither severity or global thread causes */
/* the message to get logged, the ReportWrite routine checks the contents of RptMask -- the */
/* index into which IS the message type.  A 00 means DON'T LOG,  a FF means always LOG, a value */
/* between 1 and FE means LOG if and only if the Logging Thread in the RTRACE parameters match. */

#define LOGGING_THREAD_MASK         ((UINT32) 0x00FF0000)
#define GLOBAL_SEVERITY_MASK		((UINT32) 0xFF000000)
#define MESSAGE_TYPE_MASK			((UINT32) 0x000000FF)
#define YES_LOG_MESSAGE				((UINT32) 0x000000FF)

/* The following two defines are used to initialize the BLAM_Filters_Defaults */
/* array which is a UINT8 array (thus the cast to UINT8... */
#define YES_LOG_MSG_INIT			((UINT8) 0xFF)
#define DONT_LOG_MSG_INIT			((UINT8) 0x00)

#define DONT_LOG_MESSAGE			0

#define	LINE_NUM_MASK				((UINT32) 0x00003FFF)	/*DH	BITS 0-13 */
#define FNUM_MASK					((UINT32) 0x00FFC000)	/*DH	BITS 14-23 */
#define VERSION_INFO_TIMECODE_MASK	((UINT32) 0x1F000000)	/*DH	BITS 24-28 */
#define NUM_ARGS_MASK				((UINT32) 0xE0000000)	/*DH	BITS 29-31 */

#define RPT_MAX_ARGS 7

#define MAX_NUM_THREADS 255		/*current maximum number of logging threads */
#define SET_SPECIFIC_FILTER 3	/*Command to set a value for a particula type */
								/*00=OFF, FF==ON, else its a thread num */
#define SET_GLOBAL_THREAD   2	/*cmd to set the logging global logging thread */
#define SET_GLOBAL_SEVERITY 1	/*cmd to set the severity mask value */


#define MAX_DIR_PATH 100	/*DH */

typedef struct tagReportDescriptor
{
   UINT32 lnum			: 14;  /* 0 to 16K representing source code __LINE__ */
   UINT32 fnum			: 10;  /* 0 to 1023 File number */
   UINT32 version		: 3;   /* allows for varying report packet structure */
   UINT32 timefmt		: 2;   /* timecode format */
   UINT32 payload		: 3;   /* extra DWORDs after base report packet */
} REPORTDESCRIPTOR;


typedef struct tagReportRecord       /*TMW */
{
/*  REPORTDESCRIPTOR descriptor;  */
	UINT32 descriptor;
	UINT32 timestamp;
} REPORT_RECORD, *PREPORT_RECORD;


typedef void (*RPT_Q_CRITSECTION)(BOOL);

#define RPT_RECORD_VERSION 0 /* timecode ms, payload 0,1,2,3 UINT32 */

typedef struct tagMsgQueue
{
	UINT32  read;
	UINT32  write;
	UINT32  size;
	PUINT8 q;
   RPT_Q_CRITSECTION CriticalSection;
} MSGQUEUE, *PMSGQUEUE;

typedef enum 
{
    RPT_STATUS_NOT_ASSIGNED,
    RPT_STATUS_IS_ASSIGNED,
} RPT_CLIENT_STATUS;
	
typedef UINT32 (*RPT_CLIENT_TIME)(void);



typedef UINT32 (_stdcall *QPEEK) (void* hFIFO, void* pBuffer, UINT32 uSize);		  /*DH */
typedef UINT32 (_stdcall *QGETFREE) (void* hFIFO);									  /*DH */
typedef UINT32 (_stdcall *QGETCOUNT) (void* hFIFO);									  /*DH	 */
typedef UINT32 (_stdcall *QWRITE) (void* hFIFO, void* pBuffer, UINT32 uSize);		  /*DH */
typedef UINT32 (_stdcall *QREAD)  (void* hFIFO, void* pBuffer, UINT32 uSize);		  /*DH */
/*
typedef UINT32 (  *QPEEK) (void* hFIFO, void* pBuffer, UINT32 uSize);		  DH
typedef UINT32 (  *QGETFREE) (void* hFIFO);									  DH
typedef UINT32 (  *QGETCOUNT) (void* hFIFO);							        DH
typedef UINT32 (  *QWRITE) (void* hFIFO, void* pBuffer, UINT32 uSize);		  DH
typedef UINT32 (  *QREAD)  (void* hFIFO, void* pBuffer, UINT32 uSize);		  DH
*/
/*THIS STRUCTURE WILL BE INITIALIZED WITH FIFO FUNCTIONS PROVIDED WITHIN THE DRIVER SOURCE CODE */
typedef struct tagQueueStruct
{	
	void*		hQueue;							/*HANDLE TO EXTERNAL QUEUE */
	char		fileNameUNICODE[MAX_DIR_PATH];	/*ASCII NAME OF BINARY FILE CONTAINING CONTENTS OF FIFO */
	QWRITE		EXT_Q_Write;					/*POINTER TO EXTERNAL FIFO WRITE FUNCTION */
	QREAD		EXT_Q_Read;						/*POINTER TO EXTERNAL FIFO READ FUNCTION */
	QGETCOUNT	EXT_Q_GetFree;					/*POINTER TO EXTERNAL FIFO GET FREE BYTES FUNCTION */
	QGETFREE	EXT_Q_GetCont;					/*POINTER TO EXTERNAL FIFO GET USED BYTES FUNCTION */
	QPEEK		EXT_Q_Peek;						/*POINTER TO EXTERNAL FIFO PEEK FUNCTION */
	UINT32		NUM_OF_BYTES_W;					/*TOTAL NUMBER OF BYTES WRITTEN TO FIFO */
	BOOL		ACTIVE_QUEUE;					/*SIGNALS WHETHER AN ACTIVE EXTERNAL QUEUE */
} RQUEUE, *PRQUEUE;

typedef struct tagClientstruct
{
    RPT_CLIENT_TIME RPT_ClientGetElapsedTime;
    RPT_CLIENT_STATUS status;
    MSGQUEUE q;		/*INTERNALLY DEFINED QUEUE */
	RQUEUE	EXT_Q;	/*EXTERNALLY DEFINED QUEUE	 */
    UINT32 dwBaseTime;
	UINT32 dwGlobalSeverity;
	UINT32 dwGlobalLoggingThread;
    UINT8 RptMask[256];
} RPT_CLIENT, *PRPT_CLIENT;



/*******************************************************************/
/* EXTERNAL queue routines to manage buffer */
/*******************************************************************/



/*******************************************************************/
/* INTERNAL queue routines to manage buffer */
/*******************************************************************/
/*
void Q_Init(PMSGQUEUE p, PUINT8 q, UINT32 size, RPT_Q_CRITSECTION CriticalSection);
PUINT8 Q_GetWritePtr(PMSGQUEUE p);
PUINT8 Q_GetReadPtr(PMSGQUEUE p);
UINT32 Q_GetLinearAvail(PMSGQUEUE p);
UINT32 Q_GetLinear(PMSGQUEUE p);
UINT32 Q_UpdateWritePtr(PMSGQUEUE p, UINT32 d);
UINT32 Q_UpdateReadPtr(PMSGQUEUE p, UINT32 d);
UINT32 Q_Read(PMSGQUEUE p, PUINT8 pData, UINT32 size);
UINT32 Q_Write(PMSGQUEUE p, PUINT8 pData, UINT32 size);

*/

/*******************************************************************/
/* report from realtime process */
/*******************************************************************/
__shimcall__
BOOL   RPT_Init			(UINT32 pdwClientId, void* hq, char* fileNameUNICODE, PUINT8 FilterInitArray,UINT32 dwNumMessages, UINT32 SeverityDefault, QWRITE qw_fn, QREAD qr_fn, QGETFREE qgf_fn, QGETCOUNT qgc_fn, QPEEK qp_fn, RPT_CLIENT_TIME fn1);
__shimcall__
void   RPT_Shutdown		(UINT32 dwClientId);
__shimcall__
BOOL   RPT_Write		(UINT32 dwClientId, UINT32 TypeSev, UINT32 FormatStr, ...);
__shimcall__
BOOL   RPT_Read			(UINT32 dwClientId, PUINT8 pBuffer, UINT32 requestedSize, PUINT32 actualSize);
__shimcall__
BOOL   RPT_SetMask		(UINT32 dwClientId, UINT32 dwType, UINT32 bSet);
__shimcall__
BOOL   RPT_ScrWrite		(UINT32 dwClientId, UINT32 TypeSev, PUINT8 pScrBuf);
__shimcall__
BOOL   RPT_ScrRead		(UINT32 dwClientId, PUINT8 pBuffer, UINT32 requestedSize, PUINT32 actualSize);

__shimcall__
PRPT_CLIENT    GetIOClientFromId(UINT32 id);
__shimcall__
void   EX_Q_Init(PRQUEUE pq, void* hq, char* fileNameUNICODE, QWRITE qw, QREAD qr, QGETFREE qgf_fn, QGETCOUNT qgc_fn, QPEEK qp_fn);
/*BOOL   RPT_End(UINT32 dwClientId); */

#endif /* defined BLAM22 */

#ifdef __cplusplus
}
#endif

#endif  /* !defined __RPT_IO_H__ */



			
