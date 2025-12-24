#include "stdafx.h"
#include "cfginfobindingunit.h"
#include "cfginfoproductID_def.h"

#include "app\appbindings\appbindingunit\bindingunitvirtual.h"

#include "NodeDatabaseOperations.h"

#include "TMGlobals.h"
using namespace TMNodeDatabase;


// Sedert versie 0x0C02
// Public: 
BOOL CCfgInfoBindingUnit::getSupportedAsInput(
	const CBindingUnit* pBindingUnit)
{
	BOOL fResult=FALSE;

	if (BINDING_UNIT_TYPE_LCD_VIRTUAL == pBindingUnit->GetBindingUnitType())
	{
		const CBindingVirtualUnit* const pBindingUnitVirtual =
			static_cast<const CBindingVirtualUnit*>(pBindingUnit);

		if (0!=pBindingUnitVirtual)
		{
			// Zoeken naar de CCanNode ifv de Binding Unit
			CCanNode* pNode=0;
			STRUCT_UNIT* pUnit=0;
			CNodeDatabaseOperations nodeDatabaseOperations(TMGetDocumentNodes());
			
			if (TRUE==nodeDatabaseOperations.Find(pBindingUnit,&pNode,&pUnit))
			{
				ASSERT(0!=pNode);
				ASSERT(0!=pUnit);

				const BYTE bProductID=pNode->GetProductID();

				switch(bProductID)
				{
					case PRODUCT_ID_OLED:
					{
						// De eerste 4 virtuals van de Oled.
						const BYTE bUnitAddress = pBindingUnit->GetUnitAddress();
						if ((bUnitAddress>=0x01)&&(bUnitAddress<=0x04))
						{
							fResult=TRUE;
						}
					} break; /* PRODUCT_ID_OLED */

					case PRODUCT_ID_OLED_HOTEL:
					{
						// De eerste 2 virtuals van de Oled.
						const BYTE bUnitAddress = pBindingUnit->GetUnitAddress();
						if ((bUnitAddress>=0x01)&&(bUnitAddress<=0x02))
						{
							fResult=TRUE;
						}
					} break; /* PRODUCT_ID_OLED_HOTEL */

					case PRODUCT_ID_OLED_TOUCHBTN_4V:
					{
						// De eerste 4 virtuals van de Oled.
						const BYTE bUnitAddress = pBindingUnit->GetUnitAddress();
						if ((bUnitAddress>=0x01)&&(bUnitAddress<=0x04))
						{
							fResult=TRUE;
						}
					} break;

				} /* switch(bProductID) */
			} /* if (TRUE==nodeDatabaseOperations.Find(pBindingUnit,&pNode,&pUnit)) */				 
		} /* if (0!=pBindingUnitVirtual) */
	} /* if (BINDING_UNIT_TYPE_LCD_VIRTUAL == pBindingUnit->GetBindingUnitType()) */
	return fResult;
}
