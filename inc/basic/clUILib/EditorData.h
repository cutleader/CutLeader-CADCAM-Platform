#pragma once

#include "clUILibCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// 编辑器信息。
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
	// 编辑器所在的视图。
	CWnd* m_pView;

	// 编辑器名称。
	CString m_strEditorName;
};

END_CUTLEADER_NAMESPACE()
