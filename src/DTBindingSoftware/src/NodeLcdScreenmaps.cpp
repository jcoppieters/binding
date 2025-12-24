// NodeLcdScreenmaps.cpp: implementation of the CNodeLcdScreenmaps class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeLcdScreenmaps.h"
#include "screenmaps.h"
#include "screenmapid.h"
#include "screenmaptypes.h"
#include "NodeDatabase.h"
#include "MainFrm.h"			// to use createsubdirectory ...
#include "NodeDatabaseOperations.h"

#include "utils/win32/ShellFunctions.h"	
#include "utils/win32/NotifyError.h"


//--

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/******************************************************************************/
/**
 *	Todo:	In een aparte header bestand.
 */
struct CFactoryScreenmap {
	static IScreenmap* create(CNodeDatabase *pDatabase,CCanNode* pNode,const CScreenmapID& ID);
};


IScreenmap* CFactoryScreenmap::create(CNodeDatabase *pDatabase,CCanNode* pNode,const CScreenmapID& ID)
{
	IScreenmap* pNew = 0;
	if (ID.get() == SCREENMAP_V65_MAIN_MOODS || ID.get() == SCREENMAP_V65_MAIN_SENSOR || ID.get() == SCREENMAP_V65_MAIN_AUDIO) {
		pNew = new CScreenmapMain(pDatabase, ID);
	}
	else if (ID.get() == SCREENMAP_V65_MYHOME) {
		if (pNode->GetSoftwareVersion() == 0x66) {
			pNew = new CScreenmap_V2(pDatabase, ID);
		}
		else {
			pNew = new CScreenmapMain(pDatabase, ID);
		}
	}
	else if (ID.get() == SCREENMAP_V65_MYHOME_SENS) {
		pNew = new CScreenmapMain(pDatabase, ID);		
	}
	else {
		pNew = new CScreenmap(pDatabase, ID);
	}
	ASSERT(pNew);
	return pNew;
}
/*****************************************************************************/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNodeLcdScreenmaps::CNodeLcdScreenmaps(CNodeDatabase* pNodeDatabase,CCanNode* pNode,int maxScreenmaps,const CScreenmapID* pArrTypes) :
m_pNode( pNode ),
m_pNodeDatabase( pNodeDatabase ),
m_MaxScreenmaps( maxScreenmaps ),
m_arrScreenmaps( ),
m_parrScreenmapTypes( pArrTypes ) 
{
	// m_parrScreenmapTypes is een array van screenmap types.
	// via deze screenmap types worden de juiste screenmap klasses aangemaakt.
	// De screenmap instanties worden bijghouden in de m_arrScreenmaps

	ASSERT( pNode );
	ASSERT( pArrTypes );
	ASSERT( pNodeDatabase );

	m_arrScreenmaps.SetSize( maxScreenmaps );

	for (int i=0;i<maxScreenmaps;i++) {
		m_arrScreenmaps[i] = CFactoryScreenmap::create(m_pNodeDatabase, pNode, m_parrScreenmapTypes[i]);
		ASSERT(m_arrScreenmaps[i]); 
	}

	Initialize( );
}

CNodeLcdScreenmaps::~CNodeLcdScreenmaps()
{
	for (int i=0;i<m_MaxScreenmaps;i++) {
		if (m_arrScreenmaps[i]) {
			delete m_arrScreenmaps[i];
		}		
	}
	m_arrScreenmaps.RemoveAll( );
}

const IScreenmap* CNodeLcdScreenmaps::GetScreenmap(BYTE i) const
{
	const IScreenmap* pReturn = 0;
	if (i < m_MaxScreenmaps) {
		pReturn = m_arrScreenmaps[i];
	}
	return pReturn;
}

IScreenmap* CNodeLcdScreenmaps::GetScreenmap(BYTE i)
{ 
	return const_cast<IScreenmap*>(const_cast<const CNodeLcdScreenmaps*>(this)->GetScreenmap(i)); 
};

const IScreenmap* CNodeLcdScreenmaps::FindScreenmap(const CScreenmapID& id) const
{
	const IScreenmap* pReturn = 0;
	const int len = m_arrScreenmaps.GetSize( );
	for (int i=0;i<len;i++) {
		if ((m_arrScreenmaps[i] != 0) && (m_arrScreenmaps[i]->getScreenmapID( ).get() == id.get())) {
			pReturn = m_arrScreenmaps[i];
			break;
		}
	}
	return pReturn;
}

IScreenmap* CNodeLcdScreenmaps::FindScreenmap(const CScreenmapID& id)
{
	return const_cast<IScreenmap*>(const_cast<const CNodeLcdScreenmaps*>(this)->FindScreenmap(id));
}


void CNodeLcdScreenmaps::Initialize(void)
{
	CString szError(_T("Error : This type of Lcd Node doesn't have screenmapping"));

	for (int i=0;i<m_MaxScreenmaps;i++) {	
		ASSERT(m_arrScreenmaps[i]);
		if (m_arrScreenmaps[i]) {
			m_arrScreenmaps[i]->Initialize();			
		}
	}

	if (m_pNode != NULL) 
	{
		if (CNodeScreenmapInfo_Util::isSupported(m_pNode))
		{	
			if ((m_pNode->GetSoftwareVersion() == 0x64)||
				(m_pNode->GetSoftwareVersion() == 0x65)||
				(m_pNode->GetSoftwareVersion() == 0x66))
			{				
				int* const pNrOfButtons = new int[ m_MaxScreenmaps ];
				ASSERT( pNrOfButtons );
				
				if (pNrOfButtons)
				{
					if (CNodeScreenmapInfo_Util::getMaxControls(m_pNode,
																m_parrScreenmapTypes,
																pNrOfButtons,
																m_MaxScreenmaps))
					{
						for (int i=0;i<m_MaxScreenmaps;i++)
						{	
							ASSERT(m_arrScreenmaps[i]);
							IScreenmap* const p = FindScreenmap(CScreenmapID(m_parrScreenmapTypes[i]));
							ASSERT( p );
							if (p)
							{
								p->SetMaxButtons(pNrOfButtons[i]);
							}
						}			
					}
					else
					{
						ASSERT( 0 );
					}

					delete [] pNrOfButtons;
				}

				int* pSupportedTypes = new int[ m_MaxScreenmaps ];
				ASSERT(pSupportedTypes);

				if (pSupportedTypes)
				{
					if (CNodeScreenmapInfo_Util::getSupportedTypes(m_pNode,
																   m_parrScreenmapTypes,
																   pSupportedTypes,
																   m_MaxScreenmaps))
					{
						for (int i=0;i<m_MaxScreenmaps;i++)
						{														
							IScreenmap* const p = FindScreenmap(CScreenmapID(m_parrScreenmapTypes[i]));
							ASSERT( p );

							if (p)
							{
								p->SetTypes(pSupportedTypes[i]);
							}
						}		
					}
					else
					{
						ASSERT( 0 );
					}

					delete [] pSupportedTypes;
				}								
			}
		}
		else
		{
			AfxMessageBox(szError);
		}
	}
}


// Sedert V16.21: USE_SMAP_ENERGYMEASURE
// Integratie van de energy measurement screenmapping.
// Om te voorkomen dat we bij het openen van een bestaand project (waar de energy meting nog niet aanwezig was) 
// telkens een melding krijgen gaan we geen melding geven bij het niet vinden van dit type bestand.
void CNodeLcdScreenmaps::Load(CString szPathName)
{
	BOOL fResult;
	BOOL warningMessage = FALSE;

	for (int i=0;i<m_MaxScreenmaps;i++) {
		ASSERT( m_arrScreenmaps[i] );
		CString szFileName;	
		if (FormatFileName(szPathName,i,szFileName) == TRUE) {					
			if ((fResult = m_arrScreenmaps[i]->Load(szFileName)) == FALSE) {
				const CScreenmapID screenmapID = m_arrScreenmaps[i]->getScreenmapID();
				if (screenmapID.get() != SCREENMAP_ENERGYMEASURE) {
					warningMessage = TRUE;					
				}
				if (m_arrScreenmaps[i]) {
					m_arrScreenmaps[i]->FillStandard(m_pNode);
				}
			}			
		}	
		else
		{
			const CScreenmapID screenmapID = m_arrScreenmaps[i]->getScreenmapID();
			if (screenmapID.get() != SCREENMAP_ENERGYMEASURE) {
				ASSERT(0);			
				warningMessage = TRUE;
				
			}
			if (m_arrScreenmaps[i]) {
				m_arrScreenmaps[i]->FillStandard(m_pNode);			
			}
		}
	} // end for ...

	if (warningMessage) {
		const CString szMessage("Some screenmap files cannot be found. These files are initialized to defaults.");		
		AfxMessageBox(szMessage);		
	}
}

void CNodeLcdScreenmaps::Save(CString szPathName)
{
	CString szFileName;
	DWORD dwResult;

	if (szPathName.IsEmpty() == FALSE) 
	{
		if (FormatDirectory(szPathName,szFileName) == TRUE)
		{							  
			if ((dwResult=CreateSubDirectory(szFileName)) == 0) 
			{
				for (int i=0;i<m_MaxScreenmaps;i++) 
				{
					ASSERT( m_arrScreenmaps[i] );

					if (FormatFileName(szPathName,i,szFileName) == TRUE)
					{									
						if (m_arrScreenmaps[i]->Save(szFileName) == FALSE) 
						{
							// TM 21/01/2008
							// CScreenmap save toont al een fout melding ...
						
						} // if (Screenmaps[i].Save(szFileName) == FALSE) 						
					
					} // if ((FormatFileName(szPathName,i,szFileName) == TRUE)
					else
					{
						ASSERT(0);
					}
				} 
			}
			else 
			{
				// ERROR
				CString szError;
				szError.Format(_T("Failed to Create Directory :%s\nFunction returned error code %d"),szPathName,dwResult);
				AfxMessageBox(szError);
			}
		}
	}
	else 
	{
		CString szError("First save project,Screenmaps can't be stored");
		AfxMessageBox(szError);
	}
}

BOOL CNodeLcdScreenmaps::IsModified(void)
{
	for (int i=0;i<m_MaxScreenmaps;i++) {	
		if (m_arrScreenmaps[i]) {
			if (m_arrScreenmaps[i]->IsModified()) {
				return TRUE;
			}	
		}
	}
	return FALSE;
}



BOOL CNodeLcdScreenmaps::FormatDirectory(CString szPathName,CString &szDirectory)
{
	CString szScreenmapDirectory;
	if (szScreenmapDirectory.LoadString(IDS_FILENAME_SUBDIRSCREENMAP) == FALSE) {
		CString szError(_T("Unable to load string resource IDS_FILENAME_SUBDIRSCREENMAP"));
		AfxMessageBox(szError);
		return FALSE;
	}
	szDirectory.Format(_T("%s%s\\"),szPathName,szScreenmapDirectory);
	return TRUE;

}

BOOL CNodeLcdScreenmaps::FormatFileName(CString szPathName,int index,CString &szFileName)
{
	CString szProjectFileName;
	CString szScreenmapFileName;
	CString szScreenmapDirectory;

	if (szScreenmapDirectory.LoadString(IDS_FILENAME_SUBDIRSCREENMAP) == FALSE) 
	{
		CString szError(_T("Unable to load string resource IDS_FILENAME_SUBDIRSCREENMAP"));
		AfxMessageBox(szError);
		return FALSE;
	}
	if (szScreenmapFileName.LoadString(IDS_FILENAME_SCREENMAP) == FALSE) 
	{
		CString szError(_T("Unable to load string resource IDS_FILENAME_SUBDIRSCREENMAP"));
		AfxMessageBox(szError);
		return FALSE;
	}
	
	if (m_pNode != NULL)
	{
		szFileName.Format(_T("%s%s\\%s%02x_%d.bin"),
						  szPathName,
						  szScreenmapDirectory,				
						  szScreenmapFileName,
						  m_pNode->bLogicalAddress,
						  m_parrScreenmapTypes[index]);
						  // index);

		return TRUE;
	}

	return FALSE;
}


/**
 * Copy all screenmaps from 1 Node to another node ...
 */
BOOL CNodeLcdScreenmaps::Copy(CNodeLcdScreenmaps* pDest,const CNodeLcdScreenmaps* pSrc)
{
	ASSERT( pSrc );
	ASSERT( pDest );

	if (pSrc != NULL && pDest != NULL) {
		ASSERT( pSrc->getMaxScreenmaps( ) == pDest->getMaxScreenmaps( ));	
		for (int i=0;i<pDest->getMaxScreenmaps( );i++) {			
			ASSERT( pDest->m_arrScreenmaps[i] && pSrc->m_arrScreenmaps[i] );
			if (CScreenmapUtil::Copy(pDest->m_arrScreenmaps[i],pSrc->m_arrScreenmaps[i]) == FALSE) {
				ASSERT(0);
				return FALSE;
			}			
		}		
		return TRUE;
	}
	return FALSE;
}






