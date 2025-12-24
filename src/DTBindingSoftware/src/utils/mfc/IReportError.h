#ifndef INCLUDED_DUOTECNO_MFC_HELPER_IREPORTERROR_H
#define INCLUDED_DUOTECNO_MFC_HELPER_IREPORTERROR_H
/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/
struct IReportError
{
	virtual void reportError(const char* szError) = 0;
};
/**************************************************************/
};
};
/**************************************************************/
#endif
/**************************************************************/
