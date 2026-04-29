/*
 * Copyright (c) 2003-2004 Linuxant inc.
 * Copyright (c) 2001-2003 Conexant Systems, Inc.
 *
 * NOTE: The use and distribution of this software is governed by the terms in
 * the file LICENSE, which is included in the package. You must read this and
 * agree to these terms before using or distributing this software.
 *
 */
#include "oscompat.h"
#include "osservices.h"

#include "osresour_ex.h"
#include "oslinux.h"

#include <linux/pci.h>

static struct pci_device_id cnxthwpci_tbl[] =
{
    	/* 71DP */
        {0x127A, 0x1002, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1002, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1003, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1003, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1004, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1004, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1005, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1005, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1006, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1006, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},

        {0x127A, 0x1022, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1022, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1023, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1023, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1024, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1024, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1026, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1026, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},

	/* some of these might not have an EEPROM and in reality be HSF */
        {0x127A, 0x1025, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1025, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},

        {0x127A, 0x1032, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1032, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1033, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1033, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1034, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1034, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1035, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1035, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x1036, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1036, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},

	/* 1085 is P85 (part number R6795) without an EEPROM; might also be HSF? */
        {0x127A, 0x1085, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x1085, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},

	/* The 2xxx device ID was supposed to be reserved for HSF only, but Sony
	 * programmed some older Vaios with this ID for the 2-chip HCF. Phew!
	 */
        {0x127A, 0x2004, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x2004, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x2005, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x2005, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x127A, 0x2006, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},
        {0x14F1, 0x2006, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")},

        {0x1024, 0x1024, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")}, /* Zenith Data Systems */
        {0x148D, 0x1003, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")}, /* DIGICOM Systems */
        {0x158B, 0x0015, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")}, /* Allied Data Technologies Tornado VFM56x-PRC */

        {0x141A, 0x1035, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90")}, /* Apache Micro */

        {0x127A, 0x1802, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90lan")}, /* LAN/Modem Combo */
        {0x127A, 0x1804, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90lan")}, /* LAN/Modem Combo */
        {0x127A, 0x1805, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90lan")}, /* LAN/Modem Combo */
        {0x14F1, 0x1813, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90lan")}, /* LAN/Modem Combo */
        {0x14F1, 0x1815, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v90lan")}, /* LAN/Modem Combo */


        {0x14F1, 0x10B3, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92")},
        {0x14F1, 0x10B4, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92")},
        {0x14F1, 0x10B5, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92")},
        {0x14F1, 0x10B6, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92")},

        {0x14F1, 0x1F10, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92")},
        {0x14F1, 0x1F11, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92")},
        {0x14F1, 0x1F14, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92")},
        {0x14F1, 0x1F15, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92")},

	/* SmartHCF 20434/20463 world wide configuration,  (board layout rev. 3) */
        {0x14F1, 0x1053, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1054, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1055, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1056, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},

	/* SmartHCF 20434/20463 world wide or US (board layout rev. 4) */
        {0x14F1, 0x1052, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1057, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1059, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},

	/* SmartHCF 20434/20463 US only configuration (mostly Dell) */
        {0x14F1, 0x1063, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1064, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1065, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1066, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},

	/* mini-PCI SmartHCF 20434/20463 world wide configuration (board layout rev. 3) */
        {0x14F1, 0x1453, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1454, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1455, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1456, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},

	/* mini-PCI SmartHCF 20434/20463 world wide or US (board layout rev. 4) */
        {0x14F1, 0x1452, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1457, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},
        {0x14F1, 0x1459, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("v92smart")},


        {0x127A, 0x4321, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("riptide")},
        {0x14F1, 0x4321, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CNXTHWCFG("riptide")},


        {0,}                    /* terminate list */
};

#define CNXTHWPCI_NAME "hcfpcihw"

#include "cnxthwpci_common.c"
