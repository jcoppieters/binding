#ifndef CBindingFileInfo

class CBindingFile;
class CBindingFileInfo_Util;

class CBindingFileInfo 
{
	friend class CBindingFileInfo_Util;
	
public:
	CBindingFileInfo(CBindingFile* parent) : 
		m_pBindingFile(parent),
		m_NrOfBindings(0),
		m_NrOfTimers(0),
		m_NrOfReferences(0),
		m_NrOfCBindings(0),
		m_NodeMaxTimers(0),
		m_NodeMaxReferences(0),
		m_NodeMaxCBindings(0),
		m_NodeMaxBindings(0),
		m_NodeMaxConditionLength(0)
	{
	};

	int GetNrBinding(void) const {
		return m_NrOfBindings; 
	};
	int GetNrTimers(void) const {
		return m_NrOfTimers;	
	};
	int GetNrReferences(void) const {
		return m_NrOfReferences; 
	};
	int GetNrCBindings(void) const	{
		return m_NrOfCBindings; 
	};
	int getCBindingCondLength(int idx) const;

	// Node properties...
	int GetNodeMaxBindings(void) const {
		return m_NodeMaxBindings; 
	}
	int GetNodeMaxTimers(void)	const {
		return m_NodeMaxTimers;	
	}
	int GetNodeMaxReferences(void)	const {
		return m_NodeMaxReferences; 
	}
	int GetNodeMaxCBindings(void) const {
		return m_NodeMaxCBindings; 
	}	
	int getNodeMaxConditionLength(void) const {
		return m_NodeMaxConditionLength;
	}
	void SetNodeMaxBindings(int max) {
		m_NodeMaxBindings = max; 
	}
	void SetNodeMaxTimers(int max)	{
		m_NodeMaxTimers = max;	
	}
	void SetNodeMaxReferences(int max)	{
		m_NodeMaxReferences = max; 
	}
	void SetNodeMaxCBindings(int max){
		m_NodeMaxCBindings = max; 
	}
	void setNodeMaxConditionLength(int max) {
		m_NodeMaxConditionLength=max;
	}

private:	
	CBindingFile* const m_pBindingFile;

	// Worden enkel gezet via de CBindingFileInfo_Util klasse.
	int m_NrOfBindings;
	int m_NrOfTimers;
	int m_NrOfReferences;
	int m_NrOfCBindings;

	// Node properties.
	int m_NodeMaxTimers;
	int m_NodeMaxReferences;
	int m_NodeMaxCBindings;
	int m_NodeMaxBindings;
	int m_NodeMaxConditionLength;
};

/////////////////////////////////////////////////

// CHANGES_BINDINGS_REFERENCES
class CBindingFileInfo_Util
{
public:
	static void reset(CBindingFileInfo* ptr);

	static void setNrOfBindings(CBindingFileInfo* ptr,int nr);
	static void incNrOfCBindings(CBindingFileInfo* ptr);
	static void incNrOfTimers(CBindingFileInfo* ptr);
	static void incNrOfReferences(CBindingFileInfo* ptr);
};

///////////////////////////////////////////////////

#endif
