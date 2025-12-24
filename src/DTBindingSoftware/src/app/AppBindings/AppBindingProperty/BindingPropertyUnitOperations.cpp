// BindingPropertyUnitOperations.cpp: implementation of the CBindingPropertyUnitOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingPropertyUnitOperations.h"

#include "BindingPropertyUnit.h"
//#include "BindingUnit.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"
#include "BindingPropertyUnitConvert.h"

// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingPropertyUnitOperations::CBindingPropertyUnitOperations(CBindingPropertyUnit* const p) :
m_pBindingPropertyUnit( p )
{
}
CBindingPropertyUnitOperations::~CBindingPropertyUnitOperations()
{
}

CBindingPropertyUnit* CBindingPropertyUnitOperations::Find(BYTE bNodeAddress,BYTE bUnitAddress) {
	STRUCT_UNIT* pUnit = 0;
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());	
	if (NodeDatabaseOperations.Find(bNodeAddress,bUnitAddress,&pUnit)) {
		ASSERT( pUnit );
		return (pUnit->m_pBindingUnit);
	}
	return 0;
}

BOOL CBindingPropertyUnitOperations::Update(const CBindingUnit* const pBindingUnit)
{
	if (pBindingUnit == 0) {	
		ASSERT( pBindingUnit );
		return FALSE;
	}

	BOOL fResult = FALSE;

	CBindingPropertyUnit* const pBindingPropertyUnit = Find(pBindingUnit->GetNodeAddress(),pBindingUnit->GetUnitAddress());
	if (pBindingPropertyUnit == 0) {
		return FALSE;
	}

	CBindingPropertyUnitConvert* pConverter =  CBindingPropertyUnitConvert::Create(pBindingUnit,pBindingPropertyUnit);	
	if (pConverter != 0) {
		fResult = pConverter->Update();	
		delete pConverter;	
		pConverter = 0;
	}
	
	return fResult;
}

BOOL CBindingPropertyUnitOperations::Clear()
{	
	int nNodes = 0;
	if (TMGetDocumentNodes()->NROfNodes(&nNodes) == FALSE) {
		ASSERT( 0 );
		return FALSE;		
	}
		
	for (int i=0;i<nNodes;i++) {
		CCanNode* pNode = 0;
		if (TMGetDocumentNodes()->GetNode(i,&pNode)) {
			for (int j=0;j<pNode->bNROfUnits;j++) {
				STRUCT_UNIT* pUnit = 0;
				if (TMGetDocumentNodes()->GetUnit(pNode,j,&pUnit)) {
					if (pUnit->m_pBindingUnit != 0) {
						pUnit->m_pBindingUnit->SetToDefaults();
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL CBindingPropertyUnitOperations::Export(FILE* fp)
{
	if (fp == 0) {
		return FALSE;
	}

	STRUCT_UNIT* pUnit = 0;
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());

	if (NodeDatabaseOperations.Find(m_pBindingPropertyUnit->GetNodeAddress(),
									m_pBindingPropertyUnit->GetUnitAddress(),
									&pUnit))
	{
		CString szMessage;
		szMessage.Format("\nName: '%s',Address: 0x%02x,0x%02x",
						 pUnit->szUnitName,
						 pUnit->bNodeLogicalAddress,
						 pUnit->bUnitAddress);
		
		fputs((LPCTSTR)szMessage,fp);			// CHANGES_0X0A01_USE_FPUTS
		return TRUE;
	}
	return FALSE;
}




