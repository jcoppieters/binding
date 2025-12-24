#include "stdafx.h"
#include "nodescheduleinfo.h"
#include "cannode.h"

#include "app\appCfg\AppCfginfo\cfginfonode.h"	
#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"	

// Sedert versie 0x0700

bool CNodeScheduleConfigInfo_Util::isSupported(CCanNode* pNode)
{
	bool result = false;

	if (pNode)
	{
		result = CNodeExtConfigInfo_Util::isSupported(pNode,EXTERNAL_CONFIG_SCHEDULE);
	}

	return result;
}


NodeScheduleConfigtype CNodeScheduleConfigInfo_Util::getNodeType(CCanNode* pNode)
{
	NodeScheduleConfigtype cfgType = NODESCHEDULE_CONFIG_TYPE_NONE;

	if (pNode)
	{
		const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();

		if (bSoftwareVersion == 0x65)
		{
			// LCD & Gateway nodes als master.
			const BYTE bNodeType = pNode->bNodeType;
			const BYTE bNrOfUnits = pNode->bNROfUnits;

			if (bNodeType == NODE_LCD || bNodeType == NODE_PIN_GATEWAY)
			{
				if (pNode->bNodeFlags & NODE_MASTER_LCD)
				{
					cfgType = NODESCHEDULE_CONFIG_TYPE_NORMAL;
				}
			}
		}
		else if (bSoftwareVersion >= 0x90)
		{
			// De healthcare/smart box
			const int productID = pNode->GetProductID();
			if (productID == PRODUCT_ID_HC_BOX)
			{
				cfgType = NODESCHEDULE_CONFIG_TYPE_LITE;
			}
		}
	}

	return cfgType;
}