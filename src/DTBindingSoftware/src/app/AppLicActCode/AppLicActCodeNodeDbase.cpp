#include "stdafx.h"
#include "AppLicActCodeNodeDbase.h"
#include "AppLicActCodeDef.h"
#include "AppLicActCodeAlg.h"

#include "NodeDatabase.h"

//#include "app\appCfg\AppCfginfo\cfginfonode.h"	

#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"	
#include "app\appnodedbase\appnodedbase_util.h"
	
namespace
{
	// globals.
	typedef struct
	{
		int productID;		
		int allowed;	

	} STRUCT_ENTRY_PRODUCT;

	static const STRUCT_ENTRY_PRODUCT g_arrLicCode[ ] =
	{
		/*  productID,  allowed */
		{ PRODUCT_ID_HC_BOX, PRODUCT_HC_BOX_ALLOWED_ITEMS },
		{ PRODUCT_ID_STD_DMX_MASTER_64DIM, PRODUCT_DMX_MASTER_ALLOWED_ITEMS },
		{ PRODUCT_ID_STD_DALI_MASTER_DIM, PRODUCT_DALI_MASTER_ALLOWED_ITEMS },
		{ PRODUCT_ID_TFT_32VIRT, PRODUCT_TFT_ALLOWED_ITEMS },
		{ PRODUCT_ID_OLED, PRODUCT_OLED_ALLOWED_ITEMS }
	};

	static const int PRODUCT_NR_OF_ITEMS = (sizeof(g_arrLicCode)/sizeof(STRUCT_ENTRY_PRODUCT));
};

/***************************** private ****************************/

static 
inline BOOL checkNodeDatabase(
	  CNodeDatabase* pNodeDatabase)
{
	int nrOfNodes = 0;
	return (pNodeDatabase->NROfNodes(&nrOfNodes));	
}


// Controle of deze nodetype een licentie code nodig heeft.
static
BOOL checkNodeTypeNeedsLicActCode(
  const CCanNode& refNode)
{
	BOOL fResult = FALSE;
	const BYTE productID = refNode.GetProductID();
	int product;

	for (product=0;product<PRODUCT_NR_OF_ITEMS;++product)
	{
		const STRUCT_ENTRY_PRODUCT* const pEntry = &g_arrLicCode[product];

		if ((productID==pEntry->productID)&&(pEntry->allowed!=NO_LICENSE_NEEDED))
		{
			fResult = TRUE;
			break;
		}
	}
	return fResult;
}

/***************************** public ****************************/

BOOL CAppLicActCodeNodeDbase::CheckLicActCodeNeeded(
	CNodeDatabase *pNodeDatabase)
{
	BOOL fResult = FALSE;
	int product;	
	int count[PRODUCT_NR_OF_ITEMS];

	for (product=0;product<PRODUCT_NR_OF_ITEMS;++product)
	{
		count[product] = 0;
	}

	if (TRUE == checkNodeDatabase(pNodeDatabase))
	{
		for (CNodeIterator it(pNodeDatabase->CreateNodeIterator());
			 !it.IsDone();
			 it.Next())
		{
			const BYTE productID = it.CurrentItem()->GetProductID();
		
			for (product=0;product<PRODUCT_NR_OF_ITEMS;++product)
			{
				if (productID == g_arrLicCode[product].productID)
				{
					count[product]++;	
				}
			}								
		}
	}

	// Alle producten in de lijst overlopen.
	for (product=0;product<PRODUCT_NR_OF_ITEMS;++product)
	{
		const STRUCT_ENTRY_PRODUCT* const pEntry = &g_arrLicCode[product];

		if ((pEntry->allowed!=NO_LICENSE_NEEDED)&&(count[product]>pEntry->allowed))
		{
			fResult = TRUE;
		}
	}
	return fResult;
}

int CAppLicActCodeNodeDbase::FindNodesLicActCodeNeeded(
	CNodeDatabase *pNodeDatabase,
	CCanNode** ppNodes,
	int maxElements)
{
	int nrOfItems = 0;
	int product;

	for(product=0;product<PRODUCT_NR_OF_ITEMS;++product)
	{
		// CHANGES_ACTIVATION_LICENSE_CODE
		// Oppassen: Mogen enkel nodes toevoegen aan de lijst die hoger zijn dan de 
		// toegelaten waarden.
		const int productID = g_arrLicCode[product].productID;
		const int allowed = g_arrLicCode[product].allowed;

		if (NO_LICENSE_NEEDED != allowed && 
			CAppNodeDbase_Util::getCountNodesWithProductID(pNodeDatabase,productID)>allowed)
		{
			const int tempCnt = 
				CAppNodeDbase_Util::FindNodesWithProductID(
						pNodeDatabase,
						productID,
						&ppNodes[nrOfItems],
						PRODUCT_NR_OF_ITEMS-nrOfItems);

			nrOfItems += tempCnt;		
		}
	}

	return nrOfItems;
}

BOOL CAppLicActCodeNodeDbase::CheckLicenseCodeValid(
	CNodeDatabase *pNodeDatabase,
	unsigned long ulCode)
{
	BOOL fResult = FALSE;

	if (TRUE == checkNodeDatabase(pNodeDatabase))
	{
		const unsigned long decodeValue = CAppLicActCodeAlg::Decode(ulCode);

		for (CNodeIterator it(pNodeDatabase->CreateNodeIterator());
			 !it.IsDone();
			 it.Next())
		{	
			CCanNode* const pNode = it.CurrentItem();

			if (TRUE == checkNodeTypeNeedsLicActCode(*pNode))
			{
				// Enkel indien deze node een productID heeft die een licentie code nodig heeft.

				const unsigned long ulPhysicalAddress = pNode->getPhysicalAddress();	
				if (decodeValue == ulPhysicalAddress)
				{
					fResult = TRUE;
					break;
				}
			}
		}
	}
	return fResult;
}