// PropertyPageDaliBaseClass.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliBaseClass.h"

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
CPropertyPageDaliContext::CPropertyPageDaliContext( ) : m_flags(0) { }
CPropertyPageDaliContext::~CPropertyPageDaliContext( ) { }
/******************************************************************************/
void CPropertyPageDaliContext::setFlag(Flags_t flag) {
	this->m_flags |= flag;
}

void CPropertyPageDaliContext::clearFlag(Flags_t flag) {
	this->m_flags &= ~flag;
}

BOOL CPropertyPageDaliContext::getFlag(Flags_t flag) const {
	return (flag == (this->m_flags & flag));
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
CPropertyPageDaliBaseClass::CPropertyPageDaliBaseClass(unsigned short dialogID,CAppDlgDaliWorker& refWorker,
													   CPropertyPageDaliContext& refContext) :
CPropertyPage(dialogID), m_dialogID(dialogID), m_refWorker(refWorker), m_refContext(refContext), 
	m_errorString() { }
	
CPropertyPageDaliBaseClass::~CPropertyPageDaliBaseClass() { }


// Weergave van de fout berichten moet assynchroon gebeuren.
void CPropertyPageDaliBaseClass::setErrorInfo(const CString& strMessage) {
	this->m_errorString = strMessage;
}
const CString& CPropertyPageDaliBaseClass::getErrorInfo() const {
	return this->m_errorString;
}
	
void CPropertyPageDaliBaseClass::clearErrorInfo( ) {
	this->m_errorString.Empty();
}
	
/******************************************************************************/


