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
*                     Version Control Information								
*
*                                                                                       *
* $Header:   P:/V68_Projects/Octopus_PDB/archives/Octopus files/Include files/framewrk.h_v   1.52   11 Jul 2003 17:03:04   rayskymy  $
* 
*****************************************************************************************/


/****************************************************************************************

File Name:			framewrk.h	

File Description:	Compiler Directives for code modules

*****************************************************************************************/

/****************************************************************************************
TARGETS SUPPORTED				
TARGET_HCF_PCI_WDM				
TARGET_HCF_PCI_VXD				
TARGET_HCF_USB_WDM				
TARGET_HCF_USB_MACOS9			
TARGET_HCF_USB_MACOSX			
TARGET_HCF_PCI_MACOSX			
TARGET_HSF_PCI_MACOSX
TARGET_HCF_PCI_LINUX
TARGET_HCF_USB_LINUX
****************************************************************************************/

#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

/****************************************************************************************
	Octopus modem family settings
****************************************************************************************/
#define OCTOPUS_MODEM

/* Define `PROJECT' to one of: */
/* ----------------------------*/
#define PRJ_HSF				0x00000001
#define PRJ_SDSL			0x00000002
#define PRJ_HCF			0x00000004
#define PRJ_OCTOLOADER		0x00000008
#define PRJ_ORCA		    0x00000010
#define PRJ_GNRL_WIN_R3     0x00000020
#define PRJ_GNRL_VXD        0x00000040
#define PRJ_HSFTEST         0x00000080

/* Use one of next macros, or just `#if ( OS_TYPE == xxx )' */
#define PRJ_ONE_OF(prj) ( ( PROJECT & (prj) ) != 0 )
#define PRJ_IS_NOT(prj) ( ( PROJECT & (~(prj)) ) != 0 )


/* Define `PRJ_FLAVOUR' to one of: */
/* ----------------------------*/
#define PRJFLVR_HSF		0x01000000
#define PRJFLVR_LINUX	0x02000000
#define PRJFLVR_CYPRUS	0x04000000
#define PRJFLVR_NILE    0x08000000
#define PRJFLVR_CIDER   0x10000000
 
/* Use one of next macros, or just `#if ( OS_TYPE == xxx )' */
#define PRJFLVR_ONE_OF(prj) ( ( PRJ_FLAVOUR & (prj) ) != 0 )
#define PRJFLVR_IS_NOT(prj) ( ( PRJ_FLAVOUR & (~(prj)) ) != 0 )
 
/* Define `OS_TYPE' to one of: */
/* ----------------------------*/
#define OS_WIN_9X   0x00000010
#define OS_WIN_NT4  0x00000020
#define OS_WIN_WDM  0x00000040
#define OS_LINUX    0x00000080
#define OS_WIN_CE   0x00000100
#define OS_PSOS     0x00000200
#define OS_QNX      0x00000400
#define OS_MACOS    0x00000800
#define OS_APPLE    0x01000000
#define OS_DOS      0x02000000
#define OS_VXWORKS  0x04000000
#define OS_MACOSX	0x08000000

/* Use one of next macros, or just `#if ( OS_TYPE == xxx )' */
#define OS_ONE_OF(os) ( ( OS_TYPE & (os) ) != 0 )
#define OS_IS_NOT(os) ( ( OS_TYPE & (~(os)) ) != 0 )


/****************************************************************************************
	HCF PCI WDM settings
****************************************************************************************/
#if defined TARGET_HCF_PCI_WDM
#define OS_TYPE     OS_WIN_WDM
#define FRAMEWORK_HCF_PCI		1
#define FRAMEWORK_HCF_CADMUS2	0
#define SAVE_TXRX_STATES
#define	USE_DIRECT_API
#define USE_DEVMGR_DIRECT_API
#define VOICE_SUPPORT
/*#$YS$ */
#define FAX34_ENABLE 1

//#define NO_VOICE_SUPPORT
/* JT has to enable voice support before using this feature */
#define SUPPORT_DTMF_CID		

#define NO_V80_SUPPORT
#define NO_BLAM_SUPPORT
#define NO_DUMPMGR_SUPPORT
/* #define USE_SMART_DAA */

/*#define TEST_CWCID */
/*#define BLAM */
#if DBG
#define _DEBUG 1
#endif
/****************************************************************************************
	HCF PCI VxD settings
****************************************************************************************/
#elif defined TARGET_HCF_PCI_VXD
#define OS_TYPE     OS_WIN_9X
#define FRAMEWORK_HCF_PCI		1
#define FRAMEWORK_HCF_CADMUS2	0
#define SAVE_TXRX_STATES
#define	USE_DIRECT_API
/*#define BLAM */

/****************************************************************************************
	HCF USB WDM settings
****************************************************************************************/
#elif defined TARGET_HCF_USB_WDM
#define OS_TYPE     OS_WIN_WDM
#define FRAMEWORK_HCF_PCI		0
#define FRAMEWORK_HCF_CADMUS2	1
#define	USE_DIRECT_API
#define USE_DEVMGR_DIRECT_API
#define MODEM_VXD 0		/* for VxD only or VxD/SYS combination */
#define MODEM_WDM 1		/* WDM modem */
#define SAVE_TXRX_STATES
#define SAVE_MOH_STATES
#define USE_TEST_SESSION
/*#define BLAM */
/*#define USE_DCP */
#define CADMUS2	1

#if DBG
#define _DEBUG 1
#endif

#define XID_TIMEOUT_RENEG	/* rmy: to fix XID rx problem */
#define USE_TOTAL_SIGNATURE_CHECK
/*#define DP_CLOCK_WA_STAT */
/*#define LOOPBACK_RETRAIN */

#define NO_VOICE_SUPPORT
#define NO_V80_SUPPORT
#define NO_DTMF_CID_SUPPORT
#define NO_DUMPMGR_SUPPORT
#define NO_BLAM_SUPPORT
#define NO_V34FAX_SUPPORT
#define NO_DIAGMGR_SUPPORT

/****************************************************************************************
	MacOS9 settings
****************************************************************************************/
#elif defined TARGET_HCF_USB_MACOS9
#define OS_TYPE     OS_MACOS
#define USE_DIRECT_API
#define USE_DEVMGR_DIRECT_API
#define USE_TEST_SESSION
#define FRAMEWORK_HCF_CADMUS2 1
#define CADMUS2	1
#define MACTOPUS
#define BIG_ENDIAN_MACHINE 1
#define SAVE_TXRX_STATES
#define USE_DCP
#define FAKE_PULSEDIAL_DCP
#define XID_TIMEOUT_RENEG	/* rmy: to fix XID rx problem */
/*#define BLAM */

#define _stdcall
#define __stdcall

#define USE_LOOPBACK_RESTART

#define NO_VOICE_SUPPORT
#define NO_V80_SUPPORT
#define NO_DIAGMGR_SUPPORT
#define NO_DUMPMGR_SUPPORT
#define NO_DTMF_CID_SUPPORT
#define NO_BLAM_SUPPORT

/****************************************************************************************
	MacOSX settings
****************************************************************************************/
#elif defined TARGET_HCF_USB_MACOSX
#define OS_TYPE     OS_MACOSX
#define USE_DIRECT_API
#define USE_DEVMGR_DIRECT_API
#define USE_TEST_SESSION
#define FRAMEWORK_HCF_CADMUS2 1
#define CADMUS2	1
#define MACTOPUS
#define BIG_ENDIAN_MACHINE 1
#define SAVE_TXRX_STATES
#define SAVE_MOH_STATES
#define USE_DCP
#define FAKE_PULSEDIAL_DCP
#define XID_TIMEOUT_RENEG	/* rmy: to fix XID rx problem */

#define _stdcall
#define __stdcall

#define NO_VOICE_SUPPORT
#define NO_V80_SUPPORT
#define NO_V34FAX_SUPPORT
#define NO_DUMPMGR_SUPPORT
#define NO_DTMF_CID_SUPPORT
#define NO_BLAM_SUPPORT
/* #define BLAM22 */

#define USE_LOOPBACK_RESTART

/****************************************************************************************
	MacOSX HCF PCI settings
****************************************************************************************/
#elif defined TARGET_HCF_PCI_MACOSX 

#define OS_TYPE     OS_MACOSX
#undef USE_DIRECT_API
#define USE_DIRECT_API
#define USE_DEVMGR_DIRECT_API
#define USE_TEST_SESSION
#define FRAMEWORK_HCF_CADMUS2 0
#define BIG_ENDIAN_MACHINE 1
#define MACTOPUS
#define SAVE_TXRX_STATES
#define SAVE_MOH_STATES
#define USE_DCP
#define FAKE_PULSEDIAL_DCP
#define XID_TIMEOUT_RENEG	/* rmy: to fix XID rx problem */
#define DUMP_MANAGER
#define BLAM

#define _stdcall
#define __stdcall

#define NO_VOICE_SUPPORT
#define NO_V80_SUPPORT
#define NO_DUMPMGR_SUPPORT
#define NO_DTMF_CID_SUPPORT

#define USE_LOOPBACK_RESTART

/****************************************************************************************
	MacOSX HSF PCI settings
****************************************************************************************/
#elif defined TARGET_HSF_PCI_MACOSX
#undef USE_DIRECT_API
#define USE_DIRECT_API
#define USE_DEVMGR_DIRECT_API
#define USE_TEST_SESSION
#define FRAMEWORK_HCF_PCI     0
#define FRAMEWORK_HCF_CADMUS2 0
#define BIG_ENDIAN_MACHINE 1
#define MACTOPUS
#define SAVE_TXRX_STATES
#define SAVE_MOH_STATES
#define USE_DCP
#define FAKE_PULSEDIAL_DCP
#define XID_TIMEOUT_RENEG	/* rmy: to fix XID rx problem */
#define DUMP_MANAGER
#define BLAM

#define _stdcall
#define __stdcall
#include "build_modules.h"

/* BLAM interface to be hidden from system namespace */
#undef  RPT_Write
#define RPT_Init            InternalSoftModem_RPT_Init
#define RPT_SetMask         InternalSoftModem_RPT_SetMask
#define RPT_Shutdown        InternalSoftModem_RPT_Shutdown
#define RPT_Write           InternalSoftModem_RPT_Write
#define RPT_Read            InternalSoftModem_RPT_Read
#define RPT_GetMask         InternalSoftModem_RPT_GetMask
#define GetIOClient         InternalSoftModem_GetIOClient
#define GetIOClientFromId   InternalSoftModem_GetIOClientFromId

#define USE_LOOPBACK_RESTART
/* SoftModem section from HSF's framewrk.h" */
#define PROJECT     PRJ_HSF
#define PRJ_FLAVOUR	PRJFLVR_CYPRUS
#define OS_TYPE     OS_MACOSX
#define UK_MODE     UK_KERNEL
#define CPU_TYPE    CPU_PPC
#define CPU_VER     4000
#define IO_MODE     IO_ONLINE
#define COMPILER    CMP_GCC
#define CMP_VER     5
#define DEV_TOOL    DT_NONE
#define DEBUG_LEVEL DEBUG_DEVELOP
#define OCT_SW_MODEM 1
#define OCT_MEM_MGR	1
#define USE_MEM_MGR	1
#define USE_PRAGMA_PACK	0
#define OS_NOREDEFS
#define DMP

#undef FRAME_WORK
#define FRAME_WORK "Framework: MacOS X HSF"

/****************************************************************************************
	HCF PCI LINUX settings
****************************************************************************************/
#elif defined TARGET_HCF_PCI_LINUX
#define OS_TYPE     OS_LINUX
#define FRAMEWORK_HCF_PCI		1
#define FRAMEWORK_HCF_CADMUS2	0
#define	USE_DIRECT_API
#define USE_DEVMGR_DIRECT_API
#define SAVE_TXRX_STATES
#define SAVE_MOH_STATES
#define USE_TEST_SESSION
#define USE_SMART_DAA
#define SUPPORT_RIPTIDE
#define USE_PRAGMA_PACK 0
#define USE_MEM_MGR	0
#define USE_INLINE_OS_MEMMGR
/*#define TEST_CWCID */
/*#define BLAM */

#define _stdcall
#define __stdcall

#define XID_TIMEOUT_RENEG	/* rmy: to fix XID rx problem */

#define NO_V80_SUPPORT
#define NO_DUMPMGR_SUPPORT
#define NO_DTMF_CID_SUPPORT

#ifdef NO_V34FAX_SUPPORT
#define FAX34_ENABLE 0
#else
#define FAX34_ENABLE 1
#endif

#ifndef NO_VOICE_SUPPORT
#define VOICE_SUPPORT
#endif

#if defined(_BIG_ENDIAN)
#define BIG_ENDIAN_MACHINE 1
#endif


/****************************************************************************************
	HCF USB LINUX settings
****************************************************************************************/
#elif defined TARGET_HCF_USB_LINUX
#define OS_TYPE     OS_LINUX
#define FRAMEWORK_HCF_PCI		0
#define FRAMEWORK_HCF_CADMUS2	1
#define	USE_DIRECT_API
#define USE_DEVMGR_DIRECT_API
#define SAVE_TXRX_STATES
#define SAVE_MOH_STATES
#define USE_TEST_SESSION
#define USE_PRAGMA_PACK 0
#define USE_MEM_MGR	0
#define USE_INLINE_OS_MEMMGR
/*#define BLAM */
/*#define USE_DCP */
#define CADMUS2	1

#define _stdcall
#define __stdcall

#define XID_TIMEOUT_RENEG	/* rmy: to fix XID rx problem */
/*#define DP_CLOCK_WA_STAT */
/*#define LOOPBACK_RETRAIN */

#ifndef NO_VOICE_SUPPORT
#define NO_VOICE_SUPPORT
#endif

#define NO_V80_SUPPORT
#define NO_DUMPMGR_SUPPORT
#define NO_DTMF_CID_SUPPORT

#ifndef NO_V34FAX_SUPPORT
#define NO_V34FAX_SUPPORT
#endif
#define FAX34_ENABLE 0

#define USE_LOOPBACK_RESTART

#if defined(_BIG_ENDIAN)
#define BIG_ENDIAN_MACHINE 1
#endif


/****************************************************************************************
	HCF VxWorks settings
****************************************************************************************/
#elif defined TARGET_HCF_PCI_VXWORKS
#define OS_TYPE     OS_VXWORKS
#define FRAMEWORK_HCF_PCI		1
#define FRAMEWORK_HCF_CADMUS2	0
#define SAVE_TXRX_STATES
#define	USE_DIRECT_API
/*#define BLAM */
#define _VXWORKS

#define NO_VOICE_SUPPORT
#define NO_V80_SUPPORT
#define NO_DIAGMGR_SUPPORT
#define NO_DUMPMGR_SUPPORT
#define NO_DTMF_CID_SUPPORT
#define NO_BLAM_SUPPORT

#elif defined TARGET_HCF_STARFISH_VXWORKS
#define FRAMEWORK_HCF_PCI		1
#define FRAMEWORK_HCF_CADMUS2	0
#define SAVE_TXRX_STATES
#define	USE_DIRECT_API
#if 1
#define BLAM
#define USE_TRACE
#define USE_BREAKPOINT
#define TCP_BLAM_SERVER /* send BLAM writed thrue TCP port */
#endif
#define _VXWORKS

#define NO_VOICE_SUPPORT
#define NO_V80_SUPPORT
#define NO_DIAGMGR_SUPPORT
#define NO_DUMPMGR_SUPPORT
#define NO_DTMF_CID_SUPPORT

#else
#error valid build target not found

#endif	/* TARGET_XXX_YYY_ZZZ */

#if defined(__GNUC__) && (OS_TYPE != OS_MACOSX)
/* MacOS X compiler, while being GNU C does not like __attribute__((packed)) */

/* To make structures packed (with no padding), do:
 *    typedef struct PACKED_ATTRIB st_TAG { ... } st_T; */
#define PACKED_ATTRIB __attribute__ ((packed))
#define ALIGNED_ATTRIB(n) __attribute__ ((packed, aligned(n)))
#else
#define PACKED_ATTRIB
#define ALIGNED_ATTRIB(n)
#endif

#if ( OS_TYPE == OS_LINUX ) && defined(__i386__)
#define __shimcall__ __attribute__((regparm(0)))
#define __kernelcall__
#define __kernelcallregparm__ __attribute__((regparm(3)))
#define __kernelcallstkparm__ __attribute__((regparm(0)))
#else
#define __shimcall__
#define __kernelcall__
#define __kernelcallregparm__
#define __kernelcallstkparm__
#endif

#if ((FRAMEWORK_HCF_PCI == 1) || (FRAMEWORK_HCF_CADMUS2 == 1))
#define TARGET_HCF_FAMILY 1
#else
#define TARGET_HCF_FAMILY 0
#endif

#if TARGET_HCF_FAMILY
#define PROJECT PRJ_HCF
#define PRJ_FLAVOUR 0
#endif

#ifndef PROJECT
#error --==> PROJECT is not defined! <==--
#elif ( PRJ_IS_NOT(PRJ_HSF | PRJ_SDSL | PRJ_HCF | PRJ_OCTOLOADER | PRJ_ORCA | PRJ_GNRL_WIN_R3 | PRJ_GNRL_VXD | PRJ_HSFTEST) )
#error --==> Bad PROJECT definition! <==--
#endif

#ifndef PRJ_FLAVOUR
#error --==> PRJ_FLAVOUR is not defined! <==--
#elif ( PRJFLVR_IS_NOT(PRJFLVR_HSF | PRJFLVR_LINUX | PRJFLVR_CYPRUS | PRJFLVR_NILE) )
#error --==> Bad PRJ_FLAVOUR definition! <==--
#endif

#ifndef OS_TYPE
#error --==> OS_TYPE is not defined! <==--
#elif (OS_IS_NOT(OS_WIN_9X | OS_WIN_NT4 | OS_WIN_WDM | OS_LINUX | \
				 OS_WIN_CE | OS_PSOS | OS_QNX | OS_MACOS | OS_APPLE | OS_DOS | OS_VXWORKS | OS_MACOSX))
#error --==> Bad OS_TYPE definition! <==--
#endif

#endif /* __FRAMEWORK_H__ */

