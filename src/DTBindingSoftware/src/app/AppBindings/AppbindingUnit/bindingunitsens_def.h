#pragma once

#define SENS_MODE_SUN			0x00
#define SENS_MODE_HALFSUN		0x01
#define SENS_MODE_MOON			0x02
#define SENS_MODE_HALFMOON		0x03

#define SENS_EVENT_ON			0x00				// Can be Heating / Cooling
#define SENS_EVENT_OFF			0x01

#define SENS_WORKING_MODE_AUTO 			0x00	/* TM 20120917 */
#define SENS_WORKING_MODE_HEATING       0x01    /* TM 20120917 */
#define SENS_WORKING_MODE_COOLING       0x02	/* TM 20120917 */
#define SENS_WORKING_MODE_DRY           0x03	/* TM 20120917 */
#define SENS_WORKING_MODE_FAN   		0x04	/* TM 20120917 */
#define SENS_WORKING_MODE_OFF    		0xFF	/* TM 20120917 - Wordt momenteel nog niet gebruikt !! */

#define SENS_FAN_SPEED_1				0x00	/* TM 20120917 */
#define SENS_FAN_SPEED_2				0x01	/* TM 20120917 */
#define SENS_FAN_SPEED_3				0x02	/* TM 20120917 */
#define SENS_FAN_SPEED_4				0x03	/* TM 20120917 */
#define SENS_FAN_SPEED_5				0x04	/* TM 20120917 */
#define SENS_FAN_SPEED_6				0x05	/* TM 20210111 - extra fan speed. */
#define SENS_FAN_SPEED_AUTO				0xFF	/* TM 20120917 */

// Swing modes uitgebreid.
// Swing mode was vroeger enkel on/off.
// Aangepast naar max 5 swing modes.
#define SENS_SWINGMODE_MIN				0x00 
#define SENS_SWINGMODE_MAX				0x05		/* TM 20210111 - Modes 0 tem 5 */


// Sedert versie 0x0D00:
// Events voor de universele sensor unit.
enum 
{
	SENSUNIV_EVENT_INC			= 0,
	SENSUNIV_EVENT_DEC			= 1,
	SENSUNIV_EVENT_INC_PULS		= 2,
	SENSUNIV_EVENT_DEC_PULS		= 3,
};

enum
{
	SENSUNIV_INCREMENT_ON	= 0,
	SENSUNIV_INCREMENT_OFF  = 1,
	SENSUNIV_DECREMENT_ON	= 2,
	SENSUNIV_DECREMENT_OFF	= 3
};

enum
{
	SENSUNIV_SETPOINT_MAX = 1000,	/* 100% */				
	SENSUNIV_SETPOINT_MIN = 0,		/* 0% */
	SENSUNIV_VALUE_MAX = 1000,		/* 100% */				
	SENSUNIV_VALUE_MIN = 0,			/* 0% */
};

enum 
{
	SENSUNIV_SAMPLETIME_MIN		= 1,
	SENSUNIV_SAMPLETIME_MINUTE	= 60,
	SENSUNIV_SAMPLETIME_MAX		= 120
};