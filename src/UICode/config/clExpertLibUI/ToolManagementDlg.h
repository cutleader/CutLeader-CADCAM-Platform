#pragma once

#include "CGridListCtrlGroups.h"
#include "clExpertLibUICommon.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(ConditionTypeList)
DECLARE_CUTLEADER_CLASS(ToolAssignConfigItem)
DECLARE_CUTLEADER_CLASS(ToolInstance)
DECLARE_CUTLEADER_CLASS(ToolInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

class ToolGridListCtrl : public CGridListCtrlGroups
{
public:
    ToolGridListCtrl(void) {}
    ~ToolGridListCtrl(void) {}

public:
    virtual bool OnDisplayCellColor(int nRow, int nCol, COLORREF& textColor, COLORREF& backColor);

public:
    void InitToolList(ToolInstanceListPtr pToolInstanceList) { m_pToolInstanceList = pToolInstanceList; }

private:
    ToolInstanceListPtr m_pToolInstanceList;
};

class ToolManagementDlg : public CLDialog
{
	DECLARE_DYNCREATE(ToolManagementDlg)

public:
	ToolManagementDlg();
	ToolManagementDlg(ToolAssignConfigItemPtr pToolAssignConfigItem, ConditionTypeListPtr pCondTypeList);
	~ToolManagementDlg(void);

	enum { IDD = IDD_clExpertLibUI_ToolManage };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
    afx_msg void OnDestroy();
    afx_msg BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	afx_msg void OnToolGrid_selectChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnToolGrid_endLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT GetCellEditorData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnConditionValGrid_itemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnConditionValGrid_endLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

    afx_msg void OnAddTool();
    afx_msg void OnDeleteTool();
	afx_msg void OnQuit();

	DECLARE_MESSAGE_MAP()

private:
	// ��ʼ������grid����ʽ��
	void InitGridStyle();

	// ��ʼ���б����ݡ�
	void AddToolToGrid(ToolInstancePtr pToolInstance);
	void InitGrid_of_conditionVal(ToolInstancePtr pToolInstance);

	// �õ���ǰѡ�еĵ���
	ToolInstancePtr GetSelectedTool();

private: // controls
	ToolGridListCtrl m_toolsGridList;
	CGridListCtrlGroups m_conditionValGridList;

private: // data
	ToolAssignConfigItemPtr m_pToolAssignConfigItem;
	ConditionTypeListPtr m_pCondTypeList;

	// �Ƿ�رյ��߱���select-change�¼���
	bool m_bDisableSelectChangeEvent_of_toolGrid;
};

END_CUTLEADER_NAMESPACE()
