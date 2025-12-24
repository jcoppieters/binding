#pragma once

/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/

struct CComboxbox_util
{
	// Deze functie zorgt ervoor dat er pijlen komen
	// in de combobox indien er meerdere items aanwezig zijn.
	static
	BOOL setComboBoxSetHeight(
		CComboBox* pComboBox);

	// String van de huidige selectie.
	// TRUE = gevonden.
	// FALSE = geen string gevonden.
	static
	BOOL getComboBoxStrCurrSel(
		CComboBox& refComboBox,
		CString& refString);

	// Data opvragen van het geselcteerde element.
	// TRUE= item geselekteerd.
	// FALSE = item niet geselekteerd.
	static
	BOOL getComboBoxCurrSelDataItem(
		CComboBox& refComboBox,
		DWORD_PTR* pData);


	// Item toevoegen aan de combo box.
	static int setComboBoxItem(CComboBox& refComboBox,const CString& str,DWORD dwDataItem);

	/**
	 * @brief Selecteert het ComboBox-item dat het dichtst bij een opgegeven doelwaarde ligt, 
	 *        binnen een toegestane procentuele afwijking.
	 * 
	 * Deze functie doorzoekt de ComboBox op basis van integerwaarden opgeslagen via SetItemData().
	 * Het item waarvan de waarde (ItemData) het dichtst bij de opgegeven `targetValue` ligt,
	 * wordt geselecteerd, op voorwaarde dat het binnen de toegestane marge valt.
	 *
	 * @param refComboBox     Referentie naar de CComboBox waarin gezocht wordt.
	 * @param targetValue     Doelwaarde waarvoor het dichtstbijzijnde item gezocht wordt.
	 * @param percentageDiff  Maximale toegestane afwijking in procenten (bijv. 10 = ±10% marge).
	 * 
	 * @return TRUE als een item binnen de marge werd gevonden en geselecteerd, FALSE anders.
	 * 
	 * @note De functie gebruikt GetItemData() om integerwaarden op te halen. Zorg ervoor dat
	 *       de ComboBox correct is gevuld met geldige integer data via SetItemData().
	 *
	 * @see CComboBox::GetItemData
	 * @see CComboBox::SetItemData
	 * @see CComboBox::SetCurSel
	 */
	static BOOL setComboBoxItemSelect(CComboBox& refComboBox, int targetValue,int percentageDiff);
};

/*************************************************************/
};
};
/*************************************************************/