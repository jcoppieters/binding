#ifndef NODEDBASESTRUCTS
#define NODEDBASESTRUCTS

///////////////////////////////////////////////////////////////////////////
#include "nodegenestructs.h"



#define LCD_TSLOCKED 0x02
#define LCD_LOCKED   0x01
#define LCD_UNLOCKED 0x00

#define PM_FATAL_ERROR 0
#define PM_WARNING 1
#define PM_ERROR 2

/* name length */
#define UNIT_NAME_LENGTH 16
#define NODE_NAME_LENGTH 16

/* ******************** Reset Modes ***************************/
#define P0WER_ON_RESET1 0x02
#define P0WER_ON_RESET2 0x03
#define WATCHDOG_RESET  0x00
#define EXTERNAL_RESET  0x01
#define NODERESET       0x04
#define UNIT_SOFTWARE_RESET 0x05

/* ************************ EV_NODEDLLSTATUS ***************************/
#define DLL_STATUS_BLOCK0_LENGTH 5
#define DLL_STATUS_BLOCK1_LENGTH 5

/* ************************ EV_NODERESOURCES ***************************/
#define RESOURCES_BLOCK0_LENGTH 4
#define RESOURCES_BLOCK1_LENGTH 5
#define RESOURCES_BLOCK2_LENGTH 3







///////////////////////////////////////////////////////////////////////////

typedef struct STRUCT_UNIT_ID STRUCT_UNIT_ID;
typedef struct STRUCT_POWERMATE STRUCT_POWERMATE;
typedef struct STRUCT_UNIT STRUCT_UNIT;



#endif