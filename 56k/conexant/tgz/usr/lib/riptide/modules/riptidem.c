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
  MODULE NAME:     riptidem.c                       

  AUTHOR:          Konstantin Lazarev    (Transcribed by KNL)

  HISTORY:         Major Revision               Date        By
            -----------------------------     --------     -----
            Created                           09/01/99      KNL

  DESCRIPTION:

    This module is core Linux driver for Conexant RipTide and
    Conexant ICH Audio Controlers.
    
  NOTES:    None.
*/

#include <linux/config.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/sound.h>
#include <linux/slab.h>
#include <linux/soundcard.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <linux/init.h>
#include <linux/poll.h>
/*#include <asm/spinlock.h>*/
#include <asm/uaccess.h>
#include <asm/hardirq.h>
#include <linux/wait.h>
#include <linux/kmod.h>

#undef SUPPORT_DSP_DUPLEX
#define  SUPPORT_DSP_TRIGGER
#undef SUPPORT_DSP_BATCH
#define  SUPPORT_DSP_MMAP
#define  SUPPORT_DSP_REALTIME

/* --------------------------------------------------------------------- */

#undef OSS_DOCUMENTED_MIXER_SEMANTICS

/* - RipTide hardware section ------------------------------------------ */
#undef DBG

#include "linux.h"
#include "riptidem.h"

//#include <../drivers/sound/soundmodule.h>
#include <../drivers/sound/sound_config.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
#include <../drivers/sound/mpu401.h>
#else
#if 0
void attach_mpu401(struct address_info * hw_config);
int probe_mpu401(struct address_info *hw_config);
int intchk_mpu401(void *dev_id);
void mpuintr(int irq, void *dev_id, struct pt_regs * dummy);
#endif
#endif

/* --------------------------------------------------------------------- */


#define SOUND_MIXER_AUX         SOUND_MIXER_LINE1
#define SOUND_MASK_AUX          (1<<SOUND_MIXER_AUX)

#define SOUND_MIXER_I2S         SOUND_MIXER_LINE3
#define SOUND_MASK_I2S          (1<<SOUND_MIXER_I2S)

#define SOUND_MIXER_WAVESTREAM  SOUND_MIXER_ALTPCM
#define SOUND_MASK_WAVESTREAM   (1<<SOUND_MIXER_WAVESTREAM)

#define SOUND_MIXER_MODEM       SOUND_MIXER_SPEAKER
#define SOUND_MASK_MODEM        (1<<SOUND_MIXER_MODEM)


/* Table to map Linux OSS input source numbers into hardware audio source numbers */
const int MixerInputLines[8] =
{
    SOUND_MASK_MIC,         /*#define RWASM_MIC    0*/
    SOUND_MASK_CD,          /*#define RWASM_CD     1*/         
    SOUND_MASK_LINE2,       /*#define RWASM_VIDEO  2*/
    SOUND_MASK_AUX,         /*#define RWASM_AUX    3*/
    SOUND_MASK_LINE,        /*#define RWASM_LINE   4*/
    SOUND_MASK_PCM,         /*#define RWASM_MIXER  5*/
    SOUND_MASK_PCM,         /*#define RWASM_MONO   6*/
    SOUND_MASK_PCM,         /*#define RWASM_PHN    7*/  
};

	int dev_audio = 0;
	int dev_mixer = 0;
	int dev_midi  = 0;
	int dev_dmfm  = 0;

#if 0
struct cm_state {
	/* magic */
	unsigned int magic;

	/* we keep cm cards in a linked list */
	struct cm_state *next;

	/* soundcore stuff */
	int dev_audio;
	int dev_mixer;
	int dev_midi;
	int dev_dmfm;

	/* hardware resources */
	unsigned int iosb, iobase, iosynth, iomidi, iogame, irq;

        /* mixer stuff */
        struct {
                unsigned int modcnt;
#ifndef OSS_DOCUMENTED_MIXER_SEMANTICS
		unsigned short vol[13];
#endif /* OSS_DOCUMENTED_MIXER_SEMANTICS */
        } mix;

	/* wave stuff */
	unsigned int rateadc, ratedac;
	unsigned char fmt, enable;

	spinlock_t lock;
	struct semaphore open_sem;
	mode_t open_mode;
	wait_queue_head_t open_wait;

	struct dmabuf {
		void *rawbuf;
		unsigned buforder;
		unsigned numfrag;
		unsigned fragshift;
		unsigned hwptr, swptr;
		unsigned total_bytes;
		int count;
		unsigned error; /* over/underrun */
		wait_queue_head_t wait;
		/* redundant, but makes calculations easier */
		unsigned fragsize;
		unsigned dmasize;
		unsigned fragsamples;
		/* OSS stuff */
		unsigned mapped:1;
		unsigned ready:1;
		unsigned endcleared:1;
		unsigned ossfragshift;
		int ossmaxfrags;
		unsigned subdivision;
	} dma_dac, dma_adc;

	/* midi stuff */
	struct {
		unsigned ird, iwr, icnt;
		unsigned ord, owr, ocnt;
		wait_queue_head_t iwait;
		wait_queue_head_t owait;
		struct timer_list timer;
		unsigned char ibuf[MIDIINBUF];
		unsigned char obuf[MIDIOUTBUF];
	} midi;
};
#endif

/* Channel state structute.
 * OS depended.
 * It is created for each open stream 
 */
#define DMA_MAP     0x8000;

typedef struct _channel_state
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
    struct                  wait_queue *wait;   /* wait queue to wait for room on */
#else
    wait_queue_head_t wait;   /* wait queue to wait for room on */
#endif
    int                     DMAState;           /* state of DMA channel */
    int                     DMABufferSize;      /* buffer size */
    int                     int_count;          /* fragment counter */
    unsigned long           DMABuffer;          /* buffer address */
    rt_state                *newdev;            /* pointer to driver state structure (see riptidem.h)*/
    void                    *pChannelInfo;      /* pointer to OS undepended channel descriptor */
    system_objects          sysObj;
} channel_state;

/* --------------------------------------------------------------------- */

rt_state *devs = NULL;              /* pointer to first of driver state structure */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,9)
/* --------------------------------------------------------------------- */
/*************************************************************************
 *  Name:           static loff_t rt_llseek(struct file *file, loff_t offset, int origin)
 *
 *  Parameters:     file   - Pointer to file descriptor
 *                  offset - offset in file
 *                  origin - 'offset from' point
 *
 *  Returns:        0           - if successful
 *                  otherwise   - error
 *
 *  Description:
 *    The function is not used for audio devices
 *
 *************************************************************************/
static loff_t rt_llseek(struct file *file, loff_t offset, int origin)
{
	return -ESPIPE;
}
#endif

/* --------------------------------------------------------------------- */


/* --------------------------------------------------------------------- */
#ifdef SUPPORT_MIXER

/*************************************************************************
 *  Name:           int mixer_ioctl(rt_state *newdev, unsigned int cmd, unsigned long arg)
 *
 *  Parameters:     newdev - Pointer to driver state structure
 *                  cmd    - command
 *                  arg    - argument
 *
 *  Returns:        0           - if successful
 *                  otherwise   - error
 *
 *  Description:
 *    The function performs mixer related IOCTLs
 *
 *************************************************************************/
int mixer_ioctl(rt_state *newdev, unsigned int cmd, unsigned long arg)
{
void *pHwInfo = newdev->pHwInfo;
int read = -1, ret = 0, mono = 0 ;
WORD wValue = 0, wId = (WORD)(-1);
WORD mutewId = 0 ;
ULONG RecMask[] = 
{
    SOUND_MASK_MIC,
    SOUND_MASK_CD,
    SOUND_MASK_LINE2,
    SOUND_MASK_AUX,
    SOUND_MASK_LINE,
    SOUND_MASK_PCM,
    SOUND_MASK_PCM,
    SOUND_MASK_PCM
};
char *iocode = NULL;
WORD Value[2], muteValue[2];

    mdprintf2(("<RIPTIDE> mixer_ioctl started: %p\n", pHwInfo));
  	if (_SIOC_DIR(cmd) & _SIOC_WRITE)
    	newdev->modify_counter++;

    switch(cmd)
    {
        case SOUND_MIXER_INFO:
		{
			mixer_info info;

			strncpy(info.id, "RipTide", sizeof(info.id));
			strncpy(info.name, "Conexant RipTide", sizeof(info.name));
			info.name[sizeof(info.name)-1] = 0;
			info.modify_counter = newdev->modify_counter;
			if (__copy_to_user((mixer_info *)arg, &info,  sizeof(info)))
				ret = -EFAULT;
			else
			    ret = 0;

			break;
		}        
        case SOUND_MIXER_WRITE_VOLUME:
            read = 0;
            iocode = "SOUND_MIXER_WRITE_VOLUME";
            wId = MXMIX_MASTER;
            break;
        case SOUND_MIXER_READ_VOLUME:
            read = 1;
            iocode = "SOUND_MIXER_READ_VOLUME";
            wId = MXMIX_MASTER;
            break;
        case SOUND_MIXER_WRITE_BASS:
        case SOUND_MIXER_READ_BASS:
        case SOUND_MIXER_WRITE_TREBLE:
        case SOUND_MIXER_READ_TREBLE:
            iocode = "SOUND_MIXER_BASS/TREBLE";
            break;
#ifdef SUPPORT_SYNTH
        case SOUND_MIXER_WRITE_SYNTH:
            read = 0;
            iocode = "SOUND_MIXER_WRITE_SYNTH";
            wId = MXMIX_OPL;
            break;
        case SOUND_MIXER_READ_SYNTH:
            read = 1;
            iocode = "SOUND_MIXER_READ_SYNTH";
            wId = MXMIX_OPL;
            break;
#endif
#ifdef SUPPORT_WAVE_OUT
        case SOUND_MIXER_WRITE_PCM:
            read = 0;
            wId = PEAK_WAVE;
            iocode = "SOUND_MIXER_WRITE_PCM";
            break;
        case SOUND_MIXER_READ_PCM:
            read = 1;
            wId = PEAK_WAVE;
            iocode = "SOUND_MIXER_READ_PCM";
            break;
#endif
#ifdef SUPPORT_MODEM
        case SOUND_MIXER_WRITE_SPEAKER:
	    read = 0 ;
	    wId = MXMIX_PHONE ;
	    mono = 1 ;
	    iocode = "SOUND_MIXER_WRITE_SPEAKER" ;
	    break;
        case SOUND_MIXER_READ_SPEAKER:
	    read = 1 ;
	    wId = MXMIX_PHONE ;
	    mono = 1 ;
            iocode = "SOUND_MIXER_READ_SREAKER";
            break;
#endif
#ifdef SUPPORT_LINE_IN
        case SOUND_MIXER_WRITE_LINE:
            read = 0;
            wId = MXMIX_LINE;
            iocode = "SOUND_MIXER_WRITE_LINE";
            break;
        case SOUND_MIXER_READ_LINE:
            read = 1;
            wId = MXMIX_LINE;
            iocode = "SOUND_MIXER_READ_LINE";
            break;
#endif
#ifdef SUPPORT_MICROPHONE
        case SOUND_MIXER_WRITE_MIC:
            read = 0;
            wId = MXMIX_MIC;
	    mono = 1 ;
            iocode = "SOUND_MIXER_WRITE_MIC";
            break;
        case SOUND_MIXER_READ_MIC:
            read = 1;
            wId = MXMIX_MIC;
	    mono = 1 ;
            iocode = "SOUND_MIXER_READ_MIC";
            break;
#endif
#ifdef SUPPORT_CD_ROM
        case SOUND_MIXER_WRITE_CD:
            read = 0;
            wId = MXMIX_CD;
            iocode = "SOUND_MIXER_WRITE_CD";
            break;		    
        case SOUND_MIXER_READ_CD:
            read = 1;
            wId = MXMIX_CD;
            iocode = "SOUND_MIXER_READ_CD";
            break;		    
#endif
#ifdef SUPPORT_WAVE_IN
        case SOUND_MIXER_WRITE_RECLEV:
            read = 0;
            wId = MXMUX_MASTER;
            iocode = "SOUND_MIXER_WRITE_RECLEV";
            break;
        case SOUND_MIXER_READ_RECLEV:
            read = 1;
            wId = MXMUX_MASTER;
            iocode = "SOUND_MIXER_READ_RECLEV";
            break;
#endif
#ifdef SUPPORT_VIDEO
        case MIXER_WRITE(SOUND_MIXER_LINE2):
            read = 0;
            iocode = "SOUND_MIXER_WRITE_LINE2";
            wId = MXMIX_VIDEO;
            break;
       case MIXER_READ(SOUND_MIXER_LINE2):
            read = 1;
            iocode = "SOUND_MIXER_READ_LINE2";
            wId = MXMIX_VIDEO;
            break;
#endif
#ifdef SUPPORT_AUX
        case SOUND_MIXER_WRITE_LINE1:
            read = 0;
            iocode = "SOUND_MIXER_WRITE_LINE1";
            wId = MXMIX_AUX;
            break;
        case SOUND_MIXER_READ_LINE1:
            read = 1;
            iocode = "SOUND_MIXER_READ_LINE1";
            wId = MXMIX_AUX;
            break;
#endif
#ifdef SUPPORT_I2S
        case SOUND_MIXER_WRITE_LINE3:
        case SOUND_MIXER_READ_LINE3:
            break;
#endif
/* Obsolete macros */
/*
        case SOUND_MIXER_READ_MUTE:
            break;
        case SOUND_MIXER_READ_ENHANCE:
            break;
        case SOUND_MIXER_READ_LOUD:
            break;
*/
        case SOUND_MIXER_READ_DEVMASK:
        {
            int devmask = 0;
            devmask = 0
#ifdef SUPPORT_WAVE_IN
                |   SOUND_MASK_RECLEV
#endif
#ifdef SUPPORT_WAVE_OUT
                |   SOUND_MASK_PCM
#endif
#ifdef SUPPORT_MICROPHONE
                |   SOUND_MASK_MIC
#endif
#ifdef SUPPORT_CD_ROM
                |   SOUND_MASK_CD
#endif
#ifdef SUPPORT_LINE_IN
                |   SOUND_MASK_LINE
#endif
#ifdef SUPPORT_VIDEO
                |   SOUND_MASK_LINE2
#endif
#ifdef SUPPORT_AUX
                |   SOUND_MASK_AUX
#endif
#ifdef SUPPORT_I2S
                |   SOUND_MASK_I2S
#endif
#ifdef SUPPORT_SYNTH
                |   SOUND_MASK_SYNTH
#endif
#ifdef SUPPORT_WAVESTREAM
                |   SOUND_MASK_WAVESTREAM
#endif
#ifdef SUPPORT_MODEM
                |   SOUND_MASK_MODEM
#endif
                |   SOUND_MASK_VOLUME;
//#define SUPPORT_MIXER

// Legacy devices

//#define SUPPORT_LEGACY_DEVICES

//#define SUPPORT_MIDI_IN
//#define SUPPORT_MIDI_OUT
//#define SUPPORT_JOYSTICK
//#define SUPPORT_SOUNDBLASTER
            iocode = "SOUND_MIXER_WRITE_DEVMASK";
            ret = put_user(devmask, (int *)arg);
            break;
        }
        case SOUND_MIXER_READ_STEREODEVS:
        {
            int devmask;
            devmask =   0
                |   SOUND_MASK_RECLEV
                |   SOUND_MASK_PCM
//                |   SOUND_MASK_MIC
                |   SOUND_MASK_CD
                |   SOUND_MASK_LINE
                |   SOUND_MASK_LINE1
                |   SOUND_MASK_LINE2
                |   SOUND_MASK_SYNTH
                |   SOUND_MASK_ALTPCM
//                |   SOUND_MASK_MODEM
                |   SOUND_MASK_VOLUME;
            iocode = "SOUND_MIXER_READ_STEREODEVS";
            ret = put_user(devmask, (int *)arg);
            break;
        }
        case SOUND_MIXER_READ_CAPS:
        {
            int devmask = SOUND_CAP_EXCL_INPUT;

            iocode = "SOUND_MIXER_READ_CAPS";
            ret = put_user(devmask, (int *)arg);
            break;
        }

/* Obsolete macros */
/*
        case SOUND_MIXER_WRITE_MUTE:
            break;
        case SOUND_MIXER_WRITE_ENHANCE:
            break;
        case SOUND_MIXER_WRITE_LOUD:
            break;
*/
        case SOUND_MIXER_READ_RECMASK:
        {
            int devmask = 0;
            iocode = "SOUND_MIXER_READ_RECMASK";
            devmask = 0
#ifdef SUPPORT_WAVE_IN
                |   SOUND_MASK_PCM
#endif
#ifdef SUPPORT_MICROPHONE
                |   SOUND_MASK_MIC
#endif
#ifdef SUPPORT_CD_ROM
                |   SOUND_MASK_CD
#endif
#ifdef SUPPORT_LINE_IN
                |   SOUND_MASK_LINE
#endif
#ifdef SUPPORT_VIDEO
                |   SOUND_MASK_LINE2
#endif
#ifdef SUPPORT_AUX
                |   SOUND_MASK_AUX
#endif
                |   0;
            ret = put_user(devmask, (int *)arg);
            break;
        }
       case SOUND_MIXER_WRITE_RECSRC:
            read = 0;
       case SOUND_MIXER_READ_RECSRC:
        {
            iocode = "SOUND_MIXER_READ_RECSRC";
            if((*newdev->func_table->RipTideHwGetMixer)(
	                pHwInfo,
	                MXMUX_SELECT,
	                &Value[0]))
            {
                wValue = RecMask[Value[0] & 0x7];
                mdprintf2(("<RIPTIDE> Rec source: %x\n", wValue));
                if(read)
                {
                    ret = put_user(wValue, (int *)arg);
                    break;
                }
            }
            else
            {
                ret = -EINVAL;
                break;
            }
        }

        {
            read = -1;
            iocode = "SOUND_MIXER_WRITE_RECSRC";
            get_user_ret(wId, (int *)arg, -EFAULT);
            mdprintf2(("<RIPTIDE> Mask = %x %x\n", wId, wValue));

            if(wValue != wId)
                wValue ^= wId;
            
            wId = RWASM_MIC;

            if(wValue & SOUND_MASK_PCM)
                wId = RWASM_MIXER;
               
#ifdef SUPPORT_CD_ROM
            if(wValue & SOUND_MASK_CD)
                wId = RWASM_CD;
#endif
#ifdef SUPPORT_LINE_IN
            if(wValue & SOUND_MASK_LINE)
                wId = RWASM_LINE;
#endif
#ifdef SUPPORT_VIDEO
            if(wValue & SOUND_MASK_LINE2)
                wId = RWASM_VIDEO;
#endif
#ifdef SUPPORT_AUX
           if(wValue & SOUND_MASK_AUX)
                wId = RWASM_AUX;
#endif
            Value[0] = Value[1] = wId;
            (*newdev->func_table->RipTideHwSetMixer)(
                pHwInfo,
                MXMUX_SELECT,
                &Value[0]);
            ret = 0;
            break;
        }
        default:
            iocode = "SOUND_MIXER_UNKNOWN";
            ret = -EINVAL;
    }

    mdprintf2(("<RIPTIDE> mixer ioctl: %s\n", iocode));
    switch(read)
    {
	case 0:
	    {
	    if( wId != MXMUX_MASTER ) {
		mutewId = wId - MXMIX_MASTER + MXMUTE_MASTER ;
	    } else {
		mutewId = MXMUTE_MUX ;
	    }
            if((*newdev->func_table->RipTideHwGetMixer)(
	                pHwInfo,
	                mutewId,
	                &muteValue[0]) == FALSE)
            {
                mdprintf(("<RIPTIDE> ERROR!!! Can't get mute for %d\n", wId));
                ret = -1;
                break;
            }
	    }
        case 1:
	    {
	        if(wId != (WORD)(-1))
	        {
	            if((*newdev->func_table->RipTideHwGetMixer)(
	                pHwInfo,
	                wId,
	                &Value[0]))
	            {
			if ( mono ) { 
	                    wValue = ((((Value[1] >> 8) & 0xFF) * 0x64 / 0xFF) << 8) | (((Value[1] >> 8) * 0x64 / 0xFF) & 0xFF);
			} else {
	                    wValue = ((((Value[1] >> 8) & 0xFF) * 0x64 / 0xFF) << 8) | (((Value[0] >> 8) * 0x64 / 0xFF) & 0xFF);
			}
  	                mdprintf2(("<RIPTIDE> Get mixer %d value %x (%x:%x)\n", wId, wValue, Value[0], Value[1]));
  	                if(read)
	                    ret = put_user(wValue, (int *)arg);
	            }
	        }
	        if(read)
	           break;
	    }
	    {
	        if(wId != (WORD)(-1))
	        {
	            WORD wOldValue;

	            wOldValue = wValue;
                get_user_ret(wValue, (int *)arg, -EFAULT);

                mdprintf2(("<RIPTIDE> Old: %x new: %x\n", wOldValue, wValue));
	            if(muteValue[0])
	            {
	                if(wValue)
	                {
	                    muteValue[0] = muteValue[1] = 0;
	                    mdprintf2(("<RIPTIDE> UNMUTE\n"));
	                }
	            }
	            
                if(wValue == 0)
                {
                    if((wOldValue&0xFF) > 1 || (wOldValue&0xFF00) > 0x100)
                    {
                        mdprintf2(("<RIPTIDE> MUTE\n"));
                        muteValue[0] = muteValue[1] = 0xFFFF;
	                }
                }
                mdprintf2(("<RIPTIDE> Set mute for %d value %x %x\n", wId, muteValue[0], muteValue[1]));
                if((*newdev->func_table->RipTideHwSetMixer)(
			                pHwInfo,
			                mutewId,
			                &muteValue[0]) == FALSE)
			    {
			         mdprintf(("<RIPTIDE> ERROR!!! Can't set mute for %d\n", wId));
			         ret = -1;
			         break;
			    }
	            Value[0] = ((wValue & 0xFF) * 0xFF / 0x64) << 8;
		    if ( mono ) { 
			Value[1]=Value[0] ; 
		    } else {
	                Value[1] = (((wValue >> 8) & 0xFF) * 0xFF / 0x64) << 8;
		    }
	            mdprintf2(("<RIPTIDE> Set mixer %d value %x (%x:%x)\n", wId, wValue, Value[0], Value[1]));
	            if((*newdev->func_table->RipTideHwSetMixer)(
	                pHwInfo,
	                wId,
	                &Value[0]))
	            {
	                ret = 0;
	            }
	        }
	        break;
	    }
    }
    return ret;
}

int init_driver(void);

/*************************************************************************
 *  Name:           static int rt_open_mixdev(struct inode *inode, struct file *file)
 *
 *  Parameters:     inode - Pointer to inode
 *                  file  - pointer to file descriptor
 *
 *  Returns:        0           - if successful
 *                  otherwise   - error
 *
 *  Description:
 *    The function opens mixer device
 *
 *************************************************************************/
static int rt_open_mixdev(struct inode *inode, struct file *file)
{
//	int minor = minor(inode->i_rdev);
	rt_state *s;
    int state = 0;
    
    mdprintf2(("<RIPTIDE> rt_open_mixdev started\n"));
/*	while (s && s->dev_mixer != minor)
		s = s->next;
	if (!s)
		return -ENODEV;
*/
    if(devs == NULL)
        if(init_driver())
        {
            return -ENODEV;
        }
    s = (rt_state *)devs;
   	file->private_data = s;
    if(s->open)
        s->open();
	MOD_INC_USE_COUNT;

    mdprintf2(("<RIPTIDE> rt_open_mixdev ended: %d\n", state));
	return state;
}

/*************************************************************************
 *  Name:           static int rt_release_mixdev(struct inode *inode, struct file *file)
 *
 *  Parameters:     inode - Pointer to inode
 *                  file  - pointer to file descriptor
 *
 *  Returns:        0           - if successful
 *                  otherwise   - error
 *
 *  Description:
 *    The function relizes mixer device
 *
 *************************************************************************/
static int rt_release_mixdev(struct inode *inode, struct file *file)
{
    mdprintf2(("<RIPTIDE> rt_release_mixdev started\n"));
    if(devs->close)
         devs->close();
	MOD_DEC_USE_COUNT;
	return 0;
}

/*************************************************************************
 *  Name:           static int rt_ioctl_mixdev(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
 *
 *  Parameters:     inode - Pointer to inode
 *                  file  - pointer to file descriptor
 *
 *  Returns:        0           - if successful
 *                  otherwise   - error
 *                  cmd    - command
 *                  arg    - argument
 *
 *  Description:
 *    The function is IOCTL front-end for /dev/mixer device
 *
 *************************************************************************/
static int rt_ioctl_mixdev(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    mdprintf2(("<RIPTIDE> rt_ioctl_mixdev: %p\n", (rt_state *)file->private_data));
	return mixer_ioctl((rt_state *)file->private_data, cmd, arg);
}

/* /dev/mixer device file operations table */
static /*const*/ struct file_operations rt_mixer_fops = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	&rt_llseek,
	NULL,  /* read */
	NULL,  /* write */
	NULL,  /* readdir */
	NULL,  /* poll */
	&rt_ioctl_mixdev,
	NULL,  /* mmap */
	&rt_open_mixdev,
	NULL,	/* flush */
	&rt_release_mixdev,
	NULL,  /* fsync */
	NULL,  /* fasync */
	NULL,  /* check_media_change */
	NULL,  /* revalidate */
	NULL,  /* lock */
#else
	owner:      THIS_MODULE,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,9)
	llseek:     rt_llseek,
#else
	llseek:     no_llseek,
#endif
	ioctl:      rt_ioctl_mixdev,
	open:       rt_open_mixdev,
	release:    rt_release_mixdev,
#endif
};

#endif //SUPPORT_MIXER


/* --------------------------------------------------------------------- */
#if defined(SUPPORT_WAVE_IN) || defined(SUPPORT_WAVE_OUT)

/*
void map_buffer(unsigned char *pdata, int count)
{
    int i,size;
    void *pvirt;
    unsigned long block_address, phys_address, prev_address, page_offset;

    block_address = prev_address = virt_to_bus((void *)pdata);
    prev_address = block_address & ~(PAGE_SIZE-1);
    page_offset = block_address - prev_address;
    if(count  < PAGE_SIZE - page_offset)
    {
        size = count;
    }
    else
    {
        size = PAGE_SIZE - page_offset;
        for(i=size; i<count;)
        {
            pvirt = pdata + i;
            phys_address = virt_to_bus(pvirt);
            
            if(phys_address - prev_address == PAGE_SIZE)
            {
                prev_address = phys_address;
                if(count - i > PAGE_SIZE)
                {
                    size += PAGE_SIZE;
                    i += PAGE_SIZE;
                    continue;
                }
                size += count-i;
            }

            Add_Buffer(block_address, size);
            block_address = phys_address;
            if(count - i > PAGE_SIZE)
                size = PAGE_SIZE;
            else
                size = count - i;
        }
    }
    Add_Buffer(block_address, size);
}
*/

/*************************************************************************
 *  Name:           static ssize_t rt_read_dsp(struct file *file, char *buffer, size_t count, loff_t *ppos)
 *
 *  Parameters:     file   - pointer to file descriptor
 *                  buffer - pointer to user data buffer
 *                  count  - user data counter
 *                  ppos   - pointer to current pos  
 *
 *  Returns:        >=0       - if successful, size of read data
 *                  otherwise - error
 *
 *  Description:
 *    The function performs read from /dev/dsp device
 *
 *************************************************************************/
static ssize_t rt_read_dsp(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
    channel_state *newdev = (channel_state *)file->private_data;
	void *s = ((channel_state *)file->private_data)->pChannelInfo;
	ssize_t ret = 0;
	rt_state *rtstate = newdev->newdev;

    //printk("<RIPTIDE> rt_read started\n");
    if(!(file->f_mode & FMODE_READ))
    {
        printk("<RIPTIDE> ERROR!!! Wrong operation\n");
        return -EINVAL;
    }
	if (ppos != &file->f_pos)
	{
	    printk("<RIPTIDE> ERROR!!! Wrong position\n");
		return -ESPIPE;
    }

//		ret = count;

/*    map_buffer(buffer, count);    */
    
    while(count > ret)
    {
        ssize_t size = 0;
        BOOLEAN start = FALSE;

        if((size = (*rtstate->func_table->RipTideAddBuffer)(s, buffer + ret, count - ret)) < 0)
        {
            printk("<RIPTIDE> ERROR!!! AddBuffer error\n");
            ret = -1;
            break;
        }
        ret += size;

        if((newdev->DMAState & PCM_ENABLE_INPUT) && !start) 
        { 
            if(!(start = (*rtstate->func_table->RipTideHwStartDMA)(s)))
            {
                printk("<RIPTIDE> ERROR!!! StartDMA error!!!");
                ret = -1;
                break;
            }
        } 

        if(count > ret)
        {
			if(file->f_flags & O_NONBLOCK) {
				ret = -EAGAIN;
				goto out;
			}

            mdprintf2(("<RIPTIDE> Sleep for awhile: %d > %d\n", count, ret));
            interruptible_sleep_on(&newdev->wait);
            if (signal_pending(current))
            {
                ret = (ret)? ret : -ERESTARTSYS;
                break;
            }
        }
    }

out:
    //printk("<RIPTIDE> rt_read is completed: %d\n", ret);
    return ret;
}    

/*************************************************************************
 *  Name:           static ssize_t rt_write_dsp(struct file *file, char *buffer, size_t count, loff_t *ppos)
 *
 *  Parameters:     file   - pointer to file descriptor
 *                  buffer - pointer to user data buffer
 *                  count  - user data counter
 *                  ppos   - pointer to current pos  
 *
 *  Returns:        >=0       - if successful, size of written data
 *                  otherwise - error
 *
 *  Description:
 *    The function performs write to /dev/dsp device
 *
 *************************************************************************/
static ssize_t rt_write_dsp(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
    channel_state *newdev = (channel_state *)file->private_data;
	void *s = ((channel_state *)file->private_data)->pChannelInfo;
	ssize_t ret = 0;
	rt_state *rtstate = newdev->newdev;

    mdprintf2(("<RIPTIDE> rt_write started: %p\n", s));

    if(!(file->f_mode & FMODE_WRITE))
    {
        printk("<RIPTIDE> ERROR!!! Wrong operation\n");
        return -EINVAL;
    }
	if (ppos != &file->f_pos)
	{
	    printk("<RIPTIDE> ERROR!!! Wrong position\n");
		return -ESPIPE;
    }

    while(count > ret)
    {
        size_t size;
        BOOLEAN start = FALSE;

        mdprintf2(("<RIPTIDE> AddBuffer: %d\n", count - ret));
        if((size = (*rtstate->func_table->RipTideAddBuffer)(s, (char *)buffer + ret, count - ret)) < 0)
        {
            printk("<RIPTIDE> ERROR!!! AddBuffer error\n");
            ret = -1;
            break;
        }
        ret += size;

	        if((newdev->DMAState & PCM_ENABLE_OUTPUT) && !start) 
	        {
//	            printk("<RIPTIDE> StartDMA\n");
	            if(!(start = (*rtstate->func_table->RipTideHwStartDMA)(s)))
	            {
	                printk("<RIPTIDE> ERROR!!! StartDMA error\n");
	                ret = -1;
	                break;
	            }
	        } 

        if(count > ret)
        {
			if(file->f_flags & O_NONBLOCK) {
				ret = -EAGAIN;
				goto out;
			}

            mdprintf2(("<RIPTIDE> Sleep for awhile: %d > %d\n", count, ret));
            interruptible_sleep_on(&newdev->wait);
			if (signal_pending(current))
			{
				ret = (ret)? ret : -ERESTARTSYS;
				break;
            }
        }
    }
  
out:
    mdprintf2(("<RIPTIDE> rt_write ended: %p\n", s));
    return ret;
}

/*************************************************************************
 *  Name:           static ssize_t rt_poll(struct file *file, struct poll_table_struct *wait)
 *
 *  Parameters:     file - pointer to file descriptor
 *                  wait - pointer to polling structure
 *
 *  Returns:        >=0       - if successful, polling mask
 *                  otherwise - error
 *
 *  Description:
 *    The function returns polling state for /dev/dsp device
 *************************************************************************/
static unsigned int rt_poll(struct file *file, struct poll_table_struct *wait)
{
	int minor = minor(file->f_dentry->d_inode->i_rdev);
	channel_state *p = (channel_state *)file->private_data;
	rt_state *newdev = p->newdev;
	void *s = ((channel_state *)file->private_data)->pChannelInfo;
	unsigned int mask = 0;

    switch(minor)
    {
	    case SND_DEV_DSP:
	    case SND_DEV_DSP16:
	    {
			int fragsize, fragstotal, bytes;
		    //printk("<RIPTIDE> ------------------------------ POLL -----------------------------------------\n");
			//printk(KERN_DEBUG"p=%p\n", &p->wait);
		    poll_wait(file, &p->wait, wait);

		/*
		    if(s->Sgd_Out != s->Sgd_Out)*/
            /*if(newdev->DMAState & DMA_MAP_MAPPED) {
	            mask |= POLLOUT | POLLWRNORM;
			} else {*/

	        (*newdev->func_table->RipTideGetFragment)(s, &fragsize, &fragstotal);
		    if (file->f_mode & FMODE_WRITE) {
           		bytes        = (*newdev->func_table->RipTideQueryIdle)(s);
				if(bytes >= fragsize * (fragstotal / 2)) {
					//printk(KERN_DEBUG"%s: FMODE_WRITE bytes=%d fragsize=%d total=%d POLLOUT | POLLWRNORM\n", __FUNCTION__, bytes, fragsize, fragstotal * fragsize);
		           	mask |= POLLOUT | POLLWRNORM;
				}
			}
		    if (file->f_mode & FMODE_READ) {
           		bytes        = (*newdev->func_table->RipTideQueryBusy)(s);
				if(bytes >= fragsize) {
					mask |= POLLIN | POLLRDNORM;
					//printk(KERN_DEBUG"%s: FMODE_READ bytes=%d fragsize=%d total=%d POLLIN | POLLRDNORM\n", __FUNCTION__, bytes, fragsize, fragstotal * fragsize);
				}
			}
		 	return mask;
        }
    }
    return -EINVAL;
}

#ifdef SUPPORT_DSP_MMAP

#if 0
/*
 * retrieve a pointer to init_mm, which is not exported
 */

static struct mm_struct *init_mm_ptr;

static void retrieve_init_mm_ptr(void)
{
    struct task_struct *p;

    for (p = current ; (p = p->next_task) != current ; )
        if (p->pid == 0)
            break;

    init_mm_ptr = p->mm;
}
#endif

/*
 * open and close: just keep track of how many times the device is
 * mapped, to avoid releasing it.
 */

void rt_vma_open(struct vm_area_struct *vma)
{
    MOD_INC_USE_COUNT;
}

void rt_vma_release(struct vm_area_struct *vma)
{
    MOD_DEC_USE_COUNT;
}

/*
 * The nopage method: the core of the file. It retrieves the
 * page required from the scullv device and returns it to the
 * user. The count for the page must be incremented, because
 * it is automatically decremented at page unmap.
 *
 * There's no limit to order (unlike scullp) becase
 * vmalloc() builds regions from singe pages
 */

static
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
#define VM_OFFSET(x) (x)->vm_offset
unsigned long 
#else
#define VM_OFFSET(x) ((x)->vm_pgoff << PAGE_SHIFT)
struct page *
#endif
rt_vma_nopage(struct vm_area_struct *vma,
                                unsigned long address, int write)
{
    unsigned long offset = address - vma->vm_start + VM_OFFSET(vma);
    channel_state *dev = (channel_state *)vma->vm_file->private_data;
    void *pageptr = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
    unsigned long dmapage;
#else
    struct page *dmapage;
#endif
//    pgd_t *pgd; pmd_t *pmd; pte_t *pte;

    printk("<RIPTIDE> nopage: %lx (%lx)\n", address, offset);

    if (offset >= dev->DMABufferSize)
    {
        printk("<RIPTIDE> ERROR!!! Offset is out of buffer: %lx >= %x\n", offset, dev->DMABufferSize);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
        return 0; /* out of range: send SIGBUS */
#else
		return NOPAGE_SIGBUS;
#endif
    }

    if(offset & (PAGE_SIZE-1))
    {
        printk("<RIPTIDE> ERROR!!! Offset: %lx\n", offset);
        offset &= ~(PAGE_SIZE-1);
    }

    pageptr = ((char *)dev->DMABuffer)+offset;

    /*
     * "page" is now the address of the page
     * needed by the current process. Since it's a vmalloc address,
     * first retrieve the unsigned long value to be looked up
     * in page tables.
     */
    printk("<RIPTIDE> pageptr: %p\n", pageptr);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
    dmapage = bus_to_virt(kvirt_to_bus((unsigned long)pageptr));

#if 0     
    page = VMALLOC_VMADDR(pageptr);

    printk("<RIPTIDE> page: %lx\n", page);
    pgd = pgd_offset(init_mm_ptr, page);
    printk("<RIPTIDE> pgd: %p\n", pgd);
    pmd = pmd_offset(pgd, page);
    printk("<RIPTIDE> pmd: %p\n", pmd);
    pte = pte_offset(pmd, page);
    printk("<RIPTIDE> pte: %p\n", pte);
    page = pte_page(*pte);        /* this is the physical address */
#endif
    /* now increment the count and return */
    atomic_inc(&mem_map[MAP_NR(dmapage)].count);
/*    set_bit(PG_reserved,&mem_map[MAP_NR(dmapage)].flags);*/
  
#else
    dmapage = virt_to_page(pageptr);
	get_page(dmapage);
#endif
    printk("<RIPTIDE> system page: %lx\n", (unsigned long)dmapage);
    return dmapage;
}

#if 0
    if (pte_page(*pte)) /* already mapped? shouldn't happen, but... */
        return pte_page(*pte);
#endif


struct vm_operations_struct rt_vm_ops = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
    rt_vma_open,
    rt_vma_release,
    NULL,            /* unmap */
    NULL,            /* protect */
    NULL,            /* sync */
    NULL,            /* advise */
    rt_vma_nopage,
#else
	open: rt_vma_open,
	close: rt_vma_release,
	nopage: rt_vma_nopage,
#endif
};


int rt_mmap(struct file *file,
                struct vm_area_struct *vma)
{
    channel_state *p = (channel_state *)file->private_data;
	void *s = ((channel_state *)file->private_data)->pChannelInfo;
	rt_state *newdev = p->newdev;
	int fragsize, fragstotal;
    char *pvirt;
	unsigned long size;

    /* refuse to map if order is not 0 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
    if (vma->vm_offset & (PAGE_SIZE-1))
        return -ENXIO; /* need aligned offsets */
#endif

  	size = vma->vm_end - vma->vm_start;
    printk("<RIPTIDE> MAP: %lx-%lx(%ld)\n", vma->vm_start, vma->vm_end, size);

    (*newdev->func_table->RipTideGetFragment)(s, &fragsize, &fragstotal);

	if (size > (fragsize * fragstotal))
	{
	    printk("<RIPTIDE> ERROR!!! %ld > %ld\n", size, (long)fragsize * fragstotal);
		return -EINVAL;
    }

    fragstotal = size / fragsize;
    (*newdev->func_table->RipTideSetFragment)(s, fragsize, fragstotal);
    
    if((fragsize = (*newdev->func_table->RipTideAddBuffer)(s, (char *)vma->vm_start, 0)) < 0)
    {
        printk("<RIPTIDE> ERROR!!! MMAP AddBuffer error\n");
        return -1;
    }

    pvirt = (*newdev->func_table->RipTideHwGetDMABuffer)(s, &fragsize);
    if(pvirt == NULL || fragsize != size)
    {
        printk("<RIPTIDE> ERROR!!! NO DMA BUFFER: %p, %d, %ld\n", pvirt, fragsize, size);
        return -1;
    }
    p->DMABufferSize = fragsize;
    p->DMABuffer     = (unsigned long)pvirt;

    printk("<RIPTIDE> MAP: %p(%d) -> %lx-%lx(%ld)\n", pvirt, fragsize, vma->vm_start, vma->vm_end, size);

    /* don't do anything here: "nopage" will fill the holes */
    vma->vm_ops = &rt_vm_ops;
/*    vma->vm_inode = inode;
    inode->i_count++;
*/
    rt_vma_open(vma);
#if 0
    /*
     * A pointer to init_mm is needed to access page tables
     */
    if (!init_mm_ptr)
        retrieve_init_mm_ptr();
#endif

    p->DMAState |= DMA_MAP_MAPPED;
    return 0;
}

#if 0
static int rt_mmap(struct file *file, struct vm_area_struct *vma)
{
    channel_state *p = (channel_state *)file->private_data;
	void *s = ((channel_state *)file->private_data)->pChannelInfo;
	rt_state *newdev = p->newdev;
	int fragsize, fragstotal;
    char *pvirt;
	unsigned long size, i;

  	if (vma->vm_offset != 0)
  	{
  	    printk("<RIPTIDE> ERROR!!! Invalid offset\n");
		return -EINVAL;
    }
  	size = vma->vm_end - vma->vm_start;
    printk("<RIPTIDE> MAP: %p-%p(%ld)\n", vma->vm_start, vma->vm_end, size);

    (*newdev->func_table->RipTideGetFragment)(s, &fragsize, &fragstotal);

	if (size > (fragsize * fragstotal))
	{
	    printk("<RIPTIDE> ERROR!!! %ld > %ld\n", size, (long)fragsize * fragstotal);
		return -EINVAL;
    }

    fragstotal = size / fragsize;
    (*newdev->func_table->RipTideSetFragment)(s, fragsize, fragstotal);
    
    if((fragsize = (*newdev->func_table->RipTideAddBuffer)(s, (char *)vma->vm_start, 0)) < 0)
    {
        printk("<RIPTIDE> ERROR!!! MMAP AddBuffer error\n");
        return -1;
    }

    pvirt = (*newdev->func_table->RipTideHwGetDMABuffer)(s, &fragsize);
    if(pvirt == NULL || fragsize != size)
    {
        printk("<RIPTIDE> ERROR!!! NO DMA BUFFER: %p, %d, %ld\n", pvirt, fragsize, size);
        return -1;
    }
    p->DMABufferSize = fragsize;
    p->DMABuffer     = (unsigned long)pvirt;

    printk("<RIPTIDE> MAP: %p(%ld) -> %p-%p(%ld)\n", pvirt, fragsize, vma->vm_start, vma->vm_end, size);
    for(i = 0; i < size; i += PAGE_SIZE)
    {
        unsigned long pphys;
        pphys = kvirt_to_phys((unsigned long)(pvirt+i));
/* now mark the pages as reserved, otherwise remap_page_range does not do what we want */
        set_bit(PG_reserved,&mem_map[MAP_NR(phys_to_virt(pphys))].flags);
        
        if (remap_page_range(vma->vm_start+i, pphys, PAGE_SIZE, vma->vm_page_prot))
        {
            printk("<RIPTIDE> ERROR!!! Can't map %lx to %lx (%lx)\n", pphys, vma->vm_start+i, PAGE_SIZE);
		    return -EAGAIN;
        }
    }
    p->DMAState |= DMA_MAP_MAPPED;
	return 0;
}
#endif

#endif

/*************************************************************************
 *  Name:           static int rt_ioctl_dsp(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
 *
 *  Parameters:     inode  - pointer to inode
 *                  file   - pointer to file descriptor
 *                  cmd    - command
 *                  arg    - argument
 *
 *  Returns:        0           - if successful
 *                  otherwise   - error
 *
 *  Description:
 *    The function performs /dev/dsp related IOCTLs
 *
 *************************************************************************/
static int rt_ioctl_dsp(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    channel_state *p = (channel_state *)file->private_data;
	void *s = ((channel_state *)file->private_data)->pChannelInfo;
	rt_state *newdev = p->newdev;
//	unsigned long flags;
    audio_buf_info abinfo;
	int val, ret = 0;
	char *iocode;
	
/*
	VALIDATE_STATE(s);
        mapped = ((file->f_mode & FMODE_WRITE) && s->dma_dac.mapped) ||
		((file->f_mode & FMODE_READ) && s->dma_adc.mapped);
*/
//    printk("<");
//    mdprintf2(("<RIPTIDE> rt_ioctl started: %p %x\n", newdev->pHwInfo, cmd));
//    mdprintf2(("<RIPTIDE> IOCTL: %s (%x) ret: %d\n", iocode, cmd, ret));
        
    switch (cmd) {
    case OSS_GETVERSION:
	    iocode = "OSS_GETVERSION";  
		ret = put_user(SOUND_VERSION, (int *)arg);
        break;
	case SNDCTL_DSP_SYNC:   /* to wait until last byte written to the device has been played then stop*/
        iocode = "SNDCTL_DSP_SYNC";
        ret = 0;
        if (file->f_mode & FMODE_WRITE)
        {
            int fragsize, fragstotal, bytes;
        	(*newdev->func_table->RipTideGetFragment)(s, &fragsize, &fragstotal);
        	fragstotal *= fragsize;
            bytes = 0;
	        while((fragstotal - bytes) > 0)
	        {
                bytes        = (*newdev->func_table->RipTideQueryIdle)(s); 
	            if((fragstotal - bytes) > 0)
	            {
		            mdprintf2(("<RIPTIDE> Sleep for awhile: %d(%d) > %d\n", (fragstotal - bytes), bytes, fragsize));
		            interruptible_sleep_on(&p->wait);
					if (signal_pending(current))
					{
						ret = (ret)? ret : -ERESTARTSYS;
						break;
		            }
	            }
            }
           (*newdev->func_table->RipTideHwStopDMA)(s);
        }
		break;
	case SNDCTL_DSP_SETDUPLEX:
        iocode = "SNDCTL_DSP_SETDUPLEX";
        ret = 0;
		break;
	case SNDCTL_DSP_GETCAPS:    /* get DMA capabilities */
        iocode = "SNDCTL_DSP_GETCAPS";
        val = 0
#ifdef SUPPORT_DSP_DUPLEX
                    |   DSP_CAP_DUPLEX
#endif
#ifdef SUPPORT_DSP_REALTIME
                    |   DSP_CAP_REALTIME
#endif
#ifdef SUPPORT_DSP_TRIGGER
                    |   DSP_CAP_TRIGGER
#endif
#ifdef SUPPORT_DSP_BATCH
                    |   DSP_CAP_BATCH
#endif
#ifdef SUPPORT_DSP_MMAP
                    |   DSP_CAP_MMAP 
#endif
                    | 0;
		ret = put_user(val, (int *)arg);
		mdprintf2(("<RIPTIDE> Caps: %x\n", val));
		break;
    case SNDCTL_DSP_RESET:      /* stops the device immediately */
        iocode = "SNDCTL_DSP_RESET";
        (*newdev->func_table->RipTideHwStopDMA)(s);
		ret = 0;
        break;
    case SOUND_PCM_READ_RATE:       /* get sampling rate */
        iocode = "SOUND_PCM_READ_RATE";
        val = (*newdev->func_table->RipTideGetSampleRate)(s);
	    ret = put_user(val, (int *)arg);
        break;
    case SOUND_PCM_WRITE_RATE:      /* set sampling rate */
/*    case SNDCTL_DSP_SPEED:*/      /* set sample rate */
        iocode = "SOUND_PCM_WRITE_RATE";
        get_user_ret(val, (int *)arg, -EFAULT);
        (*newdev->func_table->RipTideSetSampleRate)(s,val);
        ret = put_user(val, (int *)arg);
		break;
    case SNDCTL_DSP_STEREO:     /* set stereo/mono mode */
        iocode = "SNDCTL_DSP_STEREO";
        get_user_ret(val, (int *)arg, -EFAULT);
        val = (val)?2:1;
        (*newdev->func_table->RipTideSetChannels)(
            s, val);
        ret = 0;
        break;
    case SOUND_PCM_READ_CHANNELS:   /* get inumber of channels */
        iocode = "SOUND_PCM_READ_CHANNELS";
        val = (*newdev->func_table->RipTideGetChannels)(s);
	    ret = put_user(val, (int *)arg);
        break;
/*    case SNDCTL_DSP_CHANNELS:*/
    case SOUND_PCM_WRITE_CHANNELS:  /* set inumber of channels */
        iocode = "SOUND_PCM_WRITE_CHANNELS";
        get_user_ret(val, (int *)arg, -EFAULT);
        (*newdev->func_table->RipTideSetChannels)(
            s,
            val);
        ret = put_user(val, (int *)arg);
        break;
    case SNDCTL_DSP_GETFMTS:        /* Returns a mask of all sample sizes */
        iocode = "SNDCTL_DSP_GETFMTS";
        ret = put_user(AFMT_S16_LE|AFMT_U8, (int *)arg);
		break;
    case SNDCTL_DSP_SETFMT:         /* Selects ONE fmt*/
        iocode = "SNDCTL_DSP_SETFMT";
        get_user_ret(val, (int *)arg, -EFAULT);
        switch(val)
        {
           case AFMT_U8:
           case AFMT_S8:
               val = 8;
               break;
           default:
               val = 16;
        }
	    (*newdev->func_table->RipTideSetBitsPerSample)(s, val);
        ret = 0;
        break;
    case SOUND_PCM_READ_BITS:       /* get sample size in bits */
        iocode = "SOUND_PCM_READ_BITS";
        val = (*newdev->func_table->RipTideGetBitsPerSample)(s);
	    if(val == 8)
            val = AFMT_U8;
	    else
	        val = AFMT_U16_LE;
        ret = put_user(val, (int *)arg);
        break;
    case SNDCTL_DSP_POST:            /* 1 - forced start of playback */
        iocode = "SOUND_PCM_READ_BITS";
        ret = 0;           /* 0 - pause playback */
        break;
    case SNDCTL_DSP_GETTRIGGER:
        iocode = "SNDCTL_DSP_GETTRIGGER";
#ifdef SUPPORT_DSP_TRIGGER
		val = 0;
		if (file->f_mode & FMODE_READ) 
			val = p->DMAState | PCM_ENABLE_INPUT;
		if (file->f_mode & FMODE_WRITE  /* && s->enable & CM_CENABLE_PE*/) 
			val = p->DMAState | PCM_ENABLE_OUTPUT;
		ret = put_user(val, (int *)arg);
#else
        ret = -EINVAL;
#endif
        break;		
	case SNDCTL_DSP_SETTRIGGER: /* stop playback/record channel*/
        iocode = "SNDCTL_DSP_SETTRIGGER";
#ifdef SUPPORT_DSP_TRIGGER
		get_user_ret(val, (int *)arg, -EFAULT);
		if (file->f_mode & FMODE_READ) {
			if (val & PCM_ENABLE_INPUT) {
               ret = (*newdev->func_table->RipTideHwStartDMA)(s);
			} else
               ret = (*newdev->func_table->RipTideHwStopDMA)(s);
		}
		if (file->f_mode & FMODE_WRITE) {
			if (val & PCM_ENABLE_OUTPUT) {
               ret = (*newdev->func_table->RipTideHwStartDMA)(s);
			} else
               ret = (*newdev->func_table->RipTideHwStopDMA)(s);
		}
        p->DMAState = val;
        ret = ret?0:(-1);
#else
        ret = -EINVAL;
#endif		
        break;
	case SNDCTL_DSP_GETOSPACE:
        iocode = "SNDCTL_DSP_GETOSPACE";
        if (!(file->f_mode & FMODE_WRITE))
            ret =  -EINVAL;
        else
        {
	        (*newdev->func_table->RipTideGetFragment)(s, &abinfo.fragsize, &abinfo.fragstotal);
            abinfo.bytes        = (*newdev->func_table->RipTideQueryIdle)(s);
            abinfo.fragments    = abinfo.bytes / abinfo.fragsize;
            mdprintf2(("<RIPTIDE> size: %d bytes: %d total: %d frag: %d\n", abinfo.fragsize, abinfo.bytes, abinfo.fragstotal, abinfo.fragments));
            //printk(KERN_DEBUG"SNDCTL_DSP_GETOSPACE size: %d bytes: %d total: %d frag: %d\n", abinfo.fragsize, abinfo.bytes, abinfo.fragstotal, abinfo.fragments);
            ret = copy_to_user((void *)arg, &abinfo, sizeof(abinfo)) ? -EFAULT : 0;
        }
        break;
	case SNDCTL_DSP_GETISPACE:
        iocode = "SNDCTL_DSP_GETISPACE";
        if (!(file->f_mode & FMODE_READ))
            ret = -EINVAL;
        else
        {
	        (*newdev->func_table->RipTideGetFragment)(s, &abinfo.fragsize, &abinfo.fragstotal);
            abinfo.bytes        = (*newdev->func_table->RipTideQueryBusy)(s);
            abinfo.fragments    = abinfo.bytes / abinfo.fragsize;
            ret = copy_to_user((void *)arg, &abinfo, sizeof(abinfo)) ? -EFAULT : 0;
		}
		break;
    case SNDCTL_DSP_NONBLOCK:
        iocode = "SNDCTL_DSP_NONBLOCK";
        file->f_flags |= O_NONBLOCK;
        ret = 0;
        break ;
    case SNDCTL_DSP_GETODELAY:
	iocode = "SNDCTL_DSP_GETODELAY";
/*	
		if (!(file->f_mode & FMODE_WRITE))
			return -EINVAL;
		spin_lock_irqsave(&s->lock, flags);
		cm_update_ptr(s);
                val = s->dma_dac.count;
		spin_unlock_irqrestore(&s->lock, flags);
		return put_user(val, (int *)arg);
*/
	printk(KERN_ERR "<RIPTIDE> SNDCTL_DSP_GETODELAY ioctl not supported\n");
	ret = -EINVAL ;
	break ;

    case SNDCTL_DSP_GETIPTR:
        iocode = "SNDCTL_DSP_GETIPTR";
#ifdef SUPPORT_DSP_REALTIME
		if (!(file->f_mode & FMODE_READ))
		{
			ret = -EINVAL;
	    }
	    else
	    {
//	        unsigned long flags;
	        count_info cinfo;
//            spin_lock_irqsave(&newdev->lock, flags);
            cinfo.bytes  = (*newdev->func_table->RipTideHwReadDMACounter)(s);
            cinfo.blocks = p->int_count;
            cinfo.ptr    = cinfo.bytes % p->DMABufferSize;
            p->int_count = 0;
//            cinfo.ptr    = cinfo.bytes % p->DMABufferSize;
//            spin_unlock_irqrestore(&newdev->lock, flags);
            ret = copy_to_user((void *)arg, &cinfo, sizeof(cinfo));
            mdprintf2(("<RIPTIDE> bytes: %d blocks: %d\n", cinfo.bytes, cinfo.blocks));
        }
#else
        ret = -EINVAL;
#endif
        break;	
    case SNDCTL_DSP_GETOPTR:
        iocode = "SNDCTL_DSP_GETOPTR";
#ifdef SUPPORT_DSP_REALTIME
		if (!(file->f_mode & FMODE_WRITE))
			ret = -EINVAL;
	    else
	    {
//	        unsigned long flags;
	        count_info cinfo;
//            spin_lock_irqsave(&newdev->lock, flags);
            cinfo.bytes  = (*newdev->func_table->RipTideHwReadDMACounter)(s);
            cinfo.ptr    = cinfo.bytes % p->DMABufferSize;
            cinfo.blocks = p->int_count;
            p->int_count = 0;
//            cinfo.ptr    = cinfo.bytes % p->DMABufferSize;
//            spin_unlock_irqrestore(&newdev->lock, flags);
            ret = copy_to_user((void *)arg, &cinfo, sizeof(cinfo));
            mdprintf2(("<RIPTIDE> bytes: %d blocks: %d\n", cinfo.bytes, cinfo.blocks));
//            printk("%d.%d",cinfo.bytes, cinfo.blocks);
        }
#else
        ret = -EINVAL;
#endif		
        break;
    case SNDCTL_DSP_GETBLKSIZE:         /* get the fragment (single DMA buffer) size in bytes */
        iocode = "SNDCTL_DSP_GETBLKSIZE";
        (*newdev->func_table->RipTideGetFragment)(s, &abinfo.fragsize, &abinfo.fragstotal);        
		ret = put_user(abinfo.fragsize, (int *)arg);
        mdprintf2(("<RIPTIDE> Blk size : %d\n", abinfo.fragsize));
        break;
    case SNDCTL_DSP_SETFRAGMENT:        /* set desired fragment size */
    {
        int fragsize, fragmax;
        iocode = "SNDCTL_DSP_SETFRAGMENT";
        get_user_ret(val, (int *)arg, -EFAULT);
        fragsize = 1<<(val & 0xFFFF);
        fragmax = (val >> 16) & 0xFFFF;
        (*newdev->func_table->RipTideSetFragment)(s, fragsize, fragmax);
        mdprintf2(("<RIPTIDE> val = %ld size = %d max = %d\n",val, fragsize, fragmax));
    	ret = 0;
        break;
    }
    case SNDCTL_DSP_SUBDIVIDE:
        iocode = "SNDCTL_DSP_SUBDIVIDE";
        ret = -EINVAL;
        break;
    case SOUND_PCM_WRITE_FILTER:
        iocode = "SOUND_PCM_WRITE_FILTER";
        ret = -EINVAL;
        break;
    case SNDCTL_DSP_SETSYNCRO:
        iocode = "SNDCTL_DSP_SETSYNCRO";
        ret = -EINVAL;
        break;
    case SOUND_PCM_READ_FILTER:
        iocode = "SOUND_PCM_READ_FILTER";
        ret = -EINVAL;
        break;
    default:
        iocode = "SNDCTL_DSP_UNSUPPORTED";
        ret = -EINVAL;
    }
    mdprintf2(("<RIPTIDE> IOCTL: %s (%x) ret: %d\n", iocode, cmd, ret));
//    printk(">");
    return ret;
}

static int rt_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int state = 0;
    int minor = minor(inode->i_rdev);

//    mdprintf2(("<RIPTIDE> rt_ioctl started\n"));

    switch(minor)
    {
	    case SND_DEV_CTL:
	    {
	        state = rt_ioctl_mixdev(inode, file, cmd, arg);
	        break;
	    }
	    case SND_DEV_DSP:
	    case SND_DEV_DSP16:
	    {
	        state = rt_ioctl_dsp(inode, file, cmd, arg);
			break;
		}
	    default:
	        printk("<RIPTIDE> ERROR!!! Invalid node: %x\n", minor);
	        state = -ENODEV;	/* no such node */
    }
    return state;
}

/*************************************************************************
 *  Name:           static ssize_t rt_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
 *
 *  Parameters:     file   - pointer to file descriptor
 *                  buffer - pointer to user data buffer
 *                  count  - user data counter
 *                  ppos   - pointer to current pos  
 *
 *  Returns:        >=0       - if successful, size of read data
 *                  otherwise - error
 *
 *  Description:
 *    The function is read front-end for /dev/dsp device
 *
 *************************************************************************/
static ssize_t rt_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
static ssize_t state = 0;
    int minor = minor(file->f_dentry->d_inode->i_rdev);

    //printk("<RIPTIDE> rt_read started\n");

    switch(minor)
    {
	    case SND_DEV_DSP:
	    case SND_DEV_DSP16:
	    {
	        state = rt_read_dsp(file, buffer, count, ppos);
			break;
		}
	    default:
	        printk("<RIPTIDE> ERROR!!! Invalid node: %x\n", minor);
	        state = -EINVAL;	/* no such node */
    }
    return state;
}

/*************************************************************************
 *  Name:           static ssize_t rt_write(struct file *file, char *buffer, size_t count, loff_t *ppos)
 *
 *  Parameters:     file   - pointer to file descriptor
 *                  buffer - pointer to user data buffer
 *                  count  - user data counter
 *                  ppos   - pointer to current pos  
 *
 *  Returns:        >=0       - if successful, size of written data
 *                  otherwise - error
 *
 *  Description:
 *    The function is write front-end for /dev/dsp device
 *
 *************************************************************************/
static ssize_t rt_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
static ssize_t state = 0;
    int minor = minor(file->f_dentry->d_inode->i_rdev);

//    printk("<RIPTIDE> rt_write started\n");

    switch(minor)
    {
	    case SND_DEV_DSP:
	    case SND_DEV_DSP16:
	    {
/*	        int fl1, fl2;
            fl1 = file->f_dentry->d_inode->i_flags;
            file->f_dentry->d_inode->i_flags &= ~MS_MANDLOCK;
            printk("<RIPTIDE> flag1: %x\n", fl1);
            if(file->f_dentry->d_inode->i_sb)
            {
                fl2 = file->f_dentry->d_inode->i_sb->s_flags;
                file->f_dentry->d_inode->i_sb->s_flags &= ~MS_MANDLOCK;
                printk("<RIPTIDE> flag2: %x\n", fl2);
            }*/
            
	        state = rt_write_dsp(file, buffer, count, ppos);
/*	        
	        file->f_dentry->d_inode->i_flags = fl1;
            if(file->f_dentry->d_inode->i_sb)
            {
                file->f_dentry->d_inode->i_sb->s_flags = fl2;
            }*/
			break;
		}
	    default:
	        printk("<RIPTIDE> ERROR!!! Invalid node: %x\n", minor);
	        state = -EINVAL;	/* no such node */
    }
    return state;
}


/*************************************************************************
 *  Name:           static int rt_open(struct inode *inode, struct file *file)
 *
 *  Parameters:     inode  - pointer to inode
 *                  file   - pointer to file descriptor
 *
 *  Returns:        0       - if successful
 *                  otherwise - error
 *
 *  Description:
 *    The function is open front-end for /dev/dsp device
 *
 *************************************************************************/
static int rt_open(struct inode *inode, struct file *file)
{
    int state = 0;
    int minor = minor(inode->i_rdev);
    rt_state *s = (rt_state *)devs;
    void *pChannelInfo = NULL;

    mdprintf2(("<RIPTIDE> rt_open stated for minor %d: %p, %p, %p\n", minor, s, inode, file));


    if(devs == NULL)
        if(init_driver())
        {
            return -ENODEV;
        }
           
    s = (rt_state *)devs;

    switch(minor)
    {
	    case SND_DEV_DSP:
	    case SND_DEV_DSP16:
	    {
	       channel_state *newch;
	       mdprintf2(("<RIPTIDE> Create channel: %p\n", s));
	       if((newch = kmalloc(sizeof(channel_state), GFP_KERNEL)) == NULL)
	       {
	           printk("<RIPTIDE> ERROR!!! Not anough memory: %d\n", sizeof(channel_state));
	           state = -EBUSY;
	       }
	       else
	       {	       
		       if((pChannelInfo = (*s->func_table->RipTideCreateChannel)(s->pHwInfo, (file->f_mode & FMODE_READ) && (!(file->f_mode & FMODE_WRITE)), newch, &newch->sysObj)) != NULL)
		       {
	                newch->newdev        = s;
	                newch->DMAState      = ((file->f_mode & FMODE_WRITE)?PCM_ENABLE_OUTPUT:0) | 
	                                       ((file->f_mode & FMODE_READ)?PCM_ENABLE_INPUT: 0);
	                newch->DMABufferSize = 0;
	                newch->DMABuffer     = (unsigned long)NULL;
	                newch->int_count     = 0;
	                newch->pChannelInfo  = pChannelInfo;
	                mdprintf2(("<RIPTIDE> Init wait queue: %p\n", &newch->wait));
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	                init_waitqueue(&newch->wait);
#else
	                init_waitqueue_head(&newch->wait);
#endif
	                file->private_data = newch;
	                MOD_INC_USE_COUNT;
	                mdprintf2(("<RIPTIDE> Open is completed successfully: %p\n", s->pHwInfo));
                    if(s->open)
                        s->open();
                    break;
	            }
	            {
		            printk("<RIPTIDE> ERROR!!! Can't create channel: %p\n", s->pHwInfo);
//        	        (*s->func_table->RipTideDeleteChannel)(pChannelInfo);
		            kfree_s(newch, sizeof(channel_state));
		            state = -EBUSY;
		        }
		    }
		    break;
	    }
	    case SND_DEV_CTL:
	    {
	        state = rt_open_mixdev(inode, file);
	        break;
	    }
	    default:
	        printk("<RIPTIDE> ERROR!!! Invalid node: %x\n", minor);
	        state = -ENODEV;	/* no such node */
    }

    return state;
}

/*************************************************************************
 *  Name:           static int rt_release(struct inode *inode, struct file *file)
 *
 *  Parameters:     inode  - pointer to inode
 *                  file   - pointer to file descriptor
 *
 *  Returns:        0       - if successful
 *                  otherwise - error
 *
 *  Description:
 *    The function is release front-end for /dev/dsp device
 *
 *************************************************************************/
static int rt_release(struct inode *inode, struct file *file)
{
    int state = 0;
    int minor = minor(inode->i_rdev);


    mdprintf2(("<RIPTIDE> rt_release started\n"));

    switch(minor)
    {
	    case SND_DEV_CTL:
	    {
	        state = rt_release_mixdev(inode, file);
	        break;
	    }
	    case SND_DEV_DSP:
	    case SND_DEV_DSP16:
	    {
            channel_state *newdev = (channel_state *)file->private_data;
            void *s = ((channel_state *)file->private_data)->pChannelInfo;
	        rt_state *rtstate = newdev->newdev;
	        
		    (*rtstate->func_table->RipTideDeleteChannel)(s, &newdev->sysObj);
		    file->private_data = NULL;
            if(!state)
                if(devs->close)
                    devs->close();
			MOD_DEC_USE_COUNT;
			state = 0;
			break;
		}
	    default:
	        printk("<RIPTIDE> ERROR!!! Invalid node: %x\n", minor);
	        state = -ENODEV;	/* no such node */
    }
	
    mdprintf2(("<RIPTIDE> rt_release ended\n"));
    return state;
}

/* /dev/dsp file operations table */
static /*const*/ struct file_operations rt_audio_fops = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	&rt_llseek,
	&rt_read,
	&rt_write,
	NULL,  /* readdir */
	&rt_poll,
	&rt_ioctl,
#ifdef SUPPORT_DSP_MMAP
	&rt_mmap,
#else
    NULL,
#endif
	&rt_open,
	NULL,	/* flush */
	&rt_release,
	NULL,  /* fsync */
	NULL,  /* fasync */
	NULL,  /* check_media_change */
	NULL,  /* revalidate */
	NULL,  /* lock */
#else
	owner:      THIS_MODULE,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,9)
	llseek:     rt_llseek,
#else
	llseek:     no_llseek,
#endif
	read:       rt_read,
	write:      rt_write,
	poll:       rt_poll,
	ioctl:      rt_ioctl,
#ifdef SUPPORT_DSP_MMAP
	mmap:       rt_mmap,
#endif
	open:       rt_open,
	release:    rt_release,
#endif
};
#endif //SUPPORT_WAVE_IN || SUPPORT_WAVE_OUT

/* --------------------------------------------------------------------- */

/*************************************************************************
 *  Name:           void rt_bh(void *p)
 *
 *  Parameters:     p  - pointer to context
 *
 *  Returns:        None
 *
 *  Description:
 *    The function is bottom half of the interrupt routine
 *
 *************************************************************************/
void rt_bh(void *p)
{
	rt_state *newdev = (rt_state *)p;
    void *pChannelInfo = NULL;
    void *pContext;
    BOOLEAN flag = FALSE;
    
//    printk("[");
    if(newdev->func_table->RipTideNextChannel && newdev->func_table->RipTideQueryInterrupt)
    {
	    while((pChannelInfo = (*newdev->func_table->RipTideNextChannel)(newdev->pHwInfo, &pContext)) != NULL)
	    {
	/*        printk("<RIPTIDE> check %d\n", pChannelInfo->ChannelId);*/
	        while((*newdev->func_table->RipTideQueryInterrupt)(pChannelInfo, &((channel_state *)pContext)->sysObj))
	        {
	           flag = TRUE;
	/*            printk("<RIPTIDE> wake up %d\n", pChannelInfo->ChannelId);*/
	            ((channel_state *)pContext)->int_count++;
	        }
	        if(flag) {
				//printk(KERN_DEBUG"w=%p\n", &((channel_state *)pContext)->wait);
                wake_up_interruptible(&((channel_state *)pContext)->wait);
			}
	    }
    }

//    printk("]");
/*    RipTideHwCompleteInterrupt(phwInfo);*/
}

/*************************************************************************
 *  Name:           static void rt_interrupt(int irq, void *dev_id, struct pt_regs *regs)
 *
 *  Parameters:     irq    - irq that is occured
 *                  dev_id - pointer to context
 *                  reg    - registers values table
 *
 *  Returns:        None
 *
 *  Description:
 *    The function is the interrupt routine
 *
 *************************************************************************/
static void rt_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	rt_state *newdev = (rt_state *)dev_id;

    if(dev_id == NULL)
    {
//        printk("\n!!!\n");
        return;
    }

//    printk("(");
    queue_task(&(newdev->run_bh), &tq_immediate);
    mark_bh(IMMEDIATE_BH);

//    rt_bh(dev_id);
    if(newdev->func_table->RipTideHwCompleteInterrupt)
        (*newdev->func_table->RipTideHwCompleteInterrupt)(newdev->pHwInfo);
#if defined(SUPPORT_MIDI_IN) || defined (SUPPORT_MIDI_IN)
    if(newdev->hwIOMidi)
        mpuintr(irq,(void *)newdev->midiSlot,regs);
#endif
        
//    printk(")");
}

/* --------------------------------------------------------------------- */
/*************************************************************************
 *  Name:           void delete_device(rt_state *phwInfo)
 *
 *  Parameters:     phwInfo - pointer to driver state structure
 *
 *  Returns:        None
 *
 *  Description:
 *    The function release driver related resources
 *
 *************************************************************************/
void delete_device(void)
{
    printk("<RIPTIDE> Delete device\n");
/*    
    if(!phwInfo)
    {
        printk("<RIPTIDE> No device descriptor.\n");
        return;
    }
    if(phwInfo->dev_dmfm > 0)
        unregister_sound_special(phwInfo->dev_dmfm);
    if(phwInfo->dev_midi > 0)
        unregister_sound_midi(phwInfo->dev_midi);
*/
#ifdef SUPPORT_SOUNDCORE
    unregister_sound_mixer(dev_mixer);
    unregister_sound_dsp(dev_audio);
#else
#if 0
    if(dev_mixer > 0)
        unregister_chrdev(dev_mixer, "mixer");
    if(dev_audio > 0)
    {
/*        unregister_sound_dsp(phwInfo->dev_audio);*/
        unregister_chrdev(dev_audio, "sound");
    }
    else
    {
        printk("<RIPTIDE> No sound device.\n");
    }
#endif
#endif
    printk("<RIPTIDE> Device resources are released\n");
}

/*************************************************************************
 *  Name:           interrupt_handler cnxt_register_hardware(void *dev)
 *
 *  Parameters:     dev - pointer to context - device state structure
 *
 *  Returns:        None
 *
 *  Description:
 *    The function registers child driver
 *
 *************************************************************************/
interrupt_handler cnxt_register_hardware(void *dev)
{
rt_state *newdev = dev;
    printk("<RIPTIDE> Register hardware %p\n", dev);
    if(newdev)
    {
/* init bottom half */
        newdev->run_bh.routine  = rt_bh;
        newdev->run_bh.data     = newdev;
        newdev->run_bh.sync     = 0;
        newdev->modify_counter  = 0;

		newdev->next = devs;
		devs = newdev;

		printk("<RIPTIDE> register is  successful\n");
		return rt_interrupt;
    }
    return NULL;
}

void cnxt_unregister_hardware(void *dev)
{
    if(devs == (rt_state *)dev)
    {
        devs = NULL;
    }
}

int init_driver(void)
{
    int index = 0;
    
	struct DevTable {
		unsigned short  deviceType;
	    char            *devicename;
	    char            *modulename;
	#ifdef MONOLIT
	    int             (*init)(void);
	    void            (*clean)(void);
	#endif
	} devicetable[] =
	{
	    {
	        0,
	        "Riptide",
	        "riptidelow"
	#ifdef MONOLIT
	        ,rt_init, rt_clean
	#endif
	    }
	#ifdef SUPPORT_ICH   
	   ,{
	        1, 
	        "CoreLogic", 
	        "cnxt_ich"
	#ifdef MONOLIT
	        ,ich_init, ich_clean
	#endif
	    }
	#endif
	};

    
    MOD_INC_USE_COUNT;

    for(index = 0; index < sizeof(devicetable)/sizeof(struct DevTable); index++)
    {
#ifdef MONOLIT
        if((*devicetable[index].init)())
#else
        request_module(devicetable[index].modulename);
        if(0)
#endif
        {
            printk("<RIPTIDE> ERROR!!! Can't init device %d: %s\n", index, devicetable[index].devicename);
            return -1;
            continue;
        }
        else
        {
	        if(devs != NULL)
	        {
#ifndef MONOLIT
	            devs->modulename = devicetable[index].modulename;
#endif
                break;
	        }
        }       
#ifdef  MONOLIT
        (*devicetable[index].clean)();        
#else
//        release_module(devicetable[index].modulename);
#endif
        devs = NULL;
    }

    MOD_DEC_USE_COUNT;

    if(devs == NULL)
    {
        printk("<RIPTIDE> ERROR!!! Not device\n");
        return -1;
    }
    return 0;
}

#ifdef MONOLIT
int     rt_init(void);
void    rt_clean(void);
#ifdef SUPPORT_ICH
int     ich_init(void);
void    ich_clean(void);
#endif
#else
EXPORT_SYMBOL_NOVERS(cnxt_register_hardware);
EXPORT_SYMBOL_NOVERS(cnxt_unregister_hardware);
//EXPORT_SYMBOL_NOVERS(rt_interrupt);
#endif

#ifdef MODULE

#if 0
MODULE_PARM(io, "i");		/* IO base */
MODULE_PARM(irq, "i");		/* IRQ */
MODULE_PARM(dma, "i");		/* DMA */
MODULE_PARM(dma2, "i");		/* DMA2 */
#endif

MODULE_AUTHOR("Conexant Systems Inc.");
MODULE_DESCRIPTION("RipTide Audio Driver");
MODULE_LICENSE("Copyright (C) 1995-2002 Conexant Systems Inc. All Rights Reserved.");


/*************************************************************************
 *  Name:           int init_module(void)
 *
 *  Parameters:     none
 *
 *  Returns:        None
 *
 *  Description:
 *    The function initializes module, starts child drivers and allocate resources
 *
 *************************************************************************/
int init_module(void)
{
//    unsigned int hwIOBase, hwIOSynth, hwIOMidi, hwIrq;
    int ret = -1;
/* register devices */
	printk(KERN_ERR "Conexant Sound Driver v%s time " __TIME__ " " __DATE__ "\n", RIPLINUXVERSION);

    dev_audio = 
    dev_mixer = 0;


#ifdef SUPPORT_SOUNDCORE
    if ((dev_audio = register_sound_dsp(&rt_audio_fops, -1)) < 0)
    {
        printk("<RIPTIDE> Can't register 'sound' device: %d\n", dev_audio);
        goto err_exit;
    }
    
    if ((dev_mixer = register_sound_mixer(&rt_mixer_fops, -1)) < 0)
    {
        printk("<RIPTIDE> Can't register mixer device\n");
        goto err_exit;
    }
#else
#if 0
    if(!dev_audio)
    if ((dev_audio = register_chrdev(14, "sound", &rt_audio_fops)) < 0)
    {
        printk("<RIPTIDE> Can't register 'sound' device: %d\n", dev_audio);
        goto err_exit;
    }

    if(!dev_audio)
        dev_audio = 14;

    if(!dev_mixer)
    if ((dev_mixer = register_chrdev(15, "mixer", &rt_mixer_fops)) < 0)
    {
        printk("<RIPTIDE> Can't register mixer device\n");
        goto err_exit;
    }
    if(!dev_mixer)
        dev_mixer = 15;
#else
        printk("<RIPTIDE> SUPPORT_SOUNDCORE not defined\n");
        goto err_exit;
#endif
#endif
        
    ret = 0;
    
	printk(KERN_ERR "<RIPTIDE> Init_module is completed\n");
    return ret;

err_exit:
    delete_device();
    printk(KERN_ERR "<RIPTIDE> ERROR!!! Device or resource is busy\n");
    ret = -1;
    return ret;
}


/* --------------------------------------------------------------------- */
/*************************************************************************
 *  Name:           void cleanup_module(void)
 *
 *  Parameters:     none
 *
 *  Returns:        None
 *
 *  Description:
 *    The function cleans module up
 *
 *************************************************************************/
void cleanup_module(void)
{
//rt_state *pstate;

    delete_device();
#if 0
    while ((pstate = devs))
    {
        devs = devs->next;
#if 0		
	outb(0, s->hwIOBase + CODEC_CMI_INT_HLDCLR + 2);  /* disable ints */
	synchronize_irq();
	outb(0, s->hwIOBase + CODEC_CMI_FUNCTRL0 + 2); /* reset channels */
#endif

    delete_device();
#ifdef MONOLIT
    (*pstate->clean)();
#else
//        release_module(pstate->modulename);
#endif
    }
#endif
    printk(KERN_INFO "<RIPTIDE> unloading\n");
}

#endif /* MODULE */
