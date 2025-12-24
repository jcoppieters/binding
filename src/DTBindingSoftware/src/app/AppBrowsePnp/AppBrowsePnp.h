// BrowsePnPEngine.h: interface for the CBrowsePnPEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BROWSEPNPENGINE_H__3021B501_45A7_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BROWSEPNPENGINE_H__3021B501_45A7_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
////////////////////////////////////////////////////////////////////
#include "AppBrowsePnp_common.h"
////////////////////////////////////////////////////////////////////
class CBrowseForNodesObject;
class CPnpEngine;
class IComApplication;
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
enum BrowsePnPMode
{
	BROWSE_FOR_NODES = 0,
	BROWSE_FOR_PH_NODES,
	BROWSE_SINGLE_NODE,
	AUTO_PNP,
	IDLE_MODE = 0xFF
};

////////////////////////////////////////////////////////////////////
class CBrowsePnPEngine : public IBrowsePnPEngine
{
public:
	CBrowsePnPEngine(IComApplication* pCom);
	virtual ~CBrowsePnPEngine();

private:
	CPnpEngine *m_pPnpObject;
	CBrowseForNodesObject *m_pBrowseObject;
	BrowsePnPMode m_currentBrowsePnpMode;
	IComApplication *const pCommApplication;
	void BrowseEnded(int nResult);
	void PnPEnded(int nResult);

public:
	void BrowseForNodes(void);
	void BrowseForPhysicalNodes(void);
	void BrowseSingleNode(BYTE bNodeAddress);
	void AutoPnp(void);
	void UserCancel(void);
	void FSMSendEvent(int iEvent,int wParam=0,int lParam=0);

	BOOL IsBusyPnp(void)  {
		if (m_pPnpObject == NULL) return FALSE;
		else return TRUE;
	}

	BOOL getBusyReadUnitName(BYTE bNodeAddress,BYTE bUnitAddress);	// Sedert V16.45
    BOOL getBusyReadNodeName(BYTE bNodeAddress);					// Sedert V16.45

public:
	// Browse FSM ...
	void Fire_OnBrowseDone(void)			{ BrowseEnded(BROWSE_OK);			}
	void Fire_OnBrowseCancel(void)			{ BrowseEnded(BROWSE_USER_CANCEL);	}
	void Fire_OnBrowseError(void)			{ BrowseEnded(BROWSE_ERROR);		}
	void Fire_OnBrowseTimeout(void)			{ BrowseEnded(BROWSE_NORESPONSE);	}

	void StartBrowseTimer(int reloadValue);
	void StopBrowseTimer(void);

	// PNP FSM ...
	void Fire_OnPNPCancel(void)				{ PnPEnded(PNP_USER_CANCEL);			}
	void Fire_OnPNPError(void)				{ PnPEnded(PNP_ERROR);					}
	void Fire_OnPNPOK(void)					{ PnPEnded(PNP_OK);						}
	void Fire_OnFactoryAddressFound(void)	{ PnPEnded(PNP_FACTORY_ADDRESS_FOUND);	}
	void Fire_OnDuplicatesFound(void)		{ PnPEnded(PNP_DUPLICATES_FOUND);		}
	void Fire_OnPNPNoResponse(void)			{ PnPEnded(PNP_NORESPONSE);				}
	virtual BOOL sendPnpPhysicalAck(unsigned long ulPhysicalAddress);

	virtual void StartPnPTimer(int reloadValue);
	virtual void StopPnPTimer(void);
	virtual void StartPnPAckTimer(void);
	virtual void StopPnPAckTimer(void);
	virtual void StartPnpPollTimer(int reloadValue);
	virtual void StopPnpPollTimer( );

	// BROWSE + PNP EVENTS ...
	void Fire_EvBrowseGetUnitInfo(BYTE bNodeAddress,BYTE bUnitAddress) { 
		FSMSendEvent(EV_BROWSE_GET_UNIT_INFO, bNodeAddress, bUnitAddress); 
	}
	void Fire_EvUnitNameOk(BYTE bNodeAddress,BYTE bUnitAddress)	{ 
		FSMSendEvent(EV_UNIT_NAME_READ_OK,bNodeAddress, bUnitAddress);	
	}
	void Fire_EvUnitNameEnd(BYTE bNodeAddress,BYTE bUnitAddress) { 
		FSMSendEvent(EV_UNIT_NAME_END, bNodeAddress, bUnitAddress);
	}
	void Fire_EvUnitNameError(BYTE bNodeAddress, BYTE bUnitAddress) { 
		FSMSendEvent(EV_UNIT_NAME_READ_ERROR, bNodeAddress, bUnitAddress);
	}
	void Fire_EvNodeNameOk(BYTE bNodeAddress) { 
		FSMSendEvent(EV_NODE_NAME_READ_OK,bNodeAddress);	
	}
	void Fire_EvNodeNameEnd(BYTE bNodeAddress) { 
		FSMSendEvent(EV_NODE_NAME_END,bNodeAddress);		
	}
	void Fire_EvNodeNameError(BYTE bNodeAddress) { 
		FSMSendEvent(EV_NODE_NAME_READ_ERROR, bNodeAddress);
	}
	void Fire_EvNodeResources(BYTE bNodeAddress, int resourceID) { 
		FSMSendEvent(EV_NODE_RESOURCES0+resourceID, bNodeAddress); 
	}
	void Fire_EvNodeConfig(BYTE bNodeAddress) { 
		FSMSendEvent(EV_NODE_CONFIG, bNodeAddress, -1); 
	}
	void Fire_EvNodeBindingInfo(BYTE bNodeAddress)		{ 
		FSMSendEvent(EV_BROWSE_NODEBINDING_INFO, bNodeAddress, -1);
	}
	void Fire_EvUserCancel(void) { 
		FSMSendEvent(EV_USER_CANCEL);			
	}
	void Fire_EvPNPAddressAssigned(void) { 
		FSMSendEvent(EV_PNP_ADDRESS_ASSIGNED);
	}
	void Fire_EvPNPPhysicalAddress(int physicalAddress)	{ 
		FSMSendEvent(EV_PNP_PHYSICAL_ADDRESS,physicalAddress);
	}

	void Fire_EvTimerNoResponse(void)		{ FSMSendEvent(EV_NORESPONSE);	}
	void Fire_EvTimerBrowseTimeout(void)	{ FSMSendEvent(EV_TIMEOUT);		}
	void Fire_EvTimerPoll(void)				{ FSMSendEvent(EV_PNP_POLL_FSM); }
};
////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_BROWSEPNPENGINE_H__3021B501_45A7_11D8_B865_0050BAC412B1__INCLUDED_)
