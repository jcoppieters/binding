#ifndef DUOTECNO_CONFIG_CANFTP_INFO_H
#define	DUOTECNO_CONFIG_CANFTP_INFO_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
//////////////////// CConfigCanFTP_FileEntry //////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_FileEntry 
{
public:
	typedef void (*PF_CALLBACK)(const CConfigCanFTP_FileEntry& entry);

	enum FtpAction
	{
		ACTION_FTP_WRITE = 0,
		ACTION_FTP_READ,
		ACTION_FTP_UNDEFINED
	};

	enum CtxFlags
	{
		CTXFLAG_READ_FILENOTFOUND_SUPPRESS_ERROR	= (1<<0)
	};


	CConfigCanFTP_FileEntry( );
	~CConfigCanFTP_FileEntry( );

	void setNodeAddress(BYTE nodeAddress);
	BYTE getNodeAddress( ) const;
	void setLocalFileName(const char* filename);
	const char* getLocalFileName( ) const;
	void setRemoteFileName(const char* filename);
	const char* getRemoteFileName( ) const;
	void setInfo(const char* info);
	const char* getInfo( ) const;
	FtpAction getAction( ) const;
	void setAction(FtpAction action);
	void setCallbackStarted(PF_CALLBACK pf);
	void setCallbackDone(PF_CALLBACK pf);
	PF_CALLBACK getCallbackStarted( ) const;
	PF_CALLBACK getCallbackDone( ) const;
	void setCtxFlags(int flags);			// zie enum CtxFlags
	int getCtxFlags(void);					// zie enum CtxFlags

protected:
	void deallocate(char** s);
	void set(char** szDest,const char* s);

private:
	FtpAction m_action;
	BYTE m_bNodeAddress;
	char* m_szLocalFileName;
	char* m_szRemoteFileName;
	char* m_szInfo;
	unsigned short m_ctxFlags;
	PF_CALLBACK m_pfCallbackStarted;
	PF_CALLBACK m_pfCallbackDone;

};
////////////////////// inline functies ///////////////////////////
inline void CConfigCanFTP_FileEntry::setCallbackStarted(
	CConfigCanFTP_FileEntry::PF_CALLBACK pf)
{
	m_pfCallbackStarted = pf;
}
inline void CConfigCanFTP_FileEntry::setCallbackDone(
	 CConfigCanFTP_FileEntry::PF_CALLBACK pf)
{
	m_pfCallbackDone = pf;
}
inline CConfigCanFTP_FileEntry::PF_CALLBACK 
	CConfigCanFTP_FileEntry::getCallbackStarted( ) const
{
	return m_pfCallbackStarted;
}
inline CConfigCanFTP_FileEntry::PF_CALLBACK 
	CConfigCanFTP_FileEntry::getCallbackDone( ) const
{
	return m_pfCallbackDone;
}
///////////////////////////////////////////////////////////////////

class CConfigCanFTP_FileEntryIterator
{
public:
	CConfigCanFTP_FileEntryIterator(CPtrArray& array);
	~CConfigCanFTP_FileEntryIterator( );

	CConfigCanFTP_FileEntry* CurrentItem( );
	void Begin( );
	void Next( );
	bool IsDone( );

private:
	CPtrArray& m_refArray;
	int m_currentItem;
};

///////////////////////////////////////////////////////////////////
//////////////////////// CConfigCanFTP_Info ///////////////////////
///////////////////////////////////////////////////////////////////
class CConfigCanFTP_Info
{
public:
	static CConfigCanFTP_Info& getInstance( );
	~CConfigCanFTP_Info( );

	void clear( );
	bool addTask(CConfigCanFTP_FileEntry* pNewEntry);
	CConfigCanFTP_FileEntry* create( );
	CConfigCanFTP_FileEntryIterator createIterator( );
	int getCount( ) const;

protected:
	CConfigCanFTP_Info( );
	void destroy(CConfigCanFTP_FileEntry* pEntry);
	void destroyAll( );

private:
	CPtrArray m_array;
};
////////////////////// inline functies ////////////////////////////
inline int CConfigCanFTP_Info::getCount( ) const
{
	return m_array.GetSize( );
}
///////////////////////////////////////////////////////////////////


#endif