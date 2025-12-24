#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitboself48.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??
#include "bindingunitstate.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"


using namespace DUOTECNO::BINDINGS::UI;





const BYTE CBindingBoseLf48Unit::BoseLF48BindingFlagTabel[MAX_BOSE48_FUNCTIONS][MAX_BOSE48_SOURCES] = 
{
/* 			   FM,	DVD,	Aux,	Tv,		Vcr,	Sat, 	Stored,	Local */
/* KP1 */  	{ 0xFF,	0xFF,	0x00,	0x0a,	0x14,	0x1e,	0xFF,	0x28 	},	/*00=BOSERS_SOURCE_FUNCTION_KEY1*/
/* KP2 */	{ 0xFF,	0xFF,	0x01,	0x0b,	0x15,	0x1f,	0xFF,	0x29	},	/*01=BOSERS_SOURCE_FUNCTION_KEY2*/
/* KP3 */	{ 0xFF,	0xFF,	0x02,	0x0c,	0x16,	0x20,	0xFF,	0x2a	},	/*02=BOSERS_SOURCE_FUNCTION_KEY3*/
/* KP4 */	{ 0xFF,	0xFF,	0x03,	0x0d,	0x17,	0x21,	0xFF,	0x2b  	},	/*03=BOSERS_SOURCE_FUNCTION_KEY4*/
/* KP5 */	{ 0xFF,	0xFF,	0x04,	0x0e,	0x18,	0x22,	0xFF,	0x2c	},	/*04=BOSERS_SOURCE_FUNCTION_KEY5*/
/* KP6 */	{ 0xFF,	0xFF,	0x05,	0x0f,	0x19,	0x23,	0xFF,	0x2d	},	/*05=BOSERS_SOURCE_FUNCTION_KEY6*/
/* KP7 */	{ 0xFF,	0xFF,	0x06,	0x10,	0x1A,	0x24,	0xFF,	0x2e	},	/*06=BOSERS_SOURCE_FUNCTION_KEY7*/
/* KP8 */	{ 0xFF,	0xFF,	0x07,	0x11,	0x1B,	0x25,	0xFF,	0x2f	},	/*07=BOSERS_SOURCE_FUNCTION_KEY8*/
/* KP9 */	{ 0xFF,	0xFF,	0x08,	0x12,	0x1C,	0x26,	0xFF,	0x30	},	/*08=BOSERS_SOURCE_FUNCTION_KEY9*/
/* KP0 */	{ 0xFF,	0xFF,	0x09,	0x13,	0x1D,	0x27,	0xFF,	0x31	},	/*09=BOSERS_SOURCE_FUNCTION_KEY0*/
/* 			   FM,	DVD,	Aux,	Tv,		Vcr,	 	Sat, 	Stored,	Local */
/* OP  */	{ 0xFF,	0xFF,	0x32,	0x37,	0x3c,	0x41,	0xFF,	0xFF	},	/*0A=BOSERS_SOURCE_FUNCTION_KPUP*/
/* Down*/	{ 0xFF,	0xFF,	0x33,	0x38,	0x3d,	0x42,	0xFF,	0xFF	},	/*0B=BOSERS_SOURCE_FUNCTION_KPDOWN*/
/* Left*/	{ 0xFF,	0xFF,	0x34,	0x39,	0x3e,	0x43,	0xFF,	0xFF  	},	/*0C=BOSERS_SOURCE_FUNCTION_KPLEFT*/
/* Right*/	{ 0xFF,	0xFF,	0x35,	0x3a,	0x3f,	0x44,	0xFF,	0xFF	},	/*0D=BOSERS_SOURCE_FUNCTION_KPRIGHT/
/* Enter*/	{ 0xFF,	0xFF,	0x36,	0x3b,	0x40,	0x45,	0xFF,	0xFF	},	/*0E=BOSERS_SOURCE_FUNCTION_KPENTER*/
/* 		*/ 	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF 	},	/*0F=BOSERS_SOURCE_FUNCTION_TIENEEN*/
/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*10=BOSERS_SOURCE_FUNCTION_FREE1*/
/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*11=BOSERS_SOURCE_FUNCTION_FREE2*/
/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*12=BOSERS_SOURCE_FUNCTION_FREE3*/
/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*13=BOSERS_SOURCE_FUNCTION_FREE4*/
/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*14=BOSERS_SOURCE_FUNCTION_FREE5*/
/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*15=BOSERS_SOURCE_FUNCTION_UP1*/
/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*16=BOSERS_SOURCE_FUNCTION_DOWN1*/
/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*17=BOSERS_SOURCE_FUNCTION_UP2*/
/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*18=BOSERS_SOURCE_FUNCTION_DOWN2*/
/* 			   FM,	DVD,	Aux,	Tv,		Vcr,	 	Sat, 	Stored,	Local */
/* Play */	{ 0xFF,	0xFF,	0x48,	0x51,	0x5a,	0x63,	0xFF,	0x6e 	},	/*19=BOSERS_SOURCE_FUNCTION_PLAY*/
/* Pause */ { 0xFF,	0xFF,	0x49,	0x52,	0x5b,	0x64,	0xFF,	0x6f 	},	/*1a=BOSERS_SOURCE_FUNCTION_PAUSE*/
/* Left */	{ 0xFF,	0xFF,	0x4A,	0x53,	0x5c,	0x65,	0xFF,	0x70 	},	/*1b=BOSERS_SOURCE_FUNCTION_DEC*/
/* Right */	{ 0xFF,	0xFF,	0x4B,	0x54,	0x5d,	0x66,	0xFF,	0x71 	},	/*1c=BOSERS_SOURCE_FUNCTION_INC*/
/* On */	{ 0xFF,	0x46,	0x4C,	0x55,	0x5e,	0x67,	0x6c,	0x72	},	/*1d=BOSERS_SOURCE_FUNCTION_ON*/
/* Off */	{ 0xFF,	0x47,	0x4D,	0x56,	0x5f,	0x68,	0x6d,	0x73 	},	/*1e=BOSERS_SOURCE_FUNCTION_OFF*/
/* Prev */	{ 0xFF,	0xFF,	0x4E,	0x57,	0x60,	0x69,	0xFF,	0x74 	},	/*1f=BOSERS_SOURCE_FUNCTION_PREV*/
/* Next */	{ 0xFF,	0xFF,	0x4F,	0x58,	0x61,	0x6a,	0xFF,	0x75 	},	/*20=BOSERS_SOURCE_FUNCTION_NEXT*/
/* Stop */	{ 0xFF,	0xFF,	0x50,	0x59,	0x62,	0x6b,	0xFF,	0x76 	},	/*21=BOSERS_SOURCE_FUNCTION_STOP*/
};

CBindingBoseLf48Unit::CBindingBoseLf48Unit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{	
	SetInOutType(UNIT_TYPE_INPUT);
	Init();	
}

CBindingBoseLf48Unit::CBindingBoseLf48Unit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();	
}

CBindingBoseLf48Unit::~CBindingBoseLf48Unit()
{

}

void CBindingBoseLf48Unit::Init()
{
	CBindingBoseLF48UnitState* const pUnitState = 
		CBindingUnitState::Instance()->get_BoseLF48Unit();

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
	}
	else
	{
		Set_Source(BOSELF48_SOURCE_AUX);	
		Set_Button(BOSELF48_SOURCE_FUNCTION_KEY1);	
		Set_ControlEvent(EV_UNITCONTROLPULS);
		Set_TKFunction(BOSELF48_SOURCE_FUNCTION_PLAY);
		Set_Event(BOSELF48_EVENT_KEYPAD); 
		Set_State(0);
		Set_Function(AUDIO_OBJECT_ATTR_SOURCE_ON);
		Set_SrcFunction(0);
		Set_DestFunction(0);
	}
	

	Set_ControlState(FALSE);
}

BOOL CBindingBoseLf48Unit::UpgradeIoType()
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


BOOL CBindingBoseLf48Unit::ConvertToBindingData(void)
{
	BYTE bMethodData;	
	BYTE bEvent,bBindingFlags;		/* bFunction; */
	
	BindingData.Clear();

	switch(GetInOutType())
	{
		case UNIT_TYPE_INPUT:
		{
			bEvent = Get_Event();		

			switch (bEvent)
			{
				case BOSELF48_EVENT_DESTOFF	:
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);
					BindingData.Set_BindingFlags(BOSELF48_ADDITIONAL_DESTINATION_OFF);
					
					CBindingUnitState::Instance()->get_BoseLF48Unit()->set_Event(bEvent);
				
				} break;
			
				case BOSELF48_EVENT_DESTON :
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);
					BindingData.Set_BindingFlags(BOSELF48_ADDITIONAL_DESTINATION_ON);

					CBindingUnitState::Instance()->get_BoseLF48Unit()->set_Event(bEvent);

				} break;

				case BOSELF48_EVENT_SOURCEOFF :
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);

					bBindingFlags = ConvertFromBindingTable(Get_Source(),BOSELF48_SOURCE_FUNCTION_OFF);														
					BindingData.Set_BindingFlags(bBindingFlags);	
					
					CBindingBoseLF48UnitState* const pUnitState = 
						CBindingUnitState::Instance()->get_BoseLF48Unit();

					if (pUnitState != 0)
					{
						pUnitState->set_Event(bEvent);
						pUnitState->set_Source(Get_Source());
					}
								
				} break;

				case BOSELF48_EVENT_SOURCEON :
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);
					
					bBindingFlags = ConvertFromBindingTable(Get_Source(),BOSELF48_SOURCE_FUNCTION_ON);								
					BindingData.Set_BindingFlags(bBindingFlags);			
					
					CBindingBoseLF48UnitState* const pUnitState = 
						CBindingUnitState::Instance()->get_BoseLF48Unit();

					if (pUnitState != 0)
					{
						pUnitState->set_Event(bEvent);
						pUnitState->set_Source(Get_Source());
					}

				} break;

/* FUNCTIES LEFT,RIGHT, UP, DN , ENTER !! */

				case BOSELF48_EVENT_KEYPAD :
				{
					// Can be Ev 01 - Ev 03 ...
					BindingData.Set_MessageCode(Get_ControlEvent());
					bBindingFlags = ConvertFromBindingTable(Get_Source(),Get_Button());
					
					if ( bBindingFlags != 0xFF ) 
					{
						if (Get_ControlState()) 
							bBindingFlags |= 0x80;

						BindingData.Set_BindingFlags(bBindingFlags);	

						CBindingBoseLF48UnitState* const pUnitState = 
							CBindingUnitState::Instance()->get_BoseLF48Unit();

						if (pUnitState != 0)
						{
							pUnitState->set_Button(Get_Button());
							pUnitState->set_Source(Get_Source());
							pUnitState->set_ControlEvent(Get_ControlEvent());
							pUnitState->set_Event(bEvent);
						}
					}		

				} break;

				case BOSELF48_EVENT_TPKEYS:
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);
					
					bBindingFlags = ConvertFromBindingTable(Get_Source(),Get_TKFunction());
				
					if ( bBindingFlags != 0xFF ) 
					{
						BindingData.Set_BindingFlags(bBindingFlags);

						CBindingBoseLF48UnitState* const pUnitState = 
							CBindingUnitState::Instance()->get_BoseLF48Unit();

						if (pUnitState != 0)
						{
							pUnitState->set_Source(Get_Source());
							pUnitState->set_FunctionTK(Get_TKFunction());
							pUnitState->set_Event(bEvent);
						}
					}

				} break;

				case BOSELF48_EVENT_INPUTOFF:
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);			
					BindingData.Set_BindingFlags(BOSELF48_FUNCTION_TVINPUT_OFF);

					CBindingUnitState::Instance()->get_BoseLF48Unit()->set_Event(bEvent);

				} break;

				case BOSELF48_EVENT_INPUTON:
				{
					BindingData.Set_MessageCode(EV_UNITCONTROLPULS);					
					BindingData.Set_BindingFlags(BOSELF48_FUNCTION_TVINPUT_ON);

					CBindingUnitState::Instance()->get_BoseLF48Unit()->set_Event(bEvent);

				} break;


				default:
					ASSERT(0);
				break;
			}

		} break;

		case UNIT_TYPE_OUTPUT:
		{
			// TM 20051108
			if (bUnitAddress == BROADCAST_UNIT) 
			{
				AfxMessageBox("Bose all Rooms unit can only be used as input");
				return FALSE;
			}

			BindingData.Set_MessageCode(FC_UNITAUDIOSET);
			bMethodData = Get_Function();
			BindingData.Set_MethodData(bMethodData);

			switch(bMethodData)
			{
				case AUDIO_OBJECT_USE_SELECTOR_DATA :
				case AUDIO_OBJECT_ATTR_STATE:
				return FALSE;

				case AUDIO_OBJECT_ATTR_SOURCE_ON:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_Source());
						
					CBindingBoseLF48UnitState* const pUnitState = 
						CBindingUnitState::Instance()->get_BoseLF48Unit();

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);
						pUnitState->set_Source(Get_Source());
					}
				} return TRUE;

				case AUDIO_OBJECT_METHOD_SOURCEINC:
				case AUDIO_OBJECT_METHOD_SOURCEDEC:
				case AUDIO_OBJECT_METHOD_CSON:
				case AUDIO_OBJECT_METHOD_OFF:
				case AUDIO_OBJECT_METHOD_FSCSONOFF:
				case AUDIO_OBJECT_METHOD_VOLUP:
				case AUDIO_OBJECT_METHOD_VOLDOWN:
				case AUDIO_OBJECT_METHOD_FSVOLUPDOWN:
				case AUDIO_OBJECT_METHOD_PRESUP:
				case AUDIO_OBJECT_METHOD_PRESDOWN:
				{
					BindingData.Set_Length(1);

					CBindingBoseLF48UnitState* const pUnitState = 
						CBindingUnitState::Instance()->get_BoseLF48Unit();

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);						
					}

				} return TRUE;

				case AUDIO_OBJECT_ATTR_ONOFF:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_State() & 0x01);

					CBindingBoseLF48UnitState* const pUnitState = 
						CBindingUnitState::Instance()->get_BoseLF48Unit();

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);						
						pUnitState->set_State(Get_State() & 0x01);
					}

				} return TRUE;

/*
				case AUDIO_OBJECT_ATTR_CONFIG:
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_Config());
				return TRUE;
*/

				case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_SrcFunction() & 0x01);

					CBindingBoseLF48UnitState* const pUnitState = 
						CBindingUnitState::Instance()->get_BoseLF48Unit();

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);						
						pUnitState->set_FunctionSrc(Get_SrcFunction() & 0x01);
					}

				} return TRUE;

				case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_DestFunction() & 0x01);

					CBindingBoseLF48UnitState* const pUnitState = 
						CBindingUnitState::Instance()->get_BoseLF48Unit();

					if (pUnitState != 0)
					{
						pUnitState->set_Function(bMethodData);						
						pUnitState->set_FunctionDest(Get_DestFunction() & 0x01);
					}

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


BOOL CBindingBoseLf48Unit::ConvertFromBindingData(void)
{
	Init();		//TM 20040701

	BYTE bTemp;
	BYTE bMessageCode = BindingData.Get_MessageCode();
	
	if (GetInOutType() == UNIT_TYPE_INPUT) { 
		
		BYTE bBindingFlags = BindingData.Get_BindingFlags() & 0x7F;	
	
		if (bBindingFlags == BOSELF48_ADDITIONAL_DESTINATION_OFF) {
			Set_Event(BOSELF48_EVENT_DESTOFF);
		}
		else if (bBindingFlags == BOSELF48_ADDITIONAL_DESTINATION_ON) {
			Set_Event(BOSELF48_EVENT_DESTON);				
		}
		else if (bBindingFlags == BOSELF48_FUNCTION_TVINPUT_OFF) {
			Set_Event(BOSELF48_EVENT_INPUTOFF);				
		}
		else if (bBindingFlags == BOSELF48_FUNCTION_TVINPUT_ON) {
			Set_Event(BOSELF48_EVENT_INPUTON);				
		}
		else 
		{
			bTemp = GetSourceFromBindingTable(bBindingFlags);

			if (bTemp != 0xFF) 
			{
				Set_Source(bTemp);

				bTemp = GetFunctionFromBindingTable(bBindingFlags);
				if (bTemp != 0xFF) 
				{
				
					if ((bTemp >= BOSELF48_SOURCE_FUNCTION_KEY1) && 
						(bTemp <= BOSELF48_SOURCE_FUNCTION_KPENTER)) 
					{								
						Set_Event(BOSELF48_EVENT_KEYPAD);
						Set_Button(bTemp);
						Set_ControlEvent(bMessageCode);												
					}
					else if (bTemp == BOSELF48_SOURCE_FUNCTION_ON) 
					{
						Set_Event(BOSELF48_EVENT_SOURCEON);
					}
					else if (bTemp == BOSELF48_SOURCE_FUNCTION_OFF) 
					{
						Set_Event(BOSELF48_EVENT_SOURCEOFF);
					}

					/* On,Off absorbe by Else if above...*/
					else if ((bTemp >= BOSELF48_SOURCE_FUNCTION_PLAY) && 
							 (bTemp <=BOSELF48_SOURCE_FUNCTION_STOP)) 
					{
						Set_Event(BOSELF48_EVENT_TPKEYS);	
						Set_TKFunction(bTemp);											
					}																						
				}									
			}
		}
	}
	else 
	{
		if (bMessageCode != FC_UNITAUDIOSET) {
			ASSERT(0);
			return FALSE;
		}
		
		BYTE bMethodData = BindingData.Get_MethodData();
		Set_Function(bMethodData);
		BYTE bLength = BindingData.Get_Length();

		switch(bMethodData) 
		{
			case AUDIO_OBJECT_USE_SELECTOR_DATA :
			case AUDIO_OBJECT_ATTR_STATE:
			return FALSE;

			case AUDIO_OBJECT_ATTR_SOURCE_ON:
			{
				if (bLength != 2) return FALSE;
				Set_Source(BindingData.Get_Data(1));

			} return TRUE;

			case AUDIO_OBJECT_METHOD_SOURCEINC:
			case AUDIO_OBJECT_METHOD_SOURCEDEC:
			case AUDIO_OBJECT_METHOD_CSON:
			case AUDIO_OBJECT_METHOD_OFF:
			case AUDIO_OBJECT_METHOD_FSCSONOFF:
			case AUDIO_OBJECT_METHOD_VOLUP:
			case AUDIO_OBJECT_METHOD_VOLDOWN:
			case AUDIO_OBJECT_METHOD_FSVOLUPDOWN:
			case AUDIO_OBJECT_METHOD_PRESUP:
			case AUDIO_OBJECT_METHOD_PRESDOWN:
			{
				if(bLength != 1) return FALSE;

			} return TRUE;

			case AUDIO_OBJECT_ATTR_ONOFF:
			{
				if (bLength != 2) return FALSE;
				Set_State(BindingData.Get_Data(1));

			} return TRUE;

			case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
			{
				if (bLength != 2) return FALSE;
				Set_SrcFunction(BindingData.Get_Data(1));

			} return TRUE;

			case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
				if (bLength != 2) return FALSE;
				Set_DestFunction(BindingData.Get_Data(1));
			return TRUE;
/*		
			case AUDIO_OBJECT_ATTR_VOLUME:
				if (bLength != 2) return FALSE;
				Set_Volume(BindingData.Get_Data(1));
			return TRUE;

			case AUDIO_OBJECT_ATTR_CONFIG:
				if (bLength != 2) return FALSE;
				Set_Config(BindingData.Get_Data(1));
			return TRUE;
*/
			default:
				ASSERT(0);
			return FALSE;
		}
		
	}
	return TRUE;
}

CString CBindingBoseLf48Unit::ConvertFuntionToString(void)
{
	enum 
	{ 
		MAX_LF48_BUTTONS				= 15, 
		MAX_LF48_TKFUNCTIONS			= 9
	};

	static const char* _szButtons[MAX_LF48_BUTTONS] =
	{	
		"1",		// 0
		"2",		// 1 
		"3",		// 2 
		"4",		// 3
		"5",		// 4
		"6",		// 5
		"7",		// 6
		"8",		// 7
		"9",		// 8
		"0",		// 9
		"Up",		// 10
		"Down" ,	// 11
		"Left",		// 12
		"Right",	// 13
		"Enter"		// 14
	};

	static const char* _szFunctionsTK[MAX_LF48_TKFUNCTIONS] =
	{
		"Play",			// 0
		"Pause",		// 1
		"Down",			// 2
		"up",			// 3
		"On",			// 4
		"Off",			// 5
		"Left",			// 6
		"Right",		// 7
		"Stop"			// 8
	};

	CString strReturn;
	CString strMethod;	
	CString strFunction;
	CString strData;

	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{ 			
		const BYTE bMethodData = Get_Event();

		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(BOSELF48_INPUTSTRINGS_EXTENDED);

		ASSERT(pStringEntries);
		pStringEntries->Find(bMethodData,strFunction);
		

		if ((bMethodData == BOSELF48_EVENT_DESTOFF) || (bMethodData == BOSELF48_EVENT_DESTON) ||
			(bMethodData == BOSELF48_EVENT_INPUTON) || (bMethodData == BOSELF48_EVENT_INPUTOFF))
		{
			return strFunction;
		}

		switch(bMethodData) 
		{
			case BOSELF48_EVENT_SOURCEOFF:
			case BOSELF48_EVENT_SOURCEON:
			{
				strReturn.Format("%s - %s",strFunction,Get_strSource());

			} return strReturn;

			case BOSELF48_EVENT_KEYPAD:
			{
				if (Get_Button() < MAX_LF48_BUTTONS)
				{
					strReturn.Format("%s - %s - %s - ",
									 strFunction,
									 Get_strSource(),
									 _szButtons[Get_Button()]);
				}
				else
				{
					strReturn.Format("%s - %s - %d - ",
									 strFunction,
									 Get_strSource(),
									 Get_Button());
				}

				if (Get_ControlEvent() == EV_UNITCONTROLPULS) strReturn += "Short";				
				else strReturn += "Long";				

			} return strReturn;

			case BOSELF48_EVENT_TPKEYS:
			{
				const BYTE bTemp = Get_TKFunction() - BOSELF48_SOURCE_FUNCTION_PLAY;

				if (bTemp < MAX_LF48_TKFUNCTIONS)
				{
					strReturn.Format("%s - %s - %s",
									 strFunction,
									 Get_strSource(),
									 _szFunctionsTK[bTemp]);		
				}
				else
				{
					strReturn.Format("%s - %s - %d",
									 strFunction,
									 Get_strSource(),
									 Get_TKFunction());			
				}

			} return strReturn;
		}
	}
	else 
	{
		CString strTemp;	

		const BYTE bMethodData = Get_Function();
		
		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(BOSELF48_STRINGS_EXTENDED);

		ASSERT(pStringEntries);
		pStringEntries->Find(bMethodData,strFunction);

		switch(bMethodData)
		{
			case AUDIO_OBJECT_ATTR_SOURCE_ON:				
			{
				strReturn.Format("%s - %s",strFunction,Get_strSource());				

			} return strReturn;

			case AUDIO_OBJECT_METHOD_SOURCEINC:
			case AUDIO_OBJECT_METHOD_SOURCEDEC:
			{
				strReturn.Format("%s - %s",strFunction,Get_strSource());

			} return strReturn;

			case AUDIO_OBJECT_METHOD_PRESUP:
			case AUDIO_OBJECT_METHOD_PRESDOWN:			
			{
				strReturn.Format("%s - %s",strFunction,Get_strSource());

			} return strReturn;

			case AUDIO_OBJECT_METHOD_CSON:
			case AUDIO_OBJECT_METHOD_OFF:
			case AUDIO_OBJECT_METHOD_FSCSONOFF:
			case AUDIO_OBJECT_METHOD_VOLUP:
			case AUDIO_OBJECT_METHOD_VOLDOWN:
			case AUDIO_OBJECT_METHOD_FSVOLUPDOWN:
				strReturn.Format("%s",strFunction);
			return strReturn;

			case AUDIO_OBJECT_ATTR_ONOFF:
			{
				if (Get_State() & 0x01) strTemp.LoadString(IDS_ON);
				else strTemp.LoadString(IDS_OFF);

				strReturn.Format("%s - %s",strFunction,strTemp);

			} return strReturn;
					
			case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:					
			{
				strTemp.LoadString(IDS_FUNCTION);
				strReturn.Format("%s - %s %d",strFunction,strTemp,
											  Get_DestFunction() & IRAUDIO_FUNCTION_MASK);
			} return strReturn;
							
			case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
			{
				strTemp.LoadString(IDS_FUNCTION);
				strReturn.Format("%s - %s %d",strFunction,strTemp,
											  Get_SrcFunction() & IRAUDIO_FUNCTION_MASK);
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

CString CBindingBoseLf48Unit::Get_strSource(void) const
{
	CString str;
	switch(Get_Source()) 
	{
		case BOSELF48_SOURCE_TUNER: 
			str.LoadString(IDS_BOSELF_TUNER); 
		break;

		case BOSELF48_SOURCE_DVD: 
			str.LoadString(IDS_BOSELF_DVD); 
		break;

		case BOSELF48_SOURCE_AUX: 
			str.LoadString(IDS_BOSELF_AUX); 
		break;

		case BOSELF48_SOURCE_TV: 
			str.LoadString(IDS_BOSELF_TV); 
		break;

		case BOSELF48_SOURCE_VCR: 
			str.LoadString(IDS_BOSELF_VCR); 
		break;

		case BOSELF48_SOURCE_SAT: 
			str.LoadString(IDS_BOSELF_SAT); 
		break;

		case BOSELF48_SOURCE_STORED: 
			str.LoadString(IDS_BOSELF_STORED); 
		break;

		case BOSELF48_SOURCE_LOCAL: 
			str.LoadString(IDS_BOSELF_LOCAL); 
		break;

		default: str.Format("Undefines source"); break;	
	}
	return str;
}

// -------------------------- HELPER -------------------------------------
BYTE CBindingBoseLf48Unit::ConvertFromBindingTable(BYTE Src,BYTE func)
{
	BYTE bReturn = 0xFF;

	if (Src < MAX_BOSE48_SOURCES)
	{
		if (func < MAX_BOSE48_FUNCTIONS) 
		{
			bReturn = BoseLF48BindingFlagTabel[func][Src];					
		}
	}
	return bReturn;
}

BYTE CBindingBoseLf48Unit::GetSourceFromBindingTable(BYTE flags)
{
	BYTE src,func;

	for (func = 0; func < MAX_BOSE48_FUNCTIONS; func++) 
	{
		for (src = 0; src < MAX_BOSE48_SOURCES; src++) 
		{			
			if (BoseLF48BindingFlagTabel[func][src] == flags) 
			{
				if (src < MAX_BOSE48_SOURCES) return src;
			}		
		}	
	}
	return 0xFF;
}

BYTE CBindingBoseLf48Unit::GetFunctionFromBindingTable(BYTE flags)
{
	BYTE src,func;

	for (func = 0; func < MAX_BOSE48_FUNCTIONS; func++) 
	{
		for (src = 0; src < MAX_BOSE48_SOURCES; src++) 
		{			
			if (BoseLF48BindingFlagTabel[func][src] == flags) 
			{
				if (func < MAX_BOSE48_FUNCTIONS) return func;
			}		
		}	
	}
	return 0xFF;
}

