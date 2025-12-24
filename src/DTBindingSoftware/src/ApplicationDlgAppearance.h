// ApplicationDlgAppearance.h: interface for the CApplicationDlgAppearance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONDLGAPPEARANCE_H__AE5C01F7_5836_4A7A_9F18_15FA4E3A5778__INCLUDED_)
#define AFX_APPLICATIONDLGAPPEARANCE_H__AE5C01F7_5836_4A7A_9F18_15FA4E3A5778__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////
class CApplicationDlgAppearanceBase
{
protected:
	CString m_szTitle;
	CString m_szMessage;

public:
	CApplicationDlgAppearanceBase();
	CApplicationDlgAppearanceBase(CString szTitle, CString szMessage);
	virtual ~CApplicationDlgAppearanceBase();


	CString GetTitle() const {
		return m_szTitle;
	}
	CString GetMessage() const {
		return m_szMessage;
	}
};

//////////////////////////////////////////////////////
typedef enum
{
	TASK_PROGRESSBAR_NORMAL		= 0,
	TASK_PROGRESSBAR_ERROR,
	TASK_PROGRESSBAR_DONE,
	TASK_PROGRESSBAR_INVISIBLE

} ENUM_TASK_PROGRESS_APPEARANCE;

typedef enum
{
	TASK_BUTTON_CANCEL			= 0,
	TASK_BUTTON_DONE,
	TASK_BUTTON_ERROR,
	TASK_BUTTON_INVISIBLE

} ENUM_TASK_BUTTON_APPEARANCE;


/***************************************************************************
 *
 *
 *
 ****************************************************************************/
class CApplicationDlgAppearance : public CApplicationDlgAppearanceBase
{
protected:
	ENUM_TASK_PROGRESS_APPEARANCE m_ProgressAppearance;
	ENUM_TASK_BUTTON_APPEARANCE m_ButtonAppearance;

public:
	CApplicationDlgAppearance();
	CApplicationDlgAppearance(CString szTitle, CString szMessage, 
							  ENUM_TASK_PROGRESS_APPEARANCE ProgressAppearance,
							  ENUM_TASK_BUTTON_APPEARANCE ButtonAppearance);

	virtual ~CApplicationDlgAppearance();

public:
	ENUM_TASK_PROGRESS_APPEARANCE GetProgressProperties() const {
		return m_ProgressAppearance;
	}
	ENUM_TASK_BUTTON_APPEARANCE GetButtonProperties() const {
		return m_ButtonAppearance;
	}
};

//////////////////////////////////////////////////////


typedef enum
{
	ENUM_DIALOG_MODE_RETRY = 0,
	ENUM_DIALOG_MODE_CONFIRMATION,
	ENUM_DIALOG_MODE_INFORMATION

} ENUM_APP_DIALOG_MODE;


/****************************************************************************
 *
 *
 *
 *****************************************************************************/
class CApplicationDlgInfoAppearance : public CApplicationDlgAppearanceBase
{
protected:
	ENUM_APP_DIALOG_MODE m_DialogMode;

public:
	CApplicationDlgInfoAppearance();
	CApplicationDlgInfoAppearance(CString szTitle, CString szMessage, 
								  ENUM_APP_DIALOG_MODE DialogMode);

	virtual ~CApplicationDlgInfoAppearance();

public:
	ENUM_APP_DIALOG_MODE GetMode() const {
		return m_DialogMode;
	}
};

#endif // !defined(AFX_APPLICATIONDLGAPPEARANCE_H__AE5C01F7_5836_4A7A_9F18_15FA4E3A5778__INCLUDED_)
