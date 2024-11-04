#pragma once

#include "clPartUICommon.h"
#include "CGridListCtrlGroups.h"
#include "taoresource.h"
#include "LineArc2DStaticControl.h"
#include "CLDialog.h"

DECLARE_CUTLEADER_CLASS(PatternLoopList)
DECLARE_CUTLEADER_CLASS(LoopTopologyItemList)

BEGIN_CUTLEADER_NAMESPACE()

// 这个对话框用于从回路识别零件。
// 注：
//   1) 此时这些回路没有一个封闭外围回路。
//   2) 返回零件拓扑结构。
class ClPartUI_Export Loop2PartDlg : public CLDialog
{
	DECLARE_DYNCREATE(Loop2PartDlg)

public:
	Loop2PartDlg();
	~Loop2PartDlg(void);

	enum { IDD = IDD_clPartUI_Loop2Part };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectAll();
	afx_msg void OnRecognizeSamePart();

	afx_msg void OnAccept();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	// 初始化回路集合。
	void Init(LoopTopologyItemListPtr pLoopTopItemList, CString strName, BOOL bDisplay_RecognizeSamePartBtn);

	// 得到识别出的零件集合。
	std::vector<std::pair<CString, PatternLoopListPtr>> GetRecognizedParts(BOOL& bRecognizeSamePart)
	{
		bRecognizeSamePart = m_bRecognizeSamePart;
		return m_selPartList;
	}

private:
	// 初始化表格。
	void InitGrid();

private: // controls
	CString m_strMsg_1;
	CString m_strMsg_2;

	// 识别出的零件列表。
	CGridListCtrlGroups m_partGrid;

	// 是否全选。
	BOOL m_bSelectAll;

	// 是否识别相同的零件
	BOOL m_bRecognizeSamePart;

	// 预览窗口。
	LineArc2DStaticControl m_previewWnd;

	// use this brush to paint the background.
	CBrush m_brush;

	BOOL m_bDisplay_RecognizeSamePartBtn;

private: // 数据。
	// 所有回路的拓扑结构信息。 
	LoopTopologyItemListPtr m_pLoopTopItemList;

	CString m_strName;

	// 识别出的零件集合。
	std::vector<std::pair<CString, PatternLoopListPtr>> m_partList;

	// 用户想要的零件。
	std::vector<std::pair<CString, PatternLoopListPtr>> m_selPartList;

	// 初始化的时候禁止一些消息。
	BOOL m_bEnableEvent;
};

END_CUTLEADER_NAMESPACE()
