// DlgNetworkCanStatus.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgNetworkCanStatus.h"

#include "NodeDatabaseOperations.h"
#include "FsmNicErrorsInfo.h"
#include "TXNodeMessage.h"
#include "CanNode.h"
#include "NodeRuntimeStatus.h"
// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
using namespace TMGlobalHSM;
// ---------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CDlgNetworkCanStatus* CDlgNetworkCanStatus::m_pDialog = 0;
/////////////////////////////////////////////////////////////////////////////
void CDlgNetworkCanStatus::Show()
{
	if (m_pDialog == 0) 
	{
		m_pDialog = new CDlgNetworkCanStatus();
	
		if (m_pDialog->Create() == TRUE)
		{
			m_pDialog->ShowWindow(TRUE);
		}
	}
	else 
	{
		m_pDialog->SetActiveWindow();
	}
}

void CDlgNetworkCanStatus::Destroy()
{
	if (m_pDialog != 0) 
	{
		m_pDialog->DestroyWindow();
	}
}

void CDlgNetworkCanStatus::Update(BYTE bNodeAddress)
{
	if (m_pDialog != 0)
	{
		CCanNode* const pNode = m_pDialog->GetNode(&m_pDialog->m_ComboName);
		if ((pNode != 0) && (pNode->bLogicalAddress == bNodeAddress))
		{
			m_pDialog->PostMessage(WM_UPDATE_CONTROLS);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDlgNetworkCanStatus dialog


CDlgNetworkCanStatus::CDlgNetworkCanStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNetworkCanStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNetworkCanStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgNetworkCanStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNetworkCanStatus)
	DDX_Control(pDX, IDC_STATIC_NODEINFO, m_StaticNodeInfo);
	DDX_Control(pDX, IDC_LISTBOX_ERRORS, m_ListBoxErrorsCan);
	DDX_Control(pDX, IDC_LIST_NETWORKERRORS, m_ListBoxErrorsNetwork);	
	DDX_Control(pDX, IDC_COMBO_NODENAME, m_ComboName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNetworkCanStatus, CDialog)
	//{{AFX_MSG_MAP(CDlgNetworkCanStatus)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_CBN_SELCHANGE(IDC_COMBO_NODENAME, OnSelchangeComboNodename)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNetworkCanStatus message handlers

void CDlgNetworkCanStatus::OnOK() 
{
	DestroyWindow();
}

BOOL CDlgNetworkCanStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateNodeList();

	m_ComboName.SetCurSel(0);
	
	PostMessage( WM_UPDATE_CONTROLS );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNetworkCanStatus::OnButtonStart() 
{
	CPtrList Nodes;	
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());

	const int nCount = NodeDatabaseOperations.SubclassNodes(&Nodes);
	ASSERT(nCount);

	CFSMNicErrorsInfo::GetInstance()->Clear();
	CFSMNicErrorsInfo::GetInstance()->Add(&Nodes);

	TMGetGlobalHSM()->EvOperationNicNetworkStart();
}


void CDlgNetworkCanStatus::OnButtonRefresh() 
{
	CCanNode* const pNode = GetNode(&m_ComboName);
	if (pNode != 0)
	{
		CTXNodeMessage::getInstance()->
			RequestNodeNicErrors(pNode->bLogicalAddress,0);

		CTXNodeMessage::getInstance()->
			RequestNodeNicErrors(pNode->bLogicalAddress,1);
	}
}

void CDlgNetworkCanStatus::UpdateNodeList()
{
	CPtrList Nodes;	
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());

	const int nCount = NodeDatabaseOperations.SubclassNodes(&Nodes);
	ASSERT(nCount);

	if (nCount > 0)
	{
		POSITION pos;
		for (pos = Nodes.GetHeadPosition();pos != 0; )
		{
			CCanNode* const pNode = static_cast<CCanNode*>(Nodes.GetNext(pos));
			if (pNode != 0)
			{
				const int nIndex = m_ComboName.AddString(pNode->szNodeName);
				
				if (nIndex != -1)
				{
					m_ComboName.SetItemDataPtr(nIndex,pNode);
				}
			}
		}
	}
}

CCanNode* CDlgNetworkCanStatus::GetNode(CComboBox *pControl)
{
	if (pControl != 0)
	{
		const int sel = pControl->GetCurSel();
		if (sel != -1)
		{
			CCanNode* const pNode = static_cast<CCanNode*>
				(pControl->GetItemDataPtr(sel));

			ASSERT(pNode);

			return pNode;
		}
	}
	return 0;
}

void CDlgNetworkCanStatus::UpdateNodeCanErrors(CCanNode *pNode)
{
	if (pNode != 0)
	{
		m_ListBoxErrorsCan.ResetContent();
		m_ListBoxErrorsNetwork.ResetContent();

		if (pNode->GetRuntimeStatus() != 0)
		{
			CStringArray stringArray;

			pNode->GetRuntimeStatus()->DumpCanControllerErrors(stringArray);
			for (int i=0,n=stringArray.GetSize();
				 i<n;
				i++)
			{		
				m_ListBoxErrorsCan.AddString(stringArray.GetAt(i));
			}

			pNode->GetRuntimeStatus()->DumpNetworkErrors(stringArray);
			for (int i=0,n=stringArray.GetSize();
				 i<n;
				i++)
			{		
				m_ListBoxErrorsNetwork.AddString(stringArray.GetAt(i));
			}
		}
	}
}

LRESULT CDlgNetworkCanStatus::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	CCanNode* const pNode = GetNode(&m_ComboName);
	
	if (pNode != 0)
	{
		UpdateNodeInfo(pNode);
		UpdateNodeCanErrors(pNode);
	}
	return TRUE;
}

void CDlgNetworkCanStatus::OnSelchangeComboNodename() 
{
	PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgNetworkCanStatus::UpdateNodeInfo(CCanNode *pNode)
{
	if (pNode != 0)
	{
		CString szNodeInfo;
		szNodeInfo.Format("Physical: 0x%06x\nLogical: 0x%02x",
			pNode->ulPhysicalAddress,
			pNode->bLogicalAddress);
		
		m_StaticNodeInfo.SetWindowText(szNodeInfo);
	}
}

void CDlgNetworkCanStatus::PostNcDestroy() 
{
	delete this;
	m_pDialog = 0;
}

void CDlgNetworkCanStatus::OnClose() 
{
	DestroyWindow();
}
