#pragma once

#include "Point2D.h"
#include "clExpertLibCommon.h"
#include "CutFeaturePropertyBar.h"
#include <vector>
#include <map>

DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(LoopStartCutDataList)

BEGIN_CUTLEADER_NAMESPACE()

// ����·���������ߺ����е㡱�༭���档
//  ע��
//    1) �ý�����ʾ3�����飺���иʼ�ͽ����㡱���飬�����������á����飬�����������á����顣
class ClCutFeatureUI_Export LoopLeadPropertyBar : public CutFeaturePropertyBar
{
private:
	LoopLeadPropertyBar(void);

public:
	virtual ~LoopLeadPropertyBar(void);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg LRESULT OnPropertyChange(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	static LoopLeadPropertyBar* GetInstance(CWnd* pParent = NULL);

	// ��ʾ���ݡ�
	void DisplayLoopLead(const EditorData& editorData, LoopStartCutDataListPtr pLoopStaEndCutList, std::vector<Point2D> startPtList, LeadConfigItemPtr pLeadConfigItem);

private:
	void AdjustLayout();

	// ��ʾ���иʼ�ͽ����㡱���顣
	void DisplayStaEndPt();

	// ��ʾ������/���������á����顣
	// ע��
	//   1) �����е����е㶼�ڱպϻ�·�Ľǵ��ǽǵ���ʱ�����ܵ������������ʾ���������߷��顣
	void DisplayLeadGroup(LeadInOutType iLeadType);

private: // for UI.
	// the singleton object
	static LoopLeadPropertyBar* m_pLoopLeadPropBar;

	CMFCPropertyGridCtrl m_wndPropList;

	// the button to set the start cut point.
	CButton m_setStartPtBtn;

private: // for data.
	// Ҫ�༭�ġ���·��������������Ӧ�����е㡣��
	LoopStartCutDataListPtr m_pLoopStaEndCutList;
	std::vector<Point2D> m_startPtList;

	// �����������߷�����򡱡�
	LeadConfigItemPtr m_pLeadSetting;

	// �������������塣
	std::map<CObject*, int> m_propItemInfo;

	// �Ƿ����OnPropertyChange��Ϣ
	bool m_bIgnore_OnPropertyChangeMsg;
};

END_CUTLEADER_NAMESPACE()
