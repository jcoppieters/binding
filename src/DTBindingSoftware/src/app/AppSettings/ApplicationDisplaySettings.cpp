// ApplicationDisplaySettings.cpp: implementation of the CApplicationDisplaySettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ApplicationDisplaySettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplicationDisplaySettings::CApplicationDisplaySettings() : 
	m_DisplayMode(DISPLAY_MODE_BASIC)
{

}

CApplicationDisplaySettings::~CApplicationDisplaySettings()
{

}
