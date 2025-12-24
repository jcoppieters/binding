// TXUnitMessage.cpp: implementation of the CTXUnitMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "TXUnitMessage.h"

#include "MainFrm.h"
#include "ComApplication.h"
#include "TMGlobals.h"

#define BROADCAST_UNIT		0xFF


using namespace TMCommunication;



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CTXUnitMessage::CTXUnitMessage()
{

}

CTXUnitMessage::~CTXUnitMessage()
{
}

inline BOOL CTXUnitMessage::IsConnected()
{
	return (TMIsConnected());
}


//////////////////////////////////////////////////////////////////////////////

BOOL CTXUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,FC_UNITREQUESTCONFIG));
}

BOOL CTXUnitMessage::RequestUnitName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bIndex)
{
	ASSERT(TMGetCommunicationApp());
	char szData[3];

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",bIndex);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
											FC_UNITREQUESTNAME,01,szData));
}

BOOL CTXUnitMessage::SetUnitName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bIndex,BYTE bLength,BYTE *pszConvertedData)
{
	ASSERT(TMGetCommunicationApp());
	
	char *szData = NULL;
	BOOL fResult = FALSE;

	if (!IsConnected()) return FALSE;

	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	if (pszConvertedData != NULL) 
	{			
		const int nLen = strlen((char*) pszConvertedData);	
		
		if ((szData = new char[nLen + 3]) != NULL)
		{		
			sprintf(szData,"%02X%s",bIndex,&pszConvertedData[0]);
				
			fResult = TMGetCommunicationApp()->MessageTransfer(bNodeAddress,
																	  bUnitAddress,
																	  FC_UNITSETNAME,
																	  bLength+1,
																	  szData);

			if (szData != NULL) 
			{
				delete [] szData;
				szData = NULL;
			}
		}
	}
	return fResult;
}
//////////////////////////////////////////////////////////////////////////////
//	
//	CTXDimmerUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
CTXDimmerUnitMessage::CTXDimmerUnitMessage() : CTXUnitMessage()
{

}

CTXDimmerUnitMessage::~CTXDimmerUnitMessage() 
{
}

BOOL CTXDimmerUnitMessage::SetOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	ASSERT(TMGetCommunicationApp());
	char szData[3];

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DIMMER_OBJECT_METHOD_OFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
											   FC_UNITDIMSET,01,szData));	
}

BOOL CTXDimmerUnitMessage::SetToggleOnOff(BYTE bNodeAddress,BYTE bUnitAddress)
{	
	ASSERT(TMGetCommunicationApp());
	char szData[3];

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DIMMER_OBJECT_METHOD_FSCVONOFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
											   FC_UNITDIMSET,01,szData));	
}

BOOL CTXDimmerUnitMessage::SetFsDimUpDn(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());
	
	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DIMMER_OBJECT_METHOD_FSUD);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITDIMSET,01,szData));
}

BOOL CTXDimmerUnitMessage::SetOnCv(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	
	sprintf(szData,"%02X",DIMMER_OBJECT_METHOD_ONCV);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITDIMSET,01,szData));
}

BOOL CTXDimmerUnitMessage::SetAttrValue(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bValue)
{
	char szData[5];

	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X%02X",DIMMER_OBJECT_ATTR_VALUE,bValue);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
								 			 FC_UNITDIMSET,02,szData));
}

BOOL CTXDimmerUnitMessage::SetDimUp(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;


	sprintf(szData,"%02X",DIMMER_OBJECT_METHOD_UP);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITDIMSET,01,szData));
}

BOOL CTXDimmerUnitMessage::SetDimDn(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DIMMER_OBJECT_METHOD_DN);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITDIMSET,01,szData));
}

BOOL CTXDimmerUnitMessage::SetFsOnOffVal(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bValue)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X%02X",DIMMER_OBJECT_METHOD_FSCVOFF,bValue);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITDIMSET,02,szData));
}
// -----------------------------------------------------------------------------------

BOOL CTXDimmerUnitMessage::RequestState(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DIMMER_OBJECT_ATTR_STATE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDIMSTATUS,01,szData));
}

BOOL CTXDimmerUnitMessage::RequestValue(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DIMMER_OBJECT_ATTR_VALUE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDIMSTATUS,01,szData));
}

BOOL CTXDimmerUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DIMMER_OBJECT_ATTR_CONFIG);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDIMSTATUS,01,szData));
}

BOOL CTXDimmerUnitMessage::RequestOnOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DIMMER_OBJECT_ATTR_ONOFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDIMSTATUS,01,szData));
}

BOOL CTXDimmerUnitMessage::RequestSpeed(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	
	sprintf(szData,"%02X",DIMMER_OBJECT_ATTR_SPEED);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDIMSTATUS,01,szData));
}

BOOL CTXDimmerUnitMessage::RequestRange(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DIMMER_OBJECT_ATTR_RANGE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDIMSTATUS,01,szData));
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXControlUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
CTXControlUnitMessage::CTXControlUnitMessage() : CTXUnitMessage()
{

}

CTXControlUnitMessage::~CTXControlUnitMessage() {
}

BOOL CTXControlUnitMessage::SetEvPulsStatus(BYTE bNodeAddress,BYTE bUnitAddress,BOOL state)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	if (state == TRUE) 
		sprintf(szData,"%02X%02X",EV_UNITCONTROLPULS,EVENT_ON);
	else 
		sprintf(szData,"%02X%02X",EV_UNITCONTROLPULS,EVENT_OFF);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITCONTROLSET,02,szData));
}

BOOL CTXControlUnitMessage::SetEvControlStatus(BYTE bNodeAddress,BYTE bUnitAddress,BOOL state)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	if (state == TRUE) 
		sprintf(szData,"%02X%02X",EV_UNITCONTROLSTATUS,EVENT_ON);
	else 
		sprintf(szData,"%02X%02X",EV_UNITCONTROLSTATUS,EVENT_OFF);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITCONTROLSET,02,szData));
	
}

// -----------------------------------------------------------------------------------

BOOL CTXControlUnitMessage::RequestControlStatus(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",CONTROL_OBJECT_STATUS);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTCONTROLSTATUS,01,szData)); 
}


//////////////////////////////////////////////////////////////////////////////
//	
//	CTXSwitchUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
CTXSwitchUnitMessage::CTXSwitchUnitMessage() : CTXUnitMessage()
{

}

CTXSwitchUnitMessage::~CTXSwitchUnitMessage() {
}

BOOL CTXSwitchUnitMessage::SetToggleOnOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",SWITCH_OBJECT_METHOD_FSONOFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSWITCHSET,01,szData));
}

BOOL CTXSwitchUnitMessage::SetAttrOnOff(BYTE bNodeAddress,BYTE bUnitAddress,BOOL fState)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X%02X",SWITCH_OBJECT_ATTR_ONOFF,fState);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSWITCHSET,02,szData));
}

BOOL CTXSwitchUnitMessage::SetOn(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",SWITCH_OBJECT_METHOD_ON);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSWITCHSET,01,szData));
}

BOOL CTXSwitchUnitMessage::SetOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",SWITCH_OBJECT_METHOD_OFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSWITCHSET,01,szData));
}

// -----------------------------------------------------------------------------------

BOOL CTXSwitchUnitMessage::RequestState(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",SWITCH_OBJECT_ATTR_STATE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSWITCHSTATUS,01,szData));
}

BOOL CTXSwitchUnitMessage::RequestOnOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	
	sprintf(szData,"%02X",SWITCH_OBJECT_ATTR_ONOFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSWITCHSTATUS,01,szData));
}

BOOL CTXSwitchUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",SWITCH_OBJECT_ATTR_CONFIG);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSWITCHSTATUS,01,szData));
}

BOOL CTXSwitchUnitMessage::RequestStopTimer(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",SWITCH_OBJECT_ATTR_STOPTIMER);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSWITCHSTATUS,01,szData));
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXDuoSwitchUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
CTXDuoSwitchUnitMessage::CTXDuoSwitchUnitMessage() : CTXUnitMessage()
{

}

CTXDuoSwitchUnitMessage::~CTXDuoSwitchUnitMessage() {

}

BOOL CTXDuoSwitchUnitMessage::SetFsUpDown(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DUOSWITCH_OBJECT_METHOD_FSUPDOWN);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITDUOSWITCHSET,01,szData));
					
}

BOOL CTXDuoSwitchUnitMessage::SetUp(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DUOSWITCH_OBJECT_METHOD_UP);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITDUOSWITCHSET,01,szData));
}

BOOL CTXDuoSwitchUnitMessage::SetDown(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DUOSWITCH_OBJECT_METHOD_DOWN);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITDUOSWITCHSET,01,szData));
}

BOOL CTXDuoSwitchUnitMessage::SetStop(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DUOSWITCH_OBJECT_METHOD_STOP);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITDUOSWITCHSET,01,szData));

}

// -----------------------------------------------------------------------------------

BOOL CTXDuoSwitchUnitMessage::RequestState(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DUOSWITCH_OBJECT_ATTR_STATE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDUOSWITCHSTATUS,01,szData));
}

BOOL CTXDuoSwitchUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DUOSWITCH_OBJECT_ATTR_CONFIG);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDUOSWITCHSTATUS,01,szData));
}

BOOL CTXDuoSwitchUnitMessage::RequestStoptime(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DUOSWITCH_OBJECT_ATTR_STOPTIMER);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDUOSWITCHSTATUS,01,szData));
}

BOOL CTXDuoSwitchUnitMessage::RequestSwitchtime(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",DUOSWITCH_OBJECT_ATTR_SWITCHTIMER);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTDUOSWITCHSTATUS,01,szData));
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXAudioBasicUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
CTXAudioBasicUnitMessage::CTXAudioBasicUnitMessage() : CTXUnitMessage()
{

}

CTXAudioBasicUnitMessage::~CTXAudioBasicUnitMessage() {
}

BOOL CTXAudioBasicUnitMessage::SetFsCsOnOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_METHOD_FSCSONOFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,01,szData));	
}

BOOL CTXAudioBasicUnitMessage::SetFsVolUpDn(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_METHOD_FSVOLUPDOWN);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,01,szData));	
}

BOOL CTXAudioBasicUnitMessage::SetCurrentSourceOn(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_METHOD_CSON);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,01,szData));
}

BOOL CTXAudioBasicUnitMessage::SetVolumeUp(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_METHOD_VOLUP);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,01,szData));
}

BOOL CTXAudioBasicUnitMessage::SetVolumeDown(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_METHOD_VOLDOWN);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,01,szData));
}

BOOL CTXAudioBasicUnitMessage::SetVolume(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bVolume)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X%02X",AUDIO_OBJECT_ATTR_VOLUME,bVolume);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioBasicUnitMessage::SetSourceOn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bSource)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X%02X",AUDIO_OBJECT_ATTR_SOURCE_ON,bSource);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioBasicUnitMessage::SetSourceFunction(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bFunction)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X%02X",AUDIO_OBJECT_METHOD_SOURCE_FUNCTION,bFunction);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioBasicUnitMessage::SetSourcePressDown(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_METHOD_PRESDOWN);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,01,szData));
}

BOOL CTXAudioBasicUnitMessage::SetSourcePressUp(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_METHOD_PRESUP);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,01,szData));
}

BOOL CTXAudioBasicUnitMessage::SetDestinationOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_METHOD_OFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,01,szData));
}

// -----------------------------------------------------------------------------------

BOOL CTXAudioBasicUnitMessage::RequestSourceOn(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_ATTR_SOURCE_ON);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,01,szData));
}

BOOL CTXAudioBasicUnitMessage::RequestOnOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_ATTR_ONOFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,01,szData));
}

BOOL CTXAudioBasicUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_ATTR_CONFIG);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,01,szData));
}

BOOL CTXAudioBasicUnitMessage::RequestState(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_ATTR_STATE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,01,szData));
}

BOOL CTXAudioBasicUnitMessage::RequestVolume(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AUDIO_OBJECT_ATTR_VOLUME);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,01,szData));
}


//////////////////////////////////////////////////////////////////////////////
//	
//	CTXSensUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
CTXSensUnitMessage::CTXSensUnitMessage() : 
CTXUnitMessage()
{

}

CTXSensUnitMessage::~CTXSensUnitMessage() {
}

BOOL CTXSensUnitMessage::SetSetpoint(BYTE bNodeAddress,BYTE bUnitAddress,WORD wValue,BYTE bPreset)
{
	char szData[9];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x%02x%02x",SENS_OBJECT_ATTR_WORKING_SET_POINT,
									 wValue / 256,
									 wValue % 256,
									 (bPreset&0x03));

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSENSSET,04,szData));
}

BOOL CTXSensUnitMessage::SetSetpointInc(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	
	sprintf(szData,"%02X",SENS_OBJECT_METHOD_INC);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSENSSET,01,szData));
}

BOOL CTXSensUnitMessage::SetSetPointDec(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
		
	sprintf(szData,"%02X",SENS_OBJECT_METHOD_DEC);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSENSSET,01,szData));
}


BOOL CTXSensUnitMessage::SetPreset(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bPreset)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",SENS_OBJECT_ATTR_PRESET,(bPreset&0x03));					

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSENSSET,02,szData));
}

BOOL CTXSensUnitMessage::SetOnOff(BYTE bNodeAddress,BYTE bUnitAddress,BOOL fState)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",SENS_OBJECT_ATTR_CONTROL_ONOFF,fState);					

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSENSSET,02,szData));
}

BOOL CTXSensUnitMessage::SetFsOnOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
			
	sprintf(szData,"%02X",SENS_OBJECT_METHOD_FSONOFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSENSSET,01,szData));
}

BOOL CTXSensUnitMessage::SetFsSetPointIncDec(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
					
	sprintf(szData,"%02X",SENS_OBJECT_METHOD_FSINC_DEC);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITSENSSET,01,szData));
}

// -----------------------------------------------------------------------------------

BOOL CTXSensUnitMessage::RequestSetPoint(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
				
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_WORKING_SET_POINT);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData));
}

BOOL CTXSensUnitMessage::RequestSetPoint(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bPreset)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",SENS_OBJECT_ATTR_WORKING_SET_POINT,bPreset);								

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,02,szData));
}

BOOL CTXSensUnitMessage::RequestPreset(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
				
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_PRESET);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData));
}

BOOL CTXSensUnitMessage::RequestOnOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
					
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_CONTROL_ONOFF);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData));
}

BOOL CTXSensUnitMessage::RequestSampleInterval(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
				
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_SAMPLE_INTERVAL);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData));
}

BOOL CTXSensUnitMessage::RequestSensType(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
			
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_CONFIG);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData));
}

BOOL CTXSensUnitMessage::RequestMeasuredVal(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
							
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_CURRENT_MEASURE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData));
}

BOOL CTXSensUnitMessage::RequestState(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
					
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_STATE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData));
}

BOOL CTXSensUnitMessage::RequestHystOffset(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
			
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_HYST_OFFSETS);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData));
}

BOOL CTXSensUnitMessage::RequestHystValues(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
					
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_HYST_VALUES);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData));
}

BOOL CTXSensUnitMessage::RequestHystValues(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bPreset)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",SENS_OBJECT_ATTR_HYST_VALUES,(bPreset&0x03));								
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,02,szData));
}

BOOL CTXSensUnitMessage::RequestMeasuredValueOffset(
	BYTE bNodeAddress,
	BYTE bUnitAddress)	/* TM20120404 */
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
					
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_MEASUREDOFFSET);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData)); 
}

BOOL CTXSensUnitMessage::RequestNTCVersionInfo(
	BYTE bNodeAddress,
	BYTE bUnitAddress) /* TM20120404 */
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
					
	sprintf(szData,"%02X",SENS_OBJECT_ATTR_NTC_VERSION);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTSENSSTATUS,01,szData)); 
}


/////////////////////////////////////////////////////////////////////////////
//	
//	CIRTXUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
CIRTXUnitMessage::CIRTXUnitMessage() : CTXUnitMessage()
{

}

CIRTXUnitMessage::~CIRTXUnitMessage() {
}

// solved bug: was szData[5]
BOOL CIRTXUnitMessage::SetSendIRCode(BYTE bNodeAddress,BYTE bUnitAddress,WORD wIRCode)
{
	char szData[10];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x%02x",IRTX_OBJECT_METHOD_PULS_IRTX,
								 wIRCode / 256,
								 wIRCode % 256);								

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITIRTXSET,03,szData));
}

// -----------------------------------------------------------------------------------
	
BOOL CIRTXUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
						
	sprintf(szData,"%02X",IRTX_OBJECT_ATTR_CONFIG);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTIRTXSTATUS,01,szData));
}

BOOL CIRTXUnitMessage::RequestState(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
						
	sprintf(szData,"%02X",IRTX_OBJECT_ATTR_STATE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTIRTXSTATUS,01,szData));
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CIRRC5UnitMessage
//
//////////////////////////////////////////////////////////////////////////////
CIRRC5UnitMessage::CIRRC5UnitMessage() : CTXUnitMessage()
{

}

CIRRC5UnitMessage::~CIRRC5UnitMessage() {

}


BOOL CIRRC5UnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
						
	sprintf(szData,"%02X",RC5RX_OBJECT_ATTR_CONFIG);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTRC5IRRECEIVERSTATUS,01,szData));
}

BOOL CIRRC5UnitMessage::RequestState(BYTE bNodeAddress,BYTE bUnitAddress)
{	
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
					
	sprintf(szData,"%02X",RC5RX_OBJECT_ATTR_STATE);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTRC5IRRECEIVERSTATUS,01,szData));
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CAVMatrixUnitMessage
//
//////////////////////////////////////////////////////////////////////////////

BOOL CTXAVMatrixUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	ASSERT(TMGetCommunicationApp());
	char szData[3];

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",AVMATRIX_OBJECT_ATTR_CONFIG);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAVMATRIXSTATUS,01,szData));
}

/*
BOOL CTXAVMatrixUnitMessage::Requeststate(BYTE bNodeAddress,BYTE bUnitAddress)
{	
	CString szData;
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	szData.Format("%02x",AVMATRIX_OBJECT_ATTR_STATE);								

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAVMATRIXSTATUS,01,szData));
}
*/

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXVideomuxUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
BOOL CTXVideomuxUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",VIDEOMUX_OBJECT_ATTR_CONFIG);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTVIDEOMUXSTATUS,01,szData));
}

BOOL CTXVideomuxUnitMessage::Requeststate(BYTE bNodeAddress,BYTE bUnitAddress)
{	
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	
	sprintf(szData,"%02X",VIDEOMUX_OBJECT_ATTR_STATE);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTVIDEOMUXSTATUS,01,szData));
}
// ----
BOOL CTXVideomuxUnitMessage::SetMethodConnect(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bOutput,BYTE bInput)
{
	char szData[7];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	
	sprintf(szData,"%02X%02X%02X",VIDEOMUX_OBJECT_METHOD_CONNECT,bOutput,bInput);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITVIDEOMUXSET,03,szData));
}

BOOL CTXVideomuxUnitMessage::SetMethodDisconnect(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bOutput,BYTE bInput)
{
	char szData[7];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	
	sprintf(szData,"%02X%02X%02X",VIDEOMUX_OBJECT_METHOD_DISCONNECT,bOutput,bInput);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITVIDEOMUXSET,03,szData));
}

BOOL CTXVideomuxUnitMessage::SetMethodDisconnectAll(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	
	sprintf(szData,"%02X",VIDEOMUX_OBJECT_METHOD_DISCONNECT_ALL);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITVIDEOMUXSET,01,szData));

}
//////////////////////////////////////////////////////////////////////////////
//	
//	CTXPS2MouseUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
BOOL CTXPS2MouseUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",PS2INTELLIMOUSE_OBJECT_ATTR_CONFIG);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTPS2INTELLIMOUSESTATUS,01,szData));
}

BOOL CTXPS2MouseUnitMessage::Requeststate(BYTE bNodeAddress,BYTE bUnitAddress)
{	
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",PS2INTELLIMOUSE_OBJECT_ATTR_STATE);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTPS2INTELLIMOUSESTATUS,01,szData));
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXAlarmUnitMessage
//
//////////////////////////////////////////////////////////////////////////////

BOOL CTXAlarmUnitMessage::SetRemoteCode(BYTE bNodeAddress,BYTE bUnitAddress,BYTE *pbData)
{
	char szData[13];
	BYTE bIndex;
	BOOL fResult;
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	if (pbData == NULL) return FALSE;

	bIndex = 0;

	sprintf(szData,"%02X%02X%02X%02X%02X%02X",ALARM_OBJECT_ATTR_REMOTECODE
											 ,bIndex
											 ,pbData[bIndex]
											 ,pbData[bIndex++]
											 ,pbData[bIndex++]
											 ,pbData[bIndex++]);
	
	fResult = TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
					FC_UNITALARMSET,06,szData);

	if (fResult == FALSE) return FALSE;

	sprintf(szData,"%02X%02X%02X%02X%02X%02X",ALARM_OBJECT_ATTR_REMOTECODE
											 ,bIndex
											 ,pbData[bIndex]
											 ,pbData[bIndex++]
											 ,pbData[bIndex++]
											 ,pbData[bIndex++]);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
					FC_UNITALARMSET,06,szData));
}


BOOL CTXAlarmUnitMessage::SetConfig(BYTE bNodeAddress,BYTE bUnitAddress,BOOL fGalaxy500)
{
	char szData[5];
	BYTE bConfig;
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	bConfig = 0;
	bConfig |= fGalaxy500;
		
	sprintf(szData,"%02x%02x",ALARM_OBJECT_ATTR_CONFIG,bConfig);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
					FC_UNITALARMSET,02,szData));
}

BOOL CTXAlarmUnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x",ALARM_OBJECT_ATTR_CONFIG);							

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTALARMSTATUS,01,szData));
}

BOOL CTXAlarmUnitMessage::Requeststate(BYTE bNodeAddress,BYTE bUnitAddress)
{	
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x",ALARM_OBJECT_ATTR_STATE);								

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTALARMSTATUS,01,szData));
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXAudioHC4UnitMessage
//
//////////////////////////////////////////////////////////////////////////////
BOOL CTXAudioHC4UnitMessage::SetFsCsOnOff(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_FSCSONOFF,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetFsVolUpDn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetCurrentSourceOn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_CSON,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetSourceOn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest,BYTE bSource)
{
	char szData[7];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x%02x",AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON,bDest,bSource);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,03,szData));
}

BOOL CTXAudioHC4UnitMessage::SetSourceInc(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_SOURCEINC,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetSourceDec(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_SOURCEDEC,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetDestOff(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_OFF,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetDestVolume(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest,BYTE bVolume)
{
	char szData[7];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x%02x",AUDIO_MATRIX_OBJECT_ATTR_VOLUME,bDest,bVolume);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,03,szData));
}

BOOL CTXAudioHC4UnitMessage::SetVolumeUp(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_VOLUP,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetVolumeDn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_VOLDOWN,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetVolumeStop(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetMethodFunction(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest,BYTE bFunction)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	switch (bFunction)
	{
		case AUDIO_HC4_FUNCTION_PLAY:
			sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_PLAY,bDest);
		break;

		case AUDIO_HC4_FUNCTION_PAUZE:
			sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_PAUSE,bDest);
		break;

		case AUDIO_HC4_FUNCTION_FW:
			sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_FF,bDest);
		break;

		case AUDIO_HC4_FUNCTION_RW:
			sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_RR,bDest);
		break;

		case AUDIO_HC4_FUNCTION_FFW:
			sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_FFWD,bDest);
		break;

		case AUDIO_HC4_FUNCTION_FRW:
			sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_FRWD,bDest);
		break;

		case AUDIO_HC4_FUNCTION_STOP:
			sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_METHOD_STOP,bDest);
		break;

		default:
			return FALSE;
		break;	
	}
	
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}

BOOL CTXAudioHC4UnitMessage::SetMethodExtraIR(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest,BYTE bIRCode)
{
	char szData[7];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;
	if (bIRCode > IRKEYPAD_BITMAP_DOWN2) return FALSE; 

	sprintf(szData,"%02x%02x%02x",AUDIO_OBJECT_METHOD_EXTRAIR,bDest,bIRCode);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,03,szData));	
}

BOOL CTXAudioHC4UnitMessage::SetMethodMute(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_OBJECT_METHOD_MUTE_UNMUTE,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITAUDIOSET,02,szData));
}
// ---------------------------------

BOOL CTXAudioHC4UnitMessage::RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x",AUDIO_MATRIX_OBJECT_ATTR_CONFIG);								

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,01,szData));
}

BOOL CTXAudioHC4UnitMessage::RequestState(BYTE bNodeAddress,BYTE bUnitAddress)
{	
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x",AUDIO_MATRIX_OBJECT_ATTR_STATE);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,01,szData));
}

BOOL CTXAudioHC4UnitMessage::RequestSource(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,02,szData));
}

BOOL CTXAudioHC4UnitMessage::RequestStateOnOff(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_ATTR_ONOFF,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,02,szData));
}

BOOL CTXAudioHC4UnitMessage::RequestVolume(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest)
{
	char szData[5];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x%02x",AUDIO_MATRIX_OBJECT_ATTR_VOLUME,bDest);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,02,szData));
}

BOOL CTXAudioHC4UnitMessage::RequestVolStatus(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x",AUDIO_MATRIX_OBJECT_ATTR_VOLUME_STATUS);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,01,szData));
}

BOOL CTXAudioHC4UnitMessage::RequestDestStatus(BYTE bNodeAddress,BYTE bUnitAddress)
{
	char szData[3];
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02x",AUDIO_MATRIX_OBJECT_ATTR_DEST_STATUS);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,bUnitAddress,
			FC_UNITREQUESTAUDIOSTATUS,01,szData));
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXEnergyMeasureUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
// IMPLEMENT_UNIT_ENERGY

CTXEnergyMeasureUnitMessage::CTXEnergyMeasureUnitMessage() : 
CTXUnitMessage()
{

}

CTXEnergyMeasureUnitMessage::~CTXEnergyMeasureUnitMessage() 
{

}

BOOL CTXEnergyMeasureUnitMessage::RequestConfig(
	BYTE bNodeAddress,
	BYTE bUnitAddress)
{
	return helper_SendRequest(bNodeAddress,
							  bUnitAddress,
							  EM_ATTR_CONFIG);
}

BOOL CTXEnergyMeasureUnitMessage::RequestState(
	BYTE bNodeAddress,
	BYTE bUnitAddress)
{
	return helper_SendRequest(bNodeAddress,
							  bUnitAddress,
							  EM_ATTR_STATE);
}

BOOL CTXEnergyMeasureUnitMessage::RequestSampleInterval(
	BYTE bNodeAddress,
	BYTE bUnitAddress)
{
	return helper_SendRequest(bNodeAddress,
							  bUnitAddress,
							  EM_ATTR_SAMPLE_INTERVAL);
}

BOOL CTXEnergyMeasureUnitMessage::RequestSupportedFlags(
	BYTE bNodeAddress,
	BYTE bUnitAddress)
{
	return helper_SendRequest(bNodeAddress,
							  bUnitAddress,
							  EM_ATTR_SUPPORTED);
}

//////////////////////////////// protected ///////////////////////////////////

BOOL CTXEnergyMeasureUnitMessage::helper_SendRequest(
	BYTE bNodeAddress,
	BYTE bUnitAddress,
	BYTE bMethod)
{
	char szData[3];
	const int LENGTH = 1;

	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if (bUnitAddress == BROADCAST_UNIT) return FALSE;

	sprintf(szData,"%02X",bMethod);

	return (TMGetCommunicationApp()->MessageTransfer(
				bNodeAddress,
				bUnitAddress,
				FC_UNITREQUESTENERGYMEASURESTATUS,
				LENGTH,
				szData));
}


//////////////////////////////////////////////////////////////////////////////
