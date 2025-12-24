#pragma once

// Application license activation code.
class CAppLicActCode
{
public:
	CAppLicActCode(void);
	~CAppLicActCode(void);

	void setCode(unsigned long v);
	unsigned long getCode(void) const;

	void setFlags(BYTE v);
	BYTE getFlags(void) const;

	// Helper...
#ifdef SKIP_NOT_USED
	void setFlagNeeded(BOOL f);
#endif
	BOOL getFlagNeeded(void) const;
	void setFlagActivated(BOOL f);
	BOOL getFlagActivated(void) const;

private:
	enum
	{
		// FLAG_CODE_NEEDED: Vlag wordt momenteel niet gebruikt, staat standaard aan.
		FLAG_CODE_NEEDED = (0x01<<0),	

		// Geactiveerd: Wanneer deze vlag staat wordt de code niet meer gevraagd.
		FLAG_ACTIVATED	 = (0x01<<1)
	};

	unsigned long m_code;
	BYTE m_flags;
};
