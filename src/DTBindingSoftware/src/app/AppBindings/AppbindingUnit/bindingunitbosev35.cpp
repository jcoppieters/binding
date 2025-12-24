#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitbosev35.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??
#include "BindingUnitState.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"


using namespace DUOTECNO::BINDINGS::UI;


//#define IRAUDIO_FUNCTION_MASK		0x01


const BYTE CBindingBoseV35Unit::bindingFlagTabel[ BOSEV35_MAX_BINDINGFLAG_FUNCTIONS ][ BOSEV35_MAX_SOURCES ] = 
{
	/* 			  Tuner, HDMI1,	HDMI2,	HDMI3,	IN1,	IN2, 	IPOD,	Local */
	/* KP1 */  	{ 0xFF,	0x36,	0xFF,	0xFF,	0x4a,	0xFF,	0x2B,	0x10 	},	/*00=BOSEV35_SOURCE_FUNCTION_KEY1*/
	/* KP2 */	{ 0xFF,	0x37,	0xFF,	0xFF,	0x4b,	0xFF,	0x2C,	0x11	},	/*01=BOSEV35_SOURCE_FUNCTION_KEY2*/
	/* KP3 */	{ 0xFF,	0x38,	0xFF,	0xFF,	0x4c,	0xFF,	0x2D,	0x12	},	/*02=BOSEV35_SOURCE_FUNCTION_KEY3*/
	/* KP4 */	{ 0xFF,	0x39,	0xFF,	0xFF,	0x4d,	0xFF,	0x2E,	0x13  	},	/*03=BOSEV35_SOURCE_FUNCTION_KEY4*/
	/* KP5 */	{ 0xFF,	0x3a,	0xFF,	0xFF,	0x4e,	0xFF,	0x2F,	0x14	},	/*04=BOSEV35_SOURCE_FUNCTION_KEY5*/
	/* KP6 */	{ 0xFF,	0x3b,	0xFF,	0xFF,	0x4f,	0xFF,	0x30,	0x15	},	/*05=BOSEV35_SOURCE_FUNCTION_KEY6*/
	/* KP7 */	{ 0xFF,	0x3c,	0xFF,	0xFF,	0x50,	0xFF,	0x31,	0x16	},	/*06=BOSEV35_SOURCE_FUNCTION_KEY7*/
	/* KP8 */	{ 0xFF,	0x3d,	0xFF,	0xFF,	0x51,	0xFF,	0x32,	0x17	},	/*07=BOSEV35_SOURCE_FUNCTION_KEY8*/
	/* KP9 */	{ 0xFF,	0x3e,	0xFF,	0xFF,	0x52,	0xFF,	0x33,	0x18	},	/*08=BOSEV35_SOURCE_FUNCTION_KEY9*/
	/* KP0 */	{ 0xFF,	0x3f,	0xFF,	0xFF,	0x53,	0xFF,	0x34,	0x19	},	/*09=BOSEV35_SOURCE_FUNCTION_KEY0*/	
	/* OP  */	{ 0xFF,	0x55,	0xFF,	0xFF,	0x5c,	0xFF,	0xFF,	0x1B	},	/*10=BOSEV35_SOURCE_FUNCTION_KPUP*/
	/* Down*/	{ 0xFF,	0x56,	0xFF,	0xFF,	0x5d,	0xFF,	0xFF,	0x1C	},	/*11=BOSEV35_SOURCE_FUNCTION_KPDOWN*/
	/* Left*/	{ 0xFF,	0x57,	0xFF,	0xFF,	0x5e,	0xFF,	0xFF,	0x1D  	},	/*12=BOSEV35_SOURCE_FUNCTION_KPLEFT*/
	/* Right*/	{ 0xFF,	0x58,	0xFF,	0xFF,	0x5f,	0xFF,	0xFF,	0x1E	},	/*13=BOSEV35_SOURCE_FUNCTION_KPRIGHT/
	/* Enter*/	{ 0xFF,	0x59,	0xFF,	0xFF,	0x60,	0xFF,	0xFF,	0x1F	},	/*14=BOSEV35_SOURCE_FUNCTION_KPENTER*/
	/* 		*/ 	{ 0xFF,	0x40,	0xFF,	0xFF,	0x54,	0xFF,	0x35,	0x1A 	},	/*15=BOSEV35_SOURCE_FUNCTION_TIENEEN*/
	/* Menu */	{ 0xFF,	0x75,	0xFF,	0xFF,	0x6B,	0xFF,	0xFF,	0x71	},	/*38=BOSEV35_SOURCE_FUNCTION_FREE1_MENU*/
	/* Guide */	{ 0xFF,	0x6E,	0xFF,	0xFF,	0x6F,	0xFF,	0xFF,	0x74	},	/*39=BOSEV35_SOURCE_FUNCTION_FREE2_GUIDE*/
	/* Info */	{ 0xFF,	0x76,	0xFF,	0xFF,	0x6C,	0xFF,	0xFF,	0x72	},	/*40=BOSEV35_SOURCE_FUNCTION_FREE3_INFO*/
	/* Exit */	{ 0xFF,	0x77,	0xFF,	0xFF,	0x6D,	0xFF,	0xFF,	0x73	}, 	/*41=BOSEV35_SOURCE_FUNCTION_FREE4_EXIT*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*20=BOSEV35_SOURCE_FUNCTION_FREE5*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*21=BOSEV35_SOURCE_FUNCTION_UP1*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*22=BOSEV35_SOURCE_FUNCTION_DOWN1*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*23=BOSEV35_SOURCE_FUNCTION_UP2*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*24=BOSEV35_SOURCE_FUNCTION_DOWN2*/
	/* Play */	{ 0xFF,	0x41,	0xFF,	0xFF,	0x62,	0xFF,	0xFF,	0x22 	},	/*25=BOSEV35_SOURCE_FUNCTION_PLAY*/
	/* Pause */ { 0xFF,	0x43,	0xFF,	0xFF,	0x64,	0xFF,	0xFF,	0x24 	},	/*26=BOSEV35_SOURCE_FUNCTION_PAUSE*/
	/* CH- */	{ 0xFF,	0x48,	0xFF,	0xFF,	0x69,	0xFF,	0xFF,	0x29 	},	/*27=BOSEV35_SOURCE_FUNCTION_DEC*/
	/* CH+ */	{ 0xFF,	0x49,	0xFF,	0xFF,	0x6A,	0xFF,	0xFF,	0x2A 	},	/*28=BOSEV35_SOURCE_FUNCTION_INC*/
	/* Prev */	{ 0xFF,	0x44,	0xFF,	0xFF,	0x65,	0xFF,	0xFF,	0x25 	},	/*29=BOSEV35_SOURCE_FUNCTION_PREV*/
	/* Next */	{ 0xFF,	0x45,	0xFF,	0xFF,	0x66,	0xFF,	0xFF,	0x26 	},	/*30=BOSEV35_SOURCE_FUNCTION_NEXT*/
	/* Stop */	{ 0xFF,	0x42,	0xFF,	0xFF,	0x63,	0xFF,	0xFF,	0x23 	},	/*31=BOSEV35_SOURCE_FUNCTION_STOP*/	
	/* FREV */  { 0xFF, 0x46,	0xFF,	0xFF,	0x67,	0xFF,	0xFF,	0x27	},	/*32=BOSEV35_SOURCE_FUNCTION_FREV*/
	/* FFWD */	{ 0xFF,	0x47,	0xFF,	0xFF,	0x68,	0xFF,	0xFF,	0x28	},	/*33=BOSEV35_SOURCE_FUNCTION_FFWD*/
	/* Page+ */	{ 0xFF,	0x5a,	0xFF,	0xFF,	0x61,	0xFF,	0xFF,	0x20	},	/*34=BOSEV35_SOURCE_FUNCTION_PAGEUP*/
	/* Page- */ { 0xFF,	0x5b,	0xFF,	0xFF,	0x62,	0xFF,	0xFF,	0x21	},	/*35=BOSEV35_SOURCE_FUNCTION_PAGEDOWN*/	
	/* Record*/ { 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*36=BOSEV35_SOURCE_FUNCTION_RECORD*/
	/* Shuffle*/{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*37=BOSEV35_SOURCE_FUNCTION_SHUFFLE*/				
	/* On */	{ 0x00,	0x02,	0x04,	0x06,	0x08,	0x0A,	0x0C,	0x0E	},	/*38=BOSEV35_SOURCE_FUNCTION_ON*/
	/* Off */	{ 0x01,	0x03,	0x05,	0x07,	0x09,	0x0B,	0x0D,	0x0F 	},	/*39=BOSEV35_SOURCE_FUNCTION_OFF*/
	/* channel*/{ 0x7B, 0x7B,   0x7B,   0x7B,   0x7B,   0x7B,   0x7B,   0x7B    },  /*40=BOSEV35_SOURCE_FUNCTION_LAST_CHANNEL*/
	/* TV more*/{ 0x7D, 0x7D,   0x7D,	0x7D,	0x7D,	0x7D,	0x7D,	0x7D	},	/*41=BOSEV35_DEST_FUNCTION_TV_MORE*/
	/* TV wide*/{ 0x7C, 0x7C,   0x7C,	0x7C,	0x7C,	0x7C,	0x7C,	0x7C	},	/*42=BOSEV35_DEST_FUNCTION_TV_WIDE*/
	/* TV Power*/{ 0x7A, 0x7A,   0x7A,	0x7A,	0x7A,	0x7A,	0x7A,	0x7A	},	/*43=BOSEV35_DEST_FUNCTION_TV_POWER*/
	/* TV input*/{ 0x79, 0x79,  0x79,	0x79,	0x79,	0x79,	0x79,	0x79	},	/*44=BOSEV35_DEST_FUNCTION_TV_INPUT*/ 
	/* Dest On*/ { 0x7E, 0x7E,	0x7E,	0x7E,	0x7E,	0x7E,	0x7E,	0x7E	},  /*45=BOSEV35_DEST_FUNCTION_ON*/
	/* Dest off*/{ 0x7F, 0x7F,	0x7F,	0x7F,	0x7F,	0x7F,	0x7F,	0x7F	},  /*46=BOSEV35_DEST_FUNCTION_OFF*/
};


CBindingBoseV35Unit::CBindingBoseV35Unit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{		
	SetInOutType(UNIT_TYPE_INPUT);

	Init();	
}

CBindingBoseV35Unit::CBindingBoseV35Unit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();	
}

CBindingBoseV35Unit::~CBindingBoseV35Unit()
{

}

void CBindingBoseV35Unit::Init()
{
	CBindingBoseV35UnitState* const pUnitState = 
		CBindingUnitState::Instance()->get_BoseV35Unit();

	if (pUnitState != 0)
	{
		Set_Source(pUnitState->get_Source());
		Set_Button(pUnitState->get_Button());
		Set_ControlEvent(pUnitState->get_ControlEvent());
		Set_TKFunction(pUnitState->get_FunctionTK());
		Set_Event(pUnitState->get_Event());
		Set_State(pUnitState->get_State());
		Set_Function(pUnitState->get_Function());
		Set_SrcFunction(pUnitState->get_FunctionSrc());
		Set_DestFunction(pUnitState->get_FunctionDest());
		Set_FunctionExtraIR(pUnitState->get_FunctionExtraIR());
		Set_Volume(pUnitState->get_Volume( ));
		Set_DestFunctionExtraIR(pUnitState->get_DestFunctionExtraIR());
	}
	else
	{
		Set_Source(BOSEV35_SOURCE_HDMI1);	
		Set_Button(BOSEV35_SOURCE_FUNCTION_KEY1);	
		Set_ControlEvent(EV_UNITCONTROLPULS);
		Set_TKFunction(BOSEV35_SOURCE_FUNCTION_PLAY);
		Set_Event(BOSEV35_EVENT_KEYPAD); 
		Set_State(0);
		Set_Function(BOSERS_OBJECT_ATTR_SOURCE_ON);
		Set_SrcFunction(0);
		Set_DestFunction(0);
		Set_FunctionExtraIR(BOSEV35_SOURCE_FUNCTION_KEY1);
		Set_Volume(20);
		Set_DestFunctionExtraIR(BOSEV35_DEST_FUNCTION_TV_POWER);
	}
	

	Set_ControlState(FALSE);
}

void CBindingBoseV35Unit::Set_Function(BYTE bData)			
{
	// oppassen: Een bose all rooms als uitgang mag enkel een destination off
	// hebben ...

	if (GetUnitAddress( ) == BROADCAST_UNIT)
	{
		if (bData != BOSERS_OBJECT_METHOD_OFF)
		{
			bData = BOSERS_OBJECT_METHOD_OFF;
		}
	}

	bFunctionData = bData;
};


BOOL CBindingBoseV35Unit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) 
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}
	else if (bMessageCode == FC_UNITAUDIOSET)
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}


BOOL CBindingBoseV35Unit::ConvertToBindingData(void)
{
	BYTE bMethodData;	
	BYTE bEvent,bBindingFlags;		/* bFunction; */

	CBindingBoseV35UnitState* const pUnitState = 
		CBindingUnitState::Instance()->get_BoseV35Unit();

	BindingData.Clear();

	switch(GetInOutType())
	{
		case UNIT_TYPE_INPUT:
		{
			bEvent = Get_Event();		

			switch (bEvent)
			{
				case BOSEV35_EVENT_DESTON:
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);

					bBindingFlags = ConvertFromBindingTable(BOSEV35_SOURCE_TUNER,BOSEV35_DEST_FUNCTION_ON);				
					BindingData.Set_BindingFlags(bBindingFlags);	
	
					if (pUnitState != 0)
					{
						pUnitState->set_Event(bEvent);
					}

				} break;

				case BOSEV35_EVENT_DESTOFF:
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);

					bBindingFlags = ConvertFromBindingTable(BOSEV35_SOURCE_TUNER,BOSEV35_DEST_FUNCTION_OFF);				
					BindingData.Set_BindingFlags(bBindingFlags);	
	
					if (pUnitState != 0)
					{
						pUnitState->set_Event(bEvent);
					}

				} break;

				case BOSEV35_EVENT_SOURCEOFF :
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);

					bBindingFlags = ConvertFromBindingTable(Get_Source(),BOSEV35_SOURCE_FUNCTION_OFF);														
					BindingData.Set_BindingFlags(bBindingFlags);	
									

					if (pUnitState != 0)
					{
						pUnitState->set_Event(bEvent);
						pUnitState->set_Source(Get_Source());
					}
								
				} break;

				case BOSEV35_EVENT_SOURCEON :
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);
					
					bBindingFlags = ConvertFromBindingTable(Get_Source(),BOSEV35_SOURCE_FUNCTION_ON);								
					BindingData.Set_BindingFlags(bBindingFlags);			
					
				
					if (pUnitState != 0)
					{
						pUnitState->set_Event(bEvent);
						pUnitState->set_Source(Get_Source());
					}

				} break;

				case BOSEV35_EVENT_KEYPAD :
				{					
					BindingData.Set_MessageCode(Get_ControlEvent());
					bBindingFlags = ConvertFromBindingTable(Get_Source(),Get_Button());
					
					if ( bBindingFlags != 0xFF ) 
					{
						if (Get_ControlState()) 
						{
							bBindingFlags |= 0x80;
						}

						BindingData.Set_BindingFlags(bBindingFlags);						

						if (pUnitState != 0)
						{
							pUnitState->set_Button(Get_Button());
							pUnitState->set_Source(Get_Source());
							pUnitState->set_ControlEvent(Get_ControlEvent());
							pUnitState->set_Event(bEvent);
						}
					}		

				} break;

				case BOSEV35_EVENT_EXTRA:
				{				
					BindingData.Set_MessageCode(Get_ControlEvent());
					bBindingFlags = ConvertFromBindingTable( BOSEV35_SOURCE_TUNER ,Get_DestFunctionExtraIR());
					
					if ( bBindingFlags != 0xFF ) 
					{
						if (Get_ControlState()) 
						{
							bBindingFlags |= 0x80;
						}

						BindingData.Set_BindingFlags(bBindingFlags);						

						if (pUnitState != 0)
						{
							pUnitState->set_DestFunctionExtraIR(Get_Button());						
							pUnitState->set_ControlEvent(Get_ControlEvent());
							pUnitState->set_Event(bEvent);
						}
					}		
										
				} break;

				default:
					ASSERT(0);
				break;
			}

		} break;

		case UNIT_TYPE_OUTPUT:
		{
			BindingData.Set_MessageCode(FC_UNITAUDIOSET);
			bMethodData = Get_Function();
			BindingData.Set_MethodData(bMethodData);

			switch(bMethodData)
			{
				case BOSERS_OBJECT_USE_SELECTOR_DATA :
				case BOSERS_OBJECT_ATTR_STATE:
				return FALSE;

				case BOSERS_OBJECT_ATTR_SOURCE_ON:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_Source());
										
					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);
						pUnitState->set_Source(Get_Source());
					}

				} return TRUE;

				case BOSERS_OBJECT_METHOD_SOURCEINC:
				case BOSERS_OBJECT_METHOD_SOURCEDEC:
				case BOSERS_OBJECT_METHOD_CSON:
				case BOSERS_OBJECT_METHOD_OFF:
				case BOSERS_OBJECT_METHOD_FSCSONOFF:
				case BOSERS_OBJECT_METHOD_VOLUP:
				case BOSERS_OBJECT_METHOD_VOLDOWN:
				case BOSERS_OBJECT_METHOD_FSVOLUPDOWN:
				case BOSERS_OBJECT_METHOD_PRESUP:
				case BOSERS_OBJECT_METHOD_PRESDOWN:
				case BOSERS_OBJECT_METHOD_PLAY:
				case BOSERS_OBJECT_METHOD_STOP:
				case BOSERS_OBJECT_METHOD_PAUSE:
				case BOSERS_OBJECT_METHOD_FF:
				case BOSERS_OBJECT_METHOD_RR:
				case BOSERS_OBJECT_METHOD_FFWD:
				case BOSERS_OBJECT_METHOD_FRWD:
				{
					BindingData.Set_Length(1);

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);						
					}

				} return TRUE;

				case BOSERS_OBJECT_ATTR_ONOFF:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_State() & 0x01);

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);						
						pUnitState->set_State(Get_State() & 0x01);
					}

				} return TRUE;


				case BOSERS_OBJECT_METHOD_SOURCE_FUNCTION:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_SrcFunction() & 0x01);

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);						
						pUnitState->set_FunctionSrc(Get_SrcFunction() & 0x01);
					}

				} return TRUE;

				case BOSERS_OBJECT_METHOD_DESTINATION_FUNCTION:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_DestFunction() & 0x01);

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);						
						pUnitState->set_FunctionDest(Get_DestFunction() & 0x01);
					}

				} return TRUE;	

				case BOSERS_OBJECT_METHOD_EXTRAIR:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_FunctionExtraIR());

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);						
						pUnitState->set_FunctionExtraIR(Get_FunctionExtraIR());
					}
	
				} return TRUE;

				case BOSERS_OBJECT_ATTR_VOLUME:
				{
					/**
					 * TODO 
					 */ 
				} return TRUE;
						
				default:
				{
					ASSERT(0);

				} return FALSE;
			}
		}break;	
			
	}
	return TRUE;
}


BOOL CBindingBoseV35Unit::ConvertFromBindingData(void)
{
	Init();		//TM 20040701

	BYTE bTemp;
	BYTE bMessageCode = BindingData.Get_MessageCode();
	
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{ 		
		const BYTE bBindingFlags = BindingData.Get_BindingFlags() & 0x7F;		
		bTemp = GetFunctionFromBindingTable(bBindingFlags);

		if (bTemp != 0xFF) 
		{		
			if (bTemp == BOSEV35_DEST_FUNCTION_ON)
			{
				Set_Event(BOSEV35_EVENT_DESTON);	
			}
			else if (bTemp == BOSEV35_DEST_FUNCTION_OFF)
			{
				Set_Event(BOSEV35_EVENT_DESTOFF);
			}
			else if (bTemp == BOSEV35_SOURCE_FUNCTION_ON) 
			{
				Set_Event(BOSEV35_EVENT_SOURCEON);

				bTemp = GetSourceFromBindingTable(bBindingFlags);

				if (bTemp != 0xFF) 
				{
					Set_Source(bTemp);
				}
			}
			else if (bTemp == BOSEV35_SOURCE_FUNCTION_OFF) 
			{
				Set_Event(BOSEV35_EVENT_SOURCEOFF);

				bTemp = GetSourceFromBindingTable(bBindingFlags);

				if (bTemp != 0xFF) 
				{
					Set_Source(bTemp);
				}				
			}
			else if ((bTemp >= BOSEV35_SOURCE_FUNCTION_LAST_CHANNEL) && 
					 (bTemp <= BOSEV35_DEST_FUNCTION_TV_INPUT)) 
			{
				Set_Event(BOSEV35_EVENT_EXTRA);	
				Set_DestFunctionExtraIR(bTemp);											
			}																						
			else if ((bTemp >= BOSEV35_SOURCE_FUNCTION_KEY1) && 
					 (bTemp <= BOSEV35_SOURCE_FUNCTION_SHUFFLE)) 
			{								
				Set_Event(BOSELF48_EVENT_KEYPAD);
				Set_Button(bTemp);
				Set_ControlEvent(bMessageCode);												

				bTemp = GetSourceFromBindingTable(bBindingFlags);

				if (bTemp != 0xFF) 
				{
					Set_Source(bTemp);
				}
			}												
		}
	}
	else 
	{
		if (bMessageCode != FC_UNITAUDIOSET) 
		{
			ASSERT(0);
			return FALSE;
		}
		
		BYTE bMethodData = BindingData.Get_MethodData();
		Set_Function(bMethodData);
		BYTE bLength = BindingData.Get_Length();

		switch(bMethodData) 
		{
			case BOSERS_OBJECT_USE_SELECTOR_DATA :
			case BOSERS_OBJECT_ATTR_STATE:
			return FALSE;

			case BOSERS_OBJECT_ATTR_SOURCE_ON:
			{
				if (bLength != 2) return FALSE;
				Set_Source(BindingData.Get_Data(1));

			} return TRUE;

			case BOSERS_OBJECT_METHOD_SOURCEINC:
			case BOSERS_OBJECT_METHOD_SOURCEDEC:
			case BOSERS_OBJECT_METHOD_CSON:
			case BOSERS_OBJECT_METHOD_OFF:
			case BOSERS_OBJECT_METHOD_FSCSONOFF:
			case BOSERS_OBJECT_METHOD_VOLUP:
			case BOSERS_OBJECT_METHOD_VOLDOWN:
			case BOSERS_OBJECT_METHOD_FSVOLUPDOWN:
			case BOSERS_OBJECT_METHOD_PRESUP:
			case BOSERS_OBJECT_METHOD_PRESDOWN:
			case BOSERS_OBJECT_METHOD_PLAY:
			case BOSERS_OBJECT_METHOD_STOP:
			case BOSERS_OBJECT_METHOD_PAUSE:
			case BOSERS_OBJECT_METHOD_FF:
			case BOSERS_OBJECT_METHOD_RR:
			case BOSERS_OBJECT_METHOD_FFWD:
			case BOSERS_OBJECT_METHOD_FRWD:
			{
				if(bLength != 1) return FALSE;

			} return TRUE;

			case BOSERS_OBJECT_ATTR_ONOFF:
			{
				if (bLength != 2) return FALSE;
				Set_State(BindingData.Get_Data(1));

			} return TRUE;

			case BOSERS_OBJECT_METHOD_SOURCE_FUNCTION:
			{
				if (bLength != 2) return FALSE;
				Set_SrcFunction(BindingData.Get_Data(1));

			} return TRUE;

			case BOSERS_OBJECT_METHOD_DESTINATION_FUNCTION:
			{
				if (bLength != 2) return FALSE;
				Set_DestFunction(BindingData.Get_Data(1));

			} return TRUE;
		
			case BOSERS_OBJECT_ATTR_VOLUME:
			{
				if (bLength != 2) return FALSE;
				Set_Volume(BindingData.Get_Data(1));

			} return TRUE;

			case BOSERS_OBJECT_METHOD_EXTRAIR:
			{
				if (bLength != 2) return FALSE;
				Set_FunctionExtraIR(BindingData.Get_Data(1));

			} return TRUE;

			default:
				ASSERT(0);
			return FALSE;
		}
		
	}
	return TRUE;
}

CString CBindingBoseV35Unit::ConvertFuntionToString(void)
{
	CString strReturn;
	CString strMethod;	
	CString strFunction;
	CString strData;

	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{ 			
		const BYTE bMethodData = Get_Event();

		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(BOSEV35_INPUTSTRINGS_EXTENDED);

		ASSERT(pStringEntries);
		pStringEntries->Find(bMethodData,strFunction);
		

		if ((bMethodData == BOSEV35_EVENT_DESTOFF) || 
			(bMethodData == BOSEV35_EVENT_DESTON)) 			
		{
			return strFunction;
		}

		switch(bMethodData) 
		{
			case BOSEV35_EVENT_SOURCEOFF:
			case BOSEV35_EVENT_SOURCEON:
			{
				strReturn.Format("%s - %s",strFunction,Get_strSource( Get_Source( )));

			} return strReturn;


			case BOSEV35_EVENT_KEYPAD:
			{			
				// TM 20100823: Solved bug.
				strReturn.Format("%s - %s - %s - %s",
								 strFunction,
								 Get_strSource(Get_Source()),
								 Get_strFunctionExtraIR(Get_Button()),
								 (Get_ControlEvent() == EV_UNITCONTROLPULS ? "Short" : "Long"));
					
			} return strReturn;

			case BOSEV35_EVENT_EXTRA:
			{
				
				strReturn.Format("%s - %s - %s",
								 strFunction,								
								 Get_strFunctionExtraIR(Get_DestFunctionExtraIR()),
								 (Get_ControlEvent() == EV_UNITCONTROLPULS ? "Short" : "Long"));
										
			} return strReturn;
		}
	}
	else 
	{
		CString strTemp;	

		const BYTE bMethodData = Get_Function();
		
		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(BOSEV35_STRINGS_EXTENDED);

		ASSERT(pStringEntries);
		pStringEntries->Find(bMethodData,strFunction);

		switch(bMethodData)
		{
			case BOSERS_OBJECT_ATTR_SOURCE_ON:				
			{
				strReturn.Format("%s - %s",strFunction,Get_strSource( Get_Source( )));				

			} return strReturn;

			case BOSERS_OBJECT_METHOD_SOURCEINC:
			case BOSERS_OBJECT_METHOD_SOURCEDEC:
			{
				strReturn.Format("%s - %s",strFunction,Get_strSource( Get_Source( )));

			} return strReturn;

			case BOSERS_OBJECT_METHOD_CSON:
			case BOSERS_OBJECT_METHOD_OFF:
			case BOSERS_OBJECT_METHOD_FSCSONOFF:
			case BOSERS_OBJECT_METHOD_VOLUP:
			case BOSERS_OBJECT_METHOD_VOLDOWN:
			case BOSERS_OBJECT_METHOD_FSVOLUPDOWN:
			case BOSERS_OBJECT_METHOD_PRESUP:
			case BOSERS_OBJECT_METHOD_PRESDOWN:	
			case BOSERS_OBJECT_METHOD_PLAY:
			case BOSERS_OBJECT_METHOD_STOP:
			case BOSERS_OBJECT_METHOD_PAUSE:
			case BOSERS_OBJECT_METHOD_FF:
			case BOSERS_OBJECT_METHOD_RR:
			case BOSERS_OBJECT_METHOD_FFWD:
			case BOSERS_OBJECT_METHOD_FRWD:
			{
				strReturn.Format("%s",strFunction);

			} return strReturn;

			case BOSERS_OBJECT_ATTR_ONOFF:
			{
				if (Get_State() & 0x01) strTemp.LoadString(IDS_ON);
				else strTemp.LoadString(IDS_OFF);

				strReturn.Format("%s - %s",strFunction,strTemp);

			} return strReturn;
					
			case BOSERS_OBJECT_METHOD_DESTINATION_FUNCTION:					
			{
				strTemp.LoadString(IDS_FUNCTION);
				strReturn.Format("%s - %s %d",strFunction,strTemp,
											  Get_DestFunction() & IRAUDIO_FUNCTION_MASK);
			} return strReturn;
							
			case BOSERS_OBJECT_METHOD_SOURCE_FUNCTION:
			{
				strTemp.LoadString(IDS_FUNCTION);
				strReturn.Format("%s - %s %d",strFunction,strTemp,
											  Get_SrcFunction() & IRAUDIO_FUNCTION_MASK);
			} return strReturn;

			case BOSERS_OBJECT_METHOD_EXTRAIR:
			{
				strReturn.Format("%s - %s",
								 strFunction,
								 Get_strFunctionExtraIR(Get_FunctionExtraIR( )));


			} return strReturn;


			default:				
			break;	
		}
	}

	if (!strData.IsEmpty())
	{
		strReturn.Format("%s%s",strMethod,strData);	
	}
	else
	{
		strReturn = strMethod;
	}

	return strReturn;
}

CString CBindingBoseV35Unit::Get_strSource(int src)
{
	static const int _mappingSourceID[ BOSEV35_MAX_SOURCES ] =
	{
		IDS_BOSEV35_SOURCE0,		// BOSEV35_SOURCE_TUNER
		IDS_BOSEV35_SOURCE1,		// BOSEV35_SOURCE_HDMI1
		IDS_BOSEV35_SOURCE2,		// BOSEV35_SOURCE_HDMI2
		IDS_BOSEV35_SOURCE3,		// BOSEV35_SOURCE_HDMI3
		IDS_BOSEV35_SOURCE4,		// BOSEV35_SOURCE_IN1
		IDS_BOSEV35_SOURCE5,		// BOSEV35_SOURCE_IN2
		IDS_BOSEV35_SOURCE6,		// BOSEV35_SOURCE_IPOD
		IDS_BOSEV35_SOURCE7,		// BOSELF48_SOURCE_LOCAL
	};

	int resourceID = -1;
	CString szReturn;

	if (src >= 0 && src < BOSEV35_MAX_SOURCES)
	{
		resourceID = _mappingSourceID[ src ]; 	
	}

	if (resourceID != -1)
	{
		szReturn.LoadString( resourceID );
	}
	
	if (szReturn.IsEmpty())
	{
		szReturn = "Undefined source";
	}		

	return szReturn;
}

CString CBindingBoseV35Unit::Get_strFunctionExtraIR(int func)
{
	static const int _mappingResourceID[ BOSEV35_MAX_BINDINGFLAG_FUNCTIONS ] =
	{
		IDS_BOSELF_KEY1,			// BOSEV35_SOURCE_FUNCTION_KEY1         0
		IDS_BOSELF_KEY2,			// BOSEV35_SOURCE_FUNCTION_KEY2         1
		IDS_BOSELF_KEY3,			// BOSEV35_SOURCE_FUNCTION_KEY3         2
		IDS_BOSELF_KEY4,			// BOSEV35_SOURCE_FUNCTION_KEY4         3
		IDS_BOSELF_KEY5,			// BOSEV35_SOURCE_FUNCTION_KEY5         4
		IDS_BOSELF_KEY6,			// BOSEV35_SOURCE_FUNCTION_KEY6         5
		IDS_BOSELF_KEY7,			// BOSEV35_SOURCE_FUNCTION_KEY7         6
		IDS_BOSELF_KEY8,			// BOSEV35_SOURCE_FUNCTION_KEY8         7
		IDS_BOSELF_KEY9,			// BOSEV35_SOURCE_FUNCTION_KEY9         8
		IDS_BOSELF_KEY0,			// BOSEV35_SOURCE_FUNCTION_KEY0         9	
		IDS_BOSELF_KEYUP,			// BOSEV35_SOURCE_FUNCTION_KPUP     	10
		IDS_BOSELF_KEYDOWN,			// BOSEV35_SOURCE_FUNCTION_KPDOWN   	11
		IDS_BOSELF_KEYLEFT,			// BOSEV35_SOURCE_FUNCTION_KPLEFT   	12
		IDS_BOSELF_KEYRIGHT,		// BOSEV35_SOURCE_FUNCTION_KPRIGHT  	13
		IDS_BOSELF_KEYENTER,		// BOSEV35_SOURCE_FUNCTION_KPENTER  	14
		IDS_BOSELF_KEY10,			// BOSEV35_SOURCE_FUNCTION_TIENEEN  	15
		IDS_BOSELF_MENU,			// BOSEV35_SOURCE_FUNCTION_FREE1_MENU 	16	// MENU 
		IDS_BOSELF_GUIDE,			// BOSEV35_SOURCE_FUNCTION_FREE2_GUIDE	17	// GUIDE 
		IDS_BOSELF_INFO,			// BOSEV35_SOURCE_FUNCTION_FREE3_INFO 	18	// INFO 
		IDS_BOSELF_EXIT,			// BOSEV35_SOURCE_FUNCTION_FREE4_EXIT 	19	// EXIT 
		IDS_BOSELF_RESERVED,		// BOSEV35_SOURCE_FUNCTION_FREE5    	20
		IDS_BOSELF_RED,				// BOSEV35_SOURCE_FUNCTION_UP1      	21	// RED
		IDS_BOSELF_GREEN,			// BOSEV35_SOURCE_FUNCTION_DOWN1    	22	// GREEN
		IDS_BOSELF_YELLOW,			// BOSEV35_SOURCE_FUNCTION_UP2      	23	// YELLOW
		IDS_BOSELF_BLUE,			// BOSEV35_SOURCE_FUNCTION_DOWN2    	24	// BLUE
		IDS_BOSELF_PLAY,			// BOSEV35_SOURCE_FUNCTION_PLAY			25
		IDS_BOSELF_PAUSE,			// BOSEV35_SOURCE_FUNCTION_PAUSE		26
		IDS_BOSELF_DEC,				// BOSEV35_SOURCE_FUNCTION_DEC			27 - Channel-
		IDS_BOSELF_INC,				// BOSEV35_SOURCE_FUNCTION_INC			28 - Channel+
		IDS_BOSELF_PREV,			// BOSEV35_SOURCE_FUNCTION_PREV			29 - prev
		IDS_BOSELF_NEXT,			// BOSEV35_SOURCE_FUNCTION_NEXT			30 - next
		IDS_BOSELF_STOP,			// BOSEV35_SOURCE_FUNCTION_STOP			31			
		IDS_BOSELF_FREV,			// BOSEV35_SOURCE_FUNCTION_FREV			32
		IDS_BOSELF_FFWD,			// BOSEV35_SOURCE_FUNCTION_FFWD			33
		IDS_BOSELF_PAGEUP,			// BOSEV35_SOURCE_FUNCTION_PAGEUP		34
		IDS_BOSELF_PAGEDOWN,		// BOSEV35_SOURCE_FUNCTION_PAGEDOWN		35
		IDS_BOSELF_RECORD,			// BOSEV35_SOURCE_FUNCTION_RECORD		36
		IDS_BOSELF_SHUFFLE,			// BOSEV35_SOURCE_FUNCTION_SHUFFLE		37
		-1,							// BOSEV35_SOURCE_FUNCTION_ON			38
		-1,							// BOSEV35_SOURCE_FUNCTION_OFF			39
		IDS_BOSELF_LAST_CHANNEL,	// BOSEV35_SOURCE_FUNCTION_LAST_CHANNEL 	40
		IDS_BOSELF_TV_MORE,			// BOSEV35_DEST_FUNCTION_TV_MORE		 	41
		IDS_BOSELF_TV_WIDE,			// BOSEV35_DEST_FUNCTION_TV_WIDE			42
		IDS_BOSELF_TV_POWER,		// BOSEV35_DEST_FUNCTION_TV_POWER			43
		IDS_BOSELF_TV_INPUT,		// BOSEV35_DEST_FUNCTION_TV_INPUT			44
		-1,							// BOSEV35_DEST_FUNCTION_ON					45
		-1,							// BOSEV35_DEST_FUNCTION_OFF				46
	};


	int resourceID = -1;
	CString szReturn;

	if (func >= 0 && func < BOSEV35_MAX_BINDINGFLAG_FUNCTIONS)
	{
		resourceID = _mappingResourceID[ func ]; 	
	}

	if (resourceID != -1)
	{
		szReturn.LoadString( resourceID );
	}
	
	if (szReturn.IsEmpty())
	{
		//szReturn = "Undefined";
		szReturn.Format("func %d",func);
	}		

	return szReturn;
}

CString CBindingBoseV35Unit::Get_strFunctionTK(int func)
{
	static const int _mappingResourceID[ BOSEV35_MAX_BINDINGFLAG_FUNCTIONS ] =
	{
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY1         0
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY2         1
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY3         2
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY4         3
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY5         4
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY6         5
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY7         6
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY8         7
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY9         8
		-1,						// BOSEV35_SOURCE_FUNCTION_KEY0         9	
		-1,						// BOSEV35_SOURCE_FUNCTION_KPUP     	10
		-1,						// BOSEV35_SOURCE_FUNCTION_KPDOWN   	11
		-1,						// BOSEV35_SOURCE_FUNCTION_KPLEFT   	12
		-1,						// BOSEV35_SOURCE_FUNCTION_KPRIGHT  	13
		-1,						// BOSEV35_SOURCE_FUNCTION_KPENTER 		14
		-1,						// BOSEV35_SOURCE_FUNCTION_TIENEEN  	15
		-1,						// BOSEV35_SOURCE_FUNCTION_FREE1_MENU 	16		/* MENU */
		-1,						// BOSEV35_SOURCE_FUNCTION_FREE2_GUIDE	17		/* GUIDE */
		-1,						// BOSEV35_SOURCE_FUNCTION_FREE3_INFO 	18		/* INFO */
		-1,						// BOSEV35_SOURCE_FUNCTION_FREE4_EXIT 	19		/* EXIT */
		-1,						// BOSEV35_SOURCE_FUNCTION_FREE5    	20
		-1,						// BOSEV35_SOURCE_FUNCTION_UP1      	21
		-1,						// BOSEV35_SOURCE_FUNCTION_DOWN1    	22 
		-1,						// BOSEV35_SOURCE_FUNCTION_UP2      	23
		-1,						// BOSEV35_SOURCE_FUNCTION_DOWN2    	24	
		IDS_BOSELF_PLAY,		// BOSEV35_SOURCE_FUNCTION_PLAY			25
		IDS_BOSELF_PAUSE,		// BOSEV35_SOURCE_FUNCTION_PAUSE		26
		IDS_BOSELF_DEC,			// BOSEV35_SOURCE_FUNCTION_DEC			27 - Channel-
		IDS_BOSELF_INC,			// BOSEV35_SOURCE_FUNCTION_INC			28 - Channel+
		IDS_BOSELF_PREV,		// BOSEV35_SOURCE_FUNCTION_PREV			31 - prev
		IDS_BOSELF_NEXT,		// BOSEV35_SOURCE_FUNCTION_NEXT			32 - next
		IDS_BOSELF_STOP,		// BOSEV35_SOURCE_FUNCTION_STOP			33		
		-1,						// BOSEV35_SOURCE_FUNCTION_FREV			34
		-1,						// BOSEV35_SOURCE_FUNCTION_FFWD			35
		-1,						// BOSEV35_SOURCE_FUNCTION_PAGEUP		36
		-1,						// BOSEV35_SOURCE_FUNCTION_PAGEDOWN		37
		-1,						// BOSEV35_SOURCE_FUNCTION_RECORD		38
		-1,						// BOSEV35_SOURCE_FUNCTION_SHUFFLE		39
		-1,						// BOSEV35_SOURCE_FUNCTION_ON			38
		-1,						// BOSEV35_SOURCE_FUNCTION_OFF			39
		-1,						// BOSEV35_SOURCE_FUNCTION_LAST_CHANNEL 	40
		-1,						// BOSEV35_DEST_FUNCTION_TV_MORE		 	41
		-1,						// BOSEV35_DEST_FUNCTION_TV_WIDE			42
		-1,						// BOSEV35_DEST_FUNCTION_TV_POWER			43
		-1,						// BOSEV35_DEST_FUNCTION_TV_INPUT			44
		-1,						// BOSEV35_DEST_FUNCTION_ON					45
		-1,						// BOSEV35_DEST_FUNCTION_OFF				46
	};



	int resourceID = -1;
	CString szReturn;

	if (func >= 0 && func < BOSEV35_MAX_BINDINGFLAG_FUNCTIONS)
	{
		resourceID = _mappingResourceID[ func ]; 	
	}

	if (resourceID != -1)
	{
		szReturn.LoadString( resourceID );
	}
	
	if (szReturn.IsEmpty())
	{
		szReturn = "Undefined";
	}		

	return szReturn;
}


// -------------------------- HELPER -------------------------------------
BYTE CBindingBoseV35Unit::ConvertFromBindingTable(BYTE Src,BYTE func)
{
	BYTE bReturn = 0xFF;

	if (Src < BOSEV35_MAX_SOURCES &&
		func < BOSEV35_MAX_BINDINGFLAG_FUNCTIONS) 
	{
		bReturn = bindingFlagTabel[func][Src];						
	}
	return bReturn;
}

BYTE CBindingBoseV35Unit::GetSourceFromBindingTable(BYTE flags)
{
	BYTE src,func;

	for (func = 0; func < BOSEV35_MAX_BINDINGFLAG_FUNCTIONS; func++) 
	{
		for (src = 0; src < BOSEV35_MAX_SOURCES; src++) 
		{				
			if (bindingFlagTabel[func][src] == flags) 
			{
				return src;
			}		
		}	
	}
	return 0xFF;
}

BYTE CBindingBoseV35Unit::GetFunctionFromBindingTable(BYTE flags)
{
	BYTE src,func;

	for (func = 0; func < BOSEV35_MAX_BINDINGFLAG_FUNCTIONS; func++) 
	{
		for (src = 0; src < BOSEV35_MAX_SOURCES; src++) 
		{			
			if (bindingFlagTabel[func][src] == flags) 
			{			
				return func;			
			}		
		}	
	}
	return 0xFF;
}

/**************************************************************************************************/
/******************* CBindingBoseV35Unit_Helper::CIteratorBindingFunctions ***********************/
/**************************************************************************************************/
void CBindingBoseV35Unit_Helper::CIteratorBindingFunctions::
incSource( )
{
	// Bron wordt enkel verhoogd indien er een bron gevonden wordt die ook deze
	// functie implementeert.

	int temp = m_source;

	while (1)
	{		
		if (++temp >= BOSEV35_MAX_SOURCES)
			break;

		if (CBindingBoseV35Unit::bindingFlagTabel[m_function][temp] != 0xFF)
		{
			m_source = temp;					
			break;
		}
	}		
}

void CBindingBoseV35Unit_Helper::CIteratorBindingFunctions::
decSource( )
{
	// Bron wordt enkel verlaagd indien er een bron gevonden wordt die ook deze
	// functie implementeert.


	int temp = m_source;

	while (1)
	{		
		if (--temp < 0)
			break;

		if (CBindingBoseV35Unit::bindingFlagTabel[m_function][temp] != 0xFF)
		{
			m_source = temp;					
			break;
		}
	}		
}

void CBindingBoseV35Unit_Helper::CIteratorBindingFunctions::
beginFunction( )
{	
	// zoekt de eerste functie voor de huidige bron.

	for (m_function=0;m_function<CBindingBoseV35Unit_Helper::MAX_FUNCTIONS;m_function++)
	{
		if (CBindingBoseV35Unit::bindingFlagTabel[m_function][m_source] != 0xFF)
		{		
			break;
		}
	}
}


void CBindingBoseV35Unit_Helper::CIteratorBindingFunctions::
incFunction( )
{
	// Functie wordt enkel verhoogd indien er een functie is voor deze bron

	int temp = m_function;

	while(1)
	{
		if (++temp >= MAX_FUNCTIONS)
			break;

		if (CBindingBoseV35Unit::bindingFlagTabel[temp][m_source] != 0xFF)
		{			
			m_function = temp;
			break;
		}
	}
}

void CBindingBoseV35Unit_Helper::CIteratorBindingFunctions::
decFunction( )
{
	// Functie wordt enkel verlaagd indien er een functie is voor deze bron

	int temp = m_function;

	while(1)
	{
		if (--temp <0)
			break;

		if (CBindingBoseV35Unit::bindingFlagTabel[temp][m_source] != 0xFF)
		{			
			m_function = temp;
			break;
		}
	}
}

CString CBindingBoseV35Unit_Helper::CIteratorBindingFunctions::
getString( )
{
	CString szReturn;
	szReturn.Format("Source=%d, function=%d", m_source, m_function);
	return szReturn;
}

/////////////////////////////////////////////////////////////////////////////

bool CBindingBoseV35Unit_Helper::create_Iterator(
	int source,
	int function,
	CIteratorBindingFunctions** pIterator)
{
	// Een iterator aanmaken om over de binding vlaggen te itereren.

	CIteratorBindingFunctions* pReturn = 0;

	if (source < 0)
	{
		source = 0;
	}
	else if (source >= BOSEV35_MAX_SOURCES)
	{
		source = BOSEV35_MAX_SOURCES-1;
	}

	if (function < 0)
	{
		function = 0;
	}
	else if (function >= MAX_FUNCTIONS)
	{
		function = MAX_FUNCTIONS-1;
	}

	if (CBindingBoseV35Unit::bindingFlagTabel[function][source] != 0xFF)
	{
		*pIterator = new CIteratorBindingFunctions(source, function);
		return true;
	}
	else
	{
		int i = 0;

		for (i=0;i<MAX_FUNCTIONS;i++)
		{
			if (CBindingBoseV35Unit::bindingFlagTabel[i][source] != 0xFF)
			{
				*pIterator = new CIteratorBindingFunctions(source, i);	
				return true;
			}
		}	

		for (i=0;i<BOSEV35_MAX_SOURCES;i++)
		{
			if (CBindingBoseV35Unit::bindingFlagTabel[function][i] != 0xFF)
			{
				*pIterator = new CIteratorBindingFunctions(i,function);	
				return true;
			}
		}		

		for (function=0;function<MAX_FUNCTIONS;function++)
		{
			for (source=0;source<BOSEV35_MAX_SOURCES;source++)
			{
				if (CBindingBoseV35Unit::bindingFlagTabel[function][source] != 0xFF)
				{
					*pIterator = new CIteratorBindingFunctions(source, function);	
					return true;
				}	
			}
		}
	}

	return false;
}

void CBindingBoseV35Unit_Helper::
delete_Iterator(CIteratorBindingFunctions* pIterator)
{
	if (pIterator)
	{
		delete pIterator;
		pIterator = 0;
	}
}

#ifdef _DEBUG

	int CBindingBoseV35Unit_Helper::
	testDriver( )
	{
		// Test scenario enkel in debug mode !!!

		for (int source=0;source<0xFF;source++)
		{
			for (int func=0;func<0xFF;func++)
			{
				CIteratorBindingFunctions* pIterator = 0;
				
				if (create_Iterator(source,func,&pIterator))
				{
					TRACE("Iterator create: (%d,%d): %s\n",source,func,pIterator->getString( ));

					delete_Iterator( pIterator );
				}
				else
				{
					// Er moet altijd een iterator aangemaakt worden...
					ASSERT( 0 );
					return -1;
				}
			}
		}

		return 0;
	}

#endif