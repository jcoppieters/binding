// PropertyPageDaliBaseClass.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliErrorsBaseClass.h"
#include "app/appdali/appdali_common.h"
/******************************************************************************/
using namespace DUOTECNO::DALI;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
CPropertyPageDaliErrorsContext::CPropertyPageDaliErrorsContext( ) : m_pDaliFsmErrors(NULL) { }

CPropertyPageDaliErrorsContext::~CPropertyPageDaliErrorsContext( ) {
	this->deallocate();
}

void CPropertyPageDaliErrorsContext::deallocate() {
	if (NULL != m_pDaliFsmErrors) {
		delete m_pDaliFsmErrors;
		m_pDaliFsmErrors = NULL;
	}
}


void CPropertyPageDaliErrorsContext::setFsmErrors(CAppDaliFsmErrorEntries* pDaliFsmErrors) {
	this->deallocate( );
	this->m_pDaliFsmErrors = pDaliFsmErrors;
}

DUOTECNO::DALI::CAppDaliFsmErrorEntries* CPropertyPageDaliErrorsContext::getData( ) {
	return this->m_pDaliFsmErrors;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
CPropertyPageDaliErrorsBaseClass::CPropertyPageDaliErrorsBaseClass(unsigned short dialogID,CAppDlgDaliWorker& refWorker,CPropertyPageDaliErrorsContext& refContext) :
	CPropertyPage(dialogID), m_dialogID(dialogID), m_refWorker(refWorker), m_refContext(refContext), m_initDone(false) { }
	
CPropertyPageDaliErrorsBaseClass::~CPropertyPageDaliErrorsBaseClass() { 
	
}

bool CPropertyPageDaliErrorsBaseClass::isInitDone( ) const {
	return this->m_initDone;
}
void CPropertyPageDaliErrorsBaseClass::setInitDone(bool status) {
	this->m_initDone = status;
}

/******************************************************************************/
