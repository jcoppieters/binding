// BindingFileEntryOperationsVerify.cpp: implementation of the CBindingFileEntryOperationsVerify class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileEntryOperationsVerify.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnitOperations.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"

#include "BindingFileEntry.h"
#include "Nodemess.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFileEntryOperationsVerify::CBindingFileEntryOperationsVerify(CString s) :
CBindingFileEntryOperations( s )
{

}

CBindingFileEntryOperationsVerify::~CBindingFileEntryOperationsVerify()
{

}


BOOL CBindingFileEntryOperationsVerify::Verify()
{
	CBindingFileEntry BindingFileEntry( m_szBindingFileEntry );
	CBindingFileEntryParser* const pParser = BindingFileEntry.Parse();
	
	if (pParser == 0) {
		ASSERT( 0 );
		return FALSE;
	}

	if (pParser->IsTypeP() == TRUE) 
	{
		CBindingFileEntryOperationsVerifyP BindingFileEntryVerify( pParser );
		return ( BindingFileEntryVerify.Verify() );	
	}

	// Todo : for other Binding types ...

	return TRUE;
}

BOOL CBindingFileEntryOperationsVerify::Correct(CString& szOut)
{
	CBindingFileEntry BindingFileEntry( m_szBindingFileEntry );
	CBindingFileEntryParser* const pParser = BindingFileEntry.Parse();
	
	if (pParser == 0) {
		ASSERT( 0 );
		return FALSE;
	}

	if (pParser->IsTypeP() == TRUE) 
	{
		CBindingFileEntryOperationsVerifyP BindingFileEntryVerify( m_szBindingFileEntry, pParser );
		return ( BindingFileEntryVerify.Correct( szOut ));
	}

	// Todo : for other Binding types ...

	return TRUE;
}

/**
 * BOOL CBindingFileEntryOperationsVerify::IsValid()
 */
BOOL CBindingFileEntryOperationsVerify::IsValid()
{
	CBindingFileEntry BindingFileEntry( m_szBindingFileEntry );
	CBindingFileEntryParser* const pParser = BindingFileEntry.Parse();
	
	if (pParser == 0) {
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////
CBindingFileEntryOperationsVerifyP::CBindingFileEntryOperationsVerifyP(CString s,CBindingFileEntryParser* const pParser) :
CBindingFileEntryOperationsVerify( s ),
m_pParser( static_cast<CBindingFileEntryParserP*> (pParser) )
{
	ASSERT( m_pParser );
	m_pBindingPropertyUnit = 0;
}

CBindingFileEntryOperationsVerifyP::CBindingFileEntryOperationsVerifyP(CBindingFileEntryParser* const pParser) :
m_pParser( static_cast<CBindingFileEntryParserP*> (pParser) )
{
	ASSERT( m_pParser );
	m_pBindingPropertyUnit = 0;
}

CBindingFileEntryOperationsVerifyP::~CBindingFileEntryOperationsVerifyP()
{

}

BOOL CBindingFileEntryOperationsVerifyP::FindPropertyUnit()
{
	ASSERT( m_pParser );

	const CBindingUnit* const pBindingUnit = m_pParser->GetForwardUnit();
	if (pBindingUnit != 0)
	{
		CBindingPropertyUnitOperations PropertyUnitOperations;

		m_pBindingPropertyUnit = PropertyUnitOperations.Find( pBindingUnit->GetNodeAddress(),
															  pBindingUnit->GetUnitAddress() );

		if (m_pBindingPropertyUnit != 0) {
			return TRUE;
		}


	}
	return FALSE;
}

/**
 * BOOL CBindingFileEntryOperationsVerifyP::Verify()
 *
 * Check if FC_UNITSENSSET is correct (Bug...)
 * Check if BindingNr of P-Binding is 0x0000 or manual binding.
 *
 */
BOOL CBindingFileEntryOperationsVerifyP::Verify()
{
	if (FindPropertyUnit() == TRUE)
	{
		if (m_pParser->GetBindingNr() > 0x0000 && m_pParser->GetBindingNr() < 0x9000)
		{
			return FALSE;
		}

		switch (m_pBindingPropertyUnit->GetBindingUnitType())
		{
			case BINDING_UNIT_TYPE_SENS:
			case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
			{
				const CBindingUnit* const pBindingUnit = m_pParser->GetForwardUnit();
				if (pBindingUnit != 0)
				{			
					if (pBindingUnit->BindingData.Get_MessageCode() != FC_UNITSENSSET) 
					{
						return FALSE;
					}				
					return TRUE;
				}
			} break;

			default:
			{
				return TRUE;

			} break;		
		}	// End switch ...			
	}
	return FALSE;
}

/**
 * BOOL CBindingFileEntryOperationsVerifyP::Correct(CString& szOut)
 *
 * Only invoked if verify fails !!!!
 *
 * 1) Correct FC_UNITSENSSET.
 * 2) Correct BindingNr.
 *
 */
BOOL CBindingFileEntryOperationsVerifyP::Correct(CString& szOut)
{
	if (FindPropertyUnit() == TRUE)
	{
		switch (m_pBindingPropertyUnit->GetBindingUnitType())
		{
			case BINDING_UNIT_TYPE_SENS:
			case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
			{
				BOOL fResult;

				const CBindingUnit* const pBindingUnit = m_pParser->GetForwardUnit();
				if (pBindingUnit != 0)
				{			
					if (pBindingUnit->BindingData.Get_MessageCode() != FC_UNITSENSSET) 
					{
						fResult = ReplaceMessageCode(FC_UNITSENSSET, szOut);
						m_szBindingFileEntry = szOut;
					}

					if (m_pParser->GetBindingNr() > 0x0000 && m_pParser->GetBindingNr() < 0x9000)
					{
						const unsigned short usNewBindingNr = 0x0000;
						fResult = ReplaceBindingNr(usNewBindingNr,szOut);
					}
					
					return fResult;
				}
			} break;
		}
	}
	return FALSE;
}