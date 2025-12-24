// ExportHtmlFiles.h: interface for the CExportHtmlFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTHTMLFILES_H__63F53E11_C68B_11D8_8499_0050BAC412B1__INCLUDED_)
#define AFX_EXPORTHTMLFILES_H__63F53E11_C68B_11D8_8499_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CExportHtmlFiles  
{
public:
	CExportHtmlFiles();
	virtual ~CExportHtmlFiles();
protected:

	void Write_IncStylesheet(FILE *fDest,CString strSylesheet);
	void Write_StartHtml(FILE *fDest);
	void Write_StopHtml(FILE *fDest);
	void Write_StartHead(FILE *fDest);
	void Write_StopHead(FILE *fDest);
	void Write_StartBody(FILE *fDest);
	void Write_StopBody(FILE *fDest);
	//void Write_StartAlinea(FILE *fDest,CString strFont,CString strMessage);
	void Write_StopAlinea(FILE *fDest);
	void Write_Title(FILE *fDest,CString szTitle);
	void Write_StartTable(FILE *fDest);
	void Write_StartTable(FILE *fDest,int width,int border);
	void Write_StartTable(FILE *fDest,BYTE procent,int border);
	void Write_StartMyTable(FILE *fDest,BYTE procent,int border);
	void Write_StopTable(FILE *fDest);
	void Write_StartRow(FILE *fDest);
	void Write_StartRow(FILE *fDest,CString strVAlign);
	void Write_StartRow(FILE *fDest,CString strVAlign,int Height);
	void Write_StopRow(FILE *fDest);
	void Write_ColHeader(FILE *fDest,CString szHeader);
	//void Write_ColHeader(FILE *fDest,CString szHeader,CString strfont);
	//void Write_ColHeader(FILE *fDest,CString szHeader,BYTE procent,CString strfont);
	void Write_StartColumn(FILE *fDest);
	void Write_Column(FILE *fDest,CString szColumn);
	//void Write_Column(FILE *fDest,CString szColumn,CString strfont);
	//void Write_Column(FILE *fDest,CString szColumn,CString strfont,BYTE rowspan);
	void Write_StopColumn(FILE *fDest);
	void Write_ColumnHyperLink(FILE *fDest,CString szColumn,CString szHyperLink);
	//void Write_ColumnHyperLink(FILE *fDest,CString szColumn,CString szHyperLink,CString strfont);
	void Write_StartList(FILE *fDest);
	void Write_StopList(FILE *fDest);
	void Write_ListItem(FILE *fDest,CString str);
};
///////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_EXPORTHTMLFILES_H__63F53E11_C68B_11D8_8499_0050BAC412B1__INCLUDED_)
