/*
 * Copyright (c) 2003 Linuxant inc.
 * Copyright (c) 2001-2003 Conexant Systems, Inc.
 *
 * NOTE: The use and distribution of this software is governed by the terms in
 * the file LICENSE, which is included in the package. You must read this and
 * agree to these terms before using or distributing this software.
 *
 */
#ifndef __LINUX__H
#define __LINUX__H

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/config.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/sched.h>
//#include <asm/spinlock.h>
#include <linux/timer.h>
#include <linux/tqueue.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <asm/page.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#ifndef get_user_ret
#define get_user_ret(x,ptr,ret) ({ if (get_user(x,ptr)) return ret; })
#endif

#ifdef CONFIG_KDB
#define KDB_ENTER() asm("\tint $129\n")
#else
#define KDB_ENTER()
#endif

#ifndef minor
#define minor(d) MINOR(d)
#endif

#ifdef MODULE
#ifndef THIS_MODULE
#define THIS_MODULE (&__this_module)
#endif
#ifndef module_init
#define module_init(x)  int init_module(void) { return x(); }
#endif
#ifndef module_exit
#define module_exit(x)  void cleanup_module(void) { x(); }
#endif
#endif

#ifndef MODULE_LICENSE
#define MODULE_LICENSE(x)
#endif

#ifndef __exit
#define __exit
#endif

#define KdPrint  dprintf

#define dprintf(t) dbg_print t

#if DBG
#define dprintf2  dprintf

#define TRACE dprintf

#define DbgPrint dprintf

#define ASSERT(t)
#else
#define dprintf2(t)

#define TRACE(t)

#define DbgPrint

#define ASSERT(t)
#endif
/*
#define KdPrint(t)

#define dprintf(t)
*/
/*
    Definition of base types
*/

typedef unsigned char UCHAR, *PUCHAR;
typedef unsigned long ULONG;
typedef unsigned int UINT;
typedef unsigned long DWORD, *PDWORD;
typedef unsigned short USHORT;
typedef void VOID, *PVOID;
typedef int BOOLEAN;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef int BOOL;

#define FALSE	0
#define TRUE	1

#define C_DEFINITION

#define NTSTATUS int
/*
 *  Access to hardware I/O ports
 */
typedef unsigned int HW_PORT;

#define  READ_PORT_ULONG(p)     inl(p)
#define  WRITE_PORT_ULONG(p,x)  outl(x,p)
#define  READ_PORT_UCHAR(p)     inb(p)
#define  WRITE_PORT_UCHAR(p,x)  outb(x,p)
#define  READ_PORT_USHORT(p)    inw(p)
#define  WRITE_PORT_USHORT(p,x) outw(x,p)

typedef struct _system_objects
{
    spinlock_t          lock;
	unsigned long       flags;
    struct timer_list   timer;
} system_objects;

void wait_for_timeout(unsigned long time);

inline void init_spinlock(system_objects *sysObj);

void initialize_timer(system_objects *sysObj, 
                      void (*pfunc)(unsigned long),
                      void *pdata);

void set_timer(system_objects *sysObj, unsigned long time);

inline void cancel_timer(system_objects *sysObj);

inline void acquire_spinlock(system_objects *sysObj);

inline void release_spinlock(system_objects *sysObj);

inline void *allocate_mem(int size);

inline void *allocate_dmabuf(int size);

inline void *allocate_virtual(int size);

inline void free_mem(void *p, int size);

inline void free_virtual(void *p);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
#define kfree_s(p, s) kfree(p)
#endif

#if !defined(pte_offset) && defined(pte_offset_kernel)
#define pte_offset(dir, address) pte_offset_kernel(dir, address)
#endif

inline unsigned long virt2bus(void *virt);

inline unsigned long copy_to_dma(void *to, void *from, size_t size);

inline unsigned long copy_from_dma(void *to, void *from, size_t size);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
inline unsigned long virtual2bus(void *virt);

inline unsigned long uvirt_to_kva(pgd_t *pgd, unsigned long adr);

inline unsigned long uvirt_to_bus(unsigned long adr);

inline unsigned long kvirt_to_bus(unsigned long adr);

unsigned long kvirt_to_phys(unsigned long adr);

inline unsigned long kvirt_to_kva(unsigned long adr);
#else
inline unsigned long uvirt_to_phys(unsigned long adr);

inline unsigned long uvirt_to_bus(unsigned long adr);

unsigned long kvirt_to_phys(unsigned long adr);

inline unsigned long kvirt_to_bus(unsigned long adr);
#endif

int load_file(const char *fn, char **fp);

void dbg_print(char *pformat, ...);
#endif

