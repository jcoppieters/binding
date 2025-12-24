// PropertyPageDaliErrorsDetail.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliErrorsDetail.h"
#include "app/appdali/appdali_common.h"
#include "appdlgdaliworker.h"						// Voorlopig hier ????
#include "utils/mfc/controls_util.h"

using namespace DUOTECNO::MFC_HELPER;
using namespace DUOTECNO::DALI;

// CPropertyPageDaliErrorsDetail dialog

IMPLEMENT_DYNAMIC(CPropertyPageDaliErrorsDetail, CPropertyPage)

CPropertyPageDaliErrorsDetail::CPropertyPageDaliErrorsDetail(CAppDlgDaliWorker& refWorker,CPropertyPageDaliErrorsContext& refContext)
	: CPropertyPageDaliErrorsBaseClass(CPropertyPageDaliErrorsDetail::IDD, refWorker,refContext)
{

}

CPropertyPageDaliErrorsDetail::~CPropertyPageDaliErrorsDetail()
{
}

void CPropertyPageDaliErrorsDetail::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DALI_ERRORS, m_listErrorDetails);
}


BEGIN_MESSAGE_MAP(CPropertyPageDaliErrorsDetail, CPropertyPage)
END_MESSAGE_MAP()


// CPropertyPageDaliErrorsDetail message handlers
BOOL CPropertyPageDaliErrorsDetail::OnInitDialog() {
	CPropertyPage::OnInitDialog();	
	if (!this->isInitDone()) {	
		this->initListCtrl( );	
		this->updateListCtrl( );
		this->setInitDone(true);
	}
	return TRUE;
}

void CPropertyPageDaliErrorsDetail::initListCtrl( ) {

	// TODO: tekst niet in de center maat left alligned?

	int nCol = -1;
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listErrorDetails,COLUMN_DEVICE_ADDRESS,"Address",50);
	ASSERT(COLUMN_DEVICE_ADDRESS == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listErrorDetails,COLUMN_DEVICE_TYPE,"Type",100);
	ASSERT(COLUMN_DEVICE_TYPE == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listErrorDetails,COLUMN_DEVICE_DESCRIPTION,"Problem",420);
	ASSERT(COLUMN_DEVICE_DESCRIPTION == nCol);
}

void CPropertyPageDaliErrorsDetail::updateListCtrl( ) {
	DUOTECNO::DALI::CAppDaliFsmErrorEntries* pErrors = this->getContext().getData();
	if (NULL != pErrors) {	
		const int count = pErrors->getCount();
		for (int i=0;i<count;++i) {
			CAppDaliFsmErrorEntry* pEntry = NULL;
			if (pErrors->get(i,&pEntry)) {
				ASSERT(NULL != pEntry);
				this->addEntry(*pEntry);
			}
		}
	}
}


void CPropertyPageDaliErrorsDetail::addEntry(DUOTECNO::DALI::CAppDaliFsmErrorEntry& r) {

	const int itemIndex = m_listErrorDetails.GetItemCount();

	LVITEM lvItem;	
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = itemIndex;				// Op het einde van de lijst bij voegen..
	lvItem.iSubItem = 0;
	lvItem.pszText = "name"; 


	const DWORD dwItemData = 0;		// TODO...	<-- Foutmelding.

// TODO:
// Moeten we nog ergens een extended error bericht weergeven???


	CString strDeviceAddress;
	CString strType;
	CString strErrorDescription;

	if (r.getShortAddress() >= 0) {
		CAppDlgDaliWorker::formatStringAddress(r.getDaliDeviceType(),r.getShortAddress(),&strDeviceAddress);
		strType = (DALI_DEVICE_TYPE_CONTROLGEAR == r.getDaliDeviceType() ? "control gear" : "control input");
	}
	else {
		strDeviceAddress = "N/A";
		strType = "N/A";
	}

	if (!CAppDlgDaliWorker::formatStringProblem(r.getErrorCode(),&strErrorDescription)) {
		strErrorDescription = "General error";
	}

	const int nItem = m_listErrorDetails.InsertItem(&lvItem);
	ASSERT(nItem>=0);	
	m_listErrorDetails.SetItemText(nItem, COLUMN_DEVICE_ADDRESS, strDeviceAddress);	
	m_listErrorDetails.SetItemText(nItem, COLUMN_DEVICE_TYPE, strType);			
	m_listErrorDetails.SetItemText(nItem, COLUMN_DEVICE_DESCRIPTION, strErrorDescription);
	m_listErrorDetails.SetItemData(nItem, dwItemData);
}