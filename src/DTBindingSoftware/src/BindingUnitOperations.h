// BindingUnitOperations.h: interface for the CBindingUnitOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGUNITOPERATIONS_H__CB124A5B_5664_4720_AC84_04CDB47384AF__INCLUDED_)
#define AFX_BINDINGUNITOPERATIONS_H__CB124A5B_5664_4720_AC84_04CDB47384AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingUnit;
class CCanNode;
struct STRUCT_UNIT;

class CBindingUnitOperations  
{
protected:
	CBindingUnit* const m_pBindingUnit;

public:
	CBindingUnitOperations(CBindingUnit* pBindingUnit = NULL);
	virtual ~CBindingUnitOperations();

public:
	static BYTE ConvertType(const CCanNode* pNode,STRUCT_UNIT* const pUnit);

	// Sedert versie 0x0B01
	// Controle of het type binding unit overeenkomt met het type van de unit.
	// Er zijn een aantal uitzonderingen mogelijk.
	// o.a. UNIT_TYPE_DIM, UNIT_TYPE_BOSERS, UNIT_TYPE_AVMATRIX,
	// UNIT_TYPE_AUDIO_HC4, UNIT_TYPE_ALARM
	static BOOL verifyTypes(const CBindingUnit* pBindingUnit,const STRUCT_UNIT* pUnit);

	BOOL UpgradeBindingUnitType(CBindingUnit** pNewBindingUnit);
	BOOL Export(FILE* fp);
};

#endif // !defined(AFX_BINDINGUNITOPERATIONS_H__CB124A5B_5664_4720_AC84_04CDB47384AF__INCLUDED_)
