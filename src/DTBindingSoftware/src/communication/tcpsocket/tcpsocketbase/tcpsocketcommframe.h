#ifndef INCLUDED_TCPSOCKET_TCPSOCKETCOMMFRAME_H
#define INCLUDED_TCPSOCKET_TCPSOCKETCOMMFRAME_H
/*****************************************************************/
#include "communication/base/icommunicationframe.h"
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

// Deze klasse is een wrapper voor de applicatie.
class CTCPSocketCommFrame : public ICommunicationFrame
{
public:
	typedef enum {
		READ = 0, WRITE = 1
	} type_t;

	CTCPSocketCommFrame();
	CTCPSocketCommFrame(int msgCode,int nodeAddress,int unitAddress,int length,const BYTE* pbData,type_t type);
	virtual ~CTCPSocketCommFrame();

	virtual BYTE GetModemCmd() const;
	virtual BYTE GetMsgCode() const;
	virtual BYTE GetNodeAddress() const;
	virtual BYTE GetUnitAddress() const;
	virtual BYTE GetDataBYTE(BYTE i) const;
	virtual BYTE GetCmdDataBYTE(BYTE i) const;
	virtual BYTE GetMessageLength(void) const;
	virtual BYTE getFrameIndex(BYTE i) const { return 0xff; };	// Specifiek voor de modem.

	CTCPSocketCommFrame& operator=(const CTCPSocketCommFrame&);
	bool operator==(CTCPSocketCommFrame& r) const;

protected:
	void setData(int length,const BYTE* pbData);

private:
	BYTE m_cmd;
	BYTE m_msgCode;
	BYTE m_nodeAddress;
	BYTE m_unitAddress;
	BYTE m_length;
	BYTE* m_pbData;
};
/*****************************************************************/
#endif 
/*****************************************************************/
