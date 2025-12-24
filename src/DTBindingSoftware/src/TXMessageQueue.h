// TXMessageQueue.h: interface for the CTXMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TXMESSAGEQUEUE_H__31536D56_354B_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_TXMESSAGEQUEUE_H__31536D56_354B_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_NR_TX_MESSAGES	400		// TM20080403 --> was 200

////////////////////////////////////////////////////////////////////

/**
 * TM 20060726 : Added APDU Header for implicit/explicit Messages.
 */
typedef enum 
{
	APDU_HEADER_IMPLICIT = 0,	// 
	APDU_HEADER_EXPLICIT		// Default

}ENUM_APDU_HEADER;


////////////////////////////////////////////////////////////////////

class CTXMessageQueue  
{
private:
	int nCount;
	CString szMessageQueue[MAX_NR_TX_MESSAGES];
	ENUM_APDU_HEADER APDUHeader[MAX_NR_TX_MESSAGES];

	int nMessageReadPtr;
	int nMessageWritePtr;	

public:
	void Flush(void);

	BOOL InsertMessage(const CString& szString,
					   ENUM_APDU_HEADER Header = APDU_HEADER_EXPLICIT);
	CString GetMessage(ENUM_APDU_HEADER *Header);
	
	int Count();
	CTXMessageQueue();
	
	virtual ~CTXMessageQueue();
};

#endif // !defined(AFX_TXMESSAGEQUEUE_H__31536D56_354B_11D8_B865_0050BAC412B1__INCLUDED_)
