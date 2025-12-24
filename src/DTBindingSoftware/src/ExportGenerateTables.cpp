// ExportGenerateTables.cpp: implementation of the CExportGenerateTables class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ExportGenerateTables.h"

#include "MainFrm.h"
#include "CanNode.h"

#include "nodescreenmapinfo.h"
#include "nodescheduleinfo.h"

#include "app\appCfg\AppCfginfo\cfginfonode.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"

// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
using namespace TMBindingDatabase;
// ---------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExportGenerateTables::CExportGenerateTables()
{
}

CExportGenerateTables::~CExportGenerateTables()
{

}

CString CExportGenerateTables::GetBindingFlags(CBindingEntry *pBindingEntry)
{
	CString str;
	const BYTE bBindingFlags = pBindingEntry->GetBindingReturnFlags();

	switch (bBindingFlags)
	{
		case BINDINGFLAGS_STATUS_NOCHANGE:
			str.Format("NO CHANGE"); 
		break;
		
		case BINDINGFLAGS_STATUS_PRE: 
			str.Format("PRE"); 
		break;

		case BINDINGFLAGS_STATUS_POST: 
			str.Format("POST"); 
		break;

		case BINDINGFLAGS_STATUS_POSTPULS: 
			str.Format("POST PULS"); 
		break;

		case BINDINGFLAGS_STATUS_PREPULS: 
			str.Format("PRE PULS"); 
		break;

		default : 
			str.Format("UNDEFINED"); 
		break;
	}	
	return str;
}

CString CExportGenerateTables::GetUnitType(BYTE bUnitType)
{
	CString str;

	switch (bUnitType) 
	{
		case UNIT_TYPE_DIM: 
			str.LoadString(IDS_UNIT_TYPE_DIM);	
		break;
		case UNIT_TYPE_SWITCH: 
			str.LoadString(IDS_UNIT_TYPE_SWITCH); 
		break;
		case UNIT_TYPE_CONTROL:
			str.LoadString(IDS_UNIT_TYPE_CONTROL); 
		break;
		case UNIT_TYPE_SENS:
			str.LoadString(IDS_UNIT_TYPE_SENS); 
		break;
		
		case UNIT_TYPE_AUDIO_HC4:
		case UNIT_TYPE_AUDIO:
			str.LoadString(IDS_UNIT_TYPE_AUDIO); 
		break;

		case UNIT_TYPE_LCD:
			str.LoadString(IDS_UNIT_TYPE_LCD); 
		break;
		case UNIT_TYPE_LCD_VIRTUAL: 
			str.LoadString(IDS_UNIT_TYPE_LCD_VIRTUAL); 
		break;
		case UNIT_TYPE_DUOSWITCH:
			str.LoadString(IDS_UNIT_TYPE_DUOSWITCH); 
		break;
		case UNIT_TYPE_SELECTOR	: 
			str.LoadString(IDS_UNIT_TYPE_SELECTOR);
		break;
		case UNIT_TYPE_BOSERS: 
			str.LoadString(IDS_UNIT_TYPE_BOSERS);
		break;      
		case UNIT_TYPE_AVMATRIX: 
			str.LoadString(IDS_UNIT_TYPE_AVMATRIX);
		break;
		case UNIT_TYPE_IRTX	: 
			str.LoadString(IDS_UNIT_TYPE_IRTX); 
		break;
		case UNIT_TYPE_RC5RX: 
			str.LoadString(IDS_UNIT_TYPE_RC5RX);
		break;
		case UNIT_TYPE_VIDEOMUX	: 
			str.LoadString(IDS_UNIT_TYPE_VIDEOMUX);
		break;
		case UNIT_TYPE_PS2INTELLIMOUSE	: 
			str.LoadString(IDS_UNIT_TYPE_PS2);
		break;
		case UNIT_TYPE_SPEECHCOMM: 
			str.LoadString(IDS_UNIT_TYPE_SPEECHCOMM);
		break;
		case UNIT_TYPE_SPMATRIX	: 
			str.LoadString(IDS_UNIT_TYPE_SPMATRIX);
		break;
		case UNIT_TYPE_ALARM: 
			str.LoadString(IDS_UNIT_TYPE_ALARM);
		break;	
		default:
			str.LoadString(IDS_UNIT_TYPE_UNDEFINED);
		break;
	}

	return str;
}


CString CExportGenerateTables::GetBindingDescription(CBindingEntry *pEntry,CBindingUnit *pUnit)
{	
	CString strReturn;
	CString strTemp;
	CString strEvent;

	ASSERT(pEntry && pUnit);

	if (pUnit->GetInOutType() == PM_INOUTTYPE_OUTPUT) 
	{
		strReturn.Format("(Out) : %s",
						 pUnit->ConvertFuntionToString());
		return strReturn;
	}
	
	const BYTE ev = pUnit->GetEvent();
	switch(ev) 
	{
		case 0x01 : strEvent = "L"; break;
		case 0x02 : strEvent = "S"; break;
		case 0x03 : strEvent = "P"; break;
		default : strEvent = "-------";	
	}
	
	CBindingUnits::Iterator it(pEntry->GetColUnit()->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingUnit* const pOutput = it.CurrentItem();
		ASSERT( pOutput );

		if (pOutput->GetInOutType() == PM_INOUTTYPE_OUTPUT)
		{
			strTemp = pOutput->GetName();
			strReturn.Format("%s : %s,%s",strEvent,
										  strTemp,
										  pOutput->ConvertFuntionToString());
			return strReturn;	
		}
	}
	return strReturn;
}


////////////////////

BOOL CExportGenerateTables::Get(int ItemInStrArray,int EntryInString,CStringArray *strArray,CString *strReturn)
{
	CString strArrayEntry;

	ASSERT(strReturn);
	ASSERT(strArray);

	strReturn->Empty();

	if (!GetItem(ItemInStrArray,strArray,&strArrayEntry)) 
		return FALSE;
	
	if (!GetEntry(EntryInString,strArrayEntry,strReturn)) 
		return FALSE;

	return TRUE;
}

BOOL CExportGenerateTables::GetItem(int item,CStringArray *pstrArray,CString *strReturn)
{
	CString strArrayEntry;

	ASSERT(strReturn);
	strReturn->Empty();

	if (item > pstrArray->GetSize()) return FALSE;

	strArrayEntry = pstrArray->GetAt(item);
	if (strArrayEntry.IsEmpty()) return FALSE;

	*strReturn = strArrayEntry;
	return TRUE;
}

BOOL CExportGenerateTables::GetEntry(int Entry,CString str,CString *strReturn)
{
	CString strWanted;

	int nLast = 0;
	int nCount = 0;
	int nEntryIndex = 0;
	
	int nStart = str.Find('<',0);
	if (nStart == -1) return FALSE;

	while(1) 
	{
		if (nEntryIndex++ == Entry)
		{
			nLast = str.Find('>',nStart+1);
			nStart +=1;
			nCount = nLast - nStart;

			strWanted = str.Mid(nStart,nCount);
			*strReturn = strWanted;
			return TRUE;
		}
		
		nStart = str.Find('<',nStart+1);
		if (nStart == -1) return FALSE;
	}
	return TRUE;
}

#ifdef WORKAROUND_BUG_0106

void CExportGenerateTables::ReplaceReservedChar(CString* pString)
{
	if (pString)
	{
		pString->Replace('>','?');
		pString->Replace('<','?');
	}
}

#endif


///////////////////////////////////////////////////////////////////////////

// Solved bug typo.
CExportGenerateNodeTable::CExportGenerateNodeTable() : 
CExportGenerateTables()
{
	strNodes.RemoveAll();
}

BOOL CExportGenerateNodeTable::Generate_NodeTable(void)
{		
	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CCanNode* const pNode = it.CurrentItem( );

		CString strArrayEntry;
		CString strName,strLog,strPhysical,strNrNodes;
		CString strType,strVersion,strBindings;
		CString strFlags;

		strName.Format(_T("%s"),pNode->szNodeName);		
		strPhysical.Format(_T("0x%06x"),(pNode->ulPhysicalAddress & 0xffffff));	// Sedert Version 16.09: Physical address beperken.
		strLog.Format(_T("0x%02x"),pNode->bLogicalAddress);
		strNrNodes.Format(_T("%d"),pNode->bNROfUnits);		
		strFlags.Format(_T("0x%02x"),pNode->bNodeFlags);

		switch(pNode->bNodeType) 
		{
			case NODE_PIN_STD_UNITS : 
			{
				strType.Format(_T("Std. Node")); 

			} break;

			case NODE_LCD: 
			{
				strType.Format(_T("LCD Node")); 

			} break;

			case NODE_PIN_MODEM: 
			{
				strType.Format(_T("Modem")); 
			
			} break;
			case NODE_PIN_GATEWAY: 
			{
				strType.Format(_T("Gateway")); 

			} break;
				
			default : 
			{
				strType.Format(_T("Res. Node")); 

			} break;
		}
		
		strVersion.Format(_T("V%02X"),pNode->GetSoftwareVersion());
		strBindings.Format(_T("%d"),pNode->getNodeBindingsNrEntries());

#ifdef WORKAROUND_BUG_0106

		ReplaceReservedChar(&strName);
		ReplaceReservedChar(&strPhysical);
		ReplaceReservedChar(&strLog);
		ReplaceReservedChar(&strNrNodes);
		ReplaceReservedChar(&strType);
		ReplaceReservedChar(&strVersion);					
		ReplaceReservedChar(&strBindings);
		ReplaceReservedChar(&strFlags);			
#endif

		strArrayEntry.Format(_T("<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>"),
								strName,
								strPhysical,
								strLog,
								strNrNodes,
								strType,
								strVersion,
								strBindings,
								strFlags);

		strNodes.Add(strArrayEntry);
	}
	return TRUE;
}

BOOL CExportGenerateNodeTable::GetName(int item,CString *strReturn)
{
	if(!Get(item,0,&strNodes,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeTable::GetPhysicalAddress(int item,CString *strReturn)
{
	if(!Get(item,1,&strNodes,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeTable::GetLogicalAddress(int item,CString *strReturn)
{
	if(!Get(item,2,&strNodes,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeTable::GetNrOfUnits(int item,CString *strReturn)
{
	if(!Get(item,3,&strNodes,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeTable::GetNodeType(int item,CString *strReturn){
	if(!Get(item,4,&strNodes,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeTable::GetSwVersion(int item,CString *strReturn){
	if(!Get(item,5,&strNodes,strReturn)) return FALSE;
	return TRUE;
}

BOOL CExportGenerateNodeTable::GetNrBindings(int item,CString *strReturn){
	if(!Get(item,6,&strNodes,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeTable::GetNodeFlags(int item,CString *strReturn)
{
	if(!Get(item,7,&strNodes,strReturn)) return FALSE;
	return TRUE;
}


///////////////////////////////////////////////////////////////
CExportGenerateNodeConfigTable::CExportGenerateNodeConfigTable( ) :
CExportGenerateTables( ) , m_strArray( ) { }

BOOL CExportGenerateNodeConfigTable::Generate_Table()
{
	BOOL result = FALSE;

	m_strArray.RemoveAll( );

	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CCanNode* const pNode = it.CurrentItem( );
		if (pNode)
		{
			const unsigned long ulSupportedFlags = CNodeExtConfigInfo_Util::getSupportedFlags( pNode );
			if (ulSupportedFlags != 0)
			{
				const unsigned long ulSupportedUnitCreate = CNodeCreateDynUnits_Util::getSupportedTypes( pNode );
				const int supportedUnitCreateType =  CNodeCreateDynUnits_Util::getNodeType(pNode);		// CHANGES_AV_UNIVERSAL
				const int supportedMMConfig = (int ) CNodeMultimediaConfig_Util::getNodeType( pNode );
				const int supportedBmpConfig = (int) CNodeBitmapConfig_Util::getNodetype( pNode );
				const int supportedMemoryBackup = (int) CNodeBackupConfig_Util::getNodeType( pNode );
				const int supportedSmapConfig = (int) CNodeScreenmapInfo_Util::getNodeType( pNode );
				const int supportedScheduleConfig = (int) CNodeScheduleConfigInfo_Util::getNodeType(pNode);
				const int supportedAVSettings = CNodeAVSettingsConfig_Util::getSupportedFlags(pNode);
				const unsigned long supportedConfigHttpSupport = CNodeConfigSupportHttpInfo_Util::getSupportedFlags(pNode);			
				
				/**********************************************************//**
				 * Item 0 = bLogical Address = GetLogicalAddress( )
				 * Item 1 = Product ID
				 * Item 2 = ulSupportedFlags = GetConfigFlags( )
				 * Item 3 = ulSupportedUnitCreate = GetConfigUnitCreate( )
				 * Item 4 = supportedUnitCreateType								// CHANGES_AV_UNIVERSAL deze ingevoegd.				
				 * Item 5 = supportedMMConfig = GetConfigMM( )
				 * Item 6 = supportedBmpConfig = GetConfigBitmap( )
				 * Item 7 = supportedMemoryConfig = GetConfigMemory( )
				 * Item 8 = supportedSmapConfig = GetConfigSmapType( )
				 * Item 9 = supportedScheduleConfig = GetConfigScheduleType()
				 * Item 10 = supportedAVSettings = CNodeExtConfigInfo_Util::getSupportedFlags()
				 * Item 11 = supportedConfigHttpSupport = http getSupportedFlags()
				 **************************************************************/
				CString strArrayEntry;		
				
				strArrayEntry.Format(_T("<0x%02x>,<0x%02x>,<0x%06x>,<0x%06x>,<0x%04x>,<0x%04x>,<0x%04x>,<0x%04x>,<0x%04x>,<0x%04x>,<0x%04x>,<0x%06x>"),
							    pNode->bLogicalAddress,
								pNode->GetProductID(),
								ulSupportedFlags,
								ulSupportedUnitCreate,	// CHANGES_AV_UNIVERSAL deze ingevoegd.	
								supportedUnitCreateType,
								supportedMMConfig,
								supportedBmpConfig,
								supportedMemoryBackup,
								supportedSmapConfig,
								supportedScheduleConfig,
								supportedAVSettings,
								supportedConfigHttpSupport);

				m_strArray.Add(strArrayEntry);
				result = TRUE;
			}
		}
	}
	return result;
}

BOOL CExportGenerateNodeConfigTable::GetLogicalAddress(int item,CString *strReturn)
{
	if(!Get(item,0,&m_strArray,strReturn)) return FALSE;
	return TRUE;
}

// Sedert versie 0x0804 extra veld bProductID
BOOL CExportGenerateNodeConfigTable::GetProductID(int item,CString *strReturn)
{
	if(!Get(item,1,&m_strArray,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeConfigTable::GetConfigFlags(int item,CString *strReturn)
{
	if(!Get(item,2,&m_strArray,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeConfigTable::GetConfigUnitCreate(int item,CString *strReturn)
{
	if(!Get(item,3,&m_strArray,strReturn)) return FALSE;
	return TRUE;
}

// CHANGES_AV_UNIVERSAL
BOOL CExportGenerateNodeConfigTable::GetConfigUnitCreateType(int item,CString* strReturn) {
	if(!Get(item,4,&m_strArray,strReturn)) return FALSE;
	return TRUE;
}

BOOL CExportGenerateNodeConfigTable::GetConfigMMType(int item,CString *strReturn)
{
	if(!Get(item,5,&m_strArray,strReturn)) return FALSE;		// CHANGES_AV_UNIVERSAL was 4
	return TRUE;
}
BOOL CExportGenerateNodeConfigTable::GetConfigBitmapType(int item,CString *strReturn)
{
	if(!Get(item,6,&m_strArray,strReturn)) return FALSE;		// CHANGES_AV_UNIVERSAL was 5
	return TRUE;
}
BOOL CExportGenerateNodeConfigTable::GetConfigMemoryType(int item,CString *strReturn)
{
	if(!Get(item,7,&m_strArray,strReturn)) return FALSE;		// CHANGES_AV_UNIVERSAL was 6
	return TRUE;
}
BOOL CExportGenerateNodeConfigTable::GetConfigSmapType(int item,CString *strReturn)
{
	if(!Get(item,8,&m_strArray,strReturn)) return FALSE;		// CHANGES_AV_UNIVERSAL was 7
	return TRUE;
}
BOOL CExportGenerateNodeConfigTable::GetConfigScheduleType(int item,CString *strReturn)
{
	if(!Get(item,9,&m_strArray,strReturn)) return FALSE;		// CHANGES_AV_UNIVERSAL was 8
	return TRUE;
}

BOOL CExportGenerateNodeConfigTable::GetConfigAVSettingsType(int item,CString *strReturn) {
	if(!Get(item,10,&m_strArray,strReturn)) return FALSE;		
	return TRUE;
}

BOOL CExportGenerateNodeConfigTable::GetConfigSupportHttpInfo(int item,CString *strReturn) {
	if(!Get(item,11,&m_strArray,strReturn)) return FALSE;		
	return TRUE;
}
//////////////////////////////////////////////////////////////

CExportGenerateUnitsTable::CExportGenerateUnitsTable() :
CExportGenerateTables( )
{
	strUnits.RemoveAll();
	strBindingUnits.RemoveAll();

	strNodeName.Empty();
	strPhysicalAddress.Empty();
	strLogicalAddress.Empty();
}

BOOL CExportGenerateUnitsTable::Generate_UnitsTable(unsigned long ulPhysicalAddress)
{	
	CCanNode *pNode;	
	int t;

	strUnits.RemoveAll();
	strUnits.FreeExtra();

	if (TMGetDocumentNodes()->FindPhysicalNode(ulPhysicalAddress, &t, &pNode)) 
	{
		Generate_BindingUnitsTable(ulPhysicalAddress);	

		strNodeName.Format(_T("%s"),pNode->szNodeName);
		strPhysicalAddress.Format(_T("0x%06x"),pNode->ulPhysicalAddress);
		strLogicalAddress.Format(_T("0x%02x"),pNode->bLogicalAddress);

		const int nUnits = pNode->bNROfUnits;
		
		for (int i=0;i<nUnits;i++) 
		{			
			STRUCT_UNIT *pUnit;
			if (TMGetDocumentNodes()->GetUnit(pNode, i, &pUnit)) 
			{		
				CString strArrayEntry;
				CString strLog;
				CString strConfig;

				const BYTE bUnitType = pUnit->bUnitType;
				switch( bUnitType)
				{
					case UNIT_TYPE_DIM:
					{
						strConfig.Format("%02x",pUnit->UnitSpecs.DimmerUnit.bDimConfig);

					} break;

					case UNIT_TYPE_SWITCH:
					{
						strConfig.Format("%02x",pUnit->UnitSpecs.RelayUnit.bSwitchConfig);

					} break;
/*
					case UNIT_TYPE_LCD_VIRTUAL:
					case UNIT_TYPE_CONTROL:
					{
						strConfig.Format("%02x",pUnit->UnitSpecs.ControlUnit.bControlConfig);	
					} break;
*/

					case UNIT_TYPE_SENS:
					{
						strConfig.Format("%02x",pUnit->UnitSpecs.SensorUnit.bSensConfig);	
					} break;

					case UNIT_TYPE_DUOSWITCH:
					{
						strConfig.Format("%02x",pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchConfig);	
					} break;

					case UNIT_TYPE_BOSERS:
					case UNIT_TYPE_AUDIO_HC4:
					case UNIT_TYPE_AUDIO:
					{
						strConfig.Format("%02x",pUnit->UnitSpecs.AudioUnit.bAudioConfig);	
					} break;

					case UNIT_TYPE_RC5RX:
					{
						strConfig.Format("%02x",pUnit->UnitSpecs.RC5RxUnit.bRC5RxConfig);		
					} break;

					
					case UNIT_TYPE_ALARM:
					{
						// TM_CHANGES_ALARM_ARITECH:
						// Config van een alarm unit wordt ook geexporteerd.

						strConfig.Format("%02x",pUnit->UnitSpecs.AlarmUnit.bAlarmConfig);			

					} break;

					default:
					{
						strConfig.Format("00");	
					} break;
				}

				CString strName(pUnit->szUnitName);							
				CString strType(GetUnitType(pUnit->bUnitType));

				strLog.Format("0x%02x",pUnit->bUnitAddress);


#ifdef WORKAROUND_BUG_0106

				ReplaceReservedChar(&strName);
				ReplaceReservedChar(&strLog);
				ReplaceReservedChar(&strType);
				ReplaceReservedChar(&strConfig);
							
#endif

				strArrayEntry.Format("<%s>,<%s>,<%s>,<%s>",
									 strName,
									 strLog,
									 strType,
									 strConfig);

				strUnits.Add(strArrayEntry);
			}
			else 
			{
				ASSERT(0);
				return FALSE;
			}
		}
		return TRUE; // All Units done ...
	}
	
	ASSERT(0);
	return FALSE;
}


BOOL CExportGenerateUnitsTable::GetName(int item,CString *strReturn)
{
	CString strArrayEntry;

	ASSERT(strReturn);
	strReturn->Empty();

	if (!GetItem(item,&strUnits,&strArrayEntry)) return FALSE;
	if (!GetEntry(0,strArrayEntry,strReturn)) return FALSE;

	return TRUE;	
}

BOOL CExportGenerateUnitsTable::GetAddress(int item,CString *strReturn)
{
	if(!Get(item,1,&strUnits,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateUnitsTable::GetType(int item,CString *strReturn)
{
	if(!Get(item,2,&strUnits,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateUnitsTable::GetConfig(int item,CString *strReturn)
{
	if(!Get(item,3,&strUnits,strReturn)) return FALSE;
	return TRUE;
}

BOOL CExportGenerateUnitsTable::Generate_BindingUnitsTable(unsigned long ulPhysicalAddress)
{		
	CCanNode *pNode;			
	int t;

	strBindingUnits.RemoveAll();
	strBindingUnits.FreeExtra();

	if (TMGetDocumentNodes()->FindPhysicalNode(ulPhysicalAddress, &t, &pNode)) 
	{		
		for (int i=0;i<pNode->bNROfUnits;i++) 
		{		
			STRUCT_UNIT *pUnit;	

			if (TMGetDocumentNodes()->GetUnit(pNode, i, &pUnit)) 
			{						
				FillTableUnitInfo(pUnit);	// pUnit = Hold Current Unit ...
			}			
		}
		return TRUE;	//all units done ...
	}
	
	ASSERT(0);
	return FALSE;
}

// TM 20081223: Gebruik van Iterators.
BOOL CExportGenerateUnitsTable::FillTableUnitInfo(STRUCT_UNIT *pUnit)
{
	int k,nLastID;

	CString strAddress;
	CString strBindingName,strBindingNr,strBindingID;
	CString strEvent;
	CString strAction,strOutputName,strOutputAddress;
	CString strArrayEntry;


	strArrayEntry.Empty();

	if (TMGetBindingEntries() != NULL)
	{	
		CBindingEntry *pBindingEntry = NULL;
		CBindingUnits *pBindingUnits = NULL;
		CBindingUnit *pBindingUnit = NULL;
		CCanNode *pNode = NULL;

		CBindingEntries::Iterator it(TMGetBindingEntries()->CreateIterator());
		for (it.Begin();!it.IsDone();it.Next())
		{			
			if ((pBindingEntry = it.CurrentItem()) != NULL)
			{							
				if ((pBindingUnits = pBindingEntry->GetColUnit()) != NULL)
				{						
					CBindingUnits::Iterator it2(pBindingUnits->CreateIterator());
					for (it2.Begin();!it2.IsDone();it2.Next())
					{
						pBindingUnit = it2.CurrentItem();
						ASSERT( pBindingUnit);
															
						if ((pBindingUnit->GetNodeAddress() == pUnit->bNodeLogicalAddress) &&
							(pBindingUnit->GetUnitAddress() == pUnit->bUnitAddress)) 
						{
							strAddress.Format(_T("0x%02x,0x%02x"),pUnit->bNodeLogicalAddress,pUnit->bUnitAddress);
				
							strBindingName.Format(_T("%s"),pBindingEntry->GetName());
							strBindingNr.Format(_T("0x%04x"),pBindingEntry->GetBindingNr());
							strBindingID.Format(_T("%d"),pBindingEntry->GetID());

							if (pBindingUnit->GetInOutType() == PM_INOUTTYPE_INPUT) 
							{
								strEvent.Format(_T("%s"),pBindingUnit->ConvertFuntionToString());					
						
								// CAUTION : pBindingUnit is here assigned last output unit with largest ID ...
								nLastID = 0;

								CBindingUnits::Iterator it3(pBindingUnits->CreateIterator());
								for (it3.Begin();!it3.IsDone();it3.Next())
								{
									pBindingUnit = it3.CurrentItem( );
									ASSERT( pBindingUnit);

									if (pBindingUnit->GetInOutType() == PM_INOUTTYPE_OUTPUT) 
									{
										if (nLastID < pBindingUnit->GetID()) 										
											nLastID = pBindingUnit->GetID();										
									}
								}												
						
								if (nLastID != 0) 
								{							
									if ((pBindingUnit = pBindingUnits->Find(nLastID)) != NULL)
									{													
										strAction.Format(_T("%s"),pBindingUnit->ConvertFuntionToString());
										strOutputName.Format(_T("%s"),pBindingUnit->GetName());

										if (TMGetDocumentNodes()->FindNode(pBindingUnit->GetNodeAddress(),
																		   &k,
																		   &pNode)) 
										{
											ASSERT(pNode);
											strOutputAddress.Format(_T("%06x (%02x)"),pNode->ulPhysicalAddress,
																					  pNode->bLogicalAddress);								
										}
										else 
										{
											ASSERT(0);
											strOutputAddress.Format(_T("Error"));
										}
									}
								}					
								else 
								{
									ASSERT(0);
									strAction.Format(_T("Error"));
									strOutputName.Format(_T("Error"));
									strOutputAddress.Format(_T("Error"));
								}					
							}
							else 
							{	// Binding Unit == Output ...
		
								strEvent.Format(_T("(Out)"));
								strAction.Format(_T("%s"),pBindingUnit->ConvertFuntionToString());
													
								strOutputName.Format(_T(""));
								strOutputAddress.Format(_T(""));
							}									
				

#ifdef WORKAROUND_BUG_0106

							ReplaceReservedChar(&strAddress);
							ReplaceReservedChar(&strBindingName);
							ReplaceReservedChar(&strEvent);
							ReplaceReservedChar(&strOutputName);
							ReplaceReservedChar(&strOutputAddress);
							ReplaceReservedChar(&strAction);
							ReplaceReservedChar(&strBindingID);							
#endif
							// BindingUnit == pUnit ...
							strArrayEntry.Format(_T("<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>"),strAddress,
																						  strBindingName,
																						  strEvent,
																						  strOutputName,
																						  strOutputAddress,
																						  strAction,
																						  strBindingID);
							strBindingUnits.Add(strArrayEntry);
			
						}// binding unit == pUnit...
					}// for all binding units ...
				} // for all bindings ...
			}
		}	
	}
	return TRUE;
}
BOOL CExportGenerateUnitsTable::GetBindingItem(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *strReturn)
{
	ASSERT(strReturn);

	BYTE bItemNodeAddress,bItemUnitAddress;
	int i,nCount;
	CString strArrayEntry;
	
	strReturn->Empty();

	nCount = 0;

	for (i=0;i<strBindingUnits.GetSize();i++) 
	{
		if (!GetItem(i,&strBindingUnits,&strArrayEntry)) 
			return FALSE;

		if (!GetBindingUnitAddress(strArrayEntry,&bItemNodeAddress,&bItemUnitAddress))
			return FALSE;

		if ((bItemNodeAddress == bNodeAddress) && (bItemUnitAddress == bUnitAddress)) 
		{
			if (nCount++ == bEntry) 
			{
				*strReturn = strArrayEntry;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CExportGenerateUnitsTable::GetBindingUnitAddress(CString str,BYTE *bNodeAddress,BYTE *bUnitAddress)
{
	CString strReturn;
	
	if (GetEntry(0,str,&strReturn) == FALSE) 
	{
		return FALSE;
	}

	int nTempNodeAddress = -1; 
	int nTempUnitAddress = -1;
	
	if ((sscanf((LPCTSTR) strReturn,"%x,%x",&nTempNodeAddress,&nTempUnitAddress) == 2) &&		// CHECKED_SSCANF_OK + CHANGES_MVS_2008_CSTRING
		(nTempNodeAddress >= 0x00 && nTempNodeAddress <= 0xFF) &&
		(nTempUnitAddress >= 0x00 && nTempUnitAddress <= 0xFF))
	{	
		*bNodeAddress = (BYTE) nTempNodeAddress;
		*bUnitAddress = (BYTE) nTempUnitAddress;
		return TRUE;
	}

	ASSERT( 0 );
	return FALSE;
}

int CExportGenerateUnitsTable::GetNrBindingEntries(BYTE bNodeAddress,BYTE bUnitAddress)
{
	BYTE bItemNodeAddress,bItemUnitAddress;

	int i,nCount;
	CString strArrayEntry;

	nCount = 0;

	for (i=0;i<strBindingUnits.GetSize();i++) {
		if (!GetItem(i,&strBindingUnits,&strArrayEntry)) 
			return FALSE;

		if (!GetBindingUnitAddress(strArrayEntry,&bItemNodeAddress,&bItemUnitAddress))
			return FALSE;

		if ((bItemNodeAddress == bNodeAddress) && (bItemUnitAddress == bUnitAddress)) {
			nCount ++;
		}
	}
	return nCount;
}

BOOL CExportGenerateUnitsTable::GetBindingName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str)
{
	CString strArrayEntry;

	if(!GetBindingItem(bNodeAddress,bUnitAddress,bEntry,&strArrayEntry)) {
		ASSERT(0);	
		return FALSE;
	}

	if(!GetEntry(1,strArrayEntry,str)) {
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

BOOL CExportGenerateUnitsTable::GetBindingAction(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str)
{
	CString strArrayEntry;

	if(!GetBindingItem(bNodeAddress,bUnitAddress,bEntry,&strArrayEntry)) {
		ASSERT(0);	
		return FALSE;
	}

	if(!GetEntry(5,strArrayEntry,str)) {
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

BOOL CExportGenerateUnitsTable::GetBindingEvent(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str)
{
	CString strArrayEntry;

	if(!GetBindingItem(bNodeAddress,bUnitAddress,bEntry,&strArrayEntry)) {
		ASSERT(0);	
		return FALSE;
	}

	if(!GetEntry(2,strArrayEntry,str)) {
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

BOOL CExportGenerateUnitsTable::GetBindingOutput(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str)
{
	CString strArrayEntry;
	CString szName,szAddress;

	if(!GetBindingItem(bNodeAddress,bUnitAddress,bEntry,&strArrayEntry)) {
		ASSERT(0);	
		return FALSE;
	}

	if(!GetEntry(3,strArrayEntry,&szName)) {
		ASSERT(0);
		return FALSE;
	}

	if(!GetEntry(4,strArrayEntry,&szAddress)) {
		ASSERT(0);
		return FALSE;
	}

	str->Format("%s -%s",szName,szAddress);
	return TRUE;
}

BOOL CExportGenerateUnitsTable::GetBindingID(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str)
{
	CString strArrayEntry;

	if(!GetBindingItem(bNodeAddress,bUnitAddress,bEntry,&strArrayEntry)) {
		ASSERT(0);	
		return FALSE;
	}

	if(!GetEntry(6,strArrayEntry,str)) {
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}


//////
CExportGenerateBindingsTable::CExportGenerateBindingsTable()
{
	strInputs.RemoveAll();
	strOutputs.RemoveAll();
	strTimers.RemoveAll();
	
	strEvalString.Empty();
	strBindingName.Empty();
	strBindingNumber.Empty();
	strBindingID.Empty();
	strBindingFlags.Empty();

	nCurrEntry = 0;
}


BOOL CExportGenerateBindingsTable::Generate(CBindingEntry* pBindingEntry)
{
	strInputs.RemoveAll();
	strOutputs.RemoveAll();
	strTimers.RemoveAll();

	strEvalString.Empty();
	strBindingName.Empty();
	strBindingNumber.Empty();
	strBindingID.Empty();
	strBindingFlags.Empty();
		
	strEvalString.Format(_T("%s"),pBindingEntry->GetEvaluationString()->Get());

	strBindingName.Format(_T("%s"),pBindingEntry->GetName());
	strBindingNumber.Format(_T("0x%04x"),pBindingEntry->GetBindingNr());
	strBindingID.Format(_T("%i"),pBindingEntry->GetID());
	strBindingFlags = CExportGenerateTables::GetBindingFlags(pBindingEntry);

	CBindingUnits::Iterator it(pBindingEntry->GetColUnit()->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingUnit* const pBindingUnit = it.CurrentItem();
		ASSERT( pBindingUnit );

		if (pBindingUnit->GetInOutType() == PM_INOUTTYPE_INPUT) 
		{
			if (AddInputUnit(pBindingUnit) == FALSE) 			
				return FALSE;
			
		}
		else 
		{
			if (AddOutputUnit(pBindingUnit) == FALSE) 					
				return FALSE;					
		}
	}

	/** @todo	Via Iterators */
	CBindingOperators *pBindingOperators = 0;
	if ((pBindingOperators = pBindingEntry->GetColOperator()) != 0) 
	{				
		CBindingOperator *pBindingOperator = 0;
		for (int i=0;i<pBindingOperators->Count();i++) 
		{					
			if ((pBindingOperator = pBindingOperators->Get(i)) != 0)
			{										
				AddTimerOperator(pBindingOperator);			
			}

			ASSERT(pBindingOperator);
		}
	}
	return TRUE;	
}

/**
 * Date 20060322 : Changed functions
 */
BOOL CExportGenerateBindingsTable::Generate_BindingTable(unsigned short nEntry)
{
	if (TMGetBindingEntries() != NULL)
	{	
		if ((TMGetBindingEntries()->Count() == 0) || 
			(nEntry >= TMGetBindingEntries()->Count()))
		{
			return FALSE;
		}
		
		CBindingEntry *pBindingEntry=NULL;
	
		if ((pBindingEntry = TMGetBindingEntries()->GetEntry(nEntry)) != NULL)
		{
			return (Generate(pBindingEntry));

		}	
	}

	strInputs.RemoveAll();
	strOutputs.RemoveAll();
	strTimers.RemoveAll();

	strEvalString.Empty();
	strBindingName.Empty();
	strBindingNumber.Empty();
	strBindingID.Empty();
	strBindingFlags.Empty();


	ASSERT(0);
	return FALSE;
}

BOOL CExportGenerateBindingsTable::Generate_BindingTable(CBindingEntry* pEntry)
{
	if (pEntry != 0)
	{
		return (Generate(pEntry));
	}

	return FALSE;
}

		
BOOL CExportGenerateBindingsTable::AddInputUnit(CBindingUnit *pUnit)
{		
	int t;
	CCanNode *pNode = NULL;
	CString strName,strPhAddress,strLogAddress,strEvent,strID;
	CString strArrayEntry;
	ASSERT(pUnit);

	strName.Format(_T("%s"),pUnit->GetName());
	const BYTE bNodeAddress = pUnit->GetNodeAddress();
	
	strLogAddress.Format(_T("0x%02x,0x%02x"),pUnit->GetNodeAddress(),pUnit->GetUnitAddress());
	strEvent.Format(_T("%s"),pUnit->ConvertFuntionToString());
	strID.Format(_T("'U%x'"),pUnit->GetID());

	if (TMGetDocumentNodes()->FindNode(bNodeAddress, &t,&pNode)) 
	{	
		strPhAddress.Format(_T("0x%06x"),pNode->ulPhysicalAddress);
		
		strArrayEntry.Format(_T("<%s>,<%s>,<%s>,<%s>,<%s>"),strName,strPhAddress,strLogAddress,strEvent,strID);
		strInputs.Add(strArrayEntry);

		return TRUE;
	}	
	return FALSE;
}

BOOL CExportGenerateBindingsTable::AddOutputUnit(CBindingUnit *pUnit)
{	
	int t;
	CCanNode *pNode = NULL;
	CString strName,strPhAddress,strLogAddress,strFunction,strID;
	CString strArrayEntry;
	ASSERT(pUnit);

	strName.Format(_T("%s"),pUnit->GetName());
	const BYTE bNodeAddress = pUnit->GetNodeAddress();
	strLogAddress.Format(_T("0x%02x,0x%02x"),pUnit->GetNodeAddress(),pUnit->GetUnitAddress());
	strFunction.Format(_T("%s"),pUnit->ConvertFuntionToString());
	strID.Format(_T("'U%x'"),pUnit->GetID());

	if (TMGetDocumentNodes()->FindNode(bNodeAddress, &t, &pNode)) 
	{	
		strPhAddress.Format(_T("0x%06x"),pNode->ulPhysicalAddress);
	
		strArrayEntry.Format(_T("<%s>,<%s>,<%s>,<%s>,<%s>"),strName,strPhAddress,strLogAddress,strFunction,strID);
		strOutputs.Add(strArrayEntry);

		return TRUE;
	}
	
	return FALSE;
}

BOOL CExportGenerateBindingsTable::AddTimerOperator(CBindingOperator *pOperator)
{
	CString strArrayEntry;
	CString strType,strID,strValue;
	TimerType Type;
	BYTE bTimeValue;

	ASSERT(pOperator);

	if (pOperator->GetType() == PM_OPERATOR_TIMER) 
	{		
		Type = ((CBindingTimerOperator*) pOperator)->GetTimerType();
		if (Type < MAX_PM_TIMERS) {
			strType.LoadString(TimerStrings[Type]);
		}
		
		bTimeValue = ((CBindingTimerOperator*)pOperator)->GetTimeValue();
		if (bTimeValue <=59) {
			strValue.Format("%02d sec.",bTimeValue);
		}
		else {
			strValue.Format("%02d min.",bTimeValue-59);
		}
		strID.Format(_T("'T%x'"),pOperator->GetID());

		strArrayEntry.Format(_T("<%s>,<%s>,<%s>"),strType,strValue,strID);
		strTimers.Add(strArrayEntry);
		return TRUE;
	}
	return FALSE;
}

BOOL CExportGenerateBindingsTable::GetUnitName(int item,CString *strReturn,CStringArray *strArray)
{
	ASSERT(strArray);
	if (!Get(item,0,strArray,strReturn)) return FALSE;
	return TRUE;
}

BOOL CExportGenerateBindingsTable::GetUnitPhAddress(int item,CString *strReturn,CStringArray *strArray)
{
	ASSERT(strArray);
	if (!Get(item,1,strArray,strReturn)) return FALSE;
	return TRUE;
}

BOOL CExportGenerateBindingsTable::GetUnitLogAddress(int item,CString *strReturn,CStringArray *strArray)
{
	ASSERT(strArray);
	if (!Get(item,2,strArray,strReturn)) return FALSE;
	return TRUE;
}

BOOL CExportGenerateBindingsTable::GetUnitDescription(int item,CString *strReturn,CStringArray *strArray)
{
	ASSERT(strArray);
	if (!Get(item,3,strArray,strReturn)) return FALSE;
	return TRUE;
}

BOOL CExportGenerateBindingsTable::GetUnitId(int item,CString *strReturn,CStringArray *strArray)
{
	ASSERT(strArray);
	if (!Get(item,4,strArray,strReturn)) return FALSE;
	return TRUE;
}

//

BOOL CExportGenerateBindingsTable::GetTimerType(int Entry,CString *strReturn) 
{
	if (!Get(Entry,0,&strTimers,strReturn)) return FALSE;
	return TRUE;
}

BOOL CExportGenerateBindingsTable::GetTimerValue(int Entry,CString *strReturn) 
{
	if (!Get(Entry,1,&strTimers,strReturn)) return FALSE;
	return TRUE;
}

BOOL CExportGenerateBindingsTable::GetTimerID(int Entry,CString *strReturn) 
{
	if (!Get(Entry,2,&strTimers,strReturn)) return FALSE;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////
CExportGenerateNodeUnitTable::CExportGenerateNodeUnitTable() :
CExportGenerateTables( )
{
	strNodeUnits.RemoveAll();
}

BOOL CExportGenerateNodeUnitTable::Generate_NodeUnitTable(void)
{
	CString strArrayEntry;
	CString strNodeAddress,strUnitAddress,strUnitName;
	CString strUnitType,strPhAddress;
	CString strNodeName;
		
	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CCanNode* const pNode = it.CurrentItem( );

		strNodeAddress.Format(_T("0x%02x"),pNode->bLogicalAddress);
		strPhAddress.Format(_T("0x%06x"),pNode->ulPhysicalAddress);
		strNodeName.Format(_T("%s"),pNode->szNodeName);

		CUnitIterator it2(TMGetDocumentNodes()->CreateUnitIterator( pNode ));
		for (it2.Begin();!it2.IsDone();it2.Next())
		{
			STRUCT_UNIT* const pUnit = it2.CurrentItem( );

			strUnitAddress.Format(_T("0x%02x"),pUnit->bUnitAddress);
			strUnitName.Format(_T("%s"),pUnit->szUnitName);
			strUnitType.Format(_T("%s"),GetUnitType(pUnit->bUnitType));


#ifdef WORKAROUND_BUG_0106

			ReplaceReservedChar(&strNodeAddress);
			ReplaceReservedChar(&strNodeName);
			ReplaceReservedChar(&strUnitAddress);
			ReplaceReservedChar(&strUnitName);
			ReplaceReservedChar(&strUnitType);
			ReplaceReservedChar(&strPhAddress);					
#endif

			strArrayEntry.Format(_T("<%s>,<%s>,<%s>,<%s>,<%s>,<%s>"),
									strNodeAddress,
									strNodeName,
									strUnitAddress,
									strUnitName,
									strUnitType,
									strPhAddress);									

			strNodeUnits.Add(strArrayEntry);	
		}
	}
	return TRUE;		
}
BOOL CExportGenerateNodeUnitTable::GetLogicalAddress(int item,CString *strReturn)
{
	if(!Get(item,0,&strNodeUnits,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeUnitTable::GetNodeName(int item,CString *strReturn)
{
	if(!Get(item,1,&strNodeUnits,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeUnitTable::GetUnitAddress(int item,CString *strReturn)
{
	if(!Get(item,2,&strNodeUnits,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeUnitTable::GetName(int item,CString *strReturn)
{
	if(!Get(item,3,&strNodeUnits,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeUnitTable::GetType(int item,CString *strReturn)
{
	if(!Get(item,4,&strNodeUnits,strReturn)) return FALSE;
	return TRUE;
}
BOOL CExportGenerateNodeUnitTable::GetPhysicalAddress(int item,CString *strReturn)
{
	if(!Get(item,5,&strNodeUnits,strReturn)) return FALSE;
	return TRUE;
}
