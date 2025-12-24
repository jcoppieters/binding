#pragma once

#include "utils/mfc/ColorPreviewStatic.h"

// CDlgDaliControlGearChildDT8
namespace DUOTECNO {
	namespace DALI {		
		class CAppDaliObjectDT8ColorData;
		class CAppDaliDeviceProperties_ControlGear;
	};
};

class CDlgDaliControlGearChildDT8 : public CDialog
{
public:
	CDlgDaliControlGearChildDT8(DWORD identifier,CWnd* pParent,DUOTECNO::DALI::CAppDaliObjectDT8ColorData* pSceneDT8ColorData);   
	virtual ~CDlgDaliControlGearChildDT8();

	typedef enum {
		VIEWMODE_OFFLINE = 0,
		VIEWMODE_ONLINE,
		VIEWMODE_ONLINE_BUSY
	} ViewMode_t;				// OPPASSEN: IDENTIEK ALS IN CDlgDaliControlGearDT8Scene

	// saveToProperties 
	//	TRUE: Dialog data copieren naar de properties,
	//  FALSE: Properties copieren naar dialog data.
	virtual void updateColorData(BOOL saveToProperties) = 0;

	// Dialoog data instellen zodat alle waarden een MASK value hebben.
	virtual void clearData(BOOL saveToProperties = FALSE) = 0;

	// Dialog updaten
	void updateDialog( );

	// View mode updaten
	void setViewMode(ViewMode_t mode);

	// De Properties updaten mbv de scene color info...
	BOOL updateProperties(int sceneNumber, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* properties);

	// Property changed?
	// Opvragen of er properties veranderd werden die nog niet doorgestuurd werden.
	// Bij het verlaten van de dialoog wordt er gevraagd of de data moet doorgestuurd worden of verloren gaan.
	BOOL getPropertyChanged() const;
	void setFlagPropertyChanged();
	void clearFlagPropertyChanged();
	
protected:
	enum { WM_UPDATE_CONTROLS = WM_USER + 150 };

protected:
	static const CString STRING_MASK_VALUE;

	// Opvragen van de Color Data...
	const DUOTECNO::DALI::CAppDaliObjectDT8ColorData& getData() const;
	DUOTECNO::DALI::CAppDaliObjectDT8ColorData& getData();

	ViewMode_t getViewMode() const {
		return this->m_viewMode;
	}

private:
	ViewMode_t m_viewMode;
	DUOTECNO::DALI::CAppDaliObjectDT8ColorData* const m_pSceneDT8ColorData;
	BOOL m_flagPropertyChanged;
};



