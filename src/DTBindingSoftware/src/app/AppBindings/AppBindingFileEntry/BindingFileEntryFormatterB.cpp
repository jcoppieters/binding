// BindingFileEntryFormatterB.cpp: implementation of the CBindingFileEntryFormatterB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryFormatterB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryFormatterB::CBindingFileEntryFormatterB(CBindingEntryInfo* pBindingEntryInfo,
														 CBindingUnit* pForwardUnit,
														 CBindingUnit* pReturnUnit) :
CBindingFileEntryFormatter(pBindingEntryInfo),
m_pForwardUnit(pForwardUnit),
m_pReturnUnit(pReturnUnit)
{
	ASSERT(m_pForwardUnit);
	ASSERT(m_pReturnUnit);
}

CBindingFileEntryFormatterB::~CBindingFileEntryFormatterB()
{

}


BOOL CBindingFileEntryFormatterB::Format()
{
	ASSERT(m_pBindingEntry);
	ASSERT(m_pForwardUnit);
	ASSERT(m_pReturnUnit);

	m_szBindingFileEntry.Empty();

	if (m_pBindingEntry != NULL && 
		m_pForwardUnit != NULL && 
		m_pReturnUnit != NULL)
	{
	
		const BYTE bBindingFlags = m_pReturnUnit->BindingData.Get_BindingFlags();

		m_szBindingFileEntry.Format(_T("0000%02X_%02X_%04X_BU%02XE%02X>A0000%02XU%02XS"),
								  m_pReturnUnit->GetNodeAddress(),
								  bBindingFlags,
								  m_pBindingEntry->GetBindingNr(),
								  m_pReturnUnit->GetUnitAddress(),
								  m_pReturnUnit->BindingData.Get_MessageCode(),
								  m_pForwardUnit->GetNodeAddress(),
								  m_pForwardUnit->GetUnitAddress());
		return TRUE;	
	}
	return FALSE;
}