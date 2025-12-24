// BindingFileEntryFormatterI.cpp: implementation of the CBindingFileEntryFormatterI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryFormatterI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFileEntryFormatterI::CBindingFileEntryFormatterI(CBindingEntryInfo* pBindingEntryInfo,
														 CBindingUnit* pForwardUnit,
														 CBindingUnit* pReturnUnit) :
CBindingFileEntryFormatter(pBindingEntryInfo),
m_pForwardUnit(pForwardUnit),
m_pReturnUnit(pReturnUnit)
{
	ASSERT( m_pForwardUnit );
	ASSERT( m_pReturnUnit );
}

CBindingFileEntryFormatterI::~CBindingFileEntryFormatterI()
{

}

BOOL CBindingFileEntryFormatterI::Format()
{
	ASSERT(m_pBindingEntry);
	ASSERT(m_pForwardUnit);
	ASSERT(m_pReturnUnit);

	m_szBindingFileEntry.Empty();

	if (m_pBindingEntry != NULL && 
		m_pForwardUnit != NULL && 
		m_pReturnUnit != NULL &&
		m_pBindingEntryInfo != NULL)
	{
		BYTE bBindingFlags = m_pReturnUnit->BindingData.Get_BindingFlags();

		if (m_pBindingEntryInfo->Is_Inverted()) 
		{
			bBindingFlags &= 0x7F;
		}
		else 
		{
			bBindingFlags |= 0x80;
		}
	
		m_szBindingFileEntry.Format(_T("0000%02X_%02X_%04X_IU%02XE%02X>A0000%02XU%02XF%02XD"),
								  m_pReturnUnit->GetNodeAddress(),
								  bBindingFlags,
								  m_pBindingEntry->GetBindingNr(),
								  m_pReturnUnit->GetUnitAddress(),
								  m_pReturnUnit->BindingData.Get_MessageCode(),
								  m_pForwardUnit->GetNodeAddress(),
								  m_pForwardUnit->GetUnitAddress(),
								  m_pForwardUnit->BindingData.Get_MessageCode());

		CString szTemp;
		CString szBindingFunctionData;

		const int nLen = m_pForwardUnit->BindingData.Get_Length();
		szBindingFunctionData.Format(_T("%02x"),nLen);
	
		for (int i=0;i<nLen;i++) 
		{
			szTemp.Format(_T("%02X"),m_pForwardUnit->BindingData.Get_Data(i));
			szBindingFunctionData += szTemp;
		}
	
		m_szBindingFileEntry += szBindingFunctionData + 'S';
		return TRUE;
		
	}
	
	ASSERT(0);
	return FALSE;
}
