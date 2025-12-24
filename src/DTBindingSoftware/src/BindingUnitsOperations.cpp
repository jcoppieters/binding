// BindingUnitsOperations.cpp: implementation of the CBindingUnitsOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingUnitsOperations.h"

#include "BindingConstants.h"
#include "BindingUnitOperations.h"
#include "BindingUnitFactory.h"
#include "NodeDatabaseOperations.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"

// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingUnitsOperations::CBindingUnitsOperations(CBindingUnits* pBindingUnits) :
m_pBindingUnits(pBindingUnits)
{
	ASSERT(m_pBindingUnits);
}

CBindingUnitsOperations::~CBindingUnitsOperations()
{
}
///////////////////////////////////////////////////////////////////////
BOOL CBindingUnitsOperations::UpgradeBindingUnitTypes()
{
	if (m_pBindingUnits == 0) 
	{
		ASSERT( m_pBindingUnits );
		return FALSE;
	}

	BOOL fResult = FALSE;
	CBindingUnits::Iterator it(m_pBindingUnits->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingUnit *const pBindingUnit = it.CurrentItem();
		ASSERT(pBindingUnit);
				
		CBindingUnitOperations BindingUnitOperations(pBindingUnit);
		CBindingUnit *pNew = 0;
		if (BindingUnitOperations.UpgradeBindingUnitType(&pNew) == TRUE)
		{
			ASSERT(pNew);
			if (m_pBindingUnits->Replace(pBindingUnit,pNew) == TRUE)
			{
				fResult = TRUE;	// Set TRUE !!!
			}
			else
			{			
				if (pNew != 0)
				{				
					delete pNew;
					pNew = 0;
				}
				ASSERT(0);
				return FALSE;
			}				
		}								
	}	
	return fResult;
}


// todo ...
BYTE CBindingUnitsOperations::GetUnitType(unsigned short nID)
{
	ASSERT (nID);

	CBindingUnit* const pUnit = m_pBindingUnits->Find(nID);
	
	ASSERT(pUnit);
	
	return(pUnit->GetBindingUnitType());
}


BOOL CBindingUnitsOperations::GetUnitType(BYTE bUnitType,int iIndex,CBindingUnit **pReturnUnit)
{
	*pReturnUnit = NULL;

	if (iIndex > m_pBindingUnits->Count()) 
	{
		return FALSE;
	}

	int nCnt = 0;
	CBindingUnits::Iterator it(m_pBindingUnits->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingUnit* const pTempUnit = it.CurrentItem( );
		ASSERT( pTempUnit );

		if (pTempUnit->GetBindingUnitType() == bUnitType)
		{
			if (nCnt++ == iIndex)
			{
				*pReturnUnit = pTempUnit;
				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL CBindingUnitsOperations::GetInputs(int iIndex,CBindingUnit **pReturnUnit)
{	
	*pReturnUnit = NULL;

	if (iIndex > m_pBindingUnits->Count()) 
	{	
		return FALSE;
	}

	int nCnt = 0;	// TM 20090106 : lokale variabele werd niet geïnitialiseerd op 0 
	CBindingUnits::Iterator it(m_pBindingUnits->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingUnit* const pTempUnit = it.CurrentItem();
		ASSERT( pTempUnit );
	
		if (pTempUnit->GetInOutType() == UNIT_TYPE_INPUT) 
		{
			if (nCnt++ == iIndex) 
			{
				*pReturnUnit = pTempUnit;
				return TRUE;
			}
		}
	}	
	return FALSE;
}

BOOL CBindingUnitsOperations::GetOutputs(int iIndex,CBindingUnit **pReturnUnit)
{
	CBindingUnit *pTempUnit;

	*pReturnUnit = NULL;

	const unsigned short usCount = m_pBindingUnits->Count();

	if (iIndex > usCount) 
	{	
		return FALSE;
	}

	for (int i=0,nCnt=0;i<usCount;i++) 
	{
		if ((pTempUnit = m_pBindingUnits->Get(i)) != NULL)
		{		
			if (pTempUnit->GetInOutType() == UNIT_TYPE_OUTPUT) 
			{
				if (nCnt++ == iIndex) 
				{
					*pReturnUnit = pTempUnit;
					return TRUE;
				}
			}
		}		
		ASSERT(pTempUnit);
	}
	return FALSE;
}

BOOL CBindingUnitsOperations::FindUnit(const CBindingUnit* const pIn,CBindingUnit** pOut)
{
	ASSERT(m_pBindingUnits);

	*pOut = NULL;

	if (pIn != NULL)
	{
		const int nMaxEntries = m_pBindingUnits->Count();
		for (int i=0;i<nMaxEntries;i++)
		{
			CBindingUnit* const pUnit = m_pBindingUnits->Get(i);
			ASSERT(pUnit);

			if (pUnit != 0)
			{
				if (pUnit->IsEqual(pIn,TRUE,FALSE) == TRUE)
				{
					*pOut = pUnit;
					return TRUE;
				}			
			}			
		}
	}
	return FALSE;
}

/**
 * Import BindingUnits collection ...
 * BindingUnits that are imported are assigned a new ID.
 * Equal BindingUnits are not added to the list.
 */
BOOL CBindingUnitsOperations::Import(const CBindingUnits* const pImport)
{
	const int nMaxImportUnits = pImport->Count();
	for (int i=0;i<nMaxImportUnits;i++)
	{
		const CBindingUnit* const pBindingUnit = pImport->Get(i);
		ASSERT(pBindingUnit);

		if (pBindingUnit != NULL)
		{			
			CBindingUnit* pAlreadyInList;
			if (FindUnit(pBindingUnit,&pAlreadyInList) == FALSE)
			{
				/**
				 * Base class CBindingUnit --> Base Class CBindingUnit
				 */
				CBindingUnit *pNew = 
					CBindingUnitFactory::Clone(const_cast<CBindingUnit*>(pBindingUnit));


				pNew->SetID(0);	// !!!!!
				const unsigned short nID = m_pBindingUnits->Add(pNew);

				ASSERT(nID);
				ASSERT(nID == pNew->GetID());

				if (nID == 0)
				{			
					return FALSE;
				}		
			}
		}
		else
		{
			ASSERT(0);
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CBindingUnitsOperations::RemoveUnit(CBindingUnit* pUnitRemove)
{
	ASSERT(pUnitRemove);

	if (pUnitRemove != 0)
	{
		const int nCount = m_pBindingUnits->Count();
		for (int i=0;i<nCount;i++)
		{
			CBindingUnit* const pBindingUnit = m_pBindingUnits->Get(i);
			if (pBindingUnit != NULL)
			{
				if (pBindingUnit->IsEqual(pUnitRemove,
										  TRUE,
										  FALSE) == TRUE)
				{
					const unsigned short nID = pBindingUnit->GetID();
					ASSERT(nID);

					BOOL fResult = m_pBindingUnits->Remove(nID);
					ASSERT(fResult);

					return fResult;				
				}
			}
		}	
	}
	return FALSE;
}


int CBindingUnitsOperations::GetEquals(const CBindingUnit* const pIn,CBindingUnits* const pBindingUnits)
{
	ASSERT( pIn );
	int nCount = 0;

	if (pBindingUnits != 0)
	{
		pBindingUnits->RemoveAll( );
	}

	if (pIn != NULL)
	{
		const int nMaxEntries = m_pBindingUnits->Count();
		for (int i=0;i<nMaxEntries;i++)
		{
			CBindingUnit* const pUnit = m_pBindingUnits->Get(i);
			ASSERT(pUnit);

			if (pUnit != 0)
			{
				if (pUnit->IsEqual(pIn,TRUE,FALSE) == TRUE)
				{
					if (pBindingUnits != 0)
					{
						ASSERT( pUnit != 0);
						
						/**
						 * Base class CBindingUnit --> Base Class CBindingUnit
						 */
						CBindingUnit* const pNew = CBindingUnitFactory::Clone( pUnit );
						if (pNew != 0)
						{							
							const unsigned short nID = pBindingUnits->Add(pNew);

							ASSERT( nID != 0 );
							ASSERT( nID == pUnit->GetID() );
							ASSERT( pNew->GetID() == pUnit->GetID() );
						}
					}

					nCount++;					
				}
			}
		}
	}
	return nCount;
}

BOOL CBindingUnitsOperations::SetDefaultNames(CBindingUnit* const pLast)
{
	ASSERT( m_pBindingUnits );
	ASSERT( pLast );

	CBindingUnit* pBindingUnit = 0;
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());

	STRUCT_UNIT* pLastUnit = 0;
	CCanNode* pNode=0;

	if (NodeDatabaseOperations.Find(pLast,&pNode,&pLastUnit))
	{
		char szName[16];

		// if last unit first char == ',' then don't add ',' in front of control name ...
		if (pLastUnit->szUnitName[0] == ',') 
		{					
			strcpy(szName,pLastUnit->szUnitName);
			szName[15] = '\0';						
		}
		else 
		{
			szName[0] = ',';
			strncpy(&szName[1],&pLastUnit->szUnitName[0],14);
			szName[15] = '\0';	
		}

		for (int j=0;j<m_pBindingUnits->Count();j++) 
		{		
			if ((pBindingUnit = m_pBindingUnits->Get(j)) != NULL) 
			{					
				if (pBindingUnit->GetInOutType() == UNIT_TYPE_INPUT) 
				{											
					// TM 20050624: Only change if unit type = UNIT_TYPE_CONTROL ...

					if (pBindingUnit->GetBindingUnitType() == BINDING_UNIT_TYPE_CONTROL) 
					{											
						if (TMGetDocumentNodes()->SetUnitName(pBindingUnit->GetNodeAddress(),
														      pBindingUnit->GetUnitAddress(),
															  szName) 
															  == FALSE) 
						{
							return FALSE;
						}	
					}

				} // if (pBindingUnit->GetType() == BINDING_UNIT_TYPE_CONTROL)

			} // if (pBindingUnit->GetInOutType() == UNIT_TYPE_INPUT) 			
		}
	} // for (int j=0;j<pBindingUnits->Count();j++) 			

	return TRUE;
}

// Bepalen van het aantal unieke node/unit addressen in de collectie.
int CBindingUnitsOperations::getCountUniqueUnitAddresses() {
	int retValue = 0;
	const int count = m_pBindingUnits->Count();
	for (int i = 0; i < count; ++i) {
		CBindingUnit* pBindingUnit = m_pBindingUnits->Get(i);
		if (NULL != pBindingUnit) {
			const int nodeAddress = pBindingUnit->GetNodeAddress();
			const int unitAddress = pBindingUnit->GetUnitAddress();
			if (1 == getCountUnitAddress(nodeAddress, unitAddress)) {
				retValue++;
			}
		}
	}	
	ASSERT(retValue <= count);
	return retValue;
}

// Aantal units in deze collectie met dit node en unit adres.
int CBindingUnitsOperations::getCountUnitAddress(int nodeAddress, int unitAddress) {
	int retValue = 0;
	CBindingUnits::Iterator it(m_pBindingUnits->CreateIterator());
	for (it.Begin(); !it.IsDone(); it.Next()) {
		CBindingUnit* const pBindingUnit = it.CurrentItem();
		if (NULL != pBindingUnit) {
			if ((pBindingUnit->GetNodeAddress() == nodeAddress) && (pBindingUnit->GetUnitAddress() == unitAddress)) {
				retValue++;
			}
		}
	}
	return retValue;
}
