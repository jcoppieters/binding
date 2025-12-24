// BindingConversionToGui2.cpp: implementation of the CBindingConversionToGui2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingConversionToGui2.h"

#include "BindingLink.h"
#include "BindingLinks.h"
#include "BindingUnits.h"
//#include "BindingUnit.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"
#include "BindingOperators.h"
#include "BindingOperator.h"
#include "BindingEntry.h"
#include "BindingGuiComponent.h"
#include "BindingConvertHelper.h"

#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringSeperator.h"
#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringSeperatorAlg1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingConversionToGui2::CBindingConversionToGui2(CBindingEntry* pBindingEntry) :
m_pBindingEntry(pBindingEntry)
{
	m_StringArray.RemoveAll();
}

CBindingConversionToGui2::~CBindingConversionToGui2()
{	
	m_StringArray.RemoveAll();
}

/**
 *
 */
BOOL CBindingConversionToGui2::Process()
{
	if (m_pBindingEntry == 0) {
		ASSERT(m_pBindingEntry);
		return FALSE;
	}

	if (BuildArrayLinkInfo() == FALSE) {
		ASSERT(0);
		return FALSE;
	}

	ASSERT(m_StringArray.GetSize() > 0);
	
	if (BindingInputUnits() == FALSE) {
		ASSERT(0);
		return FALSE;
	}

	if (BindingOperators() == FALSE) {
		ASSERT(0);
		return FALSE;
	}

	if (BindingOutputUnits() == FALSE) {
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

BOOL CBindingConversionToGui2::BuildArrayLinkInfo()
{
	CBindingEvaluationStringSeperatorAlg1 Algoritme(m_pBindingEntry);
	CBindingEvaluationStringSeperator StringSeperator(m_pBindingEntry,
													  m_StringArray,
													  &Algoritme);

	if (StringSeperator.Build() == FALSE)
	{
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

BOOL CBindingConversionToGui2::BindingInputUnits()
{
	const int nSize = m_StringArray.GetSize();
	CBindingUnits* const pBindingUnits = m_pBindingEntry->GetColUnit();
	if (pBindingUnits == 0)	
		return FALSE;
	
	int nCntInputs = 0;

	m_xCurrentPos = 0;
	m_yCurrentPos = 0;

	// Position inputs
	for (int i=0;i<nSize;i++)
	{
		CString szData = m_StringArray.GetAt(i);		
		const int nInputs = Convert.ParseNInputs(szData);

		for (int j=0;j<nInputs;j++)
		{
			char c = '\0';
			unsigned short nId = 0;

			if (Convert.ParseInputs(szData,&c,&nId,j) == FALSE) 
			{
				break;
			}

			if (c=='U')
			{
				CBindingUnit* const pBindingUnit = pBindingUnits->Find(nId);
				if (pBindingUnit == NULL) {
					return FALSE;			
				}
				
				m_xCurrentPos = 200;
				m_yCurrentPos = (float) ((nCntInputs*1000) + ((nCntInputs+1)* 200));				
				
				pBindingUnit->SetPosition(m_xCurrentPos,m_yCurrentPos);
				nCntInputs++;					//new
			}
		}
	}
	return TRUE;
}

BOOL CBindingConversionToGui2::BindingOperators()
{
	const int nSize = m_StringArray.GetSize();
	CBindingOperators* const pBindingOperators = m_pBindingEntry->GetColOperator();
	if (pBindingOperators == 0)	
		return FALSE;
	
	int nCntOperators = 0;

	m_xCurrentPos = 0;
	m_yCurrentPos = 0;

	for (int i=0;i<nSize;i++)
	{
		CString szData = m_StringArray.GetAt(i);	
		const int nInputs = Convert.ParseNInputs(szData);
		int k = 0;

		for (int j=0;j<nInputs;j++)
		{
			char c ='\0';
			unsigned short nId = 0;

			if (Convert.ParseInputs(szData,&c,&nId,j) == FALSE) 
			{
				break;
			}

			if ((c!='U') && (c!= 'T'))
			{
				CBindingOperator* const pBindingOperator = pBindingOperators->Find(nId);
				if (pBindingOperator == 0) {
					return FALSE;
				}
				
				m_yCurrentPos = (float) (440 + (k * 600));
				m_xCurrentPos = (float) (2000 + ((nCntOperators-k) * 1000));
				
				pBindingOperator->SetPosition(m_xCurrentPos,m_yCurrentPos);
				
				nCntOperators++;
				k++;	// k holds nr operators/Level
			}		
		}
	}

	return TRUE;
}

BOOL CBindingConversionToGui2::BindingOutputUnits()
{
	CBindingUnits* const pBindingUnits = m_pBindingEntry->GetColUnit();
	if (pBindingUnits == 0)	
		return FALSE;

	m_yCurrentPos = 200;

	const int nSize = m_StringArray.GetSize();
	int nIndex = 0;
	if (nSize > 1) nIndex = nSize - 1;

	CString szData = m_StringArray.GetAt(nIndex);

	for (int j=0;j<Convert.ParseNOutputs(szData);j++)
	{
		char c ='\0';
		unsigned short nId = 0;

		if (Convert.ParseOutputs(szData,&c,&nId,j) == FALSE) 
		{
			return FALSE;
		}

		if (c!='U') 
		{
			return FALSE;
		}

		CBindingUnit* const pBindingUnit = pBindingUnits->Find(nId);
		if (pBindingUnit == 0) {
			return FALSE;			
		}
				
		m_xCurrentPos += 1500;
		m_yCurrentPos = 200;

		pBindingUnit->SetPosition(m_xCurrentPos,m_yCurrentPos);	
	}
	return TRUE;
}
