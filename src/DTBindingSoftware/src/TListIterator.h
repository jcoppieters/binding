#ifndef TLIST_ITERATOR_20110209_H
#define TLIST_ITERATOR_20110209_H
/******************************************************************/
#include "afxtempl.h"
/*******************************************************************/
template <class ITEM, class REF_ITEM>
	class TListIterator
{
public:	
	TListIterator(CList<ITEM,REF_ITEM>& parentList) :
	  m_ParentList( parentList )
	{
		_Begin( );  
	}
	~TListIterator( ) 
	{ 
	
	}

	void Begin()			{ _Begin(); };
	void Next()				{ _Next(); };
	bool IsDone() const		{ return _IsDone(); }
	bool IsEmpty() const { return (m_ParentList.GetCount( ) <=0 ? true : false); }

	REF_ITEM CurrentItem() 
	{ 
		ASSERT( m_pos != 0);
		ASSERT( m_currentItem < m_ParentList.GetCount( ));

		return m_ParentList.GetAt(m_pos); 
	};
	const REF_ITEM CurrentItem() const 
	{ 
		ASSERT( m_pos != 0 );
		ASSERT( m_currentItem < m_ParentList.GetCount( ));

		return m_ParentList.GetAt(m_pos); 
	};

protected:
	inline void _Begin( )
	{
		m_currentItem = 0;
		m_pos = m_ParentList.GetHeadPosition( );
	}
	inline void _Next( )
	{	
		m_ParentList.GetNext( m_pos );
		m_currentItem++;
	}
	inline bool _IsDone( ) const
	{
		return (!(m_currentItem < m_ParentList.GetCount( ))); 
	}

private:
	CList<ITEM,REF_ITEM>& m_ParentList;
	POSITION m_pos;
	int m_currentItem;
};

/*******************************************************************/
#endif
/*******************************************************************/