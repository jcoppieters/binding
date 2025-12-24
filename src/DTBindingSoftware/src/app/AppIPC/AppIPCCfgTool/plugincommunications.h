#ifndef DUOTECNO_PLUGIN_COMMUNICTATION_H
#define DUOTECNO_PLUGIN_COMMUNICTATION_H
/*******************************************************/
namespace DUOTECNO {
namespace PLUGIN {
/*******************************************************/

/*******************************************************/
class CMFCOpenFile {
public:
	static BOOL OpenFile(const CString& szFileName,const CString& szParameters);
	static BOOL OpenWindow(const CString& szWindowName);
};
/*******************************************************/

/*******************************************************/
class CMFCFindWindow {
public:
	enum
	{
		FLAG_NAME_WINDOW = 0x01,
		FLAG_NAME_CLASS  = 0x02
	};
	static CWnd* Find(const CString& szName,int flags);
};
/*******************************************************/

struct CMFCMessage
{
	virtual ~CMFCMessage( ) { };
	virtual UINT getWindowMessage( ) const	= 0;	
	virtual WPARAM getWParam( ) const		= 0;
	virtual LPARAM getLParam( ) const		= 0;
};

/********************************************************/
class CMFCMsgCopyData : public CMFCMessage
{
public:
	CMFCMsgCopyData( );
	CMFCMsgCopyData(CWnd& srcWindow,CString szMessage);	
	CMFCMsgCopyData(const COPYDATASTRUCT& copyDataStruct);
	~CMFCMsgCopyData( );

	void Build(CWnd& srcWindow,CString szMessage);	
	virtual UINT getWindowMessage( ) const;
	virtual WPARAM getWParam( ) const;
	virtual LPARAM getLParam( ) const;
	CWnd* getWindowSrc( ) const;
	CString getMessage( ) const;

protected:
	void BuildMessage(CWnd& srcWindow,const CString& szMessage);	
	void Clear( );

private:
	COPYDATASTRUCT m_copyDataStructure;
};

/**********************************************/
inline UINT CMFCMsgCopyData::getWindowMessage( ) const
{
	return WM_COPYDATA;
}
/**********************************************/

/******************* free functions ********************/
extern LRESULT SendWindowMessage(CWnd& destWindow,const CMFCMessage& msg);
extern BOOL PostWindowMessage(CWnd& destWindow,const CMFCMessage& msg);
/*******************************************************/

/*******************************************************/
};
};
/*******************************************************/
#endif
/*******************************************************/

