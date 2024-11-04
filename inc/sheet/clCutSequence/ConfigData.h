#pragma once

#include "clCutSequenceCommon.h"

DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(SequenceConfigItem)
DECLARE_CUTLEADER_CLASS(IMaterialSize)

BEGIN_CUTLEADER_NAMESPACE()

// 工序处理时用到的参数。
class ClCutSequence_Export ConfigData
{
public:
	ConfigData(void);
	ConfigData(LeadConfigItemPtr pLeadConfigItem, SequenceConfigItemPtr pSeqConfigItem, IMaterialSizePtr pMaterialSize);
	~ConfigData(void);

public: // get/set functions.
	const LeadConfigItem* GetLeadConfigItem() const { return m_pLeadConfigItem.get(); }
    LeadConfigItemPtr GetLeadConfigItem_FW() const { return m_pLeadConfigItem; }
	void SetLeadConfigItem(LeadConfigItemPtr pLeadConfigItem) { m_pLeadConfigItem = pLeadConfigItem; }

	const SequenceConfigItem* GetSeqConfigItem() const { return m_pSeqConfigItem.get(); }
	void SetSeqConfigItem(SequenceConfigItemPtr pSeqConfigItem) { m_pSeqConfigItem = pSeqConfigItem; }

	const IMaterialSize* GetMatSize() const { return m_pMatSize.get(); }
	void SetMatSize(IMaterialSizePtr pMaterialSize) { m_pMatSize = pMaterialSize; }

private:
	LeadConfigItemPtr m_pLeadConfigItem;
	SequenceConfigItemPtr m_pSeqConfigItem;

	IMaterialSizePtr m_pMatSize;
};

END_CUTLEADER_NAMESPACE()
