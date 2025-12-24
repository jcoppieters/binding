// BindingGuiComponent.cpp: implementation of the CBindingGuiComponent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingGuiComponent.h"
#include "BindingConvertHelper.h"
#include "BindingEntry.h"
#include "MainFrm.h"
#include "MyWindowMessages.h"


#include "views/main/MyAddFlowSheet.h"			// ???

// CAddFlow1
#include "addflow\node1.h"
#include "addflow\Link1.h"
#include "addflow\Links1.h"
#include "addflow\nodes1.h"
#include "addflow\AddflowConst.h"
#include "addflow\AddFlow1.h"
#include "addflow\pictures.h"

#include "CanNode.h"

// --------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
// ---------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TWIPS_PER_INCH           1440
float XPixelsToTwips(int x) {
  HDC hdc = ::GetDC(NULL);
  long iTwipsPerPixel = TWIPS_PER_INCH / GetDeviceCaps(hdc, LOGPIXELSX);
  ::ReleaseDC(NULL, hdc); 
  return (float)(x * iTwipsPerPixel);
}      

float YPixelsToTwips(int y) {
  HDC hdc = ::GetDC(NULL);
  long iTwipsPerPixel = TWIPS_PER_INCH / GetDeviceCaps(hdc, LOGPIXELSY);
  ::ReleaseDC(NULL, hdc); 
  return (float)(y * iTwipsPerPixel);
}      


//////////////////////////////////////////////////////////////////////////////////////////
// Drag & Drop : Calculation coordinates --> When negative AddFlow returns error
// AFSize unit = 1000 --> 500 offset positions mouse pointer in middle...
float CheckXCoordinates(float x) {
	if (x <= 500) x =500;
	if (x >= 20000) x = 20000;	
	return (x-500);
}
float CheckYCoordinates(float y) {
	if (y <= 500) y =500;
	if (y >= 20000) y = 20000;
	return (y-500);
}
//////////////////////////////////////////////////////////////////////////////////////////

CFlowLayout::CFlowLayout(CBindingGuiComponent *pParent)
{
	m_pParent = pParent;
}

CFlowLayout::~CFlowLayout()
{	
}


////////////////////////////////////////////////////////////////////
void CFlowLayout::ClearScreen(void)
{
	CAddFlow1 *pAddFlow;
	pAddFlow = m_pParent->GetAddFlowComponent();
	pAddFlow->SelectAll();
	pAddFlow->DeleteSel();
}

void CFlowLayout::UpdateScreen(BOOL fRedraw)
{
	//CBindingUnit *pUnit;
	CBindingOperator *pOperator;
	int i;
	float xPos,yPos;
	CNode1 node;

	CBindingEntry *pBindingEntry = m_pParent->GetCurrBindingEntry();

	if (1)
	{
		CBindingUnits::Iterator it(pBindingEntry->GetColUnit()->CreateIterator());
		for (it.Begin();!it.IsDone();it.Next())
		{
			CBindingUnit* const pUnit = it.CurrentItem( );
			ASSERT( pUnit );

			if (fRedraw == FALSE) 
			{
				pUnit->GetPosition(&xPos,&yPos);
				node = m_pParent->GetControls()->GetControl(pUnit->GetID(),PM_UNIT);	
				ASSERT(node);
				m_pParent->MoveNode(node,xPos,yPos);
			}
			else
			{
				m_pParent->GetControls()->AddNode(pUnit);
			}
		}
	}

	/** @todo	Via iterators */
	const int nOperators = pBindingEntry->GetColOperator()->Count();
	for (i=0;i<nOperators;i++) {
		if ((pOperator = pBindingEntry->GetColOperator()->Get(i)) != 0) {		
			if (fRedraw == FALSE) {
				pOperator->GetPosition(&xPos,&yPos);
				if (pOperator->GetType() == PM_OPERATOR_TIMER) {
					node = m_pParent->GetControls()->GetControl(pOperator->GetID(), PM_TIMEROPERATOR);
				}
				else {
					node = m_pParent->GetControls()->GetControl(pOperator->GetID(), PM_OPERATOR);
				}		
				ASSERT(node);			
				m_pParent->MoveNode(node,xPos,yPos);
			}
			else {
				m_pParent->GetControls()->AddNode(pOperator);
			}
		}
		ASSERT(pOperator);
	}

	if (fRedraw == TRUE)
	{
		// Reconstruct Links ...	
		CBindingLink *pLink;

		/** @todo	Via iterators */
		const int nLinkCount = pBindingEntry->GetColLink()->Count();
		for (i=0;i<nLinkCount;i++)
		{
			if ((pLink = pBindingEntry->GetColLink()->Get(i)) != 0)
			{
				m_pParent->GetConnections()->Addlink(pLink);
			}

			ASSERT(pLink);
		}
	}
}

BOOL CFlowLayout::AddControl(OperatorType Type,float *x,float *y)
{
	int i,nOperators;
	float xPos,yPos;
	float x2Pos,y2Pos;
	CBindingEntry *pBindingEntry = NULL;
	CBindingUnit *pUnit = NULL;
	CBindingOperator *pOperator = NULL;

	pBindingEntry = m_pParent->GetCurrBindingEntry();
	ASSERT(pBindingEntry);

	nOperators = pBindingEntry->GetColOperator()->Count();
	x2Pos = 2000;
	y2Pos = 440;

	if (nOperators == 0) 
	{
		*x = x2Pos; *y = y2Pos;
		return TRUE;
	}

	for (i=0;i<nOperators;i++) 
	{		
		if ((pOperator = pBindingEntry->GetColOperator()->Get(i)) == 0)
		{
			ASSERT(pOperator);
			return FALSE;
		}

		pOperator->GetPosition(&xPos,&yPos);

		if (xPos >= x2Pos) x2Pos = xPos;
		if (yPos >= y2Pos) y2Pos = yPos;
	}

	*x = x2Pos;
	*y = y2Pos + 1040;
	return TRUE;
}

BOOL CFlowLayout::AddControl(BYTE bUnitType,float *x,float *y)
{
	int i;
	float xPos,yPos;
	float x2Pos,y2Pos;
	CBindingEntry *pBindingEntry = NULL;
	CBindingOperator *pOperator = NULL;
	BOOL fInput = FALSE;

	pBindingEntry = m_pParent->GetCurrBindingEntry();
	ASSERT(pBindingEntry);
	
	if (bUnitType == UNIT_TYPE_CONTROL || bUnitType == UNIT_TYPE_RC5RX ||
	    bUnitType == UNIT_TYPE_LCD_VIRTUAL || bUnitType == UNIT_TYPE_SENS)
	{
		fInput = TRUE;
	}

	xPos = 0; x2Pos = 0;
	yPos = 0; y2Pos = 0;

	if (fInput == FALSE) 
	{
		// get n-opertaors --> Xpos;
		const int nOperators = pBindingEntry->GetColOperator()->Count();

		/** @todo Via iterators */
		for (i=0;i<nOperators;i++) 
		{		
			if ((pOperator = pBindingEntry->GetColOperator()->Get(i)) == 0)
			{
				ASSERT(pOperator);
				return FALSE;
			}

			pOperator->GetPosition(&xPos,&yPos);

			if (xPos >= x2Pos) x2Pos = xPos;
		}
	}

	if (fInput == TRUE) 
	{			
		x2Pos = 200; y2Pos = 200;
	}
	else 
	{
		if (x2Pos==0) x2Pos = 3000;
		else x2Pos += 400;
		y2Pos = 200;
	}

	// 1 Nr of Inputs == 0
	// const int nUnits = pBindingEntry->GetColUnit()->Count();
	if (pBindingEntry->GetColUnit()->Count() == 0) 
	{
		*x = x2Pos;
		*y = y2Pos;
		return TRUE;
	}

	// 2 Nr of Inputs != 0 --> StartValues ifv Type;
	if (1)
	{
		CBindingUnits::Iterator it(pBindingEntry->GetColUnit()->CreateIterator());
		for (it.Begin();!it.IsDone();it.Next())
		{
			CBindingUnit* const pUnit = it.CurrentItem();
			ASSERT( pUnit );

			pUnit->GetPosition(&xPos,&yPos);
		
			if (fInput == TRUE) 
			{	
				if (xPos <= x2Pos) x2Pos = xPos;
				if (yPos >= y2Pos) y2Pos = yPos;
			}
			else 
			{
				if (xPos >= x2Pos) x2Pos = xPos;
				if (yPos <= y2Pos) y2Pos = yPos;
			}
		}
	}

	if (fInput == TRUE) 
	{
		*x = x2Pos;
		*y = y2Pos + 1400;
	}
	else 
	{
		*x = x2Pos + 1400;
		*y = y2Pos;
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingGuiComponent::CBindingGuiComponent()
{
	pLayoutManager = NULL;	
	pConnections = NULL;
	pControls = NULL;
	m_pParent= NULL;
}

CBindingGuiComponent::~CBindingGuiComponent()
{
	if (pLayoutManager != NULL) 
		delete pLayoutManager;
	if (pConnections != NULL) 
		delete pConnections;
	if (pControls != NULL) 
		delete pControls;
}


void CBindingGuiComponent::CreateBitmaps(void) 
{
	ASSERT(m_pAddFlow);	

	// Create Pictures ... NEW 20040525
	CPictureHolder picture;
	CPictures Afpictures;

	// Operators ...
	Afpictures = m_pAddFlow->GetPictures();
	Afpictures.Clear();

	picture.CreateFromIcon(TMGetGraphics()->OperatorIcons[ICON_AND]);
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->OperatorIcons[ICON_OR]);
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->OperatorIcons[ICON_XOR]);
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->OperatorIcons[ICON_TIMER]);
	Afpictures.Add(picture.GetPictureDispatch());

	// Units ...
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_VIRTUAL]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_VIRTUAL_ON]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_VIRTUAL_DALI]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_VIRTUAL_DALI_ON]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_CONTROL]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_CONTROL_ON]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DALI_CONTROL]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DALI_CONTROL_ON]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DIMMER]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DIMMER_ON]);  
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DIMMER_PIR]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DIMMER_EC]);		
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DALI_DIMMER]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DALI_DIMMER_ON]);  
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DALI_DIMMER_PIR]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_DALI_DIMMER_EC]);		
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_MOTOR]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_MOTOR_DN]);  
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_MOTOR_UP]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_IRRX]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_IRTX]);
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_SWITCH]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_SWITCH_ON]);  
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_SWITCH_PIR]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_SENS]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_AUDIO]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_AUDIO_ON]);  
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_BOSE]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_BOSE_ON]); 
	Afpictures.Add(picture.GetPictureDispatch());

	// TM 20041119
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_AVMATRIX]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_VIDEOMUX]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_ALARM]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_PS2MOUSE]); 
	Afpictures.Add(picture.GetPictureDispatch());

	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_NODE]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_NODE_MASTER]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_NODE_LCD]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_NODE_GATEWAY]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_UNDEFINED_NODE]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_UNDEFINED_UNIT]); 
	Afpictures.Add(picture.GetPictureDispatch());
	picture.CreateFromIcon(TMGetGraphics()->UnitIcons[ICON_NOT_IMPLEMENTED]); 
	Afpictures.Add(picture.GetPictureDispatch());	
}


void CBindingGuiComponent::Init(CAddFlow1 *pAddFlow,CWnd *pParent)
{
	ASSERT(pAddFlow);
	ASSERT(pParent);

	m_pAddFlow = pAddFlow;
	m_pParent = pParent;
	fDragging = FALSE;

	m_pAddFlow->SetDrawColor(RGB(0,0,0));
	m_pAddFlow->SetShape(afRectangle);
	m_pAddFlow->SetAutoSize(afNoAutoSize);
	m_pAddFlow->SetCanMoveNode(FALSE);					/* MOVE NODES */
	m_pAddFlow->SetCanDrawNode(FALSE);					/* DRAW NODES */
	m_pAddFlow->SetCanDrawLink(TRUE);
	m_pAddFlow->SetLinkCreationMode(afAllNodeArea);		/* LNK CREATION */
	m_pAddFlow->SetForeColor(RGB(0,0,255));				/* TXT COLOR BLUE */


	if (pConnections == NULL) pConnections = new CBindingConnections(this);
	if (pControls == NULL) pControls = new CBinding2Controls(this);
	if (pLayoutManager == NULL) pLayoutManager = new CFlowLayout(this);

}

/////////////////////////////////////////////////////////////////
BOOL CBindingGuiComponent::OnUnitStatusChanged(BYTE bNodeAddress,BYTE bUnitAddress) {
	if (pCurrBindingEntry == NULL) {
		return TRUE;
	}	
	CNodes1 nodes = m_pAddFlow->GetNodes();
	if (nodes) {		
		VARIANT vaIdx;
		VariantInit(&vaIdx);
		vaIdx.vt = VT_I4;
		const int count = nodes.GetCount();
		for (int i = 1; i <= count; i++) {
			vaIdx.lVal = i;
			CNode1 node = nodes.Item(vaIdx);
			if ((node) && (pControls->GetControlType(node) == PM_UNIT)) {			
				CBindingUnit* const pBindingUnit = pControls->GetUnit(node);					
				if ((NULL != pBindingUnit) && (pBindingUnit->GetNodeAddress() == bNodeAddress) && (pBindingUnit->GetUnitAddress() == bUnitAddress)) {
					pControls->AddIcon(&node, pBindingUnit);
				}
			}
		}
	}	
	return TRUE;
}

BOOL CBindingGuiComponent::DeleteControl(void) {
	CNode1 SelectedNode = m_pAddFlow->GetSelectedNode();	
	if (SelectedNode == NULL) {
		return FALSE;
	}
	pControls->Remove(SelectedNode);
	this->refreshControlsToolTipText();
	return TRUE;	
}

BOOL CBindingGuiComponent::DeleteLink(void) {
	CLink1 SelectedLink = m_pAddFlow->GetSelectedLink();	
	if (SelectedLink == NULL) {
		return FALSE;
	}
	pConnections->Remove(SelectedLink);
	this->refreshControlsToolTipText();
	return TRUE;
}

BOOL CBindingGuiComponent::AddLink(void) {
	CLink1 lnk = m_pAddFlow->GetSelectedLink();	
	if (lnk == NULL) {
		return FALSE;
	}		
	const BOOL res = pConnections->Add(lnk);
	if (res) {
		this->refreshControlsToolTipText();
	}
	return res;
}
/////////////////////////////////////////////////////////////////


/***********************************************************************
* Function :	AddControl(OperatorType)
* Invoked by :	TestAddFlowView
* Arguments :	OperatorType (AND,OR,XOR,TIMER)
*  
* Task : Add New Node To addFlow + Collections.
*		 Free properties of current selected node, 
*		 Set Selectednode = new Node
*        If New node is Timer, set default value.
*        Show properties of new added node .
************************************************************************/
void CBindingGuiComponent::AddControl(OperatorType Type)
{
	// Must Calculate Position f(Number of Controls) +
	// f(type - input,Output,...)

	float x=0,y=0;
	
	ASSERT(pLayoutManager);
	pLayoutManager->AddControl(Type,&x,&y);

	unsigned short nID = pControls->Add(x,y,Type);
	ASSERT(nID != 0);
	
	//SEND WM WITH SHOW PROPERTIES ...
}
void CBindingGuiComponent::AddControl(STRUCT_UNIT *pUnit)
{
	// Must Calculate Position f(Number of Controls) +
	// f(type - input,Output,...)

	CString szMessage;

	BOOL fSkip = FALSE;
	float x=0,y=0;

	szMessage.FormatMessage("Unit is already used in this binding."
						    "\nAre you sure you want to add this unit?");

	if (pControls->IsAlreadyPresent(pUnit)) 
	{
		if (AfxMessageBox(szMessage,MB_ICONQUESTION | MB_YESNO) == IDNO) 
		{
		
			fSkip = TRUE;
		}
	}


	if (fSkip == FALSE) 
	{

		ASSERT(pLayoutManager);
		pLayoutManager->AddControl(pUnit->bUnitType,&x,&y);
	
		unsigned short nID = pControls->Add(x,y,pUnit);
		ASSERT(nID != 0);
	
		//SEND WM WITH SHOW PROPERTIES
	}
}


/////////////////////////////////////////////////////////////////////////////////

void CBindingGuiComponent::DropControl(void)
{
	CNode1 node = m_pAddFlow->GetSelectedNode();
	pControls->SetPosition(node);
	fDragging = FALSE;
	SetNodeSelected(FALSE);
}

/*
void CBindingGuiComponent::DragControl(CString szName, BYTE bNodeAddress, BYTE bUnitAddress,BYTE bUnitType,int x,int y)
{

	static float xOffset=0;
	float xScrollPos = XPixelsToTwips(m_pAddFlow->GetScrollPos(afHorizontal));
	float yScrollPos = YPixelsToTwips(m_pAddFlow->GetScrollPos(afBoth));
	float xpos = XPixelsToTwips(x) + yScrollPos;
	float ypos = YPixelsToTwips(y) + xScrollPos;

	if (fDragging == FALSE) {
		fDragging = TRUE;
		ASSERT(m_pParent);
		CNode1 node = pControls->Add(szName,bNodeAddress,bUnitAddress,bUnitType);
		ASSERT(node);	
		m_pAddFlow->SetSelectedNode(node);	
		xOffset = XPixelsToTwips(x);
		MoveNode(node,0,ypos);
	}
	MoveNode(m_pAddFlow->GetSelectedNode(),CheckXCoordinates(xpos-xOffset),CheckYCoordinates(ypos));
}
*/

void CBindingGuiComponent::DragControl(STRUCT_UNIT *pUnit,int x,int y)
{
	static float xOffset=0;

	const float xScrollPos = XPixelsToTwips(m_pAddFlow->GetScrollPos(afHorizontal));
	const float yScrollPos = YPixelsToTwips(m_pAddFlow->GetScrollPos(afBoth));
	const float xpos = XPixelsToTwips(x) + yScrollPos;
	const float ypos = YPixelsToTwips(y) + xScrollPos;

	if (fDragging == FALSE) 
	{
		fDragging = TRUE;
		ASSERT(m_pParent);
	

		CNode1 node = pControls->Add(pUnit);

		ASSERT(node);	
		m_pAddFlow->SetSelectedNode(node);	
		xOffset = XPixelsToTwips(x);
		MoveNode(node,0,ypos);
	}
	
	MoveNode(m_pAddFlow->GetSelectedNode(),
			 CheckXCoordinates(xpos-xOffset),
			 CheckYCoordinates(ypos));
}

//////////////////////////////////////////////////////////////////
BOOL CBindingGuiComponent::SetNodeSelected(BOOL fSelected)
{
	CNode1 node = m_pAddFlow->GetSelectedNode();
	if (node != NULL)  {
		node.SetSelected(fSelected);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingGuiComponent::SetLinkSelected(BOOL fSelected)
{	
	if (m_pAddFlow->GetSelectedLink() != NULL)  {
		CLink1 lnk = m_pAddFlow->GetSelectedLink();
		lnk.SetSelected(FALSE);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingGuiComponent::SetLinkInverse(BOOL fInverse)
{
	CLink1 SelectedLink = m_pAddFlow->GetSelectedLink();
	if (SelectedLink != NULL) {	
		pConnections->Set(SelectedLink,fInverse);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingGuiComponent::SetToggleLink()
{
	CLink1 SelectedLink = m_pAddFlow->GetSelectedLink();
	if (SelectedLink != NULL) {	
		pConnections->Set(SelectedLink,TOGGLE);
		return TRUE;
	}
	return FALSE;
}
void CBindingGuiComponent::PrepareToMove(BOOL fMove,float xPos,float yPos)
{
	int i;
	CNodes1 nodes;
	CNode1 node;
	VARIANT vaIdx;
	float top,height,left,width;
	
	float xScrollPos = XPixelsToTwips(m_pAddFlow->GetScrollPos(afHorizontal));
	float yScrollPos = YPixelsToTwips(m_pAddFlow->GetScrollPos(afBoth));
	// Add Scroll Pos to xPos & yPos
	yPos += xScrollPos;
	xPos += yScrollPos;

	if (fMove == TRUE) 
	{
		VariantInit(&vaIdx);
		vaIdx.vt = VT_I4;
		nodes = m_pAddFlow->GetNodes(); 
	
		for (i = 1; i <= nodes.GetCount(); i++)
		{				
			vaIdx.lVal = i;
			node = nodes.Item(vaIdx);				
			top = node.GetTop(); 
			height = node.GetHeight();
			left = node.GetLeft(); 
			width = node.GetWidth();

			if ((top < yPos) && ((top+height) > yPos) && 
				(left<xPos) && ((left+width) > xPos))
			{
				((CMyAddFlowSheet*) m_pParent)->fDropping = TRUE;		//NEW !!!!!
				m_pAddFlow->SetSelectedNode(node);
				m_pAddFlow->SetCanMoveNode(TRUE);		/* enable can move */
				break;
			}	
		}
	}
	else
	{
		m_pAddFlow->SetCanMoveNode(FALSE);
		node = m_pAddFlow->GetSelectedNode();
		if (node != NULL) node.SetSelected(FALSE);
		// SetPosition Of Node ...
		pControls->SetPosition(node);
		((CMyAddFlowSheet*) m_pParent)->fDropping = FALSE;		//NEW !!!!!

	}
}

void CBindingGuiComponent::MoveSelectedNode(float xPos,float yPos)
{
	CNode1 SelectedNode = m_pAddFlow->GetSelectedNode();
	
	float xScrollPos = XPixelsToTwips(m_pAddFlow->GetScrollPos(afHorizontal));
	float yScrollPos = YPixelsToTwips(m_pAddFlow->GetScrollPos(afBoth));
	// Add Scroll Pos to xPos & yPos
	yPos += xScrollPos;
	xPos += yScrollPos;

	if (SelectedNode != NULL) 
	{
		if ((yPos - SelectedNode.GetHeight()/2) > 0)
			SelectedNode.SetTop( yPos - (SelectedNode.GetHeight()/2));
			
		if ((xPos - SelectedNode.GetWidth()/2) > 0)
			SelectedNode.SetLeft( xPos - (SelectedNode.GetWidth()/2));
	}

}

void CBindingGuiComponent::MoveNode(CNode1 node,float xPos,float yPos)
{
	if (node != NULL) 
	{
		node.SetTop(yPos);
		node.SetLeft(xPos);
	}
}
/////////////////////////////////////////////////////////////////
void CBindingGuiComponent::ShowPropDlg(int xpos,int ypos)
{	
	if (AfxGetMainWnd() != NULL)
	{
		CNode1 node = m_pAddFlow->GetSelectedNode();
		inouttype type;
	
		if (node != NULL)
		{	
			if (GetControls()->GetControlType(node) == PM_UNIT) 
			{
				CBindingUnit* const pUnit = GetControls()->GetUnit(node);
				if (pUnit != NULL) 
				{
					// Changes Versie 0x0c01:
					// Type PM_INOUTTYPE_UNDEFINED bijgevoegd 
					// Indien niet zeker zijn dan wordt de input/output property van de 
					// binding unit niet aangepast.

					type = GetControls()->CheckinoutType(node);
					if (PM_INOUTTYPE_UNDEFINED != type)
					{
						pUnit->SetInOutType(type);
					}
					
					const unsigned long ulAddress = (pUnit->GetID() << 16) | 
													(pUnit->GetNodeAddress() << 8) |
													 pUnit->GetUnitAddress();
							
					const unsigned long ulPos = ((xpos & 0x7FFF) << 16) | 
												  ypos & 0x7FFF;				
				

					// Refreshen van de tooltip text: Bijhouden van het id van de binding unit.					
					this->m_CurrentBindingUnitID = pUnit->GetID();

					AfxGetMainWnd()->SendMessage(WM_SHOW_DIALOG_BINDINGUNIT,
											     (WPARAM) ulAddress,
									             (LPARAM) ulPos);
				}
			}
			else 
			{
				CBindingOperator* const pOperator = GetControls()->GetOperator(node);
		
				if (pOperator != NULL) 
				{
					if (pOperator->GetType() == PM_OPERATOR_TIMER) 
					{
						const unsigned long ulAddress = pOperator->GetID();
						const unsigned long ulPos = ((xpos & 0x7FFF) << 16) | 
													  ypos & 0x7FFF;
						
						AfxGetMainWnd()->SendMessage(WM_SHOW_DIALOG_BINDINGTIMER,
													(WPARAM) ulAddress,
													(LPARAM) ulPos);
					}
					else 
					{
						DestroyPropDlg();				
					}
				}
				this->m_CurrentBindingUnitID = -1;
			}
		}
		else 
		{
			DestroyPropDlg();	
		}
	}
}

void CBindingGuiComponent::DestroyPropDlg(void) {
	if (AfxGetMainWnd() != NULL) {		
		AfxGetMainWnd()->SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);
	}
	if (this->m_CurrentBindingUnitID >= 0) {
		this->refreshControlToolTipText(this->m_CurrentBindingUnitID);
		this->m_CurrentBindingUnitID = -1;
	}
}

BOOL CBindingGuiComponent::refreshControlToolTipText(int bindingUnitIdentifier) {
	CBindingEntry* const pBindingEntry = this->GetCurrBindingEntry();
	if (NULL != pBindingEntry) {
		CBindingUnits::Iterator it(pBindingEntry->GetColUnit()->CreateIterator());
		for (it.Begin(); !it.IsDone(); it.Next()) {
			CBindingUnit* const pUnit = it.CurrentItem();
			if ((NULL != pUnit) && (bindingUnitIdentifier == pUnit->GetID())) {
				CNode1 node = this->GetControls()->GetControl(pUnit->GetID(), PM_UNIT);
				if (node) {					
					pControls->RefreshTooltipText(&node, pUnit);
				}
			}
		}
	}
	return TRUE;
}

BOOL CBindingGuiComponent::refreshControlsToolTipText() {
	if (pCurrBindingEntry == NULL) {
		return TRUE;
	}
	CNodes1 nodes = m_pAddFlow->GetNodes();
	if (nodes) {
		VARIANT vaIdx;
		VariantInit(&vaIdx);
		vaIdx.vt = VT_I4;
		const int count = nodes.GetCount();
		for (int i = 1; i <= count; i++) {
			vaIdx.lVal = i;
			CNode1 node = nodes.Item(vaIdx);
			if ((node) && (pControls->GetControlType(node) == PM_UNIT)) {
				CBindingUnit* const pBindingUnit = pControls->GetUnit(node);
				if (NULL != pBindingUnit) {
					pControls->RefreshTooltipText(&node, pBindingUnit);
				}
			}
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////
