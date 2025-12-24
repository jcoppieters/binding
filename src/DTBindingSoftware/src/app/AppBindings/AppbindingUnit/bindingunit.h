#pragma once

#include "bindingdata.h"
#include "bindingunit_def.h"


class CBindingUnit  
{
public:
	CBindingUnit( );
	CBindingUnit(BYTE bBindingUnitType);
	virtual ~CBindingUnit();

	enum {
		APPFLAG_DALI_BINDING_UNIT				= (1<<0)
	};

	/**
	 * virtual Methods to convert UnitSpecific data to struct
	 * function or Struct Event
	 */
	virtual BOOL ConvertToBindingData(void) 
	{
		return FALSE;
	};	

	virtual BOOL ConvertFromBindingData(void) 
	{
		return FALSE; 
	};	

	virtual BOOL UpgradeIoType() 
	{
		return FALSE;
	}
	
	virtual CString ConvertFuntionToString(void);

	unsigned short GetID(void) const;
	void SetID(unsigned short id);
	CString GetName(void) const;
	void SetName(CString szName);	
	BYTE GetNodeAddress(void) const;
	void SetNodeAddress(BYTE bAddress);
	BYTE GetUnitAddress(void) const;
	void SetUnitAddress(BYTE bAddress);
	BYTE GetBindingUnitType(void) const;
	void SetBindingUnitType(BYTE bUnitType);
	void SetPosition(float x,float y);
	void GetPosition(float *x,float *y) const;
	void SetInOutType(BYTE io);
	BYTE GetInOutType(void) const;
	void SetApplicationFlags(unsigned long x);
	unsigned long GetApplicationFlags(void) const;
	BYTE GetEvent(void) const;								// New ...
	BOOL IsEqual(const CBindingUnit *const pUnit, 
				 BOOL fCheckBindingData = TRUE, 
				 BOOL fCheckID = TRUE) const;

	CString getTextToolTip( );
	 
public:
	CBindingData BindingData;

protected:			
	unsigned short nID;		/* Collection : Id - Properties */	
	CString szName;			/* Unit - Properties */
	BYTE bNodeAddress;		/* Unit - Properties */
	BYTE bUnitAddress;		/* Unit - Properties */
	BYTE bType;				/* Unit - Properties */	
	float xPos;				/* Screen Layout */
	float yPos;				/* Screen Layout */
	BOOL bIOType;			/* Binding Data */
	unsigned long ulApplicationFlags;
	CString m_strToolTipText;

protected:
	virtual void Init(void)   {};		//TM 20040701
};


///////////////////////// inline functies //////////////////////////////
inline unsigned short CBindingUnit::GetID(void) const
{
	return (nID);
}

inline void CBindingUnit::SetID(unsigned short id)
{
	nID = id;
}

inline CString CBindingUnit::GetName(void) const
{
	return szName;
}
	
inline BYTE CBindingUnit::GetNodeAddress(void) const
{
	return bNodeAddress;
}

inline void CBindingUnit::SetNodeAddress(BYTE bAddress)
{
	bNodeAddress = bAddress;
}

inline BYTE CBindingUnit::GetUnitAddress(void) const
{
	return bUnitAddress;
}

inline void CBindingUnit::SetUnitAddress(BYTE bAddress)
{
	bUnitAddress = bAddress;
}

inline BYTE CBindingUnit::GetBindingUnitType(void) const
{
	return bType;
}

inline void CBindingUnit::SetBindingUnitType(BYTE bUnitType)
{
	bType = bUnitType;
}

inline void CBindingUnit::SetPosition(float x,float y)
{
	xPos = x;
	yPos = y;
}

inline void CBindingUnit::GetPosition(float *x,float *y) const
{
	*x = xPos;
	*y = yPos;
}

inline CString CBindingUnit::ConvertFuntionToString(void)
{
	return CString(""); 
}

inline void CBindingUnit::SetInOutType(BYTE io)	
{
	bIOType = io;	 
};

inline BYTE CBindingUnit::GetInOutType(void) const
{
	return (bIOType == TRUE ? 1 : 0);	// CHANGES_MVS_2008_WARNINGS
};

inline void CBindingUnit::SetApplicationFlags(unsigned long x)	
{
	ulApplicationFlags = x; 
};
	
inline unsigned long CBindingUnit::GetApplicationFlags(void) const	
{
	return ulApplicationFlags; 
};