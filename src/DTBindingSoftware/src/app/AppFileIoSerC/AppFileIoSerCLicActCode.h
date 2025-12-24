#pragma once

class CAppFileIoSer;

class CAppFileIoSerCLicActCode
{
public:
	virtual void update(
		CAppFileIoSer* pDest,
		const void* pvData);

	virtual void update(
		void* pvData,
		const CAppFileIoSer& pSrc);
};