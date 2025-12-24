#ifndef INCLUDED_ICOMMUNICATIONOBJECT_H
#define INCLUDED_ICOMMUNICATIONOBJECT_H
/*****************************************************************/
#ifndef INCLUDED_ICOMMUNICATIONFRAME_H
	#include "icommunicationframe.h"
#endif 
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ICommunicationObject
{
	virtual ~ICommunicationObject() = 0;

	// functies...
	virtual void Init(HWND hParent = 0) = 0;
	virtual BOOL IsConnectionOpen() = 0;
	virtual BOOL OpenConnection() = 0;
	virtual BOOL CloseConnection() = 0;
	virtual BYTE MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,BYTE bLength=0,const CString& szData ='\0') = 0;
	virtual const ICommunicationFrame* GetRxModemFrame() const = 0;
	virtual BOOL GetRXMsgFromQueue(void) = 0;	
};

/****************************************************************/

// socketIdentifier:
// Bij de USB-Modem is deze -1
struct ICommunicationObject_Host 
{
	virtual ~ICommunicationObject_Host() = 0;

	virtual void displayMsgInfo(int socketIdentifier, const CString& msg) = 0;
	virtual void displayMsgError(int socketIdentifier, const CString& msg) = 0;
};
/*****************************************************************/
#endif 
/*****************************************************************/