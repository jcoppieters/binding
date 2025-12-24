// FactoryNodeLCDScreenmaps.cpp: implementation of the CFactoryNodeLCDScreenmaps class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "FactoryNodeLCDScreenmaps.h"

#include "NodeLcdScreenmaps.h"
#include "screenmapid.h"
#include "screenmaptypes.h"
#include "cannode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace
{
	// Screenmaps units : V64 en V65 nodes.
	const CScreenmapID screenmapTypes_units_v64[ SCREENMAP_MAX_V64 ] = 
	{ 
		CScreenmapID(SCREENMAP_DIMMER),
		CScreenmapID(SCREENMAP_SWITCH),
		CScreenmapID(SCREENMAP_DUOSWITCH),
		CScreenmapID(SCREENMAP_AUDIO),
		CScreenmapID(SCREENMAP_SENS),
		CScreenmapID(SCREENMAP_CONTROLS),
	};


#if (USE_SMAP_ENERGYMEASURE == 1)
	// Sedert V16.21: Nodig voor de LCD nodes >= V66.11.00
	const CScreenmapID screenmapTypes_units_v661100[ SCREENMAP_MAX_V661100 ] = 
	{ 
		CScreenmapID(SCREENMAP_DIMMER),
		CScreenmapID(SCREENMAP_SWITCH),
		CScreenmapID(SCREENMAP_DUOSWITCH),
		CScreenmapID(SCREENMAP_AUDIO),
		CScreenmapID(SCREENMAP_SENS),
		CScreenmapID(SCREENMAP_CONTROLS),
		CScreenmapID(SCREENMAP_ENERGYMEASURE),
	};

#endif 



	// Screenmap main, enkel v65
	const CScreenmapID screenmapTypes_main_v65[ 3 ] = 
	{ 	
		CScreenmapID(SCREENMAP_V65_MAIN_MOODS),
		CScreenmapID(SCREENMAP_V65_MAIN_SENSOR),
		CScreenmapID(SCREENMAP_V65_MAIN_AUDIO)
	};


	// Screenmap my home, enkel V65
	const CScreenmapID screenmapTypes_myHome_v65[ 2 ] = 
	{ 	
		CScreenmapID(SCREENMAP_V65_MYHOME),
		CScreenmapID(SCREENMAP_V65_MYHOME_SENS)	
	};
};


CNodeLcdScreenmaps* CFactoryNodeLCDScreenmaps::create(CNodeDatabase* pNodeDatabase,CCanNode* pNode, Type type)
{
	CNodeLcdScreenmaps* pNew = 0;
	int maxScreenmapping = -1;
	const CScreenmapID* pScreenmapTypes = 0;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{
		case 0x64: {
			switch (type) {
				case TYPE_SCREENMAP_UPDATE_UNITS: {
					pScreenmapTypes = &screenmapTypes_units_v64[0];
					maxScreenmapping = SCREENMAP_MAX_V64;
				} break;
				case TYPE_SCREENMAP_UPDATE_MYHOME: {					
					ASSERT( 0 );	// geen screenmaps ...
				} break;
				case TYPE_SCREENMAP_UPDATE_MAIN: {		
					ASSERT( 0 );	// geen screenmaps ...
				} break;
			}
		} break;
		

#if (USE_SMAP_ENERGYMEASURE == 0)		
		case 0x65:
		case 0x66:
		{
			switch (type) {
				case TYPE_SCREENMAP_UPDATE_UNITS: {
					pScreenmapTypes = &screenmapTypes_units_v64[0];
					maxScreenmapping = SCREENMAP_MAX_V64;		
				} break;
				case TYPE_SCREENMAP_UPDATE_MYHOME: {
					pScreenmapTypes = &screenmapTypes_myHome_v65[0];
					maxScreenmapping = 2;
				} break;
				case TYPE_SCREENMAP_UPDATE_MAIN: {
					pScreenmapTypes = &screenmapTypes_main_v65[0];
					maxScreenmapping = 3;		
				} break;
			}
		} break;
#endif

#if (USE_SMAP_ENERGYMEASURE == 1)
		case 0x65:
		case 0x66:
		{			
			switch (type) {
				case TYPE_SCREENMAP_UPDATE_UNITS: {
					bool useV64 = true;
					if (bSoftwareVersion == 0x66) { 
						const BYTE bMinorRevision = pNode->GetSoftwareVersionRev();
						if (bMinorRevision >= 0x11) {
							useV64 = false;
						}
					}
					if (useV64) {
						pScreenmapTypes = &screenmapTypes_units_v64[0];
						maxScreenmapping = SCREENMAP_MAX_V64;		
					}
					else {
						pScreenmapTypes = &screenmapTypes_units_v661100[0];
						maxScreenmapping = SCREENMAP_MAX_V661100;		
					}
				} break;
				case TYPE_SCREENMAP_UPDATE_MYHOME: {
					pScreenmapTypes = &screenmapTypes_myHome_v65[0];
					maxScreenmapping = 2;
				} break;
				case TYPE_SCREENMAP_UPDATE_MAIN: {
					pScreenmapTypes = &screenmapTypes_main_v65[0];
					maxScreenmapping = 3;		
				} break;
			}
		} break;
#endif 
	}

	if (maxScreenmapping > 0) {
		pNew = new CNodeLcdScreenmaps( pNodeDatabase, pNode, maxScreenmapping, pScreenmapTypes );
		ASSERT( pNew );
	}
	return pNew;
}
