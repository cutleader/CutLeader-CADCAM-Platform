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
	// �����и�ڵ㻺�档
	void RefreshCutNodes();

protected:
	// the layout info of the sheet.
	PartLayoutDataPtr m_pPartLayoutData;

	// the sequence unit list of the sheet.
	CutSequenceListPtr m_pCutSequences;

	// �������õ��Ĳ�����
	ConfigDataPtr m_pConfigData;

	// �������й���ġ��и�ڵ㡱��
	// ע��
	//   1) ������Щ���и�ڵ㡱ʱû�п���ƫ�ơ�
	//   2) ��Ҫʱ��Ҫ�������������
	CutNodeListPtr m_pCutNodeList;
};

END_CUTLEADER_NAMESPACE()
