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
  MODULE NAME:        Rp_Cmdif.cpp                       

  AUTHOR:             O. Druzhinin  (Transcribed by OLD)
                      K. Lazarev    (Transcribed by KNL)

  HISTORY:         Major Revision               Date        By
            -----------------------------     --------     -----
            Adopted from NT4 driver            6/22/99      OLD
            Ported to Linux                    9/01/99      KNL

  DESCRIPTION:

    This module contains code for communicating with the hardware
    on the RipTide card.

  NOTES:    None.
*/

#include "riptide.h"

#define DATA_REC         0
#define EXT_SEG_ADDR_REC 2
#define EXT_LIN_ADDR_REC 4
#define EXT_END_OF_FILE  1
#define EXT_GOTO_CMD_REC 3

//tCMDIF_AB CMDIF;
static
USHORT uErrorCount;

#define CMDIF_TO (10000L)
#define MAX_ERROR_COUNT 60

//
// Host Interface Port (HIP) messaging support
//
//KSPIN_LOCK  hipLock;

#if DBG
BOOLEAN     hipLockHeld   = FALSE;
ULONG       dwCmdNumber   = 0;
ULONG       dwCmdFail     = 0;
ULONG       dwCmdTimeAll  = 0;
ULONG       dwCmdTimeMax  = 0;
ULONG       dwCmdTimeMin  = CMDIF_TO * 10;
ULONG       dwCmdConflict = 0;
#endif


C_DEFINITION
BOOLEAN
RiptideSendCmd(
  pCMDIF_AB    pCMDIF,
  ULONG        dwFLAGS,
  ULONG        dwCMDAorB,
  ULONG        dwPARMAorB,
  PCMDRET_TYPE RetAddr
);

static
C_DEFINITION
BOOLEAN
RiptideWriteArm(
  pCMDIF_AB    pCMDIF,
  ULONG Address,
  ULONG Data,
  ULONG Mask
);
/*************************************************************************
 *
 *  Name:           unsigned long
 *                    atoh(
 *                      char* cInStr,
 *                      UINT  uNumChars
 *                    );
 *
 *  Parameters:     char *cInStr   - pointer to string
 *                  UINT uNumChars - number of characters in the string
 *
 *  Returns:        Value.
 *
 *  Description:
 *    Converts ASCII hex to unsigned long.
 *
 *************************************************************************/

static
C_DEFINITION
ULONG
atoh(
  PUCHAR cInStr,
  ULONG  uNumChars
)
{
  ULONG uSum = 0;
  ULONG uMult = 1;
  
  while(uNumChars) {

    //dprintf(("<RP_CMDIF> Char = %c\n",cInStr[uNumChars-1]));

    if((cInStr[uNumChars-1]>='0')&&(cInStr[uNumChars-1]<='9')) {
      uSum += uMult*(cInStr[uNumChars-1]-'0');
    }
    else
      if((cInStr[uNumChars-1]>='A')&&(cInStr[uNumChars-1]<='F')) {
        uSum += uMult*(cInStr[uNumChars-1]-('A'-10));
      }
      else
        if((cInStr[uNumChars-1]>='a')&&(cInStr[uNumChars-1]<='f'))
        {
          uSum+=uMult*(cInStr[uNumChars-1]-('a'-10));
        }
    
    //dprintf(("<RP_CMDIF> uSum=%08X\n",uSum));

    uMult *= 16;
    --uNumChars;
  }

  //dprintf(("<RP_CMDIF> atoh=%08X\n",uSum));
  
  return uSum;
}


/*************************************************************************
 *
 *  Name:           ULONG
 *                    ParseData(
 *                      char* cInStr
 *                    );
 *
 *  Parameters:     char *cInStr = pointer to string
 *
 *  Returns:        Value.
 *
 *  Description:
 *    Converts ASCII hex to a DWORD of data.
 *
 *************************************************************************/

static
C_DEFINITION
ULONG
ParseData(
  PUCHAR cInStr
)
{
  ULONG        uCharNum;
  ULONG        uSum     = 0;
  static ULONG uShift[] = { 4, 0, 12, 8, 20, 16, 28, 24 };

  for( uCharNum = 0; uCharNum < 8; ++uCharNum ) {

    if((cInStr[uCharNum]>='0')&&(cInStr[uCharNum]<='9')) {
      uSum += (ULONG)(cInStr[uCharNum]-'0')<<uShift[uCharNum];
    }
    else
      if((cInStr[uCharNum]>='A')&&(cInStr[uCharNum]<='F')) {
        uSum += (ULONG)(cInStr[uCharNum]-('A'-10))<<uShift[uCharNum];
      }
      else
        if((cInStr[uCharNum]>='a')&&(cInStr[uCharNum]<='f')) {
          uSum += (ULONG)(cInStr[uCharNum]-('a'-10)) << uShift[uCharNum];
        }
  }

  //dprintf(("<RP_CMDIF> DATA %08X\n",uSum));
  
  return uSum;
}


/*************************************************************************
 *
 *  Name:           BOOLEAN
 *                    RiptideProcessDataRec(
 *                      char* cInRec,
 *                      ULONG uExtAddr
 *                    );
 *
 *  Parameters:     cInRec   - pointer to first byte to be processed
 *                  uExtAddr - address data should be loaded to
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Load data into hardware.
 *
 *************************************************************************/

static
C_DEFINITION
BOOLEAN
RiptideProcessDataRec(
  tCMDIF_AB *pCMDIF,
  PUCHAR cInRec,
  ULONG  uExtAddr
)
{
  ULONG  uDataAddr;  /* Data address                        */
  ULONG  uData;      /* Program data to download            */
  ULONG  uByteCnt;   /* Number of data bytes in this record */
  PUCHAR pInPtr;     /* Points into the input data record   */
  
  /* Parse the record byte count */
  uByteCnt = (ULONG) atoh(&cInRec[1],2);
  
  /* Calculate the data load address */
  uDataAddr = (uExtAddr+atoh(&cInRec[3],4));
  //dprintf(("<RP_CMDIF> Address = %04X, uExtAddr = %04X\n",uDataAddr,uExtAddr));
  
  /* Tell the RipTide where to put the coming data */
  
  if(SEND_SMEM(CMDA, 0, uDataAddr) == FALSE) {
    dprintf(("<RP_CMDIF> Can't set address: %lx\n", uDataAddr));
    return FALSE;
  }
    
  /* Point to the start of the data in the input record */
  pInPtr = cInRec + 9;
    
  dprintf2(("RLOAD: "));
  while( uByteCnt )
  {
    /* Read a ULONG of data from the record */
    uData = ParseData( pInPtr );
      
    /* Write the data to the RipTide */
#ifdef VERIFY_LOAD
    do
    {
#endif
      
      dprintf2(("%2X%2X%2X%2X", uData&0xFF, (uData&0xFF00)>>8, (uData&0xFF0000)>>16, (uData&0xFF000000)>>24));
      if(SEND_WMEM(CMDA,2,uData) == FALSE) {
        dprintf(("<RP_CMDIF> Can't write command: %lx\n", uData));
        return FALSE;
      }

#ifdef VERIFY_LOAD
      {
        CMDRET_TYPE rptr = {(ULONG)0,(ULONG)0};
            
        if(SEND_RMEM(CMDA, 2, uDataAddr, &rptr) == FALSE) {
          return FALSE;
        }
        bVerify = (rptr.dRetDwords[0] == uData);
      }
    } while(!bVerify);
#endif

    /* Adjust the byte count, input pointer, and data address */
    uByteCnt -= 4;
    pInPtr   += 8;
  }
  dprintf2(("\n"));
    
  return TRUE;
}


/*************************************************************************
 *
 *  Name:           BOOLEAN
 *                    RiptideLoadImage(
 *                      PUCHAR puFileImage,
 *                      ULONG  ulImageSize
 *                    );
 *
 *  Parameters:     puFileImage - pointer to firmware image
 *                  ulImageSize - size of image
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Load image of firmware into hardware
 *
 *************************************************************************/

static
C_DEFINITION
BOOLEAN
RiptideLoadImage(
  tCMDIF_AB *pCMDIF,
  PUCHAR puFileImage,
  ULONG  ulImageSize
)
{
  UCHAR   *cInRec;
  ULONG   uLinAddr;
  ULONG   uSegAddr;
  ULONG   uRecType;
  BOOLEAN ret = TRUE;
  
  dprintf(("<RP_CMDIF> Start RiptideLoadImage\n"));
  
  uLinAddr = uSegAddr = 0;
  while((ulImageSize > 0) && ret) {

    cInRec = puFileImage;
    
    /* Make sure it's a colon record */
    if(cInRec[0]==':') {

      /* Parse the record type */
      uRecType = (ULONG) atoh(&cInRec[7],2);
      
      switch( uRecType ) {
      
        /* Process the data record */
        case DATA_REC:
//          dprintf(("<RP_CMDIF> DATA_REC\n"));
          ret &= RiptideProcessDataRec(pCMDIF, cInRec, uLinAddr + uSegAddr );
          break;
        
        /* Parse the extended segment address */
        case EXT_SEG_ADDR_REC:
          uSegAddr = atoh(&cInRec[9],4)<<4;
//          dprintf(("<RP_CMDIF> EXT_SEG_ADDR_REC %04X\n",uSegAddr));
          break;
        
        /* Parse the extended linear address */
        case EXT_LIN_ADDR_REC:
          uLinAddr = atoh(&cInRec[9],4)<<16;
//          dprintf(("<RP_CMDIF> EXT_LIN_ADDR_REC %04X\n",uLinAddr));
          break;
        /* Parse the GOTO command and its address */
        case EXT_GOTO_CMD_REC:
        {
          ULONG dwIPValue;

          dwIPValue = atoh(&cInRec[9],8);
          dprintf2(("<RP_CMDIF> EXT_GOTO_CMD_REC %04X\n",dwIPValue));
          if(SEND_GOTO( CMDA, dwIPValue) == FALSE) {
              dprintf(("<RP_CMDIF> Can't go to start of firmware\n",dwIPValue));
              ret = FALSE;
          }
          break;
        }
        /* The last record read must contain a special char */
        case EXT_END_OF_FILE:
          dprintf2(("<RP_CMDIF> EXT_END_OF_FILE\n"));
          ulImageSize = 0;
          break;
        
        default:
          dprintf(("<RP_CMDIF> EXT_UNKNOWN\n"));
          break;
      }
      
      while(ulImageSize > 0)
      {
        ulImageSize--;
        if(*puFileImage++ == '\n')
          break;
      }
    }
  }
  
  return ret;
}


/*************************************************************************
 *
 *  Name:           RiptideLoadImage(PUCHAR puFileImage, ULONG ulImageSize)
 *
 *  Parameters:     puFileImage - pointer to firmware image
 *                  ulImageSize - size of image
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *     Load image of firmware into hardware
 *************************************************************************/

static
C_DEFINITION
BOOLEAN
RiptideLoadFirmware(
  tCMDIF_AB *pCMDIF,
  PUCHAR pFileName
)
{
  unsigned char *FileImage;
  unsigned      LengthOfFile;
  BOOLEAN       Status = TRUE;

  /* Read image to RipTide */

  LengthOfFile = load_file(RIPFIRMWR_FILE, (void *) &FileImage);

  /* Load image to RipTide */
  
  dprintf(("<RP_CMDIF> Firmware size: %d\n", LengthOfFile));
  if(RiptideLoadImage(pCMDIF, FileImage, LengthOfFile) == FALSE)
  {
    dprintf(("<RP_CMDIF> ERROR!!! Can't load firmware\n"));
    Status = FALSE;
  }
  else
  {
    CMDRET_TYPE rptr = {{(ULONG)0,(ULONG)0}};
    int i, j;

    dprintf(("<RP_CMDIF> Firmware is loaded.\n"));
    for(i=0; i<2; i++)
    {
      for(j = 0; j < 10000; j++)
      {
        if(SEND_RACR(CMDA, 2, &rptr) != FALSE)
        {
          Status = TRUE;
          break;
        }
        wait_for_timeout(1000L);    // wait 1000 microseconds
        dprintf(("<RP_CMDIF> ERROR!!! Firmware init is failed\n"));
        Status = FALSE;
      }
    }
  }
  return Status;
}

/*************************************************************************
 *
 *  Name:           ULONG
 *                    RiptideInitialize(
 *                      USHORT  device_id,
 *                      HW_PORT base_io
 *                    );
 *
 *  Parameters:     base_io - Riptide I/O base
 *                  
 *  Returns:        TRUE  - success
 *                  FALSE - fail
 *                  
 *  Description:
 *     Initialize Riptide hardware.
 *
 *************************************************************************/

C_DEFINITION
voidfunc
RiptideInitialize(
  void **ppCMDIF,
  USHORT    device_id,
  HW_PORT   base_io
)
{
  tCMDIF_AB *pCMDIF;
  /* CMDRET_TYPE rptr = {{(ULONG)0, (ULONG) 0}}; */
  ULONG       timeout;
  
  dprintf2(("<RP_CMDIF> Started CmdIf_Initialize\n"));
  uErrorCount = MAX_ERROR_COUNT;
  
  if(*ppCMDIF == NULL)
  {
  if((*ppCMDIF = allocate_mem(sizeof(tCMDIF_AB))) == NULL)
  {
    dprintf(("<RP_CMDIF> Can't allocate interface\n"));
    return NULL;
  }

  pCMDIF = *((tCMDIF_AB **)ppCMDIF);
  pCMDIF->audio_control = base_io + AUDIO_CONTROL;
  pCMDIF->audio_status  = base_io + AUDIO_STATUS;
  pCMDIF->cmda          = base_io + CMDA_OFFSET; 
  pCMDIF->data1a        = base_io + DATA1A_OFFSET;   
  pCMDIF->parma         = base_io + PARMA_OFFSET;     
  pCMDIF->data2a        = base_io + DATA2A_OFFSET;     
  pCMDIF->stata         = base_io + STATA_OFFSET;             
  pCMDIF->cmdb          = base_io + CMDB_OFFSET;           
  pCMDIF->data1b        = base_io + DATA1B_OFFSET;            
  pCMDIF->parmb         = base_io + PARAMB_OFFSET;           
  pCMDIF->data2b        = base_io + DATA2B_OFFSET;           
  pCMDIF->statb         = base_io + STATB_OFFSET;          
  
  dprintf2(("<RP_CMDIF> CmdIf:\n"
    "audio_control: %x\n"
    "audio_status:  %x\n"
    "cmda:          %x\n"
    "data1a:        %x\n"
    "parma:         %x\n"
    "data2a:        %x\n"
    "stata:         %x\n"
    "cmdb:          %x\n"
    "data1b:        %x\n"
    "parmb:         %x\n"
    "data2b:        %x\n"
    "statb:         %x\n",
    pCMDIF->audio_control,
    pCMDIF->audio_status,
    pCMDIF->cmda,
    pCMDIF->data1a,
    pCMDIF->parma,
    pCMDIF->data2a,
    pCMDIF->stata, 
    pCMDIF->cmdb,
    pCMDIF->data1b,
    pCMDIF->parmb,
    pCMDIF->data2b,
    pCMDIF->statb)
  );
  }
  pCMDIF = *((tCMDIF_AB **)ppCMDIF);

  WRITE_DATA1B( 0 );
  WRITE_DATA2B( 0 );
/*    
  SEND_GETV( CMDA, &rptr);  
  
  dprintf(("<RP_CMDIF> RipTide version:\nASIC  : %d\nCODEC : %d\nAUXDSP: %d\nPROG  : %d\n",
    rptr.wRetWords[0],
    rptr.wRetWords[1],
    rptr.wRetWords[2],
    rptr.wRetWords[3]));
*/
//  timeout = 0x9000;
//  
//  do 
//  {
//    timeout--;
//    if ( (IS_READY) && (!IS_GERR) )
//      break;
//  } while( timeout );
  
//  if ( !timeout )
//  {
    SET_GRESET;

    wait_for_timeout(10);

    UNSET_GRESET;

    wait_for_timeout(10);
      
    timeout = 0x30000;
    do 
    {
      timeout--;
      if ( (IS_READY) && (!IS_GERR) )
        break;
      wait_for_timeout(1);
    } while(timeout);

    if ( !timeout )
    {
      dprintf(("<RP_CMDIF> Reset is failed (READY=%s ERROR=%s) !!!\n",
        (IS_READY) ? "ON" : "OFF",
        (IS_GERR)  ? "ON" : "OFF"
      ));
//      return FALSE;
    }
#if DBG
    else
    {
      dprintf(("<RP_CMDIF> Reset delay is %ld ms\n",(0x30000-timeout)/1000));
    }
#endif
//  }
  
  /* Get hardware version */

  timeout=500;
  while(--timeout)
  {
    CMDRET_TYPE rptr = {{(ULONG)0, (ULONG) 0}};
    
    uErrorCount = 0;
    if( SEND_GETV(CMDA, &rptr) )
    {
      break;
    }
  }
  
#if DBG
  if(timeout == 0)
    dprintf(("<RP_CMDIF> Wait failed\n"));
  else
    dprintf(("<RP_CMDIF> Wait %ld\n",500-timeout));
#endif
  
  SET_AIE;  
  
  uErrorCount = 0;

  /* Get hardware version */
  timeout=0;
  while(timeout<5000)
  {
    CMDRET_TYPE rptr = {{(ULONG)0, (ULONG) 0}};

    uErrorCount = 0;
    if( SEND_GETV(CMDA, &rptr) )
    {
      dprintf(("<RP_CMDIF> RipTide version before:\nASIC  : %d\nCODEC : %d\nAUXDSP: %d\nPROG  : %d\n",
        rptr.wRetWords[0],
        rptr.wRetWords[1],
        rptr.wRetWords[2],
        rptr.wRetWords[3]));
      break;
    }
    timeout++;
  }

  if(!RiptideLoadFirmware(pCMDIF, ""))
  {
    dprintf(("<RP_CMDIF> ERROR!!! Can't load firmware\n"));
    return NULL;//FALSE;
  }

  /* Get hardware version */
  timeout=0;
  while(timeout<5000)
  {
    CMDRET_TYPE rptr = {{(ULONG)0, (ULONG) 0}};

    uErrorCount = 0;
    if( SEND_GETV(CMDA, &rptr) )
    {
      dprintf(("<RP_CMDIF> RipTide version after:\nASIC  : %d\nCODEC : %d\nAUXDSP: %d\nPROG  : %d\n",
        rptr.wRetWords[0],
        rptr.wRetWords[1],
        rptr.wRetWords[2],
        rptr.wRetWords[3]));
      break;
    }
    timeout++;
  }

  /* Set default values */
  SEND_PLST(CMDA, 0);
  SEND_SLST(CMDA, 0);
  SEND_DLST(CMDA, 0);
  SEND_ALST(CMDA, 0);

  /* Start modem if needed */
#ifdef SUPPORT_MODEM
  switch(device_id)
  case 0x4320:
  case 0x4330: { SEND_SETDPLL(CMDA); }
#endif

  /* Get hardware version */
  timeout=0;
  while(timeout<5000)
  {
    CMDRET_TYPE rptr = {{(ULONG)0, (ULONG) 0}};

    uErrorCount = 0;
    if( SEND_GETV(CMDA, &rptr) )
    {
      dprintf2(("<RP_CMDIF> RipTide version before:\nASIC  : %d\nCODEC : %d\nAUXDSP: %d\nPROG  : %d\n",
        rptr.wRetWords[0],
        rptr.wRetWords[1],
        rptr.wRetWords[2],
        rptr.wRetWords[3]));
      break;
    }
    timeout++;
  }
  
  //!!!!!!!!!!!!
  //only for HP power Amp
  RiptideWriteArm(pCMDIF,0x301F8, 1, 1);
  RiptideWriteArm(pCMDIF,0x301F4, 1, 1);
  //!!!!!!!!!!!!

  SET_EMPUIRQ;

  INTERRUPT_ACK;

  return (voidfunc)(&RiptideSendCmd);//(TRUE);
}

void RiptideRelease
(void *pContext)
{
    if(pContext != NULL)
        free_mem(pContext, sizeof(tCMDIF_AB))    ;    
}


/*************************************************************************
 *
 *  Name:           BOOLEAN
 *                    RiptideWriteArm(
 *                      ULONG Address,
 *                      ULONG Data,
 *                      ULONG Mask
 *                    );
 *
 *  Parameters:     Address -
 *                  Data    -
 *                  Mask    -
 *
 *  Returns:        TRUE  - success
 *                  FALSE - fail
 *
 *  Description:
 *      This routine changes particular bits of an ARM register.
 *  e.g: RealWriteArm(0x30648,0x01,0x05) will change bit 0 
 *  and bit 2 of ARM register 0x30648 to 1 and 0,respectively,
 *  depending on what's in "Data".
 *************************************************************************/

static
C_DEFINITION
BOOLEAN
RiptideWriteArm(
  pCMDIF_AB    pCMDIF,
  ULONG Address,
  ULONG Data,
  ULONG Mask
)
{
   CMDRET_TYPE rptr = {{(ULONG)0, (ULONG) 0}};
   ULONG       TimeOut;
   BOOLEAN     Flag;

   Flag = FALSE;

   dprintf2(("<RP_CMDIF> Arm-RW Address %lx\n", Address));
   dprintf2(("<RP_CMDIF> Data           %lx\n", Data   ));

   TimeOut = 20;
   rptr.dRetDwords[0]=0x00000000;
   SEND_RMEM(CMDA, 0x02, Address, &rptr);
   rptr.dRetDwords[0]&=(~Mask);
  
   while(--TimeOut)
   {

     SEND_SMEM(CMDA, 0x0001, Address);
     SEND_WMEM(CMDA, 0x0002, (rptr.dRetDwords[0] | Data));  
     SEND_RMEM(CMDA, 0x0002, Address, &rptr);
     if((rptr.dRetDwords[0] & Data) == Data)
     {
       Flag=TRUE;
       break;
     }
     else
     {
       rptr.dRetDwords[0]&=(~Mask);
     }
   }

#if DBG
   if(Flag == FALSE)
     dprintf(("Send to ARM memory is failed\n"));
#endif

   return ( Flag );
}


/*****************************************************************************
 *
 *      NAME:           RiptideSendCmd
 * 
 *      USAGE:          Only legal way to issue a command to the
 *                      RipTide Audio Device's CMDA or CMDB I/F.
 *
 *
 *      DESCRIPTION:    This routine should be called only by
 *                      RASM to talk to the RipTide hardware.
 *                      beginning of every command line.  When
 *                      It sends a command and waits for a
 *                      response if the command has one.
 *
 *      PARAMETERS:     dwFlags  -  Has indicators (see includes) that
 *                                  instruct the routine whether to use CMDA
 *                                  or CMDB, whether to wait for a response,
 *                                  and whether it needs to issue the second
 *                                  PARMA or PARMB 32-bit parameter register.
 *
 *                      dwCMDA0rB - This is the first 32-bit register containing
 *                                  the command in byte 0 and possible parameters
 *                                  in the other 24-bits. Writing Interrupts ARM.
 *
 *                      dwPARMSorB -This is the 2nd DWORD or parameters if the 
 *                                  command has any (otherwise 0);
 *
 *                      RetAddr   - A pointer to an 8-byte structure of type 
 *                                  CMDRET_TYPE, which will house the returned
 *                                  values from the command if any (otherwise 0);
 *
 *      RETURNED VALUE: Routine returns TRUE if command issued successfully,
 *                      else it returns FALSE (Most likely a timeout).
 *
 *      NOTES:          The command macros defined in the CMD.H 
 *                      file should be used to generate calls to
 *                      RiptideSendCmd -- it should not be
 *                      normally called explicitly since the
 *                      macros pack the parameters according to
 *                      specifics of each command.
 * 
 ****************************************************************************/

C_DEFINITION
BOOLEAN
RiptideSendCmd(
  pCMDIF_AB    pCMDIF,
  ULONG        dwFLAGS,
  ULONG        dwCMDAorB,
  ULONG        dwPARMAorB,
  PCMDRET_TYPE RetAddr
)
{
  ULONG   ulCount;
  BOOLEAN bResult = FALSE;

#if DBG
  ULONG   dwCmdTime = 0;
#endif

//  dprintf(("<RP_CMDIF> CMDIF: %p\n", pCMDIF));
try_again:
  if(pCMDIF == NULL)
  {
    return  RiptideWriteArm(
                (pCMDIF_AB)RetAddr,
                dwFLAGS,
                dwCMDAorB,
                dwPARMAorB);
  }
  else
  if(uErrorCount < MAX_ERROR_COUNT)
  {

    if ( ( dwFLAGS & CMDA )== CMDA )        
    {
//      wait_for_timeout(10);
      for(ulCount=0; (ulCount<CMDIF_TO) && !CMDAE; ulCount++)
        wait_for_timeout(1);
//      dprintf(("<RP_CMDIF> %d\n", ulCount));
/*
      if (ulCount<CMDIF_TO)
      {
	      if( RetAddr )
	      {
	          if(DATAF)
	          {
	              for(ulCount=0; (ulCount<CMDIF_TO) && DATAF; ulCount++)
	              {
		          RetAddr->dRetDwords[0] = READ_DATA1A;
		          RetAddr->dRetDwords[1] = READ_DATA2A;
	                  wait_for_timeout(1000);
	              }
	              if (ulCount>=CMDIF_TO)
	              {
	                dprintf(("<RP_CMDIF> ERROR!!! Can't get data %d\n", ulCount));
	                return bResult;
	              }
              }
	      }
	  }
*/
#if DBG
      dwCmdTime += ulCount;
#endif
      
      if(ulCount<CMDIF_TO)
      {
        
        if( dwFLAGS & USES_PARMREG )
        {
//          dprintf(("<RP_CMDIF> Param: %x\n", dwPARMAorB));
          WRITE_PARMA( dwPARMAorB );
  //        wait_for_timeout(10); /* Give some time to respond */
        }
        
        /* Then put out CMDA 32-bit value with CMD Id and           */
        /* possible 24-bits of parameters.  This DOES interrupt ARM */

//        dprintf(("<RP_CMDIF> CMD: %x\n", dwCMDAorB));
        WRITE_CMDA( dwCMDAorB );
        
        /* If no returned result address is supplied, we're done    */
        if ( RetAddr )
        {
          /* Wait for DATAF bit */

          wait_for_timeout(1); /* Give some time to respond */
          for(ulCount=0; (ulCount<CMDIF_TO) && !DATAF; ulCount++)
            wait_for_timeout(1);
//          wait_for_timeout(10); /* Give some time to respond */

#if DBG
          dwCmdTime += ulCount+1;
#endif

          if(ulCount<CMDIF_TO)
            bResult=TRUE;
          
          RetAddr->dRetDwords[0] = READ_DATA1A;
          RetAddr->dRetDwords[1] = READ_DATA2A;
//          dprintf(("<RP_CMDIF> Result: %x %x %s %d\n", RetAddr->dRetDwords[0], RetAddr->dRetDwords[1], (bResult)?"TRUE":"FALSE", ulCount));
        }
        else
          bResult = TRUE;
      }
      
    }
    else
    {  
      
      for(ulCount=0; (ulCount<CMDIF_TO) && !CMDBE; ulCount++)
        wait_for_timeout(1);
      
#if DBG
      dwCmdTime += ulCount;
#endif
      
      if(ulCount<CMDIF_TO)
      {

        if( dwFLAGS & USES_PARMREG )
        {
          WRITE_PARMB( dwPARMAorB );
        }
        
        /* Then put out CMDB 32-bit value with CMD Id and           */
        /* possible 24-bits of parameters.  This DOES interrupt ARM */

        WRITE_CMDB( dwCMDAorB );
        
        /* If no returned result address is supplied, we're done    */
        if ( RetAddr )
        {
          /* Wait for DATAF bit */

          wait_for_timeout(10); /* Give some time to respond */
          for(ulCount=0; (ulCount<CMDIF_TO) && !DATBF; ulCount++)
            wait_for_timeout(1);

#if DBG
          dwCmdTime += ulCount+1;
#endif

          if(ulCount<CMDIF_TO)
            bResult=TRUE;
          
          RetAddr->dRetDwords[0] = READ_DATA1B;
          RetAddr->dRetDwords[1] = READ_DATA2B;
          
        }
        else
          bResult = TRUE;

      }
    }
  
#if DBG

    /* Update statistics, but skip command wich takes longer than 60 */

    if(dwCmdTime<600)
    {
      if(bResult)
      {
        dwCmdNumber++;
        dwCmdTimeAll += dwCmdTime;
        if (dwCmdTime>dwCmdTimeMax)
          dwCmdTimeMax = dwCmdTime;
        if (dwCmdTime<dwCmdTimeMin)
          dwCmdTimeMin = dwCmdTime;
      }
      else
      {
        dwCmdFail++;
      }
    }
#endif

  }
  else
  {
    voidfunc pf;
        pf = 
		RiptideInitialize(
		  (void **)(&pCMDIF),
		  0,
		  0
		);
    if(pf != NULL)
    {
        uErrorCount=0;    
        goto try_again;
    }
  }
  

#if DBG
  
  /* Output statistics on every 100 commands */

  if(dwCMDAorB == KSTR)
  {
    dprintf(("<RP_CMDIF> ------ COMMAND INTERFACE ------\n"));
    dprintf(("<RP_CMDIF> Commands sent:     %ld\n",dwCmdNumber+dwCmdFail));
    dprintf(("<RP_CMDIF> Commands failed:   %ld\n",dwCmdFail));
    dprintf(("<RP_CMDIF> Commands conflict: %ld\n",dwCmdConflict));
    dprintf(("<RP_CMDIF> Max time:          %ld\n",dwCmdTimeMax));
    dprintf(("<RP_CMDIF> Min time:          %ld\n",dwCmdTimeMin));
    if(dwCmdNumber!=0)
      dprintf(("<RP_CMDIF> Average time:      %ld.%02d\n",
        dwCmdTimeAll/dwCmdNumber,
        (int)(dwCmdTimeAll%dwCmdNumber*100/dwCmdNumber)));
    dprintf(("<RP_CMDIF> -------------------------------\n"));
  }

  if(bResult && (dwCmdTime > 600))
  {
    dprintf(("<RP_CMDIF> \n\n!!!!!!!!!!!!!!!!!!!!!!! Timing problem (%ld) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n",dwCmdTime));
  }
#endif

  uErrorCount = (bResult)?0:uErrorCount+1;
  
#if DBG
  ASSERT( hipLockHeld == TRUE);   
  hipLockHeld = FALSE;            
#endif

  return bResult;
}

#undef CMDIF
