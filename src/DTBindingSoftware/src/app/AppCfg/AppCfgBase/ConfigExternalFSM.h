#ifndef INCLUDED_DUOTECNO_CONFIGEXTERNAL_FSM_H
#define INCLUDED_DUOTECNO_CONFIGEXTERNAL_FSM_H
/***********************************************************/
#include "utils/mfc/ITimerControl.h"
#include "utils/mfc/IDisplayDevice.h"
/***********************************************************/
struct CConfigStateInfo;
struct CConfigExternalClass;
/***********************************************************/
struct IConfigExternalFSM_Events
{
	virtual void NotifyOnEventDone( )		= 0;
	virtual void NotifyOnEventError( )		= 0;	
};
/***********************************************************/
struct IConfigExternalFSM_Master
{
	virtual IConfigExternalFSM_Events& getEventSink( )					= 0;
	virtual DUOTECNO::MFC_HELPER::ITimerControl& getTimerControl( )		= 0;
	virtual DUOTECNO::MFC_HELPER::IDisplayDevice& getDisplayDevice( )	= 0;
};
/***********************************************************/
class CConfigExternalFSM
{
public:
	enum 
	{ 
		TIMER_ID = 0 
	};
protected:
	IConfigExternalFSM_Master& m_parent;
	CConfigStateInfo* m_pStateInfo;
	CConfigStateInfo* m_pCurrentState;
	CConfigExternalClass** m_pConfigClasses;
	CConfigExternalClass* m_pCurrentClass;

	int m_currentState;
	int m_currentIndex;
	int m_visualisationIndex;
	//int m_previousIndex;

	BYTE m_bNodeAddress;

protected:
	inline void StartTimer(void);
	inline void StopTimer(void);
	inline void DisplayInfoMsg(const char* s);
	inline void DisplayErrorMsg(const char* s);

	void TxMessage(BYTE bMethod);
	void TxData( );
	void DisplayCurrent(const char* s);

	void Fire_EvDone(void);
	void Fire_EvError(void);

public:
	CConfigExternalFSM(IConfigExternalFSM_Master& parent);
	virtual ~CConfigExternalFSM( );

public:
	BOOL Start(BYTE bNodeAddress,
			   CConfigExternalClass** pConfigClasses,
			   CConfigStateInfo* pStateInfo,
			   int visualisationIndex = 0);

	BOOL Start(BYTE bNodeAddress,			   
			   CConfigStateInfo* pStateInfo);

	BOOL Cancel( );

public:
	BOOL EvSignalStatusConfigOk( );
	BOOL EvSignalStatusConfigError( );
	BOOL EvAfterConfigTimeout( );	
};

/***********************************************************/
#endif 
/***********************************************************/
