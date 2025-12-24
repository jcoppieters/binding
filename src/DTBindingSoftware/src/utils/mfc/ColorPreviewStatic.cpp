#include "stdafx.h"
#include "ColorPreviewStatic.h"

/*************************************************************/
BEGIN_MESSAGE_MAP(DUOTECNO::MFC_HELPER::CColorPreviewStatic, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()
/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/

CColorPreviewStatic::CColorPreviewStatic( ) : CStatic() { 
	this->m_Color = RGB(255,255,255);
	this->m_ColorTransparent = RGB(255,255,255);
}

void CColorPreviewStatic::SetColor(COLORREF color) {
    m_Color = color;
    Invalidate(); // Force redraw
}

void CColorPreviewStatic::SetColorTransparent(COLORREF color) {
	m_ColorTransparent = color;
	 Invalidate(); // Force redraw
}

void CColorPreviewStatic::OnPaint()
{
    CPaintDC dc(this);

    CRect rect;
    GetClientRect(&rect);

	 // 1. Maak een clipping region zodat we nergens buiten tekenen
    dc.IntersectClipRect(&rect);

	if (this->m_Color == this->m_ColorTransparent) {
		// Dambord achtergrond tekenen voor transparantie simulatie		
		this->paintTransparent(dc,rect);
	}
	else {
		// Kleurvlak tekenen 
		CBrush brush(m_Color);
		dc.FillRect(rect, &brush);
	}

	// Optioneel: rand
    dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);
}

void CColorPreviewStatic::paintTransparent(CPaintDC& dc,const CRect& rect) {
	const int boxSize = 8;
	bool light = true;
	for (int y = rect.top; y < rect.bottom; y += boxSize) {
		for (int x = rect.left; x < rect.right; x += boxSize) {
			CRect tileRect(x, y, min(x + boxSize, rect.right), min(y + boxSize, rect.bottom));
			CBrush bgBrush(light ? RGB(230, 230, 230) : RGB(255, 255, 255));
			dc.FillRect(tileRect, &bgBrush);
			light = !light;
		}
		if ((rect.Width() / boxSize) % 2 == 0)
			light = !light;
	}
}


/*************************************************************/
};
};
/*************************************************************/