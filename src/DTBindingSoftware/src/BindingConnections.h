// BindingConnections.h: interface for the CBindingConnections class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONNECTIONS_H__C9953FA7_765C_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGCONNECTIONS_H__C9953FA7_765C_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
#include "BindingConstants.h"

class CNode1;
class CAddFlow1;
class CLink1;

class CBindingEntry;
class CBindingGuiComponent;
class CBindingLink;

/////////////////////////////////////////////////////////////////////

class CBindingConnections  
{
	CBindingGuiComponent *m_pParent;
	CAddFlow1 *m_pAddFlow;

public:
	//CBindingConnections();
	CBindingConnections(CBindingGuiComponent *);
	virtual ~CBindingConnections();

private:
	BYTE CheckLinkinoutType(CNode1 afSrcNode,CNode1 afDestNode,CBindingLink *pNewLink);
	BYTE CheckLinkNLinks(CNode1 afSrcNode,CNode1 afDestNode,CBindingLink *pNewLink);	
	BYTE CheckLinkControlsType(CBindingLink *pNewLink);
	BYTE CheckLink(CNode1 afSrcNode,CNode1 afDestNode,CBindingLink *pNewLink);
	BYTE GetTypeProperties(unsigned short nID,ControlType Type);

	// Sedert versie 0x0C01: Controle of er geen input/output combinatie is van dezelfde unit.
	BYTE CheckLinkInOutUnits(CNode1 afSrcNode,CNode1 afDestNode,CBindingLink *pNewLink);

public:
	BYTE Remove(CLink1 lnk);
	BYTE Remove(CNode1 node,LinkType Type);
	BYTE Set(CLink1 lnk,BYTE bInvert);	
	BYTE Add(CLink1 lnk);
	unsigned short Add(unsigned short inID,ControlType InType,unsigned short OutID,ControlType OutType,BYTE bInvers);
	BOOL Addlink(CBindingLink *pLink);
};

#endif // !defined(AFX_BINDINGCONNECTIONS_H__C9953FA7_765C_11D8_B865_0050BAC412B1__INCLUDED_)
