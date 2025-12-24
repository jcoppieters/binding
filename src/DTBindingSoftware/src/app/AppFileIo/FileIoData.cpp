// FileIoData.cpp: implementation of the CFileIoData class.
//
// TM_CHANGES_ALARM_ARITECH:
// bAlarmConfig laden en opslaan voor een alarm unit.
//
// Changes TM20120402: CBindingPropertySensUnit via Set( ) en Get().
// Changes TM20120405: CBindingPropertySensUnit MeasuredOffsetVal
// Changes TM20160315: CBindingPropertySensUnit: Principe van sensor unit delegation.
// Changes TM20160317: ref naar CCanNode meegegeven bij inladen en opslaan
//					   van de unit properties.
// Changes TM20160523: Inladen/opslaan van de dimmer Address Cfg data.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"

#include "FileIoData.h"
#include "appfileioactliccode.h"

#include "cannode.h"
#include "nodedatabase.h"
#include "app\appnodedbase\appnodedbase.h"

#include "BindingEntries.h"

#include "BindingUnitFactory.h"
#include "BindingOperatorFactory.h"
#include "BindingLinkFactory.h"
#include "BindingEntriesOperations.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyFactory.h"

// ---------------------------------------
//#include "TMGlobals.h"
//using namespace TMNodeDatabase;
// ---------------------------------------
#include <afx.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
static unsigned short file_read_sw_version = 0xFFFF;
static char szDefaultString[3] = {"TM"};
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFileIoData::CFileIoData(
		CNodeDatabase *pNodeDatabase,
		CBindingEntries *pBindings,
		CAppLicActCode* pAppLicActCode) :
m_pNodeDatabase(pNodeDatabase),
m_pBindingsDatabase(pBindings),
m_pAppLicActCode(pAppLicActCode),
sw_version(CURRENT_SW_VERSION)
{	
}

CFileIoData::~CFileIoData()
{

}

////////////////////////////////////////////////////////////////////

BOOL CFileIoData::Save(CFile *fp)
{
	ASSERT(fp);
	
	try 
	{	
		// Opslaan van de default string.
		fp->Write(szDefaultString, sizeof(szDefaultString));

		// Opslaan van de huidige software versie.
		fp->Write(&sw_version, sizeof(sw_version));

		// Opslaan van de node database
		NodesDatabase.Save(m_pNodeDatabase, fp);

		// opslaan van de bindings.
		BindingEntries.Save(m_pBindingsDatabase, fp);

		// CHANGES_ACTIVATION_LICENSE_CODE: Opslaan van de licentie activatie code.
		CAppFileIoActLicCode::Save(fp,*m_pAppLicActCode);
	}

	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}		
	return TRUE;
}

BOOL CFileIoData::Load(CFile *fp)
{
	ASSERT(fp);

	unsigned short usTemp;
	char szTemp[3];

	try 
	{
		// Lezen van de default string...
		fp->Read(szTemp,sizeof(szDefaultString));
			
		if (strcmp(szTemp,szDefaultString) != 0)
		{
			const CString szError(_T("Error cannot load the project file.\n"
									 "Possible the file is corrupt or a wrong project file was selected.\n"));
			AfxMessageBox(szError);
			return FALSE;
		}

		// Lezen van de software versie.
		fp->Read(&usTemp,sizeof(sw_version));
		file_read_sw_version = usTemp;
		
		const BYTE bReadSwVersion = (BYTE) ((file_read_sw_version & 0xFF00) >> 8);
		const BYTE bCurrSwVersion = (BYTE) ((sw_version & 0xFF00) >> 8);
					
		// V17.00: Aanpassing melding wanneer het binding configuratie bestand niet kan ingeladen worden.
		if (bReadSwVersion > bCurrSwVersion) {			
			CString szError;
			szError.Format(_T("Unable to load the configuration file.\r\n"						  							
							  "This configuration file has been saved with a newer software version V%02d.%02d\r\n"
							  "To continue update your binding software to this newer software version."),							  
							   (file_read_sw_version&0xFF00)>>8,
							   (file_read_sw_version&0xFF));

			AfxMessageBox(szError);
			return FALSE;
		}		
		
		// Laden van de node database
		NodesDatabase.Load(m_pNodeDatabase,fp);

		// Laden van de binding entries.
		BindingEntries.Load(m_pBindingsDatabase,fp);

		// CHANGES_ACTIVATION_LICENSE_CODE: Inladen van de licentie activatie code.
		// Serializable klassen: Voorlopig nog op deze manier inlezen...
		if ((file_read_sw_version&0xFF00)>=0x0800) 
		{
			CAppFileIoActLicCode::Load(fp,m_pAppLicActCode);
		}
	}
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}		
	return TRUE;
}

//////////////////////BINDINGS DATABASE /////////////////////////////

// TM 20081223: Gebruik van Iterators.
BOOL CFileIo_BindingEntries::Save(CBindingEntries *pEntries,CFile* fp)
{
	const unsigned short nEntries = pEntries->Count();

	try 
	{
		// Schrijven van het aantal binding entries.
		fp->Write(&nEntries,sizeof(unsigned short));
		
		if (nEntries != 0) 
		{
			// Alle binding entries opslaan.
			CBindingEntries::Iterator it(pEntries->CreateIterator());
			for (it.Begin();!it.IsDone();it.Next())
			{
				ASSERT( it.CurrentItem() );
				BindingEntry.Save( it.CurrentItem(), fp );
			}		
		}
	}
	
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	
	return TRUE;
}

BOOL CFileIo_BindingEntries::Load(CBindingEntries *pEntries,CFile* fp)
{
	unsigned short nEntries=0;	
	CBindingEntry EntryRead;	

	try 
	{		
		// Lezen van het aantal binding entries.
		fp->Read(&nEntries,sizeof(unsigned short));

		if (nEntries != 0) 
		{			
			// Alle binding entries inlezen.
			for (int i=0;i<nEntries;i++) 
			{
				// BUG-0136:
				// CBindingEntry::Clear( ) --> m_nID member wordt niet geinitialiseerd.
				// Daarom hier setID(0) aanroepen.
				EntryRead.SetID(0);
				EntryRead.Clear();
				BindingEntry.Load(&EntryRead,fp);	

				// Ingeladen binding entry toevoegen aan de database.
				CBindingEntriesOperations BindingEntriesOperations(pEntries);
				const unsigned short nID = BindingEntriesOperations.Add(&EntryRead,EntryRead.GetBindingNr());							
			}		
		}
	}
	
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------
BOOL CFileIo_BindingEntry2::Save(CBindingEntry *pEntry,CFile* fp)
{
	int i;
	char *szName=NULL,*szEvalString = NULL;
	BYTE bNameLength,bEvalLength;
	CString szTemp;
	BYTE bTemp;
	unsigned short usTemp;
	unsigned long ulTemp;

	CBindingOperator *pBindingOperator=0;
	CBindingLink *pBindingLink = 0;		// CHANGES_MVS_2008

	CBindingUnits* const pBindingUnits= pEntry->GetColUnit();
	CBindingOperators* const pBindingOperators = pEntry->GetColOperator();
	CBindingLinks* const pBindingLinks = pEntry->GetColLink();
	

	ASSERT(pBindingUnits);
	ASSERT(pBindingOperators);
	ASSERT(pBindingLinks);

	// Copy Name
	szTemp = pEntry->GetName();
	bNameLength = strlen((LPCTSTR) szTemp);		// CHANGES_MVS_2008_CSTRING
	szName = new char[bNameLength+1];
	strcpy(szName,(LPCTSTR) szTemp);			// CHANGES_MVS_2008_CSTRING

	//Copy EvalString
	szTemp = pEntry->GetEvaluationString()->Get();
	bEvalLength = szTemp.GetLength();
	bEvalLength = strlen((LPCTSTR) szTemp);		// CHANGES_MVS_2008_CSTRING
	szEvalString = new char[bEvalLength+1];
	strcpy(szEvalString,(LPCTSTR) szTemp);		// CHANGES_MVS_2008_CSTRING

	try 
	{
		fp->Write(&bNameLength,sizeof(BYTE));			// NAMELENGTH + NAME
		fp->Write(szName,bNameLength * sizeof(BYTE));

		fp->Write(&bEvalLength,sizeof(BYTE));			// LENGTH + EVALSTRING 
		fp->Write(szEvalString,bEvalLength * sizeof(BYTE));
	
		bTemp = pEntry->GetBindingReturnFlags();			// BINDINGFLAGS
		fp->Write(&bTemp,sizeof(BYTE));
		usTemp = pEntry->GetBindingNr();				// BINDINGNR
		fp->Write(&usTemp,sizeof(unsigned short));

		bTemp = (pEntry->IsSend()) & 0x01;						// IsSend Flag 
		fp->Write(&bTemp,sizeof(BYTE));

		// New SW VERSION 0x0200
		ulTemp = pEntry->GetApplicationFlags();
		fp->Write(&ulTemp,sizeof(unsigned long));

		// Save Binding Units ...
		usTemp = pBindingUnits->Count();
		fp->Write(&usTemp,sizeof(unsigned short));

		if (1)	// Save Binding Units
		{
			CBindingUnits::Iterator it(pBindingUnits->CreateIterator());
			for (it.Begin();!it.IsDone();it.Next())
			{
				ASSERT( it.CurrentItem() );
				BindingUnit.Save( it.CurrentItem(), fp);
			}
		}
	
		// Save Binding Operators ...
		usTemp = pBindingOperators->Count();
		fp->Write(&usTemp,sizeof(unsigned short));
		
		/** @todo Via iterators */
		for (i=0;i<usTemp;i++) 
		{			
			if ((pBindingOperator = pBindingOperators->Get(i)) != 0)
			{
				BindingOperator.Save(pBindingOperator,fp);
			}

			ASSERT(pBindingOperator);
		}
		// Save Binding Links ...
		usTemp = pBindingLinks->Count();
		fp->Write(&usTemp,sizeof(unsigned short));
		
		/** @todo Via iterators */
		for (i=0;i<usTemp;i++) 
		{		
			if ((pBindingLink = pBindingLinks->Get(i)) != 0)
			{
				BindingLink.Save(pBindingLink,fp);
			}

			ASSERT(pBindingLink);
		}

	}
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();

		if (szName != NULL) 
			delete [] szName;
		if (szEvalString != NULL) 
			delete [] szEvalString;

		return FALSE;
	}

	if (szName != NULL) 
		delete [] szName;

	if (szEvalString != NULL) 
		delete [] szEvalString;

	return TRUE;
}

BOOL CFileIo_BindingEntry2::Load(CBindingEntry *pEntry,CFile* fp)
{
	int i;
	char *szName=NULL,*szEvalString = NULL;
	BYTE bNameLength,bEvalLength;
	CString szTemp,szError;
	BYTE bTemp;
	unsigned short usTemp;
	//unsigned short nId;
	TimerType typeoftimer;
	unsigned long ulTemp;
	
	CBindingUnit BindingUnitRead,*pBindingUnit;
	CBindingOperator BindingOperatorRead;//*pBindingOperator;
	CBindingLink BindingLinkRead;

	CBindingUnits* const pBindingUnits = pEntry->GetColUnit();
	CBindingOperators* const pBindingOperators = pEntry->GetColOperator();
	CBindingLinks* const pBindingLinks = pEntry->GetColLink();

	ASSERT(pBindingUnits);
	ASSERT(pBindingOperators);
	ASSERT(pBindingLinks);

	try 
	{		
		// Binding Name ...
		fp->Read(&bNameLength,sizeof(BYTE));
		szName = new char[bNameLength+1];
		ASSERT(szName);	
		fp->Read(szName,bNameLength*sizeof(BYTE));
		szName[bNameLength] = '\0';	
		szTemp.Format(_T("%s"),szName);
		pEntry->SetName(szTemp);

		// Eval string ...
		fp->Read(&bEvalLength,sizeof(BYTE));
		szEvalString = new char[bEvalLength+1];
		ASSERT(szEvalString);
		fp->Read(szEvalString,bEvalLength*sizeof(BYTE));
		szEvalString[bEvalLength] = '\0';	
		szTemp.Format(_T("%s"),szEvalString);
		//pEntry->SetEvalString(szTemp);
		pEntry->GetEvaluationString()->Set(szTemp);
	
		fp->Read(&bTemp,sizeof(BYTE));				//BindingfLags
		pEntry->SetBindingReturnFlags(bTemp);	
		fp->Read(&usTemp,sizeof(unsigned short));	//Binding Nr
		pEntry->SetBindingNr(usTemp);

		fp->Read(&bTemp,sizeof(BYTE));				//Is Send Flag
		pEntry->SetSend((BOOL) (bTemp&0x01));

		if ( (file_read_sw_version & 0xFF00) >= 0x0200)
		{			
			fp->Read(&ulTemp,sizeof(unsigned long));
			pEntry->SetApplicationFlags(ulTemp);
		}

		// BindingUnits ...
		fp->Read(&usTemp,sizeof(unsigned short));	// Nr of Units ...

		for (i=0;i<usTemp;i++) 
		{
			BindingUnit.Load(&BindingUnitRead,fp);

			/**
			 * BindingUnitRead = Base class CBindingUnit --> pNew = Derived class
			 */
			CBindingUnit* const pNew = CBindingUnitFactory::Clone(&BindingUnitRead);
			if (pNew != NULL)
			{
				ASSERT(pNew->GetID() != 0);
				ASSERT(pNew->GetID() == BindingUnitRead.GetID());

				const unsigned short nIDResult = pBindingUnits->Add(pNew);
				
				ASSERT(nIDResult);
				ASSERT(nIDResult == pNew->GetID());

				if (nIDResult != NULL) 
				{			
					// MUST FORMAT BINDINGDATA --> UNITSPECIFIC DATA ...			
					pBindingUnit = pBindingUnits->Get(i);	//use PolyMorphisme !!!

					ASSERT(pBindingUnit);
					ASSERT(pBindingUnit->GetID() == pNew->GetID());

					// TM 20040701: 		
					// in Function CBindingUnits::_Add is basic type copied to
					// inherited type therefore all initialisation is lost in Constructor ...
					// Therefore CBindingXXXUnit::ConvertFromBindingData explicitly invokes
					// Initialisation function ...

					if (pBindingUnit->ConvertFromBindingData() == FALSE) 
					{
						// TM 20110512:
						// Deze foutmelding kan getoond worden wanneer er een onbekend binding unit type
						// moet ingeladen worden.						
						// zie ook NOTE-0100

						szError.Format("Binding '%s', Binding nr=0x%04x:\n"
									   "Unable To Parse Data of unit '%s',NodeAddress=0x%02x, UnitAddress= 0x%02x.",
									    pEntry->GetName(),
										pEntry->GetBindingNr(),
										pBindingUnit->GetName(),
										pBindingUnit->GetNodeAddress(),
										pBindingUnit->GetUnitAddress());

						AfxMessageBox(szError);
					}
				}
				else 
				{
					szError.Format("Unable To Add BindingUnit to BindingUnits Collection in Binding %s",pEntry->GetName());
					AfxMessageBox(szError);
				}
			}
		}

		// BindingOperators ..
		// TM 20040706 BUG : Serialization of TimerOperator Not OK !
		// SOLVED IN NEW SW VERSION = 0x0300
		fp->Read(&usTemp,sizeof(unsigned short));	// Nr of Operators ...
		
		for(i=0;i<usTemp;i++) 
		{
			BindingOperator.Load(&BindingOperatorRead,fp);

			CBindingOperator* const pNew = CBindingOperatorFactory::Clone(&BindingOperatorRead);
			ASSERT(pNew);		

			if (pNew != NULL)
			{
				ASSERT(pNew->GetID() == BindingOperatorRead.GetID());

				const unsigned short nID = pBindingOperators->Add(pNew);
				ASSERT(nID);

				if (nID != 0)
				{
					ASSERT(nID == pNew->GetID());											
			
					// Operator specific --> TimerType + Value ?
					fp->Read(&typeoftimer,sizeof(typeoftimer));
					fp->Read(&bTemp,sizeof(BYTE));
				
					CBindingOperator* pBindingOperator = NULL;

					if ((pBindingOperator = pBindingOperators->Get(i)) != 0)
					{							
						ASSERT(pBindingOperator == pNew);

						// if OperatorType == PM_OPERATOR_TIMER ...			
						if (pBindingOperator->GetType() == PM_OPERATOR_TIMER) 
						{
							((CBindingTimerOperator*)pBindingOperator)->SetTimerType(typeoftimer);
							((CBindingTimerOperator*)pBindingOperator)->SetTimeValue(bTemp);
						}		
					}				
				}
			}
		}

		// BindingLinks ...
		fp->Read(&usTemp,sizeof(unsigned short));
		for (i=0;i<usTemp;i++) 
		{
			BindingLink.Load(&BindingLinkRead,fp);

			CBindingLink* const pNew = CBindingLinkFactory::Clone(&BindingLinkRead);
			ASSERT(pNew);
			
			if (pNew != NULL)
			{
				ASSERT(pNew->GetID() != NULL);

				const unsigned short nID = pBindingLinks->Add( pNew );

				ASSERT(nID);
				ASSERT(nID == pNew->GetID());
				ASSERT(nID == BindingLinkRead.GetID());
			}		
		}
	}
	
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();

		if (szName != NULL) 
		{
			delete [] szName;
		}
		if (szEvalString != NULL) 
		{
			delete [] szEvalString;
		}
		
		return FALSE;
	}
	
	if (szName != NULL) 
	{
		delete [] szName;
	}

	if (szEvalString != NULL) 
	{
		delete [] szEvalString;
	}

	return TRUE;
}



BOOL CFileIo_BindingUnit2::Save(CBindingUnit *pUnit,CFile* fp)
{	
	BYTE bTemp;
	unsigned short usTemp;
	unsigned long ulTemp;

	CString szTemp(pUnit->GetName());
	const BYTE bLength = strlen((LPCTSTR) szTemp);	// CHANGES_MVS_2008_CSTRING
	char* szName = new char[bLength+1];

	strcpy(szName,(LPCTSTR) szTemp);				// CHANGES_MVS_2008_CSTRING

	try 
	{
		usTemp = pUnit->GetID();					//nId
		fp->Write(&usTemp,sizeof(unsigned short));
		fp->Write(&bLength,sizeof(BYTE));			//NameLength
		fp->Write(szName,bLength*sizeof(BYTE));		//Name

		bTemp = pUnit->GetNodeAddress();			//NodeAddress
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->GetUnitAddress();			//UnitAddress
		fp->Write(&bTemp,sizeof(BYTE));	

		bTemp = pUnit->GetBindingUnitType();		//UnitType
		fp->Write(&bTemp,sizeof(BYTE));			

		bTemp = pUnit->GetInOutType();				//in-Out Type
		fp->Write(&bTemp,sizeof(BYTE));

		// Vanaf software versie 0x0200
		ulTemp = pUnit->GetApplicationFlags();
		fp->Write(&ulTemp,sizeof(unsigned long));		

		// BindingData 
		BindingUnitData.Save(pUnit,fp);
	}
	
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();

		if (szName != NULL) 
			delete [] szName;
			
		return FALSE;
	}

	if (szName != NULL) 
		delete [] szName;
		
	return TRUE;
}

BOOL CFileIo_BindingUnit2::Load(CBindingUnit *pUnit,CFile* fp)
{
	unsigned short usTemp;
	BYTE bTemp;
	CString szTemp;
	char *szName;
	unsigned long ulTemp;

	try 
	{
		fp->Read(&usTemp,sizeof(unsigned short));	//nID
		pUnit->SetID(usTemp);
		
		fp->Read(&bTemp,sizeof(BYTE));				//Unit Name ...
		szName = new char[bTemp+1];
		ASSERT(szName);	
		fp->Read(szName,bTemp*sizeof(BYTE));
		szName[bTemp] = '\0';
		szTemp.Format(_T("%s"),szName);
		pUnit->SetName(szTemp);

		fp->Read(&bTemp,sizeof(BYTE));				// NodeAddress
		pUnit->SetNodeAddress(bTemp);
		fp->Read(&bTemp,sizeof(BYTE));				// UnitAddress
		pUnit->SetUnitAddress(bTemp);
		fp->Read(&bTemp,sizeof(BYTE));				// UnitType
		pUnit->SetBindingUnitType(bTemp);
		fp->Read(&bTemp,sizeof(BYTE));				// In-Out Type
		pUnit->SetInOutType(bTemp);

		// Vanaf software versie 0x0200 : Applicatie vlaggen.
		if ((file_read_sw_version & 0xFF00) >= 0x0200) 
		{
			fp->Read(&ulTemp,sizeof(unsigned long));
			pUnit->SetApplicationFlags(ulTemp);
		}
		
		// BindingData
		BindingUnitData.Load(pUnit,fp);
	}

	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();

		if (szName != NULL) 
			delete [] szName;
			
		return FALSE;
	}
	
	if (szName != NULL) 
		delete [] szName;
		
	return TRUE;
}

//----------------------------------------------------------------------

BOOL CFileIo_BindingUnitData::Save(CBindingUnit *pUnit,CFile* fp)
{
	ASSERT(pUnit);
	BYTE bTemp;

	try 
	{
		bTemp = pUnit->BindingData.Get_MessageCode();	//bMessageCode
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->BindingData.Get_Length();		//bLength
		fp->Write(&bTemp,sizeof(BYTE));

		for (int i=0;i<6;i++) 
		{							//Data[0] .. [6]
			bTemp = pUnit->BindingData.Get_Data(i);
			fp->Write(&bTemp,sizeof(BYTE));
		}

		bTemp = pUnit->BindingData.Get_BindingFlags();	//BindingFlags
		fp->Write(&bTemp,sizeof(BYTE));
	}

	catch (CFileException *e) 
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}

BOOL CFileIo_BindingUnitData::Load(CBindingUnit *pUnit,CFile* fp)
{
	BYTE bTemp;

	try
	{
		fp->Read(&bTemp,sizeof(BYTE));					//bMessageCode
		pUnit->BindingData.Set_MessageCode(bTemp);

		fp->Read(&bTemp,sizeof(BYTE));					//bLength
		pUnit->BindingData.Set_Length(bTemp);
		
		for (int i=0;i<6;i++) 
		{
			fp->Read(&bTemp,sizeof(BYTE));				//Data[0] ... [6]
			pUnit->BindingData.Set_Data(i,bTemp);
		}

		fp->Read(&bTemp,sizeof(BYTE));					//BindingFlags
		pUnit->BindingData.Set_BindingFlags(bTemp);	
	}

	catch (CFileException *e) {	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}


//----------------------------------------------------------------------
BOOL CFileIo_BindingOperator2::Save(CBindingOperator *pOperator,CFile* fp)
{
	unsigned short usTemp;
	unsigned long ulTemp;
	BYTE bTemp;
	OperatorType type;
	TimerType typeoftimer;

	try 
	{	
		usTemp = pOperator->GetID();
		fp->Write(&usTemp,sizeof(unsigned short));
		type = pOperator->GetType();
		fp->Write(&type,sizeof(OperatorType));

		// TM 20040607 : Bug 
		// NEW SW_VERSION 0x0300
		ulTemp = pOperator->GetApplicationFlags();
		fp->Write(&ulTemp,sizeof(unsigned long));


		// If OperatorType == PM_OPERATOR_TIMER ...
		if(type == PM_OPERATOR_TIMER) 
		{
			typeoftimer = ((CBindingTimerOperator*)pOperator)->GetTimerType();		
			bTemp = ((CBindingTimerOperator*)pOperator)->GetTimeValue();	
		}
		else 
		{
			typeoftimer = PM_TIMER_NONE;
			bTemp = 0;
		}
		fp->Write(&typeoftimer,sizeof(typeoftimer));
		fp->Write(&bTemp,sizeof(BYTE));

		// NEW SW_VERSION 0x0200
		// TM 20040607 : Bug 
		//ulTemp = pOperator->GetApplicationFlags();
		//fp->Write(&ulTemp,sizeof(unsigned long));
	}

	catch (CFileException *e) {	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}

BOOL CFileIo_BindingOperator2::Load(CBindingOperator *pOperator,CFile* fp)
{
	unsigned short usTemp;
	unsigned long ulTemp;
	OperatorType type;


	try 
	{
		fp->Read(&usTemp,sizeof(unsigned short));
		pOperator->SetID(usTemp);	
		fp->Read(&type,sizeof(OperatorType));
		pOperator->SetType(type);

		if ( (file_read_sw_version & 0xFF00) >= 0x0200) 
		{
			fp->Read(&ulTemp,sizeof(unsigned long));
			pOperator->SetApplicationFlags(ulTemp);
		}
	}
	catch (CFileException *e) 
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}

BOOL CFileIo_BindingLink2::Save(CBindingLink *pLink,CFile* fp)
{
	unsigned short usTemp;
	unsigned long ulTemp;
	BYTE bTemp;

	try 
	{	
		usTemp = pLink->GetID();					// nID
		fp->Write(&usTemp,sizeof(unsigned short));
		
		bTemp = (BYTE) pLink->isInverted();			// Inverse					
		fp->Write(&bTemp,sizeof(BYTE));
		
		usTemp = pLink->GetInLinkID();				// InLink Id
		fp->Write(&usTemp,sizeof(unsigned short));
		
		bTemp = (BYTE) pLink->GetInLinkType();		// Inlink Type
		fp->Write(&bTemp,sizeof(BYTE));
		
		usTemp = pLink->GetOutLinkID();				// outLink Id
		fp->Write(&usTemp,sizeof(unsigned short));
		
		bTemp = (BYTE) pLink->GetOutLinkType();		// Outlink Type
		fp->Write(&bTemp,sizeof(BYTE));
		
		// NEW SW_VERSION 0x0200
		ulTemp = pLink->GetApplicationFlags();
		fp->Write(&ulTemp,sizeof(unsigned long));
	}

	catch (CFileException *e) 
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}


BOOL CFileIo_BindingLink2::Load(CBindingLink *pLink,CFile* fp)
{
	unsigned short usTemp;
	unsigned long ulTemp;
	BYTE bTemp;

	try 
	{
		fp->Read(&usTemp,sizeof(unsigned short));		// nID
		pLink->SetID(usTemp);
		
		fp->Read(&bTemp,sizeof(BYTE));					// Inverse
		pLink->SetInverse((BOOL) bTemp);
		
		fp->Read(&usTemp,sizeof(unsigned short));
		fp->Read(&bTemp,sizeof(BYTE));
		pLink->SetInLink(usTemp,(ControlType) bTemp);	// InLink
		
		fp->Read(&usTemp,sizeof(unsigned short));
		fp->Read(&bTemp,sizeof(BYTE));
		pLink->SetOutLink(usTemp,(ControlType) bTemp);	// OutLink

		if ((file_read_sw_version & 0xFF00) >= 0x0200) 
		{
			fp->Read(&ulTemp,sizeof(unsigned long));
			pLink->SetApplicationFlags(ulTemp);
		}
	}

	catch (CFileException *e) 
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}


//////////////////////NODE DATABASE /////////////////////////////////

BOOL CFileIo_PropertyUnits::Save(STRUCT_UNIT *pUnit,CFile* fp)
{
	switch(pUnit->bUnitType) 
	{
		case UNIT_TYPE_DIM:
		{
			if(!DimUnitProperties.Save(dynamic_cast<CBindingPropertyDimUnit*>(pUnit->m_pBindingUnit),fp)) 
				return FALSE;
		} break;
		case UNIT_TYPE_SENS:
		{
			if(!SensUnitProperties.Save(dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit),fp)) 
				return FALSE;
		} break;
		case UNIT_TYPE_SWITCH:
		{
			if(!SwitchUnitProperties.Save(dynamic_cast<CBindingPropertySwitchUnit*>(pUnit->m_pBindingUnit),fp)) 
				return FALSE;
		} break;
		case UNIT_TYPE_DUOSWITCH:
		{
			if(!DuoSwitchUnitProperties.Save(dynamic_cast<CBindingPropertyDuoSwitchUnit*>(pUnit->m_pBindingUnit),fp)) 
				return FALSE;
		} break;
		case UNIT_TYPE_ALARM:
		{
			if(!AlarmUnitProperties.Save(dynamic_cast<CBindingPropertyAlarmUnit*>(pUnit->m_pBindingUnit),fp)) 
				return FALSE;
		} break;
		case UNIT_TYPE_AVMATRIX:
		{
			if(!AVMatrixUnitProperties.Save(dynamic_cast<CBindingPropertyAVMatrixUnit*>(pUnit->m_pBindingUnit),fp)) 
				return FALSE;
		} break;
		default: { /* absorbe */ } break;
	}
	return TRUE;
}

BOOL CFileIo_PropertyUnits::Load(const CCanNode* pNode,STRUCT_UNIT *pUnit,CFile* fp)
{
	ASSERT(pUnit);
	ASSERT(fp);

	CBindingPropertyUnit *pReturnUnit = 0;
	CString szError;

	switch(pUnit->bUnitType)
	{
		case UNIT_TYPE_DIM:
		{
			if (DimUnitProperties.Load(pNode,pUnit,&pReturnUnit,fp)==FALSE) {
				pUnit->m_pBindingUnit = NULL;
				return FALSE;
			}			
		} break;		
		case UNIT_TYPE_SENS:
		{
			if (SensUnitProperties.Load(pNode,pUnit,&pReturnUnit,fp)==FALSE) {
				pUnit->m_pBindingUnit = NULL;
				return FALSE;
			}				
		} break;
		case UNIT_TYPE_SWITCH:
		{
			if (SwitchUnitProperties.Load(pNode,pUnit,&pReturnUnit,fp) == FALSE){
				pUnit->m_pBindingUnit = NULL;
				return FALSE;
			}	
		} break;
		case UNIT_TYPE_DUOSWITCH:
		{
			if (DuoSwitchUnitProperties.Load(pNode,pUnit,&pReturnUnit,fp) == FALSE) {
				pUnit->m_pBindingUnit = NULL;
				return FALSE;
			}	
		} break;
		case UNIT_TYPE_ALARM:
		{			
			if ((file_read_sw_version & 0xFF00) >= 0x0400) {				
				// Vanaf software versie 0x0400
				if (AlarmUnitProperties.Load(pNode,pUnit,&pReturnUnit,fp) == FALSE) {
					pUnit->m_pBindingUnit = NULL;
					return FALSE;
				}	
			}
			else {			
				pUnit->m_pBindingUnit = NULL;	
				return FALSE;
			}
		} break;
		case UNIT_TYPE_AVMATRIX:
		{
			// Sedert versie 0x0F00: Indien bestand ingeladen wordt van een vorige versie dan wordt een
			// standaard binding unit aangemaakt - subtype=0.
			if (AVMatrixUnitProperties.Load(pNode,pUnit,&pReturnUnit,fp) == FALSE) {
				pUnit->m_pBindingUnit = NULL;
				return FALSE;
			}				
		} break;
		default:
			pUnit->m_pBindingUnit = NULL;
		return FALSE;
	}
	
	// TM20120402:
	// Node en Unit address worden meegeven in de constructor.
	// Assert failures plaatsen om dit te testen...

	ASSERT( pReturnUnit );
	ASSERT( pReturnUnit->GetNodeAddress() == pUnit->bNodeLogicalAddress &&
			pReturnUnit->GetUnitAddress() == pUnit->bUnitAddress);

	pUnit->m_pBindingUnit = pReturnUnit;
	return TRUE;
}

//************************* CFileIo_DimmerProperties ******************************

BOOL CFileIo_DimmerProperties::Save(CBindingPropertyDimUnit *pUnit,CFile* fp)
{
	BYTE bTemp;

	REQUIRE(NULL != pUnit);
	if (NULL == pUnit) return FALSE;

	try 
	{	
		bTemp = pUnit->get_SolidState();	//SolidState
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->get_DelayedOff();	//Delayed Off
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->get_DelayedOn();		//Delayed On
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->get_DimSpeedDown();	//SpeedDown
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->get_DimSpeedUp();	//SpeedUp
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->get_DimMinRange();	//MinRange
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->get_DimMaxRange();	//MaxRange
		fp->Write(&bTemp,sizeof(BYTE));			

		// Sedert versie 0x0B00
		bTemp = pUnit->get_AddressCfgType();		// type.
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->get_AddressCfgMSB();	// nodeaddress
		fp->Write(&bTemp,sizeof(BYTE));

		bTemp = pUnit->get_AddressCfgLSB();	// unitaddress
		fp->Write(&bTemp,sizeof(BYTE));
	}
	
	catch (CFileException *e) 
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}

BOOL CFileIo_DimmerProperties::Load(
	const CCanNode* pNode,
	const STRUCT_UNIT* const pUnit,
	CBindingPropertyUnit **pBindingUnit,
	CFile* fp)
{
	ASSERT(pUnit);
	ASSERT(fp);

	BYTE bTemp;

	CBindingPropertyDimUnit* const pNewUnit = 
		static_cast<CBindingPropertyDimUnit*>
			(CBindingPropertyFactory::Create(pNode,pUnit));

	if (pNewUnit != NULL)
	{
		try 
		{	
			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->set_SolidState(bTemp&0x01);	//SolidState

			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->set_DelayedOff(bTemp&0x01);	//Delayed Off

			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->set_DelayedOn(bTemp&0x01);	//Delayed On

			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->set_DimSpeedDown(bTemp);		//SpeedDown

			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->set_DimSpeedUp(bTemp);		//SpeedUp

			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->set_DimMinRange(bTemp);		//MinRange

			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->set_DimMaxRange(bTemp);		//MaxRange

			// Sedert versie 0x0B00
			if ((file_read_sw_version & 0xFF00) >= 0x0B00) 
			{
				fp->Read(&bTemp,sizeof(BYTE));
				pNewUnit->set_AddressCfgType(bTemp);

				fp->Read(&bTemp,sizeof(BYTE));
				pNewUnit->set_AddressCfgMSB(bTemp);

				fp->Read(&bTemp,sizeof(BYTE));
				pNewUnit->set_AddressCfgLSB(bTemp);
			}
		}
		catch (CFileException *e) 
		{	
			e->ReportError();
			e->Delete();

			delete pNewUnit;
			*pBindingUnit = NULL;

			return FALSE;
		}

		*pBindingUnit = pNewUnit; 
		return TRUE;
	}

	*pBindingUnit = NULL;
	return FALSE;
}


//************************* CFileIo_SwitchProperties ******************************

BOOL CFileIo_SwitchProperties::Save(CBindingPropertySwitchUnit *pUnit,CFile* fp)
{
	BYTE bTemp;

	REQUIRE(NULL != pUnit);
	if (NULL == pUnit) return FALSE;

	try 
	{
		bTemp = pUnit->fTimeEnabled;
		fp->Write(&bTemp,sizeof(BYTE));					//fTimeEnabled

		fp->Write(&pUnit->bTimeValue,sizeof(BYTE));		//bTimeValue
	}
	
	catch (CFileException *e) 
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}

BOOL CFileIo_SwitchProperties::Load(
	const CCanNode* pNode,
	const STRUCT_UNIT* const pUnit,
	CBindingPropertyUnit **pBindingUnit,
	CFile* fp)
{
	ASSERT(pUnit);
	ASSERT(fp);

	BYTE bTemp;

	CBindingPropertySwitchUnit *const pNewUnit = 
		static_cast<CBindingPropertySwitchUnit*>
			(CBindingPropertyFactory::Create(pNode,pUnit));

	if (pNewUnit != NULL)
	{
		try 
		{
			fp->Read(&bTemp,sizeof(BYTE));					//fTimeEnabled
			pNewUnit->fTimeEnabled = bTemp&0x01;

			fp->Read(&bTemp,sizeof(BYTE));					//bTimeValue
			pNewUnit->bTimeValue = bTemp;
		}
	
		catch (CFileException *e) 
		{	
			e->ReportError();
			e->Delete();

			*pBindingUnit = NULL;
			return FALSE;
		}

		*pBindingUnit = pNewUnit;
		return TRUE;
	}

	*pBindingUnit = NULL;
	return FALSE;
}

//************************* CFileIo_DuoSwitchProperties ******************************

BOOL CFileIo_DuoSwitchProperties::Save(CBindingPropertyDuoSwitchUnit *pUnit,CFile* fp)
{
	BYTE bTemp=0xFF;

	REQUIRE(NULL != pUnit);
	if (NULL == pUnit) return FALSE;

	try
	{	
		bTemp = pUnit->getStopTime();
		fp->Write(&bTemp,sizeof(BYTE));		//bStopTime

		bTemp = pUnit->getSwitchTime();
		fp->Write(&bTemp,sizeof(BYTE));		//bSwitchTime
	}
	
	catch (CFileException *e) 
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	
	return TRUE;
}

BOOL CFileIo_DuoSwitchProperties::Load(
	const CCanNode* pNode,
	const STRUCT_UNIT* const pUnit,
	CBindingPropertyUnit **pBindingUnit,
	CFile* fp)
{
	ASSERT(pUnit);
	ASSERT(fp);

	BYTE bTemp;

	CBindingPropertyDuoSwitchUnit *pNewUnit = 
			static_cast<CBindingPropertyDuoSwitchUnit*>
				(CBindingPropertyFactory::Create(pNode,pUnit));

	if (pNewUnit != NULL)
	{
		try 
		{
			fp->Read(&bTemp,sizeof(BYTE));					//bStopTime
			pNewUnit->setStopTime(bTemp);

			fp->Read(&bTemp,sizeof(BYTE));					//bSwitchTime
			pNewUnit->setSwitchTime(bTemp);
		}
	
		catch (CFileException *e) 
		{	
			e->ReportError();
			e->Delete();

			delete pNewUnit;
			*pBindingUnit = NULL;
			return FALSE;
		}
		
		*pBindingUnit = pNewUnit;
		return TRUE;
	}

	*pBindingUnit = NULL;
	return FALSE;
}

//************************** CFileIo_SensProperties ******************************


BOOL CFileIo_SensProperties::Save(CBindingPropertySensUnit *pUnit,CFile* fp)
{
	BYTE bTemp;
	short sMeasuredOffsetVal;
	int temp;

	REQUIRE(NULL != pUnit);
	if (NULL == pUnit) return FALSE;

	try 
	{
		//Sample Time	
		bTemp = pUnit->GetSampleTime();					
		fp->Write(&bTemp,sizeof(BYTE));

		//Sens Time
		bTemp = pUnit->GetSensType();					
		fp->Write(&bTemp,sizeof(BYTE));

		for(int i=0;i<2;i++) {	
			for(int j=0;j<4;j++) {
				//HysVal[0][4]  + [1][4]
				bTemp = pUnit->GetHysValues(i,j);		
				fp->Write(&bTemp,sizeof(BYTE));
			}

			//HysOffsets[0] + [1] 
			bTemp = pUnit->GetHysOffsetValues(i);		
			fp->Write(&bTemp,sizeof(BYTE));
		}

		// TM20120405: Measured Offset Value opslaan.
		// Sedert Versie 0x0600.
		sMeasuredOffsetVal = pUnit->GetMeasuredValueOffset( );
		fp->Write(&sMeasuredOffsetVal,sizeof(short));		

		// TM20160315: Delegation struct opslaan.
		// Delegation - bType
		bTemp=pUnit->GetDelegationType();
		fp->Write(&bTemp,sizeof(BYTE));

		// Delegation - bNodeAddress
		bTemp=pUnit->GetDelegationNodeAddress();
		fp->Write(&bTemp,sizeof(BYTE));

		// Delegation - bUnitAddress
		bTemp=pUnit->GetDelegationUnitAddress();
		fp->Write(&bTemp,sizeof(BYTE));

		// Setpoint range: Min & Max waarden. Sedert 0x0E00
		temp = pUnit->getSetpointRangeValueMin();
		fp->Write(&temp,sizeof(int));

		temp = pUnit->getSetpointRangeValueMax();
		fp->Write(&temp,sizeof(int));	

		// Sedert V16.00: USE_SENSOR_PIDCONTROL	
		// Sedert Versie V16.00: Opslaan van de PID control properties.
		for (int mode=0;mode<SENS_PIDCONTROL_WORKINGMODE_MAX;++mode) {
			BYTE bNodeAddress = 0xff;
			BYTE bUnitAddress = 0xff;		
			if (!pUnit->getPropertyPIDControlUnit(mode,&bNodeAddress,&bUnitAddress)) {
				ASSERT(0);
			}			
			fp->Write(&bNodeAddress,sizeof(BYTE));
			fp->Write(&bUnitAddress,sizeof(BYTE));

			for (int param=0;param<SENS_PIDCONTROL_PARAM_MAX;++param) {
				bTemp = pUnit->getPropertyPIDParam(mode,param);
				fp->Write(&bTemp,sizeof(BYTE));
			}
		}
	}
	catch (CFileException *e) 
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}

BOOL CFileIo_SensProperties::Load(
	const CCanNode* pNode,
	const STRUCT_UNIT* const pUnit,
	CBindingPropertyUnit **pBindingUnit,
	CFile* fp)
{
	ASSERT(pUnit);
	ASSERT(fp);

	BYTE bTemp;
	
	CBindingPropertySensUnit* const pNewUnit = 
		static_cast<CBindingPropertySensUnit*>
			(CBindingPropertyFactory::Create(pNode,pUnit));

	ASSERT(pNewUnit);

	if (pNewUnit != NULL)
	{	
		try 
		{
			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->SetSampleTime(bTemp);

			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->SetSensType(bTemp);

			for(int i=0;i<2;i++) 
			{
				for(int j=0;j<4;j++) 
				{
					fp->Read(&bTemp,sizeof(BYTE));
					pNewUnit->SetHysValues(i,j,bTemp);
				}
			
				fp->Read(&bTemp,sizeof(BYTE));
				pNewUnit->SetHysOffsetValues(i,bTemp);
			}

			if ((file_read_sw_version & 0xFF00) >= 0x0600) 
			{
				// TM20120405: Sedert Versie 0x0600 Measured Offset Value laden.				
				short sMeasuredOffsetVal;

				fp->Read(&sMeasuredOffsetVal,sizeof(short));
				pNewUnit->SetMeasuredValueOffset(sMeasuredOffsetVal);						
			}		

			if ((file_read_sw_version & 0xFF00) >= 0x0900) 
			{
				// TM20160315: Sedert versie 0x0900 Sensor unit delegation inladen.

				// Delegation - bType
				fp->Read(&bTemp,sizeof(BYTE));
				pNewUnit->SetDelegationType(bTemp);

				// Delegation - bNodeAddress
				fp->Read(&bTemp,sizeof(BYTE));
				pNewUnit->SetDelegationNodeAddress(bTemp);

				// Delegation - bUnitAddress
				fp->Read(&bTemp,sizeof(BYTE));
				pNewUnit->SetDelegationUnitAddress(bTemp);
			}

			if ((file_read_sw_version & 0xFF00) >= 0x0E00) 
			{
				int temp;

				fp->Read(&temp,sizeof(int));
				pNewUnit->setSetpointRangeValueMin((short)temp);

				fp->Read(&temp,sizeof(int));
				pNewUnit->setSetpointRangeValueMax((short)temp);
			}

			if ((file_read_sw_version & 0xFF00) >= 0x1000) {
				
				for (int mode=0;mode<SENS_PIDCONTROL_WORKINGMODE_MAX;++mode) {

					// PID Control: Node & unit Address van de pwm output.
					BYTE bNodeAddress = 0xff;
					BYTE bUnitAddress = 0xff;						
					fp->Read(&bNodeAddress,sizeof(BYTE));
					fp->Read(&bUnitAddress,sizeof(BYTE));
					pNewUnit->setPropertyPIDControlUnit(mode,bNodeAddress,bUnitAddress);

					// PID Control: Params: kp,ki & kd.
					for (int param=0;param<SENS_PIDCONTROL_PARAM_MAX;++param) {
						fp->Read(&bTemp,sizeof(BYTE));
						pNewUnit->setPropertyPIDParam(mode,param,bTemp);
					}
				}
			} 
		}
	
		catch (CFileException *e) 
		{	
			e->ReportError();
			e->Delete();

			delete pNewUnit;
			*pBindingUnit = NULL;

			return FALSE;
		}

		*pBindingUnit = pNewUnit;
		return TRUE;
	}

	*pBindingUnit = NULL;
	return FALSE;
}

//************************** CFileIo_AlarmProperties ******************************

BOOL CFileIo_AlarmProperties::Save(CBindingPropertyAlarmUnit *pUnit,CFile* fp)
{
	BYTE bTemp;

	REQUIRE(NULL != pUnit);
	if (NULL == pUnit) return FALSE;

	try 
	{
		// Type alarm centrale
		bTemp = pUnit->GetAlarmCentraleType();			
		fp->Write(&bTemp,sizeof(BYTE));

		// Toegangscode 
		for (int i=0;i<ALARM_REMOTECODE_BUFFER_MAX;i++) 
		{			
			bTemp = pUnit->GetRemoteCode(i);			
			fp->Write(&bTemp,sizeof(BYTE));
		}
	}
	catch (CFileException *e)
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;
}

BOOL CFileIo_AlarmProperties::Load(
	const CCanNode* pNode,
	const STRUCT_UNIT* const pUnit,
	CBindingPropertyUnit **pBindingUnit,
	CFile* fp)
{
	ASSERT(pUnit);
	ASSERT(fp);

	BYTE bTemp;
	
	CBindingPropertyAlarmUnit* const pNewUnit = 
		static_cast<CBindingPropertyAlarmUnit*>
			(CBindingPropertyFactory::Create(pNode,pUnit));

	ASSERT(pNewUnit);

	if (pNewUnit != NULL)
	{
		try 
		{		
			// Type alarm centrale
			fp->Read(&bTemp,sizeof(BYTE));
			pNewUnit->SetAlarmCentraleType(bTemp);		

			// Toegangscode
			for (int i=0;i<ALARM_REMOTECODE_BUFFER_MAX;i++) 
			{						
				fp->Read(&bTemp,sizeof(BYTE));			
				pNewUnit->SetRemoteCode(i,bTemp);
			}	
		}

		catch (CFileException *e) 
		{	
			e->ReportError();
			e->Delete();

			delete pNewUnit;

			*pBindingUnit = NULL;
			return FALSE;
		}

		*pBindingUnit = pNewUnit;
		return TRUE;
	}

	*pBindingUnit = NULL;
	return FALSE;
}

//************************** CFileIo_AVMatrixProperties ******************************
// Binding property unit AVMatrix

BOOL CFileIo_AVMatrixProperties::Save(CBindingPropertyAVMatrixUnit *pUnit,CFile* fp)
{
	BYTE bTemp;

	REQUIRE(NULL != pUnit);
	if (NULL == pUnit) return FALSE;

	try 
	{
		// Type configuratie.
		bTemp = pUnit->GetSubType();			
		fp->Write(&bTemp,sizeof(BYTE));	
	}
	catch (CFileException *e)
	{	
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	return TRUE;
}

// Oppassen: Versies < 0x0F00 werd er niet opgeslagen...
BOOL CFileIo_AVMatrixProperties::Load(const CCanNode* pNode,const STRUCT_UNIT* const pUnit,CBindingPropertyUnit **pBindingUnit,CFile* fp)
{
	REQUIRE(pUnit);
	CBindingPropertyAVMatrixUnit* const pNewUnit =  
		static_cast<CBindingPropertyAVMatrixUnit*>(CBindingPropertyFactory::Create(pNode,pUnit));

	if (NULL != pNewUnit) {
		if ((file_read_sw_version & 0xFF00) >= 0x0F00) {	
			ASSERT(fp);				
			BYTE bTemp;	
			try 
			{		
				// Subtype.
				fp->Read(&bTemp,sizeof(BYTE));
				pNewUnit->SetSubType(bTemp);		
			}
			catch (CFileException *e) 
			{	
				e->ReportError();
				e->Delete();
				delete pNewUnit;
				*pBindingUnit = NULL;
				return FALSE;
			}			
		}
	}
	else {
		ASSERT(pNewUnit);
	}
	*pBindingUnit = pNewUnit;
	return TRUE;
}

// ------------------------------------------------------------------

BOOL CFileIo_UnitSpecs::Save(
	STRUCT_UNIT *pUnit,
	CFile* fp)
{
	BYTE bConfig;

	switch(pUnit->bUnitType)
	{
		case UNIT_TYPE_DIM:
			bConfig = pUnit->UnitSpecs.DimmerUnit.bDimConfig;
		break;

		case UNIT_TYPE_SWITCH:
			bConfig = pUnit->UnitSpecs.RelayUnit.bSwitchConfig;
		break;

		case UNIT_TYPE_LCD_VIRTUAL:
		case UNIT_TYPE_CONTROL:
			bConfig = pUnit->UnitSpecs.ControlUnit.bControlConfig;
		break;	

		case UNIT_TYPE_SENS:
			bConfig = pUnit->UnitSpecs.SensorUnit.bSensConfig;
		break;

		case UNIT_TYPE_AUDIO:
			bConfig = pUnit->UnitSpecs.AudioUnit.bAudioConfig;
		break;

		case UNIT_TYPE_DUOSWITCH:
			bConfig = pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchConfig;
		break;

		case UNIT_TYPE_BOSERS:
			bConfig = pUnit->UnitSpecs.AudioUnit.bAudioConfig;
		break;		

		case UNIT_TYPE_RC5RX:
			bConfig = pUnit->UnitSpecs.RC5RxUnit.bRC5RxConfig;
		break;
	
		case UNIT_TYPE_ALARM:		// TM_CHANGES_ALARM_ARITECH:
			bConfig = pUnit->UnitSpecs.AlarmUnit.bAlarmConfig;
		break;

		case UNIT_TYPE_AUDIO_HC4:
			bConfig = pUnit->UnitSpecs.AudioUnit.bAudioConfig;
		break;

		case UNIT_TYPE_AVMATRIX:	// TM_AVMATRIX_INPUT
		{
			bConfig = pUnit->UnitSpecs.AVMatrixUnit.bAVMatrixConfig;
		} break;

		default:
			bConfig = 0;
		break;		
	}

	try 
	{
		fp->Write(&bConfig,sizeof(BYTE));	
	}

	catch (CFileException *e)
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	return TRUE;

}

BOOL CFileIo_UnitSpecs::Load(
	const CCanNode* pNode,
	STRUCT_UNIT *pUnit,
	CFile* fp)
{
	BYTE bConfig = 0;

	try 
	{		
		if ((file_read_sw_version & 0xFF00) >= 0x0500) 
		{
			fp->Read(&bConfig,sizeof(BYTE));	
		}
	}
	catch (CFileException *e)
	{	
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	
	memset((void*) &pUnit->UnitSpecs,0, sizeof(UNION_UNITSPECS));	// CHANGES_MVS_2008

	switch(pUnit->bUnitType)
	{
		case UNIT_TYPE_DIM:
		{
			pUnit->UnitSpecs.DimmerUnit.bDimConfig = bConfig;

		} break;

		case UNIT_TYPE_SWITCH:
		{
			pUnit->UnitSpecs.RelayUnit.bSwitchConfig = bConfig;

		} break;

		case UNIT_TYPE_LCD_VIRTUAL:
		case UNIT_TYPE_CONTROL:
		{
			pUnit->UnitSpecs.ControlUnit.bControlConfig = bConfig;

		} break;	

		case UNIT_TYPE_SENS:
		{
			pUnit->UnitSpecs.SensorUnit.bSensConfig = bConfig;

		} break;

		case UNIT_TYPE_AUDIO:
		{
			pUnit->UnitSpecs.AudioUnit.bAudioConfig = bConfig;

		} break;

		case UNIT_TYPE_DUOSWITCH:
		{
			pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchConfig = bConfig;

		} break;

		case UNIT_TYPE_BOSERS:
		{
			pUnit->UnitSpecs.AudioUnit.bAudioConfig = bConfig;

		} break;		

		case UNIT_TYPE_RC5RX:
		{
			pUnit->UnitSpecs.RC5RxUnit.bRC5RxConfig = bConfig;

		} break;

		case UNIT_TYPE_ALARM:			// TM_CHANGES_ALARM_ARITECH:
		{
			pUnit->UnitSpecs.AlarmUnit.bAlarmConfig = bConfig;

		} break;

		case UNIT_TYPE_AUDIO_HC4:
		{
			pUnit->UnitSpecs.AudioUnit.bAudioConfig = bConfig;

		} break;

		case UNIT_TYPE_AVMATRIX:		// TM_AVMATRIX_INPUT
		{
			pUnit->UnitSpecs.AVMatrixUnit.bAVMatrixConfig = bConfig;

		} break;

		default:
		{
			bConfig = 0;

		} break;		
	}	

	return TRUE;	
}
/*****************************************************************************/
BOOL CFileIo_NodeDatabase::Save(
	CNodeDatabase *pDocDatabase,
	CFile *fp)
{
	ASSERT(pDocDatabase);
	ASSERT(fp);
	
	int iNrOfNodes;
	pDocDatabase->NROfNodes(&iNrOfNodes);
	
	// Store Nr Of Nodes ...
	fp->Write(&iNrOfNodes,sizeof(int));

	// Store Nodes and units.
	CNodeIterator it(pDocDatabase->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CCanNode* const pNode = it.CurrentItem( );

		Nodes.Save( pNode, fp );

		CUnitIterator it2(pDocDatabase->CreateUnitIterator( pNode ));
		for (it2.Begin();!it2.IsDone();it2.Next())
		{
			Units.Save( it2.CurrentItem(), fp );
		}
	}
	return TRUE;
}

BOOL CFileIo_NodeDatabase::Load(
	CNodeDatabase *pDocDatabase,
	CFile *fp)
{
	ASSERT(pDocDatabase);
	ASSERT(fp);

	// Read Nr Of Nodes 
	int iNrOfNodes;
	fp->Read(&iNrOfNodes,sizeof(int));

	if (iNrOfNodes >= 255) 
	{
		AfxMessageBox("Error : File IO Unable to read nodeDatabase from file");
		return FALSE;
	}

	int t;
	for (int i=0;i<iNrOfNodes;i++) 
	{	
		CCanNode NodeRead;		
		Nodes.Load(&NodeRead,fp);
		
		CCanNode *pNode;
		if (pDocDatabase->AddNode(NodeRead.bLogicalAddress,&t,&pNode) == FALSE) 
		{
			AfxMessageBox("Error : File IO Unable to Add Node To Database");
			return FALSE;
		}	

		pDocDatabase->CopyNodeObject(pNode,&NodeRead);
		const BYTE bNrOfUnits = pNode->bNROfUnits;
		
		for (int j=0;j<bNrOfUnits;j++) 
		{
			STRUCT_UNIT UnitRead;
			STRUCT_UNIT *pUnit=0;
			memset(&UnitRead,0,sizeof(STRUCT_UNIT));
			Units.Load(pNode,&UnitRead,fp);
			if (pDocDatabase->AddUnit(pNode,UnitRead.bUnitAddress,&t,&pUnit) == FALSE) {
				if (UnitRead.bUnitAddress == 0xFF)  {				
					if (CAppNodeDatabase::AddBroadcastUnit(pNode,&pUnit) == FALSE) {
						AfxMessageBox("Error add Virtual Unit",MB_ICONSTOP);	
						return FALSE;
					}
				}
				else 
				{
					AfxMessageBox("Error add Virtual Unit",MB_ICONSTOP);	
					return FALSE;
				}
			}
			CNodeDatabase::InitRuntimeUnitSpecs(&UnitRead);			
			CNodeDatabase::CopyUnitObject(pUnit,&UnitRead);			
		}	
	}
	return TRUE;
}


BOOL CFileIo_UnitData2::Save(
	STRUCT_UNIT *pUnit,
	CFile* fp)
{
	const BYTE bNameLength = strlen(pUnit->szUnitName);
	char* szName = new char[bNameLength+1];
	
	strcpy(szName,pUnit->szUnitName);	

	try 
	{
		fp->Write(&bNameLength,1);	
		fp->Write(szName,bNameLength);

		fp->Write(&pUnit->bNodeLogicalAddress,sizeof(pUnit->bNodeLogicalAddress));
		fp->Write(&pUnit->bUnitAddress,sizeof(pUnit->bUnitAddress));
		fp->Write(&pUnit->bUnitType,sizeof(pUnit->bUnitType));
		fp->Write(&pUnit->bUnitFlags,sizeof(pUnit->bUnitFlags));

		// NEW SW_VERSION 0x0200
		fp->Write(&pUnit->ulApplicationFlags,sizeof(unsigned long));

		UnitSpecs.Save(pUnit,fp);
		UnitBindingProperties.Save(pUnit,fp);
	}
	
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();

		if (szName != NULL) 
			delete [] szName;
			
		return FALSE;
	}		
	
	if (szName != NULL) 
		delete [] szName;
		
	return TRUE;
}


BOOL CFileIo_UnitData2::Load(
	const CCanNode* pNode,
	STRUCT_UNIT *pUnit,
	CFile* fp)
{
	BYTE bNameLength;
	char *szName = NULL;

	try 
	{
		fp->Read(&bNameLength,1);	
		szName = new char[bNameLength+1];
		if (0!=szName)
		{
			fp->Read(szName,bNameLength);
			szName[bNameLength] = '\0';	
			strcpy(pUnit->szUnitName,szName);
		}
		
		fp->Read(&pUnit->bNodeLogicalAddress,sizeof(pUnit->bNodeLogicalAddress));
		fp->Read(&pUnit->bUnitAddress,sizeof(pUnit->bUnitAddress));
		fp->Read(&pUnit->bUnitType,sizeof(pUnit->bUnitType));
		fp->Read(&pUnit->bUnitFlags,sizeof(pUnit->bUnitFlags));


		if ((file_read_sw_version&0xFF00)>=0x0200) 
		{
			fp->Read(&pUnit->ulApplicationFlags,sizeof(unsigned long));
		}

		if(szName != NULL) 
		{
			delete [] szName;
			szName = NULL;
		}

		UnitSpecs.Load(pNode,pUnit,fp);
		UnitBindingProperties.Load(pNode,pUnit,fp);
	}
	
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();

		if(szName != NULL) 
			delete [] szName;

		return FALSE;
	}		
	
	if(szName != NULL) 
		delete [] szName;

	return TRUE;
}


BOOL CFileIo_NodeData2::Save(
	CCanNode *pNode,
	CFile* fp)
{	
	BYTE bTemp = strlen(pNode->szNodeName);
	char* szName = new char[bTemp+1];

	if (0!=szName)
	{
		strcpy(szName,pNode->szNodeName);
	}

	try 
	{
		fp->Write(&bTemp,1);
		if (0!=szName)
		{
			fp->Write(szName,bTemp);
		}
		else
		{
			fp->Write(pNode->szNodeName,bTemp);
		}
		fp->Write(&pNode->ulPhysicalAddress,sizeof(pNode->ulPhysicalAddress));
		fp->Write(&pNode->bLogicalAddress,sizeof(pNode->bLogicalAddress));
		fp->Write(&pNode->bNodeType,sizeof(pNode->bNodeType));
		fp->Write(&pNode->bNodeFlags,sizeof(pNode->bNodeFlags));
		fp->Write(&pNode->bDLLAccessControl,sizeof(pNode->bDLLAccessControl));
		fp->Write(&pNode->bNROfUnits,sizeof(pNode->bNROfUnits));	
		fp->Write(&pNode->iNROfUnits,sizeof(pNode->iNROfUnits));

		// bDllAccess Control + Status Control
		fp->Write(&pNode->pbResourcesBlock0,RESOURCES_BLOCK0_LENGTH); 
		fp->Write(&pNode->pbResourcesBlock1,RESOURCES_BLOCK1_LENGTH); 
		fp->Write(&pNode->pbResourcesBlock2,RESOURCES_BLOCK2_LENGTH); 

		// Sedert v0x0700: 2 extra resource blocks opslaan.
		fp->Write(&pNode->pbResourcesBlock3,RESOURCES_BLOCK3_LENGTH); 
		fp->Write(&pNode->pbResourcesBlock4,RESOURCES_BLOCK4_LENGTH); 


		// Sedert V17.00: Uitbreiding aantal bindings.
		// Moeten het aantal bindings in een module opslaan als een unsigned short ipv een BYTE.	
		const unsigned short nrOfBindingEntries = pNode->getNodeBindingsNrEntries();		
		fp->Write(&nrOfBindingEntries,sizeof(unsigned short));

		bTemp = pNode->getNodeBindingsXORChecksum();			//XOR Checksum
		fp->Write(&bTemp,sizeof(BYTE));

		// NEW SOFTWARE VERSION 0x0200
		fp->Write(&pNode->ulApplicationFlags,sizeof(unsigned long));
	}

	catch (CFileException *e) 
	{	
		e->ReportError();
		e->Delete();

		if (szName != NULL) 
			delete [] szName;
			
		return FALSE;
	}		

	if (szName != NULL) 
		delete [] szName;
		
	return TRUE;
}

BOOL CFileIo_NodeData2::Load(CCanNode *pNode,CFile* fp)
{
	char *szName = NULL;
	BYTE bTemp;

	try 
	{
		fp->Read(&bTemp,1);
		szName = new char[bTemp+1];
		ASSERT(szName);

		fp->Read(szName,bTemp);
		szName[bTemp] = '\0';	
		strcpy(pNode->szNodeName,szName);

		fp->Read(&pNode->ulPhysicalAddress,sizeof(pNode->ulPhysicalAddress));
		fp->Read(&pNode->bLogicalAddress,sizeof(pNode->bLogicalAddress));
		fp->Read(&pNode->bNodeType,sizeof(pNode->bNodeType));
		fp->Read(&pNode->bNodeFlags,sizeof(pNode->bNodeFlags));
		fp->Read(&pNode->bDLLAccessControl,sizeof(pNode->bDLLAccessControl));
		fp->Read(&pNode->bNROfUnits,sizeof(pNode->bNROfUnits));

		fp->Read(&pNode->iNROfUnits,sizeof(pNode->iNROfUnits));

		// bDllAccess Control + Status Control
		fp->Read(&pNode->pbResourcesBlock0,RESOURCES_BLOCK0_LENGTH); 
		fp->Read(&pNode->pbResourcesBlock1,RESOURCES_BLOCK1_LENGTH); 

		// Sedert versie 0x0700:
		// Oppassen: Resource block 2 is er een extra BYTE bijgevoegd deze bepaalt het aantal
		// extra resource blocks.
		if ((file_read_sw_version & 0xFF00) >= 0x0700) 
		{
			fp->Read(&pNode->pbResourcesBlock2,RESOURCES_BLOCK2_LENGTH); 
		}
		else
		{
			fp->Read(&pNode->pbResourcesBlock2,RESOURCES_BLOCK2_LENGTH-1); 
		}

		// Sedert versie 0x0700: 2 extra resource blocks opslaan.
		if ((file_read_sw_version & 0xFF00) >= 0x0700) 
		{
			fp->Read(&pNode->pbResourcesBlock3,RESOURCES_BLOCK3_LENGTH); 
			fp->Read(&pNode->pbResourcesBlock4,RESOURCES_BLOCK4_LENGTH); 		
		}

		// Sedert V17.00: Uitbreiding aantal bindings.
		// Bestanden opgeslagen met software versie V17.00 houden het aantal binding entries bij in een unsigned short.
		if ((file_read_sw_version & 0xFF00) >= 0x1100) {
			unsigned short numberOfBindingEntries = 0;
			fp->Read(&numberOfBindingEntries,sizeof(unsigned short));
			pNode->setNodeBindingsNrEntries(numberOfBindingEntries);	
		}
		else {			
			fp->Read(&bTemp,sizeof(BYTE));
			pNode->setNodeBindingsNrEntries(bTemp);		
		}

		//XOR Checksum
		fp->Read(&bTemp,sizeof(BYTE));
		pNode->setNodeBindingsXORChecksum(bTemp);	

		if ((file_read_sw_version & 0xFF00) >= 0x0200) 
		{		
			fp->Read(&pNode->ulApplicationFlags,sizeof(unsigned long));
		}

		if (file_read_sw_version < 0x0304) 
		{
			// TM 20041115 : Set all bindingfiles to be send ...
			pNode->ulApplicationFlags |= CCanNode::NODE_BINDINGFILE_MODIFIED_FLAG;
		}

		if(szName != NULL) 
		{
			delete [] szName;
			szName = NULL;
		}
	}
	
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();

		if (szName != NULL) 
			delete [] szName;
			
		return FALSE;
	}		
	
	if (szName != NULL) 
		delete [] szName;
		
	return TRUE;
}
