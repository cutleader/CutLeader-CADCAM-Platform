#pragma once

#include "clCutFeatureUICommon.h"
#include "CGridListCtrlGroups.h"
#include "CLResizableDialog.h"
#include "NumEdit.h"
#include "taoResource.h"
#include "EditorData.h"
#include "SubjectBase.h"


DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(LoopToolData)
DECLARE_CUTLEADER_CLASS(LoopToolDataList)
DECLARE_CUTLEADER_CLASS(ToolAssignConfigItem)
DECLARE_CUTLEADER_CLASS(ConditionTypeList)
DECLARE_CUTLEADER_CLASS(LoopStartCutDataList)


// �޸���һЩ�����ĵ��߳ߴ�󣬸��߹۲��ߡ�
#define ID_EVENT_ChangedToolSizeOfSelectedLoops   52136842


BEGIN_CUTLEADER_NAMESPACE()

// ����Ի���༭�����ϵĵ�����Ϣ��ֻ�а���apply��������޸�����������Ϣ��
class LoopToolEditDlg : public CLResizableDialog, public SubjectBase
{
	DECLARE_DYNCREATE(LoopToolEditDlg)

public:
	LoopToolEditDlg();
	~LoopToolEditDlg();

	enum { IDD = IDD_clCutFeatureUI_LoopToolInfo };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

protected:
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

    afx_msg void OnNoCutLoop();
	afx_msg void OnToolSelectChanged();
	afx_msg void OnColorClicked();

    afx_msg LRESULT GetCellEditorData(WPARAM wParam, LPARAM lParam);
    afx_msg void OnGridItemchanged_doPierceCondition(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnGridItemchanged_unPierceCondition(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnGridItemchanged_cutCondition(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGridEndEdit_doPierceCondition(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGridEndEdit_unPierceCondition(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGridEndEdit_cutCondition(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:
	void DisplayLoopToolInfo(LoopToolDataListPtr pLoopToolList, LoopStartCutDataListPtr pLoopStaEndCutList, LeadConfigItemPtr pLeadConfigItem,
		ToolAssignConfigItemPtr pToolAssignConfigItem, ConditionTypeListPtr pCondTypeList, EditorData editorData);

private:
	void InitGridStyle(CGridListCtrlGroups* pConditionGridList);
	void InitGridData(CGridListCtrlGroups* pConditionGridList, ConditionTypeListPtr pCondTypeList);

	// �ͷ���Դ
	void ReleaseGridData();
	void ReleaseComboBoxData();

	void AddConditionNode(LONGLONG iCondTypeID, CString strCondVal_4_display);
	void DeleteConditionNode(LONGLONG iCondTypeID);

private: // controls
	CComboBox m_toolName_comboBox;
	CNumEdit m_toolSize_editBox;
	CMFCColorButton	m_toolColor_btn;
	CGridListCtrlGroups m_doPierceCondition_gridList;
	CGridListCtrlGroups m_unPierceCondition_gridList;
	CGridListCtrlGroups m_cutCondition_gridList;

	// about scroll bar.
	CRect m_rcOriginalRect; // dialog size as you see in the resource view (original size)
	int m_nScrollPos; // actual scroll position
	int m_nCurHeight; // actual dialog height

	CBrush m_brush;

private: // data
    BOOL m_bNoCutting;
	LoopToolDataListPtr m_pLoopToolList;
	ToolAssignConfigItemPtr m_pToolAssignConfigItem;
	LoopStartCutDataListPtr m_pLoopStaEndCutList;
	LeadConfigItemPtr m_pLeadConfigItem;
    ConditionTypeListPtr m_pCondTypeList;

    // �༭������Ϣ
    EditorData m_editorData;
};

END_CUTLEADER_NAMESPACE()
