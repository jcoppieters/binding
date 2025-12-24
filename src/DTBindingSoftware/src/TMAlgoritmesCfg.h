#ifndef TMALGORITMESCFG_H
#define TMALGORITMESCFG_H

/*******************************************************/
class CCanNode;
class CNodeDatabase;		// Sedert v11.01
/*******************************************************/

namespace TMAlgoritmesCfg
{
	struct INotifier
	{
		virtual void AddMessage(const char* s) = 0;
	};

	class CNotifierDefault : public INotifier
	{
		virtual void AddMessage(const char* s)
		{
			// absorbe...
		}
	};

	extern CNotifierDefault g_defaultNotifier;

	// Sedert versie V11.01
	extern void TMDownloadAllSystemLog(
		CNodeDatabase& refDocDatabase,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadBoseSTCfg(
		CCanNode* pNode,
		const char* szFileDescription,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMDownloadSystemLog(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadGatewayCfg(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadRoomCtrlAudioCfg(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadUnitCreate(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern BOOL TMLoadFileUnitCreate(		
		int nodeAddress,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadMoods(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadIRRX(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadScheduling(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadIRTX(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadSmapAVRoom(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadSmapMyHome(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadSmapMain(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadBitmaps(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);	

	extern void TMUploadLedsCfg(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);	

	extern void TMUploadSonosSrcCfg(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadSmapLiteCfg(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);

	extern void TMUploadAVSettings(
		CCanNode* pNode,
		INotifier& refNotifier = g_defaultNotifier);
};

#endif