#ifndef INCLUDED_TCPSOCKET_OPTIONS_H
#define INCLUDED_TCPSOCKET_OPTIONS_H
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CTCPSocketOptions
{
public:
	CTCPSocketOptions( );

	void setOptionLoginSecure(BOOL flag);					// Standaard on.
	BOOL getOptionLoginSecure() const;

	void setOptionLoginSubscribeCanMsg(BOOL flag);			// Standaard on.
	BOOL getOptionLoginSubscribeCanMsg( ) const;

	void setOptionLoginSubscribeDALIMsg(BOOL flag);			// Standaard off (sedert V16.36)
	BOOL getOptionLoginSubscribeDALIMsg( ) const;

	void setOptionLoginAllowSlaveConnect(BOOL flag);		// Standaard off (sedert V16.38)
	BOOL getOptionLoginAllowSlaveConnect() const;

private:
	enum {
		FLAG_OPTION_LOGIN_SECURED				= (1<<0),
		FLAG_OPTION_LOGIN_SUBSCRIBE_CANMSG		= (1<<1),
		FLAG_OPTION_LOGIN_SUBSCRIBE_DALIMSG		= (1<<2),		// Nodig voor de Sbox ultra DALI config.
		FLAG_OPTION_LOGIN_ALLOW_SLAVE			= (1<<3)		// Nodig voor de Sbox ultra DALI config.
	};
	int m_flags;
};
/*****************************************************************/
#endif
/*****************************************************************/