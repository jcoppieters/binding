// RenameNodeUnitsEngine.h: interface for the CRenameNodeUnitsEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENAMENODEUNITSENGINE_H__7ED3ECE1_516E_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_RENAMENODEUNITSENGINE_H__7ED3ECE1_516E_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class IComApplication;
class CNodeDatabase;
class CCanNode;


#define UNIT_NAME_LENGTH	16
#define NODE_NAME_LENGTH	16


class IRenameNodeUnitsEngine  
{
public:
	virtual void Fire_EvAckTimeout(void) = 0;
	virtual void Fire_EvUnitNameAck(void) = 0;
	virtual void Fire_EvUnitNameError(void) = 0;
	virtual void Fire_EvNodeNameAck(void) = 0;
	virtual void Fire_EvNodeNameError(void) = 0;
	virtual void Fire_EvUserCancel(void) = 0;
};


class CRenameNodeUnitsEngine : public IRenameNodeUnitsEngine
{
public:	
	CRenameNodeUnitsEngine(IComApplication *pCon,CNodeDatabase *pDocDatabase);
	virtual ~CRenameNodeUnitsEngine();

protected:

	enum Events 
	{
		EV_RENAME_START=0,
		EV_RENAME_CANCEL,
		EV_RENAME_UNITNAME_ACK,
		EV_RENAME_UNITNAME_ERROR,
		EV_RENAME_NODENAME_ACK,
		EV_RENAME_NODENAME_ERROR,
		EV_RENAME_TIMEOUT
	};

	enum Mode 
	{
		MODE_NONE = 0,
		MODE_SEND_ALLNAMES,
		MODE_SEND_NODENAME,
		MODE_SEND_UNITNAME,	
	};	

	enum States 
	{
		STATE_IDLE = 0, 
		STATE_NODENAME_WRITE, 
		STATE_NODENAME_ERROR,
		STATE_UNITNAME_WRITE, 
		STATE_UNITNAME_ERROR,
		STATE_END
	};

	BYTE m_pbData[UNIT_NAME_LENGTH];	
	IComApplication* const m_pComApplication;
	CNodeDatabase* const m_pDatabase;
	CCanNode *m_pNode;
	CString m_szName;
	Mode m_CurrentMode;
	States m_CurrentState;
	int m_ErrorCnt;
	int m_NrOfNodes;
	int m_NodeIndex;	
	BYTE m_bLength;
	BYTE m_bUnitAddress;
	BYTE m_bNodeAddress;
	BYTE m_bPos;	

		
protected:	
	void FSMRenameNodeUnits(Events bEvent);
	void FSMRenameNode(Events bEvent);
	void FSMRenameUnit(Events bEvent);
	void FSMSendEvents(Events iEvent);	
	void GetConvertedNameData(BYTE bLength,const BYTE *pbData,BYTE *szConvertedData);
	BYTE FormatData(CString szName,BYTE *pbData);
	BYTE WriteNodeName(BYTE bNodeAddress,BYTE *pbData,BYTE bPos,BYTE bLengthToWrite);
	BYTE WriteUnitName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE *pbData,BYTE bPos,BYTE bLengthToWrite);
	void DisplayMessage(BYTE bNodeAddress,BOOL fResult);
	void DisplayMessage(BYTE bNodeAddress,BYTE bUnitAddress,BOOL fResult);	
	
	inline void Fire_OnFsmEnded(void);
	inline BYTE GetLengthToWrite(int length,int pos);

public:

	void SendUnitName(const BYTE bNodeAddress,const BYTE bUnitAddress);
	void SendNodeName(const BYTE bNodeAddress);
	void SendAllNames(void);

	void Fire_EvAckTimeout(void)			{ FSMSendEvents(EV_RENAME_TIMEOUT);}
	void Fire_EvUnitNameAck(void)			{ FSMSendEvents(EV_RENAME_UNITNAME_ACK); }
	void Fire_EvUnitNameError(void)         { FSMSendEvents(EV_RENAME_UNITNAME_ERROR); }
	void Fire_EvNodeNameAck(void)			{ FSMSendEvents(EV_RENAME_NODENAME_ACK); }
	void Fire_EvNodeNameError(void)			{ FSMSendEvents(EV_RENAME_NODENAME_ERROR); }
	void Fire_EvUserCancel(void)			{ FSMSendEvents(EV_RENAME_CANCEL); }
};

#endif // !defined(AFX_RENAMENODEUNITSENGINE_H__7ED3ECE1_516E_11D8_B865_0050BAC412B1__INCLUDED_)
