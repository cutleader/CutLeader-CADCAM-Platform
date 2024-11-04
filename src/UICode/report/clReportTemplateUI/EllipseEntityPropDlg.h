#pragma once

#include "clReportTemplateCommon.h"
#include "NumEdit.h"
#include "ColorStatic.h"
#include "taoResource.h"

DECLARE_CUTLEADER_CLASS(EllipseTplEntity)

BEGIN_CUTLEADER_NAMESPACE()

class EllipseEntityPropDlg : public CLDialog
{
	DECLARE_DYNCREATE(EllipseEntityPropDlg)

public:
	EllipseEntityPropDlg();
	~EllipseEntityPropDlg(void);

	enum { IDD = IDD_clReportTemplateUI_Ellipse_Property };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnFillColor();
	afx_msg void OnBorderColor();

	afx_msg void OnOK();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	void Init(EllipseTplEntityPtr pEllipseEntity) { m_pEllipseEntity = pEllipseEntity; }

private:
	CString m_strName;
	
	// for fill
	BOOL m_bFill;
	ColorStatic	m_ctrlFillColor;

	// for border.
	BOOL m_bBorder;
	CNumEdit m_iBorderThickness;
	CComboBox m_ctrlBorderStyle;
	ColorStatic	m_ctrlBorderColor;

private:
	// use this brush to paint the background.
	CBrush m_brush;

	EllipseTplEntityPtr m_pEllipseEntity;
};

END_CUTLEADER_NAMESPACE()
