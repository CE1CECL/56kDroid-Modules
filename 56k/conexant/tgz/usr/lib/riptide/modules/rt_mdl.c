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
  MODULE NAME:     rt_mdl.c                       

  AUTHOR:          Konstantin Lazarev    (Transcribed by KNL)

  HISTORY:         Major Revision               Date        By
            -----------------------------     --------     -----
            Created                           10/01/99      KNL

  DESCRIPTION:

    This module is child driver for Conexant RipTide.
    
  NOTES:    None.
*/

#include <linux/config.h>
#include <linux/module.h>

EXPORT_NO_SYMBOLS;

/*#include <linux/version.h>*/
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

#include "cnxt_rt.h"
#include "cnxt_int.h"

#include <../drivers/sound/sound_config.h>
#include <../drivers/sound/opl3.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
#include <../drivers/sound/mpu401.h>
#endif
rt_func_table   func_table;
struct address_info hw;

extern interrupt_handler cnxt_register_hardware(void *dev);
extern void cnxt_unregister_hardware(void *);
extern HwContext *cnxt_rt_open(int ioPort, int deviceId);
extern void cnxt_rt_close(HwContext *pContext);

/* --------------------------------------------------------------------- */
/*************************************************************************
 *  Name:           void release_device(rt_state *phwInfo)
 *
 *  Parameters:     phwInfo - driver state structure
 *
 *  Returns:        None
 *
 *  Description:
 *    The function release driver resources
 *
 *************************************************************************/
void release_device(rt_state *phwInfo)
{
    printk("<RTLOW> Delete device: %p\n", phwInfo);
    if(!phwInfo)
    {
        printk("<RTLOW> No device descriptor.\n");
        return;
    }

    if(phwInfo->hwIrq)
    {
        free_irq(phwInfo->hwIrq, phwInfo);
    }
    else
    {
        printk("<RTLOW> NO INTERRUPT.\n");
    }
#if 0
    if(phwInfo->hwIOBase)
        release_region(phwInfo->hwIOBase, 64);
    else
        printk("<RTLOW> No io region to release\n");
#endif
#ifdef SUPPORT_LEGACY_DEVICES

#ifdef SUPPORT_SYNTH
    if(phwInfo->hwIOSynth)
        release_region(phwInfo->hwIOSynth, 4);
#endif //SUPPORT_SYNTH

#if defined(SUPPORT_MIDI_IN) || defined(SUPPORT_MIDI_OUT)
    if(phwInfo->hwIOMidi)
    {
//        release_region(phwInfo->hwIOMidi, 2);
		hw.irq = phwInfo->hwIrq;
		hw.io_base = phwInfo->hwIOMidi;
        unload_mpu401(&hw);
    }
#endif //SUPPORT_MIDI_IN || SUPPORT_MIDI_OUT
    {
    u32 pciDWORD;
    pci_read_config_dword (phwInfo->pcidev, 0x50, &pciDWORD);                   /* Read PCI config space */        
    pci_write_config_dword(phwInfo->pcidev, 0x50, pciDWORD & (~0x00008000L));   /* Disable legacy devices */        
    }
#endif //SUPPORT_LEGACY_DEVICES

    kfree_s(phwInfo, sizeof(rt_state));
    printk("<RTLOW> Device resources are released\n");
}

rt_state *newdev = NULL;

void rt_open(void)
{
    MOD_INC_USE_COUNT;
}

void rt_close(void)
{
    MOD_DEC_USE_COUNT;
}

/*************************************************************************
 *  Name:           int init_module(void)
 *
 *  Parameters:     none
 *
 *  Returns:        None
 *
 *  Description:
 *    The function initializes module, finds device, inits device and allocate resources
 *
 *************************************************************************/
#ifndef MONOLIT
MODULE_AUTHOR("Conexant Systems Inc.");
MODULE_DESCRIPTION("RipTide Hardware Control Driver");
MODULE_LICENSE("Copyright (C) 1995-2002 Conexant Systems Inc. All Rights Reserved.");

int init_module(void)
#else
void rt_clean(void);

int rt_init(void)
#endif /* MODULE */
{
//    unsigned int hwIOBase, hwIOSynth, hwIOMidi, hwIrq;
    int ret = -ENODEV;
    struct pci_dev *pcidev = NULL;
	unsigned int hwIOBase;
#if defined(SUPPORT_LEGACY_DEVICES) || defined(SUPPORT_JOYSTICK)
    u32 pciDWORD;
#endif
    int index = 0, i;
struct {
    unsigned short  vendorId;
    unsigned short  deviceId;
	unsigned short  deviceType;
    char            *devicename;
    char            *modulename;
} devicetable[] =
{
   {RIPTIDE_VENDOR_ID,RIPTIDE_DEVICE_ID, 0, "Riptide", "riptidelow"},
   {0x127a,0x4330, 0, "Riptide", "riptidelow"},   
   {0x127a,0x4310, 0, "Riptide", "riptidelow"},   
   {0x127a,0x4340, 0, "Riptide", "riptidelow"}
};

interrupt_handler rt_interrupt;

/* 
        return -ENODEV;
*/

#ifdef CONFIG_PCI
    if (!pci_present())   /* No PCI bus in this machine! */
#endif
    {
        printk(KERN_INFO "<RTLOW> No PCI bus in this machine!\n");
        return -ENODEV;
    }
    printk(KERN_INFO "<RTLOW> version v%s time " __TIME__ " " __DATE__ "\n", RIPLINUXVERSION);
/*
    while (pcidev == NULL && 
          (pcidev = pci_find_device(devicetable[index].vendorId,
                                    devicetable[index].deviceId,
                                    pcidev)))
*/
    for(index=0; index < sizeof(devicetable)/sizeof(devicetable[0]); index++)
    {
        printk("<RTLOW> Look for %x %x", devicetable[index].vendorId, devicetable[index].deviceId);
        pcidev = pci_find_device(devicetable[index].vendorId,
                                    devicetable[index].deviceId,
                                    NULL);
        if(!pcidev)
        {
            printk(" - absent\n");
            continue;
        }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
		if (pci_enable_device(pcidev) ) {
			printk("<RTLOW> pci_enable_device() failed\n");
			continue;
		}

		hwIOBase = 0;
		for (i = 0; i < DEVICE_COUNT_RESOURCE; i++) {
			if ((pci_resource_flags(pcidev, i) & PCI_BASE_ADDRESS_SPACE) ==
					PCI_BASE_ADDRESS_SPACE_IO) {
				hwIOBase = pci_resource_start(pcidev, i) & PCI_BASE_ADDRESS_IO_MASK;
				break;
			}
		}
#else
		for (i = 0; i < sizeof(pcidev->base_address) / sizeof(pcidev->base_address[0]); i++) {
			if ((pcidev->base_address[i] & PCI_BASE_ADDRESS_SPACE) ==
					PCI_BASE_ADDRESS_SPACE_IO) {
				hwIOBase = pcidev->base_address[i] & PCI_BASE_ADDRESS_IO_MASK;
				break;
			}
		}
#endif

        printk(" - found at io %x irq %u\n", hwIOBase, pcidev->irq);

        if (pcidev->irq == 0)
        {
            printk("<RTLOW> No IRQ for device\n");
            continue;
        }

        if(hwIOBase == 0)
        {
            printk("<RTLOW> No base address for device\n");
            continue;
        }

        if (!(newdev = kmalloc(sizeof(rt_state), GFP_KERNEL)))
        {
            printk(KERN_WARNING "<RTLOW> out of memory\n");
            continue;
        }
        newdev->pcidev = pcidev;
        newdev->dev_type = devicetable[index].deviceType;
        
        newdev->dev_dmfm = 
#ifdef SUPPORT_FM
        0;
#else
        -1;
#endif

        newdev->dev_midi =
#if defined(SUPPORT_MIDI_IN) || defined (SUPPORT_MIDI_IN)
        0;
#else
        -1;
#endif

        newdev->dev_mixer = 
#ifdef SUPPORT_MIXER
        0;
#else
        -1;
#endif

        newdev->dev_audio = 0;

        newdev->hwIOBase =
        newdev->hwIOSynth = 
        newdev->hwIOMidi = 
        newdev->hwIrq = 0;

        newdev->open = &rt_open;
        newdev->close = &rt_close;
/* get from config space an save I/O base */
        printk("<RTLOW> Start allocation of resources: %p\n", newdev);
        newdev->hwIOBase = hwIOBase;

#if 0
        if (check_region(newdev->hwIOBase, 64))
        {
            printk(KERN_ERR "<RTLOW> io ports %#x-%#x in use\n", newdev->hwIOBase, newdev->hwIOBase+63);
            goto err_exit;
        }
/* allocate region for I/O ports */
        request_region(newdev->hwIOBase, 64, "riptide");
#endif
/* get from config space and save IPQ number */
        newdev->hwIrq = pcidev->irq;
        newdev->func_table = &func_table;

/* register hardware to core driver */
#ifdef MONOLIT
        newdev->clean = rt_clean;
#endif

        if((rt_interrupt = cnxt_register_hardware(newdev)) == NULL)
        {
            printk("<RTLOW> Can't register device\n");            
            goto err_exit;
        }

/* request irq */
        if (request_irq(newdev->hwIrq, rt_interrupt, SA_SHIRQ, "riptidesound", newdev))
        {
            printk(KERN_ERR "<RTLOW> irq %u in use\n", newdev->hwIrq);
            goto err_exit;
        }
#ifdef SUPPORT_LEGACY_DEVICES
        pci_read_config_dword (pcidev, 0x50, &pciDWORD);          /* Read PCI config space */        
        pci_write_config_dword(pcidev, 0x50, pciDWORD & (~0x00008000L));          /* Disable legacy devices */        
        pciDWORD |= 0x00008000L;                                /* Enable legacy devices */
#if defined(SUPPORT_MIDI_IN) || defined(SUPPORT_MIDI_OUT)
        pciDWORD &= ~0xF0;
        pciDWORD |=  0x00009800L | ((newdev->hwIrq << 4) & 0xF0);                 /* Enable MIDI */
#else
        pciDWORD &= ~0x00001800L;                 /* disable MIDI */
#endif
#ifdef SUPPORT_SYNTH
        pciDWORD |=  0x0000A000L;                 /* enable FM */
#else
        pciDWORD &= ~0x0000A000L;                 /* disable FM */
#endif
#ifdef SUPPORT_JOYSTICK
        {
        unsigned char c;
        c = inb(0x201);
        printk("<RTLOW> Joystick: %x\n",c);
        if(c == 0xFF)
        {
            pciDWORD |= 0x00000400L;              /* enable gameport */
        }
        else
        {
            printk(KERN_ERR "<RTLOW> Joystick is used by another device.\n");
            pciDWORD &= ~0x00000400L;             /* disable gameport */
        }
        }
#else
        pciDWORD &= ~0x00000400L;                 /* disable gameport */
#endif
        pci_write_config_dword(pcidev, 0x50, pciDWORD | 0x1000000);  /* Enable Legacy devices and FM */
#ifdef SUPPORT_JOYSTICK
        pci_read_config_dword (pcidev, 0x4C, &pciDWORD);          /* Read PCI config space */
        pci_write_config_dword(pcidev, 0x4C, (pciDWORD & 0xFFFF) | (0x02010000));  /* Enable Legacy devices and FM */
#endif        
#endif //SUPPORT_LEGACY_DEVICES

/* initialize codec registers */
        printk(KERN_INFO "<RTLOW> Resources are allocated: io %#06x irq %u\n",
		       newdev->hwIOBase, newdev->hwIrq);

        printk("<RTLOW> Init command interface: %x %x %p\n",newdev->hwIOBase, newdev->hwIrq, newdev->func_table);

        if((newdev->pHwContext = cnxt_rt_open(newdev->hwIOBase, devicetable[index].deviceId)) == NULL)
        {
            printk("<RTLOW> ERROR!!! Can't init device interface\n");
            goto err_exit;
        }
        
        printk("<RTLOW> Init command riptide: %x %x %p %p %p %p\n",
            newdev->hwIOBase, newdev->hwIrq, newdev->func_table,
            ((HwContext *)newdev->pHwContext)->pInterface,
            ((HwContext *)newdev->pHwContext)->pfunc,
           &((HwContext *)newdev->pHwContext)->sysObj);
        if((newdev->pHwInfo = RipTideHwInitialize(newdev->hwIOBase,
                                                  newdev->hwIrq,
                                                  newdev->func_table,
                                                  ((HwContext *)newdev->pHwContext)->pInterface,
                                                  ((HwContext *)newdev->pHwContext)->pfunc,
                                                  &((HwContext *)newdev->pHwContext)->sysObj)) == NULL)
        {
            printk("<RTLOW> ERROR!!! Can't init device\n");
            goto err_exit;
        }

#ifdef SUPPORT_LEGACY_DEVICES
#if defined(SUPPORT_MIDI_IN) || defined(SUPPORT_MIDI_OUT)
        newdev->hwIOMidi = 0x330;
        if (check_region(newdev->hwIOMidi, 2))
        {
            printk(KERN_ERR "<RTLOW> io ports %#x-%#x in use\n", newdev->hwIOMidi, newdev->hwIOMidi+1);
            newdev->hwIOMidi = 0;
            goto err_exit;
        }
        else
        {
/* allocate region for MIDI ports */

//	        request_region(newdev->hwIOMidi, 2, "riptide Midi");
        
    	    request_module("mpu401");

			hw.irq = -(newdev->hwIrq);
			hw.io_base = newdev->hwIOMidi;
			probe_mpu401(&hw);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
			attach_mpu401(&hw,THIS_MODULE);                  /* Enable MIDI */
#else
			attach_mpu401(&hw);                  /* Enable MIDI */
#endif
			newdev->midiSlot = hw.slots[1];
		}
#endif

#ifdef SUPPORT_SYNTH
        newdev->hwIOSynth = 0x388;
        if (check_region(newdev->hwIOSynth, 4))
        {
            printk(KERN_ERR "<RTLOW> io ports %#x-%#x in use\n", newdev->hwIOSynth, newdev->hwIOSynth+3);
            newdev->hwIOSynth = 0;
            goto err_exit;
        }
        printk(KERN_INFO "<RTLOW> riptide FM at io ports %#x-%#x\n", newdev->hwIOSynth, newdev->hwIOSynth+3);
        //request_region(newdev->hwIOSynth, 4, "riptide FM");
        
	    request_module("opl3");

        opl3_detect(newdev->hwIOSynth,NULL);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	    opl3_init(newdev->hwIOSynth,NULL,THIS_MODULE);         /* enable FM */
#else
	    opl3_init(newdev->hwIOSynth,NULL);         /* enable FM */
#endif
#endif
#endif //SUPPORT_LEGACY_DEVICES
        ret = 0;
#ifdef SUPPORT_JOYSTICK
        pci_read_config_dword (pcidev, 0x4C, &pciDWORD);          /* Read PCI config space */
        printk(KERN_ERR "<RTLOW> Joystick reg: %x\n", pciDWORD);                 /* enable FM */
#endif
        break;

#if 0
/* init mixer settings */
		fs = get_fs();
		set_fs(KERNEL_DS);
		/* set mixer output */
		frobindir(s, DSP_MIX_OUTMIXIDX, 0x1f, 0x1f);
		/* set mixer input */
		val = SOUND_MASK_LINE|SOUND_MASK_SYNTH|SOUND_MASK_CD|SOUND_MASK_MIC;
		mixer_ioctl(s, SOUND_MIXER_WRITE_RECSRC, (unsigned long)&val);
		for (i = 0; i < sizeof(initvol)/sizeof(initvol[0]); i++) {
			val = initvol[i].vol;
			mixer_ioctl(s, initvol[i].mixch, (unsigned long)&val);
		}
		set_fs(fs);
#endif
//		index++;

//        spin_lock_init(phwInfo->lock);  // init spinlock

        printk("<RTLOW> Device initialization is complete:\nsound: %d\nmixer: %d\nmidi : %d\nfm   : %d\n",
        newdev->dev_audio,newdev->dev_mixer,newdev->dev_midi,newdev->dev_dmfm);
        continue;
err_exit:
        release_device(newdev);
        printk(KERN_ERR "<RTLOW> Error exit\n");
        ret = -ENODEV;
    }
    
    printk(KERN_ERR "<RTLOW> Init_module is completed: index = %x\n", index);
    return ret;
}

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
#ifndef MONOLIT
void cleanup_module(void)
#else
void rt_clean(void)
#endif /* MODULE */
{
    if(newdev == NULL)
        return;

#if 0		
	outb(0, s->hwIOBase + CODEC_CMI_INT_HLDCLR + 2);  /* disable ints */
	synchronize_irq();
	outb(0, s->hwIOBase + CODEC_CMI_FUNCTRL0 + 2); /* reset channels */
#endif

    (*newdev->func_table->RipTideHwRelease)(newdev->pHwInfo);
    cnxt_rt_close(newdev->pHwContext);
    cnxt_unregister_hardware(newdev);
    release_device(newdev);
    printk(KERN_INFO "<RTLOW> unload\n");
}
