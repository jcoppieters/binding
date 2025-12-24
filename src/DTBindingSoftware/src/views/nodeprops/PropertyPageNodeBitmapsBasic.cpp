// PropertyPageNodeBitmapsBasic.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeBitmapsBasic.h"

#include "MainFrm.h"
#include "MemoryBitmapInfo.h"
#include "MyWindowMessages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmapsBasic property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeBitmapsBasic, CPropertyPageNodeBitmaps)


CPropertyPageNodeBitmapsBasic::CPropertyPageNodeBitmapsBasic() 
: CPropertyPageNodeBitmaps()
{
	//{{AFX_DATA_INIT(CPropertyPageNodeBitmapsBasic)
	//}}AFX_DATA_INIT
}

CPropertyPageNodeBitmapsBasic::CPropertyPageNodeBitmapsBasic(CCanNode *pNode) 
: CPropertyPageNodeBitmaps(pNode)
{
}

CPropertyPageNodeBitmapsBasic::~CPropertyPageNodeBitmapsBasic()
{
}

void CPropertyPageNodeBitmapsBasic::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPageNodeBitmapsBasic)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeBitmapsBasic, CPropertyPageNodeBitmaps)
	//{{AFX_MSG_MAP(CPropertyPageNodeBitmapsBasic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmapsBasic message handlers

void CPropertyPageNodeBitmapsBasic::CreateButtons(void)
{

	//CMyButton *pBtn=NULL;
	int i;

	//CPoint TopLeft;	
	CString szName;

	#define MOODBUTTON_XSIZE		45
	#define MOODBUTTON_YSIZE		45
	#define MOODBUTTON_TOP_OFFSET	30
	#define MOODBUTTON_LEFT_OFFSET	0
	#define MOODBUTTON_SPACE_X		2
	#define MOODBUTTON_SPACE_Y		2

	CSize	szControlSize(MOODBUTTON_XSIZE,MOODBUTTON_YSIZE);
	CPoint TopLeft(MOODBUTTON_LEFT_OFFSET,MOODBUTTON_TOP_OFFSET + 110);

	//szControlSize.cx = MOODBUTTON_XSIZE;
	//szControlSize.cy = MOODBUTTON_YSIZE;
	//TopLeft.x = MOODBUTTON_LEFT_OFFSET;
	//TopLeft.y = MOODBUTTON_TOP_OFFSET + 110;

	for (i=0;i<BITMAP_BASIC_V64_MAX_BITMAPS;i++) 
	{
		if ((i>=BITMAP_BASIC_V64_MOOD0) && (i<= BITMAP_BASIC_V64_CONTINUE)) 
		{	
			if (i== BITMAP_BASIC_V64_SENS) szName.Format("Temp");
			else if (i==BITMAP_BASIC_V64_CONTINUE) szName.Format("Go");
			else szName.Format("Mood %d",i);

			if (!((i+1)%2)) 
			{
				TopLeft.y += MOODBUTTON_YSIZE + MOODBUTTON_SPACE_Y;				
			}
			else 
			{			
				TopLeft.x += MOODBUTTON_XSIZE+ MOODBUTTON_SPACE_X;
				TopLeft.y = MOODBUTTON_TOP_OFFSET + 110;			
			}
		}

		if ((i>= BITMAP_BASIC_V64_SOURCE0) && (i<= BITMAP_BASIC_V64_VOLDN)) 
		{					
			if (i== BITMAP_BASIC_V64_SOURCE0) TopLeft.x = MOODBUTTON_LEFT_OFFSET;
			else if (i== BITMAP_BASIC_V64_VOLUP) TopLeft.x += 5;

			if (i== BITMAP_BASIC_V64_VOLUP) szName.Format("Vol +");
			else if (i== BITMAP_BASIC_V64_VOLDN) szName.Format("Vol -");
			else szName.Format("Source %d",i-BITMAP_BASIC_V64_SOURCE0);			

			if (!((i+1)%2)) {
				TopLeft.y += MOODBUTTON_YSIZE + MOODBUTTON_SPACE_Y;				
			}
			else {			
				TopLeft.x += MOODBUTTON_XSIZE + MOODBUTTON_SPACE_X;
				TopLeft.y = MOODBUTTON_TOP_OFFSET;			
			}
		}
	
		CMyButton* const pBtn = &m_Button[i];
		if (pBtn != NULL) 
		{
			pBtn->Create(szName,
						 WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_PUSHBUTTON,
						 CRect(TopLeft,szControlSize),
						 this,
						 IDC_BUTTON_HC4MOODS_MOOD0 + i);
		}
		pBtn->SetButtonIndex(i);
	}

	m_StaticMoods.Create("Mainscreen: 49x49 Pixels",
					     WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						 CRect(30,8,350,250),
						 this,
						 0);

	m_BtnClear.MoveWindow(CRect(400,350,590,380),TRUE);
	m_BtnUpload.MoveWindow(CRect(10,350,190,380),TRUE);
	m_BtnUploadAll.MoveWindow(CRect(200,350,390,380),TRUE);

	if (m_pDefaultFont != NULL) 
	{
		m_StaticMoods.SetFont(m_pDefaultFont);
		m_BtnClear.SetFont(m_pDefaultFont);
		m_BtnUpload.SetFont(m_pDefaultFont);
		m_BtnUploadAll.SetFont(m_pDefaultFont);
	}
}

void CPropertyPageNodeBitmapsBasic::UpdateButtons(void)
{
	if (m_ptrMemoryBitmapInfo != NULL) 
	{
		for (int iMemoryIndex=0;
			 iMemoryIndex<BITMAP_BASIC_V64_MAX_BITMAPS;
			 iMemoryIndex++) {					
						
			CMyButton* const pBtn = &m_Button[iMemoryIndex];
			if (pBtn != NULL)
			{
				pBtn->SetButtonState(m_ptrMemoryBitmapInfo->IsBitmapStoredToSend(iMemoryIndex));					
			}
		}
	}
	else
	{
		ASSERT(0);
	}
}

