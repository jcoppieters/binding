#pragma once

/*********************************************************************/
namespace DUOTECNO {
/*********************************************************************/

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
class CDlgHelper
{
public: 
	CDlgHelper(CWnd& refParent);
	virtual ~CDlgHelper( );

	// Deze functie moet aangeroepen worden in de OnInitDialog( )
	void initDialog();

protected:
	virtual void doWorkInitDialog(CWnd& refParent) = 0;

private:
	CWnd& m_refParent;
};
/*********************************************************************/

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

// Om deze klasse te gebruiken in een dialoog.
// Instantie aanmaken en Dialog referentie meegeven in de ctor.
// initDialog( ) aanroepen in de OnInitDialog( ) van de dialoog.
class CDlgHelperFont : public CDlgHelper
{
public:
	CDlgHelperFont(CWnd& refParent);
	virtual ~CDlgHelperFont( );

protected:
	virtual void doWorkInitDialog(CWnd& refParent);

	void createSystemFont(CWnd& refParent);
	void createCustomFont(CWnd& refParent);
	void setDlgFont(CWnd& refParent);

private:
	CFont m_DlgFont;

	typedef CDlgHelper Base;
};
/*********************************************************************/

/*********************************************************************/
}; /* end namespace */
/*********************************************************************/