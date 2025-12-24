#include "stdafx.h"
#include "ConfigIRTXFile.h"
#include <stdio.h>

////////////////////////////////////////////////
namespace
{
	const int ID_SER_IRTXCODE_BASIC		= 420;
	const int ID_SER_IRTXCODE_HC4		= 421;

	enum Max_Crunched_CodeSize
	{
		MAX_CRUNCHED_CODESIZE_BASIC 	= 	100,
		MAX_CRUNCHED_CODESIZE_HC4   	= 	150
	};

	enum
	{
		MAX_IRTX_DEVICE_NAME 		  	= 16,
		MAX_IRTX_FUNCTION_NAME          = 16
	};

#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */

	struct STRUCT_IRTX_HEADER
	{
		/** De naam van het toestel */
		char szDeviceName[ MAX_IRTX_DEVICE_NAME ];
		/** De naam van de functie */
		char szFunctionName[ MAX_IRTX_FUNCTION_NAME ];
		/** identifier in de database */
		unsigned short usIdentifier;
	};

	struct STRUCT_CRUNCHED_IR_CODE_BASIC
	{
		/** Device type : 0x00 = standard 0x01 = B&O */
		unsigned char  bDeviceType;
		/** time of the smallest puls */
  		unsigned char  bSmallestPuls;
		/** number of carriers in one cycle */
  		unsigned char  bNROfSequences;
		/** for the alignment !! */
  		unsigned char  bDummy;
		/** time between 2 IR codes */
  		unsigned long  ulRepeatTimer;
		/** number of repeatings ( min 1 ) */
  		unsigned char  bNROfRepeats;
		/** length of the buffer, is used MAX_CRUNCHED_CODESIZE */
  		unsigned char  bBufferLength;

		unsigned char  pbIRCrunchedCodeInfo[MAX_CRUNCHED_CODESIZE_BASIC];
		unsigned short pusIRCrunchedCodeTimings[MAX_CRUNCHED_CODESIZE_BASIC];
	};
	struct STRUCT_CRUNCHED_IR_CODE_HC4
	{
		/** Device type : 0x00 = standard 0x01 = B&O */
		unsigned char  bDeviceType;
		/** time of the smallest puls */
  		unsigned char  bSmallestPuls;
		/** number of carriers in one cycle */
  		unsigned char  bNROfSequences;
		/** for the alignment !! */
  		unsigned char  bDummy;
		/** time between 2 IR codes */
  		unsigned long  ulRepeatTimer;
		/** number of repeatings ( min 1 ) */
  		unsigned char  bNROfRepeats;
		/** length of the buffer, is used MAX_CRUNCHED_CODESIZE */
  		unsigned char  bBufferLength;

		unsigned char  pbIRCrunchedCodeInfo[MAX_CRUNCHED_CODESIZE_HC4];
		unsigned short pusIRCrunchedCodeTimings[MAX_CRUNCHED_CODESIZE_HC4];
	};

	struct CSerIRTXCodeBasic
	{
		STRUCT_IRTX_HEADER m_header;
		STRUCT_CRUNCHED_IR_CODE_BASIC m_data;
	};

	struct CSerIRTXCodeHC4
	{
		STRUCT_IRTX_HEADER m_header;
		STRUCT_CRUNCHED_IR_CODE_HC4 m_data;
	};
#pragma pack()

	template <class SERCLASS>
	void update(CConfigIRTXFileEntry* pEntry, const SERCLASS& serClass)
	{
		pEntry->getHeader().setDeviceName(serClass.m_header.szDeviceName);
		pEntry->getHeader().setFunctionName(serClass.m_header.szFunctionName);
	}
		
};
////////////////////////////////////////////////

////////////////////////////////////////////////

CConfigIRTXFileEntry::CConfigIRTXFileEntry(void) :
m_header( ),
m_data( )
{
}

CConfigIRTXFileEntry::~CConfigIRTXFileEntry(void)
{

}

////////////////////////////////////////////////

CConfigIRTXFile::CConfigIRTXFile(void) :
m_list( ),
m_count( 0 )
{
}

CConfigIRTXFile::~CConfigIRTXFile(void)
{
	removeAll( );
}

BOOL CConfigIRTXFile::Load(CString szFilename)
{
	FILE* fp = 0;

	removeAll( );

	// openen van het bestand in binaire mode.
	if ((fp = fopen(szFilename,"rb")) != NULL)
	{
		int numRead = 0;
		int identifier = 0;

		while(!feof(fp))	 // Moeten lezen tot we EOF hebben ...
		{
			numRead = fread((void*)&identifier,sizeof(unsigned short),1,fp );
			if (ferror(fp) != 0) 
			{
				fclose(fp);
				return FALSE;
			}
			else
			{
				if ( numRead != 0 )
				{
					switch (identifier)
					{
						case ID_SER_IRTXCODE_BASIC:
						{
							CSerIRTXCodeBasic serClass;

							numRead = fread((void*)&serClass,sizeof(CSerIRTXCodeBasic),1,fp);
							if (numRead > 0)
							{
								CConfigIRTXFileEntry* pNewEntry = new CConfigIRTXFileEntry( );
								update(pNewEntry,serClass);						
								add(*pNewEntry);
							}
							else
							{
								fclose(fp);
								return FALSE;
							}

						} break;

						case ID_SER_IRTXCODE_HC4:
						{
							CSerIRTXCodeHC4 serClass;

							numRead = fread((void*)&serClass,sizeof(CSerIRTXCodeHC4),1,fp);
							if (numRead > 0)
							{
								CConfigIRTXFileEntry* pNewEntry = new CConfigIRTXFileEntry( );
								update(pNewEntry,serClass);		
								add(*pNewEntry);
							}
							else
							{
								fclose(fp);
								return FALSE;
							}

						} break;

						default:
						{
							fclose(fp);
							return FALSE;	

						} break;
					}
				}
			}
		}
		fclose(fp);
		return TRUE;	
	}
	return FALSE;
}

/////////////////// protected //////////////////
void CConfigIRTXFile::add(CConfigIRTXFileEntry& entry)
{
	m_list.addTail(&entry);
	m_count++;
}

CConfigIRTXFileEntry* CConfigIRTXFile::get(int index)
{
	ASSERT(index < m_list.getCount());

	return m_list.getEntry(index);
}
void CConfigIRTXFile::removeAll( )
{
	while (!m_list.isEmpty())
	{
		CConfigIRTXFileEntry* const p = m_list.removeHead( );
		ASSERT( p );
		delete p;
	}
	ASSERT( m_list.getCount( ) == 0 );
	m_count = 0;
}

////////////////////////////////////////////////
