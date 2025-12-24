#pragma once
#include "appdali_fwd.h"
#include "appdali_common.h"
#include "appdalifsm_helper.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/	  

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsm;
class CAppDaliTransmitter;
class CAppDaliDeviceProperties_ControlGear;
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsmTimer {
		
public:
	CAppDaliFsmTimer( );
	~CAppDaliFsmTimer( );

	void start(int reloadValue);
	void stop( );
	BOOL tick(int interval);
	BOOL running( );
	BOOL expired( );

private:
	typedef enum {
		STATE_IDLE = 0,
		STATE_RUNNING = 1,
		STATE_EXPIRED = 2		
	} State_t;

	State_t m_state;
	int m_interval;
};
	
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsmEvent {
public:
	typedef enum {
		EVENT_UNKNOWN  = -1,	
		EVENT_BACKWARD_RESPONSE = 0,
		EVENT_BACKWARD_RESPONSE_NO_RESPONSE,
		EVENT_BACKWARD_RESPONSE_ERROR,
		EVENT_TX_DONE,
		EVENT_TIMER_EXPIRY,
		EVENT_CANCEL
	} Event_t;

	CAppDaliFsmEvent(Event_t ev);
	virtual ~CAppDaliFsmEvent( );

	const char* toString( ) const;

private:
	Event_t m_event;
	friend class CAppDaliFsm;
};
/***********************************************************************************/

class CAppDaliFsmEvent_BackwardResponse : public CAppDaliFsmEvent {
public:
	CAppDaliFsmEvent_BackwardResponse(uint8_t data);
	virtual ~CAppDaliFsmEvent_BackwardResponse( );

private:
	uint8_t m_data;

	typedef CAppDaliFsmEvent Base;
	friend class CAppDaliFsm;
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsm : public IAppDaliFsm_HelperFsmEvents {

	typedef void (CAppDaliFsm::*PF_STATE_HANDLERS)(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);

public:
	CAppDaliFsm(IAppDaliFsm_Parent& refParent,CAppDaliTransmitter& refTransmitter);
	~CAppDaliFsm( );

	/////////////////////////////////////////
	// DALI FSM Taken.
	bool startSearchDevices(CAppDaliFsmContext& refContext);
	bool startAddressing(CAppDaliFsmContext& refContext, AddressingInstallation_t type);
	bool startAddressSingleDevice(CAppDaliFsmContext& refContext, DaliDeviceType_t type, int shortAddress, int newShortAddress);
	bool startAddressingSwapDeviceAddresses(CAppDaliFsmContext& refContext, DaliDeviceType_t type, int address_a0, int address_a1); // ,int addres_temp);
	bool startUploadControlGearDeviceProperties(CAppDaliFsmContext& refContext, int shortAddress, CAppDaliDeviceProperties_ControlGear* properties);
	bool startUploadControlGearDeviceScenes(CAppDaliFsmContext& refContext, int shortAddress, CAppDaliDeviceProperties_ControlGear* properties);
	bool startUploadControlGearDeviceSingleScene(CAppDaliFsmContext& refContext, int shortAddress, int sceneNumber, CAppDaliDeviceProperties_ControlGear* properties);
	bool startUploadControlGearDimmerCurve(CAppDaliFsmContext& refContext, int shortAddress, int deviceType,int dimmerCurve);
	bool startRequestControlGearActualValue(CAppDaliFsmContext& refContext, int shortAddress);
	bool startUploadControlInputDeviceInstanceEnableOnOff(CAppDaliFsmContext& refContext, int shortAddress,int instanceNumber,bool stateOnOff);
	bool startUploadControlInputDeviceInstances(CAppDaliFsmContext& refContext, int shortAddress, CAppDaliInputControlDeviceInstances* data);
	bool startReadControlGearDeviceProperties(CAppDaliFsmContext& refContext, int shortAddress);
	bool startReadControlGearDeviceGroups(CAppDaliFsmContext& refContext, int shortAddress);	
	bool startReadControlGearDeviceScenes(CAppDaliFsmContext& refContext, int shortAddress,bool requestDT8ColorInfo);	
	bool cancelTask(CAppDaliFsmContext& refContext);

	///////////////////////////////////
	// Pollen van de FSM timers
	void poll(CAppDaliFsmContext&, int intervalSec);

	///////////////////////////////////
	// DALI FSM event triggers.
	void setDaliFsmEventTransmitDone(CAppDaliFsmContext& );
	void setDaliFsmEventBackwardResponse(CAppDaliFsmContext&, int backwardResponseData);
	void setDaliFsmEventNoBackwardResponse(CAppDaliFsmContext& );
	void setDaliFsmEventBackwardResponseError(CAppDaliFsmContext& );
	void setDaliFsmEventTimerExpiry(CAppDaliFsmContext&);
	void setDaliFsmEventSystemBusFailure(CAppDaliFsmContext&);

	///////////////////////////////////
	// IAppDaliFsm_HelperFsmEvents: Events van de helper FSM algoritmes.	
	virtual void onNotifyFsmAlgoritmeStarted(DaliDeviceType_t daliDeviceType, int shortAddress,AlgoritmeName_t type,int param1,int param2);
	virtual void onNotifyFsmAlgoritmeDone(DaliDeviceType_t daliDeviceType, int shortAddress,AlgoritmeName_t type,int param1,int param2);
	virtual void onNotifyFsmAlgoritmeError(DaliDeviceType_t daliDeviceType, int shortAddress, AlgoritmeName_t type, int param1, int param2);
	virtual void onNotifyFsmAlgoritmeShortAddressAssigned(DaliDeviceType_t AlgoritmeName_t, int shortAddress);
	virtual void onNotifyFsmAlgoritmeControlInputDeviceInstanceConfigured(int shortAddress, int instanceNumber,int instanceType);	
	virtual void onNotifyFsmAlgoritmeMsgInfo(const CString& str);

protected:
	PF_STATE_HANDLERS getHandler(CAppDaliFsmContext& refContext);
	bool invoke(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);

	////////////////////////////////////////////////////////////////////////////////////////
	// state handlers...
	void stateUndefined(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void stateInit(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
		
	////////////////////////////////////////////////////////////////////////////////////////
	// state handlers: Task search devices...
	void taskSearchDevices_enter(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	// Control gear
	void taskSearchDevices_stateSearchContolGearBroadcast(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskSearchDevices_stateControlGearDeviceSearchContolGear(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskSearchDevices_stateControlGearDeviceRequestDeviceProperties(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskSearchDevices_stateControlGearReadMemorymap(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);			
	void taskSearchDevices_stateControlGearDeviceQueryGroups(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskSearchDevices_stateControlGearDeviceQueryScenes(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);	
	void taskSearchDevices_stateWaitControlGearDone(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	// Control input devices...
	void taskSearchDevices_stateSearchBroadcastControlInputDevices(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskSearchDevices_stateSearchControlInputDevice(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskSearchDevices_stateControlInputDeviceRequestDeviceProperties(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskSearchDevices_stateReadMemorymapControlInputDevice(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskSearchDevices_stateControlInputDeviceRequestInstances(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskSearchDevices_stateWaitControlInputDevicesDone(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);

	////////////////////////////////////////////////////////////////////////////////////////
	// state handlers: Task Commissioning devices.
	void taskCommissioning_stateControlGearErase(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	void taskCommissioning_stateControlGearSearchUnAddressed(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	void taskCommissioning_stateControlGearAddress(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskCommissioning_stateControlDevicesErase(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	void taskCommissioning_stateControlDevicesSearchUnAddressed(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	void taskCommissioning_stateControlDevicesAddress(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
	void taskCommissioning_stateControlDevicesInstanceAutoCfgParams(CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);
		
	////////////////////////////////////////////////////////////////////////////////////////
	// device addressing.
	void taskCommissioning_stateAddressSingleDevice(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	void taskCommissioning_stateSwapDeviceAddresses(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);

	////////////////////////////////////////////////////////////////////////////////////////
	// Single control gear cfg params.
	void taskConfig_stateControlGearCfgProperties(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	void taskConfig_stateControlGearCfgScenes(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	void taskConfig_stateControlGearCfgSingleScene(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	void taskConfig_stateControlGearCfgDT6DimmerCurve(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);

	// Opvragen van de status / actual value van het control gear device.
	void taskConfig_stateControlGearQueryActualLevel(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);

	////////////////////////////////////////////////////////////////////////////////////////
	// Single control device instance cfg params.
	void taskConfig_stateControlDeviceInstanceEnableOnOff(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);
	void taskConfig_stateControlDeviceInstances(CAppDaliFsmContext& refContext, const CAppDaliFsmEvent& ev);


	// Helper functie voor de tasks
	// Deze geeft de events door aan het algoritme.
	// TODO: Wat gaan we doen met de return waarde.
	bool helper_state_handleEvent(CAppDaliFsmHelper_Base& refAlg,CAppDaliFsmContext& refContext,const CAppDaliFsmEvent& ev);

	// helper functies.
	void cleanUp( );
	void setStatusDone(CAppDaliFsmContext& refContext);
	void handleAlgoritmeError(CAppDaliFsmContext& refContext, const ErrorInfo& refInfo,bool flagCancelTask = true);
	void startTimer(CAppDaliFsmContext& refContext);
	void stopTimer( );			

protected:
	// Helper functies voor de fsm.
	bool startSearchFirstControlInputDevice(CAppDaliFsmContext& refContext);

	// de transmitter.
	CAppDaliTransmitter& getTransmitter( ) {
		return this->m_refTransmitter;
	}

	////////////////////////////////////////////////////////////////////////////
	// Algoritmes...
	template <typename T>
    T& getAlgorithmHelper(T*& pAlgorithmHelper);

	CAppDaliFsmHelperSearchDevices& getAlgSearchDevices();
	CAppDaliFsmHelperReadMemoryControlGear& getAlgControlGearReadMemory( );
	CAppDaliFsmHelperReadMemoryInputDevice& getAlgInputDeviceReadMemory( );
	CAppDaliFsmHelperQueryGroupsControlGear& getAlgControlGearQueryGroups( );
	CAppDaliFsmHelperQueryScenesControlGear& getAlgControlGearQueryScenes( );
	CAppDaliFsmHelperSearchControlGearDevice& getAlgSearchControlGearDevice( );				
	CAppDaliFsmHelperSearchControlInputDevice& getAlgSearchControlInputDevices();
	CAppDaliFsmHelperRequestPropertiesControlGear& getAlgControlGearRequestProperties();
	CAppDaliFsmHelperRequestPropertiesControlInputDevice& getAlgInputRequestProperties( );
	CAppDaliFsmHelperRequestControlInputDeviceInstances& getAlgInputDeviceRequestInstances( );
	CAppDaliFsmHelperDaliDevicesSearchUnaddressed& getAlgDaliDevicesSearchUnaddressed( );
	CAppDaliFsmHelperDaliDevicesAddress& getAlgControlDaliDevicesAddressDevices( );									
	CAppDaliFsmHelperControlGearErase& getAlgControlGearEraseAddresses();
	CAppDaliFsmHelperControlInputDeviceErase& getAlgControlInputDevicesEraseAddresses();
	CAppDaliFsmHelperControlInputDeviceInstanceAutoParams& getAlgControlInputDeviceInstanceAutoParams();
	CAppDaliFsmHelperAssingShortAddress& getAlgAssingShortAddress( );
	CAppDaliFsmHelperControlGearDeviceCfg& getAlgCfgControlGearProperties();
	CAppDaliFsmHelperControlGearActualValueRequest& getAlgCfgControlGearActualValueRequest();
	CAppDaliFsmHelperControlGearCfgScenes& getAlgControlGearCfgScenes();
	CAppDaliFsmHelperControlGearCfgSingleScene& getAlgControlGearCfgSingleScene();
	CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff& getAlgControlDeviceInstanceEnableOnOff();
	CAppDaliFsmHelperControlInputDeviceInstancesCfg& getAlgControlInputDeviceInstancesCfg();
	CAppDaliFsmHelperSwapShortAddresses& getAlgSwapShortAddresses();
	CAppDaliFsmHelperSelectDimmingCurve& getAlgSelectDimmingCurve();

private:	
	enum {	
		TIMERINTERVAL_RESPONSE_MSEC				= (10000)					// tijd ontvangen van een backward response.		
	};

	// De verschillende task handlers	
	static const PF_STATE_HANDLERS m_handlers[TASK_COMMISSIONING_STATES_MAX];

	// members.
	IAppDaliFsm_Parent& m_refParent;
	CAppDaliTransmitter& m_refTransmitter;
	CAppDaliFsmTimer m_timer;
	// De verschillende algoritmes...
	CAppDaliFsmHelperSearchDevices* m_pAlgSearchDevices;										// 0
	CAppDaliFsmHelperReadMemoryControlGear* m_pAlgControlGearReadMemory;						// 1	
	CAppDaliFsmHelperReadMemoryInputDevice* m_pAlgInputDeviceReadMemory;						// 2
	CAppDaliFsmHelperQueryGroupsControlGear* m_pAlgQueryControlGearGroups;						// 3
	CAppDaliFsmHelperQueryScenesControlGear* m_pAlgQueryControlGearScenes;						// 4
	CAppDaliFsmHelperSearchControlGearDevice* m_pAlgSearchControlGearDevice;					// 5
	CAppDaliFsmHelperSearchControlInputDevice* m_pAlgSearchControlInputDevices;					// 6
	CAppDaliFsmHelperRequestPropertiesControlGear* m_pAlgQueryControlGearDeviceProperties;		// 7
	CAppDaliFsmHelperRequestPropertiesControlInputDevice* m_pAlgQueryInputDeviceProperties;		// 8
	CAppDaliFsmHelperRequestControlInputDeviceInstances* m_pAlgQueryInputDeviceInstances;		// 9
	CAppDaliFsmHelperDaliDevicesSearchUnaddressed* m_pAlgSearchUnaddressedDevices;				// 10
	CAppDaliFsmHelperDaliDevicesAddress* m_pAlgAddressDevices;									// 11
	CAppDaliFsmHelperControlGearErase* m_pAlgControlGearEraseAddresses;							// 12
	CAppDaliFsmHelperControlInputDeviceErase* m_pAlgControlInputDeviceEraseAddresses;			// 13
	CAppDaliFsmHelperControlInputDeviceInstanceAutoParams* m_pAlgControlDeviceInstanceAutoParams;	// 14
	CAppDaliFsmHelperAssingShortAddress* m_pAlgAddressSingleDevice;									// 15
	CAppDaliFsmHelperControlGearDeviceCfg* m_pAlgCfgControlGearProperties;							// 16
	CAppDaliFsmHelperControlGearActualValueRequest* m_pAlgControlGearQueryActualLevel;				// 17
	CAppDaliFsmHelperControlGearCfgScenes* m_pAlgControlGearCfgScenes;								// 18
	CAppDaliFsmHelperControlGearCfgSingleScene* m_pAlgControlGearCfgSingleScene;					// 19
	CAppDaliFsmHelperControlDeviceCfgInstanceEnableOnOff* m_pAlgControlDeviceCfgInstanceEnableOnOff;	// 20
	CAppDaliFsmHelperControlInputDeviceInstancesCfg* m_pAlgControlDeviceCfgInstances;				// 21
	CAppDaliFsmHelperSwapShortAddresses* m_pAlgSwapShortAddresses;									// 22
	CAppDaliFsmHelperSelectDimmingCurve* m_pAlgSelectDimmingCurve;									// 23
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
};
};
/***********************************************************************************/