// PnpEngine.h: interface for the CPnpEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PNPENGINE_H__7A697FC6_44EA_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_PNPENGINE_H__7A697FC6_44EA_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////
#include "NodeDatabase.h"
#include "Nodemess.h"
#include "AppBrowsePnp_common.h"

class CCanNode;
class IBrowsePnPEngine;
class CTXBrowsePnpMessages;
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
class CPnpEngine  
{
private:
	typedef enum {
		STATE_PNP_IDLE=0,	
		STATE_PNP_BROWSE_PHYSICAL,
		STATE_PNP_BROWSE_PHYSICAL_EXTRA_NODES,		// sedert V16.44: Extra toestand.
		STATE_PNP_ASSIGN_LA
	} PnpStates_t;

	typedef void (CPnpEngine::*FSM_MODE)(int pnpEvent,int wParam,int lParam);
	FSM_MODE m_pf_CurrentMode;

public:
	CPnpEngine(BYTE bMode,IBrowsePnPEngine *pEngine,CNodeDatabase *pNodes);	
	virtual ~CPnpEngine();
	void PnpStateMachine(int pnpEvent,int wParam,int lParam);
	void Stop(void);
	void Start(void);

	// publiek zetten voor het uitvoeren van tests.
	class CQueue : public CList<int> {
	public:
		void Add(int address) {
			this->AddTail(address);
		}
		int getFirst( ) {
			if (this->IsEmpty()) {
				return -1;
			}
			return this->RemoveHead();
		}
	};

protected:	
	void FSMAutoPnp(int pnpEvent,int wParam,int lParam);
	void FSMRetrieveInfo(int pnpEvent,int wParam,int lParam);
	
	// Sedert V16.30: Berekenen van de timer reload waarde die nodig is om de nodes te vinden.
	// Deze wordt telkens gevonden ifv het aantal gevonden nodes.
	// Sedert V16.32: Timerwaarden verhoogd.
	int getTimerPnpReloadValue(int foundNodes);

	// Sedert V16.30: Berekenen van het aantal retries die mogen uitgevoerd worden.
	int getMaxPnpRetries(int foundNodes);

	// Sedert V16.43.
	BOOL sendPnpPhysicalAck(unsigned long ulPhysicalAddress);

	// Sedert V16.44: helper functie.
	typedef enum {
		RESULT_BROWSE_PHYSICALNODES_FACTORY_ADDRESSES_FOUND = -3,
		RESULT_BROWSE_PHYSICALNODES_DUPLICATES_FOUND = -2,
		RESULT_BROWSE_PHYSICALNODES_NO_NODES_FOUND = -1,
		RESULT_BROWSE_PHYSICALNODES_SUCCESS = 0,	
	} ResultBrowsePhysicalNodes_t;
	ResultBrowsePhysicalNodes_t checkDatabaseFoundDevices(BOOL checkForFactoryAddresses);

	// Sedert V16.44
	// Wanneer pNode een factory init adres heeft dan wordt er gezocht naar een volgend beschikbaar adres.
	// return waarde: 
	//	- TRUE = adres ok (ook als er geen adres aantoegekend wordt.)
	//  - FALSE = geen vrije adressen meer.
	BOOL helper_checkNodeFactoryAddressAndAssignNextAddress(CCanNode* pNode);

private:
	enum { 
		// Tijd die er gewacht wordt om te antwoorden met een FC_PNPPHYSADRACK.
		TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC = 1500,	

		// Polling interval om de queue te controleren en te antwoorden met een FC_PNPPHYSADRACK
		TIMER_PNP_POLL_RELOAD_VALUE_MSEC = 10,				// 100 nodes in een seconde.

		// Sedert V16.44.
		// Tijd voor de extra broadcast om naar LCD en Gateway nodes te zoeken.
		TIMER_PNP_BROADCAST_RELOAD_EXTRA_NODES_VALUE_MSEC = 12000,	

		// Sedert V16.44
		// Waarde die bepaalt of er nog een extra browse for nodes uitgevoerd wordt opzoek naar
		// nodes van het type Gateway en LCD.
		THRESSHOLD_SEARCH_EXTRA_NODES = 50, 			// Vanaf 50 nodes nog eens extra de LCD en gateway nodes opvragen.

		// Sedert V16.44
		// Waarde die bepaalt of er nog een extra browse for nodes uitgevoerd wordt opzoek naar
		// standaard nodes.
		THRESSHOLD_SEARCH_EXTRA_NODES_NORMAL = 65,		// Vanaf 65 nodes nog eens extra de normale nodes opvragen.

		// Sedert V16.44
		// Extra browse for nodes maximum aantal keren dat er opnieuw proberen extra nodes te vinden.
		PNP_SEARCH_EXTRA_NODES_MAX_RETRIES = 5
	};

	static const int BROWSE_NODE_TYPE_FLAGS_ALL;
	static const int BROWSE_NODE_TYPE_FLAGS_EXTRA;

	CNodeDatabase* m_pDatabase;
	IBrowsePnPEngine* m_pBrowsePnpEngine;		
	CTXBrowsePnpMessages* m_pTxPnpMessages;
	CList<int> m_listPhysicalAddress;
	CQueue m_queuePhysicalAddressPendingAck;	
	BYTE m_bLogicalAddress;
	PnpStates_t m_CurrentPnpState;
	unsigned int m_NodeCount;
	unsigned int m_CurrentBrowseNodesCounter;
	unsigned int m_PreviousBrowseNodes;
	unsigned int m_maxRetryCountValue;
	unsigned int m_RetryCount;	
	unsigned int m_NrOfNodesToAssignAddresses;			// Het aantal nodes in de node database aan wie er een LA adres moet toegekend worden.
	bool m_searchNormalNodes;							// Nog eens extra zoeken naar standaard nodes.
};

//////////////////////////////////////////////////////////////

#endif // !defined(AFX_PNPENGINE_H__7A697FC6_44EA_11D8_B865_0050BAC412B1__INCLUDED_)
