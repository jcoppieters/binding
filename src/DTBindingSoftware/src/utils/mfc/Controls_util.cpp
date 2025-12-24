#include "stdafx.h"
#include "controls_util.h"
/*************************************************************/
using namespace DUOTECNO::MFC_HELPER;
/*************************************************************/

/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/

BOOL CControls_Utils::getNwkIPPort(CEdit& refCtrl,int* value) {
	int v = -1;
	CString szText;
	refCtrl.GetWindowText(szText);		

	if (1 == sscanf(szText.GetBuffer(0),"%d",&v)) {
		if (v>=0 && v<=65536) {				
			*value = v;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CControls_Utils::getNwkHostAddress(CWnd& refCtrl,CString* hostAddress) {		
	CIPAddressCtrl* const pCtrl = dynamic_cast<CIPAddressCtrl*>(&refCtrl);
	if (0 != pCtrl) {
		BYTE v[4] = { 0,0,0,0 };
		if (4 == pCtrl->GetAddress(v[0],v[1],v[2],v[3])) {
			hostAddress->Format("%d.%d.%d.%d",v[0],v[1],v[2],v[3]);
			return TRUE;
		}		
	}
	else {
		CEdit* const pCtrl = dynamic_cast<CEdit*>(&refCtrl);
		if (0 != pCtrl) {		
			return getTrimmedString(*pCtrl, hostAddress);		
		}		
	}		
	return FALSE;
}

// Sedert V17.05: Extra controle op het password.
BOOL CControls_Utils::getPassword(CEdit& refCtrl,CString* pwd) {
	return getTrimmedString(refCtrl,pwd);
}

// Sedert V17.05: helper functie.
BOOL CControls_Utils::getTrimmedString(CEdit& refCtrl, CString* pStrOut) {
	CString szText;
	refCtrl.GetWindowText(szText);
	if (szText.IsEmpty()) {
		return FALSE;
	}

	*pStrOut = szText.Trim();
	return (pStrOut->GetLength() > 0 ? TRUE : FALSE);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


int MFCGuiHelper_ListCtrl::listCtrlFindItemIndex(CListCtrl& refListCtrl,DWORD dwItemData) {
	int retVal = -1;
	const int numberOfItems = refListCtrl.GetItemCount();
	for (int i=0; i < numberOfItems; i++) {
		if (refListCtrl.GetItemData(i) == dwItemData) {
			retVal = i;
			break;
		}
	}
	return retVal;
}

int MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(CListCtrl* pCtrl,int columnNR,const char* szName,int sizeX) {
	int nCol = -1;
	if (pCtrl) {
		LVCOLUMN lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.cx = sizeX;
		lvColumn.pszText = (LPSTR) szName;
		nCol = pCtrl->InsertColumn(columnNR, &lvColumn);
	}
	return nCol;
}


void MFCGuiHelper_ListCtrl::setTopIndex(CListCtrl* pList) {
	const int count = pList->GetItemCount();
	if (count > 0) {
		if (pList->EnsureVisible(count-1, FALSE)) {
		}
	}
}


int MFCGuiHelper_ListCtrl::getSelectedRow(CListCtrl* pList, DWORD* dwItemItemData) {
	int nSelected = pList->GetNextItem(-1, LVNI_SELECTED);
	if (nSelected < 0) {
		return nSelected;
	}
	*dwItemItemData = pList->GetItemData(nSelected);
	return nSelected;
}

/*************************************************************/
};
};
/*************************************************************/