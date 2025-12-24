// ExportTextFilesApp.h: interface for the CExportTextFilesApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTTEXTFILESAPP_H__311BDDEE_402F_463A_97B0_B36F41181995__INCLUDED_)
#define AFX_EXPORTTEXTFILESAPP_H__311BDDEE_402F_463A_97B0_B36F41181995__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExportTextFilesApp  
{
protected:
	void exportNodes(FILE*fs);
	void exportUnits(FILE*fs,unsigned long phAddress);
	void exportNodesConfig(FILE* fs);
	void exportBindingsConfig(FILE* fs);

public:
	CExportTextFilesApp();
	virtual ~CExportTextFilesApp();

	void StartExport(void);
};

#endif // !defined(AFX_EXPORTTEXTFILESAPP_H__311BDDEE_402F_463A_97B0_B36F41181995__INCLUDED_)
