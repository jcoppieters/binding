// PropertyPageNodeBitmapsHC5.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeBitmapsHC5.h"

#include "MainFrm.h"
#include "MemoryBitmapInfo.h"
#include "MyWindowMessages.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmapsHC5 property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeBitmapsHC5, CPropertyPageNodeBitmaps)

CPropertyPageNodeBitmapsHC5::CPropertyPageNodeBitmapsHC5() 
: CPropertyPageNodeBitmaps()
{
	//{{AFX_DATA_INIT(CPropertyPageNodeBitmapsHC5)
	//}}AFX_DATA_INIT
}


CPropertyPageNodeBitmapsHC5::CPropertyPageNodeBitmapsHC5(CCanNode *pNode) 
: CPropertyPageNodeBitmaps(pNode) 
{
}

CPropertyPageNodeBitmapsHC5::~CPropertyPageNodeBitmapsHC5()
{
}

void CPropertyPageNodeBitmapsHC5::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPageNodeBitmapsHC5)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeBitmapsHC5, CPropertyPageNodeBitmaps)
	//{{AFX_MSG_MAP(CPropertyPageNodeBitmapsHC5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmapsHC5 message handlers

void CPropertyPageNodeBitmapsHC5::UpdateButtons(void)
{		
	if (m_ptrMemoryBitmapInfo != NULL) 
	{
		int i;

		// Update Destination Buttons ...
		for (i=0;i<MAX_NR_DESTINATIONS_AUDIO;i++) 
		{					
			CMyButton* const pBtn = &m_AudioDestButtons[i];
			if (pBtn != NULL) 
			{
				const int iMemoryIndex = i+BITMAP_HC5_V64_AUDIODEST0;
				pBtn->SetButtonState(m_ptrMemoryBitmapInfo->IsBitmapStoredToSend(iMemoryIndex));			
			}
		}

		// Update Source Buttons ...
		for (i=0;i<MAX_NR_SOURCES_AUDIO;i++) 
		{					
			CMyButton* const pBtn = &m_AudioSources[i];
			if (pBtn != NULL) 
			{
				const int iMemoryIndex = i+BITMAP_HC5_V64_AUDIOSOURCE0;
				pBtn->SetButtonState(m_ptrMemoryBitmapInfo->IsBitmapStoredToSend(iMemoryIndex));			
			}
		}

		// Update Extra IR-Keypad...
		for (i=0;i<MAX_NR_EXTRAIR;i++) 
		{					
			CMyButton* const pBtn = &m_AudioExtraIR[i];
			if (pBtn != NULL) 
			{
				const int iMemoryIndex = i+BITMAP_HC5_V64_EXTRAIR_0;
				pBtn->SetButtonState(m_ptrMemoryBitmapInfo->IsBitmapStoredToSend(iMemoryIndex));			
			}
		}
	}
}

void CPropertyPageNodeBitmapsHC5::CreateButtons(void)
{	
	int i;

	CString szName;

#define MOODBUTTON_XSIZE		40
#define MOODBUTTON_YSIZE		40
#define MOODBUTTON_TOP_OFFSET	25
#define MOODBUTTON_LEFT_OFFSET	15
#define MOODBUTTON_SPACE_X		2
#define MOODBUTTON_SPACE_Y		2



	CSize szControlSize(MOODBUTTON_XSIZE,MOODBUTTON_YSIZE);
	CPoint TopLeft(400,MOODBUTTON_TOP_OFFSET);



	for (i=0;i<MAX_NR_DESTINATIONS_AUDIO;i++) {

		szName.Format(_T("dest%d"),i);

		CMyButton* const pBtn = &m_AudioDestButtons[i];		
		pBtn->Create(szName,
				 WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_PUSHBUTTON,
				 CRect(TopLeft,szControlSize),
				 this,
				 IDC_BUTTON_HC4MOODS_MOOD0);// + i);


		pBtn->SetButtonIndex(BITMAP_HC5_V64_AUDIODEST0 + i);

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
				 IDC_BUTTON_HC4MOODS_MOOD0);


		pBtn->SetButtonIndex(BITMAP_HC5_V64_AUDIOSOURCE0 + i);
	
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


		pBtn->SetButtonIndex(BITMAP_HC5_V64_EXTRAIR_0 + i);

		TopLeft.x += MOODBUTTON_SPACE_X + MOODBUTTON_XSIZE;	
	
	}


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

	if (m_pDefaultFont != NULL) {	
		m_StaticDest.SetFont(m_pDefaultFont);
		m_StaticSources.SetFont(m_pDefaultFont);
		m_StaticExtra.SetFont(m_pDefaultFont);
		m_BtnClear.SetFont(m_pDefaultFont);
		m_BtnUpload.SetFont(m_pDefaultFont);
		m_BtnUploadAll.SetFont(m_pDefaultFont);
	}
}

