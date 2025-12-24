// DlgViewIRTXCodes.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgViewIRTXCodes.h"

#include "App\AppCfg\AppCfgIRTX\ConfigIRTXFile.h"

namespace
{
	BOOL helper_Get(CConfigIRTXFile* pFile,
					int index,
					CConfigIRTXFileEntry** pEntry)
	{
		ASSERT(pFile);

		BOOL result = FALSE;
		if (index < pFile->getCount())
		{
			*pEntry = pFile->get(index);
			result = TRUE;
		}
		return result;
	}

	template <class T>
	int Sort(T t,T s)
	{
		if (t > s) return 1;
		else if (t < s) return -1;
		return 0;
	}
};

// private data
CDlgViewIRTXCodes* CDlgViewIRTXCodes::m_pInstance = 0;

// CDlgViewIRTXCodes dialog

IMPLEMENT_DYNAMIC(CDlgViewIRTXCodes, CDialog)

CDlgViewIRTXCodes::CDlgViewIRTXCodes(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgViewIRTXCodes::IDD, pParent),
	  m_pConfigFile( 0 )
{
	m_nID = CDlgViewIRTXCodes::IDD;

	for (int i=0;i<HDR_LISTCTRL_MAXITEMS;i++)
	{
		m_fSortMethod[i] = TRUE;
	}
}

CDlgViewIRTXCodes::~CDlgViewIRTXCodes()
{
}

void CDlgViewIRTXCodes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IRTX, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CDlgViewIRTXCodes, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgViewIRTXCodes::OnBnClickedOk)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnNotifyHdrItem)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CDlgViewIRTXCodes message handlers

BOOL CDlgViewIRTXCodes::Create()
{ 
	return CDialog::Create(m_nID,AfxGetMainWnd());
}

CDlgViewIRTXCodes* CDlgViewIRTXCodes::getInstance( )
{
	if (m_pInstance == 0) 
	{
		m_pInstance = new CDlgViewIRTXCodes();
		if (m_pInstance != 0)
		{
			if (m_pInstance->Create() == TRUE)
			{
				m_pInstance->ShowWindow(TRUE);
			}
			else
			{
				delete m_pInstance;
				m_pInstance = 0;
			}
		}
	}
	ASSERT( m_pInstance );
	return m_pInstance;
}

void CDlgViewIRTXCodes::Show(void)
{
	if (m_pInstance != 0) 
	{		
		m_pInstance->SetActiveWindow();
	}
}
bool CDlgViewIRTXCodes::isActive( )
{
	return (m_pInstance != 0 ? true : false);
}

void CDlgViewIRTXCodes::OnBnClickedOk()
{
	DestroyWindow( );
}

BOOL CDlgViewIRTXCodes::Destroy(void)
{
	if (m_pInstance != 0) 
	{	
		m_pInstance->DestroyWindow( );
		return TRUE;
	}
	return FALSE;
}

void CDlgViewIRTXCodes::PostNcDestroy()
{
	delete this;
	m_pInstance = NULL;
}

BOOL CDlgViewIRTXCodes::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitStaticControls( );	

	PostMessage( WM_UPDATE_CONTROLS );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgViewIRTXCodes::InitStaticControls(void)
{
	// TODO: Add extra initialization here
	LVCOLUMN lvColumn;

	const int SIZE_DEVICE_ID = 40;

	RECT rc;
	m_listCtrl.GetClientRect(&rc);
	const int size = rc.right - rc.left;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH; /*| LVCF_ORDER*/;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = SIZE_DEVICE_ID;
	lvColumn.pszText = "ID";
	m_listCtrl.InsertColumn(0, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = (size/2) - (SIZE_DEVICE_ID/2) - 1;
	lvColumn.pszText = "Device";
	m_listCtrl.InsertColumn(1, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = (size/2) - (SIZE_DEVICE_ID/2) - 1;
	lvColumn.pszText = "Function";
	m_listCtrl.InsertColumn(2, &lvColumn);
}

int CALLBACK CDlgViewIRTXCodes::SortFunc(
	LPARAM lParam1, 
	LPARAM lParam2, 
	LPARAM lParamSort)
{
	int retVal = 0;

	const int index = (int) lParam1;
	const int index2 = (int) lParam2;

	switch(lParamSort)
	{
		case HDR_LISTCTRL_INDEX:	/* id */
		{
			retVal = Sort(lParam1,lParam2);

		} break;

		case HDR_LISTCTRL_DEVICE:	/* device name */
		{
			CConfigIRTXFile* p = CDlgViewIRTXCodes::m_pInstance->m_pConfigFile;
			if (p)
			{
				CConfigIRTXFileEntry* pEntry;
				CConfigIRTXFileEntry* pEntry2;

				if (helper_Get(p,index,&pEntry) &&
					helper_Get(p,index2,&pEntry2))
				{
					retVal = strcmp(pEntry->getHeader().getDeviceName(),
									pEntry2->getHeader().getDeviceName());
				}				
			}
		} break;

		case HDR_LISTCTRL_FUNCTION:	/* function name */
		{
			CConfigIRTXFile* p = CDlgViewIRTXCodes::m_pInstance->m_pConfigFile;
			if (p)
			{
				CConfigIRTXFileEntry* pEntry;
				CConfigIRTXFileEntry* pEntry2;

				if (helper_Get(p,index,&pEntry) &&
					helper_Get(p,index2,&pEntry2))
				{
					retVal = strcmp(pEntry->getHeader().getFunctionName(),
									 pEntry2->getHeader().getFunctionName());	
				}				
			}
		} break;
	}

	if (lParamSort < HDR_LISTCTRL_MAXITEMS)
	{
		if (CDlgViewIRTXCodes::m_pInstance->m_fSortMethod[lParamSort] == FALSE)
		{
			if (retVal == 1) retVal = -1;
			else if (retVal == -1) retVal = 1;
		}
	}

	return retVal;
}

void CDlgViewIRTXCodes::Add(CListCtrl* pList,int index,CConfigIRTXFileEntry* pEntry)
{
	if (pList)
	{
		if (pEntry)
		{
			LVITEM lvItem;
			int nItem;

			char strNumber[20];
			sprintf(strNumber, "%d", index);		

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = 0;
			lvItem.iSubItem = 0;
			lvItem.pszText = strNumber;			
			nItem = pList->InsertItem(&lvItem);
			pList->SetItemData(nItem, index);

			pList->SetItemText(nItem, 1, pEntry->getHeader().getDeviceName());
			pList->SetItemText(nItem, 2, pEntry->getHeader().getFunctionName());
		}
	}
}

LONG CDlgViewIRTXCodes::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	m_listCtrl.DeleteAllItems( );

	if (m_pConfigFile)
	{
		const int count = m_pConfigFile->getCount( );
		for (int i=0;i<count;i++)
		{
			Add(&m_listCtrl,i,m_pConfigFile->get(i));
		}

		m_listCtrl.SortItems(SortFunc, (LPARAM) HDR_LISTCTRL_INDEX);

	}

	return TRUE;
}

void CDlgViewIRTXCodes::attach(CConfigIRTXFile* pCfg)
{
	m_pConfigFile = pCfg;
}

void CDlgViewIRTXCodes::OnNotifyHdrItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW *plvItem = (NMLISTVIEW *) pNMHDR;
							
	if (plvItem->iItem < HDR_LISTCTRL_MAXITEMS)
	{
		if (m_fSortMethod[plvItem->iItem] == FALSE)
		{
			m_fSortMethod[plvItem->iItem] = TRUE;
		}
		else
		{
			m_fSortMethod[plvItem->iItem] = FALSE;	
		}

		m_listCtrl.SortItems(SortFunc, plvItem->iItem);					
	}

	*pResult = 0;	
}

void CDlgViewIRTXCodes::Update( )
{
	PostMessage( WM_UPDATE_CONTROLS );
}

void CDlgViewIRTXCodes::OnClose()	// BUG-0139
{
	DestroyWindow( );
}