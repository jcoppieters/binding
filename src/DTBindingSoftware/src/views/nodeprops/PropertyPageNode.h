// PropertyPageNode.h: interface for the PropertyPageNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTYPAGENODE_H__D0BA56FB_8170_45F3_BE6F_D1880B66FF6B__INCLUDED_)
#define AFX_PROPERTYPAGENODE_H__D0BA56FB_8170_45F3_BE6F_D1880B66FF6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCanNode;

class CPropertyPageNode : public CPropertyPage
{
public:
	CPropertyPageNode(unsigned short nID) :
	  CPropertyPage(nID), m_dialogID(nID), m_pSelNode(0) {	 };

	CPropertyPageNode(unsigned short nID,CCanNode *pNode) :
	  CPropertyPage(nID), m_dialogID(nID), m_pSelNode(pNode) { };

	virtual ~CPropertyPageNode() { }

#if(0)
	void doConstruct() {
		this->CPropertyPage::Construct(m_dialogID);
	}
#endif 

	virtual void UpdateRunTimeControls() = 0;

protected:
	CCanNode* const m_pSelNode;

private:
	const int m_dialogID;

};

#endif // !defined(AFX_PROPERTYPAGENODE_H__D0BA56FB_8170_45F3_BE6F_D1880B66FF6B__INCLUDED_)
