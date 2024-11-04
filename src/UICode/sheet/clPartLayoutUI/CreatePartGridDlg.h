#pragma once

#include "clPartLayoutUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(PartPlacement)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// 这个对话框用来定义零件阵列。
class ClPartLayoutUI_Export CreatePartGridDlg : public CLDialog
{
	DECLARE_DYNCREATE(CreatePartGridDlg)

public:
	CreatePartGridDlg(CWnd* pParent = NULL);
	~CreatePartGridDlg(void);

	enum { IDD = IDD_clPartLayoutUI_CreatePartGrid };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckComcut();
	afx_msg void OnPreview();
	afx_msg void OnOK();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()

public: // get/set functions.
	BOOL IsCanceled() const { return m_bCanceled; }
	BOOL IsCreateGrid() const { return m_bCreateGrid; }

public:
	// 显示对话框。
	// 注：
	//  1) 在动作中按下左键时调用这个函数以显示对话框。
	void ShowPartGridDlg(GlViewPortPtr pViewPort, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartPlacementPtr pBasePartPmt, PartPlacementListPtr pGridPartPmts);

private:
	// 创建零件阵列。
	// 注：
	//  1) 这个函数需要检查输入的数据。
	PartPlacementPtr CreatePartGrid();

private: // controls
	// use this brush to paint the background.
	CBrush m_brush;

	// 零件间距。
	CNumEdit m_ctrlPartDis;
	BOOL m_bComCut;

	// 行列数。
	CNumEdit m_ctrlRowNum;
	CNumEdit m_ctrlColumnNum;

	// 阵列方向。
	CComboBox m_ctrlGridDir;

	CMFCButton m_ctrlPreview;
	CMFCButton m_ctrlOK;
	CMFCButton m_ctrlCancel;

private: // data
	GlViewPortPtr m_pViewPort;

	// 所有零件放置和零件实例。
	PartPlacementListPtr m_pPartPmts;
	PartInstanceListPtr m_pPartInsts;

	// 基准零件。
	PartPlacementPtr m_pBasePartPmt;

	// 创建的阵列。
	// 注:
	//  1) 虽然是集合对象，里面最多只有一个元素。
	PartPlacementListPtr m_pGridPartPmts;

	// 是否取消了。
	// 注：
	//  1) 动作中需要获得这个值以退出动作。
	BOOL m_bCanceled;

	// 是否确定创建阵列。
	// 注：
	//  1) 动作中需要获得这个值，然后创建命令和退出动作。
	BOOL m_bCreateGrid;
};

END_CUTLEADER_NAMESPACE()
