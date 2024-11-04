#pragma once

#include "clExpertLibUICommon.h"
#include "NumEdit.h"
#include "Rect2D.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(GridCutParam)

BEGIN_CUTLEADER_NAMESPACE()

class GridCutParamDlg : public CLDialog
{
	DECLARE_DYNCREATE(GridCutParamDlg)

public:
	GridCutParamDlg();
	GridCutParamDlg(GridCutParamPtr pGridCutParam);
	~GridCutParamDlg(void);

	enum { IDD = IDD_clExpertLibUI_GridCutConfig };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:
	void DisplayGridCutParam();

private: // controls
	BOOL m_bVertical_4_inner;
	BOOL m_bCCW_4_outer;

    CBrush m_brush;

private:
	GridCutParamPtr m_pGridCutParam;
};

END_CUTLEADER_NAMESPACE()
