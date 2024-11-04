#pragma once

#include "clSheetCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(CutSequenceList)
DECLARE_CUTLEADER_CLASS(SheetSequenceData)

BEGIN_CUTLEADER_NAMESPACE()

// this class hold the sequences of sheet.
class ClSheet_Export SheetSequenceData : public ClData
{
public:
	SheetSequenceData(void);
	virtual ~SheetSequenceData(void);

public: // implement IData interface.
	virtual BOOL IsModified() const override;
    virtual LONGLONG GetLatestModifyTimeStamp() const override;
	virtual IDataPtr Clone() const override;

public: // get/set functions.
	void SetCutSequences(CutSequenceListPtr pSeqUnitList) { m_pCutSequences = pSeqUnitList; }
	CutSequenceListPtr GetCutSequences() const { return m_pCutSequences; }

public: // other functions.
	// filter out the part instances which have not finished the sequence.
	PartInstanceListPtr GetUnSequencedPartInstances(const PartInstanceList* pPartInstances);

private:
	// this class will allocate the memory.
	CutSequenceListPtr m_pCutSequences;
};

END_CUTLEADER_NAMESPACE()
