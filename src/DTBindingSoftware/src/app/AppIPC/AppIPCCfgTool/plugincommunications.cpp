#include "stdafx.h"
#include "plugincommunications.h"
/*******************************************************/
namespace DUOTECNO {
namespace PLUGIN {
/*******************************************************/
static BOOL GetDirLocation(char* szPathname,unsigned long ulCSIDL) {
	if (SHGetSpecialFolderPath(AfxGetMainWnd()->m_hWnd, szPathname, ulCSIDL, FALSE)==FALSE) {
		AfxMessageBox("False");
		return FALSE;
	}
	return TRUE;
};

static CString ParseFileName(const CString& szFileName) {
	CString szReplace,szReturn;
	CString szTemp(szFileName);
	int i,j;

	szTemp.MakeUpper();
	szReturn = szTemp;

	if ((j = szTemp.Find(".EXE")) != -1)				// EXE
		szReplace = ".EXE";
	else if ((j = szTemp.Find(".LNK")) != -1)			// LNK
		szReplace = ".LNK";
	else if ((j = szTemp.Find(".BAT")) != -1)			// BAT
		szReplace = ".BAT";

	if (szReplace.IsEmpty() == FALSE) {
		i = szTemp.Find('%');
	
		if ((i != -1) && (i<j)) {
			// CSIDL defined --> Substitue to Directory ...
			i = szTemp.Find('%',i+1);
		
			CString szCsidl(szTemp.Mid(1,i));
			char szPathName[255];
		
			if (szCsidl == "%PROGRAMFILES%") {						
				if (GetDirLocation(&szPathName[0],0x0026) == FALSE)
					return CString("");

				szTemp.Replace(szCsidl,szPathName);
			}
			else if (szCsidl == "%DESKTOP%") {						
				if (GetDirLocation(&szPathName[0],0x0000) == FALSE)
					return CString("");

				szTemp.Replace(szCsidl,szPathName);
			}
			else 
			{
				CString szError;
				szError.Format(_T("Unable to convert CSIDL '%s' from registry..."),szCsidl);
				AfxMessageBox(szError);
				return CString("");
			}
		}
				
		if ((i = szTemp.Find(szReplace)) != -1) {
			szReturn = szTemp.Left(i+5);
		}	
	}
	return szReturn;
};

/*******************************************************/


/*******************************************************/
/********************* CMFCOpenFile ********************/
/*******************************************************/
BOOL CMFCOpenFile::OpenFile(const CString& szFileName,const CString& szParameters) {	
	BOOL result = FALSE;
	CString szNewFileName(ParseFileName(szFileName));
	if (szNewFileName.IsEmpty() == FALSE) {		
		const HINSTANCE ErrorCode = ShellExecute(NULL, _T("OPEN"), szNewFileName, szParameters, 0, SW_RESTORE );	
		if ((int)ErrorCode < 32) {
			CString szError;
			szError.Format(_T("Can't load filename :'%s'\nError code %d"),szNewFileName,(int) ErrorCode);
			AfxMessageBox(szError);
			result = FALSE;
		}	
		else {
			result = TRUE;
		}
	}
	return result;
}

BOOL CMFCOpenFile::OpenWindow(const CString& szWindowName) {
	BOOL result = FALSE;
	if (szWindowName.IsEmpty() == FALSE) {
		CWnd* const pWnd = CWnd::FindWindow(NULL,szWindowName);
		if (pWnd != 0)  {					
			pWnd->ShowWindow(SW_RESTORE);
			pWnd->BringWindowToTop();				
			result = TRUE;
		}
	}
	return result;
}		


/**************************************************************/
/********************** CMFCFindWindow ************************/
/**************************************************************/
CWnd* CMFCFindWindow::Find(const CString& szName,int flags) {
	CWnd* pReturn = 0;
	switch (flags) {
		case FLAG_NAME_WINDOW: {
			pReturn = CWnd::FindWindow(0, szName);
		} break;
		case FLAG_NAME_CLASS: {
			pReturn = CWnd::FindWindow(szName, 0);		
		} break;
	}
	return pReturn;
};

/*************************************************************/
/********************* CMFCMsgCopyData ***********************/
/*************************************************************/
CMFCMsgCopyData::CMFCMsgCopyData( ) {
	Clear( );
}

CMFCMsgCopyData::CMFCMsgCopyData(CWnd& srcWindow,CString szMessage) {
	BuildMessage(srcWindow, szMessage);
}

CMFCMsgCopyData::CMFCMsgCopyData(const COPYDATASTRUCT& copyDataStruct) {
	memcpy(&m_copyDataStructure, &copyDataStruct, sizeof(COPYDATASTRUCT));
}

CMFCMsgCopyData::~CMFCMsgCopyData() { }
/*************************** public **************************/	
void CMFCMsgCopyData::Build(CWnd& srcWindow,CString szMessage) {
	BuildMessage(srcWindow, szMessage);	
}

CString CMFCMsgCopyData::getMessage( ) const {
	CString szReturn;
	if ((m_copyDataStructure.cbData > 0) && (m_copyDataStructure.lpData != 0)) {
		szReturn = CString((const char*) m_copyDataStructure.lpData, m_copyDataStructure.cbData);
	}
	return szReturn;
}

CWnd* CMFCMsgCopyData::getWindowSrc( ) const {
	return CWnd::FromHandle((HWND) m_copyDataStructure.dwData);
}

LPARAM CMFCMsgCopyData::getLParam( ) const {
	return (WPARAM) &m_copyDataStructure;	
}

WPARAM CMFCMsgCopyData::getWParam( ) const {
	return 0;
}

/*************************** protected ***********************/
void CMFCMsgCopyData::BuildMessage(CWnd& srcWindow,const CString& szMessage) {
	m_copyDataStructure.cbData = szMessage.GetLength();
	m_copyDataStructure.dwData = (DWORD) srcWindow.GetSafeHwnd();		// HWND van CMainFrame.
	m_copyDataStructure.lpData = (void*) (const char*) szMessage;
}	

void CMFCMsgCopyData::Clear( ) {
	m_copyDataStructure.cbData = 0;
	m_copyDataStructure.dwData = 0;
	m_copyDataStructure.lpData = 0;
}
	
/*****************************************************************/
/********************** Free Functions ***************************/
/*****************************************************************/

/******************* free functions ********************/
LRESULT SendWindowMessage(CWnd& destWindow,const CMFCMessage& msg) {
	const LRESULT res = destWindow.SendMessage(msg.getWindowMessage(),msg.getWParam(),msg.getLParam()); 
	return res;
}

BOOL PostWindowMessage(CWnd& destWindow,const CMFCMessage& msg) {
	const BOOL res = destWindow.PostMessage(msg.getWindowMessage(),msg.getWParam(), msg.getLParam());	
	return res;
}
/*******************************************************/
};
};
/*******************************************************/


