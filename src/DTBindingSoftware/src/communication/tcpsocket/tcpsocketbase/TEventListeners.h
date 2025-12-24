#ifndef INCLUDED_TCPSOCKET_TEVENTLISTENER_H
#define INCLUDED_TCPSOCKET_TEVENTLISTENER_H
/*****************************************************************/
#include <vector>
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
template <class Type>
class TEventListeners 
{
protected:
	std::vector<Type> m_Listeners;	// Define as volatile ?
	typename std::vector<Type>::iterator m_Listener;	// TM 20070711

public:
	TEventListeners(const Type& T = NULL) {};
	~TEventListeners() {};

public:	
		
	Type Begin() 
	{
		if (m_Listeners.size() == 0) return NULL;	
		m_Listener = m_Listeners.begin();	
		return (*m_Listener);
	}

	Type Next()
	{			
		if (++m_Listener == m_Listeners.end()) return NULL;			
		return (*m_Listener);
	}

	void Initialise()
	{	
		m_Listeners.clear();
	}
	
	void operator +=(const Type& t)		// Subscribe ...
	{	
		m_Listeners.push_back(t);
	}
	
	void operator -=(const Type& t)		// Unsubscribe ...
	{
		for (m_Listener = m_Listeners.begin();
			 m_Listener != m_Listeners.end();
			 m_Listener++)
		{
			if (*m_Listener == t) {
				m_Listeners.erase(m_Listener);
				break;
			}
		}
	}
};
/*****************************************************************/
#endif 
/*****************************************************************/
