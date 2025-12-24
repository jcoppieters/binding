#pragma once

#include "app/appdali/appdali_fwd.h"

class CCanNode;
class CAppDlgDaliWorker;
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
class CPropertyPageDaliContext {
public:
	CPropertyPageDaliContext( );
	~CPropertyPageDaliContext( );

	typedef enum {
		FLAG_UPDATE_DIALOG_WELCOME	= (1<<0),
		FLAG_UPDATE_DIALOG_NWK		= (1<<1),
		FLAG_UPDATE_DIALOG_DEVICES	= (1<<2),
		FLAG_UPDATE_DIALOG_LOCATE	= (1<<3),
		FLAG_UPDATE_DIALOG_GROUPS	= (1<<4)
	} Flags_t;


	void setFlag(Flags_t flag);
	void clearFlag(Flags_t flag);
	BOOL getFlag(Flags_t flag) const;

private:
	uint16_t m_flags;
};
/******************************************************************************/


// Open en sluiten van de DALI errors dialoog.
// Dit is nodig wanneer we in de welcome pagina veranderen van Dali interface node.
enum 
{ 
	WM_PROPERTYSHEET_DIALOG_DALI_ERRORS_CLOSE	= WM_USER+1,
	WM_PROPERTYSHEET_DIALOG_DALI_ERRORS_OPEN
};


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
class CPropertyPageDaliBaseClass : public CPropertyPage, public DUOTECNO::DALI::IAppDlgDaliGui_Host
{
public:
	CPropertyPageDaliBaseClass(unsigned short dialogID,CAppDlgDaliWorker& refWorker,CPropertyPageDaliContext& refContext);
	virtual ~CPropertyPageDaliBaseClass();

	CPropertyPageDaliContext& getContext( ) {
		return m_refContext;
	}
	CAppDlgDaliWorker& getWorker() {
		return m_refWorker;
	}

	///////////////////////////////////////////////////////////////
	// IAppDlgDaliGui_Host
	// Events van de worker...
	virtual void onNotifyGuiStatusConnected(BYTE bNodeAddress,bool statusConnected) { };
	virtual void onNotifyGuiMsgCommunication(BYTE bNodeAddress,Type_t typeMessage, const CString& msg) { };
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) { }
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) { }
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) { }
	virtual void onNotifyGuiFoundDevice(BYTE bNodeAddress,DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, bool newDevice) { }
	virtual void onNotifyGuiUpdateDeviceSerialNumber(BYTE bNodeAddress,DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long serialNumber) { }
	virtual void onNotifyGuiUpdateDeviceGTIN(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long GTIN) { }
	virtual void onNotifyGuiRxDaliMessage(BYTE bNodeAddress,BYTE lengthBits,uint32_t recievedFrame) { }

	// Algemene functies nodig voor de property sheet.
	virtual BOOL taskBusy( ) const {
		return FALSE;
	};

	// Weergave van de fout berichten moet assynchroon gebeuren.
	void setErrorInfo(const CString& strMessage);
	const CString& getErrorInfo() const;
	void clearErrorInfo( );
	

private:
	const int m_dialogID;
	CAppDlgDaliWorker& m_refWorker;
	CPropertyPageDaliContext& m_refContext;
	CString m_errorString;			// Weergave van de fout berichten moet assynchroon gebeuren.
};
/******************************************************************************/





