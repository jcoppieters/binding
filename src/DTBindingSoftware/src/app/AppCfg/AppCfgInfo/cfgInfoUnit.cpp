#include "stdafx.h"
#include "cfginfounit.h"
#include "cfginfoProductID_def.h"

#include "app\appbindings\appbindingproperty\BindingPropertyUnit.h"
#include "CanNode.h"

/***********************************************************/
#include "nodedatabase.h"
// #include "tmglobals.h"				// Nodig voor de sens unit DT13-ANA
// using namespace TMNodeDatabase;		// Nodig voor de sens unit DT13-ANA
/***********************************************************/

// prototypes.
static 
BOOL getBindingsForwardSupported(
	const CCanNode* pNode,
	const STRUCT_UNIT* pUnit);



// public:

BOOL CCfgInfoUnit::isSupported(
    const CCanNode* pNode,
	const STRUCT_UNIT* pUnit,
	CfgInfoUnitFlags_t flag)
{
	BOOL fResult=FALSE;
	BOOL (*pf)(const CCanNode*,const STRUCT_UNIT*)=0;
	
	switch(flag)
	{
		case FLAG_SUPPORTED_BINDINGS_FORWARD:
		{
			pf=getBindingsForwardSupported;
		} break;

		default:
		{
			pf=0;
		} break;
	} /* switch (flag) */

	if (0!=pf)
	{
		fResult=pf(pNode,pUnit);
	}
	return fResult;
}


unsigned short CfgInfoUnitDim::getSupportedFlags(
	const CCanNode* pNode,
	const STRUCT_UNIT* pUnit)
{
	unsigned short usSupportedFlags=0;

	REQUIRE((pNode!=0)&&(pUnit!=0));
	REQUIRE(pUnit->bUnitType==UNIT_TYPE_DIM);

	if ((pNode!=0) && (pUnit!=0) && (pUnit->bUnitType==UNIT_TYPE_DIM))
	{
		const BYTE productID = pNode->GetProductID( );
		
		switch(productID)
		{
			case PRODUCT_ID_STD_DALI_MASTER_DIM:
			{
				// In principe moeten hier controleren dat de dimmerunit idx < 64 is.
				// Aangezien dit product enkel uit dimmers bestaat is dit geen probleem.

				if (pUnit->bUnitAddress<64)
				{
					usSupportedFlags |= CBindingPropertyDimUnit::DIM_SUPPORT_FLAGS_DALI_DEVICE;
				}
				else 
				{
					usSupportedFlags |= CBindingPropertyDimUnit::DIM_SUPPORT_FLAGS_DALI_GROUP;
				}
			} break; /* PRODUCT_ID_STD_DALI_MASTER_DIM */

			default: 
			{
				usSupportedFlags = 0;
			} break;
		}
	}
	return usSupportedFlags;
}


unsigned short CfgInfoUnitSens::getSupportedFlags(const CCanNode* pNode,const STRUCT_UNIT* pUnit)
{
	unsigned short usSupportedFlags = 0;

	REQUIRE((pNode!=0)&&(pUnit!=0));
	REQUIRE(pUnit->bUnitType==UNIT_TYPE_SENS);

	if ((pNode!=0)&&(pUnit!=0)&&(pUnit->bUnitType==UNIT_TYPE_SENS))
	{
		const BYTE productID = pNode->GetProductID( );
		const int swVersionMajor = pNode->GetSoftwareVersion( );
		const int swVersionMinor = pNode->GetSoftwareVersionRev( );

		BOOL fSupportDelegation=FALSE;
		BOOL fSupportDelegate=FALSE;
		BOOL fSupportDelegator=FALSE;
		BOOL fSupportRequestValues=FALSE;
		BOOL fSupportSetpointRange=FALSE;
		BOOL fSupportPIDControl = FALSE;

		switch(productID)
		{
			// Sedert versie 0x0C04 - Vanaf firmware V90.17.
			// De Oled is de delegate: Deze meet de waarde en doet de sturing.
			// Er kan hieraan een delegator gekoppeld worden.
			case PRODUCT_ID_OLED:
			{
				if ((swVersionMajor>0x90) || ((swVersionMajor==0x90)&&(swVersionMinor>=0x17))) {
					fSupportDelegation=TRUE;
					fSupportDelegator=TRUE;
				}
				fSupportSetpointRange=getSoftwareVersionSupportSetpointRange(swVersionMajor,swVersionMinor);
				fSupportPIDControl=getSoftwareVersionSupportPIDControl(swVersionMajor,swVersionMinor);

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
				if ((swVersionMajor>0x90)||((swVersionMajor==0x90)&&(swVersionMinor>=0x17))) {
					fSupportDelegation=TRUE;
					fSupportDelegator=TRUE;
				}
				fSupportSetpointRange=getSoftwareVersionSupportSetpointRange(swVersionMajor,swVersionMinor);
				fSupportPIDControl=getSoftwareVersionSupportPIDControl(swVersionMajor,swVersionMinor);
			} break;

			case PRODUCT_ID_STD_ANALOGIN_8UNIVSE8CO: {
				// Absorbe:
				// Geen support voor delegation
			} break;

			case PRODUCT_ID_OLED_HOTEL:
			case PRODUCT_ID_OLED_TOUCHBTN_4V: {
				// De Oled is de delegate: Deze meet de waarde en doet de sturing.
				// Er kan hieraan een delegator gekoppeld worden.
				if ((swVersionMajor>0x90)||((swVersionMajor==0x90)&&(swVersionMinor>=0x14))) {
					fSupportDelegation=TRUE;
					fSupportDelegator=TRUE;
				}

				fSupportSetpointRange=getSoftwareVersionSupportSetpointRange(swVersionMajor,swVersionMinor);
				fSupportPIDControl=getSoftwareVersionSupportPIDControl(swVersionMajor,swVersionMinor);
			} break; /* PRODUCT_ID_OLED_HOTEL */

			case PRODUCT_ID_HC_BOX:
			{
				// De Smartbox+ is de weergave in de sbox+ app. (+ kan gebruikt worden in de tijdsschemmas).
				// Er kan hieraan een delegate gekoppeld worden.
				if ((swVersionMajor>0x90)||((swVersionMajor==0x90)&&(swVersionMinor>=0x14))) {
					fSupportDelegation=TRUE;
					fSupportDelegate=TRUE;
				} 
				fSupportSetpointRange=getSoftwareVersionSupportSetpointRange(swVersionMajor,swVersionMinor);
				fSupportPIDControl=getSoftwareVersionSupportPIDControl(swVersionMajor,swVersionMinor);
			} break; /* PRODUCT_ID_HC_BOX */

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
			
			case PRODUCT_ID_STD_ANALOG_8SENS:	// Sedert V16.01 (Bug) : zie WORKAROUND_DT13_ANA nodig
			{
				fSupportSetpointRange=getSoftwareVersionSupportSetpointRange(swVersionMajor,swVersionMinor);
				fSupportPIDControl=getSoftwareVersionSupportPIDControl(swVersionMajor,swVersionMinor);
			} break;
			
			// Sedert versie 0x0C03:
			// Aan een modbus sensor unit kan een andere sensor unit gekoppeld worden die de temperatuur
			// meet.
			case PRODUCT_ID_GATEWAY_MODBUS_16SE32VIRT:
			{
				// Modbus sensor unit vanaf v65.54(.04)
				if ((swVersionMajor>=0x65)&&(swVersionMinor>=0x54)) {
					fSupportRequestValues=TRUE;
				}
			} break;

			case PRODUCT_ID_TFT_32VIRT:
			case PRODUCT_ID_GATEWAY_HS:
			case PRODUCT_ID_GATEWAY_HS_32VIRT:
			case PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT:
			// case PRODUCT_ID_HC_BOX_ULTRA:						// Sedert V16.32
			{
				// Virtuele sensor unit sinds v66.04.00
				if ((swVersionMajor==0x66)&&(swVersionMinor>=0x04)) {
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

				if (swVersionMajor>=0x90) {		
					const int nrOfUnits = pNode->getNROfUnits();
					if (20 == nrOfUnits) {
						if ((pNode->getPhysicalAddress() & 0x00FF0000) == 0x00130000) {
							fSupportSetpointRange=TRUE;
						} 
					}
				}
			} break;
		} /* switch(productID) */	

		if (TRUE==fSupportDelegation) {
			usSupportedFlags |= CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION;

			if (TRUE==fSupportDelegate) {
				usSupportedFlags |= CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_ATTACH_DELEGATE;	
			}

			if (TRUE==fSupportDelegator) {
				usSupportedFlags |= CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_ATTACH_DELEGATOR;				
			}

			// Als delegation ondersteund is dan moet altijd de het type enabled zijn.
			ASSERT((usSupportedFlags&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION)&&
				   ((usSupportedFlags&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_ATTACH_DELEGATE)||
				    (usSupportedFlags&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_ATTACH_DELEGATOR)));		
		}

		if (TRUE==fSupportRequestValues) {
			usSupportedFlags |= CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE;	

			// Indien request measured value ondersteund is mag geen andere vorm van delegation
			// ondersteund zijn.
			ASSERT(!(usSupportedFlags&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION));
		}
		if (TRUE == fSupportSetpointRange) {
			usSupportedFlags |= CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_SETPOINTRANGE;	
		}			
		if (TRUE == fSupportPIDControl) {
			usSupportedFlags |= CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_PIDCONTROL;	
		}
	}
	return usSupportedFlags;
}

BOOL CfgInfoUnitSens::getSoftwareVersionSupportSetpointRange( BYTE bSoftwareVersion,BYTE bSoftwareVersionRev) {
	BOOL result=FALSE;
	if ((bSoftwareVersion>0x90)|| ((bSoftwareVersion==0x90)&&(bSoftwareVersionRev>=0x21))) {
		result=TRUE;
	}
	return result;
}

// Sedert V16.00: USE_SENSOR_PIDCONTROL	
BOOL CfgInfoUnitSens::getSoftwareVersionSupportPIDControl(BYTE bSoftwareVersion,BYTE bSoftwareVersionRev) {
	BOOL result=FALSE;
	if ((bSoftwareVersion>0x90)|| ((bSoftwareVersion==0x90)&&(bSoftwareVersionRev>=0x26))) {
		result=TRUE;
	}
	return result;
} 

// Sedert versie 0x0D00
CfgInfoUnitSens::Type_t CfgInfoUnitSens::getType(
	const CCanNode* pNode,
	const STRUCT_UNIT* pUnit)
{
	CfgInfoUnitSens::Type_t type = TYPE_SENSOR_NORMAL;

	REQUIRE((pNode!=0)&&(pUnit!=0));
	REQUIRE(pUnit->bUnitType==UNIT_TYPE_SENS);

	if ((pNode!=0)&&(pUnit!=0)&&(pUnit->bUnitType==UNIT_TYPE_SENS))
	{
		const BYTE productID = pNode->GetProductID( );

		if (PRODUCT_ID_STD_ANALOGIN_8UNIVSE8CO==productID)
		{
			type = TYPE_SENSOR_UNIVERSAL;
		}
	}
	return type;
}

// Rolluik unit vanaf V90.24: Mogelijkheid om stoptijd waarde op 0 te zetten.
// De relais van de rolluik unit blijft dan actief en zal niet uitvallen.
unsigned short CfgInfoUnitDuoswitch::getSupportedFlags(
		const CCanNode* pNode,
		const STRUCT_UNIT* pUnit)
{
	unsigned short usSupportedFlags = 0;	
	if (pNode && pUnit) 
	{	
		const int swVersionMajor = pNode->GetSoftwareVersion( );
		const int swVersionMinor = pNode->GetSoftwareVersionRev( );

		if ((0x90==swVersionMajor)&&(swVersionMinor>=0x24))
		{
			const BYTE productID = pNode->GetProductID( );
			switch(productID)
			{
				case PRODUCT_ID_HC_BOX:
				case PRODUCT_ID_STD_6DUOSWITCH:				
					usSupportedFlags += FLAG_SUPPORTED_STOPTIME_NONE;	
					usSupportedFlags += FLAG_SUPPORTED_STOPTIME_LONGERVALUES;
				break;				
				default: /* absorbe */ break;
			}
		}
	}
	return usSupportedFlags;
}



// private:
static 
BOOL getBindingsForwardSupported(
	const CCanNode* pNode,
	const STRUCT_UNIT* pUnit)
{
	BOOL fResult=FALSE;
	const BYTE bUnitType=pUnit->bUnitType;

	switch(bUnitType)
	{
		case UNIT_TYPE_DIM:
		case UNIT_TYPE_SWITCH:
		case UNIT_TYPE_DUOSWITCH:
		{
			const int swVersionMajor = pNode->GetSoftwareVersion( );
			const int swVersionMinor = pNode->GetSoftwareVersionRev( );

			// Nxp Modules vanaf V90.16 ifv hun product id.
			if ((swVersionMajor>0x90)|| ((swVersionMajor==0x90)&&(swVersionMinor>=0x16)))
			{
				const BYTE bProductID = pNode->GetProductID();
				if ((PRODUCT_ID_HC_BOX==bProductID)||
					(PRODUCT_ID_STD_DMX_MASTER_64DIM==bProductID)||
					(PRODUCT_ID_STD_DALI_MASTER_DIM==bProductID)||
					(PRODUCT_ID_STD_12SWITCH==bProductID)||
					(PRODUCT_ID_STD_6DUOSWITCH==bProductID)||
					(PRODUCT_ID_STD_6DIM==bProductID))
				{				
					fResult=TRUE;
				}
			}
			// RPI modules vanaf V66.04.xx ifv hun product id.
			// Sedert V16.32: Sbox ultra ingevoegd.
			else if (swVersionMajor==0x66 && swVersionMinor>=0x04) 
			{
				const BYTE bProductID = pNode->GetProductID();
				if ((PRODUCT_ID_TFT_32VIRT == bProductID)||
					(PRODUCT_ID_GATEWAY_HS == bProductID)||
					(PRODUCT_ID_GATEWAY_HS_32VIRT == bProductID) ||
					(PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT == bProductID) ||
					(PRODUCT_ID_HC_BOX_ULTRA == bProductID))	
				{
					fResult=TRUE;
				}
			}
		} break;

		default:
		{		
		} break;
	}

	return fResult;
}

/* CfgInfoUnitAVMatrix */

unsigned short CfgInfoUnitAVMatrix::getSupportedFlags(
	const CCanNode* pNode,
	const STRUCT_UNIT* pUnit)
{
	unsigned short usSupportedFlags = 0;
	const CfgInfoUnitAVMatrix::Type_t type = getType(pNode,pUnit);
	if (TYPE_AVMATRIX_AUDAC == type || TYPE_AVMATRIX_INTEGRA == type) {
		usSupportedFlags |= FLAG_SUPPORTED_CONFIG_SUBTYPE;
	}
	return usSupportedFlags;
}

CfgInfoUnitAVMatrix::Type_t CfgInfoUnitAVMatrix::getType(
	const CCanNode* pNode,
	const STRUCT_UNIT* pUnit)
{
	Type_t type = TYPE_AVMATRIX_UNKNOWN;
	const int productID = pNode->GetProductID();
	switch(productID) {
		case PRODUCT_ID_8CHIRTX_AVAUDAC:
			type = TYPE_AVMATRIX_AUDAC;
		break;
		case PRODUCT_ID_8CHIRTX_AVINTEGRA:
			type = TYPE_AVMATRIX_INTEGRA;
		break;
	}	
	return type;
}

/* CfgInfoUnitAVMatrix - end */
 