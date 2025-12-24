// ConfigExternalFile.cpp: implementation of the CConfigExternalFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigExternalFile.h"
/////////////////////////////////////////////////////////////////////
#include "ConfigExternalClass.h"
/////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigExternalFile::CConfigExternalFile(int maxClass,CConfigExternalClass** pClasses) :
m_maxClass( maxClass ),
m_current( 0xFF ),
m_pClasses( pClasses )
{
}

CConfigExternalFile::~CConfigExternalFile()
{
}

///////////////////////////////////////////////////////////////////////

// BUG-0159 & BUG-0160:
// Alle data in de array's vullen met een '\0'
// fgets( ) lengte MAX_BUFFER_LENGTH-1 nemen.
BOOL CConfigExternalFile::load(const char* szFileName)
{
	enum { MAX_BUFFER_LENGTH = 255 };

	char buffer[MAX_BUFFER_LENGTH];
	char szAttribute[MAX_BUFFER_LENGTH];
	char szData[MAX_BUFFER_LENGTH];

	initialise();

	FILE* const fs = fopen(szFileName,"r");
	if (fs != NULL)
	{    
		memset(buffer,'\0',sizeof(buffer));
		while(fgets(&buffer[0],(MAX_BUFFER_LENGTH-1),fs) != NULL)
		{
			memset(szAttribute,'\0',sizeof(szAttribute));		// Sedert V14.12
			memset(szData,'\0',sizeof(szData));					// Sedert V14.12

			sscanf(buffer,"%[^'=']=%[^\n]",szAttribute,szData);  // CHECKED_SSCANF_OK		
			if ((szAttribute[0]!=0x0a)&&
				(szAttribute[0]!=0x0d)&&		/* since 0x0C05 */
				(szAttribute[0]!=0x20)&& 
				(szAttribute[0]!=';'))
			{
				CheckAttribute(szAttribute,szData);
			}
		}
		fclose(fs);
		return TRUE;
	}	
	return FALSE;	
}

// Since 0x0C05.
BOOL CConfigExternalFile::save(const char* szFileName)
{
	BOOL fResult=FALSE;		
	FILE* const fs = fopen(szFileName,"w");
	if (fs != NULL)
	{   	
		char szTempBuffer[1024];
		fResult=TRUE;
		
		for (int i=0;i<m_maxClass;i++) 
		{
			CConfigExternalClass* const pClass=m_pClasses[i];

			if (pClass != NULL) 
			{		
				const int items=pClass->GetMax();
				for(int j=0;j<items;++j)
				{	
					ASSERT(pClass->IsUsed(j));

					memset(szTempBuffer,0,sizeof(szTempBuffer));
					
					if (TRUE==pClass->GetEntryStrCfg(
								j,
								&szTempBuffer[0],
								sizeof(szTempBuffer)-1))
					{
						fprintf(fs,szTempBuffer);
						//fprintf(fs,"\r\n");						
					}
					else
					{
						fResult=FALSE;
					}
				}
			}
		} /* for */	
		fclose(fs);
	}					
	return fResult;	
}

void CConfigExternalFile::initialise( )
{
	for (int i=0;i<m_maxClass;i++) 
	{
		if (m_pClasses[i] != NULL) 
		{	
			m_pClasses[i]->Initialise();
		}
	}
}

///////////////////////// PROTECTED ////////////////////////////
BOOL CConfigExternalFile::ParseHeader(char *szAttribute,char *szData)
{
	//first we search for a header that identifies the current class we initialise ...
	for (int i=0;i<m_maxClass;i++) 
	{
		if (m_pClasses[i] != NULL) 
		{		
			if (m_pClasses[i]->ParseHeader(szAttribute,szData)) 
			{							
				m_current = i;	
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CConfigExternalFile::ParseData(char *szAttribute,char *szData)
{
	if (m_current < m_maxClass) 
	{
		if (m_pClasses[m_current]->ParseData(szAttribute,szData)) 
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CConfigExternalFile::CheckAttribute(char *szAttribute,char *szData)
{
	// first parse header, if no header must be data ...
	// Parsedata uses pointers initialised by ParseHeader ...
	if (!ParseHeader(szAttribute,szData)) 
	{
		if(!ParseData(szAttribute,szData)) 
		{
			return FALSE;
		}
	}
	return TRUE;
}