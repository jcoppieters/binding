// FileIoData.h: interface for the CFileIoData class.
//
// Changes TM20160317: ref naar CCanNode meegegeven bij inladen en opslaan
//					   van de unit properties.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEIODATA_H__9B79C566_93A7_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_FILEIODATA_H__9B79C566_93A7_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCanNode;
struct STRUCT_UNIT;
class CNodeDatabase;
class CBindingEntries;
class CBindingEntry;

class CBindingUnit;
class CBindingOperator;
class CBindingLink;

// Binding property units.
class CBindingPropertyDimUnit;
class CBindingPropertySwitchUnit;
class CBindingPropertyDuoSwitchUnit;
class CBindingPropertySensUnit;
class CBindingPropertyAlarmUnit;	
class CBindingPropertyAVMatrixUnit;
class CBindingPropertyUnit;

class CAppLicActCode;

/////////////////////////// BINDINGENTRIES //////////////////////////

/**********************************************
	+ CFileIoBindingEntries
		+ CfileIoBindingEntry
			+ CFileIoBindingUnit
				+CFileBindingUnitData
			+ CFileIoBindingOperator
			+ CFileIoBindingLink
***********************************************/
class CFileIo_BindingUnitData
{
public:
	BOOL Save(CBindingUnit *pUnit,CFile* fp);
	BOOL Load(CBindingUnit *pUnit,CFile* fp);
};

class CFileIo_BindingLink2
{
public:
	BOOL Save(CBindingLink *pLink,CFile* fp);
	BOOL Load(CBindingLink *pLink,CFile* fp);
};

class CFileIo_BindingOperator2
{
public:
	BOOL Save(CBindingOperator *pOperator,CFile* fp);
	BOOL Load(CBindingOperator *pOperator,CFile* fp);
};

class CFileIo_BindingUnit2
{
public:
	BOOL Save(CBindingUnit *pUnit,CFile* fp);
	BOOL Load(CBindingUnit *pUnit,CFile* fp);
public:
	CFileIo_BindingUnitData BindingUnitData;
};

class CFileIo_BindingEntry2
{
public:
	BOOL Save(CBindingEntry *pEntry,CFile* fp);
	BOOL Load(CBindingEntry *pEntry,CFile* fp);
public:
	CFileIo_BindingUnit2 BindingUnit;
	CFileIo_BindingOperator2 BindingOperator;
	CFileIo_BindingLink2 BindingLink;
};

class CFileIo_BindingEntries
{
public:
	BOOL Save(CBindingEntries *pEntries,CFile* fp);
	BOOL Load(CBindingEntries *pEntries,CFile* fp);

public:

	CFileIo_BindingEntry2 BindingEntry;
};

///////////////////////////// NODEDATABASE //////////////////////////


/**********************************************
	+CFileIoNodedatabase
		+CFileIoNodeData
		+CfileIoUnitData
			+CFileIoPropertyUnits
				+CFileIoDimmerProperties
				+CFileIoSwitchProperties
				+CFileIoDuoSwitchProperties
				+CFileIoSensProperties
				+CFileIoAlarmProperties		
				+CFileIo_AVMatrixProperties (Sedert 0x0F00)
************************************************/

class CFileIo_DimmerProperties
{
public:
	BOOL Save(CBindingPropertyDimUnit *pUnit,CFile* fp);
	BOOL Load(const CCanNode* pNode,const STRUCT_UNIT* const pUnit,CBindingPropertyUnit **pBindingUnit,CFile* fp);
};

class CFileIo_SwitchProperties
{
public:
	BOOL Save(CBindingPropertySwitchUnit *pUnit,CFile* fp);
	BOOL Load(const CCanNode* pNode,const STRUCT_UNIT* const pUnit,CBindingPropertyUnit **pBindingUnit,CFile* fp);
};

class CFileIo_DuoSwitchProperties
{
public:
	BOOL Save(CBindingPropertyDuoSwitchUnit *pUnit,CFile* fp);
	BOOL Load(const CCanNode* pNode,const STRUCT_UNIT* const pUnit,CBindingPropertyUnit **pBindingUnit,CFile* fp);
};

class CFileIo_SensProperties
{
public:
	BOOL Save(CBindingPropertySensUnit *pUnit,CFile* fp);
	BOOL Load(const CCanNode* pNode,const STRUCT_UNIT* const pUnit,CBindingPropertyUnit **pBindingUnit,CFile* fp);
};

class CFileIo_AlarmProperties
{
public:
	BOOL Save(CBindingPropertyAlarmUnit *pUnit,CFile* fp);
	BOOL Load(const CCanNode* pNode,const STRUCT_UNIT* const pUnit,CBindingPropertyUnit **pBindingUnit,CFile* fp);
};

// Binding property unit AVMatrix
class CFileIo_AVMatrixProperties
{
public:
	BOOL Save(CBindingPropertyAVMatrixUnit *pUnit,CFile* fp);
	BOOL Load(const CCanNode* pNode,const STRUCT_UNIT* const pUnit,CBindingPropertyUnit **pBindingUnit,CFile* fp);
};

//--------------------------------------------------------------------

class CFileIo_PropertyUnits
{
public:
	BOOL Save(STRUCT_UNIT *pUnit,CFile* fp);
	BOOL Load(const CCanNode* pNode,STRUCT_UNIT *pUnit,CFile* fp);

protected:
	CFileIo_DimmerProperties DimUnitProperties;
	CFileIo_SwitchProperties SwitchUnitProperties;
	CFileIo_DuoSwitchProperties DuoSwitchUnitProperties;
	CFileIo_SensProperties SensUnitProperties;
	CFileIo_AlarmProperties AlarmUnitProperties;	
	CFileIo_AVMatrixProperties AVMatrixUnitProperties;
};

// -------------------------------------------------------------------
// TM 20050617 : New FileIo ...

class CFileIo_UnitSpecs
{
public:
	BOOL Save(STRUCT_UNIT *pUnit,CFile* fp);
	BOOL Load(const CCanNode* pNode,STRUCT_UNIT *pUnit,CFile* fp);
};
// -------------------------------------------------------------------


// ------------------------------------------------------------------

// Sedert v0700: Aantal resource blocks verhoogd.
class CFileIo_NodeData2
{	
public:	
	BOOL Save(CCanNode *pNode,CFile* fp);
	BOOL Load(CCanNode *pNode,CFile* fp);
};

class CFileIo_UnitData2
{
public:
	BOOL Save(STRUCT_UNIT *pUnit,CFile* fp);
	BOOL Load(const CCanNode* pNode,STRUCT_UNIT *pUnit,CFile* fp);

protected:
	CFileIo_PropertyUnits UnitBindingProperties;
	CFileIo_UnitSpecs	UnitSpecs;
};


class CFileIo_NodeDatabase
{
public:
	BOOL Save(CNodeDatabase *pDocDatabase,CFile* fp);
	BOOL Load(CNodeDatabase *pDocDatabase,CFile* fp);

protected:
	CFileIo_NodeData2 Nodes;
	CFileIo_UnitData2 Units;
};

//////////////////////////////////////////////////////////////////

/***********************************
  
	+CFileIOData
		+DefaultString
		+SW_Version
			+CFileIoNodeDatabase
			+CFileBindingEntries

************************************/

class CFileIoData  
{
public:
	CFileIoData(CNodeDatabase *pNodeDatabase,CBindingEntries *pBindings,CAppLicActCode* pAppLicActCode);
	virtual ~CFileIoData();

	BOOL Save(CFile *f);
	BOOL Load(CFile *f);

protected:
	CFileIo_NodeDatabase	NodesDatabase;
	CFileIo_BindingEntries	BindingEntries;

private:
	CNodeDatabase* const m_pNodeDatabase;
	CBindingEntries* const m_pBindingsDatabase;
	CAppLicActCode* const m_pAppLicActCode;
	const unsigned short sw_version;
};

//////////////////////////////////////////////////////////////////

#endif // !defined(AFX_FILEIODATA_H__9B79C566_93A7_11D8_B865_0050BAC412B1__INCLUDED_)
