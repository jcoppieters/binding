#pragma once
/***********************************************************************************/
#include "appdali_fwd.h"
#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_H
	#include "communication/tcpsocket/homeclient/itcpsockethomeclient.h"	
#endif 
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
class CAppDali;
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Singleton klasse.
// Beheert de 'CAppDali' instanties voor de smartbox ultra.
class CAppDaliManager {

public:
	static CAppDaliManager& getInstance();
	static void destroy();

public:
	virtual ~CAppDaliManager();

	// Aanmaken van een instantie voor een bepaalde node.	
	BOOL create(IAppDaliHost_App* parent,int nodeAddress,ITcpSocketHomeClient* pSocket, CAppDali** ppDali);

	// Verwijderen van de instantie voor een bepaalde nodeAddress.
	bool remove(int nodeAddress);

	// Alle instanties verwijderen.
	// Aanroepen wanneer er een nieuw project gestart wordt.
	void erase();

	// Zoeken naar de DALI implementatie die bij een node behoort.
	BOOL find(int nodeAddress,CAppDali** ppInstance);

	// Opvragen van het aantal implementaties.
	// Indien er geen zijn moet de lijst opnieuw aangemaakt worden.
	int getCount() const;

	// Opvragen van de huidig geselecteerd.
	// Indien er nog geen geslecteerd was dan is het de eerste in de lijst.
	int getCurrentSelectedNode( );

	// Instellen van de huige geselecteerd.
	// Deze moet aanwezig zijn in de lijst.
	BOOL setCurrentSelectedNode(int nodeAddress);


	// Utility functies.
	// Pollen van de AppDali implementaties. (Nodig voor de FSM timers).
	static void poll(CAppDaliManager& ref,int intervalSec);

protected:
	CAppDaliManager();

private:
	static CAppDaliManager* m_pThis;
	CMap<int,int,CAppDali*,	CAppDali*> m_map;
	int m_currSelectedNodeAddress;						// De huidige geselecteerde node.
};
/***********************************************************************************/
};
};
/***********************************************************************************/

