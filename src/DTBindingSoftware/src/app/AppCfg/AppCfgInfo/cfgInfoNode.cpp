#include "stdafx.h"
#include "cfginfonode.h"
#include "cfginfoproductid_def.h"

#include "cannode.h"

/***********************************************************/
#include "tmglobals.h"
using namespace TMNodeDatabase;
/***********************************************************/

/******************** private static ***********************/

/***********************************************************/
/***************** CNodeExtConfigInfo_Util *****************/
/***********************************************************/
bool CNodeExtConfigInfo_Util::isSupported(CCanNode* pNode)
{
	ASSERT( pNode );

	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (bSoftwareVersion == 0x64)
	{
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD)
		{
			/* Extended color LCD */
			return true;
		}
	}
	else if (bSoftwareVersion == 0x65)
	{
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD)
		{
			/* Basic & Extended color TFT */
			return true;
		}
		else if (bNodeType == NODE_PIN_GATEWAY)
		{
			if (pNode->bNROfUnits > 0)
			{
				/* Gateway TCP/IP & MODBUS */
				return true;
			}
		}
	}
	else if (bSoftwareVersion==0x66)
	{
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD) {
			/* Basic & Extended color TFT */
			return true;
		}
		else if (bNodeType == NODE_PIN_GATEWAY)
		{
			if (pNode->bNROfUnits > 0) {
				/* Gateway TCP/IP & MODBUS */
				return true;
			}
		}
		else if (bNodeType == NODE_PIN_STD_UNITS) {			
			// CHANGES_AV_UNIVERSAL
			const BYTE productID = pNode->GetProductID();
			if (PRODUCT_ID_8CHIRTX_AVIP == productID) {
				// universele Ip AV-matrix heeft extra cfg nodig.
				return true;
			}
		}
	}
	else if (bSoftwareVersion >= 0x90)
	{
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD)
		{
			/* RoomController OLED */
			return true;
		}
		else
		{
			bool fResult=false;
			const BYTE productID = pNode->GetProductID();
			if ((PRODUCT_ID_HC_BOX==productID) ||
			    (PRODUCT_ID_AVMATRIX_BOSEST==productID) ||
				(PRODUCT_ID_AVMATRIX_SONOS==productID))			// CHANGES_SONOS_CFG
			{
				fResult=true;
			}
			// Voor de Oleds & Touchbuttons is de external config toeglaten.
			// Congfiguratie van de LEDS.
			// Voor de mechanische swith & pushbuttons is deze config niet nodig !
			else if ((PRODUCT_ID_OLED_HOTEL==productID)||
					 (PRODUCT_ID_TCHBTN_4CO1SE==productID)||
					 (PRODUCT_ID_TCHBTN_1CO1SE==productID)||
					 (PRODUCT_ID_TCHBTN_2CO1SE==productID)||
					 (PRODUCT_ID_TCHBTN_8CO1SE==productID) ||
					 (PRODUCT_ID_OLED_TOUCHBTN_4V==productID))
			{
				const int softwareVersionRev=pNode->GetSoftwareVersionRev();
				if((softwareVersionRev>=0x11)&&(softwareVersionRev!=0xFF)) {
					fResult=true;
				}	
			}
			return fResult;
		}
	}
	return false;
}

// Changes V15.15: Nodes V66 TCP/IP Homeserver configuratie screenmapping toelaten
bool CNodeExtConfigInfo_Util::
isSupported(CCanNode* pNode,ExternalConfigtype type)
{
	ASSERT( pNode );

	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (bSoftwareVersion == 0x64)
	{
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD)
		{
			switch (type)
			{
				case EXTERNAL_CONFIG_MULTIMEDIA:
				{	
					if (pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)
					{
						return true;
					}
				} break;

				default:
				{					
				} return false;
			}
		}
	} /* if (bSoftwareVersion == 0x64) */
	else if (bSoftwareVersion == 0x65)
	{
		// Software versie v65 gateway, nr of units > 0
		const BYTE bNodeType = pNode->bNodeType;

		if (bNodeType == NODE_LCD || 
			(bNodeType == NODE_PIN_GATEWAY && pNode->bNROfUnits > 0))
		{
			switch (type)
			{
				case EXTERNAL_CONFIG_UNITS_CREATE:
				case EXTERNAL_CONFIG_MOODS:
				{					
				} return true;
				case EXTERNAL_CONFIG_MULTIMEDIA:				
				{			
					if (bNodeType == NODE_LCD &&
						pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)
					{
						// LCD altijd wnr Matrix audio vlag.

						return true;
					}
					// TM,v0638: Gateway ook als slave
					// Moet wel een node/unit database hebben anders kunnen er
					// geen audio rooms doorgestuurd worden.					
					if (bNodeType == NODE_PIN_GATEWAY &&
						pNode->bNROfUnits > 0 &&
						pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO &&
						pNode->bNodeFlags & INCLUDE_UPDATE_DATABASE)
					{									
						return true;
					}									
				} return false;

				case EXTERNAL_CONFIG_SCHEDULE:
				case EXTERNAL_CONFIG_IRRX:
				case EXTERNAL_CONFIG_IRTX:
				{				
					if (pNode->bNodeFlags & NODE_MASTER_LCD)
					{
						return true;
					}					
					return false;				
				} break;		
				
				case EXTERNAL_CONFIG_SMAP_MAIN:
				case EXTERNAL_CONFIG_SMAP_MYHOME:
				case EXTERNAL_CONFIG_SMAP_AVROOM:
				{
					if (bNodeType == NODE_LCD)
					{
						return true;
					}
				} break;

				case EXTERNAL_CONFIG_GATEWAY:
				{
					if (bNodeType == NODE_PIN_GATEWAY)
					{	
						// Sedert V15.04:
						// Indien het productid gekend is en is een Homeserver dan geen gateway config 
						// toelaten.
						//
						// Sedert V16.32: Integratie sbox ultra. 
						const BYTE bProductID=pNode->GetProductID();
						if ((PRODUCT_ID_GATEWAY_HS == bProductID)||
							(PRODUCT_ID_GATEWAY_HS_32VIRT == bProductID) ||
							(PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT == bProductID)) 
						{
							return false;
						}
						else if (PRODUCT_ID_HC_BOX_ULTRA == bProductID) {
							return false;
						}
						
						// Sedert V15.04
						// Indien er sens units aanwezig zijn dan gateway config toelaten.
						// Modbus gateway heeft altijd sens units aanwezig.					
						if (pNode->bNROfUnits > 0) {
							for (int i=0;i<pNode->bNROfUnits;i++) {
								STRUCT_UNIT* pUnit = 0;							
								if (TMGetDocumentNodes()->GetUnit(pNode,i,&pUnit)) {
									if (UNIT_TYPE_SENS == pUnit->bUnitType) {
										return true;
									}
								}
							}
						}
						return false;
					}
				} break;
			}
		}
	} /* else if (bSoftwareVersion == 0x65) */
	else if (bSoftwareVersion==0x66)
	{
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD)
		{
			switch (type)
			{
				case EXTERNAL_CONFIG_UNITS_CREATE:
				case EXTERNAL_CONFIG_MOODS:
				{					
				} return true;
				case EXTERNAL_CONFIG_MULTIMEDIA:				
				{			
					if (pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)											
						return true;
																
				} return false;
				case EXTERNAL_CONFIG_SCHEDULE:
				case EXTERNAL_CONFIG_IRRX:
				case EXTERNAL_CONFIG_IRTX:
				{				
					if (pNode->bNodeFlags & NODE_MASTER_LCD)					
						return true;						

				} return false;						
				case EXTERNAL_CONFIG_SMAP_MAIN:
				case EXTERNAL_CONFIG_SMAP_MYHOME:
				case EXTERNAL_CONFIG_SMAP_AVROOM:
				{														
				} return true;		
				case EXTERNAL_CONFIG_AVSETTINGS:
				{
				} return true; 
				case EXTERNAL_CONFIG_TRIGGERS:
				{
					if (pNode->GetSoftwareVersionRev()>=0x03) {
						return true;
					}
				} break;
				case EXTERNAL_CONFIG_MOBILE_APP:
				{
					if (pNode->GetSoftwareVersionRev()>=0x05) {
						return true;
					}
				} break;
			}
		}
		else if (bNodeType == NODE_PIN_STD_UNITS) {
			// CHANGES_AV_UNIVERSAL
			const BYTE productID = pNode->GetProductID();
			if (PRODUCT_ID_8CHIRTX_AVIP == productID) {	
				// Universele AV-Matrix config.
				if (EXTERNAL_CONFIG_UNITS_CREATE==type) {
					return true;
				}
				else if (EXTERNAL_CONFIG_AVMATRIX==type) {
					return true;
				}
				else if (EXTERNAL_CONFIG_TRIGGERS==type) {
					return true;
				}
			}
		}
		else if (bNodeType == NODE_PIN_GATEWAY) {
			// Sedert V16.32: Integratie sbox ultra
			const BYTE bProductID=pNode->GetProductID();
			if ((PRODUCT_ID_TFT_32VIRT == bProductID)||
				(PRODUCT_ID_GATEWAY_HS == bProductID)||
				(PRODUCT_ID_GATEWAY_HS_32VIRT == bProductID) ||
				(PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT == bProductID)||
				(PRODUCT_ID_HC_BOX_ULTRA == bProductID)) 
			{
				switch (type)
				{
					case EXTERNAL_CONFIG_UNITS_CREATE:
					case EXTERNAL_CONFIG_MOODS:
					case EXTERNAL_CONFIG_TRIGGERS:
					case EXTERNAL_CONFIG_MOBILE_APP:
					{					
					} return true;
					case EXTERNAL_CONFIG_MULTIMEDIA:				
					{			
						if (pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)											
							return true;

					} return false;
					case EXTERNAL_CONFIG_SCHEDULE:
					case EXTERNAL_CONFIG_IRRX:
					case EXTERNAL_CONFIG_IRTX:
					{				
						if (pNode->bNodeFlags & NODE_MASTER_LCD)					
							return true;													
					} return false;			
					case EXTERNAL_CONFIG_SMAP_MAIN:
					case EXTERNAL_CONFIG_SMAP_MYHOME:
					case EXTERNAL_CONFIG_SMAP_AVROOM:
					{														
					} return true;	

					// Sedert V16.29
					// Extra controle via de product vlaggen.
					case EXTERNAL_CONFIG_AVSETTINGS: {						
						const BYTE bSoftwareVersionMinor = pNode->GetSoftwareVersionRev();
						if (bSoftwareVersionMinor >= 0x11) {
							const int productFlagsInfo = pNode->GetProductExtraFlags();
							if (productFlagsInfo & PRODUCT_DT18PRO_FLAG_CAMERASERVER) {	
								return true;
							}
						}
					} return false;

					// Sedert V16.33:
					// DALI Configuratie enkel voor de sbox ultra.
					case EXTERNAL_CONFIG_DALI: {
						if (PRODUCT_ID_HC_BOX_ULTRA == bProductID) {
							return true;
						}   
					} return false;
				}
			}
		}
	}
	else if (bSoftwareVersion >= 0x90)
	{
		const BYTE bNodeType = pNode->bNodeType;
		if (bNodeType == NODE_LCD)
		{
			bool result = false;

			switch (type)
			{
				case EXTERNAL_CONFIG_ROOMCTRL_AUDIO:
				{
					result = true;
				} break;

				case EXTERNAL_CONFIG_LEDSCFG:
				{
					// PRODUCT_ID_OLED, node type is altijd een LCD node.
					const BYTE bProductID=pNode->GetProductID();

					if (PRODUCT_ID_OLED==bProductID)
					{
						const int softwareVersionRev=pNode->GetSoftwareVersionRev();
						if((softwareVersionRev>=0x11)&&(softwareVersionRev!=0xFF))
						{
							result = true;
						}
					}
				} break;

				default:
				{
					result = false;
				} break;
			}
			return result;
		} /* if (bNodeType == NODE_LCD) */
		else
		{
			bool result = false;
			switch (type)
			{
				case EXTERNAL_CONFIG_SCHEDULE:
				case EXTERNAL_CONFIG_MOODS:
				{
					if (pNode->GetProductID() == PRODUCT_ID_HC_BOX)
					{
						result = true;
					}
				} break;
				case EXTERNAL_CONFIG_BOSEST:
				{
					const BYTE bProductID=pNode->GetProductID();
					if ((bProductID == PRODUCT_ID_AVMATRIX_BOSEST)||
						(bProductID == PRODUCT_ID_AVMATRIX_SONOS))		// CHANGES_SONOS_CFG
					{
						result = true;
					}
				} break;
				case EXTERNAL_CONFIG_SONOSSRC:
				{
					const BYTE bProductID=pNode->GetProductID();
					if (bProductID == PRODUCT_ID_AVMATRIX_SONOS)		// CHANGES_SONOS_CFG
					{
						result = true;
					}
				} break;
				case EXTERNAL_CONFIG_LEDSCFG:
				{
					// Voor de mechanische swith & pushbuttons is deze config niet nodig !
					const BYTE bProductID=pNode->GetProductID();
					if((PRODUCT_ID_OLED_HOTEL==bProductID) ||
					   (PRODUCT_ID_TCHBTN_4CO1SE==bProductID)||
					   (PRODUCT_ID_TCHBTN_1CO1SE==bProductID)||
					   (PRODUCT_ID_TCHBTN_2CO1SE==bProductID)||
					   (PRODUCT_ID_TCHBTN_8CO1SE==bProductID) ||
					   (PRODUCT_ID_OLED_TOUCHBTN_4V==bProductID))
					{
						const int softwareVersionRev=pNode->GetSoftwareVersionRev();
						if((softwareVersionRev>=0x11)&&(softwareVersionRev!=0xFF)) {
							result = true;
						}
					}
				} break;
			}
			return result;		
		}
	} /* else if (bSoftwareVersion == 0x90) */

	return false;		/* Oppassen !! */
};

unsigned long CNodeExtConfigInfo_Util::
getSupportedFlags(CCanNode* pNode)
{
	unsigned long ulSupportedFlags = 0;
	if (isSupported(pNode, EXTERNAL_CONFIG_MULTIMEDIA)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_MULTIMEDIA; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_MOODS)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_MOODS; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_UNITS_CREATE)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_UNITS_CREATE; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_IRRX)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_IRRX; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_IRTX)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_IRTX; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_SCHEDULE)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_SCHEDULE; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_SMAP_MAIN)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_SMAP_MAIN; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_SMAP_MYHOME)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_SMAP_MYHOME; 
	}		
	if (isSupported(pNode, EXTERNAL_CONFIG_SMAP_AVROOM)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_SMAP_AVROOM; 
	}	
	if (isSupported(pNode, EXTERNAL_CONFIG_GATEWAY)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_GATEWAY; 
	}	
	if (isSupported(pNode, EXTERNAL_CONFIG_ROOMCTRL_AUDIO)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_ROOMCTRL_AUDIO; 
	}	
	if (isSupported(pNode, EXTERNAL_CONFIG_BOSEST)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_BOSEST; 
	}	
	if (isSupported(pNode, EXTERNAL_CONFIG_LEDSCFG)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_LEDSCFG; 
	}	
	if (isSupported(pNode, EXTERNAL_CONFIG_SONOSSRC)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_SONOSSRC; 
	}	
	if (isSupported(pNode, EXTERNAL_CONFIG_AVSETTINGS)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_AVSETTINGS; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_AVMATRIX)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_AVMATRIX; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_TRIGGERS)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_TRIGGERS; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_MOBILE_APP)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_MOBILE_APP; 
	}
	if (isSupported(pNode, EXTERNAL_CONFIG_DALI)) {
		ulSupportedFlags |= EXTERNAL_CONFIG_DALI; 
	}
	return ulSupportedFlags;
}

/***********************************************************/
/***************** CNodeBitmapConfig_Util ******************/
/***********************************************************/
bool CNodeBitmapConfig_Util::isSupported(CCanNode* pNode)
{
	ASSERT( pNode );

	const BYTE bNodeType = pNode->bNodeType;

	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{
		case 0x62:	
		{
			return (NODE_LCD == bNodeType ? true : false );
		};
		case 0x63:	
		{
			return (NODE_LCD == bNodeType ? true : false );
		};
		case 0x64:	
		{
			return (NODE_LCD == bNodeType ? true : false );
		};
		case 0x65:	
		{
			if (NODE_LCD == bNodeType)
			{
				return true;
			}
			else if (NODE_PIN_GATEWAY == bNodeType)
			{
				/** @todo */
			}
		} break;
		case 0x66:
		{
			return (NODE_LCD == bNodeType ? true : false );
			// absorbe: Geen bitmap cfg.
		} break;
		default:
		{
			// Absorbe ...
		} break;
	}	
	return false;
}

NodeBitmapConfigtype CNodeBitmapConfig_Util::
getNodetype(CCanNode* pNode)
{
	NodeBitmapConfigtype type = NODEBITMAP_CONFIG_TYPE_NONE;
	const BYTE bNodeType = pNode->bNodeType;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{
		case 0x62:	
		{	
			if (NODE_LCD == bNodeType)
			{
				type = NODEBITMAP_CONFIG_TYPE_V62_BASIC;
			}
		} break;
		case 0x63:	
		{
			if (NODE_LCD == bNodeType)
			{
				type = NODEBITMAP_CONFIG_TYPE_V63_BASIC;
			}	
		} break;
		case 0x64:	
		{
			if (NODE_LCD == bNodeType)
			{
				const BYTE bNodeFlags = pNode->bNodeFlags;
				if ((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_HC4) 
				{
					type = NODEBITMAP_CONFIG_TYPE_V64_HC4;	
				}
				else if ((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_BASIC)
				{
					type = NODEBITMAP_CONFIG_TYPE_V64_BASIC;	
				}
				else
				{
					type = NODEBITMAP_CONFIG_TYPE_V64_HC5;
				}
			}
		} break;
		case 0x65:	
		{
			if (NODE_LCD == bNodeType)
			{			
				type = NODEBITMAP_CONFIG_TYPE_V65;
			}
			else if ((NODE_PIN_GATEWAY == bNodeType) && 
					 (pNode->bNROfUnits > 0))
			{			
				type = NODEBITMAP_CONFIG_TYPE_V65;
			}
		} break;	
		case 0x66:
		{
			// V66 touchscreen - bitmap cfg enkel via ftp.	
			type = NODEBITMAP_CONFIG_TYPE_V66;	// NODEBITMAP_CONFIG_TYPE_NONE;
		} break;
		default:
		{
			// Absorbe ...
		} break;
	}
	return type;
};

bool CNodeBitmapConfig_Util::getRemoteDirectory(
	CCanNode* pNode,
	char* szFileName,
	int maxLength)
{
	bool result = false;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{
		case 0x65:
		{			
			const char* _szFileNameRemote = "b:\\Icon\\";				
			const int len = strlen( _szFileNameRemote );
			if (len > 0 && len < maxLength)
			{
				strcpy(szFileName, _szFileNameRemote);
				result = true;
			}	
		} break;
	}
	return result;
}

/******************************************************/

/***********************************************************/
/************** CNodeMultimediaConfig_Util *****************/
/***********************************************************/
bool CNodeMultimediaConfig_Util::isSupported(CCanNode* pNode)
{
	ASSERT( pNode );

	const BYTE bNodeType = pNode->bNodeType;

	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{
		case 0x64:
		{
			if (NODE_LCD == bNodeType)
			{
				const BYTE bNodeFlags = pNode->bNodeFlags;
				if (pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)
				{
					return true;
				}
			}
		} break;
		case 0x65:
		case 0x66:
		{
			if (NODE_LCD == bNodeType)
			{
				const BYTE bNodeFlags = pNode->bNodeFlags;
				if (pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)
				{
					return true;
				}
			}
			else if (NODE_PIN_GATEWAY == bNodeType)
			{			
				const BYTE bNodeFlags = pNode->bNodeFlags;

				// Indien master en Extended audio.
				if (bNodeFlags & NODE_MASTER_LCD &&
					bNodeFlags & NODE_LCD_MATRIX_AUDIO)
				{
					return true;
				}	

				// TM,v0638: Gateway ook als slave
				// Moet wel een node/unit database hebben anders kunnen er
				// geen audio rooms doorgestuurd worden.
				if (pNode->bNROfUnits > 0 &&
					bNodeFlags & NODE_LCD_MATRIX_AUDIO &&
					bNodeFlags & INCLUDE_UPDATE_DATABASE)
				{
					return true;
				}
			}
		} break;
#if(0)
		case 0x66:
		{
			if (NODE_LCD == bNodeType)
			{
				const BYTE bNodeFlags = pNode->bNodeFlags;
				if (pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)
				{
					return true;
				}
			}
		} break;
#endif 
	}
	return false;
}

NodeMultimediaConfigType CNodeMultimediaConfig_Util::
getNodeType(CCanNode* pNode)
{
	NodeMultimediaConfigType type = NODEMULTIMEDIA_CONFIG_TYPE_NONE;

	if (isSupported(pNode))
	{
		const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
		switch (bSoftwareVersion)
		{
			case 0x64:		
			{
				type = NODEMULTIMEDIA_CONFIG_TYPE_V64;				
			} break;
			case 0x65:
			{
				type = NODEMULTIMEDIA_CONFIG_TYPE_V65;					
			} break;
			case 0x66:
			{
				type = NODEMULTIMEDIA_CONFIG_TYPE_V66;						
			} break;	
		}
	}
	return type;
}
/******************************************************/



/***********************************************************/
/****************** CNodeBackupConfig_Util *****************/
/***********************************************************/
bool CNodeBackupConfig_Util::isSupported(CCanNode* pNode)
{
	ASSERT( pNode );

	const BYTE bNodeType = pNode->bNodeType;

	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{
		case 0x62:	
		{
			return (NODE_LCD == bNodeType ? true : false );
		};
		case 0x63:	
		{
			return (NODE_LCD == bNodeType ? true : false );
		};
		case 0x64:	
		{
			return (NODE_LCD == bNodeType ? true : false );
		};
		case 0x65:	
		{
			// Niet ondersteund voor de V65 nodes
			if (NODE_LCD == bNodeType)
			{
				/** @todo */
			}
			else if (NODE_PIN_GATEWAY == bNodeType)
			{
				/** @todo */
			}
		} break;	

		case 0x66: return (NODE_LCD == bNodeType ? true : false );

		default:
		{
			// Absorbe ...
		} break;
	}	
	return false;
}
NodeBackupConfigType CNodeBackupConfig_Util::
getNodeType(CCanNode* pNode)
{
	ASSERT( pNode );

	NodeBackupConfigType type = NODEBACKUP_CONFIG_TYPE_NONE;

	if (isSupported(pNode))
	{
		const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
		switch (bSoftwareVersion)
		{				
			case 0x62:
			{
				// isSupported(): is zeker een lcd node.
				type = NODEBACKUP_CONFIG_TYPE_V62_BASIC; 
			} break;
			case 0x63:
			{
				// isSupported(): is zeker een lcd node.
				type = NODEBACKUP_CONFIG_TYPE_V63_BASIC; 
			} break;
			case 0x64:
			{
				// isSupported(): is zeker een lcd node.
				const BYTE bNodeFlags = pNode->bNodeFlags;
				if ((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_HC4) 
				{
					type = NODEBACKUP_CONFIG_TYPE_V64_BASIC;	
				}
				else if ((bNodeFlags&NODE_LCD_TYPE_MASK) == NODE_LCD_TYPE_BASIC)
				{
					type = NODEBACKUP_CONFIG_TYPE_V64_BASIC;	
				}
				else
				{
					type = NODEBACKUP_CONFIG_TYPE_V64_HC5;
				}	
			} break;
			case 0x65:
			{				
				type = NODEBACKUP_CONFIG_TYPE_V65;		// Niet ondersteund voor de V65 nodes
			} break;
			case 0x66:
			{		
				type = NODEBACKUP_CONFIG_TYPE_V66;	
			} break;
		}
	}
	return type;
}
/**********************************************************/

/***********************************************************/
/****************** CNodeCreateDynUnits_Util ***************/
/***********************************************************/
unsigned long CNodeCreateDynUnits_Util::
getSupportedTypes(CCanNode* pNode)
{
	ASSERT( pNode );

	unsigned long ulSupportedUnits = 0;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{	
		case 0x65:
		{
			const BYTE bNodeType = pNode->bNodeType;
			switch (bNodeType)
			{
				case NODE_LCD:
				{
					ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_LCD_VIRTUAL;	

					// Sedert v0567 
					// Indien audio hc4 mogen er audio units aangemaakt worden.
					// BUG-0103: enkel voor de master LCD.

					const BYTE bNodeFlags = pNode->bNodeFlags;
					if (bNodeFlags & NODE_MASTER_LCD)
					{					
						if (pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)
						{
							ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_AUDIO;
						}
					}

				} break;
				case NODE_PIN_GATEWAY:
				{					
					// Oppassen:
					// Product id wordt pas ondersteund in de V65 modules vanaf versie
					// V65.54.04.
					const BYTE productID = pNode->GetProductID();

					if (pNode->bNROfUnits > 0)
					{
						// Sedert v0567:
						// Indien de gateway reeds units heeft mogen er units 
						// aangemaakt worden

						ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_LCD_VIRTUAL;	


						// BUG-0149: Solved in versie 0x0C03
						// Deze aanpassing zal enkel invloed hebben op de V65 modules
						// met formware versie >V65.54.04.

						if (PRODUCT_ID_GATEWAY_MODBUS_16SE32VIRT != productID)
						{
							// Sedert v0637
							// Homeserver kan een AV Matrix unit hebben.
							// Hiermee kan een 3rd party AV Matrix aangestuurd worden via
							// open protocol.
							ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_AVMATRIX;
							ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_IRTX;	

							// Sedert v15.03: Virtuele units aanmaken.
							ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_DIM;
							ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_SWITCH;
						}

						// Sedert v0620: TM20121025
						// Indien er reeds units van een bepaald type aanwezig zijn dan 
						// mogen er bijgemaakt worden...

						for (int i=0;i<pNode->bNROfUnits;i++)
						{
							STRUCT_UNIT* pUnit = 0;							
							if (TMGetDocumentNodes()->GetUnit(pNode,i,&pUnit))
							{
								const BYTE bUnitType = pUnit->bUnitType;
								switch(bUnitType)
								{
									case UNIT_TYPE_SENS:
									{
										ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_SENS;
									} break;
									case UNIT_TYPE_DIM:
									{
										ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_DIM;	
									} break;
									case UNIT_TYPE_SWITCH:
									{
										ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_SWITCH;	
									} break;
									case UNIT_TYPE_IRTX:
									{
										ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_IRTX;		
									} break;
									case UNIT_TYPE_DUOSWITCH:
									{
										ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_DUOSWITCH;	
									} break;
									case UNIT_TYPE_AVMATRIX:
									{
										ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_AVMATRIX;	
									} break;
									case UNIT_TYPE_VIDEOMUX:
									{
										ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_VIDEOMUX;										
									} break;
								}
							}
						}
					}	

					// TM, v0577: Indien de gateway als master en audio type Matrix audio
					// mogen er audio kamers aangemaakt worden.
					const BYTE bNodeFlags = pNode->bNodeFlags;
					if (bNodeFlags & NODE_MASTER_LCD)
					{					
						if (pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)
						{
							ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_AUDIO;
						}
					}

				} break;
			}
		} break;

		case 0x66:
		{
			const BYTE bNodeType = pNode->bNodeType;
			if (NODE_LCD==bNodeType || NODE_PIN_GATEWAY == bNodeType)
			{
				ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_LCD_VIRTUAL;	
				
				const BYTE bNodeFlags = pNode->bNodeFlags;
				if ((bNodeFlags & NODE_MASTER_LCD)&&
					(bNodeFlags & NODE_LCD_MATRIX_AUDIO))
				{
					ulSupportedUnits |=CONFIGFLAG_CREATE_UNIT_TYPE_AUDIO;
				}	
				
				const BYTE bSoftwareVersionMinor = pNode->GetSoftwareVersionRev();
				if (bSoftwareVersionMinor>=0x04) {
					ulSupportedUnits |=CONFIGFLAG_CREATE_UNIT_TYPE_DIM;
					ulSupportedUnits |=CONFIGFLAG_CREATE_UNIT_TYPE_SWITCH;
					ulSupportedUnits |=CONFIGFLAG_CREATE_UNIT_TYPE_DUOSWITCH;
					ulSupportedUnits |=CONFIGFLAG_CREATE_UNIT_TYPE_SENS;
					ulSupportedUnits |=CONFIGFLAG_CREATE_UNIT_TYPE_IRTX;
					ulSupportedUnits |=CONFIGFLAG_CREATE_UNIT_TYPE_AVMATRIX;
				}
			}
			else if (NODE_PIN_STD_UNITS == bNodeType) {
				// CHANGES_AV_UNIVERSAL
				const BYTE productCode = pNode->GetProductID();
				if (PRODUCT_ID_8CHIRTX_AVIP == productCode) {
					// Universele AV-Matrix: Momenteel enkle virtuals & AV-Matrix toegelaten.
					ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_LCD_VIRTUAL;
					ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_AVMATRIX;
				}			
			}

			// Sedert V16.34: Integratie smartbox ultra.
			// Voor de DALI control devices moeten we Control units kunnen bijmaken.
			if (PRODUCT_ID_HC_BOX_ULTRA == pNode->GetProductID()) {
				ulSupportedUnits |= CONFIGFLAG_CREATE_UNIT_TYPE_CONTROL;
			}

		} break;
	}
	return ulSupportedUnits;
}

// CHANGES_AV_UNIVERSAL
ExternalConfigUnitsCreateType CNodeCreateDynUnits_Util::getNodeType(CCanNode* pNode) {
	ExternalConfigUnitsCreateType retVal = UNITSCREATE_TYPE_DEFAULT;
	if (pNode) {
		const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
		if (0x66 == bSoftwareVersion) {	
			const BYTE productCode = pNode->GetProductID();
			if (PRODUCT_ID_8CHIRTX_AVIP == productCode) { 
				retVal = UNITSCREATE_TYPE_UNITCONFIG_IP;
			}
		}
	}
	return retVal;
}

/*******************************************************/

/********************************************************/
/****************** CNodeIRTXConfig_Util ****************/
/********************************************************/
bool CNodeIRTXConfig_Util::isSupported(
	CCanNode* pNode)
{
	bool result = false;
	const BYTE bNodeType = pNode->bNodeType;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{	
		case 0x65:	
		{
			// TM,v0577: Ook wnr Node Type Gateway.
			if (NODE_LCD == bNodeType ||
				NODE_PIN_GATEWAY == bNodeType)
			{
				result = true;
			}
		} break;	
		case 0x66:
		{
			if (NODE_LCD==bNodeType)
				result=true;
		} break;
		default:
		{
			// Absorbe ...
		} break;
	}	
	return result;
}

bool CNodeIRTXConfig_Util::getFileNameLocal(CCanNode* pNode,char* szFileName,int maxLength)
{
	bool result = false;
	const BYTE bNodeType = pNode->bNodeType;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{	
		case 0x65:	
		{
			// TM,v0577: Ook wnr Node Type Gateway.
			if (NODE_LCD == bNodeType || NODE_PIN_GATEWAY == bNodeType)
			{
				const char* _szFileNameLocal = "config\\irtx.dat";
				const int len = strlen( _szFileNameLocal );
				if (len > 0 && len < maxLength)
				{
					strcpy(szFileName, _szFileNameLocal);
					result = true;
				}
			}			
		} break;	
		case 0x66:
		{
			if (NODE_LCD == bNodeType || NODE_PIN_GATEWAY == bNodeType)
			{
				const char* _szFileNameLocal = "config\\irtx.dat";
				const int len = strlen( _szFileNameLocal );
				if (len > 0 && len < maxLength)
				{
					strcpy(szFileName, _szFileNameLocal);
					result = true;
				}
			} 
		}break;
		default:
		{
			// Absorbe ...
		} break;
	}	
	return result;
}

bool CNodeIRTXConfig_Util::getFileNameRemote(
	CCanNode* pNode,
	char* szFileName,
	int maxLength)
{
	bool result = false;
	const BYTE bNodeType = pNode->bNodeType;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{	
		case 0x65:	
		{
			// TM,v0577: Ook wnr Node Type Gateway.
			if (NODE_LCD == bNodeType || NODE_PIN_GATEWAY == bNodeType)
			{
				const char* _szFileNameRemote = "b:\\data\\irtx.dat";				
				const int len = strlen( _szFileNameRemote );
				if (len > 0 && len < maxLength)
				{
					strcpy(szFileName, _szFileNameRemote);
					result = true;
				}
			}
		} break;	
		case 0x66:
		{
			if (NODE_LCD == bNodeType || NODE_PIN_GATEWAY == bNodeType)
			{
				const char* _szFileNameRemote = "%%CONFIG%%irtx.dat";				
				const int len = strlen( _szFileNameRemote );
				if (len > 0 && len < maxLength)
				{
					strcpy(szFileName, _szFileNameRemote);
					result = true;
				}
			}
		} break;
		default:
		{
			// Absorbe ...
		} break;
	}	
	return result;
}

/******************************************************/

/********************************************************/
/**************** CNodeAVSettingsConfig_Util ************/
/********************************************************/

// Sedert binding software V16.29.
// Bepalen of de DT18-PRO een camera server ondersteund.
// Om dit te bepalen controle van de firmware versie (>=V66.11.03) en
// controle van de extra product vlaggen (enkel CM4S hardware).
bool CNodeAVSettingsConfig_Util::isSupported(CCanNode* pNode)
{
	bool result = false;	
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion)
	{		
		case 0x66: {
			const BYTE bNodeType = pNode->bNodeType;
			if (NODE_LCD==bNodeType)
				result=true;

			if (NODE_PIN_GATEWAY == bNodeType) {
				const BYTE bSoftwareVersionMinor = pNode->GetSoftwareVersionRev();
				if (bSoftwareVersionMinor >= 0x11) {
					const int productFlagsInfo = pNode->GetProductExtraFlags();
					if (productFlagsInfo & PRODUCT_DT18PRO_FLAG_CAMERASERVER) {				
						result = true;
					}
				}			
			}

		} break;
		default: { /* Absorbe ... */ } break;
	}	
	return result;
}

bool CNodeAVSettingsConfig_Util::getFileNameLocal(CCanNode* pNode,char* szFileName,int maxLength)
{
	bool result = false;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion) {
		case 0x66: {
			CString strTemp;
			strTemp.Format("config\\avsettings_%02x.txt",pNode->getNodeAddress());	
			const int len = strTemp.GetLength();
			if (len>0 && len<maxLength) {
				strcpy(szFileName,(LPCTSTR)strTemp);
				result = true;
			}			
		}break;

		default: { /* absorbe */ } break;
	}
	return result;
}

bool CNodeAVSettingsConfig_Util::getFileNameRemote(CCanNode* pNode,char* szFileName,int maxLength)
{
	bool result = false;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	switch (bSoftwareVersion) {
		case 0x66: {		
			const char* _szFileNameRemote = "%%CONFIG%%.avsettings.txt";				
			const int len = strlen( _szFileNameRemote );
			if (len > 0 && len < maxLength) {
				strcpy(szFileName, _szFileNameRemote);
				result = true;
			}			
		} break;
		default: { /* absorbe */ } break;
	}
	return result;
}

int CNodeAVSettingsConfig_Util::getSupportedFlags(CCanNode* pNode) {	
	if (!isSupported(pNode)) {	/* EXTRA CONTROLE OF HET ONDERSTEUND WORDT !! */
		return 0;
	}
	int flags = 0;
	const BYTE bNodeType = pNode->bNodeType;
	switch(bNodeType) {
		case NODE_LCD: {
			flags |= AVSETTINGS_CFG_FLAGS_CAMERA;	
			flags |= AVSETTINGS_CFG_FLAGS_INDOORVIDEOSIP;		
		} break;
		case NODE_PIN_GATEWAY: {
			flags |= AVSETTINGS_CFG_FLAGS_CAMERA;								   
		} break;
	}
	return flags;
}

/*******************************************************/

#ifdef SKIP
bool CNodeSonosSrcItemsConfig_Util::
	isSupported(
		CCanNode* pNode)
{

}
#endif


bool CNodeSonosSrcItemsConfig_Util::
getFileNameLocal(
	CCanNode* pNode,
	char* szFileName,
	int maxLength)
{	
	bool result = false;
	char szTemp[64];

	memset(szTemp,0,sizeof(szTemp));

	sprintf(szTemp,
			"config\\srccfg_%02x.dat",
			pNode->getNodeAddress());

	if ((int)strlen(szTemp)<maxLength)
	{
		strcpy(szFileName,szTemp);
		result=true;
	}
	return result;
}

// Oppassen:
// Er wordt eerst een *.tmp bestand aangemaakt en verstuurd over CAN.
// Het embedded device zal het *.tmp hernoemen naar een *.dat bestand.
bool CNodeSonosSrcItemsConfig_Util::
getFileNameRemote(
	CCanNode* pNode,
	char* szFileName,
	int maxLength)
{
	bool result = false;
	const char* const s="a:srccfg.tmp";

	if ((int)strlen(s)<maxLength)
	{
		strcpy(szFileName,s);
		result=true;
	}
	return result;
}

/*******************************************************/



/********************************************************/
/****************** CNodeSystemLog_Util *****************/
/********************************************************/

bool CNodeSystemLog_Util::isSupported(CCanNode* pNode,FileID fileid)
{
	if (pNode)
	{
		const BYTE bNodeType = pNode->bNodeType;
		const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
		switch (bSoftwareVersion)
		{	
			case 0x65:
			{
				if (bNodeType == NODE_LCD || bNodeType == NODE_PIN_GATEWAY)
				{
					return true;
				}
			} break;
			case 0x66:
			{
				// Sedert V14.11: Alle producten van software V66, vroeger enkel de LCD node.				
				return true;
			} break;
			case 0x90:
			{
				// Alle producten met firmware versie >= V90 ondersteunen logfile.
				return true;	
			} break;
		} 
	}
	return false;
}

bool CNodeSystemLog_Util::getFileNameLocal(
	CCanNode* pNode,
	FileID fileid,
	char* szFileName,
	int maxLength)
{
	if (isSupported(pNode,fileid))
	{
		bool fContinue = false;
		char szTempBuffer[255];

		switch (fileid)
		{
			case FILE_ID_SYSTEMLOG:
			{
				sprintf(szTempBuffer,
						"data\\Systemlog_%02x.log",
						pNode->getNodeAddress());
				fContinue = true;
			} break;
			case FILE_ID_SYSTEMLOG_BACKUP:
			{
				sprintf(szTempBuffer,
						"data\\Systemlog_bak_%02x.log",
						pNode->getNodeAddress());
				fContinue = true;
			} break;
		}	
		if (fContinue)
		{
			const int len = strlen(szTempBuffer) + 1;
			if (maxLength >= len)
			{
				strcpy(szFileName,szTempBuffer);
				return true;
			}
		}
	}
	return false;
}

bool CNodeSystemLog_Util::getFileNameRemote(
	CCanNode* pNode,
	FileID fileid,
	char* szFileName,
	int maxLength)
{
	if (isSupported(pNode,fileid))
	{
		bool fContinue = false;
		char szTempBuffer[255];
		char chDrive = 'a';

		const BYTE bNodeType = pNode->bNodeType;
		const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
		const BYTE bProductID=pNode->GetProductID();

		switch (fileid)
		{
			case FILE_ID_SYSTEMLOG:
			{
				if (0x66==bSoftwareVersion)
				{									
					strcpy(szTempBuffer,"%%SYSLOG%%");
					fContinue = true;					
				} 
				else if (bSoftwareVersion == 0x65)
				{
					switch (bNodeType)
					{
						case NODE_LCD:
						{
							chDrive = 'b';							
						} break;
						case NODE_PIN_GATEWAY:
						{
							// Omdat we niet weten welke hardware gaan we ervan uit dat een
							// master gateway op sc143 hardware is en een slave altijd op de
							// sc23 hardware.

							const BYTE bNodeFlags = pNode->bNodeFlags;
							if (bNodeFlags & NODE_MASTER_LCD)
							{		
								chDrive = 'b';
							}
							else
							{
								chDrive = 'a';
							}
						} break;
					}
			
					// Testen: Oplossing voor BUG-0132 ??
					sprintf(szTempBuffer,
							"%c:Systemlog.log",
							chDrive);							

					fContinue = true;
				} /* if (bSoftwareVersion == 0x65) */
				else if (bSoftwareVersion == 0x90)
				{				
					// Alle producten met firmware versie >= V90 ondersteunen logfile.
					
					// Oppassen: Geen drive nummer meegeven in de naam !!
					strcpy(szTempBuffer,"syslog.txt");							
					fContinue = true;

				} /* else if (bSoftwareVersion == 0x90) */
			} break; /* case FILE_ID_SYSTEMLOG: */

			case FILE_ID_SYSTEMLOG_BACKUP:
			{
				if (0x66==bSoftwareVersion)
				{									
					strcpy(szTempBuffer,"%%SYSLOG_BAK%%");
					fContinue = true;					
				} 
				else if (bSoftwareVersion == 0x65)
				{
					switch (bNodeType)
					{
						case NODE_LCD:
						{
							chDrive = 'b';	
						} break;
						case NODE_PIN_GATEWAY:
						{
							// Omdat we niet weten welke hardware gaan we ervan uit dat een
							// master gateway op sc143 hardware is en een slave altijd op de
							// sc23 hardware.

							const BYTE bNodeFlags = pNode->bNodeFlags;
							if (bNodeFlags & NODE_MASTER_LCD)
							{		
								chDrive = 'b';
							}
							else
							{
								chDrive = 'a';
							}
						} break;
					}

					// Testen: Oplossing voor BUG-0132 ??
					sprintf(szTempBuffer,
							"%c:bak\\Systemlog.log",
							chDrive);

					fContinue = true;
				} /* if (bSoftwareVersion == 0x65) */
				else if (bSoftwareVersion == 0x90)
				{
					// Alle producten met firmware versie >= V90 ondersteunen logfile.

					// Oppassen: Geen drive nummer meegeven in de naam !!
					strcpy(szTempBuffer,"syslog.bak");							
					fContinue = true;
				} /* else if (bSoftwareVersion == 0x90) */
			} break; /* case FILE_ID_SYSTEMLOG_BACKUP: */
		} /* end switch( ) */
		if (fContinue)
		{
			const int len = strlen(szTempBuffer) + 1;
			if (maxLength >= len)
			{
				strcpy(szFileName,szTempBuffer);
				return true;
			}
		}

	}
	return false;
}
/******************************************************/
bool CCanNodeInfo_Util::formatSoftwareVersion(
	const CCanNode& refNode,
	CString& szDest)
{
	const BYTE bSoftwareVersion = refNode.GetSoftwareVersion();
	const BYTE bSoftwareVersionREV=refNode.GetSoftwareVersionRev();

	if ((PRODUCT_ID_UNKNOWN!=refNode.GetProductID())&&(0xFF!=bSoftwareVersionREV))
	{
		szDest.Format(_T("V%02X.%02X"),bSoftwareVersion,bSoftwareVersionREV);
	}
	else
	{
		szDest.Format(_T("V%02X"),bSoftwareVersion);		
	}
	return true;
}

/******************************************************/
bool CNodeConfigSupportHttpInfo_Util::isSupported(CCanNode* pNode)
{
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_MOODS)) {
		return true;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_UNITS)) {
		return true;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_IRTX)) {
		return true;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_SCHEDULE)) {
		return true;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_AVMATRIX)) {
		return true;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_TRIGGERACTIONS)) {
		return true;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_DALI)) {
		return true;
	}
	return false;
}

bool CNodeConfigSupportHttpInfo_Util::isSupported(CCanNode* pNode,ConfigSupportHTTP type) {
	if (NULL==pNode) {
		return false;
	}

	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (0x66 == bSoftwareVersion) {
		const BYTE bNodeType = pNode->bNodeType;
		switch(bNodeType) 
		{
			case NODE_LCD:
			{
				// Moods & units vanaf V66.04
				// IRTX vanaf V66.05 (indien master)
				// Scheduling vanaf V66.04 (indien master)
				// Triggers & actions: vanaf V66.02
				const BYTE bSoftwareVersionRev = pNode->GetSoftwareVersionRev();
				if (CONFIG_SUPPORT_HTTP_MOODS == type || CONFIG_SUPPORT_HTTP_UNITS == type) {
					return (bSoftwareVersionRev>=4 ? TRUE : FALSE);
				}			
				else if (CONFIG_SUPPORT_HTTP_IRTX == type) {
					if (bSoftwareVersionRev>=5) {
						return (NODE_MASTER_LCD == (pNode->bNodeFlags & NODE_MASTER_LCD));
					}
				}
				else if (CONFIG_SUPPORT_HTTP_SCHEDULE == type) {
					if (bSoftwareVersionRev>=4) {
						return (NODE_MASTER_LCD == (pNode->bNodeFlags & NODE_MASTER_LCD));
					}
				}
				else if (CONFIG_SUPPORT_HTTP_TRIGGERACTIONS == type) {
					return (bSoftwareVersionRev>=2 ? TRUE : FALSE);										
				}
			} break;
			case NODE_PIN_GATEWAY:
			{
				// Bij tcp/ip module:
				// Moods, units & trigger 
				// irtx & scheduling wanneer master.
				//
				// Sedert V16.32: Integratie sbox ultra.
				const BYTE productCode = pNode->GetProductID();
				if (PRODUCT_ID_GATEWAY_HS == productCode ||
					PRODUCT_ID_GATEWAY_HS_32VIRT == productCode || 
					PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT == productCode ||
					PRODUCT_ID_HC_BOX_ULTRA == productCode) 
				{ 
					if (CONFIG_SUPPORT_HTTP_MOODS == type || 
						CONFIG_SUPPORT_HTTP_UNITS == type ||
						CONFIG_SUPPORT_HTTP_TRIGGERACTIONS == type) {
						return true;	
					}
					if (CONFIG_SUPPORT_HTTP_IRTX == type ||
						CONFIG_SUPPORT_HTTP_SCHEDULE == type) {
						return (NODE_MASTER_LCD == (pNode->bNodeFlags & NODE_MASTER_LCD));
					}
				}	  	

				// Sedert V16.33: DALI config is via HTTP.
				// Enkel ondersteund voor de sbox ultra.
				if (PRODUCT_ID_HC_BOX_ULTRA == productCode) {
					if (CONFIG_SUPPORT_HTTP_DALI == type) {
						return true;
					}
				}

			} break;
			case NODE_PIN_STD_UNITS:
			{
				// Bij universele audio interface.
				// AvMatrix & Trigger and actions.
				//
				// BUG-FIX V16.31: Ook de units config (enkel via de API).
				const BYTE productCode = pNode->GetProductID();
				if (PRODUCT_ID_8CHIRTX_AVIP == productCode) { 
					if (CONFIG_SUPPORT_HTTP_AVMATRIX == type ||
						CONFIG_SUPPORT_HTTP_TRIGGERACTIONS == type ||
						CONFIG_SUPPORT_HTTP_UNITS == type) {
						return true;
					}
				}
			} break;
		}	
	}
	return false;
}

unsigned long CNodeConfigSupportHttpInfo_Util::getSupportedFlags(CCanNode* pNode) {
	unsigned long ulValue = CONFIG_SUPPORT_HTTP_NONE;

	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_MOODS)) {
		ulValue += CONFIG_SUPPORT_HTTP_MOODS;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_UNITS)) {
		ulValue += CONFIG_SUPPORT_HTTP_UNITS;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_IRTX)) {
		ulValue += CONFIG_SUPPORT_HTTP_IRTX;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_SCHEDULE)) {
		ulValue += CONFIG_SUPPORT_HTTP_SCHEDULE;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_AVMATRIX)) {
		ulValue += CONFIG_SUPPORT_HTTP_AVMATRIX;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_TRIGGERACTIONS)) {
		ulValue += CONFIG_SUPPORT_HTTP_TRIGGERACTIONS;
	}
	if (isSupported(pNode,CONFIG_SUPPORT_HTTP_DALI)) {
		ulValue += CONFIG_SUPPORT_HTTP_DALI;
	}
	return ulValue;
}
/******************************************************/

/******************************************************/
// Sedert Version 16.04			
bool CNodeFirmware_Util::isSupported(CCanNode* pNode) {
	bool result = false;
	if (pNode) {
		const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
		if (0x66 == bSoftwareVersion) {	
			const BYTE productCode = pNode->GetProductID();
			if (PRODUCT_ID_8CHIRTX_AVIP == productCode || 
				PRODUCT_ID_TFT_32VIRT == productCode || 
				PRODUCT_ID_GATEWAY_HS == productCode ||
				PRODUCT_ID_GATEWAY_HS_32VIRT == productCode ||
				PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT == productCode ||
				PRODUCT_ID_HC_BOX_ULTRA == productCode) 
			{ 
				result = true;
			}
		}
	}
	return result;
}
/******************************************************/


/******************************************************/
// Sedert V16.41:
// Specifieke klasse voor de smartbox ultra.
bool CNodeConfigInfo_SmartboxUltra::isType(CCanNode* pNode) {
	if (NULL == pNode) {
		return false;
	}	
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (0x66 != bSoftwareVersion) {	
		return false;
	}			
	return (PRODUCT_ID_HC_BOX_ULTRA == pNode->GetProductID());
}

int CNodeConfigInfo_SmartboxUltra::getUnitAddressLastInternalUnit(CCanNode* pNode) {
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return -1;
	}
	return 0x30;
}

int CNodeConfigInfo_SmartboxUltra::getUnitAddressLastInternalUnit(CCanNode* pNode,int unitType) {
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return -1;
	}
	int retVal = -1;
	switch(unitType) {
		case UNIT_TYPE_SENS: {
			// Unit 0x00 & 0x01
			retVal = 0x01;
		} break;
		case UNIT_TYPE_LCD_VIRTUAL: {
			// Unit 0x02 tem 0x0B
			retVal = 0x0b;														
		} break;
		case UNIT_TYPE_CONTROL: {
			// Unit 0x0C tem 0x1B
			retVal = 0x1B;														
		} break;
		case UNIT_TYPE_DUOSWITCH: {
			// Unit 0x1C
			retVal = 0x1C;														
		} break;
		case UNIT_TYPE_DIM: {
			// Unit 0x1D tem 0x20
			retVal = 0x20;														
		} break;
		case UNIT_TYPE_SWITCH: {
			// Unit 0x21 tem 0x30
			retVal = 0x30;														
		} break;
	}
	return retVal;
}

int CNodeConfigInfo_SmartboxUltra::getUnitAddressFirstInternalUnit(CCanNode* pNode,int unitType) {
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return -1;
	}
	int retVal = -1;
	switch(unitType) {
		case UNIT_TYPE_SENS: {
			// Unit 0x00 & 0x01
			retVal = 0x00;
		} break;
		case UNIT_TYPE_LCD_VIRTUAL: {
			// Unit 0x02 tem 0x0B
			retVal = 0x02;														
		} break;
		case UNIT_TYPE_CONTROL: {
			// Unit 0x0C tem 0x1B
			retVal = 0x0C;														
		} break;
		case UNIT_TYPE_DUOSWITCH: {
			// Unit 0x1C
			retVal = 0x1C;														
		} break;
		case UNIT_TYPE_DIM: {
			// Unit 0x1D tem 0x20
			retVal = 0x1D;														
		} break;
		case UNIT_TYPE_SWITCH: {
			// Unit 0x21 tem 0x30
			retVal = 0x21;														
		} break;
	}
	return retVal;
}


CString CNodeConfigInfo_SmartboxUltra::getStringLabelInfo(CCanNode* pNode, int unitType, int unitAddress) {
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return CString();
	}
	// Momenteel is er enkel een label voor de interne units van de smartbox ultra.
	if (unitAddress > getUnitAddressLastInternalUnit(pNode)) {
		return CString();
	}

	CString str;
	const int first = getUnitAddressFirstInternalUnit(pNode,unitType);
	if (first < 0) {
		INVARIANT(first >= 0);
		return CString();
	}

	switch(unitType) {
		case UNIT_TYPE_CONTROL: {
			// UNIT_TYPE_CONTROL:		0 tem F				
			str.Format("%X Input",unitAddress-first);
		} break;
		case UNIT_TYPE_SENS: {
			// UNIT_TYPE_SENS:			T0 , T1				
			str.Format("T%d Sens",unitAddress-first);
		} break;
		case UNIT_TYPE_DIM: {
			// UNIT_TYPE_DIM:			0 tem 3				
			str.Format("%X Output",unitAddress-first);							
		} break;
		case UNIT_TYPE_SWITCH: {
			// UNIT_TYPE_SWITCH:		4 tem 13						
			str.Format("%X Output",(unitAddress-first)+4);							
		} break;
		case UNIT_TYPE_DUOSWITCH: {
			// UNIT_TYPE_DUOSWITCH:		Up/Down			
			str = "Up/Down";					  
		} break;
	}
	return str;
}

/******************************************************/
// Sedert V16.42:
// Specifieke klasse voor de touch buttons.	
bool CNodeConfigInfo_TouchButtons::isType(CCanNode* pNode) {
	if (NULL == pNode) {
		return false;
	}	
	const BYTE bProductCode = pNode->GetProductID();
	return ((PRODUCT_ID_TCHBTN_4CO1SE == bProductCode) || 
		    (PRODUCT_ID_TCHBTN_1CO1SE == bProductCode) ||
			(PRODUCT_ID_TCHBTN_2CO1SE == bProductCode) ||
			(PRODUCT_ID_TCHBTN_8CO1SE == bProductCode));
}

// Opvragen van de tekst label info.
// De tekst die weergeven wordt op de sticker van de module.
CString CNodeConfigInfo_TouchButtons::getStringLabelInfo(CCanNode* pNode, int unitType, int unitAddress) {
	if (NULL == pNode) {
		return CString();
	}	
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return CString();
	}

	CString str;
	const BYTE bProductCode = pNode->GetProductID();
	const int buttonIdx = unitAddress;
	switch(bProductCode) {
		case PRODUCT_ID_TCHBTN_1CO1SE: {
			switch(buttonIdx) {
				case 0: {
					str = "Center";
				} break;			   
			}
		} break;
		case PRODUCT_ID_TCHBTN_2CO1SE: {
			switch(buttonIdx) {
				case 0: {
					str = "Left";
				} break;
				case 1: {
					str = "Right";
				} break;
			}									   
		} break;
		case PRODUCT_ID_TCHBTN_4CO1SE: {
			switch(buttonIdx) {
				case 0: {
					str = "Topleft";
				} break;
				case 1: {
					str = "Bottomleft";
				} break;
				case 2: {
					str = "Topright";
				} break;
				case 3: {
					str = "Bottomright";
				} break;
			}
		} break;
		case PRODUCT_ID_TCHBTN_8CO1SE: {
			// Voorlopig leeg laten...		   
		} break;
	}
	return str;
}

/******************************************************/
// Sedert V16.42:
// Specifieke klasse voor de Serinity buttons.
bool CNodeConfigInfo_SerenityButtons::isType(CCanNode* pNode) {
	if (NULL == pNode) {
		return false;
	}	
	const BYTE bProductCode = pNode->GetProductID();
	return ((PRODUCT_ID_1BTN_PUSH_1CO1SE == bProductCode) || 
		    (PRODUCT_ID_2BTN_PUSH_2CO1SE == bProductCode) ||
			(PRODUCT_ID_4BTN_PUSH_4CO1SE == bProductCode));
}


// Opvragen van de tekst label info.
// De tekst die weergeven wordt op de sticker van de module.
CString CNodeConfigInfo_SerenityButtons::getStringLabelInfo(CCanNode* pNode, int unitType, int unitAddress) {
	if (NULL == pNode) {
		return CString();
	}	
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return CString();
	}

	CString str;
	const BYTE bProductCode = pNode->GetProductID();
	const int buttonIdx = unitAddress;
	switch(bProductCode) {
		case PRODUCT_ID_1BTN_PUSH_1CO1SE: {
			switch(buttonIdx) {
				case 0: {
					str = "Center";
				} break;			   
			}
		} break;
		case PRODUCT_ID_2BTN_PUSH_2CO1SE: {
			switch(buttonIdx) {
				case 0: {
					str = "Left";
				} break;
				case 1: {
					str = "Right";
				} break;
			}									   
		} break;
		case PRODUCT_ID_4BTN_PUSH_4CO1SE: {
			switch(buttonIdx) {
				case 0: {
					str = "Topleft";
				} break;
				case 1: {
					str = "Bottomleft";
				} break;
				case 2: {
					str = "Topright";
				} break;
				case 3: {
					str = "Bottomright";
				} break;
			}
		} break;
	}
	return str;
}

/******************************************************/
// Sedert V16.42:
// Specifieke klasse voor de Serinity buttons.
bool CNodeConfigInfo_VelocityButtons::isType(CCanNode* pNode) {
	if (NULL == pNode) {
		return false;
	}	
	const BYTE bProductCode = pNode->GetProductID();
	return ((PRODUCT_ID_1BTN_SWITCH_2CO1SE == bProductCode) || 
		    (PRODUCT_ID_2BTN_SWITCH_4CO1SE == bProductCode) ||
			(PRODUCT_ID_4BTN_SWITCH_8CO1SE == bProductCode));
}

// Opvragen van de tekst label info.
// De tekst die weergeven wordt op de sticker van de module.
CString CNodeConfigInfo_VelocityButtons::getStringLabelInfo(CCanNode* pNode, int unitType, int unitAddress) {
	if (NULL == pNode) {
		return CString();
	}	
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return CString();
	}

	CString str;
	const BYTE bProductCode = pNode->GetProductID();
	const int buttonIdx = unitAddress;
	switch(bProductCode) {
		case PRODUCT_ID_1BTN_SWITCH_2CO1SE: {
			switch(buttonIdx) {
				case 0: {
					str = "Center up";
				} break;
				case 1: {
					str = "Center down";
				} break;
			}
		} break;
		case PRODUCT_ID_2BTN_SWITCH_4CO1SE: {
			switch(buttonIdx) {
				case 0: {
					str = "Left up";
				} break;
				case 1: {
					str = "Left down";
				} break;
				case 2: {
					str = "Right up";
				} break;
				case 3: {
					str = "Right down";
				} break;
			}									   
		} break;
		case PRODUCT_ID_4BTN_SWITCH_8CO1SE: {
			switch(buttonIdx) {
				case 0: {
					str = "Topleft up";
				} break;
				case 1: {
					str = "Topleft dn";
				} break;
				case 2: {
					str = "Bottomleft up";
				} break;
				case 3: {
					str = "Bottomleft dn";
				} break;
				case 4: {
					str = "Topright up";
				} break;
				case 5: {
					str = "Topright dn";
				} break;
				case 6: {
					str = "Bottomright up";
				} break;
				case 7: {
					str = "Bttomright dn";
				} break;
			}
		} break;
	}
	return str;
}


/******************************************************/
/******************************************************/
/******************************************************/

// Opvragen of de node van het type LCD is
bool CNodeConfigInfo_Touchscreen_V66::isType(CCanNode* pNode) {
	if (NULL == pNode) {
		return false;
	}	
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (0x66 != bSoftwareVersion) {	
		return false;
	}			
	return (PRODUCT_ID_TFT_32VIRT == pNode->GetProductID());
}

// Het laatste unit address opvragen van de interne units. 
// -1 indien ongeldig
int CNodeConfigInfo_Touchscreen_V66::getUnitAddressLastInternalUnit(CCanNode* pNode) {
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return -1;
	}
	return 0x63;
};

/******************************************************/
// Sedert V16.47

// Opvragen of de node van het type LCD is.
bool CNodeConfigInfo_DT10_AU::isType(CCanNode* pNode) {
	if (NULL == pNode) {
		return false;
	}	
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (0x66 != bSoftwareVersion) {	
		return false;
	}			
	return (PRODUCT_ID_8CHIRTX_AVIP == pNode->GetProductID());
}

	// Het laatste unit address opvragen van de interne units. 
	// -1 indien ongeldig
int CNodeConfigInfo_DT10_AU::getUnitAddressLastInternalUnit(CCanNode* pNode) {
	// 1 AV-Matrix unit		0x00
	// 8 IRTX Units.		0x01 - 0x08
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return -1;
	}
	return 0x08;
}


/******************************************************/
// Sedert V16.47

bool CNodeConfigInfo_DT18_PRO::isType(CCanNode* pNode) {
	if (NULL == pNode) {
		return false;
	}	
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();
	if (0x66 != bSoftwareVersion) {	
		return false;
	}			
	return (PRODUCT_ID_GATEWAY_HS == pNode->GetProductID() || 
			PRODUCT_ID_GATEWAY_HS_32VIRT == pNode->GetProductID() ||
			PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT == pNode->GetProductID());
}

int CNodeConfigInfo_DT18_PRO::getUnitAddressLastInternalUnit(CCanNode* pNode) {
	// 32 virtuals: 0x00 tem 0x1F
	if (!isType(pNode)) {
		REQUIRE(true == isType(pNode));
		return -1;
	}
	return 0x1F;
}
