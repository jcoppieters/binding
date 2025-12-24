// BindingPropertyUnitConvert.cpp: implementation of the CBindingPropertyUnitConvert class.
//
// TM20120406: Conversie van de Sensor Unit: Property bindings voor de 
//			   Measured offset value.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingPropertyUnitConvert.h"

#include "app\appbindings\appbindingunit\bindingunit_api.h"

#include "BindingPropertyUnit.h"
#include "BindingPropertyUnitDefault.h"
#include "BindingPropertyUnitDefaultFactory.h"

#include "LogFileConversionFromNodes.h"

#include "bindingconstants.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingPropertyUnitConvert::CBindingPropertyUnitConvert(const CBindingUnit* const pBindingUnit,
														 CBindingPropertyUnit* const pBindingPropertyUnit) :
m_pBindingUnit( pBindingUnit ),
m_pBindingPropertyUnit( pBindingPropertyUnit ),
m_pLogFile( CLogFile::GetConversionFromNodes() )
{
	ASSERT( m_pBindingUnit );
	ASSERT( m_pBindingPropertyUnit );
	ASSERT( m_pLogFile );
}

CBindingPropertyUnitConvert::~CBindingPropertyUnitConvert()
{
}

///////////////////////////////////////////////////////////////////////////////////

/**
 * Factory ...
 */
CBindingPropertyUnitConvert* CBindingPropertyUnitConvert::Create(
	const CBindingUnit* const pBindingUnit,
 	CBindingPropertyUnit* const pBindingPropertyUnit)
{
	if (pBindingUnit == 0 || pBindingPropertyUnit == 0) 
	{
		ASSERT( pBindingUnit );
		ASSERT( pBindingPropertyUnit );
		return 0;
	}

	CBindingPropertyUnitConvert* pNew = 0;
	
	try
	{
		switch (pBindingUnit->GetBindingUnitType())
		{
			case BINDING_UNIT_TYPE_DIM:
			case BINDING_UNIT_TYPE_DIM_FWD:
			{
				pNew = new CBindingPropertyUnitConvertDim(pBindingUnit,pBindingPropertyUnit);
			} break;		

			case BINDING_UNIT_TYPE_SWITCH:
			case BINDING_UNIT_TYPE_SWITCH_FWD:
			{
				pNew = new CBindingPropertyUnitConvertSwitch(pBindingUnit,pBindingPropertyUnit);
			} break;

			case BINDING_UNIT_TYPE_DUOSWITCH:
			case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
			{
				pNew = new CBindingPropertyUnitConvertDuoswitch(pBindingUnit,pBindingPropertyUnit);				
			} break;

			case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */	/* TODO !!!!!!!!!!!!!!!!*/
			case BINDING_UNIT_TYPE_SENS:
			{
				pNew = new CBindingPropertyUnitConvertSens(pBindingUnit,pBindingPropertyUnit);
			} break;
			
			case BINDING_UNIT_TYPE_ALARM_GALAXY:
			case BINDING_UNIT_TYPE_ALARM_ARITECH:	
			case BINDING_UNIT_TYPE_ALARM_ANB:		/* TM_CHANGES_ALARM_ANB */
			case BINDING_UNIT_TYPE_ALARM_TEXECOM:	/* TM_CHANGES_ALARM_TEXECOM */
			case BINDING_UNIT_TYPE_ALARM_INTEGRA:	/* TM_CHANGES_ALARM_INTEGRA */
			{
				pNew = new CBindingPropertyUnitConvertAlarm(pBindingUnit,pBindingPropertyUnit);			
			} break;
			case BINDING_UNIT_TYPE_AVMATRIX:
			{
				pNew = new CBindingPropertyUnitConvertAVMatrix(pBindingUnit,pBindingPropertyUnit);		
			} break;
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return 0;
	}

	return pNew;
}

///////////////////////////////////////////////////////////////////////////////////

BOOL CBindingPropertyUnitConvert::_Update()
{
	return FALSE;
}

BOOL CBindingPropertyUnitConvert::Update()
{
	if (m_pBindingUnit == 0 || m_pBindingPropertyUnit == 0)
	{
		ASSERT( m_pBindingUnit );
		ASSERT( m_pBindingPropertyUnit );	
		return FALSE;
	}

	return (_Update());
}


////////////////////////////////////////////////////////////////////////////
/**
 * CBindingPropertyUnitConvertDim ...
 */
////////////////////////////////////////////////////////////////////////////
BOOL CBindingPropertyUnitConvertDim::_Update()
{
	/**
	 * Controlleren of Types : Unit & Property unit = correcte type !!!	 
	 */
	CBindingPropertyDimUnit* const pBindingUnitPropertyDim = 
		static_cast<CBindingPropertyDimUnit*>(m_pBindingPropertyUnit);

	const CBindingDimmerUnit* const pBindingUnit = 
		static_cast<const CBindingDimmerUnit*>(m_pBindingUnit);


	if (pBindingUnitPropertyDim != 0)
	{
		// TM 20130328:
		// Structuur gebruiken om de params mee te geven aan de factory.
		// Binding unit type meegeven omdat de unit mogelijk nog niet aanwezig is 
		// in de nodedatabase.
		const CBindingPropertyUnitDefaultFactory::CUnitInfo unitInfo =
		{
			pBindingUnit->GetNodeAddress(),
			pBindingUnit->GetUnitAddress(),
			pBindingUnit->GetBindingUnitType()			// Binding unit type.
		};

		// TM 20080122: Moeten het node adres en unit adres meegeven
		// om de juiste standaard waarden te verkrijgen.
		const CBindingPropertyUnitDimDefault* const pDefaults = 
			static_cast<const CBindingPropertyUnitDimDefault* const>
				(CBindingPropertyUnitDefaultFactory::Instance()->
					Create(UNIT_TYPE_DIM,unitInfo));						 
		
		if (pDefaults == 0) 
		{
			ASSERT ( pDefaults );

			return FALSE;
		}

		if (pDefaults->GetSpeedDown() != pBindingUnit->Get_SpeedDown() ||
			pDefaults->GetSpeedUp() != pBindingUnit->Get_SpeedUp()) 
		{		
			pBindingUnitPropertyDim->UpdateSpeed(pBindingUnit->Get_SpeedDown(),
												 pBindingUnit->Get_SpeedUp());

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update Speed",	pBindingUnitPropertyDim );	
			}

			return TRUE;	
		}
		else if (pDefaults->GetMaxRange() != pBindingUnit->Get_MaxRange() ||
				 pDefaults->GetMinRange() != pBindingUnit->Get_MinRange())
		{		
			pBindingUnitPropertyDim->UpdateRange(pBindingUnit->Get_MinRange(),
												 pBindingUnit->Get_MaxRange());

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update Range",	pBindingUnitPropertyDim );	
			}

			return TRUE;
		}
		else if (pDefaults->GetConfig() != pBindingUnit->Get_Config())
		{		
			pBindingUnitPropertyDim->UpdateConfig(pBindingUnit->Get_Config());

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update Config", pBindingUnitPropertyDim );	
			}

			return TRUE;
		}		
		// Sedert vesrie 0x0B00
		else if (pBindingUnit->Get_AddressCfgType()!=0xFF)
		{
			pBindingUnitPropertyDim->UpdateCfgAddress( 
				pBindingUnit->Get_AddressCfgType(),
				pBindingUnit->Get_AddressCfgMSB(),
				pBindingUnit->Get_AddressCfgLSB());

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update Address Cfg", pBindingUnitPropertyDim );	
			}
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CBindingPropertyUnitConvertSwitch::_Update()
{
	/**
	 * Controlleren of Types : Unit & Property unit = correcte type !!!	 
	 */
	CBindingPropertySwitchUnit* const pBindingUnitPropertySwitch = 
		static_cast<CBindingPropertySwitchUnit*>(m_pBindingPropertyUnit);

	const CBindingRelaisUnit* const pBindingUnit = 
		static_cast<const CBindingRelaisUnit*>(m_pBindingUnit);


	if (pBindingUnitPropertySwitch != 0)
	{
		const CBindingPropertyUnitSwitchDefault* const pDefaults = 
			static_cast<const CBindingPropertyUnitSwitchDefault* const>
				(CBindingPropertyUnitDefaultFactory::Instance()->
					Create(UNIT_TYPE_SWITCH));
		
		if (pDefaults == 0) 
		{
			ASSERT ( pDefaults );
			return FALSE;
		}

		if (pDefaults->GetPrecisionTimed() == FALSE && pBindingUnit->Get_StopTime() != 0)
		{
			pBindingUnitPropertySwitch->UpdatePrecisionTiming(TRUE,	pBindingUnit->Get_StopTime());

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update Precision Timed", pBindingUnitPropertySwitch );	
			}

			return TRUE;
		}
	}
	return FALSE;
}

BOOL CBindingPropertyUnitConvertDuoswitch::_Update()
{
	/**
	 * Controlleren of Types : Unit & Property unit = correcte type !!!	 
	 */
	CBindingPropertyDuoSwitchUnit* const pBindingUnitPropertyDuoswitch = 
		static_cast<CBindingPropertyDuoSwitchUnit*>(m_pBindingPropertyUnit);

	const CBindingDuoSwitchUnit* const pBindingUnit = 
		static_cast<const CBindingDuoSwitchUnit*>(m_pBindingUnit);


	if (pBindingUnitPropertyDuoswitch != 0)
	{
		const CBindingPropertyUnitDuoswitchDefault* const pDefaults = 
			static_cast<const CBindingPropertyUnitDuoswitchDefault* const>
				(CBindingPropertyUnitDefaultFactory::Instance()->
					Create(UNIT_TYPE_DUOSWITCH));
		
		if (pDefaults == 0) 
		{
			ASSERT ( pDefaults );
			return FALSE;
		}

		if ( pDefaults->GetStopTime() != pBindingUnit->Get_StopTime() )
		{
			pBindingUnitPropertyDuoswitch->UpdateStopTime( pBindingUnit->Get_StopTime() );

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update stoptime", pBindingUnitPropertyDuoswitch );	
			}

			return TRUE;
		}
		else if ( pDefaults->GetSwitchTime() != pBindingUnit->Get_SwitchTime() )
		{
			pBindingUnitPropertyDuoswitch->UpdateSwitchTime( pBindingUnit->Get_SwitchTime() );

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update switchtime", pBindingUnitPropertyDuoswitch );	
			}

			return TRUE;
		}
	}
	return FALSE;
}

BOOL CBindingPropertyUnitConvertSens::_Update()
{
	/**
	 * Controlleren of Types : Unit & Property unit = correcte type !!!	 
	 */
	CBindingPropertySensUnit* const pBindingUnitPropertySens= 
		static_cast<CBindingPropertySensUnit*>(m_pBindingPropertyUnit);

	const CBindingSensUnit* const pBindingUnit = 
		static_cast<const CBindingSensUnit*>(m_pBindingUnit);


	if (pBindingUnitPropertySens != 0)
	{
		// TM 20130328:
		// Structuur gebruiken om de params mee te geven aan de factory.
		// Binding unit type meegeven omdat de unit mogelijk nog niet aanwezig is 
		// in de nodedatabase.
		const CBindingPropertyUnitDefaultFactory::CUnitInfo unitInfo =
		{
			pBindingUnit->GetNodeAddress(),
			pBindingUnit->GetUnitAddress(),
			pBindingUnit->GetBindingUnitType()			// Binding unit type.
		};

		const CBindingPropertyUnitSensDefault* const pDefaults = 
			static_cast<const CBindingPropertyUnitSensDefault* const>
				(CBindingPropertyUnitDefaultFactory::Instance()->
					Create(UNIT_TYPE_SENS,unitInfo));						 			
		
		if (pDefaults == 0) 
		{
			ASSERT ( pDefaults );
			return FALSE;
		}

		if (pDefaults->GetSampleTime() != pBindingUnit->Get_SampleTime())
		{
			pBindingUnitPropertySens->UpdateSampleTime( pBindingUnit->Get_SampleTime() );

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update sample time", pBindingUnitPropertySens );	
			}

			return TRUE;
		}
		else if ((pDefaults->GetHysOffsetHighValue() != pBindingUnit->Get_HysHighOffsetValue()) ||
				 (pDefaults->GetHysOffsetLowValue() != pBindingUnit->Get_HysLowOffsetValue()))
		{
			pBindingUnitPropertySens->UpdateOffsetValues(pBindingUnit->Get_HysHighOffsetValue(),
														 pBindingUnit->Get_HysLowOffsetValue());

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update offset values", pBindingUnitPropertySens );	
			}

			return TRUE;
		}
		else if (pDefaults->GetMeasuredValueOffset() != pBindingUnit->GetMeasuredValueOffset())
		{
			/* TM20120405: Property bindings voor de Offset waarde. */
			pBindingUnitPropertySens->UpdateMeasuredValueOffset(pBindingUnit->GetMeasuredValueOffset());

			if (m_pLogFile != 0)  {
				m_pLogFile->Success("Update measured value offset", pBindingUnitPropertySens );	
			}
			return TRUE;
		}
		else
		{
			{
				// zie: USE_SENSOR_PIDCONTROL

				// Oppassen: Standaard is de workingsmode -1.
				// Indien er bindings aanwezig zijn voor de controlUnit of voor de params dan zal de workingsmode een
				// geldige (!=-1) param hebben.				
				const int workingsMode = pBindingUnit->getPIDWorkingMode();
				if (SENS_PIDCONTROL_WORKINGMODE_COOLING == workingsMode ||  SENS_PIDCONTROL_WORKINGMODE_HEATING== workingsMode) 
				{
					{
						int nodeAddress = 0xff;
						int unitAddress = 0xff;
						if (pBindingUnit->getPIDControlUnit(workingsMode,&nodeAddress,&unitAddress)) {
							if ((0xff != nodeAddress) && (0xff != unitAddress)) {
								pBindingUnitPropertySens->setPropertyPIDControlUnit(workingsMode,nodeAddress,unitAddress);
								if (m_pLogFile != 0) {
									m_pLogFile->Success("Update PID-Control unit", pBindingUnitPropertySens );	
								}
								return TRUE;
							}
						}
					}

					{
						int kp=-1;
						int ki=-1;
						int kd=-1;
						if (pBindingUnit->getPIDControlParams(workingsMode,&kp,&ki,&kd)) {
							if ((pDefaults->getPIDControlParam(workingsMode,SENS_PIDCONTROL_PARAM_KP) != kp) ||
								(pDefaults->getPIDControlParam(workingsMode,SENS_PIDCONTROL_PARAM_KI) != ki) ||
								(pDefaults->getPIDControlParam(workingsMode,SENS_PIDCONTROL_PARAM_KD) != kd)) 
							{
								pBindingUnitPropertySens->setPropertyPIDParam(workingsMode,SENS_PIDCONTROL_PARAM_KP,kp);
								pBindingUnitPropertySens->setPropertyPIDParam(workingsMode,SENS_PIDCONTROL_PARAM_KI,ki);
								pBindingUnitPropertySens->setPropertyPIDParam(workingsMode,SENS_PIDCONTROL_PARAM_KD,kd);
								if (m_pLogFile != 0) {
									m_pLogFile->Success("Update PID-Control params", pBindingUnitPropertySens );	
								}
								return TRUE;
							}
						}
					}
				}
			}


#if(0)
			{
				// zie: USE_SENSOR_PIDCONTROL			
				for (int mode=0;mode<SENS_PIDCONTROL_WORKINGMODE_MAX;++mode) { 
					// PID Control unit.
					int nodeAddress = 0xff;
					int unitAddress = 0xff;
					if (pBindingUnit->getPIDControlUnit(mode,&nodeAddress,&unitAddress)) {
						if ((0xff != nodeAddress) && (0xff != unitAddress)) {
							pBindingUnitPropertySens->setPropertyPIDControlUnit(mode,nodeAddress,unitAddress);
							if (m_pLogFile != 0) {
								m_pLogFile->Success("Update PID-Control unit", pBindingUnitPropertySens );	
							}
							return TRUE;
						}
					}	
				}				
			}	
			{
				// zie: USE_SENSOR_PIDCONTROL			
				for (int mode=0;mode<SENS_PIDCONTROL_WORKINGMODE_MAX;++mode) { 
					// PID params.
					int kp=-1;
					int ki=-1;
					int kd=-1;
					if (pBindingUnit->getPIDControlParams(mode,&kp,&ki,&kd)) {
						if ((pDefaults->getPIDControlParam(mode,SENS_PIDCONTROL_PARAM_KP) != kp) ||
							(pDefaults->getPIDControlParam(mode,SENS_PIDCONTROL_PARAM_KI) != ki) ||
							(pDefaults->getPIDControlParam(mode,SENS_PIDCONTROL_PARAM_KD) != kd)) 
						{
							pBindingUnitPropertySens->setPropertyPIDParam(mode,SENS_PIDCONTROL_PARAM_KP,kp);
							pBindingUnitPropertySens->setPropertyPIDParam(mode,SENS_PIDCONTROL_PARAM_KI,ki);
							pBindingUnitPropertySens->setPropertyPIDParam(mode,SENS_PIDCONTROL_PARAM_KD,kd);
							if (m_pLogFile != 0) {
								m_pLogFile->Success("Update PID-Control params", pBindingUnitPropertySens );	
							}
							return TRUE;
						}
					}
				}
			}
#endif 

			for (int i=0;i<4;i++)
			{		
				if ((pDefaults->GetHysHighValue(i) != pBindingUnit->Get_HysHighValue(i)) ||
					(pDefaults->GetHysLowValue(i) != pBindingUnit->Get_HysLowValue(i)))
				{
					pBindingUnitPropertySens->UpdateHysValues(
							i, 
							pBindingUnit->Get_HysHighValue(i),
						    pBindingUnit->Get_HysLowValue(i));

					if (m_pLogFile != 0) 
					{
						m_pLogFile->Success("Update hystersis values", pBindingUnitPropertySens );	
					}
					return TRUE;
				}			
			}

			// Sedert versie 0x0C03.
			// Ook controleren of deze sensor delegation Request Measured value ondersteund.
			const unsigned short usSupportedFlags = pBindingUnitPropertySens->GetSupportedFlags();

			if ((usSupportedFlags&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION)||
				(usSupportedFlags&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE))
			{
				if (pBindingUnit->GetDelegationType()!=CBindingSensUnit::DELEGATION_TYPE_NONE)
				{
					pBindingUnitPropertySens->UpdateDelegation(
						pBindingUnit->GetDelegationType(),
						pBindingUnit->GetDelegationNodeAddress(),
						pBindingUnit->GetDelegationUnitAddress());

					if (0!=m_pLogFile)
					{
						m_pLogFile->Success("Update delegation", pBindingUnitPropertySens);
					}
				
					return TRUE;
				}
			}

			// Sedert Versie 0x0E00
			if (TRUE==pBindingUnitPropertySens->getSetpointRangeSupported())
			{
				if ((pBindingUnit->getSetpointRangeValueMin() != pDefaults->getSetpointRangeMin())||
					(pBindingUnit->getSetpointRangeValueMax() != pDefaults->getSetpointRangeMax()))
				{
					pBindingUnitPropertySens->updateSetpointRange(
						pBindingUnit->getSetpointRangeValueMin(),
						pBindingUnit->getSetpointRangeValueMax());

					if (0!=m_pLogFile)
					{
						m_pLogFile->Success("Update setpoint range", pBindingUnitPropertySens);
					}
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


BOOL CBindingPropertyUnitConvertAlarm::_Update()
{
	/**
	 * Controlleren of Types : Unit & Property unit = correcte type !!!	 
	 */
	CBindingPropertyAlarmUnit* const pBindingUnitPropertyAlarm = 
		static_cast<CBindingPropertyAlarmUnit*>(m_pBindingPropertyUnit);

	const CBindingAlarmUnit* const pBindingUnit = 
		static_cast<const CBindingAlarmUnit*>(m_pBindingUnit);

	if (pBindingUnitPropertyAlarm != 0)
	{
		const CBindingPropertyUnitAlarmDefault* const pDefaults = 
			static_cast<const CBindingPropertyUnitAlarmDefault* const>
				(CBindingPropertyUnitDefaultFactory::Instance()->
					Create(UNIT_TYPE_ALARM));
		
		
		if (pDefaults == 0) 
		{
			ASSERT ( pDefaults );
			return FALSE;
		}
		
		if (pDefaults->GetAlarmType() != pBindingUnit->Get_Config()) {
		
			pBindingUnitPropertyAlarm->UpdateType( pBindingUnit->Get_Config() );

			if (m_pLogFile != 0) 
			{
				m_pLogFile->Success("Update config", pBindingUnitPropertyAlarm );	
			}

			return TRUE;
		}
		else
		{
			BOOL fResult = FALSE;

			const BYTE *pbDefaultRemoteCode = pDefaults->GetRemoteCode();
			const BYTE *pbRemoteCode = pBindingUnit->Get_RemoteCode();

			for (int i=0;i<ALARM_REMOTECODE_BUFFER_MAX;i++)
			{			
				if (pbDefaultRemoteCode[i] != pbRemoteCode[i]) {
					pBindingUnitPropertyAlarm->UpdateRemotecode(i,pbRemoteCode[i]);
					fResult = TRUE;
				}
			}		

			if (fResult == TRUE) 
			{
				if (m_pLogFile != 0) 
				{
					m_pLogFile->Success("Update remote code", pBindingUnitPropertyAlarm );	
				}
			}
			return fResult;
		}
	}
	return FALSE;
}

// Uitlezen van de bindings.
// AV-Matrix property unit: Config Byte --> AVMatrix subtype.
BOOL CBindingPropertyUnitConvertAVMatrix::_Update()
{
	ASSERT(0!=m_pBindingPropertyUnit);
	ASSERT(0!=m_pBindingUnit);

	CBindingPropertyAVMatrixUnit* const pBindingUnitPropertyAVMatrixUnit = 
		dynamic_cast<CBindingPropertyAVMatrixUnit*>(m_pBindingPropertyUnit);

	const CBindingAVMatrixUnit* const pBindingUnit = 
		dynamic_cast<const CBindingAVMatrixUnit*>(m_pBindingUnit);

	ASSERT((0 != pBindingUnitPropertyAVMatrixUnit)&& (0 != pBindingUnit));
	if ((0 != pBindingUnitPropertyAVMatrixUnit)&& (0 != pBindingUnit))
	{
		const int defaultConfig = 0;
		if (defaultConfig != pBindingUnit->Get_Config()) {
			pBindingUnitPropertyAVMatrixUnit->UpdateSubType(pBindingUnit->Get_Config());
			if (m_pLogFile != 0) {
				m_pLogFile->Success("Update config", pBindingUnitPropertyAVMatrixUnit );	
			}
			return TRUE;
		}
	}
	return FALSE;
}