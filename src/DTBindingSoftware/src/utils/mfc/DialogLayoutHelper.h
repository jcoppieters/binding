#pragma once

class CDialogLayoutHelper {

public:
	CDialogLayoutHelper(CWnd* pDialog);

	// Deze aanroepen wanneer de coordinaten opnieuw moeten berekend worden...
	void setInvalidate() {
		this->m_invalidate = TRUE;
	}

	// Normalize coordinates wanneer er meerdere schermen gebruikt worden.
	// static void normalizeWindowCoordinates(CRect);


	// Berekent de non-client borders (marges) van een dialoogvenster.
	//
	// Voorbeeld output:
	// Borders (L,T,R,B): 3, 31, 3, 5			(Top= Header balk)
	CRect getWindowBorders();

	//////////////////////////////////////////////////////////////////
	// Posities waar de window zich bevindt op het scherm.
	CRect getScreenWindowPos();						// Positie op het scherm.
	CPoint getScreenWindowPosTopLeft();				// Topleft positie op het scherm.
	CPoint getScreenWindowPosBottomRight();			// Bottom right positie op het scherm.
	CSize getWindowSizeWidth();
	CSize getWindowSizeHeight();

	//////////////////////////////////////////////////////////////////
	// De client Rectangle.
	CRect getClientAreaRectangle();
	CSize getClientAreaSizeWidth();
	CSize getClientAreaSizeHeight();


protected:
	BOOL isValidWindow();
	void checkInvalidate();

private:
	// De Dialoog.
	CWnd* const m_pDialog;			

	// De coordinaten opnieuw berekenen.
	BOOL m_invalidate;				
	
	// De positie op het scherm.
	CRect m_rectWindow;				

	// De client rectangle.	
	// Dit is het gebied in de dialoog die geruikt worden om controls te plaatsen.
	CRect m_rectClientArea;			

	// De Margins van de borders top (header), bottom, left, right
	CRect m_rectBorderMargins;		
};