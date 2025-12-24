// BrowseForNodesObject.h: interface for the CBrowseForNodesObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BROWSEFORNODESOBJECT_H__B0E4EC21_3A35_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BROWSEFORNODESOBJECT_H__B0E4EC21_3A35_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////
#include "Nodemess.h"
#include "AppBrowsePnp_common.h"
//////////////////////////////////////////////////////////////
class CCanNode;
struct STRUCT_UNIT;
class IBrowsePnPEngine;
class CTXBrowsePnpMessages;
class CTXUnitMessage;
class CTXNodeBindingFileMessage;
class CNodeDatabase;

//////////////////////////////////////////////////////////////

// TODO:
// Code opkuisen van de fsm.
// - Veel van de toestanden/events van de fsm zijn gemeenschappelijk 
//   voor de broadcast/single node...
//
// principe:
//		BOOL BrowseAllNodes(iEvent) {
//			...
//			if event handled: 
//				return true.
//			
//			// gemeenschappelijke code in de basis fsm en daar verwerken.
//			return Browse_base_fasm(iEvent);
//		}
//

////////////////////////////////////////////////////////////////
class CBrowseForNodesObject  
{
private:
	typedef void (CBrowseForNodesObject::*FSM_MODE)(int iEvent,int wParam,int lParam);
	FSM_MODE pf_CurrentMode;
	
	// BROWSE STATES
	enum BrowseStates {
		STATE_BROWSE_IDLE=0, 
		STATE_BROWSE_NODES, 
		STATE_GET_NODE_CONFIG,	// Sedert V16.45
		STATE_GET_NODE_NAME,
		STATE_GET_UNIT_NAME, 
		STATE_GET_UNIT_INFO,
		STATE_GET_NODE_RESOURCES,
		STATE_GET_NODE_BINDINGFILEINFO		// Sedert 16.32: Extra toestand.
	} m_CurrentState;
	
	CNodeDatabase* const m_pDatabase;
	IBrowsePnPEngine* const pBrowsePnpEngine;	
	CCanNode* m_pNode;
	STRUCT_UNIT* m_pUnit;
	int iNodeIndex,iUnitIndex;
	BYTE bNodeType,bNodeAddress;
	int m_iCurrentNROfNodes;
	int m_iPreviousNROfNodes;
	CTXBrowsePnpMessages* pTxBrowseMessages;
	CTXNodeBindingFileMessage* pTxBindingFileMessage;
	CTXUnitMessage* pTxUnitMessages;

public:	
	CBrowseForNodesObject(BYTE bAddress,IBrowsePnPEngine *pEngine,CNodeDatabase *pNodes);
	CBrowseForNodesObject(IBrowsePnPEngine *pEngine,CNodeDatabase *pNodes);
	virtual ~CBrowseForNodesObject();
	void Start();
	void Stop();	
	void BrowseForNodes(int iEvent,int wParam,int lParam);
	BOOL getBusyReadUnitName(BYTE bNodeAddress,BYTE bUnitAddress);	// Sedert V16.45
    BOOL getBusyReadNodeName(BYTE bNodeAddress);					// Sedert V16.45


protected:
	void BrowseAllNodes(int iEvent,int wParam,int lParam);
	void BrowseSingleNode(int iEvent,int wParam,int lParam);	

	// Sedert V16.30: Berekenen van de timer reload waarde die nodig is om de nodes te vinden.
	// Deze wordt telkens gevonden ifv het aantal gevonden nodes.
	// Sedert V16.32: Timers iets verhoogd.
	int getTimerBroadcastReloadValue( );

	// Sedert V16.30: Controle doen of alle nodes correct in de database aanwezig zijn.
	bool checkNodeDatabaseValid( );

private:
	static const CString getStrState(BrowseStates state);
	void helper_handleUserCancel();
	void helper_handleNoResponse();
	void helper_handleResponseCloseFile();
	void helper_handleWrongResponse();
	void helper_displayWrongEventReceived();
	void setCurrentState(BrowseStates state);
};

#endif // !defined(AFX_BROWSEFORNODESOBJECT_H__B0E4EC21_3A35_11D8_B865_0050BAC412B1__INCLUDED_)
