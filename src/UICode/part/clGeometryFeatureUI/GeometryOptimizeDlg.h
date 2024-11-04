#pragma once

#include "clGeometryFeatureUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"

BEGIN_CUTLEADER_NAMESPACE()

// 图形优化对话框
class ClGeometryFeatureUI_Export GeometryOptimizeDlg : public CLDialog
{
	DECLARE_DYNCREATE(GeometryOptimizeDlg)

public:
	GeometryOptimizeDlg();
	~GeometryOptimizeDlg(void);

	enum { IDD = IDD_clGeometryFeatureUI_Optimize };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnBnClickedOk();
	afx_msg void OnKillAllowOffset();

	DECLARE_MESSAGE_MAP()

public:
	void SetdAllowOffset(double dAllowOffset) { m_dAllowOffset = dAllowOffset; }
	double GetdAllowOffset() { return m_dAllowOffset; }

private: // controls
	CNumEdit m_ctrlAllowOffset;

private:
	double m_dAllowOffset;
};

END_CUTLEADER_NAMESPACE()
