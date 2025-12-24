// BindingFileEntryFormatterG.cpp: implementation of the CBindingFileEntryFormatterG class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryFormatterG.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryFormatterG::CBindingFileEntryFormatterG(CBindingEntryInfo* pBindingEntryInfo,
														 CBindingUnit* pForwardUnit,
														 CBindingUnit* pReturnUnit) :
CBindingFileEntryFormatter(pBindingEntryInfo),
m_pForwardUnit(pForwardUnit),
m_pReturnUnit(pReturnUnit)
{
	ASSERT(m_pForwardUnit);
	ASSERT(m_pReturnUnit);
}

CBindingFileEntryFormatterG::~CBindingFileEntryFormatterG()
{

}

BOOL CBindingFileEntryFormatterG::Format()
{
	ASSERT(m_pBindingEntry);
	ASSERT(m_pForwardUnit);
	ASSERT(m_pReturnUnit);

	m_szBindingFileEntry.Empty();

	if (m_pBindingEntry != NULL && 
		m_pForwardUnit != NULL && 
		m_pReturnUnit != NULL)
	{

		const BYTE bBindingFlags = m_pBindingEntry->GetBindingReturnFlags();

		ASSERT(bBindingFlags <= BINDINGFLAGS_STATUS_PREPULS);

		m_szBindingFileEntry.Format(_T("0000%02X_%02X_%04X_GA0000%02XU%02XE%02X=U%02XF%02XD"),
								  m_pForwardUnit->GetNodeAddress(),
								  bBindingFlags,
								  m_pBindingEntry->GetBindingNr(),
								  m_pReturnUnit->GetNodeAddress(),
								  m_pReturnUnit->GetUnitAddress(),
								  m_pReturnUnit->BindingData.Get_MessageCode(),
								  m_pForwardUnit->GetUnitAddress(),
								  m_pForwardUnit->BindingData.Get_MessageCode());

		const int nLen = m_pForwardUnit->BindingData.Get_Length();


		CString szBindingFunctionData;
		CString szTemp;

		szBindingFunctionData.Empty();
		szBindingFunctionData.Format(_T("%02x"),nLen);
	
		for (int i=0;i<nLen;i++) 
		{
			szTemp.Format(_T("%02X"),m_pForwardUnit->BindingData.Get_Data(i));
			szBindingFunctionData += szTemp;
		}
	
		m_szBindingFileEntry += szBindingFunctionData + 'S';

		return TRUE;	
	}
	return FALSE;
}

