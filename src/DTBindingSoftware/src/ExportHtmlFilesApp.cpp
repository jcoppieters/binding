// ExportHtmlFilesApp.cpp: implementation of the CExportHtmlFilesApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ExportHtmlFilesApp.h"

#include "MainFrm.h"
#include "ExportGenerateTables.h"

#include "utils/app/DisplayDevice.h"

// -------------------------------------
#include "TMGlobals.h"
using namespace TMBindingDatabase;
using namespace TMGlobalSettings;
// -------------------------------------

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExportHtmlFilesApp::CExportHtmlFilesApp()
{
/*	
	if (AfxGetMainWnd() != NULL)
	{
		szPathName = ((CMainFrame*) AfxGetMainWnd())->GetPathName();
	}
*/

	szPathName = TMGetFileSettings()->GetFullyQualifiedPathName();

	try
	{
		if (szDirectory.LoadString(IDS_FILENAME_SUBDIRHTMLFILES) == FALSE) 
		{	
			CString szError;
			szError.Format(_T("Unable to load string resource : IDS_FILENAME_SUBDIRHTMLFILES"));
			AfxMessageBox(szError);
		}
		
		if (szStyleSheet.LoadString(IDS_FILENAME_STYLESHEET) == FALSE) 
		{
			CString szError;
			szError.Format(_T("Unable to load string resource : IDS_FILENAME_STYLESHEET"));
			AfxMessageBox(szError);
		}
	
		if (szNodeList.LoadString(IDS_FILENAME_HTMLNODELIST) == FALSE) 
		{
			CString szError;
			szError.Format(_T("Unable to load string resource : IDS_FILENAME_HTMLNODELIST"));
			AfxMessageBox(szError);
		}
	
		if (szBindingList.LoadString(IDS_FILENAME_HTMLBINDINGLIST) == FALSE) 
		{
			CString szError;
			szError.Format(_T("Unable to load string resource : IDS_FILENAME_HTMLBINDINGLIST"));
			AfxMessageBox(szError);
		}
	
		if (szNodeUnitList.LoadString(IDS_FILENAME_HTMLNODEUNITLIST) == FALSE) 
		{
			CString szError;
			szError.Format(_T("Unable to load string resource : IDS_FILENAME_HTMLNODEUNITLIST"));
			AfxMessageBox(szError);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
	}
}

CExportHtmlFilesApp::~CExportHtmlFilesApp()
{

}


void CExportHtmlFilesApp::HandleFileIoError(CString szFileName)
{
	DWORD error = GetLastError();
	CString szError,szErrorMessage;

	switch (error) 
	{
		case ERROR_FILE_NOT_FOUND:
			szError.Format(_T("File Io Error : File not Found"));
		break;

		case ERROR_PATH_NOT_FOUND:
			szError.Format(_T("File Io Error : Path not Found"));
		break;

		case ERROR_TOO_MANY_OPEN_FILES:
			szError.Format(_T("File Io Error : Too many open Files"));
		break;

		case ERROR_ACCESS_DENIED:
			szError.Format(_T("File Io Error : Access denied : File is probably set as read only"));
		break;

		case ERROR_SHARING_VIOLATION:
			szError.Format(_T("File Io Error : Access denied : File is being used by another process"));
		break;
		
		default:
			szError.Format(_T("File Io Error - Error code %02x"),error);
		break;	
	}

	szErrorMessage.FormatMessage(_T("%1 :\n%2"),
								 szFileName,
								 szError);
	AfxMessageBox(szErrorMessage);
}

void CExportHtmlFilesApp::StartExport(void)
{
	CString szInfo;

	CDisplayDevice::Clear();

	szInfo.Format(_T("Start Exporting HTML Files ..."));
	CDisplayDevice::DisplayInfoMsg(szInfo);

	Export_StyleSheet();
	Export_NodeList();

	Export_UnitLists();
	Export_BindingList();
	Export_NodeUnitList();

	szInfo.Format(_T("Exporting HTML Files done..."));
	CDisplayDevice::DisplayInfoMsg(szInfo);
}

void CExportHtmlFilesApp::Export_StyleSheet(void)
{
	FILE *fHtmlFile;
	CString szFilename;

	szFilename.Format("%s%s\\%s",
					  szPathName,
					  szDirectory,
					  szStyleSheet);		
	
	if ((fHtmlFile = fopen((LPCTSTR) szFilename,"w")) != NULL)		// CHANGES_MVS_2008_CSTRING 
	{	
		CString str;
		str.Format("Exporting HTML stylesheet File : '%s'",szFilename);
		CDisplayDevice::DisplayInfoMsg(str);

		for (int i=0;i<(int) HTML_FONTCLASS_MAX;i++)
		{
			str.Format(".%s {%s}\n",Fonts[i].GetName(),
									Fonts[i].GetStyle());

			fputs(str,fHtmlFile);	// CHANGES_0X0A01_USE_FPUTS
		}

		str.Format(_T(".tablestyle {table-layout:auto;border-collapse:collapse }"));
		fputs(str,fHtmlFile);		// CHANGES_0X0A01_USE_FPUTS

		fclose(fHtmlFile);
	}
	else 
	{
		HandleFileIoError(szFilename);
	}
}

void CExportHtmlFilesApp::Export_NodeList(void)
{
	CExportGenerateNodeTable NodeTable;
	int nRows;

	CString szFilename;
	FILE *fHtmlFile;

	NodeTable.Generate_NodeTable();

	szFilename.Format("%s%s\\%s",
					  szPathName,
					  szDirectory,
					  szNodeList);
	

	if ((fHtmlFile = fopen((LPCTSTR) szFilename,"w")) != NULL)		// CHANGES_MVS_2008_CSTRING
	{
		CString strName,strPhysical,strLogical,strNrUnits;
		CString strType,strBindings,strSwVersion;

		CString str;
		str.Format("Exporting HTML File : '%s'",szFilename);
		CDisplayDevice::DisplayInfoMsg(str);

		str.Format("Duotecno Nodes List");		//HTML-Title 

		Write_StartHtml(fHtmlFile);
		Write_StartHead(fHtmlFile);
		Write_Title(fHtmlFile,str);
		Write_IncStylesheet(fHtmlFile,szStyleSheet);
		Write_StopHead(fHtmlFile);

		Write_StartBody(fHtmlFile);
	
		Write_StartMyTable(fHtmlFile,75,1);

		Write_StartRow(fHtmlFile,"Top",20);
	
		Write_ColHeader(fHtmlFile,"Name",FONT1());
		Write_ColHeader(fHtmlFile,"Type",FONT1());
		Write_ColHeader(fHtmlFile,"Ph. Addr.",FONT1());
		Write_ColHeader(fHtmlFile,"Log. Addr.",FONT1());
		Write_ColHeader(fHtmlFile,"Nr Units",FONT1());
		Write_ColHeader(fHtmlFile,"Nr Bindings",FONT1());
		Write_ColHeader(fHtmlFile,"SW ver.",FONT1());

		Write_StopRow(fHtmlFile);

		nRows = NodeTable.GetNrItems();
		for (int i=0;i<nRows;i++) 
		{	
			Write_StartRow(fHtmlFile,"Top",20);

			if (!NodeTable.GetName(i,&strName)) ASSERT(0);
			if (!NodeTable.GetPhysicalAddress(i,&strPhysical)) ASSERT(0);
			if (!NodeTable.GetLogicalAddress(i,&strLogical)) ASSERT(0);
			if (!NodeTable.GetNrOfUnits(i,&strNrUnits)) ASSERT(0);
			if (!NodeTable.GetNodeType(i,&strType)) ASSERT(0);
			if (!NodeTable.GetNrBindings(i,&strBindings)) ASSERT(0);
			if (!NodeTable.GetSwVersion(i,&strSwVersion)) ASSERT(0);

			str.Format("%s.html",strPhysical);	//HyperLink ...
			
			Write_ColumnHyperLink(fHtmlFile,strName,str,FONT4());
			Write_Column(fHtmlFile,strType,FONT2());
			Write_Column(fHtmlFile,strPhysical,FONT2());
			Write_Column(fHtmlFile,strLogical,FONT2());
			Write_Column(fHtmlFile,strNrUnits,FONT2());
			Write_Column(fHtmlFile,strBindings,FONT2());
			Write_Column(fHtmlFile,strSwVersion,FONT2());

			Write_StopRow(fHtmlFile);
		}
	
		Write_StopTable(fHtmlFile);
		Write_StopBody(fHtmlFile);
		Write_StopHtml(fHtmlFile);

		fclose(fHtmlFile);
	}
	else 
	{
		HandleFileIoError(szFilename);
	}
}

void CExportHtmlFilesApp::Export_UnitLists(void)
{
	CExportGenerateNodeTable NodeTable;
	CExportGenerateUnitsTable UnitsTable;
	int nNodes,i;
	int nUnits,j;
	int nBindings,t;
	unsigned long ulPhysicalAddress;
	BYTE bNodeAddress,bUnitAddress;
	CString szFilename;
	CString strPhysical,strNodeName,strNodeAddress;
	CString strUnitName,strUnitAddress,strUnitType,strBindingID;
	CString strBindingName,strBindingDescr,strBindingEvent,strBindingOutput;
	CString str;
	FILE *fHtmlFile;
	BOOL found = FALSE;


	NodeTable.Generate_NodeTable();

	nNodes = NodeTable.GetNrItems();
	for (i=0;i<nNodes;i++) 
	{		
		if (!NodeTable.GetPhysicalAddress(i,&strPhysical)) 
		{
			ASSERT(0);
		}

		if (sscanf(strPhysical,"%x",&ulPhysicalAddress) == 0)
		{
			ASSERT(0);
		}

		if (!UnitsTable.Generate_UnitsTable(ulPhysicalAddress)) 
		{
			ASSERT(0);
		}

		szFilename.Format("%s%s\\%s.html",szPathName,
										  szDirectory,
										  strPhysical);
	
		if ((fHtmlFile = fopen((LPCTSTR) szFilename,"w")) != NULL)			// CHANGES_MVS_2008_CSTRING
		{
			str.Format("Exporting HTML File : '%s'",szFilename);		
			CDisplayDevice::DisplayInfoMsg(str);	

			strPhysical = UnitsTable.GetPhysicalAddress();
			strNodeName = UnitsTable.GetNodeName();
			strNodeAddress = UnitsTable.GetLogicalAddress();

			Write_StartHtml(fHtmlFile);
			Write_StartHead(fHtmlFile);
			Write_Title(fHtmlFile,strPhysical);
			Write_IncStylesheet(fHtmlFile,szStyleSheet);
			Write_StopHead(fHtmlFile);

			Write_StartBody(fHtmlFile);

			Write_StartList(fHtmlFile);
			

			// !!!!
			str.Format("<span class=\"%s\">\n",FONT5());
			fputs(str,fHtmlFile);	// CHANGES_0X0A01_USE_FPUTS

			str.Format("Node Name : %s",strNodeName);
			Write_ListItem(fHtmlFile,str);
			str.Format("Physical Address : %s",strPhysical);
			Write_ListItem(fHtmlFile,str);
			str.Format("Logical Address : %s",strNodeAddress);
			Write_ListItem(fHtmlFile,str);
			
			// !!!!!
			str.Format("</span>\n");
			fputs(str,fHtmlFile);	// CHANGES_0X0A01_USE_FPUTS

			Write_StopList(fHtmlFile);

			Write_StartMyTable(fHtmlFile,90,1);
			Write_StartRow(fHtmlFile,"top",20);
			
			Write_ColHeader(fHtmlFile,"Name",15,FONT1());
			Write_ColHeader(fHtmlFile,"Id",5,FONT1());
			Write_ColHeader(fHtmlFile,"Type",8,FONT1());		
			Write_ColHeader(fHtmlFile,"Event",FONT1());
			Write_ColHeader(fHtmlFile,"Output",FONT1());
			Write_ColHeader(fHtmlFile,"Binding Description",FONT1());
			Write_ColHeader(fHtmlFile,"Binding Name",15,FONT1());

			Write_StopRow(fHtmlFile);

			nUnits = UnitsTable.GetNrItems();
			for (j=0;j<nUnits;j++) 
			{
				if (!UnitsTable.GetName(j,&strUnitName)) ASSERT(0);
				if (!UnitsTable.GetAddress(j,&strUnitAddress)) ASSERT(0);
				if (!UnitsTable.GetType(j,&strUnitType)) ASSERT(0);
				
				if (1)
				{
					int temp = -1;
					t = sscanf((LPCTSTR) strNodeAddress,"%x",&temp);	// CHANGES_MVS_2008_SSCANF + CHANGES_MVS_2008_CSTRING
					bNodeAddress = (BYTE) temp;
					ASSERT(t != -1);
				}

				if (1)
				{
					int temp = -1;
					t = sscanf((LPCTSTR) strUnitAddress,"%x",&temp);	// CHANGES_MVS_2008_SSCANF + CHANGES_MVS_2008_CSTRING
					bUnitAddress = (BYTE) temp;
					ASSERT(t != -1);
				}

				nBindings = UnitsTable.GetNrBindingEntries(bNodeAddress,bUnitAddress);
					
				Write_StartRow(fHtmlFile,"top",20);
				
				Write_Column(fHtmlFile,strUnitName,FONT2());
				Write_Column(fHtmlFile,strUnitAddress,FONT3());
				Write_Column(fHtmlFile,strUnitType,FONT3());

				if (nBindings == 0) 
				{					
					Write_Column(fHtmlFile,"--",FONT3());
					Write_Column(fHtmlFile,"--",FONT3());				
					Write_Column(fHtmlFile,"--",FONT3());
					Write_Column(fHtmlFile,"--",FONT3());	
					Write_StopRow(fHtmlFile);
				}
				else 
				{					
					for(t=0;t<nBindings;t++)
					{
						if (t>0) 
						{
							Write_StartRow(fHtmlFile,"top",20);
							Write_Column(fHtmlFile,"",FONT3());
							Write_Column(fHtmlFile,"",FONT3());
							Write_Column(fHtmlFile,"",FONT3());	
						}

						
						if (!UnitsTable.GetBindingEvent(bNodeAddress,bUnitAddress,
														t,&strBindingEvent))
								ASSERT(0);
						
						if (!UnitsTable.GetBindingOutput(bNodeAddress,bUnitAddress,
															  t,&strBindingOutput))
								ASSERT(0);

						if (!UnitsTable.GetBindingAction(bNodeAddress,bUnitAddress,
															 t,&strBindingDescr))
								ASSERT(0);

						if (!UnitsTable.GetBindingID(bNodeAddress,bUnitAddress,
														t,&strBindingID))
								ASSERT(0);

						if (!UnitsTable.GetBindingName(bNodeAddress,bUnitAddress,
														  t,&strBindingName)) 
								ASSERT(0);
						
						
						Write_Column(fHtmlFile,strBindingEvent,FONT3());
						Write_Column(fHtmlFile,strBindingOutput,FONT3());
						Write_Column(fHtmlFile,strBindingDescr,FONT3());
						str.Format("BindingList.html#%s",strBindingID);				//HyperLink
						Write_ColumnHyperLink(fHtmlFile,strBindingName,str,FONT4());

						if(t>0)  Write_StopRow(fHtmlFile);
					}
					
					if (t==0) Write_StopRow(fHtmlFile);
				}	
			}

			Write_StopTable(fHtmlFile);
			Write_StopBody(fHtmlFile);	
			fclose(fHtmlFile);
		}
		else 
		{
			HandleFileIoError(szFilename);
		}
	}
}

void CExportHtmlFilesApp::Export_BindingList(void)
{
	CExportGenerateBindingsTable BindingTable;
	CString strBindingName,strBindingNumber,strBindingEval;
	CString strBindingID;
	CString str,strData;
	CString strName,strAddress,strPhysical,strDescription,strID;
	CString strType,strValue;
	CString szFilename;
	FILE *fHtmlFile;
	int i,nEntries;
	int j,nUnits,nOperators;

	
	szFilename.Format("%s%s\\%s",szPathName,
								 szDirectory,
								 szBindingList);
	

	if ((fHtmlFile = fopen((LPCTSTR) szFilename,"w")) != NULL)	// CHANGES_MVS_2008_CSTRING
	{
		str.Format("Exporting HTML File : '%s'",szFilename);
		CDisplayDevice::DisplayInfoMsg(str);

		Write_StartHtml(fHtmlFile);
		Write_StartHead(fHtmlFile);
		Write_Title(fHtmlFile,"Binding lists");
		Write_IncStylesheet(fHtmlFile,szStyleSheet);
		Write_StopHead(fHtmlFile);

		Write_StartBody(fHtmlFile);
		
		nEntries = 0;
		if (TMGetBindingEntries() != NULL)
		{
			nEntries = TMGetBindingEntries()->Count();
		}

		for (i=0;i<nEntries;i++) 
		{
			BindingTable.Generate_BindingTable(i);
			strBindingName.Format(_T("%s"),BindingTable.GetBindingName());
			strBindingNumber.Format(_T("%s"),BindingTable.GetBindingNumber());
			strBindingEval.Format(_T("%s"),BindingTable.GetEvalString());



			str.Format(_T("<a name=\"%s\">Bindingname : %s</a>\n"),BindingTable.GetBindingID(),strBindingName);
			Write_StartAlinea(fHtmlFile,FONT6(),str);
			
			strData.Format(_T("BindingNumber : %s\n"),strBindingNumber);
			str.Format(_T("%s<br>\nEval : '%s'<br>\nBindingFlags : '%s'"),
				strData,strBindingEval,BindingTable.GetBindingFlags());
			
			Write_StartAlinea(fHtmlFile,FONT2(),str);		

			Write_StartMyTable(fHtmlFile,70,1);

			Write_StartRow(fHtmlFile,"Top",20);
			Write_ColHeader(fHtmlFile,"InputName",25,FONT1());
			Write_ColHeader(fHtmlFile,"Logical Id",15,FONT1());
			Write_ColHeader(fHtmlFile,"Physical Node",20,FONT1());
			Write_ColHeader(fHtmlFile,"Event",FONT1());
			Write_ColHeader(fHtmlFile,"Id",5,FONT1());
			Write_StopRow(fHtmlFile);

			nUnits=BindingTable.GetNrInputs();
			for (j=0;j<nUnits;j++) 
			{
				if(!BindingTable.GetInputName(j,&strName)) ASSERT(0);
				if(!BindingTable.GetInputLogAddress(j,&strAddress)) ASSERT(0);
				if(!BindingTable.GetInputPhAddress(j,&strPhysical)) ASSERT(0);
				if(!BindingTable.GetInputDescription(j,&strDescription)) ASSERT(0);
				if(!BindingTable.GetInputId(j,&strID)) ASSERT(0);
				
				Write_StartRow(fHtmlFile,"Top",20);
				Write_Column(fHtmlFile,strName,FONT3());
				Write_Column(fHtmlFile,strAddress,FONT3());
				Write_Column(fHtmlFile,strPhysical,FONT3());
				Write_Column(fHtmlFile,strDescription,FONT3());
				Write_Column(fHtmlFile,strID,FONT3());
				Write_StopRow(fHtmlFile);
			}
			


			Write_StartRow(fHtmlFile,"Top",20);	
			Write_ColHeader(fHtmlFile,"OutputName",25,FONT1());
			Write_ColHeader(fHtmlFile,"Logical Id",15,FONT1());
			Write_ColHeader(fHtmlFile,"Physical Node",20,FONT1());
			Write_ColHeader(fHtmlFile,"Function",FONT1());
			Write_ColHeader(fHtmlFile,"Id",5,FONT1());
			Write_StopRow(fHtmlFile);

			nUnits=BindingTable.GetNrOutputs();
			for (j=0;j<nUnits;j++) 
			{
				if(!BindingTable.GetOutputName(j,&strName)) ASSERT(0);
				if(!BindingTable.GetOutputLogAddress(j,&strAddress)) ASSERT(0);
				if(!BindingTable.GetOutputPhAddress(j,&strPhysical)) ASSERT(0);
				if(!BindingTable.GetOutputDescription(j,&strDescription)) ASSERT(0);
				if(!BindingTable.GetOutputId(j,&strID)) ASSERT(0);
				
				Write_StartRow(fHtmlFile,"Top",20);
				Write_Column(fHtmlFile,strName,FONT3());
				Write_Column(fHtmlFile,strAddress,FONT3());
				Write_Column(fHtmlFile,strPhysical,FONT3());
				Write_Column(fHtmlFile,strDescription,FONT3());
				Write_Column(fHtmlFile,strID,FONT3());
				Write_StopRow(fHtmlFile);
			}
			
			nOperators = BindingTable.GetNrTimers();
			if (nOperators != 0) 
			{
				// TODO TIMER OPERATORS + BINDINGFLAGS ...
				Write_StartRow(fHtmlFile,"Top",20);	
				Write_ColHeader(fHtmlFile,"Timer Type",25,FONT1());
				Write_ColHeader(fHtmlFile,"Timer Value",15,FONT1());
				Write_ColHeader(fHtmlFile,"",20,FONT1());
				Write_ColHeader(fHtmlFile,"",20,FONT1());				
				Write_ColHeader(fHtmlFile,"Id",5,FONT1());
				Write_StopRow(fHtmlFile);

				for(j=0;j<nOperators;j++) 
				{
					if(!BindingTable.GetTimerType(j,&strType)) ASSERT(0);
					if(!BindingTable.GetTimerValue(j,&strValue)) ASSERT(0);
					if(!BindingTable.GetTimerID(j,&strID)) ASSERT(0);

					Write_StartRow(fHtmlFile,"Top",20);
					Write_Column(fHtmlFile,strType,FONT3());
					Write_Column(fHtmlFile,strValue,FONT3());
					Write_Column(fHtmlFile,"",FONT3());
					Write_Column(fHtmlFile,"",FONT3());
					Write_Column(fHtmlFile,strID,FONT3());
					Write_StopRow(fHtmlFile);				
				}
			}

			Write_StopTable(fHtmlFile);
		}

		Write_StopBody(fHtmlFile);
		Write_StopHtml(fHtmlFile);

		fclose(fHtmlFile);
	}
	else 
	{
		HandleFileIoError(szFilename);
	}
}

void CExportHtmlFilesApp::Export_NodeUnitList(void)
{
	CExportGenerateNodeUnitTable NodeUnitTable;
	int nRows;

	CString szNodeAddress,szUnitAddress,szUnitName;
	CString szPhysicalAddress,szUnitType;
	CString szNodeName;
	
	CString szReference;

	CString str;
	CString szFilename;
	FILE *fHtmlFile;

	NodeUnitTable.Generate_NodeUnitTable();

	szFilename.Format("%s%s\\%s",szPathName,
								 szDirectory,
								 szNodeUnitList);


	if ((fHtmlFile = fopen((LPCTSTR) szFilename,"w")) != NULL)	// CHANGES_MVS_2008_CSTRING
	{
		str.Format("Exporting HTML File : '%s'",szFilename);
		CDisplayDevice::DisplayInfoMsg(str);

		str.Format("Duotecno Node Unit List");		//HTML-Title 

		Write_StartHtml(fHtmlFile);
		Write_StartHead(fHtmlFile);
		Write_Title(fHtmlFile,str);
		Write_IncStylesheet(fHtmlFile,szStyleSheet);
		Write_StopHead(fHtmlFile);

		Write_StartBody(fHtmlFile);
	
		Write_StartMyTable(fHtmlFile,75,1);

		Write_StartRow(fHtmlFile,"Top",20);
	
		Write_ColHeader(fHtmlFile,"NodeAddress",FONT1());
		Write_ColHeader(fHtmlFile,"UnitAddress",FONT1());
		Write_ColHeader(fHtmlFile,"UnitName",FONT1());
		Write_ColHeader(fHtmlFile,"Unit Type",FONT1());
		Write_ColHeader(fHtmlFile,"Physical Address",FONT1());
		Write_ColHeader(fHtmlFile,"_Reference",FONT1());

		Write_StopRow(fHtmlFile);

		nRows = NodeUnitTable.GetNrItems();
		for (int i=0;i<nRows;i++) 
		{	
			Write_StartRow(fHtmlFile,"Top",20);
			if (!NodeUnitTable.GetLogicalAddress(i,&szNodeAddress)) ASSERT(0);
			if (!NodeUnitTable.GetUnitAddress(i,&szUnitAddress)) ASSERT(0);
			if (!NodeUnitTable.GetName(i,&szUnitName)) ASSERT(0);
			if (!NodeUnitTable.GetType(i,&szUnitType)) ASSERT(0);
			if (!NodeUnitTable.GetPhysicalAddress(i,&szPhysicalAddress)) ASSERT(0);
			if (!NodeUnitTable.GetNodeName(i,&szNodeName)) ASSERT(0);

			str.Format("%s.html",szPhysicalAddress);	//HyperLink ...

			szReference.Format(_T("%s-%s,%s"),szNodeName,szNodeAddress,szUnitAddress);
			
			Write_Column(fHtmlFile,szNodeAddress,FONT2());
			Write_Column(fHtmlFile,szUnitAddress,FONT2());
			Write_Column(fHtmlFile,szUnitName,FONT2());
			Write_Column(fHtmlFile,szUnitType,FONT2());
			Write_ColumnHyperLink(fHtmlFile,szPhysicalAddress,str,FONT4());
			Write_Column(fHtmlFile,szReference,FONT2());

			Write_StopRow(fHtmlFile);
		}
	
		Write_StopTable(fHtmlFile);
		Write_StopBody(fHtmlFile);
		Write_StopHtml(fHtmlFile);

		fclose(fHtmlFile);
	}
	else {
		HandleFileIoError(szFilename);
	}
}
