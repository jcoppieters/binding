#ifndef DUOTECNO_CONFIGMOOD_FSMMASTER_H
#define	DUOTECNO_CONFIGMOOD_FSMMASTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFSMMaster.h"
#include "configMoodClass.h"
///////////////////////////////////////////////////////////////////
class CConfigMoodInfo;
///////////////////////////////////////////////////////////////////
class IComApplication;
///////////////////////////////////////////////////////////////////

/**************************************************************//**
 * @class	CConfigMoodFSMMaster
 * @brief	Master FSM voor uploaden van Moods
 *
 * @author	Mitchell Tom
 * @date	20090429
 *****************************************************************/
class CConfigMoodFSMMaster : public CConfigExternalFSMMaster
{
	typedef CConfigExternalFSMMaster Base;
	
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

	} m_currentState;

	enum Event
	{
		EVENT_START = 0,
		EVENT_CANCEL,
		EVENT_DONE,
		EVENT_ERROR
	};

protected:
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
	CConfigMoodClass* m_pMoodClasses;

	/**
	 * presentatie van een mood header
	 */
	CConfigMoodHeaderClass	m_moodHeader;

	/**
	 * presentatie van een mood entry
	 */
	CConfigMoodEntryClass	m_moodEntry;

protected:
	BOOL FSM(Event ev);
	BOOL startChildFSMHeader(int headerIndex);
	BOOL startChildFSMEntry( );
	BOOL startFSMInit( );
	BOOL startFSMDone( );

public:
	CConfigMoodFSMMaster(IComApplication& parent);
	~CConfigMoodFSMMaster( );

public:
	BOOL Start(CConfigMoodInfo& info);
	BOOL Cancel( );

protected:
	virtual void onChildFSMDone( );
	virtual void onChildFSMError( );
};
///////////////////////////////////////////////////////////////////
#endif