#pragma once

#include "ConfigAlgView.h"

class CCanNode;

/************************************************
 * IDs
 ***********************************************/
typedef enum
{
	CONFIGALGVIEW_ID_CREATE_UNITS		= 0,
	CONFIGALGVIEW_ID_MOODS,
	CONFIGALGVIEW_ID_IRTX,
	CONFIGALGVIEW_ID_IRRX,
	CONFIGALGVIEW_ID_SCHEDULE,
	CONFIGALGVIEW_ID_BITMAPS,
	CONFIGALGVIEW_ID_MULTIMEDIA,
	CONFIGALGVIEW_ID_SMAP_UNITS,
	CONFIGALGVIEW_ID_SMAP_MAIN,
	CONFIGALGVIEW_ID_SMAP_MYHOME,
	CONFIGALGVIEW_ID_SMAP_AVROOM,
	CONFIGALGVIEW_ID_CFG_GATEWAY,
	CONFIGALGVIEW_ID_ROOMCTRL_AV,
	CONFIGALGVIEW_ID_BOSEST,
	CONFIGALGVIEW_ID_LEDSCFG,
	CONFIGALGVIEW_ID_SONOSSRC,
	CONFIGALGVIEW_ID_AVSETTINGS,
	CONFIGALGVIEW_ID_AVMATRIX,			// CHANGES_AV_UNIVERSAL
	CONFIGALGVIEW_ID_TRIGGERS,			// CHANGES_AV_UNIVERSAL
	CONFIGALGVIEW_ID_MOBILE_APP,
	CONFIGALGVIEW_ID_DALI				// sedert V16.33

} CConfigAlgViewID;


/************************************************
 * Factory klasse.
 ************************************************/
struct CConfigAlgViewFactory
{
public:
	static CConfigAlgView* Create(
		CConfigAlgViewID id,
		CCanNode* pNode,
		CConfigAlgView::IViewDisplay* pNotifyView);

	static void Destroy(CConfigAlgView** p);
};
/************************* EOF *******************/