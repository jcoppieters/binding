// SerialCommObject.cpp: implementation of the CSerialCommObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "SerialCommObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// #include "Mainfrm.h"
#include "mscomm.h"
#include "ModemFrame.h"
#include "MyWindowTimers.h"				


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialCommObject::CSerialCommObject(ISerialCommObject_Host& parent) : 
m_parent(parent),
pTxQueue(new CTXMessageQueue()),
pRxQueue(new CRXMessageQueue()),
pRXModemFrame(new CModemFrame(&cReceiveBuffer[0])),
pTXModemFrame(new CModemFrame()),
bTxCurrentState(Idle),
bCommACKRetries(0),
fConnected(FALSE)
{
	ASSERT(pTxQueue);
	ASSERT(pRxQueue);
	ASSERT(pRXModemFrame);
	ASSERT(pTXModemFrame);
}


CSerialCommObject::~CSerialCommObject()
{
	delete pTxQueue;
	delete pRxQueue;
	delete pTXModemFrame;
	delete pRXModemFrame;
}

//////////////////////////////////////////////////////////////////////
void CSerialCommObject::TxStateMachine(TxEvents event)
{
	switch (bTxCurrentState)
	{
		case Idle:
		{			
			switch(event)
			{
				case EV_RESET_MODEM:
				{
					if (fConnected) 
					{
						bCommACKRetries = 0;
						pTxQueue->Flush();
						pRxQueue->Flush();		
						
						CString szMessage;
						szMessage.Format("%02X",PININSTALL_MODEM_RESET);
						SendMessage(szMessage);
					}

				} break;

				case EV_DISCONNECT:
				{
					bCommACKRetries = 0;
					pTxQueue->Flush();
					pRxQueue->Flush();

				} break;
			
				case EV_ACK:
				case EV_NAK:
				case EV_RETRANSMIT:
				case EV_ACK_TIMEOUT:
				{

				} break; 

				case EV_REQUEST_FOR_SEND:
				{					
					if (pTxQueue->Count() != 0) 
					{  
						ENUM_APDU_HEADER Header;
						//CString szMessage = pTxQueue->GetMessage(&Header);											
						SendMessage(pTxQueue->GetMessage(&Header));	// --- Send Message ---								
						if (Header == APDU_HEADER_EXPLICIT) {																			
							StartAckTimer();
							bCommACKRetries = 0;											
							TRAN(WaitForAck);					
						}
						else {
							StartRetryTimer();
							TRAN(WaitForNextSend);
						}
					}
				} break;

				default:
				{
					AfxMessageBox("Error : Undefined event received in TxStateMachine !");
				} break;
			}
		} break;

		case WaitForAck:
		{
			switch(event)
			{
				case EV_RESET_MODEM:
				{				
					StopAckTimer();

					bCommACKRetries = 0;
					pTxQueue->Flush();
					pRxQueue->Flush();		

					CString szMessage;
					szMessage.Format("%02X",PININSTALL_MODEM_RESET);
					SendMessage(szMessage);

					TRAN(Idle);
					
				} break;

				case EV_DISCONNECT:			
				{
					StopAckTimer();
					bCommACKRetries = 0;
					
					pTxQueue->Flush();					
					pRxQueue->Flush();
					
					TRAN(Idle);

				} break;

				case EV_ACK:
				{					
					StopAckTimer();
					
					if (pTxQueue->Count() != 0)
					{ 
						ENUM_APDU_HEADER Header;										

						SendMessage(pTxQueue->GetMessage(&Header));	//---- Send Message ----				
						
						if (Header == APDU_HEADER_EXPLICIT)
						{
							StartAckTimer();
							bCommACKRetries = 0;					
						}
						else
						{
							StartRetryTimer();
							TRAN(WaitForNextSend);
						}
					}
					else 
					{
						TRAN(Idle);
					}
				} break; 

				case EV_NAK:
				{				
					
					StopAckTimer();
					StartRetryTimer();				
					
					TRAN(WaitForRetransmit);

				} break;

				case EV_ACK_TIMEOUT:
				{				
					StopAckTimer();
					
					if (bCommACKRetries++ <= MAX_RETRY) 
					{						
						Fire_OnNoResponse();					
						StartAckTimer();                		
						SendMessage("");		// --- Send Last Message ---
					}
					else 
					{
						bCommACKRetries = 0;											
						Fire_OnMaxRetries();
						
						if (pTxQueue->Count() != 0) 
						{ 
							ENUM_APDU_HEADER Header;										

							SendMessage(pTxQueue->GetMessage(&Header));	// ---- Send Message ----			

							if (Header == APDU_HEADER_EXPLICIT)
							{
								StartAckTimer();
								bCommACKRetries = 0;																	
							}
							else
							{
								StartRetryTimer();
								TRAN(WaitForNextSend);
							}
						}						
						else
						{
							TRAN(Idle);
						}					
					}										
				} break; // EV_ACK_TIMEOUT

				//case EV_COMMAND_REQUEST_FOR_SEND:
				case EV_RETRANSMIT:
				case EV_REQUEST_FOR_SEND:
				{
				} break; 

				default:
				{					
					AfxMessageBox("ERROR, unknown event received in SendCommStateMachine...",MB_ICONSTOP);
				} break;
			}
		} break;
		
		case WaitForRetransmit:
		{
			switch(event)
			{				
				case EV_RESET_MODEM:
				{				
					StopRetryTimer();

					bCommACKRetries = 0;
					pTxQueue->Flush();
					pRxQueue->Flush();		

					CString szMessage;
					szMessage.Format("%02X",PININSTALL_MODEM_RESET);
					SendMessage(szMessage);

					TRAN(Idle);
					
				} break;

				case EV_DISCONNECT:								
				{
					StopRetryTimer();
					bCommACKRetries = 0;

					pTxQueue->Flush();
					pRxQueue->Flush();					

					TRAN(Idle);

				} break;

				case EV_RETRANSMIT:
				{					
					StopRetryTimer();
					
					if (bCommACKRetries++ <= MAX_RETRY) 
					{					
						StartAckTimer();                		
						SendMessage("");	// --- Send Last Message ---						
					
						TRAN(WaitForAck);
					}
					else
					{
						bCommACKRetries = 0;
						Fire_OnMaxRetries();
						
						if (pTxQueue->Count() != 0) 
						{ 
							ENUM_APDU_HEADER Header;
								
							SendMessage(pTxQueue->GetMessage(&Header));	// --- Send Message ---										
							
							if (Header == APDU_HEADER_EXPLICIT)
							{
								StartAckTimer();
								bCommACKRetries = 0;																	
							
								TRAN(WaitForAck);
							}
							else
							{
								StartRetryTimer();
								TRAN(WaitForNextSend);
							}
						}						
						else 
						{
							TRAN(Idle);
						}
					}
				} break; 

				default:
				{
					
				} break;
			} // switch(iEvent) 
		} break;

		case WaitForNextSend:
		{
			switch(event)
			{								
				case EV_RESET_MODEM:
				{				
					StopRetryTimer();

					bCommACKRetries = 0;
					pTxQueue->Flush();
					pRxQueue->Flush();		

					CString szMessage;
					szMessage.Format("%02X",PININSTALL_MODEM_RESET);
					SendMessage(szMessage);

					TRAN(Idle);
					
				} break;

				case EV_DISCONNECT:								
				{
					StopRetryTimer();
					bCommACKRetries = 0;

					pTxQueue->Flush();
					pRxQueue->Flush();					

					TRAN(Idle);

				} break;

				case EV_RETRANSMIT:
				{					
					StopRetryTimer();
					
					if (pTxQueue->Count() != 0)
					{ 
						ENUM_APDU_HEADER Header;
							
						SendMessage(pTxQueue->GetMessage(&Header));		//---- Send Message ----				
						
						if (Header == APDU_HEADER_EXPLICIT)
						{
							StartAckTimer();
							bCommACKRetries = 0;					
							TRAN(WaitForAck);
						}
						else
						{
							StartRetryTimer();						
						}
					}
					else 
					{
						TRAN(Idle);
					}				

				} break;
			}

		} break;

		default: 
		{			
			AfxMessageBox("Error : Undefined state in TxStateMachine !",
						  MB_ICONSTOP);

		}break;
	}
}


////////////////////////////////////////////////////////////
void CSerialCommObject::SendMessage(const CString& CStringMsg)
{
   BYTE bCnt=0,bCheckSum=0,i,bKbCnt;   
   char bKar;     
   static CString CStringAssembledMsg;
   
   if (CStringMsg.GetLength() != 1)
   {
      // Only assemble new string if Msg is not empty, otherwise send same string...
      if (CStringMsg.GetLength() != 0)
      {      
		 CStringAssembledMsg.Format(_T("   %s   "),(LPCSTR)(CStringMsg)); 
         bKbCnt = CStringMsg.GetLength();
   
         CStringAssembledMsg.SetAt(bCnt++,SOH);
   
         // Length 
         if (((bKbCnt/16) >= 0) && ((bKbCnt/16) <= 9)) bKar = (bKbCnt/16) + 48;
         else if (((bKbCnt/16) >= 10) && ((bKbCnt/16) <= 15)) bKar = (bKbCnt/16) + 55;
         CStringAssembledMsg.SetAt(bCnt++,bKar); 
   
         bCheckSum ^= bKar;  
   
         if (((bKbCnt%16) >= 0) && ((bKbCnt%16) <= 9)) bKar = (bKbCnt%16) + 48;
         else if (((bKbCnt%16) >= 10) && ((bKbCnt%16) <= 15)) bKar = (bKbCnt%16) + 55;
         CStringAssembledMsg.SetAt(bCnt++,bKar); 
   
         bCheckSum ^= bKar;  
   
         for(i=0;i<bKbCnt;i++) {
            bCheckSum ^= CStringMsg.GetAt(i);
            bCnt++;
         } // for all bytes in pbData

         // CheckSum
         if (((bCheckSum/16) >= 0) && ((bCheckSum/16) <= 9)) bKar = (bCheckSum/16) + 48;
         else if (((bCheckSum/16) >= 10) && ((bCheckSum/16) <= 15)) bKar = (bCheckSum/16) + 55;
         CStringAssembledMsg.SetAt(bCnt++,bKar); 

         if (((bCheckSum%16) >= 0) && ((bCheckSum%16) <= 9)) bKar = (bCheckSum%16) + 48;
         else if (((bCheckSum%16) >= 10) && ((bCheckSum%16) <= 15)) bKar = (bCheckSum%16) + 55;
         CStringAssembledMsg.SetAt(bCnt++,bKar); 
   
         CStringAssembledMsg.SetAt(bCnt++,EOT); 

      } // New message 
      else
      {
		// Old Msg
      }
	
	  pTXModemFrame->Assign(&CStringAssembledMsg.GetBuffer(0)[1]);
	  Fire_OnMessageSend(pTXModemFrame);

#if !defined(USE_MSCOMMCONTROL)
	  if (m_comport.Connected())
	  {
			BOOL fResult = m_comport.Send((BYTE*)((LPCTSTR)(CStringAssembledMsg)),		// CHANGES_MVS_2008_CSTRING
										  CStringAssembledMsg.GetLength() );
			ASSERT( fResult );
	  }
#endif

#if defined(USE_MSCOMMCONTROL)
	  
	  try 
	  {	  
		CMSComm* pCommCtrl = GetCommCtrl();
		VARIANT var;    
		var.vt = VT_BSTR;
		var.bstrVal = CStringAssembledMsg.AllocSysString();
		
		if (pCommCtrl->GetPortOpen()) 		
			pCommCtrl->SetOutput(var);		
	
		SysFreeString(var.bstrVal);		
	  }
	  
	  catch (COleException *e) 
	  {
		e->ReportError();
		e->Delete();
	  }

#endif

   }
} // void CCommFrame::SendMessage(CString CStringMsg)


/////////////////////////////////////////////////////////////////////////////////
void CSerialCommObject::SendAck(void)
{

#if !defined(USE_MSCOMMCONTROL)
	
	const char ch[2] = { ACK , '\0' };

	if (m_comport.Connected())
	{
		BOOL fResult = m_comport.Send( (BYTE*) &ch[0] , 1 );

		ASSERT( fResult );
	}

#endif


#if defined(USE_MSCOMMCONTROL)

	CString ch(ACK);	
	VARIANT var;

	try 
	{
		CMSComm* pCommCtrl = GetCommCtrl();						
		var.vt = VT_BSTR;
		var.bstrVal = ch.AllocSysString();
		pCommCtrl->SetOutput(var);		
		SysFreeString(var.bstrVal);
	}
	
	catch (COleException *e) 
	{
		e->ReportError();
		e->Delete();
	 }

#endif
}

void CSerialCommObject::SendNack(void) 
{

#if !defined(USE_MSCOMMCONTROL)
	
	const char ch[2] = { NAK , '\0' };

	if (m_comport.Connected())
	{
		BOOL fResult = m_comport.Send( (BYTE*) &ch[0] , 1 );

		ASSERT( fResult );
	}

#endif

#if defined(USE_MSCOMMCONTROL)

	CString ch(NAK);	// TM 20080128 : was ACK ???
	VARIANT var;
	try 
	{	
		CMSComm* pCommCtrl = GetCommCtrl();						
		var.vt = VT_BSTR;
		var.bstrVal = ch.AllocSysString();
		pCommCtrl->SetOutput(var);
		SysFreeString(var.bstrVal);
	}
	catch (COleException *e) 
	{
		e->ReportError();
		e->Delete();
	}
#endif
}


void CSerialCommObject::Parse(CString &str)
{
	const int Len = str.GetLength();

	for (int i=0; i< Len; i++)		// TM 20080128 : Can be more than 1 !!
	{
		const BYTE bKar = str.GetAt(i); 

		switch(bKar)
		{
			case ACK:
			{					
				TxStateMachine(EV_ACK);														
				Fire_OnReceivedAck();
			} break;
			case NAK:
			{					
				TxStateMachine(EV_NAK);
				Fire_OnReceivedNack();								
			} break;
			case SOH:
			{
				bReceiveBufferIndex = 0;
			} break;
			case EOT:
			{											
				cReceiveBuffer[bReceiveBufferIndex]='\0';

				if (pRXModemFrame->CheckXOR())
				{							
					// TM 20081222: Het mag nooit voorkomen dat de RxQueue vol raakt.
					// De threadsync. zorgt ervoor dat de UART rxQueue gewist wordt wanneer de main-applicatie
					// bezig is.
					if (pRxQueue->InsertMessage(&cReceiveBuffer[0])) 
					{			
						const BYTE modemCommand = pRXModemFrame->GetModemCmd();

						// Dit bericht zal verwerkt worden, dus mag er een ACK verstuurd worden.
						SendAck();			

						// TM, Sedert V16.20: Ook logger commandos toelaten.
						if ((MODEM_RXMESSAGE == modemCommand) || (MODEM_RXMESSAGE_CANLOG == modemCommand)) {
							// Assync bericht dat er een correct bericht werd ontvangen, 
							// de UI-applicatie mag deze verwerken wanneer idle.						
							Fire_OnReceivedMessage();		
						}
						else {
							Fire_OnReceivedModemCommand();
						}									
					}
					else 
					{	
						// TM 20080129: Indien er geen plaats is in de Rxqueue om
						// berichten op te slaan mag er geen NAK verstuurd worden !!
						// SendNack();													

						// TM 20081222: Met de bijkomende threadsync mag dit nooit voorkomen !
						Fire_OnCommBuffersFull();
					}
				}
				else
				{											
					SendNack();
					Fire_OnReceivedBadChecksum();	
				}			
					
			}break;

			default:
			{
				if (bReceiveBufferIndex >= RX_BUFFER_LENGTH-1) {
					bReceiveBufferIndex = 0;
				}
				cReceiveBuffer[bReceiveBufferIndex++]=bKar;
			} break;
		} // switch(bKar)   				
	}
}


// #define DEBUG_TRY		0

void CSerialCommObject::ParseRXMessage(void)
{	
	#if !defined(USE_MSCOMMCONTROL)

// #if(DEBUG_TRY == 0)
	BYTE* pbBuffer = 0;
	const int nrOfBytes = m_comport.Get(&pbBuffer);
	if (nrOfBytes != 0) {	
		pbBuffer[nrOfBytes] = '\0';		
		CString str((LPCTSTR)pbBuffer, nrOfBytes);
		Parse( str );
	}
// #endif 
/*
#if(DEBUG_TRY == 1)
	static int prevLoopCounter = 1;
	int loopCounter = 0;
	int nrOfBytes = 0;
	do {
		BYTE* pbBuffer = 0;
		nrOfBytes = m_comport.Get(&pbBuffer);
		if (nrOfBytes > 0) {	
			pbBuffer[nrOfBytes] = '\0';		
			CString str((LPCTSTR)pbBuffer, nrOfBytes);
			Parse( str );
			loopCounter++;
		}
	} while(nrOfBytes <= 0);

	if (loopCounter>prevLoopCounter) {
		prevLoopCounter = loopCounter;
	}
#endif 
*/
	#endif

	#if defined(USE_MSCOMMCONTROL)

	// TM 20050112 : Changed this : store in variant --> Check wrapper CMsComm
	// GetInput() returns reference to variant, We Must copy the original BSTR ...
	// VARIANT *pVar = NULL;

	try 
	{
		CMSComm* const pCommCtrl = GetCommCtrl();

		switch(pCommCtrl->GetCommEvent())
		{
			case 1: // vbMSCommEvSend:
			{	

			} break;

  			case 2: // vbMSCommEvReceive:
			{				
/*
				VARIANT *pVar = &(pCommCtrl->GetInput());
				CString str;

				if (pVar != NULL) 
				{
					str = pVar->bstrVal;
					SysFreeString(pVar->bstrVal);
				}

				Parse( str );
			
				//SysFreeString(vt.bstrVal);
*/

				VARIANT *pVar = &(pCommCtrl->GetInput());				

				if (pVar != NULL) 
				{
					CString str(pVar->bstrVal);

					SysFreeString(pVar->bstrVal);

					Parse( str );
				}
						
				//SysFreeString(vt.bstrVal);

			} break; /* CASE 2 */

			case 3: // vbMSCommEvCTS:
			{
			}break;

			default:
			{
			} break;
			
		} /* SWITCH GetCommEvent */
	}
	
	catch (COleException *e)
	{
		e->ReportError();
		e->Delete();
	}

	#endif
}




//////////////////////////////////////////////////////////////
				/* INTERFACE FUNCTIONS */
//////////////////////////////////////////////////////////////

void CSerialCommObject::Init(HWND hParent) 
{	
#if !defined(USE_MSCOMMCONTROL)
	REQUIRE( hParent != 0 );
	ASSERT( m_comport.Connected() == FALSE);
	m_comport.get_Settings().set_Baudrate( 38400 );
	m_comport.get_Settings().set_Comport( 0 );
	m_comport.get_Events().subscribe( hParent, WM_COMMUNICATION_EVENT );	
#endif

#if defined(USE_MSCOMMCONTROL)
	try 
	{	
		CMSComm* const pCommCtrl = GetCommCtrl();
		ASSERT(pCommCtrl);

		// Set RThreshold property to 1. The default RThreshold value (0)
		// causes the OnComm event to NOT fire when a character is detected
		// in the serial port. 

		
		if (pCommCtrl != NULL)
		{
			pCommCtrl->SetRThreshold(1);
			pCommCtrl->SetInputLen(1);
			pCommCtrl->SetNullDiscard(false);	

			CString str(_T("38400n81"));
			pCommCtrl->SetSettings(str);	
			pCommCtrl->SetCommPort(3);	/* TEMP */
		
		}
	}
	
	catch (COleException *e) 
	{
		e->ReportError();
		e->Delete();
	}
#endif
}


BOOL CSerialCommObject::IsConnectionOpen(void) 
{
	#if !defined(USE_MSCOMMCONTROL)

	fConnected = m_comport.Connected( );

	return fConnected;

	#endif

	#if defined(USE_MSCOMMCONTROL)

	try 
	{	
		CMSComm* pCommCtrl = GetCommCtrl();
		fConnected = pCommCtrl->GetPortOpen();

		return (fConnected);
	}
	catch (COleException *e) 
	{
		e->ReportError();
		e->Delete();
	}

	#endif

	return FALSE;
}


void CSerialCommObject::ResetModem()
{
	TxStateMachine(EV_RESET_MODEM);
}


BOOL CSerialCommObject::Send_PininstallConnect()
{
	#if !defined(USE_MSCOMMCONTROL)

	if (m_comport.Connected() == FALSE)
		return FALSE;
	
	DispatchMsg("06");

	#endif

	#if defined(USE_MSCOMMCONTROL)

	CMSComm* const pCommCtrl = GetCommCtrl();

	if (pCommCtrl == 0)
		return FALSE;

	if (pCommCtrl->GetPortOpen() == FALSE)
		return FALSE;

	CString CStringCommand(_T("06"));				
	DispatchMsg(CStringCommand);
	
	#endif
	
	return TRUE;
}

BOOL CSerialCommObject::Send_CanLoggerEnable(BOOL flag) {

	if (m_comport.Connected() == FALSE)
		return FALSE;

	CString msg;
	msg.Format("%02X%02X",PININSTALL_CANLOG_ENABLE,(flag==TRUE ? 1 : 0));
	DispatchMsg(msg);
	return TRUE;
}

BOOL CSerialCommObject::Send_PininstallDisconnect()
{
	#if !defined(USE_MSCOMMCONTROL)
		
		if (m_comport.Connected() == FALSE)
			return FALSE;
	
		DispatchMsg("07");

	#endif

	#if defined(USE_MSCOMMCONTROL)

	CMSComm* const pCommCtrl = GetCommCtrl();

	if (pCommCtrl == 0)
		return FALSE;

	if (pCommCtrl->GetPortOpen() == FALSE)
		return FALSE;

	CString CStringCommand(_T("07"));
	DispatchMsg(CStringCommand);	

	#endif

	return TRUE;
}


void CSerialCommObject::SetConnect()
{
	#if !defined(USE_MSCOMMCONTROL)
	
	if (!m_comport.Connected())
	{
		m_comport.Open( );

		Send_PininstallConnect();	
	}

	fConnected = m_comport.Connected( );

	#endif

	#if defined(USE_MSCOMMCONTROL)

	CMSComm* pCommCtrl = GetCommCtrl();

	const int nComPort = pCommCtrl->GetCommPort();

	if (pCommCtrl->GetPortOpen() == FALSE) 
	{	
		pCommCtrl->SetPortOpen(TRUE);		

		ASSERT(pCommCtrl->GetPortOpen());

		Send_PininstallConnect();	
	}

	fConnected = pCommCtrl->GetPortOpen();

	#endif
}

void CSerialCommObject::SetDisconnect()
{
	#if !defined(USE_MSCOMMCONTROL)

	if (m_comport.Connected())
	{
		Send_PininstallDisconnect();	
		TxStateMachine(EV_DISCONNECT);
		m_comport.Close( );
	}
	fConnected = m_comport.Connected( );

	#endif

	#if defined(USE_MSCOMMCONTROL)

	CMSComm* pCommCtrl = GetCommCtrl();
			
	if (pCommCtrl->GetPortOpen() == TRUE)
	{			
		Send_PininstallDisconnect();
	
		TxStateMachine(EV_DISCONNECT);
		pCommCtrl->SetPortOpen(FALSE);
	}

	fConnected = pCommCtrl->GetPortOpen();

	#endif
}

BOOL CSerialCommObject::OpenConnection() {
#if !defined(USE_MSCOMMCONTROL)
	SetConnect();
	fConnected = m_comport.Connected();
	return fConnected;
#else
	CMSComm* pCommCtrl = GetCommCtrl();	
	try 
	{
		const int nComPort = pCommCtrl->GetCommPort();		
		if (pCommCtrl->GetPortOpen() == FALSE) {	
			pCommCtrl->SetPortOpen(TRUE);		
			Send_PininstallConnect();
		}
		else { 
			return FALSE;
		}				
	}	

	catch(CException* e) {
		char szException[255];
		CString szError;
		e->GetErrorMessage(szException,255,NULL);
		szError.Format(_T("Windows exception error occured :\n%s\n"
						  "Cannot go online to duotecno modem ...")
						  ,szException);
		AfxMessageBox(szError);
		e->Delete();
	}
	fConnected = pCommCtrl->GetPortOpen();	
	return fConnected;
#endif 
}


BOOL CSerialCommObject::CloseConnection() {
#if !defined(USE_MSCOMMCONTROL)
	SetDisconnect();
	fConnected = m_comport.Connected();
	return fConnected;
#else
	Send_PininstallDisconnect();			
	TxStateMachine(EV_DISCONNECT);
	pCommCtrl->SetPortOpen(FALSE);
	fConnected = m_comport.Connected();
	return fConnected;
#endif 
}

#if(0)
// TM 20041115
BOOL CSerialCommObject::SetConnected(BOOL Connected)
{
	#if !defined(USE_MSCOMMCONTROL)

	if (Connected)
	{
		SetConnect();		
	}
	else
	{
		SetDisconnect();
	}

	fConnected = m_comport.Connected();

	#endif


	#if defined(USE_MSCOMMCONTROL)

	CMSComm* pCommCtrl = GetCommCtrl();
	
	try 
	{
		const int nComPort = pCommCtrl->GetCommPort();

		if (Connected == TRUE) 
		{
			if (pCommCtrl->GetPortOpen() == FALSE) 
			{	
				pCommCtrl->SetPortOpen(TRUE);		

				Send_PininstallConnect();
			}
			else
			{ 
				return FALSE;
			}
		}
		if (Connected == FALSE)  
		{		
			Send_PininstallDisconnect();
			
			TxStateMachine(EV_DISCONNECT);
			pCommCtrl->SetPortOpen(FALSE);
		}
	}	

	catch(CException* e)
	{
		char szException[255];
		CString szError;

		e->GetErrorMessage(szException,255,NULL);

		szError.Format(_T("Windows exception error occured :\n%s\n"
						  "Cannot go online to duotecno modem ...")
						  ,szException);

		AfxMessageBox(szError);

		e->Delete();
	}

	fConnected = pCommCtrl->GetPortOpen();

	#endif

	return (fConnected);
}
#endif


void CSerialCommObject::SetComPort(short Port) 
{
	#if !defined(USE_MSCOMMCONTROL)		
	if (Port<=0) {
		Port = 1;
	}
	m_comport.get_Settings().set_Comport(Port - 1);

	#endif

	#if defined(USE_MSCOMMCONTROL)
	try 
	{	
		CMSComm* pCommCtrl = GetCommCtrl();
		pCommCtrl->SetCommPort(Port);
	}
	
	catch (COleException *e) 
	{
		e->ReportError();
		e->Delete();
	}
	#endif
}

short CSerialCommObject::GetComPort(void) 
{	
	int port = 1;

#if !defined(USE_MSCOMMCONTROL)
	
	port = m_comport.get_Settings().get_Comport() + 1;

#endif

#if defined(USE_MSCOMMCONTROL)
	try 
	{	
		CMSComm* pCommCtrl = GetCommCtrl();
		port = pCommCtrl->GetCommPort();
	}
	
	catch (COleException *e) 
	{
		e->ReportError();
		e->Delete();
	}
#endif

	ASSERT( port > 0 );
	return port;
}

// -------------------------------------------------------------

BOOL CSerialCommObject::GetRXMsgFromQueue(void) 
{
	// TM 20060414 --> This happens !!!!!
	// ASSERT(pRxQueue->Count() <= 1);

	const char *m_chData = pRxQueue->GetMessage();
	if (m_chData != NULL)
	{
		pRXMsgQueueFrame.Assign(const_cast<char*>(m_chData));
		return TRUE;	
	}
	return FALSE;
}


BYTE CSerialCommObject::DispatchMsg(const CString& CStringMsg) 
{
	const BYTE bResult = pTxQueue->InsertMessage(CStringMsg);
	TxStateMachine(EV_REQUEST_FOR_SEND);
	return (bResult);
} 

/**
 * Transfer single command ...
 * Command must return 'ACK' 
 */
BYTE CSerialCommObject::MessageTransfer(BYTE cmd)
{
	CString szCommandString;
	szCommandString.Format("%02X",cmd);

	const BYTE bResult = pTxQueue->InsertMessage(szCommandString,APDU_HEADER_IMPLICIT);

	TxStateMachine(EV_REQUEST_FOR_SEND);
	return (bResult);	
}

BYTE CSerialCommObject::MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,
										BYTE bLength,const CString& szData)
{
	CString szCommandString;
	
	if (bLength == 0) 
	{
		szCommandString.Format(_T("00%06x%02x%02x00"),
							   bNodeAddress,
							   bUnitAddress,
							   bMsgCode);
	}
	else 
	{
		szCommandString.Format(_T("00%06x%02x%02x%02x%s"),bNodeAddress,
													      bUnitAddress,
													      bMsgCode,
														  bLength,
														  szData);		
	}
	
	const BYTE bResult = pTxQueue->InsertMessage(szCommandString);
	TxStateMachine(EV_REQUEST_FOR_SEND);
	return (bResult);	
}


///////////////////////////////////////////////////////////////////////////////

inline void CSerialCommObject::Fire_OnMessageSend(CModemFrame* pTxModemFrame) {	
	m_parent.onSerialMessageSend(pTxModemFrame);
}
inline void CSerialCommObject::Fire_OnReceivedAck(void) {
	m_parent.onSerialReceivedAck();
}
inline void CSerialCommObject::Fire_OnReceivedNack(void) {	
	m_parent.onSerialReceivedNack();
}
inline void CSerialCommObject::Fire_OnReceivedMessage(void) {
	m_parent.onSerialReceivedMessage();
}
inline void CSerialCommObject::Fire_OnReceivedModemCommand(void) {	
	m_parent.onSerialReceivedModemCommand();
}
inline void CSerialCommObject::Fire_OnReceivedBadChecksum(void) {
	m_parent.onSerialReceivedBadChecksum();
}
inline void CSerialCommObject::Fire_OnCommBuffersFull(void) {
	m_parent.onSerialCommBuffersFull();
}
inline void CSerialCommObject::Fire_OnMaxRetries(void) {
	m_parent.onSerialMaxRetries();
}
inline void CSerialCommObject::Fire_OnNoResponse(void) {
	m_parent.onSerialNoResponse();
}

inline void CSerialCommObject::StartAckTimer(void)  {
	m_parent.startTimer( TIMERID_ACK_TIMEOUT,ACK_TIMEOUT);
}
inline void CSerialCommObject::StopAckTimer(void)  {
	m_parent.stopTimer( TIMERID_ACK_TIMEOUT );
}

inline void CSerialCommObject::StartRetryTimer(void)  {
	m_parent.startTimer( TIMERID_RETRANSMIT_TIMEOUT,RETRANSMIT_TIMEOUT);	
}
inline void CSerialCommObject::StopRetryTimer(void)  {	
	m_parent.stopTimer( TIMERID_RETRANSMIT_TIMEOUT );
}