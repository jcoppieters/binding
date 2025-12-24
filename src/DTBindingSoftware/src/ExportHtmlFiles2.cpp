// ExportHtmlFiles2.cpp: implementation of the CExportHtmlFiles2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ExportHtmlFiles2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const CHtmlFontClass CExportHtmlFiles2::Fonts[HTML_FONTCLASS_MAX] =
{
	CHtmlFontClass("font_h1","font-family:Arial;font-weight:bold;font-size:10pt;color:black"),
	CHtmlFontClass("font_h2","font-family:Arial;font-weight:normal;font-size:9pt;color:black"),
	CHtmlFontClass("font_h3","font-family:Arial;font-weight:normal;font-size:8pt;color:black"),
	CHtmlFontClass("font_h4","font-family:Arial;font-weight:normal;font-size:8pt"),
	CHtmlFontClass("font_h5","font-family:Arial;font-weight:normal;font-size:12pt;color:red"),
	CHtmlFontClass("font_h6","font-family:Arial;font-weight:bold;font-size:10pt;color:black;text-decoration:underline")
};

CExportHtmlFiles2::CExportHtmlFiles2()
{

}

CExportHtmlFiles2::~CExportHtmlFiles2()
{

}

//////////////////////////////////////////////////////////////////////
void CExportHtmlFiles2::Write_StartAlinea(FILE *fDest,CString strFont,CString strMessage)
{
	CString strHtml;

	// BUG-0146
	strHtml.Format(_T("<p><span class=\"%s\">%s</span></p>\n"),
				  strFont,
				  strMessage);

	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles2::Write_ColHeader(FILE *fDest,CString szHeader,CString strfont) 
{
	CString strHtml;
	
	strHtml.Format("<th><span class=\"%s\">%s</span></th>\n",
				   strfont,
				   szHeader);

	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS	
}

void CExportHtmlFiles2::Write_ColHeader(FILE *fDest,CString szHeader,BYTE procent,CString strfont) 
{
	CString strHtml;
	strHtml.Format("<th width=\"%i%%%%\"><span class=\"%s\">%s</span></th>\n",
					procent,
					strfont,
					szHeader);

	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS	
}

void CExportHtmlFiles2::Write_Column(FILE *fDest,CString szColumn,CString strfont) 
{
	CString strHtml;
	strHtml.Format("<td><span class=\"%s\">%s</span></td>\n",
				   strfont,
				   szColumn);

	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS	
}

void CExportHtmlFiles2::Write_Column(FILE *fDest,CString szColumn,CString strfont,BYTE rowspan)
 {
	CString strHtml;

	if ((rowspan == 0) || (rowspan == 1)) 
	{
		strHtml.Format("<td><span class=\"%s\">%s</span></td>\n",
					   strfont,
					   szColumn);
	}
	else 
	{
		strHtml.Format("<td rowspan=\"%d\"><span class=\"%s\">%s</span></td>\n",
						rowspan,
						strfont,
						szColumn);
	}

	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS	
}

void CExportHtmlFiles2::Write_ColumnHyperLink(FILE *fDest,CString szColumn,CString szHyperLink,CString strfont) 
{
	CString strHtml;

	strHtml.Format("<td>\n");
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS

	strHtml.Format("<a href=\"%s\"><span class=\"%s\">%s</span></a>",
					szHyperLink,
					strfont,
					szColumn);

	// Zie BUG-0146
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS

	strHtml.Format("</td>\n");
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS
}