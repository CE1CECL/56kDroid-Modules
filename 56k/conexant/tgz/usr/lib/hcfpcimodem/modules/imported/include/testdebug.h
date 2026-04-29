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
*	$Header:   P:/V68_Projects/Octopus_PDB/archives/Octopus files/Include files/TestDebug.h_v   1.66   11 Jul 2003 17:53:28   rayskymy  $
*
*****************************************************************************************/


/****************************************************************************************

File Name:			TestDebug.h

File Description:		

*****************************************************************************************/


/****************************************************************************************
*****************************************************************************************
***                                                                                   ***
***                                 Copyright (c) 2000                                ***
***                                                                                   ***
***                                Conexant Systems, Inc.                             ***
***                             Personal Computing Division                           ***
***                                                                                   ***
***                                 All Rights Reserved                               ***
***                                                                                   ***
*                                    CONFIDENTIAL                                   ***
***                                                                                   ***
***               NO DISSEMINATION OR USE WITHOUT PRIOR WRITTEN PERMISSION            ***
***                                                                                   ***
*****************************************************************************************
*****************************************************************************************/


#ifndef __TESTDEBUG_H__
#define __TESTDEBUG_H__

#include "osservices.h"

#if defined(_DEBUG) 

    #define USE_TRACE
    #define USE_BREAKPOINT
    #define USE_ASSERT

    #define TRACE_FILE_NAME "ModemLog"

#endif

#if !defined NO_BLAM_SUPPORT && !defined USE_TRACE
    #define USE_TRACE
#endif

#ifdef USE_ASSERT
    #undef ASSERT
    #define ASSERT(x)   {                                                                                           \
                            if(!(x))                                                                                \
                            {																						\
                                RTRACE2(T__ASSERT,0, "Assertion failed, File: %s, Line: %d\n", __FILE__, __LINE__);   \
                                CNXT_BREAKPOINT("Assert");                                                               \
                            }                                                                                       \
                        }
#else
    #undef ASSERT
    #define ASSERT(x)
#endif

#ifdef USE_BREAKPOINT
    #define CNXT_BREAKPOINT(msg)     { OsDebugBreakpoint(msg);   }
#else
    #define CNXT_BREAKPOINT(msg)
#endif

#if defined(USE_TRACE) || defined(USE_TRACE_ONLY)

#ifdef USE_TRACE_ONLY
#undef ulTraceMask
#define ulTraceMask (USE_TRACE_ONLY)
#undef ulLogThread
#define ulLogThread 0xdeadbeef
#else
    extern unsigned long ulTraceMask;
    extern unsigned long ulLogThread;
#endif

    #define	T__FATAL_ERROR	0x80000000
    #define T__ERROR		0x40000000
    #define T__ASSERT		0x20000000


#if !defined NO_BLAM_SUPPORT
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*   Background Logging Definitions .... */
/* */
/*		NOTE:	PLEASE, any changes made in TRACE message mask definitions should */
/*				be made also in enum and defaults defined below.  */
/* */
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

#ifndef FILEIDNUM
#warning FILEIDNUM not defined
#define FILEIDNUM 1000
#endif

#if defined BLAM22

	#include "rpt_io.h"

	/*additional severity mask bits to handle TRACEDM conversion */
	#define TDM__ERROR		0x10000000
	#define TDM__WARNING	0x08000000


	/* */
	/*used to initialize Global Severity filter at port open */
	/* */
	#define BLAM_SEVERITY_DEFAULT (T__FATAL_ERROR|T__ERROR|TDM__ERROR)


	/*arbitrary at the moment */
	#define MAX_NUM_RTRACE_IDS 1024

	/*NOTE:  New trace types can be added into this enum anywhere and tracing should */
	/*       work after a recompile.  However, the BLAM application uses this enum */
	/*       to create its database and has trouble parsing comments within it so */
	/*       at the present time PLEASE ADD NO COMMENTS WITHIN THIS ENUM!!! */
	/*       ALSO: Note that any changes to this enum needs corresponding (order */
	/*       dependent changes tot he BLAM_Filters_Default list below.... */

        /* NEW NOTE:  It is important to keep the order of definitions in this enum in sync
           with the BLAM_FILTERS_DEFAULTS definition of defaults below.  This means you will most likely
           find it EASIER to add any new categrories at the END rather than have to fiddle with adjusting
           the BLAM_FILTERS_DEFAULTS definition
        */
	enum RTRACE_GROUP {	
		T__CALLPROG,                         /*ALL/CONTROLLER/CALLPROGRESS*/
		T__SESSIONMGR,                       /*ALL/MANAGER/SESSION*/
		T__STREAMMGR,                        /*ALL/MANAGER/STREAM*/
		T__DATASESSION,                      /*ALL/SESSION/DATA*/
		T__MNP,	                             /*ALL/EC/MNP*/
		T__V42,	                             /*ALL/EC/V42*/
		T__V42BIS,                           /*ALL/EC/V42B*/
		T__FAXSESSION,	                     /*ALL/SESSION/FAX*/
		T__FAXPIM,                           /*ALL/CONTROLLER/FAXPIM*/
		T__EEPROM,                           /*ALL/CONTROLLER/EEPROM*/
		T__ATPARSER,                         /*ALL/CONTROLLER/ATPARSER*/
		T__COMMCTRL,                         /*ALL/CONTROLLER/COMMCTRL*/
		T__UTILS,                            /*ALL/CONTROLLER/UTILS*/
		T__RTMGR,                            /*ALL/MANAGER/RT*/
		T__CONFIGMGR,                        /*ALL/MANAGER/CONFIG*/
		T__V8BIS,                            /*ALL/SESSION/V8BIS*/
		T__STATMGR,	                         /*ALL/MANAGER/STAT*/
		T__HAL,	                             /*ALL/HAL*/
		T__HALOS,                            /*ALL/HAL/OS*/
		T__HALSCRIPT,                        /*ALL/HAL/SCRIPT*/
		T__PORT,                             /*ALL/CONTROLLER/PORT*/
		T__V8,                               /*ALL/SESSION/V8*/
		T__PROFILE,                          /*ALL/PROFILE*/
		T__NVMMGR,                           /*ALL/MANAGER/NVM*/
		T__TESTSESSION,                      /*ALL/SESSION/TEST*/
		T__DRIVER,                           /*ALL/SESSION/DRIVER*/
		T__CALLERID,                         /*ALL/V92/CALLERID*/
		T__MOH,                              /*ALL/V92/MOH*/
		T__QC,                               /*ALL/V92/QC*/
		T__DIAG,                             /*ALL/SESSION/DIAG*/
        	T__CW,                               /*ALL/V92/CW*/
        	T__SOFT,                             /*ALL/SOFT*/
       		T__SOFT_AUTOMODE,                    /*ALL/SOFT/AUTOMODE*/
       		T__SOFT_DATA_EL,                     /*ALL/SOFT/DATA/EL*/
       		T__SOFT_DIALER_EL,                   /*ALL/SOFT/DIALER/EL*/
       		T__SOFT_RINGDET,                     /*ALL/SOFT/RINGDET*/
       		T__SOFT_TONE_EL,                     /*ALL/SOFT/TONE/EL*/
       		T__SOFT_SIGNALMGR,                   /*ALL/SOFT/SIGNALMGR*/
       		T__SOFT_B2HW,                        /*ALL/SOFT/B2HW*/
       		T__SOFT_HW,                          /*ALL/SOFT/HW*/
       		T__SOFT_FILTER_EC,                   /*ALL/SOFT/FILTER/EC*/
       		T__SOFT_FRED,                        /*ALL/SOFT/FRED*/
       		T__SOFT_MODEM_CONFIG,                /*ALL/SOFT/MODEMCONFIG*/
       		T__SOFT_FSK,                         /*ALL/SOFT/FSK*/
       		T__SOFT_QAM,                         /*ALL/SOFT/QAM*/
       		T__SOFT_V22,                         /*ALL/SOFT/V22*/
       		T__SOFT_V32,                         /*ALL/SOFT/V32*/
       		T__SOFT_HDLC,                        /*ALL/SOFT/HDLC*/
       		T__SOFT_PROFILER,                    /*ALL/SOFT/PROFILER*/
       		TDM__ENTRY,                          /*ALL/TDM/ENTRY*/
		TDM__DM,                             /*ALL/TDM/DM*/
		TDM__DPDWNLD,                        /*ALL/TDM/DPDWNLD*/
		TDM__V8BIS,                          /*ALL/TDM/V8BIS*/
		TDM__V8,                             /*ALL/TDM/V8*/
		TDM__SOFTRI,                         /*ALL/TDM/SOFTRI*/
		TDM__LMON,                           /*ALL/TDM/LMON*/
		TDM__HDLC,                           /*ALL/TDM/HDLC*/
		TDM__V14,                            /*ALL/TDM/V14*/
		TDM__VSP,                            /*ALL/TDM/VSP*/
		TDM__HWDS,                           /*ALL/TDM/HWDS*/
		TDM__FAX,                            /*ALL/TDM/FAX*/
		TDM__WA0,                            /*ALL/TDM/WA0*/
		TDM__WA1,                            /*ALL/TDM/WA1*/
		TDM__WA2,                            /*ALL/TDM/WA2*/
		TDM__FILTER,                         /*ALL/TDM/FILTER*/
		TDM__ASIC,                           /*ALL/TDM/ASIC*/
		TDM__TEST,                           /*ALL/TDM/TEST*/
		TDM__PULSED,                         /*ALL/TDM/PULSED*/
		TDM__DAA,                            /*ALL/TDM/DAA*/
		TDM__VOICE,                          /*ALL/TDM/VOICE*/
		TDM__HAL,                            /*ALL/TDM/HAL*/
		TDM__HALOS,                          /*ALL/TDM/HALOS*/
		TDM__HALDATA,                        /*ALL/TDM/HALDATA*/
		TDM__CID,                            /*ALL/TDM/CID*/
		TDM__CW,                             /*ALL/TDM/CW*/
		TDM__V92,                            /*ALL/TDM/V92*/
		TDM__MOH,                            /*ALL/TDM/MOH*/
		TDM__QC,                             /*ALL/TDM/QC*/
        TDM__PIG,                            /*ALL/TDM/PIG*/
        T__SOFT_CAESAR,                      /*ALL/SOFT/CAESAR*/
        T__BUFSCHEME,                        /*ALL/SOFT/BUFSCHEME*/

        KEN_SPECIAL,			     /*Ken Test/Special */
        KEN_FLOWDUMP,			     /*Ken Test/RtraceDump Tests */
        KEN_RDUMPTEST,                       /*Ken Test/RDUMP Test */
    	T__SOFT_FAX_EL,                 /*ALL/SOFT/FAX_EL*/
                            
		T__VOICESESSION,                     /*ALL/SESSION/VOICE*/

		Num_Messages
	};


	/*The FOllowing list is used to initialize trace ON/OFF defaults */
	/*for all of the above enum elements at port open time.  This list may */
	/*be overridden by soft defaults (such as registry....).   */
	/* */
	/*NOTE:		Any change to the above list MUST have a corresponding change to  */
	/*			this list...and THIS list IS order dependent -- it is used */
	/*          to initialize an array of filter criteria values...!!!!! */



	/*This macro is used to initialize the default tracing behavior.  It MUST be kept */
	/*current with the enum above and is order dependent.  It initialized each bit for each */
	/*trace category. */
	#define BLAM_FILTERS_DEFAULTS 	{						\
					/*00*/	DO_LOG_MSG_INIT(T__CALLPROG)		\
					/*01*/|	DO_LOG_MSG_INIT(T__SESSIONMGR)		\
					/*02*/|	DO_LOG_MSG_INIT(T__STREAMMGR)		\
					/*03*/|	DO_LOG_MSG_INIT(T__DATASESSION)		\
					/*04*/|	DO_LOG_MSG_INIT(T__MNP)			\
					/*05*/|	DO_LOG_MSG_INIT(T__V42)			\
					/*06*/|	DO_LOG_MSG_INIT(T__V42BIS)		\
					/*07*/|	DO_LOG_MSG_INIT(T__FAXSESSION)		\
					/*08*/|	DO_LOG_MSG_INIT(T__FAXPIM)		\
					/*09*/|	DO_LOG_MSG_INIT(T__EEPROM)		\
					/*10*/|	DO_LOG_MSG_INIT(T__ATPARSER)		\
					/*11*/|	DO_LOG_MSG_INIT(T__COMMCTRL)		\
					/*12*/|	NO_LOG_MSG_INIT(T__UTILS)		\
					/*13*/|	NO_LOG_MSG_INIT(T__RTMGR)		\
					/*14*/|	NO_LOG_MSG_INIT(T__CONFIGMGR)		\
					/*15*/|	NO_LOG_MSG_INIT(T__V8BIS)		\
					/*16*/|	NO_LOG_MSG_INIT(T__STATMGR)		\
					/*17*/|	NO_LOG_MSG_INIT(T__HAL)			\
					/*18*/|	NO_LOG_MSG_INIT(T__HALOS)		\
					/*19*/|	NO_LOG_MSG_INIT(T__HALSCRIPT)		\
					/*20*/|	NO_LOG_MSG_INIT(T__PORT)		\
					/*21*/|	NO_LOG_MSG_INIT(T__V8)			\
					/*22*/|	NO_LOG_MSG_INIT(T__PROFILE)		\
					/*23*/|	NO_LOG_MSG_INIT(T__NVMMGR)		\
					/*24*/|	NO_LOG_MSG_INIT(T__TESTSESSION)		\
					/*25*/|	NO_LOG_MSG_INIT(T__DRIVER)		\
					/*26*/|	NO_LOG_MSG_INIT(T__CALLERID)		\
					/*27*/|	NO_LOG_MSG_INIT(T__MOH)			\
					/*28*/|	NO_LOG_MSG_INIT(T__QC)			\
					/*29*/|	NO_LOG_MSG_INIT(T__DIAG)		\
          			/*30*/| NO_LOG_MSG_INIT(T__CW)			\
                    /*31*/|	NO_LOG_MSG_INIT(T__SOFT)		\
		  ,						\
                    /*00*/	NO_LOG_MSG_INIT(T__SOFT_AUTOMODE) 	\
                    /*01*/|	NO_LOG_MSG_INIT(T__SOFT_DATA_EL) 	\
                    /*02*/|	NO_LOG_MSG_INIT(T__SOFT_DIALER_EL)	\
                    /*03*/|	NO_LOG_MSG_INIT(T__SOFT_RINGDET) 	\
                    /*04*/|	NO_LOG_MSG_INIT(T__SOFT_TONE_EL)  	\
                    /*05*/|	NO_LOG_MSG_INIT(T__SOFT_SIGNALMGR)	\
                    /*06*/|	NO_LOG_MSG_INIT(T__SOFT_B2HW)    	\
                    /*07*/|	NO_LOG_MSG_INIT(T__SOFT_HW)      	\
                    /*08*/|	NO_LOG_MSG_INIT(T__SOFT_FILTER_EC)	\
                    /*09*/|	NO_LOG_MSG_INIT(T__SOFT_FRED)    	\
                    /*10*/|	NO_LOG_MSG_INIT(T__SOFT_MODEM_CONFIG)	\
                    /*11*/|	NO_LOG_MSG_INIT(T__SOFT_FSK)      	\
                    /*12*/|	NO_LOG_MSG_INIT(T__SOFT_QAM)      	\
                    /*13*/|	NO_LOG_MSG_INIT(T__SOFT_V22)     	\
                    /*14*/|	NO_LOG_MSG_INIT(T__SOFT_V32)      	\
                    /*15*/|	NO_LOG_MSG_INIT(T__SOFT_HDLC)     	\
                    /*16*/|	NO_LOG_MSG_INIT(T__SOFT_PROFILER) 	\
					/*17*/|	NO_LOG_MSG_INIT(TDM__ENTRY)		\
					/*18*/|	NO_LOG_MSG_INIT(TDM__DM)		\
					/*19*/|	NO_LOG_MSG_INIT(TDM__DPDWNLD)		\
					/*20*/|	NO_LOG_MSG_INIT(TDM__V8BIS)		\
					/*21*/|	NO_LOG_MSG_INIT(TDM__V8)		\
					/*22*/|	NO_LOG_MSG_INIT(TDM__SOFTRI)		\
					/*23*/|	NO_LOG_MSG_INIT(TDM__LMON)		\
					/*24*/|	NO_LOG_MSG_INIT(TDM__HDLC)		\
					/*25*/|	NO_LOG_MSG_INIT(TDM__V14)		\
					/*26*/|	NO_LOG_MSG_INIT(TDM__VSP)		\
					/*27*/|	NO_LOG_MSG_INIT(TDM__HWDS)		\
					/*28*/|	NO_LOG_MSG_INIT(TDM__FAX)		\
					/*29*/|	NO_LOG_MSG_INIT(TDM__WA0)		\
					/*30*/|	NO_LOG_MSG_INIT(TDM__WA1)		\
					/*31*/|	NO_LOG_MSG_INIT(TDM__WA2)		\
					      ,						\
					/*00*/	NO_LOG_MSG_INIT(TDM__FILTER)		\
					/*01*/|	NO_LOG_MSG_INIT(TDM__ASIC)		\
					/*02*/|	NO_LOG_MSG_INIT(TDM__TEST)		\
					/*03*/|	NO_LOG_MSG_INIT(TDM__PULSED)		\
					/*04*/|	NO_LOG_MSG_INIT(TDM__DAA)		\
					/*05*/|	NO_LOG_MSG_INIT(TDM__VOICE)		\
					/*06*/|	NO_LOG_MSG_INIT(TDM__HAL)		\
					/*07*/|	NO_LOG_MSG_INIT(TDM__HALOS)		\
					/*08*/|	NO_LOG_MSG_INIT(TDM__HALDATA)		\
					/*09*/|	NO_LOG_MSG_INIT(TDM__CID)		\
					/*10*/|	NO_LOG_MSG_INIT(TDM__CW)		\
					/*11*/|	NO_LOG_MSG_INIT(TDM__V92)		\
					/*12*/|	NO_LOG_MSG_INIT(TDM__MOH)		\
					/*13*/|	NO_LOG_MSG_INIT(TDM__QC)		\
                    /*14*/|	NO_LOG_MSG_INIT(TDM__PIG)		\
                    /*15*/| NO_LOG_MSG_INIT(T__SOFT_CAESAR)         \
                    /*16*/| NO_LOG_MSG_INIT(T__BUFSCHEME)         \
                    /*17*/| NO_LOG_MSG_INIT(KEN_SPECIAL)            \
                    /*18*/| NO_LOG_MSG_INIT(KEN_FLOWDUMP)           \
                    /*19*/| NO_LOG_MSG_INIT(KEN_RDUMPTEST)          \
                    /*20*/| NO_LOG_MSG_INIT(T__SOFT_FAX_EL)          \
					/*21*/|	NO_LOG_MSG_INIT(T__VOICESESSION)		\
					}


    #define RTRACE0(TypeAndSeverity, LoggingThread, str)  RPT_Write(                      \
					(TypeAndSeverity | (LoggingThread << 16)),        \
					((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 13), \
					(UINT32)0)


	#define RTRACE1(TypeAndSeverity, LoggingThread, str, arg1)  RPT_Write(                         \
					(TypeAndSeverity | (LoggingThread << 16)),                     \
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 13) | (1 <<29)), \
					(UINT32)arg1)
	
	#define RTRACE2(TypeAndSeverity, LoggingThread, str, arg1, arg2)  RPT_Write(                   \
					(TypeAndSeverity | (LoggingThread << 16)),                     \
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 13) | (2 <<29)), \
					(UINT32)arg1, (UINT32)arg2)


	#define RTRACE3(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3)  RPT_Write(              \
					(TypeAndSeverity | (LoggingThread << 16)),                      \
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 13) | (3 <<29)),  \
					(UINT32)arg1, (UINT32)arg2, (UINT32)arg3)


	#define RTRACE4(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4)  RPT_Write(       \
					(TypeAndSeverity | (LoggingThread << 16)),                     \
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 13) | (4 <<29)), \
					(UINT32)arg1, (UINT32)arg2, (UINT32)arg3, (UINT32)arg4)


	#define RTRACE5(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5)  RPT_Write(        \
					(TypeAndSeverity | (LoggingThread << 16)),                            \
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 13) | (5 <<29)),        \
					(UINT32)arg1, (UINT32)arg2, (UINT32)arg3, (UINT32)arg4, (UINT32)arg5)						


	#define RTRACE6(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6)  RPT_Write(                \
					(TypeAndSeverity | (LoggingThread << 16)),                                          \
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 13) | (6 <<29)),                      \
					(UINT32)arg1, (UINT32)arg2, (UINT32)arg3, (UINT32)arg4, (UINT32)arg5, (UINT32)arg6)


	#define RTRACE7(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7)  RPT_Write(                        \
					(TypeAndSeverity | (LoggingThread << 16)),                                                        \
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 13) | (7 <<29)),                                    \
					(UINT32)arg1, (UINT32)arg2, (UINT32)arg3, (UINT32)arg4, (UINT32)arg5, (UINT32)arg6, (UINT32)arg7)

    #define RTRACE_DUMP(TypeAndSeverity, LoggingThread, BufferAddress, Size)  RPT_Write( \
					(TypeAndSeverity | (LoggingThread << 16)),       \
					(RTRACE_DUMP_CODE << 13),                        \
					(UINT32)BufferAddress,                           \
					(UINT32)Size)

    #define RDUMP(TypeAndSeverity, LoggingThread, BufferAddress, Size)  RPT_Write( \
			    (TypeAndSeverity | (LoggingThread << 16)),             \
			    (RDUMP_CODE << 13),                                    \
			    (UINT32)BufferAddress,                                 \
			    (UINT32)Size)


    #define RTRACE_EOS(TypeAndSeverity, LoggingThread)  RPT_Write(     \
			    (TypeAndSeverity | (LoggingThread << 16)), \
			    (RTRACE_EOS_CODE << 13))


    #define RDUMP_EOS(TypeAndSeverity, LoggingThread)  RPT_Write(      \
			    (TypeAndSeverity | (LoggingThread << 16)), \
			    (RDUMP_EOS_CODE << 13))

#else  /* defined BLAM22 */

	#include "rpt_io.h"

	/*additional severity mask bits to handle TRACEDM conversion */
	#define TDM__ERROR		0x10000000
	#define TDM__WARNING	0x08000000


	/* */
	/*used to initialize Global Severity filter at port open */
	/* */
	#define BLAM_SEVERITY_DEFAULT (T__FATAL_ERROR|T__ERROR|TDM__ERROR)


	/*arbitrary at the moment */
	#define MAX_NUM_RTRACE_IDS 256

	/*NOTE:  New trace types can be added into this enum anywhere and tracing should */
	/*       work after a recompile.  However, the BLAM application uses this enum */
	/*       to create its database and has trouble parsing comments within it so */
	/*       at the present time PLEASE ADD NO COMMENTS WITHIN THIS ENUM!!! */
	/*       ALSO: Note that any changes to this enum needs corresponding (order */
	/*       dependent changes tot he BLAM_Filters_Default list below.... */
	enum RTRACE_GROUP {	
		T__CALLPROG, 
		T__SESSIONMGR,     
		T__STREAMMGR,
		T__DATASESSION,
		T__MNP,
		T__V42,
		T__V42BIS,
		T__FAXSESSION,
		T__FAXPIM,
		T__EEPROM,
		T__ATPARSER,
		T__COMMCTRL,
		T__UTILS,
		T__RTMGR,
		T__CONFIGMGR,
		T__V8BIS,
		T__STATMGR,
		T__HAL,
		T__HALOS,
		T__HALSCRIPT,
		T__PORT,
		T__V8,
		T__PROFILE,
		T__NVMMGR,
		T__TESTSESSION,
		T__VOICESESSION,
		T__DRIVER,
		T__CALLERID,
		T__MOH,
		T__QC,
		T__DIAG,
        T__CW,
        T__SOFT,
        T__SOFT_AUTOMODE,
        T__SOFT_DATA_EL,
        T__SOFT_DIALER_EL,
		T__SOFT_FAX_EL,
        T__SOFT_RINGDET,
        T__SOFT_TONE_EL,
        T__SOFT_SIGNALMGR,
        T__SOFT_B2HW,
        T__SOFT_HW,
        T__SOFT_FILTER_EC,
        T__SOFT_FRED,
        T__SOFT_MODEM_CONFIG,
        T__SOFT_FSK,
        T__SOFT_QAM,
        T__SOFT_V22,
        T__SOFT_V32,
        T__SOFT_HDLC,
        T__SOFT_PROFILER,
        T__SOFT_CAESAR,
		T__DEVMGR_PTT,
		T__BUFSCHEME,
        TDM__ENTRY,
		TDM__DM,
		TDM__DPDWNLD,
		TDM__V8BIS,
		TDM__V8,
		TDM__SOFTRI,
		TDM__LMON,
		TDM__HDLC,
		TDM__V14,
		TDM__VSP,
		TDM__HWDS,
		TDM__FAX,
		TDM__WA0,
		TDM__WA1,
		TDM__WA2,
		TDM__FILTER,
		TDM__ASIC,
		TDM__TEST,
		TDM__PULSED,
		TDM__DAA,
		TDM__VOICE,
		TDM__HAL,
		TDM__HALOS,
		TDM__HALDATA,
		TDM__CID,
		TDM__CW,
		TDM__V92,
		TDM__MOH,
		TDM__QC,
        TDM__PIG,

		Num_Messages
	};


	/*The FOllowing list is used to initialize trace ON/OFF defaults */
	/*for all of the above enum elements at port open time.  This list may */
	/*be overridden by soft defaults (such as registry....).   */
	/* */
	/*NOTE:		Any change to the above list MUST have a corresponding change to  */
	/*			this list...and THIS list IS order dependent -- it is used */
	/*          to initialize an array of filter criteria values...!!!!! */
	#define BLAM_FILTERS_DEFAULTS {											\
									YES_LOG_MSG_INIT,  /*T__CALLPROG*/		\
									YES_LOG_MSG_INIT,  /*T__SESSIONMGR*/	\
									YES_LOG_MSG_INIT,  /*T__STREAMMGR*/		\
									YES_LOG_MSG_INIT,  /*T__DATASESSION*/	\
									YES_LOG_MSG_INIT,  /*T__MNP*/			\
									DONT_LOG_MSG_INIT, /*T__V42*/			\
									DONT_LOG_MSG_INIT, /*T__V42BIS*/		\
									YES_LOG_MSG_INIT,  /*T__FAXSESSION*/	\
									YES_LOG_MSG_INIT,  /*T__FAXPIM*/		\
									YES_LOG_MSG_INIT,  /*T__EEPROM*/		\
									YES_LOG_MSG_INIT,  /*T__ATPARSER*/		\
									YES_LOG_MSG_INIT,  /*T__COMMCTRL*/		\
									YES_LOG_MSG_INIT, /*T__UTILS*/			\
									DONT_LOG_MSG_INIT, /*T__RTMGR*/			\
									DONT_LOG_MSG_INIT, /*T__CONFIGMGR*/		\
									DONT_LOG_MSG_INIT, /*T__V8BIS*/			\
									DONT_LOG_MSG_INIT, /*T__STATMGR*/		\
									DONT_LOG_MSG_INIT, /*T__HAL*/			\
									DONT_LOG_MSG_INIT, /*T__HALOS*/			\
									YES_LOG_MSG_INIT,  /*T__HALSCRIPT*/		\
									DONT_LOG_MSG_INIT, /*T__PORT*/			\
									YES_LOG_MSG_INIT,  /*T__V8*/			\
									YES_LOG_MSG_INIT,  /*T__PROFILE*/		\
									YES_LOG_MSG_INIT,  /*T__NVMMGR*/		\
									YES_LOG_MSG_INIT,  /*T__TESTSESSION*/	\
									YES_LOG_MSG_INIT,  /*T__VOICESESSION*/	\
									DONT_LOG_MSG_INIT, /*T__DRIVER*/		\
									YES_LOG_MSG_INIT,  /*T__CALLERID*/		\
									YES_LOG_MSG_INIT,  /*T__MOH*/			\
									YES_LOG_MSG_INIT,  /*T__QC*/			\
									YES_LOG_MSG_INIT,  /*T__DIAG*/			\
                                    YES_LOG_MSG_INIT,  /*T__CW*/			\
                                    YES_LOG_MSG_INIT,  /*T__SOFT*/			\
                                    YES_LOG_MSG_INIT,  /*T__SOFT_AUTOMODE*/ \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_DATA_EL*/  \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_DIALER_EL*/\
                                    YES_LOG_MSG_INIT,  /*T__SOFT_FAX_EL*/	\
                                    YES_LOG_MSG_INIT,  /*T__SOFT_RINGDET*/  \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_TONE_EL*/  \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_SIGNALMGR*/\
                                    YES_LOG_MSG_INIT,  /*T__SOFT_B2HW*/     \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_HW*/       \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_FILTER_EC*/\
                                    YES_LOG_MSG_INIT,  /*T__SOFT_FRED*/     \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_MODEM_CONFIG*/\
                                    YES_LOG_MSG_INIT,  /*T__SOFT_FSK*/      \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_QAM*/      \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_V22*/      \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_V32*/      \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_HDLC*/      \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_PROFILER*/  \
                                    YES_LOG_MSG_INIT,  /*T__SOFT_CAESAR*/  \
									YES_LOG_MSG_INIT,  /*TDM__ENTRY*/		\
									YES_LOG_MSG_INIT,  /*TDM__DM*/			\
									YES_LOG_MSG_INIT, /*TDM__DPDWNLD*/		\
									DONT_LOG_MSG_INIT, /*TDM__V8BIS*/		\
									YES_LOG_MSG_INIT,  /*TDM__V8*/			\
									YES_LOG_MSG_INIT,  /*TDM__SOFTRI*/		\
									YES_LOG_MSG_INIT,  /*TDM__LMON*/		\
									DONT_LOG_MSG_INIT, /*TDM__HDLC*/		\
									YES_LOG_MSG_INIT,  /*TDM__V14*/			\
									YES_LOG_MSG_INIT, /*TDM__VSP*/			\
									YES_LOG_MSG_INIT,  /*TDM__HWDS*/		\
									YES_LOG_MSG_INIT,  /*TDM__FAX*/			\
									YES_LOG_MSG_INIT,  /*TDM__WA0*/			\
									YES_LOG_MSG_INIT,  /*TDM__WA1*/			\
									YES_LOG_MSG_INIT,  /*TDM__WA2*/			\
									YES_LOG_MSG_INIT,  /*TDM__FILTER*/		\
									YES_LOG_MSG_INIT,  /*TDM__ASIC*/		\
									YES_LOG_MSG_INIT,  /*TDM__TEST*/		\
									YES_LOG_MSG_INIT,  /*TDM__PULSED*/		\
									YES_LOG_MSG_INIT,  /*TDM__DAA*/			\
									YES_LOG_MSG_INIT,  /*TDM__VOICE*/		\
									YES_LOG_MSG_INIT,  /*TDM__HAL*/			\
									DONT_LOG_MSG_INIT, /*TDM__HALOS*/		\
									DONT_LOG_MSG_INIT, /*TDM__HALDATA*/		\
									YES_LOG_MSG_INIT,  /*TDM__CID*/			\
									YES_LOG_MSG_INIT,  /*TDM__CW*/			\
									YES_LOG_MSG_INIT,  /*TDM__V92*/			\
									YES_LOG_MSG_INIT,  /*TDM__MOH*/			\
									YES_LOG_MSG_INIT,  /*TDM__QC*/			\
                                    DONT_LOG_MSG_INIT, /*TDM__PIG*/           \
								}


	/*SINCE THE RTRACE APPLICATION IS DESIGNED TO HANDLE MULTIPLE CLIENTS WE MUST ASSIGN AN ID NUMBER TO OUT CLIENT */
	#define DW_CLIENT_ID ((UINT32)0)
	
	/*already defined in rpt_io.h */
/*	#define	LINE_NUM_MASK				((DWORD) 0x00003FFF)	DH	BITS 0-13 */

    #define RTRACE0(TypeAndSeverity, LoggingThread, str)  RPT_Write(			\
					DW_CLIENT_ID,												\
					(TypeAndSeverity | (LoggingThread << 16)),					\
					((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 14),			\
					0);


	#define RTRACE1(TypeAndSeverity, LoggingThread, str, arg1)  RPT_Write(				\
					DW_CLIENT_ID,														\
					(TypeAndSeverity | (LoggingThread << 16)),							\
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 14) | (1 <<29)),		\
					arg1);

	
	#define RTRACE2(TypeAndSeverity, LoggingThread, str, arg1, arg2)  RPT_Write(	\
					DW_CLIENT_ID,													\
					(TypeAndSeverity | (LoggingThread << 16)),						\
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 14) | (2 <<29)),	\
					arg1, arg2);


	#define RTRACE3(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3)  RPT_Write(	\
					DW_CLIENT_ID,													\
					(TypeAndSeverity | (LoggingThread << 16)),						\
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 14) | (3 <<29)),	\
					arg1, arg2, arg3)


	#define RTRACE4(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4)  RPT_Write(	\
					DW_CLIENT_ID,																\
					(TypeAndSeverity | (LoggingThread << 16)),									\
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 14) | (4 <<29)),				\
					arg1, arg2, arg3, arg4)


	#define RTRACE5(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5)  RPT_Write(		\
					DW_CLIENT_ID,																		\
					(TypeAndSeverity | (LoggingThread << 16)),											\
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 14) | (5 <<29)),						\
					arg1, arg2, arg3, arg4, arg5)						


	#define RTRACE6(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6)  RPT_Write(	\
					DW_CLIENT_ID,																			\
					(TypeAndSeverity | (LoggingThread << 16)),												\
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 14) | (6 <<29)),							\
					arg1, arg2, arg3, arg4, arg5, arg6)


	#define RTRACE7(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7)  RPT_Write(		\
					DW_CLIENT_ID,																					\
					(TypeAndSeverity | (LoggingThread << 16)),														\
					(((__LINE__) & LINE_NUM_MASK) | (FILEIDNUM << 14) | (7 <<29)),									\
					arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#endif /* defined BLAM22 */

#else     /* !defined NO_BLAM_SUPPORT */

#if ((defined SCR) && (OS_TYPE == OS_MACOSX))
	#include "rpt_io.h"
	#define DW_CLIENT_ID ((UINT32)0)
	#define Num_Messages 0
	#define BLAM_SEVERITY_DEFAULT 0

	#include "octopus/include/dbgscr_wrapper.h"

    #define RTRACE0(TypeAndSeverity, LoggingThread, str) DbgScrWrap_Write( str )
	#define RTRACE1(TypeAndSeverity, LoggingThread, str, arg1) DbgScrWrap_Write( str, arg1 )
	#define RTRACE2(TypeAndSeverity, LoggingThread, str, arg1, arg2) DbgScrWrap_Write( str, arg1, arg2 )
	#define RTRACE3(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3) DbgScrWrap_Write( str, arg1, arg2, arg3 )
	#define RTRACE4(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4) DbgScrWrap_Write( str, arg1, arg2, arg3, arg4 )
	#define RTRACE5(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5) DbgScrWrap_Write( str, arg1, arg2, arg3, arg4, arg5 )
	#define RTRACE6(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6) DbgScrWrap_Write( str, arg1, arg2, arg3, arg4, arg5, arg6 )
	#define RTRACE7(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7) DbgScrWrap_Write( str, arg1, arg2, arg3, arg4, arg5, arg6, arg7 )

    #define SCR_RTL_PRINT0(TypeAndSeverity, LoggingThread, str) DbgScrWrap_Write( str )
	#define SCR_RTL_PRINT1(TypeAndSeverity, LoggingThread, str, arg1) DbgScrWrap_Write( str, arg1 )
	#define SCR_RTL_PRINT2(TypeAndSeverity, LoggingThread, str, arg1, arg2) DbgScrWrap_Write( str, arg1, arg2 )
	#define SCR_RTL_PRINT3(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3) DbgScrWrap_Write( str, arg1, arg2, arg3 )
	#define SCR_RTL_PRINT4(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4) DbgScrWrap_Write( str, arg1, arg2, arg3, arg4 )
	#define SCR_RTL_PRINT5(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5) DbgScrWrap_Write( str, arg1, arg2, arg3, arg4, arg5 )
	#define SCR_RTL_PRINT6(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6) DbgScrWrap_Write( str, arg1, arg2, arg3, arg4, arg5, arg6 )
	#define SCR_RTL_PRINT7(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7) DbgScrWrap_Write( str, arg1, arg2, arg3, arg4, arg5, arg6, arg7 )

#ifdef DPRINTF
#undef DPRINTF
#endif
	#define DPRINTF( x ) DbgScrWrap_Write x
	#define TRACE(x, y ) DbgScrWrap_Write y
#else  /* ((defined SCR) && (OS_TYPE == OS_MACOSX)) */
//	#define TRACE(x, y ) 
#endif  /* ((defined SCR) && (OS_TYPE == OS_MACOSX)) */

    #define	T__CALLERID		0x00000001
    #define T__CALLPROG		0x00000002
    #define T__SESSIONMGR	0x00000004
    #define T__STREAMMGR	0x00000008
    #define T__DATASESSION	0x00000010
    #define T__MNP			0x00000020
    #define T__V42			0x00000040
    #define T__V42BIS		0x00000080
    #define T__FAXSESSION	0x00000100
    #define T__FAXPIM		0x00000200
    #define T__ATPARSER		0x00000400
    #define T__COMMCTRL		0x00000800
    #define T__UTILS		0x00001000
    #define T__RTMGR		0x00002000
    #define T__CONFIGMGR    0x00004000
	#define	T__MOH			0x00008000
    #define T__V8BIS        0x00010000
    #define T__STATMGR      0x00020000
	#define T__QC			0x00040000
    #define T__TIMING		0x00100000
    #define T__PORT         0x00200000
    #define T__V8           0x00400000
	#define	T__DIAG			0x00800000
    #define T__DRIVER	    0x01000000
    #define T__PROFILE		0x02000000
    #define T__NVMMGR		0x04000000
    #define T__TESTSESSION	0x08000000	  /*PQ */
	#define T__VOICESESSION 0x10000000	  /*#$YS$ */
    #define T__SOFT             0x00000001
    #define T__SOFT_AUTOMODE    0x00000001
    #define T__SOFT_DATA_EL     0x00000001
    #define T__SOFT_DIALER_EL   0x00000001
	#define T__SOFT_FAX_EL      0x00000001
    #define T__SOFT_RINGDET     0x00000001
    #define T__SOFT_TONE_EL     0x00000001
    #define T__SOFT_SIGNALMGR   0x00000001
    #define T__SOFT_B2HW        0x00000001
    #define T__SOFT_HW          0x00000001
    #define T__SOFT_FILTER_EC   0x00000001
    #define T__SOFT_FRED        0x00000001
    #define T__SOFT_MODEM_CONFIG 0x00000001
    #define T__SOFT_FSK         0x00000001
    #define T__SOFT_QAM         0x00000001
    #define T__SOFT_V22         0x00000001
    #define T__SOFT_V32         0x00000001
    #define T__SOFT_HDLC        0x00000001
    #define T__SOFT_PROFILER    0x00000001
    #define T__SOFT_CAESAR      0x00000001
	#define T__BUFSCHEME        0x00000001
    #define T__CW           T__CALLERID
	
	/*This lets us use either ulTraceMask or dwDmTraceMask assuming the  */
	/*os_x.h file is always included before the Testdebug.h file... */
	#ifndef NOBLAMTRACEMASK
		#define NOBLAMTRACEMASK  ulTraceMask
	#endif
    #define NOBLAMLOGGINGTHREAD ulLogThread

#if TARGET_HCF_FAMILY
	/*These are here to let code converted to BLAM trace via WinICE */
	#define RTRACE0(TypeAndSeverity, LoggingThread, str) {										\
                    if (((TypeAndSeverity) & NOBLAMTRACEMASK) ||	(LoggingThread == NOBLAMLOGGINGTHREAD)) \
                                {																\
                                    UINT32 dwTime = OsGetSystemTime();							\
                                    OsDebugPrintf ("%07u.%03u: HCF: ", dwTime/1000, dwTime%1000);    \
                                    OsDebugPrintf (str);										\
                                }																\
                            }


	#define RTRACE1(TypeAndSeverity, LoggingThread, str, arg1) {								\
                    if (((TypeAndSeverity) & NOBLAMTRACEMASK) ||	(LoggingThread == NOBLAMLOGGINGTHREAD)) \
                                {																\
                                    UINT32 dwTime = OsGetSystemTime();							\
                                    OsDebugPrintf ("%07u.%03u: HCF: ", dwTime/1000, dwTime%1000);    \
                                    OsDebugPrintf (str, arg1);									\
                                }																\
                            }

	#define NOTIME_RTRACE1(TypeAndSeverity, LoggingThread, str, arg1) {								\
                    if (((TypeAndSeverity) & NOBLAMTRACEMASK) ||	(LoggingThread == NOBLAMLOGGINGTHREAD)) \
                                {																\
                                    OsDebugPrintf (str, arg1);									\
                                }																\
                            }


	#define RTRACE2(TypeAndSeverity, LoggingThread, str, arg1, arg2) {						\
                    if (((TypeAndSeverity) & NOBLAMTRACEMASK) ||	(LoggingThread == NOBLAMLOGGINGTHREAD)) \
							{																\
                                UINT32 dwTime = OsGetSystemTime();							\
                                OsDebugPrintf ("%07u.%03u: HCF: ", dwTime/1000, dwTime%1000);    \
                                OsDebugPrintf (str, arg1, arg2);							\
                            }																\
                        }


	#define RTRACE3(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3) {	        \
                    if (((TypeAndSeverity) & NOBLAMTRACEMASK) ||	(LoggingThread == NOBLAMLOGGINGTHREAD)) \
						{												                \
                            UINT32 dwTime = OsGetSystemTime();					        \
                            OsDebugPrintf ("%07u.%03u: HCF: ", dwTime/1000, dwTime%1000);    \
                            OsDebugPrintf (str, arg1, arg2, arg3);                      \
						}															    \
                    }


	#define RTRACE4(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4) {	    \
                    if (((TypeAndSeverity) & NOBLAMTRACEMASK) ||	(LoggingThread == NOBLAMLOGGINGTHREAD)) \
					{												                    \
                        UINT32 dwTime = OsGetSystemTime();					            \
                        OsDebugPrintf ("%07u.%03u: HCF: ", dwTime/1000, dwTime%1000);    \
                        OsDebugPrintf (str, arg1, arg2, arg3, arg4);                    \
                    }																	\
                }


	#define RTRACE5(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5) {	\
                    if (((TypeAndSeverity) & NOBLAMTRACEMASK) ||	(LoggingThread == NOBLAMLOGGINGTHREAD)) \
					{																		\
                        UINT32 dwTime = OsGetSystemTime();									\
                        OsDebugPrintf ("%07u.%03u: HCF: ", dwTime/1000, dwTime%1000);    \
                        OsDebugPrintf (str, arg1, arg2, arg3, arg4, arg5);                  \
                    }																		\
                }


	#define RTRACE6(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6) {	\
                    if (((TypeAndSeverity) & NOBLAMTRACEMASK) ||	(LoggingThread == NOBLAMLOGGINGTHREAD)) \
					{																			\
                        UINT32 dwTime = OsGetSystemTime();										\
                        OsDebugPrintf ("%07u.%03u: HCF: ", dwTime/1000, dwTime%1000);    \
                        OsDebugPrintf (str, arg1, arg2, arg3, arg4, arg5, arg6);                \
                    }																			\
                }



	#define RTRACE7(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7) {	\
                    if (((TypeAndSeverity) & NOBLAMTRACEMASK) ||	(LoggingThread == NOBLAMLOGGINGTHREAD)) \
						{																				\
                            UINT32 dwTime = OsGetSystemTime();											\
                            OsDebugPrintf ("%07u.%03u: HCF: ", dwTime/1000, dwTime%1000);    \
                            OsDebugPrintf (str, arg1, arg2, arg3, arg4, arg5, arg6, arg7);              \
                        }																				\
                    }
#elif ( !defined SCR )
	/*These are here to let code converted to BLAM trace via WinICE */
	#define RTRACE0(TypeAndSeverity, LoggingThread, str) 
	#define RTRACE1(TypeAndSeverity, LoggingThread, str, arg1) 
	#define NOTIME_RTRACE1(TypeAndSeverity, LoggingThread, str, arg1) 
	#define RTRACE2(TypeAndSeverity, LoggingThread, str, arg1, arg2) 
	#define RTRACE3(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3) 
	#define RTRACE4(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4) 
	#define RTRACE5(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5)
	#define RTRACE6(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6)
	#define RTRACE7(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7) 

#endif /* TARGET_HCF_FAMILY */

#endif   /* !defined NO_BLAM_SUPPORT */


#if TARGET_HCF_FAMILY
    #define TRACE(mask,printf)  {                                           \
                                    if ((mask) & ulTraceMask)               \
                                    {                                       \
                                        UINT32 dwTime = OsGetSystemTime();  \
                                        OsDebugPrintf ("%07u.%03u: ", dwTime/1000, dwTime%1000); \
                                        OsDebugPrintf printf;               \
                                    }                                       \
                                }
#else
    #define TRACE(mask,printf)
#endif /* TARGET_HCF_FAMILY */

    #define DUMP(mask,printf)  {                                           \
                                    if ((mask) & ulTraceMask)               \
                                    {                                       \
                                        OsDebugPrintf printf;               \
                                    }                                       \
                                }
#else

    #define TRACE(mask,printf)
	#define RTRACE0(TypeAndSeverity, LoggingThread, str) 
	#define RTRACE1(TypeAndSeverity, LoggingThread, str, arg1) 
	#define NOTIME_RTRACE1(TypeAndSeverity, LoggingThread, str, arg1) 
	#define RTRACE2(TypeAndSeverity, LoggingThread, str, arg1, arg2) 
	#define RTRACE3(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3) 
	#define RTRACE4(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4) 
	#define RTRACE5(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5)
	#define RTRACE6(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6)
	#define RTRACE7(TypeAndSeverity, LoggingThread, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7) 

#endif /* defined(USE_TRACE) || defined(USE_TRACE_ONLY) */

	#define DUMPDATA(mask, data, len)

#endif  /* #ifndef __DEBUG_H__ */
