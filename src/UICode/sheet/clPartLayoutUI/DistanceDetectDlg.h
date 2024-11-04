#pragma once

#include "NumEdit.h"
#include "taoresource.h"
#include "PartPlacementMoveBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// “智能靠碰参数”对话框。
// 注：
//  1) 对话框初始化时从“系统参数”中获得参数。
//  2) 移动零件时用到的参数来自这个对话框。
class ClPartLayoutUI_Export DistanceDetectDlg : public CLDialog
{
	DECLARE_DYNCREATE(DistanceDetectDlg)

public:
	DistanceDetectDlg(CWnd* pParent = NULL);
	~DistanceDetectDlg(void);

	enum { IDD = IDD_clPartLayoutUI_DistanceDetect };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckComcut();
	afx_msg void OnApply();
    afx_msg void OnQuitCurrentActionButton();

	DECLARE_MESSAGE_MAP()

public:
	void InitAction(PartPlacementMoveBase* pPartPmtMoveBase) { m_pPartPmtMoveBase = pPartPmtMoveBase; }
	PartAttachConfig GetPartAttachConfig();

private: // controls
	// use this brush to paint the background.
	CBrush m_brush;

	BOOL m_bComCut;
	CNumEdit m_ctrlPartGap;
	CNumEdit m_ctrlShtMargin;
    CMFCButton m_ctrlQuitCurrentActionButton;

private:
	// 传入支持智能移动的动作。
	PartPlacementMoveBase* m_pPartPmtMoveBase;
};

END_CUTLEADER_NAMESPACE()
