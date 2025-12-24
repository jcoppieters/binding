// FactoryDlgUnitProp.cpp: implementation of the CFactoryDlgUnitProp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "FactoryDlgUnitProp.h"

#include "views/unitprops/DlgDimmerProp.h"
#include "views/unitprops/DlgSwitchProp.h"
#include "views/unitprops/DlgUnitProp.h"
#include "views/unitprops/DlgDuoSwitchProp.h"
#include "views/unitprops/DlgControlProp.h"
#include "views/unitprops/DlgSensPropDlg.h"
#include "views/unitprops/DlgIRTXProp.h"
#include "views/unitprops/DlgBosersProp.h"
#include "views/unitprops/DlgRC5RXProp.h"
#include "views/unitprops/DlgAudioBasicProp.h"
#include "views/unitprops/DlgAVMatrixProp.h"
#include "views/unitprops/DlgVideomuxProp.h"
#include "views/unitprops/DlgMouseProp.h"
#include "views/unitprops/DlgAudioHC4Prop.h"
#include "views/unitprops/DlgAlarmProp.h"
#include "views/unitprops/DlgEnergyMeasureProp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgUnitProp* CFactoryDlgUnitProp::Create(const CCanNode* pNode,STRUCT_UNIT* pUnit) {
	CDlgUnitProp* pNew = 0;
	const BYTE bUnitType = pUnit->bUnitType;
	switch(bUnitType)  {
		case UNIT_TYPE_DIM:			
			ASSERT(pUnit->m_pBindingUnit);
			pNew = new CDlgDimmerProp();	
		break;		
		case UNIT_TYPE_SWITCH:	
			ASSERT(pUnit->m_pBindingUnit);
			pNew = new CDlgSwitchProp();	
		break;		
		case UNIT_TYPE_DUOSWITCH:
			ASSERT(pUnit->m_pBindingUnit);
			pNew = new CDlgDuoSwitchProp();	
		break;		
		case UNIT_TYPE_CONTROL:		// same as virtual ...
		case UNIT_TYPE_LCD_VIRTUAL:	
			pNew = new CDlgControlProp();	
		break;
		case UNIT_TYPE_SENS:
			ASSERT(pUnit->m_pBindingUnit);
			pNew = new CDlgSensPropDlg();	
		break;		
		case UNIT_TYPE_IRTX:		
			pNew = new CDlgIRTXProp();	
		break;		
		case UNIT_TYPE_BOSERS:		
			pNew = new CDlgBosersProp();
		break;			
		case UNIT_TYPE_RC5RX:		
			pNew = new CDlgRC5RXProp();		
		break;		
		case UNIT_TYPE_AUDIO :		
			pNew = new CDlgAudioBasicProp();
		break;
		case UNIT_TYPE_AVMATRIX:	
			pNew = new CDlgAVMatrixProp();
		break; 		
		case UNIT_TYPE_VIDEOMUX:    
			pNew = new CDlgVideomuxProp();	
		break;		
		case UNIT_TYPE_PS2INTELLIMOUSE: 
			pNew = new CDlgMouseProp();
		break;		
		case UNIT_TYPE_AUDIO_HC4:	
			pNew = new CDlgAudioHC4Prop();	
		break;		
		case UNIT_TYPE_ALARM:						
			pNew = new CDlgAlarmProp();		
		break;		
		case UNIT_TYPE_ENERGYMEASURE:
			pNew = new CDlgEnergyMeasureProp();
		break;
		default:
			ASSERT(0); 
		break;
	}
	ASSERT(pNew);
	return pNew;
}