#ifndef INCLUDED_DT_NODESCHEDULE_INFO_H
#define INCLUDED_DT_NODESCHEDULE_INFO_H

/*******************************************************/
class CCanNode;
/*******************************************************/

// Sedert versie 0x0700
enum NodeScheduleConfigtype
{
	NODESCHEDULE_CONFIG_TYPE_NONE	= 0x0000,
	NODESCHEDULE_CONFIG_TYPE_NORMAL = 0x0001,
	NODESCHEDULE_CONFIG_TYPE_LITE 	= 0x0002,
};


//////////////////////////////////////////////////////

struct CNodeScheduleConfigInfo_Util
{
	/**********************************************//**
	 * @brief	Is scheduling supported ?
	 *************************************************/
	static bool isSupported(CCanNode* pNode);

	/**********************************************//**
	 * @brief	het type scheduling config die bij deze node
	 *			hoort.
	 *************************************************/
	static NodeScheduleConfigtype getNodeType(CCanNode* pNode);
};

//////////////////////////////////////////////////////


#endif /* INCLUDED_DT_NODESCHEDULE_INFO_H */