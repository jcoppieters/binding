#pragma once

/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/
struct CControls_Utils 
{
	static BOOL getNwkIPPort(CEdit& refCtrl,int* value);

	// Sedert V17.05: Spaties worden verwijderd aan het begin/einde van de string (zie getTrimmedString)
	static BOOL getNwkHostAddress(CWnd& refCtrl,CString* hostAddress);

	// Sedert V17.05: Spaties worden verwijderd aan het begin/einde van de string (zie getTrimmedString)
	// return waarde TRUE wanneer geldig paswoord.
	static BOOL getPassword(CEdit& refCtrl,CString* pwd);

	// return waarde TRUE wanneer string niet leeg is.
	static BOOL getTrimmedString(CEdit& refCtrl, CString* pStrOut);
};
/*************************************************************/

/*************************************************************/
/*************************************************************/
/*************************************************************/
struct MFCGuiHelper_ListCtrl {

	static int listCtrlFindItemIndex(CListCtrl& refListCtrl,DWORD dwItemData);
	static int listCtrlInitAddColumn(CListCtrl* pCtrl,int columnNR,const char* szName,int sizeX);
	static void setTopIndex(CListCtrl* pList);
	static int getSelectedRow(CListCtrl* pList, DWORD* dwItemItemData);
};

/*************************************************************/
};
};
/*************************************************************/