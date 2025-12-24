#if !defined(AFX_PROPERTYWIZARD_H__33AD7FB3_416D_46FC_BF44_EA4C5FA3462C__INCLUDED_)
#define AFX_PROPERTYWIZARD_H__33AD7FB3_416D_46FC_BF44_EA4C5FA3462C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizard.h : header file
//

namespace DUOTECNO {
	class CDlgHelperFont;
};
/////////////////////////////////////////////////////////////////////////////
// CPropertyWizard


class CPropertyWizardPageBase;
/////////////////////////////////////////////////////////////////////////////
typedef enum 
{
	// Pre browse
	WIZARD_CONTINUE_FACTORYADDRESS = 0,
	WIZARD_CONTINUE_DUPLICATESFOUND,
	WIZARD_CONTINUE_FACTORYADDRESS_ASSIGN_MASTER,		// sedert V16.44
	WIZARD_NEW_FACTORYADDRESS,
	WIZARD_NEW_DUPLICATESFOUND,
	WIZARD_NEW_FACTORYADDRESS_ASSIGN_MASTER,			// sedert V16.44

	// Post browse.
	WIZARD_CONTINUE_NODESADDED,
	WIZARD_CONTINUE_NODESREMOVED,
	WIZARD_CONTINUE_NODESINCOMPATIBLE,
	WIZARD_USER_SELECTION,
	WIZARD_CONTINUE_PROGRAMFILEDIFFERENCES,
	WIZARD_CONTINUE_NAMEDIFFERENCES,
	WIZARD_LICENCE_ACTIVATION_CODE,

	WIZARD_TASK_UNKNOWN = 0xFF
	
} WizardTasks_t;


typedef enum 
{
	WIZARDPAGE_INFO						= 0,
	WIZARDPAGE_CHOICES					= 1,
	WIZARDPAGE_CHOICE_PNP				= 2,
	WIZARDPAGE_CHOICE_REPLACE			= 3,	
	WIZARDPAGE_CHOICE_PROGRAMFILE		= 4,	
	WIZARDPAGE_CHOICE_MANUALASSIGN		= 5,
	WIZARDPAGE_CHOICE_ASSIGNMASTER		= 6,			// V16.44
	WIZARDPAGE_CHOICE_INIT				= 7,	
	WIZARDPAGE_CHOICE_NODESADDED		= 8,
	WIZARDPAGE_CHOICE_NODESINCOMPATIBLE = 9,
	WIZARDPAGE_CHOICE_SENDPROGRAMFILE   = 10,
	WIZARDPAGE_CHOICE_COPYNAMES			= 11,	
	WIZARDPAGE_ENTER_ACCESSCODE			= 12,
	WIZARDPAGE_MAX

} WizardPages_t;


#define WIZARD_PAGE1	1
#define WIZARD_PAGE2	2
#define WIZARD_PAGE3	3
#define WIZARD_PAGE4	4
#define WIZARD_PAGE5	5
//--------------------------------------------------------------------------

#define MAX_PROPERTY_SEQUENCES		5

class CPropertyWizard;

class CPropertySequence
{
	CPropertyWizard* m_pWizard;
	BYTE m_Chain[WIZARDPAGE_MAX][MAX_PROPERTY_SEQUENCES];
	BYTE m_bActiveChain;

public:
	void Init(WizardTasks_t task,CPropertyWizard *pSheet);
	BOOL SetNextPage(WizardPages_t CurrentPage);
	BOOL SetPrevPage(WizardPages_t CurrentPage);
	void SetActiveChain(WizardPages_t StartPage);
	BOOL IsLastPage(WizardPages_t CurrentPage);
	BOOL IsActiveChain(WizardPages_t Chain);
};


//--------------------------------------------------------------------------


/****************************************************************************
 * CPropertyWizardPage0: Weergave van de fouten.
 * CPropertyWizardPage1: Mogelijke acties.
 * CPropertyWizardPage2: Automatisch herstellen van de addressen.
 * CPropertyWizardPage3: Manueel toekennen van een adres.
 * CPropertyWizardPage4: Auto pnp.
 * CPropertyWizardPage5: Vervangen van een device node.
 * CPropertyWizardPage6: Factory init van een node.
 * CPropertyWizardPage7: Verwijderen van een node / weergave van een nieuwe node.
 * CPropertyWizardPage8: Incompatible nodes.
 * CPropertyWizardPage9: Configuratie versturen naar het can-bus netwerk.
 * CPropertyWizardPage10: Copy names (sedert v16.16 niet meer gebruikt.)
 * CPropertyWizardPage11: Access licentie code (wordt niet meer gebruikt.)
 * CPropertyWizardPage12: Oplossen van de naam verschillen.
 ***************************************************************************/

class CPropertyWizard : public CPropertySheet		
{
	DECLARE_DYNAMIC(CPropertyWizard)
	
// Construction
public:
	CPropertyWizard(CWnd* pParentWnd,WizardTasks_t task = WIZARD_TASK_UNKNOWN);
	virtual BOOL OnInitDialog();

	enum {
		WM_NOTIFY_NO_MASTER_FOUND = WM_USER + 1,
		WM_NOTIFY_ADDRESS_ASSIGNED
	};
	
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyWizard();

	BOOL NextPage(WizardPages_t Current) {
		return (m_SequenceManager.SetNextPage(Current)); 
	}
	
	BOOL PrevPage(WizardPages_t Current) {
		return (m_SequenceManager.SetPrevPage(Current)); 
	}

	void SetActiveChain(WizardPages_t chain) {
		m_SequenceManager.SetActiveChain(chain);
	}
	
	BOOL IsLastPage(WizardPages_t Current) {
		return (m_SequenceManager.IsLastPage(Current));
	}

	BOOL IsActiveChain(WizardPages_t Current) {
		return (m_SequenceManager.IsActiveChain(Current));
	}

	void SetAutoPnp(BOOL flag = TRUE) { 
		m_fAutoPnp = flag; 
	}
	void SetSendNames(BOOL flag = TRUE)	{ 
		m_fSendNames = flag; 
	}
	void SetSendBindings(BOOL flag = TRUE) { 
		m_fSendBindings = flag; 
	}
	void SetBrowseAgain(BOOL flag = TRUE) { 
		m_fBrowseAgain = flag; 
	}
	
	BOOL IsAutoPnpSelected(void) { 
		return m_fAutoPnp;		
	};
	BOOL IsSendNamesSelected(void) { 
		return m_fSendNames;	
	};
	BOOL IsSendBindingsSelected(void) { 
		return m_fSendBindings; 
	};
	BOOL IsBrowseAgain(void) {
		return m_fBrowseAgain;
	}

	WizardTasks_t GetTask(void) const {
		return m_task; 
	};

	// Generated message map functions
protected:
		
	//{{AFX_MSG(CPropertyWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Sedert V16.44:
	// Zorgt ervoor bij het openen van de dialoog er een extra messagebox komt met de melding.
	LONG OnApplicationNotifyNoMasterFound(WPARAM wParam, LPARAM lParam);

	// Sedert V16.44:
	// wParam is het node address.
	LONG OnApplicationNotifyLogicalAddressAssigned(WPARAM wParam, LPARAM lParam);


	void AddPages(WizardTasks_t tasks);
	void InitTasks(void);
	void checkMasterFoundAndPromptUserIfNotFound();

private:
	WizardTasks_t m_task;
	BOOL m_fAutoPnp;
	BOOL m_fSendNames;
	BOOL m_fSendBindings;
	BOOL m_fBrowseAgain;						// Sedert V16.16: Standaard op true.
	CPropertyWizardPageBase* m_pages[WIZARDPAGE_MAX];
	CPropertySequence m_SequenceManager;
	CBitmap m_bmpWizardHeader;
	CBitmap m_bmpWizardWatermark;
	DUOTECNO::CDlgHelperFont* m_pDlgFontHelper;
};

/////////////////////////////////////////////////////////////////////////////

// Oppassen wanneer in testmode dan worden geen runtime events ontvangen van de applicatie.
// bvb: 
//	- OnApplicationNotifyLogicalAddressAssigned( ) wordt niet getriggered
struct CPropertWizardTest
{
	static BOOL showWizard(WizardTasks_t task);
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARD_H__33AD7FB3_416D_46FC_BF44_EA4C5FA3462C__INCLUDED_)
