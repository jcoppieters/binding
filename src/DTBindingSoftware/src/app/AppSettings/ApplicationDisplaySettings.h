// ApplicationDisplaySettings.h: interface for the CApplicationDisplaySettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONDISPLAYSETTINGS_H__CF8F44D0_6432_43DE_AF62_610DD0F5C8C7__INCLUDED_)
#define AFX_APPLICATIONDISPLAYSETTINGS_H__CF8F44D0_6432_43DE_AF62_610DD0F5C8C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CApplicationDisplaySettings  
{
protected:
	
	enum Display_Mode
	{
		DISPLAY_MODE_SIMPLE = 0,
		DISPLAY_MODE_BASIC,
		DISPLAY_MODE_EXTENDED
	
	} m_DisplayMode;


public:
	CApplicationDisplaySettings();
	virtual ~CApplicationDisplaySettings();

	BOOL IsModeNormal() const {
		return (m_DisplayMode == DISPLAY_MODE_BASIC);
	}
	BOOL IsModeExtended() const {
		return (m_DisplayMode == DISPLAY_MODE_EXTENDED);
	}
	BOOL IsModeSimple() const {
		return (m_DisplayMode == DISPLAY_MODE_SIMPLE);
	}

	void SetModeNormal() {
		m_DisplayMode = DISPLAY_MODE_BASIC;
	}
	void SetModeExtended() {
		m_DisplayMode = DISPLAY_MODE_EXTENDED;
	}
	void SetModeSimple() {
		m_DisplayMode = DISPLAY_MODE_SIMPLE;
	}
};

#endif // !defined(AFX_APPLICATIONDISPLAYSETTINGS_H__CF8F44D0_6432_43DE_AF62_610DD0F5C8C7__INCLUDED_)
