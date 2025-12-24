// NodeScreenmapList.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NodeScreenmapList.h"

#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

/////////////////////////////////////////////////////////////////////////////
template <class ITEM, class REF_ITEM>
	class TListIterator
{
public:
	TListIterator(CList<ITEM,REF_ITEM>& parentList) :
	  m_ParentList( parentList )
	{
		_Begin( );  
	}
	~TListIterator( ) { }

	void Begin()			{ _Begin(); };
	void Next()				{ _Next(); };
	bool IsDone() const		{ return _IsDone(); }

	REF_ITEM CurrentItem() 
	{ 
		return m_ParentList.GetAt(m_pos); 
	};
	const REF_ITEM CurrentItem() const 
	{ 
		return m_ParentList.GetAt(m_pos); 
	};

protected:
	inline void _Begin( )
	{
		m_pos = m_ParentList.GetHeadPosition( );
	}
	inline void _Next( )
	{
		m_ParentList.GetNext( m_pos );
	}
	inline bool _IsDone( ) const
	{
		return (m_pos == 0); 
	}

private:
	CList<ITEM,REF_ITEM>&	m_ParentList;
	POSITION				m_pos;
};
////////////////////////////////////////////////////////////////////////////////

template
<
   typename T1,
   typename T2
>
class Duo
{
public:
    typedef T1 Type1;  // type of first field
    typedef T2 Type2;  // type of second field
    enum { N = 2 };    // number of fields

private:
    T1 value1;         // value of first field
    T2 value2;         // value of second field

public:
	Duo() :
   	value1(T1()), value2(T2()) { }
    Duo (T1 const & a, T2 const & b)
     : value1(a), value2(b) { }

    // field access
    inline T1& v1() 					{ return value1; }
    inline T1 const& v1() const 	{ return value1; }
    inline T2& v2() 					{ return value2; }
    inline T2 const& v2() const 	{ return value2; }
};



template <class KEY, class REF_KEY, class VALUE, class REF_VALUE>
	class TMapIterator
{
private:
	typedef CMap<KEY,REF_KEY,VALUE,REF_VALUE> _CMap;

protected:
	_CMap&	m_Map;
	POSITION m_pos;
	KEY m_key;
	REF_VALUE m_item;

protected:
	inline void _Begin( )
	{
		m_pos = m_Map.GetStartPosition( );
		if (m_pos != 0)
		{
			_Next( );		
		}
	}

	inline void _Next( )
	{		
		m_Map.GetNextAssoc( m_pos, m_key, m_item );
	}

	inline bool _IsDone( ) const
	{
		return (m_pos == 0); 
	}

public:
	typedef Duo<KEY,REF_VALUE> PAIR;

	TMapIterator(_CMap& parentList) :
	  m_Map( parentList )
	{
		_Begin( );  
	}
	~TMapIterator( ) { }

public:
	void Begin()			{ _Begin(); };
	void Next()				{ _Next(); };
	bool IsDone() const		{ return _IsDone(); }

	PAIR CurrentItem() 
	{ 
		return PAIR(m_key,m_item);
	};
	const PAIR CurrentItem() const 
	{ 
		return PAIR(m_key,m_item);
	};
};
////////////////////////////////////////////////////////////////////////////////

CScreenmapIDList::CScreenmapIDList( ) :
m_map( )
{
}

CScreenmapIDList::~CScreenmapIDList( )
{
	destroy( );
}

void CScreenmapIDList::add(BYTE bNodeAddress, BYTE bScreenmapID)
{
	CListScreenmapID* listScreenmaps = 0;

	if (m_map.Lookup(bNodeAddress,listScreenmaps) == FALSE)
	{
		listScreenmaps = new CListScreenmapID( );

		m_map.SetAt(bNodeAddress, listScreenmaps);
	}


	if (listScreenmaps != 0)
	{
		if (listScreenmaps->Find(bScreenmapID) == FALSE)
		{
			listScreenmaps->AddTail( bScreenmapID );
		}
	}
}

void CScreenmapIDList::destroy( )
{
	POSITION pos = m_map.GetStartPosition( );

	while (pos != 0)
	{
		int key = 0;
		CListScreenmapID* listScreenmaps = 0;

		m_map.GetNextAssoc(pos, key, listScreenmaps);
		
		if (listScreenmaps != 0)
		{
			delete listScreenmaps;
			listScreenmaps = 0;
		}
	
	};

	m_map.RemoveAll( );
}


void CScreenmapIDList::dump( )
{
	typedef TMapIterator<int, int, CListScreenmapID*, CListScreenmapID*> MapIterator;

	for (MapIterator it(m_map);
	     !it.IsDone();
		 it.Next())
	{
		CListScreenmapID* listScreenmaps = it.CurrentItem( ).v2( );;

		cout << "NodeAddress = " << it.CurrentItem( ).v1( ) << endl;

		for (TListIterator<int,int> it(*listScreenmaps);
			 !it.IsDone( );
			 it.Next())
		{
			cout << it.CurrentItem() << ",";				
		} 
		cout << endl;		
	}

/*
	POSITION pos = m_map.GetStartPosition( );

	while (pos != 0)
	{
		int key = 0;
		CListScreenmapID* listScreenmaps = 0;

		m_map.GetNextAssoc(pos, key, listScreenmaps);
		
		if (listScreenmaps != 0)
		{
			cout << "NodeAddress = " << key << endl;

			for (TListIterator<int,int> it(*listScreenmaps);
				 !it.IsDone( );
				 it.Next())
			{
				cout << it.CurrentItem() << ",";				

			} 
			cout << endl;
		}	
	};
*/
}




//////////////////////////////////////////////////////////////////////////////
CScreenmapIDList g_screenmapIDList;
//////////////////////////////////////////////////////////////////////////////



int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		g_screenmapIDList.add( 0xFC, 0x00 );
		g_screenmapIDList.add( 0xFC, 0x01 );
		g_screenmapIDList.add( 0xFC, 0x02 );
		g_screenmapIDList.add( 0xFC, 0x03 );

		g_screenmapIDList.add( 0x03, 0x00 );
		g_screenmapIDList.add( 0x03, 0x01 );
		g_screenmapIDList.add( 0x03, 0x02 );
		g_screenmapIDList.add( 0x03, 0x03 );

		g_screenmapIDList.dump( );

		getch( );
	}

	return nRetCode;
}


