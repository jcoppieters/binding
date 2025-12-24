// ConfigMultiMediaClass.cpp: implementation of the CConfigMultiMediaClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigMultiMediaClass.h"
#include "ConfigMultiMediaFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
using DUOTECNO::MFC_HELPER::IReportError;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigMultiMediaClass::CConfigMultiMediaClass(
	IReportError *ptr, 
	const char* szHeaderID, 
	int MaxStructures) :
m_nMaxStructures( MaxStructures ),
m_pErrorReport( ptr ),
m_szHeaderID( szHeaderID ),
bCurrentIndex( 0 )
{
}

CConfigMultiMediaClass::~CConfigMultiMediaClass()
{
}

void CConfigMultiMediaClass::ReportError(CString s) 
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError((const char*) s);
	}
}

unsigned short CConfigMultiMediaClass::SwapUnsignedShort(unsigned short usValue)
{
  return((unsigned short)((unsigned short)((BYTE)usValue)*256+
						  (unsigned short)((BYTE)(usValue>>8))));
}

void CConfigMultiMediaClass::dump( ) //ostream& os)
{
	TRACE("TODO\n");
}

////////////////////////////////////////////////////////////////////////////

CConfigAudioRoom::CConfigAudioRoom(IReportError *ptr) : 
CConfigMultiMediaClass(ptr, "AUDIOROOM", NR_OF_ROOM_UNITS),
pCurrentRoom( 0 )
{
}

CConfigAudioRoom::~CConfigAudioRoom()
{
}

BOOL CConfigAudioRoom::AssignPointer(BYTE bIndex) 
{
	CString s;
	pCurrentRoom = NULL;

	if (bIndex < this->GetMaxStructures()) 
	{
		bCurrentIndex = bIndex;
		pCurrentRoom = &AudioRoom[bIndex];
		bAudioRoomUsed[bIndex] = 1;			//Set Room Used !
		return TRUE;
	}
	s.Format("Index Fault: Index:%d, max Audiorooms: %d",bIndex,this->GetMaxStructures());
	ReportError(s);
	return FALSE;
}

BOOL CConfigAudioRoom::ParseHeader(char *szAttribute,char *szData)
{
	int currentRoom = -1;

	if (strcmp(GetHeaderID(),szAttribute) == 0)			// CHANGES_MVS_2008_CSTRING
	{			
		if (sscanf(szData,"%x",&currentRoom) == 1)				// CHANGES_MVS_2008_SSCANF
		{		
			ASSERT(currentRoom >= 0 && currentRoom <= 0xFF);

			if (!AssignPointer(static_cast<BYTE>(currentRoom))) 
			{
				return FALSE;
			}

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigAudioRoom::ParseData(char *szAttribute,char *szData)
{
	unsigned int i;
	BYTE bNROfAttributes;
	CString s;
	int values[10];		// CHANGES_MVS_2008_SSCANF

	if (pCurrentRoom == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}

	// Here we can start parsing the audio Structure ...
	if (strcmp(szAttribute,"bSource") == 0)
	{		
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3],&values[4],&values[5],&values[6],&values[7]);	// CHANGES_MVS_2008_SSCANF                    		
		for (i=0;i<bNROfAttributes;i++) pCurrentRoom->bSrc[i]=(BYTE)values[i];		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bDestination") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);		// CHANGES_MVS_2008_SSCANF                
		for (i=0;i<bNROfAttributes;i++) pCurrentRoom->bDest[i]=(BYTE)values[i];			// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bDestinationLink") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3],&values[4],&values[5],&values[6],&values[7]);   // CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) pCurrentRoom->bDestLink[i]=(BYTE)values[i];		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bAudioConfig") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);			// // CHANGES_MVS_2008_SSCANF               
		if (bNROfAttributes) pCurrentRoom->bAudioConfig=(BYTE)values[0];	// // CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bAudioConfig") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);	// // CHANGES_MVS_2008_SSCANF                   
		if (bNROfAttributes) pCurrentRoom->bAudioConfig=(BYTE)values[0];	// // CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bIRReceiverAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;",&values[0],&values[1]);			// // CHANGES_MVS_2008_SSCANF             
		for (i=0;i<bNROfAttributes;i++) pCurrentRoom->bIRReceiverAddress[i]=(BYTE)values[i];	// // CHANGES_MVS_2008_SSCANF
	}
	else
	{	
		s.Format(_T("Attribute %s not a member of the %d audioroom structure"),szAttribute,bCurrentIndex);
		ReportError(s);
	}

	return TRUE;
}

void CConfigAudioRoom::Initialise(void) 
{
	int i,t;

	for (i=0;i<this->GetMaxStructures();i++)
	{
	   AudioRoom[i].bAudioConfig = 0;
       for(t=0;t<MAX_AUDIO_UNIT_SOURCES;t++) AudioRoom[i].bSrc[t]=NO_ASSIGNMENT;
       for(t=0;t<MAX_AUDIO_UNIT_DESTINATIONS;t++) AudioRoom[i].bDest[t]=NO_ASSIGNMENT;
       for(t=0;t<MAX_AUDIO_UNIT_DESTINATIONS;t++) AudioRoom[i].bDestLink[t]=NO_ASSIGNMENT;
	   AudioRoom[i].bReserved0=0;
	   AudioRoom[i].bReserved1=0;
	   AudioRoom[i].bReserved2=0;
	   AudioRoom[i].bReserved3=0;

       AudioRoom[i].bAudioStatus1=0;
       AudioRoom[i].bAudioStatus2=0;
       AudioRoom[i].bCurrentState=0;
	   AudioRoom[i].bPreviousState=0;

       AudioRoom[i].bDestCurSel=NO_ASSIGNMENT;
	   for(t=0;t<MAX_AUDIO_UNIT_DESTINATIONS;t++)
	   {
          AudioRoom[i].bDestSrcSel[t]=NO_ASSIGNMENT;
          AudioRoom[i].bDestVolume[t]=0;
	   }
       AudioRoom[i].bAudioStatus1 =0;

       AudioRoom[i].bIRReceiverAddress[NODE_ADR]=NODE_ADDRESS_INVALID;
       AudioRoom[i].bIRReceiverAddress[UNIT_ADR]=UNIT_ADDRESS_INVALID;

       bAudioRoomUsed[i]=0;

	} // for all audioroom units
}

int CConfigAudioRoom::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);
	int nBlocklength = 0;
	if (index < this->GetMaxStructures()) 
	{
		memcpy(pbData,&AudioRoom[index],sizeof(UNIT_AUDIOROOM_STRUCT));
		nBlocklength = sizeof(UNIT_AUDIOROOM_STRUCT);
		return nBlocklength;
	}
	pbData = NULL;
	return 0;
}

BOOL CConfigAudioRoom::GetData(int index,void **pvData)
{
	if (index < this->GetMaxStructures()) 
	{	
		*pvData = &AudioRoom[index];
		return TRUE;
	}
	return FALSE;
}


BOOL CConfigAudioRoom::IsUsed(int index)
{
	if (index < this->GetMaxStructures()) return ((BOOL) bAudioRoomUsed[index]);
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////

CConfigAudioDest_Base::CConfigAudioDest_Base(IReportError *ptr,int maxStructures) : 
CConfigMultiMediaClass(ptr, "DESTINATION", maxStructures),
m_pMatrixDest(0),
m_pCurrentDest(0),
m_pbDestinationUsed(0)
{	
	ASSERT(maxStructures>=0);

	m_pbDestinationUsed = new BYTE[maxStructures];
	for (int i=0;i<maxStructures;i++)
	{
		m_pbDestinationUsed[i] = 0;	
	}
	m_pMatrixDest = new STRUCT_MATRIX_AUDIO_UNIT_DESTINATION[maxStructures];
}
CConfigAudioDest_Base::~CConfigAudioDest_Base()
{
	if (m_pbDestinationUsed) {
		delete [ ] m_pbDestinationUsed;
		m_pbDestinationUsed=0;
	}
	if (m_pMatrixDest) {
		delete [ ] m_pMatrixDest;
		m_pMatrixDest=0;
	}
}

BOOL CConfigAudioDest_Base::AssignPointer(BYTE bIndex) 
{
	CString s;
	m_pCurrentDest = NULL;
	if (bIndex<this->GetMaxStructures()) 
	{
		bCurrentIndex = bIndex;
		m_pCurrentDest = &m_pMatrixDest[bIndex];
		m_pbDestinationUsed[bIndex]=1;			//Set Dest Used !
		return TRUE;
	}
	s.Format("Index Fault: Index:%d, max MatrixDest: %d",bIndex,this->GetMaxStructures());
	ReportError(s);
	return FALSE;
}

BOOL CConfigAudioDest_Base::ParseHeader(char *szAttribute,char *szData)
{
	int currentDest = -1;

	if (strcmp(GetHeaderID(),szAttribute) == 0)				// CHANGES_MVS_2008_CSTRING
	{				
		if (sscanf(szData,"%x",&currentDest))						// CHECKED_SSCANF_OK
		{			
			ASSERT(currentDest >= 0 && currentDest <= 0xFF);		// CHANGES_MVS_2008_WARNINGS

			if (!AssignPointer(static_cast<BYTE>(currentDest)))		// CHANGES_MVS_2008_WARNINGS
			{
				return FALSE;
			}

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigAudioDest_Base::ParseData(char *szAttribute,char *szData)
{
	unsigned int i;
	BYTE bNROfAttributes;
	CString s;
	int values[10];		

	if (m_pCurrentDest == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}

	// Here we can start parsing the audio Structure ...
	if (strcmp(szAttribute,"szDestinationName") == 0)
	{
		/* TM 05/02/2002 : allow only strings < 16 char's */
		sprintf(m_pCurrentDest->szDestinationName,"%.15s",szData);
	}
	else if (strcmp(szAttribute,"usVideoOn") == 0)
	{	
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);					          
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usVideoOn[i]=SwapUnsignedShort(values[i]);	
	}
	else if (strcmp(szAttribute,"usVideoOff") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);					            
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usVideoOff[i]=SwapUnsignedShort(values[i]);	
	}
	else if (strcmp(szAttribute,"usAudioOn") == 0)
	{		
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);					
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usAudioOn[i]=SwapUnsignedShort(values[i]);	
	}
	else if (strcmp(szAttribute,"usAudioOff") == 0)
	{		
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);					
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usAudioOff[i]=SwapUnsignedShort(values[i]);	
	}
	else if (strcmp(szAttribute,"usVideoCVInput") == 0)
	{	
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);					
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usVideoCVInput[i]=SwapUnsignedShort(values[i]);
	}
	else if (strcmp(szAttribute,"usVideoRGBInput") == 0)
	{		
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);			
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usVideoRGBInput[i]=SwapUnsignedShort(values[i]);
	}
    else if (strcmp(szAttribute,"usVideoSVHSInput") == 0)
	{		
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);				
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usVideoSVHSInput[i]=SwapUnsignedShort(values[i]);	
	}
	else if (strcmp(szAttribute,"usVideoDigitalInput") == 0)
	{		
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);							
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usVideoDigitalInput[i]=SwapUnsignedShort(values[i]);
	}
	else if (strcmp(szAttribute,"usAudioAnalogInput") == 0)
	{		
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);						
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usAudioAnalogInput[i]=SwapUnsignedShort(values[i]);
	}
	else if (strcmp(szAttribute,"usAudioDigitalInput") == 0)
	{	
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);						
		if (bNROfAttributes>4) bNROfAttributes=4;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->usAudioDigitalInput[i]=SwapUnsignedShort(values[i]);	
	}
	else if (strcmp(szAttribute,"bIRTXAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++;
		sscanf(szData,"%x;%x;",&values[0],&values[1]);
		if (bNROfAttributes>2) bNROfAttributes=2;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->bIRTXAddress[i]=(BYTE)values[i];
	}
	else if (strcmp(szAttribute,"bAudioOnOffAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++;
		sscanf(szData,"%x;%x;",&values[0],&values[1]);														
		if (bNROfAttributes>2) bNROfAttributes=2;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->bAudioOnOffAddress[i]=(BYTE)values[i];				
	}
	else if (strcmp(szAttribute,"bVideoOnOffAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;",&values[0],&values[1]);													
		if (bNROfAttributes>2) bNROfAttributes=2;		/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentDest->bVideoOnOffAddress[i]=(BYTE)values[i];
	}
	else if (strcmp(szAttribute,"bMaxVolume") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentDest->bMaxVolume=(BYTE)values[0];										// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bStartVolume") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentDest->bStartVolume=(BYTE)values[0];									// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bTreble") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentDest->bTreble=(BYTE)values[0];											// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bBass") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentDest->bBass=(BYTE)values[0];											// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bConfig") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentDest->bConfig=(BYTE)values[0];											// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bAVMatrixOutput") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentDest->bAVMatrixOutput=(BYTE)values[0];									// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bVideoMuxSrc") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentDest->bVideoMuxSrc=(BYTE)values[0];									// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bType") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentDest->bType=(BYTE)values[0];											// CHANGES_MVS_2008_SSCANF
	}
	else
	{
		s.Format(_T("Attribute %s not a member of the destination %d!"),szAttribute,bCurrentIndex);
		ReportError(s); 	 
	}	
	return FALSE;
}

void CConfigAudioDest_Base::Initialise(void) 
{
	int i,t;

	for (i=0;i<this->GetMaxStructures();i++)
	{
       sprintf(m_pMatrixDest[i].szDestinationName,"DESTINATION%d",i);

	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usVideoOn[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usVideoOff[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usAudioOn[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usAudioOff[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usVideoCVInput[t]=NO_IR_CODE_ID; 
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usVideoRGBInput[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usVideoSVHSInput[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usVideoDigitalInput[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usAudioAnalogInput[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixDest[i].usAudioDigitalInput[t]=NO_IR_CODE_ID;
       m_pMatrixDest[i].bIRTXAddress[NODE_ADR]=NODE_ADDRESS_INVALID;
       m_pMatrixDest[i].bIRTXAddress[UNIT_ADR]=UNIT_ADDRESS_INVALID;
       m_pMatrixDest[i].bAudioOnOffAddress[NODE_ADR]=NODE_ADDRESS_INVALID;
       m_pMatrixDest[i].bAudioOnOffAddress[UNIT_ADR]=UNIT_ADDRESS_INVALID;
       m_pMatrixDest[i].bVideoOnOffAddress[NODE_ADR]=NODE_ADDRESS_INVALID;
       m_pMatrixDest[i].bVideoOnOffAddress[UNIT_ADR]=UNIT_ADDRESS_INVALID;
       m_pMatrixDest[i].bMaxVolume=40;
       m_pMatrixDest[i].bStartVolume=0;
	   m_pMatrixDest[i].bVolume=0;
       m_pMatrixDest[i].bTreble=0;
       m_pMatrixDest[i].bBass=0;
       m_pMatrixDest[i].bConfig=0;
       m_pMatrixDest[i].bSourceSelection=NO_ASSIGNMENT;
       m_pMatrixDest[i].bPreviousSourceSelection=NO_ASSIGNMENT;
       m_pMatrixDest[i].bAVMatrixOutput=0xff;						/* TM20110818 */
       m_pMatrixDest[i].bVideoMuxSrc=0xff;
       m_pMatrixDest[i].bType=0x00;
       m_pMatrixDest[i].bReserved1=0x00;
       m_pMatrixDest[i].bReserved2=0x00;
       m_pMatrixDest[i].bReserved3=0x00;

	   m_pbDestinationUsed[i]=0;

	} // for all destinations
}

int CConfigAudioDest_Base::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);

	int nBlocklength = 0;

	if (index < this->GetMaxStructures()) 
	{
		memcpy(pbData,&m_pMatrixDest[index],sizeof(STRUCT_MATRIX_AUDIO_UNIT_DESTINATION));
		nBlocklength = sizeof(STRUCT_MATRIX_AUDIO_UNIT_DESTINATION);
		return nBlocklength;
	}
	pbData = NULL;
	return 0;
}

BOOL CConfigAudioDest_Base::GetData(int index,void **pvData)
{
	if (index < this->GetMaxStructures()) {	
		*pvData = &m_pMatrixDest[index];
		return TRUE;
	}
	return FALSE;
}


BOOL CConfigAudioDest_Base::IsUsed(int index)
{
	if (index < this->GetMaxStructures()) return ((BOOL)m_pbDestinationUsed[index]);
	return FALSE;
}

// -------------------------
// Klasse CConfigAudioDest
CConfigAudioDest::CConfigAudioDest(DUOTECNO::MFC_HELPER::IReportError *ptr) :
CConfigAudioDest_Base(ptr,NR_OF_MATRIX_DESTINATIONS)
{
}
CConfigAudioDest::~CConfigAudioDest()
{
}

// Klasse CConfigAudioDest_V65
CConfigAudioDest_V65::CConfigAudioDest_V65(DUOTECNO::MFC_HELPER::IReportError *ptr) :
CConfigAudioDest_Base(ptr,NR_OF_MATRIX_DESTINATIONS_V65)
{
}
CConfigAudioDest_V65::~CConfigAudioDest_V65()
{
}

// Klasse CConfigAudioDest_V66
CConfigAudioDest_V66::CConfigAudioDest_V66(DUOTECNO::MFC_HELPER::IReportError *ptr) :
CConfigAudioDest_Base(ptr,NR_OF_MATRIX_DESTINATIONS_V66)
{
}
CConfigAudioDest_V66::~CConfigAudioDest_V66()
{
}


////////////////////////////////////////////////////////////////////////////

CConfigAudioSource::CConfigAudioSource(IReportError *ptr, int maxStructures) : 
CConfigMultiMediaClass(ptr, "SOURCE", maxStructures),
m_pCurrentSource( 0 ),
m_pMatrixSrc( 0 ),
m_pbSourceUsed( 0 )
{	
	ASSERT(maxStructures>=0);

	m_pbSourceUsed = new BYTE[maxStructures];
	for (int i=0;i<maxStructures;i++)
	{
		m_pbSourceUsed[i] = 0;	
	}

	m_pMatrixSrc = new STRUCT_MATRIX_AUDIO_UNIT_SOURCE[maxStructures];
}

CConfigAudioSource::~CConfigAudioSource()
{
	if (m_pbSourceUsed)
	{
		delete [ ] m_pbSourceUsed;
	}
	if (m_pMatrixSrc)
	{
		delete [ ] m_pMatrixSrc;
	}
}

BOOL CConfigAudioSource::AssignPointer(BYTE bIndex) 
{
	CString s;

	m_pCurrentSource = NULL;
	if (bIndex < this->GetMaxStructures()) 
	{
		bCurrentIndex = bIndex;
		m_pCurrentSource = &m_pMatrixSrc[bIndex];
		m_pbSourceUsed[bIndex]=1;		// Set Source Used ...
		return TRUE;
	}
	s.Format("Index Fault: Index:%d, max MatrixSrc: %d",bIndex,this->GetMaxStructures());
	ReportError(s);
	return FALSE;
}

BOOL CConfigAudioSource::ParseHeader(char *szAttribute,char *szData)
{
	int currentSrc = -1;

	if (strcmp(GetHeaderID(),szAttribute) == 0)	// CHANGES_MVS_2008_CSTRING
	{		
		if (sscanf(szData,"%x",&currentSrc))			// CHECKED_SSCANF_OK
		{			
			if (!AssignPointer(currentSrc)) 
			{
				return FALSE;
			}

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigAudioSource::ParseData(char *szAttribute,char *szData)
{
	unsigned int i,j;
	BYTE bNROfAttributes;
	CString s;
	int values[10];		// CHANGES_MVS_2008_SSCANF

	if (m_pCurrentSource == NULL) 
	{
		// ASSERT(0);
		return FALSE;
	}

	if (strcmp(szAttribute,"szSourceName") == 0)
	{
		/* TM 05/02/2002 : allow only strings < 16 char's */
		sprintf(m_pCurrentSource->szSourceName,"%.15s",szData);
	}
	else if (strcmp(szAttribute,"usOn") == 0)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);					// CHANGES_MVS_2008_SSCANF                   
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usOn[i]=SwapUnsignedShort(values[i]);		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"usOff") == 0)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);				       // CHANGES_MVS_2008_SSCANF             
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usOff[i]=SwapUnsignedShort(values[i]);		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"usPlay") == 0)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);						// CHANGES_MVS_2008_SSCANF              
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usPlay[i]=SwapUnsignedShort(values[i]);		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"usStop") == 0)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);						// CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usStop[i]=SwapUnsignedShort(values[i]);		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"usPause") == 0)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);					// CHANGES_MVS_2008_SSCANF                 
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usPause[i]=SwapUnsignedShort(values[i]);	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"usRR") == 0)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);					// CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usRR[i]=SwapUnsignedShort(values[i]);		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"usFF") == 0)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]); 				    // CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usFF[i]=SwapUnsignedShort(values[i]);		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"usFrwd") == 0)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);						// CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usFrwd[i]=SwapUnsignedShort(values[i]);		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"usFfwd") == 0)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);       				// CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usFfwd[i]=SwapUnsignedShort(values[i]);		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bPlayControlAddress") == 0)
	{
		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;",&values[0],&values[1]);												// CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->bPlayControlAddress[i]=(BYTE)values[i];		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bIRTXAddress") == 0)
	{
		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;",&values[0],&values[1]);												// CHANGES_MVS_2008_SSCANF               
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->bIRTXAddress[i]=(BYTE)values[i];			// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bConfig") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);															// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentSource->bConfig=(BYTE)values[0];								// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bAVMatrixInput") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);															// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentSource->bAVMatrixInput=(BYTE)values[0];						// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bType") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);															// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) m_pCurrentSource->bType=(BYTE)values[0];									// CHANGES_MVS_2008_SSCANF
	}
	else if ((j=ParseExtraIR(szAttribute)) != -1)
	{
		// CHANGES_SSCANF_PARAM_4
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);						// CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSource->usExtraIR[j][i]=SwapUnsignedShort(values[i]);	// CHANGES_MVS_2008_SSCANF
	}
	else
	{
		s.Format(_T("Attribute %s not a member of the source %d!"),szAttribute,bCurrentIndex);
		ReportError(s);
		return FALSE;
	}	
	return TRUE;
}

void CConfigAudioSource::Initialise(void) 
{
	int i,t,j;

	// Initialise Sources
	for (i=0;i<this->GetMaxStructures();i++)
	{
       sprintf(m_pMatrixSrc[i].szSourceName,"SOURCE%d",i);
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usOn[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usOff[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usPlay[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usStop[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usPause[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usRR[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usFF[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usFrwd[t]=NO_IR_CODE_ID;
	   for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usFfwd[t]=NO_IR_CODE_ID;
       
	   for(j=0;j<EXTRAIRCODES;j++)  
	   {
	      for(t=0;t<MAX_IR_CODES;t++) m_pMatrixSrc[i].usExtraIR[j][t]=NO_IR_CODE_ID;
	   }

       m_pMatrixSrc[i].bPlayControlAddress[NODE_ADR]=NODE_ADDRESS_INVALID;
       m_pMatrixSrc[i].bPlayControlAddress[UNIT_ADR]=UNIT_ADDRESS_INVALID;
       m_pMatrixSrc[i].bIRTXAddress[NODE_ADR]=NODE_ADDRESS_INVALID;
       m_pMatrixSrc[i].bIRTXAddress[UNIT_ADR]=UNIT_ADDRESS_INVALID;
	   m_pMatrixSrc[i].bConfig=0x00;
       m_pMatrixSrc[i].bSourceStatus=0x00;
       m_pMatrixSrc[i].bAVMatrixInput=0xff;		/* TM20110818 */
       m_pMatrixSrc[i].bType=0x00;
       m_pMatrixSrc[i].bReserved1=0x00;
       m_pMatrixSrc[i].bReserved2=0x00;
       m_pMatrixSrc[i].bReserved3=0x00;

  	   m_pbSourceUsed[i]=0;
	} // for all sources
}

int CConfigAudioSource::ParseExtraIR(char *szAttribute)
{
	int value = -1;

	if (sscanf(szAttribute,"usExtraIR_%x",&value) != 0)		// CHANGES_MVS_2008_SSCANF
	{
		if ((value >= 0) && (value < EXTRAIRCODES)) 
		{
			return value;
		}   
	}
	return(-1);
} 

int CConfigAudioSource::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);
	int nBlocklength = 0;
	if (index < this->GetMaxStructures()) 
	{
		memcpy(pbData,&m_pMatrixSrc[index],sizeof(STRUCT_MATRIX_AUDIO_UNIT_SOURCE));
		nBlocklength = sizeof(STRUCT_MATRIX_AUDIO_UNIT_SOURCE);
		return nBlocklength;
	}
	pbData = NULL;
	return 0;
}

BOOL CConfigAudioSource::GetData(int index,void **pvData)
{
	if (index < this->GetMaxStructures()) 
	{	
		*pvData = &m_pMatrixSrc[index];
		return TRUE;
	}
	return FALSE;
}

BOOL CConfigAudioSource::IsUsed(int index)
{
	if (index < this->GetMaxStructures()) return ((BOOL) m_pbSourceUsed[index]);
	return FALSE;
}


/////////////////////////////////////////////////////////////////
// CConfigAudioSource_V64

CConfigAudioSource_V64::CConfigAudioSource_V64(DUOTECNO::MFC_HELPER::IReportError *ptr) :
CConfigAudioSource(ptr, NR_OF_MATRIX_SOURCES)
{
}

CConfigAudioSource_V64::~CConfigAudioSource_V64()
{
}

/////////////////////////////////////////////////////////////////
// CConfigAudioSource_V65
CConfigAudioSource_V65::CConfigAudioSource_V65(DUOTECNO::MFC_HELPER::IReportError *ptr) :
CConfigAudioSource(ptr, NR_OF_MATRIX_SOURCES_V65)
{
}

CConfigAudioSource_V65::~CConfigAudioSource_V65()
{
}

/////////////////////////////////////////////////////////////////
// CConfigAudioSource_V66
CConfigAudioSource_V66::CConfigAudioSource_V66(DUOTECNO::MFC_HELPER::IReportError *ptr) :
CConfigAudioSource(ptr, NR_OF_MATRIX_SOURCES_V66)
{
}

CConfigAudioSource_V66::~CConfigAudioSource_V66()
{
}

////////////////////////////////////////////////////////////////////////////

CConfigVideoMuxSource::CConfigVideoMuxSource(IReportError *ptr) : 
CConfigMultiMediaClass(ptr, "VIDEOMUXSOURCE", NR_OF_VIDEOMUXSOURCES),
pCurrentVideoMuxSrc( 0 )
{	
}

CConfigVideoMuxSource::~CConfigVideoMuxSource()
{
}

BOOL CConfigVideoMuxSource::AssignPointer(BYTE bIndex) 
{
	CString s;
	pCurrentVideoMuxSrc = NULL;

	if (bIndex < this->GetMaxStructures()) 
	{
		bCurrentIndex = bIndex;
		pCurrentVideoMuxSrc = &VideoMuxSrc[bIndex];
		bVideoMuxSrcUsed[bIndex]=1;		// Set Source Used ...
		return TRUE;
	}
	s.Format("Index Fault: Index:%d, max VideoMuxSrc: %d",bIndex,this->GetMaxStructures());
	ReportError(s);
	return FALSE;
}

BOOL CConfigVideoMuxSource::ParseHeader(char *szAttribute,char *szData)
{
	int currentVideoSrc = -1;

	if (strcmp(GetHeaderID(),szAttribute) == 0)	// CHANGES_MVS_2008_CSTRING
	{		
		if (sscanf(szData,"%x",&currentVideoSrc))		// CHECKED_SSCANF_OK
		{			
			if (!AssignPointer(currentVideoSrc)) 
			{
				return FALSE;
			}

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigVideoMuxSource::ParseData(char *szAttribute,char *szData)
{
	unsigned int i;
	BYTE bNROfAttributes;
	CString s;
	int values[10];				// CHANGES_MVS_2008_SSCANF

	if (pCurrentVideoMuxSrc == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}

	if (strcmp(szAttribute,"szVideoMuxSourceName") == 0)
	{
		/* TM 05/02/2002 : allow only strings < 16 char's */
		sprintf(pCurrentVideoMuxSrc->szVideoMuxSourceName,"%.15s",szData);
	}
	else if (strcmp(szAttribute,"bVideoMuxInput") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);												// CHANGES_MVS_2008_SSCANF           
		if (bNROfAttributes) pCurrentVideoMuxSrc->bVideoMuxInput=(BYTE)values[0];		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bConfig") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);												// CHANGES_MVS_2008_SSCANF                 
		if (bNROfAttributes) pCurrentVideoMuxSrc->bConfig=(BYTE)values[0];				// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bType") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);												// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) pCurrentVideoMuxSrc->bType=(BYTE)values[0];				// CHANGES_MVS_2008_SSCANF
	}
	else
	{
		 s.Format(_T("Attribute %s not a member of the muxsource %d!"),szAttribute,bCurrentIndex);
		 ReportError(s);
		 return FALSE;
	}
	return TRUE;
}

void CConfigVideoMuxSource::Initialise(void) 
{
	int i;
	for (i=0;i<this->GetMaxStructures();i++)
	{
       sprintf(VideoMuxSrc[i].szVideoMuxSourceName,"VIDEOMUXSRC%d",i);
       VideoMuxSrc[i].bVideoMuxInput=NO_ASSIGNMENT;
       VideoMuxSrc[i].bConfig=0x00;
       VideoMuxSrc[i].bType=0x00;
       VideoMuxSrc[i].bReserved1=0x00;
       VideoMuxSrc[i].bReserved2=0x00;
       VideoMuxSrc[i].bReserved3=0x00;

	   bVideoMuxSrcUsed[i]=0; 
	}
}

int CConfigVideoMuxSource::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);
	int nBlocklength = 0;
	if (index < this->GetMaxStructures()) 
	{
		memcpy(pbData,&VideoMuxSrc[index],sizeof(STRUCT_VIDEOMUXSOURCE));
		nBlocklength = sizeof(STRUCT_VIDEOMUXSOURCE);
		return nBlocklength;
	}
	pbData = NULL;
	return 0;
}

BOOL CConfigVideoMuxSource::GetData(int index,void **pvData)
{
	if (index < this->GetMaxStructures()) {	
		*pvData = &VideoMuxSrc[index];
		return TRUE;
	}
	return FALSE;
}

BOOL CConfigVideoMuxSource::IsUsed(int index)
{
	if (index < this->GetMaxStructures()) return ((BOOL) bVideoMuxSrcUsed[index]);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////

CConfigVideoMuxDest::CConfigVideoMuxDest(IReportError *ptr) : 
CConfigMultiMediaClass(ptr, "VIDEOMUXDESTINATION", NR_OF_VIDEOMUXDESTINATIONS),
pCurrentVideoMuxDest( 0 )
{
}

CConfigVideoMuxDest::~CConfigVideoMuxDest()
{
}

BOOL CConfigVideoMuxDest::AssignPointer(BYTE bIndex) 
{
	CString s;

	pCurrentVideoMuxDest = NULL;
	if (bIndex < this->GetMaxStructures()) 
	{
		bCurrentIndex = bIndex;
		pCurrentVideoMuxDest = &VideoMuxDest[bIndex];
		bVideoMuxDestUsed[bIndex]=1;		// Set Source Used ...
		return TRUE;
	}
	s.Format("Index Fault: Index:%d, max VideoMuxDest: %d",bIndex,this->GetMaxStructures());
	ReportError(s);
	return FALSE;
}

BOOL CConfigVideoMuxDest::ParseHeader(char *szAttribute,char *szData)
{
	int currentVideoSrc = -1;

	if (strcmp(GetHeaderID(),szAttribute) == 0)	// CHANGES_MVS_2008_CSTRING 
	{		
		if (sscanf(szData,"%x",&currentVideoSrc))		// CHECKED_SSCANF_OK
		{			
			if (!AssignPointer(currentVideoSrc)) 
			{
				return FALSE;
			}

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigVideoMuxDest::ParseData(char *szAttribute,char *szData)
{
	unsigned int i;
	BYTE bNROfAttributes;
	CString s;
	int values[10];					// CHANGES_MVS_2008_SSCANF

	if (pCurrentVideoMuxDest == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}
 
	if (strcmp(szAttribute,"szVideoMuxDestinationName") == 0)
	{
		/* TM 05/02/2002 : allow only strings < 16 char's */
		sprintf(pCurrentVideoMuxDest->szVideoMuxDestinationName,"%.15s",szData);
	}
	else if (strcmp(szAttribute,"bVideoMuxOutput") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);												// CHANGES_MVS_2008_SSCANF                
		if (bNROfAttributes) pCurrentVideoMuxDest->bVideoMuxOutput=(BYTE)values[0];		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bConfig") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);												// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) pCurrentVideoMuxDest->bConfig=(BYTE)values[0];				// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bVideoOnOffAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;",&values[0],&values[1]);													// CHANGES_MVS_2008_SSCANF          
		for (i=0;i<bNROfAttributes;i++) pCurrentVideoMuxDest->bVideoOnOffAddress[i]=(BYTE)values[i];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bAVMatrixSrc") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																// CHANGES_MVS_2008_SSCANF             
		if (bNROfAttributes) pCurrentVideoMuxDest->bAVMatrixSrc=(BYTE)values[0];						// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bType") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);																// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) pCurrentVideoMuxDest->bType=(BYTE)values[0];								// CHANGES_MVS_2008_SSCANF
	}
	else
	{
		s.Format(_T("Attribute %s not a member of the muxdestination %d!"),szAttribute,bCurrentIndex);
		ReportError(s);
		return FALSE;
	}
	return TRUE;
}

void CConfigVideoMuxDest::Initialise(void) 
{
   int i;
   for (i=0;i<this->GetMaxStructures();i++)
   {
       sprintf(VideoMuxDest[i].szVideoMuxDestinationName,"VIDEOMUXSRC%d",i);
       VideoMuxDest[i].bVideoMuxOutput=NO_ASSIGNMENT;
       VideoMuxDest[i].bVideoOnOffAddress[NODE_ADR]=NO_ASSIGNMENT;
       VideoMuxDest[i].bVideoOnOffAddress[UNIT_ADR]=NO_ASSIGNMENT;
	   VideoMuxDest[i].bAVMatrixSrc=NO_ASSIGNMENT;	
       VideoMuxDest[i].bConfig=0x00;
       VideoMuxDest[i].bState=0x00;
       VideoMuxDest[i].bType=0x00;
	   VideoMuxDest[i].bSourceSelection = NO_ASSIGNMENT;
	   VideoMuxDest[i].bPreviousSourceSelection = NO_ASSIGNMENT;
       VideoMuxDest[i].bReserved0=0x00;
       VideoMuxDest[i].bReserved1=0x00;
       VideoMuxDest[i].bReserved2=0x00;
       VideoMuxDest[i].bReserved3=0x00;

	   bVideoMuxDestUsed[i]=0;
	}
}

int CConfigVideoMuxDest::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);
	int nBlocklength = 0;
	if (index < this->GetMaxStructures()) {
		memcpy(pbData,&VideoMuxDest[index],sizeof(STRUCT_VIDEOMUXDESTINATION));
		nBlocklength = sizeof(STRUCT_VIDEOMUXDESTINATION);
		return nBlocklength;
	}
	pbData = NULL;
	return 0;
}

BOOL CConfigVideoMuxDest::GetData(int index,void **pvData)
{
	if (index < this->GetMaxStructures()) {	
		*pvData = &VideoMuxDest[index];
		return TRUE;
	}
	return FALSE;
}

BOOL CConfigVideoMuxDest::IsUsed(int index)
{
	if (index < this->GetMaxStructures()) return ((BOOL) bVideoMuxDestUsed[index]);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////

CConfigSpeechDevIndoor::CConfigSpeechDevIndoor(IReportError *ptr) : 
CConfigMultiMediaClass(ptr, "SPEECHDEVINDOOR", NR_OF_SPEECHDEVINDOORS),
pCurrentSpeechDevIndoor( 0 )
{
}

CConfigSpeechDevIndoor::~CConfigSpeechDevIndoor()
{

}

BOOL CConfigSpeechDevIndoor::AssignPointer(BYTE bIndex) 
{
	CString s;

	pCurrentSpeechDevIndoor = NULL;
	if (bIndex < this->GetMaxStructures()) 
	{
		bCurrentIndex = bIndex;
		pCurrentSpeechDevIndoor = &SpeechDevIndoor[bIndex];
		bSpeechDevIndoorUsed[bIndex]=1;		// Set Source Used ...
		return TRUE;
	}
	s.Format("Index Fault: Index:%d, max Speech device indoor: %d",bIndex,this->GetMaxStructures());
	ReportError(s);
	return FALSE;
}

BOOL CConfigSpeechDevIndoor::ParseHeader(char *szAttribute,char *szData)
{
	int currentSpeech = -1;

	if (strcmp(GetHeaderID(),szAttribute) == 0)	// CHANGES_MVS_2008_CSTRING
	{		
		if (sscanf(szData,"%x",&currentSpeech))			// CHECKED_SSCANF_OK
		{			
			if (!AssignPointer(currentSpeech))
			{
				return FALSE;
			}

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigSpeechDevIndoor::ParseData(char *szAttribute,char *szData)
{
	unsigned int i;
	BYTE bNROfAttributes;
	CString s;
	int values[10];		// CHANGES_MVS_2008_SSCANF

	if (pCurrentSpeechDevIndoor == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}
 
	if (strcmp(szAttribute,"szName") == 0)
	{
		 /* TM 05/02/2002 : allow only strings < 16 char's */
		 sprintf(pCurrentSpeechDevIndoor->szName,"%.15s",szData);
	}
	else if (strcmp(szAttribute,"bDestination") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]);				// CHANGES_MVS_2008_SSCANF                 
		for (i=0;i<bNROfAttributes;i++) pCurrentSpeechDevIndoor->bDest[i]=(BYTE)values[i];		// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bSwitchAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;",&values[0],&values[1]);												// CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) pCurrentSpeechDevIndoor->bSwitchAddress[i]=(BYTE)values[i];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bLCDMapAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);															// CHANGES_MVS_2008_SSCANF          
		if (bNROfAttributes) pCurrentSpeechDevIndoor->bLCDMapAddress=(BYTE)values[0];				// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bAVMatrixDest") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);															// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) pCurrentSpeechDevIndoor->bAVMatrixDest=(BYTE)values[0];				// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bConfig") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);															// CHANGES_MVS_2008_SSCANF            
		if (bNROfAttributes) pCurrentSpeechDevIndoor->bConfig=(BYTE)values[0];						// CHANGES_MVS_2008_SSCANF
	}
	else
	{
		s.Format(_T("Attribute %s not a member of the speechdevindoor %d!"),szAttribute,bCurrentIndex);
		ReportError(s);
		return FALSE;
	}
	return TRUE;
}

void CConfigSpeechDevIndoor::Initialise(void) 
{
   int i,t;
   for (i=0;i<this->GetMaxStructures();i++)
	{
       sprintf(SpeechDevIndoor[i].szName,"SPEECHINDOOR%d",i);
       for(t=0;t<MAX_MATRIX_DESTINATIONS;t++) SpeechDevIndoor[i].bDest[t]=NO_ASSIGNMENT;
       SpeechDevIndoor[i].bLCDMapAddress=NO_ASSIGNMENT;
	   SpeechDevIndoor[i].bAVMatrixDest=NO_ASSIGNMENT;
       SpeechDevIndoor[i].bSwitchAddress[NODE_ADR]=NO_ASSIGNMENT;
       SpeechDevIndoor[i].bSwitchAddress[UNIT_ADR]=NO_ASSIGNMENT;
       SpeechDevIndoor[i].bConfig=0x00;
       SpeechDevIndoor[i].bReserved1=0x00;
       SpeechDevIndoor[i].bReserved2=0x00;
       SpeechDevIndoor[i].bReserved3=0x00;
	   bSpeechDevIndoorUsed[i]=0;
	}
}

int CConfigSpeechDevIndoor::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);
	int nBlocklength = 0;
	if (index < this->GetMaxStructures()) 
	{
		memcpy(pbData,&SpeechDevIndoor[index],sizeof(STRUCT_SPEECHDEVINDOOR));
		nBlocklength = sizeof(STRUCT_SPEECHDEVINDOOR);
		return nBlocklength;
	}
	pbData = NULL;
	return 0;
}

BOOL CConfigSpeechDevIndoor::GetData(int index,void **pvData)
{
	if (index < this->GetMaxStructures()) {	
		*pvData = &SpeechDevIndoor[index];
		return TRUE;
	}
	return FALSE;
}

BOOL CConfigSpeechDevIndoor::IsUsed(int index)
{
	if (index < this->GetMaxStructures()) return ((BOOL) bSpeechDevIndoorUsed[index]);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
CConfigSpeechDevOutdoor_Base::CConfigSpeechDevOutdoor_Base(IReportError *ptr,int maxStructures) : 
CConfigMultiMediaClass(ptr, "SPEECHDEVOUTDOOR", maxStructures),
m_pSpeechDevOutdoor(0),
m_pCurrentSpeechDevOutdoor(0),
m_pbSpeechDevOutdoorUsed(0)
{	
	const int max = this->GetMaxStructures();

	m_pbSpeechDevOutdoorUsed = new BYTE[max];
	for (int i=0;i<max;i++) {
		m_pbSpeechDevOutdoorUsed[i] = 0;	
	}

	m_pSpeechDevOutdoor = new STRUCT_SPEECHDEVOUTDOOR[max];
}

CConfigSpeechDevOutdoor_Base::~CConfigSpeechDevOutdoor_Base()
{
	if (m_pbSpeechDevOutdoorUsed) {
		delete [] m_pbSpeechDevOutdoorUsed;
	}
	if (m_pSpeechDevOutdoor) {
		delete [] m_pSpeechDevOutdoor;
	}
}

BOOL CConfigSpeechDevOutdoor_Base::AssignPointer(BYTE bIndex) 
{
	CString s;
	m_pCurrentSpeechDevOutdoor = NULL;
	if (bIndex<this->GetMaxStructures()) 
	{
		bCurrentIndex = bIndex;
		m_pCurrentSpeechDevOutdoor = &m_pSpeechDevOutdoor[bIndex];
		m_pbSpeechDevOutdoorUsed[bIndex]=1;		// Set Source Used ...
		return TRUE;
	}
	s.Format("Index Fault: Index:%d, max Speech device outdoor: %d",bIndex,this->GetMaxStructures());
	ReportError(s);
	return FALSE;
}

BOOL CConfigSpeechDevOutdoor_Base::ParseHeader(char *szAttribute,char *szData)
{
	int currentSpeech = -1;

	if (strcmp(GetHeaderID(),szAttribute) == 0)	// CHANGES_MVS_2008_CSTRING
	{		
		if (sscanf(szData,"%x",&currentSpeech))			// CHECKED_SSCANF_OK
		{			
			if (!AssignPointer(currentSpeech))		
			{
				return FALSE;
			}
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigSpeechDevOutdoor_Base::ParseData(char *szAttribute,char *szData)
{
	unsigned int i;
	BYTE bNROfAttributes;
	CString s;
	int values[10];			

	if (m_pCurrentSpeechDevOutdoor == NULL) {
		ASSERT(0);
		return FALSE;
	}

	if (strcmp(szAttribute,"szName") == 0)
	{
		/* TM 05/02/2002 : allow only strings < 16 char's */
		sprintf(m_pCurrentSpeechDevOutdoor->szName,"%.15s",szData);
	}
	else if (strcmp(szAttribute,"bVideoMuxSrc") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;",&values[0]);									
		if (bNROfAttributes)m_pCurrentSpeechDevOutdoor->bVideoMuxSrc=(BYTE)values[0];	
	}
	else if (strcmp(szAttribute,"bAVMatrixSrc") == 0)  /* 17052001 */
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;",&values[0]);											         
		if (bNROfAttributes) m_pCurrentSpeechDevOutdoor->bAVMatrixSrc=(BYTE)values[0];	
	}
	else if (strcmp(szAttribute,"bSwitchAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;",&values[0],&values[1]);										      
		if (bNROfAttributes>2) bNROfAttributes=2;	/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSpeechDevOutdoor->bSwitchAddress[i]=(BYTE)values[i];	
	}
	else if (strcmp(szAttribute,"bDoorOpenerAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;",&values[0],&values[1]);
		if (bNROfAttributes>2) bNROfAttributes=2;	/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSpeechDevOutdoor->bDoorOpenerAddress[i]=(BYTE)values[i];
	}
	else if (strcmp(szAttribute,"bCallerAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;",&values[0],&values[1]);	
		if (bNROfAttributes>2) bNROfAttributes=2;	/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSpeechDevOutdoor->bCallerAddress[i]=(BYTE)values[i]; 
	}
	else if (strcmp(szAttribute,"bRingerAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;",&values[0],&values[1]);			
		if (bNROfAttributes>2) bNROfAttributes=2;	/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSpeechDevOutdoor->bRingerAddress[i]=(BYTE)values[i];	
	}
	// Oppassen: Nieuw veld sedert V66 nodes.
	// Wordt gebruikt om een extra deur te openen.
	else if (strcmp(szAttribute,"bFunctionOutput") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;",&values[0],&values[1]);
		if (bNROfAttributes>2) bNROfAttributes=2;	/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSpeechDevOutdoor->bFunctionOutput[i]=(BYTE)values[i];		
	}
	else if (strcmp(szAttribute,"bRefVideoPhone")==0)			
	{
		// absorbe: V66 param
	}
	else
	{
		 s.Format(_T("Attribute %s not a member of the speechdevoutdoor %d!"),szAttribute,bCurrentIndex);
		 ReportError(s);
		 return FALSE;
	}
	return TRUE;
}

void CConfigSpeechDevOutdoor_Base::Initialise(void) 
{
   int i; 
   const int max = this->GetMaxStructures();

   for (i=0;i<max;i++)
   {
       sprintf(m_pSpeechDevOutdoor[i].szName,"SPEECHOUTDOOR%d",i);
   	   m_pSpeechDevOutdoor[i].bVideoMuxSrc=NO_ASSIGNMENT;
       m_pSpeechDevOutdoor[i].bRingerAddress[NODE_ADR]=NO_ASSIGNMENT;
       m_pSpeechDevOutdoor[i].bRingerAddress[UNIT_ADR]=NO_ASSIGNMENT;
       m_pSpeechDevOutdoor[i].bDoorOpenerAddress[NODE_ADR]=NO_ASSIGNMENT;
       m_pSpeechDevOutdoor[i].bDoorOpenerAddress[UNIT_ADR]=NO_ASSIGNMENT;
       m_pSpeechDevOutdoor[i].bCallerAddress[NODE_ADR]=NO_ASSIGNMENT;
       m_pSpeechDevOutdoor[i].bCallerAddress[UNIT_ADR]=NO_ASSIGNMENT;
       m_pSpeechDevOutdoor[i].bSwitchAddress[NODE_ADR]=NO_ASSIGNMENT;
       m_pSpeechDevOutdoor[i].bSwitchAddress[UNIT_ADR]=NO_ASSIGNMENT;
       m_pSpeechDevOutdoor[i].bAVMatrixSrc=NO_ASSIGNMENT;
	   // TM 20181003: Nieuwe velden ipv de reserved var.
	   m_pSpeechDevOutdoor[i].bFunctionOutput[NODE_ADR]=NO_ASSIGNMENT;
	   m_pSpeechDevOutdoor[i].bFunctionOutput[UNIT_ADR]=NO_ASSIGNMENT;
	   m_pSpeechDevOutdoor[i].bRefVideoPhone=NO_ASSIGNMENT;
	   m_pbSpeechDevOutdoorUsed[i]=0;
	}
}

int CConfigSpeechDevOutdoor_Base::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);
	int nBlocklength = 0;
	if (index<this->GetMaxStructures()) {
		memcpy(pbData,&m_pSpeechDevOutdoor[index],sizeof(STRUCT_SPEECHDEVOUTDOOR));
		nBlocklength = sizeof(STRUCT_SPEECHDEVOUTDOOR);
		return nBlocklength;
	}
	pbData = NULL;
	return 0;
}

BOOL CConfigSpeechDevOutdoor_Base::GetData(int index,void **pvData)
{
	if (index<this->GetMaxStructures()) {	
		*pvData = &m_pSpeechDevOutdoor[index];
		return TRUE;
	}
	return FALSE;
}

BOOL CConfigSpeechDevOutdoor_Base::IsUsed(int index)
{
	if (index <this->GetMaxStructures()) 
		return ((BOOL)m_pbSpeechDevOutdoorUsed[index]);

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////

CConfigSpeechDevOutdoor::CConfigSpeechDevOutdoor(DUOTECNO::MFC_HELPER::IReportError *ptr) :
Base(ptr,NR_OF_SPEECHDEVOUTDOORS)
{
}
CConfigSpeechDevOutdoor::~CConfigSpeechDevOutdoor()
{
}

////////////////////////////////////////////////////////////////////////////
CConfigSpeechDevOutdoor_V65::CConfigSpeechDevOutdoor_V65(DUOTECNO::MFC_HELPER::IReportError *ptr) :
Base(ptr,NR_OF_SPEECHDEVOUTDOORS_V65)
{
}
CConfigSpeechDevOutdoor_V65::~CConfigSpeechDevOutdoor_V65()
{
}
////////////////////////////////////////////////////////////////////////////

CConfigSpeechDevOutdoor_V66::CConfigSpeechDevOutdoor_V66(DUOTECNO::MFC_HELPER::IReportError *ptr) :
Base(ptr,NR_OF_SPEECHDEVOUTDOORS_V66)		
{
}
CConfigSpeechDevOutdoor_V66::~CConfigSpeechDevOutdoor_V66()
{
}

BOOL CConfigSpeechDevOutdoor_V66::ParseData(char *szAttribute,char *szData)
{
	BOOL result=FALSE;
	int i;
	BYTE bNROfAttributes;
	int values[10];	

	if (m_pCurrentSpeechDevOutdoor == NULL) {
		ASSERT(0);
		return FALSE;
	}
	if (strcmp(szAttribute,"bRefVideoPhone") == 0)
	{
		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;",&values[0]);									
		if (bNROfAttributes)m_pCurrentSpeechDevOutdoor->bRefVideoPhone=(BYTE)values[0];	
		result=TRUE;
	}
#if(0)	// Moet ook in de V65 nodes geparsed worden - Nodig wnr deze de master is.
	else if (strcmp(szAttribute,"bFunctionOutput") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; 
		sscanf(szData,"%x;%x;",&values[0],&values[1]);
		if (bNROfAttributes>2) bNROfAttributes=2;	/* 20181003 */
		for (i=0;i<bNROfAttributes;i++) m_pCurrentSpeechDevOutdoor->bFunctionOutput[i]=(BYTE)values[i];
		result=TRUE;
	}
#endif
	else 
	{
		result=this->Base::ParseData(szAttribute,szData);
	}
	return result;
}


////////////////////////////////////////////////////////////////////////////

const char CConfigAlarmZoneNameUtil::AlarmZoneNr[GALAXYCENTRALEMAXALARMZONES][5] = 
{
	"1001","1002","1003","1004","1005","1006","1007","1008",
	"1011","1012","1013","1014","1015","1016","1017","1018",	
	"1021","1022","1023","1024","1025","1026","1027","1028",	
	"1031","1032","1033","1034","1035","1036","1037","1038",	
	"1041","1042","1043","1044","1045","1046","1047","1048",	
	"1051","1052","1053","1054","1055","1056","1057","1058",	
	"1061","1062","1063","1064","1065","1066","1067","1068",	
	"1071","1072","1073","1074","1075","1076","1077","1078",	
	"1081","1082","1083","1084","1085","1086","1087","1088",	
	"1091","1092","1093","1094","1095","1096","1097","1098",	
	"1101","1102","1103","1104","1105","1106","1107","1108",	
	"1111","1112","1113","1114","1115","1200","1117","1118",	
	"1121","1122","1123","1124","1125","1126","1127","1128",	
	"1131","1132","1133","1134","1135","1136","1137","1138",	
	"1141","1142","1143","1144","1145","1146","1147","1148",	
	"1151","1152","1153","1154","1155","1156","1157","1158",	
	"2001","2002","2003","2004","2005","2006","2007","2008",	
	"2011","2012","2013","2014","2015","2016","2017","2018",	
	"2021","2022","2023","2024","2025","2026","2027","2028",	
	"2031","2032","2033","2034","2035","2036","2037","2038",	
	"2041","2042","2043","2044","2045","2046","2047","2048",	
	"2051","2052","2053","2054","2055","2056","2057","2058",	
	"2061","2062","2063","2064","2065","2066","2067","2068",	
	"2071","2072","2073","2074","2075","2076","2077","2078",	
	"2081","2082","2083","2084","2085","2086","2087","2088",	
	"2091","2092","2093","2094","2095","2096","2097","2098"
};	


BOOL CConfigAlarmZoneNameUtil::GetAlarmZoneName(
	TypeAlarmCentrale type,
	int zoneNR,
	size_t maxLength,
	char* szName)
{
	if (zoneNR < GALAXYCENTRALEMAXALARMZONES)
	{
		char szTemp[16];

		switch(type)
		{
			case TYPE_ALARM_GALAXY:
			{				
				sprintf(szTemp,"Alarmzone %s", AlarmZoneNr[zoneNR]);
				if (strlen(szTemp)<maxLength-1)
				{
					strcpy(szName,szTemp);
					return TRUE;
				}
			
			} break;

			case TYPE_ALARM_TEXECOM:		/* TM_CHANGES_ALARM_TEXECOM */
			case TYPE_ALARM_ANB:			/* TM_CHANGES_ALARM_ANB */
			case TYPE_ALARM_ARITECH:
			case TYPE_ALARM_INTEGRA:		/* TM_CHANGES_ALARM_INTEGRA */ 
			{
				sprintf(szTemp,"Alarmzone %d", zoneNR+1);
				if (strlen(szTemp)<maxLength-1)
				{
					strcpy(szName,szTemp);
					return TRUE;
				}
			} break;
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
CConfigAlarmZone::CConfigAlarmZone(IReportError *ptr) : 
CConfigMultiMediaClass(ptr, "ALARMCONFIGZONE", GALAXYCENTRALEMAXALARMZONES),
pCurrentAlarmZone( 0 )
{	
}

CConfigAlarmZone::~CConfigAlarmZone()
{
}


BOOL CConfigAlarmZone::AssignPointer(BYTE bIndex) 
{
	CString s;
	pCurrentAlarmZone = NULL;

	if (bIndex < this->GetMaxStructures()) 
	{
		bCurrentIndex = bIndex;
		pCurrentAlarmZone = &AlarmConfigZone[bIndex];
		bAlarmConfigZoneUsed[bIndex]=1;		// Set Source Used ...
		return TRUE;
	}
	s.Format("Index Fault: Index:%d, max Alarm zones: %d",bIndex,this->GetMaxStructures());
	ReportError(s);
	return FALSE;
}

BOOL CConfigAlarmZone::ParseHeader(char *szAttribute,char *szData)
{
	int currentSpeech = -1;

	if (strcmp(GetHeaderID(),szAttribute) == 0)	// CHANGES_MVS_2008_CSTRING 
	{		
		if (sscanf(szData,"%x",&currentSpeech))			// CHECKED_SSCANF_OK
		{			
			if (!AssignPointer(currentSpeech))
			{
				return FALSE;
			}

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigAlarmZone::ParseData(char *szAttribute,char *szData)
{
	CString s;

	if (pCurrentAlarmZone == NULL) {
		ASSERT(0);
		return FALSE;
	}
	if (strcmp(szAttribute,"szName") == 0)
	{
		/* TM 05/02/2002 : allow only strings < 16 char's */
		 sprintf(pCurrentAlarmZone->szName,"%.15s",szData);
	}
	else
	{
		 s.Format(_T("Attribute %s not a member of the Alarm zone config %d!"),szAttribute,bCurrentIndex);
		 ReportError(s);
		 return FALSE;
	}

	return TRUE;
}

void CConfigAlarmZone::Initialise(void) 
{
   int i;
   for (i=0;i<this->GetMaxStructures();i++)
   {	   
		CConfigAlarmZoneNameUtil::GetAlarmZoneName(
			CConfigAlarmZoneNameUtil::TYPE_ALARM_GALAXY,
			i,
			ALARMZONENAME_LENGTH,
			AlarmConfigZone[i].szName);

/*		
		sprintf(AlarmConfigZone[i].szName,"AlarmZone %s",AlarmZoneNr[i]);
*/
	    
	    AlarmConfigZone[i].bStatus=0;
		bAlarmConfigZoneUsed[i]=0;
   }
}

int CConfigAlarmZone::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);
	int nBlocklength = 0;
	if (index < this->GetMaxStructures()) 
	{
		memcpy(pbData,&AlarmConfigZone[index],sizeof(STRUCT_ALARMZONE_CONFIG));
		nBlocklength = sizeof(STRUCT_ALARMZONE_CONFIG);
		return nBlocklength;
	}
	pbData = NULL;
	return 0;
}

BOOL CConfigAlarmZone::GetData(int index,void **pvData)
{
	if (index < this->GetMaxStructures()) 
	{	
		*pvData = &AlarmConfigZone[index];
		return TRUE;
	}
	return FALSE;
}

BOOL CConfigAlarmZone::IsUsed(int index)
{
	if (index < this->GetMaxStructures()) 
		return ((BOOL) bAlarmConfigZoneUsed[index]);

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////
CConfigAVMatrix::CConfigAVMatrix(IReportError *ptr) : 
CConfigMultiMediaClass(ptr, "AVMATRIXCONFIG", NR_OF_AVMATRIX_SYSTEMS),	/* TM_MULTIPLE_AVMATRIX */
pCurrentAVMatrix( 0 )
{	
}

CConfigAVMatrix::~CConfigAVMatrix()
{

}

/*
BOOL CConfigAVMatrix::AssignPointer(BYTE bIndex) 
{
	CString s;
	pCurrentAVMatrix = &ConfigAVMatrix;
	bCurrentIndex = 0;
	return TRUE;
}

BOOL CConfigAVMatrix::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp((LPCTSTR) szHeaderID,szAttribute) == 0)	// CHANGES_MVS_2008_CSTRING
	{					
		if (!AssignPointer(0)) return FALSE;
		return TRUE;		
	}
	return FALSE;
}

void CConfigAVMatrix::Initialise(void) 
{
    ConfigAVMatrix.bMatrixType=0x00;
    ConfigAVMatrix.bMatrixInputs=0x00;
    ConfigAVMatrix.bMatrixOutputs=0x00;
	ConfigAVMatrix.bMatrixOffsetInputs=0x00;			
	ConfigAVMatrix.bMatrixOffsetOutputs=0x00;			
    ConfigAVMatrix.bReserved3=0x00;
    ConfigAVMatrix.bReserved4=0x00;
    ConfigAVMatrix.bAVMatrixAddress[NODE_ADR]=NODE_ADDRESS_INVALID;
    ConfigAVMatrix.bAVMatrixAddress[UNIT_ADR]=NODE_ADDRESS_INVALID;
}

BOOL CConfigAVMatrix::GetData(int index,void **pvData)
{
	if (index == 0) 
	{	
		*pvData = &ConfigAVMatrix;
		return TRUE;
	}
	return FALSE;
}

*/

BOOL CConfigAVMatrix::AssignPointer(BYTE bIndex)		/* TM_MULTIPLE_AVMATRIX */
{	
	pCurrentAVMatrix = NULL;

	if (bIndex < this->GetMaxStructures()) 
	{
		bCurrentIndex = bIndex;
		pCurrentAVMatrix = &ConfigAVMatrix[bIndex];		
		return TRUE;
	}

	CString s;
	s.Format("Index Fault: Index:%d, max AVMatrix systems: %d",bIndex,this->GetMaxStructures());
	ReportError(s);

	return FALSE;	
}

BOOL CConfigAVMatrix::ParseHeader(char *szAttribute,char *szData)		/* TM_MULTIPLE_AVMATRIX */
{
	int currentAVMatrix = -1;

	if (strcmp(GetHeaderID(), szAttribute) == 0)				// CHANGES_MVS_2008_CSTRING 
	{		
		if (sscanf(szData,"%x",&currentAVMatrix) != 1)				// CHECKED_SSCANF_OK
		{		
			// Om compatible te blijven.
			// Soms wordt er geen waarde ingevuld dus zal deze altijd index 0 krijgen...
			currentAVMatrix = 0;	
		}

		if (!AssignPointer(currentAVMatrix))
		{
			return FALSE;
		}

		return TRUE;			
	}
	return FALSE;
	
}


BOOL CConfigAVMatrix::ParseData(char *szAttribute,char *szData)
{
	unsigned int i;
	BYTE bNROfAttributes;
	CString s;
	int values[10];			// CHANGES_MVS_2008_SSCANF

	if (pCurrentAVMatrix == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}
 
	if (strcmp(szAttribute,"szMatrixName") == 0)		/* TM 20110809 */
	{
		/* Absorbe, Nodig voor de config tool */	
	}
	else if (strcmp(szAttribute,"bMatrixType") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);	// CHANGES_MVS_2008_SSCANF                  
		if (bNROfAttributes) pCurrentAVMatrix->bMatrixType=(BYTE)values[0];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bMatrixInputs") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) pCurrentAVMatrix->bMatrixInputs=(BYTE)values[0];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bMatrixOutputs") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);	// CHANGES_MVS_2008_SSCANF                 
		if (bNROfAttributes) pCurrentAVMatrix->bMatrixOutputs=(BYTE)values[0]; // CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bAVMatrixAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;",&values[0],&values[1]);	// CHANGES_MVS_2008_SSCANF                   
		for (i=0;i<bNROfAttributes;i++) pCurrentAVMatrix->bAVMatrixAddress[i]=(BYTE)values[i];	// CHANGES_MVS_2008_SSCANF
	}	
	else if (strcmp(szAttribute,"bMatrixOffsetInputs") == 0)			/* TM_MULTIPLE_AVMATRIX */
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);	// CHANGES_MVS_2008_SSCANF                 
		if (bNROfAttributes) pCurrentAVMatrix->bMatrixOffsetInputs=(BYTE)values[0]; // CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bMatrixOffsetOutputs") == 0)			/* TM_MULTIPLE_AVMATRIX */
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);	// CHANGES_MVS_2008_SSCANF                 
		if (bNROfAttributes) pCurrentAVMatrix->bMatrixOffsetOutputs=(BYTE)values[0]; // CHANGES_MVS_2008_SSCANF
	}
	else
	{
		 s.Format(_T("Attribute %s not a member of the matrix config"),szAttribute);
		 ReportError(s);
		 return FALSE;
	}

	return TRUE;
}

void CConfigAVMatrix::Initialise(void)					/* TM_MULTIPLE_AVMATRIX */
{
	for (int i=0;i<NR_OF_AVMATRIX_SYSTEMS;i++)
	{
		ConfigAVMatrix[i].bMatrixType=0x00;
	    ConfigAVMatrix[i].bMatrixInputs=0x00;
	    ConfigAVMatrix[i].bMatrixOutputs=0x00;
		ConfigAVMatrix[i].bMatrixOffsetInputs=0x00;				/* TM_MULTIPLE_AVMATRIX */
		ConfigAVMatrix[i].bMatrixOffsetOutputs=0x00;			/* TM_MULTIPLE_AVMATRIX */  
	    ConfigAVMatrix[i].bReserved3=0x00;
	    ConfigAVMatrix[i].bReserved4=0x00;
	    ConfigAVMatrix[i].bAVMatrixAddress[NODE_ADR]=NODE_ADDRESS_INVALID;
	    ConfigAVMatrix[i].bAVMatrixAddress[UNIT_ADR]=NODE_ADDRESS_INVALID;	
	}
}

BOOL CConfigAVMatrix::GetData(int index,void **pvData)
{
	if (index < this->GetMaxStructures()) 
	{	
		*pvData = &ConfigAVMatrix[index];
		return TRUE;
	}
	return FALSE;
}

int CConfigAVMatrix::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);

	int nBlocklength = 0;
	if (index < this->GetMaxStructures()) 
	{
		memcpy(pbData,&ConfigAVMatrix[index],sizeof(STRUCT_CONFIGURATION_AVMATRIX));
		nBlocklength = sizeof(STRUCT_CONFIGURATION_AVMATRIX);
		return nBlocklength;
	}
	pbData = NULL;
	return 0;		
}

BOOL CConfigAVMatrix::IsUsed(int index)
{
	if (index<this->GetMaxStructures()) 
	{
		STRUCT_CONFIGURATION_AVMATRIX* const p = &ConfigAVMatrix[index];
		if (p)
		{
			// Oppassen:
			// Deze velden moeten juist ingesteld staan want anders zal de configuratie
			// niet juist doorgestuurd worden !!

			if ((p->bAVMatrixAddress[NODE_ADR] != NODE_ADDRESS_INVALID) &&
				(p->bAVMatrixAddress[UNIT_ADR] != UNIT_ADDRESS_INVALID) &&
				(p->bMatrixType != MATRIX_DISABLED))
			{
				return (TRUE);
			}
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
CConfigVideomux::CConfigVideomux(IReportError *ptr) : 
CConfigMultiMediaClass(ptr, "VIDEOMUXCONFIG", 1),
pCurrentVideoMux( 0 )
{	
}

CConfigVideomux::~CConfigVideomux()
{

}

BOOL CConfigVideomux::AssignPointer(BYTE bIndex) 
{
	CString s;
	pCurrentVideoMux = &ConfigVideoMux;
	bCurrentIndex = 0;
	return TRUE;
}

BOOL CConfigVideomux::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(GetHeaderID(),szAttribute) == 0)	// CHANGES_MVS_2008_CSTRING
	{					
		if (!AssignPointer(0)) return FALSE;
		return TRUE;		
	}
	return FALSE;
}


BOOL CConfigVideomux::ParseData(char *szAttribute,char *szData)
{
	unsigned int i;
	BYTE bNROfAttributes;
	CString s;
	int values[10];		// CHANGES_MVS_2008_SSCANF

	if (pCurrentVideoMux == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}
 
	if (strcmp(szAttribute,"bMuxType") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);		// CHANGES_MVS_2008_SSCANF                 
		if (bNROfAttributes) pCurrentVideoMux->bMuxType=(BYTE)values[0];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bMuxInputs") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);	// CHANGES_MVS_2008_SSCANF                   
		if (bNROfAttributes) pCurrentVideoMux->bMuxInputs=(BYTE)values[0];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bMuxOutputs") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);	// CHANGES_MVS_2008_SSCANF                    
		if (bNROfAttributes) pCurrentVideoMux->bMuxOutputs=(BYTE)values[0];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bVideoMuxAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;",&values[0],&values[1]);	// CHANGES_MVS_2008_SSCANF                    
		for (i=0;i<bNROfAttributes;i++) pCurrentVideoMux->bVideoMuxAddress[i]=(BYTE)values[i];	// CHANGES_MVS_2008_SSCANF
	}
	else
	{
		 s.Format(_T("Attribute %s not a member of the Videomux config"),szAttribute);
		 ReportError(s);
		 return FALSE;
	}
	return TRUE;
}

void CConfigVideomux::Initialise(void) 
{
    ConfigVideoMux.bMuxType=0x00;
    ConfigVideoMux.bMuxInputs=0x00;
    ConfigVideoMux.bMuxOutputs=0x00;
    ConfigVideoMux.bReserved1=0x00;
    ConfigVideoMux.bReserved2=0x00;
    ConfigVideoMux.bReserved3=0x00;
    ConfigVideoMux.bReserved4=0x00;
    ConfigVideoMux.bVideoMuxAddress[NODE_ADR]=NODE_ADDRESS_INVALID;
    ConfigVideoMux.bVideoMuxAddress[UNIT_ADR]=NODE_ADDRESS_INVALID;
}

int CConfigVideomux::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);
	int nBlocklength = 0;

	memcpy(pbData,&ConfigVideoMux,sizeof(STRUCT_CONFIGURATION_VIDEOMUX));
	nBlocklength = sizeof(STRUCT_CONFIGURATION_VIDEOMUX);
	return nBlocklength;
}

BOOL CConfigVideomux::GetData(int index,void **pvData)
{
	if (index == 0) 
	{	
		*pvData = &ConfigVideoMux;
		return TRUE;
	}

	return FALSE;
}

BOOL CConfigVideomux::IsUsed(int index)
{
	if (index<this->GetMaxStructures()) 
		return (TRUE);

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
CConfigVideophone::CConfigVideophone(IReportError *ptr) : 
CConfigMultiMediaClass(ptr, "VIDEOPHONECONFIG", 1),
pCurrentVideophone( 0 )
{	
}

CConfigVideophone::~CConfigVideophone()
{

}

BOOL CConfigVideophone::AssignPointer(BYTE bIndex) 
{
	CString s;
	pCurrentVideophone = &ConfigVideoPhone;
	bCurrentIndex = 0;
	return TRUE;
}

BOOL CConfigVideophone::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(GetHeaderID(),szAttribute) == 0)	// CHANGES_MVS_2008_CSTRING
	{					
		if (!AssignPointer(0)) return FALSE;
		return TRUE;		
	}
	return FALSE;
}


BOOL CConfigVideophone::ParseData(char *szAttribute,char *szData)
{
	unsigned int i;
	BYTE bNROfAttributes;
	CString s;
	int values[10];		// CHANGES_MVS_2008_SSCANF

	if (pCurrentVideophone == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}
 
	if (strcmp(szAttribute,"bCallTimeout") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);		// CHANGES_MVS_2008_SSCANF               
		if (bNROfAttributes) pCurrentVideophone->bCallTimeout=(BYTE)values[0];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bConnectTimeout") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);	// CHANGES_MVS_2008_SSCANF
		if (bNROfAttributes) pCurrentVideophone->bConnectTimeout=(BYTE)values[0];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bSpeechMtxAddress") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;%x;",&values[0],&values[1]);	// CHANGES_MVS_2008_SSCANF
		for (i=0;i<bNROfAttributes;i++) pCurrentVideophone->bSpeechMtxAddress[i]=(BYTE)values[i];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bVideoMuxDest") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);   // CHANGES_MVS_2008_SSCANF                 
		if (bNROfAttributes) pCurrentVideophone->bVideoMuxDest=(BYTE)values[0];	// CHANGES_MVS_2008_SSCANF
	}
	else if (strcmp(szAttribute,"bDropVolume") == 0)
	{
 		bNROfAttributes = 0;
		for (i=0;i<strlen(szData);i++) if (szData[i]==';') bNROfAttributes++; // search attributes
		sscanf(szData,"%x;",&values[0]);  // CHANGES_MVS_2008_SSCANF                  
		if (bNROfAttributes) pCurrentVideophone->bDropVolume=(BYTE)values[0];	// CHANGES_MVS_2008_SSCANF
	}
	else
	{
		 s.Format(_T("Attribute %s not a member of the Videophone config"),szAttribute);
		 ReportError(s);
		 return FALSE;
	}
	return TRUE;
}

void CConfigVideophone::Initialise(void) 
{
	ConfigVideoPhone.bCallTimeout=0x1E;  
    ConfigVideoPhone.bConnectTimeout=0x05; 
    ConfigVideoPhone.bSpeechMtxAddress[NODE_ADR]=NO_ASSIGNMENT;
    ConfigVideoPhone.bSpeechMtxAddress[UNIT_ADR]=NO_ASSIGNMENT;
    ConfigVideoPhone.bVideoMuxDest=0x00;
    ConfigVideoPhone.bDropVolume=0x00;
    ConfigVideoPhone.bReserved0=0x00;
    ConfigVideoPhone.bReserved1=0x00;
    ConfigVideoPhone.bReserved2=0x00;
    ConfigVideoPhone.bReserved3=0x00;
}

int CConfigVideophone::GetData(int index,BYTE *pbData)
{
	ASSERT(pbData);
	int nBlocklength = 0;

	memcpy(pbData,&ConfigVideoPhone,sizeof(STRUCT_VIDEOPHONE_CONFIG));
	nBlocklength = sizeof(STRUCT_VIDEOPHONE_CONFIG);
	return nBlocklength;
}

BOOL CConfigVideophone::GetData(int index,void **pvData)
{
	if (index == 0) 
	{	
		*pvData = &ConfigVideoPhone;
		return TRUE;
	}
	return FALSE;
}

BOOL CConfigVideophone::IsUsed(int index)
{
	if (index<this->GetMaxStructures()) 
		return (TRUE);

	return FALSE;
}










