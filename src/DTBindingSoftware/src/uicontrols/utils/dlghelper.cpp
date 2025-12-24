#include "stdafx.h"
#include "BindingSoftware.h"
#include "dlghelper.h"
/*********************************************************************/
namespace DUOTECNO {
/*********************************************************************/

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
CDlgHelper::CDlgHelper(CWnd& refParent) : 
	m_refParent(refParent) { }
	
CDlgHelper::~CDlgHelper() { }

void CDlgHelper::initDialog() 
{
	this->doWorkInitDialog(m_refParent);
}
/*********************************************************************/


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
CDlgHelperFont::CDlgHelperFont(CWnd& refParent) : 
	Base(refParent) , m_DlgFont( ) { }

CDlgHelperFont::~CDlgHelperFont() {
	m_DlgFont.DeleteObject();
}

// Opvragen van de systeem font.
// Deze font aanmaken en bijhouden in de dialoog. --> m_DlgFont
// De font doorgeven aan alle controls van deze dialoog. (SendMessageToDescendants)

void CDlgHelperFont::doWorkInitDialog(CWnd& refParent) {
#if (USE_DIALOG_SYSTEMFONTS == 1)	
	//createSystemFont(refParent);		// <--- De Systeem font.
	createCustomFont(refParent);		// <--- "Segoe UI", 12
	setDlgFont(refParent);
#endif 
}


//////////////////////////////////////////////////////////////////////////////////
// Windows 8.1 / visual studio 2008	(Sedert Vista is "Segoe UI" de standaard font.
//	LogFont info:
//		lfheight = -12
//		lfWeight = 400	(FW_NORMAL)
//		lfFaceName = "Segoe UI"
//////////////////////////////////////////////////////////////////////////////////
	

// Get the system message box font
void CDlgHelperFont::createSystemFont(CWnd& refParent) {
		
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	LOGFONT lfDlgFont = ncm.lfMessageFont;
	m_DlgFont.CreateFontIndirect(&lfDlgFont);
}

void CDlgHelperFont::createCustomFont(CWnd& refParent) {

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));          
	logFont.lfHeight = -12;		
	logFont.lfWeight = FW_NORMAL;	
	strcpy(logFont.lfFaceName,"Segoe UI");
	m_DlgFont.CreateFontIndirect(&logFont);
}

// Set the dialog and all its controls to use the system message box font
void CDlgHelperFont::setDlgFont(CWnd& refParent) 
{
	if (NULL != m_DlgFont.m_hObject) {	
		refParent.SetFont(&m_DlgFont, TRUE);
		refParent.SendMessageToDescendants(WM_SETFONT, (WPARAM)m_DlgFont.m_hObject, MAKELPARAM(FALSE, 0), TRUE);
	}
}

/*********************************************************************/
}; /* end namespace */
/*********************************************************************/