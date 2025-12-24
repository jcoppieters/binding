#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunit.h"

CBindingUnit::CBindingUnit( ) :
bType( 0 )
{
	BindingData.Clear();

	nID = 0;
	szName.Empty();
	bNodeAddress = 0xFF;
	bUnitAddress = 0xFF;
	bIOType = 0;
	SetPosition(1000,1000);
	SetApplicationFlags(0);
	m_strToolTipText.Empty( );
}

CBindingUnit::CBindingUnit(BYTE bBindingUnitType) :
bType(bBindingUnitType)
{
	BindingData.Clear();

	nID = 0;
	szName.Empty();
	bNodeAddress = 0xFF;
	bUnitAddress = 0xFF;
	bIOType = 0;
	SetPosition(1000,1000);
	SetApplicationFlags(0);
	m_strToolTipText.Empty( );
}

CBindingUnit::~CBindingUnit()
{
}

BYTE CBindingUnit::GetEvent(void) const
{
	return BindingData.Get_MessageCode(); 
}


void CBindingUnit::SetName(CString szUnitName)
{
	szName.Format(_T("%s"),szUnitName);
}


BOOL CBindingUnit::IsEqual(const CBindingUnit* const pUnit,BOOL fCheckBindingData,BOOL fCheckID) const
{
	
	#ifdef _DEBUG_BINDINGUNIT_IS_EQUAL

	if (1)
	{
		CString s1;
		s1.Format("this: NodeAddress:0x%02x,unitAddress:0x%02x,bIoType:%d",
				  this->bNodeAddress,
				  this->bUnitAddress,
				  this->bIOType);

		CString s2;
		s2.Format("pUnit: NodeAddress:0x%02x,unitAddress:0x%02x,bIoType:%d",
				  pUnit->bNodeAddress,
				  pUnit->bUnitAddress,
				  pUnit->bIOType);

		TRACE1("\n\n%s",s1);
		TRACE1("\n%s",s2);
	}

	#endif


	if (pUnit == NULL)
		return FALSE;

	if (fCheckID == TRUE) 
	{
		if (this->nID != pUnit->nID)
		{
			#ifdef _DEBUG_BINDINGUNIT_IS_EQUAL

			if (1)
			{
				CString s1("NId is different");
				TRACE1("\n%s",s1);
			}

			#endif

			return FALSE;		
		}
	}

	if (this->bNodeAddress != pUnit->bNodeAddress) 
		return FALSE;

	if (this->bUnitAddress != pUnit->bUnitAddress) 
		return FALSE;	

	if (this->bIOType != pUnit->bIOType) 
	{
		#ifdef _DEBUG_BINDINGUNIT_IS_EQUAL
		
		if (1)
		{
			CString s1("bIOType is different");
			TRACE1("\n%s",s1);
		}
		
		#endif

		return FALSE;
	}

	if (this->bType != pUnit->bType)
	{
		#ifdef _DEBUG_BINDINGUNIT_IS_EQUAL

		if (1)
		{
			CString s1("bType is different");
			TRACE1("\n%s",s1);
		}

		#endif

		return FALSE;

	}
	
	if (fCheckBindingData == TRUE)
	{
		BOOL fResult = this->BindingData.IsEqual(&pUnit->BindingData); 
		if (fResult == FALSE)
		{
			#ifdef _DEBUG_BINDINGUNIT_IS_EQUAL

			if (1)
			{
				CString s1("BindingData is different");
				TRACE1("\n%s",s1);
			}

			#endif
		}
		return fResult;
	}

#ifdef _DEBUG_BINDINGUNIT_IS_EQUAL

	if(1)
	{
		CString s("Equal !!");
		TRACE1("\n%s",s);
	}

#endif

	return TRUE;
}


CString CBindingUnit::getTextToolTip( ) {
#if(0)
	if (this->m_strToolTipText.IsEmpty()) {
		this->m_strToolTipText.Format("Name='%s' Address=0x%02x,0x%02x", this->GetName(), this->bNodeAddress, this->bUnitAddress);
	}
#endif
	this->m_strToolTipText.Format("Address=0x%02x,0x%02x '%s'", this->bNodeAddress, this->bUnitAddress, this->ConvertFuntionToString());
	return this->m_strToolTipText;
}