// ApplicationFileSettings.h: interface for the CApplicationFileSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONFILESETTINGS_H__0B19E68C_B376_44FB_898B_78E923858F52__INCLUDED_)
#define AFX_APPLICATIONFILESETTINGS_H__0B19E68C_B376_44FB_898B_78E923858F52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDocument;

/*************************************************************//**
 * @class	CApplicationFileSettings 
 *
 * @changes	TM,v0563: CDTConfigurator klasse.
 * @changes	TM,v0564: m_strTitle bevat de titel met extzensie "*.duo"
 * @changes TM,v0642: CSensModbusFunctions 
 ****************************************************************/
class CApplicationFileSettings  
{
protected:
	CString m_strDefaultProjectDir;					// 'd:\developer\test installations\binding software installations\0x1010\'
	CString m_strFullyQualifiedFileName;			// 'd:\developer\test installations\binding software installations\0x1010\20220214 test\20220214 test.duo'
	CString m_strFullyQualifiedPathName;			// 'd:\developer\test installations\binding software installations\0x1010\20220214 test\'
	CString m_strTitle;								// '20220214 test.duo'
	CString m_strPreviousPath;
	CString m_strDirExamples;
	CString m_strDirHelp;
	CString m_strDirConfig;

	BOOL LoadDefaultActiveProjectDir();
	BOOL SaveDefaultActiveProjectDir();

public:
	/*********************** Nested class *************************/

	class CDTConfigurator
	{
	protected:
		CDTConfigurator( );

	public:
		~CDTConfigurator( );
		CString getFullPathName( ) const 
		{		
			return m_szFullPathName;
		}

		CString getWindowName( ) const
		{		
			return m_szWindowName;
		}		

	private:
		CString m_szWindowName;
		CString m_szFullPathName;

		friend class CApplicationFileSettings;
	};


	/************************* Nested class ************************/
	class CBindingManuals
	{
	public: 
		typedef enum {
			ID_MANUAL_BINDING_EXAMPLES = 0,
			ID_MANUAL_MODBUS_OVERVIEW,
			ID_MANUAL_HARDWARE,
			ID_MANUAL_SOFTWARE_PART1,
			ID_MANUAL_SOFTWARE_PART2,
			ID_MANUAL_SOFTWARE_EXPERT,			

		} Identifier_t;

		CBindingManuals(Identifier_t id);

		void init(const CString& dirManualsHelp,const CString& dirExamples);
		const CString getFullPathName( ) const;
		BOOL found( ) const {
			return m_found;
		}
	
	private:
		const Identifier_t m_id;
		BOOL m_found;
		static CString m_dirManualsHelp;
		static CString m_dirExamples;
	};

	/***************************************************************/

	CApplicationFileSettings();
	virtual ~CApplicationFileSettings();
	BOOL Load();
	BOOL Save();

	CString GetDefaultActiveProjectDirectory() const {
		return m_strDefaultProjectDir;
	}

	CString GetPreviousPathName() const {
		return m_strPreviousPath;
	}

	// Zie BUG-0168
	CString GetFullyQualifiedPathName() const;
	void setFullyQualifiedPathName(const CString& s);

	// Zie BUG-0168
	CString GetFullyQualifiedFileName() const;
	void SetFullyQualifiedFileName(const CString& s);

	CString GetTitle() const {	// geeft altijd de extensie "*.duo" terug.
		return m_strTitle;
	}
	CString GetDirExamples() const {
		return m_strDirExamples;
	}
	CString GetDirHelp() const {
		return m_strDirHelp;
	}
	CString GetDirConfig() const {
		return m_strDirConfig;
	}
	BOOL UpdateData(CDocument* pDoc,CString szActiveProjectDir = "");
	BOOL UpdateData(CDocument* pDoc,const CString& fileNameFullPath,const CString& fullPathName,const CString& projectDir,const CString& filetitle);	// V16.17
	void Clear();

	CDTConfigurator	dtConfigurator;	
	CBindingManuals m_bindingExamples;			// V16.29: aangepast
	CBindingManuals m_SensModbusFunctions;		// V16.29: aangepast
	CBindingManuals m_manualHardware;			// Sedert V16.29
	CBindingManuals m_manualSoftwarePart1;		// Sedert V16.29
	CBindingManuals m_manualSoftwarePart2;		// Sedert V16.29
	CBindingManuals m_manualSoftwareExpert;		// Sedert V16.29
};

/****************************************************************
 * class CApplicationFileAccess
 ****************************************************************/
class CApplicationFileAccess  
{
protected:
	BOOL m_fSaveAsAccess;

public:
	CApplicationFileAccess() : m_fSaveAsAccess(TRUE) { };
	virtual ~CApplicationFileAccess() {};

	BOOL Is_SaveAs_Enabled() const {
		return (m_fSaveAsAccess == TRUE);
	}
	void Set_SaveAs_Enable(BOOL fEnable) {
		m_fSaveAsAccess = fEnable;
	}
};

/*****************************************************************/

#endif // !defined(AFX_APPLICATIONFILESETTINGS_H__0B19E68C_B376_44FB_898B_78E923858F52__INCLUDED_)
