/**********************************************************//**
 * @file	ConfigAppUiCtrlClasses.cpp
 * @brief	implementatie bestand voor de CConfigExternalClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20150806
 *
 * @since	0x0804
 *
 * @changes TM,versie V11.01: klassen initialiseren met standaard 
 * waarden zoals in de firmware. Dit is nodig wanneer general 
 * settings niet ingeladen wordt.
 *************************************************************/
#include "stdafx.h"
#include "ConfigAppUiCtrlClasses.h"

#include "app\appcfg\appcfgbase\configExternalhelper.h"
/*************************************************************/

/*************************************************************/
using DUOTECNO::MFC_HELPER::IReportError;
/*************************************************************/
const char* CConfigLedCfgGeneralClass::m_szHeader = "GENERAL";
const char* CConfigLedCfgLedClass::m_szHeader="LED";
/*************************************************************/
using DUOTECNO::DATA::CSerLedCfgGeneral;
using DUOTECNO::DATA::CSerLedCfg;
/*************************************************************/
#define DEBUG_LOCAL_ON		1
/*************************************************************/
#if (DEBUG_LOCAL_ON==1)
	#define TRACER(x)	TRACE x
#else
	#define TRACER(x)
#endif

/*************************************************************/

static 
void helper_Dump(
	DUOTECNO::DATA::CSerLedCfg& ref)
{
	TRACER(("LedID=%d\n",ref.m_LedID));
	TRACER(("function=%d\n",ref.m_function));
	TRACER(("NodeAddress=0x%02x\n",ref.m_bNodeAddress));
	TRACER(("UnitAddress=0x%02x\n",ref.m_bUnitAddress));
	TRACER(("type=0x%02x\n",ref.m_type));
}

/************************* private helper ********************/
static 
BOOL helper_parseByteDEC(
	const char* s,
	BYTE* pValue)
{
	BOOL fResult=FALSE;
	int tempValue=-1;

	if(1==sscanf(s,"%d;",&tempValue))
	{
		if (tempValue>=0 && tempValue<=0xFF)
		{
			*pValue=(BYTE)tempValue;
			fResult=TRUE;	
		}
	}	
	return fResult;
}

static 
BOOL helper_parseByteHEX(
	const char* s,
	BYTE* pValue)
{
	BOOL fResult=FALSE;
	int tempValue=-1;

	if(1==sscanf(s,"0x%x;",&tempValue))
	{
		if (tempValue>=0 && tempValue<=0xFF)
		{
			*pValue=(BYTE)tempValue;
			fResult=TRUE;	
		}
	}	
	return fResult;
}

/******************** private CSerLedCfgGeneral **************/

static
void helper_InitLedCfgSerClass(
	CSerLedCfgGeneral* pDest)
{
	memset(pDest,0,sizeof(CSerLedCfgGeneral));

	// Sedert v11.01
	pDest->m_colorDefault=0;	// COLOR_WHITE
	pDest->m_lvlIntensity=7;	// BRIGHTNESS_LVL_7		
	pDest->m_cfgData=0xFF;

	// pDest->m_type=0xFF;
	pDest->m_bNodeAddress=0xFF;
	pDest->m_bUnitAddress=0xFF;
}

/******************** private CSerLedCfg *********************/

static
void helper_InitLedCfgSerClass(
	CSerLedCfg* pDest,
	int ledID=0xFF)
{
	memset(pDest,0,sizeof(CSerLedCfg));

	pDest->m_LedID=ledID;					

	// Sedert v11.01
	pDest->m_function=0;	// APPUICTRL_LEDSCFG_LED_FUNC_NONE;
	pDest->m_cfgData=0xFF;

	// pDest->m_function=0;					
	pDest->m_bNodeAddress=0xFF;					
	pDest->m_bUnitAddress=0xFF;					
	// pDest->m_type=0xFF;
}


/*************************************************************/
/************************* public ****************************/
/*************************************************************/

CConfigLedCfgGeneralClass::CConfigLedCfgGeneralClass(
	IReportError* ptr) :
m_pErrorReport( ptr ),
m_CurrentIndex(-1)
{
	helper_InitLedCfgSerClass(&m_serDataLedCfg);
}
CConfigLedCfgGeneralClass::~CConfigLedCfgGeneralClass()
{

}
/*************************************************************/

BOOL CConfigLedCfgGeneralClass::ParseHeader(
	char *szAttribute,
	char *szData)
{
	BOOL fResult=FALSE;

	if (0==strcmp(m_szHeader,szAttribute)) 
	{
		int tempValue = 0;

		// Oppassen:
		// De header 'GENERAL' moet op 1 staan want zal anders niet geparsed worden...

		helper_InitLedCfgSerClass(&m_serDataLedCfg);

		if (1==sscanf(szData,"%x",&tempValue))		
		{			
			if (tempValue==1)
			{				
				m_CurrentIndex=0;
				fResult=TRUE;
			}
		}
	}
	return fResult;
}

/****************************************************************************
 * [GENERAL]=1;			
 * COLOR=DEC;
 * LEVEL=DEC;	
 * CFGDATA=DEC;
 * TYPE=DEC;
 * NODEADDRESS=HEX;
 * UNITADDRESS=HEX;
 * LEVELDAY=DEC;			
 * LEVELNIGHT=DEC;
 ***************************************************************************/
BOOL CConfigLedCfgGeneralClass::ParseData(
	char *szAttribute,
	char *szData)
{
	BOOL fResult=FALSE;

	if (m_CurrentIndex==0)
	{
		uint8_t byteValue;

		CString szAttributeUpper(szAttribute);
		szAttributeUpper.MakeUpper( );

		fResult=TRUE;

		if (0==strcmp(szAttributeUpper,"COLOR"))
		{
			if(TRUE==helper_parseByteDEC(szData,&byteValue))
			{				
				m_serDataLedCfg.m_colorDefault=byteValue;				
			}
		}
		else if (0==strcmp(szAttributeUpper,"LEVEL"))
		{
			if(TRUE==helper_parseByteDEC(szData,&byteValue))
			{
				m_serDataLedCfg.m_lvlIntensity=byteValue;
			}	
		}
		else if (0==strcmp(szAttributeUpper,"CFGDATA"))
		{
			if(TRUE==helper_parseByteDEC(szData,&byteValue))
			{
				m_serDataLedCfg.m_cfgData=byteValue;
			}	
		}
		else if (0==strcmp(szAttributeUpper,"TYPE"))
		{
			if(TRUE==helper_parseByteDEC(szData,&byteValue))
			{
				m_serDataLedCfg.m_type=byteValue;
			}	
		}
		else if (0==strcmp(szAttributeUpper,"NODEADDRESS"))
		{
			if(TRUE==helper_parseByteHEX(szData,&byteValue))
			{
				m_serDataLedCfg.m_bNodeAddress=byteValue;
			}	
		}
		else if (0==strcmp(szAttributeUpper,"UNITADDRESS"))
		{
			if(TRUE==helper_parseByteHEX(szData,&byteValue))
			{
				m_serDataLedCfg.m_bUnitAddress=byteValue;
			}	
		}
		else if (0==strcmp(szAttributeUpper,"LEVELDAY"))
		{
			if(TRUE==helper_parseByteDEC(szData,&byteValue))
			{
				m_serDataLedCfg.m_lvlIntensityDay=byteValue;
			}	
		}
		else if (0==strcmp(szAttributeUpper,"LEVELNIGHT"))
		{
			if(TRUE==helper_parseByteDEC(szData,&byteValue))
			{
				m_serDataLedCfg.m_lvlIntensityNight=byteValue;
			}	
		}		
		else
		{
			CString s;
			s.Format(_T("Error:idx=%d,Unknown Attribute '%s'"),
					m_CurrentIndex,
					szAttribute);

			ReportError(s);
			fResult=FALSE;
		}
	}	

	return fResult;
}

void CConfigLedCfgGeneralClass::Initialise(void)
{
	helper_InitLedCfgSerClass(&m_serDataLedCfg);
	m_CurrentIndex = -1;
}

int CConfigLedCfgGeneralClass::GetMax(void) const
{
	return 1;
}

void CConfigLedCfgGeneralClass::dump( )
{
	char szTempBuffer[512];

	sprintf(szTempBuffer,
			"GENERAL=%d;\n"			
		    "Color=%d;\n"
			"Level=%d;\n"
			"CfgData=%d;\n"
			"Type=%d;\n"
			"NodeAddress=0x%02x;\n"
			"UnitAddress=0x%02x;\n"
			"LevelDay=%d;\n"
			"LevelNight=%d;\n",
			m_CurrentIndex == 0 ? 1 : 0,
			m_serDataLedCfg.m_colorDefault,
			m_serDataLedCfg.m_lvlIntensity,
			m_serDataLedCfg.m_cfgData,
			m_serDataLedCfg.m_type,
			m_serDataLedCfg.m_bNodeAddress,
			m_serDataLedCfg.m_bUnitAddress,
			m_serDataLedCfg.m_lvlIntensityDay,
			m_serDataLedCfg.m_lvlIntensityNight);

	TRACE( szTempBuffer );
}

int CConfigLedCfgGeneralClass::GetData(
	int index,
	BYTE *pbData)
{
	static const int LENGTH=sizeof(m_serDataLedCfg);

	memcpy(pbData,&m_serDataLedCfg,LENGTH);
	return LENGTH;
}

BOOL CConfigLedCfgGeneralClass::IsUsed(
	int index)
{
	BOOL fResult=FALSE;

	if(index==0)
	{
		fResult=TRUE;
	}
	return fResult;
}
	
int CConfigLedCfgGeneralClass::getStartIndex( )
{
	return 0;
}

/**************************** protected ************************/

void CConfigLedCfgGeneralClass::ReportError(
	const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

/***************************************************************/

/***************************************************************/

CConfigLedCfgLedClass::CConfigLedCfgLedClass(IReportError* ptr) :
m_list( ),
m_pCurrSerLedCfg( 0 ),
m_pErrorReport( ptr ),
m_CurrentIndex( 0 ),
m_State(STATE_IDLE)
{

}
CConfigLedCfgLedClass::~CConfigLedCfgLedClass()
{
	removeAll( );
}

/***************************************************************/

BOOL CConfigLedCfgLedClass::ParseHeader(
	char *szAttribute,
	char *szData)
{
	BOOL fResult=FALSE;

	if (0==strcmp(m_szHeader,szAttribute)) 
	{
		int tempValue = 0;

		if (1==sscanf(szData,"%d;",&tempValue))		
		{
			if (tempValue>=0 && tempValue<=255)
			{
				m_CurrentIndex=tempValue;

				m_pCurrSerLedCfg = new CSerLedCfg( );
				if (m_pCurrSerLedCfg)
				{
					helper_InitLedCfgSerClass(m_pCurrSerLedCfg,tempValue);
					
					m_State=STATE_DATA;

					m_list.addTail( m_pCurrSerLedCfg );
					fResult=TRUE;
				}
			}
			else
			{
				char szBuffer[255];
				sprintf(szBuffer,
						"Error:Index %d is to large",
						 tempValue);

				ReportError(szBuffer);	
			}
		}
	}
	return fResult;
}

BOOL CConfigLedCfgLedClass::ParseData(
	char *szAttribute,
	char *szData)
{
	BOOL fResult=TRUE;
	BYTE byteValue=0xFF;

	if (0==m_pCurrSerLedCfg)
	{
		return FALSE;
	}

	CString szAttributeUpper(szAttribute);
	szAttributeUpper.MakeUpper( );

	 /********************************
	 * FDBK_TACTILE=DEC;		// 0 of 1 ...
	 * FDBK_DIMMER=DEC;
	 * FDBK_SWITCH=DEC;
	 * FDBK_DUOSWITCH=DEC;
	 * FDBK_VIRTUAL=DEC;
	 * FDBK_SENSSTATE=DEC;
	 * FDBK_SENSPRESET=DEC;
	 * FDBK_SENSONOFF=DEC;
	 * FDBK_AUDIO=DEC;
	 ********************************/

	// Alle Headers parsen ...
	if (0==strcmp(szAttributeUpper,"FDBK_TACTILE"))
	{
		m_State=STATE_UNDEFINED;
		if(TRUE==helper_parseByteDEC(szData,&byteValue))
		{
			if (byteValue==1)
			{
				m_State=STATE_LEDFDBK_TACTILE;	
			}		
		}
	}
	else if (0==strcmp(szAttributeUpper,"FDBK_DIMMER"))
	{
		m_State=STATE_UNDEFINED;
		if(TRUE==helper_parseByteDEC(szData,&byteValue))
		{
			if (byteValue==1)
			{
				m_State=STATE_LEDFDBK_DIM;	
			}		
		}
	}
	else if (0==strcmp(szAttributeUpper,"FDBK_SWITCH"))
	{
		m_State=STATE_UNDEFINED;
		if(TRUE==helper_parseByteDEC(szData,&byteValue))
		{
			if (byteValue==1)
			{
				m_State=STATE_LEDFDBK_SWITCH;	
			}		
		}
	}
	else if (0==strcmp(szAttributeUpper,"FDBK_DUOSWITCH"))
	{
		m_State=STATE_UNDEFINED;
		if(TRUE==helper_parseByteDEC(szData,&byteValue))
		{
			if (byteValue==1)
			{
				m_State=STATE_LEDFDBK_DUOSWITCH;	
			}		
		}
	}
	else if (0==strcmp(szAttributeUpper,"FDBK_VIRTUAL"))
	{
		m_State=STATE_UNDEFINED;
		if(TRUE==helper_parseByteDEC(szData,&byteValue))
		{
			if (byteValue==1)
			{
				m_State=STATE_LEDFDBK_VIRTUAL;	
			}		
		}
	}
	else if (0==strcmp(szAttributeUpper,"FDBK_SENSSTATE"))
	{
		m_State=STATE_UNDEFINED;
		if(TRUE==helper_parseByteDEC(szData,&byteValue))
		{
			if (byteValue==1)
			{
				m_State=STATE_LEDFDBK_SENSSTATE;	
			}		
		}
	}
	else if (0==strcmp(szAttributeUpper,"FDBK_SENSPRESET"))
	{
		m_State=STATE_UNDEFINED;
		if(TRUE==helper_parseByteDEC(szData,&byteValue))
		{
			if (byteValue==1)
			{
				m_State=STATE_LEDFDBK_SENSPRESET;	
			}		
		}
	}
	else if (0==strcmp(szAttributeUpper,"FDBK_SENSONOFF"))
	{
		m_State=STATE_UNDEFINED;
		if(TRUE==helper_parseByteDEC(szData,&byteValue))
		{
			if (byteValue==1)
			{
				m_State=STATE_LEDFDBK_SENSSTATEONOFF;	
			}		
		}
	}
	else if (0==strcmp(szAttributeUpper,"FDBK_AUDIO"))
	{
		m_State=STATE_UNDEFINED;
		if(TRUE==helper_parseByteDEC(szData,&byteValue))
		{
			if (byteValue==1)
			{
				m_State=STATE_LEDFDBK_AUDIO;	
			}		
		}
	}
	else
	{
		switch(m_State)
		{
			case STATE_DATA:
			{
				/********************************
				 * Function=DEC;  
		 		 * Type=DEC;
				 * NodeAddress=HEX;
				 * UnitAddress=HEX;				
				 * CfgData=DEC;				
				 *******************************/
				BOOL fResult=TRUE;

				if (0==strcmp(szAttributeUpper,"FUNCTION"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->m_function=byteValue;
					}
					else
					{
						TRACER(("Failed to parse:%s=%s\n",szAttributeUpper,szData));
					}
				}
				else if (0==strcmp(szAttributeUpper,"TYPE"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->m_type=byteValue;
					}
					else
					{
						TRACER(("Failed to parse:%s=%s\n",szAttributeUpper,szData));
					}
				}
				else if (0==strcmp(szAttributeUpper,"NODEADDRESS"))
				{
					if(TRUE==helper_parseByteHEX(szData,&byteValue))
					{
						m_pCurrSerLedCfg->m_bNodeAddress=byteValue;
					}
					else
					{
						TRACER(("Failed to parse:%s=%s\n",szAttributeUpper,szData));
					}
				}
				else if (0==strcmp(szAttributeUpper,"UNITADDRESS"))
				{
					if(TRUE==helper_parseByteHEX(szData,&byteValue))
					{
						m_pCurrSerLedCfg->m_bUnitAddress=byteValue;
					}
					else
					{
						TRACER(("Failed to parse:%s=%s\n",szAttributeUpper,szData));
					}
				}
				else if (0==strcmp(szAttributeUpper,"CFGDATA"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->m_cfgData=byteValue;
					}
					else
					{
						TRACER(("Failed to parse:%s=%s\n",szAttributeUpper,szData));
					}
				}			
				else
				{
					fResult=FALSE;
				}
			} break;

			case STATE_LEDFDBK_TACTILE:
			{
				/********************************
				 * ColorIdle=DEC;
				 * ColorActive=DEC;
				 * ColorActiveLong=DEC;
				 *******************************/
				
				if (0==strcmp(szAttributeUpper,"COLORIDLE"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkTactile.m_ColOn=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORACTIVE"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkTactile.m_ColActive=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORACTIVELONG"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkTactile.m_ColActiveLong=byteValue;
					}
				}
				else
				{
					CString s;
					s.Format(_T("Error:Led configuration %d,Unknown Attribute '%s'"),
								m_CurrentIndex,
								szAttribute);

					ReportError(s);
					fResult=FALSE;
				}

			} break;

			case STATE_LEDFDBK_DIM:
			{
				/********************************
				 * ColorOn=
				 * ColorOff=
				 * ColorPirTimed=
				 *******************************/

				if (0==strcmp(szAttributeUpper,"COLORON"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkDim.m_ColOn=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLOROFF"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkDim.m_ColOff=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORPIRTIMED"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkDim.m_ColPirTimed=byteValue;
					}
				}
				else
				{
					CString s;
					s.Format(_T("Error:Led configuration %d,Unknown Attribute '%s'"),
								m_CurrentIndex,
								szAttribute);

					ReportError(s);
					fResult=FALSE;
				}
			} break;

			case STATE_LEDFDBK_SWITCH:
			{
				/********************************
				 * ColorOn=
				 * ColorOff=
				 * ColorPirTimed=
				 *******************************/

				if (0==strcmp(szAttributeUpper,"COLORON"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkSwitch.m_ColOn=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLOROFF"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkSwitch.m_ColOff=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORPIRTIMED"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkSwitch.m_ColPirTimed=byteValue;
					}
				}
				else
				{
					CString s;
					s.Format(_T("Error:Led configuration %d,Unknown Attribute '%s'"),
								m_CurrentIndex,
								szAttribute);

					ReportError(s);
					fResult=FALSE;
				}			
			} break;

			case STATE_LEDFDBK_DUOSWITCH:
			{
				/********************************
				 * ColorIdle=DEC;
				 * ColorUp=DEC;
				 * ColorDown=DEC;
				 * ColorBusyUp=DEC;
				 * ColorBusyDown=DEC;
				 ********************************/

				if (0==strcmp(szAttributeUpper,"COLORIDLE"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkDuoswitch.m_ColorIdle=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORUP"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkDuoswitch.m_ColorIdleUp=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORDOWN"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkDuoswitch.m_ColorIdleDown=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORBUSYUP"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkDuoswitch.m_ColorBusyUp=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORBUSYDOWN"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkDuoswitch.m_ColorBusyDown=byteValue;
					}
				}
				else
				{
					CString s;
					s.Format(_T("Error:Led configuration %d,Unknown Attribute '%s'"),
								m_CurrentIndex,
								szAttribute);

					ReportError(s);
					fResult=FALSE;
				}		
			} break;
			
			case STATE_LEDFDBK_VIRTUAL:
			{
				/********************************
				 * ColorOn=DEC;
				 * ColorOff=DEC;
				 ********************************/

				if (0==strcmp(szAttributeUpper,"COLORON"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkVirtual.m_ColorOn=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLOROFF"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkVirtual.m_ColorOff=byteValue;
					}
				}
				else
				{
					CString s;
					s.Format(_T("Error:Led configuration %d,Unknown Attribute '%s'"),
								m_CurrentIndex,
								szAttribute);

					ReportError(s);
					fResult=FALSE;
				}		

			} break;

			case STATE_LEDFDBK_SENSSTATE:
			{
				/********************************
				 * ColorStable=DEC;
				 * ColorHeating=DEC;
				 * ColorCooling=DEC;
				 ********************************/

				if (0==strcmp(szAttributeUpper,"COLORSTABLE"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkSensState.m_ColorStable=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORHEATING"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkSensState.m_ColorHeating=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORCOOLING"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkSensState.m_ColorCooling=byteValue;
					}
				}
				else
				{
					CString s;
					s.Format(_T("Error:Led configuration %d,Unknown Attribute '%s'"),
								m_CurrentIndex,
								szAttribute);

					ReportError(s);
					fResult=FALSE;
				}		
			} break;

			case STATE_LEDFDBK_SENSPRESET:
			{
				/********************************
				 * ColorOff=DEC;
				 * ColorPreset=DEC;DEC;DEC;DEC;
				 ********************************/
					
				if(0==strcmp(szAttributeUpper,"COLOROFF"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkSensPreset.m_ColorOff=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORPRESET"))
				{
					int v[4] = {-1,-1,-1,-1};
					int count=-1;

					count=sscanf(szData,"%d;%d;%d;%d;",&v[0],&v[1],&v[2],&v[3]);

					for(int i=0;i<count;++i)
					{
						ASSERT(v[i]>=0);

						m_pCurrSerLedCfg->v.m_fdbkSensPreset.m_ColorPreset[i]=v[i];
					}
				}
				else
				{
					CString s;
					s.Format(_T("Error:Led configuration %d,Unknown Attribute '%s'"),
								m_CurrentIndex,
								szAttribute);

					ReportError(s);
					fResult=FALSE;
				}	
			} break;

			case STATE_LEDFDBK_SENSSTATEONOFF:
			{
				/********************************
				 * ColorOn=DEC;
				 * ColorOff=DEC;
				 ********************************/
				
				if(0==strcmp(szAttributeUpper,"COLORON"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkSensStateOnOff.m_ColorOn=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLOROFF"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkSensStateOnOff.m_ColorOff=byteValue;
					}
				}
				else
				{
					CString s;
					s.Format(_T("Error:Led configuration %d,Unknown Attribute '%s'"),
								m_CurrentIndex,
								szAttribute);

					ReportError(s);
					fResult=FALSE;
				}	
			} break;

			case STATE_LEDFDBK_AUDIO:
			{
				/********************************
				 * ColorOff=DEC;
				 * ColorSrc=DEC;DEC;DEC;DEC;DEC;DEC;DEC;DEC;
				 ********************************/
				
				if(0==strcmp(szAttributeUpper,"COLOROFF"))
				{
					if(TRUE==helper_parseByteDEC(szData,&byteValue))
					{
						m_pCurrSerLedCfg->v.m_fdbkAudio.m_ColorOff=byteValue;
					}
				}
				else if (0==strcmp(szAttributeUpper,"COLORSRC"))
				{
					int v[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
					int count=-1;

					count=sscanf(szData,
								 "%d;%d;%d;%d;%d;%d;%d;%d;",
								 &v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7]);

					for(int i=0;i<count;++i)
					{
						ASSERT(v[i]>=0);

						m_pCurrSerLedCfg->v.m_fdbkAudio.m_ColorSrc[i]=v[i];
					}
				}
				else
				{
					CString s;
					s.Format(_T("Error:Led configuration %d,Unknown Attribute '%s'"),
								m_CurrentIndex,
								szAttribute);

					ReportError(s);
					fResult=FALSE;
				}	


			} break;

			case STATE_UNDEFINED:
			{
				// Wanneer een header niet op 1 staat...

				fResult=TRUE;
			} break;

			default:
			{
				ASSERT(0);

				fResult=FALSE;
			} break;
		}
	}

	return fResult;
}

void CConfigLedCfgLedClass::Initialise(void)
{
	// m_pCurrSerLedCfg ???

	removeAll( );

	m_CurrentIndex=0;
	m_pCurrSerLedCfg=0;
}
	
int CConfigLedCfgLedClass::GetMax(void) const
{
	return m_list.getCount( );
}

void CConfigLedCfgLedClass::dump( )
{
	// TODO ...
}

/***************************************************************/

void CConfigLedCfgLedClass::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerLedCfg* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

void CConfigLedCfgLedClass::ReportError(
	const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

DUOTECNO::DATA::CSerLedCfg* 
	CConfigLedCfgLedClass::GetEntry(int entry)
{
	ASSERT(entry<m_list.getCount());

	return m_list.getEntry(entry);
}


int CConfigLedCfgLedClass::GetData(
	int index,
	BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		static const int LENGTH=sizeof(CSerLedCfg);

		CSerLedCfg* const p=m_list.getEntry(index);
		if (0!=p)
		{
			helper_Dump(*p);

			memcpy(pbData,p,LENGTH);
			return LENGTH;
		}
	}

	pbData = 0;
	return 0;
}

BOOL CConfigLedCfgLedClass::IsUsed(
	int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}