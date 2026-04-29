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

********************************************************************************************
********************** P V C S  V e r s i o n  I n f o r m a t i o n ***********************
********************************************************************************************
 $Header:   P:/d874/Linux/rtmixer.h_v   1.1   19 May 2000 14:57:14   lazarekn  $
********************************************************************************************
********************************************************************************************



  MODULE NAME:        mixer.h                        

  AUTHOR:             K. Kenney


  HISTORY:         Major Revision                   Date          By
            -----------------------------          --------     -----
            Created from MS W95 Sample             11/28/95      KRS
            RipTide                                10/15/97      KRS
            Adopted for Windows NT driver          01/20/98      CNL


  DESCRIPTION:

    This module contains definitions for use by the RipTide Ring
    3 mixer driver.


  NOTES:    None.
*/
//***************************************************************************
//***************************************************************************
#ifndef _RTMIXER_H_
#define _RTMIXER_H_

#define MAXSOURCES      8
#define MAXDESTINATIONS 2
//#define MAXCONTROLS     29
#define MORECONTROLS    2
#define MAXARM			13

#define MAXMUXSLCT      6
#define MAXMIXREG       0x22
#define MAXACREG        0x7E
#define RWASMMAX        6

//***************************************************************************
//***************************************************************************
// Defines for the uControlMap "columns"
#define CM_DEST     0
#define CM_SOURCE   1
#define CM_CHANNELS 2

//***************************************************************************
//***************************************************************************
// Define some stuff for sources...
#define SOURCE_WAVE     0
#define SOURCE_WSTRM    1
#define SOURCE_CD       2
#define SOURCE_LINE     3
#define SOURCE_VIDEO    4
#define SOURCE_AUX      5
#define SOURCE_MIC      6
#define SOURCE_MIXER    7
#define SOURCE_INVALID           (UINT_MAX)   /// for source mapping

//***************************************************************************
//***************************************************************************
// Define some stuff for destinations...
#define DEST_PLBK         0
#define DEST_RECD         1

//***************************************************************************
//***************************************************************************
// Define some stuff for controls...
//

// Select Controls
#define MXSEL_MIC        0
#define MXSEL_CD         1         
#define MXSEL_VIDEO      2
#define MXSEL_AUX        3
#define MXSEL_LINE       4
#define MXSEL_MIXER      5
#define MXSEL_MONO       6
#define MXSEL_PHN        7   

typedef enum
{
//Define Control numbers
MXMUX_SELECT = 0,// 0
MXMUX_MASTER ,   // 1

//Volume controls
MXMUX_MIC    ,// 2 ADC_Gain  
MXMUX_MIX    ,// 3 ADC_Gain  
MXMUX_CD     ,// 4 ADC_Gain  
MXMUX_LINE   ,// 5 ADC_Gain  
MXMUX_VIDEO  ,// 6 ADC_Gain  
MXMUX_AUX    ,// 7 ADC_Gain  

MXMIX_MASTER ,// 8 Mixer_Gain
MXMIX_WAVE0  ,// 9 
MXMIX_WAVE1  ,// 10
MXMIX_WAVE2  ,// 11
MXMIX_WAVE3  ,// 12
MXMIX_WAVE4  ,// 13
MXMIX_WAVE5  ,// 14
MXMIX_WAVE6  ,// 15
MXMIX_WAVE7  ,// 16
MXMIX_WAVE8  ,// 17
MXMIX_WAVE9  ,// 18
MXMIX_WSTRM  ,// 19
MXMIX_OPL    ,// 20
MXMIX_SB     ,// 21
MXMIX_PHONE  ,// 22
MXMIX_CD     ,// 23
MXMIX_LINE   ,// 24
MXMIX_VIDEO  ,// 25
MXMIX_AUX    ,// 26
MXMIX_MIC    ,// 27
PEAK_WAVE    ,// 28

//Mute controls
MXMUTE_MASTER,// 29
MXMUTE_WAVE0 ,// 30
MXMUTE_WAVE1 ,// 31
MXMUTE_WAVE2 ,// 32
MXMUTE_WAVE3 ,// 33
MXMUTE_WAVE4 ,// 34
MXMUTE_WAVE5 ,// 35
MXMUTE_WAVE6 ,// 36
MXMUTE_WAVE7 ,// 37
MXMUTE_WAVE8 ,// 38
MXMUTE_WAVE9 ,// 39
MXMUTE_WSTRM ,// 40
MXMUTE_OPL   ,// 41
MXMUTE_SB    ,// 42
MXMUTE_PHONE ,// 43
MXMUTE_CD    ,// 44
MXMUTE_LINE  ,// 45
MXMUTE_VIDEO ,// 46
MXMUTE_AUX   ,// 47
MXMUTE_MIC   ,// 48
PEAK_MUX     ,// 49

//Advanced Controls
MXMIX_3D     ,// 50
MXMIX_BASS   ,// 51
MXMIX_MICPGN ,// 52
MXMUX_MICPGN ,// 53
MXMUTE_MUX   ,// 54
MXMIX_BREAK  ,// 55
MXMUTE_BREAK ,// 56
MXMIX_I2S_0  ,// 57
MXMIX_I2S_1  ,// 58
MXMUTE_I2S_0 ,// 59
MXMUTE_I2S_1 ,// 60
MXMIX_HDPH   ,// 61
MXMUTE_HDPH  ,// 62
MAX_MIXER_CONTROLS //63
} MIXER_CONTROLS;


#define CONTROL_ATDEST           (UINT_MAX)   

#define AC98MAX		  (0x18)

#endif

