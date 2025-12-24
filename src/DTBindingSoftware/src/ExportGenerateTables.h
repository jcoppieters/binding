// ExportGenerateTables.h: interface for the CExportGenerateTables class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTGENERATETABLES_H__5C3074E4_CC27_11D8_8499_0050BAC412B1__INCLUDED_)
#define AFX_EXPORTGENERATETABLES_H__5C3074E4_CC27_11D8_8499_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////////////

#define WORKAROUND_BUG_0106			// Tijdelijk !!!

////////////////////////////////////////////////////////////////////////////////////

class CBindingEntry;
class CBindingUnit;
class CBindingOperator;
struct STRUCT_UNIT;

////////////////////////////////////////////////////////////////////////////////////

class CExportGenerateTables  
{

protected:
	CString GetUnitType(BYTE bUnitType);
	CString GetBindingDescription(CBindingEntry *pEntry,CBindingUnit *pUnit);
	CString GetBindingFlags(CBindingEntry *pBindingEntry);

	BOOL GetItem(int item,CStringArray *pstrArray,CString *strReturn);
	BOOL GetEntry(int Entry,CString str,CString *strReturn);
	BOOL Get(int ItemInStrArray,int EntryInString,CStringArray *strArray,CString *strReturn);

#ifdef WORKAROUND_BUG_0106
	void ReplaceReservedChar(CString* pString);
#endif


public:
	CExportGenerateTables();
	virtual ~CExportGenerateTables();
};

////////////////////////////////////////////////////////////////////////////////////

class CExportGenerateNodeTable : public CExportGenerateTables
{
	CStringArray strNodes;

public:
	CExportGenerateNodeTable();
	BOOL Generate_NodeTable();

	int GetNrItems(void)		{return (strNodes.GetSize()); }

	BOOL GetName(int item,CString *strReturn);
	BOOL GetPhysicalAddress(int item,CString *strReturn);
	BOOL GetLogicalAddress(int item,CString *strReturn);
	BOOL GetNrOfUnits(int item,CString *strReturn);
	BOOL GetNodeType(int item,CString *strReturn);
	BOOL GetSwVersion(int item,CString *strReturn);
	BOOL GetNrBindings(int item,CString *strReturn);
	BOOL GetNodeFlags(int item,CString *strReturn);
};

/////////////////////////////////////////////////////////////////////////////////////

// Sedert versie 0x0804 extra veld bProductID
class CExportGenerateNodeConfigTable : public CExportGenerateTables
{
	CStringArray m_strArray;

public:
	CExportGenerateNodeConfigTable();
	BOOL Generate_Table();

	int GetNrItems(void)		{return (m_strArray.GetSize()); }

	BOOL GetLogicalAddress(int item,CString *strReturn);
	BOOL GetProductID(int item,CString *strReturn);
	BOOL GetConfigFlags(int item,CString *strReturn);
	BOOL GetConfigUnitCreate(int item,CString *strReturn);
	BOOL GetConfigUnitCreateType(int item,CString* strReturn);
	BOOL GetConfigMMType(int item,CString *strReturn);
	BOOL GetConfigBitmapType(int item,CString *strReturn);
	BOOL GetConfigMemoryType(int item,CString *strReturn);
	BOOL GetConfigSmapType(int item,CString *strReturn);
	BOOL GetConfigScheduleType(int item,CString *strReturn);
	BOOL GetConfigAVSettingsType(int item,CString *strReturn);
	BOOL GetConfigSupportHttpInfo(int item,CString *strReturn);
};

////////////////////////////////////////////////////////////////////////////////////

class CExportGenerateUnitsTable : public CExportGenerateTables
{
	// Todo StrBindingUnits 
	CStringArray strBindingUnits;
	CStringArray strUnits;
	CString strNodeName,strPhysicalAddress,strLogicalAddress;

	BOOL FillTableUnitInfo(STRUCT_UNIT *pUnit);
	BOOL GetBindingUnitAddress(CString str,BYTE *bNodeAddress,BYTE *bUnitAddress);
	
	BOOL GetBindingItem(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *strReturn);

public:
	
	CExportGenerateUnitsTable();
	BOOL Generate_UnitsTable(unsigned long ulPhysicalAddress);
	BOOL Generate_BindingUnitsTable(unsigned long ulPhysicalAddress);


	int GetNrItems(void)		{return (strUnits.GetSize()); }

	BOOL GetName(int item,CString *strReturn);
	BOOL GetAddress(int item,CString *strReturn);
	BOOL GetType(int item,CString *strReturn);
	BOOL GetConfig(int item,CString *strReturn);

	CString GetNodeName(void)				{return strNodeName; };
	CString GetPhysicalAddress(void)		{return strPhysicalAddress; };
	CString GetLogicalAddress(void)			{return strLogicalAddress; };

	//Todo Functions to access BindingUnitsTable ...
	int GetNrBindingEntries(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL GetBindingName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str);
	BOOL GetBindingAction(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str);
	BOOL GetBindingEvent(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str);
	BOOL GetBindingOutput(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str);
	BOOL GetBindingID(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bEntry,CString *str);
};

////////////////////////////////////////////////////////////////////////////////////

class CExportGenerateBindingsTable : public CExportGenerateTables
{
	//int nEntries;
	int nCurrEntry;

	CStringArray strInputs,strOutputs,strTimers;
	CString strEvalString;
	CString strBindingName,strBindingNumber,strBindingID;
	CString strBindingFlags;

	BOOL GetUnitName(int Entry,CString *strReturn,CStringArray *strArray);
	BOOL GetUnitPhAddress(int Entry,CString *strReturn,CStringArray *strArray);
	BOOL GetUnitLogAddress(int Entry,CString *strReturn,CStringArray *strArray);
	BOOL GetUnitId(int Entry,CString *strReturn,CStringArray *strArray);
	BOOL GetUnitDescription(int Entry,CString *strReturn,CStringArray *strArray);

	BOOL AddInputUnit(CBindingUnit *pUnit);
	BOOL AddOutputUnit(CBindingUnit *pUnit);
	BOOL AddTimerOperator(CBindingOperator *pOperator);

	BOOL Generate(CBindingEntry* pEntry);
	

public:
	CExportGenerateBindingsTable();

	BOOL GetNextId(unsigned short *usNextId);	

	BOOL Generate_BindingTable(CBindingEntry* pEntry);
	BOOL Generate_BindingTable(unsigned short nEntry);

	CString GetEvalString(void)		{return strEvalString; };
	CString GetBindingName(void)	{return strBindingName; }
	CString GetBindingNumber(void)	{return strBindingNumber; }
	CString GetBindingID(void)		{return strBindingID; }
	CString GetBindingFlags(void)	{return strBindingFlags; };

	int GetNrInputs(void)		{return strInputs.GetSize(); };
	int GetNrOutputs(void)		{return strOutputs.GetSize(); };
	int GetNrTimers(void)		{return strTimers.GetSize(); };

	BOOL GetInputName(int Entry,CString *strReturn) {
		return (GetUnitName(Entry,strReturn,&strInputs));
	}
	BOOL GetInputPhAddress(int Entry,CString *strReturn) {
		return (GetUnitPhAddress(Entry,strReturn,&strInputs));
	}
	BOOL GetInputLogAddress(int Entry,CString *strReturn){
		return (GetUnitLogAddress(Entry,strReturn,&strInputs));
	}
	BOOL GetInputId(int Entry,CString *strReturn) {
		return (GetUnitId(Entry,strReturn,&strInputs));
	}
	BOOL GetInputDescription(int Entry,CString *strReturn) {
		return (GetUnitDescription(Entry,strReturn,&strInputs));
	}
//
	BOOL GetOutputName(int Entry,CString *strReturn) {
		return (GetUnitName(Entry,strReturn,&strOutputs));
	}
	BOOL GetOutputPhAddress(int Entry,CString *strReturn) {
		return (GetUnitPhAddress(Entry,strReturn,&strOutputs));
	}
	BOOL GetOutputLogAddress(int Entry,CString *strReturn){
		return (GetUnitLogAddress(Entry,strReturn,&strOutputs));
	}
	BOOL GetOutputId(int Entry,CString *strReturn) {
		return (GetUnitId(Entry,strReturn,&strOutputs));
	}
	BOOL GetOutputDescription(int Entry,CString *strReturn) {
		return (GetUnitDescription(Entry,strReturn,&strOutputs));
	}
//
	BOOL GetTimerType(int Entry,CString *strReturn);
	BOOL GetTimerValue(int Entry,CString *strReturn);
	BOOL GetTimerID(int Entry,CString *strReturn);
};
	

////////////////////////////////////////////////////////////////////////////////////

class CExportGenerateNodeUnitTable : public CExportGenerateTables
{
	CStringArray strNodeUnits;

public:
	CExportGenerateNodeUnitTable();
	BOOL Generate_NodeUnitTable();

	int GetNrItems(void)		{return (strNodeUnits.GetSize()); }

	BOOL GetLogicalAddress(int item,CString *strReturn);
	BOOL GetNodeName(int item,CString *strReturn);
	BOOL GetUnitAddress(int item,CString *strReturn);
	BOOL GetName(int item,CString *strReturn);
	BOOL GetType(int item,CString *strReturn);
	BOOL GetPhysicalAddress(int item,CString *strReturn);
};


#endif // !defined(AFX_EXPORTGENERATETABLES_H__5C3074E4_CC27_11D8_8499_0050BAC412B1__INCLUDED_)
