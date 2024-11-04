#pragma once

#include "clExpertLibUICommon.h"
#include "NumEdit.h"
#include "Rect2D.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(FlyCutParam)

BEGIN_CUTLEADER_NAMESPACE()

class FlyCutParamDlg : public CLDialog
{
	DECLARE_DYNCREATE(FlyCutParamDlg)

public:
	FlyCutParamDlg();
	FlyCutParamDlg(FlyCutParamPtr pFlyCutParam);
	~FlyCutParamDlg(void);

	enum { IDD = IDD_clExpertLibUI_FlyCutConfig };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:
	void DisplayFlyCutParam();

private: // controls
	BOOL m_bVerticalCut;
	CNumEdit m_dMax_turning_radius;

    CBrush m_brush;

private:
	FlyCutParamPtr m_pFlyCutParam;
};

END_CUTLEADER_NAMESPACE()
