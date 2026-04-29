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
*	$Header:   P:/V68_Projects/Octopus_PDB/archives/Octopus files/Include files/targetconfigtypes.h_v   1.1   11 Jul 2003 17:03:20   rayskymy  $
*
*****************************************************************************************/


/****************************************************************************************

File Name:			TargetConfigTypes.h

File Description:	Configuration parameter data structures specific to controllerless modem

*****************************************************************************************/
#if !defined __TARGETCONFIGTYPES_H__
#define __TARGETCONFIGTYPES_H__

#define DAA_RELAY_TYPE  UINT32

#pragma pack(1)

typedef struct PACKED_ATTRIB FilterThreshold_Tag {
	UINT16	ThreshU;
	UINT16	ThreshL;
} FilterThresholdStructure;

typedef struct PACKED_ATTRIB tagTHRESHOLD 
{
	FilterThresholdStructure DialThresh;
	FilterThresholdStructure AltDialThresh;
	FilterThresholdStructure WDialThresh;
	FilterThresholdStructure AltWDialThresh;
	FilterThresholdStructure ProgThresh;
	FilterThresholdStructure AltProgThresh;
	FilterThresholdStructure VoiceToneACallProgressThresh;
	UINT16 DTMFRxThresh;
	UINT16 RingThreshold;	/*[Modif0021]LKS03jun98: Add soft ring threshold set to 0 if not use*/
	short int RingBalance;	/*[Modif0021]LKS28jul98: Add soft ring threshold compensation slope, set to 0 if not use*/
} CntryThresholdStructure;

/* caesar params */


#define DEFAULT_DC_LOOP_POINTS	5
#define DEFAULT_DC_LOOP_ENTRIES	5
#define IMPEDANCE_VALUES_NUM	4
#define NUM_OF_RESISTANCES		5	

typedef struct tagCAESAR_PARAMS
{
   UINT32	R6;
   UINT32	R26;
   UINT32	R28;
   UINT32	R30_X4;
   UINT32	R30_X6;
   UINT32	R38;

   UINT32	PQac;
   UINT32	PQdc;

   UINT8	ImpedanceValues[IMPEDANCE_VALUES_NUM];

   UINT8	DCLoopVoltagePoint[DEFAULT_DC_LOOP_ENTRIES][DEFAULT_DC_LOOP_POINTS];
   UINT8	DCLoopCurrentPoint[DEFAULT_DC_LOOP_ENTRIES][DEFAULT_DC_LOOP_POINTS];
   UINT8	DCDACInitPoint[DEFAULT_DC_LOOP_ENTRIES];
   UINT8	cTargetLSDVdd;
   UINT32	Flags;
} CAESAR_PARAMS, *PCAESAR_PARAMS;

#define QC_PROFILE_LENGTH     3056
#define QC_PROFILE_RAMDATA_VALID		0x01
#define	QC_PROFILE_IAGAINS_VALID		0x10
#define	QC_PROFILE_WITH_PCMUPSTREAM		0x02

#define V92_PCMUPSTREAM_ENABLED			0x01
#define V92_PCMUPSTREAM_SYMBOLRATE_CHECKED	0x02
#define V92_PCMUPSTREAM_ACTIVE			0x10


typedef struct QCProfileStructTag
{
	UINT32	uiVersion;
	UINT8	RamData[QC_PROFILE_LENGTH];
	UINT8	CRC;
	UINT16	RTD;
	UINT8	IAGain1;
	UINT8	IAGain2;
	UINT8	PreEmphasis;
	UINT16	TxLevel;
	UINT8	CEQ;
	UINT8	AutoGain;
	BOOL	bAlaw;
	UINT16	wDCTap;
	/*fields for download */
	UINT16	RamDataCount;
	UINT8	RamDataStart;
	UINT8	CalCRC;
	UINT8	ProfileValid;
} QCProfileStruct;

#pragma pack()

#endif  /* !defined __TARGETCONFIGTYPES_H__ */
