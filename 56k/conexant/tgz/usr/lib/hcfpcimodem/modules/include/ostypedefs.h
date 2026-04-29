/*
 * Copyright (c) 2003-2004 Linuxant inc.
 * Copyright (c) 2001-2003 Conexant Systems, Inc.
 *
 * NOTE: The use and distribution of this software is governed by the terms in
 * the file LICENSE, which is included in the package. You must read this and
 * agree to these terms before using or distributing this software.
 *
 */

/****************************************************************************************
*		                     Version Control Information
*
*	$Header:   P:/d942/octopus/archives/linuxos/dist/modules/osspec/ostypedefs.h_v   1.2   28 Apr 2003 11:03:48   beliloi  $
*
*****************************************************************************************/


/****************************************************************************************

File Name:			OsTypeDefs.h

File Description:	OS specific definitions

*****************************************************************************************/

#ifndef __OSTYPEDEFS_H__
#define __OSTYPEDEFS_H__

#ifndef __TYPEDEFS_H__
//#include <linux/stddef.h>
#ifdef USBHCF
#endif

typedef void VOID, *PVOID, *LPVOID, **PPVOID, **PHANDLE;

typedef long INT32, *PINT32, LONG, *PLONG;

typedef unsigned int UINT, *PUINT;

typedef unsigned int UINT32, *PUINT32;

typedef unsigned long ULONG, *PULONG;

typedef unsigned short UINT16, *PUINT16, WORD, *PWORD;

typedef unsigned char UINT8, BYTE, *PUINT8, *PBYTE;

#define HANDLE PVOID	

typedef char*	STRING;

typedef char* LPSTR, *PCHAR;
typedef const char* LPCSTR;

typedef int INT, *PINT;

typedef int BOOL, *PBOOL, BOOLEAN, *PBOOLEAN;

#define FALSE 0
#define TRUE  (!FALSE)

#define NULL ((void *)0)

typedef unsigned long DWORD, *PDWORD;

#define _cdecl
#define __cdecl
#define __declspec(a)
#define __inline inline
/* The following definitions should be used for each parameter in a function's parameters list. */
#define IN
#define OUT
#define IO

#ifndef min
#define min(x,y) ((x) < (y)  ? (x) : (y))
#endif
#ifndef max
#define max(x,y) ((x) > (y)  ? (x) : (y))
#endif

/* The following definitions should be used with each function's definition. */
#define GLOBAL
#define STATIC
//#define static

#define __asm	#undef
#ifdef _DEBUG
#define	DbgDump(ptr, size)
#endif

#define pack message

#endif	/* __TYPEDEFS_H__ */

#endif /* __OSTYPEDEFS_H__ */


 
