#ifndef INCLUDED_APPREMOTECFGSERVERS_H
#define INCLUDED_APPREMOTECFGSERVERS_H
/**************************************************************/
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgServer
{
public:
	AppRemoteCfgServer( );
	AppRemoteCfgServer(const AppRemoteCfgServer& r);
	~AppRemoteCfgServer( );

	void setFriendlyName(const CString& v);
	const CString& getFriendlyName() const {
		return m_friendlyName;
	}

	void setHostName(const CString& v);
	const CString& getHostName() const {
		return m_hostName;
	}

	void setPassword(const CString& v);
	const CString& getPassword() const {
		return m_password;
	}

	void setHostPort(int port);
	int getHostPort() const {
		return m_hostPort;
	}

	void setSecured(bool v);
	bool getSecured() const {
		return m_secured;
	}

	// Utils.
	// Gelijk wanneer host & poort hetzelfde zijn.
	static bool equal(const AppRemoteCfgServer& r, const AppRemoteCfgServer& l);
	static bool equal(const AppRemoteCfgServer& r, const CString& hostName,int port);
	static bool update(AppRemoteCfgServer* dest,const AppRemoteCfgServer& l);

private:
	CString m_friendlyName;		// Een naam die kan toegekend worden aan de verbinding.
	CString m_hostName;			// ip-address of dns naam
	CString m_password;			// paswoord.
	int m_hostPort;				// host poort.
	bool m_secured;				// secured protocol.

	// Bijkomende info: MAC-Address, Physical Address, Node Address, ... ??
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgServers
{
public:
	AppRemoteCfgServers();
	~AppRemoteCfgServers();

	void clear();
	void add(AppRemoteCfgServer* item);
	int getCount() const;
	bool get(int index, AppRemoteCfgServer** ppItem) const;

	// Een server toevoegen.
	// Er wordt een copy aangemaakt en toegevoegd aan de lijst.
	// TODO: CONTROLE OF DE SERVER NOG NIET AANGEMAAKT IS...
	static bool add(AppRemoteCfgServers* dest,const AppRemoteCfgServer& r);
	static bool findEqual(const AppRemoteCfgServers& servers, const AppRemoteCfgServer& l, AppRemoteCfgServer** ppDest);
	static bool remove(AppRemoteCfgServers* dest,const CString& hostName,int port);

protected:
	bool remove(const AppRemoteCfgServer& item);
	void deallocate();

private:
	CList<AppRemoteCfgServer*> m_servers;
};
/**************************************************************/
}; /*  namespace DUOTECNO { */
/**************************************************************/
#endif 
/**************************************************************/