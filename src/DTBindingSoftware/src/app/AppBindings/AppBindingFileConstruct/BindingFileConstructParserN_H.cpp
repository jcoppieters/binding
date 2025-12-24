// BindingFileConstructParserN_H.cpp: implementation of the CBindingFileConstructParserN_H class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructParserN_H.h"

#include "BindingUnitFactory.h"
#include "BindingLinksOperations.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryParserFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructParserN_H::CBindingFileConstructParserN_H(CStringArray* pStringArray) :
CBindingFileConstructParser(pStringArray)
{
	m_pBindingFileEntryParserN = 0;
}

CBindingFileConstructParserN_H::~CBindingFileConstructParserN_H()
{
	if (m_pBindingFileEntryParserN != 0)
	{
		delete m_pBindingFileEntryParserN;
		m_pBindingFileEntryParserN = 0;
	}
}

/**
 * 
 */
BOOL CBindingFileConstructParserN_H::Construct()
{
	ASSERT(m_pArrayBindingStrings);
	ASSERT(m_pBindingFileEntryParserN == 0);

	m_BindingEntry.Clear();
	CBindingUnits* const pBindingUnits = m_BindingEntry.GetColUnit();
	
	ASSERT(pBindingUnits);

	if (m_pBindingFileEntryParserN != 0)
	{
		delete m_pBindingFileEntryParserN;
		m_pBindingFileEntryParserN = 0;
	}
	
	/**
	 * Construct BindingUnits ...
	 */
	if (1)
	{	
		const int nMaxEntries = m_pArrayBindingStrings->GetSize();	
		const CBindingUnit* pLastInSequence = NULL;	
		/**	
		 * Find last in sequence ...
		 */
		const int indexNBinding = GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeN);
		if (indexNBinding != -1)
		{								
			if ((m_pBindingFileEntryParserN = CBindingFileEntryParserFactory::Get_TypeN(
					m_pArrayBindingStrings->GetAt(indexNBinding))) != 0)
			{
				ASSERT(m_pBindingFileEntryParserN->IsTypeN() == TRUE);	
				pLastInSequence = m_pBindingFileEntryParserN->GetForwardUnit();	
			}
			else
			{
				return FALSE;
			}												
		}

		/**
		 * Find all Next units ...
		 */
		if (pLastInSequence != NULL)
		{
			#ifdef _DEBUG
			if(1)
			{
				CString s;
				s.Format("FOUND pLastInSequence ID:%d,NodeAddress:0x%02x,UnitAddress:0x%02x,bIOType:%d",
						 pLastInSequence->GetID(),
						 pLastInSequence->GetNodeAddress(),
						 pLastInSequence->GetUnitAddress(),
						 pLastInSequence->GetInOutType());									
					TRACE("\n%s",s);
			}
			#endif


			for (int i=0;i<nMaxEntries;i++)
			{
				if (i != indexNBinding)
				{	
					/**
					 * Use pBindingFileEntryTypeNN as auto pointer !!!
					 */
					CBindingFileEntryParserFactory BindingFileEntryParserFactory(
						m_pArrayBindingStrings->GetAt(i));
					
					CBindingFileEntryParserNN*const pBindingFileEntryTypeNN =
						BindingFileEntryParserFactory.Get_TypeNN();

					if (pBindingFileEntryTypeNN != 0)
					{
						if (pBindingFileEntryTypeNN->IsTypeNN() == TRUE)
						{
							const CBindingUnit* const pNextUnit = 
								pBindingFileEntryTypeNN->GetForwardUnit();

							if (pNextUnit != NULL)
							{
								/**
								 * Base Class CBindingUnit --> Base class CBindingUnit.
								 */	
								ASSERT(pNextUnit->GetBindingUnitType() == 0);

								CBindingUnit* const pNew = 
									CBindingUnitFactory::Clone(const_cast<CBindingUnit*>(pNextUnit));	

								if (pNew != NULL)
								{
									ASSERT(pNew->GetBindingUnitType() == 0);
									
									pNew->SetID(0);	// !!!!!!!
									pNew->SetInOutType(UNIT_TYPE_OUTPUT);

									const unsigned short nID = pBindingUnits->Add(pNew);
									ASSERT(nID);
									ASSERT(nID == pNew->GetID());
									if (nID == 0)
									{															
										return FALSE;
									}
																			
									#ifdef _DEBUG
									if(1)
									{
										CString s;
										s.Format("Add pNextUnit ID:%d,NodeAddress:0x%02x,UnitAddress:0x%02x,bIOType:%d",
												 pNew->GetID(),
												 pNew->GetNodeAddress(),
												pNew->GetUnitAddress(),
												pNew->GetInOutType());	
								
										TRACE("\n%s",s);
									}
									#endif
								}
							}
						}				
					}		
				}
			}
			if (1)
			{
				/**
				* Add pLastInSequence to the collection ...
				*/			
				ASSERT( pLastInSequence->GetBindingUnitType() == 0);

				CBindingUnit* const pNew = 
					CBindingUnitFactory::Clone(const_cast<CBindingUnit*>(pLastInSequence));

				if (pNew != NULL)
				{
					ASSERT(pNew->GetBindingUnitType()  == 0);

					pNew->SetID(0);	// !!!!
					pNew->SetInOutType(UNIT_TYPE_OUTPUT);

					const unsigned short nID = pBindingUnits->Add(pNew);

					ASSERT(nID != 0);
					ASSERT(nID == pNew->GetID());

					if (nID == 0)
					{						
						return FALSE;
					}
				

					#ifdef _DEBUG
					if(1)
					{
						CString s;
						s.Format("Add pLastInSequence ID:%d,NodeAddress:0x%02x,UnitAddress:0x%02x,bIOType:%d",
								 pNew->GetID(),
								 pNew->GetNodeAddress(),
								 pNew->GetUnitAddress(),
								 pNew->GetInOutType());		
				
						TRACE("\n%s",s);
					}
					#endif
				}
			}
		}
		else
		{
			return FALSE;
		}
	}


	#ifdef _DEBUG
	if(1)	// All units must be type io Output !!!
	{
		const int nMax = pBindingUnits->Count();

		CString t;
		t.Format("CBindingEntry 'pBindingEntryNext' holds:%d BindingUnits",nMax);
		TRACE1("\n%s",t);

		for (int i=0;i<nMax;i++)
		{
			CBindingUnit *const pBindingUnit = pBindingUnits->Get(i);

			ASSERT(pBindingUnit);

			if (pBindingUnit != NULL)
			{
				CString s;
				s.Format("BindingUnit ID:%d,NodeAddress:0x%02x,UnitAddress:0x%02x,bIOType:%d",
						 pBindingUnit->GetID(),
						 pBindingUnit->GetNodeAddress(),
						 pBindingUnit->GetUnitAddress(),
						 pBindingUnit->GetInOutType());
					
				TRACE("\n%s",s);
			}
		}
	}
	#endif

	CBindingUnit* const pFirstInSequence = pBindingUnits->Get(0);
	ASSERT(pFirstInSequence);

	/** 
	 * Add Bindinglinks to Collection ...
	 */
	if (1)
	{
		const int nMaxUnits = pBindingUnits->Count();	

		CBindingLinks* const pBindingLinks = m_BindingEntry.GetColLink();
		ASSERT(pBindingLinks);

		CBindingLinksOperations BindingLinksOperations(pBindingLinks);

		if (pFirstInSequence != NULL)
		{		
			CBindingUnit* pForwardUnit = pFirstInSequence;

			for (int i=1;i<nMaxUnits;i++)
			{
				CBindingUnit* const pNextUnit = pBindingUnits->Get(i);
				ASSERT(pNextUnit);
				

				if (pNextUnit != NULL)
				{					
					if (BindingLinksOperations.Add(pForwardUnit,pNextUnit) == FALSE)
					{
						ASSERT(0);
						return FALSE;
					}

					pForwardUnit = pNextUnit;
				}
				else
				{
					ASSERT(0);
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

	/**
	 * Generate Evaluation String ...
	 */
	if (1)
	{
		CBindingEvaluationString* const pBindingEvaluationString = 
			m_BindingEntry.GetEvaluationString();

		ASSERT(pBindingEvaluationString);

		const int nMaxUnits = pBindingUnits->Count();

		if (pFirstInSequence != NULL)
		{		
			pBindingEvaluationString->AddUnit(pFirstInSequence->GetID());		

			for (int i=1;i<nMaxUnits;i++)
			{
				CBindingUnit* const pNextUnit = pBindingUnits->Get(i);
				ASSERT(pNextUnit);

				if (pNextUnit != NULL)
				{
					pBindingEvaluationString->Add('>');
					pBindingEvaluationString->AddUnit(pNextUnit->GetID());
				}
				else
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
