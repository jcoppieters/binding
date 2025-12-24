#pragma once

/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/
class CColorPreviewStatic : public CStatic
{
public:
	CColorPreviewStatic( );
    void SetColor(COLORREF color);
	void SetColorTransparent(COLORREF color);

protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()

protected:
	void paintTransparent(CPaintDC& dc,const CRect& rc);

private:
    COLORREF m_Color; 
	COLORREF m_ColorTransparent;
};
/*************************************************************/
};
};
/*************************************************************/