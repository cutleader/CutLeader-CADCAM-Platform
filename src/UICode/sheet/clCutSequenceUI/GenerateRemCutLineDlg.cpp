#include "StdAfx.h"
#include "GenerateRemCutLineDlg.h"

#include "baseConst.h"
#include "clCutSequenceResource.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(GenerateRemCutLineDlg, CDialogEx)

GenerateRemCutLineDlg::GenerateRemCutLineDlg(CWnd* pParent /*=NULL*/)
		: CLDialog(GenerateRemCutLineDlg::IDD, pParent)
{
	m_iCmdType = GenerateRemCutLineDlg_noCmd;
}

GenerateRemCutLineDlg::~GenerateRemCutLineDlg(void)
{
}

void GenerateRemCutLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_REM_POSITION, m_comboRemnantPosition);
	DDX_Control(pDX, IDC_EDIT_OFFSET_DIS, m_ctrlOffsetDistance);
	DDX_Control(pDX, IDC_EDIT_STEP_WIDTH, m_ctrlStepWidth);
}

BOOL GenerateRemCutLineDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_ctrlOffsetDistance.Init(0.0, 3, FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void GenerateRemCutLineDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
    delete this;
}

BEGIN_MESSAGE_MAP(GenerateRemCutLineDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_BTN_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_BTN_SAVE_REM_MAT, OnSaveRemMat)
	ON_BN_CLICKED(IDC_BTN_OK, OnGenerate)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnQuit)
END_MESSAGE_MAP()

BOOL GenerateRemCutLineDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH GenerateRemCutLineDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_SHT)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void GenerateRemCutLineDlg::OnPreview()
{
	m_iCmdType = GenerateRemCutLineDlg_previewCmd;
	NotifyAllObserver(INVALID_ID, VARIANT());
}

void GenerateRemCutLineDlg::OnSaveRemMat()
{
	m_iCmdType = GenerateRemCutLineDlg_saveMatCmd;
	NotifyAllObserver(INVALID_ID, VARIANT());
}

void GenerateRemCutLineDlg::OnGenerate()
{
	m_iCmdType = GenerateRemCutLineDlg_generateCmd;
	NotifyAllObserver(INVALID_ID, VARIANT());
}

void GenerateRemCutLineDlg::OnQuit()
{
	m_iCmdType = GenerateRemCutLineDlg_cancelCmd;
	NotifyAllObserver(INVALID_ID, VARIANT());
	ShowWindow(SW_HIDE);
}

void GenerateRemCutLineDlg::Init(RECT_SIDE iRemnantPosition, double dStepWidth)
{
	m_comboRemnantPosition.ResetContent();
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GET_REM_FROM_TOP);
	m_comboRemnantPosition.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GET_REM_FROM_BOTTOM);
	m_comboRemnantPosition.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GET_REM_FROM_LEFT);
	m_comboRemnantPosition.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GET_REM_FROM_RIGHT);
	m_comboRemnantPosition.AddString(str);
	if (iRemnantPosition == RECT_TOP)
	{
		m_comboRemnantPosition.SetCurSel(0);
	}
	else if (iRemnantPosition == RECT_BOTTOM)
	{
		m_comboRemnantPosition.SetCurSel(1);
	}
	else if (iRemnantPosition == RECT_LEFT)
	{
		m_comboRemnantPosition.SetCurSel(2);
	}
	else if (iRemnantPosition == RECT_RIGHT)
	{
		m_comboRemnantPosition.SetCurSel(3);
	}

	m_ctrlStepWidth.Init(dStepWidth, 2, FALSE);
}

RECT_SIDE GenerateRemCutLineDlg::GetRemnantPosition() const
{
	RECT_SIDE iRemnantPosition;

	if (m_comboRemnantPosition.GetCurSel() == 0)
	{
		iRemnantPosition = RECT_TOP;
	}
	else if (m_comboRemnantPosition.GetCurSel() == 1)
	{
		iRemnantPosition = RECT_BOTTOM;
	}
	else if (m_comboRemnantPosition.GetCurSel() == 2)
	{
		iRemnantPosition = RECT_LEFT;
	}
	else if (m_comboRemnantPosition.GetCurSel() == 3)
	{
		iRemnantPosition = RECT_RIGHT;
	}

	return iRemnantPosition;
}

RECT_SIDE GenerateRemCutLineDlg::GetRayDir() const
{
	RECT_SIDE iRayDir;

	RECT_SIDE iRemnantPosition = GetRemnantPosition();
	if (iRemnantPosition == RECT_TOP)
	{
		iRayDir = RECT_BOTTOM;
	}
	else if (iRemnantPosition == RECT_BOTTOM)
	{
		iRayDir = RECT_TOP;
	}
	else if (iRemnantPosition == RECT_LEFT)
	{
		iRayDir = RECT_RIGHT;
	}
	else if (iRemnantPosition == RECT_RIGHT)
	{
		iRayDir = RECT_LEFT;
	}

	return iRayDir;
}

END_CUTLEADER_NAMESPACE()