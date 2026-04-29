/*
 * Copyright (c) 2003 Linuxant inc.
 * Copyright (c) 2001-2003 Conexant Systems, Inc.
 *
 * NOTE: The use and distribution of this software is governed by the terms in
 * the file LICENSE, which is included in the package. You must read this and
 * agree to these terms before using or distributing this software.
 *
 */

/*
 MODULE NAME:    Rp_Cmdif.h
 
 AUTHOR:         Duong D. Nguyen 


 HISTORY:        Major Revision                     Date         By
                 -----------------------------      --------     -----
                 Created from NADA                  08/25/97     KRS
                 Ported from rasm.h                 09/04/97     DDN
                 Ported to Linux                    09/01/99     KNL


  DESCRIPTION:

                This module contains C Language include
                defs for the  RipTide Command Interface.
        

  NOTES:        None.  

*/

#include "linux.h"

#define CMDIF (*pCMDIF)

/*
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////// C O M M A N D   I N T E R F A C E  D E F I N I T I O N S
///////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
*/
#define  CMDIF_TIMEOUT         5000
               
#define  AUDIO_CONTROL         0x0
#define  AUDIO_STATUS          0x4
#define  CMDA_OFFSET           0x10 
#define  DATA1A_OFFSET         0x10   
#define  PARMA_OFFSET          0x14     
#define  DATA2A_OFFSET         0x14     
#define  STATA_OFFSET          0x18             
#define  CMDB_OFFSET           0x30 
#define  DATA1B_OFFSET         0x30   
#define  PARAMB_OFFSET         0x34
#define  DATA2B_OFFSET         0x34   
#define  STATB_OFFSET          0x38
    

typedef struct
{  
   HW_PORT audio_control;
   HW_PORT audio_status;
   HW_PORT cmda;
   HW_PORT data1a;
   HW_PORT parma;
   HW_PORT data2a;
   HW_PORT stata; 
   HW_PORT cmdb;
   HW_PORT data1b;
   HW_PORT parmb;
   HW_PORT data2b;
   HW_PORT statb;
   
} tCMDIF_AB, *pCMDIF_AB;    

#define READ_AUDIO_CONTROL        READ_PORT_ULONG  (CMDIF.audio_control)
#define WRITE_AUDIO_CONTROL( x )  WRITE_PORT_ULONG (CMDIF.audio_control, x) 
#define UMASK_AUDIO_CONTROL( x )  WRITE_PORT_ULONG (CMDIF.audio_control, READ_PORT_ULONG(CMDIF.audio_control) | x) 
#define MASK_AUDIO_CONTROL( x )   WRITE_PORT_ULONG (CMDIF.audio_control, READ_PORT_ULONG(CMDIF.audio_control) & x)
#define READ_AUDIO_STATUS         READ_PORT_ULONG  (CMDIF.audio_status)
#define WRITE_CMDB( x )           WRITE_PORT_ULONG (CMDIF.cmdb, x)
#define WRITE_DATA1B( x )         WRITE_PORT_ULONG (CMDIF.data1b, x)
#define WRITE_PARMB( x )          WRITE_PORT_ULONG (CMDIF.parmb,  x)
#define WRITE_DATA2B( x )         WRITE_PORT_ULONG (CMDIF.data2b, x)
#define WRITE_STATB( x )          WRITE_PORT_ULONG (CMDIF.statb,  x)
#define READ_CMDB                 READ_PORT_ULONG  (CMDIF.cmdb  )
#define READ_DATA1B               READ_PORT_ULONG  (CMDIF.data1b)
#define READ_PARMB                READ_PORT_ULONG  (CMDIF.parmb )
#define READ_DATA2B               READ_PORT_ULONG  (CMDIF.data2b)
#define READ_STATB                READ_PORT_ULONG  (CMDIF.statb )
#define WRITE_CMDA( x )           WRITE_PORT_ULONG (CMDIF.cmda,   x)
#define WRITE_DATA1A( x )         WRITE_PORT_ULONG (CMDIF.data1a, x)
#define WRITE_PARMA( x )          WRITE_PORT_ULONG (CMDIF.parma,  x)
#define WRITE_DATA2A( x )         WRITE_PORT_ULONG (CMDIF.data2a, x)
#define WRITE_STATA               WRITE_PORT_ULONG (CMDIF.stata,  x)
#define READ_CMDA                 READ_PORT_ULONG  (CMDIF.cmda  )
#define READ_DATA1A               READ_PORT_ULONG  (CMDIF.data1a)
#define READ_PARMA                READ_PORT_ULONG  (CMDIF.parma )
#define READ_DATA2A               READ_PORT_ULONG  (CMDIF.data2a)
#define READ_STATA                READ_PORT_ULONG  (CMDIF.stata )


#define CMD_EMPTY         0x1   
#define DATA_FULL         0x2 

#define CMDAE  ( READ_STATA & CMD_EMPTY ) 
#define DATAF  ( READ_STATA & DATA_FULL )
/*
#define CMDAE  ( READ_AUDIO_STATUS & 0x10 ) 
#define DATAF  ( READ_AUDIO_STATUS & 0x40 )
*/
#define CMDBE  ( READ_STATB & CMD_EMPTY ) 
#define DATBF  ( READ_STATB & DATA_FULL )  
 
/*                                                     
//types
//Command Return array defintion, access as bytes, words, or dwords
*/
typedef union tagCMDRet
{
  UCHAR  bRetBytes [8];
  USHORT wRetWords [4];
  ULONG  dRetDwords[2];
} CMDRET_TYPE, *PCMDRET_TYPE;

/*
//flags -- These are used in the macros that call RiptideSendCmd.  CMDA/CMDB is specified
//         by the calling client code.  The others are specified by the macros according to
//         the specific type of command being sent. Note that some of these flags are 
//         possibly redundant since RiptideSendCmd can figure out the same information by 
//         seeing that the second parameter  dword and/or the response address is/are null.
//         The offset macros below are used to define 8,18,24, and 32 bit fields within the
//         two possible 32-bit command register dwords, corresponding to each command. See
//         their use in these macros (for example SEND_SSRC, SENT_GETV, etc.)
//
*/

#define YES_RESP     ((ULONG)0x00000001)  /*cmd returns response(s)*/
#define NO_RESP      ((ULONG)0x00000000)  /*cmd does not return response(s)*/
#define USES_PARMREG ((ULONG)0x00000002)  /*cmd has parm(s) in PARMA,B*/
#define CMDA         ((ULONG)0x00000000)  /*client specifies toCMDAinterface*/
#define CMDB         ((ULONG)0x00000004)  /*client specifies to CMDB interface  */
#define NILL         ((ULONG)0x00000000)  /*a DWORD NULL, for unused thingies */ 

/*Single Byte Parameter Offset Macros*/

#define B_0(arg)      ((ULONG)arg)
#define B1(arg)      ((ULONG)(arg)<<8)
#define B2(arg)      ((ULONG)(arg)<<16)
#define B3(arg)      ((ULONG)(arg)<<24)

/*16-bit Word Offset Parameter Macros*/
#define W01(arg)     ((ULONG)arg)
#define W23(arg)     ((ULONG)(arg)<<16)
#define W12(arg)     ((ULONG)(arg)<<8)

/*24-bit Offset Parameter Macros*/
#define W24012(arg)  ((ULONG)arg)
#define W24123(arg)  ((ULONG)(arg)<<8)

/*32-bit Macro*/
#define DW(arg)      ((ULONG)arg)

/*                               
//////////////////////////////////////////////////////////////////////////////////////////////
// MACRO TO SEND A COMMAND THAT USES BOTH CMD/PARM REGS BUT RETURNS NO RESPONSE INFORMATION
//--------------------------------------------------------------------------------------------
//Generate a call to RiptideSendCmd for commands that yeild no reponse
//and that use both the CMDA or CMDB register AND the PARMA or PARMB register.
//See SEND_SSRC command definition for an example of adding commands of 
//this type.  NOTES: The CMDAorB parameter must always be supplied. The CMDandPARMS parameter
//must have the 8-bit command identifier ORed into the low byte.
//////////////////////////////////////////////////////////////////////////////////////////////
*/

#define YESPARMAorB_NoRET(CMDAorB,CMDandPARMS,PARMAorB) RiptideSendCmd(pCMDIF,(ULONG)(CMDAorB|USES_PARMREG),(ULONG)(CMDandPARMS),(ULONG)(PARMAorB),(PCMDRET_TYPE)NILL)

/*
//--------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////
// MACRO TO SEND A COMMAND THAT DOESN'T USE THE SECOND PARM REG BUT RETURNS RESPONSE INFORMATION
//--------------------------------------------------------------------------------------------
//Generate a call to RiptideSendCmd for commands that yeilds a reponse
//and does NOT use the PARMA or PARMB register.
//See SEND_GETV command definition for an example of adding commands of 
//this type.  NOTES: The CMDAorB parameter must always be supplied and must
//be the RETADR parameter. The CMDandPARMS parameter
//must have the 8-bit command identifier ORed into the low byte.
//////////////////////////////////////////////////////////////////////////////////////////////
*/

#define NOPARMAorB_YESRET(CMDAorB,RETADR,CMDandPARMS)   RiptideSendCmd(pCMDIF,(ULONG)(CMDAorB|YES_RESP),(ULONG)(CMDandPARMS),NILL,(PCMDRET_TYPE) (RETADR))

/*
//--------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////
// MACRO TO SEND A COMMAND THAT USES BOTH CMD/PARM REGS AND RETURNS RESPONSE INFORMATION
//--------------------------------------------------------------------------------------------
//Generate a call to RiptideSendCmd for commands that yeilds a reponse
//and does use the PARMA or PARMB register.
//See SEND_RSRC command definition for an example of adding commands of 
//this type.  NOTES: The CMDAorB parameter must always be supplied and must
//be the RETADR parameter. The CMDandPARMS parameter
//must have the 8-bit command identifier ORed into the low byte.
//////////////////////////////////////////////////////////////////////////////////////////////
*/

#define YESPARMAorB_YESRET(CMDAorB,RETADR,CMDandPARMS,PARMAorB) RiptideSendCmd(pCMDIF,(ULONG)(CMDAorB|USES_PARMREG|YES_RESP),(ULONG)(CMDandPARMS),(ULONG)(PARMAorB),(PCMDRET_TYPE) (RETADR))
/*
//--------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////
// MACRO TO SEND A COMMAND THAT DOESN'T USE THE SECOND PARM REG AND RETURNS NO RESPONSE INFO
//--------------------------------------------------------------------------------------------
//Generate a call to RiptideSendCmd for commands that yeilds no reponse
//and do NOT use the PARMA or PARMB register.NOTES: The CMDAorB parameter must always be 
//supplied and must be the RETADR parameter. The CMDandPARMS parameter
//must have the 8-bit command identifier ORed into the low byte.
//////////////////////////////////////////////////////////////////////////////////////////////
*/
#define NOPARMAorB_NORET(CMDAorB,CMDandPARMS)   RiptideSendCmd(pCMDIF,(ULONG)(CMDAorB),(ULONG)(CMDandPARMS),NILL,(PCMDRET_TYPE) (NILL))

/*
//--------------------------------------------------------------------------------------------

///////////////////////////////////////////
///////// L I S T   O F   C O M M A N D S
///////////////////////////////////////////
//RipTide Host to ARM Command Identifiers 
//this value becomes byte 0 of CMDA or CMDB
//and is typically supplied automatically by
//the macro expansion in the client code
//when it invokes one of the SEND_XXXX macros
//where XXXX is one of the following symbols
//------------------------------------------  


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////    S E N D   C O M M A N D  M A C R O S     ////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------
//The RipTide code sends commands to the audio device using ONLY the macros defined
//below.  There is one macro for each possible RipTide command.  There are several flavors:
//commands that send responses,  commands that don't, commands that need the second 32-bit
//"PARMA or B" register, commands that don't, and various combinations.  These attributes are
//a result of the command definitin in the RipTide Host Interface Definitin Document.
//-----------------------------------------------------------------------------------------
*/

/*
 *  General Command Definitions
 */

#define GENERAL_CMD       0x00             /* This is the base for gernal command */

#define GETV              (GENERAL_CMD)      /* Get version summary                 */
#define GETC              (GENERAL_CMD + 1)  /* Get component information           */
#define GUNS              (GENERAL_CMD + 2)  /* Get unsolicited status infor.       */

#define SEND_GETV(GenCtrll,rptr)	NOPARMAorB_YESRET(GenCtrll,rptr,GETV)                
 
#define SEND_GETC(GenCtrll,ICOMP,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,GETC,DW(ICOMP))

#define SEND_GUNS(GenCtrll,rptr)	NOPARMAorB_YESRET(GenCtrll,rptr,GUNS)

/*
 *  Setup caller ID info
 */

#define SCID       (GENERAL_CMD + 0x71)

#define SEND_SCID(GenCtrll, Param)	YESPARMAorB_NoRET(GenCtrll,SCID,DW(Param))

/*
 *  ARM Diagnostics Command Definitions
 */

#define ARM_CMD           ((ULONG) 0x10)

#define RMEM              ARM_CMD          /* Read ARM memory                     */
#define SMEM              (ARM_CMD + 1)    /* Set ARM memory write address        */
#define WMEM              (ARM_CMD + 2)    /* Write ARM memory                    */
#define SDTM              (ARM_CMD + 3)    /* Select diagnostic or test mode      */ 
#define GOTO              (ARM_CMD + 4)    /* Start firmware from givven address  */


#define SEND_RMEM(GenCtrll,MODE,ADR,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,(RMEM) | (B1(MODE)),DW(ADR))

#define SEND_SMEM(GenCtrll,MODE,ADR)	YESPARMAorB_NoRET(GenCtrll,(SMEM) | (B1(MODE)),DW(ADR))
    
#define SEND_WMEM(GenCtrll,MODE,VAL)	YESPARMAorB_NoRET(GenCtrll,(WMEM) | (B1(MODE)),DW(VAL))   
                                                                                     
#define SEND_SDTM(GenCtrll,TMODE,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,(SDTM) | (W24123(TMODE)),0) 
  
// Start modem data pump
#define ARM_SETDPLL 0x72

#define SEND_SETDPLL(GenCtrll)	NOPARMAorB_NORET(GenCtrll,ARM_SETDPLL)

// Added to load firmware (CLN)
#define SEND_GOTO(GenCtrll,ADR)	YESPARMAorB_NoRET(GenCtrll,GOTO,DW(ADR))

/*
  Audio Stream Control Command Definitions 
*/

#define AUDIO_CMD         ((ULONG) 0x20)

#define SSTR              (AUDIO_CMD + 0)   /* Start stream                        */
#define PSTR              (AUDIO_CMD + 1)   /* Pause stream                        */
#define KSTR              (AUDIO_CMD + 2)   /* Kill  stream                        */  
#define KDMA              (AUDIO_CMD + 3)   /* Kill all audio stream               */
#define GPOS              (AUDIO_CMD + 4)   /* Get stream position                 */
#define SETF              (AUDIO_CMD + 5)   /* Setup DMA stream format             */ 
#define GSTS              (AUDIO_CMD + 6)
#define NGPOS             (AUDIO_CMD + 7)

#define SEND_SSTR(GenCtrll,STREAMN,LISTHEAD)	YESPARMAorB_NoRET(GenCtrll,SSTR | B3(STREAMN),DW(LISTHEAD))

#define SEND_PSTR(GenCtrll,STREAMN)	YESPARMAorB_NoRET(GenCtrll,PSTR,B3(STREAMN))

#define SEND_KSTR(GenCtrll,STREAMN)	YESPARMAorB_NoRET(GenCtrll,KSTR,B3(STREAMN))
 
#define SEND_KDMA(GenCtrll)	NOPARMAorB_NORET(GenCtrll,KDMA)
                                                                                        
#define SEND_GPOS(GenCtrll,MODE,STREAMN,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,GPOS,B3(STREAMN)|B2(MODE))

#define SEND_SETF(GenCtrll,M,W,CH,ORDER,SIG,STREAMN)	YESPARMAorB_NoRET(GenCtrll,SETF|W12(M)|B3(W),CH|B1(ORDER)|B2(SIG)|B3(STREAMN))

#define SEND_GSTS(GenCtrll,MODE,STREAMN,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,GSTS,B3(STREAMN)|B2(MODE))

#define SEND_NGPOS(GenCtrll,MODE,STREAMN,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,NGPOS,B3(STREAMN)|B2(MODE))

/*
  Digital Path Select ( MUX ) Command Definitions
*/

#define MUX_CMD           ((ULONG)0x30)

#define PSEL              (MUX_CMD + 0)      /* Path select                         */
#define PCLR              (MUX_CMD + 1)      /* Path clear                          */
#define PLST              (MUX_CMD + 2)      /* Select pre-canned list of path      */
#define RSSV              (MUX_CMD + 3)      /* Read given Sink and/or Source tag   */
#define LSEL              (MUX_CMD + 4)      /* Notify all legacy supports */

#define SEND_PSEL(GenCtrll,E1SOURCE,E2SINK)	YESPARMAorB_NoRET(GenCtrll,PSEL,B2(E1SOURCE)|B3(E2SINK))
                                    
#define SEND_PCLR(GenCtrll,E1SOURCE,E2SINK)	YESPARMAorB_NoRET(GenCtrll,PCLR,B2(E1SOURCE)|B3(E2SINK))       

#define SEND_PLST(GenCtrll,PROFILEID)	YESPARMAorB_NoRET(GenCtrll,PLST,B3(PROFILEID))             

#define SEND_RSSV(GenCtrll,E1SOURCE,E2SINK,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,RSSV,B2(E1SOURCE)|B3(E2SINK))                          

#define SEND_LSEL(GenCtrll,cmd,p1,p2,p3,p4,p5,p6)	YESPARMAorB_NoRET(GenCtrll,(LSEL|B1(cmd)|B2(p1)|B3(p2)),(B_0(p3)|B1(p4)|B2(p5)|B3(p6)))    

/*
  Sample Rate Converter Setup Command Definitions 
*/

#define SRC_CMD           ((ULONG)0x40)

#define SSRC              (SRC_CMD + 0)       /* Setup SRC                           */
#define SLST              (SRC_CMD + 1)       /* Select pre-canned list of SRC config*/
#define RSRC              (SRC_CMD + 2)       /* Read given SRC setting              */ 
#define SSRB              (SRC_CMD + 3)       /* Set SRC Band Limiting Value         */

#define SEND_SSRC(GenCtrll,SRCID,D,M,N)	YESPARMAorB_NoRET(GenCtrll,(SSRC) | (B1(SRCID)) | (W23(D)),(W01(M)) | (W23(N))) 

#define SEND_SLST(GenCtrll,PROFILEID)	YESPARMAorB_NoRET(GenCtrll,SLST,B3(PROFILEID))
  
#define SEND_RSRC(GenCtrll,SRCID,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,RSRC|B1(SRCID),0)

#define SEND_SSRB(GenCtrll,SRCID,BVAL)	YESPARMAorB_NoRET(GenCtrll,SSRB|B1(SRCID),W23(BVAL)) 

/*
  Digital Mixer Setup Command Definitions
*/

#define DMIXER_CMD        ((ULONG)0x50)

#define SDGV              (DMIXER_CMD + 0)     /* Set digital volume or gain         */
#define RDGV              (DMIXER_CMD + 1)     /* Read digital volume or gain        */
#define DLST              (DMIXER_CMD + 2)     /* Select pre-canned list of digitalCF*/   

#define SEND_SDGV(GenCtrll,ID1,ID2,VAL1,VAL2)	YESPARMAorB_NoRET(GenCtrll,(SDGV)|(B2(ID1))|(B3(ID2)),(W01(VAL1)|W23(VAL2)))  

#define SEND_RDGV(GenCtrll,ID1,ID2,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,RDGV|B2(ID1)|B3(ID2), 0)

#define SEND_DLST(GenCtrll,PROFILEID)	YESPARMAorB_NoRET(GenCtrll,DLST,B3(PROFILEID))

/*
  AC98-LINK Command Definitions
*/

#define AC98_CMD          ((ULONG) 0x60)

#define SACR              (AC98_CMD + 0)       /* Set AC98 register value            */
#define RACR              (AC98_CMD + 1)       /* Read AC98 regsiter value           */   
#define ALST              (AC98_CMD + 2)
#define TXAC              (AC98_CMD + 3)       /* Send sample through AC98-link      */
#define RXAC              (AC98_CMD + 4)       /* Read sample through AC98-link      */

#define SEND_SACR(GenCtrll,VAL, ADR)	YESPARMAorB_NoRET(GenCtrll,SACR,W01(VAL)|W23(ADR))

#define SEND_RACR(GenCtrll,ADR,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,RACR,W23(ADR))

#define SEND_ALST(GenCtrll,PROFILEID)	YESPARMAorB_NoRET(GenCtrll,ALST,B3(PROFILEID))

#define SEND_TXAC(GenCtrll,UPPERS,DAT2,DAT1,SLOT2,SLOT1)	YESPARMAorB_NoRET(GenCtrll,TXAC|B1(UPPERS)|W23(DAT2),W01(DAT1)|B2(SLOT2)|B3(SLOT1))

#define SEND_RXAC(GenCtrll,SLOT2,SLOT1,rptr)	YESPARMAorB_YESRET(GenCtrll,rptr,RXAC,B2(SLOT2)|B3(SLOT1))

#define SI2S              ((ULONG) 0x70)         
         
#define SEND_SI2S(GenCtrll,IS2VAL)	YESPARMAorB_NoRET(GenCtrll,SI2S,W23(IS2VAL))

/*
  Audio control register bit definitions.
*/
 
#define GRESET            0x0001              /*  Global software reset   */
#define XMRESET           0x0002        
#define AIE               0x0004              /*  Global audio interrupt enable    */
#define AIACK             0x0008              /*  Global audio interrupt ACK       */
#define ECMDAE            0x0010              /*  Enable cmd A empty interrupt     */
#define ECMDBE            0x0020              /*  Enable cmd B empty interrupt     */
#define EDATF             0x0040              /*  Enable data A full interrupt     */
#define EDATBF            0x0080              /*  Enable data B full interrupt     */
#define ESBIRQON          0x0100              /*  Enable Sound Blaster EMU IRQ NOTI*/ 
#define EMPUIRQ           0x0200              /*  Enable MPU401 IRQ                */


#define SET_GRESET        UMASK_AUDIO_CONTROL( GRESET )
#define UNSET_GRESET      MASK_AUDIO_CONTROL( ~GRESET ) 

#define SET_AIE           UMASK_AUDIO_CONTROL( AIE )
#define UNSET_AIE         MASK_AUDIO_CONTROL( ~AIE ) 



#define SET_AIACK         UMASK_AUDIO_CONTROL( AIACK )
#define UNSET_AIACKT      MASKAUDIO_CONTROL( ~AICK  ) 



#define INTERRUPT_ACK     SET_AIACK

 
#define SET_ECMDAE        UMASK_AUDIO_CONTROL( ECMDAE )
#define UNSET_ECMDAE      MASK_AUDIO_CONTROL( ~ECMDAE )

#define SET_ECMDBE        UMASK_AUDIO_CONTROL( ECMDBE )
#define UNSET_ECMDBE      MASK_AUDIO_CONTROL( ~ECMDBE )

#define SET_EDATAF        UMASK_AUDIO_CONTROL( EDATAF )
#define UNSET_EDATAF      MASK_AUDIO_CONTROL( ~EDATAF )

#define SET_EDATBF        UMASK_AUDIO_CONTROL( EDATBF )
#define UNSET_EDATBF      MASK_AUDIO_CONTROL( ~EDATBF )
 
#define SET_ESBIRQON      UMASK_AUDIO_CONTROL( ESBIRQON )
#define UNSET_ESBIRQON    MASK_AUDIO_CONTROL( ~ESBIRQON ) 

#define SET_EMPUIRQ       UMASK_AUDIO_CONTROL( EMPUIRQ )
#define UNSET_EMPUIRQ     MASK_AUDIO_CONTROL( ~EMPUIRQ ) 

             
/*                                                            
 Audio status register bit definitions.
*/

#define MSK_READY             0x0001              /* Audio device ready, ARM is active */
#define MSK_DLREADY           0x0002              /* ARM download mode                 */
#define MSK_DLERR             0x0004              /* ARM download error                */  
#define MSK_GERR              0x0008              /* General error                     */
#define MSK_CMDAE             0x0010              /* CMD A empty                       */
#define MSK_CMDBE             0x0020              /* CMD B empty                       */
#define MSK_DATAF             0x0040              /* DATA A full                       */
#define MSK_DATBF             0x0080              /* DATA B full                       */
#define MSK_EOBIRQ            0x0100              /* End of block interrupt pending    */
#define MSK_EOSIRQ            0x0200              /* End of stream interrupt pending   */
#define MSK_EOCIRQ            0x0400              /* End of count interrupt pending    */
#define MSK_UNSLIRQ           0x0800              /* Unsolicited interrupt pending     */
#define MSK_SBIRQ             0x1000              /* SB interrupt pending              */
#define MSK_MPUIRQ            0x2000              /* MPU401 interrupt pending          */  
  
#define IS_READY          ( READ_AUDIO_STATUS & MSK_READY   )
#define IS_DLREADY        ( READ_AUDIO_STATUS & MSK_DLREADY )                                                                                                                                                                                        
#define IS_DLERR          ( READ_AUDIO_STATUS & MSK_DLER    )   
#define IS_GERR           ( READ_AUDIO_STATUS & MSK_GERR    )                     
#define IS_CMDAE          ( READ_AUDIO_STATUS & MSK_CMDAE   )        
#define IS_CMDBE          ( READ_AUDIO_STATUS & MSK_CMDBE   )                
#define IS_DATAF          ( READ_AUDIO_STATUS & MSK_DATAF   )                        
#define IS_DATBF          ( READ_AUDIO_STATUS & MSK_DATBF   )                       
#define IS_EOBIRQ         ( READ_AUDIO_STATUS & MSK_EOBIRQ  )   
#define IS_EOSIRQ         ( READ_AUDIO_STATUS & MSK_EOSIRQ  ) 
#define IS_EOCIRQ         ( READ_AUDIO_STATUS & MSK_EOCIRQ  )   
#define IS_UNSLIRQ        ( READ_AUDIO_STATUS & MSK_UNSLIRQ )       
#define IS_SBIRQ          ( READ_AUDIO_STATUS & MSK_SBIRQ   )         
#define IS_MPUIRQ         ( READ_AUDIO_STATUS & MSK_MPUIRQ  )             

/*
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////// P C I   C O N F I G ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
*/

#define LEGACY_ENABLE_ALL      0x8000
#define LEGACY_ENABLE_SB       0x4000
#define LEGACY_ENABLE_FM       0x2000
#define LEGACY_ENABLE_MPU_INT  0x1000
#define LEGACY_ENABLE_MPU      0x0800
#define LEGACY_ENABLE_GAMEPORT 0x0400

#define LEGACY_ALL	(LEGACY_ENABLE_ALL | LEGACY_ENABLE_SB | LEGACY_ENABLE_FM | LEGACY_ENABLE_MPU_INT | LEGACY_ENABLE_MPU | LEGACY_ENABLE_GAMEPORT)

typedef struct {
 UCHAR  CapId;       /* 0x40*/
 UCHAR  NextCapPrt;  /* 0x41 next capabilitiy*/
 USHORT PWMC;        /* 0x42 power manager capabilities*/
 USHORT PWSCR;       /* 0x44*/
 UCHAR  Data00;      /* 0x46*/
 UCHAR  PMSCR_BSE;   /* 0x47*/
 USHORT SB_Base;     /* 0x48 Sound Blaster I/O base*/
 USHORT FM_Base;     /* 0x4a FM/OPL3 I/O base*/
 USHORT MPU_Base;    /* 0x4C MPU I/O base*/
 USHORT Game_Base;   /* 0x4E Joystick I/O base*/
 USHORT Legacy_Mask; /* 0x50 Enable/Disable mask, IRQs and SoundBlaster DMA*/
 UCHAR  AsicRev;     /* 0x52*/
 UCHAR  Reserved3;   /* 0x53 Snoop circuit and other stuff*/
} RipTidePciExtension;


#define D0_POWER_STATE   0x00    /* fully on*/
#define D1_POWER_STATE   0x01    /* stop audio-related clocks*/
#define D2_POWER_STATE   0x02    /* stop audio-related clocks*/
#define D3_POWER_STATE   0x03    /* stop audio-related clocks and slowdown ARM cl?*/
#define POWER_STATE_MASK 0x03
   
/* 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////// E X T E R N A L S //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//External defintion for main routine that sends a RipTide Audio Device Command
*/

typedef BOOLEAN
(* fsend_command)(
  pCMDIF_AB    pCMDIF,
  ULONG        dwFLAGS,
  ULONG        dwCMDAorB,
  ULONG        dwPARMAorB,
  PCMDRET_TYPE RetAddr
);

typedef void (*voidfunc)(void);
