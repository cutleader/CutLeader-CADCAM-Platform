#pragma once

#include "clFrameCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

#define ID_FIRST_PANE	1000
#define ID_SECOND_PANE	1001

// ״̬����
// ע��
//  1) Ŀǰ������2����塣
class StatusBar
{
public:
	StatusBar(void);
	~StatusBar(void);

public:
	// ��ʼ��״̬����
	void Init(CWnd* pParentWnd);

	// ����ָ���������ݡ�
	void UpdateStatusBar(LPCTSTR str, int iID);

	// ��ʾ/����״̬����
	void Show();
	void Hide();

private:
	CMFCRibbonStatusBar m_statusBar;
};

END_CUTLEADER_NAMESPACE()
