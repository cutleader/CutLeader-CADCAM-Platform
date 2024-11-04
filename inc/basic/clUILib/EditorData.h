#pragma once

#include "clUILibCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// �༭����Ϣ��
class ClUILib_Export EditorData
{
public:
	EditorData();
	EditorData(CWnd* pView, CString strEditorName);
	~EditorData(void);

public: // get/set functions.
	void SetView(CWnd* pView) { m_pView = pView; }
	CWnd* GetView() const { return m_pView; }

	void SetEditorName(CString strEditorName) { m_strEditorName = strEditorName; }
	CString GetEditorName() const { return m_strEditorName; }

private:
	// �༭�����ڵ���ͼ��
	CWnd* m_pView;

	// �༭�����ơ�
	CString m_strEditorName;
};

END_CUTLEADER_NAMESPACE()
