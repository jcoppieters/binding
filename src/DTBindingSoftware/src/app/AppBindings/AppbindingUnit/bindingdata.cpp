#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingdata.h"

void CBindingData::Clear(void)
{
	bMessageCode = 0;
	bLength = 0;
	bBindingFlags = 0;
	
	for (int i=0;i<6;i++)
	{
		bData[i] = 0;
	}
}

BOOL CBindingData::IsEqual(const CBindingData* const pSource) const
{
	if (pSource == NULL)
		return FALSE;
	
	if (this->bMessageCode != pSource->bMessageCode)
		return FALSE;

	if (this->bLength != pSource->bLength)
		return FALSE;

	for (int i=0;i<bLength && i<6;i++) 
	{
		if (this->bData[i] != pSource->bData[i])
			return FALSE;
	}

	if ((this->bBindingFlags != pSource->bBindingFlags) &&
		(pSource->bBindingFlags != 0xFF))		// new !!!!
	{
		return FALSE;
	}

	return TRUE;

}
