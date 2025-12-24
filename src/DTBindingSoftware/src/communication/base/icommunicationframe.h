#ifndef INCLUDED_ICOMMUNICATIONFRAME_H
#define INCLUDED_ICOMMUNICATIONFRAME_H
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ICommunicationFrame
{
	virtual ~ICommunicationFrame() = 0;

	virtual BYTE GetModemCmd() const = 0; 
	virtual BYTE GetMsgCode() const = 0;
	virtual BYTE GetNodeAddress() const = 0;  
	virtual BYTE GetUnitAddress() const = 0; 
	virtual BYTE GetDataBYTE(BYTE i) const = 0;  
	virtual BYTE GetCmdDataBYTE(BYTE i) const = 0;   
	virtual BYTE GetMessageLength(void) const = 0;

	// Sedert V16.20
	virtual BYTE getFrameIndex(BYTE i) const = 0;
};
/*****************************************************************/
#endif 
/*****************************************************************/