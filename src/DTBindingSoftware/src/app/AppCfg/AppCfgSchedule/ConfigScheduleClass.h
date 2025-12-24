/**********************************************************//**
 * @file	configScheduleClass
 * @brief	header bestand voor de CConfigScheduleClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20091001
 *
 * @since	0x0560
 *************************************************************/
#ifndef DUOTECNO_CONFIG_SCHEDULECONFIGCLASS
#define DUOTECNO_CONFIG_SCHEDULECONFIGCLASS
/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"

#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"
/*************************************************************/
class ostream;
/*************************************************************/
namespace DUOTECNO {
namespace DATA {

	/** @brief Serializable klasse */

	#pragma pack(1)	/* Oppassen: #pragma pack() toevoegen aan einde */

	struct CSerScheduleEvent
	{
		BYTE bWeekNr;			// 0 - 3
		BYTE bDayNr;			// 0 - 6
		BYTE bHour;				// 0 - 23
		BYTE bMinute;			// 0 - 59
		BYTE bNodeAddress;		// 0 - 0xFC
		BYTE bUnitAddress;		// 0 - 0xFE
		BYTE bAction;			// zie enumeratie.
	};

	struct CSerScheduleItem
	{
		BYTE bNodeAddress;		// NOV:
		BYTE bUnitAddress;		// NOV:
		BYTE bStateOnOff;		// NOV: Scheduling toestand: On, Off
		BYTE bCurrentState;		// Reserve.
		BYTE bPreviousState;	// Vorige toestand: Sens: Zon, Maan,
	};

	#pragma	pack()


/***************************************************************/
};
};
/***************************************************************/

/***************************************************************/
class CConfigScheduleEventsClass : public CConfigExternalClass
{
protected:
	static const char* _szHeader;
	DUOTECNO::DATA::CSerScheduleEvent* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

private:
	DUOTECNO::MFC_HELPER::TPtrList<DUOTECNO::DATA::CSerScheduleEvent> m_list;

public:
	CConfigScheduleEventsClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigScheduleEventsClass();

	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);

protected:
	void removeAll( );
	void ReportError(const char* szMessage);
	DUOTECNO::DATA::CSerScheduleEvent* GetEntry(int entry);
};
/***************************************************************/

/***************************************************************/
class CConfigScheduleItemsClass : public CConfigExternalClass
{
protected:
	static const char* _szHeader;
	DUOTECNO::DATA::CSerScheduleItem* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

private:
	DUOTECNO::MFC_HELPER::TPtrList<DUOTECNO::DATA::CSerScheduleItem> m_list;

public:
	CConfigScheduleItemsClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigScheduleItemsClass();

	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);

protected:
	void removeAll( );
	void ReportError(const char* szMessage);
	DUOTECNO::DATA::CSerScheduleItem* GetEntry(int entry);
};
/***************************************************************/



#ifdef SKIP
		/***********************************************************//**
		 * @class	CConfigScheduleHeaderClass
		 * @brief	Deze klasse wordt gebruikt door de FSM.
		 * 
		 * @note	deze klasse wordt niet gebruikt om de config file
		 *			in te lezen.
		 *
		 * @author	Mitchell Tom
		 * @date	20091001
		 **************************************************************/
		class CConfigScheduleHeaderClass : public CConfigExternalClass
		{
		protected:
			CSerScheduleHeader m_SerScheduleHeader;

		public:
			CConfigScheduleEventsHeaderClass( );
			~CConfigScheduleEventsHeaderClass( );

			void setNrOfEntries(unsigned short usNrOfEntries);
			void setType(BYTE bType);

		public:
			virtual BOOL ParseHeader(char *szAttribute,char *szData) {
				return FALSE;
			}
			virtual BOOL ParseData(char *szAttribute,char *szData) {
				return FALSE;
			}
			virtual void Initialise(void) {
			}	
		public:
			virtual int GetData(int index,BYTE *pbData);
			virtual BOOL IsUsed(int index);
			virtual int GetMax(void) const;
			virtual void dump( );
			int getStartIndex( ) const;
		};

#endif

/*************************************************************/
#endif
/*************************************************************/
