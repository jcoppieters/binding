// MyProgressCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MyProgressCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyProgressCtrl

CMyProgressCtrl::CMyProgressCtrl()
{
	nNrSteps = 10;
	nCurrStep = 0;

	rcProgress = NULL;
	CurrentPos = NULL;
	PrevPos = NULL;
	fDrawComplete = TRUE;
	fUpdateBusy = FALSE;
}

CMyProgressCtrl::~CMyProgressCtrl()
{
	if (rcProgress!=NULL) {
		delete [] rcProgress;
	}
	if (CurrentPos!=NULL) {
		delete [] CurrentPos;
	}
	if (PrevPos!=NULL) {
		delete [] PrevPos;
	}
}


BEGIN_MESSAGE_MAP(CMyProgressCtrl, CWnd)
	//{{AFX_MSG_MAP(CMyProgressCtrl)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyProgressCtrl message handlers

void CMyProgressCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (fDrawComplete == TRUE) {
		dc.FillRect(rcClient,dc.GetCurrentBrush());
		dc.RoundRect(rcClient,CPoint(5,2));
		DrawProgress(&dc);
		fDrawComplete = FALSE;
	}
	else {
		DrawProgress(&dc);
	}

	fUpdateBusy = FALSE;
}
/*
void CMyProgressCtrl::DrawProgress(CDC *pDC)
{
	int i;

	for (i=0;i<nNrSteps;i++) {	
		if ((PrevPos[i] != DRAW_PROGRESS_NOBLOCK) && (CurrentPos[i] == DRAW_PROGRESS_NOBLOCK)) {
			// Fill Rect with bkgnd ...
			pDC->FillRect(rcProgress[i],pDC->GetCurrentBrush());
		}
		else if (CurrentPos[i] == DRAW_PROGRESS_BLOCK0) {
			pDC->FillRect(rcProgress[i],&m_BrushBlock0);		
		}
		else if (CurrentPos[i] == DRAW_PROGRESS_BLOCK1) {		
			pDC->FillRect(rcProgress[i],&m_BrushBlock1);		
		}
		else if (CurrentPos[i] == DRAW_PROGRESS_BLOCK2) {
			pDC->FillRect(rcProgress[i],&m_BrushBlock2);
		}
		else if (CurrentPos[i] == DRAW_PROGRESS_BLOCKERROR) {
			pDC->FillRect(rcProgress[i],&m_BrushBlockError);
		}
	}
}
*/
void CMyProgressCtrl::DrawProgress(CDC *pDC)
{
	CBrush *pBrush = NULL;
	pBrush = pDC->GetCurrentBrush();
	
	if (pBrush != NULL) 
	{
		for (int i=0;i<nNrSteps;i++) {	
			if ((PrevPos[i] != DRAW_PROGRESS_NOBLOCK) && (CurrentPos[i] == DRAW_PROGRESS_NOBLOCK)) {
				// Fill Rect with bkgnd ...
				pDC->FillRect(rcProgress[i],pBrush);
			}
			else if (CurrentPos[i] == DRAW_PROGRESS_BLOCK0) {
				pDC->FillRect(rcProgress[i],&m_BrushBlock0);		
			}
			else if (CurrentPos[i] == DRAW_PROGRESS_BLOCK1) {		
				pDC->FillRect(rcProgress[i],&m_BrushBlock1);		
			}
			else if (CurrentPos[i] == DRAW_PROGRESS_BLOCK2) {
				pDC->FillRect(rcProgress[i],&m_BrushBlock2);
			}
			else if (CurrentPos[i] == DRAW_PROGRESS_BLOCKERROR) {
				pDC->FillRect(rcProgress[i],&m_BrushBlockError);
			}
		}
	}
}

BOOL CMyProgressCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL fResult;
	int i;
	int l,r,t,b;		//left,right,top,bottom ...

	fResult = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	if (fResult == TRUE) {	
		GetClientRect(&rcClient);
	

		m_BrushBlock0.CreateSolidBrush(COLOR_BLOCK0);
		m_BrushBlock1.CreateSolidBrush(COLOR_BLOCK1);
		m_BrushBlock2.CreateSolidBrush(COLOR_BLOCK2);
		m_BrushBlockError.CreateSolidBrush(COLOR_BLOCK_ERROR);

	    CurrentPos = new ENUM_DRAW_PROGRESS_BLOCK[nNrSteps];
		PrevPos = new ENUM_DRAW_PROGRESS_BLOCK[nNrSteps];
		rcProgress = new CRect[nNrSteps];
		// Calculate rectangles ....
		for (i=0;i<nNrSteps;i++) {
			t = rcClient.top+2;
			b = rcClient.bottom-2;
			l = (i * (rcClient.Width()/(nNrSteps-1))) + 1;
			r = ((i+1) * (rcClient.Width()/(nNrSteps-1)));

			*rcProgress[i] = CRect(l,t,r,b);

			//TRACE2("\n%d-%d\t\t",l,r);
			//TRACE2("%d-%d",t,b);


			CurrentPos[i] = DRAW_PROGRESS_NOBLOCK;
			PrevPos[i] = DRAW_PROGRESS_NOBLOCK;
		}

		rcProgress[nNrSteps-1].right -= 2;//
	}

	fDrawComplete = TRUE;
	return fResult;
}

BOOL CMyProgressCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	// Called before Create Window ...
	
	return CWnd::PreCreateWindow(cs);
}

void CMyProgressCtrl::SetProgress(int step)
{
	nCurrStep = step;
	if (nCurrStep>=nNrSteps) nCurrStep = nNrSteps-1;

	UpdateProgressData();
}

void CMyProgressCtrl::SetRange(int nMin,int nMax)
{
	nNrSteps = nMax;
}

void CMyProgressCtrl::StepUp(void)
{
	if (fUpdateBusy == FALSE) {
		if (++nCurrStep==nNrSteps) nCurrStep=0;
		UpdateProgressData();
		fUpdateBusy = TRUE;
	}
}

void CMyProgressCtrl::UpdateProgressData(void) 
{
	int i;

	memcpy(&PrevPos[0],&CurrentPos[0],nNrSteps * sizeof(ENUM_DRAW_PROGRESS_BLOCK));

	for (i=0;i<nNrSteps;i++){
		CurrentPos[i] = DRAW_PROGRESS_NOBLOCK;
	}

	if (nCurrStep == nNrSteps-1) {
		CurrentPos[nCurrStep] = DRAW_PROGRESS_BLOCK1;
		CurrentPos[nCurrStep-1] = DRAW_PROGRESS_BLOCK2;
	}
	else if (nCurrStep == nNrSteps) {						//Last Position
		CurrentPos[nCurrStep] = DRAW_PROGRESS_BLOCK0;
	}
	else if (nCurrStep == 0) {
		CurrentPos[nCurrStep] = DRAW_PROGRESS_BLOCK0;
	}
	else if (nCurrStep == 1) {
		CurrentPos[nCurrStep-1] = DRAW_PROGRESS_BLOCK1;
		CurrentPos[nCurrStep] = DRAW_PROGRESS_BLOCK0;
	}
	else if (nCurrStep >= 2) {
		CurrentPos[nCurrStep-2] = DRAW_PROGRESS_BLOCK2;
		CurrentPos[nCurrStep-1] = DRAW_PROGRESS_BLOCK1;
		CurrentPos[nCurrStep] = DRAW_PROGRESS_BLOCK0;
	}
	if (nCurrStep == nNrSteps) ASSERT(0);

	fDrawComplete = FALSE;
	Invalidate();
}


void CMyProgressCtrl::Clear()
{
	for (int i=0;i<nNrSteps;i++) {
		PrevPos[i] = DRAW_PROGRESS_BLOCK0;
		CurrentPos[i] = DRAW_PROGRESS_NOBLOCK;
	}
	fDrawComplete = TRUE;
	Invalidate();
	nCurrStep = nNrSteps-1;
}

void CMyProgressCtrl::SetComplete(void)
{
	for (int i=0;i<nNrSteps;i++) {
		CurrentPos[i] = DRAW_PROGRESS_BLOCK0;
		PrevPos[i] = DRAW_PROGRESS_NOBLOCK;
	}
	fDrawComplete = TRUE;
	Invalidate();
	nCurrStep = nNrSteps-1;
}

void CMyProgressCtrl::SetError(void)
{
	for (int i=0;i<nNrSteps;i++) {
		CurrentPos[i] = DRAW_PROGRESS_BLOCKERROR;
		PrevPos[i] = DRAW_PROGRESS_NOBLOCK;
	}
	fDrawComplete = TRUE;
	Invalidate();
	nCurrStep = nNrSteps-1;
}

void CMyProgressCtrl::OnNcPaint() 
{
	fDrawComplete = TRUE;
	Invalidate();	
	//TRACE("OnNCPaint");
}
