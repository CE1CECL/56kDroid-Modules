/*
 * Copyright (c) 2003 Linuxant inc.
 * Copyright (c) 2001-2003 Conexant Systems, Inc.
 *
 * NOTE: The use and distribution of this software is governed by the terms in
 * the file LICENSE, which is included in the package. You must read this and
 * agree to these terms before using or distributing this software.
 *
 */
#include "linux.h"

typedef struct _HwContext
{
    struct _HwContext *next;
    int             ioPort;
    void            *pInterface;
    void            (*pfunc)(void);
    system_objects  sysObj;
} HwContext;

typedef void (*voidfunc)(void);

C_DEFINITION
voidfunc
RiptideInitialize(
  void      **pContext,
  USHORT    device_id,
  HW_PORT   base_io
);

void RiptideRelease
(void *pContext);
