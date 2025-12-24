#ifndef INCLUDED_DUOTECNO_MFC_HELPER_ITIMERCONTROL_H
#define INCLUDED_DUOTECNO_MFC_HELPER_ITIMERCONTROL_H
/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/
struct ITimerControl
{
	virtual void startTimer(int ID )					= 0;
	virtual void startTimer(int ID, int reloadValue)	= 0;
	virtual void stopTimer(int ID)						= 0;
};
/**************************************************************/
};
};
/**************************************************************/
#endif
/**************************************************************/
