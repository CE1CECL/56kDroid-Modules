/*
 * Copyright (c) 2003 Linuxant inc.
 * Copyright (c) 2001-2003 Conexant Systems, Inc.
 *
 * NOTE: The use and distribution of this software is governed by the terms in
 * the file LICENSE, which is included in the package. You must read this and
 * agree to these terms before using or distributing this software.
 *
 */
#include <linux/config.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/ioport.h>
//#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/fs.h>
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
#include <linux/highmem.h>
#endif

#define __KERNEL_SYSCALLS__
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>

#include "linux.h"

/*
    Wait for timeout.
    Holds up the calling process for period of "time" microseconds of until signal is received
    This function provides time granularity not better that one tick
*/
void wait_for_timeout(unsigned long time)
{
    time = (time)? time: 1;
    if(time > 1000)
    {
		current->state   = TASK_INTERRUPTIBLE;
		schedule_timeout(HZ*time/1000000L);
    }
    else
        udelay(time);
}

void initialize_timer(system_objects *sysObj, 
                      void (*pfunc)(unsigned long),
                      void *pdata)
{
struct timer_list *ptimer = &sysObj->timer;
    init_timer(ptimer);
    ptimer->function = pfunc;
    ptimer->data = (unsigned long)pdata;
}

void set_timer(system_objects *sysObj,
               unsigned long time)
{
struct timer_list *ptimer = &sysObj->timer;

    ptimer->expires = jiffies + (HZ*time)/(1000000L);
/*    printk("<LINUX> Add timer: %d %d\n", jiffies, ptimer->expires);*/
    add_timer(ptimer);
}

inline void cancel_timer(system_objects *sysObj)
{
    del_timer(&sysObj->timer);
}

inline void init_spinlock(system_objects *sysObj)
{
    spin_lock_init(&sysObj->lock);
}

inline void acquire_spinlock(system_objects *sysObj)
{
	spin_lock_irqsave(&sysObj->lock, sysObj->flags);
}

inline void release_spinlock(system_objects *sysObj)
{
	spin_unlock_irqrestore(&sysObj->lock, sysObj->flags);
}

inline void *allocate_mem(int size)
{
    return (void *)kmalloc(size, GFP_KERNEL);
}

inline void *allocate_dmabuf(int size)
{
    return (void *)kmalloc(size, GFP_KERNEL | GFP_DMA);
}

inline void *allocate_virtual(int size)
{
    return (void *)vmalloc(size);
}

inline void free_mem(void *p, int size)
{
    kfree_s(p, size);
}

inline void free_virtual(void *p)
{
    vfree(p);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
inline unsigned long virtual2bus(void *virt)
{
    pgd_t   *pgd;
    pmd_t   *pmd;
    pte_t   *pte;
    unsigned long a = (unsigned long)virt;

    if( pgd_none(*(pgd = pgd_offset(current->mm, a))) ||
        pmd_none(*(pmd = pmd_offset(pgd,         a))) ||
        pte_none(*(pte = pte_offset(pmd,         a))))
            return 0;
    return virt_to_bus((void *)pte_page(*pte))
            + (a & (PAGE_SIZE - 1));
}

inline unsigned long uvirt_to_kva(pgd_t *pgd, unsigned long adr)
{
        unsigned long ret = 0UL;
	pmd_t *pmd;
	pte_t *ptep, pte;
  
	if (!pgd_none(*pgd)) {
                pmd = pmd_offset(pgd, adr);
                if (!pmd_none(*pmd)) {
                        ptep = pte_offset(pmd, adr);
                        pte = *ptep;
                        if(pte_present(pte)) {
                                ret = (unsigned long)
									page_address(pte_page(pte));
								ret |= (adr & (PAGE_SIZE - 1));
						}
                }
        }
//    printk("uv2kva(%lx-->%lx)\n", adr, ret);
	return ret;
}

inline unsigned long uvirt_to_bus(unsigned long adr) 
{
    unsigned long kva, ret;

    kva = uvirt_to_kva(pgd_offset(current->mm, adr), adr);
	ret = virt_to_bus((void *)kva);
//    printk("uv2b(%lx-->%lx)\n", adr, ret);
    return ret;
}


inline unsigned long kvirt_to_kva(unsigned long adr) 
{
    unsigned long va, kva;

    va = VMALLOC_VMADDR(adr);
    kva = uvirt_to_kva(pgd_offset_k(va), va);
    return kva;
}

inline unsigned long kvirt_to_bus(unsigned long adr) 
{
    unsigned long va, kva, ret;

    va = VMALLOC_VMADDR(adr);
    kva = uvirt_to_kva(pgd_offset_k(va), va);
	ret = virt_to_bus((void *)kva);
    //printk("kv2b(%lx-->%lx->%lx)\n", adr, kva, ret);
    return ret;
}

unsigned long kvirt_to_phys(unsigned long adr) 
{
    unsigned long va, kva, ret;

    va = VMALLOC_VMADDR(adr);
    kva = uvirt_to_kva(pgd_offset_k(va), va);
	ret = virt_to_phys((void *)kva);
//    dprintf2(("kv2p(%lx-->%lx)\n", adr, ret));
    return ret;
}
#else
/*******************************/
/* Memory management functions */
/*******************************/

/* convert virtual user memory address to physical address */
/* (virt_to_phys only works for kmalloced kernel memory) */

inline unsigned long uvirt_to_phys(unsigned long adr)
{
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *ptep, pte;
  
	pgd = pgd_offset(current->mm, adr);
	if (pgd_none(*pgd))
		return 0;
	pmd = pmd_offset(pgd, adr);
	if (pmd_none(*pmd))
		return 0;
	ptep = pte_offset(pmd, adr/*&(~PGDIR_MASK)*/);
	pte = *ptep;
	if(pte_present(pte))
		return 
		  virt_to_phys((void *)(pte_page(pte)|(adr&(PAGE_SIZE-1))));
	return 0;
}

inline unsigned long uvirt_to_bus(unsigned long adr) 
{
	return virt_to_bus(phys_to_virt(uvirt_to_phys(adr)));
}

/* convert virtual kernel memory address to physical address */
/* (virt_to_phys only works for kmalloced kernel memory) */

unsigned long kvirt_to_phys(unsigned long adr) 
{
	return uvirt_to_phys(VMALLOC_VMADDR(adr));
}

inline unsigned long kvirt_to_bus(unsigned long adr) 
{
	return uvirt_to_bus(VMALLOC_VMADDR(adr));
}
#endif

inline unsigned long virt2bus(void *virt)
{
    return virt_to_bus(virt);
}

inline unsigned long copy_from_dma(void *to, void *from, size_t size)
{
    return copy_to_user(to, from, size);
}

inline unsigned long copy_to_dma(void *to, void *from, size_t size)
{
    return copy_from_user(to, from, size);
}

static int errno;
static int do_mod_firmware_load(const char *fn, char **fp)
{
	int fd;
	long l;
	char *dp;

	fd = open(fn, 0, 0);
	if (fd == -1)
	{
		printk(KERN_INFO "Unable to load '%s'.\n", fn);
		return 0;
	}
	l = lseek(fd, 0L, 2);
	if (l <= 0 || l > 131072)
	{
		printk(KERN_INFO "Invalid firmware '%s'\n", fn);
		sys_close(fd);
		return 0;
	}
	lseek(fd, 0L, 0);
	dp = vmalloc(l);
	if (dp == NULL)
	{
		printk(KERN_INFO "Out of memory loading '%s'.\n", fn);
		sys_close(fd);
		return 0;
	}
	if (read(fd, dp, l) != l)
	{
		printk(KERN_INFO "Failed to read '%s'.\n", fn);
		vfree(dp);
		sys_close(fd);
		return 0;
	}
	close(fd);
	*fp = dp;
	return (int) l;
}

int load_file(const char *fn, char **fp)
{
	int r;
	mm_segment_t fs = get_fs();

	set_fs(get_ds());
	r = do_mod_firmware_load(fn, fp);
	set_fs(fs);
	return r;
}

/*
char dbg_str[256];

void dbg_print(int i)
{
    printk("%s", dbg_str);
}
*/

void dbg_print(char *pformat, ...)
{

char *p, c;
va_list ap;
int flag;

    va_start(ap, pformat);

    p = pformat;
    flag = 0;
    do
    {
        while(*p != '%' && *p != '\0')
            p++;
        c = *p;
        if(*p != '\0')
        {
            if(*(p+1) == '%')
            {
                p += 2;
                continue;
            }
            *p = '\0';
        }
        if(flag++)
            switch(*(pformat+1))
            {
                case 'l':
                    printk(pformat, va_arg(ap, long));
                    break;
                case 'd':
                case 'c':
                case 'x':
                default:
                    printk(pformat, va_arg(ap, unsigned int));
                    break;
                case 's':
                    printk(pformat, va_arg(ap, char *));
                    break;
            }
        else
            printk(pformat);
        *p = c;
        pformat = p;
        p += 2;
    }
    while(*pformat != '\0');

    va_end(ap);
}
