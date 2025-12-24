// ConfigMultiMediaInfo.cpp: implementation of the CConfigMultiMediaInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ConfigMultiMediaInfo.h"
#include "ConfigMultiMediaFile.h"
#include "ConfigMultiMediaFileFactory.h"

#include "NodeDatabase.h"
#include "app\appCfg\AppCfginfo\cfginfonode.h"	
// -------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalSettings;
// -------------------------------------



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//const char* CConfigMultiMediaInfo::szMultiMediaFilename = "audioconfig_fc.txt";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigMultiMediaInfo* CConfigMultiMediaInfo::GetInstance()
{
	static CConfigMultiMediaInfo instance;
	return (&instance);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigMultiMediaInfo::CConfigMultiMediaInfo() :
m_fSendToAllNodes( FALSE ),
m_fFileLoaded( FALSE ),
m_fReady(TRUE),
m_pMultiMediaFile( 0 )
{
	resetInfo( );
}

CConfigMultiMediaInfo::~CConfigMultiMediaInfo()
{
	deallocate( );

	TRACE("Destructor ...");
}

void CConfigMultiMediaInfo::resetInfo( )
{
	m_bCount = 0;

	for (int i=0;i<MAX_COMPATIBLE_NODES;i++)
	{
		m_nodeEntries[i].bLogicalNodeAddress = 0xFF;
		m_nodeEntries[i].type = -1;
	}
}

void CConfigMultiMediaInfo::allocate(CCanNode* pNode)
{
	CConfigMultiMediaFileFactory::Type type = CConfigMultiMediaFileFactory::V64;

	const NodeMultimediaConfigType multimediaType = CNodeMultimediaConfig_Util::getNodeType(pNode);
	if (multimediaType == NODEMULTIMEDIA_CONFIG_TYPE_V64)
	{
		type = CConfigMultiMediaFileFactory::V64;
	}
	else if (multimediaType == NODEMULTIMEDIA_CONFIG_TYPE_V65)
	{
		type = CConfigMultiMediaFileFactory::V65;
	}	
	else if (multimediaType == NODEMULTIMEDIA_CONFIG_TYPE_V66)
	{
		type = CConfigMultiMediaFileFactory::V66;
	}	
	m_pMultiMediaFile = CConfigMultiMediaFileFactory::Create(type);
	ASSERT( m_pMultiMediaFile );
}

void CConfigMultiMediaInfo::deallocate( )
{
	if (m_pMultiMediaFile)
	{
		delete m_pMultiMediaFile;
		m_pMultiMediaFile = 0;	
	}
}

///////////////////////////////////////////////////////////////

BOOL CConfigMultiMediaInfo::Load(CCanNode* pNode)
{	
	ASSERT( CNodeMultimediaConfig_Util::isSupported(pNode) );

	m_fFileLoaded = FALSE;

	deallocate( );
	allocate( pNode );

	if (m_pMultiMediaFile)
	{
		m_fFileLoaded = m_pMultiMediaFile->Load(GetFullPathName());
		if (!m_fFileLoaded)
		{
			// Kon het multimedia bestand niet inladen.
		}
	}

	return (m_fFileLoaded);
}

CString CConfigMultiMediaInfo::GetFullPathName(void)
{
	CString sztemp;
	CString szSubDir((LPCSTR) IDS_FILENAME_SUBDIR_AUDIOCONFIG);
	CString szName((LPCSTR) IDS_FILENAME_AUDIOCONFIG);
	CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	

	sztemp.Format("%s%s\\%s",szPathName,szSubDir,szName);	
	return sztemp;
}

// TM20190708: V65 & V66 nodes opnieuw compatible.
// Versturen van cfg naar alle nodes.
BOOL CConfigMultiMediaInfo::InitNodeEntries(
	BOOL fSendToAll,
	CNodeDatabase *pDatabase,
	BYTE bNodeAddress)
{	
	ASSERT( m_fFileLoaded );
	ASSERT( m_pMultiMediaFile );

	resetInfo( );

	if (m_pMultiMediaFile)
	{		
		m_fSendToAllNodes = fSendToAll;	

		ASSERT( pDatabase != 0 );			// DATABASE moet altijd geldig zijn !!!!
		ASSERT( bNodeAddress != 0xFF );		// Moeten een geldig node adres hebben.
		ASSERT( m_bCount == 0 );			// Dit gebeurt in de resetInfo( );

		if (pDatabase == 0 || bNodeAddress == 0xFF)
		{
			return FALSE;	// We gaan er direkt uit...
		}

		if (m_fSendToAllNodes) 
		{	
			// Naar alle compatible nodes versturen.
			// alle nodes zoeken die een compatible multimedia bestand type hebben.

			int t;
			CCanNode* pNode = 0;
			if (pDatabase->FindNode(bNodeAddress,&t,&pNode))
			{
				ASSERT(CNodeMultimediaConfig_Util::isSupported(pNode));
				
				// type van deze node.
				const NodeMultimediaConfigType type = 
					CNodeMultimediaConfig_Util::getNodeType(pNode);

				CNodeIterator it(pDatabase->CreateNodeIterator());
				for (it.Begin(); !it.IsDone(); it.Next())
				{
					CCanNode* const p = it.CurrentItem(); 
					if (CNodeMultimediaConfig_Util::isSupported(p))
					{					
						bool flag_continue = false;
						const NodeMultimediaConfigType type2 = CNodeMultimediaConfig_Util::getNodeType(p);
						if (type2==type) 
						{	
							flag_continue = true;
						}
						else if ((NODEMULTIMEDIA_CONFIG_TYPE_V65==type)&&(NODEMULTIMEDIA_CONFIG_TYPE_V66==type2)) {
							flag_continue = true;
						}
						else if ((NODEMULTIMEDIA_CONFIG_TYPE_V66==type)&&(NODEMULTIMEDIA_CONFIG_TYPE_V65==type2)) {
							flag_continue = true;
						}						
						if (true==flag_continue)
						{
							m_nodeEntries[m_bCount].bLogicalNodeAddress = p->bLogicalAddress;
							m_nodeEntries[m_bCount].type = CNodeMultimediaConfig_Util::getNodeType(p);
							m_bCount++;	
						}		
					}
				}

				ASSERT( m_bCount > 0);
				if (m_bCount > 0)
				{
					return TRUE;
				}
			}	
			else
			{
				ASSERT( pNode );		// Node niet gevonden in de dbase...
			}
			return FALSE;
		}
		else
		{	
			// Niet naar alle nodes versturen ...
			int t;
			CCanNode* pNode = 0;
			if (pDatabase->FindNode(bNodeAddress,&t,&pNode))
			{
				if (CNodeMultimediaConfig_Util::isSupported(pNode))
				{
					// Er moet altijd naar 1 adres een multimedia bestand gestuurd worden.

					m_nodeEntries[0].bLogicalNodeAddress = bNodeAddress;
					m_nodeEntries[0].type = CNodeMultimediaConfig_Util::getNodeType(pNode); 

					m_bCount = 1;
					return TRUE;
				}
			}
		}
	}

	ASSERT( m_bCount > 0 );		// Moeten altijd minimum 1 node hebben.
	return FALSE;
}

///////////////////////////////////////////////////////////////

BOOL CConfigMultiMediaInfo::IsValidMultiMediaFile( ) const
{
	return (m_pMultiMediaFile == 0 ? FALSE : TRUE);
}

BOOL CConfigMultiMediaInfo::isReady() const
{
	return m_fReady;
}

void CConfigMultiMediaInfo::setReady(BOOL flag)
{
	m_fReady = flag;
}

CConfigExternalClass* CConfigMultiMediaInfo::GetClass(int type)
{
	ASSERT( m_pMultiMediaFile );

	CConfigExternalClass* p = 0;

	if (m_pMultiMediaFile)
	{
		p = m_pMultiMediaFile->GetClass( type );
		ASSERT( p );
	}

	return p;
}

DUOTECNO::MFC_HELPER::CArrayErrors* CConfigMultiMediaInfo::getErrors( )
{
	ASSERT( m_pMultiMediaFile );

	if (m_pMultiMediaFile)
	{
		return m_pMultiMediaFile->getErrors( );
	}
	return 0;
}