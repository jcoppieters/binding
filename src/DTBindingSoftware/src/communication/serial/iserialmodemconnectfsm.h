#ifndef INCLUDED_ISERIALMODEMCONNECTFSM_HOST_H
#define INCLUDED_ISERIALMODEMCONNECTFSM_HOST_H
/*****************************************************************/
#ifndef INCLUDED_COMMUNICATION_ICONNECTFSM_H
	#include "../base/icommunicationconnectfsm.h"
#endif 
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ISerialModemConnectFsm : public ICCommunicationConnectFsm
{
	virtual ~ISerialModemConnectFsm() = 0;

	virtual void EvSignalAckReceived() = 0;
	virtual void EvSignalMessageReceived() = 0;
	virtual void EvSignalModemStatus() = 0;
	virtual void EvSignalModemAddress() = 0;
	virtual void EvSignalSoftwareVersion( ) = 0;
};
/*****************************************************************/
#endif 
/*****************************************************************/