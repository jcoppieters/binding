#pragma once

class CConfigMultiMediaFile;

struct CConfigMultiMediaFileFactory
{
	enum Type
	{
		V64 = 0,
		V65 = 1,
		V66 = 2
	};
	static CConfigMultiMediaFile* Create(Type type);
};
