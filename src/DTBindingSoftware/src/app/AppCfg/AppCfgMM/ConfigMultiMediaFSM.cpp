// ConfigMultiMediaFSM.cpp: implementation of the CConfigMultiMediaFSM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigMultiMediaFSM.h"

//----------------------------------------
#include "ConfigMultiMediaInfo.h"
#include "ConfigMultiMediaFSMMaster.h"
//----------------------------------------
#include "app\appCfg\AppCfginfo\cfginfonode.h"	
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"		// since 0x0560
#include "Nodemess.h"
#include "TxNodeMessage.h"
//----------------------------------------

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigMultiMediaFSM::CConfigMultiMediaFSM(IConfigMultiMediaFSMMaster *pParent) :
m_pMaster(pParent),
//m_pMultimediaFile( 0 ),
m_pCurrentClass( 0 ),
m_bCurrentIndex( 0 ),
m_bMaxIndex( 0 ),
m_CurrentState( STATE_CONFIG_MM_IDLE ) 
{
}
CConfigMultiMediaFSM::~CConfigMultiMediaFSM()
{
}
///////////////////////////////////////////////////////////////////////
void CConfigMultiMediaFSM::Init(int mode)
{
	ASSERT( mode == NODEMULTIMEDIA_CONFIG_TYPE_V64 ||
		    mode == NODEMULTIMEDIA_CONFIG_TYPE_V65 ||
			mode == NODEMULTIMEDIA_CONFIG_TYPE_V66);

	m_StateInfo[STATE_CONFIG_MM_SOURCE].ConfigClass = ENUM_CONFIGCLASS_AUDIOSOURCE;	
	m_StateInfo[STATE_CONFIG_MM_SOURCE].bMethodData = AUDIO_CONFIG_SET_SOURCE;
	m_StateInfo[STATE_CONFIG_MM_SOURCE].szInfo.Format(_T("Uploading source:"));
	m_StateInfo[STATE_CONFIG_MM_SOURCE].NextState = STATE_CONFIG_MM_DEST;

	m_StateInfo[STATE_CONFIG_MM_DEST].ConfigClass = ENUM_CONFIGCLASS_AUDIODEST;	
	m_StateInfo[STATE_CONFIG_MM_DEST].bMethodData = AUDIO_CONFIG_SET_DESTINATION;
	m_StateInfo[STATE_CONFIG_MM_DEST].szInfo.Format(_T("Uploading destination:"));
	m_StateInfo[STATE_CONFIG_MM_DEST].NextState = STATE_CONFIG_MM_ROOM;

	m_StateInfo[STATE_CONFIG_MM_ROOM].ConfigClass = ENUM_CONFIGCLASS_AUDIOROOM;	
	m_StateInfo[STATE_CONFIG_MM_ROOM].bMethodData = AUDIO_CONFIG_SET_ROOM;
	m_StateInfo[STATE_CONFIG_MM_ROOM].szInfo.Format(_T("Uploading audioroom:"));
	m_StateInfo[STATE_CONFIG_MM_ROOM].NextState = STATE_CONFIG_MM_MUXSRC;

	m_StateInfo[STATE_CONFIG_MM_MUXSRC].ConfigClass = ENUM_CONFIGCLASS_VIDEOMUXSRC;	
	m_StateInfo[STATE_CONFIG_MM_MUXSRC].bMethodData = VIDEOMUXSOURCE_CONFIG_SET;
	m_StateInfo[STATE_CONFIG_MM_MUXSRC].szInfo.Format(_T("Uploading videomux src:"));
	m_StateInfo[STATE_CONFIG_MM_MUXSRC].NextState = STATE_CONFIG_MM_MUXDEST;

	m_StateInfo[STATE_CONFIG_MM_MUXDEST].ConfigClass = ENUM_CONFIGCLASS_VIDEOMUXDEST;	
	m_StateInfo[STATE_CONFIG_MM_MUXDEST].bMethodData = VIDEOMUXDESTINATION_CONFIG_SET;
	m_StateInfo[STATE_CONFIG_MM_MUXDEST].szInfo.Format(_T("Uploading videomux dest:"));
	m_StateInfo[STATE_CONFIG_MM_MUXDEST].NextState = STATE_CONFIG_MM_SPEECHINDOOR;

	m_StateInfo[STATE_CONFIG_MM_SPEECHINDOOR].ConfigClass = ENUM_CONFIGCLASS_SPEECHDEVINDOOR;	
	m_StateInfo[STATE_CONFIG_MM_SPEECHINDOOR].bMethodData = SPEECHDEVINDOOR_CONFIG_SET;
	m_StateInfo[STATE_CONFIG_MM_SPEECHINDOOR].szInfo.Format(_T("Uploading speech dev. indoor:"));
	m_StateInfo[STATE_CONFIG_MM_SPEECHINDOOR].NextState = STATE_CONFIG_MM_SPEECHOUTDOOR;

	m_StateInfo[STATE_CONFIG_MM_SPEECHOUTDOOR].ConfigClass = ENUM_CONFIGCLASS_SPEECHDEVOUTDOOR;	
	m_StateInfo[STATE_CONFIG_MM_SPEECHOUTDOOR].bMethodData = SPEECHDEVOUTDOOR_CONFIG_SET;
	m_StateInfo[STATE_CONFIG_MM_SPEECHOUTDOOR].szInfo.Format(_T("Uploading speech dev. outdoor:"));
	m_StateInfo[STATE_CONFIG_MM_SPEECHOUTDOOR].NextState = STATE_CONFIG_MM_VIDEOMUX;

	m_StateInfo[STATE_CONFIG_MM_VIDEOMUX].ConfigClass = ENUM_CONFIGCLASS_VIDEOMUX;	
	m_StateInfo[STATE_CONFIG_MM_VIDEOMUX].bMethodData = VIDEOMUX_CONFIG_SET;
	m_StateInfo[STATE_CONFIG_MM_VIDEOMUX].szInfo.Format(_T("Uploading videomux configuration"));
	m_StateInfo[STATE_CONFIG_MM_VIDEOMUX].NextState = STATE_CONFIG_MM_AVMATRIX;

	m_StateInfo[STATE_CONFIG_MM_AVMATRIX].ConfigClass = ENUM_CONFIGCLASS_AVMATRIX;	
	m_StateInfo[STATE_CONFIG_MM_AVMATRIX].bMethodData = AVMATRIX_CONFIG_SET;
	m_StateInfo[STATE_CONFIG_MM_AVMATRIX].szInfo.Format(_T("Uploading AVMatrix configuration:"));	/* TM_MULTIPLE_AVMATRIX ':'*/
	m_StateInfo[STATE_CONFIG_MM_AVMATRIX].NextState = STATE_CONFIG_MM_VIDEOPHONE;

	m_StateInfo[STATE_CONFIG_MM_VIDEOPHONE].ConfigClass = ENUM_CONFIGCLASS_VIDEOPHONE;	
	m_StateInfo[STATE_CONFIG_MM_VIDEOPHONE].bMethodData = VIDEOPHONE_CONFIG_SET;
	m_StateInfo[STATE_CONFIG_MM_VIDEOPHONE].szInfo.Format(_T("Uploading videophone configuration"));
	m_StateInfo[STATE_CONFIG_MM_VIDEOPHONE].NextState = STATE_CONFIG_MM_ALARMZONES;


	if ((NODEMULTIMEDIA_CONFIG_TYPE_V65 == mode)||(NODEMULTIMEDIA_CONFIG_TYPE_V66 == mode))
	{
		/********************************************************************//**
		 * @attention 
		 * Als laatste toestand wordt STATE_CONFIG_MM_DONE toegekend.\n
		 * Dit is enkel voor de nodes V65.\n
		 * Deze toestand stuurt op het einde van de FSM een Config Done bericht.
		 ***********************************************************************/
		m_StateInfo[STATE_CONFIG_MM_ALARMZONES].ConfigClass = ENUM_CONFIGCLASS_ALARMZONE;	
		m_StateInfo[STATE_CONFIG_MM_ALARMZONES].bMethodData = ALARMZONE_CONFIG_SET;
		m_StateInfo[STATE_CONFIG_MM_ALARMZONES].szInfo.Format(_T("Uploading Alarmzone:"));
		m_StateInfo[STATE_CONFIG_MM_ALARMZONES].NextState = STATE_CONFIG_MM_DONE_V65;	/** @attention */	

		/** @note TM 20091226 Kunnen hier extra toestanden aanmaken ifv de mode */
	}
	else
	{
		/********************************************************************//**
         *
		 *
		 ***********************************************************************/
		m_StateInfo[STATE_CONFIG_MM_ALARMZONES].ConfigClass = ENUM_CONFIGCLASS_ALARMZONE;	
		m_StateInfo[STATE_CONFIG_MM_ALARMZONES].bMethodData = ALARMZONE_CONFIG_SET;
		m_StateInfo[STATE_CONFIG_MM_ALARMZONES].szInfo.Format(_T("Uploading Alarmzone:"));
		m_StateInfo[STATE_CONFIG_MM_ALARMZONES].NextState = STATE_CONFIG_MM_DONE_V64;	/** @attention */	
	}
}

BOOL CConfigMultiMediaFSM::Start(CConfigMultiMediaInfo *pInfo,BYTE bNodeLogicalAddress, int mode)
{
	if (m_CurrentState != STATE_CONFIG_MM_IDLE) 
	{
		return FALSE;
	}
		
	// Start Config ...
	//if ((m_pMultimediaFile = pInfo->GetMultiMediaFile()) != 0) 
	if (pInfo && pInfo->IsValidMultiMediaFile())
	{
		//m_pMultiMediaInfo = pInfo;

		Init( mode );	// TM20091218 De toestanden configureren ifv de mode.

		StartTimer();		
				
		m_pCurrentClass = GetClass(*pInfo, m_StateInfo[STATE_CONFIG_MM_SOURCE].ConfigClass);

		if (m_pCurrentClass != 0) 
		{
			m_bCurrentIndex = 0;
			m_bMaxIndex = 0;
			//m_bNodeAddress = bNodeLogicalAddress;
			m_CurrentState = STATE_CONFIG_MM_SOURCE;

			// Send External Config init ...
			CString s("Initialise external config.");
			DisplayInfoMsg(s);
		
			TxMessage(bNodeLogicalAddress, EXTERNAL_CONFIG_INITIALIZE);
			return TRUE;
		}

		CString s("ptr to multimedia class = 0,can't start.");
		DisplayErrorMsg(s);
		return FALSE;
	}
	else 
	{
		CString s("ptr to multimedia file = 0,can't start.");
		DisplayErrorMsg(s);
		return FALSE;
	}
}

void CConfigMultiMediaFSM::Fire_EvDone(void)
{
	m_CurrentState = STATE_CONFIG_MM_IDLE;
	
	CString s("Configuration file is send");
	DisplayInfoMsg(s);

	m_pMaster->NotifyOnEventDone();
}

void CConfigMultiMediaFSM::Fire_EvError(void)
{
	m_CurrentState = STATE_CONFIG_MM_IDLE;	
	m_pMaster->NotifyOnEventError();
}

BOOL CConfigMultiMediaFSM::EvSignalStatusConfigError(
	BYTE bNodeAddress,
	CConfigMultiMediaInfo* pInfo)
{
	if (m_CurrentState == STATE_CONFIG_MM_IDLE) 
	{
		return TRUE;
	}

	m_CurrentState = STATE_CONFIG_MM_IDLE;

	CString s("Error Received - FSM is stopped");
	DisplayInfoMsg(s);
	
	m_pMaster->NotifyOnEventError();
	return TRUE;
}


BOOL CConfigMultiMediaFSM::EvAfterConfigTimeout(BYTE bNodeAddress,CConfigMultiMediaInfo* pInfo)
{
	if (m_CurrentState == STATE_CONFIG_MM_IDLE) {
		return TRUE;
	}

	m_CurrentState = STATE_CONFIG_MM_IDLE;

	CString s("Timeout - FSM is stopped");
	DisplayInfoMsg(s);

	m_pMaster->NotifyOnEventError();
	return TRUE;
}


BOOL CConfigMultiMediaFSM::EvSignalStatusConfigOk(
	BYTE bNodeAddress,
	CConfigMultiMediaInfo* pInfo)
{
	enum { MAX_SEGMENTED_LENGTH	 = 128 };

	ASSERT( m_CurrentState != STATE_CONFIG_MM_DONE_V64 );

	if (m_CurrentState == STATE_CONFIG_MM_DONE_V65)
	{
		// Speciale toestand voor de nodes v65.
		// Er wordt op het laatste een Config done bericht verstuurd.
		// We wachten hier op de acknowledge, bij het krijgen van een ACK
		// stopt de FSM.

		StopTimer();

		Fire_EvDone();		// FSM gedaan
		return TRUE;
	}
	else if (m_CurrentState != STATE_CONFIG_MM_IDLE) 
	{
		StopTimer();

		ASSERT( m_pCurrentClass );
		if (m_pCurrentClass == 0) 
		{
			CString s("ptr to multimedia class = NULL,can't continue.");
			DisplayErrorMsg(s);
			Fire_EvError();
			return FALSE;
		}
		
		m_bMaxIndex = m_pCurrentClass->GetMax();
			
		// all indexes done for current class ...
		if (m_bCurrentIndex == m_bMaxIndex) 
		{	
			if (m_StateInfo[m_CurrentState].NextState == STATE_CONFIG_MM_DONE_V64) 
			{
				Fire_EvDone();		// FSM gedaan
				return TRUE;
			}
			else if (m_StateInfo[m_CurrentState].NextState == STATE_CONFIG_MM_DONE_V65)
			{
				// Speciale toestand voor de nodes v65.
				// Er wordt op het laatste een Config done bericht verstuurd.

				StartTimer( );
				TxMessage(bNodeAddress, EXTERNAL_CONFIG_DONE);
				m_CurrentState = m_StateInfo[m_CurrentState].NextState;
				return TRUE;
			}
			else 
			{
				m_CurrentState = m_StateInfo[m_CurrentState].NextState;
			}

			// restart again ...
			m_bCurrentIndex = 0;

			ASSERT( pInfo );
			if (pInfo)
			{
				m_pCurrentClass = GetClass(*pInfo,m_StateInfo[m_CurrentState].ConfigClass);
			}
			
			ASSERT( m_pCurrentClass );
			if (m_pCurrentClass == NULL) 
			{
				CString s("ptr to multimedia class = NULL,can't continue.");
				DisplayErrorMsg(s);

				Fire_EvError();
				return TRUE;
			}
		}
		
		BOOL fContinue = TRUE;
		while(fContinue) 
		{							
			if (m_pCurrentClass->IsUsed(m_bCurrentIndex)) 
			{									
				// Info Msg ...
				if ((m_CurrentState == STATE_CONFIG_MM_VIDEOMUX) || 
					//(m_CurrentState == STATE_CONFIG_MM_AVMATRIX) ||	/* TM_MULTIPLE_AVMATRIX */
					(m_CurrentState == STATE_CONFIG_MM_VIDEOPHONE)) 
				{				
					CString s(m_StateInfo[m_CurrentState].szInfo);
					DisplayInfoMsg(s);
				}
				else 
				{			
					CString s;
					s.Format("%s%d",m_StateInfo[m_CurrentState].szInfo,m_bCurrentIndex);
					DisplayInfoMsg(s);
				}											
				StartTimer();
						
				BYTE pbData[MAX_SEGMENTED_LENGTH];
				m_pCurrentClass->GetData(m_bCurrentIndex,&pbData[0]);
				TxMessage(bNodeAddress, m_StateInfo[m_CurrentState].bMethodData, m_bCurrentIndex,&pbData[0]);															

				m_bCurrentIndex++;
				return TRUE;				
			}	

			if (++m_bCurrentIndex >= m_bMaxIndex) 
			{
				fContinue = FALSE;			
			}
		} // end while ...

		// We leave the while loop and no messages has been send ...
		StartTimer();	
		this->EvSignalStatusConfigOk(bNodeAddress, pInfo);		// Recursive aanroep

		return TRUE;
	}
	return FALSE;
}


// TODO Use TXNodeMessage !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void CConfigMultiMediaFSM::TxMessage(BYTE bNodeAddress,BYTE bMethod,BYTE bData,BYTE *pbData)
{	
	if (bMethod == EXTERNAL_CONFIG_INITIALIZE || bMethod == EXTERNAL_CONFIG_DONE) {
		CTXExternalConfigMessages::getInstance()->SendControl(bNodeAddress, bMethod);
	}
	else 
	{
		if (bData == 0xFF) {				
			CTXExternalConfigMessages::getInstance()->SendConfig(bNodeAddress, bMethod, pbData);
		}
		else {		
			CTXExternalConfigMessages::getInstance()->SendConfig(bNodeAddress, bMethod, bData, pbData);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void CConfigMultiMediaFSM::StartTimer(void)
{
	m_pMaster->StartTimer();
}
void CConfigMultiMediaFSM::StopTimer(void)
{
	m_pMaster->StopTimer();
}
void CConfigMultiMediaFSM::DisplayInfoMsg(CString s)
{
	m_pMaster->NotifyOnEventInfo(s);
}
void CConfigMultiMediaFSM::DisplayErrorMsg(CString s)
{
	m_pMaster->NotifyOnEventInfo(s);
}
///////////////////////////////////////////////////////////////////////////////

BOOL CConfigMultiMediaFSM::Cancel(void)
{
	if (m_CurrentState == STATE_CONFIG_MM_IDLE) 
	{
		return TRUE;
	}

	m_CurrentState = STATE_CONFIG_MM_IDLE;

	CString s("User cancel - FSM is stopped");
	DisplayInfoMsg(s);

	m_pMaster->NotifyOnEventError();
	return TRUE;
}

CConfigExternalClass* CConfigMultiMediaFSM::GetClass(CConfigMultiMediaInfo& info,int type)
{	
	return info.GetClass(type);
}