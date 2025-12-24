#pragma once

#include "AppUDPServerCfg.h"

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
class AppUDPServerCfgWrapper
{
public:
	typedef enum {
		RESPONSE_SUCCESS = 0,
		RESPONSE_EC_NAK = -1,
		RESPONSE_EC_TIMEOUT = -2,
		RESPONSE_EC_GENERAL = -3,
		RESPONSE_EC_FORMATTER = -4

	} ErrorResponseCode_t;

	enum { DEFAULT_WAIT_MSEC = (8 * 1000) };

	AppUDPServerCfgWrapper( );
	~AppUDPServerCfgWrapper( );
	
	// Opvragen van de applicatie cfg mogelijkheden.
	bool sendRequestCfgSupported(AppUDPDevice* pDevice, int milliseconds = DEFAULT_WAIT_MSEC);

	// Netwerk cfg instellen.
	// DHCP ofwel een fix ip-adres.
	bool sendNwkConfig(AppUDPDevice& refDevice,int milliseconds = DEFAULT_WAIT_MSEC);

	// App homeserver cfg instellingen.
	bool sendCfgAppHomeSrv(AppUDPDevice& refDevice, int milliseconds = DEFAULT_WAIT_MSEC);
	bool sendRequestCfgAppHomeSrv(AppUDPDevice* pDevice,int milliseconds = DEFAULT_WAIT_MSEC);

	// DynDns instellingen.
	bool sendDynDNSSettings(AppUDPDevice& refDevice,int milliseconds = DEFAULT_WAIT_MSEC);
	bool sendRequestDynDNSSettings(AppUDPDevice* pDevice,int milliseconds = DEFAULT_WAIT_MSEC);

	// DNS provider/server.
	bool sendCfgDNSProvider(AppUDPDevice& refDevice,int milliseconds = DEFAULT_WAIT_MSEC);
	bool sendRequestCfgDNSProvider(AppUDPDevice* pDevice,int milliseconds = DEFAULT_WAIT_MSEC);

	// TCP/IP client...
	bool sendCfgTCPIPClient(AppUDPDevice& refDevice,int milliseconds = DEFAULT_WAIT_MSEC);
	bool sendRequestCfgTCPIPClient(AppUDPDevice* pDevice,int milliseconds = DEFAULT_WAIT_MSEC);

	// Time and date.
	bool sendCfgTimeDate(AppUDPDevice& refDevice, int milliseconds = DEFAULT_WAIT_MSEC);
	bool sendRequestCfgTimeDate(AppUDPDevice* pDevice, int milliseconds = DEFAULT_WAIT_MSEC);

	// SNTP.
	bool sendCfgNTP(AppUDPDevice& refDevice, int milliseconds = DEFAULT_WAIT_MSEC);
	bool sendRequestCfgNTP(AppUDPDevice* pDevice, int milliseconds = DEFAULT_WAIT_MSEC);

	// Systemmode.
	bool sendCfgSystemMode(AppUDPDevice& refDevice, int milliseconds = DEFAULT_WAIT_MSEC);
	bool sendRequestCfgSystemMode(AppUDPDevice* pDevice, int milliseconds = DEFAULT_WAIT_MSEC);

	// Opvragen van de laatste responsecode.
	ErrorResponseCode_t getResponseCode( ) const;

protected:
	// Controle op de geldigheid van pDevice en de IP-Instellingen.
	bool checkDevice(AppUDPDevice* pDevice);

private:
	ErrorResponseCode_t m_responseCode;
};
/************************************************************************************/
