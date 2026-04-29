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

  MODULE NAME:     int_mdl.c                       

  AUTHOR:          Konstantin Lazarev    (Transcribed by KNL)

  HISTORY:         Major Revision               Date        By
            -----------------------------     --------     -----
            Created                           10/01/99      KNL

  DESCRIPTION:

    This module is hardware interface child driver for Conexant RipTide.
    
  NOTES:    None.
*/

#include <linux/config.h>
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
#include <linux/module.h>
/*#include <asm/spinlock.h>*/
#include <asm/uaccess.h>
#include <asm/hardirq.h>
#include <linux/wait.h>
#include <linux/kmod.h>

#include "cnxt_int.h"
//#include "lowlevelrp_cmdif.h"

HwContext *devs = NULL;

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
void release_device(HwContext *phwInfo)
{
    printk("<RTINT> Delete device: %p\n", phwInfo);
    if(!phwInfo)
    {
        printk("<RTINT> No device descriptor.\n");
        return;
    }

    if(phwInfo->ioPort)
        release_region(phwInfo->ioPort, 64);
    else
        printk("<RTINT> No io region to release\n");

    kfree_s(phwInfo, sizeof(HwContext));
    printk("<RTINT> Device resources are released\n");
}

HwContext *cnxt_rt_open(int ioPort, int deviceId)
{
HwContext *p = devs, *ret = NULL;

    while(p != NULL)
    {
        if(p->ioPort == ioPort)
        {
            ret = p;
            break;
        }  
        p = p->next;
    }
    if(p == NULL)
    {
        if (!(p = kmalloc(sizeof(HwContext), GFP_KERNEL)))
        {
            printk(KERN_ERR"<RTINT> out of memory\n");
        }
        else
        {
            p->ioPort = ioPort;
            p->pInterface = NULL;
            if((p->pfunc = (voidfunc)RiptideInitialize(&p->pInterface, deviceId, ioPort)) == NULL)
            {  
                printk(KERN_ERR"<RTINT> Can't init interface\n");
            }
            else
            {
                init_spinlock(&p->sysObj);
                if (check_region(ioPort, 64))
                {
                    printk(KERN_ERR "<RTINT> io ports %#x-%#x in use\n", ioPort, ioPort+63);
                }
                else
                {
/* allocate region for I/O ports */
                    printk(KERN_INFO "<RTINT> requesting io ports %#x-%#x\n", ioPort, ioPort+63);
                    request_region(p->ioPort, 64, "riptide");
                    ret = p;
                    p->next = devs;
                    devs = p;
                }
            }
        }
    }
    if(ret != NULL)
        MOD_INC_USE_COUNT;
    else
        release_device(p);
    return ret;
}

void cnxt_rt_close(HwContext *pContext)
{
HwContext *p = devs;
HwContext **pp = &devs;

    while(p != NULL)
    {
        if(p == pContext)
        {
            *pp = p->next;
            release_device(pContext);
            break;
        }
        pp = &(p->next);
        p = p->next;
    }
    if(p != NULL)
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
MODULE_AUTHOR("Conexant Systems Inc.");
MODULE_DESCRIPTION("RipTide Hardware Interface Driver");
MODULE_LICENSE("Copyright (C) 1995-2002 Conexant Systems Inc. All Rights Reserved.");

EXPORT_SYMBOL_NOVERS(cnxt_rt_open);
EXPORT_SYMBOL_NOVERS(cnxt_rt_close);

int init_module(void)
{
    return 0;
}

void cleanup_module(void)
{
}
