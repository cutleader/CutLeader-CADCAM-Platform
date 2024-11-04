#pragma once

#include "clPartLayoutUICommon.h"
#include "DataProperty.h"
#include "CGridListCtrlGroups.h"
#include "CLResizableDialog.h"
#include "taoResource.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ProductItemList)
DECLARE_CUTLEADER_CLASS(ParamConfig)
DECLARE_CUTLEADER_CLASS(PartLayoutEditor)

BEGIN_CUTLEADER_NAMESPACE()

class PartAddDlg : public CLResizableDialog
{
	DECLARE_DYNCREATE(PartAddDlg)

public:
	PartAddDlg();
	~PartAddDlg();

	enum { IDD = IDD_clPartLayoutUI_PartAdd };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnPartListchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnSearchPart();
	afx_msg void OnAddToSheet();

	DECLARE_MESSAGE_MAP()

public:
	// call this to update this dialog.
	void DisplayPartList(PartLayoutEditorPtr pPartLayoutEditor, ParamConfigPtr pParamConfig, FolderNodePtr pWorkspace);

private:
	// init the style of the list ctrl
	void InitListStyle();

	// ���±��
	void UpdatePartGrid(ProductItemListPtr pProductItemList);

	// �ͷű�����ݡ�
	void ReleaseGridData();

	// Ԥ��ѡ�е������
	void PreviewPart();

private: // controls
	// use this brush to paint the background.
	CBrush m_brush;

	/************************************************************************/
	// about scroll bar.

	// dialog size as you see in the resource view (original size)
	CRect m_rcOriginalRect;

	// actual scroll position
	int m_nScrollPos;

	// actual dialog height
	int m_nCurHeight;
	/************************************************************************/

	// ��ʾ��
	CString m_strMsg;

	// available parts group.
	CGridListCtrlGroups m_partList;

	// Ԥ�����ڡ�
	CStatic m_previewWnd;

	// ����б��Ƿ��ʼ���ꡣ
	BOOL m_bFinishAddPart;

private: // data member.
	// the root node of the product structure.
	FolderNodePtr m_pWorkspace;

	// the sheet editor.
	PartLayoutEditorPtr m_pPartLayoutEditor;

	// ��ĵġ��������á���
	ParamConfigPtr m_pParamConfig;
};

END_CUTLEADER_NAMESPACE()
