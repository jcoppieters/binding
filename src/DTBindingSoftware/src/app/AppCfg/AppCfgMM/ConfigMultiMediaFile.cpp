#include "stdafx.h"
#include "ConfigMultiMediaFile.h"
#include "ConfigMultiMediaClass.h"
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
using DUOTECNO::MFC_HELPER::CArrayErrors;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigMultiMediaFile::CConfigMultiMediaFile() :
m_configErrors( )
{			
	ClearErrors( );
}

CConfigMultiMediaFile::~CConfigMultiMediaFile()
{
	ClearErrors( );
}

void CConfigMultiMediaFile::ClearErrors( )
{
	m_configErrors.removeAll( );
}

