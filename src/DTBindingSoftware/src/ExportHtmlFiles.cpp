// ExportHtmlFiles.cpp: implementation of the CExportHtmlFiles class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ExportHtmlFiles.h"

#include "MainFrm.h"
#include "ExportGenerateTables.h"
#include "BindingDatabaseApplication.h"

#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExportHtmlFiles::CExportHtmlFiles()
{

}

CExportHtmlFiles::~CExportHtmlFiles()
{

}

void CExportHtmlFiles::Write_StartHtml(FILE *fDest) 
{
	fputs("<html>\n",fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StopHtml(FILE *fDest) 
{
	fputs("</html>\n",fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StartHead(FILE *fDest) 
{
	fputs("<head>\n",fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StopHead(FILE *fDest) 
{
	fputs("</head>\n",fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StartBody(FILE *fDest) 
{
	fputs("<body>\n",fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StopBody(FILE *fDest) 
{
	fputs("</body>\n",fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StopAlinea(FILE *fDest) 
{
	fputs("</p>\n",fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_Title(FILE *fDest,CString szTitle) 
{
	CString strHtml;

	strHtml.Format("<title>%s</title>\n",szTitle);
	fputs(strHtml,fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StartTable(FILE *fDest) 
{	
	CString strHtml;

	strHtml.Format("<table>\n");	
	fputs(strHtml,fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StartTable(FILE *fDest,int width,int border) 
{	
	CString strHtml;

	strHtml.Format("<table width=\"%d%%%%\" border=\"%d\">\n",width,border);
	fputs(strHtml,fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StartTable(FILE *fDest,BYTE procent,int border) 
{
	CString strHtml;

	strHtml.Format("<table width=\"%d%%%%\" border=\"%d\">\n",procent,border);
	fputs(strHtml,fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StartMyTable(FILE *fDest,BYTE procent,int border) 
{
	CString strHtml;
	strHtml.Format("<table width=\"%d%%%%\" border=\"%d\"\n"
				   "style=\"table-layout:fixed;border-collapse:collapse\">\n",
				   procent,border);

	fputs(strHtml,fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StopTable(FILE *fDest) 
{
	fputs("</table>\n",fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StartRow(FILE *fDest) 
{
	fputs("<tr>\n",fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StartRow(FILE *fDest,CString strVAlign) 
{
	CString strHtml;

	strHtml.Format("<tr valign=\"%s\">\n",strVAlign);
	fputs(strHtml,fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StartRow(FILE *fDest,CString strVAlign,int Height) 
{
	CString strHtml;

	strHtml.Format("<tr valign=\"%s\" height=\"%i\">\n",strVAlign,Height);
	fputs(strHtml,fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StopRow(FILE *fDest) 
{
	fputs("</tr>\n",fDest);			// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_ColHeader(FILE *fDest,CString szHeader) 
{
	CString strHtml;

	strHtml.Format("<th>%s</th>\n",szHeader);
	fputs(strHtml,fDest);			// CHANGES_0X0A01_USE_FPUTS
}

/*
void CExportHtmlFiles::Write_ColHeader(FILE *fDest,CString szHeader,CString strfont) {
	CString strHtml;
	strHtml.Format("<th><%s>%s</%s></th>\n",strfont,szHeader,strfont);
	fprintf(fDest,strHtml);
}
*/
/*
void CExportHtmlFiles::Write_ColHeader(FILE *fDest,CString szHeader,BYTE procent,CString strfont) {
	CString strHtml;
	strHtml.Format("<th width=\"%i%%%%\"><%s>%s</%s></th>\n",
		procent,strfont,szHeader,strfont);
	fprintf(fDest,strHtml);
}
*/

void CExportHtmlFiles::Write_StartColumn(FILE *fDest) 
{
	CString strHtml;

	strHtml.Format("<td>\n");
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StopColumn(FILE *fDest) 
{
	CString strHtml;

	strHtml.Format("</td>\n");
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS

}

void CExportHtmlFiles::Write_Column(FILE *fDest,CString szColumn) 
{
	CString strHtml;

	strHtml.Format("<td>%s</td>\n",szColumn);
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS
}

/*
void CExportHtmlFiles::Write_Column(FILE *fDest,CString szColumn,CString strfont) 
{
	CString strHtml;
	strHtml.Format("<td><%s>%s<%s></td>\n",strfont,szColumn,strfont);
	fprintf(fDest,strHtml);
}

void CExportHtmlFiles::Write_Column(FILE *fDest,CString szColumn,CString strfont,BYTE rowspan)
 {
	CString strHtml;

	if ((rowspan == 0) || (rowspan == 1)) 
	{
		strHtml.Format("<td><%s>%s<%s></td>\n",strfont,szColumn,strfont);
	}
	else 
	{
		strHtml.Format("<td rowspan=\"%d\"><%s>%s<%s></td>\n",rowspan,strfont,szColumn,strfont);
	}

	fprintf(fDest,strHtml);
}

*/

void CExportHtmlFiles::Write_ColumnHyperLink(FILE *fDest,CString szColumn,CString szHyperLink) 
{
	CString strHtml;

	strHtml.Format("<td>\n");
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS

	strHtml.Format("<a href=\"%s\">%s</a>",szHyperLink,szColumn);
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS

	strHtml.Format("</td>\n");
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS
}
/*
void CExportHtmlFiles::Write_ColumnHyperLink(FILE *fDest,CString szColumn,CString szHyperLink,CString strfont) 
{
	CString strHtml;
	strHtml.Format("<td>\n");
	fprintf(fDest,strHtml);

	strHtml.Format("<a href=\"%s\"><%s>%s</%s></a>",szHyperLink,strfont,szColumn,strfont);
	fprintf(fDest,strHtml);

	strHtml.Format("</td>\n");
	fprintf(fDest,strHtml);
}
*/

void CExportHtmlFiles::Write_StartList(FILE *fDest) 
{
	fputs("<ul>\n",fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_StopList(FILE *fDest) 
{
	fputs("</ul>\n",fDest);		// CHANGES_0X0A01_USE_FPUTS
}
void CExportHtmlFiles::Write_ListItem(FILE *fDest,CString str) 
{
	CString strHtml;
	strHtml.Format("<li>%s</li>\n",str);
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS
}

void CExportHtmlFiles::Write_IncStylesheet(FILE *fDest,CString strStylesheet)
 {
	CString strHtml;

	strHtml.Format(_T("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\">\n"),
		strStylesheet);
	fputs(strHtml,fDest);		// CHANGES_0X0A01_USE_FPUTS
}

