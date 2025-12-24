#pragma once
/*****************************************************************************************/


/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
// sedert V16.47: Automatisch herstellen van de unit configuratie in een module.
class CAppFsmNodeTaskEntry {
public:
	typedef enum {
		APPFSM_TASK_IDLE = 0,
		APPFSM_TASK_SENDUNITS
	}Task_t;

	CAppFsmNodeTaskEntry();
	CAppFsmNodeTaskEntry(Task_t task, int nodeAddress);
	~CAppFsmNodeTaskEntry();

	Task_t getTask() const;
	void setTask(Task_t task);

	int getNodeAddress() const;
	void setNodeAddress(int value);

	static CAppFsmNodeTaskEntry CAppFsmNodeTaskEntry::EMPTY_ITEM();
	
private:
	Task_t m_task;
	int m_nodeAddress;
};
/*****************************************************************************************/


/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
// sedert V16.47: Automatisch herstellen van de unit configuratie in een module.
class CAppFsmNodeTaskEntries {
public:
	CAppFsmNodeTaskEntries();
	~CAppFsmNodeTaskEntries();

	void clear();
	bool isIdle() const;	// Geen taken meer uit te voeren.
	bool addTask(CAppFsmNodeTaskEntry task);
	int getCount() const;

	CAppFsmNodeTaskEntry peekNextTask( );
	CAppFsmNodeTaskEntry getNextTask();
	CAppFsmNodeTaskEntry getCurrentTask() const;

private:
	class CQueue : public CList<CAppFsmNodeTaskEntry> {
	public:
		void Add(CAppFsmNodeTaskEntry entry) {
			this->AddTail(entry);
		}
		CAppFsmNodeTaskEntry getFirst() {
			if (this->IsEmpty()) {
				return CAppFsmNodeTaskEntry::EMPTY_ITEM();
			}
			return this->RemoveHead();
		}
		CAppFsmNodeTaskEntry peek() {
			if (this->IsEmpty()) {
				return CAppFsmNodeTaskEntry::EMPTY_ITEM();
			}
			return this->GetHead();
		}
	};

private:
	CQueue m_queueTasks;
	CAppFsmNodeTaskEntry m_currentTask;

	friend struct CAppFsmNodeTaskEntries_Utils;
};
/*****************************************************************************************/
struct CAppFsmNodeTaskEntries_Utils {
	// Entry invoegen - doet geen controle of deze reeds aanwezig is.
	static bool addTask(CAppFsmNodeTaskEntries& me, CAppFsmNodeTaskEntry::Task_t task, int nodeAddress);

	// Zoeken of deze entry reeds aanwezig is.
	static bool find(CAppFsmNodeTaskEntries& me, const CAppFsmNodeTaskEntry& ref);
};


/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
class CAppGlobalDbaseActions {
public:	
	enum Actions {
		APPDBASE_ACTION_IDLE = 0,
		APPDBASE_ACTION_PNP,
		APPDBASE_ACTION_BROWSE,
		APPDBASE_ACTION_SENDNAMES,
		APPDBASE_ACTION_SENDBINDINGS,
		APPDBASE_ACTION_SENDPROGFILE
	};
	CAppGlobalDbaseActions() : m_Value(APPDBASE_ACTION_IDLE) {};
	CAppGlobalDbaseActions(Actions value) : m_Value(value) { };

protected:
	Actions m_Value;

public:
	Actions Get() const {
		return m_Value;
	}
	void Set(Actions value) {
		m_Value = value;
	}
	void Set_Idle() {
		m_Value = APPDBASE_ACTION_IDLE;
	}
};
/*****************************************************************************************/

class CAppMode
{
public:

	enum Types
	{	
		APP_NEW_OFFLINE = 0,
		APP_NEW_ONLINE,
		APP_CONTINUE_OFFLINE,
		APP_CONTINUE_ONLINE,
		APP_IDLE
	};

	CAppMode() : m_Value(APP_IDLE) {};
	CAppMode(Types value) : m_Value(value) { };

protected:
	Types m_Value;

public:
	Types Get() const {
		return m_Value;
	}
	void Set(Types value) {
		m_Value = value;
	}
	void Set_Idle() {
		m_Value = APP_IDLE;
	}
};

/*****************************************************************************************/
class CAppBrowseError
{
public:	

	// sedert V16.44: extra fout 'BROWSE_EC_FACTORY_ADDRESSES_FOUND_ASSIGN_MASTER'
	// Momenteel kunnen enkel de Sbox ultra als master geconfigueerd worden.
	// (Aanpassing in de firmware: V66.13.08: Bij het toekennen van een 0xFC adres ook configureren als master.)
	enum BrowseError {
		BROWSE_EC_SUCCESS = 0,
		BROWSE_EC_CANCEL,
		BROWSE_EC_ERROR,
		BROWSE_EC_NO_RESPONSE,
		BROWSE_EC_DUPLICATES_FOUND,							// Pre browse errors: Dubbele logische adressen.
		BROWSE_EC_FACTORY_ADDRESSES_FOUND,					// Pre browse errors: 0xFE adressen gevonden.
		BROWSE_EC_FACTORY_ADDRESSES_FOUND_ASSIGN_MASTER		// Pre browse errors: 0xFE adressen gevonden + geen master + Mogelijkheid om een master toe te kennen (Sbox ultra modules aanwezig.)
	};

	CAppBrowseError() : m_Value(BROWSE_EC_SUCCESS) {};
	CAppBrowseError(BrowseError value) : m_Value(value) { };

protected:
	BrowseError m_Value;

public:
	BrowseError Get() const { 
		return m_Value;
	}
	void Set(BrowseError value) {
		m_Value = value;
	}	
	void Set_Idle() {
		m_Value = BROWSE_EC_SUCCESS;
	}
};
/*****************************************************************************************/

/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
class CAppDbasePostBrowseError
{
public:		
	enum Result {
		DBASE_EC_COMPARE_SUCCESS	= 0,
		DBASE_EC_NODES_NOT_FOUND,
		DBASE_EC_NODES_ADDED,
		DBASE_EC_NODES_INCOMPATIBLE,
		DBASE_EC_PROGRAMFILE_DIFFERENCE,
		DBASE_EC_NAMES_DIFFERENCE
	};

	CAppDbasePostBrowseError() : m_Value(DBASE_EC_COMPARE_SUCCESS) { };
	CAppDbasePostBrowseError(Result value) : m_Value(value) {};

protected:
	Result m_Value;

public:
	Result Get() const {
		return m_Value;
	}
	void Set(Result value) {
		m_Value = value;
	}
	void Set_Idle() {
		m_Value = DBASE_EC_COMPARE_SUCCESS;
	}
};

/*********************************************************/

class CAppState;

struct CAppStateUtil
{
	static void setAppStateIdle(CAppState& ref);

	static const CAppGlobalDbaseActions::Actions getDbaseActions(const CAppState& ref);
	static void setDbaseActions(CAppState& ref,CAppGlobalDbaseActions::Actions actions);

	static const CAppMode::Types getAppMode(const CAppState& ref);
	static void setAppMode(CAppState& ref,CAppMode::Types type);

	static const CAppBrowseError::BrowseError getAppBrowseError(const CAppState& ref);
	static void setAppBrowseError(CAppState& ref,CAppBrowseError::BrowseError ec);
	static BOOL IsPreBrowseError(const CAppState& ref);

	static const CAppDbasePostBrowseError::Result getAppDbasePostBrowseErrorResult(const CAppState& ref);
	static void setAppDbasePostBrowseErrorResult(CAppState& ref,CAppDbasePostBrowseError::Result v);
};

/***********************************************************/

// Sedert V16.14: Gaan deze in een klasse stoppen.
// Mogelijkheid uitbreiden met physical node address.
class CAppPermisions
{
public:
	CAppPermisions( );
	virtual ~CAppPermisions( );

	void clear( );

	// Mogen via tcpip met de touchscreen connecteren 
	bool getTcpCfgTouchscreenEnabled( ) const;
	void setTcpCfgTouchscreenEnabled(bool value);

private:
	enum {
		FLAG_CONNECT_TCP_TOUCHSCREEN	= (1<<0)		// Mogen via tcpip met de ts connecteren.
	};
	unsigned int m_flags;
};
/***********************************************************/

class CAppState
{
public:
	typedef enum {
		APP_TYPE_INSTALLATION = 0,
		APP_TYPE_ENGINEERING,
		APP_TYPE_SIMULATION
	} Type_t;

	// Sedert V16.32: zie BUG-0175
	// Tijdens de binding configuratie werd de Global FSM nog niet in de toestand APP_STATE_BUSY_BINDINGS_WRITE geplaatst.
	//
	// TODO: Andere 'States_t' invoegen. 
	// Nu wordt bij alle andere de APP_STATE_BUSY_CONFIGURATION gebruikt wanneer deze start met een taak.
	typedef enum
	{
		APP_STATE_IDLE = 0,							// Geen taken bezig. (Wel Pnp/Browse - zie mode.)
		APP_STATE_BUSY_BINDINGS_WRITE,
		APP_STATE_BUSY_CONFIGURATIONFILE_WRITE,
		APP_STATE_BUSY_CONFIGURATION,				// Sedert V16.32: andere moeten nog uitgewerkt worden
		APP_STATE_BUSY_BROWSE_SINGLE_NODE
#if(0)
		APP_STATE_NEW,
		APP_STATE_BROWSE,
		APP_STATE_AUTOPNP,		
		APP_STATE_SENDNAMES,
		APP_STATE_READBINDINGS,
		APP_STATE_COMPARE_DATABASES,
		APP_STATE_UPDATELCDSCREENMAP,
		APP_STATE_MEMORYDUMP,
		APP_STATE_UPLOADBITMAPS,
		APP_STATE_MMCONFIG,
		APP_STATE_REQUESTFORSTATUSSEN		
#endif

	} States_t;

public:	
	~CAppState(void);
	static CAppState& getInstance(void);

	// Property m_appType
	void setAppType(Type_t type);
	Type_t getAppType(void) const;

	// Property m_appState
	States_t getAppState(void) const;
	void setAppState(States_t state);

	// permision connect via tcp with the touchscreen...
	CAppPermisions& getAppPermisions( ) {
		return m_permisions;
	}

	// Sedert V16.36
	void setEnableMenuItemDaliCfg(bool flagOnOff) {
		this->m_enableMenuItemDaliCfg = flagOnOff;	
	}
	bool getEnableMenuItemDaliCfg() const {
		return this->m_enableMenuItemDaliCfg;
	}


	// Sedert V17.03: CHANGES_CLOUDBRIDGE_POC:
	// Commandline - optie: `-enable_tcp_cloud`
	// Bij inschakeling verloopt remote TCP / IP - communicatie via de cloudserver i.p.v.direct via DNS.
	void setEnableUseRemoteTCPCloudServer(bool flagOnOff) {
		this->m_enableUseRemoteTCPCloudServer = flagOnOff;
	}
	bool getEnableUseRemoteTCPCloudServer( ) const {
		return this->m_enableUseRemoteTCPCloudServer;
	}

	//////////////////////////////////////////////////////////////////////////
	// Sedert V16.41:
	// Mogelijkheid om op te vragen welke taken er uitgevoerd werden...
	// Er kunnen meerdere taken na elkaar uitgevoerd worden.
	typedef enum {
		TASK_FLAG_BROWSE_FOR_NODES		= (1<<0),
		TASK_FLAG_REQUEST_FOR_STATES	= (1<<1),
		TASK_FLAG_PNP					= (1<<2),
		TASK_FLAG_BINDINGS_WRITE		= (1<<3),
		TASK_FLAG_BROWSE_SINGLE_NODE	= (1<<4)
		// Aanvullen wanneer we deze nodig hebben...	
	} TaskFlags_t;

	void setTask(TaskFlags_t flag);
	bool getTask(TaskFlags_t flag) const;
	CAppFsmNodeTaskEntries& getTaskEntries();
	
private:
	CAppState(void);

private:
	Type_t m_appType;
	States_t m_appState;
	CAppDbasePostBrowseError m_appDbasePostBrowseError;
	CAppBrowseError m_appBrowseError;
	CAppMode m_appMode;
	CAppGlobalDbaseActions m_appDbaseAction;
	CAppPermisions m_permisions;
	unsigned short m_taskFlags;
	bool m_enableMenuItemDaliCfg;						// sedert V16.36
	bool m_enableUseRemoteTCPCloudServer;				// V17.03: CHANGES_CLOUDBRIDGE_POC - POC TCP/IP communicatie over de cloud server.
	CAppFsmNodeTaskEntries m_appFsmTaskEntries;			// sedert V16.47: Automatisch herstellen van de unit configuratie in een module.

	friend struct CAppStateUtil;
};
