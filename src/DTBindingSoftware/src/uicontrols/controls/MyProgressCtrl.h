#if !defined(AFX_MYPROGRESSCTRL_H__D3C7F574_287A_4B97_BD7C_D51F51A23441__INCLUDED_)
#define AFX_MYPROGRESSCTRL_H__D3C7F574_287A_4B97_BD7C_D51F51A23441__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyProgressCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyProgressCtrl window

typedef enum {
	DRAW_PROGRESS_BLOCK0 = 0,
	DRAW_PROGRESS_BLOCK1,
	DRAW_PROGRESS_BLOCK2,
	DRAW_PROGRESS_BLOCKERROR = 3,
	DRAW_PROGRESS_NOBLOCK = 0xFF

}ENUM_DRAW_PROGRESS_BLOCK;


//#define RED_COLOR
#define GREEN_COLOR
//#define BLUE_COLOR

#ifdef GREEN_COLOR
	#define COLOR_BLOCK0		(RGB(128,255,128))	
	#define COLOR_BLOCK1		(RGB(192,255,192))	
	#define COLOR_BLOCK2		(RGB(224,255,224))	
#endif

#ifdef RED_COLOR
	#define COLOR_BLOCK0		(RGB(255,128,128))	
	#define COLOR_BLOCK1		(RGB(255,192,192))	
	#define COLOR_BLOCK2		(RGB(255,224,224))	
#endif

#ifdef BLUE_COLOR
	#define COLOR_BLOCK0		(RGB(64,64,255))	
	#define COLOR_BLOCK1		(RGB(192,192,255))	
	#define COLOR_BLOCK2		(RGB(224,224,255))	
#endif

#define COLOR_BLOCK_ERROR		(RGB(255,0,0))

class CMyProgressCtrl : public CWnd
{
protected:
	CBrush m_BrushBlock0;
	CBrush m_BrushBlock1;
	CBrush m_BrushBlock2;
	CBrush m_BrushBlockError;

	CRect  rcClient;

	int		nNrSteps;
	int		nCurrStep;
	CRect	*rcProgress;

	ENUM_DRAW_PROGRESS_BLOCK *CurrentPos,*PrevPos;
	BOOL fDrawComplete;

	volatile BOOL fUpdateBusy;		// TM 20050825 !!!

protected:
	void DrawProgress(CDC *pDC);


// Construction
public:
	CMyProgressCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyProgressCtrl)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetComplete(void);
	void Clear();
	void SetProgress(int step);
	void SetRange(int nMin=0,int nMax=10);
	void StepUp(void);
	void SetError(void);
	
	void UpdateProgressData(void);

	virtual ~CMyProgressCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyProgressCtrl)
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPROGRESSCTRL_H__D3C7F574_287A_4B97_BD7C_D51F51A23441__INCLUDED_)
