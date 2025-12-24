// BindingFileConstructParserP.cpp: implementation of the CBindingFileConstructParserP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileConstructParserP.h"

#include "BindingUnits.h"
#include "BindingUnitFactory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructParserP::CBindingFileConstructParserP(CStringArray* pArrayBindingStrings) :
CBindingFileConstruct( pArrayBindingStrings ),
m_pBindingUnits ( new CBindingUnits() )
{
	ASSERT( m_pBindingUnits );

	m_pBindingUnits->RemoveAll();
}

CBindingFileConstructParserP::~CBindingFileConstructParserP()
{
	delete m_pBindingUnits;
}	


/**
 *
 */
BOOL CBindingFileConstructParserP::Construct()
{
	ASSERT( m_pBindingUnits );
	ASSERT( m_pArrayBindingStrings );
	
	BOOL fResult = FALSE;
	const int nMaxEntries = m_pArrayBindingStrings->GetSize();

	for (int i=0;i<nMaxEntries;i++)
	{
		CBindingFileEntry BindingFileEntry(m_pArrayBindingStrings->GetAt(i));
		CBindingFileEntryParser* const pBindingFileEntryParser =
			BindingFileEntry.Parse();
		
		if (pBindingFileEntryParser == 0) {
			ASSERT( 0 );
			return FALSE;
		}

		if (pBindingFileEntryParser->IsTypeP() == FALSE) {
			ASSERT( 0 );
			return FALSE;
		}

		ASSERT(pBindingFileEntryParser->GetForwardUnit()->GetBindingUnitType() == 0);

		CBindingUnit* const pBindingUnit = CBindingUnitFactory::Clone( 
			const_cast<CBindingUnit*>(pBindingFileEntryParser->GetForwardUnit()));

		if (pBindingUnit == 0) {
			ASSERT( 0 );
			return FALSE;
		}

		ASSERT(pBindingUnit->GetBindingUnitType() == 0);

		pBindingUnit->SetID(0);
		const unsigned short nID = m_pBindingUnits->Add( pBindingUnit );
		if (nID == 0)
		{
			ASSERT(0);
			return FALSE;
		}

		ASSERT( nID == pBindingUnit->GetID() );
		
		fResult = TRUE;

	} // for (int i=0;i<nMaxEntries;i++)

	return fResult;
}