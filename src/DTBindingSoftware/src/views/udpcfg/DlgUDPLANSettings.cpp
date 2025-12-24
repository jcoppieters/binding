// DlgUDPLANSettings.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgUDPLANSettings.h"
#include "uicontrols/utils/dlghelper.h"				// GUI fonts
/******************************************************************************/
using namespace DUOTECNO;
/******************************************************************************/

namespace
{
	// Netwerk interface adapter: Momenteel maximum 16.
	enum { MAX_NETWORK_INTERFACES = 16 };

	typedef struct
	{
		BOOL fUsed;
		char szName[32];
		char szIPAddress[16];
		char szNetMask[16];

	} STRUCT_NETWORK_INTERFACE;

	STRUCT_NETWORK_INTERFACE g_networkInterfaces[MAX_NETWORK_INTERFACES];

	enum
	{	
		WM_UPDATE_DIALOG		= WM_USER+1,	
	};
};


// Zoeken naar netwerk adapters in de host computer en g_networkInterfaces[]
// opvullen.
// TODO: Ander algoritme gebruiken zodat ook de naam kan getoond worden.
static int updateNetworkInterfaces(
	STRUCT_NETWORK_INTERFACE* pNetworkInterfaces)
{
	int sd;
	int result = -1;

	for (int i=0;i<MAX_NETWORK_INTERFACES;i++)
	{
		g_networkInterfaces[i].fUsed = FALSE;
		memset(g_networkInterfaces[i].szName,'\0',sizeof(g_networkInterfaces[i].szName));
		memset(g_networkInterfaces[i].szIPAddress,'\0',sizeof(g_networkInterfaces[i].szIPAddress));
		memset(g_networkInterfaces[i].szNetMask,'\0',sizeof(g_networkInterfaces[i].szNetMask));
	}

	if ((sd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != SOCKET_ERROR)
    {
		INTERFACE_INFO InterfaceList[20];
		unsigned long nBytesReturned;
		if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList,
			sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR) 
		{
			closesocket(sd);
			return 1;
		}

		int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);	
		for (int i=0;i<nNumInterfaces;++i) 
		{
			sockaddr_in *pAddress;
			pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
			strcpy(g_networkInterfaces[i].szName,inet_ntoa(pAddress->sin_addr));
			strcpy(g_networkInterfaces[i].szIPAddress,inet_ntoa(pAddress->sin_addr));

			pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
			strcpy(g_networkInterfaces[i].szNetMask,inet_ntoa(pAddress->sin_addr));

			g_networkInterfaces[i].fUsed = TRUE;			
		}

		closesocket(sd);
		result = 0;
	}
	else
	{
	
	}
	return result;
}

// Combobox in de user interface vullen met netwerk interfaces...
static int updateComboBox(
	CComboBox* pComboBox,
	STRUCT_NETWORK_INTERFACE* pNetworkInterfaces)
{
	
	ASSERT(0 != pComboBox);

	pComboBox->InsertString(0,"All adapters");
	pComboBox->SetItemDataPtr(0,0);

	for (int i=0,j=1;i<MAX_NETWORK_INTERFACES;i++)
	{
		if (TRUE == pNetworkInterfaces[i].fUsed)
		{
			if (strcmp(pNetworkInterfaces[i].szIPAddress,
				"127.0.0.1") != 0)
			{
				pComboBox->InsertString(j,pNetworkInterfaces[i].szName);
				pComboBox->SetItemDataPtr(j,pNetworkInterfaces[i].szIPAddress);

				++j;
			}
		}
	}

	pComboBox->SetCurSel(0);
	return 0;
}

// Zoeken naar de idx in het array ifv het IP-address.
static int findIndex(
	STRUCT_NETWORK_INTERFACE* pNetworkInterfaces,
	const char* szIPAddress)
{
	int retVal = -1;

	for (int i=0;i<MAX_NETWORK_INTERFACES;i++)
	{
		if (TRUE == pNetworkInterfaces[i].fUsed)
		{
			if (0 == strcmp(pNetworkInterfaces[i].szIPAddress,szIPAddress))
			{
				retVal = i;
				break;
			}
		}
	}
	return retVal;
}


// CDlgUDPLANSettings dialog

IMPLEMENT_DYNAMIC(CDlgUDPLANSettings, CDialog)

CDlgUDPLANSettings::CDlgUDPLANSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUDPLANSettings::IDD, pParent), m_pDlgFontHelper(NULL), m_szNetworkAdapterAddress() { }

CDlgUDPLANSettings::~CDlgUDPLANSettings() {
	if (NULL != m_pDlgFontHelper) {
		delete m_pDlgFontHelper;
		m_pDlgFontHelper = NULL;
	}
}

void CDlgUDPLANSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ADAPTERS, m_cmbNetworkAdapters);
	DDX_Control(pDX, IDC_STATIC_IPADDRESS, m_staticIPAddress);
	DDX_Control(pDX, IDC_STATIC_NETMASK, m_staticNetmask);
}


BEGIN_MESSAGE_MAP(CDlgUDPLANSettings, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgUDPLANSettings::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgUDPLANSettings::OnBnClickedCancel)
	ON_CBN_SELENDOK(IDC_COMBO_ADAPTERS, &CDlgUDPLANSettings::OnCbnSelendokComboNetworkadapter)
	ON_MESSAGE(WM_UPDATE_DIALOG,OnUpdateDialog)
END_MESSAGE_MAP()


// CDlgUDPLANSettings message handlers
BOOL CDlgUDPLANSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (NULL == m_pDlgFontHelper) {
		m_pDlgFontHelper = new CDlgHelperFont(*this);	
		ASSERT(NULL != m_pDlgFontHelper);
		if (NULL != m_pDlgFontHelper) {
			m_pDlgFontHelper->initDialog();	
		}
	}

	if (0 == updateNetworkInterfaces(g_networkInterfaces)) {
		updateComboBox(&m_cmbNetworkAdapters,g_networkInterfaces);
	}
	
	PostMessage(WM_UPDATE_DIALOG);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgUDPLANSettings::OnBnClickedOk()
{
	m_szNetworkAdapterAddress.Empty();	

	const int sel = m_cmbNetworkAdapters.GetCurSel();
	if (sel != -1)
	{
		const char* pszIPAddress = (const char*) m_cmbNetworkAdapters.GetItemDataPtr(sel);
		if (0 != pszIPAddress)
		{
			m_szNetworkAdapterAddress.Format(
					"%s",
					(const char*) pszIPAddress);
		}		
	}

	OnOK();
}

void CDlgUDPLANSettings::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgUDPLANSettings::OnCbnSelendokComboNetworkadapter()
{
	PostMessage(WM_UPDATE_DIALOG);
}

LONG CDlgUDPLANSettings::OnUpdateDialog(WPARAM wParam,LPARAM lParam)
{
	const int sel = m_cmbNetworkAdapters.GetCurSel();
	if (sel != -1)
	{
		const char* pszIPAddress = (const char*) m_cmbNetworkAdapters.GetItemDataPtr(sel);
		if (0 == pszIPAddress)
		{
			m_staticIPAddress.ShowWindow(FALSE);
			m_staticNetmask.ShowWindow(FALSE);
		}
		else
		{
			m_staticIPAddress.ShowWindow(TRUE);
			m_staticNetmask.ShowWindow(TRUE);

			// De index zoeken ifv het ip-address...
			const int idx = findIndex(g_networkInterfaces,pszIPAddress);
			if (idx != -1)
			{
				{
					CString szTemp;
					szTemp.Format("IP-Address=%s",g_networkInterfaces[idx].szIPAddress);
					m_staticIPAddress.SetWindowText(szTemp);
				}
				{
					CString szTemp;
					szTemp.Format("Netmask=%s",g_networkInterfaces[idx].szNetMask);
					m_staticNetmask.SetWindowText(szTemp);
				}
			}
		}
	}
	return TRUE;
}

CString CDlgUDPLANSettings::getNetworkAdapterAddress(void)
{
	return m_szNetworkAdapterAddress;
}