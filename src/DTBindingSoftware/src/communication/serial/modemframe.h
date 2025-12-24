// ModemFrame.h: interface for the CModemFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODEMFRAME_H__FAC5FF21_35F1_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MODEMFRAME_H__FAC5FF21_35F1_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
#include "../base/icommunicationframe.h"
#include "modemframe_def.h"
//////////////////////////////////////////////////////////////////////
// V24 frame receive
#define SOH 0x01
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15

// ActiveX communication ID for data exchange...
//#define ID_COMMCTRL 2001  // Ctrl ID for comm control

// Offsets in modem frame...
#define OFFSET_MESSAGE_LENGTH		00
#define OFFSET_MODEM_COMMAND		02
#define OFFSET_PM_ADDRESS			04
#define OFFSET_PM_UNIT				10
#define OFFSET_PM_MSGCODE			12
#define OFFSET_PM_LENGTH			14
#define OFFSET_PM_DATA				16

#define OFFSET_PM_COMMAND_DATA		04

#define RX_BUFFER_LENGTH			512
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

// [SOH][Length][Command][Data][Checksum][EOT]
class CModemFrame : public ICommunicationFrame
{
public:
	CModemFrame(BYTE *pbKar);
	CModemFrame();
	virtual ~CModemFrame();

protected:
	BYTE *pbReceivedFrame;

public:
   void Assign(char *pData);

public:
	// Sedert V16.20
	// Specifiek voor de modem (tracer)
	virtual BYTE getFrameIndex(BYTE i) const { 
		return ConvertToBYTE(pbReceivedFrame,(i*2));
	}

   virtual BYTE GetModemCmd() const {
      return(ConvertToBYTE(pbReceivedFrame,OFFSET_MODEM_COMMAND));
   }

   virtual BYTE GetMsgCode() const { 
	   return(ConvertToBYTE(pbReceivedFrame,OFFSET_PM_MSGCODE));
   }

   virtual BYTE GetNodeAddress() const {
      return(ConvertToBYTE(pbReceivedFrame,OFFSET_PM_ADDRESS+4));
   }

   virtual BYTE GetUnitAddress() const {
      return(ConvertToBYTE(pbReceivedFrame,OFFSET_PM_UNIT));
   }

#if(0)
   virtual BYTE GetMemberNR() const {
      return(ConvertToBYTE(pbReceivedFrame,OFFSET_PM_DATA));
   }
#endif 

   virtual BYTE GetDataBYTE(BYTE i) const {
      return(ConvertToBYTE(pbReceivedFrame,OFFSET_PM_DATA+i*2));
   }

   virtual BYTE GetCmdDataBYTE(BYTE i) const {
      return(ConvertToBYTE(pbReceivedFrame,OFFSET_PM_COMMAND_DATA+i*2));
   }

   virtual BYTE GetMessageLength(void) const {
      return(ConvertToBYTE(pbReceivedFrame,OFFSET_PM_LENGTH));
   }

public:
   BOOL CheckXOR(void);

protected:
   BYTE ConvertToBYTE(unsigned char *pbKar, BYTE bPos) const;
   WORD ConvertToWORD(unsigned char *pbKar, BYTE bPos) const ;
   unsigned long ConvertTo24BIT(unsigned char *pbKar, BYTE bPos) const;
};

#endif // !defined(AFX_MODEMFRAME_H__FAC5FF21_35F1_11D8_B865_0050BAC412B1__INCLUDED_)
