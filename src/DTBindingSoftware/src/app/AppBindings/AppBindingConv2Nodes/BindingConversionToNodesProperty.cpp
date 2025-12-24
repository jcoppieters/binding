// BindingConversionToNodesProperty.cpp: implementation of the CBindingConversionToNodesProperty class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConversionToNodesProperty.h"

#include "BindingFile1.h"
#include "views/debug/DlgDebugBindingConversion.h"
#include "BindingEntryInfo.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"


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
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingConversionToNodesProperty::CBindingConversionToNodesProperty(CBindingPropertyUnit* const pBindingUnit) :
m_pBindingUnit( pBindingUnit ),
m_pNodeDatabase( TMGetDocumentNodes() )
{
	ASSERT( m_pBindingUnit );
	ASSERT( m_pNodeDatabase );
}

CBindingConversionToNodesProperty::~CBindingConversionToNodesProperty()
{

}

BOOL CBindingConversionToNodesProperty::CheckBindingFileProperties(void)
{
	if (m_pNodeDatabase != NULL)
	{
		return (CheckNrEntries(m_pNodeDatabase));
	}

	return FALSE;
}

int CBindingConversionToNodesProperty::ProcessPropertyBindings(
	BOOL fMakeNoDefaults,
	BOOL fShowErrorMessage)
{
	int retVal=0;
	int nCount=0;

	ClearStringArray();

	nCount = m_pBindingUnit->ConstructPBindings(this,fMakeNoDefaults);

	if (nCount != GetStringArraySize()) 
	{
		ASSERT(0);
		return -1;
	}

	ClearBindings();

	if (nCount != 0) 
	{
		if (CheckBindingFileProperties() == FALSE) 
		{
			if (TRUE==fShowErrorMessage)
			{
				CString szMessage;
				szMessage.Format(_T("Property Bindings are deleted from the device: node 0x%02x,unit 0x%02x"
									"Property Bindings couldn't be added !"),
									m_pBindingUnit->GetNodeAddress(),
									m_pBindingUnit->GetUnitAddress());

				AfxMessageBox(szMessage);
			}
			return -2;
		}

		if (TRUE==AddBindingsToNodes())
		{
			retVal=nCount;	
		}
	}
	return retVal;
}

BOOL CBindingConversionToNodesProperty::AddBinding(CString s)
{
	const int i = AddToStringArray(s);
	return TRUE;
}

BOOL CBindingConversionToNodesProperty::ClearBindings(void)
{
	CDlgDebugBindingConversion::Add("Delete property bindings from the device.");

	if (m_pNodeDatabase != NULL)
	{	
		CCanNode *pNode;	
		int iEntryNr;

		if (m_pNodeDatabase->FindNode(m_pBindingUnit->GetNodeAddress(),
								      &iEntryNr,
									  &pNode)) 
		{	
			CBindingFile *ptrBindingFile = NULL;
			if ((ptrBindingFile = pNode->GetBindingFilePtr()) != NULL)
			{			
				ptrBindingFile->RemovePropertyStrings(m_pBindingUnit->GetUnitAddress());
				return TRUE;
			}
		}	
	}

	ASSERT(0);
	return FALSE;
}

BOOL CBindingConversionToNodesProperty::AddBindingsToNodes(void)
{
	CDlgDebugBindingConversion::Add("Add property bindings to the device.");

	if (m_pNodeDatabase != NULL)
	{					
		const int nEntries = GetStringArraySize();	

		if (nEntries == 0) 
		{
			return FALSE;
		}
			
		const unsigned short usBindingNr = 0;

		for (int i=0;i<nEntries;i++) 
		{
			CString szBindingString = GetStringFromArray(i);	
		
			int iEntryNr;	
			CCanNode *pNode;

			if (m_pNodeDatabase->FindNode(m_pBindingUnit->GetNodeAddress(),
									      &iEntryNr,
										  &pNode)) 
			{			
				CBindingFile *ptrBindingFile;
				if ((ptrBindingFile = pNode->GetBindingFilePtr()) != NULL)
				{							
					if (ptrBindingFile->AddString(usBindingNr,szBindingString)) 
					{	
						CString szDebug;

						szDebug.Format("Node 0x%02X : BindingString '%s'",
									   m_pBindingUnit->GetNodeAddress(),
									   szBindingString);

						CDlgDebugBindingConversion::Add(szDebug);

					}
					else {	ASSERT(0);	}
				}
				else { ASSERT(0); }
			} 
			else { ASSERT(0); }
		
		} // end for (int i=0;i<nEntries;i++) 

		return TRUE;
	}
	return FALSE;
}


