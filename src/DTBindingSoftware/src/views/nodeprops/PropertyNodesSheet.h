#if !defined(AFX_PROPERTYNODESSHEET_H__910595C8_A980_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_PROPERTYNODESSHEET_H__910595C8_A980_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyNodesSheet.h : header file
//

#include "MyDefinitions.h"		// nodig voor enum PropertyNodesSheetPage_t
/////////////////////////////////////////////////////////////////////////////
// CPropertyNodesSheet


class CCanNode;
class CPropertyPageNode;
/*********************************************************************/
class CPropertyNodesSheet;

class CPropertyNodesSheet_Holder
{
protected:
	static CPropertyNodesSheet* m_pPropertyNodesSheet;

public:
	static CPropertyNodesSheet* Get();
	static CPropertyNodesSheet* Create(CWnd* pParent,CCanNode* pNode,PropertyNodesSheetPage_t firstpage);
	static void Destroy();
};

/*********************************************************************/

class CPropertyNodesSheet : public CPropertySheet		
{
	DECLARE_DYNAMIC(CPropertyNodesSheet)
	CCanNode *pSelNode;

// Construction
public:
	CPropertyNodesSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, CCanNode* pNode = NULL,
						UINT iSelectPage = 0);

	void setPage(PropertyNodesSheetPage_t page);

// Operations
public:
	static int convertPageNr(PropertyNodesSheetPage_t page,CCanNode* pNode);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyNodesSheet)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyNodesSheet();

	//void DestroyPropertySheet(void);
	CCanNode* GetAssignedNode(void)		{return pSelNode;}

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyNodesSheet)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LONG OnNodeStatusChanged(WPARAM wParam,LPARAM lParam);
	void DestroyPropertySheet(void);
	int normalizePageNr(int page);		// sedert V16.14.

	typedef enum 
	{
		NODE_BINDINGS = 0,
		NODE_RESOURCES = 1,
		NODE_CONTROL = 2,
		NODE_SCREENMAPS = 3,
		NODE_BITMAPS = 4,
		NODE_BACKUP = 5,
		NODE_CONFIGURATION = 6,		
		NODE_HOMECLIENT_MASTER = 7,
		MAX_PAGES 

	}PROPERTY_NODES_PAGES;		// Oppassen: zie ook CPropertyNodesSheet::convertPageNr

	CPropertyPageNode* m_pPages[MAX_PAGES];
	CButton m_Button;
	int m_firstPage;			// sedert V16.14: Meegegevn in de ctor maar gebruikt in de OnInitDialog()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYNODESSHEET_H__910595C8_A980_11D8_B865_0050BAC412B1__INCLUDED_)
