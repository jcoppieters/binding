// FactoryDlgUnitChildProp.cpp: implementation of the CFactoryDlgUnitChildProp class.
//
// TM_CHANGES_ALARM_ARITECH:
// Deze klasse aangemaakt om dialogen aan te maken ifv het binding unit type
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "FactoryDlgUnitChildProp.h"

#include "views/unitprops/DlgAlarmChildProp.h"
#include "views/unitprops/DlgBoseChildProp.h"
#include "views/unitprops/DlgDimmerChildProp.h"
#include "views/unitprops/DlgDuoSwitchChildProp.h"
#include "views/unitprops/DlgSensPropChild.h"
#include "views/unitprops/DlgSwitchPropChild.h"
#include "views/unitprops/DlgAVMatrixChildProp.h"

#include "bindingconstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgAlarmChildProp* CFactoryDlgUnitChildProp::createDlgAlarm(const CBindingPropertyUnit* pBindingUnit)
{
	CDlgAlarmChildProp* pNew = 0;
	
	if (pBindingUnit)
	{	
		const BYTE bindingUnitType = pBindingUnit->GetBindingUnitType();

		switch(bindingUnitType)
		{
			case BINDING_UNIT_TYPE_ALARM_ARITECH:
			{
				pNew = new CDlgAlarmChildPropAritech( );					
			} break;
			case BINDING_UNIT_TYPE_ALARM_GALAXY:
			{
				pNew = new CDlgAlarmChildProp( );				
			} break;	
			case BINDING_UNIT_TYPE_ALARM_ANB:			/* TM_CHANGES_ALARM_ANB */
			{
				pNew = new CDlgAlarmChildPropAnbBibus( );					
			} break;
			case BINDING_UNIT_TYPE_ALARM_TEXECOM:		/* TM_CHANGES_ALARM_TEXECOM */
			{
				pNew = new CDlgAlarmChildPropTexecom( );
			} break;
			case BINDING_UNIT_TYPE_ALARM_INTEGRA:		/* TM_CHANGES_ALARM_INTEGRA */
			{
				pNew = new CDlgAlarmChildPropIntegra( );
			} break;
		}
	}
	return pNew;
}

CDlgAvMatrixChildProp* CFactoryDlgUnitChildProp::createDlgAVMatrix(const CBindingPropertyUnit* pBindingUnit)
{
	CDlgAvMatrixChildProp* pNew = NULL;
	if (NULL!=pBindingUnit) {
		const CBindingPropertyAVMatrixUnit* p = dynamic_cast<const CBindingPropertyAVMatrixUnit*>(pBindingUnit);
		if (NULL!=p) {
			const CBindingPropertyAVMatrixUnit::AVMatrixType_t type = p->getMatrixType();
			switch(type) {
				case CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_AUDAC:
				{					
					pNew = new CDlgAVMatrixChildPropAudac();
					INVARIANT(NULL!=pNew);
				} break;
				case CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_INTEGRA:
				{					
					pNew = new CDlgAVMatrixChildPropIntegra();
					INVARIANT(NULL!=pNew);
				} break;
				case CBindingPropertyAVMatrixUnit::AVMATRIX_TYPE_UNKNOWN:
				{
					pNew = NULL;
				} break;
			}		
		}
	}
	return pNew;
}







