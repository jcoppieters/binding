#include "stdafx.h"
#include "AppState.h"


/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
CAppFsmNodeTaskEntry::CAppFsmNodeTaskEntry() : m_task(APPFSM_TASK_IDLE), m_nodeAddress(-1) { }
CAppFsmNodeTaskEntry::CAppFsmNodeTaskEntry(Task_t task, int nodeAddress) : m_task(task), m_nodeAddress(nodeAddress) {}

CAppFsmNodeTaskEntry::~CAppFsmNodeTaskEntry() { }

CAppFsmNodeTaskEntry::Task_t CAppFsmNodeTaskEntry::getTask() const {
	return this->m_task;
}
void CAppFsmNodeTaskEntry::setTask(Task_t task) {
	this->m_task = task;
}

int CAppFsmNodeTaskEntry::getNodeAddress() const {
	return this->m_nodeAddress;
}
void CAppFsmNodeTaskEntry::setNodeAddress(int value) {
	this->m_nodeAddress = value;
}

CAppFsmNodeTaskEntry CAppFsmNodeTaskEntry::EMPTY_ITEM() {
	return CAppFsmNodeTaskEntry(APPFSM_TASK_IDLE, -1);
}
/*****************************************************************************************/

/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
CAppFsmNodeTaskEntries::CAppFsmNodeTaskEntries() : m_queueTasks(), m_currentTask() { }
CAppFsmNodeTaskEntries::~CAppFsmNodeTaskEntries() { }

void CAppFsmNodeTaskEntries::clear() {
	while(!this->isIdle()) {
		CAppFsmNodeTaskEntry entry = this->getNextTask();	
	};
	this->m_currentTask = CAppFsmNodeTaskEntry::EMPTY_ITEM();
	ENSURE(this->getCount() == 0);
}

bool CAppFsmNodeTaskEntries::isIdle() const {
	return (this->getCount() > 0 ? false : true);
}

bool CAppFsmNodeTaskEntries::addTask(CAppFsmNodeTaskEntry task) {
	if (task.getTask() == CAppFsmNodeTaskEntry::APPFSM_TASK_IDLE) {
		REQUIRE(0);
		return false;
	}
	this->m_queueTasks.Add(task);
	return true;
}

int CAppFsmNodeTaskEntries::getCount() const {
	return this->m_queueTasks.GetCount();
}

CAppFsmNodeTaskEntry CAppFsmNodeTaskEntries::getNextTask() {
	REQUIRE(!this->isIdle());
	REQUIRE(this->getCount() > 0);

	CAppFsmNodeTaskEntry task = this->m_queueTasks.getFirst();
	this->m_currentTask = task;
	return this->m_currentTask;
}

CAppFsmNodeTaskEntry CAppFsmNodeTaskEntries::peekNextTask( ) {
	REQUIRE(!this->isIdle());
	REQUIRE(this->getCount() > 0);
	return this->m_queueTasks.peek();
}

CAppFsmNodeTaskEntry CAppFsmNodeTaskEntries::getCurrentTask() const {
	return this->m_currentTask;
}


/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
bool CAppFsmNodeTaskEntries_Utils::addTask(CAppFsmNodeTaskEntries& me, CAppFsmNodeTaskEntry::Task_t task, int nodeAddress) {
	const CAppFsmNodeTaskEntry taskEntry(task,nodeAddress);
	return me.addTask(taskEntry);
}

// Zoeken of deze entry reeds aanwezig is.
bool CAppFsmNodeTaskEntries_Utils::find(CAppFsmNodeTaskEntries& me, const CAppFsmNodeTaskEntry& ref) {
	if (me.m_queueTasks.GetCount() <= 0) {
		return false;
	}
	POSITION pos = me.m_queueTasks.GetHeadPosition();
	while(pos!=NULL) {
		CAppFsmNodeTaskEntry entry = me.m_queueTasks.GetNext(pos);
		if ((entry.getTask() == ref.getTask()) && (entry.getNodeAddress() == ref.getNodeAddress())) {
			return true;
		}
	}
	return false;
}

/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
// Singleton
CAppState& CAppState::getInstance(void)
{
	static CAppState _instance;
	return _instance;
}

// Ctor & dtor/
CAppState::CAppState(void) :
m_appType(APP_TYPE_INSTALLATION), m_appState(APP_STATE_IDLE), m_appDbasePostBrowseError( ), m_appBrowseError( ),
m_appMode( ), m_appDbaseAction( ), m_permisions( ), m_taskFlags(0), m_enableMenuItemDaliCfg(false), m_enableUseRemoteTCPCloudServer(false),
m_appFsmTaskEntries()
{
}

CAppState::~CAppState(void)
{
}

// Member functions.
void CAppState::setAppType(Type_t type) {
	m_appType = type;
}

CAppState::Type_t CAppState::getAppType(void) const {
	return m_appType;
}

CAppState::States_t CAppState::getAppState(void) const {
	return m_appState;
}

void CAppState::setAppState(States_t state) {
	m_appState = state;
}

void CAppState::setTask(TaskFlags_t flag) {
	this->m_taskFlags |= flag;
}
bool CAppState::getTask(TaskFlags_t flag) const {
	return ((this->m_taskFlags & flag) != 0);
}

CAppFsmNodeTaskEntries& CAppState::getTaskEntries() {
	return this->m_appFsmTaskEntries;
}
/**************************************************************************/
void CAppStateUtil::setAppStateIdle(CAppState& ref) {
	ref.setAppState(CAppState::APP_STATE_IDLE);
	ref.m_appDbasePostBrowseError.Set_Idle();
	ref.m_appBrowseError.Set_Idle();
	ref.m_appMode.Set_Idle();
	ref.m_appDbaseAction.Set_Idle();
	ref.m_appFsmTaskEntries.clear();
	ref.m_taskFlags = 0;
}

const CAppGlobalDbaseActions::Actions CAppStateUtil::getDbaseActions(const CAppState& ref)
{
	return ref.m_appDbaseAction.Get();
}

void CAppStateUtil::setDbaseActions(CAppState& ref,CAppGlobalDbaseActions::Actions actions)
{
	ref.m_appDbaseAction.Set(actions);
}

const CAppMode::Types CAppStateUtil::getAppMode(const CAppState& ref)
{
	return ref.m_appMode.Get();
}

void CAppStateUtil::setAppMode(CAppState& ref,CAppMode::Types type)
{
	ref.m_appMode.Set(type);
}

const CAppBrowseError::BrowseError CAppStateUtil::getAppBrowseError(const CAppState& ref)
{
	return ref.m_appBrowseError.Get();
}

void CAppStateUtil::setAppBrowseError(CAppState& ref,CAppBrowseError::BrowseError ec)
{
	ref.m_appBrowseError.Set(ec);
}	

BOOL CAppStateUtil::IsPreBrowseError(const CAppState& ref)
{
	const CAppBrowseError::BrowseError ec = ref.m_appBrowseError.Get();
	return ((ec == CAppBrowseError::BROWSE_EC_DUPLICATES_FOUND) ||
		    (ec == CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND) ||
			(ec == CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND_ASSIGN_MASTER));
}

const CAppDbasePostBrowseError::Result CAppStateUtil::getAppDbasePostBrowseErrorResult(const CAppState& ref)
{
	return ref.m_appDbasePostBrowseError.Get();
}

void CAppStateUtil::setAppDbasePostBrowseErrorResult(CAppState& ref,CAppDbasePostBrowseError::Result v)
{
	ref.m_appDbasePostBrowseError.Set(v);
}


/***********************************************************/
CAppPermisions::CAppPermisions( ) : m_flags(0) { }
CAppPermisions::~CAppPermisions( ) { }

void CAppPermisions::clear( ) {
	m_flags = 0;
}

// Mogen via tcpip met de touchscreen connecteren 
bool CAppPermisions::getTcpCfgTouchscreenEnabled( ) const {
	return (FLAG_CONNECT_TCP_TOUCHSCREEN == (m_flags & FLAG_CONNECT_TCP_TOUCHSCREEN));
}

void CAppPermisions::setTcpCfgTouchscreenEnabled(bool value) {
	if (value) {
		m_flags |= FLAG_CONNECT_TCP_TOUCHSCREEN;		
	}
	else {
		m_flags &= ~FLAG_CONNECT_TCP_TOUCHSCREEN;
	}
}
