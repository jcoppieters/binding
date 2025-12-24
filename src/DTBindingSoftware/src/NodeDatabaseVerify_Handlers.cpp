// NodeDatabaseVerify_Handlers.cpp: implementation of the CNodeDatabaseVerify_Handlers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeDatabaseVerify_Handlers.h"

#include "views/solver/PropertyWizardPage0.h"
#include "views/solver/PropertyWizardPage7.h"
#include "views/solver/PropertyWizardPage8.h"
// -----------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNodeDatabaseVerify_Handler0::CNodeDatabaseVerify_Handler0(CPropertyWizardPage8& parent) :
m_Parent(parent) { }
CNodeDatabaseVerify_Handler0::~CNodeDatabaseVerify_Handler0() { }

void CNodeDatabaseVerify_Handler0::On_Incompatible_Nodes(int index,const CCanNode* pDocNode,const CCanNode* pRTNode)
{
	ASSERT(pDocNode);
	if (pDocNode) {
		m_Parent.AddNodeToComboBox(index,*pDocNode);		
	}
}

//////////////////////////////////////////////////////////////////////
CNodeDatabaseVerify_Handler1::CNodeDatabaseVerify_Handler1(CPropertyWizardPage7& parent) :
m_Parent(parent) { }

CNodeDatabaseVerify_Handler1::~CNodeDatabaseVerify_Handler1() {  }

void CNodeDatabaseVerify_Handler1::On_Incompatible_Nodes(int index, const CCanNode* pDocNode, const CCanNode* pRTNode)
{
	ASSERT(pDocNode);
	if (pDocNode) {
		m_Parent.SetComboBox(index,*pDocNode);		
	}
}

void CNodeDatabaseVerify_Handler1::On_Removed_Nodes(int index, const CCanNode* pDocNode)
{
	ASSERT(pDocNode);
	if (pDocNode) {
		m_Parent.SetComboBox(index,*pDocNode);		
	}
}

void CNodeDatabaseVerify_Handler1::On_Added_Nodes(int index, const CCanNode* pRTNode)
{
	ASSERT(pRTNode);
	if (pRTNode) {
		m_Parent.SetComboBox(index,*pRTNode);		
	}
}

//////////////////////////////////////////////////////////////////////
CNodeDatabaseVerify_Handler2::CNodeDatabaseVerify_Handler2(CPropertyWizardPage0& parent) :
m_Parent(parent) { }

CNodeDatabaseVerify_Handler2::~CNodeDatabaseVerify_Handler2() { }

void CNodeDatabaseVerify_Handler2::On_Incompatible_Nodes(int index,const CCanNode* pDocNode,const CCanNode* pRTNode)
{
	ASSERT(pDocNode);
	if (pDocNode != 0 && pRTNode != 0)
	{
		CString szText;
		szText.Format(_T("PC database device 0x%06x - CAN-bus device 0x%06x"),					
					  pDocNode->ulPhysicalAddress, pRTNode->ulPhysicalAddress);									
		m_Parent.AddToListBox(szText);		
	}
}

void CNodeDatabaseVerify_Handler2::On_Removed_Nodes(int index, const CCanNode* pDocNode)
{
	ASSERT(pDocNode);
	if (pDocNode) {		
		CString szText;
		szText.Format("Device 0x%06x (LA=0x%02x) name='%s'", pDocNode->ulPhysicalAddress,
			pDocNode->bLogicalAddress, pDocNode->szNodeName);

		m_Parent.AddToListBox(szText);	
	}
}

void CNodeDatabaseVerify_Handler2::On_Added_Nodes(int index, const CCanNode* pRTNode)
{
	ASSERT(pRTNode);
	if (pRTNode) {
		CString szText;
		szText.Format("Device 0x%06x (LA=0x%02x) name='%s'", pRTNode->ulPhysicalAddress, pRTNode->bLogicalAddress,
					  pRTNode->szNodeName);		
		m_Parent.AddToListBox(szText);	
	}
}




