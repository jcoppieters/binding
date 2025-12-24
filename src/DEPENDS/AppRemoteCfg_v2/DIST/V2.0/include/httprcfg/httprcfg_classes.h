#ifndef INCLUDED_HTTPRCFG_CLASSES_H
#define INCLUDED_HTTPRCFG_CLASSES_H
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppCfgBindAction
{
public:
	typedef enum {
		ERASE = 0, WRITE, READ 
	} Action_t;

	AppCfgBindAction(Action_t action);
	void clear( );
	void add(int nodeAddress);	
	int getCount() const;
	int get(int index) const;
	Action_t getAction() const;

private:
	const Action_t m_action;
	int m_nodes[256];
	int m_numberOfNodes;
};
/**************************************************************/



/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppCfgLogInfo 
{
public:
	AppCfgLogInfo( );

	void add(const char* s);
	const CStringList& getStrings() const;

private:
	CStringList m_stringList;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
typedef enum {
	BINDINGFILE_FSM_STATUS_IDLE = 0,
	BINDINGFILE_FSM_STATUS_BUSY,
	BINDINGFILE_FSM_STATUS_DONE,
	BINDINGFILE_FSM_STATUS_CANCELLED,
	BINDINGFILE_FSM_STATUS_ERROR

} BindingFileFsmStatus_t;
/**************************************************************/
class AppCfgBindInfoNode
{
public:
	AppCfgBindInfoNode( );
	AppCfgBindInfoNode(const AppCfgBindInfoNode& l);
	~AppCfgBindInfoNode( );

	AppCfgBindInfoNode* clone();
	AppCfgBindInfoNode& operator=(const AppCfgBindInfoNode& l);

	void setNodeAddress(int nodeAddress);
	int getNodeAddress() const { return m_nodeAddress; }

	int getBindFileNrOfEntries( ) const { return m_bindFileNrOfEntries; }
	void setBindFileNrOfEntries(int v);

	int getBindFileChecksum() const { return m_bindFileChecksum; }
	void setBindFileChecksum(int v);
	
	void setBindingsProgressCurrent(int v);
	int getBindingsProgressCurrent( ) const;
	void setBindingsProgressMax(int v);	
	int getBindingsProgressMax( ) const;

/*
	void setError(AppBindCfgError_t error);
	AppBindCfgError_t getError() const;
	const char* getStrError() const;
*/
	BindingFileFsmStatus_t getStatus( ) const { return m_status; }
	void setStatus(BindingFileFsmStatus_t status) { m_status = status;}

	void formatSummary(CString* dest);

private:
	int m_nodeAddress;					// Node address van de node.
	int m_bindFileNrOfEntries;			// Binding file property: Indien -1 dan werd de bindinfile niet geopend.
	int m_bindFileChecksum;				// Binding file property: Indien -1 dan werd de bindinfile niet geopend.
	int m_nrOfEntries;					// progress: aantal te lezen/schrijven.
	int m_countBindDone;				// progress: aantal gedaan.
	BindingFileFsmStatus_t m_status;	// de status.
	// AppBindCfgError_t m_error;			// de laatste fout
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppCfgBindInfoNodes
{
public:	
	AppCfgBindInfoNodes( );
	~AppCfgBindInfoNodes( );

	AppCfgBindInfoNodes& operator=(const AppCfgBindInfoNodes& l);

	void addNode(AppCfgBindInfoNode* newNode);

	BindingFileFsmStatus_t getStatus() const { return m_status; }
	void setStatus(BindingFileFsmStatus_t s) { m_status = s; }

	int getNodesProgressCurrent() const { return m_currentNodeIndex; }
	void setNodesProgressCurrent(int v) { m_currentNodeIndex = v; }

	int getNodesProgressMax() const { return m_maxNodes; }
	void setNodesProgressMax(int v) { m_maxNodes = v; }

	void formatSummary(CStringList* dest);

	int getCount() const;
	bool getNodeInfo(int index,AppCfgBindInfoNode** ppInfo) const;

private:
	BindingFileFsmStatus_t m_status;
	CList<AppCfgBindInfoNode*> m_listNodes;
	int m_currentNodeIndex;					// progress: huidige node.
	int m_maxNodes;							// progress: max aantal nodes
};
/**************************************************************/
class AppCfgNodeInfo 
{
public:
	AppCfgNodeInfo( );

	void setLogicalNodeAddress(int v);
	int getLogicalNodeAddress( ) const {
		return m_logicalNodeAddress;
	}

	void setMasterAddress(int v);
	int getMasterAddress( ) const {
		return m_masterAddress;
	}

	void setNumberOfUnits(int v);
	int getNumberOfUnits( ) const {
		return m_numberOfUnits;
	}

	void setPhysicalNodeAddress(int v);
	int getPhysicalNodeAddress() const {
		return m_physicalAddress;
	}

	const CString& getName( ) const {
		return m_name;
	}
	void setName(const CString& s);

	const CString& getProductName( ) const {
		return m_productName;
	}
	void setProductName(const CString& s);

	const CString& getSoftwareVersion( ) const {
		return m_softwareVersion;
	}
	void setSoftwareVersion(const CString& s);

	const CString& getNicSoftwareVersion( ) const {
		return m_nicSoftwareVersion;
	}
	void setNicSoftwareVersion(const CString& s);

	void setQtVersion(const CString& version);
	const CString& getQtVersion() const {
		return m_qtVersion;
	}

	void setHardware(const CString& v);
	const CString& getHardware() const {
		return m_hardware;
	}

	void format(CStringList* dest);

private:
	int m_logicalNodeAddress;
	int m_masterAddress;
	int m_numberOfUnits;
	int m_physicalAddress;
	CString m_name;	
	CString m_productName;
	CString m_softwareVersion;
	CString m_nicSoftwareVersion;
	CString m_qtVersion;
	CString m_hardware;
};


/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppCfgFileServerFile 
{
public:
	typedef enum {
		TYPE_UNKNOWN = -1,
		TYPE_DIR = 0,
		TYPE_FILE
	} type_t;

	AppCfgFileServerFile( );
	
	type_t getType() const { return m_type; }
	void setType(type_t type) { m_type = type; }

	void setFileName(const CString& s) {  m_fileName = s; }
	const CString& getFileName( ) const { return m_fileName; }

	void setSize(int size) { m_size = size; };
	int getSize( ) const { return m_size; };

private:
	type_t m_type;
	CString m_fileName;
	int m_size;
};
/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppCfgFileServerDir
{
public:
	AppCfgFileServerDir();
	~AppCfgFileServerDir( );

	void addFile(AppCfgFileServerFile* newNode);
	int getNrOfFiles( ) const;
	bool getFile(int idx,AppCfgFileServerFile** ppFile);

private:
	CList<AppCfgFileServerFile*> m_listFiles;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppCfgClientCredentials
{
public:
	AppCfgClientCredentials();

	void setAccessToken(const CString& s) { m_accessToken = s; }
	const CString& getAccessToken() const { return m_accessToken; }

	void setClientID(const CString& s) { m_clientID = s; }
	const CString& getClientID() const { return m_clientID; }

	void setRefreshToken(const CString& s) { m_refreshToken = s; }
	const CString& getRefreshToken() const { return m_refreshToken; }

	void setType(const CString& s) { m_type = s; }
	const CString& getType() const { return m_type; }

	void setExpiryTimeSec(int v) { m_expiryTimeSec = v; }
	int getExpiryTimeSec() const { return m_expiryTimeSec; }

protected:
	void clear();

protected:
	void update(const AppCfgClientCredentials& v);

private:	
	CString m_accessToken;
	CString m_clientID;
	int m_expiryTimeSec;
	CString m_refreshToken;
	CString m_type;
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppCfgRemoteAction
{
public:
	typedef enum {
		ACTION_UNDEFINED = -1,
		ACTION_WRITE_NAME = 0,

	} Action_t;

	virtual ~AppCfgRemoteAction() = 0;
	virtual AppCfgRemoteAction* clone() const {
		return 0;
	}
	virtual Action_t getID() {
		return ACTION_UNDEFINED;
	}
};

/**************************************************************/
/**************************************************************/
/**************************************************************/

// Deze klasse is algemene klasse.
// Bevat lijst met afgeleidde acties van AppCfgRemoteAction.
// o.a. AppCfgRemoteActionWriteName
class AppCfgRemoteActionList
{
public:
	AppCfgRemoteActionList();
	AppCfgRemoteActionList(const AppCfgRemoteActionList& refList);
	virtual ~AppCfgRemoteActionList();

	void add(AppCfgRemoteAction* item);
	int getCount() const;
	bool get(int index, AppCfgRemoteAction** ppItem) const;

protected:
	void deallocate();
	CList<AppCfgRemoteAction*>& getList();

private:
	CList<AppCfgRemoteAction*> m_list;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppCfgRemoteActionWriteName : public AppCfgRemoteAction
{
public:
	AppCfgRemoteActionWriteName();
	AppCfgRemoteActionWriteName(int nodeAddress, int unitAddress, const char* szName);
	AppCfgRemoteActionWriteName(const AppCfgRemoteActionWriteName& r);
	virtual ~AppCfgRemoteActionWriteName();

	AppCfgRemoteAction* clone() const;
	virtual Action_t getID() {
		return ACTION_WRITE_NAME;
	}

	AppCfgRemoteActionWriteName& operator=(const AppCfgRemoteActionWriteName& entry);
	void clear();
	int getNodeAddress() const;
	int getUnitAddress() const;
	const char* getName() const;

protected:
	void deallocName();
	void setName(const char* szName);

private:
	int m_nodeAddress;
	int m_unitAddress;
	char* m_szName;

	typedef AppCfgRemoteAction Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppCfgCmnProgressInfo
{
public:
	typedef enum {
		STATE_IDLE = 0,
		STATE_BUSY,
		STATE_DONE_SUCCESS,
		STATE_DONE_ERROR,
		STATE_DONE_CANCELLED

	} State_t;

	AppCfgCmnProgressInfo();
	~AppCfgCmnProgressInfo();

	void setState(State_t state);
	State_t getState() const;

private:
	State_t m_state;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgFirmwareVersion
{
public:
	AppRemoteCfgFirmwareVersion();
	virtual ~AppRemoteCfgFirmwareVersion();

	AppRemoteCfgFirmwareVersion* clone();

	// Property version
	const CString& getVersion() const {
		return m_version;
	}
	void setVersion(const CString& v) {
		m_version = v;
	}

	// property ipk.
	const CString& getIpk() const {
		return m_refIpk;
	}
	void setIpk(const CString& v) {
		m_refIpk = v;
	}

	// property productname
	const CString& getProductName() const {
		return m_productName;
	}
	void setProductName(const CString& v) {
		m_productName = v;
	}

	// property platform
	const CString& getPlatform() const {
		return m_platform;
	}
	void setPlatform(const CString& v) {
		m_platform = v;
	}

	// property release 
	const bool getFlagReleaseVersion() const {
		return m_release;
	}
	void setFlagReleaseVersion(bool flag) {
		m_release = flag;
	}
private:
	CString m_version;
	CString m_refIpk;
	CString m_productName;
	CString m_platform;
	bool m_release;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgFirmwareVersionInfo
{
public:
	AppRemoteCfgFirmwareVersionInfo();
	virtual ~AppRemoteCfgFirmwareVersionInfo();

	AppRemoteCfgFirmwareVersionInfo& operator=(const AppRemoteCfgFirmwareVersionInfo& l);

	void clear();
	void add(AppRemoteCfgFirmwareVersion* item);
	int getCount() const;
	bool get(int index, AppRemoteCfgFirmwareVersion** ppItem) const;

protected:
	void deallocate();

private:
	CList<AppRemoteCfgFirmwareVersion*> m_list;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
struct AppRemoteCfgFirmwareVersionInfo_Utils
{
	static bool isValid(const AppRemoteCfgFirmwareVersionInfo& r);
	static bool find(const AppRemoteCfgFirmwareVersionInfo& r, const CString& productName, const CString& platform, 
		bool releaseVersion, const AppRemoteCfgFirmwareVersion** ppVersion);
};

/**************************************************************/
}; /* namespace DUOTECNO*/
/**************************************************************/
#endif 
/**************************************************************/