#pragma once

#include "clExpertLibUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"
#include "clExpertLibResource.h"

DECLARE_CUTLEADER_CLASS(CollisionAvoidParam)

BEGIN_CUTLEADER_NAMESPACE()

// set the cut avoidance params in this dialog.
class CollisionAvoidParamDlg : public CLDialog
{
	DECLARE_DYNCREATE(CollisionAvoidParamDlg)

public:
	CollisionAvoidParamDlg();
	CollisionAvoidParamDlg(CollisionAvoidParamPtr pCollisionAvoidParam);
	~CollisionAvoidParamDlg(void);

	enum { IDD = IDD_clExpertLib_CollisionAvoid_Config };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:
	// display the cut avoidance param.
	void DisplayCollisionAvoidParam();

private: // controls
	// ���������е㡱�ӹ���
	BOOL m_bAdjustStartCutPt;
	CComboBox m_ctrlAdjustStartCutPtType;

	// �����ɱ���·�����ӹ���
	BOOL m_bGenerateAvoidPath;
	CNumEdit m_dCutOutAvoid;
	CNumEdit m_dAvoidDis;
	CNumEdit m_dAvoidRatio;

    CBrush m_brush;

private:
	CollisionAvoidParamPtr m_pCollisionAvoidParam;
};

END_CUTLEADER_NAMESPACE()
