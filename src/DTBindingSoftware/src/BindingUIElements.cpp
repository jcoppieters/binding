#include "stdafx.h"
#include "bindingUIElements.h"
#include "BindingSoftware.h"
#include "nodemess.h"

#include "TMGlobals.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



/*************************************************************************/
using namespace DUOTECNO::BINDINGS::UI;
using namespace DUOTECNO::BINDINGS::UI::PRIVATE;
/*************************************************************************/

#define DEF_KEY_OUTPUT(BINDINGUNIT,_ID,KEY,VALUE)	template <> const COLLECTION_KEY \
													TBindingOutputStringEntries_Collection<BINDINGUNIT,_ID>::KEY = VALUE;

#define DEF_KEY_INPUT(BINDINGUNIT,_ID,KEY,VALUE)	template <> const COLLECTION_KEY \
													TBindingInputStringEntries_Collection<BINDINGUNIT,_ID>::KEY = VALUE;


#define DEF_KEYS_INPUT(BINDINGUNIT,_ID,_EXT,_BASIC,_SIMPLE)		DEF_KEY_INPUT(BINDINGUNIT, _ID, EXTENDED, _EXT);		\
																DEF_KEY_INPUT(BINDINGUNIT, _ID, BASIC,    _BASIC);	\
																DEF_KEY_INPUT(BINDINGUNIT, _ID, SIMPLE,   _SIMPLE);  													

#define DEF_KEYS_OUTPUT(BINDINGUNIT,_ID,_EXT,_BASIC,_SIMPLE)		DEF_KEY_OUTPUT(BINDINGUNIT, _ID, EXTENDED, _EXT);	\
																	DEF_KEY_OUTPUT(BINDINGUNIT, _ID, BASIC,    _BASIC);	\
																	DEF_KEY_OUTPUT(BINDINGUNIT, _ID, SIMPLE,   _SIMPLE);  


DEF_KEYS_OUTPUT(CBindingDimmerUnit, CBindingStringEntries_Col::DIMMER_UNITS, 
					DIMMER_STRINGS_EXTENDED, DIMMER_STRINGS_BASIC, DIMMER_STRINGS_SIMPLE);
DEF_KEYS_OUTPUT(CBindingRelaisUnit, CBindingStringEntries_Col::SWITCH_UNITS, 
					SWITCH_STRINGS_EXTENDED, SWITCH_STRINGS_BASIC, NOT_USED);

// Sedert versie 0x0D00
DEF_KEYS_INPUT(CBindingSensUnivUnit, CBindingStringEntries_Col::SENSUNIV_UNITS_INPUT, 
				SENSUNIV_STRINGS_INPUT, NOT_USED, NOT_USED);

// Sedert versie 0x0B01
DEF_KEYS_INPUT(CBindingDimmerUnitInput, CBindingStringEntries_Col::DIMMER_UNITS_INPUT, 
					DIMMER_STRINGS_INPUT, NOT_USED, NOT_USED);
// Sedert versie 0x0C01
DEF_KEYS_INPUT(CBindingRelaisUnitInput, CBindingStringEntries_Col::SWITCH_UNITS_INPUT, 
					SWITCH_STRINGS_INPUT, NOT_USED, NOT_USED);
// Sedert versie 0x0C01
DEF_KEYS_INPUT(CBindingDuoSwitchUnitInput, CBindingStringEntries_Col::DUOSWITCH_UNITS_INPUT, 
					DUOSWITCH_STRINGS_INPUT, NOT_USED, NOT_USED);

DEF_KEYS_OUTPUT(CBindingDuoSwitchUnit, CBindingStringEntries_Col::DUOSWITCH_UNITS, 
					DUOSWITCH_STRINGS_EXTENDED, DUOSWITCH_STRINGS_BASIC, NOT_USED);
DEF_KEYS_OUTPUT(CBindingIRTXUnit, CBindingStringEntries_Col::IRTX_UNITS, 
					IRTX_STRINGS_EXTENDED, IRTX_STRINGS_BASIC, NOT_USED);
DEF_KEYS_OUTPUT(CBindingSensUnit, CBindingStringEntries_Col::SENS_UNITS_OUTPUT, 
					SENS_STRINGS_EXTENDED, SENS_STRINGS_BASIC, NOT_USED);
DEF_KEYS_INPUT(CBindingSensUnit, CBindingStringEntries_Col::SENS_UNITS_INPUT, 
					SENS_STRINGS_INPUT, NOT_USED, NOT_USED);
DEF_KEYS_OUTPUT(CBindingBoseLf48Unit, CBindingStringEntries_Col::BOSE_LF48_OUTPUT, 
					BOSELF48_STRINGS_EXTENDED, BOSELF48_STRINGS_BASIC, BOSELF48_STRINGS_SIMPLE);
DEF_KEYS_INPUT(CBindingBoseLf48Unit, CBindingStringEntries_Col::BOSE_LF48_INPUT, 
					BOSELF48_INPUTSTRINGS_EXTENDED, BOSELF48_INPUTSTRINGS_BASIC, NOT_USED);
DEF_KEYS_INPUT(CBindingBoseLf48Unit, CBindingStringEntries_Col::BOSE_LF48_INPUT_VIRTUALS, 
					BOSELF48_INPUTSTRINGS_VIRTUALS, NOT_USED, NOT_USED);
DEF_KEYS_OUTPUT(CBindingBasicAudioUnit, CBindingStringEntries_Col::IRAUDIO_UNITS_OUTPUT, 
					BOSELF48_STRINGS_EXTENDED, BOSELF48_STRINGS_BASIC, BOSELF48_STRINGS_SIMPLE);
DEF_KEYS_OUTPUT(CBindingAVMatrixUnit, CBindingStringEntries_Col::AVMATRIX_UNITS, 
					AVMATRIX_STRINGS_EXTENDED, AVMATRIX_STRINGS_BASIC, NOT_USED);
DEF_KEYS_INPUT(CBindingAVMatrixUnitInput, CBindingStringEntries_Col::AVMATRIX_INPUT_UNITS, 
					AVMATRIX_INPUTSTRINGS_BASIC, AVMATRIX_INPUTSTRINGS_BASIC, NOT_USED);				/* TM_AVMATRIX_INPUT */
DEF_KEYS_OUTPUT(CBindingVideomuxUnit, CBindingStringEntries_Col::VIDEOMUX_UNITS, 
					VIDEOMUX_STRINGS_EXTENDED, VIDEOMUX_STRINGS_BASIC, NOT_USED);
DEF_KEYS_OUTPUT(CBindingHC4AudioUnit, CBindingStringEntries_Col::MATRIXAUDIO_UNITS, 
					AUDIOHC4_STRINGS_EXTENDED, AUDIOHC4_STRINGS_BASIC, AUDIOHC4_STRINGS_SIMPLE);
DEF_KEYS_INPUT(CBindingHC4AudioUnit_Input, CBindingStringEntries_Col::MATRIXAUDIO_INPUT_UNITS, 
				    AUDIOHC4_INPUTSTRINGS_BASIC, AUDIOHC4_INPUTSTRINGS_BASIC, NOT_USED);				/* TM_AUDIOHC4_BINDING_INPUT */
DEF_KEYS_INPUT(CBindingHC4AudioUnit_Input, CBindingStringEntries_Col::MATRIXAUDIOBNO_INPUT_UNITS, 
				    AUDIOHC4_INPUTSTRINGS_BNO_BASIC, AUDIOHC4_INPUTSTRINGS_BNO_BASIC, NOT_USED);		/* TM_AUDIOHC4_BINDING_INPUT */
DEF_KEYS_OUTPUT(CBindingBoseV35Unit, CBindingStringEntries_Col::BOSE_V35_OUTPUT, 
					BOSEV35_STRINGS_EXTENDED, BOSEV35_STRINGS_BASIC, BOSEV35_STRINGS_SIMPLE);
DEF_KEYS_OUTPUT(CBindingBoseV35Unit, CBindingStringEntries_Col::BOSE_V35_OUTPUT_VIRTUALS, 
					BOSEV35_STRINGS_VIRTUALS, NOT_USED, NOT_USED);
DEF_KEYS_INPUT(CBindingBoseV35Unit, CBindingStringEntries_Col::BOSE_V35_INPUT, 
					BOSEV35_INPUTSTRINGS_EXTENDED, BOSEV35_INPUTSTRINGS_BASIC, NOT_USED);
DEF_KEYS_INPUT(CBindingBoseV35Unit, CBindingStringEntries_Col::BOSE_V35_INPUT_VIRTUALS, 
					BOSEV35_INPUTSTRINGS_VIRTUALS, NOT_USED, NOT_USED);
/* TM_CHANGES_BINDINGS_ALARM */
DEF_KEYS_OUTPUT(CBindingAlarmUnit, CBindingStringEntries_Col::ALARM_OUTPUT, 
				ALARM_OUTPUTSTRING, NOT_USED, NOT_USED);
DEF_KEYS_INPUT(CBindingAlarmUnit, CBindingStringEntries_Col::ALARM_INPUT, 
			   ALARM_INPUTSTRINGS_EXTENDED, ALARM_INPUTSTRINGS_BASIC, NOT_USED);

/*************************************************************************/

const CStringEntries* CBindingStringEntries_Col::Get(const CBindingUnit* const pBindingUnit,
													 const COLLECTION_KEY_UNITTYPE Key) const
{	

	#define IMPLEMENTATION_STRATEGY(CLASS,_ID, ALG)		ALG<CLASS,_ID> strategy( \
															static_cast<const CLASS* const>(pBindingUnit));		 \
														return (strategy.Get());

	#define IMPLEMENTATION_STRATEGY_OUTPUT(CLASS,_ID)	IMPLEMENTATION_STRATEGY(CLASS, _ID,	\
															TBindingOutputStringEntries_Collection);
	#define IMPLEMENTATION_STRATEGY_INPUT(CLASS,_ID)	IMPLEMENTATION_STRATEGY(CLASS, _ID,  \
															TBindingInputStringEntries_Collection);
	
	switch(Key)
	{
		case DIMMER_UNITS:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingDimmerUnit, DIMMER_UNITS);
		} break;		
		case DIMMER_UNITS_INPUT:		// Sedert versie 0x0B01
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingDimmerUnitInput, DIMMER_UNITS_INPUT);
		} break;
		case SWITCH_UNITS:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingRelaisUnit, SWITCH_UNITS);
		} break;		
		case SWITCH_UNITS_INPUT:		// sedert versie 0x0C01
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingRelaisUnitInput, SWITCH_UNITS_INPUT);
		} break;
		case DUOSWITCH_UNITS:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingDuoSwitchUnit, DUOSWITCH_UNITS);
		} break;
		case DUOSWITCH_UNITS_INPUT:		// sedert versie 0x0C01
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingDuoSwitchUnitInput, DUOSWITCH_UNITS_INPUT);
		} break;
		case BOSE_LF48_OUTPUT:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingBoseLf48Unit, BOSE_LF48_OUTPUT);		
		} break;
		case BOSE_LF48_INPUT:
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingBoseLf48Unit, BOSE_LF48_INPUT);
		} break;
		case BOSE_LF48_INPUT_VIRTUALS:
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingBoseLf48Unit, BOSE_LF48_INPUT_VIRTUALS);					
		} break;
		case IRTX_UNITS:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingIRTXUnit, IRTX_UNITS);
		} break;
		case SENS_UNITS_OUTPUT:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingSensUnit, SENS_UNITS_OUTPUT);
		} break;
		case SENS_UNITS_INPUT:
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingSensUnit, SENS_UNITS_INPUT);
		} break;
		case SENSUNIV_UNITS_INPUT:
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingSensUnivUnit, SENSUNIV_UNITS_INPUT);
		} break;
		case MATRIXAUDIO_UNITS:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingHC4AudioUnit, MATRIXAUDIO_UNITS);
		} break;
		case MATRIXAUDIO_INPUT_UNITS:		/* TM_AUDIOHC4_BINDING_INPUT */
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingHC4AudioUnit_Input, MATRIXAUDIO_INPUT_UNITS);	
		} break;
		case MATRIXAUDIOBNO_INPUT_UNITS:	/* TM_AUDIOHC4_BNO */
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingHC4AudioUnit_Input, MATRIXAUDIOBNO_INPUT_UNITS);	
		} break;
		case IRAUDIO_UNITS_OUTPUT:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingBasicAudioUnit, IRAUDIO_UNITS_OUTPUT);
		} break;
		case AVMATRIX_UNITS:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingAVMatrixUnit, AVMATRIX_UNITS);
		} break;
		case AVMATRIX_INPUT_UNITS:			/* TM_AVMATRIX_INPUT */
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingAVMatrixUnitInput, AVMATRIX_INPUT_UNITS);
		} break;
		case VIDEOMUX_UNITS:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingVideomuxUnit, VIDEOMUX_UNITS);
		} break;
		case BOSE_V35_OUTPUT:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingBoseV35Unit, BOSE_V35_OUTPUT);		
		} break;
		case BOSE_V35_OUTPUT_VIRTUALS:
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingBoseV35Unit, BOSE_V35_OUTPUT_VIRTUALS);		
		} break;
		case BOSE_V35_INPUT:
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingBoseV35Unit, BOSE_V35_INPUT);
		} break;
		case BOSE_V35_INPUT_VIRTUALS:
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingBoseV35Unit, BOSE_V35_INPUT_VIRTUALS);	
		} break;	
		case ALARM_OUTPUT:				/* TM_CHANGES_BINDINGS_ALARM */
		{
			IMPLEMENTATION_STRATEGY_OUTPUT(CBindingAlarmUnit, ALARM_OUTPUT);
		} break;
		case ALARM_INPUT:
		{
			IMPLEMENTATION_STRATEGY_INPUT(CBindingAlarmUnit, ALARM_INPUT);
		} break;
		default:
		{
			ASSERT(0);
		}break;
	}
	return 0;
}

/**********************************************************************************************/

template <> const COLLECTION_KEY2 
TBindingEntryStringEntries_Collection<CBindingStringEntries_Col::BINDINGFLAGS>::EXTENDED = BINDINGFLAGS_EXTENDED;

template <> const COLLECTION_KEY2 
TBindingEntryStringEntries_Collection<CBindingStringEntries_Col::BINDINGFLAGS>::BASIC = BINDINGFLAGS_BASIC;

template <> const COLLECTION_KEY2 
TBindingEntryStringEntries_Collection<CBindingStringEntries_Col::BINDINGFLAGS>::SIMPLE = KEY_NOT_USED;


const CStringEntries* CBindingStringEntries_Col::Get(const CBindingEntry* const pBindingEntry,
													 const COLLECTION_KEY_TYPE Key) const
{
	switch (Key)
	{
		case BINDINGFLAGS:
		{
			TBindingEntryStringEntries_Collection<CBindingStringEntries_Col::BINDINGFLAGS>
				strategy(pBindingEntry);

			return (strategy.Get());

		} break;
	}
	return 0;
}

BOOL PRIVATE::CStringEntries_Collection::ViewModeExtended() const
{
	using namespace TMGlobalSettings;
	return (TMGetDisplaySettings()->IsModeExtended());
}
BOOL PRIVATE::CStringEntries_Collection::ViewModeNormal() const
{
	using namespace TMGlobalSettings;
	return (TMGetDisplaySettings()->IsModeNormal());
}