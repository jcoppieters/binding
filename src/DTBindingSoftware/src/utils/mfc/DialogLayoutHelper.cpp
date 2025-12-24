#include "stdafx.h"
#include "DialogLayoutHelper.h"


CDialogLayoutHelper::CDialogLayoutHelper(CWnd* pDialog) : 
	m_pDialog(pDialog), m_rectWindow(0,0,0,0), m_rectClientArea(0,0,0,0), 
	m_rectBorderMargins(0,0,0,0), m_invalidate(TRUE) { }


/**
 * @brief Berekent de non-client borders (marges) van een dialoogvenster.
 *
 * Deze functie bepaalt hoeveel ruimte er rond de client area zit
 * in een dialoogvenster — met inbegrip van de linker-, boven-, rechter-
 * en onderborder. De bovenste marge bevat meestal de titelbalk (caption).
 *
 * @param pDialog Pointer naar het dialoogvenster (CWnd of CDialog).
 *                Moet een geldig en zichtbaar venster zijn.
 * 
 * @return CRect waarin:
 *         - `left`   = linkerbuitenrand tot client area
 *         - `top`    = bovenrand inclusief titelbalk
 *         - `right`  = rechterrand
 *         - `bottom` = onderrand
 *         
 *         Indien `pDialog` ongeldig is, wordt een lege `CRect(0,0,0,0)` teruggegeven.
 *
 * @note Deze functie gebruikt schermcoördinaten om de offset van de
 *       client area binnen het venster correct te bepalen.
 *
 * @example
 *     CRect borders = CDialogLayoutHelper::GetWindowBorders(this);
 *     TRACE(_T("Borders (L,T,R,B): %d, %d, %d, %d\n"),
 *           borders.left, borders.top, borders.right, borders.bottom);
 */
CRect CDialogLayoutHelper::getWindowBorders() {
	if (!isValidWindow())
        return CRect(0, 0, 0, 0);

	this->checkInvalidate();
    return m_rectBorderMargins;
}

CRect CDialogLayoutHelper::getScreenWindowPos() {
	if (!isValidWindow())
        return CRect(0, 0, 0, 0);

	this->checkInvalidate();
	return m_rectWindow;
}

// Posities waar de window zicht bevindt op het scherm.
CPoint CDialogLayoutHelper::getScreenWindowPosTopLeft() {
	return getScreenWindowPos().TopLeft();
}

// Posities waar de window zicht bevindt op het scherm.
CPoint CDialogLayoutHelper::getScreenWindowPosBottomRight() {
	return getScreenWindowPos().BottomRight();
}

CSize CDialogLayoutHelper::getWindowSizeWidth() {
	return getScreenWindowPos().Width();
}

CSize CDialogLayoutHelper::getWindowSizeHeight() {
	return getScreenWindowPos().Height();
}

CRect CDialogLayoutHelper::getClientAreaRectangle() {
	if (!isValidWindow())
        return CRect(0, 0, 0, 0);

	this->checkInvalidate();
	return m_rectClientArea;
}

CSize CDialogLayoutHelper::getClientAreaSizeWidth() {
	return getClientAreaRectangle().Width();
}
CSize CDialogLayoutHelper::getClientAreaSizeHeight() {
	return getClientAreaRectangle().Height();
}

BOOL CDialogLayoutHelper::isValidWindow() {
	if (!m_pDialog || !::IsWindow(m_pDialog->GetSafeHwnd()))
        return FALSE;

	return TRUE;
}

void CDialogLayoutHelper::checkInvalidate() {
	if (this->m_invalidate) {

		m_rectWindow = CRect(0,0,0,0);
		m_rectClientArea = CRect(0,0,0,0);
		m_rectBorderMargins = CRect(0,0,0,0);
		
		// Window rect (in screen coordinates)	
		m_pDialog->GetWindowRect(&m_rectWindow);

		// Client rect (in client coordinates)
		m_pDialog->GetClientRect(&m_rectClientArea);
		
		// Client point to screen.
		CPoint cliTopLeft(0,0);		
		m_pDialog->ClientToScreen(&cliTopLeft);

		// Margins van de dialoog berekenen.
		m_rectBorderMargins.left   = cliTopLeft.x - m_rectWindow.left;
		m_rectBorderMargins.top    = cliTopLeft.y - m_rectWindow.top;
		m_rectBorderMargins.right  = m_rectWindow.right - (cliTopLeft.x + m_rectClientArea.Width());
		m_rectBorderMargins.bottom = m_rectWindow.bottom - (cliTopLeft.y + m_rectClientArea.Height());

		// Invalidate vlag.
		this->m_invalidate = FALSE;
	}
}

/*
void CDialogLayoutHelper::normalizeWindowCoordinates( ) {

}
*/