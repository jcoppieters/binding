// PropertyPageNodeBitmapsHC4.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeBitmapsHC4.h"

#include "MainFrm.h"
#include "MemoryBitmapInfo.h"
#include "MyWindowMessages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmapsHC4 property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeBitmapsHC4, CPropertyPageNodeBitmaps)

CPropertyPageNodeBitmapsHC4::CPropertyPageNodeBitmapsHC4() :
CPropertyPageNodeBitmaps() 
{
	ASSERT(0);
}

CPropertyPageNodeBitmapsHC4::CPropertyPageNodeBitmapsHC4(CCanNode *pNode) 
: CPropertyPageNodeBitmaps(pNode)
{
	//{{AFX_DATA_INIT(CPropertyPageNodeBitmapsHC4)
	//}}AFX_DATA_INIT
}

CPropertyPageNodeBitmapsHC4::~CPropertyPageNodeBitmapsHC4()
{
}

void CPropertyPageNodeBitmapsHC4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPageNodeBitmapsHC4)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeBitmapsHC4, CPropertyPageNodeBitmaps)
	//{{AFX_MSG_MAP(CPropertyPageNodeBitmapsHC4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmapsHC4 message handlers
	
void CPropertyPageNodeBitmapsHC4::CreateButtons(void)
{
	//CMyButton *pBtn=NULL;
	int i;

	//CPoint TopLeft;
	//CSize	szControlSize;
	CString szName;

#define MOODBUTTON_XSIZE		40
#define MOODBUTTON_YSIZE		40
#define MOODBUTTON_TOP_OFFSET	25
#define MOODBUTTON_LEFT_OFFSET	15
#define MOODBUTTON_SPACE_X		2
#define MOODBUTTON_SPACE_Y		2


	CSize  szControlSize(MOODBUTTON_XSIZE,MOODBUTTON_YSIZE);
	CPoint TopLeft(MOODBUTTON_LEFT_OFFSET,MOODBUTTON_TOP_OFFSET + (2*MOODBUTTON_YSIZE) +
										  (2*MOODBUTTON_SPACE_Y));

	//szControlSize.cx = MOODBUTTON_XSIZE;
	//szControlSize.cy = MOODBUTTON_YSIZE;
	//TopLeft.x = MOODBUTTON_LEFT_OFFSET;		
	//TopLeft.y = MOODBUTTON_TOP_OFFSET + (2 * MOODBUTTON_YSIZE) +
	//	        ( 2* MOODBUTTON_SPACE_Y);		

	for (i=0;i<MAX_NR_MOOD_BUTTONS;i++) 
	{
		szName.Format(_T("Mood%d"),i);

		CMyButton* const pBtn = &m_MoodButtons[i];

		pBtn->Create(szName,
					 WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_PUSHBUTTON,
					 CRect(TopLeft,szControlSize),
					 this,
					 IDC_BUTTON_HC4MOODS_MOOD0 + i);
		
		if ( (i == 1) || (i == 3)) {
		
			TopLeft.x += MOODBUTTON_SPACE_X + MOODBUTTON_XSIZE;	//(szControlSize.cx + 10);
			TopLeft.y = MOODBUTTON_TOP_OFFSET + (2 * MOODBUTTON_YSIZE) +
						( 2* MOODBUTTON_SPACE_Y);	//110;
		}
		else if ((i == 5) || (i == 9) || (i == 13) )
		{
			TopLeft.x += MOODBUTTON_SPACE_X + MOODBUTTON_XSIZE; //(szControlSize.cx + 10);
			TopLeft.y = MOODBUTTON_TOP_OFFSET;//10;
		}
		else {			
			TopLeft.y += MOODBUTTON_YSIZE + MOODBUTTON_SPACE_Y;//(szControlSize.cy + 10);		
		}	

		pBtn->SetButtonIndex(BITMAP_HC4_V64_MOOD0 + i);
	
	
	}
		

	for (i=0;i<MAX_NR_MAINSCREEN_AUDIO;i++) 
	{
		CMyButton* const pBtn = &m_MainScreenActionButtons[i];	

		if (i<4) 
		{		
			szName.Format(_T("Source%d"),i);
		}
		else 
		{		
			// CHANGES_PRINTF_PARAM_TYPOS
			
			if (i==4) szName.Format(_T("play"));
			else if (i==5) szName.Format(_T("stop"));
			else if (i==6) szName.Format(_T("Min"));
			else if (i==7) szName.Format(_T("plus"));
			else if (i==8) szName.Format(_T("RR"));
			else if (i==9) szName.Format(_T("FF"));
			else if (i==10) szName.Format(_T("pauze"));				
		}

		if (i == 0) {
		
			TopLeft.x = MOODBUTTON_LEFT_OFFSET + 8*MOODBUTTON_SPACE_X + 6*MOODBUTTON_XSIZE ;
			TopLeft.y = MOODBUTTON_TOP_OFFSET; 			
		}

		if ( i>0 && i<4) {
			TopLeft.y += MOODBUTTON_SPACE_Y + MOODBUTTON_YSIZE;
		}

		if (i == 4) {
		
			TopLeft.x = MOODBUTTON_LEFT_OFFSET;
			TopLeft.y = MOODBUTTON_TOP_OFFSET + 6*MOODBUTTON_SPACE_Y + 4*MOODBUTTON_YSIZE ;
		}

		if (i>4 && i<10) {
			TopLeft.x += MOODBUTTON_SPACE_X + MOODBUTTON_XSIZE;
		}

		if (i == 10) {
			TopLeft.x = MOODBUTTON_LEFT_OFFSET + 9*MOODBUTTON_SPACE_X + 7*MOODBUTTON_XSIZE ;
			TopLeft.y = MOODBUTTON_TOP_OFFSET + MOODBUTTON_SPACE_Y + MOODBUTTON_YSIZE;
		}

		pBtn->Create(szName,
				 WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_PUSHBUTTON,
				 CRect(TopLeft,szControlSize),
				 this,
				 IDC_BUTTON_HC4MOODS_MOOD0);// + i);


		pBtn->SetButtonIndex(BITMAP_HC4_V64_MAINSCREEN_SOURCE0 + i);
	
	}




	TopLeft.x = 400;
	TopLeft.y = MOODBUTTON_TOP_OFFSET;

	for (i=0;i<MAX_NR_DESTINATIONS_AUDIO;i++) {

		szName.Format(_T("dest%d"),i);

		CMyButton* const pBtn = &m_AudioDestButtons[i];		
		pBtn->Create(szName,
				 WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_PUSHBUTTON,
				 CRect(TopLeft,szControlSize),
				 this,
				 IDC_BUTTON_HC4MOODS_MOOD0);// + i);


		pBtn->SetButtonIndex(BITMAP_HC4_V64_AUDIODEST0 + i);

		if ((i+1)%4) {
			TopLeft.x += MOODBUTTON_SPACE_X + MOODBUTTON_XSIZE;			
		}
		else {
			TopLeft.x = 400;
			TopLeft.y += MOODBUTTON_SPACE_Y + MOODBUTTON_YSIZE;	
		}
	}


	TopLeft.x = 400;
	TopLeft.y = 235;

	for (i=0;i<MAX_NR_SOURCES_AUDIO;i++) {
	
		szName.Format(_T("source%d"),i);

		CMyButton* const pBtn = &m_AudioSources[i];		
		pBtn->Create(szName,
				 WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_PUSHBUTTON,
				 CRect(TopLeft,szControlSize),
				 this,
				 IDC_BUTTON_HC4MOODS_MOOD0);// + i);


		pBtn->SetButtonIndex(BITMAP_HC4_V64_AUDIOSOURCE0 + i);
	
		if ((i+1)%4) {
			TopLeft.x += MOODBUTTON_SPACE_X + MOODBUTTON_XSIZE;			
		}
		else {
			TopLeft.x = 400;
			TopLeft.y += MOODBUTTON_SPACE_Y + MOODBUTTON_YSIZE;	
		}

	}


	TopLeft.x = MOODBUTTON_LEFT_OFFSET;
	TopLeft.y = 280;

	for (i=0;i<MAX_NR_EXTRAIR;i++) {

		szName.Format(_T("IR %d"),i);

		CMyButton* const pBtn = &m_AudioExtraIR[i];		
		pBtn->Create(szName,
				 WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_PUSHBUTTON,
				 CRect(TopLeft,szControlSize),
				 this,
				 IDC_BUTTON_HC4MOODS_MOOD0);// + i);


		pBtn->SetButtonIndex(BITMAP_HC4_V64_EXTRAIR_0 + i);

		TopLeft.x += MOODBUTTON_SPACE_X + MOODBUTTON_XSIZE;	
	
	}

	m_StaticMoods.Create("Mainscreen: 39x39 Pixels",
					     WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						 CRect(5,5,365,250),
						 this,
						 0);
	m_StaticDest.Create("Audio Destinations: 54x44 Pixels",
					     WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						 CRect(390,5,580,205),
						 this,
						 0);
	m_StaticSources.Create("Audio Sources: 44x34 Pixels",
					     WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						 CRect(390,215,580,330),
						 this,
						 0);
	m_StaticExtra.Create("Audio Extra IR: 31x31 Pixels",
					     WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						 CRect(5,260,270,330),
						 this,
						 0);


	m_BtnUpload.MoveWindow(CRect(10,380,190,410),TRUE);
	m_BtnClear.MoveWindow(CRect(400,380,590,410),TRUE);
	m_BtnUploadAll.MoveWindow(CRect(200,380,390,410),TRUE);

	if (m_pDefaultFont != NULL) 
	{
		m_StaticMoods.SetFont(m_pDefaultFont);
		m_StaticDest.SetFont(m_pDefaultFont);
		m_StaticSources.SetFont(m_pDefaultFont);
		m_StaticExtra.SetFont(m_pDefaultFont);
		m_BtnClear.SetFont(m_pDefaultFont);
		m_BtnUpload.SetFont(m_pDefaultFont);
		m_BtnUploadAll.SetFont(m_pDefaultFont);
	}
}

void CPropertyPageNodeBitmapsHC4::UpdateButtons(void)
{	
	ASSERT( m_ptrMemoryBitmapInfo );

	if (m_ptrMemoryBitmapInfo != NULL) 
	{
		int i;	

		// Update Mood Buttons ...
		for (i=0;i<MAX_NR_MOOD_BUTTONS;i++) 
		{					
			CMyButton* const pBtn = &m_MoodButtons[i];
			if (pBtn != NULL) 
			{
				const int iMemoryIndex = i+BITMAP_HC4_V64_MOOD0;
				pBtn->SetButtonState(m_ptrMemoryBitmapInfo->IsBitmapStoredToSend(iMemoryIndex));			
			}
		}

		// Update Extra MainScreen Buttons ...
		for (i=0;i<MAX_NR_MAINSCREEN_AUDIO;i++) 
		{					
			CMyButton* const pBtn = &m_MainScreenActionButtons[i];
			if (pBtn != NULL) 
			{
				const int iMemoryIndex = i+BITMAP_HC4_V64_MAINSCREEN_SOURCE0;
				pBtn->SetButtonState(m_ptrMemoryBitmapInfo->IsBitmapStoredToSend(iMemoryIndex));			
			}
		}

		// Update Destination Buttons ...
		for (i=0;i<MAX_NR_DESTINATIONS_AUDIO;i++) 
		{					
			CMyButton* const pBtn = &m_AudioDestButtons[i];
			if (pBtn != NULL) 
			{
				const int iMemoryIndex = i+BITMAP_HC4_V64_AUDIODEST0;
				pBtn->SetButtonState(m_ptrMemoryBitmapInfo->IsBitmapStoredToSend(iMemoryIndex));			
			}
		}

		// Update Source Buttons ...
		for (i=0;i<MAX_NR_SOURCES_AUDIO;i++) 
		{					
			CMyButton* const pBtn = &m_AudioSources[i];
			if (pBtn != NULL) 
			{
				const int iMemoryIndex = i+BITMAP_HC4_V64_AUDIOSOURCE0;
				pBtn->SetButtonState(m_ptrMemoryBitmapInfo->IsBitmapStoredToSend(iMemoryIndex));			
			}
		}

		// Update Extra IR-Keypad...
		for (i=0;i<MAX_NR_EXTRAIR;i++) 
		{					
			CMyButton* const pBtn = &m_AudioExtraIR[i];
			if (pBtn != NULL) 
			{
				const int iMemoryIndex = i+BITMAP_HC4_V64_EXTRAIR_0;
				pBtn->SetButtonState(m_ptrMemoryBitmapInfo->IsBitmapStoredToSend(iMemoryIndex));			
			}
		}
	}
}
