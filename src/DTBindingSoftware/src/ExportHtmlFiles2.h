// ExportHtmlFiles2.h: interface for the CExportHtmlFiles2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTHTMLFILES2_H__21F9BBB9_7131_4949_B170_71C2C63BA773__INCLUDED_)
#define AFX_EXPORTHTMLFILES2_H__21F9BBB9_7131_4949_B170_71C2C63BA773__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ExportHtmlFiles.h"
#include "HtmlFontClass.h"


/**
 *
 */

typedef enum
{
	HTML_FONTCLASS_H1		= 0,
	HTML_FONTCLASS_H2		= 1,
	HTML_FONTCLASS_H3       = 2,
	HTML_FONTCLASS_H4       = 3,
	HTML_FONTCLASS_H5		= 4,
	HTML_FONTCLASS_H6		= 5,
	HTML_FONTCLASS_MAX

}ENUM_HTML_FONTCLASS_NAMES;

/**
 *
 */

class CExportHtmlFiles2 : public CExportHtmlFiles  
{
protected:
	const static CHtmlFontClass Fonts[HTML_FONTCLASS_MAX];

public:
	CExportHtmlFiles2();
	virtual ~CExportHtmlFiles2();

	void Write_StartAlinea(FILE *fDest,CString strFont,CString strMessage);

	void Write_ColHeader(FILE *fDest,CString szHeader,CString strfont);
	void Write_ColHeader(FILE *fDest,CString szHeader,BYTE procent,CString strfont);

	void Write_Column(FILE *fDest,CString szColumn,CString strfont);
	void Write_Column(FILE *fDest,CString szColumn,CString strfont,BYTE rowspan);

	void Write_ColumnHyperLink(FILE *fDest,CString szColumn,CString szHyperLink,CString strfont);

};

#endif // !defined(AFX_EXPORTHTMLFILES2_H__21F9BBB9_7131_4949_B170_71C2C63BA773__INCLUDED_)
