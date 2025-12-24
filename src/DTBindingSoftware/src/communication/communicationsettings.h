#ifndef INCLUDED_COMMUNICATION_SETTINGS_H
#define INCLUDED_COMMUNICATION_SETTINGS_H
/*****************************************************************/
#include "base/communication_def.h"
/*****************************************************************/
class CCommunicationSettingsUsb;
class CCommunicationSettingsTcp;
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CCommunicationSettings
{
public:
	CCommunicationSettings();
	virtual ~CCommunicationSettings();

	typedef enum {
		UNKNOWN = 0, USB = 1, TCPIP = 2
	} Method_t;

	void clear();
	BOOL set(const CCommunicationSettingsUsb& settings);
	BOOL set(const CCommunicationSettingsTcp& settings);
	const CCommunicationSettingsTcp* getSettingsTcp() const {
		return m_pSettingsTcp;
	}	
	const CCommunicationSettingsUsb* getSettingsUsb() const {
		return m_pSettingsUsb;
	}	

	Method_t getMethod() const {
		return m_method;
	}
protected:
	void deallocSettingsUsb();
	void deallocSettingsTcp();

private:
	Method_t m_method;
	CCommunicationSettingsUsb* m_pSettingsUsb;
	CCommunicationSettingsTcp* m_pSettingsTcp;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CCommunicationSettingsUsb
{
public:
	// comport: Waarde van 1 tem 255
	CCommunicationSettingsUsb(int comport);
	CCommunicationSettingsUsb(const CCommunicationSettingsUsb& r);

	int getPort( ) const {
		return m_comport;
	}
	BOOL valid() const;

private:
	const int m_comport;
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CCommunicationSettingsTcp
{
public:
	CCommunicationSettingsTcp(ConnectionMethod_t method, const CString& hostName,int port,const CString& pwd,const CString& cloudServerLoginToken);
	CCommunicationSettingsTcp(const CCommunicationSettingsTcp& r);

	const CString& getHostName() const {
		return m_hostName;
	}
	const CString& pwd() const {
		return m_pwd;
	}
	int getPort( ) const {
		return m_port;
	}
	const CString& getCloudServerLoginToken( ) const {
		return m_cloudServerLoginToken;
	}
	ConnectionMethod_t getConnectionMethod( ) const {
		return m_method;
	}

	BOOL valid() const;

private:
	const ConnectionMethod_t m_method;
	const CString m_hostName;
	const int m_port;
	const CString m_pwd;
	const CString m_cloudServerLoginToken;
};

/*****************************************************************/
#endif /* INCLUDED_COMMUNICATION_SETTINGS_H */
/*****************************************************************/