// ApplicationGraphics.h: interface for the CApplicationGraphics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONGRAPHICS_H__5826ED81_B9F2_11D8_8499_0050BAC412B1__INCLUDED_)
#define AFX_APPLICATIONGRAPHICS_H__5826ED81_B9F2_11D8_8499_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum DaliDeviceStatusIcon {
	ICON_DALIDEVICE_STATUS_NORMAL = 0,
	ICON_DALIDEVICE_STATUS_ERROR = 1,
	ICON_DALIDEVICE_STATUS_UNKNOWN = 2,
};


enum UnitIconType 
{
	ICON_VIRTUAL = 0, 
	ICON_VIRTUAL_ON,
	ICON_VIRTUAL_DALI, 
	ICON_VIRTUAL_DALI_ON,
	ICON_CONTROL, 
	ICON_CONTROL_ON,
	ICON_DALI_CONTROL,
	ICON_DALI_CONTROL_ON,
	ICON_DIMMER, 
	ICON_DIMMER_ON, 
	ICON_DIMMER_PIR, 
	ICON_DIMMER_EC,		
	ICON_DALI_DIMMER,
	ICON_DALI_DIMMER_ON,
	ICON_DALI_DIMMER_PIR,
	ICON_DALI_DIMMER_EC,
	ICON_MOTOR, 
	ICON_MOTOR_DN, 
	ICON_MOTOR_UP,
    ICON_IRRX , 
	ICON_IRTX , 
	ICON_SWITCH , 
	ICON_SWITCH_ON, 
	ICON_SWITCH_PIR,
	ICON_SENS ,
	ICON_AUDIO , 
	ICON_AUDIO_ON,
	ICON_BOSE, 
	ICON_BOSE_ON,
	ICON_AVMATRIX,		
	ICON_VIDEOMUX,		
	ICON_ALARM,				
	ICON_PS2MOUSE,		
	ICON_NODE, 
	ICON_NODE_MASTER, 
	ICON_NODE_LCD,
	ICON_NODE_GATEWAY,
	ICON_UNDEFINED_NODE, 
	ICON_UNDEFINED_UNIT,
	ICON_NOT_IMPLEMENTED,
	ICON_BINDINGS_NC_NOTSEND,
	ICON_BINDINGS_NC_SEND,
	ICON_BINDINGS_PRE_NOTSEND,
	ICON_BINDINGS_PRE_SEND,
	ICON_BINDINGS_POST_NOTSEND,
	ICON_BINDINGS_POST_SEND,
	ICON_BINDINGS_L_NOTSEND,
	ICON_BINDINGS_L_SEND,
	ICON_BINDINGS_K_NOTSEND,
	ICON_BINDINGS_K_SEND,
	ICON_BINDINGS_P_NOTSEND,
	ICON_BINDINGS_P_SEND,
	MAX_ICONS
};


enum BindingOperatorIconType 
{
	ICON_AND=0, 
	ICON_TIMER=2,
	ICON_OR , 
	ICON_XOR , 
	MAX_BINDING_OPERATORS
};

enum ApplicationFontType
{
	FONT_TIMES_NEW_ROMAN_12 = 0	,
	FONT_TIMES_NEW_ROMAN_16		,
	FONT_ARIAL_BOLD_18			,
	FONT_TREEVIEW_FIXED			,
	MAX_FONTS
};



/**
 *
 */
class CApplicationGraphics  
{
public:
	static CApplicationGraphics* GetInstance();

protected :
	CImageList m_lNodeImageList;
	CImageList m_sNodeImageList;
	CImageList m_sBindingImageList;
	CImageList m_TabImageList;
	CImageList m_sDaliDeviceStatusImageList;
	HFONT ApplicationFonts[MAX_FONTS];

protected:
	CApplicationGraphics();

public:
	virtual ~CApplicationGraphics();

protected:
	void AddBitmapToImageList(CImageList *List, UINT nResourceID);
	void AddIconToImageList(CImageList *List, UINT nResourceID);
	
	void CreateImageList(void);
	void LoadAppGlobalIcons(void);
	void CreateAppFonts(void);
	
	void DestroyAppFonts(void);
	void DestroyAppGlobalIcons(void);

public:
	// TM_CHANGES_BUG_0144
	void Init(void);

	CImageList* GetSmallNodeUnitImageList(void) 
	{
		return &m_sNodeImageList; 
	}
	CImageList* GetLargeNodeUnitImageList(void) 
	{
		return &m_lNodeImageList; 
	}
	CImageList* GetSmallBindingsImageList(void) 
	{
		return &m_sBindingImageList; 
	}

	CImageList& GetSmallDaliDeviceStatusImageList(void) {
		return m_sDaliDeviceStatusImageList;
	}

	CImageList* GetTabControlImageList(void)	
	{
		return &m_TabImageList; 
	}
	CFont* GetFont(ApplicationFontType font) 
	{
		return (CFont::FromHandle(ApplicationFonts[font]));
	}
public:	
	HICON UnitIcons[MAX_ICONS];
	HICON OperatorIcons[MAX_BINDING_OPERATORS];	
	HICON BtnReturn;
	HICON gIconModes[4],gIconModesSel[4];
	HICON gIconConnected,gIconDisConnected;
	HICON gIconCommError,gIconCommSend,gIconCommReply,gIconCommACK,gIconCommDisconnected;
};

#endif // !defined(AFX_APPLICATIONGRAPHICS_H__5826ED81_B9F2_11D8_8499_0050BAC412B1__INCLUDED_)
