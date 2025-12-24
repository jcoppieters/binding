#ifndef INCLUDED_DUOTECNO_MFC_HELPER_TPTRLIST_H
#define INCLUDED_DUOTECNO_MFC_HELPER_TPTRLIST_H
/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/
template <class ITEM>
	class TPtrList
{
private:
	CPtrList m_list;

public:
	TPtrList( ) : m_list( ) { };
	~TPtrList( ) { };

public:
	ITEM* removeHead( ) 
	{
		return reinterpret_cast<ITEM*>(m_list.RemoveHead( ));
	}

	int getCount( ) const 
	{
		return m_list.GetCount( );	
	}

	ITEM* getEntry(int index)
	{
		ITEM* pReturn = 0;
		const POSITION pos = m_list.FindIndex( index );
		if (pos != NULL)
		{
			pReturn = 
				reinterpret_cast<ITEM*>(m_list.GetAt(pos));
		}
		return pReturn;
	}
	BOOL isEmpty( ) const
	{
		return m_list.IsEmpty();
	}
	void addTail(ITEM* p)
	{
		m_list.AddTail( p );
	}
};
/**************************************************************/
};
};
/**************************************************************/
#endif
/**************************************************************/
