#pragma once

#include "clReportTemplateCommon.h"
#include "ColorStatic.h"
#include "taoResource.h"

DECLARE_CUTLEADER_CLASS(BindItemList)
DECLARE_CUTLEADER_CLASS(DynImgEntity)

BEGIN_CUTLEADER_NAMESPACE()

class DynImgEntityPropDlg : public CLDialog
{
	DECLARE_DYNCREATE(DynImgEntityPropDlg)

public:
	DynImgEntityPropDlg();
	~DynImgEntityPropDlg(void);

	enum { IDD = IDD_clReportTemplateUI_DynamicPicture_Property };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnBorderColor();
	afx_msg void OnOK();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	void Init(DynImgEntityPtr pDynImgEntity, BindItemListPtr pAvailBindItemList);

private:
	CString m_strName;

	// for border.
	BOOL m_bBorder;
	int m_iBorderThickness;
	CComboBox m_ctrlBorderStyle;
	ColorStatic	m_ctrlBorderColor;

	CComboBox m_ctrlBindType;

private:
	// use this brush to paint the background.
	CBrush m_brush;

	DynImgEntityPtr m_pDynImgEntity;
	BindItemListPtr m_pAvailBindItemList;
};

END_CUTLEADER_NAMESPACE()
