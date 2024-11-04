#pragma once

#include "clFrameCommon.h"
#include "XBarBase.h"
#include "PartComFuncDlg.h"
#include "SheetComFuncDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

// 常用功能面板。
class CommonFuncPane : public XBarBase
{
public:
	CommonFuncPane();
	virtual ~CommonFuncPane();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public:
	static CommonFuncPane* GetInstance(CWnd* pParent = NULL);

public:
	// 显示零件或板材功能面板。
	void DisplayPartPane();
	void DisplaySheetPane();

	PartComFuncDlg* GetPartPane() { return &m_partComFuncDlg; }
	SheetComFuncDlg* GetSheetPane() { return &m_sheetComFuncDlg; }

private: // for controls
	static CommonFuncPane* m_pCommonFuncPane;

private: // data members.
	PartComFuncDlg m_partComFuncDlg;
	SheetComFuncDlg m_sheetComFuncDlg;

	BOOL m_bPartMode;
	BOOL m_bSheetMode;
};

END_CUTLEADER_NAMESPACE()
