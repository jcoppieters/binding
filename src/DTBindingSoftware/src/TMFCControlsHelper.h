#ifndef TMFCCONTROLSHELPER_20081224_H
#define TMFCCONTROLSHELPER_20081224_H
////////////////////////////////////////////////////////
template <class WINDOW>
	class TShowWindow
{
protected:
	WINDOW* const m_pWnd;
	BOOL m_fVisible;

public:
	TShowWindow(WINDOW* pWnd) : m_pWnd( pWnd ) {
		m_fVisible = pWnd->IsWindowVisible();
		if (m_fVisible)
			m_pWnd->ShowWindow( FALSE );
	}
	~TShowWindow( ) {
		if (m_fVisible)
			m_pWnd->ShowWindow( TRUE );
	}
};
////////////////////////////////////////////////////////
#endif