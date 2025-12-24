// BindingFiles.cpp: implementation of the CBindingFiles class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFiles.h"

#include "BindingEntries.h"
#include "NodeDatabase.h"
#include "BindingSoftwareDoc.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFiles::CBindingFiles()
{
	ClearDatabase();
}
CBindingFiles::~CBindingFiles()
{
}

//////////////////////////////// DATABASE ///////////////////////////////
void CBindingFiles::ClearDatabase(void)
{
	while (!List.IsEmpty())
	{
		CBindingFile* const pEntry = 
			reinterpret_cast<CBindingFile*>(List.RemoveHead( ));

		ASSERT( pEntry );
		delete pEntry;
	}
	ASSERT( List.GetCount( ) == 0 );
}

unsigned short CBindingFiles::Count(void)
{
	return (List.GetCount());
}

BOOL CBindingFiles::Add(BYTE bNodeAddress) {
	CBindingFile *pEntry = NULL;
	CBindingFile *pNew = NULL;	
	if ((pEntry = Get(bNodeAddress)) != NULL) {
		return FALSE;
	}
	BOOL result = FALSE;
	if ((pNew = new CBindingFile(bNodeAddress)) != NULL) {		
		List.AddTail(pNew);
		result = TRUE;
	}
	ENSURE(NULL != pNew);
	return result;
}

CBindingFile* CBindingFiles::GetEntry(BYTE i)
{	
	if (i >= Count()) 
	{
		CString szError;
		szError.Format("Binding File Not Found");
		AfxMessageBox(szError);
		return NULL;	
	}

	int number = 0;
	CBindingFile *pEntry;

	POSITION pos = List.GetHeadPosition();
	while(pos!=NULL) 
	{
		if ((pEntry = (CBindingFile*) List.GetNext(pos)) != NULL)
		{		
			if (number++ == i)
			{ 
				return pEntry;			
			}
		}
	}
	return NULL;
}

BOOL CBindingFiles::Delete(BYTE bNodeAddress)
{
	CBindingFile *pEntry;
	
	POSITION pos2;
	POSITION pos1 = List.GetHeadPosition();

	while (pos1 != NULL) 
	{
  	   pos2 = pos1;
 
	   if ((pEntry = (CBindingFile*) List.GetNext( pos1 )) != NULL)
	   {	   
			if (pEntry->GetNodeAddress() == bNodeAddress)
			{
				if ((pEntry = (CBindingFile*) List.GetAt( pos2 )) != NULL)	// Save the old pointer for deletion
				{
					List.RemoveAt( pos2 );

					delete pEntry;								// Deletion avoids memory leak.
					return (TRUE);
				}
			}
		}
	}
   
	if (1)
	{
		CString szError;
		szError.Format("BindingFile Not Found");
		AfxMessageBox(szError);
	}

	return FALSE;
}

CBindingFile* CBindingFiles::Get(BYTE bNodeAddress)
{
	CBindingFiles::Iterator it(CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingFile* const pEntry = it.CurrentItem();
		ASSERT( pEntry );

		if (pEntry->GetNodeAddress() == bNodeAddress) 		
			return pEntry;										
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////
BOOL CBindingFiles::Save(void)
{
	CBindingFiles::Iterator it(CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		ASSERT( it.CurrentItem() );
		it.CurrentItem()->Save();
	}
	return TRUE;
}
/***********************************************************************
  name : BOOL CBindingFiles::Load(void)
  
  description :
		 For each bindingfile in database the Load Method is invoked

  Invoked By :CTimerDoc::Serialize (Load)
			
*************************************************************************/
BOOL CBindingFiles::Load(CStringArray* pStrArrayError) {
	CBindingFiles::Iterator it(CreateIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CBindingFile* const pBindingFile=it.CurrentItem();
		ASSERT( pBindingFile );
		if (0!=pBindingFile) {
			pBindingFile->Load(pStrArrayError);
		}
	}
	return TRUE;
}







