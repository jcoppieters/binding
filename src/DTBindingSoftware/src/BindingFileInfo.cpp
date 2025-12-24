
#include "stdafx.h"
#include "BindingFileInfo.h"
#include "BindingFile1.h"

///////////////////////////////////////////////////////////////////////

void CBindingFileInfo_Util::reset(CBindingFileInfo* ptr)
{
	ptr->m_NrOfBindings = 0;
	ptr->m_NrOfTimers = 0;
	ptr->m_NrOfReferences = 0;
	ptr->m_NrOfCBindings = 0;
}

void CBindingFileInfo_Util::setNrOfBindings(CBindingFileInfo* ptr,int nr)
{
	ptr->m_NrOfBindings=nr;
}

void CBindingFileInfo_Util::incNrOfCBindings(CBindingFileInfo* ptr)
{
	ptr->m_NrOfCBindings++;
}

void CBindingFileInfo_Util::incNrOfTimers(CBindingFileInfo* ptr)
{
	ptr->m_NrOfTimers++;
}

void CBindingFileInfo_Util::incNrOfReferences(CBindingFileInfo* ptr)
{
	ptr->m_NrOfReferences++;
}
