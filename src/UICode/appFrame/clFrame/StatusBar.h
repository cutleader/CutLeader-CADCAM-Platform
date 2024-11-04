#pragma once

#include "clFrameCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

#define ID_FIRST_PANE	1000
#define ID_SECOND_PANE	1001

// 状态栏。
// 注：
//  1) 目前定义了2个面板。
class StatusBar
{
public:
	StatusBar(void);
	~StatusBar(void);

public:
	// 初始化状态栏。
	void Init(CWnd* pParentWnd);

	// 更新指定面板的内容。
	void UpdateStatusBar(LPCTSTR str, int iID);

	// 显示/隐藏状态栏。
	void Show();
	void Hide();

private:
	CMFCRibbonStatusBar m_statusBar;
};

END_CUTLEADER_NAMESPACE()
