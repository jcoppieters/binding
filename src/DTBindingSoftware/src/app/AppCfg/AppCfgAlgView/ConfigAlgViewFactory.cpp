#include "stdafx.h"
#include "ConfigAlgViewfactory.h"

#include "ConfigAlgViewBitmaps.h"
#include "ConfigAlgViewgateway.h"
#include "ConfigAlgViewirrx.h"
#include "ConfigAlgViewirtx.h"
#include "ConfigAlgViewMM.h"
#include "ConfigAlgViewMoods.h"
#include "ConfigAlgViewRoomCtrlAv.h"
#include "ConfigAlgViewschedule.h"
#include "ConfigAlgViewsmapunits.h"
#include "ConfigAlgViewsmapavroom.h"
#include "ConfigAlgViewsmapmain.h"
#include "ConfigAlgViewsmapmyhome.h"
#include "ConfigAlgViewUnits.h"
#include "ConfigAlgViewBoseST.h"
#include "ConfigAlgViewLedsCfg.h"
#include "ConfigAlgViewSonosSrc.h"
#include "ConfigAlgViewavsettings.h"
#include "ConfigAlgViewavmatrix.h"
#include "ConfigAlgViewtriggers.h"
#include "ConfigAlgViewMobileApp.h"
#include "ConfigAlgViewDALI.h"

/************************** private ************************/

template <class PRODUCT>
	static PRODUCT* helper_Create(
		CCanNode* pNode,
		CConfigAlgView::IViewDisplay* pNotifyView)
	{
		return new PRODUCT(pNode,pNotifyView);
	}

/***********************************************************/

CConfigAlgView* CConfigAlgViewFactory::Create(
	CConfigAlgViewID id,
	CCanNode* pNode,
	CConfigAlgView::IViewDisplay* pNotifyView)
{
	CConfigAlgView* p = 0;
	switch (id) {
		case CONFIGALGVIEW_ID_CREATE_UNITS: {
			p = helper_Create<CConfigAlgViewUnits>(pNode,pNotifyView);	
		} break;
		case CONFIGALGVIEW_ID_MOODS: {
			p = helper_Create<CConfigAlgViewMoods>(pNode,pNotifyView);	
		} break;
		case CONFIGALGVIEW_ID_IRRX: {
			p = helper_Create<CConfigAlgViewIRRX>(pNode,pNotifyView);	
		} break;
		case CONFIGALGVIEW_ID_IRTX: {
			p = helper_Create<CConfigAlgViewIRTX>(pNode,pNotifyView);	
		} break;
		case CONFIGALGVIEW_ID_SCHEDULE: {
			p = helper_Create<CConfigAlgViewShedule>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_BITMAPS: {
			p = helper_Create<CConfigAlgViewBitmaps>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_MULTIMEDIA: {
			p = helper_Create<CConfigAlgViewMM>(pNode,pNotifyView);	
		} break;
		case CONFIGALGVIEW_ID_SMAP_UNITS: {
			p = helper_Create<CConfigAlgViewSmapUnits>(pNode,pNotifyView);	
		} break;
		case CONFIGALGVIEW_ID_SMAP_MAIN: {
			p = helper_Create<CConfigAlgViewSmapMain>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_SMAP_MYHOME: {
			p = helper_Create<CConfigAlgViewSmapMyHome>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_SMAP_AVROOM: {
			p = helper_Create<CConfigAlgViewSmapAVRoom>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_CFG_GATEWAY: {
			p = helper_Create<CConfigAlgViewGateway>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_ROOMCTRL_AV: {
			p = helper_Create<CConfigAlgViewRoomCtrlAV>(pNode,pNotifyView);		
		} break;
		case CONFIGALGVIEW_ID_BOSEST: {
			p = helper_Create<CConfigAlgViewBoseST>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_LEDSCFG: {
			p = helper_Create<CConfigAlgViewLedsCfg>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_SONOSSRC: {
			p = helper_Create<CConfigAlgViewSonosSrc>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_AVSETTINGS: {
			p = helper_Create<CConfigAlgViewAVSettings>(pNode,pNotifyView);
		} break;
		case CONFIGALGVIEW_ID_AVMATRIX:	 {
			p = helper_Create<CConfigAlgViewAVMatrix>(pNode,pNotifyView);			
		} break;
		case CONFIGALGVIEW_ID_TRIGGERS:  {
			p = helper_Create<CConfigAlgViewTriggers>(pNode,pNotifyView);			
		} break;
		case CONFIGALGVIEW_ID_MOBILE_APP: {
			p = helper_Create<CConfigAlgViewMobileApp>(pNode,pNotifyView);			
		} break;
		case CONFIGALGVIEW_ID_DALI: {
			p = helper_Create<CConfigAlgViewDALI>(pNode,pNotifyView);				
		} break;
		default: {
			ASSERT(p);
		} break;
	}
	return p;
}

void CConfigAlgViewFactory::Destroy(CConfigAlgView** p) {
	if (*p){
		delete *p;
		*p = 0;
	}
};

/************************* EOF *******************/