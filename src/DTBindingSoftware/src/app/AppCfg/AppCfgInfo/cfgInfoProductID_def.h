#pragma once


// Sedert versie 0x0700
typedef enum
{
	// Product ID gebruiken voor onderscheid in de verschillende node types.
	// Kan gebruikt worden om bvb te bepalen welk type AV-Matrix,Alarm centrale,

	PRODUCT_ID_UNKNOWN 						= 0x00,	/* NIET VERANDEREN */

	// Beck RS232 modem.
	PRODUCT_ID_MODEM_RS232					= 0x01,	/* NIET VERANDEREN */

	// Beck USB modem.
	PRODUCT_ID_MODEM_USB					= 0x02,	/* NIET VERANDEREN */

	// NXP Oled 4 buttons.
	PRODUCT_ID_OLED	   						= 0x10,	/* NIET VERANDEREN - Smarttouch*/
	PRODUCT_ID_OLED_HOTEL					= 0x11,	/* NIET VERANDEREN - Room-e */
	PRODUCT_ID_OLED_TOUCHBTN_4V				= 0x12, /* NIET VERANDEREN - Easytouch */

	// NXP TchBtn 4co1SE
	PRODUCT_ID_TCHBTN_4CO1SE				= 0x15,	/* NIET VERANDEREN */
	PRODUCT_ID_TCHBTN_1CO1SE				= 0x16,	/* NIET VERANDEREN */
	PRODUCT_ID_TCHBTN_2CO1SE				= 0x17,	/* NIET VERANDEREN */
	PRODUCT_ID_TCHBTN_8CO1SE				= 0x18,	/* NIET VERANDEREN */

	// Mechanische switch en push button
	PRODUCT_ID_1BTN_SWITCH_2CO1SE			= 0x19,	/* NIET VERANDEREN */
	PRODUCT_ID_2BTN_SWITCH_4CO1SE			= 0x1a,	/* NIET VERANDEREN */
	PRODUCT_ID_4BTN_SWITCH_8CO1SE			= 0x1b,	/* NIET VERANDEREN */
	PRODUCT_ID_1BTN_PUSH_1CO1SE				= 0x1c,	/* NIET VERANDEREN */
	PRODUCT_ID_2BTN_PUSH_2CO1SE				= 0x1d,	/* NIET VERANDEREN */
	PRODUCT_ID_4BTN_PUSH_4CO1SE				= 0x1e,	/* NIET VERANDEREN */

	// NXP Smartbox v2.0
	PRODUCT_ID_HC_BOX						= 0x20,	/* NXP Smartbox: NIET VERANDEREN */
	PRODUCT_ID_HC_BOX_ULTRA					= 0x21,	/* RPI Smartbox: NIET VERANDEREN */

	// Beck Gateway homeserver
	PRODUCT_ID_GATEWAY_HS  					= 0x30,	/* NIET VERANDEREN */

	// Beck Gateway homeserver 32 virtuals
	PRODUCT_ID_GATEWAY_HS_32VIRT 			= 0x31,	/* NIET VERANDEREN */

	// Beck Gateway homeserver master 100 virtuals
	PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT 	= 0x32,	/* NIET VERANDEREN */

	// Beck Modbus temperatuurs interface.
	PRODUCT_ID_GATEWAY_MODBUS_16SE32VIRT 	= 0x40,	/* NIET VERANDEREN */

	// Beck TFT.
	PRODUCT_ID_TFT_32VIRT  					= 0x50,	/* NIET VERANDEREN */

	// Std Modules.
	PRODUCT_ID_STD_ANALOG_8SENS				= 0x60,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_DMX_MASTER_64DIM			= 0x61,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_DALI_MASTER_DIM			= 0x62,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_16CO2SE					= 0x63,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_6SWITCH					= 0x64,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_3DUOSWITCH				= 0x65,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_6DIM						= 0x66,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_6DIM_PWMLED				= 0x67,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_ANALOGIN_8UNIVSE8CO		= 0x68,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_ANALOGIN_8SE16CO			= 0x69,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_12SWITCH 				= 0x6A,	/* NIET VERANDEREN */
	PRODUCT_ID_STD_6DUOSWITCH				= 0x6B, /* NIET VERANDEREN */


	PRODUCT_ID_8CHIRTX						= 0x80,	/* NIET VERANDEREN */
	// De 0x81 reserve houden voor basic ?
	PRODUCT_ID_8CHIRTX_AVMARANTZ			= 0x82, /* NIET VERANDEREN */
	PRODUCT_ID_8CHIRTX_AVINTEGRA			= 0x83, /* NIET VERANDEREN */
	PRODUCT_ID_8CHIRTX_AVRTI				= 0x84, /* NIET VERANDEREN */
	PRODUCT_ID_8CHIRTX_AVBNO				= 0x85, /* NIET VERANDEREN */
	PRODUCT_ID_8CHIRTX_AVAUDAC				= 0x86, /* NIET VERANDEREN */

	PRODUCT_ID_8CHIRTX_AVIP					= 0x90,	/* NIET VERANDEREN */

	// Alarm Modules.
	PRODUCT_ID_ALARM_TEXECOM				= 0xA0,	/* NIET VERANDEREN */
	PRODUCT_ID_ALARM_INTEGRA				= 0xA1,	/* NIET VERANDEREN */

	// AVMatrix modules
	PRODUCT_ID_AVMATRIX_BOSEST				= 0xB0,	/* NIET VERANDEREN */
	PRODUCT_ID_AVMATRIX_SONOS				= 0xB1,	/* NIET VERANDEREN */

	// TODO: Nieuwe product ids hier aanvullen...

} ProductID_t;


/**********************************************************************/
// Sedert V16.33
// Product smartbox ultra bevat interne units + virtuele/DALI units die 
// kunnen aangemaakt worden.
//
// Het laatste unitAddress van de interne units.
enum { PRODUCT_SBOXULTRA_UNITADDRESS_INTERNAL_LAST = 0x30 };
/**********************************************************************/
