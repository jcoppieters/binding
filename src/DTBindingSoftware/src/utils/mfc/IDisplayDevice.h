#ifndef INCLUDED_DUOTECNO_MFC_HELPER_IDISPLAYDEVICE_H
#define INCLUDED_DUOTECNO_MFC_HELPER_IDISPLAYDEVICE_H
/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/
struct IDisplayDevice
{
	virtual void displayInfoMsg(const char* szMessage) = 0;
	virtual void displayErrorMsg(const char* szMessage) = 0;
};
/**************************************************************/
};
};
/**************************************************************/
#endif
/**************************************************************/
