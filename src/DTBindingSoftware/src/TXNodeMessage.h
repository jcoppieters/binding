// TXNodeMessage.h: interface for the CTXNodeMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TXNODEMESSAGE_H__553D8761_9CE1_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_TXNODEMESSAGE_H__553D8761_9CE1_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeMessage
//
//////////////////////////////////////////////////////////////////////////////

class CTXNodeMessage  
{
public:
	static CTXNodeMessage* getInstance();

protected:
	CTXNodeMessage() {};

public:
	virtual ~CTXNodeMessage() {	};

protected:
	inline BOOL IsConnected();

public:
	BOOL SetNodeName(BYTE bNodeAddress,BYTE bIndex,BYTE bLength,BYTE *pszConvertedData);
	BOOL ResetNode(BYTE bNodeAddress);
	BOOL FactoryResetNode(BYTE bNodeAddress);
	BOOL FactoryInitNode(BYTE bNodeAddress);
	BOOL SetNodeConfig(BYTE bNodeAddress,BYTE bNodeConfig,BYTE bDllAccessControl);

	BOOL RequestNodeNicErrors(BYTE bNodeAddress,BYTE bNicErrorBlock);

	// TM 20041227
	BOOL SendLcdInfo(BYTE bNodeAddress,CString s);
	BOOL ReadMemoryData(BYTE bNodeAddress,unsigned long ulAddress,unsigned short usBlockLength);
	BOOL WriteMemoryData(BYTE bNodeAddress,unsigned long ulAddress,unsigned short usBlockLength,BYTE *pbData);

	BOOL SendLcdLock(BYTE bNodeAddress,BOOL fLock);

	BOOL SendNodeGetTextString(BYTE bNodeAddress, CString s);		/* v0564 */
};



//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeBindingFileMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXNodeBindingFileMessage : public CTXNodeMessage
{
public:
	static CTXNodeBindingFileMessage* getInstance();

public:
	BOOL OpenBindingFile(BYTE bNodeAddress);
	BOOL CloseBindingFile(BYTE bNodeAddress);
	BOOL EraseBindingFile(BYTE bNodeAddress);
	BOOL RequestBindingInfoFile(BYTE bNodeAddress);
};



//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeBindingEntryMessage
//
//////////////////////////////////////////////////////////////////////////////
class CTXNodeBindingEntryMessage : public CTXNodeMessage
{
public:
	static CTXNodeBindingEntryMessage* getInstance();

public:
	BOOL ReadData(BYTE bNodeAddress);

	// Sedert V17.00: Uitbreiding van de bindings.
	// Mogelijkheid om in een module meer dan 256 bindings te hebben.
	// De datalengte van het packet is afhankelijk van de waarde van bindingEntryIndex.	
	BOOL ReadHeader(BYTE bNodeAddress,int bindingEntryIndex);

	// Sedert V17.00: Uitbreiding van de bindings.
	// Mogelijkheid om in een module meer dan 256 bindings te hebben.
	// Erase entry commando is hier niet geintegreerd maar moet hetzelfde principe volgen als deze van ReadHeader
	// De datalengte van het packet is afhankelijk van de waarde van bindingEntryIndex.	
	/* BOOL EraseEntry(BYTE bNodeAddress,int bindingEntryIndex); */

	BOOL WriteData(BYTE bNodeAddress,CString szData);
	BOOL WriteDone(BYTE bNodeAddress);
	BOOL WriteHeader(BYTE bNodeAddress,unsigned short usBindingNR,BYTE bBindingFlags);
	BOOL WriteHeaderBindingIndex(BYTE bNodeAddress,CString szData);
};



//////////////////////////////////////////////////////////////////////////////
//	
//	CTXBrowsePnpMessages
//
//////////////////////////////////////////////////////////////////////////////
class CTXBrowsePnpMessages : public CTXNodeMessage
{
public:
	static CTXBrowsePnpMessages* getInstance();

public:
	BOOL StartPhysicalBrowse(BYTE bNodeType);
	BOOL StartBrowse(BYTE bNodeType);
	BOOL AssignPhysicalAddress(unsigned long ulPhysicalAddress,BYTE bNodeAddress);

	BOOL RequestNodeName(BYTE bNodeAddress,BYTE bIndex);
	BOOL RequestNodeResources(BYTE bNodeAddress,BYTE bResourceBlock);
	BOOL RequestNodeConfig(BYTE bNodeAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXLCDScreenmapMessages
//
//////////////////////////////////////////////////////////////////////////////

class CTXLCDScreenmapMessages : public CTXNodeMessage
{
public:
	static CTXLCDScreenmapMessages* getInstance();

public:
	BOOL SendInit(BYTE bNodeAddress,BYTE bScreenmapID,unsigned short usNrEntries, BYTE startpage);
	BOOL SendEntry(BYTE bNodeAddress,BYTE bScreenmapID,BYTE bPageNr,BYTE bButtonNr,BYTE bNodeLogAddress,BYTE bUnitAddress);
	BOOL SendDone(BYTE bNodeAddress,BYTE bScreenmapID);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXExternalConfigMessages
//
//////////////////////////////////////////////////////////////////////////////

class CTXExternalConfigMessages : public CTXNodeMessage
{
public:
	static CTXExternalConfigMessages* getInstance();

public:
	BOOL SendControl(BYTE bNodeAddress,BYTE bMethod);
	BOOL SendConfig(BYTE bNodeAddress,BYTE bMethod,BYTE bIndex,BYTE *pbData);
	BOOL SendConfig(BYTE bNodeAddress,BYTE bMethod,BYTE *pbData);
};


//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeGatewayMessages
//
//////////////////////////////////////////////////////////////////////////////
class CTXNodeGatewayMessages : public CTXNodeMessage
{
public:
	static CTXNodeGatewayMessages* getInstance();

public:
	BOOL RequestState(BYTE bNodeAddress);
	BOOL EraseDatabase(BYTE bNodeAddress);
};

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeFTPFileControl
//	@since v0561
//////////////////////////////////////////////////////////////////////////////
class CTXNodeFTPFileControl : public CTXNodeMessage
{
public:
	static CTXNodeFTPFileControl* getInstance( );

	BOOL SendWriteOpen(
		BYTE bNodeAddress, 
		const char* szFileName);

	BOOL SendReadOpen(
		BYTE bNodeAddress, 
		const char* szFileName);

	BOOL SendClose(BYTE bNodeAddress);

	BOOL SendAbort(BYTE bNodeAddress);

	BOOL SendAppendData(
		BYTE bNodeAddress, 
		BYTE* pbData, 
		int length);

	BOOL SendAppendData(				// New !!
		BYTE bNodeAddress, 
		unsigned long ulPosition,
		BYTE* pbData, 
		int length);

	BOOL SendReadData(
		BYTE bNodeAddress, 
		unsigned long ulPosition, 
		unsigned short usLength);

protected:
	BOOL formatFileName(
		const char* szFileName, 
		char* pbBuffer,
		int maxLength,
		int* pLength);

	BOOL SendMessage(
		BYTE bNodeAddress,
		BYTE bMethod,
		const char* pbBuffer,
		int length);

	BOOL SendMessage(
		BYTE bNodeAddress,
		BYTE bMethod);	
};

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeDatabaseManagment
//	@since v0562
//
//////////////////////////////////////////////////////////////////////////////
class CTXNodeDatabaseManagment : public CTXNodeMessage
{
public:
	static CTXNodeDatabaseManagment* getInstance( );

	BOOL SendReload(
		BYTE bNodeAddress,
		int databaseID);

	BOOL SendErase(
		BYTE bNodeAddress,
		int databaseID);

	BOOL SendSave(
		BYTE bNodeAddress,
		int databaseID);

	BOOL SendBackup(
		BYTE bNodeAddress,
		int databaseID);

protected:
	BOOL SendOperation(
		BYTE bNodeAddress,
		int operation,
		int databaseID);
};

//////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TXNODEMESSAGE_H__553D8761_9CE1_11D8_B865_0050BAC412B1__INCLUDED_)
