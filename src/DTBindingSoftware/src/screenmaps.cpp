// NodeLcdScreenmaps.cpp: implementation of the CNodeLcdScreenmaps class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "screenmaps.h"
#include "screenmaptypes.h"

//--
#include "NodeDatabase.h"
#include "MainFrm.h"			// to use createsubdirectory ...
#include "NodeDatabaseOperations.h"

#include "utils/win32/ShellFunctions.h"	
#include "utils/win32/NotifyError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/******************************************************************************/

BOOL CScreenmapUtil::Copy(
	IScreenmap *pDest,
	const IScreenmap* pSrc)
{
	if (pDest && pSrc)
	{
		if (pDest->getClassID( ) == CScreenmap::CLASS_ID &&
			pSrc->getClassID( ) == CScreenmap::CLASS_ID)
		{
			CScreenmap* const pDestScreenmap = static_cast<CScreenmap*>(pDest);
			const CScreenmap* const pSrcScreenmap = static_cast<const CScreenmap*>(pSrc);

			if (pDestScreenmap && pSrcScreenmap)
			{
				*pDestScreenmap = *pSrcScreenmap;
				return TRUE;
			}
		}
		else if (pDest->getClassID( ) == CScreenmapMain::CLASS_ID &&
				 pSrc->getClassID( ) == CScreenmapMain::CLASS_ID)
		{
			CScreenmapMain* const pDestScreenmap = static_cast<CScreenmapMain*>(pDest);
			const CScreenmapMain* const pSrcScreenmap = static_cast<const CScreenmapMain*>(pSrc);

			if (pDestScreenmap && pSrcScreenmap)
			{
				*pDestScreenmap = *pSrcScreenmap;
				return TRUE;
			}
		}		
	}

	return FALSE;
};

BOOL CScreenmapUtil::ConvertStringToArray(
	IScreenmap* pScreenmap,
	CString szBuffer,
	int iLength,
	CDWordArray *ptrArray)
{
	CString szCompare;
	int nStart = 0;
	int iLineCnt = 0;

	ASSERT(ptrArray);
	ptrArray->RemoveAll();
	szCompare.Format("%c%c",0x0d,0x0a);

// parse full string and store all nodeAddress,Unitaddress + linenr ...
	while(1) 
	{	
		int nStop = szBuffer.Find(szCompare,nStart);
	
		if (nStop == -1) 
		{
			break;
		}

		CString szText( szBuffer.Mid(nStart,nStop-nStart) );

		szText.MakeLower();

		//TRACE1("tekst : %s\n",szText);
			
		int nodeAddress = -1;
		int unitAddress = -1;
	
		if ((sscanf(szText,"%x;%x;",&nodeAddress,&unitAddress) == 2) &&			// CHANGES_MVS_2008_SSCANF
			(nodeAddress >= 0 && nodeAddress <= 0xFF) &&
			(unitAddress >= 0 && unitAddress <= 0xFF))
		{				
			const DWORD dWord = ((DWORD) iLineCnt << 16) +
								 ((DWORD) (nodeAddress << 8)) + 
								  unitAddress;
				
			ptrArray->Add(dWord);				
		}
			
		nStart = nStop + szCompare.GetLength();
		iLineCnt++;
	}
	return TRUE;
}

BOOL CScreenmapUtil::CheckArrayForErrors(
	IScreenmap* pScreenmap,
	CDWordArray *ptrArray,
	CString *szRet)
{	
	BOOL fResult = TRUE;
	
	ASSERT(ptrArray);

	for (int i=0;i<ptrArray->GetSize();i++) 
	{
		const DWORD dWord = ptrArray->GetAt(i);
		const int iLineCnt = (unsigned int) (dWord >> 16);

		const BYTE bNodeAddress = (BYTE) ((dWord & 0xFF00) >>8);
		const BYTE bUnitAddress = (BYTE) (dWord & 0xFF);

		if (bNodeAddress == 0xFF)
		{
			// Indien bNodeAddress == 0xFF 
			// is bUnitAddress een referentie naar een virtuele audio room.
		}
		else
		{
			if (pScreenmap->CheckUnitType(bNodeAddress,bUnitAddress) == FALSE) 
			{
				CString szError;
				szError.Format(_T("Linenr %d, nodeAddress : 0x%02x, UnitAddress : 0x%02x"),
								iLineCnt,
								bNodeAddress,
								bUnitAddress);

				*szRet += szError;
				*szRet += "\n";

				fResult = FALSE;
			}
		}
	}
	return fResult;
}

BOOL CScreenmapUtil::CheckStringForErrors(
	IScreenmap* pScreenmap,
	CString szBuffer,
	int iLength,
	CString *szRet)
{
	CDWordArray ArrayDWords;

	ASSERT(szRet);
	szRet->Empty();

	if (ConvertStringToArray(pScreenmap, szBuffer,iLength,&ArrayDWords)) 
	{
		if (CheckArrayForErrors(pScreenmap, &ArrayDWords,szRet) == TRUE) 
		{		
			return TRUE;
		}
		
		return FALSE;
	}
	szRet->Format("Error : Unable to convertStringToArray");

	return FALSE;
}


BOOL CScreenmapUtil::ConvertStringToStructs(
	IScreenmap* pScreenmap,
	CString szBuffer,
	int iLength,
	CString *szRet)
{
	CDWordArray ArrayDWords;

	ASSERT(szRet);
	szRet->Empty();

	if (ConvertStringToArray(pScreenmap, szBuffer,iLength,&ArrayDWords)) 
	{
		if (CheckArrayForErrors(pScreenmap, &ArrayDWords,szRet) == FALSE) 
		{
			CorrectArray(pScreenmap, &ArrayDWords);
		}
		
			// continue here ...
		if (ConvertArrayToStructs(pScreenmap, &ArrayDWords) != TRUE) 
		{
			szRet->Format("Error : ConvertArrayToStructs Failed");
			return FALSE;
		}
			
		return TRUE;

	}
	szRet->Format("Error : Unable to convertStringToArray");

	return FALSE;
}


BOOL CScreenmapUtil::CorrectArray(IScreenmap* pScreenmap,CDWordArray *ptrArray)
{
	const int arraySize = ptrArray->GetSize();
	if (arraySize > 0) 
	{		
		int i = 0;
		while(1) 
		{		
			// Solved BUG-0163 - Sedert V0F06
			// Stoppen met de loop wanneer index groter is dan de upperbound.
			// Vroeger werd de laatste niet verwijderd wanneer deze niet toegelaten was.
			const int arrayUpperBound = ptrArray->GetUpperBound(); 
			if (i > arrayUpperBound) 
				break;

			ASSERT(i<ptrArray->GetSize());
			ASSERT(i<=ptrArray->GetUpperBound());
			
			const DWORD dWord = ptrArray->GetAt(i);
			const BYTE bNodeAddress = (BYTE) ((dWord & 0xFF00) >>8);
			const BYTE bUnitAddress = (BYTE) (dWord & 0xFF);

			if (bNodeAddress == 0xFF)
			{
				// Indien bNodeAddress == 0xFF 
				// is bUnitAddress een referentie naar een virtuele audio room.

				// Solved BUG-0162 - Sedert V0F06
				// Moeten de pointer verhogen blijft anders in de while loop hangen...
				i++;
			}
			else
			{
				if (pScreenmap->CheckUnitType(bNodeAddress,bUnitAddress) == FALSE) {
					ptrArray->RemoveAt(i);
					i = 0;
				}
				else {
					i++;	
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CScreenmapUtil::NormalizeString(CString* pStr)
{
	if (pStr != 0 && (!pStr->IsEmpty()))
	{
		// controleren of de string afgesloten is met 0x0d en 0x0a.
		// indien niet het geval moet deze toegevoegd worden aan het einde.

		CString szEndOfString;
		szEndOfString.Format("%c%c",0x0d,0x0a);

		const int startIndex = pStr->GetLength()-2; 
		if (startIndex > 0)
		{
			if (pStr->Find(szEndOfString, startIndex) == -1)
			{
				*pStr += szEndOfString;
			}		
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CScreenmapUtil::ConvertArrayToStructs(
	IScreenmap* pScreenmap,
	CDWordArray *ptrArray)
{
	pScreenmap->Initialize( );

	if (ptrArray->GetSize() == 0)	/* BUG-0107 en BUG-0108: GetSize ipv GetSize() */
	{
		return TRUE;
	}
	else 
	{				
		const int size = ptrArray->GetSize();
		if (pScreenmap->createEntries(size))
		{		
			const int maxButtonsOnPage = pScreenmap->GetButtonsPerPage( );
			ASSERT( maxButtonsOnPage > 0); 	// wijst erop dat niet goed geinitialiseerd is --> divide by zero
		
			for (int i=0;i<size;i++) 
			{
				CScreenmap::ENTRY& entry = pScreenmap->getEntry( i );

				const DWORD dWord = ptrArray->GetAt(i);
				const unsigned int lineCnt = (unsigned int) (dWord >> 16);

				entry.bPageNr = (BYTE) (lineCnt / maxButtonsOnPage);
				entry.bButtonNr = (BYTE) (lineCnt % maxButtonsOnPage);
				entry.bNodeAddress = (BYTE) ((dWord & 0xFF00) >>8);
				entry.bUnitAddress = (BYTE) (dWord & 0xFF);
			}
			return TRUE;			
		}
	}
	return FALSE;
}

void CScreenmapUtil::format(const CScreenmapID& ref,CString* pStr) 
{
	const int type = ref.get();
	switch(type) {
		case SCREENMAP_DIMMER: {
			*pStr = CString("dimmers");							   			
		} break;
		case SCREENMAP_SWITCH: {
			*pStr = CString("devices");							   			
		} break;
		case SCREENMAP_CONTROLS: {
			*pStr = CString("inputs");				 	
		} break;
		case SCREENMAP_SENS: {
			*pStr = CString("sensors");			 		
		} break;
		case SCREENMAP_DUOSWITCH: {
			*pStr = CString("motors");			
		} break;
		case SCREENMAP_AUDIO: {
			*pStr = CString("audio");			  		
		} break;
		case SCREENMAP_V65_MAIN_MOODS: {
			*pStr = CString("main moods");					   		
		} break;
		case SCREENMAP_V65_MAIN_SENSOR: {
			*pStr = CString("main sensor");										
		} break;
		case SCREENMAP_V65_MAIN_AUDIO: {
			*pStr = CString("main audio");					   		
		} break;
		case SCREENMAP_V65_MYHOME: {
			*pStr = CString("my home");				   			
		} break;
		case SCREENMAP_V65_MYHOME_SENS: {
			*pStr = CString("my home sensor");				
		} break;
#if (USE_SMAP_ENERGYMEASURE == 1)
		case SCREENMAP_ENERGYMEASURE: {
			*pStr = CString("energy measure");					  			
		} break;
#endif 
		default: {
			pStr->Format("screenmap id=%d",type);
		} break;
	}
}

/******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IScreenmap::~IScreenmap( )
{
	// pure virtuele constructor
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CScreenmap::CScreenmap(CNodeDatabase* pNodeDatabase, const CScreenmapID& id) :
m_ScreenmapID( id ),
m_pNodeDatabase( pNodeDatabase ),
m_ulScreenmapFlags( 0 ),
m_nMaxButtonsOnPage( 0 ),	
m_Screenmap( 0 ),
m_nNrOfEntries( 0 ),
m_fModified( FALSE ),
m_StartPage( 0 )
{
	ASSERT( m_pNodeDatabase );			
}

CScreenmap::~CScreenmap( )
{
	if (m_Screenmap != NULL) 
	{		
		delete [ ] m_Screenmap;		// new 0x0571 !
		m_Screenmap = NULL;
	}
}

void CScreenmap::Initialize()
{
	m_nNrOfEntries = 0;
	m_fModified = FALSE;
	
	if (m_Screenmap != NULL) 
	{		
		delete [ ] m_Screenmap;		// new 0x0571 !
		m_Screenmap = NULL;
	}
}

const CScreenmap& CScreenmap::operator=(const CScreenmap& item)
{
	if (this != &item)
	{
		this->Initialize( );

		this->m_Screenmap = new CScreenmap::ENTRY[item.m_nNrOfEntries];
		if (this->m_Screenmap)
		{
			this->m_nNrOfEntries = item.m_nNrOfEntries;		
			this->m_nMaxButtonsOnPage = item.m_nMaxButtonsOnPage;	
			this->m_ulScreenmapFlags = item.m_ulScreenmapFlags;
			this->m_StartPage = item.m_StartPage;		

			for (int i=0;i<item.m_nNrOfEntries;i++)
			{
				memcpy(&this->m_Screenmap[i], &item.m_Screenmap[i], sizeof(CScreenmap::ENTRY));			
			}							
			this->SetModified(TRUE);				 			
		}
	}

	return *this;
}


void CScreenmap::FillStandard(CCanNode* pThisNode)
{	
	CCanNode *pNode = 0;
	STRUCT_UNIT *pUnit = 0;
	
	if (m_Screenmap != NULL) 
	{
		Initialize();
	}

	// Get Nr Of Entries ...
	m_nNrOfEntries = 0;
	
	int nNodes = 0;
	m_pNodeDatabase->NROfNodes(&nNodes);

	for(int i=0;i<nNodes;i++) 
	{
		if (m_pNodeDatabase->GetNode(i,&pNode)) 
		{
			const int nUnits = pNode->bNROfUnits;

			for (int j=0;j<nUnits;j++) 
			{
				if (m_pNodeDatabase->GetUnit(pNode,j,&pUnit)) 
				{					
					if (CheckUnitType(pUnit) == TRUE) 
					{
						m_nNrOfEntries++;					
					}
				}
			}
		}
	}

	// Allocate Memory ...
	if (m_nNrOfEntries != 0) 
	{
		m_Screenmap = new CScreenmap::ENTRY[m_nNrOfEntries];

		// Fill in Structure ...
		int nButtonNr = 0;
		int nPageNr = 0;
		int t = 0;				// CHANGES_MVS_2008

		ASSERT(m_pNodeDatabase);
		m_pNodeDatabase->NROfNodes(&nNodes);
	
		for(int i=0,t=0;i<nNodes;i++)			// CHANGES_MVS_2008
		{
			if (m_pNodeDatabase->GetNode(i,&pNode)) 
			{
				const int nUnits = pNode->bNROfUnits;

				for (int j=0;j<nUnits;j++) 
				{
					if (m_pNodeDatabase->GetUnit(pNode,
												 j,
												 &pUnit)) 
					{
						
						if (CheckUnitType(pUnit) == TRUE) 
						{
							ASSERT( t < m_nNrOfEntries );

							m_Screenmap[t].bButtonNr = nButtonNr;
							m_Screenmap[t].bPageNr = nPageNr;
							m_Screenmap[t].bNodeAddress = pNode->bLogicalAddress;
							m_Screenmap[t].bUnitAddress = pUnit->bUnitAddress;
							
							strcpy(m_Screenmap[t].szUnitName,
								   pUnit->szUnitName);
							
							if (++nButtonNr > m_nMaxButtonsOnPage-1) 
							{
								nButtonNr = 0;
								nPageNr++;							
							}
	
							t++;

						}//if (CheckUnitType(pUnit) == TRUE) 
					}//if (m_pNodeDatabase->GetUnit(pNode
				}//for (int j=0;j<nUnits;j++) 
			}// if (m_pNodeDatabase->GetNode(i,&pNode)) 
		} // for(i=0,t=0;i<nNodes;i++) 

		//ASSERT(t == m_nNrOfEntries);		// KRIJGEN HIER EEN ASSERT FAILLURE MAAR WEET NIET VAN WAAR ????

	}// if (nNrOfEntries != 0) 
}

BOOL CScreenmap::CheckUnitType(BYTE bNodeAddress,BYTE bUnitAddress)
{	
	if (bUnitAddress != 0xFF) 
	{
		int iEntryNR;
		CCanNode *pNode;

		if (m_pNodeDatabase->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode)) 
		{
			STRUCT_UNIT *pUnit;

			if (m_pNodeDatabase->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit)) 
			{			
				return (CheckUnitType(pUnit));
			}		
		}
	}
	return FALSE;
}

BOOL CScreenmap::CheckUnitType(STRUCT_UNIT *pUnit)
{
	if (pUnit->bUnitAddress == 0xFF) return FALSE;

	switch(pUnit->bUnitType)
	{
		case UNIT_TYPE_DIM :
			if ((m_ulScreenmapFlags & SCREENMAP_UNITTYPE_DIM) == SCREENMAP_UNITTYPE_DIM)
				return TRUE;
			else return FALSE;
		break;

		case UNIT_TYPE_SWITCH:
			if ((m_ulScreenmapFlags & SCREENMAP_UNITTYPE_SWITCH) == SCREENMAP_UNITTYPE_SWITCH)
				return TRUE;
			else return FALSE;
		break;

		case UNIT_TYPE_DUOSWITCH:
			if ((m_ulScreenmapFlags & SCREENMAP_UNITTYPE_DUOSWITCH) == SCREENMAP_UNITTYPE_DUOSWITCH)
				return TRUE;
			else return FALSE;
		break;

		case UNIT_TYPE_SENS:
			if ((m_ulScreenmapFlags & SCREENMAP_UNITTYPE_SENS) == SCREENMAP_UNITTYPE_SENS)
				return TRUE;
			else return FALSE;
		break;

		case UNIT_TYPE_AUDIO_HC4:		// !!!!!!!!!!!!!!!!!!!!
		case UNIT_TYPE_AUDIO:
			if ((m_ulScreenmapFlags & SCREENMAP_UNITTYPE_AUDIO) == SCREENMAP_UNITTYPE_AUDIO)
				return TRUE;
			else return FALSE;
		break;

		case UNIT_TYPE_BOSERS:
			if ((m_ulScreenmapFlags & SCREENMAP_UNITTYPE_BOSE) == SCREENMAP_UNITTYPE_BOSE)
				return TRUE;
			else return FALSE;
		break;

		case UNIT_TYPE_CONTROL:
			if ((m_ulScreenmapFlags & SCREENMAP_UNITTYPE_CONTROL) == SCREENMAP_UNITTYPE_CONTROL)
				return TRUE;
			else return FALSE;
		break;

		case UNIT_TYPE_LCD_VIRTUAL:
			if ((m_ulScreenmapFlags & SCREENMAP_UNITTYPE_VIRTUAL) == SCREENMAP_UNITTYPE_VIRTUAL)
				return TRUE;
			else return FALSE;
		break;

		default:
			return FALSE;
		break;	
	}
}


// CHANGE_SCREENMAP_LOAD
// Indien een leeg bestand ingelezen wordt:
// Vroeger:
//	- Messagebox met Foutmelding
//  - Standaard smap wordt doorgestuurd.
// Nu:
//	- Geen foutmelding
//  - Lege smap
BOOL CScreenmap::Load(CString szFilename)
{		
	FILE *fs = fopen(szFilename,"rb"); 
	if (fs != NULL) 
	{	
		enum { MAX_TEMP_BUFFER_LENGTH = 50000 };
		char szBuffer[MAX_TEMP_BUFFER_LENGTH];

		memset(szBuffer, 0, MAX_TEMP_BUFFER_LENGTH-1);
		const int nNumRead = fread(&szBuffer[0],sizeof(char),MAX_TEMP_BUFFER_LENGTH-1,fs);
		fclose(fs);		

		// Indien een leeg bestand krijgen we 0 terug...
		if (nNumRead >= 0)		
		{		
			// Convert to structs ...					
			BOOL fResult;
			CString szError;
			if ((fResult = CScreenmapUtil::CheckStringForErrors(this,szBuffer,nNumRead,&szError)) == FALSE) 			
			{				
				CString SzExtendedError;
				SzExtendedError.Format(_T("Screenmappingfile: '%s' contains Errors!\n"
									      "These entries will be discarded ...\n\n"
									      "Possible these nodes/units don't respond : \n\n%s"),
										   szFilename,
										   szError);				
				AfxMessageBox(SzExtendedError);		
			}		

			if ((fResult = CScreenmapUtil::ConvertStringToStructs(this,szBuffer,nNumRead,&szError)) == TRUE) 
			{
				return (UpdateNames());
			}
			
			return fResult;			
		}		
	}
	return FALSE;
}


#if(0)

		BOOL CScreenmap::Load(CString szFilename)
		{		
			FILE *fs = fopen(szFilename,"rb"); 
			if (fs != NULL) 
			{	
				enum { MAX_TEMP_BUFFER_LENGTH = 50000 };

				char szBuffer[MAX_TEMP_BUFFER_LENGTH];

				memset(szBuffer, 0, MAX_TEMP_BUFFER_LENGTH-1);

				int nNumRead = fread(&szBuffer[0],
									 sizeof(char),
									 MAX_TEMP_BUFFER_LENGTH-1,
									 fs);

				if (nNumRead != 0) 
				{		
					// Convert to structs ...		
					fclose(fs);		

					BOOL fResult;
					CString szError;

					if ((fResult = CScreenmapUtil::CheckStringForErrors(this,szBuffer,nNumRead,&szError)) == FALSE) 			
					{				
						CString SzExtendedError;
						SzExtendedError.Format(_T("Screenmappingfile: '%s' contains Errors!\n"
												  "These entries will be discarded ...\n\n"
												  "Possible these nodes/units don't respond : \n\n%s"),
												   szFilename,
												   szError);
						
						AfxMessageBox(SzExtendedError);		
					}		
					
					if ((fResult = CScreenmapUtil::ConvertStringToStructs(this,szBuffer,nNumRead,&szError)) == TRUE) 
					{
						return (UpdateNames());
					}
					
					return fResult;			
				}
				else 
				{
					CString szError;
					szError.Format("Error : CScreenmap::Load \nfRead returned 0 :%s",
									szFilename);
					AfxMessageBox(szError);

					fclose(fs);
					return FALSE;
				}	
			}
			else 
			{
				//szError.Format("Error : CScreenmap::Load \nUnable to open file :%s",
				//	szFilename);
				//AfxMessageBox(szError);
			}
			return FALSE;
		}

#endif 


BOOL CScreenmap::Save(CString szFilename)
{	
	CString szBuffer;
	int iStringlength;

	if (ConvertToString(&szBuffer)) 
	{	
		FILE *fd = fopen(szFilename,"wb"); 
		if (fd != NULL) 
		{		
			iStringlength = szBuffer.GetLength();

			fwrite(szBuffer.GetBuffer(iStringlength+1),
				   sizeof(char),
				   iStringlength + 1,
				   fd);

			fclose(fd);

			m_fModified = FALSE;		//new ...
			return TRUE;
		}
		else 
		{
			CString szError;
			szError.Format("Error : CScreenmap::Save \nUnable to open file :%s",
						   szFilename);
			AfxMessageBox(szError);
		}
	}
	return FALSE;
}



BOOL CScreenmap::UpdateNames(void)
{
	CNodeDatabaseOperations NodeDatabaseOperations(m_pNodeDatabase);

	for (int i=0;i<m_nNrOfEntries;i++) 
	{	
		CScreenmap::ENTRY *pCurrentEntry = NULL;
		if ((pCurrentEntry = &m_Screenmap[i]) !=NULL) 
		{
			STRUCT_UNIT *pUnit;
			if (NodeDatabaseOperations.Find(pCurrentEntry->bNodeAddress,
											pCurrentEntry->bUnitAddress,
											&pUnit) == TRUE)
			{
		
				ASSERT(pUnit);

				if (strcmp(pCurrentEntry->szUnitName,
						   pUnit->szUnitName) 
						   != 0) 
				{					
					strcpy(pCurrentEntry->szUnitName,
						   pUnit->szUnitName);
				}				
			}
			else
			{
				if (pCurrentEntry->bNodeAddress == 0xFF)
				{
					sprintf(pCurrentEntry->szUnitName,
							"Virtual Room %d",
							pCurrentEntry->bUnitAddress);
				}
			}
		}
		else 
		{
			CString szError("Error : CScreenmap::UpdateNames : pCurrentEntry points to NULL");
			AfxMessageBox(szError);
			return FALSE;
		}
	}
	return TRUE;
}

CScreenmap::ENTRY& CScreenmap::getEntry(int entryNR)
{
	return const_cast<ENTRY&>(const_cast<const CScreenmap*>(this)->getEntry(entryNR)); 
}

const CScreenmap::ENTRY& CScreenmap::getEntry(int entryNR) const
{
	static const CScreenmap::ENTRY ret_screenmapEmpty =
	{
		0xFF,
		0xFF,
		0xFF,
		0xFF,
		"0123456789ABCDE"
	};

	if (entryNR < m_nNrOfEntries)
	{
		return m_Screenmap[ entryNR ];
	}

	ASSERT(entryNR < m_nNrOfEntries);
	return ret_screenmapEmpty;	
}


BOOL CScreenmap::createEntries(int count)
{
	BOOL result = FALSE;

	Initialize();	

	if (count > 0)
	{
		try 
		{
			m_Screenmap = new CScreenmap::ENTRY[count];
			m_nNrOfEntries = count;
			result = TRUE;
		}
		
		catch(CMemoryException *e)		
		{
			e->ReportError();
			delete e;

			result = FALSE;
		}	
	}
	return result;
}

BOOL CScreenmap::ConvertToString(CString *pstr)
{
	int nNumLines;
	int nLastPage;


	CScreenmap::ENTRY *pCurrentEntry = NULL;

	if (pstr == NULL) return FALSE;

	pstr->Empty();

	if (1)
	{
		/**
		 * Fill in nLastPage ...
		 */

		nLastPage = 0;
	
		for (int i=0;i<m_nNrOfEntries;i++) 
		{		
			if ((pCurrentEntry = &m_Screenmap[i]) != NULL)
			{
				if (pCurrentEntry->bPageNr+1 > nLastPage) 
				{
					nLastPage = pCurrentEntry->bPageNr+1;
				}
			}
			ASSERT(pCurrentEntry);
		}
	}

	if (1)
	{
		/**
		 * Format string ...
		 */

		CString strEntry;
		int t;
		nNumLines = 0;

		for (int i=0,nCount=0;
			 i<nLastPage;
			 i++) 
		{	
			for (int j=0;j<m_nMaxButtonsOnPage;j++) 
			{		
				strEntry.Empty();

				for (t=0;t<m_nNrOfEntries;t++) 
				{													
					if ((pCurrentEntry = &m_Screenmap[t]) !=NULL) 
					{				
						if ((pCurrentEntry->bPageNr == i) && (pCurrentEntry->bButtonNr == j)) 
						{
							break;				
						}
					}	
				
					ASSERT(pCurrentEntry);
				}

				if (t<m_nNrOfEntries) 
				{
					nCount++;
					
					if (pCurrentEntry != NULL)
					{
						// entry found for pagenr && button nr ...										
						strEntry.Format("0x%02x;0x%02x;%s%c%c",
										pCurrentEntry->bNodeAddress,
										pCurrentEntry->bUnitAddress,
										pCurrentEntry->szUnitName,
										0x0d,
										0x0a);
					}
					
					ASSERT(pCurrentEntry);
				
				}
				else 
				{			
					if ((!((i == nLastPage-1)&&(j==m_nMaxButtonsOnPage-1))) &&
						  (nCount < m_nNrOfEntries))
					{
						strEntry.Format("%c%c",0x0D,0x0A);
					}
				}

				*pstr += strEntry;
				nNumLines++;
			}
		}
	}

	/**
	 * Check !!!
	 */
	const int nCalcNumLines = nLastPage * m_nMaxButtonsOnPage;

	if (nNumLines != nCalcNumLines) 
	{
		CString szError;
		szError.Format("Error : Numlines %d != Max calculated lines %d",
					   nNumLines,
					   nCalcNumLines);

		AfxMessageBox(szError);

		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
CScreenmapMain::CScreenmapMain(CNodeDatabase* pNodeDatabase,const CScreenmapID& id) :
CScreenmap( pNodeDatabase, id )
{			
}
CScreenmapMain::~CScreenmapMain( )
{
}

//////////////////////////////////////////////////////////////////////

void CScreenmapMain::FillStandard(CCanNode* pNode)
{	
	// Voor het moment geen standaards invullen !!!!

	// Mogen geen entries aanmaken met 0xFF.

	if (m_Screenmap != NULL) 
	{
		Initialize();
	}
	m_nNrOfEntries = 0;
}

const CScreenmapMain& CScreenmapMain::operator=(const CScreenmapMain& item)
{
	if (this != &item)
	{		
		this->CScreenmap::operator=(item);
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////
	
CScreenmap_V2::CScreenmap_V2(CNodeDatabase* pNodeDatabase,const CScreenmapID& id) :
CScreenmap( pNodeDatabase, id )
{			
}
CScreenmap_V2::~CScreenmap_V2( )
{
}

void CScreenmap_V2::FillStandard(CCanNode* pNode)
{	
	// Voor het moment geen standaards invullen !!!!
	if (m_Screenmap != NULL) 
	{
		Initialize();
	}
	m_nNrOfEntries = 0;
}

const CScreenmap_V2& CScreenmap_V2::operator=(const CScreenmap_V2& item)
{
	if (this != &item)
	{		
		this->CScreenmap::operator=(item);
	}
	return *this;
}

BOOL CScreenmap_V2::Save(CString szFilename)
{
	// Niet geimplementeerd - gebeurd door de configurator.
	return FALSE;
}


// Indien het bestand niet kan gevonden worden wordt er lege smap doorgestuurd.
// Indien er geen entries in smap aanwezig zijn idem.
// Indien er syntax fouten in het bestand zitten dan wordt een leeg bestand doorgestuurd.
// Indien er units niet correct zijn dan worden ENKEL deze units niet doorgestuurd.
BOOL CScreenmap_V2::Load(CString szFilename)
{
	BOOL result=FALSE;	
	FILE *fs = fopen(szFilename,"r+"); 
	if (fs != NULL) 
	{	
		enum { MAX_ENTRY_LENGTH = 255 };
		char szBuffer[MAX_ENTRY_LENGTH];
		CPtrArray tempArray;
		int idx = 0;
		CString szError;	
		
		result=TRUE;	
		while(!feof(fs))
		{
			while (fgets(&szBuffer[0],MAX_ENTRY_LENGTH-1,fs)!=NULL)
			{
				const char ch = szBuffer[0];
				if ((ch!=';')&&(ch!=' '))
				{
					int page=-1;
					int button=-1;
					int nodeAddress=-1;
					int unitAddress=-1;

					const int ret = sscanf(szBuffer,"%d;%d;%x;%x;",&page,&button,&nodeAddress,&unitAddress);
					if (ret==4)
					{
						if (nodeAddress<0 || nodeAddress>0xFF)
						{
							CString szTemp;
							szTemp.Format("Linenr %d,Invalid node address\n",idx);						
							szError+=szTemp;								
							result=FALSE;
							break;
						}
						else if (unitAddress<0 || unitAddress>0xFF)
						{
							CString szTemp;
							szTemp.Format("Linenr %d,Invalid unit address\n",idx);						
							szError+=szTemp;
							result=FALSE;
							break;
						}
						else if (page<0 || page>255)
						{
							CString szTemp;
							szTemp.Format("Linenr %d,Invalid page/room number\n",idx);						
							szError+=szTemp;
							result=FALSE;
							break;
						}
						else
						{											
							if (nodeAddress == 0xFF)
							{
								// Indien bNodeAddress == 0xFF 
								// is bUnitAddress een referentie naar een virtuele audio room.

								// Sedert V0x0E08:
								// Virtuele audio kamers toevoegen aan de MyHome...

								CScreenmap::ENTRY* pNew = create(page,button,nodeAddress,unitAddress);
								if (0!=pNew)
								{
									tempArray.Add(pNew);
									idx++;
								}							
							}
							else
							{
								if (TRUE==CheckUnitType(nodeAddress,unitAddress)) 
								{																	
									CScreenmap::ENTRY* pNew = create(page,button,nodeAddress,unitAddress);
									if (0!=pNew)
									{
										tempArray.Add(pNew);
										idx++;
									}	
								}
								else
								{
									// Geen Fout.
									// Wordt niet toegevoegd aan het array...
									CString szTemp;
									szTemp.Format("Linenr %d,NodeAddress=0x%02x,UnitAddress=0x%02x\n",idx,nodeAddress,unitAddress);						
									szError+=szTemp;
								}
							}
						}					
					}					
				}
			}
		}

		if (!szError.IsEmpty())
		{
			CString SzExtendedError;
			SzExtendedError.Format(_T("Screenmappingfile: '%s' contains Errors!\n"
								      "These entries will be discarded ...\n\n"
								      "Possible these nodes/units don't respond:\n%s"),
									   szFilename,
									   szError);				
			AfxMessageBox(SzExtendedError);	
		}		

		if (TRUE==result)
		{
			const int size = tempArray.GetSize();
			if (size>0)
			{
				int i;
			
				ASSERT(0 == this->m_Screenmap);
				createEntries(size);
				ASSERT(0 != this->m_Screenmap);

				// Copieren van de items in tempArray naar screenmap struct.
				for(i=0;i<size;++i)
				{
					CScreenmap::ENTRY* const p = 
						reinterpret_cast<CScreenmap::ENTRY*>(tempArray.GetAt(i));
					if (p)
					{
						CScreenmap::ENTRY& entry = getEntry(i);
						entry.bPageNr = p->bPageNr;
						entry.bButtonNr = p->bButtonNr;					
						entry.bNodeAddress = p->bNodeAddress;
						entry.bUnitAddress = p->bUnitAddress;
					}				
				}						

				// Remove items string array...
				for(i=0;i<size;++i)
				{
					CScreenmap::ENTRY* p = 
						reinterpret_cast<CScreenmap::ENTRY*>(tempArray.GetAt(i));
					if (p)
					{
						delete p;
						p=0;
					}
				}				
				tempArray.RemoveAll();
			}
		}
		fclose(fs);
	}
	else 
	{
		// Absorbe.
		// Er wordt een foutmelding gegeven door de FSM.
	}
	return result;
}

// Sedert 0x0E08
CScreenmap::ENTRY* CScreenmap_V2::create(int pageNR,int buttonNR,int nodeAddress,int unitAddress)
{
	CScreenmap::ENTRY* pNew = 0;
	if ((nodeAddress>=0 && nodeAddress<=0xFF) && 
		(unitAddress>=0 && unitAddress<=0xFF) &&
		(pageNR>=0 && pageNR<=0xFF) &&
		(buttonNR>=0 && buttonNR<=0xFF))
	{	
		pNew = new CScreenmap::ENTRY;
		if (0!=pNew)
		{
			pNew->bPageNr = static_cast<BYTE>(pageNR);
			pNew->bButtonNr = static_cast<BYTE>(buttonNR);
			pNew->bNodeAddress = static_cast<BYTE>(nodeAddress);
			pNew->bUnitAddress = static_cast<BYTE>(unitAddress);	
		}
	}
	return pNew;
}




