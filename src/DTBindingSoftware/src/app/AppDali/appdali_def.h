#pragma once


/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/


/***********************************************************************************/

/***********************************************************************************/
/*********************** Control device capabilities *******************************/
/***********************************************************************************/
enum {
	DALI_CONTROLDEVICE_CAPABILITIES_APPCONTROLLER_PRESENT		= (1<<0),	
	DALI_CONTROLDEVICE_CAPABILITIES_INSTANCES_ENABLED			= (1<<1),
	DALI_CONTROLDEVICE_CAPABILITIES_APPCONTROLLER_ALWAYS_ON		= (1<<2),
};

/******************************************************************************/
/**************** Address schema's voor de control devices ********************/
/******************************************************************************/
/*
typedef enum {
	DALI_EVENT_SCHEME_ADDRESSING_DEVICE_INSTANCE = 0,
	DALI_EVENT_SCHEME_ADDRESSING_INSTANCE = 1,
	DALI_EVENT_SCHEME_ADDRESSING_DEVICE = 2,
	DALI_EVENT_SCHEME_ADDRESSING_DEVICEGROUP = 3,
	DALI_EVENT_SCHEME_ADDRESSING_INSTANCEGROUP = 4,
	DALI_EVENT_SCHEME_ADDRESSING_UNKNOWN =0xff
} DaliEventScheme_t;
*/

// Deze enum bevat andere waarden dan enum DaliEventScheme_t
enum {
	DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCE = 0,
	DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCE_TYPE = 1,
	DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCE_NUMBER = 2,
	DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_DEVICEGROUP = 3,
	DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCEGROUP = 4,
};

enum {
	DALI_CONTROLDEVICE_EVENT_SCHEME_DEFAULT = DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCE_NUMBER
};

/******************************************************************************/
/****************** Instance types voor de control devices ********************/
/******************************************************************************/


typedef enum {
	DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON = 1,
	DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_ABS_INPUT = 2,
	DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY = 3,
	DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_LIGHT_SENSOR = 4,						// NOT SUPPORTED
	DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_COLOR_SENSOR = 5,						// NOT SUPPORTED
	DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_GENERAL_SENSOR = 6						// NOT SUPPORTED
} DaliInstanceType_t;


/******************************************************************************/
/****************** Instance Event filters de control devices *****************/
/******************************************************************************/

typedef enum {
	DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_RELEASED			= (1<<0),
	DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_PRESSED			= (1<<1),
	DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_SHORT_PRESS		= (1<<2),
	DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_DOUBLE_PRESS		= (1<<3),
	DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_LONG_START		= (1<<4),
	DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_LONG_REPEAT		= (1<<5),
	DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_LONG_STOP		= (1<<6),
	DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_STUCK			= (1<<7),
	
} DaliPushButtonEventFilters_t;

enum {
	DALI_CONTROLDEVICE_PUSHBUTTON_DEFAULT_EVENTFILTER = 
		DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_SHORT_PRESS +
		DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_LONG_START +
		DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_LONG_STOP +
		DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_STUCK
};


typedef enum {
	DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_OCCUPIED					= (1<<0),
	DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_VACANT						= (1<<1),
	DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_STILL_OCCUPIED_VACANT		= (1<<2),
	DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_MOVEMENT					= (1<<3),
	DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_NO_MOVEMENT				= (1<<4)
	
} DaliDetectorOccupancyEventFilters_t;

enum {
	DALI_CONTROLDEVICE_DETECTOR_DEFAULT_EVENTFILTER = 
		DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_OCCUPIED +
		DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_VACANT +
		DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_STILL_OCCUPIED_VACANT +
		DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_MOVEMENT +
		DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_NO_MOVEMENT
};


typedef enum {
	DALI_CONTROLDEVICE_ABSINPUT_EVENT_ENABLE_POSITION			= (1<<0),
	
} DaliAbsInputEventFilters_t;

enum {
	DALI_CONTROLDEVICE_ABSINPUT_DEFAULT_EVENTFILTER = 
		DALI_CONTROLDEVICE_ABSINPUT_EVENT_ENABLE_POSITION
};

/******************************************************************************/

/***********************************************************************************/
}; /* namespace DUOTECNO */
}; /* namespace DALI */
/***********************************************************************************/