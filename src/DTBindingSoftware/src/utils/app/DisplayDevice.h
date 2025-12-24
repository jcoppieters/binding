// DisplayDevice.h: interface for the CDisplayDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPLAYDEVICE_H__6AD82041_9F3D_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DISPLAYDEVICE_H__6AD82041_9F3D_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDisplayDevice  
{
public:
	static void DisplayInfoMsg(const CString& szMessage);
	static void DisplayErrorMsg(const CString& szMessage);
	static void Clear(void);
	static void DisplayTracerErrorMsg(const CString& szMessage);

protected:
	CDisplayDevice();
	virtual ~CDisplayDevice();
};

#endif // !defined(AFX_DISPLAYDEVICE_H__6AD82041_9F3D_11D8_B865_0050BAC412B1__INCLUDED_)
