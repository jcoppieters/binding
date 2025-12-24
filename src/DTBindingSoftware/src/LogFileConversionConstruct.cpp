// LogFileConversionConstruct.cpp: implementation of the CLogFileConversionConstruct class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "LogFileConversionConstruct.h"
#include "log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFileConversionConstruct::CLogFileConversionConstruct()
{
}

CLogFileConversionConstruct::~CLogFileConversionConstruct()
{

}

void CLogFileConversionConstruct::Report(CLog* const pLog,BOOL fShowStrings)
{
	if (pLog != 0)
	{
		const int nMax = pLog->Count();

		if (nMax != 0)
		{		
			AddSeperator();

			CString s;
			s.Format("BindingNr : 0x%04x\n",m_usCurrentBindingNr);
			Write( s );

			if (fShowStrings == TRUE)
			{

				if (m_pCurrentStrings != 0)
				{
					this->CLogFileConversionFromNodes::Report( m_pCurrentStrings );
				}

				Write( "\n" );
			}
	
			for (int i=0;i<nMax;i++)
			{
				CString s(pLog->GetMessage(i));
				if (s.IsEmpty() == FALSE)
				{				
					Write( s );
				}		
			}

			AddSeperator();
		}
	}
}

void CLogFileConversionConstruct::Init(FILE* fp)
{
	m_fp = fp;
}
