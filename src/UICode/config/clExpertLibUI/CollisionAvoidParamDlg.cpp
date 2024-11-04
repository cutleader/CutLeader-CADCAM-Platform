#include "StdAfx.h"
#include "CollisionAvoidParamDlg.h"

#include "baseConst.h"

#include "CollisionAvoidParam.h"
#include "clUtilityResource.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(CollisionAvoidParamDlg, CDialogEx)

CollisionAvoidParamDlg::CollisionAvoidParamDlg()
: CLDialog(CollisionAvoidParamDlg::IDD)
{
}

CollisionAvoidParamDlg::CollisionAvoidParamDlg(CollisionAvoidParamPtr pCollisionAvoidParam)
				: CLDialog(CollisionAvoidParamDlg::IDD)
{
	m_pCollisionAvoidParam = pCollisionAvoidParam;
}

CollisionAvoidParamDlg::~CollisionAvoidParamDlg(void)
{
}

void CollisionAvoidParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_ADJUST_STARTCUT_POINT, m_bAdjustStartCutPt);
	DDX_Control(pDX, IDC_COMBO_ADJUST_STARTCUT_TYPE, m_ctrlAdjustStartCutPtType);

	DDX_Check(pDX, IDC_CHECK_GENERATE_AVOID_PATH, m_bGenerateAvoidPath);
	DDX_Control(pDX, IDC_EDIT_CUTOUT_AVOID, m_dCutOutAvoid);
	DDX_Control(pDX, IDC_EDIT_AVOID_DIS, m_dAvoidDis);
	DDX_Control(pDX, IDC_EDIT_AVOID_RATIO, m_dAvoidRatio);
}

BEGIN_MESSAGE_MAP(CollisionAvoidParamDlg, CDialogEx)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CollisionAvoidParamDlg::OnInitDialog()
{
	__super::OnInitDialog();
	
	// display the cut avoid params.
	DisplayCollisionAvoidParam();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

void CollisionAvoidParamDlg::DisplayCollisionAvoidParam()
{
	m_bAdjustStartCutPt = m_pCollisionAvoidParam->GetIsAdjustStartCutPt();
	m_ctrlAdjustStartCutPtType.ResetContent();
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ADJUST_STARTPT_INNERLOOP);
	m_ctrlAdjustStartCutPtType.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ADJUST_STARTPT_ALLLOOP);
	m_ctrlAdjustStartCutPtType.AddString(str);
	m_ctrlAdjustStartCutPtType.SetCurSel((int)m_pCollisionAvoidParam->GetAdjustStartCutPtType());

	m_bGenerateAvoidPath = m_pCollisionAvoidParam->GetIsGenerateAvoidPath();
	m_dCutOutAvoid.Init(m_pCollisionAvoidParam->GetCutOutAvoid(), 4, FALSE);
	m_dAvoidDis.Init(m_pCollisionAvoidParam->GetAvoidDis(), 4, FALSE);
	m_dAvoidRatio.Init(m_pCollisionAvoidParam->GetAvoidRatio(), 4, FALSE);
}

BOOL CollisionAvoidParamDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect r;
    GetClientRect(&r);
    if ((HBRUSH)m_brush == NULL)
        m_brush.CreateSolidBrush(RGB(239, 247, 253));
    pDC->FillRect(&r, &m_brush);

    return TRUE;
}

HBRUSH CollisionAvoidParamDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    TCHAR classname[MAX_PATH];
    if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
        return hbr;

    if (_tcsicmp(classname, _T("EDIT")) == 0)
        return hbr;

    if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
        return hbr;

    if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
        return hbr;

    if (_tcsicmp(classname, _T("LISTBOX")) == 0)
        return hbr;

    if (_tcsicmp(classname, WC_TREEVIEW) == 0)
        return hbr;

    pDC->SetBkColor(RGB(239, 247, 253));

    if ((HBRUSH)m_brush == NULL)
        m_brush.CreateSolidBrush(RGB(239, 247, 253));

    return (HBRUSH) m_brush;
}

void CollisionAvoidParamDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_pCollisionAvoidParam)
	{
		// “调整起切点”子规则
		if (m_bAdjustStartCutPt != m_pCollisionAvoidParam->GetIsAdjustStartCutPt())
		{
			m_pCollisionAvoidParam->SetIsAdjustStartCutPt(m_bAdjustStartCutPt);
			m_pCollisionAvoidParam->SetModified(TRUE);
		}
		if ((AdjustStartCutPtType)m_ctrlAdjustStartCutPtType.GetCurSel() != m_pCollisionAvoidParam->GetAdjustStartCutPtType())
		{
			m_pCollisionAvoidParam->SetAdjustStartCutPtType((AdjustStartCutPtType)m_ctrlAdjustStartCutPtType.GetCurSel());
			m_pCollisionAvoidParam->SetModified(TRUE);
		}

		// “生成避让路径”子规则
		if (m_bGenerateAvoidPath != m_pCollisionAvoidParam->GetIsGenerateAvoidPath())
		{
			m_pCollisionAvoidParam->SetIsGenerateAvoidPath(m_bGenerateAvoidPath);
			m_pCollisionAvoidParam->SetModified(TRUE);
		}
		if (m_pCollisionAvoidParam->GetCutOutAvoid() != m_dCutOutAvoid.Get_Number())
		{
			m_pCollisionAvoidParam->SetCutOutAvoid(m_dCutOutAvoid.Get_Number());
			m_pCollisionAvoidParam->SetModified(TRUE);
		}
		if (m_pCollisionAvoidParam->GetAvoidDis() != m_dAvoidDis.Get_Number())
		{
			m_pCollisionAvoidParam->SetAvoidDis(m_dAvoidDis.Get_Number());
			m_pCollisionAvoidParam->SetModified(TRUE);
		}
		if (m_pCollisionAvoidParam->GetAvoidRatio() != m_dAvoidRatio.Get_Number())
		{
			m_pCollisionAvoidParam->SetAvoidRatio(m_dAvoidRatio.Get_Number());
			m_pCollisionAvoidParam->SetModified(TRUE);
		}
	}

	OnOK();
}

END_CUTLEADER_NAMESPACE()
