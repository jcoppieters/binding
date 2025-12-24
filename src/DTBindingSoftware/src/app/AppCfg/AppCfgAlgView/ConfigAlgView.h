#pragma once

#include "utils/mfc/ArrayErrors.h"
#include "TMAlgoritmesCfg.h"

class CCanNode;

/************************************************
 * Basis klasse
 ************************************************/
class CConfigAlgView
{
public:
	/**
	 * @brief	Via deze interface berichten tonen in de parent dialog.
	 */
	struct IViewDisplay {
		virtual void AddMessage(const char* s) = 0;
	};

public:
	CConfigAlgView(CCanNode* pNode,IViewDisplay* pNotifyView);
	virtual ~CConfigAlgView(void);

	/**
	 * @brief	Bestand laden
	 */
	virtual BOOL Load(void)				= 0;

	/**
	 * @brief	Configuratie doorsturen
	 */
	virtual BOOL Upload(void)			= 0;

	/**
	 * @brief	Configuratie Naar alle doorsturen
	 */
	virtual BOOL UploadToAll(void)		= 0;

	/**
	 * @brief	Interprocess communicatie ID opvragen.
	 */
	virtual int GetViewID(void) const	= 0;

	/**
	 * @brief	Configuratie ondersteund voor deze node.
	 */
	virtual BOOL IsSupported(void)		= 0;

	/**
	 * @brief	Configuratie bestanden gevonden
	 */
	virtual BOOL IsFileFound(void)		= 0;

	/**
	 *  @brief	Kan de configuratie voor deze node afzonderlijk ingeladen worden
	 */
	virtual BOOL IsLoadSupported(void)	= 0;

	/**
	 * @brief	Kan de configuratie voor deze node doorgestuurd worden
	 */
	virtual BOOL IsUploadCanSupported(void)= 0;

	/**
	 * @brief	Kan de configuratie voor deze node ingelezen worden
	 */
	virtual BOOL IsDownloadCanSupported(void) = 0;

	/**
	 * @brief	Kan de configuratie naar alle compatible nodes doorgestuurd worden
	 */
	virtual BOOL IsUploadCanAllSupported(void)	= 0;

	/**
	 * @brief	Is de configurator nodig ?
	 */
	virtual BOOL IsUseConfigurator(void) = 0;

	/**
	 * @brief	Naam opvragen
	 */
	virtual const char* getName(void) = 0;

	/**
	 * @brief	Naam van het configuratie bestand opvragen
	 * @param	idx [IN] index van de gevraagde bestandsnaam - normaal 0
	 * @param	pszFullPathName [OUT] bestandsnaam
	 * @return	TRUE bij een geldige bestandsnaam.
	 */
	virtual BOOL getFullPathName(int idx,CString* pszFullPathName) = 0;

	/**
	 * @brief	Configuratie via het FTP protocol. (Andere is via external config).
	 */
	virtual BOOL IsUseFTPProtocol(void)	= 0;

	/**
	 * @brief	Opvragen of het algoritme gereed is (=niet bezig is).	
	 *			Zie BUG_SOLVE_0158:
	 *			Bepaalde algoritmes crashten wanneer reeds bezig en 
	 *			kregen request om opnieuw te starten.
	 */
	virtual BOOL IsReady(void) { return TRUE; };	


	/**
	 * @brief	Opvragen of configuratie via HTTP Configurator mogelijk is.	 
	 */
	virtual BOOL IsSupportedHTTPProtocol(void) { return FALSE; }

	/**
	 * @brief	Opvragen of de configuratie via HTTP Moet gebeuren.
	 *			Deze vlag kan overschreven worden door de installateur.
	 *			Te gebruiken in combinatie met IsSupportedHTTPProtocol( )
	 */
	virtual BOOL IsUseUploadHTTPProtocol(void) { return FALSE; }
	virtual void SetUseUploadHTTPProtocol(BOOL flag) { /* absorbe */ }

	// Sedert V16.01: Info berichten ivm uit te voeren configuratie.
	// Deze strings worden weergegeven in de listbox bij de selectie van het 
	// algoritme. (zie CHANGES_SMAP_ONLY_FOR_APP)
	virtual BOOL getInfoMessages(CStringArray* messages) { return FALSE; };



	/**
	 * Sedert V16.40:
	 * Via de functie kunnen we zelf de configuratie openen.
	 * Nodig voor de DALI subbus configuratie.
	 *
	 * return waarde: TRUE: Ok configuratie geopend.
	 *			      FALSE: Niet geopend via deze methode.
	 */
	virtual BOOL openConfiguration(BYTE bNodeAddress) {
		return FALSE;
	}

protected:
	CCanNode& getCanNode(void);
	const CCanNode& getCanNode(void) const;
	void DisplayMessage(const char* s);
	void DisplayErrors(DUOTECNO::MFC_HELPER::CArrayErrors& arrErrors);

	template <class ERROR_FILE>
		void DisplayErrors(ERROR_FILE& configFile)
	{
		const int errors = configFile.GetNrErrors();
		if (errors != 0) 
		{										
			for (int i=0;i<errors;i++) 
			{
				DisplayMessage(configFile.GetErrors(i));								
			}				
		}
	}

protected:
	class CNotifier : public TMAlgoritmesCfg::INotifier
	{
	public:
		CNotifier(CConfigAlgView& ref) : m_parent(ref) {};
		~CNotifier( ) { };
		virtual void AddMessage(const char* s)
		{
			m_parent.DisplayMessage(s);
		}
	private:
		CConfigAlgView& m_parent;
	};

private:
	CCanNode& m_refCanNode;
	IViewDisplay* const m_pNotifyView;
};


/*********************** inline ******************/
inline CCanNode& CConfigAlgView::getCanNode(void)
{
	return m_refCanNode;
}

inline const CCanNode& CConfigAlgView::getCanNode(void) const
{
	return m_refCanNode;
}

/*************************************************/
/*************** Utility class *******************/
/*************************************************/
struct CConfigAlgView_Util
{
	typedef enum {
		STRINGS_ID_SCREENMAP_PROAPP = 0,
	} StringID_t;

	static BOOL getInfoStrings(CStringArray* messages,StringID_t id);
};
/************************* EOF *******************/