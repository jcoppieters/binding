// ConfigMultiMediaInfo.h: interface for the CConfigMultiMediaInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGMULTIMEDIAINFO_H__4451F528_9761_4ACB_B731_28C4C251EC21__INCLUDED_)
#define AFX_CONFIGMULTIMEDIAINFO_H__4451F528_9761_4ACB_B731_28C4C251EC21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////
class CConfigMultiMediaFile;
struct CConfigExternalClass;
class CNodeDatabase;
class CCanNode;
/////////////////////////////////////////////////////
#include "utils/mfc/ArrayErrors.h"


/**************************************************************************************//**
 * @class	CConfigMultiMediaInfo
 *			Klasse bevat info om de FSM uit te voeren.\n
 *			De info wordt aangemaakt door de UI en mbv de node database.
 *
 * @changes	TM20091218: inline functies.\n
 *			TM20091218: Nodes bijhouden in struct STRUCT_NODEENTRY.
 *						Op deze manier kunnen we onderscheid maken tss de nodetypes.
 *			TM20091218: Opkuisen functie init
 *			TM20110719: CConfigMultiMediaFile alloceren op de heap
 *			TM20110719: Facade voor CConfigMultiMediaFile klasse
 *****************************************************************************************/
class CConfigMultiMediaInfo  
{
public:
	static CConfigMultiMediaInfo* GetInstance();

	virtual ~CConfigMultiMediaInfo();

	CString GetFullPathName( );
	BYTE GetNrNodes( ) const; 
	BYTE GetNodeAddress(int index) const;	
	int GetNodeInfoType(int index) const;
	BOOL Load(CCanNode* pNode);
	BOOL IsSendToAllNodes() const;
	BOOL IsFileLoaded() const;
	BOOL InitNodeEntries(BOOL fSendToAll,CNodeDatabase *pDatabase,BYTE bNodeAddress);
	void SendToAllNodes(BOOL flag = TRUE);

// Facade voor CConfigMultiMediaFile.
	CConfigExternalClass* GetClass(int type);	
	DUOTECNO::MFC_HELPER::CArrayErrors* getErrors( );
	BOOL IsValidMultiMediaFile( ) const;

	// Zie BUG_SOLVE_0158
	BOOL isReady() const;
	void setReady(BOOL flag);

protected:	
	CConfigMultiMediaInfo();
	void resetInfo( );

private:
	void allocate(CCanNode* pNode);
	void deallocate( );

	enum { MAX_COMPATIBLE_NODES	= 254 };

	struct STRUCT_NODE_ENTRY
	{
		BYTE bLogicalNodeAddress;		// Logisch node address van de node.
		int type;						// Type V64, V65, ...
	};

	BOOL m_fFileLoaded;
	BOOL m_fSendToAllNodes;		
	BOOL m_fReady;	// BUG_SOLVE_0158
	BYTE m_bCount;
	CConfigMultiMediaFile* m_pMultiMediaFile;
	STRUCT_NODE_ENTRY m_nodeEntries[ MAX_COMPATIBLE_NODES ];

};

/************************************* inline functies ***************************************/

inline BYTE CConfigMultiMediaInfo::GetNrNodes( ) const
{
	return m_bCount;
}

inline BYTE CConfigMultiMediaInfo::GetNodeAddress(int index) const
{
	ASSERT( index < m_bCount && index >= 0);

	return (index < m_bCount ?
			m_nodeEntries[index].bLogicalNodeAddress :
			0xFF);
}

inline int CConfigMultiMediaInfo::GetNodeInfoType(int index) const
{
	ASSERT( index < m_bCount && index >= 0);

	return (index < m_bCount ? m_nodeEntries[index].type : -1);
}

inline BOOL CConfigMultiMediaInfo::IsSendToAllNodes( ) const
{
	return m_fSendToAllNodes;
}

inline BOOL CConfigMultiMediaInfo::IsFileLoaded( ) const
{
	return m_fFileLoaded; 
};

inline void CConfigMultiMediaInfo::SendToAllNodes(BOOL flag) 
{
	m_fSendToAllNodes = flag;
}

/*******************************************************************************************/

#endif // !defined(AFX_CONFIGMULTIMEDIAINFO_H__4451F528_9761_4ACB_B731_28C4C251EC21__INCLUDED_)
