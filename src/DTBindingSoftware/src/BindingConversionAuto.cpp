// BindingConversionAuto.cpp: implementation of the CBindingConversionAuto class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConversionAuto.h"

#include "BindingLink.h"
#include "BindingLinks.h"
#include "BindingUnits.h"

#include "app\appbindings\appbindingunit\bindingunit_api.h"
#include "app\appbindings\appbindingunit\bindingunitdim_def.h"

#include "BindingOperators.h"
#include "BindingOperator.h"
#include "BindingEntry.h"
#include "NodeMess.h"
#include "BindingOperatorFactory.h"
#include "BindingUnitFactory.h"
#include "BindingUnitsOperations.h"
#include "BindingLinkFactory.h"
#include "BindingEntriesOperations.h"

// Sedert versie 0x0C02
#include "app\appcfg\appcfginfo\cfginfobindingunit.h"

#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CBindingConversionAutoInfo::Report()
{											
	for (int i=0;i<GetNrMessages();i++) 
	{
		CString szMessage;		

		if (GetMessage(i,&szMessage) == TRUE) 
		{		
			CDisplayDevice::DisplayInfoMsg(szMessage);
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingConversionAuto::CBindingConversionAuto()
{
}

CBindingConversionAuto::~CBindingConversionAuto()
{
	AutoBindingEntries.RemoveAll();
}

BOOL CBindingConversionAuto::GetBindingEntry(int index,CBindingEntry **pReturnEntry)
{
	CString szError;

	if (index > AutoBindingEntries.Count()) 
	{
		return FALSE;
	}

	ASSERT(index >= 0 && index <= 0xFFFF);													// CHANGES_MVS_2008_WARNINGS
	*pReturnEntry = AutoBindingEntries.GetEntry(static_cast<unsigned short>(index));		// CHANGES_MVS_2008_WARNINGS
	
	if (*pReturnEntry == NULL) 
	{
		ASSERT(0);
		szError.Format(_T("Fatal Error :\n"
						  "CBindingConversionAuto::GetBindingEntry(index,**pReturnEntry)\n"
						  "Nr of Entries in Auto binding database : %d\n"
						  "Index = %d, pReturnEntry = NULL"),AutoBindingEntries.Count(),index);
		AfxMessageBox(szError);
		return FALSE;			              
	}

	return TRUE;
}

/********************************************************************************

  Task : Controls + Dimmers (+switches)
*********************************************************************************/
BOOL CBindingConversionAuto::CheckTemplate_Dimmers(CBindingAutoTemplate **pReturn)
{
	BOOL fResult=FALSE;
	const int nBindingUnits = NrUnitInfo.nBindingUnits;
	if (nBindingUnits != NrUnitInfo.nUniqueBindingUnits) {
		return FALSE;	// V17.03: Extra controle of de unit adressen van de binding units uniek zijn.
	}

	const int nVirtualInputUnits = NrUnitInfo.virtualUnit.nrOfInputs;
	const int nDimUnits = NrUnitInfo.dimUnit.nrOfUnits;
	const int nSwitchUnits = NrUnitInfo.nSwitchUnits;
	const int nControlUnits = NrUnitInfo.nControlUnits;

	if (nVirtualInputUnits>0)
	{
		// Sedert versie 0x0C02:
		// Indien er virtual input units aanwezg zijn dan is de auto binding enkel mogelijk voor aan/uit
		// maar niet voor de PIR functie...

		if (DIMMER_AUTO_OPTION_ONOFF==AutoBindingOptions.GetAutoDimmerOption())
		{
			// Sedert versie 0x0C02: Bijkomende controle:
			// Er moeten dimmer units + Control units aanwezig zijn !
			if (((nDimUnits>0)&&(nControlUnits+nVirtualInputUnits>0))&&
			     (nBindingUnits==(nDimUnits+nControlUnits+nSwitchUnits+nVirtualInputUnits)))
			{
				fResult=TRUE;	
			}
		}
	}
	else
	{
		// Sedert versie 0x0C02: Bijkomende controle:
		// Er moeten dimmer units + Control units aanwezig zijn !
		if (((nDimUnits>0)&&(nControlUnits>0))&&
		    (nBindingUnits==(nDimUnits+nControlUnits+nSwitchUnits))) 
		{
			fResult=TRUE;
		}
	}

	if (TRUE==fResult)
	{
		CBindingAutoTemplate *pNewAutoTemplate = new CBindingAutoTemplate_Dimmer(this);
		if (0==pNewAutoTemplate)
		{
			return FALSE;
		}

		*pReturn = pNewAutoTemplate;		
	}

	return fResult;
}
/********************************************************************************

  Task : Controls + Switches 
*********************************************************************************/
BOOL CBindingConversionAuto::CheckTemplate_Relais(CBindingAutoTemplate **pReturn)
{	
	BOOL fResult=FALSE;

	// Als er dimmer units aanwezig zijn dat moet de template voor de 
	// dimmers gebruikt worden...
	const int nDimUnits = NrUnitInfo.dimUnit.nrOfUnits;
	if (nDimUnits>0) {
		return FALSE;
	}

	const int nBindingUnits = NrUnitInfo.nBindingUnits;
	if (nBindingUnits != NrUnitInfo.nUniqueBindingUnits) {
		return FALSE;	// V17.03: Extra controle of de unit adressen van de binding units uniek zijn.
	}

	const int nVirtualInputUnits = NrUnitInfo.virtualUnit.nrOfInputs;
	const int nSwitchUnits = NrUnitInfo.nSwitchUnits;
	const int nControlUnits = NrUnitInfo.nControlUnits;

	if (nVirtualInputUnits>0)
	{
		if (RELAIS_AUTO_OPTION_ONOFF==AutoBindingOptions.GetAutoRelaisOption())
		{
			if (nBindingUnits==(nSwitchUnits+nControlUnits+nVirtualInputUnits)) 
			{
				fResult=TRUE;	
			}
		}
	}
	else
	{
		if (nBindingUnits==(nSwitchUnits+nControlUnits)) 
		{
			fResult=TRUE;	
		}
	}

	if(TRUE==fResult)
	{
		CBindingAutoTemplate *pNewAutoTemplate= 
			new CBindingAutoTemplate_Switch(this);

		if (0==pNewAutoTemplate)
		{
			return FALSE;
		}
		*pReturn = pNewAutoTemplate;
	}

	return fResult;
}
/********************************************************************************
  TASK : Controls + Switches , nBindingUnits max 3 && nControlUnits < 2
*********************************************************************************/
// V17.03: zie BUG-0183 - De adressen van de binding units moeten uniek zijn.
BOOL CBindingConversionAuto::CheckTemplate_Moods(CBindingAutoTemplate **pReturn) {
	CBindingAutoTemplate *pNewAutoTemplate;
	const int nBindingUnits = NrUnitInfo.nBindingUnits;	
	if (nBindingUnits != NrUnitInfo.nUniqueBindingUnits) {
		return FALSE;	// zie BUG-0183
	}

	const int nVirtualUnits = NrUnitInfo.virtualUnit.nrOfUnits;
	const int nControlUnits = NrUnitInfo.nControlUnits;
	if (nBindingUnits > 3) return FALSE;
	if (nControlUnits > 2) return FALSE;

	if (nBindingUnits == nVirtualUnits + nControlUnits) {
		pNewAutoTemplate = new CBindingAutoTemplate_Moods(this);
		*pReturn = pNewAutoTemplate;
		return TRUE;	
	}
	return FALSE;
}

/********************************************************************************

  TASK : Sens + Relais && max 2 Relais 
*********************************************************************************/
BOOL CBindingConversionAuto::CheckTemplate_Sens(CBindingAutoTemplate **pReturn)
{
	CBindingAutoTemplate *pNewAutoTemplate;

	const int nBindingUnits = NrUnitInfo.nBindingUnits;
	const int nSensUnits = NrUnitInfo.nSensUnits;
	const int nSwitchUnits = NrUnitInfo.nSwitchUnits;

	if (nSwitchUnits > 2) return FALSE;

	if (nBindingUnits == nSensUnits + nSwitchUnits) 
	{
		pNewAutoTemplate = new CBindingAutoTemplate_Sens(this);
		*pReturn = pNewAutoTemplate;
		return TRUE;	
	}
	return FALSE;
}

/********************************************************************************

  TASK : Controls + Bose Room && max 1 Bose Room && max 2 Controls
*********************************************************************************/
#ifdef SKIP
		BOOL CBindingConversionAuto::CheckTemplate_BoseRs(CBindingAutoTemplate **pReturn)
		{
			CBindingAutoTemplate *pNewAutoTemplate;

			const int nBindingUnits = NrUnitInfo.nBindingUnits;
			const int nControlUnits = NrUnitInfo.nControlUnits;
			const int nBoseRsUnits = NrUnitInfo.nBoseUnits;
			const int nBoseLf48Units = NrUnitInfo.nBoseLf48Units;

			if (nBoseRsUnits > 1) return FALSE;
			if (nControlUnits > 2) return FALSE;
			if (nBoseLf48Units > 1) return FALSE;

			if (nBindingUnits == nBoseRsUnits + nControlUnits + nBoseLf48Units) 
			{
				pNewAutoTemplate = new CBindingAutoTemplate_Bose(this);
				*pReturn = pNewAutoTemplate;
				return TRUE;	
			}
			return FALSE;
		}
#endif


// Altijd 2 inputs + 1 Bose RS unit.
BOOL CBindingConversionAuto::CheckTemplate_BoseRs(CBindingAutoTemplate **pReturn)
{
	const int nBindingUnits = NrUnitInfo.nBindingUnits;
	const int nControlUnits = NrUnitInfo.nControlUnits;
	const int nBoseRsUnits = NrUnitInfo.nBoseUnits;
	const int nVirtualInputUnits = NrUnitInfo.virtualUnit.nrOfInputs;

	if (NrUnitInfo.boseType==CBindingConversionAutoUnitInfo::BOSE_TYPE_UNDEFINED) 
		return FALSE;

	if (nBoseRsUnits>1)
		return FALSE;

	if ((nControlUnits+nVirtualInputUnits)>2) 
		return FALSE;

	// Ofwel 2 controls , ofwel 2 virtuals: Geen combinatie van de 2 !
	if (nControlUnits==nVirtualInputUnits)
		return FALSE;

	if (nBindingUnits==(nBoseRsUnits+nControlUnits+nVirtualInputUnits)) 
	{
		*pReturn = new CBindingAutoTemplate_Bose(this);	
		return TRUE;	
	}
	return FALSE;
}

/********************************************************************************

  TASK : Controls + Bose Room && max 1 Bose Room && max 2 Controls
*********************************************************************************/
BOOL CBindingConversionAuto::CheckTemplate_DuoSwitch(CBindingAutoTemplate **pReturn) {
	const int nBindingUnits = NrUnitInfo.nBindingUnits;
	if (nBindingUnits != NrUnitInfo.nUniqueBindingUnits) {
		return FALSE;	// V17.03: Extra controle of de unit adressen van de binding units uniek zijn.
	}

	const int nControlUnits = NrUnitInfo.nControlUnits;
	const int nDuoSwitchUnits = NrUnitInfo.nDuoSwitchUnits;
	const int nVirtualInputUnits = NrUnitInfo.virtualUnit.nrOfInputs;

	if ((nControlUnits+nVirtualInputUnits)>2) 
		return FALSE;

	// Ofwel 2 controls , ofwel 2 virtuals: Geen combinatie van de 2 !
	if (nControlUnits==nVirtualInputUnits)
		return FALSE;

	if (nBindingUnits==(nControlUnits+nVirtualInputUnits+nDuoSwitchUnits)) 
	{
		CBindingAutoTemplate* const pNewAutoTemplate = new CBindingAutoTemplate_DuoSwitch(this);
		if (0!=pNewAutoTemplate)
		{
			*pReturn = pNewAutoTemplate;
			return TRUE;	
		}
	}
	return FALSE;
}
/********************************************************************************

  TASK : Controls + Bose Room && max 1 Bose Room && max 2 Controls
*********************************************************************************/
BOOL CBindingConversionAuto::CheckTemplate_CondPir(CBindingAutoTemplate **pReturn) {	
	const int nBindingUnits = NrUnitInfo.nBindingUnits;
	if (nBindingUnits != NrUnitInfo.nUniqueBindingUnits) {
		return FALSE;	// V17.03: Extra controle of de unit adressen van de binding units uniek zijn.
	}

	const int nDimUnits = NrUnitInfo.dimUnit.nrOfUnits;
	const int nSwitchUnits = NrUnitInfo.nSwitchUnits;
	const int nControlUnits = NrUnitInfo.nControlUnits;
	const int nVirtualUnits = NrUnitInfo.virtualUnit.nrOfUnits;

	if ((nVirtualUnits == 0)||(nVirtualUnits>3)) return FALSE;
	if (nControlUnits == 0) return FALSE;
	if ((nDimUnits == 0) && (nSwitchUnits == 0)) return FALSE; 


	CBindingAutoTemplate *pNewAutoTemplate = NULL;
	if (nBindingUnits==(nControlUnits+nVirtualUnits+nDimUnits+nSwitchUnits)) {
		pNewAutoTemplate = new CBindingAutoTemplate_CondPIR(this);
		*pReturn = pNewAutoTemplate;
		return TRUE;	
	}
	return FALSE;
}

BOOL CBindingConversionAuto::CheckTemplate_Audio(
	CBindingAutoTemplate **pReturn)
{
	const int nBindingUnits = NrUnitInfo.nBindingUnits;
	const int nControlUnits = NrUnitInfo.nControlUnits;
	const int nVirtualInputUnits = NrUnitInfo.virtualUnit.nrOfInputs;
	const int nAudioUnitsExtended = NrUnitInfo.nAudioUnitsExtended;

	if (nBindingUnits!=nControlUnits+nVirtualInputUnits+nAudioUnitsExtended)
		return FALSE;

	if (nVirtualInputUnits!=NrUnitInfo.virtualUnit.nrOfUnits)
		return FALSE;

	if (nVirtualInputUnits+nControlUnits!=2)
		return FALSE;

	if ((nVirtualInputUnits==2)&&(nControlUnits==2))
		return FALSE;

	// Oppassen:
	// De bindings werken enkel voor 1 audio unit !!
	if (nAudioUnitsExtended==1)
	{
		CBindingAutoTemplate* const pNewAutoTemplate = new CBindingAutoTemplate_AudioExt(this);
		if (0!=pNewAutoTemplate)
		{
			*pReturn = pNewAutoTemplate;
			return TRUE;
		}
	}
	return FALSE;
}

// 
BOOL CBindingConversionAuto::CheckTemplate_DimmersFwd(CBindingAutoTemplate **pReturn)
{
#ifdef SKIP
	enum  { MAX_DIMMER_INPUTS = 6 };
#endif

	const int nBindingUnits = NrUnitInfo.nBindingUnits;
	if (nBindingUnits != NrUnitInfo.nUniqueBindingUnits) {
		return FALSE;	// V17.03: Extra controle of de unit adressen van de binding units uniek zijn.
	}
	const int nDimUnits = NrUnitInfo.dimUnit.nrOfUnits;
	const int nDimUnitsInputs = NrUnitInfo.dimUnit.nrOfInputs;
	const int nRelaisUnits = NrUnitInfo.nSwitchUnits;

	if (nBindingUnits!=(nDimUnitsInputs+nRelaisUnits))
		return FALSE;

	if (nRelaisUnits!=1)
		return FALSE;

#ifdef SKIP
	if (nDimUnitsInputs>MAX_DIMMER_INPUTS)
	{
		CString szMessage;
		szMessage.Format(_T("Cannot make the auto binding because there are %d dimmer units.\n"
							"The maximum allowed dimmer units are %d.\n"
							"To solve the problem divide the number of dimmer units over several autobindings."), 
							nDimUnitsInputs,
							MAX_DIMMER_INPUTS);

		AfxMessageBox(szMessage);
		return FALSE;
	}
#endif

	CBindingAutoTemplate* const p = new CBindingAutoTemplate_DimmerFWD(this);
	if (0!=p)
	{
		*pReturn=p;
		return TRUE;
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////
BYTE CBindingConversionAuto::ProcessAutoBindings(CBindingUnits *pColUnits /*IN*/)
{
	ASSERT(pColUnits);
	
	CBindingAutoTemplate *pBindingAutoTemplates = NULL;
	int nAutoBindings = 0;

	m_pgBindingUnits = pColUnits;
	ASSERT(0!=m_pgBindingUnits);

	InitNewProcess();
	RetrieveNrOfUnits();
		
	if (NrUnitInfo.nBindingUnits == 0) 
		return 0;

	enum { MAX_TEMPLATE_FUNCTIONS = 9 };
	typedef BOOL (CBindingConversionAuto::*Template)(CBindingAutoTemplate **pReturn);
	
	Template TemplateFunctions[MAX_TEMPLATE_FUNCTIONS] = 
	{
		&CBindingConversionAuto::CheckTemplate_Relais,			// CHANGES_MVS_2008
		&CBindingConversionAuto::CheckTemplate_Dimmers,			// CHANGES_MVS_2008
		&CBindingConversionAuto::CheckTemplate_Moods,			// CHANGES_MVS_2008
		&CBindingConversionAuto::CheckTemplate_Sens,			// CHANGES_MVS_2008
		&CBindingConversionAuto::CheckTemplate_BoseRs,			// CHANGES_MVS_2008
		&CBindingConversionAuto::CheckTemplate_DuoSwitch,		// CHANGES_MVS_2008
		&CBindingConversionAuto::CheckTemplate_CondPir,			// CHANGES_MVS_2008
		&CBindingConversionAuto::CheckTemplate_Audio,			// CHANGES_MVS_2008
		&CBindingConversionAuto::CheckTemplate_DimmersFwd
	};

	for (int i=0;i<MAX_TEMPLATE_FUNCTIONS;i++) 
	{
		if ((this->*TemplateFunctions[i])(&pBindingAutoTemplates)) break;
	}

	if (pBindingAutoTemplates != NULL) 
	{
		nAutoBindings = pBindingAutoTemplates->ProcessTemplate();
		delete pBindingAutoTemplates;
		pBindingAutoTemplates = NULL;	
	}

	ASSERT(nAutoBindings>=0 && nAutoBindings<=0xFF);		//CHANGES_MVS_2008_WARNINGS
	return static_cast<BYTE>(nAutoBindings);				//CHANGES_MVS_2008_WARNINGS
}

void CBindingConversionAuto::InitNewProcess(void)
{
	AutoBindingEntries.RemoveAll();
	AutoBindingInfo.Clear();
	
	NrUnitInfo.boseType = CBindingConversionAutoUnitInfo::BOSE_TYPE_UNDEFINED;
	NrUnitInfo.nBindingUnits = 0;
	NrUnitInfo.nUniqueBindingUnits = 0;						// Wanneer de node en unit addressen verschillend zijn.
	NrUnitInfo.nBoseUnits = 0;
	NrUnitInfo.nControlUnits = 0;
	NrUnitInfo.nDuoSwitchUnits = 0;
	NrUnitInfo.nSwitchUnits = 0;
	NrUnitInfo.nBoseVirtualUnits = 0;
	NrUnitInfo.nIrtxUnits = 0;
	NrUnitInfo.nSensUnits = 0;
	NrUnitInfo.nAudioUnitsExtended=0;

	// Virtual units
	NrUnitInfo.virtualUnit.nrOfInputs = 0;
	NrUnitInfo.virtualUnit.nrOfUnits = 0;

	// Dimmer units.
	NrUnitInfo.dimUnit.nrOfUnits = 0;
	NrUnitInfo.dimUnit.nrOfInputs = 0;
}


////////////////////////////////////////////////////////////////////////////////

// V17.03: zie BUG-0183.
// Extra info: 'nUniqueBindingUnits' Aantal binding units met een unieke unit addres.
BOOL CBindingConversionAuto::RetrieveNrOfUnits(void)
{	
	REQUIRE(0!=m_pgBindingUnits);

	CBindingUnitsOperations bindingUnitOperations(m_pgBindingUnits);

	NrUnitInfo.nBindingUnits = m_pgBindingUnits->Count();
	NrUnitInfo.nUniqueBindingUnits = bindingUnitOperations.getCountUniqueUnitAddresses();	// BUG-0183

	CBindingUnits::Iterator it(m_pgBindingUnits->CreateIterator());
	for (it.Begin(); !it.IsDone(); it.Next())
	{
		CBindingUnit* const pBindingUnit = it.CurrentItem();
		ASSERT( pBindingUnit );
	
		const BYTE bUnitType = pBindingUnit->GetBindingUnitType();
		switch( bUnitType )
		{
			case BINDING_UNIT_TYPE_DIM:	
				NrUnitInfo.dimUnit.nrOfUnits++;
			break;
			case BINDING_UNIT_TYPE_DIM_FWD:
				NrUnitInfo.dimUnit.nrOfUnits++;
				NrUnitInfo.dimUnit.nrOfInputs++;
			break;

			case BINDING_UNIT_TYPE_CONTROL:		
				NrUnitInfo.nControlUnits++;				
			break;

			case BINDING_UNIT_TYPE_SWITCH:	
			case BINDING_UNIT_TYPE_SWITCH_FWD:
				NrUnitInfo.nSwitchUnits++;				
			break;

			case BINDING_UNIT_TYPE_DUOSWITCH:	
			case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
				NrUnitInfo.nDuoSwitchUnits++;				
			break;

			case BINDING_UNIT_TYPE_BOSELF48:
			case BINDING_UNIT_TYPE_BOSERS:
			case BINDING_UNIT_TYPE_BOSEV35:
			{				
				if (pBindingUnit->GetUnitAddress() != 0xFF)
				{
					NrUnitInfo.nBoseUnits++;	
				}
				else 
				{
					NrUnitInfo.nBoseVirtualUnits++;
				}

				if (BINDING_UNIT_TYPE_BOSELF48==bUnitType)
				{
					NrUnitInfo.boseType = CBindingConversionAutoUnitInfo::BOSE_TYPE_LF48;	
				}
				else if (BINDING_UNIT_TYPE_BOSERS==bUnitType)
				{
					NrUnitInfo.boseType = CBindingConversionAutoUnitInfo::BOSE_TYPE_LF50;	
				}
				else if (BINDING_UNIT_TYPE_BOSEV35==bUnitType)
				{
					NrUnitInfo.boseType = CBindingConversionAutoUnitInfo::BOSE_TYPE_V35;	
				}

			} break;
			
			case BINDING_UNIT_TYPE_IRTX:
				NrUnitInfo.nIrtxUnits++;
			break;

			case BINDING_UNIT_TYPE_LCD_VIRTUAL:
			{
				NrUnitInfo.virtualUnit.nrOfUnits++;

				// Sedert versie 0x0C02
				if (TRUE==CCfgInfoBindingUnit::getSupportedAsInput(pBindingUnit))
				{
					NrUnitInfo.virtualUnit.nrOfInputs++;
				}

			} break; /* BINDING_UNIT_TYPE_LCD_VIRTUAL */

			case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
			case BINDING_UNIT_TYPE_SENS:
				NrUnitInfo.nSensUnits++;
			break;

			case BINDING_UNIT_TYPE_AUDIO_HC4:
			case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:
			{
				NrUnitInfo.nAudioUnitsExtended++;
			} break;
		
			default :
			break;
		}
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
CBindingAutoUnit::CBindingAutoUnit( ) : bEvent(0xff), bFunction(0xff) {}
CBindingAutoConditionState::CBindingAutoConditionState( ) : CBindingAutoUnit( ), m_fStateInverse(FALSE) {}
CBindingAutoDimUnit::CBindingAutoDimUnit( ) : CBindingAutoUnit(), bDimValue(DIM_VALUE_DEFAULT), bPirTime(0xff) { }
CBindingAutoSwitchUnit::CBindingAutoSwitchUnit( ) : CBindingAutoUnit(), bPirTime(0xff) { }
CBindingAutoBoseUnit::CBindingAutoBoseUnit() : CBindingAutoUnit(), bSourceSelect(0xff) { }
CBindingAutoExtendedHC4::CBindingAutoExtendedHC4( ) : CBindingAutoUnit(), bSourceSelect(0xff) , bDest(0xff) {}
CBindingAutoOperator::CBindingAutoOperator() : type(PM_OPERATOR_UNDEFINED), fUsed(FALSE) { }
/////////////////////////////////////////////////////////////////////////////
CBindingAutoTemplate::CBindingAutoTemplate(CBindingConversionAuto *pParent) :
	m_pNrUnitInfo(NULL), m_pAutoBindingOptions(NULL), m_pAutoBindingInfo(NULL),
	m_ControlUnitProp(), m_DuoSwitchUnitProp(),m_VirtualUnitProp(),m_DimmerUnitProp(),
	m_SwitchUnitProp(),m_BoseUnitProp(), m_SensUnitProp(), m_AudioUnitProp(),
	m_InputOperator(), m_ConditionalOperator(),
	m_pgBindingEntries(NULL), m_pgBindingUnits(NULL)

{
	ASSERT(pParent);
	m_pNrUnitInfo = pParent->GetBindingUnitInfo();
	m_pgBindingUnits = pParent->GetBindingUnits();
	m_pgBindingEntries = pParent->GetAutoBindingEntries();
	m_pAutoBindingOptions = pParent->GetAutoBindingOptions();
	m_pAutoBindingInfo = pParent->GetAutoBindingInfo();

	m_CondUnitProp[0] = CBindingAutoConditionState();
	m_CondUnitProp[1] = CBindingAutoConditionState();


	ASSERT(NULL != m_pNrUnitInfo);
	ASSERT(NULL != m_pgBindingUnits);
	ASSERT(NULL != m_pgBindingEntries);
	ASSERT(NULL != m_pAutoBindingOptions);
	ASSERT(NULL != m_pAutoBindingInfo);
}

CBindingAutoTemplate::~CBindingAutoTemplate() {
}

/*************************************************************************

  Use Functie pointer f to process and add new binding to collection
  ... TRICKY ?? ...

  BUG TM 20040817 !!!!!!

**************************************************************************/
int CBindingAutoTemplate::AddProcessBinding(Action f)
{
	BOOL fResult;
	CBindingEntry NewBindingEntry;	

	// Initialise new BindingEntry ...
	NewBindingEntry.GetColUnit()->RemoveAll();
	NewBindingEntry.GetColOperator()->RemoveAll();
	NewBindingEntry.GetColLink()->RemoveAll();
	NewBindingEntry.SetApplicationFlags(0);
	NewBindingEntry.SetBindingReturnFlags(BINDINGFLAGS_STATUS_NOCHANGE);		
	
	NewBindingEntry.SetID(0);	//SOLVED BUG : TM 20040817 must explicitly be set to ID = 0 
	
	m_InputOperator.SetUsed(FALSE);
	m_ConditionalOperator.SetUsed(FALSE);

	// Process Via Functionptr + Add to dbase ...
	fResult = (this->*f)(&NewBindingEntry);
	
	if (fResult) 
	{
		CBindingEntriesOperations BindingEntriesOperations(m_pgBindingEntries);
		const unsigned short nID = BindingEntriesOperations.Add(&NewBindingEntry);
		if (nID != 0)
		{
			return 1;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CBindingAutoTemplate::AddUnit(CBindingUnit *pUnit,CBindingEntry *pBindingEntry)
{
	if (pBindingEntry != NULL)
	{					
		if (pUnit != NULL)
		{					
			CBindingUnits* const pBindingUnits = pBindingEntry->GetColUnit();
			if (pBindingUnits != NULL)
			{
				/**
				 * Clone Derived Classes of CBindingUnit 
				 */
				ASSERT(pUnit->GetBindingUnitType() != 0);
				CBindingUnit* const pNew = CBindingUnitFactory::_Clone(pUnit);
				if (pNew != NULL)
				{
					ASSERT(pNew->GetBindingUnitType() != 0);
					ASSERT(pNew->GetID() != NULL);
					ASSERT(pNew->GetID() == pUnit->GetID());

					const unsigned short nID = pBindingUnits->Add(pNew);
					
					ASSERT(nID != 0);
					ASSERT(nID == pNew->GetID());

					if (nID != 0)
					{
						return TRUE;
					}					
				}
				ASSERT(0);
			}
			ASSERT(pBindingUnits);
		}
		ASSERT(pUnit);
	}
	ASSERT(pBindingEntry);

	return FALSE;
}

BOOL CBindingAutoTemplate::AddLink(CBindingLink *pLink,CBindingEntry *pBindingEntry)
{
	ASSERT(pLink);
	ASSERT(pBindingEntry);
	
	if (pBindingEntry != NULL)
	{
		CBindingLinks* const pBindingLinks = pBindingEntry->GetColLink();
	
		if (pBindingLinks != NULL)
		{		
			CBindingLink* const pNewLink = CBindingLinkFactory::Clone(pLink);
			ASSERT(pNewLink);

			if (pNewLink != NULL)
			{							
				pNewLink->SetID(0);	// !!!!
				const unsigned short nID = pBindingLinks->Add(pNewLink);

				ASSERT(nID != 0);
				ASSERT(nID == pNewLink->GetID());
				
				if (nID != 0)
				{
					return TRUE;
				}

				return FALSE;
			}

			ASSERT(pNewLink);
		}

		ASSERT(pBindingLinks);
	}
	
	ASSERT(pBindingEntry);

	return FALSE;
}





BOOL CBindingAutoTemplate::AddOperator(OperatorType type,CBindingEntry *pBindingEntry)
{
	if (pBindingEntry != NULL)
	{
		CBindingOperators* const pOperators = pBindingEntry->GetColOperator();

		if (pOperators != NULL)
		{	
			CBindingOperator* const pNewOperator = 
				CBindingOperatorFactory::Create(type);

			if (pNewOperator != NULL)
			{
				pNewOperator->SetID(0);	// !!!!
				const unsigned short nID = pOperators->Add(pNewOperator);
				ASSERT(nID != 0);
				ASSERT(nID == pNewOperator->GetID());

				if (nID != 0)
				{
					return TRUE;
				}

				return FALSE;
			}

			ASSERT(pNewOperator);
		}

		ASSERT(pOperators);
	}

	ASSERT(pBindingEntry);

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
int CBindingAutoTemplate::CopyUnits(CBindingEntry *pEntry,BYTE bUnitType)
{
	CBindingUnit *pBindingUnit = NULL;
	int nCnt = 0;

	for (int i=0;i<m_pNrUnitInfo->nBindingUnits;i++) 
	{
		if ((pBindingUnit=m_pgBindingUnits->Get(i))!=NULL)
		{	
			if (pBindingUnit->GetBindingUnitType()==bUnitType) 
			{
				AddUnit(pBindingUnit,pEntry);
				nCnt++;
			}
		}
		ASSERT(pBindingUnit);
	}

	return nCnt;
}

int CBindingAutoTemplate::CopyUnit(CBindingEntry *pEntry,BYTE bUnitType,int index)
{
	CBindingUnit *pBindingUnit = NULL;
	int nCnt = 0;

	for (int i=0;i<m_pNrUnitInfo->nBindingUnits;i++) 
	{
		if ((pBindingUnit=m_pgBindingUnits->Get(i))!=NULL)
		{	
			if (pBindingUnit->GetBindingUnitType() == bUnitType) 
			{
				if (nCnt++ == index) 
				{
					AddUnit(pBindingUnit,pEntry);
					return nCnt;
				}
			}
		}

		ASSERT(pBindingUnit);
	}
	return nCnt;
}

BOOL CBindingAutoTemplate::AddLinks(CBindingEntry *pEntry,int nInputs,int nOutputs)
{
	unsigned short nOpID = 0;				// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	unsigned short nInID = 0;				// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	unsigned short nOutID = 0;				// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	int i = 0;								// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	CBindingOperator *pBindingOperator = 0;	// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	CBindingUnit *pBindingUnit = 0;			// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	CBindingLink BindingLink;

	CBindingOperators* const pBindingOperators = pEntry->GetColOperator();
	CBindingUnits* const pBindingUnits = pEntry->GetColUnit();
	ASSERT(pBindingUnits);
	ASSERT(pBindingOperators);

	CBindingUnitsOperations BindingUnitsOperations(pBindingUnits);

	// Get Operator id ...
	if (nInputs > 1) 
	{			
		if ((pBindingOperator = pBindingOperators->Get(0)) == 0)
		{		
			ASSERT(pBindingOperator);
			return FALSE;
		}

		nOpID = pBindingOperator->GetID();
	}

	// Build links ...
	if (nInputs > 1) 
	{	
		for (i=0;i<nInputs;i++) 
		{
			// link all inputs to operator ...
			if (BindingUnitsOperations.GetInputs(i,&pBindingUnit)) 
			{
				nInID = pBindingUnit->GetID();

				BindingLink.SetInLink(nInID,PM_UNIT);
				BindingLink.SetOutLink(nOpID,PM_OPERATOR);					// MVS_2008_WARNINGS_UNITIALISED_!!!!!		
				BindingLink.SetInverse(m_ControlUnitProp.IsStateInvers());
				AddLink(&BindingLink,pEntry);
			}			
		}

		// Link operator to first output ...
		if (BindingUnitsOperations.GetOutputs(0,&pBindingUnit)) 
		{			
			nOutID = pBindingUnit->GetID();

			BindingLink.SetInLink(nOpID,PM_OPERATOR);	
			BindingLink.SetOutLink(nOutID,PM_UNIT);
			BindingLink.SetInverse(FALSE);
			AddLink(&BindingLink,pEntry);	
		}
		else
		{
			ASSERT( 0 );
		}
	}// end if nInputs > 1
	else 
	{
		// Link input directly to first output ...
		if (BindingUnitsOperations.GetInputs(0,&pBindingUnit)) 
		{
			nInID = pBindingUnit->GetID();
			BindingLink.SetInLink(nInID,PM_UNIT);
		}	
		else
		{
			ASSERT( 0 );
		}
		
		if (BindingUnitsOperations.GetOutputs(0,&pBindingUnit)) 
		{
			nOutID = pBindingUnit->GetID();
			BindingLink.SetOutLink(nOutID,PM_UNIT);
		}
		else
		{
			ASSERT( 0 );
		}
		
		BindingLink.SetInverse(m_ControlUnitProp.IsStateInvers());
		AddLink(&BindingLink,pEntry);				
	}

	// Check if there are more then 1 output ...
	if (nOutputs > 1) 
	{
		nInID = 0;
		nOutID = 0;

		if (BindingUnitsOperations.GetOutputs(0,&pBindingUnit)) 
		{
			nInID = pBindingUnit->GetID();
		}
		
		for(i=1;i<nOutputs;i++) 
		{
			nOutID = 0;
		
			if (BindingUnitsOperations.GetOutputs(i,&pBindingUnit)) 
			{
				nOutID = pBindingUnit->GetID();
			}
						
			if (nInID != 0 && nOutID != 0) 
			{
				BindingLink.SetInLink(nInID,PM_UNIT);
			    BindingLink.SetOutLink(nOutID,PM_UNIT);
				BindingLink.SetInverse(FALSE);

				AddLink(&BindingLink,pEntry);

				nInID = nOutID;
			}
		}
	}
	return TRUE;
}



////////////////////////////////////////////////////////////////////////////////////////////////

void CBindingAutoTemplate::AddUnitProperties(
	CBindingEntry* pEntry,
	const BYTE* pbUnitTypes,
	int nrOfUnitTypes)
{
	if (0!=pbUnitTypes)
	{
		int i=0;

		for(i=0;i<nrOfUnitTypes;++i)
		{
			this->AddUnitProperties(pEntry,pbUnitTypes[i]);
		}	
	}
}

void CBindingAutoTemplate::AddUnitProperties(CBindingEntry *pEntry,BYTE bUnitType,BOOL fIoType)
{
	CBindingUnit *pBindingUnit = NULL;
	int i;

	CBindingUnits* const pBindingUnits = pEntry->GetColUnit();
	ASSERT(pBindingUnits);

	CBindingUnitsOperations BindingUnitsOperations(pBindingUnits);

	switch(bUnitType) 
	{
		case BINDING_UNIT_TYPE_CONTROL:
		{
			for (i=0;i<m_pNrUnitInfo->nControlUnits;i++) 
			{				
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit)) 
				{
					ASSERT(0!=pBindingUnit);

					CBindingControlUnit* const p=
						static_cast<CBindingControlUnit*>(pBindingUnit);

					if (0!=p)
					{
						p->SetInOutType(PM_INOUTTYPE_INPUT);
						p->Set_Event(m_ControlUnitProp.GetEvent());	
					}					
				}	
			}
		} break;

		case BINDING_UNIT_TYPE_DIM:
		case BINDING_UNIT_TYPE_DIM_FWD:
		{
			for (i=0;i<m_pNrUnitInfo->dimUnit.nrOfUnits;i++) 
			{			
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit))
				{
					BOOL fUseAsOutput=TRUE;
					ASSERT(0!=pBindingUnit);

					CBindingDimmerUnit* const p=
						static_cast<CBindingDimmerUnit*>(pBindingUnit);

					if (0!=p)
					{
						fUseAsOutput=TRUE;

						if (BINDING_UNIT_TYPE_DIM_FWD==bUnitType)
						{
							if((m_DimmerUnitProp.GetFunction()==0xFF)&&			
								(m_DimmerUnitProp.GetEvent()!=0xFF))
							{
								fUseAsOutput = FALSE;	
							}
						}
						
						if (fUseAsOutput)
						{
							p->SetInOutType(PM_INOUTTYPE_OUTPUT);		
							p->Set_Function(m_DimmerUnitProp.GetFunction());
							p->Set_PirTime(m_DimmerUnitProp.GetPirTime());
							p->Set_Value(m_DimmerUnitProp.GetDimValue());		
						}
						else
						{
							ASSERT(BINDING_UNIT_TYPE_DIM_FWD==bUnitType);

							if (BINDING_UNIT_TYPE_DIM_FWD==bUnitType)
							{
								CBindingDimmerUnitInput* const p2=
									static_cast<CBindingDimmerUnitInput*>(p);

								if(0!=p2)
								{								
									p2->SetInOutType(PM_INOUTTYPE_INPUT);								
									p2->Set_Event(m_DimmerUnitProp.GetEvent());
								}
							}
						}					
					}
				}
			}
		} break;

		case BINDING_UNIT_TYPE_SWITCH:
		case BINDING_UNIT_TYPE_SWITCH_FWD:
		{
			for (i=0;i<m_pNrUnitInfo->nSwitchUnits;i++) 
			{			
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit))
				{
					ASSERT(0!=pBindingUnit);

					CBindingRelaisUnit* const p=
						static_cast<CBindingRelaisUnit*>(pBindingUnit);

					if (0!=p)
					{
						p->SetInOutType(PM_INOUTTYPE_OUTPUT);
						p->Set_Function(m_SwitchUnitProp.GetFunction());
						p->Set_PirTime(m_SwitchUnitProp.GetPirTime());
					}
				}	
			}
		} break;

		case BINDING_UNIT_TYPE_DUOSWITCH:
		case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
		{
			for (i=0;i<m_pNrUnitInfo->nDuoSwitchUnits;i++) 
			{				
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit))
				{
					ASSERT(0!=pBindingUnit);

					CBindingDuoSwitchUnit* const p=
						static_cast<CBindingDuoSwitchUnit*>(pBindingUnit);

					if (0!=p)
					{
						p->SetInOutType(PM_INOUTTYPE_OUTPUT);	
						p->Set_Function(m_DuoSwitchUnitProp.GetFunction());				
					}
				}
			}
		} break;

		case BINDING_UNIT_TYPE_BOSERS:
		{
			for (i=0;i<m_pNrUnitInfo->nBoseUnits;i++) 
			{			
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit)) 
				{				
					ASSERT(0!=pBindingUnit);

					CBindingBoseRsUnit* const p=
						static_cast<CBindingBoseRsUnit*>(pBindingUnit);

					if (0!=p)
					{
						p->SetInOutType(fIoType);
						p->Set_Function(m_BoseUnitProp.GetFunction());
						p->Set_Source(m_BoseUnitProp.GetSource());				
					}
				}
			}
		} break;

		case BINDING_UNIT_TYPE_BOSELF48:
		{
			for (i=0;i<m_pNrUnitInfo->nBoseUnits;i++) 
			{			
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit)) 
				{		
					ASSERT(0!=pBindingUnit);

					CBindingBoseLf48Unit* const p=
						static_cast<CBindingBoseLf48Unit*>(pBindingUnit);

					if (0!=p)
					{
						p->SetInOutType(fIoType);
						p->Set_Function(m_BoseUnitProp.GetFunction());
						p->Set_Source(m_BoseUnitProp.GetSource());				
					}
				}
			}
		} break;	

		case BINDING_UNIT_TYPE_BOSEV35:
		{
			for (i=0;i<m_pNrUnitInfo->nBoseUnits;i++) 
			{			
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit)) 
				{				
					ASSERT(0!=pBindingUnit);

					CBindingBoseV35Unit* const p=
						static_cast<CBindingBoseV35Unit*>(pBindingUnit);

					if (0!=p)
					{
						p->SetInOutType(fIoType);
						p->Set_Function(m_BoseUnitProp.GetFunction());
						p->Set_Source(m_BoseUnitProp.GetSource());				
					}
				}
			}
		} break;	

		case BINDING_UNIT_TYPE_LCD_VIRTUAL :
		{
			for (i=0;i<m_pNrUnitInfo->virtualUnit.nrOfUnits;i++) 
			{			
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit)) 
				{		
					ASSERT(0!=pBindingUnit);

					CBindingVirtualUnit* const p=
						static_cast<CBindingVirtualUnit*>(pBindingUnit);

					if (0!=p)
					{
						p->SetInOutType(fIoType);
						p->Set_Function(m_VirtualUnitProp.GetFunction());				
						p->Set_Event(m_VirtualUnitProp.GetEvent());
					}
				}
			}
		} break;

		case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
		case BINDING_UNIT_TYPE_SENS :
		{
			for (i=0;i<m_pNrUnitInfo->nSensUnits;i++) 
			{			
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit)) 
				{
					ASSERT(0!=pBindingUnit);

					CBindingSensUnit* const p=
						static_cast<CBindingSensUnit*>(pBindingUnit);

					if (0!=p)
					{
						p->SetInOutType(PM_INOUTTYPE_INPUT);
						p->Set_Event(m_SensUnitProp.GetEvent());			
					}
				}	
			}
		}break;

		case BINDING_UNIT_TYPE_AUDIO_HC4:
		case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:
		{
			for (i=0;i<m_pNrUnitInfo->nAudioUnitsExtended;i++) 
			{			
				if (BindingUnitsOperations.GetUnitType(bUnitType,i,&pBindingUnit)) 
				{
					ASSERT(0!=pBindingUnit);
					ASSERT((pBindingUnit->GetBindingUnitType()==BINDING_UNIT_TYPE_AUDIO_HC4)||
						   (pBindingUnit->GetBindingUnitType()==BINDING_UNIT_TYPE_AUDIO_HC4_BNO));

					CBindingHC4AudioUnit* const p=
						static_cast<CBindingHC4AudioUnit*>(pBindingUnit);

					if (0!=p)
					{
						p->SetInOutType(PM_INOUTTYPE_OUTPUT);
						p->Set_Function(m_AudioUnitProp.GetFunction());
						p->Set_Destination(m_AudioUnitProp.GetDest());
						p->Set_Source(m_AudioUnitProp.GetSource());
					}
				}
			}

		} break;

		default:
		{
			ASSERT(0);
		} break;
	}
}
/////////////////////////////////////////////////////////////////////////////

CBindingAutoTemplate_Dimmer::CBindingAutoTemplate_Dimmer(CBindingConversionAuto *pParent) 
: CBindingAutoTemplate(pParent) 
{
	DimmerAutoBindingActions[AUTO_BINDING_DIM_ONOFF] = (Action) &CBindingAutoTemplate_Dimmer::ProcessBindingShort;	// CHANGES_MVS_2008
	DimmerAutoBindingActions[AUTO_BINDING_DIM_UPDN] = (Action) &CBindingAutoTemplate_Dimmer::ProcessBindingLong;	// CHANGES_MVS_2008
	DimmerAutoBindingActions[AUTO_BINDING_DIM_PIR] = (Action) &CBindingAutoTemplate_Dimmer::ProcessBindingPIR;		// CHANGES_MVS_2008

	DimmerAutoStrings[AUTO_BINDING_DIM_ONOFF].Format(_T("Short Event : On/Off f(State)"));
	DimmerAutoStrings[AUTO_BINDING_DIM_UPDN].Format(_T("Long Event : Dim Up/Dn f(State)"));
	DimmerAutoStrings[AUTO_BINDING_DIM_PIR].Format(_T("PIR : Set Pir On 3min 99%%"));		
}

CBindingAutoTemplate_Dimmer::~CBindingAutoTemplate_Dimmer() 
{
	for (int i=0;i<AUTO_BINDING_DIM_MAX;i++) 
	{
		DimmerAutoStrings[i].Empty();
	}
}

int CBindingAutoTemplate_Dimmer::ProcessTemplate(void)
{
	int nAutoBindings = 0;

	switch (m_pAutoBindingOptions->GetAutoDimmerOption())
	{
		case DIMMER_AUTO_OPTION_ONOFF:			
		{
			nAutoBindings += ProcessBinding(AUTO_BINDING_DIM_ONOFF);
			nAutoBindings += ProcessBinding(AUTO_BINDING_DIM_UPDN);

			if (nAutoBindings == 2) 
			{
				m_pAutoBindingInfo->AddMessage(DimmerAutoStrings[AUTO_BINDING_DIM_ONOFF]);
				m_pAutoBindingInfo->AddMessage(DimmerAutoStrings[AUTO_BINDING_DIM_UPDN]);
			}
		} break; /* DIMMER_AUTO_OPTION_ONOFF */

		case DIMMER_AUTO_OPTION_PIRON:		
		{	
			nAutoBindings += ProcessBinding(AUTO_BINDING_DIM_PIR);

			if (nAutoBindings == 1) 
			{
				m_pAutoBindingInfo->AddMessage(DimmerAutoStrings[AUTO_BINDING_DIM_PIR]);
			}
		} break; /* DIMMER_AUTO_OPTION_PIRON */

		default: 
		{
			ASSERT(0); 
			nAutoBindings=0;			
		} break;
	}
	return nAutoBindings;
}

//--------
BOOL CBindingAutoTemplate_Dimmer::ProcessBindingShort(CBindingEntry *pEntry)
{
	// Set properties.
	const BOOL fStateInverse=FALSE;
	const int nrOfInputs=m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs;
	const int nrOfOutputs=m_pNrUnitInfo->dimUnit.nrOfUnits+m_pNrUnitInfo->nSwitchUnits;
	BYTE bEvent=EV_UNITCONTROLPULS;

	ASSERT(m_pNrUnitInfo->virtualUnit.nrOfInputs==m_pNrUnitInfo->virtualUnit.nrOfUnits);

	// Input properties.
	if (nrOfOutputs>1) 
	{
		bEvent=EV_UNITCONTROLPULSTOGGLE;
	}

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);

	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	// Output properties.		
	m_DimmerUnitProp.SetFunction(DIMMER_OBJECT_METHOD_FSCVONOFF);
	m_DimmerUnitProp.SetEvent(0xFF);
	m_DimmerUnitProp.SetDimValue(DIM_VALUE_DEFAULT);
	m_DimmerUnitProp.SetPirTime(0xFF);
	
	m_SwitchUnitProp.SetFunction(SWITCH_OBJECT_METHOD_FSONOFF);
	m_SwitchUnitProp.SetPirTime(0xFF);

	// Copy units.

	// Sedert versie 0x0C02: Ook Virtual units.
	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM_FWD))== 0)
	{
		return FALSE;
	}

	CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH);
	CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD);


	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_CONTROL,
		BINDING_UNIT_TYPE_LCD_VIRTUAL,
		BINDING_UNIT_TYPE_DIM,
		BINDING_UNIT_TYPE_DIM_FWD,
		BINDING_UNIT_TYPE_SWITCH,
		BINDING_UNIT_TYPE_SWITCH_FWD
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));

	// Set Operators+links.
	if (nrOfInputs>1)
	{		
		m_InputOperator.SetUsed(TRUE);
		m_InputOperator.SetType(PM_OPERATOR_XOR);		
		AddOperator(m_InputOperator.GetType(),pEntry);	
	}

	AddLinks(pEntry,nrOfInputs,nrOfOutputs);
	return (TRUE);
}

// Oppassen: Enkel de dimmer units worden verwerkt.
BOOL CBindingAutoTemplate_Dimmer::ProcessBindingLong(CBindingEntry *pEntry)
{
	const int nrOfInputs=m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs;
	const int nrOfOutputs=m_pNrUnitInfo->dimUnit.nrOfUnits;	// Oppassen: Enkel de dimmer units tellen als output !!

	ASSERT(m_pNrUnitInfo->virtualUnit.nrOfInputs==m_pNrUnitInfo->virtualUnit.nrOfUnits);

	m_DimmerUnitProp.SetFunction(DIMMER_OBJECT_METHOD_FSUD);
	m_DimmerUnitProp.SetEvent(0xFF);
	m_DimmerUnitProp.SetDimValue(DIM_VALUE_DEFAULT);	
	m_DimmerUnitProp.SetPirTime(0xFF);
	
	m_ControlUnitProp.SetEvent(EV_UNITCONTROLTOGGLE);
	m_ControlUnitProp.SetStateInverse(FALSE);

	m_VirtualUnitProp.SetEvent(EV_UNITCONTROLTOGGLE);
	m_VirtualUnitProp.SetStateInverse(FALSE);
	
	if ((nrOfInputs==1)&&(nrOfOutputs==1))
	{
		pEntry->SetBindingReturnFlags(BINDINGFLAGS_STATUS_POST);
	}
	else 
	{
		pEntry->SetBindingReturnFlags(BINDINGFLAGS_STATUS_NOCHANGE);
	}

   // Copy units.
	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM_FWD))==0)
	{
		return FALSE;
	}

	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_CONTROL,
		BINDING_UNIT_TYPE_LCD_VIRTUAL,
		BINDING_UNIT_TYPE_DIM,
		BINDING_UNIT_TYPE_DIM_FWD,
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));

	if (nrOfInputs>1) 
	{
		m_InputOperator.SetUsed(TRUE);
		m_InputOperator.SetType(PM_OPERATOR_OR);	
		AddOperator(m_InputOperator.GetType(),pEntry);	
	}
	AddLinks(pEntry,nrOfInputs,nrOfOutputs);
	return (TRUE);
}

BOOL CBindingAutoTemplate_Dimmer::ProcessBindingPIR(CBindingEntry *pEntry)
{
	m_DimmerUnitProp.SetFunction(DIMMER_OBJECT_METHOD_PIRON);
	m_DimmerUnitProp.SetEvent(0xFF);
	m_DimmerUnitProp.SetDimValue(99);
	m_DimmerUnitProp.SetPirTime(0xFF);
	
	m_ControlUnitProp.SetEvent(EV_UNITCONTROLTOGGLE);
	m_ControlUnitProp.SetStateInverse(TRUE);			// !!!

	m_SwitchUnitProp.SetFunction(SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR);
	m_SwitchUnitProp.SetPirTime(0xFF);

	if (CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL) == 0) return FALSE;

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM_FWD))==0)
	{
		return FALSE;
	}

	CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH);
	CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD);

	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_CONTROL,
		BINDING_UNIT_TYPE_DIM,
		BINDING_UNIT_TYPE_DIM_FWD,
		BINDING_UNIT_TYPE_SWITCH,
		BINDING_UNIT_TYPE_SWITCH_FWD
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));

	if (m_pNrUnitInfo->nControlUnits>1) 
	{
		m_InputOperator.SetUsed(TRUE);
		m_InputOperator.SetType(PM_OPERATOR_OR);	
		AddOperator(m_InputOperator.GetType(),pEntry);	
	}

	AddLinks(pEntry,m_pNrUnitInfo->nControlUnits,
			 m_pNrUnitInfo->dimUnit.nrOfUnits+m_pNrUnitInfo->nSwitchUnits);

	return (TRUE);
}

//------------------------------------------------------------------------------

CBindingAutoTemplate_Switch::CBindingAutoTemplate_Switch(CBindingConversionAuto *pParent)
: CBindingAutoTemplate(pParent) 
{

	SwitchAutoBindingActions[AUTO_BINDING_SWITCH_ONOFF] = (Action) &CBindingAutoTemplate_Switch::ProcessBindingShort;	// CHANGES_MVS_2008
	SwitchAutoBindingActions[AUTO_BINDING_SWITCH_PIR] = (Action)  &CBindingAutoTemplate_Switch::ProcessBindingPIR;	// CHANGES_MVS_2008

	SwitchAutoStrings[AUTO_BINDING_SWITCH_ONOFF].Format(_T("Short Event : On/Off f(State)"));
	SwitchAutoStrings[AUTO_BINDING_SWITCH_PIR].Format(_T("PIR : Set Pir On 3min 99%%"));
}

CBindingAutoTemplate_Switch::~CBindingAutoTemplate_Switch() 
{
	for (int i=0;i<AUTO_BINDING_SWITCH_MAX;i++) 
	{
		SwitchAutoStrings[i].Empty();
	}
}


int CBindingAutoTemplate_Switch::ProcessTemplate(void)
{
	int nAutoBindings=0;

	switch (m_pAutoBindingOptions->GetAutoRelaisOption())
	{
		case RELAIS_AUTO_OPTION_ONOFF:
		{
			nAutoBindings += ProcessBinding(AUTO_BINDING_SWITCH_ONOFF);
			if (nAutoBindings == 1) 
			{
				m_pAutoBindingInfo->AddMessage(SwitchAutoStrings[AUTO_BINDING_SWITCH_ONOFF]);
			}
		} break;

		case RELAIS_AUTO_OPTION_PIRON:
		{
			nAutoBindings += ProcessBinding(AUTO_BINDING_SWITCH_PIR);
			if (nAutoBindings == 1) 
			{
				m_pAutoBindingInfo->AddMessage(SwitchAutoStrings[AUTO_BINDING_SWITCH_PIR]);
			}
		}break;

		default:
		{
			ASSERT(0);
			nAutoBindings = 0;
		} break;
	}
	return nAutoBindings;
}

BOOL CBindingAutoTemplate_Switch::ProcessBindingShort(CBindingEntry *pEntry)
{
	const int nrOfOutputs = m_pNrUnitInfo->nSwitchUnits;
	const int nrOfInputs = m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs;
	const BOOL fStateInverse=FALSE;
	BYTE bEvent=EV_UNITCONTROLPULS;

	ASSERT(m_pNrUnitInfo->virtualUnit.nrOfInputs==m_pNrUnitInfo->virtualUnit.nrOfUnits);

	m_SwitchUnitProp.SetFunction(SWITCH_OBJECT_METHOD_FSONOFF);
	m_SwitchUnitProp.SetPirTime(0xFF);

	if (nrOfOutputs>1) 
	{
		bEvent=EV_UNITCONTROLPULSTOGGLE;
	}

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);

	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	// CopyUnits.
	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL))==0)
	{
		return FALSE;
	}
	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD))==0)
	{
		return FALSE;
	}

	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_CONTROL,
		BINDING_UNIT_TYPE_LCD_VIRTUAL,
		BINDING_UNIT_TYPE_SWITCH,
		BINDING_UNIT_TYPE_SWITCH_FWD
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));

	if (nrOfInputs>1) 
	{
		m_InputOperator.SetUsed(TRUE);
		m_InputOperator.SetType(PM_OPERATOR_XOR);	
		AddOperator(m_InputOperator.GetType(),pEntry);	
	}

	AddLinks(pEntry,nrOfInputs,nrOfOutputs);

	return (TRUE);
}

BOOL CBindingAutoTemplate_Switch::ProcessBindingPIR(CBindingEntry *pEntry)
{
	m_SwitchUnitProp.SetFunction(SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR);
	m_SwitchUnitProp.SetPirTime(0xFF);

	m_ControlUnitProp.SetEvent(EV_UNITCONTROLTOGGLE);
	m_ControlUnitProp.SetStateInverse(TRUE);				// !!!

	if (CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL)==0) 
		return FALSE;

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SWITCH);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD);

	if (m_pNrUnitInfo->nControlUnits>1) 
	{
		m_InputOperator.SetUsed(TRUE);
		m_InputOperator.SetType(PM_OPERATOR_OR);	
		AddOperator(m_InputOperator.GetType(),pEntry);	
	}

	AddLinks(pEntry,m_pNrUnitInfo->nControlUnits,m_pNrUnitInfo->nSwitchUnits);

	return (TRUE);
}


//------------------------------------------------------------------------------
CBindingAutoTemplate_DuoSwitch::CBindingAutoTemplate_DuoSwitch(CBindingConversionAuto *pParent)
: CBindingAutoTemplate(pParent) {

	DuoswitchAutoBindingActions[AUTO_BINDING_DUOSWITCH_UPDOWN] = (Action) &CBindingAutoTemplate_DuoSwitch::ProcessBindingShort;		// CHANGES_MVS_2008
	DuoswitchAutoBindingActions[AUTO_BINDING_DUOSWITCH_STOP] = (Action) &CBindingAutoTemplate_DuoSwitch::ProcessBindingLong;		// CHANGES_MVS_2008
	DuoswitchAutoBindingActions[AUTO_BINDING_DUOSWITCH_UP] = (Action) &CBindingAutoTemplate_DuoSwitch::ProcessBindingShort_Input1;	// CHANGES_MVS_2008
	DuoswitchAutoBindingActions[AUTO_BINDING_DUOSWITCH_DOWN] = (Action) &CBindingAutoTemplate_DuoSwitch::ProcessBindingShort_Input2;	// CHANGES_MVS_2008

	DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_UPDOWN].Format(_T("Short Event : Up / Down f(State)"));
	DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_STOP].Format(_T("Long Event : Stop"));
	DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_UP].Format(_T("Short Event (Input1) : Switch Up"));
	DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_DOWN].Format(_T("Short Event (Input1) : Switch Down"));

}

CBindingAutoTemplate_DuoSwitch::~CBindingAutoTemplate_DuoSwitch() 
{
	for (int i=0;i<AUTO_BINDING_DUOSWITCH_MAX;i++) 
	{
		DuoSwitchAutoStrings[i].Empty();
	}
}

int CBindingAutoTemplate_DuoSwitch::ProcessTemplate(void)
{
	const int nrOfInputs=(m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs);
	int nAutoBindings = 0;	

	ASSERT(m_pNrUnitInfo->virtualUnit.nrOfInputs==m_pNrUnitInfo->virtualUnit.nrOfUnits);

	if (nrOfInputs==1) 
	{
		nAutoBindings += ProcessBinding(AUTO_BINDING_DUOSWITCH_UPDOWN);
		nAutoBindings += ProcessBinding(AUTO_BINDING_DUOSWITCH_STOP);

		if (nAutoBindings==2) 
		{
			m_pAutoBindingInfo->AddMessage(DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_UPDOWN]);
			m_pAutoBindingInfo->AddMessage(DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_STOP]);
		}
	}
	else if (nrOfInputs==2) 
	{
		nAutoBindings += ProcessBinding(AUTO_BINDING_DUOSWITCH_UP);
		nAutoBindings += ProcessBinding(AUTO_BINDING_DUOSWITCH_DOWN);
		nAutoBindings += ProcessBinding(AUTO_BINDING_DUOSWITCH_STOP);

		if (nAutoBindings==3) 
		{
			m_pAutoBindingInfo->AddMessage(DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_UP]);
			m_pAutoBindingInfo->AddMessage(DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_DOWN]);
			m_pAutoBindingInfo->AddMessage(DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_STOP]);
		}
	}
	return nAutoBindings;
}


BOOL CBindingAutoTemplate_DuoSwitch::ProcessBindingShort(CBindingEntry *pEntry)
{
	const int nrOfInputs=(m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs);
	const int nrOfOutputs=m_pNrUnitInfo->nDuoSwitchUnits;

	// Er mag maar 1 input zijn !!
	ASSERT(nrOfInputs==1);	

	m_DuoSwitchUnitProp.SetFunction(DUOSWITCH_OBJECT_METHOD_FSUPDOWN);

	m_ControlUnitProp.SetEvent(EV_UNITCONTROLPULS);
	m_ControlUnitProp.SetStateInverse(FALSE);
	m_VirtualUnitProp.SetEvent(EV_UNITCONTROLPULS);
	m_VirtualUnitProp.SetStateInverse(FALSE);

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_DUOSWITCH)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_DUOSWITCH_FWD))==0)
	{
		return FALSE;
	}

	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_CONTROL,
		BINDING_UNIT_TYPE_LCD_VIRTUAL,
		BINDING_UNIT_TYPE_DUOSWITCH,
		BINDING_UNIT_TYPE_DUOSWITCH_FWD
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));	

	AddLinks(pEntry,nrOfInputs,nrOfOutputs);

	return (TRUE);
}

BOOL CBindingAutoTemplate_DuoSwitch::ProcessBindingLong(CBindingEntry *pEntry)
{
	const int nrOfInputs=(m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs);
	const int nrOfOutputs=m_pNrUnitInfo->nDuoSwitchUnits;

	m_DuoSwitchUnitProp.SetFunction(DUOSWITCH_OBJECT_METHOD_STOP);
	
	m_ControlUnitProp.SetEvent(EV_UNITCONTROLTOGGLE);
	m_ControlUnitProp.SetStateInverse(FALSE);
	m_VirtualUnitProp.SetEvent(EV_UNITCONTROLTOGGLE);
	m_VirtualUnitProp.SetStateInverse(FALSE);

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_DUOSWITCH)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_DUOSWITCH_FWD))==0)
	{
		return FALSE;
	}

	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_CONTROL,
		BINDING_UNIT_TYPE_LCD_VIRTUAL,
		BINDING_UNIT_TYPE_DUOSWITCH,
		BINDING_UNIT_TYPE_DUOSWITCH_FWD
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));	

	if (nrOfInputs>1) 
	{
		m_InputOperator.SetUsed(TRUE);
		m_InputOperator.SetType(PM_OPERATOR_OR);	
		AddOperator(m_InputOperator.GetType(),pEntry);	
	}

	AddLinks(pEntry,nrOfInputs,nrOfOutputs);

	return (TRUE);
}

//--
BOOL CBindingAutoTemplate_DuoSwitch::ProcessBindingShort_Input1(CBindingEntry *pEntry)
{	
	const int nrOfOutputs=m_pNrUnitInfo->nDuoSwitchUnits;

	// zie BUG-0138

	// Er moeten altijd 2 inputs zijn en ze moeten van hetzelfde type zijn.
	ASSERT((m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs)==2);
	ASSERT((m_pNrUnitInfo->nControlUnits==2)||(m_pNrUnitInfo->virtualUnit.nrOfInputs==2));

	m_DuoSwitchUnitProp.SetFunction(DUOSWITCH_OBJECT_METHOD_UP);
	
	m_ControlUnitProp.SetEvent(EV_UNITCONTROLPULS);
	m_ControlUnitProp.SetStateInverse(FALSE);
	m_VirtualUnitProp.SetEvent(EV_UNITCONTROLPULS);
	m_VirtualUnitProp.SetStateInverse(FALSE);


	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_DUOSWITCH)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_DUOSWITCH_FWD))==0)
	{
		return FALSE;
	}

	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_CONTROL,
		BINDING_UNIT_TYPE_LCD_VIRTUAL,
		BINDING_UNIT_TYPE_DUOSWITCH,
		BINDING_UNIT_TYPE_DUOSWITCH_FWD
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));	

	AddLinks(pEntry,1,nrOfOutputs);

	return (TRUE);
}

BOOL CBindingAutoTemplate_DuoSwitch::ProcessBindingShort_Input2(CBindingEntry *pEntry)
{
	const int nrOfOutputs=m_pNrUnitInfo->nDuoSwitchUnits;

	// zie BUG-0138

	// Er moeten altijd 2 inputs zijn en ze moeten van hetzelfde type zijn.
	ASSERT((m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs)==2);
	ASSERT((m_pNrUnitInfo->nControlUnits==2)||(m_pNrUnitInfo->virtualUnit.nrOfInputs==2));

	m_DuoSwitchUnitProp.SetFunction(DUOSWITCH_OBJECT_METHOD_DOWN);

	m_ControlUnitProp.SetEvent(EV_UNITCONTROLPULS);
	m_ControlUnitProp.SetStateInverse(FALSE);
	m_VirtualUnitProp.SetEvent(EV_UNITCONTROLPULS);
	m_VirtualUnitProp.SetStateInverse(FALSE);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,1)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,1))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_DUOSWITCH)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_DUOSWITCH_FWD))==0)
	{
		return FALSE;
	}

	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_CONTROL,
		BINDING_UNIT_TYPE_LCD_VIRTUAL,
		BINDING_UNIT_TYPE_DUOSWITCH,
		BINDING_UNIT_TYPE_DUOSWITCH_FWD
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));	

	AddLinks(pEntry,1,nrOfOutputs);

	return (TRUE);
}
/******************************************************************************
*
*	class CBindingAutoTemplate_Moods
*	Tasks : Make Auto bindings for Controls --> Virtuals or Virtuals --> Virtuals ...
*
*******************************************************************************/
CBindingAutoTemplate_Moods::CBindingAutoTemplate_Moods(CBindingConversionAuto *pParent)
: CBindingAutoTemplate(pParent) {

	MoodsAutoBindingActions[AUTO_BINDINGS_MOODS_CONTROL_SHORT] = (Action) &CBindingAutoTemplate_Moods::ProcessBindingShort;			// CHANGES_MVS_2008
	MoodsAutoBindingActions[AUTO_BINDINGS_MOODS_CONTROL_LONG] = (Action) &CBindingAutoTemplate_Moods::ProcessBindingLong;			// CHANGES_MVS_2008
	MoodsAutoBindingActions[AUTO_BINDINGS_MOODS_VIRTUAL_SHORT] = (Action) &CBindingAutoTemplate_Moods::ProcessBindingShort_virt;	// CHANGES_MVS_2008
	MoodsAutoBindingActions[AUTO_BINDINGS_MOODS_VIRTUAL_LONG] = (Action) &CBindingAutoTemplate_Moods::ProcessBindingLong_virt;		// CHANGES_MVS_2008

	MoodsAutoStrings[AUTO_BINDINGS_MOODS_CONTROL_SHORT].Format(_T("Short Event : Mood Pulse"));
	MoodsAutoStrings[AUTO_BINDINGS_MOODS_CONTROL_LONG].Format(_T("Long Event : Mood Pulse"));
	MoodsAutoStrings[AUTO_BINDINGS_MOODS_VIRTUAL_SHORT].Format(_T("Short Event : Mood Pulse"));
	MoodsAutoStrings[AUTO_BINDINGS_MOODS_VIRTUAL_LONG].Format(_T("Long Event : Mood Pulse"));
}

CBindingAutoTemplate_Moods::~CBindingAutoTemplate_Moods() {
	for (int i=0;i<AUTO_BINDINGS_MOODS_MAX;i++) {
		MoodsAutoStrings[i].Empty();
	}
}

int CBindingAutoTemplate_Moods::ProcessTemplate(void)
{
	int nAutoBindings = 0;
	
	// 3 Virtuals 
	// make Virtual 0 --> Virtual 1 (SHORT)
	// make Virtual 0 --> Virtual 2 (LONG)
	if ((m_pNrUnitInfo->nControlUnits == 0) && (m_pNrUnitInfo->virtualUnit.nrOfUnits == 3)) 
	{
		nVirtualShort = 1; nVirtualLong = 2;
		nAutoBindings += ProcessBinding(AUTO_BINDINGS_MOODS_VIRTUAL_SHORT);
		nAutoBindings += ProcessBinding(AUTO_BINDINGS_MOODS_VIRTUAL_LONG);
		if (nAutoBindings == 2) 
		{
			m_pAutoBindingInfo->AddMessage(MoodsAutoStrings[AUTO_BINDINGS_MOODS_VIRTUAL_SHORT]);
			m_pAutoBindingInfo->AddMessage(MoodsAutoStrings[AUTO_BINDINGS_MOODS_VIRTUAL_LONG]);
		}
	}
	// 1 Control and 2 Virtuals
	// make Control --> Virtual 0 (Short)
	// make Control --> Virtual 1 (Long)
	else if ((m_pNrUnitInfo->nControlUnits == 1) && (m_pNrUnitInfo->virtualUnit.nrOfUnits==2))
	{
		nVirtualShort = 0; nVirtualLong = 1;
		nAutoBindings += ProcessBinding(AUTO_BINDINGS_MOODS_CONTROL_SHORT);
		nAutoBindings += ProcessBinding(AUTO_BINDINGS_MOODS_CONTROL_LONG);	
		if (nAutoBindings == 2) {
			m_pAutoBindingInfo->AddMessage(MoodsAutoStrings[AUTO_BINDINGS_MOODS_CONTROL_SHORT]);
			m_pAutoBindingInfo->AddMessage(MoodsAutoStrings[AUTO_BINDINGS_MOODS_CONTROL_LONG]);
		}
	}	
	// 1 Control and 1 Virtual 
	// make Control --> Virtual 0 (Short / long f(settings))
	else if ((m_pNrUnitInfo->nControlUnits == 1) && (m_pNrUnitInfo->virtualUnit.nrOfUnits==1)) 
	{
		switch (m_pAutoBindingOptions->GetAutoMoodOption())
		{
			case MOODS_AUTO_OPTION_SHORT:
				nVirtualShort = 0;
				nAutoBindings += ProcessBinding(AUTO_BINDINGS_MOODS_CONTROL_SHORT);
				if (nAutoBindings == 1) {
					m_pAutoBindingInfo->AddMessage(MoodsAutoStrings[AUTO_BINDINGS_MOODS_CONTROL_SHORT]);
				}
			break;

			case MOODS_AUTO_OPTION_LONG:
				nVirtualLong = 0;
				nAutoBindings += ProcessBinding(AUTO_BINDINGS_MOODS_CONTROL_LONG);
				if (nAutoBindings == 1) {
					m_pAutoBindingInfo->AddMessage(MoodsAutoStrings[AUTO_BINDINGS_MOODS_CONTROL_LONG]);
				}
			break;

			default: ASSERT(0); return 0;
			break;
		}
	}
	// 2 Virtuals , no Controls allowed
	// make Virtual 0 --> Virtual 1 (Short / Long f(Settings))
	else if ((m_pNrUnitInfo->nControlUnits == 0) && (m_pNrUnitInfo->virtualUnit.nrOfUnits==2)) 
	{		
		switch (m_pAutoBindingOptions->GetAutoMoodOption())
		{
			case MOODS_AUTO_OPTION_SHORT:
				nVirtualShort = 1;
				nAutoBindings += ProcessBinding(AUTO_BINDINGS_MOODS_VIRTUAL_SHORT);
				if (nAutoBindings == 1) {
					m_pAutoBindingInfo->AddMessage(MoodsAutoStrings[AUTO_BINDINGS_MOODS_VIRTUAL_SHORT]);
				}
			break;

			case MOODS_AUTO_OPTION_LONG:
				nVirtualLong = 1;
				nAutoBindings += ProcessBinding(AUTO_BINDINGS_MOODS_VIRTUAL_LONG);				
				if (nAutoBindings == 1) {
					m_pAutoBindingInfo->AddMessage(MoodsAutoStrings[AUTO_BINDINGS_MOODS_VIRTUAL_LONG]);
				}
			break;

			default: ASSERT(0); return 0;
			break;
		}
	}
	return nAutoBindings;
}

/******************************************************************************
	Name : CBindingAutoTemplate_Moods::ProcessBindingShort
	Task : Make Auto Binding : 1 Control -> 1 Virtual
*******************************************************************************/
BOOL CBindingAutoTemplate_Moods::ProcessBindingShort(CBindingEntry *pEntry)
{
	//ClearBindingEntry(pEntry);

	m_ControlUnitProp.SetEvent(EV_UNITCONTROLPULS);
	m_ControlUnitProp.SetStateInverse( FALSE );						/* TM BUG_RODE_PIJL_20100727 */

	m_VirtualUnitProp.SetFunction(VIRTUAL_OBJECT_METHOD_PULS);
	m_VirtualUnitProp.SetStateInverse( FALSE );						/* TM BUG_RODE_PIJL_20100727 */

	if (CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL) == 0) return FALSE;
	CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,nVirtualShort);

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,UNIT_TYPE_OUTPUT);

	AddLinks(pEntry,m_pNrUnitInfo->nControlUnits,m_pNrUnitInfo->virtualUnit.nrOfUnits);

	return (TRUE);
}
/******************************************************************************
	Name : CBindingAutoTemplate_Moods::ProcessBindingLong
	Task : Make Auto Binding : 1 Control -> 1 Virtual
*******************************************************************************/
BOOL CBindingAutoTemplate_Moods::ProcessBindingLong(CBindingEntry *pEntry)
{
	m_ControlUnitProp.SetEvent(EV_UNITCONTROLTOGGLE);
	m_ControlUnitProp.SetStateInverse( FALSE );						/* TM BUG_RODE_PIJL_20100727 */

	m_VirtualUnitProp.SetFunction(VIRTUAL_OBJECT_METHOD_PULS);
	m_VirtualUnitProp.SetStateInverse( FALSE );						/* TM BUG_RODE_PIJL_20100727 */

	//ClearBindingEntry(pEntry);	//BUG 20040817

	if (CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL) == 0) return FALSE;
	CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,nVirtualLong);

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,UNIT_TYPE_OUTPUT);

	AddLinks(pEntry,m_pNrUnitInfo->nControlUnits,m_pNrUnitInfo->virtualUnit.nrOfUnits);

	return (TRUE);
}
/******************************************************************************
	Name : CBindingAutoTemplate_Moods::ProcessBindingShort_virt
	Task : Make Auto Binding : 1 Virtual -> 1 Virtual
*******************************************************************************/
BOOL CBindingAutoTemplate_Moods::ProcessBindingShort_virt(CBindingEntry *pEntry)
{
	CBindingUnits *pBindingUnits = NULL;
	CBindingUnit *pBindingUnit = NULL;

	//ClearBindingEntry(pEntry);

	m_VirtualUnitProp.SetEvent(EV_UNITCONTROLPULS);
	m_VirtualUnitProp.SetFunction(VIRTUAL_OBJECT_METHOD_PULS);
	m_VirtualUnitProp.SetStateInverse( FALSE );						/* TM BUG_RODE_PIJL_20100727 */

	m_ControlUnitProp.SetStateInverse( FALSE );						/* TM BUG_RODE_PIJL_20100727 */

	CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0);
	CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,nVirtualShort);

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,UNIT_TYPE_OUTPUT);

	// Set Virtual 0 as input ...
	pBindingUnits = pEntry->GetColUnit();
	ASSERT(pBindingUnits);
	pBindingUnit = pBindingUnits->Get(0);
	ASSERT(pBindingUnit);
	pBindingUnit->SetInOutType(UNIT_TYPE_INPUT);
		
	AddLinks(pEntry,1,m_pNrUnitInfo->virtualUnit.nrOfUnits-1);	
	return (TRUE);
}
/******************************************************************************
	Name : CBindingAutoTemplate_Moods::ProcessBindingLong_virt
	Task : Make Auto Binding : 1 Virtual -> 1 Virtual
*******************************************************************************/
BOOL CBindingAutoTemplate_Moods::ProcessBindingLong_virt(CBindingEntry *pEntry)
{
	CBindingUnits *pBindingUnits = NULL;
	CBindingUnit *pBindingUnit = NULL;

	//ClearBindingEntry(pEntry);

	m_VirtualUnitProp.SetEvent(EV_UNITCONTROLTOGGLE);
	m_VirtualUnitProp.SetFunction(VIRTUAL_OBJECT_METHOD_PULS);
	m_VirtualUnitProp.SetStateInverse( FALSE );						/* TM BUG_RODE_PIJL_20100727 */

	m_ControlUnitProp.SetStateInverse( FALSE );						/* TM BUG_RODE_PIJL_20100727 */

	CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0);
	CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,nVirtualLong);

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,UNIT_TYPE_OUTPUT);

	// Set Virtual 0 as input ...
	pBindingUnits = pEntry->GetColUnit();
	ASSERT(pBindingUnits);
	pBindingUnit = pBindingUnits->Get(0);
	ASSERT(pBindingUnit);
	pBindingUnit->SetInOutType(UNIT_TYPE_INPUT);

	AddLinks(pEntry,1,m_pNrUnitInfo->virtualUnit.nrOfUnits-1);

	return (TRUE);
}

//-----------
CBindingAutoTemplate_Sens::CBindingAutoTemplate_Sens(CBindingConversionAuto *pParent)
: CBindingAutoTemplate(pParent) 
{

	SensAutoBindingActions[AUTO_BINDING_SENS_HEATING] = (Action) &CBindingAutoTemplate_Sens::ProcessBindingHeating;		// CHANGES_MVS_2008
	SensAutoBindingActions[AUTO_BINDING_SENS_COOLING] = (Action) &CBindingAutoTemplate_Sens::ProcessBindingCooling;		// CHANGES_MVS_2008

	SensAutoStrings[AUTO_BINDING_SENS_HEATING].Format(_T("Event Heating : Switch relais On/Off f(Heating)"));
	SensAutoStrings[AUTO_BINDING_SENS_COOLING].Format(_T("Event Cooling : Switch relais On/Off f(Cooling)"));
}

CBindingAutoTemplate_Sens::~CBindingAutoTemplate_Sens() 
{
	for (int i=0;i<AUTO_BINDING_SENS_MAX;i++) 
	{
		SensAutoStrings[i].Empty();
	}
}

int CBindingAutoTemplate_Sens::ProcessTemplate(void)
{
	int nAutoBindings = 0;

	nSwitchHeat=0; nSwitchCool=0;

	if ((m_pNrUnitInfo->nSensUnits >= 0) && (m_pNrUnitInfo->nSwitchUnits == 1)) 
	{
		switch(m_pAutoBindingOptions->GetAutoSensOption())
		{
			case AUTO_BINDING_SENS_HEATING :
				nSwitchHeat = 0;	nSwitchCool = 1;
				nAutoBindings += ProcessBinding(AUTO_BINDING_SENS_HEATING);
				if (nAutoBindings == 1) {
					m_pAutoBindingInfo->AddMessage(SensAutoStrings[AUTO_BINDING_SENS_HEATING]);
				}
			break;

			case AUTO_BINDING_SENS_COOLING :
				nSwitchHeat = 1;	nSwitchCool = 0;
				nAutoBindings += ProcessBinding(AUTO_BINDING_SENS_COOLING);
				if (nAutoBindings == 1) {
					m_pAutoBindingInfo->AddMessage(SensAutoStrings[AUTO_BINDING_SENS_COOLING]);
				}
			break;

			default:
				ASSERT(0);
				return 0;
			break;
		}		
	}
	
	if ((m_pNrUnitInfo->nSensUnits >= 0) && (m_pNrUnitInfo->nSwitchUnits == 2)) 
	{
		nSwitchHeat = 0;	nSwitchCool = 1;
		nAutoBindings += ProcessBinding(AUTO_BINDING_SENS_HEATING);
		nAutoBindings += ProcessBinding(AUTO_BINDING_SENS_COOLING);
	
		if (nAutoBindings == 2) 
		{
			m_pAutoBindingInfo->AddMessage(SensAutoStrings[AUTO_BINDING_SENS_HEATING]);
			m_pAutoBindingInfo->AddMessage(SensAutoStrings[AUTO_BINDING_SENS_COOLING]);
		}
	}
	return nAutoBindings;
}

BOOL CBindingAutoTemplate_Sens::ProcessBindingHeating(CBindingEntry *pEntry)
{
	m_ControlUnitProp.SetStateInverse( FALSE );						/* TM BUG_RODE_PIJL_20100727 */

	m_SensUnitProp.SetEvent(EV_HEATING);

	m_SwitchUnitProp.SetFunction(SWITCH_OBJECT_METHOD_FSONOFF);
	m_SwitchUnitProp.SetPirTime(0xFF);

	/* Sedert versie 0x0D00 */
	if (0==(CopyUnits(pEntry,BINDING_UNIT_TYPE_SENS)+
		   (CopyUnits(pEntry,BINDING_UNIT_TYPE_SENSUNIV))))
	{
		return FALSE;
	}


	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_SWITCH,nSwitchHeat)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD,nSwitchHeat))==0)
	{
		return FALSE;
	}


	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SENS,UNIT_TYPE_INPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SENSUNIV,UNIT_TYPE_INPUT);	/* Sedert versie 0x0D00 */ 

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SWITCH);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD);

	if (m_pNrUnitInfo->nSensUnits > 1) 
	{
		m_InputOperator.SetUsed(TRUE);
		m_InputOperator.SetType(PM_OPERATOR_OR);	
		AddOperator(m_InputOperator.GetType(),pEntry);	
	}

	AddLinks(pEntry,m_pNrUnitInfo->nSensUnits,m_pNrUnitInfo->nSwitchUnits);

	return (TRUE);
}

BOOL CBindingAutoTemplate_Sens::ProcessBindingCooling(CBindingEntry *pEntry)
{
	m_ControlUnitProp.SetStateInverse( FALSE );						

	m_SensUnitProp.SetEvent(EV_COOLING);

	m_SwitchUnitProp.SetFunction(SWITCH_OBJECT_METHOD_FSONOFF);
	m_SwitchUnitProp.SetPirTime(0xFF);

	/* Sedert versie 0x0D00 */
	if (0==(CopyUnits(pEntry,BINDING_UNIT_TYPE_SENS)+
		   (CopyUnits(pEntry,BINDING_UNIT_TYPE_SENSUNIV))))
	{
		return FALSE;
	}

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_SWITCH,nSwitchCool)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD,nSwitchCool))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SENS,UNIT_TYPE_INPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SENSUNIV,UNIT_TYPE_INPUT);

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SWITCH);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD);

	if (m_pNrUnitInfo->nSensUnits > 1 ) 
	{
		m_InputOperator.SetUsed(TRUE);
		m_InputOperator.SetType(PM_OPERATOR_OR);	
		AddOperator(m_InputOperator.GetType(),pEntry);	
	}

	AddLinks(pEntry,m_pNrUnitInfo->nSensUnits,m_pNrUnitInfo->nSwitchUnits);

	return (TRUE);
}



//-----------
CBindingAutoTemplate_Bose::CBindingAutoTemplate_Bose(CBindingConversionAuto *pParent)
: CBindingAutoTemplate(pParent) 
{

	BoseAutoBindingActions[AUTO_BINDING_BOSE_SOURCE] = (Action) &CBindingAutoTemplate_Bose::ProcessBindingShort;			// CHANGES_MVS_2008
	BoseAutoBindingActions[AUTO_BINDING_BOSE_VOLUPDN] = (Action) &CBindingAutoTemplate_Bose::ProcessBindingLong;			// CHANGES_MVS_2008
	BoseAutoBindingActions[AUTO_BINDING_BOSE_SOURCE_FM] = (Action) &CBindingAutoTemplate_Bose::ProcessBindingShort_Input1;	// CHANGES_MVS_2008
	BoseAutoBindingActions[AUTO_BINDING_BOSE_SOURCE_CD] = (Action) &CBindingAutoTemplate_Bose::ProcessBindingShort_Input2;	// CHANGES_MVS_2008
	BoseAutoBindingActions[AUTO_BINDING_BOSE_VOLUP] = (Action) &CBindingAutoTemplate_Bose::ProcessBindingLong_Input1;		// CHANGES_MVS_2008
	BoseAutoBindingActions[AUTO_BINDING_BOSE_VOLDN] = (Action) &CBindingAutoTemplate_Bose::ProcessBindingLong_Input2;		// CHANGES_MVS_2008
	BoseAutoBindingActions[AUTO_BINDING_BOSE_VOLUP_STOP] = (Action) &CBindingAutoTemplate_Bose::ProcessBindingLong_InvInput1;	// CHANGES_MVS_2008
	BoseAutoBindingActions[AUTO_BINDING_BOSE_VOLDN_STOP] = (Action) &CBindingAutoTemplate_Bose::ProcessBindingLong_InvInput2;	// CHANGES_MVS_2008
	BoseAutoBindingActions[AUTO_BINDING_BOSE_DESTOFF] = (Action) &CBindingAutoTemplate_Bose::ProcessBindingShort_Inv;	// CHANGES_MVS_2008

	BoseAutoStrings[AUTO_BINDING_BOSE_SOURCE].Format(_T("Event Short : Select FM"));
	BoseAutoStrings[AUTO_BINDING_BOSE_VOLUPDN].Format(_T("Event Long : Volume up/dn f(State)"));
	BoseAutoStrings[AUTO_BINDING_BOSE_SOURCE_FM].Format(_T("Event Short (input1): Select FM"));
	BoseAutoStrings[AUTO_BINDING_BOSE_SOURCE_CD].Format(_T("Event Short (input2): Select CD"));
	BoseAutoStrings[AUTO_BINDING_BOSE_VOLUP].Format(_T("Event Long (input1): Volume Up"));
	BoseAutoStrings[AUTO_BINDING_BOSE_VOLDN].Format(_T("Event Long (input2): Volume Down"));
	BoseAutoStrings[AUTO_BINDING_BOSE_VOLUP_STOP].Format(_T("Event Long (input1): Volume Stop"));
	BoseAutoStrings[AUTO_BINDING_BOSE_VOLDN_STOP].Format(_T("Event Long (input2): Volume Stop"));
	BoseAutoStrings[AUTO_BINDING_BOSE_DESTOFF].Format(_T("Event Short : Destination Off"));
}

CBindingAutoTemplate_Bose::~CBindingAutoTemplate_Bose() 
{
	for (int i=0;i<AUTO_BINDING_BOSE_MAX;i++) 
	{
		BoseAutoStrings[i].Empty();
	}
}

int CBindingAutoTemplate_Bose::ProcessTemplate(void)
{
	int nAutoBindings = 0;
	
	ASSERT(m_pNrUnitInfo->virtualUnit.nrOfUnits==m_pNrUnitInfo->virtualUnit.nrOfInputs);

	if (((m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs)==1)&&
		(m_pNrUnitInfo->nBoseUnits==1))
	{
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_SOURCE);
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_DESTOFF);
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_VOLUPDN);	
		
		if (nAutoBindings == 3) 
		{
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_SOURCE]);
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_DESTOFF]);	//new ...
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_VOLUPDN]);
		}
	}
	else if (((m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs)==2)&& 
			 (m_pNrUnitInfo->nBoseUnits==1))
	{	
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_SOURCE_FM);
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_DESTOFF);
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_SOURCE_CD);
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_VOLUP);
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_VOLDN);
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_VOLUP_STOP);
		nAutoBindings += ProcessBinding(AUTO_BINDING_BOSE_VOLDN_STOP);	
		
		if (nAutoBindings == 7) 
		{
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_SOURCE_FM]);
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_DESTOFF]);	//new ...
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_VOLUP]);
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_VOLUP_STOP]);
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_SOURCE_CD]);
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_VOLDN]);
			m_pAutoBindingInfo->AddMessage(BoseAutoStrings[AUTO_BINDING_BOSE_VOLDN_STOP]);
		}
	}
	return nAutoBindings;
}

BYTE CBindingAutoTemplate_Bose::getBoseBindingUnitType(void)
{
	BYTE bBindingUnitType = 0xFF;

	if (m_pNrUnitInfo->boseType == CBindingConversionAutoUnitInfo::BOSE_TYPE_LF50)
	{
		bBindingUnitType = BINDING_UNIT_TYPE_BOSERS;
	}
	else if (m_pNrUnitInfo->boseType == CBindingConversionAutoUnitInfo::BOSE_TYPE_LF48)
	{
		bBindingUnitType = BINDING_UNIT_TYPE_BOSELF48;
	}
	else if (m_pNrUnitInfo->boseType == CBindingConversionAutoUnitInfo::BOSE_TYPE_V35)
	{
		bBindingUnitType = BINDING_UNIT_TYPE_BOSEV35;
	}

	return bBindingUnitType;
}


// Kort: Selectie Eerste bron.
BOOL CBindingAutoTemplate_Bose::ProcessBindingShort(CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks=0;
	const BYTE bEvent=EV_UNITCONTROLPULSTOGGLE;
	const BOOL fStateInverse=FALSE;

	m_BoseUnitProp.SetFunction(BOSERS_OBJECT_ATTR_SOURCE_ON);
	m_BoseUnitProp.SetSource(0);
	
	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);
	
	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL))==0)
	{
		return FALSE;
	}

	const BYTE bOutputUnitType=getBoseBindingUnitType();
	if (0xFF==bOutputUnitType)
	{
		return FALSE;
	}

	if (CopyUnits(pEntry,bOutputUnitType)>0)
	{
		ASSERT((m_pNrUnitInfo->nBoseUnits>=0)&&(m_pNrUnitInfo->nBoseUnits<=0xFF));		// CHANGES_MVS_2008_WARNINGS
		bNrOutputLinks = static_cast<BYTE>(m_pNrUnitInfo->nBoseUnits);				// CHANGES_MVS_2008_WARNINGS
	}
	else
	{
		return FALSE;
	}
	
	AddUnitProperties(pEntry,bOutputUnitType,UNIT_TYPE_OUTPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);

	AddLinks(pEntry,m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs,bNrOutputLinks);
	return (TRUE);
}

// Kort: Destination Off
BOOL CBindingAutoTemplate_Bose::ProcessBindingShort_Inv(CBindingEntry *pEntry)
{
	const BYTE bEvent=EV_UNITCONTROLPULSTOGGLE;
	const BOOL fStateInverse=TRUE;	
	BYTE bNrOutputLinks = 0;

	m_BoseUnitProp.SetFunction(BOSERS_OBJECT_METHOD_OFF);
	m_BoseUnitProp.SetSource(0);
	
	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);
	
	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}	

	const BYTE bOutputUnitType=getBoseBindingUnitType();
	if (0xFF==bOutputUnitType)
	{
		return FALSE;
	}

	if (CopyUnits(pEntry,bOutputUnitType)>0)
	{
		bNrOutputLinks = m_pNrUnitInfo->nBoseUnits;
	}
	else
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,bOutputUnitType,UNIT_TYPE_OUTPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);

	AddLinks(pEntry,1,bNrOutputLinks);

	return (TRUE);
}

// Lang: Volume f(up/down) - Bindingvlaggen=POST
BOOL CBindingAutoTemplate_Bose::ProcessBindingLong(CBindingEntry *pEntry)	
{	
	const BYTE bEvent=EV_UNITCONTROLTOGGLE;
	const BOOL fStateInverse=FALSE;
	BYTE bNrOutputLinks;

	pEntry->SetBindingReturnFlags(BINDINGFLAGS_STATUS_POST);

	m_BoseUnitProp.SetFunction(BOSERS_OBJECT_METHOD_FSVOLUPDOWN);
	m_BoseUnitProp.SetSource(0);
	
	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);
	
	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL))==0)
	{
		return FALSE;
	}

	const BYTE bOutputUnitType=getBoseBindingUnitType();
	if (0xFF==bOutputUnitType)
	{
		return FALSE;
	}

	if (CopyUnits(pEntry,bOutputUnitType)>0)
	{
		bNrOutputLinks = m_pNrUnitInfo->nBoseUnits;
	}
	else
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,bOutputUnitType,UNIT_TYPE_OUTPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);

	AddLinks(pEntry,m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs,bOutputUnitType);
	return (TRUE);
}

//-----------

//dk0 -  Kort: Selectie bron 0
BOOL CBindingAutoTemplate_Bose::ProcessBindingShort_Input1(CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLPULSTOGGLE;
	const BOOL fStateInverse=FALSE;

	m_BoseUnitProp.SetFunction(BOSERS_OBJECT_ATTR_SOURCE_ON);
	m_BoseUnitProp.SetSource(0);
	
	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);
	
	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}

	const BYTE bOutputUnitType=getBoseBindingUnitType();
	if (0xFF==bOutputUnitType)
	{
		return FALSE;
	}

	if (CopyUnits(pEntry,bOutputUnitType) > 0)
	{
		bNrOutputLinks = m_pNrUnitInfo->nBoseUnits;
	}
	else
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,bOutputUnitType,UNIT_TYPE_OUTPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);

	AddLinks(pEntry,1,bNrOutputLinks);
	return (TRUE);
}

// dk1 - Kort: Selectie bron 1
BOOL CBindingAutoTemplate_Bose::ProcessBindingShort_Input2(CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLPULS;
	const BOOL fStateInverse=FALSE;

	m_BoseUnitProp.SetFunction(BOSERS_OBJECT_ATTR_SOURCE_ON);
	m_BoseUnitProp.SetSource(1);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,1)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,1))==0)
	{
		return FALSE;
	}

	const BYTE bOutputUnitType=getBoseBindingUnitType();
	if (0xFF==bOutputUnitType)
	{
		return FALSE;
	}

	if (CopyUnits(pEntry,bOutputUnitType) > 0)
	{
		bNrOutputLinks = m_pNrUnitInfo->nBoseUnits;
	}
	else
	{
		return FALSE;
	}
	
	AddUnitProperties(pEntry,bOutputUnitType,UNIT_TYPE_OUTPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);

	AddLinks(pEntry,1,bNrOutputLinks);
	return (TRUE);
}

// dk0 - Lang: Volume op
BOOL CBindingAutoTemplate_Bose::ProcessBindingLong_Input1(CBindingEntry *pEntry)
{
	const BYTE bEvent=EV_UNITCONTROLTOGGLE;
	const BOOL fStateInverse=FALSE;
	BYTE bNrOutputLinks;

	m_BoseUnitProp.SetFunction(BOSERS_OBJECT_METHOD_VOLUP);
	
	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);
	
	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}

	const BYTE bOutputUnitType=getBoseBindingUnitType();
	if (0xFF==bOutputUnitType)
	{
		return FALSE;
	}

	if (CopyUnits(pEntry,bOutputUnitType) > 0)
	{
		bNrOutputLinks = m_pNrUnitInfo->nBoseUnits;
	}
	else
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,bOutputUnitType,UNIT_TYPE_OUTPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);

	AddLinks(pEntry,1,bNrOutputLinks);
	return (TRUE);
}

// dk1 - Lang: Volume neer
BOOL CBindingAutoTemplate_Bose::ProcessBindingLong_Input2(CBindingEntry *pEntry)
{
	const BYTE bEvent=EV_UNITCONTROLTOGGLE;
	const BOOL fStateInverse=FALSE;
	BYTE bNrOutputLinks;

	m_BoseUnitProp.SetFunction(BOSERS_OBJECT_METHOD_VOLDOWN);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);
	
	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,1)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,1))==0)
	{
		return FALSE;
	}	

	const BYTE bOutputUnitType=getBoseBindingUnitType();
	if (0xFF==bOutputUnitType)
	{
		return FALSE;
	}

	if (CopyUnits(pEntry,bOutputUnitType) > 0)
	{
		bNrOutputLinks = m_pNrUnitInfo->nBoseUnits;
	}
	else
	{
		return FALSE;
	}
	
	AddUnitProperties(pEntry,bOutputUnitType,UNIT_TYPE_OUTPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);

	AddLinks(pEntry,1,bNrOutputLinks);
	return (TRUE);
}

// dk0 - Lang inv: Current Src On (stop vol)
BOOL CBindingAutoTemplate_Bose::ProcessBindingLong_InvInput1(CBindingEntry *pEntry)
{
	const BYTE bEvent=EV_UNITCONTROLTOGGLE;
	const BOOL fStateInverse=TRUE;
	BYTE bNrOutputLinks;

	m_BoseUnitProp.SetFunction(BOSERS_OBJECT_METHOD_CSON);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);
	
	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}

	const BYTE bOutputUnitType=getBoseBindingUnitType();
	if (0xFF==bOutputUnitType)
	{
		return FALSE;
	}

	if (CopyUnits(pEntry,bOutputUnitType) > 0)
	{
		bNrOutputLinks = m_pNrUnitInfo->nBoseUnits;
	}
	else
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,bOutputUnitType,UNIT_TYPE_OUTPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);

	AddLinks(pEntry,1,bNrOutputLinks);
	return (TRUE);
}

// dk0 - Lang inv: Current Src On (stop vol)
BOOL CBindingAutoTemplate_Bose::ProcessBindingLong_InvInput2(CBindingEntry *pEntry)
{
	const BYTE bEvent=EV_UNITCONTROLTOGGLE;
	const BOOL fStateInverse=TRUE;
	BYTE bNrOutputLinks;

	m_BoseUnitProp.SetFunction(BOSERS_OBJECT_METHOD_CSON);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);
	
	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,1)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,1))==0)
	{
		return FALSE;
	}

	const BYTE bOutputUnitType=getBoseBindingUnitType();
	if (0xFF==bOutputUnitType)
	{
		return FALSE;
	}

	if (CopyUnits(pEntry,bOutputUnitType) > 0)
	{
		bNrOutputLinks = m_pNrUnitInfo->nBoseUnits;
	}
	else
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,bOutputUnitType,UNIT_TYPE_OUTPUT);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);

	AddLinks(pEntry,1,bNrOutputLinks);

	return (TRUE);
}


//------------------------------------------------------------------------------
CBindingAutoTemplate_CondPIR::CBindingAutoTemplate_CondPIR(CBindingConversionAuto *pParent)
: CBindingAutoTemplate(pParent) 
{

	ConditionalBindingActions[AUTO_BINDING_COND_DONKERBUITEN] = (Action) &CBindingAutoTemplate_CondPIR::ProcessBindingPIR_DkBuiten;		// CHANGES_MVS_2008
	ConditionalBindingActions[AUTO_BINDING_COND_KLAAR] = (Action) &CBindingAutoTemplate_CondPIR::ProcessBindingPIR_Klaar;				// CHANGES_MVS_2008
	ConditionalBindingActions[AUTO_BINDING_COND_NA22H] = (Action) &CBindingAutoTemplate_CondPIR::ProcessBindingPIR_Na22h;				// CHANGES_MVS_2008
	ConditionalBindingActions[AUTO_BINDING_COND_VOOR22H] = (Action) &CBindingAutoTemplate_CondPIR::ProcessBindingPIR_Voor22h;			// CHANGES_MVS_2008
	ConditionalBindingActions[AUTO_BINDING_COND_DONKER99] = (Action) &CBindingAutoTemplate_CondPIR::ProcessBindingPIR_Dk99;				// CHANGES_MVS_2008

	ConditionalAutoStrings[AUTO_BINDING_COND_DONKERBUITEN].Format(_T("PIR with 1 Condition : PIR On 3min 30%%"));
	ConditionalAutoStrings[AUTO_BINDING_COND_KLAAR].Format(_T("PIR with 1 Condition : PIR On 3min 99%%"));
	ConditionalAutoStrings[AUTO_BINDING_COND_NA22H].Format(_T("PIR with 2 Conditions : PIR On 3min 30%%"));
	ConditionalAutoStrings[AUTO_BINDING_COND_VOOR22H].Format(_T("PIR with 2 Conditions : PIR On 3min 99%%"));
	ConditionalAutoStrings[AUTO_BINDING_COND_DONKER99].Format(_T("PIR with 1 Condition : PIR On 3min 99%%"));
}

CBindingAutoTemplate_CondPIR::~CBindingAutoTemplate_CondPIR() 
{
	for (int i=0;i<AUTO_BINDING_COND_MAX;i++) 
	{
		ConditionalAutoStrings[i].Empty();
	}
}

BOOL CBindingAutoTemplate_CondPIR::CheckAddress(BYTE bNodeAddress,BYTE bUnitAddress)
{
	CBindingUnit *pBindingUnit = NULL;
	const int nCount = m_pgBindingUnits->Count();
	for (int i=0;i<nCount;i++) {	
		if ((pBindingUnit=m_pgBindingUnits->Get(i))!=NULL) {				
			if ((pBindingUnit->GetNodeAddress() == bNodeAddress) &&
				(pBindingUnit->GetUnitAddress() == bUnitAddress)) {
				return TRUE;
			}
		}	
		ASSERT(pBindingUnit);
	}
	return FALSE;
}

// Sedert V16.17: 
// Twilight (0xfc,0x02) wordt op dezelfde manier gebruikt als de Dark outside (0xfc,0x03).
// Aanpassingen aan (1)
BOOL CBindingAutoTemplate_CondPIR::AddConditinalUnitProperties(CBindingEntry *pEntry)
{
	CBindingUnit *pBindingUnit = NULL;
	CBindingUnits* const pBindingUnits = pEntry->GetColUnit();
	ASSERT(pBindingUnits);
	CBindingUnitsOperations BindingUnitsOperations(pBindingUnits);	
	for (int i=0;i<m_pNrUnitInfo->virtualUnit.nrOfUnits;i++) {
		if (BindingUnitsOperations.GetUnitType(BINDING_UNIT_TYPE_LCD_VIRTUAL,i,&pBindingUnit)) {
			pBindingUnit->SetInOutType(PM_INOUTTYPE_INPUT);						
			if(m_pNrUnitInfo->virtualUnit.nrOfUnits==2) {
				const BYTE bUnitAddress = pBindingUnit->GetUnitAddress();
				if (pBindingUnit->GetNodeAddress() == 0xFC && ((bUnitAddress == 0x02) || (bUnitAddress == 0x03))) {
					/* (1) */
					((CBindingVirtualUnit*)pBindingUnit)->Set_Event(m_CondUnitProp[0].GetEvent());
					((CBindingVirtualUnit*)pBindingUnit)->Set_ControlState(m_CondUnitProp[0].IsStateInvers());			
				}
				else {			
					((CBindingVirtualUnit*)pBindingUnit)->Set_Event(m_CondUnitProp[1].GetEvent());
					((CBindingVirtualUnit*)pBindingUnit)->Set_ControlState(m_CondUnitProp[1].IsStateInvers());			
				}
			}
			else {
				((CBindingVirtualUnit*)pBindingUnit)->Set_Event(m_CondUnitProp[0].GetEvent());
				((CBindingVirtualUnit*)pBindingUnit)->Set_ControlState(m_CondUnitProp[0].IsStateInvers());
				return TRUE;
			}					
		}
	}
	return TRUE;
}


// Sedert V16.17: 
// Twilight (0xfc,0x02) wordt op dezelfde manier gebruikt als de Dark outside (0xfc,0x03).
// Aanpassingen aan (1) en (3)
int CBindingAutoTemplate_CondPIR::ProcessTemplate(void)
{
	int nAutoBindings = 0;

	if (m_pNrUnitInfo->virtualUnit.nrOfUnits==1) {
		if (CheckAddress(0xFC,0x02) || CheckAddress(0xFC,0x03)) {
			/* (1) */
			nAutoBindings += ProcessBinding(AUTO_BINDING_COND_DONKER99);		
			if (nAutoBindings == 1) {
				m_pAutoBindingInfo->AddMessage(ConditionalAutoStrings[AUTO_BINDING_COND_DONKER99]);			
			}
		}	
		else if (CheckAddress(0xFC,0x04)) {
			/* (2) */
			nAutoBindings += ProcessBinding(AUTO_BINDING_COND_DONKERBUITEN);
			nAutoBindings += ProcessBinding(AUTO_BINDING_COND_KLAAR);		
			if (nAutoBindings == 2) {
				m_pAutoBindingInfo->AddMessage(ConditionalAutoStrings[AUTO_BINDING_COND_DONKERBUITEN]);
				m_pAutoBindingInfo->AddMessage(ConditionalAutoStrings[AUTO_BINDING_COND_KLAAR]);
			}		
		}
	}
	else if (m_pNrUnitInfo->virtualUnit.nrOfUnits==2) {
		if ((CheckAddress(0xFC,0x02) || CheckAddress(0xFC,0x03)) && CheckAddress(0xFC,0x04)) {
			/* (3) */
			nAutoBindings += ProcessBinding(AUTO_BINDING_COND_NA22H);
			nAutoBindings += ProcessBinding(AUTO_BINDING_COND_VOOR22H);		
			if (nAutoBindings == 2) {
				m_pAutoBindingInfo->AddMessage(ConditionalAutoStrings[AUTO_BINDING_COND_NA22H]);
				m_pAutoBindingInfo->AddMessage(ConditionalAutoStrings[AUTO_BINDING_COND_VOOR22H]);
			}
		}
	}
	return nAutoBindings;
}

BOOL CBindingAutoTemplate_CondPIR::ProcessBindingPIR(CBindingEntry *pEntry,BYTE bDimValue,BYTE bPirTime,BOOL fInv,BOOL fInv2)	// used by DK-Buiten & klaar
{
	int nOutput = 0;

	m_DimmerUnitProp.SetFunction(DIMMER_OBJECT_METHOD_PIRON);
	m_DimmerUnitProp.SetEvent(0xFF);
	m_DimmerUnitProp.SetDimValue(bDimValue);
	m_DimmerUnitProp.SetPirTime(bPirTime);
	
	m_ControlUnitProp.SetEvent(EV_UNITCONTROLTOGGLE);
	m_ControlUnitProp.SetStateInverse(TRUE);			// !!!

	m_SwitchUnitProp.SetFunction(SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR);
	m_SwitchUnitProp.SetPirTime(bPirTime);

	// CondUnitProp[0] = DK BUITEN of NA 22H
	m_CondUnitProp[0].SetEvent(EV_UNITCONTROLPULSTOGGLE);
	m_CondUnitProp[0].SetStateInverse(fInv);

	m_CondUnitProp[1].SetEvent(EV_UNITCONTROLPULSTOGGLE);
	m_CondUnitProp[1].SetStateInverse(fInv2);

	if (CopyUnits(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL) == 0) return FALSE;
	if (CopyUnits(pEntry,BINDING_UNIT_TYPE_CONTROL) == 0) return FALSE;

	nOutput += CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM);
	nOutput += CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM_FWD);	
	nOutput += CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH);
	nOutput += CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD);

	if (nOutput==0) 
	{
		return FALSE;
	}

	AddConditinalUnitProperties(pEntry);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_DIM);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_DIM_FWD);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SWITCH);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD);

	if (m_pNrUnitInfo->nControlUnits>1) 
	{
		m_InputOperator.SetUsed(TRUE);
		m_InputOperator.SetType(PM_OPERATOR_OR);	
		AddOperator(m_InputOperator.GetType(),pEntry);	
	}

	// add Conditional Operator ...
	m_ConditionalOperator.SetUsed(TRUE);
	m_ConditionalOperator.SetType(PM_OPERATOR_AND);
	AddOperator(m_ConditionalOperator.GetType(),pEntry);

	// set conditional input ...
	AddLinks(pEntry,m_pNrUnitInfo->nControlUnits,
			 m_pNrUnitInfo->dimUnit.nrOfUnits+m_pNrUnitInfo->nSwitchUnits,
			 m_pNrUnitInfo->virtualUnit.nrOfUnits);

	return (TRUE);
}

BOOL CBindingAutoTemplate_CondPIR::ProcessBindingPIR_Dk99(CBindingEntry *pEntry)
{
	// DK Buiten 
	return (ProcessBindingPIR(pEntry,99,0xFF,FALSE,FALSE));
}


BOOL CBindingAutoTemplate_CondPIR::ProcessBindingPIR_DkBuiten(CBindingEntry *pEntry)
{
	// DK Buiten / NA 22h 
	return (ProcessBindingPIR(pEntry,30,0xFF,FALSE,FALSE));
}

BOOL CBindingAutoTemplate_CondPIR::ProcessBindingPIR_Klaar(CBindingEntry *pEntry)
{
	// KLAAR / VOOR 22h
	return (ProcessBindingPIR(pEntry,99,0xFF,TRUE,FALSE));
}

BOOL CBindingAutoTemplate_CondPIR::ProcessBindingPIR_Na22h(CBindingEntry *pEntry)
{
	// DK BUITEN en Na 22h
	return (ProcessBindingPIR(pEntry,30,0xFF,FALSE,FALSE));
}

BOOL CBindingAutoTemplate_CondPIR::ProcessBindingPIR_Voor22h(CBindingEntry *pEntry)
{
	// DK BUITEN en VOOR 22h
	return (ProcessBindingPIR(pEntry,99,0xFF,FALSE,TRUE));
}


/***************************************************************************************

  name : CBindingAutoTemplate_CondPIR::AddLinks
  task : overrides CBindingAutoTemplate::AddLinks
		 Adds intelligence to link conditional (AND) operator. 
  Arguments : CBindingEntry *pEntry [in/Out] : Bindingentry to add Links
			  int nInputs : Nr of inputs : UNIT_TYPE_CONTROLS
			  int nOutputs : Nr of Outputs : UNIT_TYPE_DIM + UNIT_TYPE_SWITCH
			  int nConditions : nr of virtuals : 0xfc-0x03 & 0xfc-0x04 : Max 2 allowed !! 

****************************************************************************************/

// Sedert V16.17: zie (1).
// Twilight (0xfc,0x02) wordt op dezelfde manier gebruikt als de Dark outside (0xfc,0x03).
BOOL CBindingAutoTemplate_CondPIR::AddLinks(CBindingEntry *pEntry,int nInputs,int nOutputs,int nConditions)
{
	unsigned short nOpID = 0;		// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	unsigned short nCondOpID = 0;	// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	unsigned short nInID = 0;		// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	unsigned short nOutID = 0;		// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	int i = 0;						// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	int j = 0;						// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	CBindingOperator *pBindingOperator = 0;	// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	CBindingUnit *pBindingUnit = 0;			// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	CBindingLink BindingLink;
	CBindingOperators* const pBindingOperators = pEntry->GetColOperator();
	CBindingUnits* const pBindingUnits = pEntry->GetColUnit();

	ASSERT(pBindingUnits);
	ASSERT(pBindingOperators);

	CBindingUnitsOperations BindingUnitsOperations(pBindingUnits);

	if (nConditions == 0) 
	{
		return (CBindingAutoTemplate::AddLinks(pEntry,nInputs,nOutputs));
	}

	const int nUnits = pBindingUnits->Count();
	const int nOperators = pBindingOperators->Count();
	
	// Get Normal operator ID ...
	if (nInputs > 1) 
	{	
		for (i=0;i<nOperators;i++) 
		{
			if ((pBindingOperator = pBindingOperators->Get(i)) == 0)
			{
				ASSERT(pBindingOperator);
				return FALSE;
			}

			if (pBindingOperator->GetType() != PM_OPERATOR_AND) 
			{
				nOpID = pBindingOperator->GetID();
				break;
			}
		}	
	}


	// Get Condtional inputs + link to conditional operator ...	
	for (i=0;i<nOperators;i++) 
	{
		if ((pBindingOperator = pBindingOperators->Get(i)) == 0)
		{
			ASSERT(pBindingOperator);
			return FALSE;
		}

		if (pBindingOperator->GetType() == PM_OPERATOR_AND) 
		{	
			// if and we found cond. operator ...
			nCondOpID = pBindingOperator->GetID();
			break;
		}
	}

	// link all Conditional units to Conditional operator ...
	for (i=0,j=0;i<nUnits;i++) 
	{		
		if ((pBindingUnit = pBindingUnits->Get(i)) == 0)
		{
			ASSERT(pBindingUnit);
			return FALSE;
		}

		if (pBindingUnit->GetInOutType() == UNIT_TYPE_INPUT) 
		{
			if (pBindingUnit->GetBindingUnitType() == BINDING_UNIT_TYPE_LCD_VIRTUAL) 
			{				
				nInID = pBindingUnit->GetID();
				BindingLink.SetInLink(nInID,PM_UNIT);
				BindingLink.SetOutLink(nCondOpID,PM_OPERATOR);				// MVS_2008_WARNINGS_UNITIALISED_!!!!!
				

	// DK BUITEN is always [0] , NA 22H can be [0] or [1]
			
				if(nConditions > 1) 
				{				
					const BYTE bUnitAddress = pBindingUnit->GetUnitAddress();
					if ((pBindingUnit->GetNodeAddress() == 0xFC) && ((0x02 == bUnitAddress) || (0x03 == bUnitAddress)))	/* (1) */{
							BindingLink.SetInverse(m_CondUnitProp[0].IsStateInvers());
					}
					else {
						BindingLink.SetInverse(m_CondUnitProp[1].IsStateInvers());
					}				
				}
				else  {				
					BindingLink.SetInverse(m_CondUnitProp[0].IsStateInvers());
				}
				AddLink(&BindingLink,pEntry);
			}
		}
	}

	if (nInputs >1) 
	{
		// link normal operator to cond. operator ...
		BindingLink.SetInLink(nOpID,PM_OPERATOR);				// MVS_2008_WARNINGS_UNITIALISED_!!!!!
		BindingLink.SetOutLink(nCondOpID,PM_OPERATOR);	
		BindingLink.SetInverse(FALSE);
		AddLink(&BindingLink,pEntry);
	}
	else 
	{
		// link first input to cond. operator ...
		for(i=0;i<nUnits;i++) 
		{		
			if ((pBindingUnit = pBindingUnits->Get(i)) == 0)
			{
				ASSERT(pBindingUnit);
				return FALSE;
			}

			if (pBindingUnit->GetInOutType() == UNIT_TYPE_INPUT) 
			{
				if (pBindingUnit->GetBindingUnitType() != BINDING_UNIT_TYPE_LCD_VIRTUAL) 
				{				
					nInID = pBindingUnit->GetID();

					BindingLink.SetInLink(nInID,PM_UNIT);
					BindingLink.SetOutLink(nCondOpID,PM_OPERATOR);	
					BindingLink.SetInverse(m_ControlUnitProp.IsStateInvers());

					AddLink(&BindingLink,pEntry);

					break;
				}
			}
		}
	}

	// link conditional operator to 1st output	
	if (BindingUnitsOperations.GetOutputs(0,&pBindingUnit)) {			
		nOutID = pBindingUnit->GetID();
		BindingLink.SetInLink(nCondOpID,PM_OPERATOR);	
		BindingLink.SetOutLink(nOutID,PM_UNIT);
		BindingLink.SetInverse(FALSE);		
		AddLink(&BindingLink,pEntry);	
	}		

	
	//............................................................................
	// standaard : applies not only to nConditions ...
	// ...........................................................................
	// if nr of inputs != 0 : get all inputs and add to normal operator ...
	
	if (nInputs >1) 
	{
		for(i=0;i<nUnits;i++) {
			if ((pBindingUnit = pBindingUnits->Get(i)) == 0) {
				ASSERT(pBindingUnit);
				return FALSE;
			}
		
			if (pBindingUnit->GetInOutType() == UNIT_TYPE_INPUT) {
				if (pBindingUnit->GetBindingUnitType() != BINDING_UNIT_TYPE_LCD_VIRTUAL) {
					nInID = pBindingUnit->GetID();
					BindingLink.SetInLink(nInID,PM_UNIT);
					BindingLink.SetOutLink(nOpID,PM_OPERATOR);			
					BindingLink.SetInverse(m_ControlUnitProp.IsStateInvers());					
					AddLink(&BindingLink,pEntry);
				}		
			}
		}
	}

		// Check if there are more then 1 output ...
	if (nOutputs > 1)  {
		nInID = 0;nOutID = 0;
		if (BindingUnitsOperations.GetOutputs(0,&pBindingUnit)) {
			nInID = pBindingUnit->GetID();
		}

		for(i=1;i<nOutputs;i++) {
			nOutID = 0;
			if (BindingUnitsOperations.GetOutputs(i,&pBindingUnit)) {
				nOutID = pBindingUnit->GetID();
			}

			if (nInID != 0 && nOutID != 0) {
				BindingLink.SetInLink(nInID,PM_UNIT);
			    BindingLink.SetOutLink(nOutID,PM_UNIT);
				BindingLink.SetInverse(FALSE);
				AddLink(&BindingLink,pEntry);
				nInID = nOutID;
			}
		}
	}
	return TRUE;
}

// 
CBindingAutoTemplate_AudioExt::CBindingAutoTemplate_AudioExt(CBindingConversionAuto *pParent)
: CBindingAutoTemplate(pParent) 
{
	m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_SOURCE] = (Action) &CBindingAutoTemplate_AudioExt::ProcessBindingShort_Input1;		
	m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_VOLUP] = (Action) &CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input1;				
	m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_VOLDOWN] = (Action) &CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input2;				
	m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_VOLSTOP1] = (Action) &CBindingAutoTemplate_AudioExt::ProcessBindingLong_InvInput1;			
	m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_VOLSTOP2] = (Action) &CBindingAutoTemplate_AudioExt::ProcessBindingLong_InvInput2;	
	m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_DESTOFF] = (Action) &CBindingAutoTemplate_AudioExt::ProcessBindingShort_Input2;				
	m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_SOURCE_PTON] = (Action) &CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input1_PTON;	
	m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_DESTOFF_PTOFF] = (Action) &CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input1_PTOFF;	
	m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_SOURCE_INC] = (Action) &CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input2_SRCINC;	

	m_AutoAutoStrings[AUTO_BINDING_AUDIO_SOURCE].Format(_T("Event Short: dest0 - Select Source"));
	m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLUP].Format(_T("Event long: dest0 - Volume up"));
	m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLDOWN].Format(_T("Event long: dest0 - Volume down"));
	m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLSTOP1].Format(_T("Event long: dest0 - Volume stop"));
	m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLSTOP2].Format(_T("Event long: dest0 - Volume stop"));
	m_AutoAutoStrings[AUTO_BINDING_AUDIO_DESTOFF].Format(_T("Event Short: dest0 - Set Off"));
	m_AutoAutoStrings[AUTO_BINDING_AUDIO_SOURCE_PTON].Format(_T("Event Short: dest0 - Select Source"));
	m_AutoAutoStrings[AUTO_BINDING_AUDIO_DESTOFF_PTOFF].Format(_T("Event Short: dest0 - Set Off"));
	m_AutoAutoStrings[AUTO_BINDING_AUDIO_SOURCE_INC].Format(_T("Event Short: dest0 - Source Inc"));
}

CBindingAutoTemplate_AudioExt::~CBindingAutoTemplate_AudioExt()
{
	for(int i=0;i<AUTO_BINDING_AUDIO_MAX;++i)
	{
		m_AutoAutoStrings[i].Empty();
	}
}

int CBindingAutoTemplate_AudioExt::ProcessTemplate(void)
{
	const int nrOfInputs=(m_pNrUnitInfo->nControlUnits+m_pNrUnitInfo->virtualUnit.nrOfInputs);
	int nAutoBindings = 0;	

	ASSERT(m_pNrUnitInfo->virtualUnit.nrOfInputs==m_pNrUnitInfo->virtualUnit.nrOfUnits);

	if (nrOfInputs==2)
	{
		// Momenteel enkel met 2 inputs...

		const AudioAutoOptions option=this->m_pAutoBindingOptions->GetAutoAudioOption();

		switch(option)
		{
			case AUDIO_AUTO_OPTION_SOURCE_SIMPLE:
			default:
			{			
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_VOLSTOP1);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_VOLSTOP2);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_VOLUP);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_VOLDOWN);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_SOURCE);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_DESTOFF);

				if (nAutoBindings==6)
				{
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_SOURCE]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_DESTOFF]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLUP]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLDOWN]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLSTOP1]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLSTOP2]);
				} /* if (nAutoBindings==5) */

			} break;

			case AUDIO_AUTO_OPTION_SOURCE_INC:
			{
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_VOLSTOP1);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_VOLSTOP2);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_VOLUP);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_VOLDOWN);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_SOURCE_PTON);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_DESTOFF_PTOFF);
				nAutoBindings+=ProcessBinding(AUTO_BINDING_AUDIO_SOURCE_INC);

				if (nAutoBindings==7)
				{
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_SOURCE_PTON]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_DESTOFF_PTOFF]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_SOURCE_INC]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLUP]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLDOWN]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLSTOP1]);
					m_pAutoBindingInfo->AddMessage(m_AutoAutoStrings[AUTO_BINDING_AUDIO_VOLSTOP2]);
				} /* if (nAutoBindings==5) */

			} break;
		}
	} /* if (nrOfInputs==2) */
	return nAutoBindings;
}

// Source
BOOL CBindingAutoTemplate_AudioExt::ProcessBindingShort_Input1(CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLPULS;
	const BOOL fStateInverse=FALSE;

	m_AudioUnitProp.SetFunction(AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON);
	m_AudioUnitProp.SetDest(0);
	m_AudioUnitProp.SetSource(0);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO);

	bNrOutputLinks = m_pNrUnitInfo->nAudioUnitsExtended;

	AddLinks(pEntry,1,bNrOutputLinks);
	return TRUE;
}

// Off
BOOL CBindingAutoTemplate_AudioExt::ProcessBindingShort_Input2(CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLPULS;
	const BOOL fStateInverse=FALSE;

	m_AudioUnitProp.SetFunction(AUDIO_MATRIX_OBJECT_METHOD_OFF);
	m_AudioUnitProp.SetDest(0);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,1)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,1))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO);

	bNrOutputLinks = m_pNrUnitInfo->nAudioUnitsExtended;

	AddLinks(pEntry,1,bNrOutputLinks);
	return TRUE;
}

// Vol Up
BOOL CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input1(CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLTOGGLE;
	const BOOL fStateInverse=FALSE;

	m_AudioUnitProp.SetFunction(AUDIO_MATRIX_OBJECT_METHOD_VOLUP);
	m_AudioUnitProp.SetDest(0);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO);

	bNrOutputLinks = m_pNrUnitInfo->nAudioUnitsExtended;

	AddLinks(pEntry,1,bNrOutputLinks);
	return TRUE;
}

// Vol down
BOOL CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input2(CBindingEntry *pEntry)		
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLTOGGLE;
	const BOOL fStateInverse=FALSE;

	m_AudioUnitProp.SetFunction(AUDIO_MATRIX_OBJECT_METHOD_VOLDOWN);
	m_AudioUnitProp.SetDest(0);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,1)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,1))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO);

	bNrOutputLinks = m_pNrUnitInfo->nAudioUnitsExtended;

	AddLinks(pEntry,1,bNrOutputLinks);
	return TRUE;
}

// Vol Stop
BOOL CBindingAutoTemplate_AudioExt::ProcessBindingLong_InvInput1(CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLTOGGLE;
	const BOOL fStateInverse=TRUE;

	m_AudioUnitProp.SetFunction(AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP);
	m_AudioUnitProp.SetDest(0);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO);

	bNrOutputLinks = m_pNrUnitInfo->nAudioUnitsExtended;

	AddLinks(pEntry,1,bNrOutputLinks);
	return TRUE;
}

// Vol Stop
BOOL CBindingAutoTemplate_AudioExt::ProcessBindingLong_InvInput2(CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLTOGGLE;
	const BOOL fStateInverse=TRUE;

	m_AudioUnitProp.SetFunction(AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP);
	m_AudioUnitProp.SetDest(0);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,1)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,1))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO);

	bNrOutputLinks = m_pNrUnitInfo->nAudioUnitsExtended;

	AddLinks(pEntry,1,bNrOutputLinks);
	return TRUE;
}


// DK0 - Source 1 On
BOOL CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input1_PTON(
	CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLPULSTOGGLE;
	const BOOL fStateInverse=FALSE;

	m_AudioUnitProp.SetFunction(AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON);
	m_AudioUnitProp.SetDest(0);
	m_AudioUnitProp.SetSource(0);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO);

	bNrOutputLinks = m_pNrUnitInfo->nAudioUnitsExtended;

	AddLinks(pEntry,1,bNrOutputLinks);
	return TRUE;
}

// DK0 - Dest
BOOL CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input1_PTOFF(
	CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLPULSTOGGLE;
	const BOOL fStateInverse=TRUE;

	m_AudioUnitProp.SetFunction(AUDIO_MATRIX_OBJECT_METHOD_OFF);
	m_AudioUnitProp.SetDest(0);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,0)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,0))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO);

	bNrOutputLinks = m_pNrUnitInfo->nAudioUnitsExtended;

	AddLinks(pEntry,1,bNrOutputLinks);
	return TRUE;
}

// DK1 Source Inc
BOOL CBindingAutoTemplate_AudioExt::ProcessBindingLong_Input2_SRCINC(
	CBindingEntry *pEntry)
{
	BYTE bNrOutputLinks;
	const BYTE bEvent=EV_UNITCONTROLPULS;
	const BOOL fStateInverse=FALSE;

	m_AudioUnitProp.SetFunction(AUDIO_MATRIX_OBJECT_METHOD_SOURCEINC);
	m_AudioUnitProp.SetDest(0);

	m_ControlUnitProp.SetEvent(bEvent);
	m_ControlUnitProp.SetStateInverse(fStateInverse);
	m_VirtualUnitProp.SetEvent(bEvent);
	m_VirtualUnitProp.SetStateInverse(fStateInverse);

	if ((CopyUnit(pEntry,BINDING_UNIT_TYPE_CONTROL,1)+
		 CopyUnit(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL,1))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO))==0)
	{
		return FALSE;
	}

	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_CONTROL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_LCD_VIRTUAL);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4);
	AddUnitProperties(pEntry,BINDING_UNIT_TYPE_AUDIO_HC4_BNO);

	bNrOutputLinks = m_pNrUnitInfo->nAudioUnitsExtended;

	AddLinks(pEntry,1,bNrOutputLinks);
	return TRUE;
}


/*********************************************************************/

CBindingAutoTemplate_DimmerFWD::CBindingAutoTemplate_DimmerFWD(CBindingConversionAuto *pParent) :
CBindingAutoTemplate(pParent) 
{
	m_dimmerFwdAutoBindingActions[AUTO_BINDING_DIMMERFWD_SINGLE]= (Action)&CBindingAutoTemplate_DimmerFWD::ProcessBinding_Input_Single;
	m_dimmerFwdAutoBindingActions[AUTO_BINDING_DIMMERFWD_MULTIPLE]= (Action)&CBindingAutoTemplate_DimmerFWD::ProcessBinding_Input_Multiple;

	m_dimmerFwdAutoStrings[AUTO_BINDING_DIMMERFWD_SINGLE].Format(_T("Switch Relais on/off - f(status dimmer)"));
	m_dimmerFwdAutoStrings[AUTO_BINDING_DIMMERFWD_MULTIPLE].Format(_T("Switch Relais on/off - f(status dimmers)"));
}

CBindingAutoTemplate_DimmerFWD::~CBindingAutoTemplate_DimmerFWD()
{
	for(int i=0;i<AUTO_BINDING_DIMMERFWD_MAX;++i)
	{
		m_dimmerFwdAutoStrings[i].Empty();
	}
}

int CBindingAutoTemplate_DimmerFWD::ProcessTemplate(void)
{
	const int nrOfInputs=m_pNrUnitInfo->dimUnit.nrOfInputs;
	int nAutoBindings=0;

	ASSERT((m_pNrUnitInfo->nSwitchUnits==1)&&
		   (m_pNrUnitInfo->dimUnit.nrOfInputs==m_pNrUnitInfo->dimUnit.nrOfUnits));

	if (nrOfInputs==1)
	{
		nAutoBindings+=ProcessBinding(AUTO_BINDING_DIMMERFWD_SINGLE);

		if (1==nAutoBindings)
		{
			m_pAutoBindingInfo->AddMessage(m_dimmerFwdAutoStrings[AUTO_BINDING_DIMMERFWD_SINGLE]);	
		}
	}
	else
	{
		nAutoBindings+=ProcessBinding(AUTO_BINDING_DIMMERFWD_MULTIPLE);

		if (2==nAutoBindings)
		{
			m_pAutoBindingInfo->AddMessage(m_dimmerFwdAutoStrings[AUTO_BINDING_DIMMERFWD_MULTIPLE]);	
		}	
	}

	return nAutoBindings;
}

BOOL CBindingAutoTemplate_DimmerFWD::ProcessBinding_Input_Single(CBindingEntry *pEntry)
{
	ASSERT((m_pNrUnitInfo->nSwitchUnits==1)&&
		   (m_pNrUnitInfo->dimUnit.nrOfInputs==1)&&
		   (m_pNrUnitInfo->dimUnit.nrOfInputs==m_pNrUnitInfo->dimUnit.nrOfUnits));

	m_DimmerUnitProp.SetEvent(EV_UNITCONTROLPULSTOGGLE);	
	m_DimmerUnitProp.SetFunction(0xFF);					

	// zie BUG-0174
	// Om de toestand (zwarte/rode pijl) te bepalen wordt de toestand van de 'm_ControlUnitProp' gebruikt.
	m_ControlUnitProp.SetStateInverse(FALSE);

	m_SwitchUnitProp.SetFunction(SWITCH_OBJECT_METHOD_FSONOFF);

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM_FWD))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD))==0)
	{
		return FALSE;
	}

	// Binding vlaggen:
	pEntry->SetBindingReturnFlags(BINDINGFLAGS_STATUS_POST);

	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_DIM,
		BINDING_UNIT_TYPE_DIM_FWD,
		BINDING_UNIT_TYPE_SWITCH,
		BINDING_UNIT_TYPE_SWITCH_FWD
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));

	AddLinks(pEntry,1,1);

	return (TRUE);
}

BOOL CBindingAutoTemplate_DimmerFWD::ProcessBinding_Input_Multiple(
	CBindingEntry *pEntry)
{
	ASSERT((m_pNrUnitInfo->nSwitchUnits==1)&&
		   (m_pNrUnitInfo->dimUnit.nrOfInputs>1)&&
		   (m_pNrUnitInfo->dimUnit.nrOfInputs==m_pNrUnitInfo->dimUnit.nrOfUnits));

	m_DimmerUnitProp.SetEvent(EV_UNITCONTROLPULSTOGGLE);
	m_DimmerUnitProp.SetFunction(0xFF);			

	// zie BUG-0174
	// Om de toestand (zwarte/rode pijl) te bepalen wordt de toestand van de 'm_ControlUnitProp' gebruikt.
	m_ControlUnitProp.SetStateInverse(FALSE);


	m_SwitchUnitProp.SetFunction(SWITCH_OBJECT_METHOD_FSONOFF);

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_DIM_FWD))==0)
	{
		return FALSE;
	}

	if ((CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH)+
		 CopyUnits(pEntry,BINDING_UNIT_TYPE_SWITCH_FWD))==0)
	{
		return FALSE;
	}

	// AddUnit properties.
	const BYTE pbUnitTypes[] = 
	{
		BINDING_UNIT_TYPE_DIM,
		BINDING_UNIT_TYPE_DIM_FWD,
		BINDING_UNIT_TYPE_SWITCH,
		BINDING_UNIT_TYPE_SWITCH_FWD
	};
	AddUnitProperties(pEntry,pbUnitTypes,sizeof(pbUnitTypes));

	// Operator
	m_InputOperator.SetUsed(TRUE);
	m_InputOperator.SetType(PM_OPERATOR_OR);		
	AddOperator(m_InputOperator.GetType(),pEntry);	

	AddLinks(pEntry,m_pNrUnitInfo->dimUnit.nrOfInputs,m_pNrUnitInfo->nSwitchUnits);

	return (TRUE);
}



