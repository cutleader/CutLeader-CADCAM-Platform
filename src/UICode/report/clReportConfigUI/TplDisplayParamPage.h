#pragma once

#include "clReportConfigUICommon.h"
#include "taoResource.h"
#include "CLResizableDialog.h"
#include "ColorStatic.h"

DECLARE_CUTLEADER_CLASS(TplDisplayParam)

BEGIN_CUTLEADER_NAMESPACE()

class TplDisplayParamPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(TplDisplayParamPage)

public:
	TplDisplayParamPage(void);
	~TplDisplayParamPage(void);

	enum { IDD = IDD_clReportConfigUI_Display };

protected: // inherit the interface
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public: // message mapping.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnMarginColor();
	afx_msg void OnGridColor();

	DECLARE_MESSAGE_MAP()

public:
	void DisplayDisParam(TplDisplayParamPtr pTplDisplayParam);

	// fill the Display Param using the user input.
	TplDisplayParamPtr GetTplDisplayParam();

private: // controls.
	CString m_strMsg;

	// text justification type.
	CComboBox m_ctrlDisplayUnit;

	// paper margin.
	BOOL m_bShowMargin;
	ColorStatic	m_ctrlMarginColor;

	// grid.
	BOOL m_bShowGrid;
	ColorStatic	m_ctrlGridColor;

private:
	// use this brush to paint the background.
	CBrush m_brush;

	TplDisplayParamPtr m_pTplDisplayParam;
};

END_CUTLEADER_NAMESPACE()
