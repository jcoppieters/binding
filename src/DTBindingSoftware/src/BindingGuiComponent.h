// BindingGuiComponent.h: interface for the CBindingGuiComponent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGGUICOMPONENT_H__F7A32861_7CCB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGGUICOMPONENT_H__F7A32861_7CCB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
#include "BindingConstants.h"

class CNode1;
class CAddFlow1;
class CLink1;

class CBindingEntry;
class CBinding2Controls;
class CBindingConnections;

class CBindingUnit;
class CBindingOperator;

struct STRUCT_UNIT;

//////////////////////////////////////////////////////////////////////
//class CBindingGuiComponent;


class CFlowLayout;

class CBindingGuiComponent  
{
	CAddFlow1 *m_pAddFlow;
	CFlowLayout *pLayoutManager;
	CBindingEntry *pCurrBindingEntry;
	CBindingConnections *pConnections;
	CBinding2Controls *pControls;
	CWnd *m_pParent;
	BOOL fDragging;
	int m_CurrentBindingUnitID;				// Sedert V16.49: Identifier van de binding unit voor de property dialoog geopend werd.

public:
	CBindingGuiComponent();
	virtual ~CBindingGuiComponent();

	void Init(CAddFlow1 *pAddFlow,CWnd *pParent);
	BOOL OnUnitStatusChanged(BYTE bNodeAddress,BYTE bUnitAddress);
	void AddControl(OperatorType Type);
	void AddControl(STRUCT_UNIT *pUnit);
	void CreateBitmaps(void);

	// Drag & drop
	void DropControl(void);
	void DragControl(STRUCT_UNIT *pUnit,int x,int y);

	BOOL DeleteControl(void);
	BOOL DeleteLink(void);
	BOOL AddLink(void);

	BOOL SetNodeSelected(BOOL fSelected);
	BOOL SetLinkSelected(BOOL fSelected);
	BOOL SetLinkInverse(BOOL fInverse);
	BOOL SetToggleLink(void);

	void PrepareToMove(BOOL fMove,float xPos=0,float yPos=0);
	void MoveSelectedNode(float xPos,float yPos);
	void MoveNode(CNode1 node,float xPos,float yPos);
	void ShowPropDlg(int xpos,int ypos);
	void DestroyPropDlg(void);

	void SetPtrToBindingEntry(CBindingEntry *pEntry)
	{
		pCurrBindingEntry = pEntry; 
	};

	CBindingEntry* GetCurrBindingEntry(void)		 
	{ 
		return pCurrBindingEntry; 
	};

	CFlowLayout* GetLayoutManager(void) 
	{
		return pLayoutManager; 
	};
	CAddFlow1 *GetAddFlowComponent(void) 
	{
		return m_pAddFlow; 
	};
	CBindingConnections *GetConnections(void) 
	{
		return pConnections; 
	};
	CBinding2Controls *GetControls(void) 
	{
		return pControls; 
	};

protected:
	// Sedert V16.46: Tooltip text van de Controls updaten.
	// Deze geven ifv de onderliggende functie.
	// Refreshen van alle controls.
	BOOL refreshControlsToolTipText( );

	// Sedert V16.46: Tooltip text van de Controls updaten.
	// Deze geven ifv de onderliggende functie.
	// Refreshen van een control na het openen van de properties dialoog.
	BOOL refreshControlToolTipText(int bindingUnitIdentifier);
};


///////
class CFlowLayout
{
	private:
		CBindingGuiComponent *m_pParent;
		void AddNode(CBindingUnit *pUnit);
		void AddNode(CBindingOperator *pOperator);
	
	public:
		CFlowLayout(CBindingGuiComponent *pParent);
		~CFlowLayout();
		BOOL AddControl(BYTE bUnitType,float *x,float *y);
		BOOL AddControl(OperatorType Type,float *x,float *y);

	public:
		void UpdateScreen(BOOL fRedraw);
		void ClearScreen(void);		
};

#endif // !defined(AFX_BINDINGGUICOMPONENT_H__F7A32861_7CCB_11D8_B865_0050BAC412B1__INCLUDED_)
