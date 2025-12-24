#include "stdafx.h"
#include "nodescreenmapinfo.h"
#include "cannode.h"
#include "screenmaptypes.h"

#include "app/appcfg/appcfginfo/cfginfoproductid_def.h"


bool CNodeScreenmapInfo_Util::getMaxControls(
	CCanNode* pNode, 
	const CScreenmapID* pTypes, 
	int* pNrofButtons, 
	int MaxTypes)
{
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (bSoftwareVersion == 0x64) 
	{
		const BYTE bNodeType = pNode->bNodeType;			// CHANGES_MVS_2008
		if (bNodeType == NODE_LCD)
		{
			const BYTE bNodeFlags = pNode->bNodeFlags;
			if ((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_HC4) 
			{
				for (int i=0;i<MaxTypes;i++)
				{
					const screenmaptype type = (const screenmaptype) pTypes[i].get( );
					switch( type )
					{
						case SCREENMAP_CONTROLS:
						case SCREENMAP_SWITCH:
						case SCREENMAP_DIMMER:
						case SCREENMAP_SENS:			
						case SCREENMAP_AUDIO:
						{
							pNrofButtons[i] = 9;					
						} break;			
						case SCREENMAP_DUOSWITCH:
						{
							pNrofButtons[i] = 4;
						} break;
						default:
						{
							pNrofButtons[i] = 0;	
						} break;
					}
				}
				return true;
			}
			else if ((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_HC5) 
			{
				for (int i=0;i<MaxTypes;i++)
				{
					const screenmaptype type = (const screenmaptype) pTypes[i].get( );
					switch( type )
					{
						case SCREENMAP_CONTROLS:
						case SCREENMAP_SWITCH:
						case SCREENMAP_DIMMER:
						case SCREENMAP_SENS:			
						case SCREENMAP_AUDIO:
						{
							pNrofButtons[i] = 9;					
						} break;			
						case SCREENMAP_DUOSWITCH:
						{
							pNrofButtons[i] = 2;
						} break;
						default:
						{
							pNrofButtons[i] = 0;	
						} break;
					}
				}
				return true;
			}
		}
	}
	else if (bSoftwareVersion == 0x65)
	{
		const BYTE bNodeType = pNode->bNodeType;			// CHANGES_MVS_2008
		if (bNodeType == NODE_LCD)
		{
/*
			const BYTE bNodeFlags = pNode->bNodeFlags;
			if ((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_HC4) 
			{
*/
			for (int i=0;i<MaxTypes;i++)
			{
				const screenmaptype type = (const screenmaptype) pTypes[i].get();
				switch( type )
				{
					case SCREENMAP_CONTROLS:
					case SCREENMAP_SWITCH:
					case SCREENMAP_DIMMER:
					case SCREENMAP_SENS:			
					case SCREENMAP_AUDIO:
					{
						pNrofButtons[i] = 10;					
					} break;			
					case SCREENMAP_DUOSWITCH:
					{
						pNrofButtons[i] = 5;
					} break;				
					case SCREENMAP_V65_MAIN_MOODS:
					{
						pNrofButtons[i] = 10;	
					} break;
					case SCREENMAP_V65_MAIN_SENSOR:
					{
						pNrofButtons[i] = 1;
					} break;
					case  SCREENMAP_V65_MAIN_AUDIO:
					{
						pNrofButtons[i] = 1;	
					} break;	
					case SCREENMAP_V65_MYHOME:
					{
						pNrofButtons[i] = 10;			
					} break;
					case SCREENMAP_V65_MYHOME_SENS:
					{
						pNrofButtons[i] = 1;	
					} break;
					default:
					{
						pNrofButtons[i] = 0;	
					} break;
				}
			}
			return true;
/*
			}
*/
		}
	}
	else if (bSoftwareVersion == 0x66) {
		const BYTE bNodeType = pNode->bNodeType;			
		if (bNodeType == NODE_LCD) {
			for (int i=0;i<MaxTypes;i++) {
				const screenmaptype type = (const screenmaptype) pTypes[i].get();
				switch( type )
				{
					case SCREENMAP_CONTROLS:
					case SCREENMAP_SWITCH:
					case SCREENMAP_DIMMER:
					case SCREENMAP_SENS:			
					case SCREENMAP_AUDIO:
					case SCREENMAP_DUOSWITCH:
					case SCREENMAP_V65_MYHOME:
#if(USE_SMAP_ENERGYMEASURE == 1)
					case SCREENMAP_ENERGYMEASURE:			// Sedert V16.21 (Firmware V66.11.00)
#endif 
					{
						pNrofButtons[i] = 0xFFFF;					
					} break;					
					// Sedert v0x0E06
					// Er zijn 20 buttons waar telkens 3 units kunnen aangekoppeld worden...
					// puls / toggle & feedback unit.
					case SCREENMAP_V65_MAIN_MOODS:
					{
						pNrofButtons[i] = 60;	// 20;	
					} break;
					case SCREENMAP_V65_MAIN_SENSOR:
				    case SCREENMAP_V65_MAIN_AUDIO:
					case SCREENMAP_V65_MYHOME_SENS:
					{
						pNrofButtons[i] = 1;
					} break;													
				}
			}
			return true;
		}
	}
	return false;
};

bool CNodeScreenmapInfo_Util::getSupportedTypes(
	CCanNode* pNode, 
	const CScreenmapID* pTypes, 
	int* supportedUnitTypes, 
	int MaxTypes)
{
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (bSoftwareVersion == 0x64) 
	{
		const BYTE bNodeType = pNode->bNodeType;		// CHANGES_MVS_2008
		if (bNodeType == NODE_LCD)
		{
			const BYTE bNodeFlags = pNode->bNodeFlags;
			if (((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_HC4) ||
				((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_HC5))				
			{
				for (int i=0;i<MaxTypes;i++)
				{
					const screenmaptype type = (const screenmaptype) pTypes[i].get();
					switch( type )
					{
						case SCREENMAP_DIMMER:
						case SCREENMAP_SWITCH:
						{
							supportedUnitTypes[i] = SCREENMAP_UNITTYPE_DIM +
													SCREENMAP_UNITTYPE_SWITCH +
													SCREENMAP_UNITTYPE_CONTROL +
													SCREENMAP_UNITTYPE_VIRTUAL;
						} break;

						case SCREENMAP_CONTROLS:
						{
							supportedUnitTypes[i] = SCREENMAP_UNITTYPE_CONTROL +
													SCREENMAP_UNITTYPE_VIRTUAL;
							
						} break;
					
						case SCREENMAP_SENS:
						{
							supportedUnitTypes[i] = SCREENMAP_UNITTYPE_SENS;
						} break;
							
						case SCREENMAP_AUDIO:
						{
							supportedUnitTypes[i] = SCREENMAP_UNITTYPE_AUDIO +
													SCREENMAP_UNITTYPE_BOSE;
								
						} break;		
						
						case SCREENMAP_DUOSWITCH:
						{
							supportedUnitTypes[i] = SCREENMAP_UNITTYPE_DUOSWITCH;

						} break;
						default:
						{
							supportedUnitTypes[i] = 0;
							
						} break;
					}
				}
				return true;
			}		
		}
	}
	else if ((bSoftwareVersion == 0x65)||(bSoftwareVersion == 0x66))
	{
		const BYTE bNodeType = pNode->bNodeType;		// CHANGES_MVS_2008
		if (bNodeType == NODE_LCD)
		{
			for (int i=0;i<MaxTypes;i++)
			{
				const screenmaptype type = (const screenmaptype) pTypes[i].get();
				switch( type )
				{
					case SCREENMAP_DIMMER:					
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_DIM +
												SCREENMAP_UNITTYPE_SWITCH +
												SCREENMAP_UNITTYPE_CONTROL +
												SCREENMAP_UNITTYPE_VIRTUAL;
					} break;
					case SCREENMAP_SWITCH:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_DIM +
												SCREENMAP_UNITTYPE_SWITCH +
												SCREENMAP_UNITTYPE_CONTROL +
												SCREENMAP_UNITTYPE_VIRTUAL;

						if (0x66 == bSoftwareVersion) {
							supportedUnitTypes[i] += SCREENMAP_UNITTYPE_DUOSWITCH;
						}

					} break;
					case SCREENMAP_CONTROLS:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_CONTROL +
												SCREENMAP_UNITTYPE_VIRTUAL;						
					} break;				
					case SCREENMAP_SENS:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_SENS;
					} break;						
					case SCREENMAP_AUDIO:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_AUDIO +
												SCREENMAP_UNITTYPE_BOSE;							
					} break;							
					case SCREENMAP_DUOSWITCH:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_DUOSWITCH;

						if (0x66 == bSoftwareVersion) {
							supportedUnitTypes[i] += SCREENMAP_UNITTYPE_SWITCH;
						}

					} break;
					case SCREENMAP_V65_MAIN_MOODS:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_CONTROL +
												SCREENMAP_UNITTYPE_VIRTUAL;
					} break;
					case SCREENMAP_V65_MAIN_SENSOR:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_SENS;
					} break;
					case  SCREENMAP_V65_MAIN_AUDIO:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_AUDIO +
												SCREENMAP_UNITTYPE_BOSE;
					} break;					
					case SCREENMAP_V65_MYHOME:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_DIM +
												SCREENMAP_UNITTYPE_SWITCH +
												SCREENMAP_UNITTYPE_CONTROL +
												SCREENMAP_UNITTYPE_VIRTUAL +
												SCREENMAP_UNITTYPE_DUOSWITCH +
												SCREENMAP_UNITTYPE_AUDIO +
												SCREENMAP_UNITTYPE_BOSE +
												SCREENMAP_UNITTYPE_SENS;
					} break;
					case SCREENMAP_V65_MYHOME_SENS:
					{
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_SENS;
					} break;

#if(USE_SMAP_ENERGYMEASURE == 1)
					case SCREENMAP_ENERGYMEASURE: {					
						supportedUnitTypes[i] = SCREENMAP_UNITTYPE_ENERGYMEASURE;					
					} break;
#endif 
					default:
					{
						supportedUnitTypes[i] = 0;						
					} break;
				}
			}
			return true;
		}
	}
	return false;
};

// Changes V15.15: Nodes V66 TCP/IP Homeserver configuratie screenmapping toelaten
// Changes V16.32: Integratie sbox ultra: Screenmapping toelaten voor de DT18-PRO.
bool CNodeScreenmapInfo_Util::isSupported(CCanNode* pNode)
{
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (bSoftwareVersion == 0x64)  {
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD){
			const BYTE bNodeFlags = pNode->bNodeFlags; 
			if ((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_HC4) {
				return true;
			}
			else if ((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_HC5) {
				return true;
			}
		}
	}
	else if (bSoftwareVersion == 0x65) {
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD) {
			return true;	
		}
	}
	else if (bSoftwareVersion == 0x66){
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD) {			
			return true;			
		}
		else if (bNodeType == NODE_PIN_GATEWAY) {
			const BYTE bProductID = pNode->GetProductID();
			if ((PRODUCT_ID_GATEWAY_HS == bProductID)||
				(PRODUCT_ID_GATEWAY_HS_32VIRT == bProductID) ||
				(PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT == bProductID)) {
				return true;
			}
			else if (PRODUCT_ID_HC_BOX_ULTRA == bProductID) {
				return true;
			}
		}
	}
	return false;
}

// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
// Voor de nieuwe tcp/ip module is er configuratie van de screenmapping nodig voor de Duotecno Pro-App.
// Deze wordt niet verstuurd via de CAN-bus maar enkel via de Duotecno Pro App config (Json files upload).
//
// Changes V16.32: Integratie sbox ultra: Screenmapping toelaten voor de DT18-PRO.
bool CNodeScreenmapInfo_Util::isOnlyProAppConfig(CCanNode* pNode) {
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (bSoftwareVersion == 0x66){
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_PIN_GATEWAY) {
			const BYTE bProductID = pNode->GetProductID();
			if ((PRODUCT_ID_GATEWAY_HS == bProductID)||
				(PRODUCT_ID_GATEWAY_HS_32VIRT == bProductID) ||
				(PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT == bProductID)) {
				return true;
			}
			else if (PRODUCT_ID_HC_BOX_ULTRA == bProductID) {
				return true;
			}
		}
	}
	return false;
}

NodeScreenmapConfigtype CNodeScreenmapInfo_Util::
getNodeType(CCanNode* pNode)
{
	ASSERT( pNode );

	NodeScreenmapConfigtype type = NODESCREENMAP_CONFIG_TYPE_NONE;	
	if (isSupported(pNode))
	{
		const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
		if (bSoftwareVersion == 0x64) 
		{
			type = NODESCREENMAP_CONFIG_TYPE_V64; 
		}
		else if (bSoftwareVersion == 0x65) 
		{
			type = NODESCREENMAP_CONFIG_TYPE_V65;
		}
		else if (bSoftwareVersion == 0x66) 
		{
			type = NODESCREENMAP_CONFIG_TYPE_V66;	
		}
	}
	return type;
}

bool CNodeScreenmapLiteInfo_Util::isSupported(CCanNode* pNode)
{
	bool fResult=false;
	
	if(true==isSupported(pNode,SUPPORTED_FLAG_SENSOR))
	{
		fResult=true;
	}

	return fResult;
}

bool CNodeScreenmapLiteInfo_Util::isSupported(CCanNode* pNode,Flags_t flag)
{
	bool fResult=false;		

	switch(flag)
	{
		case SUPPORTED_FLAG_SENSOR:
		{	
			const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
			if (bSoftwareVersion >= 0x90) {		
				const BYTE bNodeType = pNode->bNodeType;
				const int softwareVersionRev=pNode->GetSoftwareVersionRev();
				if (NODE_LCD==bNodeType) {					
					ASSERT(PRODUCT_ID_OLED == pNode->GetProductID( ));					
					if((softwareVersionRev>=0x17)&&(softwareVersionRev!=0xFF)) {
						fResult=true;
					}			
				}
				else if (NODE_PIN_STD_UNITS==bNodeType) {
					// Sedert V15.02: 
					// Room-e vanaf firmware versie ook externe smap van de sensor toelaten.
					const BYTE bProductID = pNode->GetProductID();
					if (PRODUCT_ID_OLED_HOTEL == bProductID) {
						if((softwareVersionRev>=0x24)&&(softwareVersionRev!=0xFF)) {
							fResult=true;
						}
					}
				}
			}
		} break;

		default:
		{
			fResult=false;
		} break;
	}
	return fResult;	
}

// Bij een Smarttouch nodig, Bij een Room-e niet.
bool CNodeScreenmapLiteInfo_Util::isUpdateNodeDbaseNeeded(CCanNode* pNode)
{
	bool result=false;
	const BYTE bProductID = pNode->GetProductID();
	if (PRODUCT_ID_OLED_HOTEL == bProductID) {	/* Room-e */
		result=false;
	}
	else if (PRODUCT_ID_OLED == bProductID) {	/* Smarttouch */
		result=true;
	}
	else {
		ASSERT(0);	// Onbekend product ???
	}
	return result;
}