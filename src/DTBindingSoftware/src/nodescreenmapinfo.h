#ifndef INCLUDED_DT_NODESCREENMAP_INFO_H
#define INCLUDED_DT_NODESCREENMAP_INFO_H

#ifndef INCLUDED_SCREENMAPID_H
	#include "screenmapid.h"
#endif
///////////////////////////////////////////////////////
class CCanNode;
///////////////////////////////////////////////////////


enum NodeScreenmapConfigtype
{
	NODESCREENMAP_CONFIG_TYPE_NONE  = 0x0000,
	NODESCREENMAP_CONFIG_TYPE_V64 	= 0x0001,
	NODESCREENMAP_CONFIG_TYPE_V65 	= 0x0002,
	NODESCREENMAP_CONFIG_TYPE_V66 	= 0x0003
};


//////////////////////////////////////////////////////
struct CNodeScreenmapInfo_Util
{
	/**********************************************//**
	 * @brief	Geef het aantal controls per pagina
	 *
	 * @param	pNode : [IN] Cannode
	 * @param	pTypes: [IN] Array van screenmap types.
	 * @param	NrOfButtons: [OUT] Het aantal controls 
	 * @param	MaxType: [IN] De lengte van het array.
	 *
	 *************************************************/
	static bool getMaxControls(CCanNode* pNode, 
							  const CScreenmapID* pTypes,
							  int* NrofButtons,
							  int MaxTypes);

	/**********************************************//**
	 * @brief	Geef het aantal controls per pagina
	 *
	 * @param	pNode : [IN] Cannode
	 * @param	pTypes: [IN] Array van screenmap types.
	 * @param	supportedUnittype: [OUT] vlaggen
	 * @param	MaxType: [IN] De lengte van het array.
	 *
	 *************************************************/
	static bool getSupportedTypes(CCanNode* pNode, 
								 const CScreenmapID* pTypes,
								 int* supportedUnitTypes,
								 int MaxTypes);

	/**********************************************//**
	 * @brief	Geef het totaal aantal entries in de smap
	 *
	 * @param	pNode : [IN] Cannode
	 * @param	pTypes: [IN] Array van screenmap types.
	 * @param	NrOfButtons: [OUT] Het aantal controls 
	 * @param	MaxType: [IN] De lengte van het array 
	 *			-1 is don't care
	 *
	 *************************************************/
	static bool getMaxEntries(CCanNode* pNode, 
							  const CScreenmapID* pTypes,
							  int* NrofButtons,
							  int MaxTypes);

	/**********************************************//**
	 * @brief	Is screenmapping supported ?
	 *************************************************/
	static bool isSupported(CCanNode* pNode);

	/**********************************************//**
     * @brief	Is enkel via de Duotecno Pro App en niet via de CAN-Bus.
	 *			Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
	 *************************************************/
	static bool isOnlyProAppConfig(CCanNode* pNode);

	/**********************************************//**
	 * @brief	het type screenmap config die bij deze node
	 *			hoort.
	 *************************************************/
	static NodeScreenmapConfigtype getNodeType(CCanNode* pNode);
};
///////////////////////////////////////////////////////

struct CNodeScreenmapLiteInfo_Util
{
public:
	typedef enum 
	{
		// Lite screenmapping van een smarttouch oled sensor.
		SUPPORTED_FLAG_SENSOR		=	(1<<0)	

	} Flags_t;


	/**********************************************//**
	 * @brief	Is screenmapping supported ?
	 *************************************************/
	static bool isSupported(CCanNode* pNode);

	/**********************************************//**
	 * @brief	Is dit onderdeel van screenmapping ondersteund.
	 *************************************************/
	static bool isSupported(CCanNode* pNode,Flags_t flag);


	/**********************************************//**
     * @brief	Is er na het doorsturen een Update node database nodig ?
	 *
	 * Bij de Room-e is er geen update node database nodig, bij de Smarttouch wel.
	 **************************************************/
	static bool isUpdateNodeDbaseNeeded(CCanNode* pNode);
					
}; /* CNodeScreenmapLiteInfo_Util */

#endif