// ApplicationDlgAppearance.cpp: implementation of the CApplicationDlgAppearance class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ApplicationDlgAppearance.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CApplicationDlgAppearanceBase::CApplicationDlgAppearanceBase() :
m_szTitle("Task Title"),
m_szMessage("Task Message: Busy,Done,Error")
{

}

CApplicationDlgAppearanceBase::CApplicationDlgAppearanceBase(CString szTitle, CString szMessage) :
m_szTitle(szTitle),
m_szMessage(szMessage)
{
}

CApplicationDlgAppearanceBase::~CApplicationDlgAppearanceBase()
{

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CApplicationDlgAppearance::CApplicationDlgAppearance()
{
	m_ProgressAppearance = TASK_PROGRESSBAR_INVISIBLE;
	m_ButtonAppearance = TASK_BUTTON_DONE;
}

CApplicationDlgAppearance::CApplicationDlgAppearance(CString szTitle, CString szMessage, 
							  ENUM_TASK_PROGRESS_APPEARANCE ProgressAppearance,
							  ENUM_TASK_BUTTON_APPEARANCE ButtonAppearance) :
CApplicationDlgAppearanceBase(szTitle,szMessage),
m_ProgressAppearance(ProgressAppearance),
m_ButtonAppearance(ButtonAppearance)
{
}

CApplicationDlgAppearance::~CApplicationDlgAppearance()
{

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CApplicationDlgInfoAppearance::CApplicationDlgInfoAppearance()
{
	m_DialogMode = ENUM_DIALOG_MODE_INFORMATION;
}

CApplicationDlgInfoAppearance::CApplicationDlgInfoAppearance(CString szTitle, CString szMessage, 
								ENUM_APP_DIALOG_MODE DialogMode) :
CApplicationDlgAppearanceBase(szTitle,szMessage),
m_DialogMode(DialogMode)
{
}

CApplicationDlgInfoAppearance::~CApplicationDlgInfoAppearance()
{
}







