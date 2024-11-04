#pragma once

#include "clNestJobCommon.h"
#include "NestPriority.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(PartItem)

BEGIN_CUTLEADER_NAMESPACE()

// 待排零件。
// 注：
//  1) “待排零件”对象和零件一一对应。
//  2) 整个排版过程中这个对象只有一份而不会被克隆。
class ClNestJob_Export NestPart : public ClData
{
public:
	NestPart(void);
	NestPart(PartItemPtr pPartItem);

	~NestPart(void);

public: // set/get functions.
	void SetPartItem(PartItemPtr pPartItem) { m_pPartItem = pPartItem; }
	const PartItem* GetPartItem() const { return m_pPartItem.get(); }

	void SetPriority(NestPriority nestPriority) { m_nestPriority = nestPriority; }
	NestPriority GetPriority() const { return m_nestPriority; }

	void SetNestCount(int iNestCount) { m_iNestCount = iNestCount; }
	int GetNestCount() const { return m_iNestCount; }

	void SetRotStyle(PART_ROT_STYLE iRotStyle) { m_iRotStyle = iRotStyle; }
	PART_ROT_STYLE GetRotStyle() const { return m_iRotStyle; }

public:
	static CString GetRotStyleName(PART_ROT_STYLE iRotAng);

private:
	PartItemPtr m_pPartItem;

	// the nesting priority
	NestPriority m_nestPriority;

	// the count
	int m_iNestCount;

	// the rotation style of the part.
	PART_ROT_STYLE m_iRotStyle;
};

END_CUTLEADER_NAMESPACE()
