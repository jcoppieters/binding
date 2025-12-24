// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__31536D47_354B_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_STDAFX_H__31536D47_354B_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
/*#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls*/


#include <afxctl.h>			// NEEDED FOR CPICTUREHOLDER 

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

///////////////////////////////////////////////////////////////////

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

///////////////////////////////////////////////////////////////////

#define FALSE		0
#define TRUE		1

//#define USE_MSCOMMCONTROL							// TM20080128

///////////////////////////////////////////////////////////////////

#if !defined(USE_MSCOMMCONTROL)						// TM 20080128

	#define INCLUDE_DUOTECNO_RS232

	#include "RS232\QAssert.h"
	#include "RS232\Kernel_Include.h"
#endif

///////////////////////////////////////////////////////////////////
#define QHSM_APPLICATIONGLOBAL_IMPLEMENTATION			1
#define BINDING_SOFTWARE					


// Tijdelijke macros - Integra alarm
#define ALARM_INTEGRA_USE_ARM_MODES						1		// TM_CHANGES_ALARM_INTEGRA
#define ALARM_INTEGRA_USE_REMOTE_CODE					1		// TM_CHANGES_ALARM_INTEGRA

// Sedert V15.03
// Mogelijkheid om een tcp/ip als master door een touchscreen up te graden...	
#ifndef USE_UPGRADE_TCPIP_MASTER
	#define USE_UPGRADE_TCPIP_MASTER	1
#endif 

// Sedert V15.06
// Dimmer modules die toegevoegd worden aan een installatie de minimum waarde op 20% zetten.
#ifndef USE_DIMMER_PBINDINGS_MINVALUE
	#define USE_DIMMER_PBINDINGS_MINVALUE	1
#endif 

// sedert V15.13
// De sensoren van de OLEDS & drukknop interface:
// De modules die toegevoegd worden aan een installatie de offset van de gemeten waarde -2C
#ifndef USE_SENSOR_PBINDINGS_OFFSETVALUE
	#define USE_SENSOR_PBINDINGS_OFFSETVALUE	1
#endif 

// sedert V15.13
// De rolluik modules met firmware versie < V90.25.
// De modules die toegevoegd worden aan een installatie de switch tijd (= tijd omschakeling op/neer) op 1.2 sec.
#ifndef USE_DUOSWITCH_PBINDINGS_SWITCHTIME
	#define USE_DUOSWITCH_PBINDINGS_SWITCHTIME	0		// Momenteel op 0 laten staan.
#endif 
#ifndef USE_TOOLTIP_CONTROLS
	#define USE_TOOLTIP_CONTROLS					0	// Testen.
#endif 
#ifndef USE_TOOLS_APPREMOTECFG
	#define USE_TOOLS_APPREMOTECFG					0	// TM20220624: Verwijderd.
#endif 
///////////////////////////////////////////////////////////////////
// Sedert V16.13.
//
// Start integratie van de UDP config tool.
//  - Library include files:
//		$(SolutionDir)\libraries\dtudpconfiglib
//  - library 
//		$(SolutionDir)\Debug\lib\vs2008\dtudpconfigLIB
//      $(SolutionDir)\Release\lib\vs2008\dtudpconfigLIB
//  - preprocessor:
//		DTUDPCONFIGSTATIC_LINK
// 
//  Vroeger macro USE_UDPCONFIG_LIBRARY



////////////////////////////////////////////////////////////////////////
// Windows 8.1 / visual studio 2008
//	LogFont info:
//		lfheight = -12
//		lfWeight = 400
//		lfFaceName = "Segoe UI"
////////////////////////////////////////////////////////////////////////
// Sedert V16.16
#undef USE_DIALOG_SYSTEMFONTS

#ifndef USE_DIALOG_SYSTEMFONTS
	#define USE_DIALOG_SYSTEMFONTS					1
#endif 


// Sedert V16.16
// Oplossing weergave van de iconen op windows 10 ????
#undef USE_CHANGES_IMAGELISTS

#ifndef USE_CHANGES_IMAGELISTS
	#define USE_CHANGES_IMAGELISTS		1
#endif 

// Sedert V16.21
// Screenmapping van de virtuele energie meet units onder een aparte menu optie.
// Wordt ondersteund door de firmware V66.11.00
#undef USE_SMAP_ENERGYMEASURE

#ifndef USE_SMAP_ENERGYMEASURE
	#define USE_SMAP_ENERGYMEASURE		1						
#endif

// Sedert V16.35
// DALI commandos versturen/ontvangen via het TCP/IP protocol.
#undef USE_TCPIP_DALI 
#ifndef USE_TCPIP_DALI
	#define USE_TCPIP_DALI				1
#endif 
///////////////////////////////////////////////////////////////////
#pragma warning(disable: 4996)		// CHANGES_MVS_2008	

#endif // !defined(AFX_STDAFX_H__31536D47_354B_11D8_B865_0050BAC412B1__INCLUDED_)
#include <afxdlgs.h>
#include <afxdhtml.h>
