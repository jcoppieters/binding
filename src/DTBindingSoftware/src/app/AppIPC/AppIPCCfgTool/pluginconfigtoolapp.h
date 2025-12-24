#ifndef DUOTECNO_PLUGIN_CONFIGTOOLAPP_H
#define DUOTECNO_PLUGIN_CONFIGTOOLAPP_H
/*******************************************************/
#ifndef DUOTECNO_PLUGIN_ICONFIGTOOLAPP_H
	#include "ipluginconfigtoolapp.h"
#endif
#ifndef _MAP_
	#include <map>
#endif
/*******************************************************/
namespace DUOTECNO {
namespace PLUGIN {
/*******************************************************/

/****************************************************//**
 * @class	CConfigToolApp
 * @brief	Implementatie van de IConfigToolApp interface.
 *
 *******************************************************/
class CConfigToolApp : public IConfigToolApp
{
public:
	virtual ~CConfigToolApp( );

	virtual void show(View view, BYTE bNodeAddress);
	virtual void load(View view, BYTE bNodeAddress);
	virtual void show( );
	virtual void newDirectory( );
	virtual void close( );
	virtual void save( );
	virtual void refreshDisplay( );
	virtual void exportHTMLFiles( );
	virtual BOOL findWindowConfigurator(CWnd** pWnd);
	virtual ICommandStringParser* parseReceived(CString& s);
	
protected:
	CConfigToolApp( );

	void sendMessage(CWnd* const pDestWindow, Command command, bool bringToFront = false, const char* szFullName = 0,
		View view = VIEW_NONE, BYTE bNodeAddress = 0xFF);

private:
	friend struct IConfigToolApp;

	// Friend functie om tests uit te voeren.
	friend int pluginconfigtoolapp_testcase(int testcase);
};


/**************************************************//**
 * @class	CCommandStringFormatter
 * @brief	Klasse die een commando string aanmaakt.
 *****************************************************/
struct CCommandStringFormatter
{
	static BOOL Build(CString* pString,Command command, const char* szFullName = 0, View view = VIEW_NONE,
		BYTE bNodeAddress = 0xFF);
};
/*******************************************************/

class CCommandStringParser : public ICommandStringParser
{
public:
	CCommandStringParser(CString& str);
	BOOL Parse( );

	virtual Command getCommand() const;
	virtual CString getFullName() const;
	virtual View getView() const;
	virtual int getNodeAddress( ) const;

private:
	CString& m_str;
	std::map<CString, CString> m_parsedValues;
};
/*******************************************************/
};
};
/*******************************************************/
#endif
/*******************************************************/

