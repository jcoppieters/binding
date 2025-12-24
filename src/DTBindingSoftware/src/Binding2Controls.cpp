// Binding2Controls.cpp: implementation of the CBinding2Controls class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "Binding2Controls.h"

#include "BindingLinks.h"
#include "BindingEntry.h"
#include "BindingGuiComponent.h"

#include "CanNode.h"

#include "addflow\Link1.h"
#include "addflow\Links1.h"
#include "addflow\Node1.h"
#include "addflow\nodes1.h"
#include "addflow\AddFlow1.h"
#include "addflow\AddflowConst.h"

#include "BindingUnitFactory.h"
#include "BindingOperatorFactory.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
// ------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define AF_SIZE_UNIT			 1000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBinding2Controls::CBinding2Controls(CBindingGuiComponent *pGui)
{
	ASSERT(pGui);
	m_pParent = pGui;
	m_pAddFlow = pGui->GetAddFlowComponent();
}
CBinding2Controls::~CBinding2Controls()
{
}
//////////////////////////////////////////////////////////////////////
ControlType CBinding2Controls::Convert(const char cControl)
{
	switch(cControl) 
	{
		case 'U': 
		{
			return (PM_UNIT); 

		} break;

		case 'O': 
		case 'X': 
		case 'Y': 
		{
			return (PM_OPERATOR); 

		} break;
	}

	return UNDEFINED;
}



//////////////////////////////////////////////////////////////////////

void CBinding2Controls::AddIcon(CNode1 *afNode, OperatorType Type) {	
	ASSERT (afNode);
	switch(Type) {
		case PM_OPERATOR_AND:		afNode->SetPictureIndex(1);		break;
		case PM_OPERATOR_OR:		afNode->SetPictureIndex(2);		break;
		case PM_OPERATOR_XOR:		afNode->SetPictureIndex(3);		break;
		case PM_OPERATOR_TIMER:		afNode->SetPictureIndex(4);		break;
		default:					ASSERT(0);						break;
	}
}

void CBinding2Controls::AddIcon(CNode1 *afNode, STRUCT_UNIT *pDocUnit) {	
	if (NULL == afNode || NULL == pDocUnit) {
		REQUIRE(afNode);
		REQUIRE(pDocUnit);
		return;
	}
	CApplicationGuiUnit::AppUiIconsExtraInfo_t appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_NONE;
	if ((NULL != pDocUnit->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pDocUnit->m_pAppUnitData->getUnitDataType())) {				
		appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_DALI;
	}
	UnitIconType iImage;
	TMGetGuiUnit()->GetIcon(pDocUnit->bNodeLogicalAddress, pDocUnit->bUnitAddress, appUiExtraInfo, &iImage);
	afNode->SetPictureIndex((BYTE) iImage + 5);	
}

void CBinding2Controls::AddIcon(CNode1 *afNode, CBindingUnit *pBindingUnit) {
	if (NULL == afNode || NULL == pBindingUnit) {
		REQUIRE(afNode);
		REQUIRE(pBindingUnit);
		return;
	}
	CApplicationGuiUnit::AppUiIconsExtraInfo_t appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_NONE;
	if (pBindingUnit->GetApplicationFlags() & CBindingUnit::APPFLAG_DALI_BINDING_UNIT) {
		appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_DALI;
	}
	UnitIconType iImage;
	TMGetGuiUnit()->GetIcon(pBindingUnit->GetNodeAddress(), pBindingUnit->GetUnitAddress(), appUiExtraInfo, &iImage);
	afNode->SetPictureIndex((BYTE) iImage + 5);	
}

void CBinding2Controls::RefreshTooltipText(CNode1 *afNode, CBindingUnit *pBindingUnit) {
	if (NULL == afNode || NULL == pBindingUnit) {
		REQUIRE(afNode);
		REQUIRE(pBindingUnit);
		return;
	}
	this->SetToolTipText(afNode, pBindingUnit);
}

void CBinding2Controls::SetToolTipText(CNode1 *afNode, OperatorType Type) {
	if (NULL == afNode) {
		REQUIRE(afNode);
		return;
	}
	CString strText;
	switch(Type) {
		case PM_OPERATOR_AND: strText = "And"; break;
		case PM_OPERATOR_OR: strText = "Or"; break;
		case PM_OPERATOR_XOR: strText = "Xor"; break;
		case PM_OPERATOR_TIMER: strText = "Timer"; break;
	}
	if (!strText.IsEmpty()) {
		afNode->SetToolTip(strText);
	}
}

void CBinding2Controls::SetToolTipText(CNode1 *afNode, CBindingUnit *pBindingUnit) {
	if (NULL == afNode || NULL == pBindingUnit) {
		REQUIRE(afNode);
		REQUIRE(pBindingUnit);
		return;
	}
	afNode->SetToolTip(pBindingUnit->getTextToolTip());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBinding2Controls::SetNodeUnitStyle(CNode1 node)
{
	node.SetSizeable(FALSE);
	node.SetAlignment(afCenterBOTTOM);
	node.SetDrawStyle(afTransparent);
	node.SetTransparent(TRUE);
	node.SetAutoSize(afNoAutoSize);
	node.SetHeight(1000);
	node.SetWidth(1000);

}

void CBinding2Controls::SetNodeOperatorStyle(CNode1 node)
{
	node.SetAutoSize(afNoAutoSize);
	node.SetHeight(500);
	node.SetWidth(500);
	node.SetSizeable(FALSE);
	node.SetDrawStyle(afTransparent);
	node.SetTransparent(TRUE);
}

void CBinding2Controls::SetNodeText(CNode1 node,CString szText)
{
	CString szFormatedText;

	int nIndex = 0;
	const int nLength = szText.GetLength();

	if (nLength <= 16)
	{
		nIndex = szText.Find('|');

		if ((nIndex != -1) && (nIndex <= 16))
		{			
			szFormatedText = szText;
			szFormatedText.Replace('|','\n');
		}
		else
		{
			if (nLength >= 10) nIndex = 8;				

			szFormatedText = szText;
			szFormatedText.Insert(nIndex,'\n');						
		}
	}
	else
	{
		if (nLength >= 10) nIndex = 8;				

		szFormatedText = szText;
		szFormatedText.Insert(nIndex,'\n');
	}

	node.SetText(szFormatedText);
}


// Sedert versie 0x0B01: 'STRUCT_UNIT' meegeven aan CBindingUnitFactory::Create
CNode1 CBinding2Controls::Add(STRUCT_UNIT *pDocUnit)
{
	CBindingUnit* const pBindingUnit = CBindingUnitFactory::Create(pDocUnit);
	if (pBindingUnit == NULL) {
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_ADDUNIT);
		AfxMessageBox(szError);
		return 0;
	}

	pBindingUnit->SetID(0);	// Get New ID from collection !!!
	pBindingUnit->SetName(pDocUnit->szUnitName);
	pBindingUnit->SetNodeAddress(pDocUnit->bNodeLogicalAddress);
	pBindingUnit->SetUnitAddress(pDocUnit->bUnitAddress);

	CBindingUnits *const pBindingUnits = (m_pParent->GetCurrBindingEntry())->GetColUnit();
	ASSERT(pBindingUnits);
	const unsigned short nID = pBindingUnits->Add(pBindingUnit);

	ASSERT(nID);
	ASSERT(pBindingUnit->GetID() != 0);
	
	if (nID == NULL)  {
		ASSERT(nID);
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_ADDUNIT);
		AfxMessageBox(szError);
		return 0;	
	}


	static float x=1000,y=4000;

	x += 150;		/* temporaray */
	y += 150;

	CNode1 AfNode = m_pAddFlow->GetNodes().Add(x,y,AF_SIZE_UNIT,AF_SIZE_UNIT);	
	pBindingUnit->SetPosition(x,y);


	SetNodeText(AfNode,pBindingUnit->GetName());
	SetNodeUnitStyle(AfNode);
	this->AddIcon(&AfNode,pDocUnit);	

	CString szTag;
	szTag.Format("U,%d",nID);
	AfNode.SetTag(szTag);	

	SetToolTipText(&AfNode,pBindingUnit);
	return  AfNode;
}

// Sedert versie 0x0B01: 'STRUCT_UNIT' meegeven aan CBindingUnitFactory::Create
unsigned short CBinding2Controls::Add(float x,float y,STRUCT_UNIT *pUnit)
{	
	CBindingUnit* const pBindingUnit =CBindingUnitFactory::Create(pUnit);

	if (pBindingUnit == 0)
	{
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_ADDUNIT);
		AfxMessageBox(szError);
		return 0;
	}

	pBindingUnit->SetID(0);	// !!!!!

	pBindingUnit->SetName(pUnit->szUnitName);
	pBindingUnit->SetNodeAddress(pUnit->bNodeLogicalAddress);
	pBindingUnit->SetUnitAddress(pUnit->bUnitAddress);

	CBindingUnits *const pBindingUnits = m_pParent->GetCurrBindingEntry()->GetColUnit();
	ASSERT(pBindingUnits);
	const unsigned short nID = pBindingUnits->Add(pBindingUnit);
	
	ASSERT(nID);
	ASSERT(pBindingUnit->GetID() != 0);

	if (nID == 0) 
	{
		ASSERT(nID);
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_ADDUNIT);
		AfxMessageBox(szError);
		return 0;
	}

	CNode1 AfNode = m_pAddFlow->GetNodes().Add(x,y,AF_SIZE_UNIT,AF_SIZE_UNIT);

	pBindingUnit->SetPosition(x,y);
	SetNodeText(AfNode,pBindingUnit->GetName());	
	
	CString szTag;
	szTag.Format("U,%d",nID);
	AfNode.SetTag(szTag);	

	SetToolTipText(&AfNode,pBindingUnit);

	AddIcon(&AfNode,pUnit);	
	SetNodeUnitStyle(AfNode);

	return (nID);
}

// -------------------------
unsigned short CBinding2Controls::Add(float x, float y, OperatorType Type)
{	
	CBindingOperators *pBindingOperators = m_pParent->GetCurrBindingEntry()->GetColOperator();
	ASSERT(pBindingOperators);

	CBindingOperator* const pUnit = CBindingOperatorFactory::Create(Type);

	if (pUnit == NULL)
	{
		ASSERT(0);

		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_ADDOPERATOR);
		AfxMessageBox(szError);
		return 0;
	}

	pUnit->SetID(0);	// !!!
	const unsigned short nID = pBindingOperators->Add(pUnit);
	
	ASSERT(nID != 0);
	ASSERT(pUnit->GetID() == nID);

	if (nID == 0) 
	{
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_ADDOPERATOR);
		AfxMessageBox(szError);
		return 0;
	}
	
	CNode1 AfNode = m_pAddFlow->GetNodes().Add(x,y,AF_SIZE_UNIT,AF_SIZE_UNIT);

	pUnit->SetPosition(x,y);
	AddIcon(&AfNode,Type);
	SetNodeOperatorStyle(AfNode);
	
	if (1) {
		CString szTag;
		if (Type == PM_OPERATOR_TIMER) {
			szTag.Format("T,%d",nID);
		}
		else {
			szTag.Format("O,%d",nID);
		}	
		AfNode.SetTag(szTag);
	}

	SetToolTipText(&AfNode,Type);

	return (nID);
}

CNode1 CBinding2Controls::Add(OperatorType Type)
{
	CBindingOperators* const pBindingOperators = m_pParent->GetCurrBindingEntry()->GetColOperator();
	ASSERT(pBindingOperators);

	CBindingOperator* const pUnit = CBindingOperatorFactory::Create(Type);

	if (pUnit == NULL)
	{
		ASSERT(0);
		
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_ADDOPERATOR);
		AfxMessageBox(szError);
		return 0;
	}

	static float x=1000,y=4000;
	x += 150;		/* temporaray */
	y += 150;

	pUnit->SetID(0);	// !!!
	const unsigned short nID = pBindingOperators->Add(pUnit);

	ASSERT(nID != 0);
	ASSERT(nID == pUnit->GetID());

	if (nID == 0) 
	{
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_ADDOPERATOR);
		AfxMessageBox(szError);
		return NULL;
	}
	
	/* TODO :
	   Calculate x / y Position 
	   1st : if unit == Unit : Input : add Vertical 
							   Output : Add Horizontal
	   2nd : if operator : add Horizontal 
	*/

	CNode1 AfNode = m_pAddFlow->GetNodes().Add(x,y,AF_SIZE_UNIT,AF_SIZE_UNIT);

	pUnit->SetPosition(x,y);
	SetNodeOperatorStyle(AfNode);

	AddIcon(&AfNode,Type);

	if (1) {
		CString szTag;	
		if (Type == PM_OPERATOR_TIMER) {
			szTag.Format("T,%d",nID);
		}
		else  {
			szTag.Format("O,%d",nID);
		}
		AfNode.SetTag(szTag);
	}

	SetToolTipText(&AfNode,Type);

	return AfNode;
}


////////////////////////////////////////////////////////////////////////
BYTE CBinding2Controls::Remove(CNode1 node)
{
	int id = -1;			// CHANGES_MVS_2008_SSCANF
	char chType = '\0';		

	CBindingOperators* const pBindingOperators = m_pParent->GetCurrBindingEntry()->GetColOperator();
	CBindingUnits* const pBindingUnits = m_pParent->GetCurrBindingEntry()->GetColUnit();
	CBindingConnections* const pLinkConnections = m_pParent->GetConnections();
	
	ASSERT(pBindingUnits);
	ASSERT(pLinkConnections);
	ASSERT(pBindingOperators);

	CString szTag = node.GetTag();

	if(sscanf((LPCTSTR) szTag,"%c,%d",&chType,&id) == FALSE)		
	{
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_TAG);
		AfxMessageBox(szError);
		return FALSE;
	}
	
	switch(chType)
	{
		case 'O':
		case 'T':			
		{			
			if (pBindingOperators->Remove(id) == FALSE) 
			{
				ASSERT(0);
				CString szError;
				szError.LoadString(IDS_ERROR_CONTROLS_REMOVEOPERATOR);
				AfxMessageBox(szError);			
				return FALSE;
			}				
		} break;

		case 'U':
		{	
			if (pBindingUnits->Remove(id) == FALSE)	
			{
				ASSERT(0);
				CString szError;
				szError.LoadString(IDS_ERROR_CONTROLS_REMOVEUNIT);
				AfxMessageBox(szError);			
				return FALSE;
			}	

		} break;
	}
	
	/* Remove all Links */	
	if (pLinkConnections->Remove(node, PM_ALLLINKS) == FALSE)  
	{
		ASSERT(0);

		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_ALLLINKS);
		AfxMessageBox(szError);
	}		
	
	node.SetMarked(TRUE);
	node.SetTag("\0");			
	m_pAddFlow->DeleteMarked();	

	return TRUE;
}

////////////////////////////////////////////////////////////////////////

// Aanpassingen versie 0x0C01

inouttype CBinding2Controls::CheckinoutType(CNode1 node)
{
	inouttype retVal = PM_INOUTTYPE_UNDEFINED;

	CLinks1 linksIn = node.GetInLinks();
	CLinks1 linksOut = node.GetOutLinks();
	
	if((linksOut.GetCount()!=0)&&(linksIn.GetCount()==0))
	{
		// Er zijn output links maar geen input links.
		retVal=PM_INOUTTYPE_INPUT;
	}
	else if (linksIn.GetCount()!=0)
	{
		// Er is minimum 1 input link.
		retVal=PM_INOUTTYPE_OUTPUT;
	}	
	else if(linksIn.GetCount()==0 && linksOut.GetCount()==0)
	{
		// Er zijn nog geen links aanwezig
		retVal=PM_INOUTTYPE_UNDEFINED;
	}
	return retVal;
}


/*
inouttype CBinding2Controls::CheckinoutType(CNode1 node)
{
  CLinks1 links = node.GetInLinks();

  if (links.GetCount() != 0) return (PM_INOUTTYPE_OUTPUT);
  else return (PM_INOUTTYPE_INPUT);
}
*/



ControlType CBinding2Controls::GetControlType(CNode1 node)
{
	char chType = '\0';	
	int id = -1;		// CHANGES_MVS_2008_SSCANF

	ASSERT(node);
	
	CString szTag = node.GetTag();	
	ASSERT(szTag.IsEmpty() == FALSE);

	if(sscanf((LPCTSTR) szTag,"%c,%d",&chType,&id) == FALSE)		
	{
		ASSERT(0);
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_TAG);
		AfxMessageBox(szError);
		return UNDEFINED;
	}

	switch(chType)
	{
		case 'U':	
			return PM_UNIT;							
		break;
		case 'O':	
			return PM_OPERATOR;			
		break;
		case 'T':	
			return PM_TIMEROPERATOR;	
		break;
	
		default:
		{
			ASSERT(0);
			CString szError;
			szError.LoadString(IDS_ERROR_CONTROLS_GETTYPE);
			AfxMessageBox(szError);
		} break;
	}
	
	return UNDEFINED;
}


BYTE CBinding2Controls::GetType(CNode1 node)
{
	CBindingUnit *pUnit = NULL;
	pUnit = GetUnit(node);
	
	ASSERT(pUnit);
	return(pUnit->GetBindingUnitType());
}

///////////////////////////////////////////////////////////////
CBindingOperator* CBinding2Controls::GetOperator(CNode1 node)
{
	char chType = '\0';	
	int id = -1;		// CHANGES_MVS_2008_SSCANF

	CBindingOperators* const pBindingOperators = m_pParent->GetCurrBindingEntry()->GetColOperator();
	ASSERT(pBindingOperators);
	ASSERT(node);

	CString szTag;
	szTag = node.GetTag();

	ASSERT(szTag.IsEmpty() == FALSE);

	if(sscanf((LPCTSTR) szTag,"%c,%d",&chType,&id) == 0)	
	{
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_TAG);
		AfxMessageBox(szError);
		return 0;
	}

	ASSERT(id);

	CBindingOperator* const pOperator = pBindingOperators->Find(id);
	ASSERT(pOperator);

	return pOperator;
}

CBindingUnit* CBinding2Controls::GetUnit(CNode1 node)
{
	char chType = '\0';	
	int id = -1;		// CHANGES_MVS_2008_SSCANF

	CBindingUnits* const pBindingUnits = m_pParent->GetCurrBindingEntry()->GetColUnit();
	ASSERT(pBindingUnits);
	
	ASSERT(node);
	CString szTag = node.GetTag();

	ASSERT(szTag.IsEmpty() == FALSE);

	if (sscanf((LPCTSTR) szTag,"%c,%d",&chType,&id) == 0)
	{
		CString szError;
		szError.LoadString(IDS_ERROR_CONTROLS_TAG);
		AfxMessageBox(szError);
		return 0;
	}

	ASSERT(id);
	
	CBindingUnit *pUnit = pBindingUnits->Find(id);
	ASSERT(pUnit);

	return pUnit;
}


CNode1 CBinding2Controls::GetControl(unsigned short nControlID,ControlType Type)
{
	CNode1 node;	
	VARIANT vaIdx;
	CString szTag;
	char chType = '\0';	
	int id = -1;		// CHANGES_MVS_2008_SSCANF

	VariantInit(&vaIdx);
	vaIdx.vt = VT_I4;
	for (int i = 1; i <= m_pAddFlow->GetNodes().GetCount(); i++)
	{
		vaIdx.lVal = i;
		node = m_pAddFlow->GetNodes().Item(vaIdx);
		szTag = node.GetTag();

		if (!(sscanf((LPCTSTR) szTag,"%c,%d",&chType,&id))) return 0;		

		if ((GetControlType(node) == Type) && (nControlID == id))
			return node;
	}

	return 0;
}

///////
BOOL CBinding2Controls::AddNode(CBindingOperator *pOperator)
{
	CNode1 AfNode;
	CString szTag,szError;
	OperatorType Type;
	float x,y;

	pOperator->GetPosition(&x,&y);
	AfNode = m_pAddFlow->GetNodes().Add(x,y,AF_SIZE_UNIT,AF_SIZE_UNIT);
	SetNodeOperatorStyle(AfNode);
	Type = pOperator->GetType();
	AddIcon(&AfNode,Type);
	if (Type == PM_OPERATOR_TIMER) szTag.Format("T,%d",pOperator->GetID());
	else szTag.Format("O,%d",pOperator->GetID());	
	AfNode.SetTag(szTag);
	SetToolTipText(&AfNode,Type);
	return TRUE;
}

BOOL CBinding2Controls::AddNode(CBindingUnit *pBindingUnit)
{
	CNode1 AfNode;
	float x,y;
	CString szTag;

	pBindingUnit->GetPosition(&x,&y);
	AfNode = m_pAddFlow->GetNodes().Add(x,y,AF_SIZE_UNIT,AF_SIZE_UNIT);
	SetNodeText(AfNode,pBindingUnit->GetName());
	szTag.Format("U,%d",pBindingUnit->GetID());
	AfNode.SetTag(szTag);	
	SetToolTipText(&AfNode,pBindingUnit);
	AddIcon(&AfNode,pBindingUnit);//TM NEW 13/5/2004
	SetNodeUnitStyle(AfNode);
	return TRUE;
}


BOOL CBinding2Controls::SetPosition(CNode1 node)
{
	if (node) 
	{
		if (GetControlType(node) == PM_UNIT) 
		{
			CBindingUnit *const pUnit = GetUnit(node);	
			if (pUnit != NULL)
			{
				pUnit->SetPosition(node.GetLeft(),node.GetTop());
			}
		}
		else 
		{
			CBindingOperator *const pOperator = GetOperator(node);
			if (pOperator != NULL)
			{
				pOperator->SetPosition(node.GetLeft(),node.GetTop());
			}
		}
	}
	return TRUE;
}

// TM 20081223
BOOL CBinding2Controls::IsAlreadyPresent(STRUCT_UNIT *pUnit)
{	
	ASSERT( pUnit );

	CBindingUnits *const pBindingUnits = m_pParent->GetCurrBindingEntry()->GetColUnit();
	if (pBindingUnits != NULL)
	{
		CBindingUnits::Iterator it(pBindingUnits->CreateIterator());
		for (it.Begin();!it.IsDone();it.Next())
		{
			CBindingUnit* const pBindingUnit = it.CurrentItem();
			ASSERT( pBindingUnit );

			if ((pBindingUnit->GetNodeAddress() == pUnit->bNodeLogicalAddress) && 
				(pBindingUnit->GetUnitAddress() == pUnit->bUnitAddress)) 
			{
				return TRUE;	
			}
		}	
	}
	return FALSE;
}
