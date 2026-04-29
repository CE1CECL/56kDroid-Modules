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
  MODULE NAME:        rt_hw.c                       

  AUTHOR:             O. Druzhinin  (Transcribed by OLD)
                      C. Lazarev    (Transcribed by CNL)

  HISTORY:         Major Revision               Date        By
            -----------------------------     --------     -----
            Created                           11/18/97      OLD
            Hardware functions for RipTide    11/24/97      CNL
            (ES1) are coded
            Hardware functions for RipTide    12/24/97      CNL
            (A0) are coded
            Hardware functions for RipTide    03/20/98      CNL
            (A1) are coded
            Boot loader is included           05/07/98      CNL

            Redesigned for WDM                07/27/98      CNL
            Redesigned for Linux              09/01/99      CNL

  DESCRIPTION:

    This module contains code for communicating with the hardware
    on the RipTide card.

  NOTES:    None.
*/

#include "riptide.h"

#define GET_POSITION

extern tCMDIF_AB CmdIf;  

#define LBUS_DYNAMIC

#define LBUS_SINK_OUT  0
#define LBUS_SINK_TYPE 1
#define LBUS_EMPTY     0xFF

#define ASIC_74XX   0x3
//
//  Table of correspondence between LBUS sinks and sources:
//  index is LBUS sink number
//  Format:
//  {LBUS source, LBUS sink type}
//
static
const UCHAR LbusIn2Out[E2SINK_MAX+1][2] = {
  {0xFF,               0xFF},  //  LBUS2ARM_FIFO0,
  {0xFF,               0xFE},  //  LBUS2ARM_FIFO1,
  {0xFF,               0xFF},  //  LBUS2ARM_FIFO2,
  {0xFF,               0xFE},  //  LBUS2ARM_FIFO3, 
  {0xFF,               0xFF},  //  LBUS2ARM_FIFO4,
  {0xFF,               0xFE},  //  LBUS2ARM_FIFO5,
  {0xFF,               0xFF},  //  LBUS2ARM_FIFO6,
  {0xFF,               0xFE},  //  LBUS2ARM_FIFO7,
  
  {INTER0_OUT,         LS_SRC_INTERPOLATOR}, //  INTER0_IN,
  {INTER1_OUT,         LS_SRC_INTERPOLATOR}, //  INTER1_IN,
  {INTER2_OUT,         LS_SRC_INTERPOLATOR}, //  INTER2_IN,
  {INTER3_OUT,         LS_SRC_INTERPOLATOR}, //  INTER3_IN,
  
#ifdef ASIC_A0
  {DECIM0_OUT,         LS_SRC_DECIMATOR}, //  DECIM0_IN,
  {DECIM1_OUT,         LS_SRC_DECIMATOR}, //  DECIM1_IN,
  
  {SR3_4_OUT,          0xFF}, //  SR3_4_IN,
  
  {0xFF,               0xFE}, //  PDAC2ACLNK,
  
  {0xFF,               0xFF}, //  MODEM0TX2ACLNK,
  {0xFF,               0xFE}, //  MODEM1TX2ACLNK, 
  
  {0xFF,               0xFF}, //  HNDSPK2ACLNK,
  
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN0,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN1, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN2,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN3, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN4,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN5, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN6,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN7, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN8,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN9, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN10,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN11, 
  
  {GAINFUNC0_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC0_IN,
  {GAINFUNC1_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC1_IN,
  {GAINFUNC2_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC2_IN,
  {GAINFUNC3_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC3_IN,
  {GAINFUNC4_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC4_IN,
                
  {SOFTMODEMTX,        0xFF}, //  SOFTMODEMRX,

#elif defined(ASIC_A1) // ASIC_A0

  {INTER4_OUT ,        LS_SRC_INTERPOLATOR}, //  INTER4_IN,
                       
  {INTERM0_OUT,        LS_SRC_INTERPOLATORM}, //  INTERM0_IN,
  {INTERM1_OUT,        LS_SRC_INTERPOLATORM}, //  INTERM1_IN,
  {INTERM2_OUT,        LS_SRC_INTERPOLATORM}, //  INTERM2_IN,
  {INTERM3_OUT,        LS_SRC_INTERPOLATORM}, //  INTERM3_IN,
  {INTERM4_OUT,        LS_SRC_INTERPOLATORM}, //  INTERM4_IN,
  {INTERM5_OUT,        LS_SRC_INTERPOLATORM}, //  INTERM5_IN,
  {INTERM6_OUT,        LS_SRC_INTERPOLATORM}, //  INTERM6_IN,
                       
  {DECIMM0_OUT,        0xFF},    //  DECIMM0_IN,
  {DECIMM1_OUT,        0xFF},    //  DECIMM1_IN,
  {DECIMM2_OUT,        0xFF},    //  DECIMM2_IN,
  {DECIMM3_OUT,        LS_SRC_DECIMATORM},    //  DECIMM3_IN,
                       
  {DECIM0_OUT,         LS_SRC_DECIMATOR},     //  DECIM0_IN,
                       
  {SR3_4_OUT,          0xFF}, //  SR3_4_IN,
  
  {0xFF,               0xFE}, //  PDAC2ACLNK,
  
  {0xFF,               0xFF}, //  MODEM0TX2ACLNK,
  {0xFF,               0xFE}, //  MODEM1TX2ACLNK, 
  
  {0xFF,               0xFF}, //  HNDSPK2ACLNK,

  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN0,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN1, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN2,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN3, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN4,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN5, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN6,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN7, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN8,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN9, 
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN10,
  {DIGITAL_MIXER_OUT0, LS_MIXER_IN}, //  DIGITAL_MIXER_IN11, 
  
  {GAINFUNC0_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC0_IN,
  {GAINFUNC1_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC1_IN,
  {GAINFUNC2_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC2_IN,
  {GAINFUNC3_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC3_IN,
  {GAINFUNC4_OUT,      LS_MIXER_GAIN_FUNCTION}, //  GAINFUNC4_IN,
  
  {SOFTMODEMTX,        0xFF}, //  SOFTMODEMRX,

  {SPLITTER0_OUTL,     LS_SRC_SPLITTER}, //  SPLITTER0_IN,
  {SPLITTER1_OUTL,     LS_SRC_SPLITTER}, //  SPLITTER1_IN,
  {SPLITTER2_OUTL,     LS_SRC_SPLITTER}, //  SPLITTER2_IN,
  {SPLITTER3_OUTL,     LS_SRC_SPLITTER}, //  SPLITTER3_IN,

  {MERGER0_OUT,        LS_SRC_MERGER}, //  MERGER0_INL,                        
  {MERGER0_OUT,        LS_SRC_MERGER}, //  MERGER0_INR,
  {MERGER1_OUT,        LS_SRC_MERGER}, //  MERGER1_INL,
  {MERGER1_OUT,        LS_SRC_MERGER}, //  MERGER1_INR,
  {MERGER2_OUT,        LS_SRC_MERGER}, //  MERGER2_INL,
  {MERGER2_OUT,        LS_SRC_MERGER}, //  MERGER2_INR,
  {MERGER3_OUT,        LS_SRC_MERGER}, //  MERGER3_INL,
  {MERGER3_OUT,        LS_SRC_MERGER}, //  MERGER3_INR,

#else
#error ASIC type is invalid
#endif  // ASIC_A1
  {0xFF,               0xFE}, //  MODEM1TX2ACLNK, 
};


static fsend_command RiptideSendCmd;

static
UCHAR OutputChannels[] = {
  SPARE,
  M0TX,
  M1TX,
  TAMTX,
  HSSPKR,
  PDAC,
  DSNDTX0,
  DSNDTX1,
  DSNDTX2,
  DSNDTX3,
  DSNDTX4,
  DSNDTX5,
  DSNDTX6,
  DSNDTX7,
  WVSTRTX,
  COP3DTX,
};

static
UCHAR InputChannels[] = {
  PADC
};

static
UCHAR LbusSource[CHANNEL_ID_COUNTER] = 
{
  ARM2LBUS_FIFO0,
  ARM2LBUS_FIFO1, 
  ARM2LBUS_FIFO2,
  ARM2LBUS_FIFO3,      
  ARM2LBUS_FIFO4,
  ARM2LBUS_FIFO5, 
  ARM2LBUS_FIFO6,
  ARM2LBUS_FIFO7, 
  ARM2LBUS_FIFO8,
  ARM2LBUS_FIFO9, 
  ARM2LBUS_FIFO10,
  ARM2LBUS_FIFO11,
  ARM2LBUS_FIFO12,
  ARM2LBUS_FIFO13,
  ARM2LBUS_FIFO14,
  DIGITAL_MIXER_OUT0,
  ACLNK2MODEM0RX,
  ACLNK2HNDMIC,
  ACLNK2MODEM1RX,
  SR3_4_OUT,
  ACLNK2MIC,     
  ACLNK2PADC, //DIGITAL_MIXER_OUT0, //ACLNK2PADC,
  ARM2LBUS_FIFO_DIRECT,
  ARM2LBUS_FIFO_DIRECT
};

static fsend_command RiptideSendCmd = NULL;

// Tables of LBUS paths for streams.
// Format:
// { sink01, sink02,...,sink0n[,0xFF][,sink11,...,sink1n, 0xFF] }
//    ^        ^                ^      ^first sink for next branch
//    ^        ^                ^------ end of current branch of the path
//    !--------!----------------------- sink numbers
// Note: If sink number is more then maximum valid LBUS sink number last source is using
//

#define SPLIT_PATH  0x80    // sing to split the path into 2 branch from next source

#ifdef ASIC_A0
// primary audio playback
static
UCHAR LbusAudioStereoPlaybackPath[] = {DIGITAL_MIXER_IN0, 0xFF};

#define LbusAudioMonoPlaybackPath LbusAudioStereoPlaybackPath

static
UCHAR LbusAudioStereoSrcPlaybackPath[] = {INTER0_IN, DIGITAL_MIXER_IN0, 0xFF};

#define LbusAudioMonoSrcPlaybackPath      LbusAudioStereoSrcPlaybackPath
#define LbusAudioAltStereoSrcPlaybackPath LbusAudioStereoSrcPlaybackPath
#define LbusAudioAltStereoSrcRecordPath   LbusAudioStereoSrcRecordPath

// primary audio record
static
UCHAR LbusAudioStereoRecordPath[] = {LBUS2ARM_FIFO5};
#define LbusAudioMonoRecordPath LbusAudioStereoRecordPath

static
UCHAR LbusAudioStereoSrcRecordPath[] = {DECIM0_IN, LBUS2ARM_FIFO5};
#define LbusAudioMonoSrcRecordPath LbusAudioStereoSrcRecordPath

#endif //ASIC_A0

#ifdef ASIC_A1

// primary audio playback
static
UCHAR LbusAudioStereoPlaybackPath[] = {DIGITAL_MIXER_IN0, 0xFF};
#define LbusAudioMonoPlaybackPath LbusAudioStereoPlaybackPath

static
UCHAR LbusAudioStereoSrcPlaybackPath[] = {INTER0_IN, DIGITAL_MIXER_IN0, 0xFF};
static
UCHAR LbusAudioMonoSrcPlaybackPath[]   = {INTERM0_IN, DIGITAL_MIXER_IN0, 0xFF};

static
UCHAR LbusAudioAltMonoSrcPlaybackPath[] = {E2SINK_MAX | SPLIT_PATH, MERGER0_INL, INTER0_IN, SPLITTER0_IN, DIGITAL_MIXER_IN0, 0xFF, MERGER0_INR, 0xFF};

static
UCHAR LbusAudioAltStereoSrcPlaybackPath[] = {SPLITTER0_IN | SPLIT_PATH, INTERM0_IN, MERGER0_INL, DIGITAL_MIXER_IN0, 0xFF, INTERM0_IN, MERGER0_INR, 0xFF};

// primary audio record
static
UCHAR LbusAudioStereoRecordPath[] = {LBUS2ARM_FIFO5};
#define LbusAudioMonoRecordPath LbusAudioStereoRecordPath

static
UCHAR LbusAudioStereoSrcRecordPath[] = {DECIM0_IN, LBUS2ARM_FIFO5};
//#define LbusAudioMonoSrcRecordPath LbusAudioStereoSrcRecordPath
static
UCHAR LbusAudioMonoSrcRecordPath[]   = {DECIMM3_IN, LBUS2ARM_FIFO5};
#ifdef RT_UNUSED
static
UCHAR LbusAudioAltMonoSrcRecordPath[] = {DECIM0_IN, LBUS2ARM_FIFO5};  // the same as LbusAudioStereoSrcRecordPath

static
UCHAR LbusAudioAltStereoSrcRecordPath[] = {SPLITTER0_IN | SPLIT_PATH, DECIMM0_IN, MERGER0_INL, LBUS2ARM_FIFO5, 0xFF, DECIMM0_IN, MERGER0_INR};

static
UCHAR LbusAudioI2SRecordPath[] = {LBUS2ARM_FIFO7};
#endif

#endif //ASIC_A1


#ifdef RT_UNUSED
// wavestream
static
UCHAR LbusWaveStreamPath[]     = {INTER0_IN, DIGITAL_MIXER_IN4, 0xFF /*PDAC2ACLNK*/};

// primary audio record
static
UCHAR LbusAudioRecordPath[]   = {DECIM0_IN, LBUS2ARM_FIFO5};

// microphone input
static
UCHAR LbusMicPath[]           = {DECIM0_IN, LBUS2ARM_FIFO4};
#endif

// digital mixer output to Line out
static
UCHAR LbusMixerOut[]          = {PDAC2ACLNK};

// digital mixer output to Head phone
static
UCHAR LbusMixerOutHnd[]          = {HNDSPK2ACLNK};

#ifdef SUPPORT_MIXER
#include <limits.h>

// Table of correspondence between mixer controls and mixer registers
// Format:
// {mixer register, value mask}
// Note: mixer registers and masks are defined in hardware.h
static
const UINT gauControlMasks [MAX_MIXER_CONTROLS][2] = {   
{RWAM_MUX_SLCT},    {RWAM_MUX_MSTR},    {RWAM_MUX_MSTR},    {RWAM_MUX_MSTR},    //4
{RWAM_MUX_MSTR},    {RWAM_MUX_MSTR},    {RWAM_MUX_MSTR},    {RWAM_MUX_MSTR},    //8
{RWAM_MIX_SPKR},    {RWAM_MIX_WAVE0},   {RWAM_MIX_WAVE1},   {RWAM_MIX_WAVE2},   //12
{RWAM_MIX_WAVE3},   {RWAM_MIX_WAVE4},   {RWAM_MIX_WAVE5},   {RWAM_MIX_WAVE6},   //16
{RWAM_MIX_WAVE7},   {RWAM_MIX_WAVE8},   {RWAM_MIX_WAVE9},   {RWAM_MIX_WSTRM},   //20
{RWAM_MIX_FMOUT},   {RWAM_MIX_SBOUT},   {RWAM_MIX_PHONE},   {RWAM_MIX_CD},      //24
{RWAM_MIX_LINE},    {RWAM_MIX_VIDEO},   {RWAM_MIX_AUX},     {RWAM_MIX_MIC},     //28
{RWAM_MIX_ASIC},    
{RWAM_MUTE_SPKR},   {RWAM_MUTE_WAVE0},  {RWAM_MUTE_WAVE1},  {RWAM_MUTE_WAVE2},  //32
{RWAM_MUTE_WAVE3},  {RWAM_MUTE_WAVE4},  {RWAM_MUTE_WAVE5},  {RWAM_MUTE_WAVE6},  //36
{RWAM_MUTE_WAVE7},  {RWAM_MUTE_WAVE8},  {RWAM_MUTE_WAVE9},  {RWAM_MUTE_WSTRM},  //40
{RWAM_MUTE_FMOUT},  {RWAM_MUTE_SBOUT},  {RWAM_MUTE_PHONE},  {RWAM_MUTE_CD},     //44
{RWAM_MUTE_LINE},   {RWAM_MUTE_VIDEO},  {RWAM_MUTE_AUX},    {RWAM_MUTE_MIC},    //48
{RWAM_MUTE_ASIC},
{RWAM_MIX_3D},      {RWAM_MIX_BASS},    {RWAM_MIC_BOOST},   {RWAM_MIC_BOOST},   //52
{RWAM_MUTE_MUX},    {RWAM_MIX_BREAK},   {RWAM_MUTE_BREAK},  {RWAM_MIX_I2S_0},   //56
{RWAM_MIX_I2S_1},   {RWAM_MUTE_I2S_0},  {RWAM_MUTE_I2S_1},  {RWAM_MIX_HDPHN},   //60
{RWAM_MUTE_HDPHN}
};

#endif  // SUPPORT_MIXER

static
BOOLEAN
RipTideHwStopDMA(
  void *pChannel
);

static
BOOLEAN
RipTideHwStartDMA(
    void *pChannel
);

static
BOOLEAN
RipTideHwFreePath(
  PChannelInfo	pChannelInfo
);




/*******************************************************************
 *
 *  Synchronization
 *
 *******************************************************************/
static
VOID
RipTideHwEnter(system_objects *sysObj)
{
//  TRACE(("SpinLock %x locked\n", pSGC->mChannel));
//  DbgPrint ("SpinLock %d locked\n", pSGC->ucChannelId);
//  dprintf(("<"));
  acquire_spinlock(sysObj);
}

static
VOID
RipTideHwLeave(system_objects *sysObj)
{
//  TRACE(("SpinLock %x released\n", pSGC->mChannel));
//  DbgPrint ("SpinLock %d released\n", pSGC->ucChannelId);
  release_spinlock(sysObj);
//  dprintf((">"));
}


/*******************************************************************/

static
BYTE
FindPlaybackChannel(PHwInfo     pHwInfo)
{
int i;
UCHAR **LbusPath = pHwInfo->LbusPath;

  TRACE(("Started FindPlaybackChannel\n"));
  for(i = 0; i < sizeof(OutputChannels); i++)
  {
    TRACE(("%x - %x %p\n", i, OutputChannels[i], LbusPath[OutputChannels[i]]));
    if(LbusPath[OutputChannels[i]] == NULL)
      return (BYTE)OutputChannels[i];
  }
  return 0xFF;
}

static
BYTE
FindRecordChannel(PHwInfo     pHwInfo)
{
int i;
UCHAR **LbusPath = pHwInfo->LbusPath;

  TRACE(("Started FindRecordChannel\n"));
  for(i = 0; i < sizeof(InputChannels); i++)
  {
    TRACE(("%x - %x %p\n", i, InputChannels[i], LbusPath[InputChannels[i]]));
    if(LbusPath[InputChannels[i]] == NULL)
      return (BYTE)InputChannels[i];
  }
  return 0xFF;
}


static
VOID 
RipTideDeleteChannel(
    void * pChannel,
    system_objects *sysObj
    )
{
    PChannelInfo    pChannelInfo = (PChannelInfo)pChannel;

    dprintf2(("<RT_HW> release channel: %p\n",pChannelInfo));
    if(pChannelInfo)
    {
        cancel_timer(sysObj);

        if(pChannelInfo->ucChannelState != SGC_STOP)
            RipTideHwStopDMA(pChannelInfo);
        
        pChannelInfo->pHwInfo->Channels[pChannelInfo->ChannelId] = NULL;

        RipTideHwFreePath(pChannelInfo);
        if(pChannelInfo->DMABuffer)
        {
            free_virtual(pChannelInfo->DMABuffer); /*, pChannelInfo->DMABufferSize);*/
            dprintf2(("%d.1: %p\n", pChannelInfo->ChannelId, pChannelInfo->DMABuffer));
        }
		if(pChannelInfo->sgdList)
		{
		    free_mem(pChannelInfo->sgdList, sizeof( SGD_DMA )*( DESC_MAX_MASK + 1 ));
            dprintf2(("%d.2: %p\n",pChannelInfo->ChannelId, pChannelInfo->sgdList));
        }
        free_mem(pChannelInfo, sizeof(ChannelInfo));
            dprintf2(("%d.3: %p\n", pChannelInfo->ChannelId, pChannelInfo));
    }
}

static
void InterruptWatchDog(unsigned long pContext)
{
  dprintf(("!\n"));
  if(((PChannelInfo)pContext)->ucChannelState == SGC_ACTIVE)
  {
      dprintf(("!!!\n"));
      RipTideHwStopDMA((PChannelInfo)pContext);
      RipTideHwStartDMA((PChannelInfo)pContext);
  }
}

static
void *
RipTideCreateChannel(
    void    *pHwInfo,
    int     iCapture,
    void    *pContext,
    system_objects *sysObj)
{
PChannelInfo    pChannelInfo = NULL;
unsigned char   ucChannelId;
BOOLEAN         state = TRUE;

    dprintf2(("<RT_HW> RipTideCreateChannel started: %p, %x\n", pHwInfo, iCapture));

    if(iCapture)
		ucChannelId = FindRecordChannel((PHwInfo)pHwInfo);
	else
		ucChannelId = FindPlaybackChannel((PHwInfo)pHwInfo);

    dprintf2(("<RT_HW> Channel : %d\n", ucChannelId));
	if(ucChannelId != 0xFF)
	{
	    if((pChannelInfo = (PChannelInfo)allocate_mem(sizeof(ChannelInfo))) != NULL)
	    {
            pChannelInfo->Capture          = iCapture;
	        pChannelInfo->ucChannelState   = SGC_STOP;
	        pChannelInfo->sgdList          = NULL;
	        pChannelInfo->DMABuffer        = NULL;
	        pChannelInfo->DMAFragmentNum   = DESC_MAX_MASK + 1;
            pChannelInfo->DMAFragmentSize  = MAX_FRAG_SIZE;
            pChannelInfo->DMAFragmentMax   = DESC_MAX_MASK + 1;
            pChannelInfo->DMABufferSize    = pChannelInfo->DMAFragmentSize * pChannelInfo->DMAFragmentMax;
            pChannelInfo->DMAType          = 0;
            pChannelInfo->pHwInfo          = (PHwInfo)pHwInfo;
	        pChannelInfo->ChannelId        = ucChannelId;
            pChannelInfo->cmdIf            = CMDA;
            ((PHwInfo)pHwInfo)->Channels[ucChannelId] = pChannelInfo;

            pChannelInfo->Sgd_In           =
            pChannelInfo->Sgd_Out          = 0;
            pChannelInfo->Total_In         =
            pChannelInfo->Total_Out        = 0;
            pChannelInfo->ulIOCounter      =
            pChannelInfo->ulIOSize         = 0;
            pChannelInfo->ulHead           =
            pChannelInfo->ulTail           = 0;
            pChannelInfo->ucChannelState   = SGC_STOP;
            pChannelInfo->ucChannelDir     = 0;
            state = FALSE;
            pChannelInfo->pContext         = pContext;
            pChannelInfo->nChannels        = 2;
            pChannelInfo->SamplesPerSec    = 44100;
            pChannelInfo->BitsPerSample    = 16;
            initialize_timer(sysObj, InterruptWatchDog, (void *)pChannelInfo);
        }
        else
        {
		    dprintf(("<RT_HW> ERROR!!! No memory for channel discriptor\n"));
        }
    }

    if(state)
    {
        RipTideDeleteChannel(pChannelInfo, sysObj);
        pChannelInfo = NULL;
    }
    return (void *)pChannelInfo;
}


/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwFreeSinglePath(PRTHardwareContext pSGC,
 *                                                  UCHAR              ucSource,
 *                                                  UCHAR              **pLbusPath)
 *
 *  Parameters:     pSGC     - Pointer to srteam context structure
 *                  ucSource - LBUS first source
 *                  pLbuPath - LBUS path
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Free single LBUS path
 *
 *************************************************************************/

static
BOOLEAN
RipTideHwFreeSinglePath(
  PChannelInfo	pChannelInfo,
  UCHAR         ucSource,
  UCHAR       **pLbusPath)
{
PHwInfo     pHwInfo = pChannelInfo->pHwInfo;
UCHAR       ucChannelId = pChannelInfo->ChannelId;
UCHAR       *pucSink;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;
UCHAR       ucCDMAorCDMB = pChannelInfo->cmdIf;

//  TRACE(("RipTideHwFreeSinglePath started\n"));

  if((*pLbusPath == NULL))
  {
    TRACE(("ERROR! NULL LBUS path: %p\n", *pLbusPath));
    return FALSE;
  }

  pucSink = *pLbusPath;

  while((*pucSink != 0xFF) && (ucSource != 0xFF))
  {
UCHAR ucSink;

    ucSink = *pucSink & 0x7f;
    
#ifdef MONO_VIA_STEREO
    if(ucSink < E2SINK_MAX)
    {
#endif

    while(pHwInfo->LbusSinks[ucSink] != ucChannelId)
    {
      if(LbusIn2Out[ucSink][LBUS_SINK_TYPE] == LbusIn2Out[ucSink+1][LBUS_SINK_TYPE])
        ucSink++;
      else
      {
        ucSink = LBUS_EMPTY;
        break;
      }
    }

    if(ucSink >= LBUS_EMPTY)
    {
      TRACE(("ERROR! - No the sink %d in path for channel: %d\n", *pucSink, ucChannelId));
      return FALSE;
    }

    TRACE(("Src: %d Sink: %d Channel: %d\n", ucSource, ucSink, ucChannelId));
    pHwInfo->LbusSinks[ucSink] = CHANNEL_ID_COUNTER;

    SEND_PCLR(ucCDMAorCDMB, ucSource, ucSink);

    ucSource = LbusIn2Out[ucSink][LBUS_SINK_OUT];

#ifdef MONO_VIA_STEREO
    }
#endif

    if(*pucSink++ & SPLIT_PATH)
    {
UCHAR ucSplit;

      switch(LbusIn2Out[ucSink][LBUS_SINK_TYPE])
      {
        case LS_SRC_SPLITTER:
          ucSplit = ucSource+1;
          break;
        default:
          ucSplit = ucSource;
      }

      if(RipTideHwFreeSinglePath(pChannelInfo, ucSplit, &pucSink) == FALSE)
      {
        TRACE(("ERROR !!! Path free failed: Src: %d Sink: %d\n", ucSource, *pucSink));
        return FALSE;
      }
    }
  }

  *pLbusPath = ++pucSink;

  return TRUE;
}

/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwFreePath(PRTHardwareContext pSGC)
 *
 *  Parameters:     pSGC     - Pointer to srteam context structure
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Free LBUS path for a stream
 *
 *************************************************************************/

static
BOOLEAN
RipTideHwFreePath(
  PChannelInfo	pChannelInfo
)
{
UCHAR *pucSinks;
BOOLEAN ret;
PHwInfo pHwInfo = pChannelInfo->pHwInfo;
UCHAR ucChannelId = pChannelInfo->ChannelId;

  TRACE(("|\n|\n|\n|\nRipTideHwFreePath started: id: %d|\n|\n|\n|\n\n", ucChannelId));
  pucSinks = pHwInfo->LbusPath[ucChannelId];

  pChannelInfo->SRCNum[0] = 
  pChannelInfo->SRCNum[1] = 0xFF;

  pHwInfo->LbusPath[ucChannelId] = NULL;

  if(pChannelInfo->DMIXNum < DM_MAX_NUM)
    pHwInfo->DigitalMixers[pChannelInfo->DMIXNum][DM_NUMBER] = 0xFF;

  pChannelInfo->DMIXNum       = DM_MAX_NUM;
  pChannelInfo->MixerControls = MAX_MIXER_CONTROLS;

  RipTideHwEnter( pHwInfo->sysObj );

  ret = RipTideHwFreeSinglePath(
            pChannelInfo,
            LbusSource[ucChannelId],
            &pucSinks);
  RipTideHwLeave   ( pHwInfo->sysObj );
  TRACE(("Ended RipTideHwFreePath\n"));
  return ret;
}


/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwSelectSinglePath(PRTHardwareContext pSGC,
 *                                                  UCHAR              ucSource,
 *                                                  UCHAR              **pLbusPath)
 *
 *  Parameters:     pSGC     - Pointer to srteam context structure
 *                  ucSource - LBUS first source
 *                  pLbuPath - LBUS path
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Allocate single LBUS path
 *
 *************************************************************************/

static
BOOLEAN
RipTideHwSelectSinglePath(
  PChannelInfo	pChannelInfo,
  UCHAR   ucSource,
  UCHAR **pLbusPath)
{
PHwInfo pHwInfo = pChannelInfo->pHwInfo;
UCHAR ucChannelId = pChannelInfo->ChannelId;
UCHAR *pucList, ucSrc, ucCDMAorCDMB;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;

  TRACE(("\n\n\nRipTideHwSelectSinglePath started: src: %d path: %p\n\n\n\n", ucSource, *pLbusPath));

  ucCDMAorCDMB = pChannelInfo->cmdIf;

  if(*pLbusPath == NULL)
  {
    TRACE(("ERROR! - NULL LBUS path\n"));
    return FALSE;
  }

  pucList = *pLbusPath;
  ucSrc   = ucSource;

  while((*pucList != 0xFF) && (ucSrc != 0xFF))
  {
UCHAR ucSink, ucSink1;
int iDelta;

    ucSink1 = ucSink  = *pucList & 0x7f;
    iDelta  = -1;


#ifdef MONO_VIA_STEREO
    if(ucSink < E2SINK_MAX)
    {
#endif

    while((pHwInfo->LbusSinks[ucSink] <  CHANNEL_ID_COUNTER) /* &&
          (pSGC->pLbusSinks[ucSink] != pSGC->ucChannelId)*/)
    {
      TRACE(("LBUS CONFLICT !\npLbusSinks[%d]: %d\n", ucSink, pHwInfo->LbusSinks[ucSink]));
      if(ucSink > 0)
        if(LbusIn2Out[ucSink][LBUS_SINK_TYPE] == LbusIn2Out[ucSink+iDelta][LBUS_SINK_TYPE])
        {
          ucSink += iDelta;
          continue;
        }

      if(iDelta < 0)
      {
        ucSink = ucSink1 + 1;
        iDelta = 1;
      }
      else
      {
        ucSink = LBUS_EMPTY;
        break;
      }
    }

    if(ucSink >= LBUS_EMPTY)
    {
      TRACE(("ERROR !!! Path allocation failed: Src: %d Sink: %d\n", ucSource, ucSink1));
      return FALSE;
    }
    
    TRACE(("Src: %d Sink: %d\n", ucSrc, ucSink));

//    if(pSGC != NULL)
    {
      switch(LbusIn2Out[ucSink][LBUS_SINK_TYPE])
      {
        case LS_SRC_INTERPOLATOR:
        case LS_SRC_INTERPOLATORM:
        case LS_SRC_DECIMATOR:
        case LS_SRC_DECIMATORM:
          if( pChannelInfo->SRCNum[0] < E2SINK_MAX )
            pChannelInfo->SRCNum[1] = ucSink;
          else
            pChannelInfo->SRCNum[0] = ucSink;
          break;
        case LS_MIXER_IN:
          if(pChannelInfo->DMIXNum < DM_MAX_NUM)
            pHwInfo->DigitalMixers[pChannelInfo->DMIXNum][DM_NUMBER] = ucSink - DIGITAL_MIXER_IN0;
          break;
      }
    
      pHwInfo->LbusSinks[ucSink] = ucChannelId;

      SEND_PSEL(ucCDMAorCDMB, ucSrc, ucSink);
    }

    ucSrc = LbusIn2Out[ucSink][LBUS_SINK_OUT];

#ifdef MONO_VIA_STEREO
    }
#endif

    if(*pucList++ & SPLIT_PATH)
    {
/*
      UCHAR ucSplit;

      switch(LbusIn2Out[ucSink][LBUS_SINK_TYPE])
      {
        case LS_SRC_SPLITTER:
          ucSplit = ucSrc+1;
          break;
        default:
          ucSplit = ucSrc;
      }
*/
      if(RipTideHwSelectSinglePath(pChannelInfo, ucSrc, &pucList) == FALSE)
      {
//        TRACE(("ERROR !!! Path allocation failed: Src: %d Sink: %d\n", ucSource, pucList));
        return FALSE;
      }

      if(LbusIn2Out[ucSink][LBUS_SINK_TYPE] == LS_SRC_SPLITTER)
        ucSrc++;
    }
  }

  *pLbusPath = ++pucList;
  return TRUE;
}

/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwSelectPath(PRTHardwareContext pSGC)
 *
 *  Parameters:     pSGC     - Pointer to srteam context structure
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Allocate LBUS path for a stream
 *
 *************************************************************************/

static
BOOLEAN
RipTideHwSelectPath(
  PChannelInfo	pChannelInfo
)
{
PHwInfo pHwInfo = pChannelInfo->pHwInfo;
UCHAR *pLbusPath, *pAltLbusPath, **LbusPath;
BOOLEAN ret = TRUE;
UCHAR	ucChannelId;
BOOLEAN	Capture;
UCHAR	nChannels;

  ucChannelId = pChannelInfo->ChannelId;
  Capture     = pChannelInfo->Capture;
  nChannels   = pChannelInfo->nChannels;
  LbusPath    = pChannelInfo->pHwInfo->LbusPath;

  TRACE(("|\n|\n|\n|\nRipTideHwSelectPath started: id: %d|\n|\n|\n|\n\n", ucChannelId));

#if 0
  {
int i;
  for(i=0; i < E2SINK_MAX; i++)
  {
    DbgPrint("%2d: %2d - ", i, LbusSinks[i]);
    switch(LbusIn2Out[i][1])
    {
    case LS_SRC_INTERPOLATOR:
      DbgPrint("SRC stereo interpolator\n");
      break;
    case LS_SRC_INTERPOLATORM:
      DbgPrint("SRC mono interpolator\n");
      break;
    case LS_SRC_DECIMATOR:
      DbgPrint("SRC stereo decimator\n");
      break;
    case LS_SRC_DECIMATORM:
      DbgPrint("SRC mono decimator\n");
      break;
    case LS_MIXER_IN:
      DbgPrint("digital mixer input\n");
      break;
    case LS_MIXER_GAIN_FUNCTION:
      DbgPrint("digital gain input\n");
      break;
    case LS_SRC_SPLITTER:
      DbgPrint("splitter input\n");
      break;
    case LS_SRC_MERGER:
      DbgPrint("merger input\n");
      break;
    default:
      DbgPrint("\n");
      break;
    }
  }
  }
#endif

  if(Capture)
  {
// record
    if(TRUE /*DataFormat->WaveFormatEx.nSamplesPerSec != 48000*/)
    {
      pAltLbusPath = NULL;
      if(nChannels < 2)
      {
        pLbusPath    = &LbusAudioMonoSrcRecordPath[0];
      }
      else
      {
        pLbusPath    = &LbusAudioStereoSrcRecordPath[0];
      }
    }
    else
    {
      pAltLbusPath = NULL;
      if(nChannels < 2)
      {
        pLbusPath = &LbusAudioMonoRecordPath[0];
      }
      else
      {
        pLbusPath = &LbusAudioStereoRecordPath[0];
      }
    }
  }
  else
  {
// playback
    if(TRUE /*DataFormat->WaveFormatEx.nSamplesPerSec != 48000*/)
    {
      if(nChannels < 2)
      {
        pLbusPath    = &LbusAudioMonoSrcPlaybackPath[0];
  #ifdef MONO_VIA_STEREO
        pAltLbusPath = &LbusAudioAltMonoSrcPlaybackPath[0];
  #else
        pAltLbusPath = &LbusAudioStereoSrcPlaybackPath[0];
  #endif
      }
      else
      {
        pLbusPath    = &LbusAudioStereoSrcPlaybackPath[0];
        pAltLbusPath = &LbusAudioAltStereoSrcPlaybackPath[0];
      }
    }
    else
    {
      pAltLbusPath = NULL;
      if(nChannels < 2)
      {
        pLbusPath = &LbusAudioMonoPlaybackPath[0];
      }
      else
      {
        pLbusPath = &LbusAudioStereoPlaybackPath[0];
      }
    }
  }

  pHwInfo->LbusPath[ucChannelId] = pLbusPath;

  pChannelInfo->DMIXNum       = ucChannelId;
  pChannelInfo->MixerControls = MXMIX_WAVE0 + ucChannelId;
  
  RipTideHwEnter( pHwInfo->sysObj );
  
  if(RipTideHwSelectSinglePath(
       pChannelInfo,
       LbusSource[ucChannelId],
       &pLbusPath) == FALSE)
  {
    if(pAltLbusPath != NULL)
    {
      pHwInfo->LbusPath[ucChannelId] = pLbusPath = pAltLbusPath;
      if(RipTideHwSelectSinglePath(
            pChannelInfo,
            LbusSource[ucChannelId],
            &pLbusPath) == FALSE)
      {
//        pHwInfo->LbusPath[ucChannelId] = NULL;
        ret = FALSE;
//        goto err_exit;
      }
    }
    else
    {
//      pHwInfo->LbusPath[ucChannelId] = NULL;
      ret = FALSE;
//      goto err_exit;
    }
  }
  
  RipTideHwLeave   ( pChannelInfo->pHwInfo->sysObj );

  if(ret == FALSE)
  {
    RipTideHwFreePath( pChannelInfo );
    ret = FALSE;
  }

  return ret;
}

#ifdef SUPPORT_MIXER
static
VOID
SoundMixerTimerDeferred(
    unsigned long Context
);
#endif

static
void RipTideSetFunction(rt_func_table *func_table);

/*************************************************************************
 *
 *  Name:           VOID
 *                    RipTideHwInitialize(
 *                      IN OUT PGLOBAL_DEVICE_INFO pGDI
 *                      )
 *
 *
 *  Parameters:     pGDI - Pointer to global device info (see sound.h)
 *
 *  Returns:        None.
 *
 *  Description:
 *    Perform initialization of structures for wave and midi operation,
 *    hardware context. Initialize RipTide hardware.
 *
 *************************************************************************/

void * 
RipTideHwInitialize
(
    int             hwIOBase,
    int             hwIrq,
    rt_func_table   *func_table,
    void            *CMDIFp,
    void            (*pfunc)(void),
    system_objects  *sysObj
)
{
PHwInfo pHwInfo;
int i;
tCMDIF_AB *pCMDIF;

  TRACE(("Started RipTideHwInitialize\n"));

  RiptideSendCmd = (fsend_command)pfunc;

  if(RiptideSendCmd == NULL)
    return NULL;

  if((pHwInfo = allocate_mem(sizeof(HwInfo))) == NULL)
  {
    dprintf(("<RT_HW> ERROR!!! No memory for hardware info\n"));
    return NULL;
  }

  
  pHwInfo->sysObj = sysObj;
  pHwInfo->hwIOBase = hwIOBase;
  pHwInfo->hwIrq    = hwIrq;
  
  pCMDIF = pHwInfo->tCmdIf = CMDIFp;
// Init interface spin locks

/*
// To do: insert initialization of Linux spinlock here
*/
  
//  init_spinlock(&pHwInfo->sysObj);
  
  pHwInfo->ulCurrentChannel = 0;
  pHwInfo->isStarted = FALSE;

// Init LBUS sink usage table
  for(i = 0; i < E2SINK_MAX; i++)
    pHwInfo->LbusSinks[i] = CHANNEL_ID_COUNTER;

// Init digital mixer usage table
  for(i = 0; i < DM_MAX_NUM; i++)
  {
    pHwInfo->DigitalMixers[i][DM_NUMBER] = 0xFF;
    pHwInfo->DigitalMixers[i][DM_LVALUE] = 
    pHwInfo->DigitalMixers[i][DM_RVALUE] = 0x7FFF;
  }

// Init resource  tables
  for(i = 0; i < CHANNEL_ID_COUNTER; i++)
  {
/*    pChannelInfo->DMIXNum         = DM_MAX_NUM;*/
/*    pChannelInfo->MixerControls   = MAX_MIXER_CONTROLS;*/
/*    pChannelInfo->SRCNum[0]       = */
/*    pChannelInfo->SRCNum[1]       = E2SINK_MAX;*/
    pHwInfo->LbusPath[i] = NULL;
    pHwInfo->Channels[i] = NULL;
  }

// Init mixer change timer and DPC
#ifdef SUPPORT_MIXER
    initialize_timer(pHwInfo->sysObj, SoundMixerTimerDeferred, (void *)pHwInfo);
    {
        char *pData;
        pData = (char *)(&(pHwInfo->Mixers[0]));
        memset(pData, 0, sizeof(MixerTab) * MAX_MIXER_CONTROLS);
    }
#endif

/* Init riptide hardware - already it is initialized
    if(!RiptideInitialize(
        pCMDIF, 
        pHwInfo->deviceId,
        pHwInfo->hwIOBase))
    {
        dprintf(("<RT_HW> ERROR!!! Init is failed\n"));
        return NULL;
    }
*/
    memcpy(pCMDIF, CMDIFp, sizeof(tCMDIF_AB));
    RiptideSendCmd = (fsend_command)pfunc;

// Get hardware version
  i=0;
  while(i<5000)
  {
      CMDRET_TYPE rptr = {{ (DWORD)0, (DWORD) 0}};

    if( SEND_GETV(CMDA, &rptr) )
    {
      pHwInfo->VersionASIC   = rptr.wRetWords[0];
      pHwInfo->VersionCODEC  = rptr.wRetWords[1];
      pHwInfo->VersionAUXDSP = rptr.wRetWords[2];
      pHwInfo->VersionPROG   = rptr.wRetWords[3];
      break;
    }
    i++;
  }

// Get hardware version
  i=0;
  while(i<5000)
  {
    CMDRET_TYPE rptr = {{ (DWORD)0, (DWORD) 0}};

    if( SEND_GETV(CMDA, &rptr) )
    {
      pHwInfo->VersionASIC   = rptr.wRetWords[0];
      pHwInfo->VersionCODEC  = rptr.wRetWords[1];
      pHwInfo->VersionAUXDSP = rptr.wRetWords[2];
      pHwInfo->VersionPROG   = rptr.wRetWords[3];
      break;
    }
    i++;
  }
  
#if 0
// Set default values  
  SEND_PLST(CMDA, 0);
  SEND_SLST(CMDA, 0);
  SEND_DLST(CMDA, 0);
  SEND_ALST(CMDA, 0);

#ifdef SUPPORT_MODEM
/*
 *  Reserve resources for modem:
 *  1. Digital mixer
 *  2. Stereo (?) interpolator
 *  3. Merger
 */
//  if(pGDI->ConfigData.ModemMemoryAddress != 0)
  {
    SEND_SETDPLL(CMDA);
    i=0;
    while(i<5000)
    {
      CMDRET_TYPE rptr = {{ (DWORD)0, (DWORD) 0}};

      if( SEND_GETV(CMDA, &rptr) )
      {
        pHwInfo->VersionASIC   = rptr.wRetWords[0];
        pHwInfo->VersionCODEC  = rptr.wRetWords[1];
        pHwInfo->VersionAUXDSP = rptr.wRetWords[2];
        pHwInfo->VersionPROG   = rptr.wRetWords[3];
        break;
      }
      i++;
    }
  }

#endif  //SUPPORT_MODEM
#endif
    pHwInfo->DigitalMixers[DM_PHONE][DM_NUMBER]  = MODEM_MIXER - DIGITAL_MIXER_IN0;
    pHwInfo->LbusSinks[MODEM_MIXER]        =
    pHwInfo->LbusSinks[MODEM_SRC]          =
    pHwInfo->LbusSinks[MODEM_MERGER]       = 
    pHwInfo->LbusSinks[MODEM_MERGER+1]     = MODEM_CHANNEL;

// Notify firmware about allocated resources
    
    SEND_LSEL(CMDA, MODEM_CMD, 0, 0, (MODEM_SRC - INTER0_IN), (MODEM_MERGER - MERGER0_INL)/2, 0, (MODEM_MIXER - DIGITAL_MIXER_IN0));
  

// Select path for PCM output
  {
UCHAR *pLbusPath;
ChannelInfo SampleChannel;
    SampleChannel.pHwInfo = pHwInfo;

#ifdef SUPPORT_SYNTH

// Reserve digital mixers for legacy SB out and FM out

//    DigitalMixers[DM_FMOUT][DM_NUMBER] =  9;
//    pGDI->LbusSinks[DIGITAL_MIXER_IN9]  = 14;

    pLbusPath             = &LbusAudioStereoPlaybackPath[0];
    SampleChannel.DMIXNum = DM_FMOUT;

    SampleChannel.ChannelId = FM_CHANNEL;
    RipTideHwSelectSinglePath(&SampleChannel, OPL3_SAMPLE, &pLbusPath);
    pHwInfo->LbusPath[FM_CHANNEL] = pLbusPath;

    SEND_LSEL(CMDA, FM_CMD, 0, 0, 0, 0, 0, (pHwInfo->DigitalMixers[DM_FMOUT][DM_NUMBER]));
// Temporary until topo does not restore FM volume
    SEND_SDGV(CMDA,
              (WORD)pHwInfo->DigitalMixers[DM_FMOUT][DM_NUMBER],
              (WORD)pHwInfo->DigitalMixers[DM_FMOUT][DM_NUMBER],
              0x7FFF, 0x7FFF);

    {
      DWORD data;

      data = 0x30648 + pHwInfo->DigitalMixers[DM_FMOUT][DM_NUMBER]*4;
//      RiptideWriteArm(pCMDIF, data, 0x01, 0x00000005 );
        RiptideSendCmd (NULL  , data, 0x01, 0x00000005, (PCMDRET_TYPE)pCMDIF);
   
      // Set 48 kHz
//      RiptideWriteArm(pCMDIF, 0x301A8, 0x02, 0x00000002 );
        RiptideSendCmd (NULL  , 0x301A8, 0x02, 0x00000002, (PCMDRET_TYPE)pCMDIF);
//      RiptideWriteArm(pCMDIF, 0x30264, 0x08, 0xFFFFFFFF );
        RiptideSendCmd (NULL  , 0x30264, 0x08, 0xFFFFFFFF, (PCMDRET_TYPE)pCMDIF);
    }
//#ifdef ASIC_A1 
//    pGDI->LbusSinks[INTER3_IN] = 14;
//#endif
#endif

/*
#ifdef SUPPORT_SOUNDBLASTER
  pGDI->LbusSinks[DIGITAL_MIXER_IN10] = 14;
  DigitalMixers[DM_SBOUT][DM_NUMBER] = 10;
#endif
*/

// Allocate path for PCM output to line out    
    pLbusPath = &LbusMixerOut[0];
    SampleChannel.ChannelId = OUT_CHANNEL;
    RipTideHwSelectSinglePath(&SampleChannel, DIGITAL_MIXER_OUT0, &pLbusPath);

// Allocate path for PCM output to head phone
    pLbusPath = &LbusMixerOutHnd[0];
    SampleChannel.ChannelId = OUT_CHANNEL;
    RipTideHwSelectSinglePath(&SampleChannel, DIGITAL_MIXER_OUT0, &pLbusPath);
    pHwInfo->LbusPath[OUT_CHANNEL] = pLbusPath;
  }

    RipTideSetFunction(func_table);
/*
  SET_AIACK;
  SET_AIE;*/
/*  SET_EMPUIRQ;*/

  UNSET_AIE;
  INTERRUPT_ACK;
  return pHwInfo;
}


/*************************************************************************
 *
 *  Name:           VOID RipTideHwCompleteInterrupt(VOID)
 *
 *  Parameters:     None
 *
 *  Returns:        None
 *
 *  Description:
 *    Complete interrupt processing. Allow further interrupts.
 *
 *************************************************************************/

static
VOID
RipTideHwCompleteInterrupt(
void *pHwInfo)
{
pCMDIF_AB   pCMDIF = ((PHwInfo)pHwInfo)->tCmdIf;

  INTERRUPT_ACK;
//  SET_AIE;
}


static
void *
RipTideNextChannel(
    void    *pHwInfo,
    void    **pContext
    
)
{
    PChannelInfo pChannelInfo;
    do
    {
        if(((PHwInfo)pHwInfo)->ulCurrentChannel >= CHANNEL_ID_COUNTER)
        {
            ((PHwInfo)pHwInfo)->ulCurrentChannel = 0;
            return NULL;
        }
        pChannelInfo = ((PHwInfo)pHwInfo)->Channels[((PHwInfo)pHwInfo)->ulCurrentChannel++];
    } while (pChannelInfo == NULL);
    *pContext = pChannelInfo->pContext;
    return pChannelInfo;
}

/*
PChannelInfo RipTideNextChannel(PHwInfo phwInfo)
{
    do
    {
        if(phwInfo->ulCurrentChannel >= CHANNEL_ID_COUNTER)
        {
            phwInfo->ulCurrentChannel = 0;
            return NULL;
        }
    }
    while(phwInfo->Channels[phwInfo->ulCurrentChannel++] == NULL);
    return phwInfo->Channels[phwInfo->ulCurrentChannel-1];
}
*/
/*************************************************************************
 *
 *  Name:           VOID RipTideHwQueryInterrupt(VOID)
 *
 *  Parameters:     None
 *
 *  Returns:        None
 *
 *  Description:
 *    Complete interrupt processing. Allow further interrupts.
 *
 *************************************************************************/

static
BOOLEAN
RipTideQueryChannel
(
  PChannelInfo pSGC
)
{

PSGD_DMA pSGD;
BOOLEAN ret = FALSE;
ULONG uStat;
/*  TRACE (("Started QueryStream: %d\n", pSGC->ChannelId));*/
  if(pSGC->ucChannelState == SGC_ACTIVE)
  {

/*    TRACE(("In: %d, Out: %d, Total: %d\n",pSGC->Sgd_In, pSGC->Sgd_Out, pSGC->Total_Out));*/
    if(pSGC->Sgd_Out == pSGC->Sgd_In)
      pSGC->Sgd_Out = pSGC->Total_Out;

    while((pSGC->Sgd_Out != pSGC->Sgd_In) && (!ret))
    {
      pSGD = &(pSGC->sgdList[pSGC->Sgd_Out]);
      uStat = pSGD->dwStat_Ctl;  
      if(pSGD->dwStat_Ctl & ERR_STATUS)
      {
        pSGD->dwStat_Ctl &= ~(ERR_STATUS);
        ret = TRUE;
        TRACE(("ERROR Device: %x\n", pSGC->DMAType));
    //          continue;
        break;
      }
  
      if(pSGD->dwStat_Ctl & EOC_STATUS)
      {
        pSGD->dwStat_Ctl &= ~EOC_STATUS;
        if(pSGD->dwStat_Ctl & IEOC_ENABLE)
        {
    //            TRACE(("IEOC Device: %x\n", pSGC->ucDeviceType));
          ret = TRUE;
    //            continue;
        }
      }

      if(pSGD->dwStat_Ctl & EOS_STATUS)
      {
        pSGD->dwStat_Ctl &= ~EOS_STATUS;
        pSGC->ucChannelState = SGC_PAUSED; //SGC_STOP;

    //          pSGC->Sgd_Out  = pSGC->Total_Out = pSGC->Sgd_In; // = 0;
    //          pSGC->ulIOSize = pSGC->ulIOCounter = 0;
    
    //          if(pSGD->dwStat_Ctl & IEOS_ENABLE)
        {
          ret = TRUE;
    //            TRACE(("Stop Device: %x\n", ulDevice));
    //            DbgPrint ("Stop Device: %x\n", ulDevice);
    //            TRACE(("Int Device: %x\n", ulDevice));
    //            continue;
//          break;
        }
      }

      if(pSGD->dwStat_Ctl & EOB_STATUS)
      {
        pSGC->ulIOCounter += pSGD->dwSegLen;

    //          TRACE(("ulIOCounter = %x\n", pSGC->ulIOCounter));

        pSGD->dwStat_Ctl &= ~(EOB_STATUS);

        if(pSGD->dwStat_Ctl & IEOB_ENABLE)
        {
          ret = TRUE;
    //            TRACE(("IEOB Device: %x\n", pSGC->ucDeviceType));
    //            DbgPrint ("IEOB Device: %x\n", pSGC->ucDeviceType);
    //            continue;
        }

      }
      else
        break;
/*
      dprintf2(("\nSGD[%d] (%x):\nAddr: %x\nLen: %x\nStat: %x\nNext: %x\n",
        pSGC->Sgd_Out,
        pSGD,
        pSGD->dwSegPtrPhys,
        pSGD->dwSegLen,
        uStat,
        pSGD->dwNextLink));
*/
      pSGC->Sgd_Out++;
      pSGC->Sgd_Out &= DESC_MAX_NUM;

    //        TRACE(("Sgd_Out = %d\n" , pSGC->Sgd_Out));
    }
  }
  else
  {
//    TRACE(("Stream is not ACTIVE\n"));
  }

  return ret;
}

static
BOOLEAN
RipTideQueryInterrupt
(
    void *pChannel,
    system_objects *sysObj
)
{
PChannelInfo pSGC = (PChannelInfo)pChannel;
BOOLEAN ret = FALSE;
int DMAFragmentSize = pSGC->DMAFragmentSize;
int NullSample = pSGC->NullSample;
BOOLEAN bCrear = (!pSGC->DMAType) && (!pSGC->Capture);
char *pBuffer = pSGC->DMABuffer;
int i = 0;

    while(RipTideQueryChannel(pSGC) && (i++ < pSGC->DMAFragmentNum))
    {
        ret = TRUE;
	    if(bCrear)
	    {
	        memset(pBuffer + (pSGC->Sgd_Out-1)*DMAFragmentSize, NullSample, DMAFragmentSize);
	    }
	    cancel_timer(sysObj);
        set_timer(sysObj, 200000L);
//        dprintf(("+"));
    }
    return ret;
}

static
int
RipTideQueryBusy
(
    void *pChannelInfo
)
{
ULONG ulLen, ulTail;

    ulLen  = ((PChannelInfo)pChannelInfo)->ulIOCounter;
    ulTail = ((PChannelInfo)pChannelInfo)->ulTail;
/* Check underun [condition */
    if(ulTail + ((PChannelInfo)pChannelInfo)->DMABufferSize < ulLen)
        ulTail = ulLen + 1 - ((PChannelInfo)pChannelInfo)->DMABufferSize;
    return ulLen - ulTail;
}

static
int
RipTideQueryIdle
(
    void *pChannelInfo
)
{
ULONG ulLen, ulHead;

    ulLen  = ((PChannelInfo)pChannelInfo)->ulIOCounter;
    ulHead = ((PChannelInfo)pChannelInfo)->ulHead;
/* Check overrun condition */
    if(ulHead < ulLen)
        ulHead = ulLen;

/*    dprintf(("<RT_HW> Head: %d (%d) Tail: %d\n", pChannelInfo->ulHead, pChannelInfo->DMABufferSize + ulLen, pChannelInfo->ulTail));*/
    return ((PChannelInfo)pChannelInfo)->DMABufferSize - (ulHead - ulLen);    
}


/*************************************************************************
 *
 *  Name:           ULONG
 *                    RipTideHwReadDMACounter(
 *                      IN PWAVE_INFO WaveInfo
 *                      )
 *
 *  Parameters:     WaveInfo - Pointer to wave info structure
 *
 *  Returns:        Quantity of bytes to be transferred
 *
 *  Description:
 *    Get byte position in DM buffer
 *
 *************************************************************************/

static
ULONG
RipTideHwReadDMACounter(
  void *pChannelInfo
)
{
CMDRET_TYPE rptr = {{ (DWORD)0, (DWORD) 0}};
/*
UCHAR       ucChannelId = ((PChannelInfo)pChannelInfo)->ChannelId;
PHwInfo     pHwInfo = ((PChannelInfo)pChannelInfo)->pHwInfo;
UCHAR       ucCDMAorCDMB = ((PChannelInfo)pChannelInfo)->cmdIf;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;
*/
//  dprintf(("<"));
//  RipTideHwEnter( pHwInfo->sysObj );
  rptr.dRetDwords[1] = ((PChannelInfo)pChannelInfo)->ulIOCounter;

#if 0
  if( SEND_GSTS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE )
    if( SEND_GSTS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE )
    {
      dprintf(("ERROR!!! Can't get state\n"));
    }

//  TRACE(("<RT_HW> State = %lx\n", rptr.dRetDwords[ 1 ]));
  rptr.dRetDwords[1] = rptr.dRetDwords[0] = 0;
  
  if(SEND_GPOS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE)
    if(SEND_GPOS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE)
    {
      rptr.dRetDwords[1] = 0;
      dprintf(("ERROR!!! GetPos operation failed\n"));
    }
#endif
//  RipTideHwLeave( pHwInfo->sysObj );
//  dprintf((">"));
  return rptr.dRetDwords[1];
}

/*************************************************************************
 *
 *  Name:           BOOLEAN
 *                    RipTideHwSetWaveFormat(
 *                    IN PWAVE_INFO WaveInfo
 *                    )
 *
 *  Parameters:     WaveInfo - Pointer to wave info structure
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Set the wave format as specified for the device
 *
 *************************************************************************/

static
BOOLEAN
RipTideHwSetWaveFormat(
    void *pChannel
)
{
PChannelInfo    pChannelInfo = (PChannelInfo)pChannel;
char w, ch, order, sig;
UCHAR       ucChannelId = pChannelInfo->ChannelId;
PHwInfo     pHwInfo = pChannelInfo->pHwInfo;
UCHAR       ucCDMAorCDMB = pChannelInfo->cmdIf;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;
//UCHAR       ucBase;
BOOLEAN     ret = TRUE;
ULONG D, M, N;
int i;
ULONG       SamplesPerSec = pChannelInfo->SamplesPerSec; 
int         BitsPerSample = pChannelInfo->BitsPerSample; 
int         Channels      = pChannelInfo->nChannels;

  TRACE(("Started RipTideHwSetWaveFormat: %x\n", ucChannelId));

  RipTideHwEnter( pHwInfo->sysObj );
  
  switch(Channels) {
    case 1:  ch=1; break;
    case 2:  ch=0; break;
    default:
      ret = FALSE;
      TRACE(("Wrong channels: %d\n", Channels));
      goto err_exit;
  }

  order = 0;

  switch(BitsPerSample) {

    case 16:
      w   = 0; /* 16 bit   */
      sig = 0; /* signed   */
      pChannelInfo->NullSample = 0;
      break;

    case 8:
      w   = 1; /* 8 bit    */
      sig = 1; /* unsigned */
      pChannelInfo->NullSample = 127;
      break;

    default:
      ret = FALSE;
      TRACE(("Wrong BitsPerSample: %d\n", BitsPerSample));
      goto err_exit;
  }

  if ( SEND_SETF( ucCDMAorCDMB,
                  ((pChannelInfo->DMIXNum < DM_MAX_NUM)?
                    (pHwInfo->DigitalMixers[pChannelInfo->DMIXNum][DM_NUMBER]) : 0xFF),
                  w, ch, order, sig, ucChannelId ) == FALSE )
    SEND_SETF( ucCDMAorCDMB,
               ((pChannelInfo->DMIXNum < DM_MAX_NUM)?
                 (pHwInfo->DigitalMixers[pChannelInfo->DMIXNum][DM_NUMBER]) : 0xFF),
               w, ch, order, sig, ucChannelId );
/*
  if(pSGC->DMIXNum < DM_MAX_NUM)
  {
    SEND_SDGV(pSGC->ucCDMAorCDMB,
              DigitalMixers[pSGC->DMIXNum][DM_NUMBER],
              DigitalMixers[pSGC->DMIXNum][DM_NUMBER],
              (DigitalMixers[pSGC->DMIXNum][DM_RVALUE] & DMIX_MUTE_FLAG)? 0: DigitalMixers[pSGC->DMIXNum][DM_RVALUE],
              (DigitalMixers[pSGC->DMIXNum][DM_LVALUE] & DMIX_MUTE_FLAG)? 0: DigitalMixers[pSGC->DMIXNum][DM_LVALUE]);
  }
*/

  {
ULONG ulSamplesPerSec;

    if(SamplesPerSec == 48000)
      ulSamplesPerSec = 47999;
    else
      ulSamplesPerSec = SamplesPerSec;

    D  = 48000;
    M  = ulSamplesPerSec * 65536;
    N  = M % D;
    M /= D;
  }

/*  
  if(WaveInfo->SamplesPerSec != 48000)
  {
    D  = 48000;
    M  = WaveInfo->SamplesPerSec * 65536;
    N  = M % D;
    M /= D;
  }
  else
  {
    M = N = 0;
  }
*/
//  TRACE(("SRC: %x\n", SRCNum[ucChannelId][0]));

  for(i = 0; (i < 2) && (pChannelInfo->SRCNum[i] < E2SINK_MAX); i++)
  {
    SEND_SSRC(ucCDMAorCDMB, pChannelInfo->SRCNum[i], D, M, N);
  }

err_exit:

  RipTideHwLeave( pHwInfo->sysObj );

  return ret;
}

/*************************************************************************
 *
 *  Name:           VOID
 *                    RipTideHwAddBuffer(
 *                      PWAVE_INFO WaveInfo,
 *                      PVOID pPhysAddr,
 *                      ULONG ulByteCount)
 *
 *  Parameters:     WaveInfo    - Pointer to wave info structure
 *                  pPhysAddr   - Physical address of a continuous DMA data segment
 *                  ulByteCount - size of DMA data in bytes
 *
 *  Returns:        None
 *
 *  Description:
 *    Add continuous segment of data to S/G descriptor queue
 *************************************************************************/
static
VOID
AddBuffer(
	PChannelInfo pStr,
    PVOID        pPhysAddr,
    ULONG        ulByteCount,
    ULONG        ulByteLimit
)
{
//ULONG            ulByteLimit;
PSGD_DMA         pSGD, pPrevSGD;
ULONG            i;
DWORD            dwStat_Ctl, dwNextLink;

  TRACE(("Started RipTideHwAddBuffer\n"));
  TRACE(("pPhysAddr: %p ulByteCount: %ld ulByteLimit: %ld\n", pPhysAddr, ulByteCount, ulByteLimit));

  if(((pStr->Sgd_In+1) & DESC_MAX_NUM) == pStr->Sgd_Out)
  {
// No room in SGD list
    TRACE(("ERROR! - No room in SGD list: %ld == %ld\n", (pStr->Sgd_In+1) & DESC_MAX_NUM, pStr->Sgd_Out));
    return;
  }

//  ulByteLimit     = ulByteCount; // / 8;
  dwStat_Ctl      = IEOB_ENABLE | IEOS_ENABLE;
  if(pStr->Total_Out == pStr->Sgd_In)
    pStr->Total_Out = pStr->Sgd_In;
  dwNextLink      = pStr->dwLinkHead + sizeof(SGD_DMA)*(pStr->Total_Out);

  pPrevSGD = &(pStr->sgdList[(pStr->Sgd_In + DESC_MAX_NUM) & DESC_MAX_NUM]);

  //DbgPrint ("Ch: %d size: %d\n", StreamId, ulByteCount);

  for(i = 0; (i < ulByteCount) || (ulByteCount == 0); i += ulByteLimit)
  {
    pSGD                  = &(pStr->sgdList[pStr->Sgd_In]);
    pSGD->dwSegPtrPhys    = (DWORD)pPhysAddr + i;
    pSGD->dwSegLen        = (ulByteCount - i >= ulByteLimit)? ulByteLimit : ulByteCount - i;
    pSGD->dwStat_Ctl      = dwStat_Ctl;
    pSGD->dwNextLink      = dwNextLink;


    TRACE(("\nSGD[%ld] (%p):\nAddr: %lx\nLen: %lx\nStat: %lx\nNext: %lx\n",
        i,
        pSGD,
        pSGD->dwSegPtrPhys,
        pSGD->dwSegLen,
        pSGD->dwStat_Ctl,
        pSGD->dwNextLink));


    pPrevSGD->dwNextLink  = pStr->dwLinkHead + sizeof(SGD_DMA)*(pStr->Sgd_In);
    pPrevSGD              = pSGD;

    pStr->Sgd_In++;
    pStr->Sgd_In &= DESC_MAX_NUM;
    if(ulByteCount == 0)
    {
      TRACE(("break\n"));
      break;
    }
//    TRACE(("%d In: %d\n", i, pStr->Sgd_In));
  }
  
  pStr->ulIOSize += ulByteCount;

  TRACE(("ulIOSize = 0x%lx\n", pStr->ulIOSize));
  TRACE(("In: %ld, Out: %ld, Total: %ld\n",pStr->Sgd_In, pStr->Sgd_Out, pStr->Total_Out));

  return;
}  

static
void
RipTideCalcFragmentSize(
	PChannelInfo pChannelInfo
	)
{
            pChannelInfo->DMAFragmentSize =
                pChannelInfo->nChannels * 			/* number of channels */
                pChannelInfo->SamplesPerSec *       /* sample rate */
                pChannelInfo->BitsPerSample / (8 * NOTIFICATION_TIME) ;    /* bits per sample */
  	    dprintf2(("<RT_HW> Fragment size:\n%d *\n%ld *\n%d / (8 * %d) = %d\n",pChannelInfo->nChannels,pChannelInfo->SamplesPerSec,pChannelInfo->BitsPerSample,NOTIFICATION_TIME,pChannelInfo->DMAFragmentSize));
}

static
ULONG RipTidePutToBuffer(
	PChannelInfo pChannelInfo,
    PVOID        pData,
    ULONG        ulByteCount
)
{
ULONG ulHead, ulTail, ulSize = 0, ulLen, i;

    if(pChannelInfo->DMABuffer == NULL)
    {
        if(pChannelInfo->DMAFragmentSize == 0)        
            RipTideCalcFragmentSize(pChannelInfo);

        if(pChannelInfo->DMAFragmentSize <= 0)
            pChannelInfo->DMAFragmentSize = 256;

/*
        if(pChannelInfo->DMAFragmentSize > CYCLIC_DMA_BUFFER_SIZE)
            pChannelInfo->DMAFragmentSize = CYCLIC_DMA_BUFFER_SIZE;
*/
        if(pChannelInfo->DMAFragmentMax < 2)
        {
            pChannelInfo->DMAFragmentSize /= 2;
            pChannelInfo->DMAFragmentMax = 2;
        }
            
        pChannelInfo->DMAFragmentNum = (pChannelInfo->DMAFragmentMax > (DESC_MAX_MASK+1))? (DESC_MAX_MASK+1): pChannelInfo->DMAFragmentMax;

        ulLen = CYCLIC_DMA_BUFFER_SIZE/pChannelInfo->DMAFragmentSize;

        if(ulLen < 2)
        {
            ulLen = 2;
        }

        if(ulLen > (DESC_MAX_MASK+1))
        {
            ulLen = (DESC_MAX_MASK+1);
        }

        if(pChannelInfo->DMAFragmentNum > ulLen)
        {
            pChannelInfo->DMAFragmentNum = ulLen;
        }

        pChannelInfo->DMABufferSize = pChannelInfo->DMAFragmentSize * pChannelInfo->DMAFragmentNum;

        dprintf2(("<RIPTIDE> Allocate new DMA: %ld (%d*%d)\n", pChannelInfo->DMABufferSize,pChannelInfo->DMAFragmentSize,pChannelInfo->DMAFragmentNum));
        if((pChannelInfo->DMABuffer = allocate_virtual(pChannelInfo->DMABufferSize)) == NULL)
        {
            dprintf(("<RIPTIDE> ERROR!!! NO memory for cyclic DMA buffer: %ld(%d)\n",pChannelInfo->DMABufferSize, CYCLIC_DMA_BUFFER_SIZE));
            return -1;
        }

        dprintf2(("<RIPTIDE> Allocate DMA descriptors: %d\n", sizeof( SGD_DMA )*( DESC_MAX_MASK + 1)));
        if((pChannelInfo->sgdList = allocate_dmabuf(sizeof( SGD_DMA )*( DESC_MAX_MASK + 1))))
        {
            pChannelInfo->dwLinkHead = virt2bus(pChannelInfo->sgdList);
        }
        else
        {
            dprintf(("<RIPTIDE> ERROR!!! no memory for DMA decsriptors: %d\n", sizeof( SGD_DMA )*( DESC_MAX_MASK + 1)));
            return -1;
	    }

        for(i = 0; i < pChannelInfo->DMABufferSize; i += PAGE_SIZE)
        {
            dprintf2(("<RT_HW> virt: %p phis: %lx\n",pChannelInfo->DMABuffer+i, kvirt_to_bus((ULONG)(pChannelInfo->DMABuffer+i))));
            AddBuffer(
                pChannelInfo,
                (PVOID)kvirt_to_bus((ULONG)(pChannelInfo->DMABuffer+i)),
                PAGE_SIZE,
                pChannelInfo->DMAFragmentSize);
        }

        dprintf2(("<RT_HW> size: %ld\n", pChannelInfo->DMABufferSize));
	    if(RipTideHwSelectPath(	pChannelInfo ))
	    {
		    if(!RipTideHwSetWaveFormat(pChannelInfo))
		    {
        	    dprintf(("<RT_HW> ERROR!!! Can't set format\n"));
                return -1;
		    }
        }
        else
        {
    	    dprintf(("<RT_HW> ERROR!!! No LBUS resources for channel\n"));
            return -1;
        }
    }

/*    ulLen = RipTideHwReadDMACounter(pChannelInfo);*/
    ulLen = pChannelInfo->ulIOCounter;

    dprintf2(("<RT_HW> Current: %ld\n", ulLen));

    if(pChannelInfo->Capture)
    {
/* Capture */
        pChannelInfo->ulHead = ulLen;
/* Check underun [condition */
        if(pChannelInfo->ulTail+pChannelInfo->DMABufferSize < ulLen)
            pChannelInfo->ulTail = ulLen + 1 - pChannelInfo->DMABufferSize;

        ulLen = ulLen - pChannelInfo->ulTail;

        dprintf2(("<RT_HW> Req: %ld Ava: %ld\n", ulByteCount , ulLen));
        if(ulByteCount > ulLen)
            ulByteCount = ulLen;
/*
        {
            int i;
            for(i = 0; i< pChannelInfo->DMABufferSize; i++)
              pChannelInfo->DMABuffer[i] = (char)((i%512) < 256)? i: 256 - (char)i;
        }
*/
        if(ulByteCount)
        {
	        ulHead  = pChannelInfo->ulHead % pChannelInfo->DMABufferSize;
	        ulTail  = pChannelInfo->ulTail % pChannelInfo->DMABufferSize;

	        if(ulTail > ulHead)
	        {
    	        ulLen = pChannelInfo->DMABufferSize - ulTail;
		        if(ulByteCount > ulLen)
		        {
			        dprintf2(("<RT_HW> 1 Copy: %ld from: %p to: %p\n", ulLen, pChannelInfo->DMABuffer+ulTail, pData));
			        copy_from_dma( pData, pChannelInfo->DMABuffer+ulTail,  ulLen);
			        ulByteCount -= ulLen;
			        pData       += ulLen;
			        ulSize      += ulLen;
			        ulTail       = 0;		        
	            }
	        }
	        
	        if(ulByteCount)
	        {
	            ulLen = ulHead - ulTail;
	            if(ulByteCount < ulLen)
	                ulLen = ulByteCount;
                dprintf2(("<RT_HW> 2 Copy: %ld from: %p to: %p\n", ulLen, pChannelInfo->DMABuffer+ulTail, pData));
	            copy_from_dma( pData, pChannelInfo->DMABuffer+ulTail,  ulLen);
	            ulSize += ulLen;
	        }
	        pChannelInfo->ulTail += ulSize;
	    }
    }
    else
    {
/* Playback */
        pChannelInfo->ulTail = ulLen;
/* Check overrun condition */
        if(pChannelInfo->ulHead < ulLen)
            pChannelInfo->ulHead = ulLen;

/*        dprintf(("<RT_HW> Head: %d (%d) Tail: %d\n", pChannelInfo->ulHead, pChannelInfo->DMABufferSize + ulLen, pChannelInfo->ulTail));*/

        ulLen = pChannelInfo->DMABufferSize - (pChannelInfo->ulHead - ulLen);
/*
        if(ulLen > pChannelInfo->DMABufferSize)
        {
            dprintf(("<RT_HW> %d > %d\n", ulLen, pChannelInfo->DMABufferSize));
            ulLen = 0;
        }
        else
        {
            if(!ulLen)
                ulLen = pChannelInfo->DMABufferSize;
        }
*/        
/*        dprintf(("<RT_HW> Req: %d Ava: %d\n", ulByteCount, ulLen));*/
        if(ulByteCount > ulLen)
            ulByteCount = ulLen;

        if(ulByteCount)
        {
	        ulHead  = pChannelInfo->ulHead % pChannelInfo->DMABufferSize;
	        ulTail  = pChannelInfo->ulTail % pChannelInfo->DMABufferSize;
/*          dprintf2(("<RT_HW>---------------- Head: %d Tail: %d\n", ulHead, ulTail));*/
	        if(ulHead > ulTail)
	        {
  	            ulLen = pChannelInfo->DMABufferSize - ulHead;
	            if(ulByteCount > ulLen)
	            {
/*			        dprintf2(("<RT_HW>--------------------------- 1 ----------------------------------------- Copy: %d from: %p to: %p\n", ulLen, pData, pChannelInfo->DMABuffer+ulHead));*/
			        copy_to_dma( pChannelInfo->DMABuffer+ulHead, pData, ulLen);
			        ulByteCount -= ulLen;
			        pData += ulLen;
			        ulSize += ulLen;
			        ulHead = 0;		        
	            }
	        }
	        
	        if(ulByteCount)
	        {
                ulLen = ulByteCount;
/*	            dprintf(("<RT_HW> 2 Copy: %d from: %p to: %p\n", ulLen, pData, pChannelInfo->DMABuffer+ulHead));*/
	            copy_to_dma( pChannelInfo->DMABuffer+ulHead, pData, ulLen);
	            ulSize += ulLen;
	        }
	        pChannelInfo->ulHead += ulSize;
        }
    }

    dprintf2(("<RT_HW> Copied size: %ld, Head: %ld, Tail: %ld\n", ulSize, pChannelInfo->ulHead, pChannelInfo->ulTail));
    return ulSize; 
}

static
size_t         /* return size of data is put to DMA buffer */
RipTideAddBuffer(
    void        *pChannelInfo,
    PVOID        pData,
    ULONG        ulByteCount
)
{
ULONG ulSize;

    if(!((PChannelInfo)pChannelInfo)->DMAType)
    {
        ulSize = RipTidePutToBuffer((PChannelInfo)pChannelInfo, pData, ulByteCount);
    }
    else
    {
        dprintf(("<RIPTIDE> ERROR!!! Scatter/gather is not supported yet\n"));
        ulSize = ulByteCount;
    }
    return ulSize;
}



/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwStartStream(PWAVE_INFO WaveInfo)
 *
 *  Parameters:     WaveInfo - Pointer to wave info structure
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *  Start the DMA
 *************************************************************************/

static
BOOLEAN
RipTideHwStartDMA(
    void *pChannel
)
{
PChannelInfo pChannelInfo = (PChannelInfo)pChannel;
BOOLEAN     ret = TRUE;
UCHAR       ucDMIXNum, ucCDMAorCDMB;
CMDRET_TYPE rptr = {{ (DWORD)0, (DWORD) 0}};
UCHAR       ucChannelId = pChannelInfo->ChannelId;
pCMDIF_AB   pCMDIF = pChannelInfo->pHwInfo->tCmdIf;
DWORD       dwLinkHead = pChannelInfo->dwLinkHead;

  if(pChannelInfo->ucChannelState == SGC_ACTIVE)
      return TRUE;

  TRACE(("<RT_HW>Started RipTideHwStartDMA: %x\n", ucChannelId));
  ucCDMAorCDMB = pChannelInfo->cmdIf;

  RipTideHwEnter( pChannelInfo->pHwInfo->sysObj );

  if(SEND_GPOS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE)
    SEND_GPOS(ucCDMAorCDMB, 0, ucChannelId, &rptr);


  TRACE(("<RT_HW> Stream position: %lx %lx\n", rptr.dRetDwords[0], rptr.dRetDwords[1]));
/*
  if( SEND_GSTS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE )
    if( SEND_GSTS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE )
    {
      TRACE(("ERROR!!! Can't get state\n"));
      ret = FALSE;
    }

  TRACE(("State = %x\n", rptr.dRetDwords[ 1 ]));

  if(rptr.dRetDwords[ 1 ] != 2)
    SEND_KSTR(ucCDMAorCDMB, ucChannelId);
*/


  TRACE(("<RT_HW>Start DMA: 0x%lx. Channel: %d State: %x\n", dwLinkHead, ucChannelId, READ_AUDIO_CONTROL));
  if ( SEND_SSTR(ucCDMAorCDMB, ucChannelId, dwLinkHead) == FALSE )
    if(SEND_SSTR(ucCDMAorCDMB, ucChannelId, dwLinkHead) == FALSE)
    {
      dprintf(("<RT_HW> ERROR!!! Start DMA is failed\n"));
      ret = FALSE;
    }

  if(ret == TRUE)
  {
    SET_AIE;      /* Enable interrupt */
    pChannelInfo->ucChannelState = SGC_ACTIVE;
  }

/*
  while(rptr.dRetDwords[1] == 0)
  {
int i;
DWORD dWork;

  if ( SEND_SSTR(ucCDMAorCDMB, ucChannelId, dwLinkHead) == FALSE )
    if(SEND_SSTR(ucCDMAorCDMB, ucChannelId, dwLinkHead) == FALSE)
    {
      TRACE(("ERROR ! Start DMA is failed\n"));
      ret = FALSE;
      break;
    }
    i = 0;
    do
    {
      dWork = rptr.dRetDwords[1];

      SEND_GPOS(ucCDMAorCDMB, 0, ucChannelId, &rptr);

      if(i++ > 10)
      {
        SEND_KSTR(ucCDMAorCDMB, ucChannelId);
        TRACE(("Stop/Start\n"));
        break;
      }
    } while (rptr.dRetDwords[1] == dWork);

    TRACE(("Wait loop: %d (%d)\n", i, rptr.dRetDwords[1]));
  }
*/  
  if((ret == TRUE) && (ucDMIXNum = pChannelInfo->DMIXNum) < DM_MAX_NUM)
  {
    ULONG (*DigitalMixers)[DM_MAX_NUM][DM_RVALUE+1] = &pChannelInfo->pHwInfo->DigitalMixers;

    (*DigitalMixers)[ucDMIXNum][DM_NUMBER] &= ~DMIX_STOP_FLAG;
    SEND_SDGV(ucCDMAorCDMB,
              (*DigitalMixers)[ucDMIXNum][DM_NUMBER],
              (*DigitalMixers)[ucDMIXNum][DM_NUMBER],
              ((*DigitalMixers)[ucDMIXNum][DM_RVALUE] & DMIX_MUTE_FLAG)? 0: (*DigitalMixers)[ucDMIXNum][DM_RVALUE],
              ((*DigitalMixers)[ucDMIXNum][DM_LVALUE] & DMIX_MUTE_FLAG)? 0: (*DigitalMixers)[ucDMIXNum][DM_LVALUE]);
  }

  RipTideHwLeave( pChannelInfo->pHwInfo->sysObj );
  return ret;
}

/*************************************************************************
 *
 *  Name:           BOOLEAN
 *                    RipTideHwStopDMA(
 *                    PWAVE_INFO WaveInfo
 *                    )
 *
 *
 *  Parameters:     WaveInfo - Pointer to wave info structure
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *  Stop the DMA
 *
 *  Whoever calls this routine had better first make sure that no
 *  Dpc routine is going to run in parallel!
 *
 *  For wave input the caller MUST NOT own the spin lock because
 *  we're going to do waits in here
 *
 *  Note we're also NOT doing the windows hack for wave out
 *  when the sampling rate is too low because that would
 *  involve doing waits in a dpc or some other complex design.
 *
 *************************************************************************/

static
BOOLEAN
RipTideHwStopDMA(
    void *pChannel
)
{
PChannelInfo pChannelInfo = (PChannelInfo)pChannel;
UCHAR   ucCDMAorCDMB;
BOOLEAN ret = TRUE;
UCHAR   ucChannelId = pChannelInfo->ChannelId;
PHwInfo	pHwInfo = pChannelInfo->pHwInfo;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;

  TRACE(("Started RipTideHwStopDMA: %x\n", ucChannelId));

  ucCDMAorCDMB = pChannelInfo->cmdIf;

  RipTideHwEnter( pHwInfo->sysObj );

  {
CMDRET_TYPE rptr  = {{ (DWORD)0, (DWORD) 0}};
DWORD       dWork = 0xffff;
int i = 0;
UCHAR ucDMIXNum;

    if((ucDMIXNum = pChannelInfo->DMIXNum) < DM_MAX_NUM)
    {
      SEND_SDGV(ucCDMAorCDMB,
                pHwInfo->DigitalMixers[ucDMIXNum][DM_NUMBER],
                pHwInfo->DigitalMixers[ucDMIXNum][DM_NUMBER],
                0,
                0);
      pHwInfo->DigitalMixers[ucDMIXNum][DM_NUMBER] |= DMIX_STOP_FLAG;
    }

/*
    if( SEND_GSTS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE )
      if( SEND_GSTS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE )
      {
        TRACE(("ERROR!!! Can't get state\n"));
        ret = FALSE;
      }

    dWork = rptr.dRetDwords[ 1 ];

    while(dWork == rptr.dRetDwords[ 1 ])
    {
      if(SEND_KSTR(ucCDMAorCDMB, ucChannelId) == FALSE)
        if(SEND_KSTR(ucCDMAorCDMB, ucChannelId) == FALSE)
        {
          TRACE(("ERROR!!! Can't stop DMA\n"));
          ret = FALSE;
          break;
        }

      KeStallExecutionProcessor(100);

      if( SEND_GSTS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE )
        if( SEND_GSTS(ucCDMAorCDMB, 0, ucChannelId, &rptr) == FALSE )
        {
          TRACE(("ERROR!!! Can't get state\n"));
          ret = FALSE;
          break;
        }

      if(i++ > 100)
      {
        TRACE(("ERROR!!! DMA can't be stoped \n"));
        ret = FALSE;
        break;
      }
    }
*/
      i = 0;

    if(SEND_KSTR(ucCDMAorCDMB, ucChannelId) == FALSE)
      if(SEND_KSTR(ucCDMAorCDMB, ucChannelId) == FALSE)
      {
        TRACE(("ERROR!!! Can't stop DMA\n"));
        ret = FALSE;
      }

    pChannelInfo->ucChannelState = SGC_STOP;

    do
    {
      dWork = rptr.dRetDwords[1];

      SEND_GPOS(ucCDMAorCDMB, 0, ucChannelId, &rptr);

      TRACE(("Wait loop: %d (%ld, %ld)\n", i++, dWork, rptr.dRetDwords[1]));

    } while ((rptr.dRetDwords[1] != dWork));
  }
  RipTideHwLeave( pHwInfo->sysObj );

  return TRUE;
}

#ifdef RT_UNUSED
static
BOOLEAN
RipTideHwPauseDMA(
  PChannelInfo pChannelInfo
)
{
UCHAR ucCDMAorCDMB;
UCHAR   ucChannelId = pChannelInfo->ChannelId;
PHwInfo	pHwInfo = pChannelInfo->pHwInfo;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;

  TRACE(("Started RipTideHwPauseDMA: %x\n", ucChannelId));

  ucCDMAorCDMB = pChannelInfo->cmdIf;

  RipTideHwEnter( pHwInfo->sysObj );

  SEND_PSTR(ucCDMAorCDMB, ucChannelId);

  RipTideHwLeave( pHwInfo->sysObj );

  return TRUE;
}
#endif


#ifdef SUPPORT_I2S

/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwAnableI2S(
 *                    PSGC  pSGC,
 *                    WORD  usChannel)
 *
 *  Parameters:     pSGC      - Pointer to context structure
 *                  usChannel - I2S channel number
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Allocate resources for I2S and anable it
 *
 *************************************************************************/

static
BOOLEAN RipTideHwAnableI2S(
  PHwInfo	pHwInfo,
  WORD  usChannel)
{
PChannelInfo SampleSGC;
pCMDIF_AB    pCMDIF = pHwInfo->tCmdIf;

  TRACE(("RipTideHwAnableI2S started: Channel = %d\n", usChannel));
  SampleSGC.pHwInfo     = pHwInfo;
  SampleSGC.ucChannelId = I2S_CHANNEL + usChannel;
  SampleSGC.LbusSource  = I2S_SOURCE + usChannel;
  SampleSGC.LbusPath    = &(LbusAudioStereoSrcPlaybackPath[0]);
  SampleSGC.DMIXNum     = DM_I2S_0 + usChannel;

  if(RipTideHwSelectPath(&SampleSGC) == TRUE)
  {
    SampleSGC.LbusSource = ASRC0 + usChannel;
    SampleSGC.LbusPath   = &(LbusAudioI2SRecordPath[0]);

    SEND_SSRC(CMDA,
              SampleSGC.SRCNum[0],
              48000, 
              ((ULONG)I2S_RATE * 65536) / 48000,
              ((ULONG)I2S_RATE * 65536) % 48000);

    RipTideHwSelectPath(&SampleSGC);
   /* Cmd, L2A, A2L, Inter/Decim, Merger, Splitter, Mixer */ 
    SEND_LSEL(CMDA,
            I2S_CMD0 + usChannel,
            ((SampleSGC.pLbusSinks[I2S_SINK] == (I2S_CHANNEL + usChannel))? I2S_SINK: (I2S_SINK+1)),
            I2S_SOURCE + usChannel,
            SampleSGC.SRCNum[0] - INTER0_IN,
            0, 
            0,
            DigitalMixers[DM_I2S_0 + usChannel]);

    SEND_SI2S( CMDA, 
      ((pHwInfo->DigitalMixers[DM_I2S_0][DM_NUMBER] < 0xFF)? 0x1: 0 ) |
      ((pHwInfo->DigitalMixers[DM_I2S_1][DM_NUMBER] < 0xFF)? 0x4: 0 ));

    return TRUE;
  }
  return FALSE;
}

/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwDisableI2S(
 *                    PSGC  pSGC,
 *                    WORD  usChannel)
 *
 *  Parameters:     pSGC      - Pointer to context structure
 *                  usChannel - I2S channel number
 *
 *  Returns:        None
 *
 *  Description:
 *    Disallocate resources for I2S and disable it
 *
 *************************************************************************/

static
VOID RipTideHwDisableI2S(
  PChannelInfo pChannelInfo,
  WORD  usChannel)
{
ULONG i;
SGC SampleSGC = pChannelInfo->Str;
PHwInfo	pHwInfo = pChannelInfo->pHwInfo;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;

  SampleSGC.ucChannelId = I2S_CHANNEL + usChannel;
  SampleSGC.LbusSource  = I2S_SOURCE + usChannel;
  SampleSGC.LbusPath    = &(LbusAudioStereoSrcPlaybackPath[0]);
  SampleSGC.DMIXNum     = DM_I2S_0 + usChannel;
  RipTideHwFreePath(&SampleSGC);

  SampleSGC.LbusSource = ASRC0 + usChannel;
  SampleSGC.LbusPath   = &(LbusAudioI2SRecordPath[0]);
  RipTideHwFreePath(&SampleSGC);

  SEND_LSEL( CMDA, I2S_CMD0 + usChannel, 0x10, 0x10, 0, 0, 0, pHwInfo->DigitalMixers[DM_I2S_0 + usChannel]);

  SEND_SI2S( CMDA, 
      ((pHwInfo->DigitalMixers[DM_I2S_0][DM_NUMBER] < 0xFF)? 0x1: 0 ) |
      ((pHwInfo->DigitalMixers[DM_I2S_1][DM_NUMBER] < 0xFF)? 0x4: 0 ));
}

#endif

#ifdef SUPPORT_MIXER
static
VOID
RipTideHwScheduleMixer(
  PHwInfo     pHwInfo,
  WORD        wId,
  WORD        *pvalue,
  UCHAR       uOper);

/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwGetMixer(
 *                    PMIXER_INFO MixerInfo,
 *                    WORD wId,
 *                    WORD *pValue)
 *
 *
 *  Parameters:     MixerInfo - Pointer to mixer info structure
 *                  wId       - Mixer control Id
 *                  pValue    - Mixer value aray (0 - left channel value,
 *                                                1 - right channel value)
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Get and adjust mixer values
 *
 *  Note:
 *    Volume values are returned in 0 - 0xFFFF range
 *    Mask values are returned as 0 or  0xFFFF value
 *************************************************************************/

static
BOOLEAN
RipTideHwGetMixer(
  void      *pInfo,
  WORD      wId,
  WORD      *pValue)
{
PHwInfo   pHwInfo = (PHwInfo)pInfo;
CMDRET_TYPE rptr = {{ (DWORD)0, (DWORD) 0}};
WORD limit, reg;
int ret = TRUE; 
WORD wMask;
int iShift;
UCHAR ucCDMAorCDMB = CMDA;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;

  TRACE(("Started RipTideHwGetMixer\n"));

  if(wId >= MAX_MIXER_CONTROLS)
  {
    TRACE(("Mixer control Id is out of range: %x\n", wId));
    ret =  FALSE;
  }
  else
  {
    RipTideHwEnter(pHwInfo->sysObj);

    reg   = gauControlMasks[wId][0];
    limit = gauControlMasks[wId][1];

    TRACE(("Get reg %x (%x)\n", reg, limit));

    switch(limit)
    {
      case 0x0F:
        iShift = 12;
        wMask  = 0x0FFF;
        break;
      case 0x1F:
        iShift = 11;
        wMask  = 0x07FF;
        break;
      case 0x7FFF:
        iShift = 1;
        wMask  = 0x0001;
        break;
      default:
        iShift = 0;
        wMask  = 0x0;
    }

    if(reg < MAXACREG)
    {
// Analog mixer
      if( SEND_RACR(ucCDMAorCDMB, reg, &rptr) != FALSE)
      {
// Adjust value to mixers limit

        if(iShift >= 8)
        {
          pValue[1] = rptr.wRetWords[1] & limit;
          pValue[0] = ( rptr.wRetWords[1] >> 8 ) & limit;
        }
        else
        {
          pValue[1] = 
          pValue[0] = rptr.wRetWords[1] & limit;
        }

        if((iShift) && (reg != 0x1C))
        {
          pValue[0] = limit - pValue[0];
          pValue[1] = limit - pValue[1];
        }

        TRACE(("Get value %x %x (%x)\n", pValue[0], pValue[1], rptr.wRetWords[1]));
      }
      else
      {
        TRACE(("SEND_RACR error\n"));
        ret =  FALSE;
      }
    }
    else
    {
      reg &= 0x7f;

      if(reg < DM_MAX_NUM)
      {
        pValue[0] = (WORD)pHwInfo->DigitalMixers[reg][DM_LVALUE];
        pValue[1] = (WORD)pHwInfo->DigitalMixers[reg][DM_RVALUE];
      }
      else
      {
        pValue[0] = pValue[1] = 0;
      }
    }

    RipTideHwLeave(pHwInfo->sysObj);

    if(pValue[0])
      pValue[0] = (pValue[0] << iShift) | wMask;

    if(pValue[1])
      pValue[1] = (pValue[1] << iShift) | wMask;

    TRACE(("value %x %x\n", pValue[0], pValue[1]));
  }
  return ret;
}

/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwSetReg(
 *                    UCHAR ucCDMAorCDMB,
 *                    WORD  reg,
 *                    WORD  limit,
 *                    WORD  *pvalue)
 *
 *  Parameters:     ucCDMAorCDMB - command interface id
 *                  reg          - register id
 *                  limit        - register value mask
 *                  pvalue       - Mixer value array (0 - left channel value,
 *                                                    1 - right channel value)
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *    Set AC register value
 *
 *************************************************************************/

static
BOOLEAN RipTideHwSetReg(
  pCMDIF_AB   pCMDIF,
  WORD  reg,
  WORD  limit,
  WORD  *pvalue)
{
CMDRET_TYPE rptr = {{(DWORD)0, (DWORD) 0}};
WORD value;
UCHAR ucCDMAorCDMB = CMDA;
int i;

  TRACE(("Started RipTideHwSetReg:\nreg: %x limit: %x vl: %x vr: %x\n", reg, limit, pvalue[0], pvalue[1]));

  for(i=0; i< 2; i++)
  {
    if( SEND_RACR(ucCDMAorCDMB, reg, &rptr) == FALSE)
    {
      TRACE(("ERROR! Can't read reg value\n"));
      return FALSE;
    }

    TRACE(("Get value %x %x %x %x\n", rptr.wRetWords[0], rptr.wRetWords[1], rptr.wRetWords[2], rptr.wRetWords[3]));
  }

  if(limit <= 0xff)
  {
    value = ((pvalue[1] & limit) | ((pvalue[0] & limit) << 8));
    
    limit = limit | (limit << 8);

    value |= (rptr.wRetWords[1] & (~limit));
  }
  else
  {
    value = (rptr.wRetWords[1] & (~limit) ) | 
            (pvalue[0] & limit);
  }
  
  i = 0;
  do
  {
    TRACE(("Set value %x\n", value));
    if( SEND_SACR(ucCDMAorCDMB, value, reg) == FALSE)
      return FALSE;
    if( SEND_RACR(ucCDMAorCDMB, reg, &rptr) == FALSE)
      return FALSE;

    TRACE(("Get value %x %x %x %x\n", rptr.wRetWords[0], rptr.wRetWords[1], rptr.wRetWords[2], rptr.wRetWords[3]));
  } while ((rptr.wRetWords[1] != value) && (i++ < 10));

  return TRUE;
}

#define MIX_SET     0
#define MIX_MUTE    1
#define MIX_UNMUTE  2

/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwSetMixer(
 *                    PMIXER_INFO MixerInfo,
 *                    WORD        wId,
 *                    WORD        *pValue,
 *                    BOOLEAN     bMute)
 *
 *
 *
 *  Parameters:     MixerInfo - Pointer to mixer info structure
 *                  wId       - Mixer control Id
 *                  pValue    - Mixer value aray (0 - left channel value,
 *                                                1 - right channel value)
 *                              range: 0-0xffff
 *                  bMute     - !=0 - if mixer is muted; 0 - otherwise
 *
 *  Returns:        TRUE  - if successful
 *                  FALSE - otherwise
 *
 *  Description:
 *  Adjust and set mixers values and control AC registers 
 *
 *************************************************************************/

static
BOOLEAN
RipTideHwSetMixer(
  void        *pInfo,
  WORD        wId,
  WORD        *pValue
)
{
PHwInfo   pHwInfo = (PHwInfo)pInfo;
WORD limit, lim, reg, mfl;
int ret = TRUE; 
ULONG       (*DigitalMixers)[DM_MAX_NUM][DM_RVALUE+1] = &pHwInfo->DigitalMixers;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;

  TRACE(("Started RipTideHwSetMixer\n"));
  if(wId >= MAX_MIXER_CONTROLS)
  {
    TRACE(("Mixer control Id is out of range: %x\n", wId));
    ret = FALSE;
  }
  else
  {
int iShift;

    RipTideHwEnter(pHwInfo->sysObj);
    reg   = gauControlMasks[wId][0];
    limit = gauControlMasks[wId][1];
    mfl = (limit < 0xFFFF && reg < MAXACREG)?1:0;
  
    lim   = (limit==AC98MAX? 0x1F: limit);

// Adjust value to mixers limit
    switch(lim)
    {
      case 0x0F:
        iShift = 12;
        break;
      case 0x1F:
        iShift = 11;
        break;
      case 0x7FFF:
        iShift = 1;
        break;
      default:
        iShift = 0;
    }

    if(mfl)
    {
// Analog mixer
WORD lim1 = 0, reg1, pValue1[2], wId1;
BOOLEAN bs, bs1;
UCHAR uOper;

      bs = bs1 = FALSE;
      uOper = MIX_UNMUTE;

      if((iShift) && (wId != MXMUX_SELECT))
      {
        pValue[0] = (pValue[0] >> iShift) & lim;
        pValue[1] = (pValue[1] >> iShift) & lim;

        if(reg != 0x1C)
        {
          pValue[0] = lim - pValue[0];
          pValue[1] = lim - pValue[1];
        }
      }

      reg1 = MAXACREG;
      wId1 = MAX_MIXER_CONTROLS;
      switch(wId)
      {
        case MXMIX_MASTER:
// Mixer Master also controls headphone volume
          TRACE(("MASTER_VOLUME\n"));
          reg1 = RWAM_HDPHN;
          lim1 = lim;
          wId1 = MXMIX_HDPH;
          pValue1[0] = pValue[0];
          pValue1[1] = pValue[1];
        case PEAK_WAVE:
          bs = bs1 = TRUE;
          uOper = MIX_SET;
          break;
        case MXMUTE_MASTER:
// Mixer Master also controls headphone volume
          TRACE(("MASTER_MUTE\n"));
          if(pValue[0])
            uOper = MIX_MUTE;
          else
            uOper = MIX_UNMUTE;
          wId  = MXMIX_MASTER;
          wId1 = MXMIX_HDPH;
          pValue[0]  = 
          pValue[1]  = 0x1f;
          pValue1[0] = pValue[0];
          pValue1[1] = pValue[1];
          bs = bs1 = TRUE;
          break;
        case PEAK_MUX:
          if(pValue[0])
            uOper = MIX_MUTE;
          else
            uOper = MIX_UNMUTE;
          wId = PEAK_WAVE;
          pValue[0]  = 
          pValue[1]  = 0x1f;
          bs = bs1 = TRUE;
          break;
        case MXMUX_MASTER:
// Recording Mic matches other record channels
          reg1 = RWAM_RECDMIC;
          lim1 = lim;
          pValue1[0] = pValue[0];
          pValue1[1] = pValue[1];
          break;

        case MXMIX_3D:
          if(pValue[0] != 0)
          {
            pValue[0] = pValue[1] = 0xffff & lim;
            lim  |= 0x1000;

            TRACE(("Set MXMIX_3D\n"));
            reg1 = RWAM_3D_CTRL;
            lim1 = RWAM_3D_MASK;
            pValue1[0] = 
            pValue1[1] = (WORD)pHwInfo->Coeff3D;
          }
          break;
        case MXMIX_BASS:
          if(pValue[0] != 0)
          {
            pValue[0] = pValue[1] = 0xffff & lim;
            lim  |= 0x2000;

            TRACE(("Set MXMIX_BASS\n"));
            reg1 = RWAM_3D_CTRL;
            lim1 = RWAM_3D_MASK;
            pValue1[0] = 
            pValue1[1] = (WORD)pHwInfo->CoeffBass;
          }
          break;

        case MXMUX_SELECT:
            dprintf2(("<RT_HW> MUX_SELECT: %x %x\n", pValue[0], pValue[1]));
#if 0
          {
int i;
CMDRET_TYPE rptr = { (DWORD)0, (DWORD) 0};
            for(i=0; i<0x2b; i+=2)
            {
              if( SEND_RACR(ucCDMAorCDMB, i, &rptr) != FALSE)
              {
                TRACE(("reg: %2x: %4x\n", i, rptr.wRetWords[1]));
              }
            }
          }
#endif
/*            reg1 = 0x6A;
            lim1 = 0x0F;
            pValue1[0] = 
            pValue1[1] = 0x08;
            */
          break;

        default:
          break;
      }

      if(bs)
      {
        TRACE(("SMOOTH\n"));
        RipTideHwScheduleMixer(pHwInfo, wId, pValue, uOper);
        if(wId1 < MAX_MIXER_CONTROLS)
          RipTideHwScheduleMixer(pHwInfo, wId1, pValue1, uOper);
      }
      else
      {
        TRACE(("NON SMOOTH\n"));
        if(RipTideHwSetReg(pCMDIF, reg, lim, pValue) == FALSE)
          ret = FALSE;
        else
        if(reg1 < MAXACREG)
          if(RipTideHwSetReg(pCMDIF, reg1, lim1, &pValue1[0]) == FALSE)
          {
            TRACE(("Set reg error\n"));
            ret =  FALSE;
          }
      }
    }
    else
    {
// digital mixer
WORD Value[2], dreg, OldValue[2];
ULONG NewValue[2];

      reg &= 0x7f;

      if(reg < DM_MAX_NUM)
      {
        dreg = (WORD)(*DigitalMixers)[reg][DM_NUMBER];

            OldValue[0] = (WORD)(*DigitalMixers)[reg][DM_LVALUE] & DMIX_LIMIT;
            OldValue[1] = (WORD)(*DigitalMixers)[reg][DM_RVALUE] & DMIX_LIMIT;

        if(limit == DMIX_MUTE) 
        {
          dprintf2(("<RT_HW> D_MUTE: %d %d\n", reg, pValue[0]));
          if(pValue[0] == 0)
          {
            Value[0] = (WORD)(NewValue[0] = (*DigitalMixers)[reg][DM_LVALUE] & ~DMIX_MUTE_FLAG) & DMIX_LIMIT;
            Value[1] = (WORD)(NewValue[1] = (*DigitalMixers)[reg][DM_RVALUE] & ~DMIX_MUTE_FLAG) & DMIX_LIMIT;

            (*DigitalMixers)[reg][DM_LVALUE] = (*DigitalMixers)[reg][DM_RVALUE] = 0;

#ifdef SUPPORT_I2S
            if((reg == DM_I2S_0) || (reg == DM_I2S_1))
            {
              if((*DigitalMixers)[reg][DM_NUMBER] >= 0xFF)
                ret = RipTideHwAnableI2S(pSGC, (WORD)((reg == DM_I2S_0)? 0: 1));
            }
#endif
          }
          else
          {
            Value[0] = 0;
            Value[1] = 0;

            NewValue[0] = (*DigitalMixers)[reg][DM_LVALUE] | DMIX_MUTE_FLAG;
            NewValue[1] = (*DigitalMixers)[reg][DM_RVALUE] | DMIX_MUTE_FLAG;

#ifdef SUPPORT_I2S
            if((reg == DM_I2S_0) || (reg == DM_I2S_1))
            {
              if((*DigitalMixers)[reg][DM_NUMBER] < 0xFF)
                RipTideHwDisableI2S(pSGC, (WORD)((reg == DM_I2S_0)? 0: 1));
            }
#endif
          }
        }
        else
        {
          dprintf2(("<RT_HW> D_SET: %d %d:%d\n", reg, pValue[0], pValue[1]));

  //        TRACE(("Digital mixer: %x %x %x %x\n", rptr.wRetWords[0], rptr.wRetWords[1], rptr.wRetWords[2], rptr.wRetWords[3]));
  //        Value[0] = (pValue[0] & limit);// | (rptr.wRetWords[0] & (~limit) );
  //        Value[1] = (pValue[1] & limit);// | (rptr.wRetWords[1] & (~limit) );


          Value[0] = pValue[0];
          Value[1] = pValue[1];

          if(iShift)
          {
            Value[0] >>= iShift;
            Value[1] >>= iShift;
          }

          NewValue[0] = Value[0] | ((*DigitalMixers)[reg][DM_LVALUE] & DMIX_MUTE_FLAG);
          NewValue[1] = Value[1] | ((*DigitalMixers)[reg][DM_RVALUE] & DMIX_MUTE_FLAG);

          if((*DigitalMixers)[reg][DM_LVALUE] & DMIX_MUTE_FLAG)
          {
            dreg = 0xFF;
//            Value[0] = 0;
//            Value[1] = 0;
            TRACE(("Mixer is muted\n"));
          }

        }

        if(dreg < 0xFF)
        {
          RipTideHwScheduleMixer(pHwInfo, wId, Value, MIX_SET);
//
//          SEND_SDGV(pSGC->ucCDMAorCDMB, dreg, dreg, Value[1], Value[0]);
        }
        else
        {
          TRACE(("Don't change mixer\n"));
        }

        (*DigitalMixers)[reg][DM_LVALUE] = NewValue[0];
        (*DigitalMixers)[reg][DM_RVALUE] = NewValue[1];

        TRACE(("Set digital %d(%d) mixer left: %x(%x) right: %x(%x) limit: %x\n", reg, dreg, pValue[0], Value[0], pValue[1], Value[1], limit));
      }
      else
      {
        TRACE(("Mixer number is out of range: %d\n", reg));
        ret = FALSE;
      }
    }

    RipTideHwLeave(pHwInfo->sysObj);
  }
  return ret;
}

#ifdef RT_UNUSED
/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwMixerNotification(PMIXER_INFO MixerInfo)
 *
 *  Parameters:     MixerInfo - Pointer to mixer info structure
 *
 *  Returns:        TRUE  - mixer value is changed
 *                  FALSE - otherwise
 *
 *  Description:
 *    Get notification that user changed mixer value
 *
 *************************************************************************/

static
BOOLEAN
RipTideHwMixerNotification(PHwInfo     pHwInfo)
{
CMDRET_TYPE rptr = {{(DWORD)0, (DWORD) 0}};
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;

  RipTideHwEnter(pHwInfo->sysObj);
  SEND_GUNS(CMDA, &rptr);
  RipTideHwLeave(pHwInfo->sysObj);

  return (rptr.wRetWords[1])? TRUE: FALSE;
}
#endif


#define MIX_STEP 0x200

static
VOID
SoundMixerStartTimer(PHwInfo pHwInfo);

/*************************************************************************
 *
 *  Name:           VOID RipTideHwScheduleMixer(
 *                    PMIXER_INFO MixerInfo,
 *                    WORD        wId,
 *                    WORD        *pvalue,
 *                    UCHAR       uOper)
 *
 *
 *  Parameters:     MixerInfo - Pointer to mixer info structure
 *                  wId       - mixer control ID
 *                  pvalue    - Mixer value aray (0 - left channel value,
 *                                                1 - right channel value)
 *                              range: 0-0xffff
 *                  uOper     - type of operation:
 *                              MIX_SET     - set mixer volume
 *                              MIX_MUTE    - mute mixer
 *                              MIX_UNMUTE  - unmute mixer
 *
 *  Returns:        TRUE  - mixer value is changed
 *                  FALSE - otherwise
 *
 *  Description:
 *    Schedule change of mixer volume for smooth changing
 *
 *************************************************************************/

static
VOID
RipTideHwScheduleMixer(
  PHwInfo     pHwInfo,
  WORD        wId,
  WORD        *pvalue,
  UCHAR       uOper)
{
int left, right, step, mix_step, ldelta, rdelta;
WORD        OldValue[2];
WORD        limit, reg;
CMDRET_TYPE rptr = {{(DWORD)0, (DWORD) 0}};
BOOLEAN     bStart;
UCHAR       ucCDMAorCDMB = CMDA;
MixerTab    *Mixers = &pHwInfo->Mixers[0];
ULONG       (*DigitalMixers)[DM_MAX_NUM][DM_RVALUE+1] = &pHwInfo->DigitalMixers;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;

  bStart = Mixers[wId].bStarted;
  Mixers[wId].bStarted = FALSE;

  reg   = gauControlMasks[wId][0];
  limit = gauControlMasks[wId][1];

  Mixers[wId].NewValue[0] = pvalue[0];
  Mixers[wId].NewValue[1] = pvalue[1];

  if(reg < MAXACREG)
  {
    mix_step = 1;

    if( SEND_RACR(ucCDMAorCDMB, reg, &rptr) == FALSE)
    {
      TRACE(("ERROR! Can't read reg value\n"));
    }
// save current value
    OldValue[0] = ((WORD)rptr.bRetBytes[3]) & limit;
    OldValue[1] = ((WORD)rptr.bRetBytes[2]) & limit;

    dprintf2(("<RT_HW> get %x reg: %x %x\n", reg, OldValue[0], OldValue[1]));
    switch(uOper)
    {
      case MIX_SET:
        TRACE(("MIX_SET\n"));
        Mixers[wId].CurValue[0] = pvalue[0];
        Mixers[wId].CurValue[1] = pvalue[1];
        if(rptr.wRetWords[1] & 0x8000)
        {
// set new value as current
          RipTideHwSetReg(pCMDIF, reg, limit, pvalue);
          return;
        }
        else
          if(bStart && Mixers[wId].bMute)
          {
            Mixers[wId].bStarted = TRUE;
          }
          else
            Mixers[wId].bMute = FALSE;
        break;
      case MIX_MUTE:
        TRACE(("MIX_MUTE\n"));
        if(!(rptr.wRetWords[1] & 0x8000))
        {
// mixer is not muted
          if(bStart)
          {
            if(Mixers[wId].bMute)
            {
// Nothing to do - continue
              Mixers[wId].bStarted = TRUE;
              return;
            }
          }
          else
          {
            Mixers[wId].CurValue[0] = OldValue[0];
            Mixers[wId].CurValue[1] = OldValue[1];
          }
          Mixers[wId].bMute = TRUE;
        }
        else
          return;
        break;
      case MIX_UNMUTE:
        TRACE(("MIX_UNMUTE\n"));
        Mixers[wId].NewValue[0] = Mixers[wId].CurValue[0]; //OldValue[0];
        Mixers[wId].NewValue[1] = Mixers[wId].CurValue[1]; //OldValue[1];
        if(rptr.wRetWords[1] & 0x8000)
        {
// set the lowest value 
          RipTideHwSetReg(pCMDIF, reg, limit, pvalue);
// and unmute mixer
          RipTideHwSetReg(pCMDIF, reg, 0x8000, pvalue);
// set current value as new

          OldValue[0] = pvalue[0];
          OldValue[1] = pvalue[1];
        }
        Mixers[wId].bMute = FALSE;
        break;
    }
    TRACE(("bMute = %s\n", (Mixers[wId].bMute)? "TRUE": "FALSE"));
  }
  else
  {
    mix_step = MIX_STEP;

    reg  &= 0x7f;

    if(bStart)
    {
      OldValue[0] = Mixers[wId].CurValue[0];
      OldValue[1] = Mixers[wId].CurValue[1];
    }
    else
    {
      if(((*DigitalMixers)[reg][DM_LVALUE] & DMIX_MUTE_FLAG))
      {
        Mixers[wId].CurValue[0] = OldValue[0] =
        Mixers[wId].CurValue[1] = OldValue[1] = (WORD)0;
      }
      else
      {
        Mixers[wId].CurValue[0] = OldValue[0] = (WORD)(*DigitalMixers)[reg][DM_LVALUE] & DMIX_LIMIT;
        Mixers[wId].CurValue[1] = OldValue[1] = (WORD)(*DigitalMixers)[reg][DM_RVALUE] & DMIX_LIMIT;
      }
      TRACE(("Digital: %x %x\n", (WORD)(*DigitalMixers)[reg][DM_LVALUE] & DMIX_LIMIT, (WORD)(*DigitalMixers)[reg][DM_RVALUE] & DMIX_LIMIT));
    }
  }

  if(Mixers[wId].NewValue[0] > OldValue[0])
  {
    left   = (Mixers[wId].NewValue[0] - OldValue[0]);
    ldelta = mix_step;
  }
  else
  {
    left = (OldValue[0] - Mixers[wId].NewValue[0]);
    ldelta = -mix_step;
  }
                                     
  if(Mixers[wId].NewValue[1] > OldValue[1])
  {
    right  = (Mixers[wId].NewValue[1] - OldValue[1]);
    rdelta = mix_step;
  }
  else
  {
    right  = (OldValue[1] - Mixers[wId].NewValue[1]);
    rdelta = -mix_step;
  }

  step   = ((left >= right)? left: right) / mix_step;
  left  /= mix_step;
  right /= mix_step;

  Mixers[wId].step   = step;
  Mixers[wId].right  = right;
  Mixers[wId].rdelta = rdelta;
  Mixers[wId].left   = left;
  Mixers[wId].ldelta = ldelta;
  Mixers[wId].cur    = 0;

  Mixers[wId].new_left = Mixers[wId].new_right = 0;
  Mixers[wId].bStarted = TRUE;

  SoundMixerStartTimer(pHwInfo);

  TRACE(("RipTideHwScheduleMixer:\nId: %d(0x%x), step: %d\n", wId, reg, step));
  TRACE(("\nOld: %x %x New: %x %x ch: %x %x d: %x %x\n", OldValue[0], OldValue[1], Mixers[wId].NewValue[0], Mixers[wId].NewValue[1], left, right, ldelta, rdelta));
}

/*************************************************************************
 *
 *  Name:           BOOLEAN RipTideHwChangeMixer(PMIXER_INFO MixerInfo)
 *
 *  Parameters:     MixerInfo - Pointer to mixer info structure
 *
 *  Returns:        TRUE  - more work is needed
 *                  FALSE - no more work is remained
 *
 *  Description:
 *    Change values of scheduled mixers
 *
 *************************************************************************/

static
BOOLEAN
RipTideHwChangeMixer(PHwInfo     pHwInfo)
{
int i;
WORD OldValue[2], reg, limit, value, lim;
int left, right, step, new_left, new_right, ldelta, rdelta;
BOOLEAN ret = FALSE;
CMDRET_TYPE rptr = {{(DWORD)0, (DWORD) 0}};
UCHAR ucCDMAorCDMB = CMDA;
MixerTab    *Mixers = &pHwInfo->Mixers[0];
ULONG       (*DigitalMixers)[DM_MAX_NUM][DM_RVALUE+1] = &pHwInfo->DigitalMixers;
pCMDIF_AB   pCMDIF = pHwInfo->tCmdIf;

//  TRACE(("Started RipTideHwChangeMixer\n"));

  RipTideHwEnter(pHwInfo->sysObj);
  for(i = 0; i < MAX_MIXER_CONTROLS; i++)
  {
    step      = Mixers[i].step;
    left      = Mixers[i].left;
    right     = Mixers[i].right;
    new_left  = Mixers[i].new_left;
    new_right = Mixers[i].new_right;

    reg   = gauControlMasks[i][0];
    limit = gauControlMasks[i][1];

    ldelta = rdelta = 0;
    value = 0;

    if(Mixers[i].bStarted == TRUE)
    {

//      TRACE(("i = %d cur = %d step = %d\n", i, Mixers[i].cur, step));
      
      new_left  += left;
      new_right += right;

      if(new_left >= step)
      {
        new_left -= step;
        ldelta    = Mixers[i].ldelta;
      }

      if(new_right >= step)
      {
        new_right -= step;
        rdelta     = Mixers[i].rdelta;
      }

      Mixers[i].new_left  = new_left;
      Mixers[i].new_right = new_right;

      if(reg < MAXACREG)
      {
        if( SEND_RACR(ucCDMAorCDMB, reg, &rptr) == FALSE)
        {
          TRACE(("ERROR! Can't read reg value\n"));
          return FALSE;
        }

        lim = (limit | (limit << 8));

        if(Mixers[i].cur < step)
        {
          OldValue[0] = (((WORD)rptr.bRetBytes[3]) & limit) + ldelta;
          OldValue[1] = (((WORD)rptr.bRetBytes[2]) & limit) + rdelta;
          ret = TRUE;
        }
        else
        {
          if(Mixers[i].bMute)
          {
            value = 0x8000;
            OldValue[0] = Mixers[i].CurValue[0];
            OldValue[1] = Mixers[i].CurValue[1];
            Mixers[i].bMute = FALSE;
          }
          else
          {
            OldValue[0] = Mixers[i].NewValue[0];
            OldValue[1] = Mixers[i].NewValue[1];
          }
        }


        value |= ((OldValue[1] & limit) | ((OldValue[0] & limit) << 8));

        value |= (rptr.wRetWords[1] & (~lim));

        TRACE(("reg: %x val: %x\n", reg, value));
        SEND_SACR(ucCDMAorCDMB, value, reg);
      }
      else
      {
        reg &= 0x7f;
        if((WORD)(*DigitalMixers)[reg][DM_NUMBER] < 0xFF)
        {
          if(Mixers[i].cur < step)
          {
            OldValue[0] = Mixers[i].CurValue[0] + ldelta;
            OldValue[1] = Mixers[i].CurValue[1] + rdelta;
            ret = TRUE;
          }
          else
          {
            OldValue[0] = Mixers[i].NewValue[0];
            OldValue[1] = Mixers[i].NewValue[1];
            TRACE(("val: %x %x\n", OldValue[0], OldValue[1]));
         }

          SEND_SDGV(ucCDMAorCDMB, (WORD)(*DigitalMixers)[reg][DM_NUMBER], (WORD)(*DigitalMixers)[reg][DM_NUMBER], OldValue[1], OldValue[0]);

          Mixers[i].CurValue[0] = OldValue[0];
          Mixers[i].CurValue[1] = OldValue[1];
        }
        else
          Mixers[i].bStarted = FALSE;
      }

      if(Mixers[i].cur++ >= step)
      {
        Mixers[i].bStarted = FALSE;
      }
    }
  }
  RipTideHwLeave(pHwInfo->sysObj);
  return ret;
}

#ifdef RT_UNUSED
static
BOOLEAN
RipTideHwSetChannelVolume(
  PChannelInfo pChannelInfo,
  WORD    *pValue
)
{
PHwInfo pHwInfo = pChannelInfo->pHwInfo;

  if(pChannelInfo->MixerControls < MAX_MIXER_CONTROLS)
    return RipTideHwSetMixer(pHwInfo, pChannelInfo->MixerControls, pValue);
  else
  {
    TRACE(("ERROR !!! MixerControls[%i] is not allocated\n", pChannelInfo->ChannelId));
    return FALSE;
  }
}

static
BOOLEAN
RipTideHwGetChannelVolume(
  PChannelInfo pChannelInfo,
  WORD    *pValue
)
{
PHwInfo pHwInfo = pChannelInfo->pHwInfo;

  if(pChannelInfo->MixerControls < MAX_MIXER_CONTROLS)
    return RipTideHwGetMixer(pHwInfo, pChannelInfo->MixerControls, pValue);
  else
  {
    TRACE(("ERROR !!! MixerControls[%i] is not allocated\n", pChannelInfo->ChannelId));
    return FALSE;
  }
}

static
BOOLEAN
RipTideHwMuteChannel(
  PChannelInfo pChannelInfo,
  BOOLEAN bMute
)
{
WORD    pValue[2];
UCHAR   MixerControls = pChannelInfo->MixerControls;

  pValue[0] =
    pValue[1] = (bMute == TRUE)? 0xffff: 0;

  if(MixerControls < MAX_MIXER_CONTROLS)
    return RipTideHwSetMixer(pChannelInfo->pHwInfo, MixerControls - MXMIX_WAVE0 + MXMUTE_WAVE0,
                             pValue);
  else
  {
    TRACE(("ERROR !!! MixerControls[%i] is not allocated\n", pChannelInfo->ChannelId));
    return FALSE;
  }
}
#endif

static
VOID
SoundMixerStartTimer(PHwInfo pHwInfo)
{
  TRACE(("Start Mixer Timer"));

  if(!pHwInfo->isStarted) {
    pHwInfo->isStarted = TRUE;

    set_timer(pHwInfo->sysObj, 10000L);
  }
}

static
VOID
SoundMixerStopTimer(PHwInfo pHwInfo)
{
  TRACE(("Stop Mixer Timer"));

  if(pHwInfo->isStarted) {
    pHwInfo->isStarted = FALSE;
    cancel_timer(pHwInfo->sysObj);
  }
}


#endif  //SUPPORT_MIXER

static
void
RipTideHwRelease
(
   void *pHwInfo
)
{
    dprintf(("<RT_HW> RipTideHwRelease started\n"));
#ifdef SUPPORT_MIXER
    SoundMixerStopTimer((PHwInfo)pHwInfo);
#endif
    free_mem(pHwInfo, sizeof(PHwInfo));
}

#ifdef SUPPORT_MIXER

static
VOID
SoundMixerTimerDeferred(
    unsigned long Context
)
{
PHwInfo     pHwInfo = (PHwInfo)Context;

//  TRACE(("Timer interrupt for Mixer"));

  if(pHwInfo->isStarted)
  {
    if(RipTideHwChangeMixer(pHwInfo))
    {
      set_timer(pHwInfo->sysObj, 10000L);
    }
    else
    {
      pHwInfo->isStarted = FALSE;
    }
  }
}


#endif  //SUPPORT_MIXER

static
void
RipTideGetFragment(
				void *pChannelInfo,
				int *fragsize,
				int *fragstotal
				)
{
ULONG ulLen;

    if(((PChannelInfo)pChannelInfo)->DMAFragmentSize == 0)
        RipTideCalcFragmentSize(pChannelInfo);

    ulLen = CYCLIC_DMA_BUFFER_SIZE/((PChannelInfo)pChannelInfo)->DMAFragmentSize;

    if(ulLen < 2)
        ulLen = 2;

    if(((PChannelInfo)pChannelInfo)->DMAFragmentNum > ulLen)
    {
        ((PChannelInfo)pChannelInfo)->DMAFragmentNum = ulLen;
    }

    *fragsize = ((PChannelInfo)pChannelInfo)->DMAFragmentSize;
    *fragstotal = ((PChannelInfo)pChannelInfo)->DMAFragmentNum;
}

static
void
RipTideSetFragment(
				void *pChannelInfo,
				int fragsize,
				int fragstotal
				)
{
ULONG ulLen;

	if(fragsize > MAX_FRAG_SIZE) {
		/* limit fragsize to MAX_FRAG_SIZE */
		if(fragstotal != 0x7fff) { /* recalc fragstotal if not unlimited */
			fragstotal = (fragsize*fragstotal)/MAX_FRAG_SIZE;
		}
		fragsize = MAX_FRAG_SIZE;
	}
    ((PChannelInfo)pChannelInfo)->DMAFragmentSize = fragsize;
    ((PChannelInfo)pChannelInfo)->DMAFragmentMax  = fragstotal;
    if((ulLen = fragsize * fragstotal) > CYCLIC_DMA_BUFFER_SIZE)
    {
        ulLen = CYCLIC_DMA_BUFFER_SIZE / fragsize;
        ((PChannelInfo)pChannelInfo)->DMAFragmentNum = (ulLen < 2)? 2: ulLen;
    }
    else
        ((PChannelInfo)pChannelInfo)->DMAFragmentNum = fragstotal;
    ((PChannelInfo)pChannelInfo)->DMABufferSize = ((PChannelInfo)pChannelInfo)->DMAFragmentSize *
                                                  ((PChannelInfo)pChannelInfo)->DMAFragmentNum;
}

static
void
RipTideSetSampleRate(
			  void 	*pChannelInfo,
			  int    SamplesPerSec
			)
{
    dprintf2(("<RT_HW> Rate: %d\n", SamplesPerSec));
    ((PChannelInfo)pChannelInfo)->SamplesPerSec = SamplesPerSec;
}

static
int
RipTideGetSampleRate(
			  void *pChannelInfo
			)
{
    return ((PChannelInfo)pChannelInfo)->SamplesPerSec;
}

static
void
RipTideSetBitsPerSample(
			  void   *pChannelInfo,
			  int     BitsPerSample
			)
{
    dprintf2(("<RT_HW> BPS: %d\n", BitsPerSample));
    ((PChannelInfo)pChannelInfo)->BitsPerSample = BitsPerSample;
}

static
int
RipTideGetBitsPerSample(
			  void *pChannelInfo
		    )
{
    return ((PChannelInfo)pChannelInfo)->BitsPerSample;
}

static
void
RipTideSetChannels(
			  void   *pChannelInfo,
			  int     Channels
			)
{
    dprintf2(("<RT_HW> Channels: %d\n", Channels));
    ((PChannelInfo)pChannelInfo)->nChannels = Channels;
}

static
int
RipTideGetChannels(
			  void *pChannelInfo
			)
{
    return ((PChannelInfo)pChannelInfo)->nChannels;
}

static
PVOID RipTideHwGetDMABuffer(
		        void *pChannelInfo,
		        int  *size
			)
{
    *size = ((PChannelInfo)pChannelInfo)->DMABufferSize;
    return ((PChannelInfo)pChannelInfo)->DMABuffer;
}

static
void RipTideSetFunction(rt_func_table *func_table)
{
    func_table->RipTideNextChannel          = RipTideNextChannel;
    func_table->RipTideQueryInterrupt       = RipTideQueryInterrupt;
    func_table->RipTideHwCompleteInterrupt  = RipTideHwCompleteInterrupt;
    func_table->RipTideDeleteChannel        = RipTideDeleteChannel;
    func_table->RipTideCreateChannel        = RipTideCreateChannel;
    func_table->RipTideAddBuffer            = RipTideAddBuffer;
    func_table->RipTideHwStartDMA           = RipTideHwStartDMA;
    func_table->RipTideHwStopDMA            = RipTideHwStopDMA; 
    func_table->RipTideHwReadDMACounter     = RipTideHwReadDMACounter; 
    func_table->RipTideGetFragment          = RipTideGetFragment; 
    func_table->RipTideSetFragment          = RipTideSetFragment; 
    func_table->RipTideQueryBusy            = RipTideQueryBusy;
    func_table->RipTideQueryIdle            = RipTideQueryIdle;                
    func_table->RipTideSetSampleRate        = RipTideSetSampleRate;
    func_table->RipTideGetSampleRate        = RipTideGetSampleRate;
    func_table->RipTideSetBitsPerSample     = RipTideSetBitsPerSample;
    func_table->RipTideGetBitsPerSample     = RipTideGetBitsPerSample;
    func_table->RipTideSetChannels          = RipTideSetChannels;
    func_table->RipTideGetChannels          = RipTideGetChannels;
    func_table->RipTideHwGetMixer           = RipTideHwGetMixer;
    func_table->RipTideHwSetMixer           = RipTideHwSetMixer; 
    func_table->RipTideHwRelease            = RipTideHwRelease; 
    func_table->RipTideHwGetDMABuffer       = RipTideHwGetDMABuffer;
}
