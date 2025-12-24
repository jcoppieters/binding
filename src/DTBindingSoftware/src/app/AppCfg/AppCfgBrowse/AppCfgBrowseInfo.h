#pragma once

class AppCfgBrowseInfo {
public:
	static AppCfgBrowseInfo& getInstance( );

	typedef enum {
		MODE_IDLE = 0,
		MODE_BROWSE_BROADCAST_ALL,
		MODE_BROWSE_SINGLE_NODE
	} Mode_t;


	void setMode(Mode_t mode);
	Mode_t getMode() const;

	void setNodeAddress(int nodeAddress);
	int getNodeAddress( ) const;

protected:
	AppCfgBrowseInfo();

private:
	Mode_t m_mode;
	int m_nodeAddress;
};

