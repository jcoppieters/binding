// BindingEvaluationString.h: interface for the CBindingEvaluationString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRING_H__A25D278B_9592_45F9_8AEE_DD6A8FE3D9AB__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRING_H__A25D278B_9592_45F9_8AEE_DD6A8FE3D9AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingEvaluationString  
{
protected:
	CString m_szEvaluationString;
	int m_TimerCnt;
	int m_InputCnt;
	int m_OutputCnt;
	BOOL m_fInverse;

public:
	CBindingEvaluationString(CString s = CString(""));
	virtual ~CBindingEvaluationString();

	CString GetInput();
	CString GetOutput();

	BOOL Parse();
	void Clear();

	CString Get() const {
		return m_szEvaluationString;
	}
	void Set(CString s) {
		m_szEvaluationString = s;
	}

	int GetNrOfTimers() const {
		return m_TimerCnt;
	}
	int GetNrOfInputs() const {
		return m_InputCnt;
	}
	int GetNrOfOutputs() const {
		return m_OutputCnt;
	}
	BOOL Is_Inverse() const {
		return m_fInverse;
	}

	BOOL ParseUnit(int IndexCh,unsigned short* nID);
	BOOL ParseUnit(int IndexCh,unsigned short* nID,char* chOperator);
	BOOL ParseTimer(int IndexCh,unsigned short* nID);

	BOOL FormatUnit(CString& szSubString,unsigned short nID,char chOperator);
	BOOL FormatUnit(CString& szSubString,unsigned short nID);
	BOOL FormatTimer(CString& szSubString,unsigned short nID);

	/**
	 * Used by Parser ...
	 */
	BOOL AddUnit(unsigned short nID);
	BOOL AddTimer(unsigned short nID);
	BOOL Add(const char c);

	/**
	 *
	 */
	BOOL FindUnit(unsigned short nID);
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRING_H__A25D278B_9592_45F9_8AEE_DD6A8FE3D9AB__INCLUDED_)
