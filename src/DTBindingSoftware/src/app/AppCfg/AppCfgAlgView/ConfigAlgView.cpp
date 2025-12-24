#include "stdafx.h"
#include "ConfigAlgView.h"

CConfigAlgView::CConfigAlgView(CCanNode* pNode,IViewDisplay* pNotifyView) :
m_refCanNode(*pNode), m_pNotifyView(pNotifyView) { }

CConfigAlgView::~CConfigAlgView(void) { }

void CConfigAlgView::DisplayMessage(const char* s) {
	if (m_pNotifyView) {
		m_pNotifyView->AddMessage(s);
	}
}

void CConfigAlgView::DisplayErrors(DUOTECNO::MFC_HELPER::CArrayErrors& arrErrors) {
	const int errors = arrErrors.GetNrErrors( );	
	if (errors != 0) {										
		for (int i=0;i<errors;i++) {
			const char* szString = 0;
			if (arrErrors.GetErrors(i,&szString)) {						
				DisplayMessage(szString);		
			}
		}				
	}
}

/*************************************************/
/*************** Utility class *******************/
/*************************************************/
BOOL CConfigAlgView_Util::getInfoStrings(CStringArray* messages,StringID_t id) {	
	if (0 == messages) {
		return FALSE;
	}
	switch(id) {
		case STRINGS_ID_SCREENMAP_PROAPP: {
			messages->RemoveAll( );
			messages->Add("Duotecno Pro-App config screenmapping");
			messages->Add("-----------------------------------------------------------");
			messages->Add("The screenmap configuration is only needed for the Duotecno Pro App");	
			messages->Add("Use the 'Duotecno Pro App config' in the 'Configuration' tab to");
			messages->Add("upload the screenmaps.");					  
		} return TRUE;	
	}
	return FALSE;
}