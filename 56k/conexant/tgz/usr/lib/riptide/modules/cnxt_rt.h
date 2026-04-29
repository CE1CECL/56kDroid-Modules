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
  MODULE NAME:        cnxt_rt.h                       

  AUTHOR:             K. Lazarev  (Transcribed by KNL)

  HISTORY:         Major Revision               Date        By
            -----------------------------     --------     -----
            Created                           02/1/2000     KNL


  DESCRIPTION:

    This module contains definitions for communicating with the
    hardware on the RipTide card.

  NOTES:    None.
*/
#ifndef _CNXT_RT_H_
#define _CNXT_RT_H_

#undef DBG

#include "linux.h"
#include "riptidem.h"

/*
 *  Board ID
 */

#define RIPTIDE_VENDOR_ID       0x127A  /* Rockwell International */
#define RIPTIDE_DEVICE_ID       0x4320  /* A0  Board              */

void *RipTideHwInitialize
(
    unsigned int    hwIOBase,
    int             hwIrq,
    rt_func_table   *func_table,
    void            *CMDIFp,
    void            (*pfunc)(void),
    system_objects  *sysObj
);

#endif /* _CNXT_RT_H_ */
