#pragma once
/******************************************************************************/
namespace DUOTECNO {
namespace DALI {
	class CAppDaliFsmErrorEntries;
	class CAppDaliFsmErrorEntry;
};
};
/******************************************************************************/
class CAppDlgDaliWorker;
/******************************************************************************/


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
class CPropertyPageDaliErrorsContext {
public:
	CPropertyPageDaliErrorsContext( );
	virtual ~CPropertyPageDaliErrorsContext( );

	void setFsmErrors(DUOTECNO::DALI::CAppDaliFsmErrorEntries* pDaliFsmErrors);
	DUOTECNO::DALI::CAppDaliFsmErrorEntries* getData( );

protected:
	void deallocate();
	
private:
	DUOTECNO::DALI::CAppDaliFsmErrorEntries* m_pDaliFsmErrors;
};

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
class CPropertyPageDaliErrorsBaseClass : public CPropertyPage
{
public:
	CPropertyPageDaliErrorsBaseClass(unsigned short dialogID,CAppDlgDaliWorker& refWorker,CPropertyPageDaliErrorsContext& refContext);
	virtual ~CPropertyPageDaliErrorsBaseClass();

	CAppDlgDaliWorker& getWorker() {
		return m_refWorker;
	}
	CPropertyPageDaliErrorsContext& getContext() {
		return m_refContext;
	}

protected:
	bool isInitDone( ) const;
	void setInitDone(bool status = true);

private:
	const int m_dialogID;
	CAppDlgDaliWorker& m_refWorker;	
	CPropertyPageDaliErrorsContext& m_refContext;
	bool m_initDone;
};
/******************************************************************************/




