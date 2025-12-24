#pragma once

#include "utils/mfc/TPtrList.h"

////////////////////////////////////////////////

class CConfigIRTXFileEntry
{
public:
	class CHeader
	{
	public:
		CHeader( ) : m_szDevice( ), m_szFunction( ) { }
		~CHeader( ) { };

		const CString& getDeviceName( ) const
		{
			return m_szDevice;
		}
		const CString& getFunctionName( ) const
		{
			return m_szFunction;
		}
		void setDeviceName(const char* s) 
		{
			m_szDevice.Format("%.16s",s);
		}
		void setFunctionName(const char* s)
		{
			m_szFunction.Format("%.16s",s);
		}
	private:
		CString m_szDevice;
		CString m_szFunction;

		friend class CConfigIRTXFileEntry;
	};

	class CData
	{
	public:
		CData( ) : m_pData(0) { };
		~CData( ) { };
	private:
		BYTE* m_pData;			/* todo */
	};

public:
	CConfigIRTXFileEntry( );
	virtual ~CConfigIRTXFileEntry();
	const CHeader& getHeader( ) const { return m_header; };
	const CData& getData( ) const { return m_data; };
	CHeader& getHeader( ) { return m_header; };
	CData& getData( ) { return m_data; };

private:
	CHeader m_header;
	CData m_data;

	friend class CConfigIRTXFile;
};

////////////////////////////////////////////////

class CConfigIRTXFile
{
public:
	CConfigIRTXFile(void);
	~CConfigIRTXFile(void);
	BOOL Load(CString szFilename);
	int getCount( ) const;
	CConfigIRTXFileEntry* get(int index);
	void removeAll( );

protected:
	void add(CConfigIRTXFileEntry& entry);		

private:
	int m_count;
	DUOTECNO::MFC_HELPER::TPtrList<CConfigIRTXFileEntry> m_list;
};

// inline functions
inline int CConfigIRTXFile::getCount( ) const
{
	return m_count;
}

////////////////////////////////////////////////