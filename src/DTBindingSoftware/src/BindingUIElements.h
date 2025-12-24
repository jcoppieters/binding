#ifndef BINDINGUIELEMENTS_H
#define BINDINGUIELEMENTS_H


/*************************************************************************/
#include "BindingUIElement.h"
class CBindingUnit;

/*************************************************************************/

namespace DUOTECNO {
namespace BINDINGS {
namespace UI {

/*************************************************************************
 * namespace DUOTECNO::BINDINGS::UI
 ************************************************************************/
class CBindingStringEntries_Col
{
public:

	typedef enum
	{
		DIMMER_UNITS = 0,
		DIMMER_UNITS_INPUT,				// Sedert versie 0x0B01
		SWITCH_UNITS,
		SWITCH_UNITS_INPUT,				// Sedert versie 0x0C01
		DUOSWITCH_UNITS,
		DUOSWITCH_UNITS_INPUT,			// Sedert versie 0x0C01
		IRTX_UNITS,
		SENS_UNITS_OUTPUT,
		SENS_UNITS_INPUT,
		SENSUNIV_UNITS_INPUT,
		BOSE_LF48_OUTPUT,
		BOSE_LF48_INPUT,
		BOSE_LF48_INPUT_VIRTUALS,
		IRAUDIO_UNITS_OUTPUT,
		AVMATRIX_UNITS,
		AVMATRIX_INPUT_UNITS,			/* TM_AVMATRIX_INPUT */
		VIDEOMUX_UNITS,
		MATRIXAUDIO_UNITS,
		MATRIXAUDIO_INPUT_UNITS,		/* TM_AUDIOHC4_BINDING_INPUT */
		MATRIXAUDIOBNO_INPUT_UNITS,		/* TM_AUDIOHC4_BNO */
		BOSE_V35_OUTPUT,
		BOSE_V35_OUTPUT_VIRTUALS,
		BOSE_V35_INPUT,
		BOSE_V35_INPUT_VIRTUALS,

		/* TM_CHANGES_BINDINGS_ALARM */
		ALARM_OUTPUT,
		ALARM_INPUT,

	
	} COLLECTION_KEY_UNITTYPE;

	typedef enum
	{
		BINDINGFLAGS = 0,
		
	} COLLECTION_KEY_TYPE;


	const CStringEntries* Get(const CBindingUnit* const pBindingUnit,
							  const COLLECTION_KEY_UNITTYPE Key) const;

	const CStringEntries* Get(const CBindingEntry* const pBindingEntry,
							  const COLLECTION_KEY_TYPE Key) const;
};

/*************************************************************************
 * namespace DUOTECNO::BINDINGS::UI::PRIVATE
 ************************************************************************/

namespace PRIVATE
{
/**********************************************************************************/
class CStringEntries_Collection
{
public:	
	CStringEntries_Collection() {} ;

	const CStringEntries* Get() const 
	{
		const CStringEntries* pStringEntries = 0;
	
		if (ViewModeExtended() || (!Basic())) 
			pStringEntries = Get_Extended();		
		else	
		{	
			if (!Simple())
			{
				pStringEntries = Get_Normal();
			}
			else
			{
				if (ViewModeNormal())
				{			
					pStringEntries = Get_Normal();
				}
				else
				{
					pStringEntries = Get_Simple();
				}
			}
		}
		return pStringEntries;			
	}

protected:

	virtual const CStringEntries* Get_Extended() const = 0;
	virtual const CStringEntries* Get_Normal() const  = 0;
	virtual const CStringEntries* Get_Simple() const = 0;
	virtual BOOL Basic() const = 0;
	virtual BOOL Simple() const = 0;


	BOOL ViewModeExtended() const;
	BOOL ViewModeNormal() const;
};



/*************************************************************************/
template <const CBindingStringEntries_Col::COLLECTION_KEY_TYPE ID>
class TBindingEntryStringEntries_Collection : public CStringEntries_Collection
{
private:
	const CBindingEntry* const m_pBindingEntry;

	static const COLLECTION_KEY2 EXTENDED;
	static const COLLECTION_KEY2 BASIC;
	static const COLLECTION_KEY2 SIMPLE;

public:
	TBindingEntryStringEntries_Collection(const CBindingEntry* const pBindingEntry) : 
	  m_pBindingEntry(pBindingEntry) {};
/*
	void Assign(const CBindingEntry* m_pBindingEntry) { 
		m_pBindingEntry = pBindingEntry;
	}
*/
protected:
	
	const CBindingStringEntries_Collection* Get_Col() const {
		return (CBindingStringEntries_Collection::Instance());
	}

	BYTE Get_Data() const {
		return (m_pBindingEntry->GetBindingReturnFlags());
	}
	
	virtual const CStringEntries* Get_Normal() const 
	{
		return (Get_Col()->Get(ID_Extended(),
							   ID_Basic(),
							   Get_Data()));
	}

	virtual const CStringEntries* Get_Simple() const 
	{
		return (Get_Col()->Get(ID_Extended(),
							   ID_Basic(),
							   ID_Simple(),
							   Get_Data()));
	}

	virtual const CStringEntries* Get_Extended() const {
		return (Get_Col()->Get(ID_Extended()));
	}

	virtual BOOL Basic() const {
		return (ID_Basic() != KEY_NOT_USED);
	}
	virtual BOOL Simple() const {
		return (ID_Simple() != KEY_NOT_USED);
	}


protected:
	COLLECTION_KEY2 ID_Extended() const { return EXTENDED; };
	COLLECTION_KEY2 ID_Basic() const { return BASIC; };
	COLLECTION_KEY2 ID_Simple() const { return SIMPLE; };
};

/*************************************************************************/

template <class BINDINGUNIT>
class TBindingStringEntries_Collection : public CStringEntries_Collection 
{
protected:
	const BINDINGUNIT* const m_pBindingUnit;

public:
	TBindingStringEntries_Collection(const BINDINGUNIT* const pBindingUnit) :
	  m_pBindingUnit(pBindingUnit) 
	  {};

protected:

	CBindingUnitStringEntries_Collection* Get_Col() const {
		return (CBindingUnitStringEntries_Collection::Instance());
	}

	virtual const CStringEntries* Get_Extended() const {
		return (Get_Col()->Get(ID_Extended()));
	}

	virtual const CStringEntries* Get_Normal() const 
	{
		return (Get_Col()->Get(ID_Extended(),
							   ID_Basic(),
							   Get_Data()));
	}

	virtual const CStringEntries* Get_Simple() const 
	{
		return (Get_Col()->Get(ID_Extended(),
							   ID_Basic(),
							   ID_Simple(),
							   Get_Data()));
	}

	virtual BOOL Basic() const {
		return (ID_Basic() != NOT_USED);
	}
	virtual BOOL Simple() const {
		return (ID_Simple() != NOT_USED);
	}

	virtual BYTE Get_Data() const = 0;
	virtual COLLECTION_KEY ID_Extended() const = 0;
	virtual COLLECTION_KEY ID_Basic() const = 0;
	virtual COLLECTION_KEY ID_Simple() const = 0;
};

/*************************************************************************/
template <class BINDINGUNIT,const CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE ID>
class TBindingInputStringEntries_Collection : public TBindingStringEntries_Collection<BINDINGUNIT>														
{
private:
	static const COLLECTION_KEY EXTENDED;
	static const COLLECTION_KEY BASIC;
	static const COLLECTION_KEY SIMPLE;

public:
	TBindingInputStringEntries_Collection(const BINDINGUNIT* const pBindingUnit) : 
	TBindingStringEntries_Collection<BINDINGUNIT> (pBindingUnit) {};

protected:
	virtual BYTE Get_Data() const { 
		return m_pBindingUnit->Get_Event();
	}
	
	virtual COLLECTION_KEY ID_Extended() const { return EXTENDED; };
	virtual COLLECTION_KEY ID_Basic() const { return BASIC; };
	virtual COLLECTION_KEY ID_Simple() const { return SIMPLE; };
};

/*************************************************************************/

template <class BINDINGUNIT,const CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE ID>
class TBindingOutputStringEntries_Collection : public TBindingStringEntries_Collection<BINDINGUNIT>
{
private:
	static const COLLECTION_KEY EXTENDED;
	static const COLLECTION_KEY BASIC;
	static const COLLECTION_KEY SIMPLE;

public:
	TBindingOutputStringEntries_Collection(const BINDINGUNIT* const pBindingUnit) : 
	TBindingStringEntries_Collection<BINDINGUNIT>(pBindingUnit) {};

protected:
	virtual BYTE Get_Data() const { 
		return m_pBindingUnit->Get_Function();
	}	

	virtual COLLECTION_KEY ID_Extended() const { return EXTENDED; };
	virtual COLLECTION_KEY ID_Basic() const { return BASIC; };
	virtual COLLECTION_KEY ID_Simple() const { return SIMPLE; };

};
/*************************************************************************/


/*************************************************************************/
} /* namespace PRIVATE */


} /* namespace UI */
} /* namespace BINDINGS */
} /* namespace DUOTECNO */

#endif

