#include "stdafx.h"
#include "ConfigBitmaps.h"

#include "App\AppCfg\AppCfgCanFtp\ConfigCanFtp_Info.h"
#include "utils/win32/ShellFunctions.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////
#define DEBUG_LOCAL_ON		0
//////////////////////////////////////////////////////////////////
namespace
{
	template <class PARAM>
		inline void TRACER(PARAM x)
	{
		#if (DEBUG_LOCAL_ON == 1)
		TRACE(x);
		#endif
	};

	template <class PARAM, class PARAM2>
		inline void TRACER(PARAM x, PARAM2 y)
	{
		#if (DEBUG_LOCAL_ON == 1)
		TRACE( x, y );
		#endif
	};
};
//////////////////////////////////////////////////////////////////
///////////////////////////// CConfigCanIRTX /////////////////////
//////////////////////////////////////////////////////////////////

void CConfigBitmaps::onFTPStarted(
	const CConfigCanFTP_FileEntry& /*entry*/)		// CHANGES_MVS_2008_WARNINGS
{
	TRACER("CConfigBitmaps::onFTPStarted\n");

	// Absorbe.

};

void CConfigBitmaps::onFTPDone(
	const CConfigCanFTP_FileEntry& entry)
{
	TRACER("CConfigBitmaps::onFTPStopped\n");

	const BYTE bNodeAddress = entry.getNodeAddress( );

	CString szFileDest = entry.getLocalFileName( );
	if (!szFileDest.IsEmpty())
	{
		CString szOld;
		CString szNew;

		szOld.Format("Config\\bitmaps_%02x", bNodeAddress);
		szNew.Format("Config\\sendedbitmaps_%02x", bNodeAddress);

		szFileDest.MakeLower( );
		szOld.MakeLower( );
		szNew.MakeLower( );

		if (szFileDest.Replace(szOld, szNew) == 1)
		{				
			TRACER("Create directory when not exists.\n");
			
			CString szTemp;
			int temp = szFileDest.ReverseFind('\\');
			if (temp > 0)
			{
				szTemp = szFileDest.Left(temp + 1);
			}

			if (!szTemp.IsEmpty())
			{
				// Solved bug: TM20100823.
				if (CreateSubDirectory( szTemp ) == 0)
				{

					TRACER("Copy files\n");

					const int res = MoveFileEx(
										entry.getLocalFileName( ),
										szFileDest,
										MOVEFILE_REPLACE_EXISTING + MOVEFILE_COPY_ALLOWED);
					if (res == 0)
					{
/*
						const DWORD dwError = GetLastError( );		// CHANGES_MVS_2008_WARNINGS
*/
						ASSERT( res != 0);
					}
				}
			}
		}
	};
};

///////////////////////////////////////////////////////////////////