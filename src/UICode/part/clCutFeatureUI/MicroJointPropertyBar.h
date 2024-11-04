#pragma once

#include "CutFeaturePropertyBar.h"
#include <map>

DECLARE_CUTLEADER_CLASS(Point2DList)
DECLARE_CUTLEADER_CLASS(MicroJointList)
DECLARE_CUTLEADER_CLASS(LoopMicroJointList)

BEGIN_CUTLEADER_NAMESPACE()

// ��΢�����������༭���档
// ע��
//   1) �༭�����΢����������ʱֻ��ʾ��һ����΢���������������ԣ��Ҳ��ܵ���΢���ӵ㡣
class ClCutFeatureUI_Export MicroJointPropertyBar : public CutFeaturePropertyBar
{
private:
	MicroJointPropertyBar(void);

public:
	virtual ~MicroJointPropertyBar(void);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg LRESULT OnPropertyChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChangeMJPt();
	afx_msg void OnUpdateClickBtn(CCmdUI* pCmdUI);
	afx_msg LRESULT OnPropCheckBoxClick(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public: // set/get functions.
	BOOL IsChangingMJPt() const { return m_bChangingMJPt; }
	void IsChangingMJPt(BOOL bChangingMJPt) { m_bChangingMJPt = bChangingMJPt; }

public:
	static MicroJointPropertyBar* GetInstance(CWnd* pParent = NULL);

	// ��ʾ΢�������ԡ�
	void DisplayMJData(const EditorData& editorData, LoopMicroJointListPtr pLoopMicroJointList, MicroJointListPtr pMicroJointList, Point2DListPtr pMJPtList);

private:
	void AdjustLayout();

	/************************************************************************/
	// ��ʾ�������顣

	// ��ʾ΢����λ�úͿ�ȡ�
	void DisplayPosWidth();

	// ��ʾ�������������ݡ�
	void DisplayLeadInData();
	void DisplayLeadOutData();
	/************************************************************************/

	// �ͷ����ݡ�
	void ReleaseData();

protected: // for the controls
	static MicroJointPropertyBar *m_pMicroJointPropBar;

	CMFCPropertyGridCtrl m_wndPropList;

	// �������������顣
	CMFCPropertyGridProperty* m_pLeadInCondGroup;

	// �������������顣
	CMFCPropertyGridProperty* m_pLeadOutCondGroup;

	// �������������塣
	std::map<CObject*, int> m_propItemInfo;

	// the button to set the micro joint position.
	CButton m_setMicroJointPtBtn;

private: // for the data.
	// ���С���·΢������������
	LoopMicroJointListPtr m_pLoopMicroJointList;

	// Ҫ�༭�ġ�΢��������������Ӧ��΢���ӵ㡣
	// ע��
	//   1) ����һ��΢�����������ܶ�Ӧ���΢���ӵ�(�����������ʱ)�����������΢�������������е�����ͬ�ġ�
	MicroJointListPtr m_pMicroJointList;
	Point2DListPtr m_pMJPtList;

	BOOL m_bChangingMJPt;

	// �Ƿ����OnPropertyChange��Ϣ
	bool m_bIgnore_OnPropertyChangeMsg;
};

END_CUTLEADER_NAMESPACE()
