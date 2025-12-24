// ConfigMultiMediaFSM.h: interface for the CConfigMultiMediaFSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGMULTIMEDIAFSM_H__4D185DE2_BA52_4A50_A940_F1122167F16C__INCLUDED_)
#define AFX_CONFIGMULTIMEDIAFSM_H__4D185DE2_BA52_4A50_A940_F1122167F16C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////////
#include "ConfigMultiMediaFile.h"
//////////////////////////////////////////////////////////////////////////////
struct CConfigExternalClass;
class CConfigMultiMediaInfo;
class IConfigMultiMediaFSMMaster;
//////////////////////////////////////////////////////////////////////////////
typedef enum 
{
	STATE_CONFIG_MM_SOURCE = 0,
	STATE_CONFIG_MM_DEST,
	STATE_CONFIG_MM_ROOM,
	STATE_CONFIG_MM_MUXSRC,
	STATE_CONFIG_MM_MUXDEST,
	STATE_CONFIG_MM_SPEECHINDOOR,
	STATE_CONFIG_MM_SPEECHOUTDOOR,
	STATE_CONFIG_MM_VIDEOMUX,
	STATE_CONFIG_MM_AVMATRIX,
	STATE_CONFIG_MM_VIDEOPHONE,
	STATE_CONFIG_MM_ALARMZONES,

	STATE_CONFIG_MM_MAX,			// Altijd als laatste: Laatste in de ARRAY !!!

	STATE_CONFIG_MM_DONE_V64,		// TM20091218 Enkel voor nodes v64
	STATE_CONFIG_MM_DONE_V65,		// TM20091218 Enkel voor nodes v65

	STATE_CONFIG_MM_IDLE = 0xFF,

} ENUM_CONFIG_MM_STATES;


typedef struct 
{	
	ENUM_MULTIMEDIA_CONFIGCLASS ConfigClass;
	BYTE bMethodData;
	CString szInfo;
	ENUM_CONFIG_MM_STATES NextState;

}STRUCT_MULTIMEDIA_STATE_DATA;

/*******************************************************************************//**
 * @class	CConfigMultiMediaFSM.
 * 
 * @changes	TM20091218: verandering aan de FSM\n
 *			Initialisatie en uitvoeren van de FSM gebeurt ifv het nodetype (versie)
 *			van de nodes.
 *
 **********************************************************************************/

class CConfigMultiMediaFSM  
{
protected:
	IConfigMultiMediaFSMMaster *const m_pMaster;

	CConfigExternalClass* m_pCurrentClass;	
	//BYTE m_bNodeAddress;
	STRUCT_MULTIMEDIA_STATE_DATA m_StateInfo[STATE_CONFIG_MM_MAX];
	ENUM_CONFIG_MM_STATES m_CurrentState;
	BYTE m_bCurrentIndex;
	BYTE m_bMaxIndex;

protected:	
	void Init(int mode);
	void Fire_EvDone( );
	void Fire_EvError( );
	void StartTimer( );
	void StopTimer( );
	void DisplayInfoMsg(CString s);
	void DisplayErrorMsg(CString s);
	void TxMessage(BYTE bNodeAddress,BYTE bMethod,BYTE bData=0xFF,BYTE *pbData=NULL);

public:
	CConfigMultiMediaFSM(IConfigMultiMediaFSMMaster *pParent);
	virtual ~CConfigMultiMediaFSM();

	BOOL Start(CConfigMultiMediaInfo *pInfo,BYTE bNodeLogicalAddress,int mode);
	BOOL Cancel(void);
	BOOL EvSignalStatusConfigOk(BYTE bNodeAddress, CConfigMultiMediaInfo* pInfo);
	BOOL EvSignalStatusConfigError(BYTE bNodeAddress, CConfigMultiMediaInfo* pInfo);
	BOOL EvAfterConfigTimeout(BYTE bNodeAddress, CConfigMultiMediaInfo* pInfo);	

private:
	CConfigExternalClass* GetClass(CConfigMultiMediaInfo& info,int type);	
};

#endif // !defined(AFX_CONFIGMULTIMEDIAFSM_H__4D185DE2_BA52_4A50_A940_F1122167F16C__INCLUDED_)
