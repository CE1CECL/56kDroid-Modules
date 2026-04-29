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
  MODULE NAME:        rt_hw.h

  AUTHOR:             C. Lazarev    (Transcribed by CNL)

  HISTORY:         Major Revision               Date        By
            -----------------------------     --------     -----
            Created                           11/18/97      CNL


  DESCRIPTION:

    This module contains definitions for communicating with the
    hardware on the RipTide card.

  NOTES:    None.
*/
#ifndef _RT_HW_H_
#define _RT_HW_H_

#define RIPTIDE_BOARD_A15


#ifdef RIPTIDE_BOARD_A15
#define LOAD_FIRMWARE
#define MONO_VIA_STEREO
#else
#error Incorrect riptide version
#endif

//#define SUPPORT_WAVE_IN
//#define SUPPORT_WAVE_OUT
//#define SUPPORT_MIXER

//#define SUPPORT_MICROPHONE
//#define SUPPORT_CD_ROM
//#define SUPPORT_LINE_IN
//#define SUPPORT_VIDEO
//#define SUPPORT_AUX
//#define SUPPORT_WAVESTREAM
//#define SUPPORT_MODEM
//#define SUPPORT_I2S

// Legacy devices

//#define SUPPORT_LEGACY_DEVICES

//#define SUPPORT_MIDI_IN
//#define SUPPORT_MIDI_OUT
//#define SUPPORT_SYNTH
//#define SUPPORT_JOYSTICK
//#define SUPPORT_SOUNDBLASTER


#define NO_RASM

#ifdef RIPTIDE_BOARD_ES1
#define ES1
#endif

#ifdef RIPTIDE_BOARD_A0
#define ASIC_A0
#endif

#ifdef RIPTIDE_BOARD_A1
#define ASIC_A1
#endif

#ifdef RIPTIDE_BOARD_A15
#define ASIC_A1
#endif

// Hardware definitions

/*
 *  Access to hardware memory
 */

//
// macros for doing port manipulation
//
#define MASK_PORT_UCHAR(x,m)   (WRITE_PORT_UCHAR  (x, READ_PORT_UCHAR(x)  & (m)))
#define UMASK_PORT_UCHAR(x,m)  (WRITE_PORT_UCHAR  (x, READ_PORT_UCHAR(x)  | (m)))
#define MASK_PORT_USHORT(x,m)  (WRITE_PORT_USHORT (x, READ_PORT_USHORT(x) & (m)))
#define UMASK_PORT_USHORT(x,m) (WRITE_PORT_USHORT (x, READ_PORT_USHORT(x) | (m)))
#define MASK_PORT_ULONG(x,m)   (WRITE_PORT_ULONG  (x, READ_PORT_ULONG(x)  & (m)))
#define UMASK_PORT_ULONG(x,m)  (WRITE_PORT_ULONG  (x, READ_PORT_ULONG(x)  | (m)))

#define MASK_REGISTER_UCHAR(x,m)   (WRITE_REGISTER_UCHAR  (x, READ_REGISTER_UCHAR(x)  & (m)))
#define UMASK_REGISTER_UCHAR(x,m)  (WRITE_REGISTER_UCHAR  (x, READ_REGISTER_UCHAR(x)  | (m)))
#define MASK_REGISTER_USHORT(x,m)  (WRITE_REGISTER_USHORT (x, READ_REGISTER_USHORT(x) & (m)))
#define UMASK_REGISTER_USHORT(x,m) (WRITE_REGISTER_USHORT (x, READ_REGISTER_USHORT(x) | (m)))
#define MASK_REGISTER_ULONG(x,m)   (WRITE_REGISTER_ULONG  (x, READ_REGISTER_ULONG(x)  & (m)))
#define UMASK_REGISTER_ULONG(x,m)  (WRITE_REGISTER_ULONG  (x, READ_REGISTER_ULONG(x)  | (m)))

#define EOB_STATUS         0x80000000
#define EOS_STATUS         0x40000000
#define EOC_STATUS         0x20000000
#define ERR_STATUS         0x10000000
#define EMPTY_STATUS       0x08000000

#define IEOB_ENABLE        0x1
#define IEOS_ENABLE        0x2
#define IEOC_ENABLE        0x4
#define RDONCE             0x8

// Path definitions
/* 

  A0 Source IDs
  
*/      

enum E1SOURCE {   

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
  ARM2LBUS_FIFO15,
  
  INTER0_OUT,             // 16
  INTER1_OUT,
  INTER2_OUT,
  INTER3_OUT,

#ifdef ASIC_A0  

  DECIM0_OUT,             // 20
  DECIM1_OUT,
  
  SR3_4_OUT,
  
  ARM2LBUS_FIFO_DIRECT, 
  
  OPL3_SAMPLE,
  
  ASRC0,
  ASRC1,
  
  ACLNK2PADC,
  ACLNK2MODEM0RX,
  ACLNK2MIC,     
  ACLNK2MODEM1RX,
  ACLNK2HNDMIC,
  
  DIGITAL_MIXER_OUT0,     // 32
  
  GAINFUNC0_OUT,
  GAINFUNC1_OUT,
  GAINFUNC2_OUT,
  GAINFUNC3_OUT,
  GAINFUNC4_OUT,
  
  SOFTMODEMTX,            // 38

#endif

#ifdef ASIC_A1

  INTER4_OUT,

  INTERM0_OUT,            // 21
  INTERM1_OUT,
  INTERM2_OUT,
  INTERM3_OUT,
  INTERM4_OUT,
  INTERM5_OUT,
  INTERM6_OUT,

  DECIMM0_OUT,            // 28
  DECIMM1_OUT,
  DECIMM2_OUT,
  DECIMM3_OUT,

  DECIM0_OUT,             // 32

  SR3_4_OUT,
  
    
  OPL3_SAMPLE,
  
  ASRC0,
  ASRC1,
  
  ACLNK2PADC,
  ACLNK2MODEM0RX,
  ACLNK2MIC,     
  ACLNK2MODEM1RX,
  ACLNK2HNDMIC,
  
  DIGITAL_MIXER_OUT0,     // 42
  
  GAINFUNC0_OUT,
  GAINFUNC1_OUT,
  GAINFUNC2_OUT,
  GAINFUNC3_OUT,
  GAINFUNC4_OUT,
  
  SOFTMODEMTX,

  SPLITTER0_OUTL,         // 49
  SPLITTER0_OUTR,
  SPLITTER1_OUTL,
  SPLITTER1_OUTR,
  SPLITTER2_OUTL,
  SPLITTER2_OUTR,
  SPLITTER3_OUTL,
  SPLITTER3_OUTR,

  MERGER0_OUT,            // 57
  MERGER1_OUT,
  MERGER2_OUT,
  MERGER3_OUT,

  ARM2LBUS_FIFO_DIRECT, 
#endif
  
};


enum E2SINK {
  
  LBUS2ARM_FIFO0,
  LBUS2ARM_FIFO1,
  LBUS2ARM_FIFO2,
  LBUS2ARM_FIFO3, 
  LBUS2ARM_FIFO4,
  LBUS2ARM_FIFO5,
  LBUS2ARM_FIFO6,
  LBUS2ARM_FIFO7,
  
  INTER0_IN,          //  8
  INTER1_IN,
  INTER2_IN,
  INTER3_IN,
  
#ifdef ASIC_A0

  DECIM0_IN,          // 12
  DECIM1_IN,
  
  SR3_4_IN,           // 14
  
  PDAC2ACLNK,
  
  MODEM0TX2ACLNK,
  MODEM1TX2ACLNK, 
  
  HNDSPK2ACLNK,
  
  DIGITAL_MIXER_IN0,  // 19 
  DIGITAL_MIXER_IN1, 
  DIGITAL_MIXER_IN2,
  DIGITAL_MIXER_IN3, 
  DIGITAL_MIXER_IN4,
  DIGITAL_MIXER_IN5, 
  DIGITAL_MIXER_IN6,
  DIGITAL_MIXER_IN7, 
  DIGITAL_MIXER_IN8,
  DIGITAL_MIXER_IN9, 
  DIGITAL_MIXER_IN10,
  DIGITAL_MIXER_IN11, 
  
  GAINFUNC0_IN,       // 31
  GAINFUNC1_IN,
  GAINFUNC2_IN,
  GAINFUNC3_IN,
  GAINFUNC4_IN,
  
  SOFTMODEMRX,
  
#endif

#ifdef ASIC_A1

  INTER4_IN,

  INTERM0_IN,         // 13
  INTERM1_IN,
  INTERM2_IN,
  INTERM3_IN,
  INTERM4_IN,
  INTERM5_IN,
  INTERM6_IN,

  DECIMM0_IN,         // 20
  DECIMM1_IN,
  DECIMM2_IN,
  DECIMM3_IN,

  DECIM0_IN,          // 24

    
  SR3_4_IN,
  
  PDAC2ACLNK,
  
  MODEM0TX2ACLNK,
  MODEM1TX2ACLNK, 
  
  HNDSPK2ACLNK,
  
  DIGITAL_MIXER_IN0,  // 30
  DIGITAL_MIXER_IN1, 
  DIGITAL_MIXER_IN2,
  DIGITAL_MIXER_IN3, 
  DIGITAL_MIXER_IN4,
  DIGITAL_MIXER_IN5, 
  DIGITAL_MIXER_IN6,
  DIGITAL_MIXER_IN7, 
  DIGITAL_MIXER_IN8,
  DIGITAL_MIXER_IN9, 
  DIGITAL_MIXER_IN10,
  DIGITAL_MIXER_IN11, 
  
  GAINFUNC0_IN,       // 42
  GAINFUNC1_IN,
  GAINFUNC2_IN,
  GAINFUNC3_IN,
  GAINFUNC4_IN,
  
  SOFTMODEMRX,

  SPLITTER0_IN,       // 48
  SPLITTER1_IN,
  SPLITTER2_IN,
  SPLITTER3_IN,

  MERGER0_INL,        // 52
  MERGER0_INR,
  MERGER1_INL,
  MERGER1_INR,
  MERGER2_INL,
  MERGER2_INR,
  MERGER3_INL,
  MERGER3_INR,

#endif
E2SINK_MAX
};         

// Channel identificators

typedef enum
{
  M0TX = 0,
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
  SPARE,
  M0RX,
  HSMIC,
  M1RX,
  CLEANRX,
  MICADC,
  PADC,
  COPRX1,
  COPRX2,
  CHANNEL_ID_COUNTER
} RT_CHANNEL_IDS;

#define MAX_STREAM_SET CHANNEL_ID_COUNTER

enum LBUS_SINK
{
  LS_SRC_INTERPOLATOR,      // SRC stereo interpolator
  LS_SRC_INTERPOLATORM,     // SRC mono interpolator
  LS_SRC_DECIMATOR,         // SRC stereo decimator
  LS_SRC_DECIMATORM,        // SRC mono decimator
  LS_MIXER_IN,              // digital mixer input
  LS_MIXER_GAIN_FUNCTION,   // digital gain input
	LS_SRC_SPLITTER,         // splitter input
	LS_SRC_MERGER,           // merger input
};


typedef enum
{
  SGC_INIT,
  SGC_ACTIVE,
  SGC_STOP,
  SGC_PAUSED,
  SGC_INTERRUPT
} SGC_STATES;

typedef enum
{
  DM_WAVEOUT0,
  DM_WAVEOUT1,
  DM_WAVEOUT2,
  DM_WAVEOUT3,
  DM_WAVEOUT4,
  DM_WAVEOUT5,
  DM_WAVEOUT6,
  DM_WAVEOUT7,
  DM_WAVEOUT8,
  DM_WAVEOUT9,
  DM_WAVESTR,
  DM_FMOUT,
  DM_SBOUT,
  DM_PHONE,
  DM_I2S_0,
  DM_I2S_1,
  DM_MAX_NUM
} DIGITAL_MIXERS;

#define MAX_DMA_PLAY   10
#define MAX_DMA_RECORD 1

// LBUS Resources for WaveStream
#define WAVESTREAM_CHANNEL  WVSTRTX
#define WAVESTREAM_SOURCE   ARM2LBUS_FIFO14
   
// LBUS Resources for I2S
#define I2S_CHANNEL   TAMTX               // Channel Id for I2S
#define I2S_SOURCE    ARM2LBUS_FIFO0      // I2S playback channel
#define I2S_SINK      LBUS2ARM_FIFO6      // I2S record channel
#define I2S_RATE      44100               // I2S rate

// LBUS Resources for FM
#define FM_CHANNEL    COP3DTX             // Channel Id for FM/OPL3

// LBUS Resources for FM
#define OUT_CHANNEL   SPARE               // Channel Id for FM/OPL3

// LBUS Resources for modem
#define MODEM_CHANNEL WVSTRTX             // Channel Id for modem
#define MODEM_MIXER   DIGITAL_MIXER_IN11  // digital mixer LBUS sink for modem
#define MODEM_SRC     INTER4_IN           // Sample rate LBUS sink convertor for modem
#define MODEM_MERGER  MERGER3_INL         // Merger LBUS sink for modem

#ifdef RIPTIDE_BOARD_A0

#ifdef SUPPORT_WAVE_IN
#define WaveInDeviceChannel PADC
#endif

#ifdef SUPPORT_WAVE_OUT
#define WaveOutDeviceChannel PDAC
#endif

#endif

typedef struct tagSGD_DMA
{  
	DWORD	dwNextLink;
	DWORD	dwSegPtrPhys;
	DWORD	dwSegLen;
	DWORD	dwStat_Ctl;

} SGD_DMA, *PSGD_DMA;

enum { SB_CMD, MODEM_CMD, I2S_CMD0, I2S_CMD1, FM_CMD, MAX_CMD };

#define DESC_MAX_NUM   0xff
#define DESC_MAX_MASK  DESC_MAX_NUM
#define MAX_FRAG_SIZE  (0x1000)

typedef struct
{
  UCHAR       ucDeviceType;           // see SOUND_DEVICE in sound.h
  DWORD       dwLinkHead;             // physical address of first SGD 

  ULONG       Sgd_In;                 // Head number in DMA descriptors round buffer
  ULONG       Sgd_Out;                // Tail number in DMA descriptors round buffer
  ULONG       Total_In;
  ULONG       Total_Out; 
  
  ULONG       ulIOCounter;            // Counter of data is already processed
  ULONG       ulIOSize;               // Size of data in buffer

  UCHAR       ucChannelId;            // Id of the s/g channel
  UCHAR       ucChannelState;         // State of the channel
  UCHAR       ucChannelDir;           // Direction of the channel
  PSGD_DMA    sgdList;                // DMA descriptors round buffer
} SGCDescriptor, *PSGCDescriptor, SGC, *PSGC;

typedef SGC RTHardwareContext, *PRTHardwareContext;

#define MIDI_BUF_MAX  0xff
#define MIDI_STARTUP  0
#define MIDI_IN_STOP  1
#define MIDI_IN_START 2

typedef struct
{
  ULONG      ulHead, ulTail;
  UCHAR      ucState;
  UCHAR      InBuffer[MIDI_BUF_MAX+1];
} RTMidiHwContext;


#define INPORT(pHw, port)		READ_PORT_UCHAR((PUCHAR)(((pHw)->PortBase) + (port))) 
#define OUTPORT(pHw, port, data)	WRITE_PORT_UCHAR((PUCHAR)(((pHw)->PortBase) + (port)), (UCHAR)(data))


#define DMIX_LIMIT      0x7FFF
#define DMIX_MUTE       0x8000
#define DMIX_MUTE_FLAG  0xFFFF0000
#define DMIX_STOP_FLAG  0xFFFFFF00

// Mixer definitions

#define RWAM_NULL       UINT_MAX,UINT_MAX
#define RWAM_MUX_SLCT   0x1A,0x07
#define RWAM_MUX_MSTR   0x1C,0x0F
#define RWAM_MIX_SPKR   0x02,0x1F
#define RWAM_MIX_CLIP   0x6A,0X0F
#define RWAM_MIX_HDPHN  0x04,0x1F 
#define RWAM_MIX_WAVE0  DM_WAVEOUT0 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WAVE1  DM_WAVEOUT1 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WAVE2  DM_WAVEOUT2 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WAVE3  DM_WAVEOUT3 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WAVE4  DM_WAVEOUT4 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WAVE5  DM_WAVEOUT5 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WAVE6  DM_WAVEOUT6 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WAVE7  DM_WAVEOUT7 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WAVE8  DM_WAVEOUT8 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WAVE9  DM_WAVEOUT9 | 0x80,DMIX_LIMIT         //0x80,0xFFFF
#define RWAM_MIX_WSTRM  DM_WAVESTR  | 0x80,DMIX_LIMIT         //0x82,0xFFFF
#define RWAM_MIX_FMOUT  DM_FMOUT    | 0x80,DMIX_LIMIT         //0x82,0xFFFF
#define RWAM_MIX_SBOUT  DM_SBOUT    | 0x80,DMIX_LIMIT         //0x82,0xFFFF
#define RWAM_MIX_PHONE  DM_PHONE    | 0x80,DMIX_LIMIT         //0x82,0xFFFF
#define RWAM_MIX_I2S_0  DM_I2S_0    | 0x80,DMIX_LIMIT         //0x82,0xFFFF
#define RWAM_MIX_I2S_1  DM_I2S_0    | 0x80,DMIX_LIMIT         //0x82,0xFFFF

#define RWAM_MIX_ASIC   0x18,0x1F  //0x18,AC98MAX
#define RWAM_MIX_MIC    0x0E,0x1F  //0x0E,AC98MAX
#define RWAM_MIX_LINE   0x10,0x1F  //0x10,AC98MAX
#define RWAM_MIX_CD     0x12,0x1F  //0x12,AC98MAX
#define RWAM_MIX_VIDEO  0x14,0x1F  //0x14,AC98MAX
#define RWAM_MIX_AUX    0x16,0x1F  //0x16,AC98MAX
#define RWAM_MUTE_SPKR  0x02,0x8000
#define RWAM_MUTE_HDPH  0x04,0x8000
#define RWAM_MUTE_WAVE0 DM_WAVEOUT0 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WAVE1 DM_WAVEOUT1 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WAVE2 DM_WAVEOUT2 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WAVE3 DM_WAVEOUT3 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WAVE4 DM_WAVEOUT4 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WAVE5 DM_WAVEOUT5 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WAVE6 DM_WAVEOUT6 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WAVE7 DM_WAVEOUT7 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WAVE8 DM_WAVEOUT8 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WAVE9 DM_WAVEOUT9 | 0x80,DMIX_MUTE         //0x80,0xFFFF
#define RWAM_MUTE_WSTRM DM_WAVESTR  | 0x80,DMIX_MUTE         //0x82,0xFFFF
#define RWAM_MUTE_FMOUT DM_FMOUT    | 0x80,DMIX_MUTE         //0x82,0xFFFF
#define RWAM_MUTE_SBOUT DM_SBOUT    | 0x80,DMIX_MUTE         //0x82,0xFFFF
#define RWAM_MUTE_PHONE DM_PHONE    | 0x80,DMIX_MUTE         //0x82,0xFFFF
#define RWAM_MUTE_I2S_0 DM_I2S_0    | 0x80,DMIX_MUTE         //0x82,0xFFFF
#define RWAM_MUTE_I2S_1 DM_I2S_0    | 0x80,DMIX_MUTE         //0x82,0xFFFF

#define RWAM_MUTE_ASIC  0x18,0x8000
#define RWAM_MUTE_MIC   0x0E,0x8000
#define RWAM_MUTE_LINE  0x10,0x8000
#define RWAM_MUTE_CD    0x12,0x8000
#define RWAM_MUTE_VIDEO 0x14,0x8000
#define RWAM_MUTE_AUX   0x16,0x8000
#define RWAM_MIX_3D     0x20,0x2000
#define RWAM_MIX_BASS   0x20,0x1000
#define RWAM_MIC_BOOST  0x0E,0x40
#define RWAM_MUTE_MUX   0x1C,0x8000
                                
#define RWAM_MIX_HDPHN  0x04,0x1F
#define RWAM_MUTE_HDPHN 0x04,0x8000

#define RWAM_HDPHN      0x04
#define RWAM_ASIC       0x18
#define RWAM_RECDMIC	  0x1E
#define RWAM_ASIC_DEF   0x0808L

#define RWAM_3D_CTRL    0x22
#define RWAM_3D_MASK    0x0F0F

#define RWAM_MIX_BREAK  0x18,0x1F       //0x8E,0x7FFF
#define RWAM_MUTE_BREAK 0x18,0x8000       //0x8E,0xFFFF

//
//  MPU401 stuff
//

#define MPU401_REG_STATUS   0x01    // Status register
#define MPU401_DRR          0x40    // Output ready (for command or data)
#define MPU401_DSR          0x80    // Input ready (for data)

#define MPU401_REG_DATA     0x00    // Data in
#define MPU401_REG_COMMAND  0x01    // Commands
#define MPU401_CMD_RESET    0xFF    // Reset command
#define MPU401_CMD_UART     0x3F    // Switch to UART mod

#define MIDI_TO             (25000L)  // Time to wait for MPU401 ready

/*
#define ALLOCATE_PAGE( s, p )   HalAllocateCommonBuffer( AdapterObject, s, p, FALSE ) 
#define FREE_PAGE( s, p, v )    HalFreeCommonBuffer( AdapterObject, s, p, v, FALSE )

#define ALLOCATE_HEAP(x)        ExAllocatePool(NonPagedPool, x )
#define FREE_HEAP(x)            ExFreePool( x )
*/

typedef struct tagMixer
{
  int   step;
  int   right;
  int   rdelta;
  int   left;
  int   ldelta;
  int   cur;
  int   new_left;
  int   new_right;
  BOOLEAN bStarted;
  BOOLEAN bMute;
  WORD  NewValue[2];
  WORD  CurValue[2];
} MixerTab;

#define DM_NUMBER 0
#define DM_LVALUE 1
#define DM_RVALUE 2

 
#endif /* _RT_HW_H_ */
