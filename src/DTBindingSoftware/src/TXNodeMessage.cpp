// TXNodeMessage.cpp: implementation of the CTXNodeMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "TXNodeMessage.h"
#include "ComApplication.h"
#include "TMGlobals.h"

#define BROADCAST_UNIT		0xFF

using namespace TMCommunication;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////
enum 
{
	SEGMENTED_BLOCK_LENGTH = 110
};

/////////////////////////////////////////////////////////////////////
//#ifdef SKIP
namespace
{
	/**
	 * @since	v0x0560: Toegevoegd om het versturen van gesegmenteerde
	 *			berichten te optimaliseren.
	 */
	inline BYTE H2A(BYTE hex)
	{
		if (hex <= 9) 
			return (hex + 48);

		return (hex + 55);
	};

	BYTE A2H(BYTE bKar)
	{
 		if ((bKar >= '0') && (bKar <= '9')) return(bKar-48);
		else if ((bKar >= 'A') && (bKar <= 'F')) return(bKar-55);
		else if ((bKar >= 'a') && (bKar <= 'f')) return(bKar-87);

		/* should not be here..., however, compiler generates warning...       */
		return(0);
	}

	inline BYTE MSN(BYTE val)
	{
		//return ((val << 4) & 0xF0);
		return ((val & 0xF0) >> 4);
	}
	inline BYTE LSN(BYTE val)
	{
		return (val & 0x0F);
	}
};
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTXNodeMessage* CTXNodeMessage::getInstance()
{
	static CTXNodeMessage instance;
	return (&instance);
}

inline BOOL CTXNodeMessage::IsConnected()
{
	return (TMIsConnected());
}

BOOL CTXNodeMessage::SetNodeName(BYTE bNodeAddress,BYTE bIndex,BYTE bLength,BYTE *pszConvertedData)
{	
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData;
	szData.Format("%02x%s",bIndex,pszConvertedData);			
	return(TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											FC_NODESETNAME,bLength+1,szData));

}

BOOL CTXNodeMessage::ResetNode(BYTE bNodeAddress)
{	
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData;
	szData.Format("%02x",0);			
	return(TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											FC_NODERESET,1,szData));
}

BOOL CTXNodeMessage::FactoryResetNode(BYTE bNodeAddress)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData;
	szData.Format("%02x",2);			
	return(TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											FC_NODERESET,1,szData));
}

BOOL CTXNodeMessage::FactoryInitNode(BYTE bNodeAddress)
{	
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData;
	szData.Format("%02x",3);			
	return(TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											FC_NODERESET,1,szData));
}

BOOL CTXNodeMessage::RequestNodeNicErrors(BYTE bNodeAddress,BYTE bNicErrorBlock)
{	
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData;
	szData.Format("%02x",bNicErrorBlock);			
	return(TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											FC_NODEREQUESTDLLSTATUS,1,szData));
}

BOOL CTXNodeMessage::SetNodeConfig(BYTE bNodeAddress,BYTE bNodeConfig,BYTE bDllAccessControl)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData;
	szData.Format("%02x%02x",bNodeConfig,bDllAccessControl);			
	return(TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											FC_NODECONFIG,2,szData));
}

/*********************************************************//**
 * @changes	TM,v0x0560: Solved Bug\n
 *	 Lengte van de string + 1 om ook het '\0' karakter mee te sturen.
 *
 * @changes TM,v0x0560: Solved Bug\n
 *	 Buffer overflow bij het invoegen van een '\0' karakter.
 *
 *  PROBLEM_BUG_MODEM_v6502
 *************************************************************/
BOOL CTXNodeMessage::SendLcdInfo(BYTE bNodeAddress,CString s)
{	
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	BYTE bLength = s.GetLength() + 1;				// Changed

	const char* szBuffer = (const char*) s;			// changed	
	const int MAX_SEGMENTED_STRING = (SEGMENTED_BLOCK_LENGTH * 2) + 1;
	char szData[ MAX_SEGMENTED_STRING ];
	int j = 0;

	for (int i=0;i<bLength;i++) 
	{
		ASSERT( j < MAX_SEGMENTED_STRING);						// CHANGES_MVS_2008
		szData[j++] = H2A(MSN(szBuffer[i]));

		ASSERT( j < MAX_SEGMENTED_STRING);						// CHANGES_MVS_2008
		szData[j++] = H2A(LSN(szBuffer[i]));	
	}

	if (j < MAX_SEGMENTED_STRING)					// changed
	{
		szData[j] = '\0';
	}
	else											// changed
	{
		ASSERT( j< MAX_SEGMENTED_STRING);
		
		// Dit mag nooit voorkomen !
		// Het frame zal niet verstuurd worden omdat de lengte niet juist is !! 
		szData[ MAX_SEGMENTED_STRING - 1 ] = '\0';
	}
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_LCDINFODIALOG,bLength,szData));
}

BOOL CTXNodeMessage::SendNodeGetTextString(
	BYTE bNodeAddress, 
	CString s)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	BYTE bLength = s.GetLength() + 1;				// Changed

	const char* szBuffer = (const char*) s;			// changed
	
	const int MAX_SEGMENTED_STRING = (SEGMENTED_BLOCK_LENGTH * 2) + 1;
	char szData[ MAX_SEGMENTED_STRING ];
	int j = 0;
	for (int i=0;i<bLength;i++) 
	{
		ASSERT( j < MAX_SEGMENTED_STRING);						// CHANGES_MVS_2008
		szData[j++] = H2A(MSN(szBuffer[i]));

		ASSERT( j < MAX_SEGMENTED_STRING);						// CHANGES_MVS_2008
		szData[j++] = H2A(LSN(szBuffer[i]));	
	}

	if (j < MAX_SEGMENTED_STRING)					// changed
	{
		szData[j] = '\0';
	}
	else											// changed
	{
		ASSERT( j< MAX_SEGMENTED_STRING);
		
		// Dit mag nooit voorkomen !
		// Het frame zal niet verstuurd worden omdat de lengte niet juist is !! 
		szData[ MAX_SEGMENTED_STRING - 1 ] = '\0';
	}

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											 FC_NODEGETTEXTSTRING,bLength,szData));
}


BOOL CTXNodeMessage::ReadMemoryData(BYTE bNodeAddress,unsigned long ulAddress,unsigned short usBlockLength)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if ( usBlockLength > SEGMENTED_BLOCK_LENGTH) return FALSE;

	CString szData;
	szData.Format(_T("%08x%04x"),ulAddress,usBlockLength);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											 FC_GETMEMORYDATA,6,szData));
}


/**
 * @changed	TM,v0560: optimalisatie.
 * @changed	TM,v0x0560: Solved Bug\n
 *	 Buffer overflow bij het invoegen van een '\0' karakter.
 *
 * PROBLEM_BUG_MODEM_v6502 
 */
BOOL CTXNodeMessage::WriteMemoryData(BYTE bNodeAddress,unsigned long ulAddress,unsigned short usBlockLength,BYTE *pbData)
{	
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	if ( usBlockLength > SEGMENTED_BLOCK_LENGTH) return FALSE;
		
	const int MAX_SEGMENTED_STRING = (SEGMENTED_BLOCK_LENGTH * 2) + 1;
	char szSegmentedData[ MAX_SEGMENTED_STRING ];
	int j = 0;

	for (unsigned short usIndex=0;
		 usIndex<usBlockLength;
		 usIndex++) 
	{
		ASSERT( j < MAX_SEGMENTED_STRING);						// CHANGES_MVS_2008
		szSegmentedData[j++] = H2A(MSN(pbData[usIndex]));

		ASSERT( j < MAX_SEGMENTED_STRING);						// CHANGES_MVS_2008
		szSegmentedData[j++] = H2A(LSN(pbData[usIndex]));
	}
	
	if (j < MAX_SEGMENTED_STRING)			// changed
	{
		szSegmentedData[j] = '\0';
	}
	else
	{
		szSegmentedData[MAX_SEGMENTED_STRING-1] = '\0';	
	}

	CString szData;	
	szData.Format(_T("%08x%s"),ulAddress,szSegmentedData);
	
	BYTE bLength = usBlockLength + 4;

	TRACE1("WriteMemoryData Length = %d\n",bLength);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											 FC_WRITEMEMORYDATA,bLength,szData));
}

BOOL CTXNodeMessage::SendLcdLock(BYTE bNodeAddress,BOOL fLock)
{
	CString szData;
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	if (fLock == TRUE) 
	{		
		szData.Format("%02x",LCD_LOCKED);			
		return(TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
												FC_LOCKLCD,1,szData));
	}
	else 
	{		
		return(TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
												FC_UNLOCKLCD));//,0,szData));
	}
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeBindingFileMessage
//
//////////////////////////////////////////////////////////////////////////////
#define TRACER_TX_NODEBINDINGFILE(x)			TRACE x				// DEBUGGEN: TRACER INSCHAKELEN.
//#define TRACER_TX_NODEBINDINGFILE(x)								// TRACER UITSCHAKELEN.

CTXNodeBindingFileMessage* CTXNodeBindingFileMessage::getInstance() {
	static CTXNodeBindingFileMessage instance;
	return (&instance);
}

BOOL CTXNodeBindingFileMessage::OpenBindingFile(BYTE bNodeAddress) {
	CString szData;
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	szData.Format("%02x",FILE_OPEN);
	TRACER_TX_NODEBINDINGFILE(("TX: FC_NODEBINDINGSFILECONTROL - Open bindingfile(0x%02x)\r\n",bNodeAddress));
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGSFILECONTROL,01,szData));
}

BOOL CTXNodeBindingFileMessage::CloseBindingFile(BYTE bNodeAddress) {
	CString szData;
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	szData.Format("%02x",FILE_CLOSE);
	TRACER_TX_NODEBINDINGFILE(("TX: FC_NODEBINDINGSFILECONTROL - Close bindingfile(0x%02x)\r\n",bNodeAddress));
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGSFILECONTROL,01,szData));
}

BOOL CTXNodeBindingFileMessage::EraseBindingFile(BYTE bNodeAddress) {
	CString szData;
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	szData.Format("%02x",FILE_ERASE);
	TRACER_TX_NODEBINDINGFILE(("TX: FC_NODEBINDINGSFILECONTROL - Erase bindingfile(0x%02x)\r\n",bNodeAddress));
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGSFILECONTROL,01,szData));
}

BOOL CTXNodeBindingFileMessage::RequestBindingInfoFile(BYTE bNodeAddress) {
	CString szData;
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	szData.Format("%02x",FILE_INFO);
	TRACER_TX_NODEBINDINGFILE(("TX: FC_NODEBINDINGSFILECONTROL - Request info bindingfile(0x%02x)\r\n",bNodeAddress));
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGSFILECONTROL,01,szData));
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeBindingEntryMessage
//
//////////////////////////////////////////////////////////////////////////////
#define TRACER_TX_NODEBINDINGENTRY(x)			TRACE x				// DEBUGGEN: TRACER INSCHAKELEN.
//#define TRACER_TX_NODEBINDINGENTRY(x)								// TRACER UITSCHAKELEN.
CTXNodeBindingEntryMessage* CTXNodeBindingEntryMessage::getInstance()
{
	static CTXNodeBindingEntryMessage instance;
	return (&instance);
}

BOOL CTXNodeBindingEntryMessage::ReadData(BYTE bNodeAddress) {
	CString szData;
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	szData.Format("%02x",READ_DATA);
	TRACER_TX_NODEBINDINGENTRY(("TX: FC_NODEBINDINGENTRY - Read data (0x%02x)\r\n",bNodeAddress));
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGENTRY,01,szData));
}

// Sedert V17.00: Uitbreiding van de bindings.
// Mogelijkheid om in een module meer dan 256 bindings te hebben.
// De datalengte van het packet is afhankelijk van de waarde van bindingEntryIndex.	
BOOL CTXNodeBindingEntryMessage::ReadHeader(BYTE bNodeAddress,int bindingEntryIndex) {
	REQUIRE(bindingEntryIndex >=0 && bindingEntryIndex<=0xFFFF);
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	CString szData;
	const int length = (bindingEntryIndex > 0xFF ? 3 : 2);
	if (2 == length) {
		szData.Format("%02x%02x",READ_HEADER,bindingEntryIndex);
	}
	else {
		const int msb = (bindingEntryIndex & 0xFF00) >> 8;
		const int lsb = (bindingEntryIndex & 0xFF);
		szData.Format("%02x%02x%02x",READ_HEADER,msb,lsb);
	}

	TRACER_TX_NODEBINDINGENTRY(("TX: FC_NODEBINDINGENTRY - Read header (0x%02x) - entry=%d\r\n",bNodeAddress,bindingEntryIndex));
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGENTRY,length,szData));
}
	
BOOL CTXNodeBindingEntryMessage::WriteData(BYTE bNodeAddress,CString szStringData)
{   
	char *szConvertedData=NULL;
	BYTE i;
	CString szData;
	BOOL fResult;

   	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	szConvertedData = new char[szStringData.GetLength()*2+1];
  
	for (i=0;i<szStringData.GetLength();i++) {
		if (((szStringData.GetAt(i)/16) >= 0) && ((szStringData.GetAt(i)/16) <= 9)) szConvertedData[i*2] = (szStringData.GetAt(i)/16) + 48;
		else if (((szStringData.GetAt(i)/16) >= 10) && ((szStringData.GetAt(i)/16) <= 15)) szConvertedData[i*2] = (szStringData.GetAt(i)/16) + 55;

		if (((szStringData.GetAt(i)%16) >= 0) && ((szStringData.GetAt(i)%16) <= 9)) szConvertedData[i*2+1] = (szStringData.GetAt(i)%16) + 48;
		else if (((szStringData.GetAt(i)%16) >= 10) && ((szStringData.GetAt(i)%16) <= 15)) szConvertedData[i*2+1] = (szStringData.GetAt(i)%16) + 55;

	}
	szConvertedData[szStringData.GetLength()*2]='\0';

	szData.Format("%02x%s",WRITE_DATA,szConvertedData);
	fResult = TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGENTRY,szStringData.GetLength()+1,szData);
	TRACER_TX_NODEBINDINGENTRY(("TX: FC_NODEBINDINGENTRY - Write data (0x%02x) - Data='%s'\r\n",bNodeAddress,szConvertedData));
	if (szConvertedData) {
		delete [] szConvertedData;
	}	
	return fResult;
}

BOOL CTXNodeBindingEntryMessage::WriteDone(BYTE bNodeAddress) {
	CString szData;
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	szData.Format("%02x",WRITE_DONE);
	TRACER_TX_NODEBINDINGENTRY(("TX: FC_NODEBINDINGENTRY - Write done (0x%02x)\r\n",bNodeAddress));
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGENTRY,01,szData));
}

BOOL CTXNodeBindingEntryMessage::WriteHeader(BYTE bNodeAddress,unsigned short usBindingNR,BYTE bBindingFlags) {
	CString szData;
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	szData.Format("%02x%04x%02x",WRITE_HEADER,usBindingNR,bBindingFlags);
	TRACER_TX_NODEBINDINGENTRY(("TX: FC_NODEBINDINGENTRY - Write header (0x%02x) - BindingNumber=%d - flags=0x%02x\r\n",bNodeAddress,usBindingNR,bBindingFlags));
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGENTRY,04,szData));
}

BOOL CTXNodeBindingEntryMessage::WriteHeaderBindingIndex(BYTE bNodeAddress,CString szStringData) {
	CString szData;
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;

	if (szStringData) {
		szData.Format("%02x",WRITE_HEADER);

		szData+= szStringData.GetAt(10);	//usBindingNr
		szData+= szStringData.GetAt(11);
		szData+= szStringData.GetAt(12);
		szData+= szStringData.GetAt(13);

		szData+= szStringData.GetAt(7);		//bindingFlags
		szData+= szStringData.GetAt(8);

		TRACER_TX_NODEBINDINGENTRY(("TX: FC_NODEBINDINGENTRY - Write header (0x%02x) - '%s'\r\n",bNodeAddress,szData));
		return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_NODEBINDINGENTRY,04,szData));
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXBrowsePnpMessages
//
//////////////////////////////////////////////////////////////////////////////
CTXBrowsePnpMessages* CTXBrowsePnpMessages::getInstance()
{
	static CTXBrowsePnpMessages instance;
	return (&instance);
}

BOOL CTXBrowsePnpMessages::StartPhysicalBrowse(BYTE bNodeType)
{
	CString szData;
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	szData.Format(_T("%02x"),bNodeType);

	return (TMGetCommunicationApp()->MessageTransfer(BROADCAST_NODE,BROADCAST_UNIT,
											FC_BROWSEFORPHYSICALNODES,01,szData));
}

BOOL CTXBrowsePnpMessages::StartBrowse(BYTE bNodeType)
{
	CString szData;
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	szData.Format(_T("%02x"),bNodeType);

	return (TMGetCommunicationApp()->MessageTransfer(BROADCAST_NODE,BROADCAST_UNIT,
											FC_BROWSEFORNODES,01,szData));
}

BOOL CTXBrowsePnpMessages::AssignPhysicalAddress(unsigned long ulPhysicalAddress,BYTE bNodeAddress)
{
	CString szData;
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	szData.Format(_T("%06x%02x"),ulPhysicalAddress,bNodeAddress);
	return (TMGetCommunicationApp()->MessageTransfer(BROADCAST_NODE,BROADCAST_UNIT,
											FC_PNPASSIGNLOGICALADDRESS,04,szData));
}

BOOL CTXBrowsePnpMessages::RequestNodeName(BYTE bNodeAddress,BYTE bIndex)
{
	CString szData;
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	szData.Format(_T("%02x"),bIndex);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											 FC_NODEREQUESTNAME,01,szData));
}

BOOL CTXBrowsePnpMessages::RequestNodeResources(BYTE bNodeAddress,BYTE bResourceBlock)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData;
	szData.Format(_T("%02x"),bResourceBlock);
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
											 FC_NODEGETRESOURCES,01,szData));
}

BOOL CTXBrowsePnpMessages::RequestNodeConfig(BYTE bNodeAddress)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,
													 BROADCAST_UNIT,
													 FC_NODEREQUESTCONFIG));
}



//////////////////////////////////////////////////////////////////////////////
//	
//	CTXLCDScreenmapMessages
//
//////////////////////////////////////////////////////////////////////////////
CTXLCDScreenmapMessages* CTXLCDScreenmapMessages::getInstance()
{
	static CTXLCDScreenmapMessages instance;
	return (&instance);
}

BOOL CTXLCDScreenmapMessages::SendInit(
	BYTE bNodeAddress,
	BYTE bScreenmapID,
	unsigned short usNrEntries,
	BYTE startPage)
{
	ASSERT(TMGetCommunicationApp());
	CString szData;

	if (!IsConnected()) return FALSE;
	
	szData.Format(_T("%02x%02x%02x%02x%02x"),SCREENMAP_INIT_SCREENMAP,
										 bScreenmapID,
										 usNrEntries/256,
										 usNrEntries%256,
										 startPage);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
						FC_LCDBUILDSCREENMAP,05,szData));
};

BOOL CTXLCDScreenmapMessages::SendEntry(BYTE bNodeAddress,BYTE bScreenmapID,BYTE bPageNr,BYTE bButtonNr,BYTE bNodeLogAddress,BYTE bUnitAddress)
{
	ASSERT(TMGetCommunicationApp());
	CString szData;

	if (!IsConnected()) return FALSE;
	
	szData.Format(_T("%02x%02x%02x%02x%02x%02x"),SCREENMAP_ENTRY,
											     bScreenmapID,
												 bPageNr,
												 bButtonNr,
												 bNodeLogAddress,
												 bUnitAddress);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
						FC_LCDBUILDSCREENMAP,06,szData));
}

BOOL CTXLCDScreenmapMessages::SendDone(BYTE bNodeAddress,BYTE bScreenmapID)
{
	ASSERT(TMGetCommunicationApp());
	CString szData;

	if (!IsConnected()) return FALSE;

	szData.Format(_T("%02x%02x"),SCREENMAP_DONE_SMAP,bScreenmapID);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,
						FC_LCDBUILDSCREENMAP,2,szData));
}

//////////////////////////////////////////////////////////////////////////////
//	
//	CTXExternalConfigMessages
//
//////////////////////////////////////////////////////////////////////////////


CTXExternalConfigMessages* CTXExternalConfigMessages::getInstance()
{
	static CTXExternalConfigMessages instance;
	return (&instance);
}

BOOL CTXExternalConfigMessages::SendControl(BYTE bNodeAddress,BYTE bMethod)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData,s;
	CString strSegmentedData;
	BYTE bLength;
	
	for (unsigned short usIndex=0;usIndex<SEGMENTED_BLOCK_LENGTH;usIndex++) {		
		strSegmentedData+="00";
	}

	szData.Format(_T("%02x%s"),bMethod,strSegmentedData);
	bLength = SEGMENTED_BLOCK_LENGTH;

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_EXTERNALCONFIG,bLength,szData));
}

/**
 * @changed	TM,v0560: optimalisatie.
 * @changed	TM,v0x0560: Solved Bug\n
 *	 Buffer overflow bij het invoegen van een '\0' karakter.
 *
 * @changes	TM,v0573: Solved Bug\n
 *	Applicatie crasht omdat we buiten het array schrijven !!!
 */
BOOL CTXExternalConfigMessages::SendConfig(BYTE bNodeAddress,BYTE bMethod,BYTE *pbData)
{		
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	const int MAX_SEGMENTED_STRING = (SEGMENTED_BLOCK_LENGTH * 2) - 1;	// CHANGES_MVS_2008
	char strSegmentedData[ MAX_SEGMENTED_STRING] ;
	int j = 0;

	const unsigned short LENGTH = SEGMENTED_BLOCK_LENGTH - 1;			// CHANGES_MVS_2008
	for (unsigned short usIndex=0;
		 usIndex<LENGTH;
		 usIndex++) 
	{			
		ASSERT( j < MAX_SEGMENTED_STRING);								// CHANGES_MVS_2008
		strSegmentedData[j++] = H2A(MSN(pbData[usIndex]));

		ASSERT( j < MAX_SEGMENTED_STRING);								// CHANGES_MVS_2008
		strSegmentedData[j++] = H2A(LSN(pbData[usIndex]));
	}

	strSegmentedData[MAX_SEGMENTED_STRING-1] = '\0';	
	strSegmentedData[MAX_SEGMENTED_STRING-2] = '\0';

	CString szData;
	szData.Format(_T("%02x%s"),bMethod,strSegmentedData);
	const BYTE bLength = SEGMENTED_BLOCK_LENGTH;

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_EXTERNALCONFIG,bLength,szData));
}
/**
 * @changed	TM,v0560: optimalisatie.
 * @changed	TM,v0x0560: Solved Bug\n
 *	 Buffer overflow bij het invoegen van een '\0' karakter.
 *
 * @changes	TM,v0573: Solved Bug\n
 *	Applicatie crasht omdat we buiten het array schrijven !!!
 */
BOOL CTXExternalConfigMessages::SendConfig(BYTE bNodeAddress,BYTE bMethod,BYTE bIndex,BYTE *pbData)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	
	const int MAX_SEGMENTED_STRING = (SEGMENTED_BLOCK_LENGTH * 2) - 2;		// CHANGES_MVS_2008
	char strSegmentedData[ MAX_SEGMENTED_STRING] ;
	int j = 0;
	const unsigned short LENGTH = SEGMENTED_BLOCK_LENGTH - 2;				// CHANGES_MVS_2008

	for (unsigned short usIndex=0;usIndex<LENGTH;usIndex++) 
	{				
		ASSERT( j < MAX_SEGMENTED_STRING);									// CHANGES_MVS_2008
		strSegmentedData[j++] = H2A(MSN(pbData[usIndex]));

		ASSERT( j < MAX_SEGMENTED_STRING);									// CHANGES_MVS_2008
		strSegmentedData[j++] = H2A(LSN(pbData[usIndex]));
	}

	strSegmentedData[MAX_SEGMENTED_STRING-1] = '\0';	
	strSegmentedData[MAX_SEGMENTED_STRING-2] = '\0';	
	
	CString szData;
	szData.Format(_T("%02x%02x%s"),bMethod,bIndex,strSegmentedData);

	const BYTE bLength = SEGMENTED_BLOCK_LENGTH;
	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,BROADCAST_UNIT,FC_EXTERNALCONFIG,bLength,szData));
}


//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeGatewayMessages
//
//////////////////////////////////////////////////////////////////////////////
CTXNodeGatewayMessages* CTXNodeGatewayMessages::getInstance()
{
	static CTXNodeGatewayMessages instance;
	return (&instance);
}


BOOL CTXNodeGatewayMessages::RequestState(BYTE bNodeAddress)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData;
	szData.Format(_T("%02x"),NODEGATEWAY_ATTR_STATE);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,
													 BROADCAST_UNIT,
													 FC_NODEGATEWAYREQUESTSTATUS,
													 1,
													 szData));
}

BOOL CTXNodeGatewayMessages::EraseDatabase(BYTE bNodeAddress)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;

	CString szData;
	szData.Format(_T("%02x"),NODEGATEWAY_OBJECT_DATABASE_ERASE);

	return (TMGetCommunicationApp()->MessageTransfer(bNodeAddress,
													 BROADCAST_UNIT,
													 FC_NODEGATEWAYSET,
													 1,
													 szData));
}


//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeFTPFileControl
//	@since v0561
//////////////////////////////////////////////////////////////////////////////
CTXNodeFTPFileControl* CTXNodeFTPFileControl::getInstance()
{
	static CTXNodeFTPFileControl instance;
	return (&instance);
}

/** PROBLEM_BUG_MODEM_v6502 */
BOOL CTXNodeFTPFileControl::SendWriteOpen(		
	BYTE bNodeAddress, 
	const char* szFileName)
{
	BOOL result = FALSE;

	ASSERT(TMGetCommunicationApp());
	ASSERT(szFileName);

	if (IsConnected())
	{	
		if (szFileName)
		{	
			int length = 0;
			const int MAX_SEGMENTED_STRING = ((SEGMENTED_BLOCK_LENGTH-2)*2)+1;
			char strSegmentedData[MAX_SEGMENTED_STRING+1];

			if (formatFileName(szFileName,
							   &strSegmentedData[0],
							   MAX_SEGMENTED_STRING,
							   &length))
			{
				// De lengte van het bericht dat moet verstuurd worden is
				// method + gesegmenteerde data, vandaar length+1		
				const int msgLength = strlen(szFileName) + 2;
				result = SendMessage(bNodeAddress,
									 FTP_OBJECT_OPEN_WRITE,
									 strSegmentedData,
									 msgLength);		// zie commentaar
			}
		}
		ASSERT( result == TRUE );
	}
	return result;
}
		
BOOL CTXNodeFTPFileControl::SendClose(BYTE bNodeAddress)
{
	ASSERT(TMGetCommunicationApp());
	if (!IsConnected()) return FALSE;
	return SendMessage(bNodeAddress, FTP_OBJECT_CLOSE);
}

BOOL CTXNodeFTPFileControl::SendAbort(BYTE bNodeAddress)
{
	ASSERT(TMGetCommunicationApp());

	if (!IsConnected()) return FALSE;
	return SendMessage(bNodeAddress, FTP_OBJECT_ABORT);
}

/** PROBLEM_BUG_MODEM_v6502 */
BOOL CTXNodeFTPFileControl::SendReadOpen(BYTE bNodeAddress,const char* szFileName)
{
	BOOL result = FALSE;

	ASSERT(TMGetCommunicationApp());
	if (IsConnected())
	{
		if (szFileName)
		{	
			int length = 0;
			const int MAX_SEGMENTED_STRING = ((SEGMENTED_BLOCK_LENGTH-2)*2)+1;
			char strSegmentedData[MAX_SEGMENTED_STRING+1];

			if (formatFileName(szFileName,
							   &strSegmentedData[0],
							   MAX_SEGMENTED_STRING,
							   &length))
			{
				// De lengte van het bericht dat moet verstuurd worden is
				// method + gesegmenteerde data, vandaar length+1		
				const int msgLength = strlen(szFileName) + 2;
				result = SendMessage(bNodeAddress,
									 FTP_OBJECT_OPEN_READ,
									 strSegmentedData,
									 msgLength);		// zie commentaar
			}
		}

		ASSERT( result == TRUE );
	}
	return result;
}

BOOL CTXNodeFTPFileControl::SendReadData(
	BYTE bNodeAddress, 
	unsigned long ulPosition, 
	unsigned short usLength)
{
	BOOL result = FALSE;

	ASSERT(TMGetCommunicationApp());

	if (IsConnected())
	{			
		const int length = 7;			// Is dit OK ??

		CString szData;
		szData.Format(_T("%08x%04x"),ulPosition,usLength);
		return SendMessage(bNodeAddress,
					   FTP_OBJECT_READ_DATA,
					   szData,
					   length);
	}
	return result;
}

BOOL CTXNodeFTPFileControl::SendAppendData(
	BYTE bNodeAddress, 
	BYTE* pbData, 
	int length)
{
	BOOL result = FALSE;
	if (length < SEGMENTED_BLOCK_LENGTH)
	{
		const int MAX_SEGMENTED_STRING = SEGMENTED_BLOCK_LENGTH*2;		// CHANGES_MVS_2008
		char strSegmentedData[MAX_SEGMENTED_STRING];							// CHANGES_MVS_2008

		int j = 0;	
		for (unsigned short usIndex=0;
			 usIndex<length;
			 usIndex++) 
		{				
			ASSERT( j < MAX_SEGMENTED_STRING);							// CHANGES_MVS_2008
			strSegmentedData[j++] = H2A(MSN((pbData[usIndex])));

			ASSERT( j < MAX_SEGMENTED_STRING);							// CHANGES_MVS_2008
			strSegmentedData[j++] = H2A(LSN((pbData[usIndex])));
		}

		// Oppassen: Moeten de string altijd afsluiten met een '\0' karakter
		// omdat de data string anders niet kan geformatteerd worden !
		ASSERT( j < MAX_SEGMENTED_STRING);								// CHANGES_MVS_2008
		strSegmentedData[j++] = '\0';		

		ASSERT( j < MAX_SEGMENTED_STRING);								// CHANGES_MVS_2008
		strSegmentedData[j++] = '\0';

		// De lengte van het bericht dat moet verstuurd worden is
		// method + gesegmenteerde data
		// J verwijst reeds een positie verder dus is ok.
		result = SendMessage(bNodeAddress,
							 FTP_OBJECT_APPEND_DATA,
							 strSegmentedData,
							 length + 1);	// zie commentaar.
	}
	return result;
}

BOOL CTXNodeFTPFileControl::SendAppendData(
	BYTE bNodeAddress, 
	unsigned long ulPosition,
	BYTE* pbData, 
	int length)
{
	BOOL result = FALSE;
	if (length < SEGMENTED_BLOCK_LENGTH)
	{
		const int MAX_SEGMENTED_STRING = SEGMENTED_BLOCK_LENGTH*2;			// CHANGES_MVS_2008
		char strSegmentedData[MAX_SEGMENTED_STRING];						// CHANGES_MVS_2008

		int j = 0;	
		for (unsigned short usIndex=0;usIndex<length;usIndex++) 
		{				
			ASSERT( j < MAX_SEGMENTED_STRING);								// CHANGES_MVS_2008
			strSegmentedData[j++] = H2A(MSN((pbData[usIndex])));

			ASSERT( j < MAX_SEGMENTED_STRING);								// CHANGES_MVS_2008
			strSegmentedData[j++] = H2A(LSN((pbData[usIndex])));
		}

		// Oppassen: Moeten de string altijd afsluiten met een '\0' karakter
		// omdat de data string anders niet kan geformatteerd worden !
		ASSERT( j < MAX_SEGMENTED_STRING);									// CHANGES_MVS_2008
		strSegmentedData[j++] = '\0';		

		ASSERT( j < MAX_SEGMENTED_STRING);									// CHANGES_MVS_2008
		strSegmentedData[j++] = '\0';

		CString szTempString;
		szTempString.Format(_T("%08x%s"),ulPosition,strSegmentedData);

		// De lengte van het bericht dat moet verstuurd worden is
		// method + position + gesegmenteerde data
		result = SendMessage(bNodeAddress,
							 FTP_OBJECT_APPEND_DATA,						
							 szTempString,
							 length + 4 + 1);	// zie commentaar.
	}
	return result;
}

/************************** protected functions *********************************/
BOOL CTXNodeFTPFileControl::SendMessage(
	BYTE bNodeAddress,
	BYTE bMethod,
	const char* pbBuffer,
	int length)
{
	CString szData;
	szData.Format(_T("%02x%s"),bMethod, pbBuffer);
	return (TMGetCommunicationApp()->MessageTransfer(
				bNodeAddress,
				BROADCAST_UNIT,
				FC_FTPFILECONTROL,
				length,
				szData));
}

BOOL CTXNodeFTPFileControl::SendMessage(
	BYTE bNodeAddress,
	BYTE bMethod)
{
	CString szData;
	szData.Format(_T("%02x"),bMethod);

	const int length = 1;
	return (TMGetCommunicationApp()->MessageTransfer(
				bNodeAddress,
				BROADCAST_UNIT,
				FC_FTPFILECONTROL,
				length,
				szData));
}

BOOL CTXNodeFTPFileControl::formatFileName(
	const char* szFileName, 
	char* pbBuffer,
	int maxLength,
	int* pLength)
{
	ASSERT( szFileName );
	ASSERT( pbBuffer );

	BOOL retVal = FALSE;
	int lenString = strlen(szFileName);
	if (lenString != -1)
	{
		lenString += 1;

		if (lenString<maxLength)
		{									
			memset(pbBuffer,0,maxLength);

			int j = 0;

			// Het null karakter wordt ook naar ascii omgezet !!!
			for (unsigned short usIndex=0;
				 usIndex<lenString;
				 usIndex++) 
			{				
				pbBuffer[j++] = H2A(MSN((szFileName[usIndex])));
				pbBuffer[j++] = H2A(LSN((szFileName[usIndex])));
			}
				 
			if (j < maxLength)
			{
				pbBuffer[j++] = '\0';
			}

			*pLength = j;
			retVal = TRUE;
		}
	}
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//	
//	CTXNodeFTPFileControl
//	@since v0561
//////////////////////////////////////////////////////////////////////////////
CTXNodeDatabaseManagment* CTXNodeDatabaseManagment::getInstance()
{
	static CTXNodeDatabaseManagment instance;
	return (&instance);
}

BOOL CTXNodeDatabaseManagment::SendReload(
	BYTE bNodeAddress,
	int databaseID)
{
	ASSERT(TMGetCommunicationApp());

	BOOL result = FALSE;
	if (IsConnected())
	{			
		result = SendOperation(
					bNodeAddress,
					DATABASE_OBJECT_LOAD, 
					databaseID);
	}
	return result;
}
BOOL CTXNodeDatabaseManagment::SendErase(
	BYTE bNodeAddress,
	int databaseID)
{
	ASSERT(TMGetCommunicationApp());

	BOOL result = FALSE;
	if (IsConnected())
	{			
		result = SendOperation(
					bNodeAddress,
					DATABASE_OBJECT_ERASE, 
					databaseID);
	}
	return result;
}
BOOL CTXNodeDatabaseManagment::SendSave(
	BYTE bNodeAddress,
	int databaseID)
{
	ASSERT(TMGetCommunicationApp());

	BOOL result = FALSE;
	if (IsConnected())
	{			
		result = SendOperation(
					bNodeAddress,
					DATABASE_OBJECT_SAVE, 
					databaseID);
	}
	return result;
}
BOOL CTXNodeDatabaseManagment::SendBackup(
	BYTE bNodeAddress,
	int databaseID)
{
	ASSERT(TMGetCommunicationApp());

	BOOL result = FALSE;
	if (IsConnected())
	{			
		result = SendOperation(
					bNodeAddress,
					DATABASE_OBJECT_BACKUP, 
					databaseID);
	}
	return result;
}

//////////////////////// protected ///////////////////////////
BOOL CTXNodeDatabaseManagment::SendOperation(
	BYTE bNodeAddress,
	int operation,
	int databaseID)
{
	CString szData;
	szData.Format(_T("%02x%02x"),operation,databaseID);

	const int length = 2;
	return TMGetCommunicationApp()->MessageTransfer(
				bNodeAddress,
				BROADCAST_UNIT,
				FC_DATABASEMANAGEMENT,
				length,
				szData);
}
//////////////////////////////////////////////////////////////////////////////



