#pragma once

#include "CGridListCtrlGroups.h"
#include "clExpertLibUICommon.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(ToolAssignConfigItem)
DECLARE_CUTLEADER_CLASS(DxfLayerToolMap)

BEGIN_CUTLEADER_NAMESPACE()

class ToolDxfMapDlg : public CLDialog
{
	DECLARE_DYNCREATE(ToolDxfMapDlg)

public:
	ToolDxfMapDlg();
	~ToolDxfMapDlg(void);

	enum { IDD = IDD_clExpertLibUI_ToolDxfMap };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();

	afx_msg void OnLayerGrid_endLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColorGrid_endLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT GetCellEditorData(WPARAM wParam, LPARAM lParam);

	afx_msg void OnMapByLayerClicked();
	afx_msg void OnMapByColorClicked();
	afx_msg void OnMapNoneClicked();

	afx_msg void OnAddRule();
	afx_msg void OnDeleteRule();
	afx_msg void OnOkClicked();
	afx_msg void OnCancelClicked();

	DECLARE_MESSAGE_MAP()

public:
	void InitDlg(ToolAssignConfigItemPtr pToolAssignConfigItem) { m_pToolAssignConfigItem = pToolAssignConfigItem; }

private:
	// ��ʼ������grid����ʽ��
	void InitGridStyle();

	// ���������б�Ŀɼ�״̬
	void UpdateStatus_of_twoGrids();

	// ���ͼ�������ظ�
	bool LayerName_exist(CString strName);
	CString CheckLayerName(CString strName); // ���Ҫ���������ͼӺ�׺1,2,3...

	// �����ɫ�����ظ�
	bool LayerColor_exist(int iColorIndex);
	int CheckLayerColor(int iColorIndex); // ���Ҫ�����������ۼ�1

private: // controls
	int m_nDxfMapType;
	CGridListCtrlGroups m_gridList_layerMap;
	CGridListCtrlGroups m_gridList_colorMap;

	CBrush m_brush;

private: // data
	ToolAssignConfigItemPtr m_pToolAssignConfigItem;
};

END_CUTLEADER_NAMESPACE()
