// Binding2Controls.h: interface for the CBinding2Controls class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDING2CONTROLS_H__C9953FA3_765C_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDING2CONTROLS_H__C9953FA3_765C_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingConstants.h"

struct STRUCT_UNIT;

class CNode1;
class CAddFlow1;
class CLink1;

class CBindingGuiComponent;
class CBindingOperator;
class CBindingUnit;

//////////////////////////////////////////////////////////////////////

class CBinding2Controls  
{
	CBindingGuiComponent *m_pParent;
	CAddFlow1 *m_pAddFlow;	

public:
	static ControlType Convert(const char cControl);

public:
	CBinding2Controls(CBindingGuiComponent *pGui);
	virtual ~CBinding2Controls();

	CNode1 Add(OperatorType Type);
	CNode1 Add(STRUCT_UNIT *pUnit);

	unsigned short Add(float x,float y,OperatorType Type);
	unsigned short Add(float x,float y,STRUCT_UNIT *pUnit);

	BOOL AddNode(CBindingOperator *pOperator);
	BOOL AddNode(CBindingUnit *pUnit);

	void AddIcon(CNode1 *afNode, CBindingUnit *pBindingUnit);
	void RefreshTooltipText(CNode1 *afNode, CBindingUnit *pBindingUnit);

	BYTE Remove(CNode1 node);

	// GETPROPERTIES
	BYTE GetType(CNode1 node);
	ControlType GetControlType(CNode1 node);
	
	inouttype CheckinoutType(CNode1 node);

	// GET - RETURNS CBindingUnit, CbindingOperator : TODO !!
	CBindingUnit* GetUnit(CNode1 node);
	CBindingOperator* GetOperator(CNode1 node);

	CNode1 GetControl(unsigned short nID,ControlType Type);
	BOOL SetPosition(CNode1 node);
	BOOL IsAlreadyPresent(STRUCT_UNIT *pUnit);

protected:	
	void SetNodeUnitStyle(CNode1 node);
	void SetNodeOperatorStyle(CNode1 node);
	void SetNodeText(CNode1 node,CString szText);

	// enum OperatorType GetOperatorType(CNode1 node);

private:
	void AddIcon(CNode1 *afNode, OperatorType Type);	
	void AddIcon(CNode1 *afNode, STRUCT_UNIT *pUnit);
	void SetToolTipText(CNode1 *afNode, OperatorType Type);
	void SetToolTipText(CNode1 *afNode, CBindingUnit *pBindingUnit);
};

#endif // !defined(AFX_BINDING2CONTROLS_H__C9953FA3_765C_11D8_B865_0050BAC412B1__INCLUDED_)
