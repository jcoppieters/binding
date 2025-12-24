// ExportTextFilesApp.cpp: implementation of the CExportTextFilesApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ExportTextFilesApp.h"
#include "ExportGenerateTables.h"
#include "utils/app/DisplayDevice.h"
//////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalSettings;
using namespace TMBindingDatabase;
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExportTextFilesApp::CExportTextFilesApp()
{

}

CExportTextFilesApp::~CExportTextFilesApp()
{

}
void CExportTextFilesApp::StartExport(void)
{
	CString szDirectory;
	szDirectory.LoadString(IDS_FILENAME_SUBDIRCONFIG);

	if (1)
	{
		CString szName("nodedatabase.txt");
		CString szFilename;
		szFilename.Format("%s%s\\%s",TMGetFileSettings()->GetFullyQualifiedPathName(),szDirectory,szName);		

		if (1)
		{
			CString szInfo;
			szInfo.Format(_T("Save file : '%s'"),szFilename);
			CDisplayDevice::DisplayInfoMsg(szInfo);
		}

		FILE* const fs = fopen((const char*) szFilename,"w");
		if (fs != NULL) 
		{	
			exportNodes( fs );		
			fclose(fs);
		}
	}

	if (1)
	{
		CString szName("nodeconfiginfo.txt");
		CString szFilename;
		szFilename.Format("%s%s\\%s",TMGetFileSettings()->GetFullyQualifiedPathName(),szDirectory,szName);		

		if (1)
		{
			CString szInfo;
			szInfo.Format(_T("Save file : '%s'"),szFilename);
			CDisplayDevice::DisplayInfoMsg(szInfo);
		}

		FILE* const fs = fopen((const char*) szFilename,"w");
		if (fs != NULL) 
		{	
			exportNodesConfig( fs );		
			fclose(fs);
		}
	}

	if (1)
	{
		CString szName("bindingconfiginfo.txt");
		CString szFilename;
		szFilename.Format("%s%s\\%s",TMGetFileSettings()->GetFullyQualifiedPathName(),szDirectory,szName);		

		if (1)
		{
			CString szInfo;
			szInfo.Format(_T("Save file : '%s'"),szFilename);
			CDisplayDevice::DisplayInfoMsg(szInfo);
		}

		FILE* const fs = fopen((const char*) szFilename,"w");
		if (fs != NULL) 
		{	
			exportBindingsConfig( fs );		
			fclose(fs);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CExportTextFilesApp::exportUnits(FILE*fs,unsigned long phAddress)
{	
	CString szOutput;
	CString strUnitName;
	CString strUnitAddress;
	CString strUnitType;
	CString strUnitConfig;

	CExportGenerateUnitsTable unitsTable;
	unitsTable.Generate_UnitsTable(phAddress);
	
	const int units = unitsTable.GetNrItems();
	for (int i=0;i<units;i++)
	{
		unitsTable.GetName(i,&strUnitName);
		unitsTable.GetAddress(i,&strUnitAddress);
		unitsTable.GetType(i,&strUnitType);
		unitsTable.GetConfig(i,&strUnitConfig);

		if (1)	// UNIT=
		{
			szOutput.Format("UNIT=%d;\r\n",i);
			fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
		}

		if (1)	// szName=
		{
			szOutput.Format("szName=%s;\r\n",strUnitName);
			fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
		}

		if (1)	// bAddress
		{
			szOutput.Format("bAddress=%s;%s;\r\n",unitsTable.GetLogicalAddress(),strUnitAddress);
			fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
		}	

		if (1)	// szUnitType=
		{
			szOutput.Format("szUnitType=%s;\r\n",strUnitType);
			fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// bConfig=
		{
			szOutput.Format("bConfig=%s;\r\n",strUnitConfig);
			fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
		}
	}

	if (1)	
	{
		szOutput.Format(";********************************************\r\n");
		fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
	}
}

void CExportTextFilesApp::exportNodes(FILE* fs)
{
	CString szOutput;

	CString strPhysicalAddress;
	CString strNodeName;
	CString strNodeAddress;
	CString strNodeType;
	CString strNrOfUnits;
	CString strSoftwareVersion;
	CString strFlags;

	unsigned long ulPhysicalAddress;

	CExportGenerateNodeTable nodeTable;


	nodeTable.Generate_NodeTable();

	const int nodes = nodeTable.GetNrItems();
	for (int i=0;i<nodes;i++) 
	{
		nodeTable.GetPhysicalAddress(i,&strPhysicalAddress);
		nodeTable.GetName(i,&strNodeName);
		nodeTable.GetLogicalAddress(i,&strNodeAddress);
		nodeTable.GetNodeType(i,&strNodeType);
		nodeTable.GetNrOfUnits(i,&strNrOfUnits);
		nodeTable.GetNodeFlags(i,&strFlags);
		nodeTable.GetSwVersion(i,&strSoftwareVersion);

		if (1)	// NODE=
		{
			szOutput.Format("NODE=%d;\r\n",i);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// szName=
		{
			szOutput.Format("szName=%s;\r\n",strNodeName);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// ulPhysical=
		{
			szOutput.Format("ulPhysical=%s;\r\n",strPhysicalAddress);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// bNodeAddress=
		{
			szOutput.Format("bNodeAddress=%s;\r\n",strNodeAddress);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// szNodeType=
		{
			szOutput.Format("szNodeType=%s;\r\n",strNodeType);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// bNrOfUnits=
		{
			szOutput.Format("bNrOfUnits=%s;\r\n",strNrOfUnits);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// bFlags=
		{
			szOutput.Format("bFlags=%s;\r\n",strFlags);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// szSoftwareVersion
		{
			szOutput.Format("szSoftwareVersion=%s;\r\n",strSoftwareVersion);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}

		if (sscanf(strPhysicalAddress,"%x",&ulPhysicalAddress) == 1)
		{
			exportUnits(fs, ulPhysicalAddress );									          
		}
	}
}

void CExportTextFilesApp::exportNodesConfig(FILE* fs)
{
	CExportGenerateNodeConfigTable table;
	table.Generate_Table( );

	const int nrOfItems = table.GetNrItems( );
	for (int i=0;i<nrOfItems;i++)
	{
		// Sedert versie 0x0700: Extra veld voor bepalen schedule type
		// Sedert versie 0x0804: Extra veld voor product ID.
		// CHANGES_AV_UNIVERSAL: Extra veld voor bepalen van de unit create type.
		// Sedert versie 0x0F05: Extra veld voor config http
		CString szOutput;
		CString strNodeAddress;
		CString strProductID;
		CString strConfigFlags;
		CString strConfigUnitCreate;
		CString strConfigUnitCreateType;		// CHANGES_AV_UNIVERSAL
		CString strConfigMMtype;
		CString strConfigBitmapType;
		CString strConfigMemoryType;
		CString strConfigSmapType;
		CString strConfigScheduleType;
		CString strConfigAVSettingsType;
		CString strConfigSupportHttpInfo;
	
		table.GetLogicalAddress(i,&strNodeAddress);
		table.GetProductID(i,&strProductID);
		table.GetConfigFlags(i,&strConfigFlags);
		table.GetConfigUnitCreate(i,&strConfigUnitCreate);
		table.GetConfigUnitCreateType(i,&strConfigUnitCreateType);	// CHANGES_AV_UNIVERSAL 
		table.GetConfigMMType(i,&strConfigMMtype);
		table.GetConfigBitmapType(i,&strConfigBitmapType);
		table.GetConfigMemoryType(i,&strConfigMemoryType);		
		table.GetConfigSmapType(i,&strConfigSmapType);
		table.GetConfigScheduleType(i,&strConfigScheduleType);
		table.GetConfigAVSettingsType(i,&strConfigAVSettingsType);
		table.GetConfigSupportHttpInfo(i,&strConfigSupportHttpInfo);


		if (1)	// NODECONFIG = dec
		{
			szOutput.Format("NODECONFIG=%d;\r\n",i);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// bNodeAddress = hex
		{
			szOutput.Format("bNodeAddress=%s;\r\n",strNodeAddress);			
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// bProductID = hex
		{
			szOutput.Format("bProductID=%s;\r\n",strProductID);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// ulConfigFlags = hex ulong
		{
			szOutput.Format("ulConfigFlags=%s;\r\n",strConfigFlags);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// ulConfigUnitCreate = hex ulong
		{
			szOutput.Format("ulConfigUnitCreate=%s;\r\n",strConfigUnitCreate);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// CHANGES_AV_UNIVERSAL: Extra veld voor bepalen van de unit create type.
		{
			szOutput.Format("usConfigUnitCreateType=%s;\r\n",strConfigUnitCreateType);
			fputs(szOutput,fs);	
		}
		if (1)	// usConfigMMtype = hex ushort
		{
			szOutput.Format("usConfigMMtype=%s;\r\n",strConfigMMtype);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// usConfigBitmaptype = hex ushort
		{
			szOutput.Format("usConfigBitmaptype=%s;\r\n",strConfigBitmapType);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// usConfigMemorytype = hex ushort
		{
			szOutput.Format("usConfigMemorytype=%s;\r\n",strConfigMemoryType);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// usConfigScreenmaptype = hex ushort
		{
			szOutput.Format("usConfigScreenmaptype=%s;\r\n",strConfigSmapType);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// usConfigScheduletype = hex ushort 
		{
			szOutput.Format("usConfigScheduletype=%s;\r\n",strConfigScheduleType);
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
		if (1)	// usConfigAVSettingstype = hex ushort 
		{
			szOutput.Format("usConfigAVSettingstype=%s;\r\n",strConfigAVSettingsType);
			fputs(szOutput,fs);						
		}
		if (1)	// ulConfigSupporthttpApi = hex ulong
		{
			szOutput.Format("ulConfigSupporthttpApi=%s;\r\n",strConfigSupportHttpInfo);
			fputs(szOutput,fs);		
		}
		if (1)		
		{
			szOutput.Format(";********************************************\r\n");
			fputs(szOutput,fs);				// CHANGES_0X0A01_USE_FPUTS
		}
	}
}

void CExportTextFilesApp::exportBindingsConfig(FILE* fs)
{	
	CBindingEntries* const pBindingEntries=TMGetBindingEntries();
	
	if (0!=pBindingEntries)
	{	
		const int nEntries = pBindingEntries->Count();
		int i;
	
		for (i=0;i<nEntries;i++) 
		{			
			CBindingEntry* const pBindingEntry=pBindingEntries->GetEntry(i);
	
			if(0!=pBindingEntry)
			{								
				if(1)	// Binding=DEC;
				{
					CString szOutput;

					szOutput.Format("BINDING=%d;\r\n",i);
					fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
				}

				if(1)	// BindingNR=HEX;
				{
					CString szOutput;
					const int bindingNR=pBindingEntry->GetBindingNr();

					szOutput.Format("BindingNR=0x%04x;\r\n",bindingNR);
					fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
				}

				if(1)	// BindingFlags=HEX;
				{
					CString szOutput;
					const int bindingFlags=pBindingEntry->GetBindingReturnFlags();

					szOutput.Format("BindingFlags=0x%02x;\r\n",bindingFlags);
					fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
				}

				if(1)	// BindingStrEval='STRING';
				{
					CString szOutput;
					CString szEval(pBindingEntry->GetEvaluationString()->Get());

					szOutput.Format("BindingStrEval='%s';\r\n",szEval);
					fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
				}

				if(1)	// BindingStrName='STRING';
				{
					CString szOutput;
					CString szName(pBindingEntry->GetName());

					// BUG-0146: Is hier een probleem als de naam 'BADK3 20%  S' is.
					// Oplossing: zie CHANGES_0X0A01_USE_FPUTS

					szOutput.Format("BindingStrName='%s';\r\n",szName);
					fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
				}

				// Dumpen van de units...
				if (1)
				{
					int idx=0;
					CBindingUnits::Iterator it(pBindingEntry->GetColUnit()->CreateIterator());					

					for (it.Begin();!it.IsDone();it.Next())
					{					
						CBindingUnit* const pBindingUnit = it.CurrentItem();
						if (0!=pBindingUnit)
						{
							if (PM_INOUTTYPE_INPUT==pBindingUnit->GetInOutType()) 
							{
								if(1)	// Input=DEC;
								{
									CString szOutput;

									szOutput.Format("INPUT=%d;\r\n",idx);
									fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
								}			

								if(1)	// Id=DEC;
								{									
									CString szOutput;

									szOutput.Format("Id=%d;\r\n",pBindingUnit->GetID());
									fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
								}

								if(1)	// strName='STRING'
								{
									CString szOutput;
									CString szName(pBindingUnit->GetName());

									szOutput.Format("Name='%s';\r\n",szName);
									fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
								}

								if(1)	// Address=HEX;HEX;
								{									
									CString szOutput;

									szOutput.Format("Address=0x%02x;0x%02x;\r\n",
													pBindingUnit->GetNodeAddress(),
													pBindingUnit->GetUnitAddress());
									fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
								}

								if(1)	// Event=HEX;
								{
									CString szOutput;

									szOutput.Format("Event=0x%02x;\r\n",pBindingUnit->GetEvent());
									fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
								}
								
								if (1)	// Type=DEC;
								{
									CString szOutput;

									szOutput.Format("Type=%d;\r\n",pBindingUnit->GetBindingUnitType());
									fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
								}

								idx++;
							}							
						}
					}
				}

				if (1)
				{
					int idx=0;
					CBindingUnits::Iterator it(pBindingEntry->GetColUnit()->CreateIterator());
					for (it.Begin();!it.IsDone();it.Next())
					{
						CBindingUnit* const pBindingUnit = it.CurrentItem();
						ASSERT( pBindingUnit );

						if (PM_INOUTTYPE_OUTPUT==pBindingUnit->GetInOutType()) 
						{
							if(1)	// Output=DEC;
							{
								CString szOutput;

								szOutput.Format("OUTPUT=%d;\r\n",idx);
								fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
							}			

							if(1)	// Id=DEC;
							{							
								CString szOutput;

								szOutput.Format("Id=%d;\r\n",pBindingUnit->GetID());
								fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
							}

							if(1)	// Address=HEX;HEX;
							{									
								CString szOutput;

								szOutput.Format("Address=0x%02x;0x%02x;\r\n",
												pBindingUnit->GetNodeAddress(),
												pBindingUnit->GetUnitAddress());

								fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
							}

							if(1)	// Name='STRING'
							{
								CString szOutput;
								CString szName(pBindingUnit->GetName());

								szOutput.Format("Name='%s';\r\n",szName);
								fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
							}

							if (1)	// Type=DEC;
							{
								CString szOutput;

								szOutput.Format("Type=%d;\r\n",pBindingUnit->GetBindingUnitType());
								fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
							}

							// TODO - Functie...
							if (1)
							{														
								CBindingData* const pBindingData=&pBindingUnit->BindingData;
								if (0!=pBindingData)
								{
									CString szOutput;
									CString szData;
									CString szTemp;
								
									szTemp.Format("%02X",pBindingData->Get_MessageCode());
									szData += szTemp;

									for (int i=0;i<pBindingData->Get_Length();++i)
									{
										szTemp.Format("%02X",pBindingData->Get_Data(i));
										szData += szTemp;
									}

									szOutput.Format("Data=%s;\r\n",szData);
									fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
								}							
							}

							idx++;
						}
					}
				}

				if (1)
				{					
					CBindingOperators* const pBindingOperators = pBindingEntry->GetColOperator();
					if (0!=pBindingOperators)
					{						
						int idx=0;
						const int count=pBindingOperators->Count();

						for(int i=0;i<count;++i)
						{
							CBindingOperator* const pBindingOperator = pBindingOperators->Get(i);
							if (0!=pBindingOperator)
							{
								if(PM_OPERATOR_TIMER==pBindingOperator->GetType())
								{
									CBindingTimerOperator* const pBindingOperatorTimer=
										dynamic_cast<CBindingTimerOperator*>(pBindingOperator);

									if(0!=pBindingOperatorTimer)
									{
										if(1)	// timer=DEC;
										{
											CString szOutput;

											szOutput.Format("TIMER=%d;\r\n",idx);
											fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
										}

										if(1)
										{
											CString szOutput;

											szOutput.Format("Id=%d;\r\n",pBindingOperator->GetID());
											fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
										}

										if(1)
										{
											CString szOutput;
											szOutput.Format("Type=%d;\r\n",(int)pBindingOperatorTimer->GetTimerType());
											fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
										}

										if(1)
										{
											CString szOutput;
											szOutput.Format("TimerValue=%d;\r\n",(int)pBindingOperatorTimer->GetTimeValue());
											fputs(szOutput,fs);			// CHANGES_0X0A01_USE_FPUTS
										}

									}
									idx++;
								}
							}
						}
					}
				}



				// einde van de binding entry.
				if (1)		
				{
					CString szOutput;

					szOutput.Format(";********************************************\r\n");
					fputs(szOutput,fs);		// CHANGES_0X0A01_USE_FPUTS
				}

			} /* if(0!=pBindingEntry) */

		} /* for (i=0;i<nEntries;i++)  */
	}
}
