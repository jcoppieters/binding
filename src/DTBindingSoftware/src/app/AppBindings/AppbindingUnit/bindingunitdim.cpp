#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitdim.h"
#include "bindingunitdim_def.h"

#include "NodeMess.h"
#include "BindingConstants.h"

#include "bindingUIElement.h"	// ??

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;

CBindingDimmerUnit::CBindingDimmerUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{
	SetInOutType(UNIT_TYPE_OUTPUT);
	Init();				
} 


CBindingDimmerUnit::CBindingDimmerUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	ASSERT(BindingUnit.GetNodeAddress()!=0xFF);
	ASSERT((BindingUnit.GetBindingUnitType()==BINDING_UNIT_TYPE_DIM)||
		   (BindingUnit.GetBindingUnitType()==BINDING_UNIT_TYPE_DIM_FWD));

	Init();				
}

CBindingDimmerUnit::~CBindingDimmerUnit() { }


void CBindingDimmerUnit::Init(void)		
{
	Set_Function(DIMMER_OBJECT_METHOD_FSCVONOFF);
	Set_Status(0);
	Set_Value(DIM_VALUE_DEFAULT);
	Set_PirTime(DIM_TIMEVALUE_DEFAULT);	

	// Sedert versie 0x0B00: Cfg van een address.
	Set_AddressCfgType(0xFF);
	Set_AddressCfgMSB(0xFF);
	Set_AddressCfgLSB(0xFF);
	Set_CalcValue(1);

	// TM 20080122: Na het clonen van een bindingunit is er info
	// genoeg om de juiste standaard waarden op te vragen ifv het node en
	// unit adres...
	const CBindingPropertyUnitDimDefault* pDefaults = 0;

	if (GetNodeAddress() == 0xFF)
	{
		pDefaults = static_cast<const CBindingPropertyUnitDimDefault* const>
				(CBindingPropertyUnitDefaultFactory::Instance()->
					Create(UNIT_TYPE_DIM));
	}
	else
	{	
		// TM 20130328:
		// Structuur gebruiken om de params mee te geven aan de factory.
		// Binding unit type meegeven omdat de unit mogelijk nog niet aanwezig is 
		// in de nodedatabase.
		const CBindingPropertyUnitDefaultFactory::CUnitInfo unitInfo =
		{
			this->GetNodeAddress(),
			this->GetUnitAddress(),
			this->GetBindingUnitType()			// Binding unit type.
		};

		pDefaults = static_cast<const CBindingPropertyUnitDimDefault* const>
				(CBindingPropertyUnitDefaultFactory::Instance()->
					Create(UNIT_TYPE_DIM, unitInfo));
	}

	ASSERT( pDefaults != 0);

	if (pDefaults != 0)
	{
		Set_Range( pDefaults->GetMinRange(), pDefaults->GetMaxRange() );
		Set_Speed( pDefaults->GetSpeedDown(), pDefaults->GetSpeedUp() );
		Set_Config( pDefaults->GetConfig() );
	}
}

BOOL CBindingDimmerUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();
	if ( bMessageCode == FC_UNITDIMSET )
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingDimmerUnit::ConvertToBindingData(void)
{
	BindingData.Clear();
	BindingData.Set_MessageCode(FC_UNITDIMSET);

	const BYTE bMethodData = Get_Function();
	BindingData.Set_MethodData(bMethodData);

	// TM 20051108 : Dimmer Unit must always be an output unit !!!!
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{
		AfxMessageBox("This type of dimmer unit cannot be used as an input in a binding.\n"
					  "It can only be used as an output.");
		return FALSE;
	}

	switch(bMethodData)
	{
		case DIMMER_OBJECT_USE_SELECTOR_DATA :
		case DIMMER_OBJECT_ATTR_STATE :
			// ABSORBE
		return FALSE;

		case DIMMER_OBJECT_ATTR_SPEED :	// Length =3, 0x01,SpeedUp,Speeddn
		{
			BindingData.Set_Length(3);
			BindingData.Set_Data(1,Get_SpeedUp());
			BindingData.Set_Data(2,Get_SpeedDown());

		} return TRUE;

		case DIMMER_OBJECT_ATTR_RANGE :	// Length =3, 0x02,MaxRange,MinRange
		{
			BindingData.Set_Length(3);
			BindingData.Set_Data(1,Get_MaxRange());
			BindingData.Set_Data(2,Get_MinRange());

		} return TRUE;

		case DIMMER_OBJECT_ATTR_VALUE :	// Length =2, 0x03,Value
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Value());

		} return TRUE;

		case DIMMER_OBJECT_ATTR_ONOFF :	// Length =2, 0x04,0/1
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Status() & 0x01);

		} return TRUE;

		case DIMMER_OBJECT_METHOD_UP :			// Length = 1,	0x05
		case DIMMER_OBJECT_METHOD_DN:			//				0x06				
		case DIMMER_OBJECT_METHOD_FSUD:			//				0x07
		case DIMMER_OBJECT_METHOD_FSUDONOFF:	//				0x08
		case DIMMER_OBJECT_METHOD_OFF:			//				0x09
		case DIMMER_OBJECT_METHOD_ONCV:			//				0x0A
		case DIMMER_OBJECT_METHOD_FSCVONOFF:	//				0x0B		
		{
			BindingData.Set_Length(1);

		} return TRUE;	

		// Solved BUG-0151
		case DIMMER_OBJECT_METHOD_INC:			//				0x0F
		case DIMMER_OBJECT_METHOD_DEC:			//				0x10
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_CalcValue());
		} return TRUE;

		case DIMMER_OBJECT_METHOD_FSCVOFF :			// Length =2, 0x0c, Value
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Value());

		} return TRUE;
	
		case DIMMER_OBJECT_ATTR_CONFIG:				// Length =2, 0x0e, Config
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Config());

		} return TRUE;

		case DIMMER_OBJECT_METHOD_ONUP:				// Length =4, 0x11,Value,Speed,State
		{
			BindingData.Set_Length(4);

			BindingData.Set_Data(1,Get_Value());
			BindingData.Set_Data(2,Get_SpeedUp());
			BindingData.Set_Data(3,Get_Status());

		} return TRUE;

		case DIMMER_OBJECT_METHOD_ONDN:				// Length =4, 0x12,Value,Speed,State
		{
			BindingData.Set_Length(4);

			BindingData.Set_Data(1,Get_Value());
			BindingData.Set_Data(2,Get_SpeedDown());
			BindingData.Set_Data(3,Get_Status());

		} return TRUE;

		case DIMMER_OBJECT_METHOD_PIRON:			// Length =3, 0x13,PirTime,Value
		{
			BindingData.Set_Length(3);

			BindingData.Set_Data(1,Get_PirTime());
			BindingData.Set_Data(2,Get_Value());			

		} return TRUE;

		case DIMMER_OBJECT_ATTR_CFG_ADDRESS:
		{
			BindingData.Set_Length(4);				// Lengte=4, Type, MSB, LSD

			BindingData.Set_Data(1,Get_AddressCfgType());
			BindingData.Set_Data(2,Get_AddressCfgMSB());
			BindingData.Set_Data(3,Get_AddressCfgLSB());

		} return TRUE;

		default:
			ASSERT(0);
		return FALSE;
	}
	return FALSE;
}

BOOL CBindingDimmerUnit::ConvertFromBindingData(void)
{
	Init();		// TM 20040701 
	
	const BYTE bMessageCode = BindingData.Get_MessageCode();
	
	if (bMessageCode != FC_UNITDIMSET) 
	{
		ASSERT(0);
		return FALSE;
	}
	
	const BYTE bMethodData = BindingData.Get_MethodData();

	Set_Function(bMethodData);
	
	const BYTE bLength = BindingData.Get_Length();

	switch(bMethodData) 
	{
		case DIMMER_OBJECT_USE_SELECTOR_DATA:
		case DIMMER_OBJECT_ATTR_STATE:
		return FALSE;

		case DIMMER_OBJECT_ATTR_SPEED:
		{
			if (bLength!=3) return FALSE;

			Set_SpeedUp(BindingData.Get_Data(1));
			Set_SpeedDown(BindingData.Get_Data(2));

		} return TRUE;

		case DIMMER_OBJECT_ATTR_RANGE :
		{
			if (bLength!=3) return FALSE;

			Set_Range(BindingData.Get_Data(2),BindingData.Get_Data(1));		

		} return TRUE;

		case DIMMER_OBJECT_ATTR_VALUE:
		{
			if (bLength!=2) return FALSE;

			Set_Value(BindingData.Get_Data(1));

		} return TRUE;

		case DIMMER_OBJECT_ATTR_ONOFF:
		{
			if (bLength!=2) return FALSE;

			Set_Status(BindingData.Get_Data(1));

		} return TRUE;

		case DIMMER_OBJECT_METHOD_UP :			// Length = 1,	0x05
		case DIMMER_OBJECT_METHOD_DN:			//				0x06				
		case DIMMER_OBJECT_METHOD_FSUD:			//				0x07
		case DIMMER_OBJECT_METHOD_FSUDONOFF:	//				0x08
		case DIMMER_OBJECT_METHOD_OFF:			//				0x09
		case DIMMER_OBJECT_METHOD_ONCV:			//				0x0A
		case DIMMER_OBJECT_METHOD_FSCVONOFF:	//				0x0B	
		{
			if (bLength!=1) return FALSE;

		} return TRUE;			

		// Solved BUG-0151
		case DIMMER_OBJECT_METHOD_INC:			//				0x0F
		case DIMMER_OBJECT_METHOD_DEC:			//				0x10
		{
			// Vroeger werden de bindings aangemaakt met lengte 1.
			// daarom deze toelaten en met waarde 1 anders zou er een
			// foutmelding komen.

			if (bLength==1)
			{
				Set_CalcValue(1);
			}
			else if (bLength==2)
			{
				Set_CalcValue(BindingData.Get_Data(1));
			}
			else
			{
				return FALSE;
			}

		} return TRUE;

		case DIMMER_OBJECT_METHOD_FSCVOFF:
		{
			if (bLength!=2) return FALSE;

			Set_Value(BindingData.Get_Data(1));

		} return TRUE;

		case DIMMER_OBJECT_ATTR_CONFIG:
		{
			if (bLength!=2) return FALSE;

			Set_Config(BindingData.Get_Data(1));
		} return TRUE;
		
		case DIMMER_OBJECT_METHOD_ONUP:
		{
			if (bLength!=4) return FALSE;

			Set_Value(BindingData.Get_Data(1));
			Set_SpeedUp(BindingData.Get_Data(2));
			Set_Status(BindingData.Get_Data(3));

		} return TRUE;

		case DIMMER_OBJECT_METHOD_ONDN:
		{
			if (bLength!=4) return FALSE;

			Set_Value(BindingData.Get_Data(1));
			Set_SpeedDown(BindingData.Get_Data(2));
			Set_Status(BindingData.Get_Data(3));

		} return TRUE;

		case DIMMER_OBJECT_METHOD_PIRON:
		{
			if (bLength!=3) return FALSE;

			Set_PirTime(BindingData.Get_Data(1));
			Set_Value(BindingData.Get_Data(2));

		} return TRUE;
	
		case DIMMER_OBJECT_ATTR_CFG_ADDRESS:
		{
			if (bLength!=4) return FALSE;

			Set_AddressCfgType(BindingData.Get_Data(1));
			Set_AddressCfgMSB(BindingData.Get_Data(2));
			Set_AddressCfgLSB(BindingData.Get_Data(3));


		} return TRUE;

		default:
		{
			ASSERT(0);

		} return FALSE;	
	}
	return FALSE;
}

CString CBindingDimmerUnit::ConvertFuntionToString(void)
{
	CString strMethod;
	CString strData;	

	const BYTE bMethodData = Get_Function();

	const CBindingStringEntries* const pStringEntries = 
		CBindingUnitStringEntries_Collection::Instance()->Get(DIMMER_STRINGS_EXTENDED);

	ASSERT(pStringEntries);

	pStringEntries->Find(bMethodData,strMethod);
	
	switch(bMethodData) 
	{
		case DIMMER_OBJECT_USE_SELECTOR_DATA:
		case DIMMER_OBJECT_ATTR_STATE:
		break;

		case DIMMER_OBJECT_ATTR_SPEED:		
		{
			strData.Format("- %d - %d",Get_SpeedDown(),Get_SpeedUp());

		} break;

		case DIMMER_OBJECT_ATTR_RANGE :				
		{
			strData.Format("- %d - %d",Get_MinRange(),Get_MaxRange());

		} break;

		case DIMMER_OBJECT_ATTR_VALUE:		
		{
			strData.Format("- %02d",Get_Value());

		} break;

		case DIMMER_OBJECT_ATTR_ONOFF:	
		{
			strData.Format("- %d",Get_Status());

		} break;

		case DIMMER_OBJECT_METHOD_UP :			// Length = 1,	0x05
		case DIMMER_OBJECT_METHOD_DN:			//				0x06				
		case DIMMER_OBJECT_METHOD_FSUD:			//				0x07
		case DIMMER_OBJECT_METHOD_FSUDONOFF:	//				0x08
		case DIMMER_OBJECT_METHOD_OFF:			//				0x09
		case DIMMER_OBJECT_METHOD_ONCV:			//				0x0A
		case DIMMER_OBJECT_METHOD_FSCVONOFF:	//				0x0B				
		break;			

		// Solved BUG-0151
		case DIMMER_OBJECT_METHOD_INC:			//				0x0F
		case DIMMER_OBJECT_METHOD_DEC:			//				0x10	
		{
			strData.Format("- %d",Get_CalcValue());
		} break;

		case DIMMER_OBJECT_METHOD_FSCVOFF:		
		{
			strData.Format("- %02d",Get_Value());
		} break;

		case DIMMER_OBJECT_ATTR_CONFIG:		
		{
			strData.Format("- %d",Get_Config());

		} break;
		
		case DIMMER_OBJECT_METHOD_ONUP:	
		{
			strData.Format("- %02d%% - %d -%d",Get_Value(),Get_SpeedUp(),Get_Status());

		} break;

		case DIMMER_OBJECT_METHOD_ONDN:													 
		{
			strData.Format("- %02d%% - %d -%d",Get_Value(),Get_SpeedDown(),Get_Status());

		} break;

		case DIMMER_OBJECT_METHOD_PIRON:	
		{
			BYTE bData1 = Get_PirTime();
			BYTE bData2 = Get_Value();
			
			if (bData1==255) bData1 = 3;					
			if (bData2==255) strData.Format("- %dmin - Curr val.",bData1);
			else strData.Format("- %dmin - %d",bData1,bData2);
	
		} break;

		case DIMMER_OBJECT_ATTR_CFG_ADDRESS:
		{
			const BYTE bData1 = Get_AddressCfgType();
			const BYTE bData2 = Get_AddressCfgMSB();
			const BYTE bData3 = Get_AddressCfgLSB( );
			if (bData1==0xFF) {
				strData.Format(_T("No address Cfg"));
			}
			else {
				strData.Format(_T("Address cfg:%d - %d"), bData1,(bData2<<8)+bData3);
			}
		} break;

		default:
			ASSERT(0);
		break;	
	}

	CString strReturn;
	if (!strData.IsEmpty()) {
		strReturn.Format("%s%s",strMethod,strData);	
	}
	else {
		strReturn = strMethod;
	}
	return strReturn;
}





////////////////////////////////////////////////////////////

CBindingDimmerUnitInput::CBindingDimmerUnitInput(BYTE bBindingUnitType) :
Base(bBindingUnitType),
m_bEvent(EV_UNITCONTROLPULSTOGGLE),
m_bInputState(1)
{
	Init( );
}

CBindingDimmerUnitInput::CBindingDimmerUnitInput(const CBindingUnit& pBindingUnit) :
Base(pBindingUnit),
m_bEvent(EV_UNITCONTROLPULSTOGGLE),
m_bInputState(1)
{
	Init( );
}

CBindingDimmerUnitInput::~CBindingDimmerUnitInput()
{

}

void CBindingDimmerUnitInput::Init(void)
{
	// TODO: Evt opvragen van unitState.

	m_bEvent = EV_UNITCONTROLPULSTOGGLE;
	m_bInputState = 1;

	this->Base::Init( );
}

BOOL CBindingDimmerUnitInput::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if (bMessageCode == EV_UNITCONTROLPULSTOGGLE)
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}

	return this->Base::UpgradeIoType();
}

BOOL CBindingDimmerUnitInput::ConvertToBindingData(void)
{		
	BOOL fResult=FALSE;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		BYTE bTemp=0;
		const BYTE bMessageCode = this->Get_Event();

		this->BindingData.Clear();		
		
		if (bMessageCode == EV_UNITCONTROLPULSTOGGLE)
		{
			if (this->Get_InputState() != 0)
			{
				bTemp|=0x80;
			}

			this->BindingData.Set_MessageCode(bMessageCode);
			this->BindingData.Set_BindingFlags(bTemp);	

			// TODO: Evt de toestand opslaan in unitState.

			fResult=TRUE;
		}		
		else
		{
			ASSERT(0);
			fResult=FALSE;
		}
	}
	else
	{
		fResult=this->Base::ConvertToBindingData();
	}
	return fResult;
}

BOOL CBindingDimmerUnitInput::ConvertFromBindingData(void)
{
	BOOL fResult=FALSE;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		Init();			

		const BYTE bMessageCode = BindingData.Get_MessageCode();
		if (bMessageCode == EV_UNITCONTROLPULSTOGGLE)
		{
			int state = 0;
			const BYTE bBindingFlags = BindingData.Get_BindingFlags();
			if (bBindingFlags & 0x80)
			{
				state=1;
			}
			
			this->Set_Event(bMessageCode);
			this->Set_InputState(state);
			fResult=TRUE;
		}
		else
		{
			fResult=FALSE;
			ASSERT(0);
		}
	}
	else
	{
		fResult=this->Base::ConvertFromBindingData();
	}
	return fResult;
}


CString CBindingDimmerUnitInput::ConvertFuntionToString(void)
{
	CString strMethod;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 	
	{				
		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(DIMMER_STRINGS_INPUT);

		if(0!=pStringEntries)
		{			
			const BYTE bMethodData = this->Get_Event();

			if (TRUE==pStringEntries->Find(bMethodData,strMethod))
			{
				return strMethod;
			}
		}
	}
	else
	{
		return this->Base::ConvertFuntionToString();
	}
	return "";
}

