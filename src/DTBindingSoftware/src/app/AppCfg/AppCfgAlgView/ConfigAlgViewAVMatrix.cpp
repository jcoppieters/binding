#include "stdafx.h"
#include "ConfigAlgViewAVMatrix.h"

#include "app\appCfg\AppCfginfo\cfginfonode.h"						
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"	

#include "cannode.h"
/////////////////////////////////////////////////////////////////////////////

// CHANGES_AV_UNIVERSAL
CConfigAlgViewAVMatrix::CConfigAlgViewAVMatrix(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView)
{
}

CConfigAlgViewAVMatrix::~CConfigAlgViewAVMatrix(void)
{
}

BOOL CConfigAlgViewAVMatrix::Load(void)
{
	return FALSE;
}

BOOL CConfigAlgViewAVMatrix::Upload(void)
{	
	return FALSE;
}

BOOL CConfigAlgViewAVMatrix::UploadToAll(void)
{
	return FALSE;
}

int CConfigAlgViewAVMatrix::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_AVMATRIX;
}

BOOL CConfigAlgViewAVMatrix::IsSupported(void)
{
	return CNodeExtConfigInfo_Util::isSupported(&getCanNode(),EXTERNAL_CONFIG_AVMATRIX);
}

BOOL CConfigAlgViewAVMatrix::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewAVMatrix::IsFileFound(void)
{
	return FALSE;
}

BOOL CConfigAlgViewAVMatrix::IsUploadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewAVMatrix::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewAVMatrix::IsUploadCanAllSupported(void)
{
	return FALSE;
}

const char* CConfigAlgViewAVMatrix::getName(void)
{
	return "AV-Matrix config";
}

BOOL CConfigAlgViewAVMatrix::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewAVMatrix::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewAVMatrix::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}