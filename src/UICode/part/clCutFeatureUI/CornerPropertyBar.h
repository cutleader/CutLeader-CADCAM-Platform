#pragma once

#include "clExpertLibCommon.h"
#include "CutFeaturePropertyBar.h"
#include <map>

DECLARE_CUTLEADER_CLASS(LoopCornerDataList)
DECLARE_CUTLEADER_CLASS(CornerList)
DECLARE_CUTLEADER_CLASS(CornerConfigItem)

BEGIN_CUTLEADER_NAMESPACE()

// ���������������ͱ༭���档
// ע��
//   1) �༭�������������ʱֻ��ʾ��һ�����������������ԡ�
class ClCutFeatureUI_Export CornerPropertyBar : public CutFeaturePropertyBar
{
private:
	CornerPropertyBar(void);

public:
	virtual ~CornerPropertyBar(void);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPropertyChange(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	static CornerPropertyBar* GetInstance(CWnd* pParent = NULL);

public:
	/************************************************************************/
	// ���ڴ�����������
	
	// ��ʾ���������ԡ�
	void DisplayCornerData_4_create(CornerType iCornerType_4_create, double dCornerValue_4_create);

	// �õ����õ����ԡ�
    CornerType GetCornerType() const { return m_iCornerType_4_create; }
    double GetCornerValue() const { return m_dCornerValue_4_create; }
	/************************************************************************/


	// ��ʾ���������ԡ�
	void DisplayCornerData_4_edit(const EditorData& editorData, LoopCornerDataListPtr pLoopCornerList, CornerListPtr pCornerList, CornerConfigItemPtr pCornerConfigItem);

private:
	void AdjustLayout();

    // ������Ϳؼ�
    XGridProperty* AddTypeControl(CMFCPropertyGridProperty* pGroup, CornerType iCurrentCornerType);

protected: // for controls.
	// the singleton object
	static CornerPropertyBar* m_pCornerPropBar;

	CMFCPropertyGridCtrl m_wndPropList;

private: // for data.
	// ���ڴ���ģʽ���Ǳ༭ģʽ��
	BOOL m_bCreate;

    // ����ģʽ���õ��ı���������������������������á�
    CornerType m_iCornerType_4_create;
    double m_dCornerValue_4_create;

	// �༭ģʽ���õ��ı�����
	LoopCornerDataListPtr m_pLoopCornerList; // ���С���·΢������������
	CornerListPtr m_pCornerList; // Ҫ�༭�ġ�����������
    CornerConfigItemPtr m_pCornerConfigItem; // �������������

	// �Ƿ����OnPropertyChange��Ϣ
	bool m_bIgnore_OnPropertyChangeMsg;
};

END_CUTLEADER_NAMESPACE()
