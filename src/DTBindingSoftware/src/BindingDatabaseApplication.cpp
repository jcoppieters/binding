// BindingDatabaseApplication.cpp: implementation of the CBindingDatabaseApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingDatabaseApplication.h"

#include "BindingFilesOperations.h"
#include "BindingEntriesOperations.h"
#include "BindingDatabaseDirector.h"
#include "BindingFiles.h"
#include "BindingEntries.h"
#include "BindingUnitState.h"

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
CBindingDatabaseApplication* CBindingDatabaseApplication::GetInstance() 
{	
	static CBindingDatabaseApplication instance;
	return (&instance);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingDatabaseApplication::CBindingDatabaseApplication() :
m_pBindingEntries( new CBindingEntries() ),
m_pBindingFiles( new CBindingFiles() ),
m_pBindingDatabaseDirector( new CBindingDatabaseDirector( m_pBindingFiles,
														  m_pBindingEntries,
														  TMGetDocumentNodes()))
{		
	ASSERT(m_pBindingEntries);
	ASSERT(m_pBindingFiles);
	ASSERT(m_pBindingDatabaseDirector);
}

CBindingDatabaseApplication::~CBindingDatabaseApplication()
{
	if (m_pBindingEntries != 0)
	{
		delete m_pBindingEntries;		
	}

	if (m_pBindingFiles != 0)
	{
		delete m_pBindingFiles;	
	}

	if (m_pBindingDatabaseDirector != 0)
	{
		delete m_pBindingDatabaseDirector;
	}
}

void CBindingDatabaseApplication::Clear()
{	
	ASSERT(m_pBindingDatabaseDirector);
	
	if (m_pBindingDatabaseDirector != 0) 
	{
		m_pBindingDatabaseDirector->Clear();
	}

	CBindingUnitState::Instance()->Reset();
}

BOOL CBindingDatabaseApplication::Delete(CBindingEntry *pEntry)
{
	ASSERT(m_pBindingDatabaseDirector);
	
	if (m_pBindingDatabaseDirector != 0) 
	{
		return (m_pBindingDatabaseDirector->Delete(pEntry));
	}
	return FALSE;
}

CBindingEntry* CBindingDatabaseApplication::Update(unsigned short nID)
{
	ASSERT(m_pBindingDatabaseDirector);
	
	if (m_pBindingDatabaseDirector != 0) 
	{
		return (m_pBindingDatabaseDirector->Update(nID));
	}
	return 0;
}

void CBindingDatabaseApplication::SetAllSend() {
	CBindingEntriesOperations BindingEntriesOperations(m_pBindingEntries);
	BindingEntriesOperations.SetAllSend();
}

void CBindingDatabaseApplication::SetAllErased() {
	CBindingEntriesOperations BindingEntriesOperations(m_pBindingEntries);
	BindingEntriesOperations.SetAllErased();
}

void CBindingDatabaseApplication::SetAllBindingsSendFailed(BYTE bNodeAddress)
{
	CBindingEntriesOperations BindingEntriesOperations(m_pBindingEntries);
	BindingEntriesOperations.SetSendFailed(bNodeAddress);
}


BOOL CBindingDatabaseApplication::ModifiedBindingFiles()
{
	CBindingFilesOperations BindingFilesOperations(m_pBindingFiles);

	return (BindingFilesOperations.CheckModified());
}

unsigned short CBindingDatabaseApplication::Delete(BYTE bNodeAddress)
{
	ASSERT(m_pBindingDatabaseDirector);
	
	if (m_pBindingDatabaseDirector != 0) 
	{
		return (m_pBindingDatabaseDirector->Delete(bNodeAddress));
	}

	return 0;
}

unsigned short CBindingDatabaseApplication::Delete(
	BYTE bNodeAddress,
	BYTE bUnitAddress)
{
	ASSERT(m_pBindingDatabaseDirector);
	
	unsigned short result = 0;
	if (m_pBindingDatabaseDirector != 0) 
	{	
		result = m_pBindingDatabaseDirector->Delete(
					bNodeAddress,
					bUnitAddress);
	}
	return result;
}

void CBindingDatabaseApplication::CreateBindingFiles()
{
	ASSERT(m_pBindingDatabaseDirector);
	
	if (m_pBindingDatabaseDirector != 0) 
	{
		const BOOL fResult = m_pBindingDatabaseDirector->CreateNewFiles();
		ASSERT( fResult );
	}
}


unsigned short CBindingDatabaseApplication::Add(CBindingEntry *pEntry,BindingStatusError::BindingError_t* errorStatus)
{		
	ASSERT(m_pBindingDatabaseDirector);
	if (m_pBindingDatabaseDirector != 0) {
		return (m_pBindingDatabaseDirector->Add(pEntry,errorStatus));
	}
	return 0;
}

void CBindingDatabaseApplication::AssignBindingFilesToNodes(CNodeDatabase *pDatabase)
{
	ASSERT(m_pBindingDatabaseDirector);
	
	if (m_pBindingDatabaseDirector != 0) 
	{
		const BOOL fResult = m_pBindingDatabaseDirector->AssignBindingFiles(pDatabase);
		ASSERT( fResult );
	}
}

void CBindingDatabaseApplication::UpdateData(BOOL fSaveToBindingClass)
{
	ASSERT(m_pBindingDatabaseDirector);
	
	if (m_pBindingDatabaseDirector != 0) 
	{
		m_pBindingDatabaseDirector->UpdateModifiedFlags(fSaveToBindingClass);
	}
}

void CBindingDatabaseApplication::Load(CStringArray* pStrArrayError)
{
	ASSERT(m_pBindingDatabaseDirector);
	
	if (m_pBindingDatabaseDirector != 0) 
	{
		m_pBindingDatabaseDirector->Load(pStrArrayError);
	}
}

void CBindingDatabaseApplication::Save()
{
	ASSERT(m_pBindingDatabaseDirector);
	
	if (m_pBindingDatabaseDirector != 0) 
	{
		m_pBindingDatabaseDirector->Save();
	}
}

BOOL CBindingDatabaseApplication::AddAuto(CBindingEntry *pEntry,BindingStatusError::BindingError_t* errorStatus)
{
	ASSERT(m_pBindingDatabaseDirector);	
	if (m_pBindingDatabaseDirector != 0) {
		return (m_pBindingDatabaseDirector->Add(pEntry,&BindingConverterAuto,errorStatus));
	}
	return FALSE;
}

CBindingEntries* CBindingDatabaseApplication::GetBindingEntries() 
{
	return (m_pBindingEntries);
}

CBindingFiles* CBindingDatabaseApplication::GetBindingFiles() 
{	
	return (m_pBindingFiles);
}

CBindingConversionAutoOptions* CBindingDatabaseApplication::GetAutoBindingOptions()
{	
	return (BindingConverterAuto.GetAutoBindingOptions());
}

void CBindingDatabaseApplication::UpgradeBindingUnitTypes(int nodeAddress) {
	ASSERT(m_pBindingDatabaseDirector);
	if (m_pBindingDatabaseDirector != 0)  {
		m_pBindingDatabaseDirector->UpgradeBindingUnitTypes(nodeAddress);
	}
}


























