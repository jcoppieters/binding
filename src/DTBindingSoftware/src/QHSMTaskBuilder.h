// QHSMTaskBuilder.h: interface for the CQHSMTaskBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QHSMTASKBUILDER_H__D3C71EEB_9D67_4CB4_B54F_263E1C8D5504__INCLUDED_)
#define AFX_QHSMTASKBUILDER_H__D3C71EEB_9D67_4CB4_B54F_263E1C8D5504__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////
class CQHSMGenericTask;
class CQHSMCompositeTask;
/////////////////////////////////////////////////////////////

class CQHSMTaskBuilder  
{
public:
	CQHSMTaskBuilder();
	virtual ~CQHSMTaskBuilder();

	// generic tasks
	static BOOL Build_BrowseSingleNode(CQHSMGenericTask* pTask);
	static BOOL Build_BrowseAllPhysicalNodes(CQHSMGenericTask* pTask);
	static BOOL Build_RequestForStates(CQHSMGenericTask* pTask);
	static BOOL Build_RequestForStatesSingleNode(CQHSMGenericTask* pTask);
	static BOOL Build_AutomaticPlugAndPlay(CQHSMGenericTask* pTask);
	static BOOL Build_UploadAllNames(CQHSMGenericTask* pTask);
	static BOOL Build_UploadAllBindings(CQHSMGenericTask* pTask);
	static BOOL Build_UploadModifiedBindings(CQHSMGenericTask* pTask);	
	static BOOL Build_UploadScreenmaps(CQHSMGenericTask* pTask);
	static BOOL Build_UploadBitmaps(CQHSMGenericTask* pTask);
	static BOOL Build_UploadMultimediaConfig(CQHSMGenericTask* pTask);
	static BOOL Build_UploadUnitsCreateConfig(CQHSMGenericTask* pTask);
	static BOOL Build_UploadUnitsCreateConfigReplaceNode(CQHSMGenericTask* pTask,int nodeAddress);
	static BOOL Build_UploadMoodConfig(CQHSMGenericTask* pTask);
	static BOOL Build_UploadIRRXConfig(CQHSMGenericTask* pTask);
	static BOOL Build_UploadScheduleConfig(CQHSMGenericTask* pTask);
	static BOOL Build_UploadMemory(CQHSMGenericTask* pTask);
	static BOOL Build_DownloadMemory(CQHSMGenericTask* pTask);
	static BOOL Build_DownloadBindings(CQHSMGenericTask* pTask);
	static BOOL Build_NicErrorsFsm(CQHSMGenericTask* pTask);
	static BOOL Build_ftpConfig(CQHSMGenericTask* pTask);				// since v0562
	static BOOL Build_UploadSmapPageInfo(CQHSMGenericTask* pTask);		// since v0571
	static BOOL Build_UploadSmapMenu(CQHSMGenericTask* pTask);			// since v0571
	static BOOL Build_UploadSmapAVRoom(CQHSMGenericTask* pTask);		// since v0610
	static BOOL Build_UploadGatewayCfg(CQHSMGenericTask* pTask);		// since v0620
	static BOOL Build_UploadRoomCtrlAVCfg(CQHSMGenericTask* pTask);		// since v0625
	static BOOL Build_UploadBoseSTCfg(CQHSMGenericTask* pTask);			// since v0801
	static BOOL Build_UploadLedsCfg(CQHSMGenericTask* pTask);			// since v0804
	static BOOL Build_UploadSmapLite(CQHSMGenericTask* pTask);			// since v0c05

	// composite tasks
	static BOOL Build_UploadProgrammationFile(CQHSMCompositeTask *pTasks);
	static BOOL Build_ExtendedBrowseAllPhNodes(CQHSMCompositeTask *pTasks);
	static BOOL Build_UploadSmapMyHome(CQHSMCompositeTask *pTasks);
	static BOOL Build_UploadSmapMainPage(CQHSMCompositeTask *pTasks);
};

#endif // !defined(AFX_QHSMTASKBUILDER_H__D3C71EEB_9D67_4CB4_B54F_263E1C8D5504__INCLUDED_)