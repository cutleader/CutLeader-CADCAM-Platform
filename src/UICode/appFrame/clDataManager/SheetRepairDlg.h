#pragma once

#include "clDataManagerCommon.h"
#include "taoresource.h"

BEGIN_CUTLEADER_NAMESPACE()

// 板材修复对话框。
// 注：
//  1) 如果板材的“参数配置”丢失，需要修复，其实此时板材中各个零件工艺的参数配置也丢失了。
//  2) 如果板材的材料丢失，需要修复。
class ClDataManager_Export SheetRepairDlg : public CLDialog
{
	DECLARE_DYNCREATE(SheetRepairDlg)

public:
	SheetRepairDlg();
	~SheetRepairDlg(void);

	enum { IDD = IDD_clDataManagerUI_SheetRepair };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnRepairScheme();
	afx_msg void OnRepairMat();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	void SetSheet(LONGLONG iSheetID) { m_iSheetID = iSheetID; }

private:
	// init the material group.
	void InitMatData();

	// init the scheme data.
	void InitSchemeData();

private: // controls
	CString m_strMsg;

	CString m_strExpLibItem;
	CString m_strConfig;

	// for "material info" group.
	CString m_strMat;
	CString m_strMatThick;
	CString m_strMatSize;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data member.
	LONGLONG m_iSheetID;
};

END_CUTLEADER_NAMESPACE()
