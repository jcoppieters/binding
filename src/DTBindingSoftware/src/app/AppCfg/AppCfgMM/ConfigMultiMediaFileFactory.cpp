#include "stdafx.h"
#include "ConfigMultiMediaFileFactory.h"

#include "ConfigMultiMediaFile.h"
#include "ConfigMultiMediaFile_v64.h"
#include "ConfigMultiMediaFile_v65.h"
#include "ConfigMultiMediaFile_v66.h"


CConfigMultiMediaFile* CConfigMultiMediaFileFactory::Create(Type type)
{
	CConfigMultiMediaFile* p = 0;
	switch (type)
	{
		case V64:
		{
			p = new CConfigMultiMediaFile_V64();
		} break;
		case V65:
		{		
			p = new CConfigMultiMediaFile_V65();
		} break;
		case V66:
		{		
			p = new CConfigMultiMediaFile_V66();
		} break;
	}
	return p;
}
