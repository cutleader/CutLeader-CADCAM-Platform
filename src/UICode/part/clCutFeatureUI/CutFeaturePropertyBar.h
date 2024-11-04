#pragma once

#include "clCutFeatureUICommon.h"
#include "XBarBase.h"
#include "IEditorInfo.h"

BEGIN_CUTLEADER_NAMESPACE()

// the basic class for all prop bars in this module.
class ClCutFeatureUI_Export CutFeaturePropertyBar : public XBarBase, public IEditorInfo
{
public:
	CutFeaturePropertyBar(void);
	virtual ~CutFeaturePropertyBar(void);

public: // implement interface IEditorInfo.
	virtual EditorData GetEditorData() const { return m_editorData; }

protected:
	// ��ʾ���ĵ��ö���Ҫ�������ֵ����Ϊ������������cam-feature��part-layout���ֱ༭����ʹ�á�
	EditorData m_editorData;
};

END_CUTLEADER_NAMESPACE()
