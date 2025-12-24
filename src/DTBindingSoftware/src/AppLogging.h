#pragma once
/***********************************************************************************************/
class AppLogging
{
public:
	static AppLogging& getInstance();

	void addInfo(const CString& msg);
	void addError(const CString& msg);
	void addWarning(const CString& msg);

protected:
	enum { 
		MAX_LOGFILES = 20		// Het aantal logbestanden die aanwezig mag zijn in de ProgramData directory.
	};
	AppLogging();
	void add(const CString& msg);
	void init();
	void cleanUpDirectory(const CString& strDir);

private:
	CString getTimeDate( );

private:
	CString m_strFileName;
};

/***********************************************************************************************/
#ifndef ENABLE_TRACE_PROPERTYNODES_SHEET
	#define ENABLE_TRACE_PROPERTYNODES_SHEET		1
#endif

#ifndef ENABLE_TRACE_BINDINGFILEINFO_INFO
	#define ENABLE_TRACE_BINDINGFILEINFO_INFO		0
#endif
/***********************************************************************************************/