#pragma once
/***************************************************************************************************/
namespace DUOTECNO {
/***************************************************************************************************/
typedef enum {
	TASK_STATUS_IDLE = 0,
	TASK_STATUS_BUSY_CONNECT,
	TASK_STATUS_BUSY_UPLOAD,
	TASK_STATUS_BUSY_INSTALLING,
	TASK_STATUS_ERROR,
	TASK_STATUS_DONE,
	TASK_STATUS_UNKNOWN
} SshTaskStatus_t;
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
class SshMessage
{
public:
	typedef enum {
		TYPE_MSG_EMPTY = -1,
		TYPE_MSG_INFO = 0,
		TYPE_MSG_ERROR,
		TYPE_MSG_STATUS,
	} msgType_t;

	SshMessage();
	SshMessage(msgType_t type, const char* msg);
	SshMessage(SshTaskStatus_t status, const char* msg);
	SshMessage(const SshMessage& r);
	~SshMessage( );

	const SshMessage& operator=(const SshMessage& r);

	const msgType_t getType( ) const {
		return m_type;
	}
	const SshTaskStatus_t getStatus( )  const {
		return m_status;
	}
	const char* getMsg( ) const {
		return m_szMsg;
	}
protected:
	void setMsg(const char* msg);
	void dealloc();

private:
	msgType_t m_type;
	SshTaskStatus_t m_status;
	char* m_szMsg;
};
/***************************************************************************************************/
}; /*  namespace DUOTECNO { */
/***************************************************************************************************/