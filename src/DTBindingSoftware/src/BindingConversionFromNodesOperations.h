// BindingConversionFromNodesOperations.h: interface for the CBindingConversionFromNodesOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERSIONFROMNODESOPERATIONS_H__5C01270A_71D0_4DC2_A88F_DA98539513B2__INCLUDED_)
#define AFX_BINDINGCONVERSIONFROMNODESOPERATIONS_H__5C01270A_71D0_4DC2_A88F_DA98539513B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingFiles;
class CBindingEntries;
class CBindingFilesOperations;
class CBindingPropertyUnitOperations;
class CBindingEntry;
class CBindingFileOperations;
class CBindingFile;
class CBindingUnit;
class CBindingUnits;
class CLogFileConversionFromNodes;



class CBindingConversionFromNodesOperations  
{
protected:
	CBindingFiles* const m_pBindingFiles;
	CBindingEntries* const m_pBindingEntries;
	CBindingFilesOperations* const m_pBindingFilesOperations;
	CBindingPropertyUnitOperations* const m_pBindingPropertyUnitOperations;
	CBindingFileOperations* m_pBindingFileOperation;

protected:
	CStringArray m_BindingStrings;
	CBindingUnits* m_pBindingUnits;
	unsigned short m_usManualBindingNr;
	unsigned short m_usFirstAvailableBindingNr;

	CLogFileConversionFromNodes* const m_pLogFile;

public:
	CBindingConversionFromNodesOperations(CBindingFiles* const pBindingFiles,
										  CBindingEntries* const pBindingEntries);
	virtual ~CBindingConversionFromNodesOperations();

protected:
	BOOL Add(CBindingEntry* pBindingEntry);
	BOOL UpgradePropertyUnit(const CBindingUnit* const pBindingUnit);
	BOOL ConvertPropertyString(CString szIn,CString &szOut,BOOL fProcessed);
	BOOL Replace(CStringArray &OldStrings,CStringArray &newStrings);
	BOOL Add(CStringArray& strArray,int index,CString newString);
	BOOL GetBindingUnits(CStringArray& strArray,CBindingUnits* pBindingUnits);
	BOOL ConvertToManual(CStringArray& strArray,CStringArray& strManualArray);

public:
	void Init();
	void Clear();
	
	int GetMaxBindingNr() const;
	int GetMaxBindingFiles() const;

	BOOL GetBindings(unsigned short usBindingNr);
	BOOL GetPropertyBindings(int iEntryNr);
	BOOL GetInstructionBindings(int iEntryNr);
	
	BOOL VerifySyntaxsis(int iEntryNr);
	BOOL VerifyAddresses(int iEntryNr);

	BOOL Process();
	BOOL ProcessPropertyBindings();
	BOOL UpgradePropertyBindings();

	BOOL SetDefaultNames();

	BOOL AdjustPBindings();
	BOOL AdjustIBindings();
};

#endif // !defined(AFX_BINDINGCONVERSIONFROMNODESOPERATIONS_H__5C01270A_71D0_4DC2_A88F_DA98539513B2__INCLUDED_)
