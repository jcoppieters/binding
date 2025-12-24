#ifndef TPTRLIST_ITERATOR_20081223_H
#define TPTRLIST_ITERATOR_20081223_H
/*******************************************************************/
template <class ITEM>
	class TPtrListIterator
{
protected:
	CPtrList&	m_ParentList;
	POSITION	m_pos;

protected:
	inline void _Begin( )
	{
		m_pos = m_ParentList.GetHeadPosition( );
	}
	inline void _Next( )
	{
		void* const p =  m_ParentList.GetNext( m_pos );
		ASSERT( p );	
	}
	inline bool _IsDone( ) const
	{
		return (m_pos == 0); 
	}

public:
	TPtrListIterator(CPtrList& parentList) :
	  m_ParentList( parentList )
	{
		_Begin( );  
	}
	~TPtrListIterator( ) { }

public:
	void Begin()			{ _Begin(); };
	void Next()				{ _Next(); };
	bool IsDone() const		{ return _IsDone(); }

	ITEM CurrentItem() 
	{ 
		return reinterpret_cast<ITEM>(m_ParentList.GetAt(m_pos)); 
	};
	const ITEM CurrentItem() const 
	{ 
		return reinterpret_cast<ITEM>(m_ParentList.GetAt(m_pos)); 
	};
};

/*******************************************************************/
#endif
/*******************************************************************/