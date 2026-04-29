/*
 * Copyright (c) 2003 Linuxant inc.
 * Copyright (c) 2001-2003 Conexant Systems, Inc.
 *
 * NOTE: The use and distribution of this software is governed by the terms in
 * the file LICENSE, which is included in the package. You must read this and
 * agree to these terms before using or distributing this software.
 *
 */
#ifndef __RIPTIDE_H_INCLUDED
#define __RIPTIDE_H_INCLUDED

/*
  MODULE NAME:        riptide.h                       

  AUTHOR:             O. Druzhinin  (Transcribed by OLD)


  HISTORY:         Major Revision               Date        By
            -----------------------------     --------     -----
            Created                           10/16/97      OLD



  DESCRIPTION:

      This include file defines driver options.

  NOTES:    None.
*/

/*
 *  Driver Version
 */

#undef DBG

#include "linux.h"
#include "riptidem.h"
#include "rp_cmdif.h"
#include "rt_hw.h"
#include "rtmixer.h"

#define  DRIVER_VERSION    0x0109

#define SUPPORT_WAVE_IN
#define SUPPORT_WAVE_OUT
#define SUPPORT_MIXER

#define SUPPORT_MICROPHONE
#define SUPPORT_CD_ROM
#define SUPPORT_LINE_IN
#define SUPPORT_VIDEO
#define SUPPORT_AUX

#ifndef MM_ROCKWELL
#define MM_ROCKWELL 111
#endif

#define ROCKWELL_RIPTIDE_WAVEIN     300  //
#define ROCKWELL_RIPTIDE_WAVEOUT    301  //
#define ROCKWELL_RIPTIDE_MIXER      302  //
#define ROCKWELL_RIPTIDE_SYNTH      303  // OPL3/FM
#define ROCKWELL_RIPTIDE_WAVESTREAM 304  // Software MIDI synthesiser
#define ROCKWELL_RIPTIDE_MPU401_IN  305  //
#define ROCKWELL_RIPTIDE_MPU401_OUT 306  //
#define ROCKWELL_RIPTIDE_LINE_IN    307  //
#define ROCKWELL_RIPTIDE_CD         308  //
#define ROCKWELL_RIPTIDE_MIC        309  // Microphone
#define ROCKWELL_RIPTIDE_AUX        310  //
#define ROCKWELL_RIPTIDE_PHONE      311  // Speakerphone/modem
#define ROCKWELL_RIPTIDE_VIDEO      312  //
#define ROCKWELL_RIPTIDE_I2S        313  // I2S channels

#define MID_WAVEIN         (MM_ROCKWELL                )
#define PID_WAVEIN         (ROCKWELL_RIPTIDE_WAVEIN    )

#define MID_WAVEOUT        (MM_ROCKWELL                )
#define PID_WAVEOUT        (ROCKWELL_RIPTIDE_WAVEOUT   )

#define MID_MIXER          (MM_ROCKWELL                )
#define PID_MIXER          (ROCKWELL_RIPTIDE_MIXER     )

#define MID_MIDIIN         (MM_ROCKWELL                )
#define PID_MIDIIN         (ROCKWELL_RIPTIDE_MPU401_IN )

#define MID_MIDIOUT        (MM_ROCKWELL                )
#define PID_MIDIOUT        (ROCKWELL_RIPTIDE_MPU401_OUT)

#define MID_SYNTH          (MM_ROCKWELL                )
#define PID_SYNTH          (ROCKWELL_RIPTIDE_SYNTH     )

#define MID_LINEIN         (MM_ROCKWELL                )
#define PID_LINEIN         (ROCKWELL_RIPTIDE_LINE_IN   )

#define MID_CD             (MM_ROCKWELL                )
#define PID_CD             (ROCKWELL_RIPTIDE_CD        )

#define MID_MIC            (MM_ROCKWELL                )
#define PID_MIC            (ROCKWELL_RIPTIDE_MIC       )

#define MID_AUX            (MM_ROCKWELL                )
#define PID_AUX            (ROCKWELL_RIPTIDE_AUX       )

#define MID_VIDEO          (MM_ROCKWELL                )
#define PID_VIDEO          (ROCKWELL_RIPTIDE_VIDEO     )

#define MID_WAVESTREAM     (MM_ROCKWELL                )
#define PID_WAVESTREAM     (ROCKWELL_RIPTIDE_WAVESTREAM)

#define MID_PHONE          (MM_ROCKWELL                )
#define PID_PHONE          (ROCKWELL_RIPTIDE_PHONE     )

#define MID_I2S            (MM_ROCKWELL                )
#define PID_I2S            (ROCKWELL_RIPTIDE_I2S       )


/*
 *  Type of board
 */

//#define RIPTIDE_BOARD_ES1
//#define RIPTIDE_BOARD_A0
//#define RIPTIDE_BOARD_A1

/*
 *  Supported Devices
 */

/*
 *  Number of Wave devices
 */

#if defined(RIPTIDE_BOARD_A1) || defined(RIPTIDE_BOARD_A15)
#define NUMBER_WAVE_OUT 10
#else
#define NUMBER_WAVE_OUT 4
#endif
#define NUMBER_WAVE_IN  1

/*
 *
 */

#ifdef  SUPPORT_WAVESTREAM
#define WSTREAM
#endif

#ifndef SUPPORT_LEGACY_DEVICES
#ifdef SUPPORT_MIDI_IN
#undef SUPPORT_MIDI_IN
#endif
#ifdef SUPPORT_MIDI_OUT
#undef SUPPORT_MIDI_OUT
#endif
#ifdef SUPPORT_SYNTH
#undef SUPPORT_SYNTH
#endif
#ifdef SUPPORT_SOUNDBLASTER
#undef SUPPORT_SOUNDBLASTER
#endif
#ifdef SUPPORT_JOYSTICK
#undef SUPPORT_JOYSTICK
#endif
#endif

/*
 *  Board options
 */

#define RIPTIDE_DUPLEX                  /* Comment this for half-duplex */

#define DTYPE_RIPTIDE	1
#define DTYPE_ICH		2

typedef struct _HwInfo
{
	struct _HwInfo			*next;			/* pointer to next present RipTide device*/

/* Hardware information */
    USHORT                  deviceId;		/* type of device: 1- RipTide, 2 - ICH*/
    HW_PORT                 hwIOBase;       /* Riptide Base I/O address*/
    unsigned int            hwIOSynth;      /* Riptide Synth I/O address*/
    unsigned int            hwIOMidi;       /* Riptide MIDI I/O address*/
    unsigned int            hwIrq;          /* RipTide IRQ */

/* device information */	
//    int                     dev_audio;      /* audio driver*/
//    int                     dev_mixer;      /* mixer driver*/
//    int                     dev_midi;       /* midi driver*/
//    int                     dev_dmfm;       /* FM driver*/

/* RipTide specific structures*/
    USHORT                  VersionASIC;
    USHORT                  VersionCODEC;
    USHORT                  VersionAUXDSP;
    USHORT                  VersionPROG;
    
/* Digital mixers value table:*/
/* {hardware digital mixer control, left channel value, right channel value}*/
    ULONG                   DigitalMixers[DM_MAX_NUM][DM_RVALUE+1]; /* Digital mixers table*/
    MixerTab                Mixers[MAX_MIXER_CONTROLS];             /* Mixer value table*/
    
    ULONG                   ulCurrentChannel;       /* current channel*/
    UCHAR                   LbusSinks[E2SINK_MAX];  /* LBUS sink usage table*/
    
/*    PKSPIN_LOCK             ChannelSync[CHANNEL_ID_COUNTER];  // channel access spin lock
    KIRQL                   Irql[CHANNEL_ID_COUNTER];
    UCHAR                   cmdIf[CHANNEL_ID_COUNTER]; // which command interface*/

    UCHAR                   *LbusPath[CHANNEL_ID_COUNTER];   // list of channel paths
    struct _ChannelInfo     *Channels[CHANNEL_ID_COUNTER];   // list of channel paths

/*    UCHAR                   SRCNum[CHANNEL_ID_COUNTER][2]; // list of SRC for channel*/
/*    UCHAR                   DMIXNum[CHANNEL_ID_COUNTER];   // list of digital mixers*/
/*    UCHAR                   MixerControls[CHANNEL_ID_COUNTER];*/
    
    WORD                    CoeffBass, Coeff3D;
    tCMDIF_AB               *tCmdIf;                /* pointer to intyerface register structure */
    int                     isStarted;              /* Mixer timeout start flag: FALSE - when not started */
    system_objects          *sysObj;                /* structure with system objects - spinlock, timer abd etc.*/
} HwInfo, *PHwInfo;


typedef struct _ChannelInfo
{
    PHwInfo                 pHwInfo;            /* pointer to hardware info */
    int                     Capture;            /* capture or render sign */

    int                     nChannels;          /* number of channels */
    unsigned long           SamplesPerSec;      /* sample rate */
    int                     BitsPerSample;      /* bits per sample */

    int                     DMAType;            /* 0 - system, 1 - scatter/gather */
    PUCHAR                  DMABuffer;          /* address of buffer for system DMA */
    ULONG                   DMABufferSize;      /* size fo buffer for system DMA */
    int                     DMAFragmentSize;    /* size of single DMA buffer */
    int                     DMAFragmentNum;     /* number of DMA buffers (usually 2) */
    int                     DMAFragmentMax;     /* max number of DMA buffers (0x7FFF - no limit)*/
    int                     DMABufferOffset;    /* offset in current DMA buffers is being filled */

    DWORD                   dwLinkHead;         /* physical address of first SGD */
    ULONG                   Sgd_In;             /* Head number in DMA descriptors round buffer */
    ULONG                   Sgd_Out;            /* Tail number in DMA descriptors round buffer */
    ULONG                   Total_In;           /* total fragments in */
    ULONG                   Total_Out;          /* Total fragments out */

    ULONG                   ulIOCounter;        /* Counter of data is already processed */
    ULONG                   ulIOSize;           /* Size of data in buffer */

    ULONG                   ulHead;             /* DMA buffer head */
    ULONG                   ulTail;             /* DMA buffer tail */
    
    UCHAR                   ucChannelState;     /* State of the channel */ 
    UCHAR                   ucChannelDir;       /* Direction of the channel */
    PSGD_DMA                sgdList;            /* DMA descriptors round buffer */
	
    BYTE                    ChannelId;          /* channel identificator*/
    UCHAR                   SRCNum[2];          /* list of SRC for channel */
    UCHAR                   DMIXNum;            /* digital mixers for channel */
    UCHAR                   cmdIf;              /* interface to connect to channel */
    UCHAR                   MixerControls;
    int                     NullSample;
    void                    *pContext;
} ChannelInfo, *PChannelInfo;


#define NOTIFICATION_TIME (8)
#define CYCLIC_DMA_BUFFER_SIZE (0x8000L)

#endif /* __RIPTIDE_H_INCLUDED */
