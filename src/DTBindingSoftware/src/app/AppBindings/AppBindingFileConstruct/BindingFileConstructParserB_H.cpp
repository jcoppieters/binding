// BindingFileConstructParserB_H.cpp: implementation of the CBindingFileConstructParserB_H class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructParserB_H.h"

#include "BindingUnitFactory.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryParserFactory.h"

#include "DebugMacros.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFileConstructParserB_H::CBindingFileConstructParserB_H(CStringArray* pStringArray) :
CBindingFileConstructParser(pStringArray)
{
	m_BindingUnits.RemoveAll();
	m_pForwardUnit = NULL;
}

CBindingFileConstructParserB_H::~CBindingFileConstructParserB_H()
{
	m_BindingUnits.RemoveAll();

	if (m_pForwardUnit != NULL)
	{
		delete m_pForwardUnit;
		m_pForwardUnit = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
const CBindingUnits* CBindingFileConstructParserB_H::GetReturnUnits() const
{
	return (&m_BindingUnits);	
}

const CBindingUnit* CBindingFileConstructParserB_H::GetForwardUnit() const
{
	return m_pForwardUnit;
}

//////////////////////////////////////////////////////////////////////

BOOL CBindingFileConstructParserB_H::GetReturnUnit(CBindingUnit* pReturnUnit)
{
	const int indexBBinding = GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeB);
	if (indexBBinding != -1)
	{
		CBindingFileEntryParserFactory BindingFileEntryParser(
			m_pArrayBindingStrings->GetAt(indexBBinding));

		CBindingFileEntryParserB* const pBindingFileEntryTypeB = 
			BindingFileEntryParser.Get_TypeB();

		if (pBindingFileEntryTypeB != 0)
		{
			CBindingUnit* const pUnit = pBindingFileEntryTypeB->GetReturnUnit();
			if (pUnit != 0)
			{
				*pReturnUnit = *pUnit;
				return TRUE;
			}			
		}
	}
	return FALSE;	
}


/**
 * Fills CBindingUnits::m_BindingUnits with pReturnUnits.
 * Fills m_pForwardUnit with pForwardUnit.
 * !!! Doesn't verify pForwardUnit !!!!
 *
 */
BOOL CBindingFileConstructParserB_H::Construct()
{
	#ifdef _DEBUG_BINDINGFILE_CONSTRUCT_PARSER
	if(1)
	{
		for (int i=0;i<m_pArrayBindingStrings->GetSize();i++)
		{
			CString s;
			s.Format("index:%d - %s",i,m_pArrayBindingStrings->GetAt(i));
			TRACE1("\n%s",s);
		}		
	}
	#endif

	BOOL fResult = FALSE;	
	int indexBBinding = 0;
	int nCntFoundBBinding = 0;
	
	do
	{	
		indexBBinding = GetBindingTypeEntry(&CBindingFileEntryParser::IsTypeB,
											nCntFoundBBinding);

		if (indexBBinding != -1)
		{
			#ifdef _DEBUG_BINDINGFILE_CONSTRUCT_PARSER

			if(1)
			{
				CString s(m_pArrayBindingStrings->GetAt(indexBBinding));
				TRACE2("\n\n%d : %s",indexBBinding,s);
			}
			
			#endif

			fResult = FALSE;	// Reset ...
		
			CBindingFileEntryParserFactory BindingFileEntryParser(
				m_pArrayBindingStrings->GetAt(indexBBinding));

			CBindingFileEntryParserB* const pBindingFileEntryTypeB = 
				BindingFileEntryParser.Get_TypeB();

			if (pBindingFileEntryTypeB != 0)
			{					
				ASSERT(pBindingFileEntryTypeB->IsTypeB() == TRUE);
	
				const CBindingUnit* const pReturnUnit = pBindingFileEntryTypeB->GetReturnUnit();
			
				if (pReturnUnit != NULL)
				{
					#ifdef _DEBUG_BINDINGFILE_CONSTRUCT_PARSER

					if(1)
					{
						CString szDebug;
						szDebug.Format("ReturnUnit: 0x%02x,0x%02x",pReturnUnit->GetNodeAddress(),
																   pReturnUnit->GetUnitAddress());
						TRACE1("\n%s",szDebug);
					}

					#endif

					/**
					 * Base Class CBindingUnit --> Base class CBindingUnit.
					 */
					ASSERT(pReturnUnit->GetBindingUnitType() == 0);

					CBindingUnit* const pNew = 
						CBindingUnitFactory::Clone(const_cast<CBindingUnit*>(pReturnUnit));	
					
					if (pNew != NULL)
					{	
						ASSERT(pNew->GetBindingUnitType() == 0);

						pNew->SetID(0);					
						const unsigned short nID = m_BindingUnits.Add(pNew);						
						if (nID != 0) 
						{
							fResult = TRUE;
						}
						ASSERT(nID != 0);
						ASSERT(nID == pNew->GetID());
					}
					else
					{
						ASSERT(pNew);
						fResult = FALSE;
						break;	// Break do...while loop ...
					}
				}
						
				if (m_pForwardUnit == NULL)
				{
					const CBindingUnit* const pForwardUnit = 
						pBindingFileEntryTypeB->GetForwardUnit();	

					/**
					 * Base Class CBindingUnit --> Base class CBindingUnit.
					 */				
					ASSERT( pForwardUnit->GetBindingUnitType() == 0);

					if ((m_pForwardUnit = 
						CBindingUnitFactory::Clone(const_cast<CBindingUnit*>(pForwardUnit))) == 0)
					{					
						ASSERT( m_pForwardUnit->GetBindingUnitType() == 0);

						fResult = FALSE;
						ASSERT(m_pForwardUnit);						
						break;	// Break do...while loop ...
					}

					ASSERT( m_pForwardUnit->GetBindingUnitType() == 0);
				}	
							
				nCntFoundBBinding++;
			}	
		}
	} while(indexBBinding != -1);

	return fResult;
}





