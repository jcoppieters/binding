// ApplicationGraphics.cpp: implementation of the CApplicationGraphics class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ApplicationGraphics.h"
//#include <versionhelpers.h>				// Opvragen van OS versie info - Niet aanwezig in VS2008

#if(0)
#include "atlimage.h"		// CImage class
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
#define USE_IMAGELIST_SMALL_BITMAPS				1
//////////////////////////////////////////////////////////////////////
// Opvragen of de O.S. hoger is dan Windows 10.
// Indien >= Windows 10 dan de bitmaps gebruiken ipv de iconen.
// Niet ok: Kan niet gebruikt worden...
static BOOL myVersionIsWindows10OrGreater() {
#if(0)
	// Probleem: Niet aanwezig in MSVC2015.
	return IsWindows10OrGreater();		// Niet aanwezig.
#endif 

	// Probleem: Antwoord altijd met 6.2 (=windows 8.1)
	OSVERSIONINFOA osVersionInfo;
	memset(&osVersionInfo, 0, sizeof(osVersionInfo));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	if (!GetVersionEx(&osVersionInfo)) {
		return FALSE;
	}

	if (osVersionInfo.dwMajorVersion >= 10) {
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
CApplicationGraphics* CApplicationGraphics::GetInstance()
{
	static CApplicationGraphics instance;
	return (&instance);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CApplicationGraphics::CApplicationGraphics()
{
	for (int i=0;i<MAX_FONTS;i++) {
		ApplicationFonts[i] = NULL;
	}
}

CApplicationGraphics::~CApplicationGraphics()
{
	DestroyAppFonts();
	DestroyAppGlobalIcons();
}

// TM_CHANGES_BUG_0144
void CApplicationGraphics::Init(void)
{
	// testLoadPngLogo();
	LoadAppGlobalIcons();
	CreateImageList();
	CreateAppFonts();
}
//////////////////////////////////////////////////////////////////////

void CApplicationGraphics::AddBitmapToImageList(CImageList *List, UINT nResourceID)
{
	CBitmap bmp;

	const BOOL bReturn = bmp.LoadBitmap( nResourceID );	// CHANGES_MVS_2008_WARNINGS
	if (bReturn != FALSE) 
	{
		List->Add( &bmp, RGB(0,0,0) );
		bmp.DeleteObject();
	}
	else 
	{
		AfxMessageBox("ERROR - Create imagelist",MB_ICONSTOP);
	}
}

void CApplicationGraphics::AddIconToImageList(CImageList *List, UINT nResourceID)
{
	HICON hIcon;
	hIcon =::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE( nResourceID ));
	ASSERT(NULL != hIcon);
	List->Add(hIcon);
}

// ---------------------------------------------------------------------------------

void CApplicationGraphics::DestroyAppFonts(void)
{
	for (int i=0;i<MAX_FONTS;i++)
	{
		if (ApplicationFonts[i] != NULL)
		{
			::DeleteObject(ApplicationFonts[i]);
			ApplicationFonts[i] = NULL;
		}
	}
}

void CApplicationGraphics::CreateAppFonts(void)
{
	if (1)
	{
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(LOGFONT));          

		logFont.lfHeight = 12;		
		logFont.lfWeight = FW_NORMAL;	
		logFont.lfCharSet = ANSI_CHARSET;
		logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		logFont.lfQuality = PROOF_QUALITY;
		logFont.lfPitchAndFamily = FF_ROMAN;
		strcpy(logFont.lfFaceName,"Times New Roman");
	
		if ((ApplicationFonts[FONT_TIMES_NEW_ROMAN_12] = ::CreateFontIndirect(&logFont)) == NULL)
		{
			CString szError("Error : CreateFontIndirect failed");
			AfxMessageBox(szError);
		}	
	}

	if (1)
	{	
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(LOGFONT));   

		logFont.lfHeight = 16;	
		logFont.lfWeight = FW_NORMAL;	
		logFont.lfCharSet = ANSI_CHARSET;
		logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		logFont.lfQuality = PROOF_QUALITY;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
		strcpy(logFont.lfFaceName,"Times New Roman");

		if ((ApplicationFonts[FONT_TIMES_NEW_ROMAN_16] = ::CreateFontIndirect(&logFont)) == NULL)
		{
			CString szError("Error : CreateFontIndirect failed");
			AfxMessageBox(szError);
		}		
	}

	if (1)
	{	
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(LOGFONT));   

		logFont.lfHeight = 18;	
		logFont.lfWeight = FW_BOLD;		
		strcpy(logFont.lfFaceName,"Arial");

		if ((ApplicationFonts[FONT_ARIAL_BOLD_18] = ::CreateFontIndirect(&logFont)) == NULL)
		{
			CString szError("Error : CreateFontIndirect failed");
			AfxMessageBox(szError);
		}		
	}	

	if (1)
	{	
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(LOGFONT));   

		logFont.lfHeight = 10;
		logFont.lfWeight = FW_NORMAL;
		logFont.lfQuality = DRAFT_QUALITY;
		logFont.lfCharSet = ANSI_CHARSET;

		strcpy(logFont.lfFaceName,"MS Sans Serif");

		if ((ApplicationFonts[FONT_TREEVIEW_FIXED] = ::CreateFontIndirect(&logFont)) == NULL)
		{
			CString szError("Error : CreateFontIndirect failed");
			AfxMessageBox(szError);
		}		
	}
}

// --------------------------------------------------------------------------------------
void CApplicationGraphics::DestroyAppGlobalIcons(void)
{
	if (gIconConnected != NULL)
	{
		::DeleteObject(gIconConnected);
		gIconConnected = NULL;
	}

	if (gIconDisConnected != NULL)
	{
		::DeleteObject(gIconDisConnected);
		gIconDisConnected = NULL;
	}

	if (gIconCommError != NULL)
	{
		::DeleteObject(gIconCommError);
		gIconCommError = NULL;
	}

	if (gIconCommSend != NULL)
	{
		::DeleteObject(gIconCommSend);
		gIconCommSend = NULL;
	}

	if (gIconCommReply != NULL)
	{
		::DeleteObject(gIconCommReply);
		gIconCommReply = NULL;
	}

	if (gIconCommACK != NULL)
	{
		::DeleteObject(gIconCommACK);
		gIconCommACK = NULL;
	}

	if (gIconCommDisconnected != NULL)
	{
		::DeleteObject(gIconCommDisconnected);
		gIconCommDisconnected = NULL;
	}

	if (BtnReturn != NULL)
	{
		::DeleteObject(BtnReturn);
		BtnReturn = NULL;
	}

	int i;
	
	for (i=0;i<MAX_ICONS;i++)
	{
		if (UnitIcons[i] != NULL)
		{
			::DeleteObject(UnitIcons[i]);
			UnitIcons[i] = NULL;
		}
	}

	for (i=0;i<MAX_BINDING_OPERATORS;i++)
	{
		if (OperatorIcons[i] != NULL)
		{
			::DeleteObject(OperatorIcons[i]);
			OperatorIcons[i] = NULL;
		}
	}

	for (i=0;i<4;i++)
	{
		if (gIconModes[i] != NULL)
		{
			::DeleteObject(gIconModes[i]);
			gIconModes[i] = NULL;
		}

		if (gIconModesSel[i] != NULL)
		{
			::DeleteObject(gIconModesSel[i]);
			gIconModesSel[i] = NULL;
		}
	}
}

void CApplicationGraphics::LoadAppGlobalIcons(void)
{	
	int i;

	for (i=0;i<MAX_ICONS;i++) {	
		UnitIcons[i] = NULL;
	}

	for (i=0;i<MAX_BINDING_OPERATORS;i++) {	
		OperatorIcons[i] = NULL;	
	}

	for (i=0;i<4;i++) {	
		gIconModes[i] = NULL;
		gIconModesSel[i] = NULL;
	}

	HINSTANCE hAppHandle = AfxGetResourceHandle( );
	
	gIconConnected =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_CONNECTED ));
	gIconDisConnected =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_DISCONNECTED ));
	gIconCommError= ::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_COMM_ERROR ));
	gIconCommSend= ::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_COMM_SEND ));
	gIconCommReply= ::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_COMM_REPLY ));
	gIconCommACK= ::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_COMM_ACK ));
	gIconCommDisconnected = ::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_COMM_DISCONNECTED ));

	UnitIcons[ICON_VIRTUAL] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_VIRTUAL));
	UnitIcons[ICON_VIRTUAL_ON] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_VIRTUAL_ON ));
	UnitIcons[ICON_VIRTUAL_DALI] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_VIRTUAL_DALI));
	UnitIcons[ICON_VIRTUAL_DALI_ON] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_VIRTUAL_DALI_ON ));
	UnitIcons[ICON_CONTROL] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_CONTROL));
	UnitIcons[ICON_CONTROL_ON] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_CONTROL_ON));
	UnitIcons[ICON_DALI_CONTROL] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_CONTROL_DALI));
	UnitIcons[ICON_DALI_CONTROL_ON] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_CONTROL_DALI_ON));
	UnitIcons[ICON_DIMMER] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_DIM));
	UnitIcons[ICON_DIMMER_ON] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_DIM_ON));
	UnitIcons[ICON_DIMMER_PIR] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_DIM_PIR));
	UnitIcons[ICON_DIMMER_EC] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_DIM_EC));	
	UnitIcons[ICON_DALI_DIMMER] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_DIM_DALI_OFF));			
	UnitIcons[ICON_DALI_DIMMER_ON] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_DIM_DALI_ON));		
	UnitIcons[ICON_DALI_DIMMER_PIR] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_DIM_DALI_PIRTIMED));		
	UnitIcons[ICON_DALI_DIMMER_EC] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_DIM_DALI_EC));		
	UnitIcons[ICON_MOTOR] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MOTOR));
	UnitIcons[ICON_MOTOR_DN] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MOTOR_DN ));
	UnitIcons[ICON_MOTOR_UP] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MOTOR_UP));
	UnitIcons[ICON_IRRX] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_IRRX));
	UnitIcons[ICON_IRTX] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_IRTX));
	UnitIcons[ICON_SWITCH] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_SWITCH));
	UnitIcons[ICON_SWITCH_ON] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_SWITCH_ON));
	UnitIcons[ICON_SWITCH_PIR] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_SWITCH_PIR));
	UnitIcons[ICON_SENS] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_SENS));
	UnitIcons[ICON_AUDIO] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_AUDIO));
	UnitIcons[ICON_AUDIO_ON] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_AUDIO_ON));
	UnitIcons[ICON_BOSE] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_BOSE));
	UnitIcons[ICON_BOSE_ON] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_BOSE_ON));
	UnitIcons[ICON_AVMATRIX] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_AVMATRIX));
	UnitIcons[ICON_VIDEOMUX] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_VIDEOMUX));
	UnitIcons[ICON_ALARM] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_ALARM));
	UnitIcons[ICON_PS2MOUSE] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_PS2MOUSE));

	UnitIcons[ICON_NODE] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_NODE));
	UnitIcons[ICON_NODE_MASTER] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_NODE_MASTER));
	UnitIcons[ICON_NODE_LCD]=::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_NODE_LCD));
	UnitIcons[ICON_NODE_GATEWAY]=::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_NODE_GATEWAY));
	UnitIcons[ICON_UNDEFINED_NODE] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_NODE_UNDEFINED));
	UnitIcons[ICON_UNDEFINED_UNIT] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_UNIT_UNDEFINED));
	UnitIcons[ICON_NOT_IMPLEMENTED] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_NOT_IMPLEMENTED));

	OperatorIcons[ICON_AND] = ::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_OPERATOR_AND));
	OperatorIcons[ICON_OR] = ::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_OPERATOR_OR));
	OperatorIcons[ICON_XOR] = ::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_OPERATOR_XOR));
	OperatorIcons[ICON_TIMER] = ::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_OPERATOR_TIMER));

	BtnReturn =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_BTN_RETURN));

	gIconModes[0] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MODES_SUN));
	gIconModes[1] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MODES_HSUN));
	gIconModes[2] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MODES_MOON));
	gIconModes[3] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MODES_HMOON));

	gIconModesSel[0] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MODES_SUN_SEL));
	gIconModesSel[1] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MODES_HSUN_SEL));
	gIconModesSel[2] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MODES_MOON_SEL));
	gIconModesSel[3] =::LoadIcon(hAppHandle,MAKEINTRESOURCE( IDI_ICON_MODES_HMOON_SEL));
}




// Aanpassing aan de imagelist.
void CApplicationGraphics::CreateImageList()
{
	////////////////////////////////////////////////////////////////////////
	/* LARGE IMAGELIST */
#if (USE_CHANGES_IMAGELISTS == 1)
	m_lNodeImageList.Create(32,32,ILC_COLOR4,40,0);						// 40 items
#else
	m_lNodeImageList.Create(32,32,TRUE,4,0);
#endif	
	m_lNodeImageList.SetBkColor(RGB(255,255,255));

	AddIconToImageList(&m_lNodeImageList,IDI_ICON_VIRTUAL);				// 0
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_VIRTUAL_ON);			// 1
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_VIRTUAL_DALI);		// 2
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_VIRTUAL_DALI_ON);		// 3
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_CONTROL);				// 4
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_CONTROL_ON);			// 5
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_CONTROL_DALI);		// 6
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_CONTROL_DALI_ON);		// 7
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_DIM);					// 8
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_DIM_ON);				// 9
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_DIM_PIR);				// 10
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_DIM_EC);				// 11
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_DIM_DALI_OFF);		// 12
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_DIM_DALI_ON);			// 13
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_DIM_DALI_PIRTIMED);	// 14
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_DIM_DALI_EC);			// 15
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_MOTOR);				// 16
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_MOTOR_DN);			// 17
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_MOTOR_UP);			// 18
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_IRRX);				// 19
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_IRTX);				// 20
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_SWITCH);				// 21
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_SWITCH_ON);			// 22
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_SWITCH_PIR);			// 23
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_SENS);				// 24
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_AUDIO);				// 25
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_AUDIO_ON);			// 26
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_BOSE);				// 27
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_BOSE_ON);				// 28
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_AVMATRIX);			// 29
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_VIDEOMUX);			// 30
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_ALARM);				// 31
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_PS2MOUSE);			// 32
// ...
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_NODE);				// 33
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_NODE_MASTER);			// 34
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_NODE_LCD);			// 35
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_NODE_GATEWAY);		// 36
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_NODE_UNDEFINED);		// 37
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_UNIT_UNDEFINED);		// 38
	AddIconToImageList(&m_lNodeImageList,IDI_ICON_NOT_IMPLEMENTED);		// 39

////////////////////////////////////////////////////////////////////////
	/* SMALL IMAGELIST */
#if (USE_CHANGES_IMAGELISTS == 1)
	m_sNodeImageList.Create(16,16,ILC_COLOR4,52,0);							// 52 items.
#else
	m_sNodeImageList.Create(16,16,TRUE,4,0);
#endif
	m_sNodeImageList.SetBkColor(RGB(255,255,255));

	AddIconToImageList(&m_sNodeImageList,IDI_ICON_VIRTUAL);					// 0
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_VIRTUAL_ON);				// 1
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_VIRTUAL_DALI);			// 2
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_VIRTUAL_DALI_ON);			// 3
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_CONTROL);					// 4
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_CONTROL_ON);				// 5
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_CONTROL_DALI);			// 6
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_CONTROL_DALI_ON);			// 7
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_DIM);						// 8
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_DIM_ON);					// 9
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_DIM_PIR);					// 10
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_DIM_EC);					// 11
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_DIM_DALI_OFF);			// 12
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_DIM_DALI_ON);				// 13
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_DIM_DALI_PIRTIMED);		// 14
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_DIM_DALI_EC);				// 15
	
#if(USE_IMAGELIST_SMALL_BITMAPS == 1)
	AddBitmapToImageList(&m_sNodeImageList, IDB_BITMAP_SMALL_DUOSWITCH);			// 16
	AddBitmapToImageList(&m_sNodeImageList, IDB_BITMAP_SMALL_DUOSWITCH_DOWN);		// 17
	AddBitmapToImageList(&m_sNodeImageList, IDB_BITMAP_SMALL_DUOSWITCH_UP);			// 18	
#else
	AddIconToImageList(&m_sNodeImageList, IDI_ICON_MOTOR);			// 16
	AddIconToImageList(&m_sNodeImageList, IDI_ICON_MOTOR_DN);		// 17
	AddIconToImageList(&m_sNodeImageList, IDI_ICON_MOTOR_UP);		// 18
#endif 
	
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_IRRX);					// 19
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_IRTX);					// 20

#if(USE_IMAGELIST_SMALL_BITMAPS == 1)
	AddBitmapToImageList(&m_sNodeImageList, IDB_BITMAP_SMALL_RELAIS);			// 21
	AddBitmapToImageList(&m_sNodeImageList, IDB_BITMAP_SMALL_RELAIS_ON);		// 22
	AddBitmapToImageList(&m_sNodeImageList, IDB_BITMAP_SMALL_RELAIS_PIR);		// 23	
#else
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_SWITCH);						// 21
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_SWITCH_ON);					// 22
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_SWITCH_PIR);					// 23
#endif

	AddIconToImageList(&m_sNodeImageList,IDI_ICON_SENS);						// 24
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_AUDIO);						// 25
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_AUDIO_ON);					// 26
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BOSE);						// 27
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BOSE_ON);						// 28
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_AVMATRIX);					// 29
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_VIDEOMUX);					// 30
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_ALARM);						// 31
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_PS2MOUSE);					// 32
// ..
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_NODE);						// 33
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_NODE_MASTER);					// 34
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_NODE_LCD);					// 35
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_NODE_GATEWAY);				// 36
	AddBitmapToImageList(&m_sNodeImageList,IDB_BITMAP7);						// 37
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_UNIT_UNDEFINED);				// 38
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_NOT_IMPLEMENTED);				// 39
// ...
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_NC_NOTSEND);			// 40
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_NC_SEND);			// 41
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_PRE_NOTSEND);		// 42
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_PRE_SEND);			// 43
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_POST_NOTSEND);		// 44
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_POST_SEND);			// 45
// ...
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_L_NOTSEND);			// 46
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_L_SEND);				// 47
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_K_NOTSEND);			// 48
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_K_SEND);				// 49
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_P_NOTSEND);			// 50
	AddIconToImageList(&m_sNodeImageList,IDI_ICON_BINDINGS_P_SEND);				// 51
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
	// Binding Small Image List
#if (USE_CHANGES_IMAGELISTS == 1)
	m_sBindingImageList.Create(16,16,ILC_COLOR4,2,0);						// 2 items
#else
	m_sBindingImageList.Create(16,16,TRUE,4,0);
#endif
	m_sBindingImageList.SetBkColor(RGB(255,255,255));
	AddIconToImageList(&m_sBindingImageList,IDI_ICON_NOT_SEND);				// 0
	AddIconToImageList(&m_sBindingImageList,IDI_ICON_SEND);					// 1


/////////////////////////////////////////////////////////////////////////////////////////
	// DALI Device status Small Image List
#if (USE_CHANGES_IMAGELISTS == 1)
	m_sDaliDeviceStatusImageList.Create(16,16,ILC_COLOR4,3,0);				// 3 items
#else
	m_sDaliDeviceStatusImageList.Create(16,16,TRUE,4,0);
#endif
	m_sDaliDeviceStatusImageList.SetBkColor(RGB(255,255,255));
	AddIconToImageList(&m_sDaliDeviceStatusImageList,IDI_ICON_DALIDEVICE_STATE_NORMAL);		// 0 ICON_DALIDEVICE_STATUS_NORMAL
	AddIconToImageList(&m_sDaliDeviceStatusImageList,IDI_ICON_DALIDEVICE_STATE_ERROR);		// 1 ICON_DALIDEVICE_STATUS_ERROR
	AddIconToImageList(&m_sDaliDeviceStatusImageList,IDI_ICON_DALIDEVICE_STATE_UNKNOWN);	// 2 ICON_DALIDEVICE_STATUS_UNKNOWN

	/////////////////////////////////////////////////////////////////////////////////////
	// TabControls ...
#if (USE_CHANGES_IMAGELISTS == 1)
	// Moeten 'ILC_MASK' gebruiken want anders worden deze niet transparent getekend...
	m_TabImageList.Create(16,16,ILC_MASK,3,0);							// 3 items
	m_TabImageList.SetBkColor(CLR_NONE);
#else
	m_TabImageList.Create(16,16,TRUE,0,3);
#endif	
	AddIconToImageList(&m_TabImageList,IDI_ICON_TAB_NODES);				// 0
	AddIconToImageList(&m_TabImageList,IDI_ICON_TAB_BINDINGS);			// 1
	AddIconToImageList(&m_TabImageList,IDI_ICON_TAB_TEMPLATES);			// 2
}
