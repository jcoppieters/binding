#include "stdafx.h"
#include "bindingpropertyunit_alg.h"
#include "bindingpropertyunit.h"

#include "nodedatabase.h"
#include "bindingconstants.h"

#include "App\Appbindings\AppBindingConv2Nodes\BindingConversionToNodesProperty.h"

#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"	
#include "app/appcfg/appcfginfo/cfginfounit.h"
#include "app\appCfg\AppCfginfo\cfginfonode.h"	


// Private: prototypes.
static void bindingpropertyunitalg_upgrade(CCanNode* pNode,STRUCT_UNIT* pUnit,CBindingPropertyUnit* pBindingUnit);


// Private helper.

static void helperflags_set(unsigned short* pVal, unsigned short flag, int state) {
	if (state) {
		*pVal|=flag;
	}
	else {
		*pVal&=~flag;
	}
}

// Private
// Alle dimmer units in de node: minimum range 20% instellen en een property binding aanmaken.
//
// Sedert V16.41: lastUnitAddressCreateDimmerBindings
// Het laatset unit address voor wie er bindings mogen aangemaakt worden. ( -1 = don't care.)
static void helper_dimmerCreateExtraPBindings(CNodeDatabase* pNodeDatabase,CCanNode* pNode,int lastUnitAddressCreateDimmerBindings) {
	if (0==pNode || 0==pNodeDatabase) {
		ASSERT(0!=pNode);
		ASSERT(0!=pNodeDatabase);
		return;
	}
	const int nrOfUnits = pNode->getNROfUnits();
	for (int i=0;i<nrOfUnits;++i) {
		STRUCT_UNIT* pUnit = 0;
		if (pNodeDatabase->GetUnit(pNode,i,&pUnit)) {
			if (UNIT_TYPE_DIM == pUnit->bUnitType) {		
				if ((lastUnitAddressCreateDimmerBindings == -1) || (pUnit->bUnitAddress <= lastUnitAddressCreateDimmerBindings)) {
					if (0 != pUnit->m_pBindingUnit) {
						CBindingPropertyDimUnit* const propertyUnitDim = dynamic_cast<CBindingPropertyDimUnit*>(pUnit->m_pBindingUnit);
						if (0 != propertyUnitDim) {
							propertyUnitDim->set_DimMinRange(20);

							CBindingConversionToNodesProperty conv(propertyUnitDim);
							const int res = conv.ProcessPropertyBindings(TRUE,FALSE);	// MakeNoDefaults, ShowNoErrors.
							ASSERT(res>0);

						} else { ASSERT(0 != propertyUnitDim); }
					} else { ASSERT(0 != pUnit->m_pBindingUnit); }
				}
			}
		}
	}
}

#if(USE_DUOSWITCH_PBINDINGS_SWITCHTIME == 1)
static void helper_duoswitchCreateExtraPBindings(CNodeDatabase* pNodeDatabase, CCanNode* pNode) {
	if (0 == pNode || 0 == pNodeDatabase) {
		ASSERT(0 != pNode);
		ASSERT(0 != pNodeDatabase);
		return;
	}
	const int nrOfUnits = pNode->getNROfUnits();
	for (int i = 0; i < nrOfUnits; ++i) {
		STRUCT_UNIT* pUnit = 0;
		if (pNodeDatabase->GetUnit(pNode, i, &pUnit)) {
			if (UNIT_TYPE_DUOSWITCH == pUnit->bUnitType) {
				if (0 != pUnit->pBindingUnit) {
					CBindingPropertyDuoSwitchUnit* const propertyUnitDuoswitch = dynamic_cast<CBindingPropertyDuoSwitchUnit*>(pUnit->pBindingUnit);
					if (0 != propertyUnitDuoswitch) {
						propertyUnitDuoswitch->setSwitchTime(DUOSWITCH_DEFAULT_SWITCHTIME_V9025);

						CBindingConversionToNodesProperty conv(propertyUnitDuoswitch);
						const int res = conv.ProcessPropertyBindings(TRUE, FALSE);	// MakeNoDefaults, ShowNoErrors.
						ASSERT(res>0);
					}
				}
			}
		}
	}
}
#endif


#if(USE_SENSOR_PBINDINGS_OFFSETVALUE == 1)
// Private
// Alle sensor units van de oleds & drukknop schakelaars.
static void helper_sensorCreateExtraPBindings(CNodeDatabase* pNodeDatabase, CCanNode* pNode) {
	if (0 == pNode || 0 == pNodeDatabase) {
		ASSERT(0 != pNode);
		ASSERT(0 != pNodeDatabase);
		return;
	}
	const int nrOfUnits = pNode->getNROfUnits();
	for (int i = 0; i<nrOfUnits; ++i) {
		STRUCT_UNIT* pUnit = 0;
		if (pNodeDatabase->GetUnit(pNode, i, &pUnit)) {
			if (UNIT_TYPE_SENS == pUnit->bUnitType) {
				if (0 != pUnit->m_pBindingUnit) {
					CBindingPropertySensUnit* const propertyUnitSens = dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit);
					if (0 != propertyUnitSens) {
						propertyUnitSens->UpdateMeasuredValueOffset(-20);	// -2C

						CBindingConversionToNodesProperty conv(propertyUnitSens);
						const int res = conv.ProcessPropertyBindings(TRUE, FALSE);	// MakeNoDefaults, ShowNoErrors.
						ASSERT(res>0);
					}
					else { ASSERT(0 != propertyUnitSens); }
				}
				else { ASSERT(0 != pUnit->m_pBindingUnit); }
			}
		}
	}

}

#endif 


// Extra P-Bindings aanmaken.
// Atmega modules: Alle modules die 6 dimmer units hebben.
// NXP Smartbox module.
static void helper_createExtraPBindings(CNodeDatabase* pNodeDatabase,CCanNode* pNode) {
	if (0==pNode || 0==pNodeDatabase) {
		ASSERT(0!=pNode);
		ASSERT(0!=pNodeDatabase);
		return;
	}

	BOOL createDimmerBindings = FALSE;
#if(USE_SENSOR_PBINDINGS_OFFSETVALUE == 1)
	BOOL createSensorBindings = FALSE;
#endif 
#if(USE_DUOSWITCH_PBINDINGS_SWITCHTIME == 1)
	BOOL createDuoswitchBindings = FALSE;
#endif 
	const BYTE bNodeType = pNode->bNodeType;
	const BYTE bSoftwareVersion = pNode->GetSoftwareVersion();

	// Sedert V16.38:
	// Smartbox ultra P-Bindings aanmaken voor de dimmer units.

	// Sedert V16.41: zie BUG-0180
	// Bij de smartbox ultra kunnen er dimmer units aangemaakt worden.
	// Mogen enkel p-binding aanmaken voor de interne dimmer units 
	int lastUnitAddressCreateDimmerBindings = -1;

	if (NODE_PIN_GATEWAY == bNodeType) {
		if (PRODUCT_ID_HC_BOX_ULTRA == pNode->GetProductID()) {
			createDimmerBindings = TRUE;
			lastUnitAddressCreateDimmerBindings = CNodeConfigInfo_SmartboxUltra::getUnitAddressLastInternalUnit(pNode,UNIT_TYPE_DIM);
		}	
	}
	else if (bNodeType == NODE_PIN_STD_UNITS) { 		
		if ((bSoftwareVersion >=0x70) && (bSoftwareVersion < 0x90)) {
			
			// ATMega: Alle modules met 6 dimmer units.
			const int nrOfUnits = pNode->getNROfUnits();
			if (6 == nrOfUnits) {
				createDimmerBindings = TRUE;
				for (int i=0;i<nrOfUnits;++i) {
					STRUCT_UNIT* pUnit = 0;
					if (pNodeDatabase->GetUnit(pNode,i,&pUnit)) {
						if (UNIT_TYPE_DIM != pUnit->bUnitType) {
							createDimmerBindings = FALSE;
							break;
						}
					}
				}
			}		
		
#if(USE_DUOSWITCH_PBINDINGS_SWITCHTIME == 1)		
			// Alle standaard nodes  met rolluik units.
			for (int i = 0; i < nrOfUnits; ++i) {
				STRUCT_UNIT* pUnit = 0;
				if (pNodeDatabase->GetUnit(pNode, i, &pUnit)) {
					if (UNIT_TYPE_DUOSWITCH == pUnit->bUnitType) {
						createDuoswitchBindings = TRUE;
						break;
					}
				}			
			}
#endif 

		}
		else if (bSoftwareVersion >= 0x90) {
			// NXP: Product code...
			const BYTE bProductCode = pNode->GetProductID();
			if (PRODUCT_ID_HC_BOX == bProductCode) {
				createDimmerBindings = TRUE;
			}		

#if(USE_DUOSWITCH_PBINDINGS_SWITCHTIME == 1)
			if (pNode->GetSoftwareVersionRev() < 0x25) {			
				const int nrOfUnits = pNode->getNROfUnits();
				for (int i = 0; i < nrOfUnits; ++i) {
					STRUCT_UNIT* pUnit = 0;
					if (pNodeDatabase->GetUnit(pNode, i, &pUnit)) {
						if (UNIT_TYPE_DUOSWITCH == pUnit->bUnitType) {
							createDuoswitchBindings = TRUE;
							break;
						}
					}
				}
			}
#endif 
		}
	}

#if(USE_SENSOR_PBINDINGS_OFFSETVALUE == 1)
	if (bSoftwareVersion >= 0x90) {
		const BYTE bProductCode = pNode->GetProductID();
		if (PRODUCT_ID_OLED == bProductCode ||
			PRODUCT_ID_OLED_HOTEL == bProductCode ||
			PRODUCT_ID_OLED_TOUCHBTN_4V == bProductCode ||
			PRODUCT_ID_TCHBTN_4CO1SE == bProductCode ||
			PRODUCT_ID_TCHBTN_1CO1SE == bProductCode ||
			PRODUCT_ID_TCHBTN_2CO1SE == bProductCode ||
			PRODUCT_ID_TCHBTN_8CO1SE == bProductCode ||
			PRODUCT_ID_1BTN_SWITCH_2CO1SE == bProductCode ||
			PRODUCT_ID_2BTN_SWITCH_4CO1SE == bProductCode ||
			PRODUCT_ID_4BTN_SWITCH_8CO1SE == bProductCode ||
			PRODUCT_ID_1BTN_PUSH_1CO1SE	== bProductCode ||
			PRODUCT_ID_2BTN_PUSH_2CO1SE	== bProductCode ||
			PRODUCT_ID_4BTN_PUSH_4CO1SE	== bProductCode)						
		{
			createSensorBindings = TRUE;
		}
	}
#endif 

	if (TRUE==createDimmerBindings) {
		helper_dimmerCreateExtraPBindings(pNodeDatabase,pNode,lastUnitAddressCreateDimmerBindings);
	}

#if(USE_SENSOR_PBINDINGS_OFFSETVALUE == 1)
	if (TRUE == createSensorBindings) {
		helper_sensorCreateExtraPBindings(pNodeDatabase, pNode);
	}
#endif
#if(USE_DUOSWITCH_PBINDINGS_SWITCHTIME == 1)
	if (TRUE == createDuoswitchBindings) {
		helper_duoswitchCreateExtraPBindings(pNodeDatabase, pNode);
	}
#endif  
}


// public

// Iteratie over de nodes.
void bindingpropertyunitalg_upgrade(CNodeDatabase* pNodeDatabase)
{
	CNodeIterator it(pNodeDatabase->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		bindingpropertyunitalg_upgrade(pNode);
	}
}

void bindingpropertyunitalg_upgrade(CCanNode* pNode) {
	REQUIRE(NULL != pNode);
	for (CUnitIterator it2(pNode);!it2.IsDone();it2.Next()) {
		STRUCT_UNIT* const pUnit = it2.CurrentItem();																								
		if(0!=pUnit) {
			CBindingPropertyUnit* const pBindingPropertyUnit= pUnit->m_pBindingUnit;	
			if (0!=pBindingPropertyUnit) {	
				bindingpropertyunitalg_upgrade(pNode,pUnit,pBindingPropertyUnit);
			}
		}
	}
}

// zie USE_DIMMER_PBINDINGS_MINVALUE
void bindingpropertyunitalg_createExtraPBindings(CNodeDatabase* pNodeDatabase,const CList<int>& listNodeAddresses) {
	if (0 == pNodeDatabase || listNodeAddresses.IsEmpty()) {
		ASSERT(0 != pNodeDatabase);
		ASSERT(!listNodeAddresses.IsEmpty());
		return;
	}
	POSITION pos = listNodeAddresses.GetHeadPosition();
	while(pos!=NULL) {
		const int nodeAddress = listNodeAddresses.GetNext(pos);
		if (nodeAddress >= 0x03 && nodeAddress<=0xfc) {
			CCanNode* pNode = 0;
			int dummy = 0;
			if (pNodeDatabase->FindNode(nodeAddress,&dummy,&pNode)) {
				ASSERT(0 != pNode);
				helper_createExtraPBindings(pNodeDatabase,pNode);
			}
			else { ASSERT(0); }
		}
		else { ASSERT(0); }
	}
}


// private:
static BOOL sens_getSoftwareVersionSupportSetpointRange(BYTE bSoftwareVersion,BYTE bSoftwareVersionRev) {
	BOOL result=FALSE;
	if ((bSoftwareVersion>0x90)||((bSoftwareVersion==0x90)&&(bSoftwareVersionRev>=0x21))) {
		result=TRUE;
	}
	return result;
}

// Sedert V16.00: USE_SENSOR_PIDCONTROL	
static BOOL sens_getSoftwareVersionSupportPIDControl(BYTE bSoftwareVersion,BYTE bSoftwareVersionRev) {
	BOOL result=FALSE;
	if ((bSoftwareVersion>0x90)||((bSoftwareVersion==0x90)&&(bSoftwareVersionRev>=0x26))) {
		result=TRUE;
	}
	return result;
} 

static void bindingpropertyunitalg_upgrade(CCanNode* pNode,STRUCT_UNIT* pUnit,CBindingPropertyUnit* pBindingUnit)
{
	BYTE bSoftwareVersion=0xFF;
	BYTE bSoftwareVersionRev=0xFF;
	BYTE bProductID=0xFF;
	BYTE bUnitType=0xFF;

	ASSERT(0!=pNode);
	ASSERT(0!=pUnit);
	ASSERT(0!=pBindingUnit);

	bSoftwareVersion=pNode->GetSoftwareVersion();
	bSoftwareVersionRev=pNode->GetSoftwareVersionRev();
	bUnitType=pBindingUnit->GetBindingUnitType();	// Oppassen: Binding unit type !!!!!!!!!
	bProductID=pNode->GetProductID();

	const unsigned short supportedFlags = pBindingUnit->GetSupportedFlags();
	unsigned short newSupportedFlags = supportedFlags;

	switch(bUnitType)	// Oppassen: Binding unit type !!!!!!!!!
	{
		// Sedert v15.01
		case BINDING_UNIT_TYPE_DUOSWITCH:
		case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
		{
			newSupportedFlags = CfgInfoUnitDuoswitch::getSupportedFlags(pNode,pUnit);		
		} break;

		// Sedert v15.01
		case BINDING_UNIT_TYPE_AVMATRIX:
		case BINDING_UNIT_TYPE_AVMATRIX_INPUT:
		{
			newSupportedFlags = CfgInfoUnitAVMatrix::getSupportedFlags(pNode,pUnit);
	
			CBindingPropertyAVMatrixUnit* const p = dynamic_cast<CBindingPropertyAVMatrixUnit*>(pBindingUnit);
			if (p) {
				CBindingPropertyAVMatrixUnit::AVMatrixType_t type = CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_UNKNOWN;
				CfgInfoUnitAVMatrix::Type_t configType = CfgInfoUnitAVMatrix::getType(pNode,pUnit);
				if (CfgInfoUnitAVMatrix::TYPE_AVMATRIX_AUDAC == configType) {
					type = CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_AUDAC;	
				}
				else if (CfgInfoUnitAVMatrix::TYPE_AVMATRIX_INTEGRA == configType) {
					type = CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_INTEGRA;
				}
				p->setMatrixType(type);
			}
		} break;

		case BINDING_UNIT_TYPE_SENS:
		{			
			BOOL fSupportDelegation=FALSE;
			BOOL fSupportDelegate=FALSE;
			BOOL fSupportDelegator=FALSE;
			BOOL fSupportRequestValues=FALSE;
			BOOL fSupportSetpointRange=FALSE;
			BOOL fSupportPIDControl = FALSE;

			switch(bProductID)
			{
				case PRODUCT_ID_OLED_HOTEL:
				case PRODUCT_ID_OLED_TOUCHBTN_4V:
				{					
					// De Oled is de delegate: Deze meet de waarde en doet de sturing.
					// Er kan hieraan een delegator gekoppeld worden.
					if ((bSoftwareVersion>0x90) || ((bSoftwareVersion==0x90)&&(bSoftwareVersionRev>=0x14))) {
						fSupportDelegation=TRUE;
						fSupportDelegator=TRUE;
					}					
					fSupportSetpointRange=sens_getSoftwareVersionSupportSetpointRange(bSoftwareVersion,bSoftwareVersionRev);
					fSupportPIDControl = sens_getSoftwareVersionSupportPIDControl(bSoftwareVersion,bSoftwareVersionRev);
				} break;

				// Sedert versie 0x0C04 - Vanaf firmware V90.17.
				// De Oled is de delegate: Deze meet de waarde en doet de sturing.
				// Er kan hieraan een delegator gekoppeld worden.
				case PRODUCT_ID_OLED:
				{
					if ((bSoftwareVersion>0x90) || ((bSoftwareVersion==0x90)&&(bSoftwareVersionRev>=0x17))) {
						fSupportDelegation=TRUE;
						fSupportDelegator=TRUE;
					}
					fSupportSetpointRange=sens_getSoftwareVersionSupportSetpointRange(bSoftwareVersion,bSoftwareVersionRev);
					fSupportPIDControl = sens_getSoftwareVersionSupportPIDControl(bSoftwareVersion,bSoftwareVersionRev);
				} break;

				// Sedert versie 0x0C04 - Vanaf firmware V90.17.
				// De touchbutton is de delegate: Deze meet de waarde en doet de sturing.
				// Er kan hieraan een delegator gekoppeld worden.
				case PRODUCT_ID_TCHBTN_4CO1SE:
				case PRODUCT_ID_TCHBTN_1CO1SE:
				case PRODUCT_ID_TCHBTN_2CO1SE:
				case PRODUCT_ID_TCHBTN_8CO1SE:
				case PRODUCT_ID_1BTN_SWITCH_2CO1SE:
				case PRODUCT_ID_2BTN_SWITCH_4CO1SE:
				case PRODUCT_ID_4BTN_SWITCH_8CO1SE:
				case PRODUCT_ID_1BTN_PUSH_1CO1SE:
				case PRODUCT_ID_2BTN_PUSH_2CO1SE:
				case PRODUCT_ID_4BTN_PUSH_4CO1SE:
				{
					if ((bSoftwareVersion>0x90) || ((bSoftwareVersion==0x90)&&(bSoftwareVersionRev>=0x17))) {
						fSupportDelegation=TRUE;
						fSupportDelegator=TRUE;
					}
					fSupportSetpointRange=sens_getSoftwareVersionSupportSetpointRange(bSoftwareVersion,bSoftwareVersionRev);
					fSupportPIDControl = sens_getSoftwareVersionSupportPIDControl(bSoftwareVersion,bSoftwareVersionRev);
				} break;

				case PRODUCT_ID_HC_BOX:
				{
					// De Smartbox+ is de weergave in de sbox+ app. (+ kan gebruikt worden in de tijdsschemmas).
					// Er kan hieraan een delegate gekoppeld worden.
					if ((bSoftwareVersion>0x90) || ((bSoftwareVersion==0x90)&&(bSoftwareVersionRev>=0x14))) {
						fSupportDelegation=TRUE;
						fSupportDelegate=TRUE;
					}
					fSupportSetpointRange=sens_getSoftwareVersionSupportSetpointRange(bSoftwareVersion,bSoftwareVersionRev);
					fSupportPIDControl = sens_getSoftwareVersionSupportPIDControl(bSoftwareVersion,bSoftwareVersionRev);
				} break;

				// Sedert V16.33: Integratie Smartbox ultra
				// Enkel de interne sensor units ondersteunen Setpoint range + PID control.
				case PRODUCT_ID_HC_BOX_ULTRA: {		
					fSupportDelegation = FALSE;
					fSupportDelegate = FALSE;
					if (pUnit->bUnitAddress <= PRODUCT_SBOXULTRA_UNITADDRESS_INTERNAL_LAST) {
						fSupportSetpointRange = TRUE;
						fSupportPIDControl = TRUE;
						fSupportRequestValues=FALSE;
					}
					else {
						// Virtuele sensor.
						fSupportRequestValues=TRUE;
					}
			} break;

				case PRODUCT_ID_GATEWAY_MODBUS_16SE32VIRT:
				{
					// Modbus sensor unit vanaf v65.54(.04)
					// Aan deze sensor unit kan een andere sensor gekoppeld worden waarvan de gemeten waarde
					// opgevraagd wordt.
					if ((bSoftwareVersion>=0x65)&&(bSoftwareVersionRev>=0x54)) {
						fSupportRequestValues=TRUE;
					}
				} break;

				case PRODUCT_ID_STD_ANALOG_8SENS:	// Sedert V16.01 (bug) : zie WORKAROUND_DT13_ANA nodig
				{
					fSupportSetpointRange = sens_getSoftwareVersionSupportSetpointRange(bSoftwareVersion,bSoftwareVersionRev);
					fSupportPIDControl = sens_getSoftwareVersionSupportPIDControl(bSoftwareVersion,bSoftwareVersionRev);
				} break;

				case PRODUCT_ID_TFT_32VIRT:
				case PRODUCT_ID_GATEWAY_HS:
				case PRODUCT_ID_GATEWAY_HS_32VIRT:
				case PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT:			
				{
					// Virtuele sensor unit sinds v66.04.00
					if ((bSoftwareVersion==0x66)&&(bSoftwareVersionRev>=0x04)) {
						fSupportRequestValues=TRUE;
					}
				} break;

				default:
				{
					// Sedert V14.13 : WORKAROUND_DT13_ANA
					// Probleem: 
					// de DT13-ANA ondersteund vanaf V90.21 wel de setpoint range.
					// Deze node verstuurd geen product code en ook geen minor software versie.
					// De node bestaat altijd uit 8sensors en 12 virtuals.
					// Het physical address = 0x0013xxxx
					// Oppassen:
					// Kunnen geen iteratie doen van de units in de nodedatabase omdat dit gebeurd bij het inladen van het document..
					// Daarom controle op het physical address dat deze altijd start met 0x13xxxx

					// Probleem: Weten de minor firmware versie niet daarom zal deze mogelijkheid ook getoond worden waar het nog niet mogelijk was...

					if (bSoftwareVersion>=0x90) {		
						const int nrOfUnits = pNode->getNROfUnits();
						if (20 == nrOfUnits) {
							if ((pNode->getPhysicalAddress() & 0x00FF0000) == 0x00130000) {
								fSupportSetpointRange=TRUE;
							} 
						}
					}
				} break;

			} /* end switch( ) */

			helperflags_set(
					&newSupportedFlags,
				    CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION,
					(TRUE==fSupportDelegation ? 1 : 0));

			helperflags_set(
					&newSupportedFlags,
				    CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_ATTACH_DELEGATE,
					(TRUE==fSupportDelegate ? 1 : 0));

			helperflags_set(
					&newSupportedFlags,
				    CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_ATTACH_DELEGATOR,
					(TRUE==fSupportDelegator ? 1 : 0));

			// serdert versie 0x0C03
			helperflags_set(
					&newSupportedFlags,
					CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE,
					(TRUE==fSupportRequestValues ? 1 : 0));

			// sedert versie 0x0E00
			helperflags_set(
				&newSupportedFlags,
				CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_SETPOINTRANGE,
				(TRUE==fSupportSetpointRange ? 1 : 0));

			// Sedert V16.00: USE_SENSOR_PIDCONTROL	
			helperflags_set(
				&newSupportedFlags,
				CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_PIDCONTROL,
				(TRUE==fSupportPIDControl ? 1 : 0));
		
		} break; /* case UNIT_TYPE_SENS: */
 
		default:
		{
			// absorbe...
		} break;
	} /* end switch(bUnitType) */

	// support vlaggen aanpassen.
	if (newSupportedFlags != supportedFlags)
	{
		pBindingUnit->SetSupportedFlags(newSupportedFlags);						
	}
}