#include "stdafx.h"
/*************************************************************/
#include "combobox_util.h"
/*************************************************************/
using namespace DUOTECNO::MFC_HELPER;
/*************************************************************/

/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/

BOOL DUOTECNO::MFC_HELPER::CComboxbox_util::
setComboBoxSetHeight(
	CComboBox* pComboBox)
{
	if (pComboBox)
	{		
		CDC* const pDC = pComboBox->GetDC();
		if (pDC)
		{
			CString str;
			CSize sz;
			const int Count = pComboBox->GetCount();
			for (int i=0;i<Count;i++)
			{
				pComboBox->GetLBText( i, str );
				sz = pDC->GetTextExtent(str);
				pComboBox->SetItemHeight( i, sz.cy );
			}
			pComboBox->ReleaseDC(pDC);	
			return TRUE;
		}
	}
	return FALSE;	
};


BOOL DUOTECNO::MFC_HELPER::CComboxbox_util::
getComboBoxStrCurrSel(
	CComboBox& refComboBox,
	CString& refString)
{
	BOOL fResult = FALSE;
	const int selected = refComboBox.GetCurSel( );
	if (selected >= 0)
	{
		refComboBox.GetLBText(selected,refString);
		fResult=TRUE;
	}
	return fResult;
}

BOOL DUOTECNO::MFC_HELPER::CComboxbox_util::
getComboBoxCurrSelDataItem(
	CComboBox& refComboBox,
	DWORD_PTR* pData)
{
	BOOL fResult = FALSE;
	const int selected = refComboBox.GetCurSel( );
	if (selected >= 0)
	{
		*pData=refComboBox.GetItemData(selected);
		fResult=TRUE;
	}
	return fResult;
}

int DUOTECNO::MFC_HELPER::CComboxbox_util::setComboBoxItem(CComboBox& refComboBox,const CString& str,DWORD dwDataItem) {
	const int index = refComboBox.AddString(str);
	if (index < 0) {
		return -1;
	}
	refComboBox.SetItemData(index,dwDataItem);
	return index;
}

BOOL DUOTECNO::MFC_HELPER::CComboxbox_util::setComboBoxItemSelect(CComboBox& refComboBox, int targetValue,int percentageDiff) {
	int smallestDifference = INT_MAX;
	const double tolerance = (percentageDiff > 0) ? (percentageDiff / 100.0) : 0.0;
	int lowerBound = static_cast<int>(targetValue * (1.0 - tolerance));
	int upperBound = static_cast<int>(targetValue * (1.0 + tolerance));

	int selectedItem = -1;
	const int count = refComboBox.GetCount();
	for (int i = 0; i < count; ++i) {
		const int value = refComboBox.GetItemData(i);
		if (value == CB_ERR)
			continue;

		if (value >= lowerBound && value <= upperBound) {
			const int diff = abs(targetValue - value);
			if (diff < smallestDifference) {
				smallestDifference = diff;
				selectedItem = i;
			}
		}
	}
	refComboBox.SetCurSel(selectedItem);
	return selectedItem >= 0;
}

/*************************************************************/
};
};
/*************************************************************/