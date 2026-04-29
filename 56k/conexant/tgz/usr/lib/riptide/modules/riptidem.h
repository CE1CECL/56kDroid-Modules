/*
 * Copyright (c) 2003 Linuxant inc.
 * Copyright (c) 2001-2003 Conexant Systems, Inc.
 *
 * NOTE: The use and distribution of this software is governed by the terms in
 * the file LICENSE, which is included in the package. You must read this and
 * agree to these terms before using or distributing this software.
 *
 */
#ifndef _RIPTIDEM_H_
#define _RIPTIDEM_H_

#if defined(CONFIG_SOUND) || defined(CONFIG_SOUND_MODULE)
#define SUPPORT_SOUNDCORE

#define SUPPORT_WAVE_IN
#define SUPPORT_WAVE_OUT
#define SUPPORT_MIXER

#define SUPPORT_MICROPHONE
#define SUPPORT_CD_ROM
#define SUPPORT_LINE_IN
#define SUPPORT_VIDEO
#define SUPPORT_AUX
#endif

#if defined(CONFIG_SOUND_OPL3SA1) || defined(CONFIG_SOUND_OPL3SA1_MODULE) || \
		defined(CONFIG_SOUND_OPL3SA2) || defined(CONFIG_SOUND_OPL3SA2_MODULE) ||\
		defined(CONFIG_SOUND_YM3812) || defined(CONFIG_SOUND_YM3812_MODULE) ||\
		defined(CONFIG_SOUND_ADLIB) || defined(CONFIG_SOUND_ADLIB_MODULE) ||\
		defined(CONFIG_SOUND_YMFPCI_LEGACY)
#define SUPPORT_LEGACY_DEVICES
#define SUPPORT_SYNTH
#endif

#if defined(CONFIG_SOUND_MIDI) || \
		defined(CONFIG_SOUND_MPU401) || defined(CONFIG_SOUND_MPU401_MODULE)
#define SUPPORT_MIDI_IN
#define SUPPORT_MIDI_OUT
#endif

#if defined(CONFIG_JOYSTICK) || defined(CONFIG_JOYSTICK_MODULE)
#define SUPPORT_JOYSTICK
#endif

#define SUPPORT_MODEM

#if DBG

#define mdprintf(t) printk t
#define mdprintf2(t) printk t

#else

#define mdprintf(t)
#define mdprintf2(t)

#endif

typedef struct _rt_func_table
{
    void * (*RipTideNextChannel)(void *pHwInfo, void **pContext);
    BOOLEAN (*RipTideQueryInterrupt)(
				void *pSGC, system_objects *sysObj
				);
    VOID    (*RipTideHwCompleteInterrupt)(
                void *pHwInfo);
    VOID    (*RipTideDeleteChannel)( 
                void *pChannelInfo, system_objects *sysObj
                );
    void *  (*RipTideCreateChannel)(
			    void   *pHwInfo,
			    int     iCapture,
			    void   *pContext, system_objects *sysObj);
    size_t  (*RipTideAddBuffer)( 
			    void     *pChannelInfo, 
			    PVOID     pData, 
			    ULONG     ulByteCount 
			);
    BOOLEAN (*RipTideHwStartDMA)( 
				void *pChannelInfo 
			);
    BOOLEAN (*RipTideHwStopDMA)( 
				void *pChannelInfo 
			); 
    ULONG   (*RipTideHwReadDMACounter)( 
				void *pChannelInfo 
				); 
    void    (*RipTideGetFragment)(
				void *pChannelInfo,
				int  *fragsize,
				int  *fragstotal
				); 
    void    (*RipTideSetFragment)(
				void *pChannelInfo,
				int   fragsize,
				int   fragstotal
				); 
	int     (*RipTideQueryBusy)(
			    void *pChannelInfo
			);
	int     (*RipTideQueryIdle)(
			    void *pChannelInfo
			);                
    void    (*RipTideSetSampleRate)(
			    void *pChannelInfo,
			    int   SamplesRate
			);
    int     (*RipTideGetSampleRate)(
			    void *pChannelInfo
			);
    void    (*RipTideSetBitsPerSample)(
			    void *pChannelInfo,
			    int   BitsPerSample
			);
	int     (*RipTideGetBitsPerSample)(
			    void *pChannelInfo
		    );
    void    (*RipTideSetChannels)(
			    void *pChannelInfo,
			    int   Channels
			);
	int     (*RipTideGetChannels)(
		        void *pChannelInfo
			);
    BOOLEAN (*RipTideHwGetMixer)( 
				PVOID     pHwInfo, 
				WORD      wId, 
				WORD      *pValue);
	BOOLEAN	(*RipTideHwSetMixer)( 
				PVOID     pHwInfo, 
				WORD      wId, 
				WORD      *pValue 
				);
	void	(*RipTideHwRelease)
			(   void *pHwInfo
			);
    PVOID   (*RipTideHwGetDMABuffer)(
		        void *pChannelInfo,
		        int  *size
			);
    

} rt_func_table;



typedef struct _rt_state
{
	/* we keep cm cards in a linked list */
	struct _rt_state *next;

    int dev_type;       /* Type of device: 0 - RipTide, 1 - ICH */
	/* soundcore stuff */
	int dev_audio;
	int dev_mixer;
	int dev_midi;
	int dev_dmfm;

    unsigned int            hwIOBase;       /* Riptide Base I/O address*/
    unsigned int            hwIOBase2;       /* Riptide Base I/O address*/
    unsigned int            hwIOSynth;      /* Riptide Synth I/O address*/
    unsigned int            hwIOMidi;       /* Riptide MIDI I/O address*/
    unsigned int            hwIrq;          /* RipTide IRQ */

    struct tq_struct        run_bh;
    int                     modify_counter;

    rt_func_table           *func_table;
	void                    *pHwInfo;
#ifdef MONOLIT
    void                    (*clean)(void);
#else
	char                    *modulename;
#endif
    void                    (*open)(void);
    void                    (*close)(void);
    void                    *pHwContext;
#if defined(SUPPORT_MIDI_IN) || defined (SUPPORT_MIDI_IN)
    int                     midiSlot;
#endif
    struct pci_dev          *pcidev;
} rt_state;

typedef void (*interrupt_handler)(int irq, void *dev_id, struct pt_regs *regs);

#ifdef SUPPORT_MIXER
#include "rtmixer.h"
//==================================
// Multiplexor Selects
#define RWASM_MIC    0
#define RWASM_CD     1         
#define RWASM_VIDEO  2
#define RWASM_AUX    3
#define RWASM_LINE   4
#define RWASM_MIXER  5
#define RWASM_MONO   6
#define RWASM_PHN    7    
#endif

#endif

