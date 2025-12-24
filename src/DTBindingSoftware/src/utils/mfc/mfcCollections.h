#pragma once
/*************************************************************/
#include <afxtempl.h> // Zorg ervoor dat deze header is opgenomen
/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/

// Alle elementen copieren van een map.
template<typename KEY, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
void CopyCMap(const CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>& srcMap, CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>& destMap)
{
    // Wis de doelmap om bestaande gegevens te verwijderen
    destMap.RemoveAll();

    // Itereer door de bronmap
    POSITION pos = srcMap.GetStartPosition();
    KEY key;
    VALUE value;

    while (pos != NULL)
    {
        // Haal de sleutel en waarde op
        srcMap.GetNextAssoc(pos, key, value);

        // Voeg ze toe aan de doelmap
        destMap.SetAt(key, value);
    }
}


// Alle elementen dealloceren en verwijderen uit de map.
template<typename KEY,typename ARG_KEY, typename VALUE, typename ARG_VALUE>
void DeallocCMapValues(CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>& destMap) {
	POSITION pos = destMap.GetStartPosition();
	KEY keyValue;	
	while (pos != NULL) {
		VALUE pData = NULL;
		destMap.GetNextAssoc(pos, keyValue, pData);
		destMap.RemoveKey(keyValue);
		if (NULL != pData) {
			delete pData;
			pData = NULL;
		}
	}
	ENSURE(destMap.GetSize() == 0);
}

/*************************************************************/
};
};
/*************************************************************/