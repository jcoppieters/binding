#pragma once

#include "NodeDatabase.h"
#include "BindingFiles.h"

class CTimerDoc : public CDocument
{
protected: // create from serialization only
	CTimerDoc();
	DECLARE_DYNCREATE(CTimerDoc)

	// Aanpassing V16.16.
	// Mogen in de CreateFolder niet de SetPathName(strPathName, TRUE) aanroepen.
	// Moeten eerst alles opslaan + daarna SetPathName(strPathName, TRUE) aanroepen.
	// Extra parameter: pszPathName.
	BOOL CreateFolder(CString& szCurrentDirecytory, CString& szDestDir, CString& szFileTitle, CString* pszFullName, CString* pszPathName);
	BOOL SetActiveProjectDirectory(const CString& strPathName);

	BOOL CopyDirectories(CString szDestination,CString szSource);
	BOOL CopyDirectory(CString szDestination,CString szSource,CString szSubFolder,CString );

// Attributes
public:	
	BOOL OpenApplicationFile(CStringArray* pStrArrayError);
	BOOL ExportHtmlFiles(void);
	BOOL CreateAppSubDirectory(CString szSubDirectory);	
	BOOL SaveApplicationDirectory(void);
	BOOL getDataLoaded() { return m_fDataLoaded; }	// Sedert V16.10: gebruiken om te controleren of het een nieuw/bestaand project is.
	BOOL getDataSaved() { return m_fDataSaved; }	// sedert V16.11: Opossing BUG-0166

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL
// Implementation
public:
	virtual ~CTimerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void DeleteContents();

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTimerDoc)
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintSetup(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// zie BUG-0172
	// Bestanden die onder de 'CONFIG' subdirectory opgeslagen worden in een afzonderlijke functie.
	// Deze functie niet meer aanroepen in de Serialize() maar in de OnSave() en OnSaveAs()
	void saveAdditionalConfigFiles( );

private:
	// CHANGES_INCOMPATIBLE_VERSION
	// Bestand correct ingeladen = TRUE, anders FALSE.
	//
	// Sedert V16.10: Zorgt er ook voor om te bepalen of het een bestaand 
	// project of een nieuw project is.
	// Na het browsen wordt er bij een nieuw project gevraagd om deze op te slaan.
	BOOL m_fDataLoaded;

	// Sedert V16.11: Oplossing BUG-0166
	BOOL m_fDataSaved;	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
