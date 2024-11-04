#pragma once

#include "clExpertLibUICommon.h"
#include "baseConst.h"
#include "NumEdit.h"
#include "taoresource.h"
#include "PierceDrawStatic.h"
#include "CGridListCtrlGroups.h"
#include "CLResizableDialog.h"

DECLARE_CUTLEADER_CLASS(ConditionTypeList)
DECLARE_CUTLEADER_CLASS(ToolAssignConfigItem)
DECLARE_CUTLEADER_CLASS(ToolAssignRule)

BEGIN_CUTLEADER_NAMESPACE()

class ClExpertLibUI_Export ToolAssignPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(ToolAssignPage)

public:
	ToolAssignPage();
	~ToolAssignPage();

	enum { IDD = IDD_clExpertLibUI_ToolAssign };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected:
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSelchangeDefaultTool();
    afx_msg LRESULT GetCellEditorData(WPARAM wParam, LPARAM lParam);
    afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnToolManagement();
	afx_msg void OnDxfToolMapping();
	afx_msg void OnAddRule();
	afx_msg void OnDeleteRule();

	DECLARE_MESSAGE_MAP()

public:
	void DisplayToolAssignInfo(ToolAssignConfigItemPtr pToolAssignConfigItem, ConditionTypeListPtr pCondTypeList);
    ToolAssignConfigItemPtr GetToolAssignConfigItem() const { return m_pToolAssignConfigItem; }

private:
	void InitGridStyle();

    // ��ʼ��grid������
	void InitGridData(bool bForInnerLoop);
    void AddRuleToGrid(ToolAssignRulePtr pToolAssignRule);

    // ��ʼ��Ĭ�ϵ�����Ϣ
    void InitDefaultTool(bool bForInnerLoop);

private: // controls
	CTabCtrl m_tabCtrl;
	CComboBox m_defaultTool_comboBox;
	CGridListCtrlGroups m_toolAssignRule_gridList;

    CBrush m_brush;

private: // for data.
    ToolAssignConfigItemPtr m_pToolAssignConfigItem;
	ConditionTypeListPtr m_pCondTypeList;
};

END_CUTLEADER_NAMESPACE()
