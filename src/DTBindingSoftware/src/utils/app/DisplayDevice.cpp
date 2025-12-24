// DisplayDevice.cpp: implementation of the CDisplayDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DisplayDevice.h"

#include "MainFrm.h"
#include "MyWindowMessages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDisplayDevice::CDisplayDevice() { }
CDisplayDevice::~CDisplayDevice() { }

void CDisplayDevice::DisplayInfoMsg(const CString& szMessage) {
	if (AfxGetMainWnd() != NULL) {
		AfxGetMainWnd()->SendMessage(WM_DISPLAY_MESSAGE,(WPARAM) ((LPCTSTR)szMessage),0);
	}
}

void CDisplayDevice::DisplayErrorMsg(const CString& szMessage) {
	if (AfxGetMainWnd() != NULL) {
		AfxGetMainWnd()->SendMessage(WM_DISPLAY_MESSAGE,(WPARAM) ((LPCTSTR)szMessage),0);		
	}
}

void CDisplayDevice::Clear(void) {
	if (AfxGetMainWnd() != NULL) {
		AfxGetMainWnd()->SendMessage(WM_CLEAR_MESSAGES);					     
	}
}

void CDisplayDevice::DisplayTracerErrorMsg(const CString& szMessage) {
	if (AfxGetMainWnd() != NULL) {
		AfxGetMainWnd()->SendMessage(WM_DISPLAY_TRACER_MESSAGEERROR,(WPARAM) ((LPCTSTR)szMessage),0);
	}
}