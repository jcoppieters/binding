// DlgCanFTP.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgCanFTP.h"

#include "App\AppCfg\AppCfgCanFtp\ConfigCanFtp.h"			// since 0x0562
#include "App\AppCfg\AppCfgCanFtp\ConfigCanFtp_info.h"		// since 0x0562

#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMGlobalSettings;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCanFTP dialog
////////////////////////////// static members ///////////////////////////////
CDlgCanFTP* CDlgCanFTP::m_pInstance = 0;
/////////////////////////////////////////////////////////////////////////////


CDlgCanFTP::CDlgCanFTP(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCanFTP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCanFTP)
	m_strFileNameLocal = _T("");
	m_strFileNameRemote = _T("");
	m_strNodeAddress = _T("");
	//}}AFX_DATA_INIT

	m_nID = CDlgCanFTP::IDD;
}


void CDlgCanFTP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCanFTP)
	DDX_Text(pDX, IDC_EDIT_FILENAME_LOCAL, m_strFileNameLocal);
	DDX_Text(pDX, IDC_EDIT_FILENAME_REMOTE, m_strFileNameRemote);
	DDX_Text(pDX, IDC_EDIT_NODEADDRESS, m_strNodeAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCanFTP, CDialog)
	//{{AFX_MSG_MAP(CDlgCanFTP)
	ON_BN_CLICKED(IDC_BUTTON_RECEIVE, OnButtonReceive)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCanFTP message handlers

BOOL CDlgCanFTP::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	m_strFileNameLocal = "Local file name";
	m_strFileNameRemote = "Remote file name";
	m_strNodeAddress = "0xFF";

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCanFTP::PostNcDestroy() 
{
	delete this;
	m_pInstance = NULL;
}

void CDlgCanFTP::OnButtonReceive() 
{
	BYTE bNodeAddress = 0xFF;
	CString szFileNameRemote;
	CString szFileNameLocal;

	if (getParameters(&bNodeAddress,
					  &szFileNameRemote,
					  &szFileNameLocal))
	{		
		CConfigCanFTP_Info& configCanFTP_Info =
		CConfigCanFTP_Info::getInstance();

		configCanFTP_Info.clear( );
	
		CConfigCanFTP_FileEntry* const pEntry =
			configCanFTP_Info.create( );
		if (pEntry)
		{
			CString szFilename;
			szFilename.Format("%s%s",TMGetFileSettings()->GetFullyQualifiedPathName(),szFileNameLocal);
			pEntry->setNodeAddress( bNodeAddress );
			pEntry->setLocalFileName(szFilename);
			pEntry->setRemoteFileName(szFileNameRemote);
			pEntry->setInfo("Receive CAN FTP");
			pEntry->setAction(CConfigCanFTP_FileEntry::ACTION_FTP_READ);
			configCanFTP_Info.addTask(pEntry);	

			TMGetGlobalHSM()->EvOperationFtpConfig( );	
		}	
	}
}

void CDlgCanFTP::OnButtonSend() 
{
	BYTE bNodeAddress = 0xFF;
	CString szFileNameRemote;
	CString szFileNameLocal;

	if (getParameters(&bNodeAddress,
					  &szFileNameRemote,
					  &szFileNameLocal))
	{

		CConfigCanFTP_Info& configCanFTP_Info =
		CConfigCanFTP_Info::getInstance();

		configCanFTP_Info.clear( );
		
		CConfigCanFTP_FileEntry* const pEntry =
			configCanFTP_Info.create( );

		if (pEntry)
		{
			CString szFilename;
			szFilename.Format("%s%s",TMGetFileSettings()->GetFullyQualifiedPathName(),szFileNameLocal);
			pEntry->setNodeAddress( bNodeAddress );
			pEntry->setLocalFileName(szFilename);
			pEntry->setRemoteFileName(szFileNameRemote);
			pEntry->setInfo("Send CAN FTP");
			pEntry->setAction(CConfigCanFTP_FileEntry::ACTION_FTP_WRITE);
			configCanFTP_Info.addTask(pEntry);	

			TMGetGlobalHSM()->EvOperationFtpConfig( );	
		}	
	}
}

void CDlgCanFTP::OnOK() 
{
	DestroyWindow( );
}

/////////////////////////////////////////////////////////////////////////////
// CDlgCanFTP custom handlers
CDlgCanFTP* CDlgCanFTP::getInstance( )
{
	if (m_pInstance == 0) 
	{
		m_pInstance = new CDlgCanFTP();
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

BOOL CDlgCanFTP::Create()
{ 
	return CDialog::Create(m_nID,AfxGetMainWnd());
}
void CDlgCanFTP::Show(void)
{
	if (m_pInstance != 0) 
	{		
		m_pInstance->SetActiveWindow();
	}
}
bool CDlgCanFTP::isActive( )
{
	return (m_pInstance != 0 ? true : false);
}

//////////////////////////////////////////////////////////////////////////////
// Protected helper klassen

BOOL CDlgCanFTP::GetNodeAddress(BYTE* pbNodeAddress)
{
	BOOL retVal = FALSE;
	if (!m_strNodeAddress.IsEmpty( ))
	{
		int nodeAddress = -1;

		if (sscanf(m_strNodeAddress,			// CHECKED_SSCANF_OK
				   "%x",
				   &nodeAddress) == 1)
		{			
			if (nodeAddress >= 0x03 && nodeAddress <= 0xFD)
			{
				*pbNodeAddress = (BYTE) nodeAddress;
				retVal = TRUE;
			}
		}
	}
	return retVal;
}

BOOL CDlgCanFTP::getParameters(
	BYTE* pbNodeAddress, 
	CString* pStrFileNameRemote,
	CString* pStrFileNameLocal)
{
	UpdateData( TRUE );


	if (!GetNodeAddress(pbNodeAddress))
	{
		/**
		 * @todo	Melding geven dat het node adres niet Ok is.
		 *			
		 */
		return FALSE;
	}

	if (m_strFileNameLocal.IsEmpty( ) ||
		m_strFileNameRemote.IsEmpty( ))
	{
		/**
		 * @todo	Melding geven dat de bestandsnamen niet juist
		 *			ingevuld zijn.
		 *			
		 */

		return FALSE;
	}


	/**
	 * @todo	Bijkomende controle op de bestandsnamen.
	 */

	*pStrFileNameRemote = m_strFileNameRemote;
	*pStrFileNameLocal = m_strFileNameLocal;

	return TRUE;
}
