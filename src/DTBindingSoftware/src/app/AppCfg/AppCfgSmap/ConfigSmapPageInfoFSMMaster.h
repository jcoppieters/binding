// ConfigSmapPageInfoFSMMaster.h: interface for the CConfigSmapPageInfoFSMMaster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPPAGEINFOFSMMASTER_H__CB08FB08_FB94_48B8_B87B_1FCF9162B475__INCLUDED_)
#define AFX_CONFIGSMAPPAGEINFOFSMMASTER_H__CB08FB08_FB94_48B8_B87B_1FCF9162B475__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"
#include "configsmappageinfoclass.h"
///////////////////////////////////////////////////////////////////
class CConfigSmapPageInfoInfo;
class IComApplication;
///////////////////////////////////////////////////////////////////


/**************************************************************//**
 * @class	CConfigSmapPageInfoFSMMaster
 * @brief	Master FSM voor uploaden van Page info
 *
 * @author	Mitchell Tom
 * @date	20110216
 *****************************************************************/
class CConfigSmapPageInfoFSMMaster : public CConfigExternalFSMMaster 
{
private:

	enum States
	{
		/**
		 * @def		STATE_INIT
		 * @brief	Initialiseren van de node
		 */
		STATE_INIT = 0,

		/**
		 * @def		STATE_IDLE
		 * @brief	Na het versturen van de init
		 */
		STATE_IDLE,

		/**
		 * @def		STATE_HEADER_DONE
		 * @brief	De moodheader werd net doorgestuurd
		 */
		STATE_HEADER_DONE,

		/**
		 * @def		STATE_ENTRY_DONE
		 * @brief	De mood entries werden net doorgestuurd
		 */
		STATE_ENTRY_DONE,

		/**
		 * @def		STATE_DONE
		 * @brief	Gedaan, wachten op reply
		 */
		STATE_DONE,

		/**
		 * @def		STATE_DONE
		 * @brief	Er is een fout gebeurd. 
		 *			De FSM zal wellicht triggeren door een timeout event.
		 */
		STATE_ERROR

	};

	enum Event
	{
		EVENT_START = 0,
		EVENT_CANCEL,
		EVENT_DONE,
		EVENT_ERROR
	};


public:
	CConfigSmapPageInfoFSMMaster(IComApplication& parent);
	~CConfigSmapPageInfoFSMMaster( );

	BOOL Start(CConfigSmapPageInfoInfo& info);
	BOOL Cancel( );

protected:
	BOOL FSM(Event ev);
	BOOL startChildFSMHeader(int headerIndex);
	BOOL startChildFSMEntry( );
	BOOL startFSMInit( );
	BOOL startFSMDone( );

	virtual void onChildFSMDone( );
	virtual void onChildFSMError( );

private:

	/**
	 * De huige header
	 */
	int m_currHeader;

	/**
	 * Het aantal entries
	 */
	int m_maxEntries;

	/**
     * Het totaal aantal headers (sferen).
	 */
	int m_maxHeaders;

	/**
	 * parent class die de configuratie bijhoudt.
	 */
	CConfigSmapPageInfoClass* m_pClasses;

	/**
	 * presentatie van een mood header
	 */
	CConfigPageInfoHeader	m_Header;

	/**
	 * presentatie van een mood entry
	 */
	CConfigPageInfoData	m_Entry;

	States m_currentState;


	typedef CConfigExternalFSMMaster Base;
};

#endif // !defined(AFX_CONFIGSMAPPAGEINFOFSMMASTER_H__CB08FB08_FB94_48B8_B87B_1FCF9162B475__INCLUDED_)
