// ExportHtmlFilesApp.h: interface for the CExportHtmlFilesApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTHTMLFILESAPP_H__1032B64F_EC23_4196_B825_2BAE778BEEF1__INCLUDED_)
#define AFX_EXPORTHTMLFILESAPP_H__1032B64F_EC23_4196_B825_2BAE778BEEF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 *
 */
#include "ExportHtmlFiles2.h"

class CExportHtmlFilesApp : public CExportHtmlFiles2
{
protected:
	void HandleFileIoError(CString szFileName);

	void Export_StyleSheet();
	void Export_NodeList();
	void Export_UnitLists();
	void Export_BindingList();
	void Export_NodeUnitList();

	#define FONT1()			(Fonts[HTML_FONTCLASS_H1].GetName())
	#define FONT2()			(Fonts[HTML_FONTCLASS_H2].GetName())
	#define FONT3()			(Fonts[HTML_FONTCLASS_H3].GetName())
	#define FONT4()			(Fonts[HTML_FONTCLASS_H4].GetName())
	#define FONT5()			(Fonts[HTML_FONTCLASS_H5].GetName())
	#define FONT6()			(Fonts[HTML_FONTCLASS_H6].GetName())

protected:
	CString szPathName;		
	CString szDirectory,szStyleSheet;
	CString szNodeList,szBindingList,szNodeUnitList;

public:
	CExportHtmlFilesApp();
	virtual ~CExportHtmlFilesApp();

	void StartExport(void);

	LPCSTR GetDestDirectory(void)
	{
		return szDirectory;	
	};

};

#endif // !defined(AFX_EXPORTHTMLFILESAPP_H__1032B64F_EC23_4196_B825_2BAE778BEEF1__INCLUDED_)
