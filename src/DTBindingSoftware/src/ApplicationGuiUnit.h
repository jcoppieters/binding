// ApplicationGuiUnit.h: interface for the CApplicationGuiUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONGUIUNIT_H__84D027EB_525D_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_APPLICATIONGUIUNIT_H__84D027EB_525D_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ApplicationGraphics.h"
#include "TXUnitMessage.h"
struct STRUCT_UNIT;


class CApplicationGuiUnit  
{
public:
	static CApplicationGuiUnit* GetInstance();

protected:
	CWnd *m_pParent;
	BOOL fPulsToggleTimer;
	BYTE bSelNodeAddress,bSelUnitAddress;
	CTXUnitMessage *pTXUnitMsg;

	void ClickShort(BYTE bNodeAddress,BYTE bUnitAddress);
	void ClickLongOn(BYTE bNodeAddress,BYTE bUnitAddress);
	void ClickLongOff(BYTE bNodeAddress,BYTE bUnitAddress);

public:
	CApplicationGuiUnit(CWnd *pParent);
	virtual ~CApplicationGuiUnit();

	CTXUnitMessage *GetMessagePtr(void) {
		return pTXUnitMsg; 
	}

	typedef enum {
		APPUI_ICONS_INFO_NONE = 0,
		APPUI_ICONS_INFO_DALI = 1,
	
	} AppUiIconsExtraInfo_t;

	BOOL GetIcon(BYTE bNodeAddress,UnitIconType *iImage);
	BOOL GetIcon(BYTE bNodeAddress,BYTE bUnitAddress,AppUiIconsExtraInfo_t info,UnitIconType *iImage);				
	HICON GetIcon(BYTE bNodeAddress,BYTE bUnitAddress);
	void ClickOn(BYTE bNodeAddress,BYTE bUnitAddress);
	void ClickOff(BYTE bNodeAddress,BYTE bUnitAddress);
	void OnTimerExpired();
	BOOL GetRuntimeUnit(BYTE bNodeAddress,BYTE bUnitAddress,STRUCT_UNIT **pRuntimeUnit);	
};


#endif // !defined(AFX_APPLICATIONGUIUNIT_H__84D027EB_525D_11D8_B865_0050BAC412B1__INCLUDED_)
