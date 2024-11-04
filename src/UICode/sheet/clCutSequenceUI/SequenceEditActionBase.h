#pragma once

#include "clCutSequenceUICommon.h"
#include "ActionBase.h"
#include "IEditorInfo.h"

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(ConfigData)
DECLARE_CUTLEADER_CLASS(CutSequenceList)
DECLARE_CUTLEADER_CLASS(PartLayoutData)
DECLARE_CUTLEADER_CLASS(CutNodeList)

BEGIN_CUTLEADER_NAMESPACE()

class ClCutSequenceUI_Export SequenceEditActionBase : public ActionBase, public IEditorInfo
{
public:
	SequenceEditActionBase(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~SequenceEditActionBase(void);

public: // implement interface IEditorInfo.
	virtual EditorData GetEditorData() const;

public: // implement IInteract interface.
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
	// 更新切割节点缓存。
	void RefreshCutNodes();

protected:
	// the layout info of the sheet.
	PartLayoutDataPtr m_pPartLayoutData;

	// the sequence unit list of the sheet.
	CutSequenceListPtr m_pCutSequences;

	// 工序处理用到的参数。
	ConfigDataPtr m_pConfigData;

	// 缓存所有工序的“切割节点”。
	// 注：
	//   1) 生成这些“切割节点”时没有考虑偏移。
	//   2) 必要时需要更新这个变量。
	CutNodeListPtr m_pCutNodeList;
};

END_CUTLEADER_NAMESPACE()
