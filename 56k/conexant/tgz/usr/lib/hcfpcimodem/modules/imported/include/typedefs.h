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
*		                     Version Control Information
*
*	$Header:   P:/V68_Projects/Octopus_PDB/archives/Octopus files/Include files/TypeDefs.h_v   1.13   19 May 2003 15:10:18   beliloi  $
*
*****************************************************************************************/


/****************************************************************************************

File Name:			Typedefs.h	

File Description:	Basic Type Definitions

*****************************************************************************************/


#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#ifndef __OSTYPEDEFS_H__

#ifndef __GNUC__
#pragma warning(disable : 4001)
#pragma warning(disable : 4100)
#endif

/* typedef enum {FALSE, TRUE} BOOL; */

typedef int BOOL, *PBOOL;

#define BOOLEAN BOOL

/*JT VOID already defined in Win2K wdm.h */
#ifndef VOID
typedef void VOID;
#endif

typedef	void *PVOID,**PPVOID;



#define FALSE	0
#define TRUE	1


#ifndef NULL
#define	NULL	0
#endif

#define CHAR    char
#define WORD    unsigned short
#define PWORD   WORD* 

#define DWORD   unsigned long
#define PDWORD  DWORD* 

#define INT     int
#define PINT    INT* 

#define LPVOID  void* 
#define	PCHAR   char*

#define ULONG  unsigned long
#define PULONG ULONG*		

#define UINT32  unsigned int
#define PUINT32 UINT32*		

#define INT32   int
#define PINT32  int*

#define UINT16 unsigned short
#define PUINT16 UINT16*		

#define UINT8 unsigned char
#define BYTE unsigned char
#define PBYTE unsigned char*
#define PUINT8 UINT8*	

#define INT8 signed char
#define PINT8 INT8*

#define INT16 signed short
#define PINT16 INT16*

#define HANDLE PVOID	

#define STRING char*	
typedef char* LPSTR;
typedef const char* LPCSTR;

typedef unsigned long ULONG_PTR;


/* The following definitions should be used for each parameter in a function's parameters list. */
#undef IN
#undef OUT
#undef IO

#ifdef __GNUC__
#define IN
#else
#define IN const
#endif
#define OUT
#define IO


/* The following definitions should be used with each function's definition. */
#define GLOBAL
#define STATIC		static

#ifndef MIN
#define MIN(x,y) ((x) < (y)  ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y) ((x) > (y)  ? (x) : (y))
#endif

/* Prevent type redefinitions by soar.h */
#define _SOAR_TYPES_DEFINED_

#endif

#endif /* __TYPEDEFS_H__ */
