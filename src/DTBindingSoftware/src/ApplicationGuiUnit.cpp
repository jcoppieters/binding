// ApplicationGuiUnit.cpp: implementation of the CApplicationGuiUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ApplicationGuiUnit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "MainFrm.h"
#include "MyWindowTimers.h"
#include "NodeDatabaseOperations.h"

#include "utils/app/DisplayDevice.h"

// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------

//////////////////////////////////////////////////////////////////////
CApplicationGuiUnit* CApplicationGuiUnit::GetInstance()
{
	static CApplicationGuiUnit instance(AfxGetMainWnd());
	return (&instance);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplicationGuiUnit::CApplicationGuiUnit(CWnd *pParent)
{
	m_pParent = pParent;
	pTXUnitMsg = new CTXUnitMessage;
}

CApplicationGuiUnit::~CApplicationGuiUnit()
{
	delete pTXUnitMsg;
}

///////////////////////////////////////////////////////////////////////////////////

BOOL CApplicationGuiUnit::GetIcon(BYTE bNodeLogicalAddress,BYTE bUnitAddress,AppUiIconsExtraInfo_t info, UnitIconType *iImage)
{
	*iImage = ICON_UNDEFINED_UNIT;

	if (bUnitAddress == 0xFF) 
	{
		*iImage = ICON_BOSE;		
		return TRUE;
	}

	CNodeDatabaseOperations NodeDatabaseOperations(TMGetRuntimeNodes());
	STRUCT_UNIT* pUnit;

	if (NodeDatabaseOperations.Find(bNodeLogicalAddress,bUnitAddress,&pUnit) == TRUE)
	{
		switch(pUnit->bUnitType)
		{
			case UNIT_TYPE_AUDIO_HC4:
			{
				for (int i=0;i<4;i++) 
				{
					if (pUnit->UnitSpecs.AudioUnit.bAudioStatus1 & (0x01<<i)) 
					{
						*iImage = ICON_AUDIO_ON;			
						return TRUE;
					}
				}
				
				*iImage = ICON_AUDIO;
			}
			return TRUE;

			case UNIT_TYPE_AUDIO:
			{
				if (pUnit->UnitSpecs.AudioUnit.bAudioStatus2 & AUDIO_UNIT_ON)
				{
					*iImage = ICON_AUDIO_ON;					
				}
				else				
				{
					*iImage = ICON_AUDIO;
				}
			}return TRUE;
				
			case UNIT_TYPE_DIM:
			{				
				// Feedback error weergeven: Gebruiken voor DALI weergave defecte lamp 
				// Wanneer FEEDBACK error wordt een icoon met kruis getoond.
				// Weergave zal niet veranderen zolang FEEDBACK error vlag niet
				// gewist wordt.
				const BYTE bDimStatus1 = pUnit->UnitSpecs.DimmerUnit.bDimStatus1;
				const BOOL flagUseDALI = (info == APPUI_ICONS_INFO_DALI ? TRUE : FALSE);
				switch(bDimStatus1 & DIM_UNIT_STATE_MASK) {
					case DIM_UNIT_ON_STATE:
					case DIM_UNIT_UP_STATE:
					case DIM_UNIT_DOWN_STATE: {					
						if (bDimStatus1 & DIM_FEEDBACK_ERROR) {
							*iImage = flagUseDALI == TRUE ? ICON_DALI_DIMMER_EC : ICON_DIMMER_EC;
						}
						else {
							*iImage = flagUseDALI == TRUE ? ICON_DALI_DIMMER_ON : ICON_DIMMER_ON;							
						}
					}break;									
					case DIM_UNIT_OFF_STATE: {
						if (bDimStatus1 & DIM_FEEDBACK_ERROR) {
							*iImage = flagUseDALI == TRUE ? ICON_DALI_DIMMER_EC : ICON_DIMMER_EC;
						}
						else {
							if (bDimStatus1 & DIM_PIR_ON) {
								*iImage = flagUseDALI == TRUE ? ICON_DALI_DIMMER_PIR : ICON_DIMMER_PIR;
							}
							else {
								*iImage =  flagUseDALI == TRUE ?  ICON_DALI_DIMMER : ICON_DIMMER;													
							}
						}
					}break;
					default: {
						*iImage = flagUseDALI == TRUE ? ICON_DALI_DIMMER : ICON_DIMMER;
					}break;	
				}// end switch state				
			}return TRUE;	// break unit type Dimmer
				
			case UNIT_TYPE_SWITCH:
			{
				const BYTE bTempState = pUnit->UnitSpecs.RelayUnit.bSwitchStatus1 & SWITCH_UNIT_STATE_MASK;

				switch (bTempState) 
				{
					case SWITCH_UNIT_OFF_STATE:
					{
						if (pUnit->UnitSpecs.RelayUnit.bSwitchStatus1 & SWITCH_PIR_ON)
						{
							*iImage = ICON_SWITCH_PIR;
						}
						else
						{
							*iImage = ICON_SWITCH;
						}
					}break;

					case SWITCH_UNIT_ON_STATE:
					{
						*iImage = ICON_SWITCH_ON;
								
					}break;

					default:
					{
						*iImage = ICON_SWITCH;
					} break;
				}
			
			}return TRUE;
					
			case UNIT_TYPE_DUOSWITCH:
			{
				const BYTE bTempState = pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1 & DUOSWITCH_UNIT_STATE_MASK;

				switch (bTempState) 
				{			
					case DUOSWITCH_UNIT_STOP_STATE:
					{
						*iImage = ICON_MOTOR;							
					}break;

					case DUOSWITCH_UNIT_WAIT_DOWN_STATE:
					case DUOSWITCH_UNIT_WAIT_UP_STATE:
					{
						*iImage = ICON_MOTOR;							
					}break;

					case DUOSWITCH_UNIT_UP_STATE:
					{
						*iImage = ICON_MOTOR_UP;							
					}break;

					case DUOSWITCH_UNIT_DOWN_STATE:
					{
						*iImage = ICON_MOTOR_DN;							
					}break;

					default:
					{
						*iImage = ICON_MOTOR;							
					}break;
				}															
			}return TRUE;	//UNIT_TYPE_DUOSWITCH		
				
			case UNIT_TYPE_LCD_VIRTUAL: {
				const BOOL flagUseDALI = (info == APPUI_ICONS_INFO_DALI ? TRUE : FALSE);
				const BYTE bControlStatus2 = pUnit->UnitSpecs.ControlUnit.bControlStatus2;
				if ((bControlStatus2 & UNIT_CHECK_ERROR_CONDITION) && (bControlStatus2 & UNIT_CHECK_TIMER_RUNNING)) {					
					*iImage = (flagUseDALI == TRUE ? ICON_VIRTUAL_DALI : ICON_VIRTUAL);		
				}
				else if (bControlStatus2 & UNIT_HW_PORT_CHECK_HIGH) {
					*iImage = (flagUseDALI == TRUE ? ICON_VIRTUAL_DALI_ON : ICON_VIRTUAL_ON);
				}
				else {
					*iImage = (flagUseDALI == TRUE ? ICON_VIRTUAL_DALI : ICON_VIRTUAL);
				}			
			} return TRUE;

			case UNIT_TYPE_CONTROL: {
				const BOOL flagUseDALI = (info == APPUI_ICONS_INFO_DALI ? TRUE : FALSE);
				const BYTE bControlStatus2 = pUnit->UnitSpecs.ControlUnit.bControlStatus2;
				if ((bControlStatus2 & UNIT_CHECK_ERROR_CONDITION) &&	
					(bControlStatus2 & UNIT_CHECK_TIMER_RUNNING)) {					
					*iImage = (flagUseDALI == TRUE ? ICON_DALI_CONTROL : ICON_CONTROL);
				}
				else if (bControlStatus2 & UNIT_HW_PORT_CHECK_HIGH) {
					*iImage = (flagUseDALI == TRUE ? ICON_DALI_CONTROL_ON : ICON_CONTROL_ON);
				}
				else {
					*iImage = (flagUseDALI == TRUE ? ICON_DALI_CONTROL : ICON_CONTROL);
				}		
			} return TRUE;

			case UNIT_TYPE_SENS:
			{
				*iImage = ICON_SENS;	
			} return TRUE;
				
			case UNIT_TYPE_SPEECHCOMM :
			case UNIT_TYPE_LCD:
			case UNIT_TYPE_SELECTOR:
			{
				*iImage = ICON_UNDEFINED_UNIT;
			} return TRUE;
      
			case UNIT_TYPE_BOSERS:
			{
	  			if (pUnit->UnitSpecs.AudioUnit.bAudioStatus2 & AUDIO_UNIT_ON)
				{
					*iImage = ICON_BOSE_ON;
				}
				else
				{
					*iImage = ICON_BOSE;
				}
			} return TRUE;
			
			case UNIT_TYPE_RC5RX:
			{
				*iImage = ICON_IRRX;
			} return TRUE;

			case UNIT_TYPE_IRTX:
			{
				*iImage = ICON_IRTX;

			} return TRUE;
			
			case UNIT_TYPE_AVMATRIX:
			{
				*iImage = ICON_AVMATRIX;	
			
			}return TRUE;

			case UNIT_TYPE_VIDEOMUX :
			{
				*iImage = ICON_VIDEOMUX;
			
			}return TRUE;	

			case UNIT_TYPE_ALARM:
			{
				*iImage = ICON_ALARM;
			
			} return TRUE;

			case UNIT_TYPE_PS2INTELLIMOUSE:
			{
				*iImage = ICON_PS2MOUSE;
				
			} return TRUE;
			 
			default:
			{
				*iImage = ICON_UNDEFINED_UNIT;
				return FALSE;										
						
			}break;

		}// end switch
	}
	else
	{
		CString szMessage;
		szMessage.Format(_T("ERROR - Unit not Found : Address 0x%02x,0x%02x"),bNodeLogicalAddress,bUnitAddress);
		CDisplayDevice::DisplayErrorMsg(szMessage);	
	}
	
	return FALSE;
}

BOOL CApplicationGuiUnit::GetIcon(BYTE bNodeAddress, UnitIconType *iImage)
{
	int iEntryNr;
	CCanNode *pNode;

	*iImage = ICON_UNDEFINED_NODE;

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress, &iEntryNr, &pNode))
	{
		switch(pNode->bNodeType)
		{
			case NODE_PIN_STD_UNITS :
			{
				*iImage = ICON_NODE;

			} break;

			case NODE_LCD:
			{
				if (pNode->bNodeFlags & NODE_MASTER_LCD)
				{
					*iImage = ICON_NODE_MASTER;
				}
				else
				{
					*iImage = ICON_NODE_LCD;
				}

			} break;

			case NODE_PIN_GATEWAY:
			{
				// TM,v0577: Gateway kan ook de master zijn...

				if (pNode->bNodeFlags & NODE_MASTER_LCD)
				{
					*iImage = ICON_NODE_MASTER;
				}
				else
				{
					*iImage = ICON_NODE_GATEWAY;
				}

			} break;

			case NODE_PIN_ROUTER_TIN:		
			case NODE_PIN_MODEM:   
			case NODE_RESERVED:	
			default:
			{
				// Absorbe : user default ...

			} break;
		}
		return TRUE;
	}
	return FALSE;
}

void CApplicationGuiUnit::ClickShort(BYTE bLogicalNodeAddress,BYTE bUnitAddress)
{	
	STRUCT_UNIT* pUnit;
	
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetRuntimeNodes());
	if (NodeDatabaseOperations.Find(bLogicalNodeAddress,bUnitAddress,&pUnit) == TRUE)
	{
		switch (pUnit->bUnitType)
		{
			case UNIT_TYPE_DIM:
			{
				((CTXDimmerUnitMessage*) pTXUnitMsg)->
					SetToggleOnOff(bLogicalNodeAddress,bUnitAddress);
			}break;
				
			case UNIT_TYPE_SWITCH:				
			{
				((CTXSwitchUnitMessage*) pTXUnitMsg)->
					SetToggleOnOff(bLogicalNodeAddress,bUnitAddress);
			}break;

			case UNIT_TYPE_DUOSWITCH:				
			{
				((CTXDuoSwitchUnitMessage*) pTXUnitMsg)->
					SetFsUpDown(bLogicalNodeAddress,bUnitAddress);					
			}break;

			case UNIT_TYPE_BOSERS:
			case UNIT_TYPE_AUDIO:	/* IRAUDIO ROOM - BASIC */
			{					
				((CTXAudioBasicUnitMessage*) pTXUnitMsg)->
					SetFsCsOnOff(bLogicalNodeAddress,bUnitAddress);					
			}break;

			case UNIT_TYPE_LCD_VIRTUAL:			   
			case UNIT_TYPE_CONTROL:
			{
				if (!(pUnit->UnitSpecs.ControlUnit.bControlStatus1 & UNIT_CONTROLPULSTOGGLE_HIGH))
				{
					pUnit->UnitSpecs.ControlUnit.bControlStatus1 |= UNIT_CONTROLPULSTOGGLE_HIGH;
					
					((CTXControlUnitMessage*) pTXUnitMsg)->
						SetEvPulsStatus(bLogicalNodeAddress,bUnitAddress,TRUE);
				}
				else
				{
					pUnit->UnitSpecs.ControlUnit.bControlStatus1 &= (UNIT_CONTROLPULSTOGGLE_HIGH^0xFF);
					
					((CTXControlUnitMessage*) pTXUnitMsg)->
						SetEvPulsStatus(bLogicalNodeAddress,bUnitAddress,FALSE);
				}									
			}break;

			case UNIT_TYPE_AVMATRIX:
			case UNIT_TYPE_VIDEOMUX:
			case UNIT_TYPE_ALARM:
			case UNIT_TYPE_PS2INTELLIMOUSE:
			case UNIT_TYPE_AUDIO_HC4:

			break;

			default :					
			break;									
		}
	}
}

void CApplicationGuiUnit::ClickLongOn(BYTE bLogicalNodeAddress,BYTE bUnitAddress)
{
	STRUCT_UNIT* pUnit;
	
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetRuntimeNodes());
	if (NodeDatabaseOperations.Find(bLogicalNodeAddress,bUnitAddress,&pUnit) == TRUE)
	{
		switch(pUnit->bUnitType)
		{
			case UNIT_TYPE_DIM:
			{
				((CTXDimmerUnitMessage *) pTXUnitMsg)->
					SetFsDimUpDn(bLogicalNodeAddress,bUnitAddress);					
			} break;

			case UNIT_TYPE_SWITCH:
			break;
			
			case UNIT_TYPE_BOSERS:
			case UNIT_TYPE_AUDIO:	// BASIC AUDIO					
			{
				((CTXAudioBasicUnitMessage*) pTXUnitMsg)->
					SetFsVolUpDn(bLogicalNodeAddress,bUnitAddress);										
			} break;

			case UNIT_TYPE_LCD_VIRTUAL:
			case UNIT_TYPE_CONTROL:
			{	
				((CTXControlUnitMessage*) pTXUnitMsg)->
					SetEvControlStatus(bLogicalNodeAddress,bUnitAddress,TRUE);
			} break;

			case UNIT_TYPE_DUOSWITCH:
			{
				((CTXDuoSwitchUnitMessage*) pTXUnitMsg)->
					SetStop(bLogicalNodeAddress,bUnitAddress);											
			} break;
		
			case UNIT_TYPE_AVMATRIX:
			case UNIT_TYPE_VIDEOMUX:
			case UNIT_TYPE_ALARM:
			case UNIT_TYPE_PS2INTELLIMOUSE:
			case UNIT_TYPE_AUDIO_HC4:
			break;
				       
			default:
			break;
		
		}
	}
}

void CApplicationGuiUnit::ClickLongOff(BYTE bLogicalNodeAddress,BYTE bUnitAddress)
{
	STRUCT_UNIT* pUnit;
	
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetRuntimeNodes());
	if (NodeDatabaseOperations.Find(bLogicalNodeAddress,bUnitAddress,&pUnit) == TRUE)
	{
		switch(pUnit->bUnitType)
		{			
			case UNIT_TYPE_DIM:			
			{
				((CTXDimmerUnitMessage *) pTXUnitMsg)->
						SetOnCv(bLogicalNodeAddress,bUnitAddress);
			} break;

			case UNIT_TYPE_SWITCH:
			break;

			case UNIT_TYPE_BOSERS:
			case UNIT_TYPE_AUDIO:	// BASIC AUDIO
			{
				((CTXAudioBasicUnitMessage*) pTXUnitMsg)->
					SetCurrentSourceOn(bLogicalNodeAddress,bUnitAddress);	
			} break;

			case UNIT_TYPE_LCD_VIRTUAL:
			case UNIT_TYPE_CONTROL:				
			{
				((CTXControlUnitMessage*) pTXUnitMsg)->
					SetEvControlStatus(bLogicalNodeAddress,bUnitAddress,FALSE);
			} break;


			case UNIT_TYPE_AVMATRIX:
			case UNIT_TYPE_VIDEOMUX:
			case UNIT_TYPE_ALARM:
			case UNIT_TYPE_PS2INTELLIMOUSE:
			case UNIT_TYPE_AUDIO_HC4:

			break;

			default:
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/////////													/////////////////	
/////////	INTERFACE FUNCTIONS FOR CONTROLLING UNIT		/////////////////
/////////													/////////////////
/////////////////////////////////////////////////////////////////////////////

void CApplicationGuiUnit::OnTimerExpired()
{
	m_pParent->KillTimer(TIMERID_MAINFRAME_PULSTOGGLE);		
	ClickLongOn(bSelNodeAddress,bSelUnitAddress);		
	fPulsToggleTimer = TRUE;
}

void CApplicationGuiUnit::ClickOn(BYTE bNodeAddress,BYTE bUnitAddress)
{
	fPulsToggleTimer = FALSE;
	bSelNodeAddress = bNodeAddress;
	bSelUnitAddress = bUnitAddress;	
	m_pParent->SetTimer(TIMERID_MAINFRAME_PULSTOGGLE,MAINFRAME_PULSTOGGLE_VALUE,(TIMERPROC) NULL);
}

void CApplicationGuiUnit::ClickOff(BYTE bNodeAddress,BYTE bUnitAddress)
{
	m_pParent->KillTimer(TIMERID_MAINFRAME_PULSTOGGLE);

	if ((bUnitAddress == bSelUnitAddress) && (bNodeAddress == bSelNodeAddress))
	{
		if (fPulsToggleTimer == TRUE) 
		{
			ClickLongOff(bNodeAddress,bUnitAddress);	
			fPulsToggleTimer = FALSE;
		}
		else 
		{
			ClickShort(bNodeAddress,bUnitAddress);	
		}
	}
}

BOOL CApplicationGuiUnit::GetRuntimeUnit(BYTE bNodeAddress,BYTE bUnitAddress,STRUCT_UNIT **pRuntimeUnit)
{
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetRuntimeNodes());
	return (NodeDatabaseOperations.Find(bNodeAddress,bUnitAddress,pRuntimeUnit));	
}


















