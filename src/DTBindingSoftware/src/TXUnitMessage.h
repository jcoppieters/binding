// TXUnitMessage.h: interface for the CTXUnitMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TXUNITMESSAGE_H__BA2D3A50_62F7_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_TXUNITMESSAGE_H__BA2D3A50_62F7_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////////////

#ifdef BINDING_SOFTWARE
	//class CComApplication;
#else
	class CCommunicationClass;
#endif


//////////////////////////////////////////////////////////////////////////////
//	
//	CTXUnitMessage
//
//////////////////////////////////////////////////////////////////////////////

#ifdef BINDING_SOFTWARE
	
	class CTXUnitMessage  
	{
	public:
		CTXUnitMessage();
		virtual ~CTXUnitMessage();			
	protected:	
		inline BOOL IsConnected();

	public:
		BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
		BOOL RequestUnitName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bIndex);

		BOOL SetUnitName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bIndex,BYTE bLength,BYTE *pszConvertedData);
	};

#else

	class CTXUnitMessage  
	{
	public:
		CTXUnitMessage();
		virtual ~CTXUnitMessage();	

		static void AssignPtr(CCommunicationClass *ptr) {
			pComApplication = ptr;
		}

	protected:
	
		BOOL IsConnected();
		static CCommunicationClass* pComApplication;

	public:
		BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
		BOOL RequestUnitName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bIndex);

		BOOL SetUnitName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bIndex,BYTE bLength,BYTE *pszConvertedData);
	};


#endif


//////////////////////////////////////////////////////////////////////////////
//	
//	CTXDimmerUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXDimmerUnitMessage : public CTXUnitMessage
{
public:
	CTXDimmerUnitMessage();
	virtual ~CTXDimmerUnitMessage();
public:
	BOOL SetToggleOnOff(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetFsDimUpDn(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetOnCv(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetAttrValue(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bValue);

	BOOL SetDimUp(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetDimDn(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetFsOnOffVal(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bValue);

	BOOL SetOff(BYTE bNodeAddress,BYTE bUnitAddress);

	BOOL RequestState(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestValue(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestOnOff(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestSpeed(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestRange(BYTE bNodeAddress,BYTE bUnitAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXSwitchUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXSwitchUnitMessage : public CTXUnitMessage
{
public:
	CTXSwitchUnitMessage();// {} ;
	virtual ~CTXSwitchUnitMessage();// {};	
public:
	BOOL SetToggleOnOff(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetAttrOnOff(BYTE bNodeAddress,BYTE bUnitAddress,BOOL fState);
	BOOL SetOn(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetOff(BYTE bNodeAddress,BYTE bUnitAddress);

	BOOL RequestState(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestOnOff(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestStopTimer(BYTE bNodeAddress,BYTE bUnitAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXControlUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXControlUnitMessage : public CTXUnitMessage
{
public:
	CTXControlUnitMessage();// {} ;
	virtual ~CTXControlUnitMessage();// {};	
public:
	BOOL SetEvPulsStatus(BYTE bNodeAddress,BYTE bUnitAddress,BOOL state);
	BOOL SetEvControlStatus(BYTE bNodeAddress,BYTE bUnitAddress,BOOL state);

	BOOL RequestControlStatus(BYTE bNodeAddress,BYTE bUnitAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXDuoSwitchUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXDuoSwitchUnitMessage : public CTXUnitMessage
{
public:
	CTXDuoSwitchUnitMessage();// {} ;
	virtual ~CTXDuoSwitchUnitMessage();// {};	
public:
	BOOL SetFsUpDown(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetStop(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetUp(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetDown(BYTE bNodeAddress,BYTE bUnitAddress);

	BOOL RequestState(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestStoptime(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestSwitchtime(BYTE bNodeAddress,BYTE bUnitAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXAudioBasicUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXAudioBasicUnitMessage : public CTXUnitMessage
{
public:
	CTXAudioBasicUnitMessage(); //{} ;
	virtual ~CTXAudioBasicUnitMessage();// {};	
public:
	BOOL SetFsCsOnOff(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetFsVolUpDn(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetCurrentSourceOn(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetVolumeUp(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetVolumeDown(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetVolume(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bVolume);
	BOOL SetSourceOn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bSource);
	BOOL SetSourceFunction(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bFunction);
	BOOL SetSourcePressDown(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetSourcePressUp(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetDestinationOff(BYTE bNodeAddress,BYTE bUnitAddress);

	BOOL RequestSourceOn(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestOnOff(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestState(BYTE bNodeAddress,BYTE bUnitAddress);

/*	NOT HANDLED BY --> Returns EV_MESSAGEERROR ...
	BOOL RequestDisc(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestFreq(BYTE bNodeAddress,BYTE bUnitAddress);
*/
	BOOL RequestVolume(BYTE bNodeAddress,BYTE bUnitAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXSensUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXSensUnitMessage : public CTXUnitMessage
{
public:
	CTXSensUnitMessage();// {} ;
	virtual ~CTXSensUnitMessage();// {};	
public:
	BOOL SetSetpoint(BYTE bNodeAddress,BYTE bUnitAddress,WORD wValue,BYTE bPreset);
	//BOOL SetSetpoint(BYTE bNodeAddress,BYTE bUnitAddress,WORD wValue);
	BOOL SetSetpointInc(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetSetPointDec(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetPreset(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bPreset);
	BOOL SetOnOff(BYTE bNodeAddress,BYTE bUnitAddress,BOOL fState);
	BOOL SetFsOnOff(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL SetFsSetPointIncDec(BYTE bNodeAddress,BYTE bUnitAddress);

	BOOL RequestSetPoint(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestSetPoint(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bPreset);
	BOOL RequestPreset(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestOnOff(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestSampleInterval(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestSensType(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestMeasuredVal(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestState(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestHystOffset(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestHystValues(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestHystValues(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bPreset);
	BOOL RequestMeasuredValueOffset(BYTE bNodeAddress,BYTE bUnitAddress);	/* TM20120404 */
	BOOL RequestNTCVersionInfo(BYTE bNodeAddress,BYTE bUnitAddress);		/* TM20120404 */
};


//////////////////////////////////////////////////////////////////////////////
//	
//	CIRTXUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CIRTXUnitMessage : public CTXUnitMessage
{
public:
	CIRTXUnitMessage();// {} ;
	virtual ~CIRTXUnitMessage();// {};	
public:
	BOOL SetSendIRCode(BYTE bNodeAddress,BYTE bUnitAddress,WORD wIRCode);
	
	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestState(BYTE bNodeAddress,BYTE bUnitAddress);
};


//////////////////////////////////////////////////////////////////////////////
//	
//	CIRRC5UnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CIRRC5UnitMessage : public CTXUnitMessage
{
public:
	CIRRC5UnitMessage();// {} ;
	virtual ~CIRRC5UnitMessage();// {};	
public:
	
	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestState(BYTE bNodeAddress,BYTE bUnitAddress);
};


// TM 20041125 : NEW HC4 MESSAGES ....
//////////////////////////////////////////////////////////////////////////////
//	
//	CAVMatrixUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXAVMatrixUnitMessage : public CTXUnitMessage
{
public:
	CTXAVMatrixUnitMessage() {} ;
	virtual ~CTXAVMatrixUnitMessage() {};	
public:
	
	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	//BOOL Requeststate(BYTE bNodeAddress,BYTE bUnitAddress);	//cant!!
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXVideomuxUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXVideomuxUnitMessage : public CTXUnitMessage
{
public:
	CTXVideomuxUnitMessage() {} ;
	virtual ~CTXVideomuxUnitMessage() {};	
public:

	//BOOL SetConnect(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bOutput,BYTE bInput);
	//BOOL SetDisconnect(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bOutput,BYTE bInput);
	BOOL SetMethodConnect(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bOutput,BYTE bInput);
	BOOL SetMethodDisconnect(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bOutput,BYTE bInput);
	BOOL SetMethodDisconnectAll(BYTE bNodeAddress,BYTE bUnitAddress);
	
	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL Requeststate(BYTE bNodeAddress,BYTE bUnitAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXPS2MouseUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXPS2MouseUnitMessage : public CTXUnitMessage
{
public:
	CTXPS2MouseUnitMessage() {} ;
	virtual ~CTXPS2MouseUnitMessage() {};	
public:
	
	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL Requeststate(BYTE bNodeAddress,BYTE bUnitAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXAlarmUnitMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXAlarmUnitMessage : public CTXUnitMessage
{
public:
	CTXAlarmUnitMessage() {} ;
	virtual ~CTXAlarmUnitMessage() {};	
public:
	
	BOOL SetRemoteCode(BYTE bNodeAddress,BYTE bUnitAddress,BYTE *pbData);
	BOOL SetConfig(BYTE bNodeAddress,BYTE bUnitAddress,BOOL fGalaxy500);

	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL Requeststate(BYTE bNodeAddress,BYTE bUnitAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXAudioHC4UnitMessage
//
//////////////////////////////////////////////////////////////////////////////
/* control ID's */
#define IRKEYPAD_BITMAP_1				0
#define IRKEYPAD_BITMAP_2				1
#define IRKEYPAD_BITMAP_3				2
#define IRKEYPAD_BITMAP_4				3
#define IRKEYPAD_BITMAP_5				4
#define IRKEYPAD_BITMAP_6				5
#define IRKEYPAD_BITMAP_7				6
#define IRKEYPAD_BITMAP_8				7
#define IRKEYPAD_BITMAP_9				8
#define IRKEYPAD_BITMAP_0				9
#define IRKEYPAD_BITMAP_KPUP			10
#define IRKEYPAD_BITMAP_KPDOWN			11
#define IRKEYPAD_BITMAP_KPLEFT			12
#define IRKEYPAD_BITMAP_KPRIGHT			13
#define IRKEYPAD_BITMAP_KPENTER			14
#define IRKEYPAD_BITMAP_TIENEEN			15
#define IRKEYPAD_BITMAP_FREE1			16
#define IRKEYPAD_BITMAP_FREE2			17
#define IRKEYPAD_BITMAP_FREE3			18
#define IRKEYPAD_BITMAP_FREE4			19
#define IRKEYPAD_BITMAP_FREE5			20
#define IRKEYPAD_BITMAP_UP1				21
#define IRKEYPAD_BITMAP_DOWN1			22 
#define IRKEYPAD_BITMAP_UP2				23
#define IRKEYPAD_BITMAP_DOWN2			24

#define AUDIO_HC4_FUNCTION_PLAY			0
#define AUDIO_HC4_FUNCTION_PAUZE		1
#define AUDIO_HC4_FUNCTION_FW			2
#define AUDIO_HC4_FUNCTION_RW			3
#define AUDIO_HC4_FUNCTION_FFW			4
#define AUDIO_HC4_FUNCTION_FRW			5
#define AUDIO_HC4_FUNCTION_STOP			6

class CTXAudioHC4UnitMessage : public CTXUnitMessage
{
public:
	CTXAudioHC4UnitMessage() {} ;
	virtual ~CTXAudioHC4UnitMessage() {};	
public:
	BOOL SetFsCsOnOff(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL SetFsVolUpDn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL SetCurrentSourceOn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL SetSourceInc(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL SetSourceDec(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL SetDestOff(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL SetVolumeUp(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL SetVolumeDn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL SetVolumeStop(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL SetMethodMute(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);

	BOOL SetSourceOn(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest,BYTE bSource);	
	BOOL SetDestVolume(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest,BYTE bVolume);
	BOOL SetMethodFunction(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest,BYTE bFunction);
	BOOL SetMethodExtraIR(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest,BYTE bIRCode);

	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestState(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestSource(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL RequestStateOnOff(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);
	BOOL RequestVolume(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bDest);	
	BOOL RequestVolStatus(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestDestStatus(BYTE bNodeAddress,BYTE bUnitAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXEnergyMeasureUnitMessage
//
//////////////////////////////////////////////////////////////////////////////

// IMPLEMENT_UNIT_ENERGY
class CTXEnergyMeasureUnitMessage : public CTXUnitMessage
{
public:
	CTXEnergyMeasureUnitMessage();
	virtual ~CTXEnergyMeasureUnitMessage();

	BOOL RequestConfig(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestState(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestSampleInterval(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL RequestSupportedFlags(BYTE bNodeAddress,BYTE bUnitAddress);

protected:
	BOOL helper_SendRequest(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMethod);
};
//////////////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_TXUNITMESSAGE_H__BA2D3A50_62F7_11D8_B865_0050BAC412B1__INCLUDED_)
