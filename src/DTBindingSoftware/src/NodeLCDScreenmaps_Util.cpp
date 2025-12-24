// NodeLCDScreenmaps_Util.cpp: implementation of the CNodeLCDScreenmaps_Util class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeLCDScreenmaps_Util.h"

#include "App\AppCfg\AppCfgsmap\configsmapsmapinfo.h"
#include "App\AppCfg\AppCfgsmap\configsmapsmapinfoclass.h"

#include "nodelcdscreenmaps.h"
#include "screenmaps.h"
#include "cannode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

namespace
{
	BOOL find(	
		CConfigSmapSmapInfoClass& refConfigSmapClass,
		const CScreenmapID screenmapID,
		CSerSmapSmapInfo** pEntry)
	{
		BOOL result = FALSE;
	
		const int Count = refConfigSmapClass.GetMax( );
		for (int j=0;j<Count;j++)
		{
			CSerSmapSmapInfo* const p = refConfigSmapClass.getEntry(j);
			
			if (p && p->getScreenmapID() == screenmapID.get())
			{
				*pEntry = p;
				result = TRUE;
				break;
			}
		}
		return result;
	}		 
	
	BOOL create(
		const CScreenmapID screenmapID,
		CSerSmapSmapInfo** pNew)
	{
		BOOL result = FALSE;
	
		CSerSmapSmapInfo* const pTemp = new CSerSmapSmapInfo( );
		if (pTemp)
		{
			pTemp->setScreenmapID( screenmapID.get() );

			*pNew = pTemp;
			result = TRUE;
		}
			
		return result;
	}
};


//////////////////////////////////////////////////////////////////////


BOOL CNodeLcdScreenmaps_Util::update_StartPage(
	CConfigSmapSmapInfo& refSmapSmapInfo,
	CNodeLcdScreenmaps* pNodeLCDScreenmaps)
{
	BOOL result = FALSE;

	const int MaxScreenmaps = pNodeLCDScreenmaps->getMaxScreenmaps( );
	for (int i=0;i<MaxScreenmaps;i++)
	{
		IScreenmap* pScreenmap = pNodeLCDScreenmaps->GetScreenmap( i );
		if (pScreenmap)
		{
			const CScreenmapID screenmapID = pScreenmap->getScreenmapID();

			CConfigSmapSmapInfoClass* pClass = static_cast<CConfigSmapSmapInfoClass*>
				(refSmapSmapInfo.getConfigFile().
					GetClass(ConfigSmapSmapInfoClasses::ENUM_CONFIGCLASS_SMAP_SMAPINFO));

			if (pClass)
			{
				CSerSmapSmapInfo* pEntry = 0;

				if (find(*pClass,screenmapID,&pEntry))
				{
					const int startPage = pEntry->getStartPageNR( );										
					pScreenmap->setStartPage( startPage );
					// pScreenmap->SetModified(TRUE);
					result = TRUE;	
				}
			}
		}
	}					
	return result;
};

BOOL CNodeLcdScreenmaps_Util::update_StartPage(
	CNodeLcdScreenmaps& refNodeLCDScreenmaps,
	CConfigSmapSmapInfo* pSmapInfo)
{
	BOOL result = FALSE;

	CConfigSmapSmapInfoClass* pClass = static_cast<CConfigSmapSmapInfoClass*>
		(pSmapInfo->getConfigFile().
			GetClass(ConfigSmapSmapInfoClasses::ENUM_CONFIGCLASS_SMAP_SMAPINFO));

	if (pClass)
	{
		const int MaxScreenmaps = refNodeLCDScreenmaps.getMaxScreenmaps( );
		for (int i=0;i<MaxScreenmaps;i++)
		{
			IScreenmap* pScreenmap = refNodeLCDScreenmaps.GetScreenmap( i );
			if (pScreenmap)
			{
				const CScreenmapID screenmapID = pScreenmap->getScreenmapID();

				CSerSmapSmapInfo* pEntry = 0;
				if (find(*pClass, screenmapID, &pEntry) == FALSE)
				{	
					if (create(screenmapID, &pEntry))
					{				
						if (!pClass->addEntry(pEntry))
						{						
							ASSERT( 0 );
							delete pEntry;
							pEntry = 0;
						}
					}
				}

				if (pEntry)
				{
					const int startPage = pScreenmap->getStartPage( );
					pEntry->setStartPageNR( startPage );
					result = TRUE;
				}
			}
		}	
	}

	return result;
}


/*
BOOL CNodeLcdScreenmaps_Util::update_StartPage(
	CNodeLcdScreenmaps& refNodeLCDScreenmaps,
	CConfigSmapSmapInfo* pSmapInfo)
{
	BOOL result = FALSE;

	CConfigSmapSmapInfoClass* pClass = static_cast<CConfigSmapSmapInfoClass*>
		(pSmapInfo->getConfigFile().
			GetClass(ConfigSmapSmapInfoClasses::ENUM_CONFIGCLASS_SMAP_SMAPINFO));

	if (pClass)
	{
		const int Count = pClass->GetMax( );
		for (int j=0;j<Count;j++)
		{
			CSerSmapSmapInfo* const pEntry = pClass->getEntry(j);
			if (pEntry)
			{
				const int MaxScreenmaps = refNodeLCDScreenmaps.getMaxScreenmaps( );
				for (int i=0;i<MaxScreenmaps;i++)
				{
					IScreenmap* pScreenmap = refNodeLCDScreenmaps.GetScreenmap( i );
					if (pScreenmap)
					{
						const CScreenmapID screenmapID = pScreenmap->getScreenmapID();
						if (pEntry->getScreenmapID( ) == screenmapID.get())
						{
							const int startPage = pScreenmap->getStartPage( );
							pEntry->setStartPageNR( startPage );
							result = TRUE;
						}
					}
				}
			}
		}	
	}

	return result;
}
*/