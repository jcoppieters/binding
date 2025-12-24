#include "stdafx.h"
#include "appdalimsg_utils.h"
/***********************************************************************************/

/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/


/*****************************************************************************/
/************************* Parsen van de event messages **********************/
/*****************************************************************************/
STRUCT_DALI_EVENT_MSG::STRUCT_DALI_EVENT_MSG(uint32_t receivedFrame) :
		m_receivedFrame(receivedFrame) { }

STRUCT_DALI_EVENT_MSG::STRUCT_DALI_EVENT_MSG(int opcodeEvent) : m_receivedFrame(0) {
	REQUIRE(opcodeEvent>=0);
	m_receivedFrame |= opcodeEvent;							// bit 0-9
}

bool STRUCT_DALI_EVENT_MSG::isEvent(uint32_t receivedFrame) {
	static const uint32_t MASK_EVENT_FLAG = (1<<16);		// bit 16
	return (0 == (receivedFrame & MASK_EVENT_FLAG));
}

bool STRUCT_DALI_EVENT_MSG::isEvent() {
	return isEvent(m_receivedFrame);
}

uint8_t STRUCT_DALI_EVENT_MSG::getData( ) {
	return getData(m_receivedFrame);
}

void STRUCT_DALI_EVENT_MSG::toByteArray(BYTE* pbData) {
	pbData[0] = (this->m_receivedFrame & 0xFF0000) >> 16;
	pbData[1] = (this->m_receivedFrame & 0x00FF00) >> 8;
	pbData[2] = (this->m_receivedFrame & 0x0000FF);
}

uint8_t STRUCT_DALI_EVENT_MSG::getData(uint32_t receivedFrame) {
	static const int MASK = 0x1F;										// TODO: IS deze ok ???
	return (receivedFrame & MASK);
}

DaliMsgEventScheme_t STRUCT_DALI_EVENT_MSG::getScheme(uint32_t receivedFrame) {
	DaliMsgEventScheme_t retVal = DALI_MSG_EVENT_SCHEME_INSTANCE;

	if ((receivedFrame & (1<<15)) && (receivedFrame & (1<<22)) && (receivedFrame & (1<<23))) {

		if ((receivedFrame & (1<<17)) && (receivedFrame & (1<<18)) && (receivedFrame & (1<<19)) &&
			(receivedFrame & (1<<20)) && (receivedFrame & (1<<21)) && (receivedFrame & (1<<13)) &&  (receivedFrame & (1<<14))) {
			retVal = DALI_MSG_EVENT_DEVICE_POWER_CYCLE;
		}
		else {
			retVal = DALI_MSG_EVENT_RESERVED;
		}
	}
	else if (receivedFrame & (1<<15)) {
		if (receivedFrame & (1<<23)) {
			if (!(receivedFrame & (1<<22))) {
				retVal = DALI_MSG_EVENT_SCHEME_INSTANCE;
			}
		}
		else {
			retVal = DALI_MSG_EVENT_SCHEME_DEVICE_AND_INSTANCE;
		}
	}
	else {
		if (receivedFrame & (1<<23)) {
			if (receivedFrame & (1<<22)) {
				retVal = DALI_MSG_EVENT_SCHEME_INSTANCE_GROUP;
			}
			else {
				retVal = DALI_MSG_EVENT_SCHEME_DEVICE_GROUP;
			}
		}
		else {
			retVal = DALI_MSG_EVENT_SCHEME_DEVICE;
		}
	}
	return retVal;
}

DaliMsgEventScheme_t STRUCT_DALI_EVENT_MSG::getScheme() {
	return getScheme(m_receivedFrame);
}
/*****************************************************************************/
STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE::STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE(uint32_t receivedFrame) :
		Base(receivedFrame) { }

STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE::STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE(int shortAddress,int instanceAddress,int opcodeEvent) :
		Base(opcodeEvent) {
	REQUIRE(instanceAddress >= 0 && instanceAddress<=31);
	REQUIRE(shortAddress >= 0 && shortAddress<=63);

	this->m_receivedFrame |= (1<<15);
	this->m_receivedFrame |= ((instanceAddress & 0x1F) << 10);
	this->m_receivedFrame |= ((shortAddress & 0x3F) << 17);

}

bool STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE::isType( )  {
	if ((m_receivedFrame & (1<<15)) && (!(m_receivedFrame & (1<<23)))) {
		return true;
	}
	return false;
}

uint8_t STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE::getInstanceAddress( ) {
	static const int MASK = (1<<10) + (1<<11) + (1<<12) + (1<<13) + (1<<14);
	uint8_t retVal = ((m_receivedFrame & MASK) >> 10);
	ENSURE(retVal<32);
	return retVal;
}

uint8_t STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE::getShortAddress( ) {
	static const int MASK = (1<<17) + (1<<18) + (1<<19) + (1<<20) + (1<<21) + (1<<22);
	uint8_t retVal = ((m_receivedFrame & MASK) >> 17);
	ENSURE(retVal<64);
	return retVal;
}

/***********************************************************************************/
};
};
/***********************************************************************************/