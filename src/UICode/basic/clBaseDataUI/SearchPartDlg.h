#pragma once

#include "clBaseDataUICommon.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ProductItemList)

BEGIN_CUTLEADER_NAMESPACE()

// “查找零件”对话框。
class ClBaseDataUI_Export SearchPartDlg : public CLDialog
{
	DECLARE_DYNAMIC(SearchPartDlg)

public:
	SearchPartDlg(CWnd* pParent = NULL);
	virtual ~SearchPartDlg();

	enum { IDD = IDD_clBaseDataUI_FindPart };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckFolderFilter();
	afx_msg void OnSelectFilterFolder();
	afx_msg void OnCheckNameFilter();

	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	void Init(FolderNodePtr pWorkspace) { m_pFilterFolder = pWorkspace; }
	ProductItemListPtr GetResult() { return m_pProductItemList; }

private: // for control.
	// 目录。
	BOOL m_bFolderFilter;

	// 名称
	BOOL m_bNameFilter;
	CString m_strNameFilter;

private: // for data
	// 搜索的位置。
	FolderNodePtr m_pFilterFolder;

	// 查找的结果。
	ProductItemListPtr m_pProductItemList;

	// use this brush to paint the background.
	CBrush m_brush;
};

END_CUTLEADER_NAMESPACE()
