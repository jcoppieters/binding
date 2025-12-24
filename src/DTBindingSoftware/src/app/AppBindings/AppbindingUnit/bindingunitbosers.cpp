#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitbosers.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;




CBindingBoseRsUnit::CBindingBoseRsUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{
	SetInOutType(UNIT_TYPE_INPUT);

	Init();		//TM 20040701
}

CBindingBoseRsUnit::CBindingBoseRsUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();	
}

CBindingBoseRsUnit::~CBindingBoseRsUnit()
{

}

void CBindingBoseRsUnit::Init(void)			// TM 20040701
{
	Set_Event(BOSELF50_EVENT_SOURCEON);
	Set_Function(AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN);
	Set_Source(BOSELF50_SOURCE_AUX);
	Set_State(0);
	Set_Config(0);
	Set_SrcFunction(0);
	Set_TKFunction(BOSELF50_TK_FUNCTION_PLAY);
	Set_DestFunction(0);
	Set_KeyPad(BOSELF50_KP_PRESET_AUX);
	Set_Button(0);
	Set_Volume(BOSELF50_MAX_VOLUME/2);
}


BOOL CBindingBoseRsUnit::UpgradeIoType()
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


#define BROADCAST_UNIT		0xFF

BOOL CBindingBoseRsUnit::ConvertToBindingData(void)
{
	BYTE bMethodData;	
	BYTE bEvent,bBindingFlags,bFunction;
	BindingData.Clear();
	switch(GetInOutType())
	{
		case UNIT_TYPE_INPUT:
			bEvent = Get_Event();
			BindingData.Set_MessageCode(EV_UNITCONTROLPULS);
			if (bUnitAddress == BROADCAST_UNIT)
			{
				switch(bEvent)
				{
					case BOSELF50_EVENT_SOURCEON:
					{
						switch(Get_Source()) 
						{
							case BOSELF50_SOURCE_AUX:	 BindingData.Set_BindingFlags(0x55); return TRUE;
							case BOSELF50_SOURCE_VIDEO1: BindingData.Set_BindingFlags(0x5C); return TRUE;
							case BOSELF50_SOURCE_VIDEO2: BindingData.Set_BindingFlags(0x63); return TRUE;
							case BOSELF50_SOURCE_TAPE:	 BindingData.Set_BindingFlags(0x6A); return TRUE;
							default : ASSERT(0); return FALSE;
						}
					} return TRUE;

					case BOSELF50_EVENT_SOURCEOFF: 
					{
						switch(Get_Source()) 
						{
							case BOSELF50_SOURCE_AUX:	 BindingData.Set_BindingFlags(0x56); return TRUE;
							case BOSELF50_SOURCE_VIDEO1: BindingData.Set_BindingFlags(0x5D); return TRUE;
							case BOSELF50_SOURCE_VIDEO2: BindingData.Set_BindingFlags(0x64); return TRUE;
							case BOSELF50_SOURCE_TAPE:	 BindingData.Set_BindingFlags(0x6B); return TRUE;
							default : ASSERT(0); return FALSE;
						}						
					} return TRUE;			
			
					case BOSELF50_EVENT_TUNE_AUX: BindingData.Set_BindingFlags(0x3c + Get_Button());
					return TRUE;

					case BOSELF50_EVENT_PRESET_AUX: BindingData.Set_BindingFlags(0x46 + Get_Button());
					return TRUE;
				
					case BOSELF50_EVENT_PRESET_VIDEO1: BindingData.Set_BindingFlags(0x0A + Get_Button());
					return TRUE;

					case BOSELF50_EVENT_TUNE_VIDEO1: BindingData.Set_BindingFlags(0x00 + Get_Button());
					return TRUE;

					case BOSELF50_EVENT_PRESET_VIDEO2: BindingData.Set_BindingFlags(0x1E + Get_Button());
					return TRUE;

					case BOSELF50_EVENT_TUNE_VIDEO2: BindingData.Set_BindingFlags(0x14 + Get_Button());
					return TRUE;

					case BOSELF50_EVENT_PRESET_TAPE: BindingData.Set_BindingFlags(0x32 + Get_Button());
					return TRUE;

					case BOSELF50_EVENT_TUNE_TAPE: BindingData.Set_BindingFlags(0x28 + Get_Button());
					return TRUE;

					case BOSELF50_EVENT_TRANSPORT_AUX:
					{
						bFunction =  Get_TKFunction();

						switch(bFunction)
						{
							case BOSELF50_TK_FUNCTION_PLAY: 
								bBindingFlags = 0x50; 
							break;
							case BOSELF50_TK_FUNCTION_STOP: 
								bBindingFlags = 0x79; 
							break;
							case BOSELF50_TK_FUNCTION_PAUZE:
								bBindingFlags = 0x51; 
							break;
							case BOSELF50_TK_FUNCTION_LEFT: 
								bBindingFlags = 0x53;	
							break;
							case BOSELF50_TK_FUNCTION_RIGHT: 
								bBindingFlags = 0x54; 
							break;
							case BOSELF50_TK_FUNCTION_PREV: 
								bBindingFlags = 0x77; 
							break;
							case BOSELF50_TK_FUNCTION_NEXT: 
								bBindingFlags = 0x78; 
							break;
							default: 
								bBindingFlags = 0xFF;				// CHANGES_MVS_2008_WARNINGS
							break;
						}

						ASSERT( bBindingFlags != 0xFF );			// CHANGES_MVS_2008_WARNINGS
						BindingData.Set_BindingFlags(bBindingFlags);

					} return TRUE;

					case BOSELF50_EVENT_TRANSPORT_VIDEO1:
						bFunction =  Get_TKFunction();
						switch(bFunction)
						{
							case BOSELF50_TK_FUNCTION_PLAY: bBindingFlags = 0x57; break;
							case BOSELF50_TK_FUNCTION_STOP: bBindingFlags = 0x70; break;
							case BOSELF50_TK_FUNCTION_PAUZE:bBindingFlags = 0x58; break;
							case BOSELF50_TK_FUNCTION_LEFT: bBindingFlags = 0x5A;	break;
							case BOSELF50_TK_FUNCTION_RIGHT: bBindingFlags = 0x5B; break;
							case BOSELF50_TK_FUNCTION_PREV: bBindingFlags = 0x6E; break;
							case BOSELF50_TK_FUNCTION_NEXT: bBindingFlags = 0x6F; break;
						}
						BindingData.Set_BindingFlags(bBindingFlags);
					return TRUE;

					case BOSELF50_EVENT_TRANSPORT_VIDEO2:
						bFunction =  Get_TKFunction();
						switch(bFunction)
						{
							case BOSELF50_TK_FUNCTION_PLAY: bBindingFlags = 0x5E; break;
							case BOSELF50_TK_FUNCTION_STOP: bBindingFlags = 0x73; break;
							case BOSELF50_TK_FUNCTION_PAUZE:bBindingFlags = 0x5F; break;
							case BOSELF50_TK_FUNCTION_LEFT: bBindingFlags = 0x61;	break;
							case BOSELF50_TK_FUNCTION_RIGHT: bBindingFlags = 0x62; break;
							case BOSELF50_TK_FUNCTION_PREV: bBindingFlags = 0x71; break;
							case BOSELF50_TK_FUNCTION_NEXT: bBindingFlags = 0x72; break;
						}
						BindingData.Set_BindingFlags(bBindingFlags);
					return TRUE;

					case BOSELF50_EVENT_TRANSPORT_TAPE:
						bFunction =  Get_TKFunction();
						switch(bFunction)
						{
							case BOSELF50_TK_FUNCTION_PLAY: bBindingFlags = 0x65; break;
							case BOSELF50_TK_FUNCTION_STOP: bBindingFlags = 0x76; break;
							case BOSELF50_TK_FUNCTION_PAUZE:bBindingFlags = 0x66; break;
							case BOSELF50_TK_FUNCTION_LEFT: bBindingFlags = 0x68;	break;
							case BOSELF50_TK_FUNCTION_RIGHT: bBindingFlags = 0x69; break;
							case BOSELF50_TK_FUNCTION_PREV: bBindingFlags = 0x74; break;
							case BOSELF50_TK_FUNCTION_NEXT: bBindingFlags = 0x75; break;
						}
						BindingData.Set_BindingFlags(bBindingFlags);
					return TRUE;

					default:
						ASSERT(0);
					return FALSE;			
				}			
			}
			else
			{
				switch(bEvent)
				{
					case BOSELF50_EVENT_SOURCEON:
						switch(Get_Source()) 
						{
							case BOSELF50_SOURCE_AUX:	BindingData.Set_BindingFlags(0x55); return TRUE;
							case BOSELF50_SOURCE_VIDEO1:	BindingData.Set_BindingFlags(0x5C); return TRUE;
							case BOSELF50_SOURCE_VIDEO2:	BindingData.Set_BindingFlags(0x63); return TRUE;
							case BOSELF50_SOURCE_TAPE:	BindingData.Set_BindingFlags(0x6A); return TRUE;
							default : ASSERT(0); return FALSE;
						}
					return TRUE;

					case BOSELF50_EVENT_SOURCEOFF: 
						switch(Get_Source()) 
						{
							case BOSELF50_SOURCE_AUX:	BindingData.Set_BindingFlags(0x56); return TRUE;
							case BOSELF50_SOURCE_VIDEO1:	BindingData.Set_BindingFlags(0x5D); return TRUE;
							case BOSELF50_SOURCE_VIDEO2:	BindingData.Set_BindingFlags(0x64); return TRUE;
							case BOSELF50_SOURCE_TAPE:	BindingData.Set_BindingFlags(0x6B); return TRUE;
							default : ASSERT(0); return FALSE;
						}						
					return TRUE;	

					case BOSELF50_EVENT_BOSERS_DESTOFF: BindingData.Set_BindingFlags(0x6D);
					return TRUE;

					case BOSELF50_EVENT_BOSERS_DESTON: BindingData.Set_BindingFlags(0x6C);
					return TRUE;

					default: ASSERT(0);
					return FALSE;
				}
			}	
		break;

		case UNIT_TYPE_OUTPUT:
			BindingData.Set_MessageCode(FC_UNITAUDIOSET);
			bMethodData = Get_Function();
			BindingData.Set_MethodData(bMethodData);
			switch(bMethodData)
			{
				case AUDIO_OBJECT_USE_SELECTOR_DATA :
				case AUDIO_OBJECT_ATTR_STATE:
				return FALSE;

				case AUDIO_OBJECT_ATTR_SOURCE_ON:
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_Source());
				return TRUE;

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
					BindingData.Set_Length(1);
				return TRUE;

				case AUDIO_OBJECT_ATTR_ONOFF:
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_State() & 0x01);
				return TRUE;

				case AUDIO_OBJECT_ATTR_CONFIG:
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_Config());
				return TRUE;

				case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_SrcFunction() & 0x01);
				return TRUE;

				case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_DestFunction() & 0x01);
				return TRUE;
		
				case AUDIO_OBJECT_ATTR_VOLUME:
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_Volume());
				return TRUE;

				default:
					ASSERT(0);
				return FALSE;
			}
		break;

		default:
			ASSERT(0);
		return FALSE;
	}
	return FALSE;
}


BOOL CBindingBoseRsUnit::ConvertFromBindingData(void)
{
	Init();		//TM 20040701

	BYTE bMessageCode = BindingData.Get_MessageCode();
	
	if (GetInOutType() == UNIT_TYPE_INPUT) { 
		
		BYTE bBindingFlags = BindingData.Get_BindingFlags() & 0x7F;	
		switch(bBindingFlags) 
		{
			case 0x55: 
				Set_Source(BOSELF50_SOURCE_AUX);
				Set_Event(BOSELF50_EVENT_SOURCEON);
			return TRUE;
			
			case 0x56: 
				Set_Source(BOSELF50_SOURCE_AUX);
				Set_Event(BOSELF50_EVENT_SOURCEOFF);
			return TRUE;

			case 0x5c:
				Set_Source(BOSELF50_SOURCE_VIDEO1);
				Set_Event(BOSELF50_EVENT_SOURCEON);
			return TRUE;

			case 0x5d:
				Set_Source(BOSELF50_SOURCE_VIDEO1);
				Set_Event(BOSELF50_EVENT_SOURCEOFF);
			return TRUE;

			case 0x63:
				Set_Source(BOSELF50_SOURCE_VIDEO2);
				Set_Event(BOSELF50_EVENT_SOURCEON);
			return TRUE;

			case 0x64:
				Set_Source(BOSELF50_SOURCE_VIDEO2);
				Set_Event(BOSELF50_EVENT_SOURCEOFF);
			return TRUE;

			case 0x6A:
				Set_Source(BOSELF50_SOURCE_TAPE);
				Set_Event(BOSELF50_EVENT_SOURCEON);
			return TRUE;

			case 0x6B:
				Set_Source(BOSELF50_SOURCE_TAPE);
				Set_Event(BOSELF50_EVENT_SOURCEOFF);
			return TRUE;

			case 0x6c:
				if (bUnitAddress != BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_BOSERS_DESTON);
					return TRUE;
				}
			return FALSE;

			case 0x6d:
				if (bUnitAddress != BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_BOSERS_DESTOFF);
					return TRUE;
				}
			return FALSE;

// TRANSPORT KEY FUNCTION AUX
			case 0x50:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_AUX);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PLAY);
					return TRUE;
				}
			return FALSE;

			case 0x51:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_AUX);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PAUZE);
					return TRUE;
				}
			return FALSE;

			case 0x53:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_AUX);
					Set_TKFunction(BOSELF50_TK_FUNCTION_LEFT);
					return TRUE;
				}
			return FALSE;

			case 0x54:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_AUX);
					Set_TKFunction(BOSELF50_TK_FUNCTION_RIGHT);
					return TRUE;
				}
			return FALSE;

			case 0x77:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_AUX);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PREV);
					return TRUE;
				}
			return FALSE;

			case 0x78:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_AUX);
					Set_TKFunction(BOSELF50_TK_FUNCTION_NEXT);
					return TRUE;
				}
			return FALSE;

			case 0x79:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_AUX);
					Set_TKFunction(BOSELF50_TK_FUNCTION_STOP);
					return TRUE;
				}
			return FALSE;
// TRANSPORT KEY FUNCTION VIDEO1
			case 0x57:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO1);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PLAY);
					return TRUE;
				}
			return FALSE;

			case 0x58:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO1);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PAUZE);
					return TRUE;
				}
			return FALSE;

			case 0x5A:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO1);
					Set_TKFunction(BOSELF50_TK_FUNCTION_LEFT);
					return TRUE;
				}
			return FALSE;

			case 0x5B:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO1);
					Set_TKFunction(BOSELF50_TK_FUNCTION_RIGHT);
					return TRUE;
				}
			return FALSE;

			case 0x6E:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO1);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PREV);
					return TRUE;
				}
			return FALSE;

			case 0x6F:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO1);
					Set_TKFunction(BOSELF50_TK_FUNCTION_NEXT);
					return TRUE;
				}
			return FALSE;

			case 0x70:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO1);
					Set_TKFunction(BOSELF50_TK_FUNCTION_STOP);
					return TRUE;
				}
			return FALSE;
// TRANSPORT KEY FUNCTION VIDEO2
			case 0x5E:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO2);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PLAY);
					return TRUE;
				}
			return FALSE;

			case 0x5F:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO2);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PAUZE);
					return TRUE;
				}
			return FALSE;

			case 0x61:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO2);
					Set_TKFunction(BOSELF50_TK_FUNCTION_LEFT);
					return TRUE;
				}
			return FALSE;

			case 0x62:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO2);
					Set_TKFunction(BOSELF50_TK_FUNCTION_RIGHT);
					return TRUE;
				}
			return FALSE;

			case 0x71:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO2);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PREV);
					return TRUE;
				}
			return FALSE;

			case 0x72:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO2);
					Set_TKFunction(BOSELF50_TK_FUNCTION_NEXT);
					return TRUE;
				}
			return FALSE;

			case 0x73:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_VIDEO2);
					Set_TKFunction(BOSELF50_TK_FUNCTION_STOP);
					return TRUE;
				}
			return FALSE;
// TRANSPORT KEY FUNCTION TAPE
			case 0x65:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_TAPE);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PLAY);
					return TRUE;
				}
			return FALSE;

			case 0x66:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_TAPE);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PAUZE);
					return TRUE;
				}
			return FALSE;

			case 0x68:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_TAPE);
					Set_TKFunction(BOSELF50_TK_FUNCTION_LEFT);
					return TRUE;
				}
			return FALSE;

			case 0x69:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_TAPE);
					Set_TKFunction(BOSELF50_TK_FUNCTION_RIGHT);
					return TRUE;
				}
			return FALSE;

			case 0x74:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_TAPE);
					Set_TKFunction(BOSELF50_TK_FUNCTION_PREV);
					return TRUE;
				}
			return FALSE;

			case 0x75:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_TAPE);
					Set_TKFunction(BOSELF50_TK_FUNCTION_NEXT);
					return TRUE;
				}
			return FALSE;

			case 0x76:
				if (bUnitAddress == BROADCAST_UNIT) 
				{
					Set_Event(BOSELF50_EVENT_TRANSPORT_TAPE);
					Set_TKFunction(BOSELF50_TK_FUNCTION_STOP);
					return TRUE;
				}
			return FALSE;
		}	
		
		if ((bBindingFlags >=0x00) && (bBindingFlags <= 0x09))
		{
			Set_Event(BOSELF50_EVENT_TUNE_VIDEO1);
			Set_Button(bBindingFlags);
			return TRUE;
		}

		if ((bBindingFlags >=0x0A) && (bBindingFlags <= 0x13))
		{
			Set_Event(BOSELF50_EVENT_PRESET_VIDEO1);
			Set_Button(bBindingFlags-0x0A);
			return TRUE;
		}

		if ((bBindingFlags >=0x14) && (bBindingFlags <= 0x1D)) 
		{
			Set_Event(BOSELF50_EVENT_TUNE_VIDEO2);
			Set_Button(bBindingFlags-0x14);
			return TRUE;
		}

		if ((bBindingFlags >=0x1E) && (bBindingFlags <= 0x27))
		{
			Set_Event(BOSELF50_EVENT_PRESET_VIDEO2);
			Set_Button(bBindingFlags-0x1E);
			return TRUE;
		}

		if ((bBindingFlags >=0x28) && (bBindingFlags <= 0x31)) 
		{
			Set_Event(BOSELF50_EVENT_TUNE_TAPE);
			Set_Button(bBindingFlags-0x28);
			return TRUE;
		}

		if ((bBindingFlags >=0x32) && (bBindingFlags <= 0x3B))
		{
			Set_Event(BOSELF50_EVENT_PRESET_TAPE);
			Set_Button(bBindingFlags-0x32);
			return TRUE;
		}

		if ((bBindingFlags >=0x3C) && (bBindingFlags <= 0x45))
		{
			Set_Event(BOSELF50_EVENT_TUNE_AUX);
			Set_Button(bBindingFlags-0x3c);
			return TRUE;
		}

		if ((bBindingFlags >=0x46) && (bBindingFlags <= 0x4F))
		{
			Set_Event(BOSELF50_EVENT_PRESET_AUX);
			Set_Button(bBindingFlags-0x46);
			return TRUE;
		}

		return FALSE;
	}
	else {
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
				if (bLength != 2) return FALSE;
				Set_Source(BindingData.Get_Data(1));
			return TRUE;

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
				if(bLength != 1) return FALSE;
			return TRUE;

			case AUDIO_OBJECT_ATTR_ONOFF:
				if (bLength != 2) return FALSE;
				Set_State(BindingData.Get_Data(1));
			return TRUE;

			case AUDIO_OBJECT_ATTR_CONFIG:
				if (bLength != 2) return FALSE;
				Set_Config(BindingData.Get_Data(1));
			return TRUE;

			case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
				if (bLength != 2) return FALSE;
				Set_SrcFunction(BindingData.Get_Data(1));
			return TRUE;

			case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
				if (bLength != 2) return FALSE;
				Set_DestFunction(BindingData.Get_Data(1));
			return TRUE;
		
			case AUDIO_OBJECT_ATTR_VOLUME:
				if (bLength != 2) return FALSE;
				Set_Volume(BindingData.Get_Data(1));
			return TRUE;

			default:
				ASSERT(0);
			return FALSE;
		}
	}

	return FALSE;
}

// NEW ...
CString CBindingBoseRsUnit::Get_strSource(void) const
{
	CString str;
	
	switch(Get_Source()) 
	{
		case 0: str.LoadString(IDS_BOSERS_FM); break;
		case 1: str.LoadString(IDS_BOSERS_CD); break;
		case 2: str.LoadString(IDS_BOSERS_AUX); break;
		case 3: str.LoadString(IDS_BOSERS_VIDEO1); break;
		case 4: str.LoadString(IDS_BOSERS_VIDEO2); break;
		case 5: str.LoadString(IDS_BOSERS_TAPE); break;
		default: str.Format("Undefines source"); break;	
	}
	return str;
}

CString CBindingBoseRsUnit::ConvertFuntionToString(void)
{
	BYTE bMethodData;
	int i;

	CString strReturn;
	CString strFunction;
	CString strTemp;

	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{ 
		if (GetUnitAddress() == BROADCAST_UNIT) 
		{	// INPUT-Virtual

			bMethodData = Get_Event();

			for (i=0;i<MAX_BOSERSVIRTUALINPUT_FUNCTIONS;i++) 
			{
				if (bMethodData == BoseRsVirtualInputStrings[i].bMethodData) {
					strFunction.LoadString(BoseRsVirtualInputStrings[i].ulStringID);
					break;	
				}
			}
			
			if (i>=MAX_BOSERSVIRTUALINPUT_FUNCTIONS) return strReturn;
		}
		else 
		{
			bMethodData = Get_Event();
			for (i=0;i<MAX_BOSERSINPUT_FUNCTIONS;i++) 
			{
				if (bMethodData == BoseRsInputStrings[i].bMethodData) 
				{
					strFunction.LoadString(BoseRsInputStrings[i].ulStringID);
					break;	
				}
			}			

			if (i>=MAX_BOSERSINPUT_FUNCTIONS) return strReturn;		

			if ((bMethodData == BOSELF50_EVENT_BOSERS_DESTOFF) || 
				(bMethodData == BOSELF50_EVENT_BOSERS_DESTON)) 
			{
				return strFunction;
			}
		}

		switch(bMethodData) 
		{
				
			case BOSELF50_EVENT_TUNE_AUX:
			case BOSELF50_EVENT_PRESET_AUX:
			case BOSELF50_EVENT_PRESET_VIDEO1:
			case BOSELF50_EVENT_TUNE_VIDEO1:
			case BOSELF50_EVENT_PRESET_VIDEO2:
			case BOSELF50_EVENT_TUNE_VIDEO2:
			case BOSELF50_EVENT_PRESET_TAPE:
			case BOSELF50_EVENT_TUNE_TAPE:
			{
				strReturn.Format("%s - %d",strFunction,Get_Button());

			} return strReturn;

			case BOSELF50_EVENT_SOURCEON:
			case BOSELF50_EVENT_SOURCEOFF:					
			{
				strReturn.Format("%s - %s",strFunction,Get_strSource());

			} return strReturn;

			case BOSELF50_EVENT_TRANSPORT_AUX	:		
			case BOSELF50_EVENT_TRANSPORT_VIDEO1:
			case BOSELF50_EVENT_TRANSPORT_VIDEO2:
			case BOSELF50_EVENT_TRANSPORT_TAPE:
			{
				strTemp.LoadString(IDS_BOSERS_TK_PLAY + Get_TKFunction());						
				strReturn.Format("%s - %s",strFunction,strTemp);

			} return strReturn;
		}
	}
	else 
	{
		bMethodData = Get_Function();

		for (i=0;i<MAX_BOSERSOUTPUT_FUNCTIONS;i++) 
		{
			if (bMethodData == BoseRsOutputStrings[i].bMethodData) 
			{
				strFunction.LoadString(BoseRsOutputStrings[i].ulStringID);
				break;
			}	
		}
		if (i>=MAX_BOSERSOUTPUT_FUNCTIONS) return strReturn;

		switch(bMethodData)
		{
			case AUDIO_OBJECT_ATTR_SOURCE_ON:		
			{
				strReturn.Format("%s - %s",strFunction,Get_strSource());				

			} return strReturn;

			case AUDIO_OBJECT_ATTR_VOLUME:
			{
				strReturn.Format("%s - Vol:%02d",strFunction,Get_Volume());

			} return strReturn;

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
			break;

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

			case AUDIO_OBJECT_ATTR_TUNERFREQ:	
			case AUDIO_OBJECT_ATTR_DISC:
			case AUDIO_OBJECT_ATTR_STATE:
			case AUDIO_OBJECT_USE_SELECTOR_DATA:				
			case AUDIO_OBJECT_ATTR_CONFIG:
			default:				
			break;	
		}
	}
	return strFunction;
}