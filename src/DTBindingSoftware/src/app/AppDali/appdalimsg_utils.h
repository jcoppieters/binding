#pragma once
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/

// Onderstaande code is gecopieerd van de RPI code.
// Kan gebruikt worden voor het parsen van de events.


/*****************************************************************************/
/************************* Parsen van de event messages **********************/
/*****************************************************************************/
typedef enum {
	DALI_MSG_EVENT_SCHEME_INSTANCE = 0,
	DALI_MSG_EVENT_SCHEME_DEVICE = 1,
	DALI_MSG_EVENT_SCHEME_DEVICE_AND_INSTANCE =2,
	DALI_MSG_EVENT_SCHEME_DEVICE_GROUP =3,
	DALI_MSG_EVENT_SCHEME_INSTANCE_GROUP = 4,
	DALI_MSG_EVENT_DEVICE_POWER_CYCLE = 5,
	DALI_MSG_EVENT_RESERVED

} DaliMsgEventScheme_t;
/*****************************************************************************/

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
struct STRUCT_DALI_EVENT_MSG {
	STRUCT_DALI_EVENT_MSG(uint32_t receivedFrame);
	STRUCT_DALI_EVENT_MSG(int opcodeEvent);

	bool isEvent();
	DaliMsgEventScheme_t getScheme();
	uint8_t getData( );

	// Conversie naar een byte array: MSB = array index 0
	void toByteArray(BYTE* pbData);

	static DaliMsgEventScheme_t getScheme(uint32_t receivedFrame);
	static bool isEvent(uint32_t receivedFrame);
	static uint8_t getData(uint32_t receivedFrame);

protected:
	uint32_t m_receivedFrame;
};
/***********************************************************************************/

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
struct STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE : public STRUCT_DALI_EVENT_MSG {
public:
	STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE(uint32_t receivedFrame);
	STRUCT_DALI_EVENT_MSG_SCHEME_DEVICE_AND_INSTANCE(int shortAddress,int instanceAddress,int opcodeEvent);

	bool isType( );
	uint8_t getInstanceAddress( );
	uint8_t getShortAddress( );

private:
	typedef STRUCT_DALI_EVENT_MSG Base;
};
/***********************************************************************************/
};
};
/***********************************************************************************/