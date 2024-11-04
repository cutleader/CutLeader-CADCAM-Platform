#include "stdafx.h"
#include "SheetEditParamPage.h"

#include "baseConst.h"
#include "baseConst.h"

#include "SheetEditParam.h"
#include "clOptionResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const size_t MAX_TIP_TEXT_LENGTH = 1024;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SheetEditParamPage, CResizableDialog)

SheetEditParamPage::SheetEditParamPage() : CLResizableDialog(SheetEditParamPage::IDD)
{
	m_strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SHEET_EDIT);
}

SheetEditParamPage::~SheetEditParamPage()
{
}

void SheetEditParamPage::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);

	// 步进
	DDX_Control(pDX, IDC_EDIT_MOVE_STEP, m_editMoveStep);
	DDX_Control(pDX, IDC_EDIT_ROTATE_STEP, m_editRotStep);

	// 零件距离检测。
	DDX_Check(pDX, IDC_CHECK_DETECT, m_bDetectPartDis);
	DDX_Check(pDX, IDC_CHECK_DETECT_COM, m_bDetectCommon);
	DDX_Control(pDX, IDC_EDIT_SHT_MARGIN, m_editSheetMargin);
	DDX_Control(pDX, IDC_EDIT_PART_GAP, m_editPartDis);
	DDX_Control(pDX, IDC_EDIT_STICK_DIS, m_editStickDis);
	DDX_Control(pDX, IDC_EDIT_ESCAPE_DIS, m_editEscapeDis);

	// 光顺控制。
	DDX_Check(pDX, IDC_CHECK_SLEEK, m_bSleekCtrl);
	DDX_Control(pDX, IDC_EDIT_SLEEK_RATE, m_editSleekRate);

	// 工序。
	DDX_Control(pDX, IDC_EDIT_LOOP_DETECT_DIS, m_editLoopDetectDis);
}

BEGIN_MESSAGE_MAP(SheetEditParamPage, CResizableDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL SheetEditParamPage::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	/************************************************************************/
	// layout controls.

	AddAnchor(IDC_STATIC_INFO, TOP_LEFT);
	AddAnchor(IDC_EDIT_MSG, TOP_LEFT, TOP_RIGHT);

	// 步进。
	AddAnchor(IDC_GROUP_STEP, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_MOVE_STEP, TOP_LEFT);
	AddAnchor(IDC_EDIT_MOVE_STEP, TOP_LEFT);
	AddAnchor(IDC_STATIC_ROTATE_STEP, TOP_LEFT);
	AddAnchor(IDC_EDIT_ROTATE_STEP, TOP_LEFT);

	// 零件距离检测。
	AddAnchor(IDC_GROUP_DETECT, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_CHECK_DETECT, TOP_LEFT);
	AddAnchor(IDC_CHECK_DETECT_COM, TOP_LEFT);
	AddAnchor(IDC_TXT_SHT_MARGIN, TOP_LEFT);
	AddAnchor(IDC_EDIT_SHT_MARGIN, TOP_LEFT);
	AddAnchor(IDC_TXT_PART_DIS, TOP_LEFT);
	AddAnchor(IDC_EDIT_PART_GAP, TOP_LEFT);
	AddAnchor(IDC_TXT_STICK_DIS, TOP_LEFT);
	AddAnchor(IDC_EDIT_STICK_DIS, TOP_LEFT);
	AddAnchor(IDC_TXT_ESCAPE_DIS, TOP_LEFT);
	AddAnchor(IDC_EDIT_ESCAPE_DIS, TOP_LEFT);

	// 光顺控制。
	AddAnchor(IDC_GROUP_SLEEK, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_CHECK_SLEEK, TOP_LEFT);
	AddAnchor(IDC_TXT_SLEEK_RATE, TOP_LEFT);
	AddAnchor(IDC_EDIT_SLEEK_RATE, TOP_LEFT);

	// 工序。
	AddAnchor(IDC_GROUP_SEQUENCE, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_TXT_LOOP_DETECT_DIS, TOP_LEFT);
	AddAnchor(IDC_EDIT_LOOP_DETECT_DIS, TOP_LEFT);
	/************************************************************************/

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BOOL SheetEditParamPage::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH SheetEditParamPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	// here, message edit box will display the bk color same as the dlg.
	if (_tcsicmp(classname, _T("EDIT")) == 0 && pWnd->GetDlgCtrlID() != IDC_EDIT_MSG)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("LISTBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, WC_TREEVIEW) == 0)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void SheetEditParamPage::DisplaySheetEditParam(SheetEditParamPtr displayInfoPtr)
{
	m_pSheetEditParam = displayInfoPtr;

	m_editMoveStep.Init(m_pSheetEditParam->GetMoveStep(), DIGITAL_NUM, FALSE);
	m_editRotStep.Init(m_pSheetEditParam->GetRotateStep(), DIGITAL_NUM, FALSE);

	m_bDetectPartDis = m_pSheetEditParam->EnableDisDetect();
	m_bDetectCommon = m_pSheetEditParam->EnableComDetect();
	m_editSheetMargin.Init(m_pSheetEditParam->GetSheetMargin(), DIGITAL_NUM, FALSE);
	m_editPartDis.Init(m_pSheetEditParam->GetPartDis(), DIGITAL_NUM, FALSE);
	m_editStickDis.Init(m_pSheetEditParam->GetStickDis(), DIGITAL_NUM, FALSE);
	m_editEscapeDis.Init(m_pSheetEditParam->GetEscapeDis(), DIGITAL_NUM, FALSE);

	m_bSleekCtrl = m_pSheetEditParam->EnableSleekCtrl();
	m_editSleekRate.Init(m_pSheetEditParam->GetSleekRate(), DIGITAL_NUM, FALSE);

	m_editLoopDetectDis.Init(m_pSheetEditParam->GetLoopDetectDis(), 0, FALSE);

	UpdateData(FALSE);
}

SheetEditParamPtr SheetEditParamPage::GetSheetEditParam()
{
	UpdateData(TRUE);

	if (m_editMoveStep.Get_Number() != m_pSheetEditParam->GetMoveStep())
	{
		m_pSheetEditParam->SetMoveStep(m_editMoveStep.Get_Number());
		m_pSheetEditParam->SetModified(TRUE);
	}
	if (m_editRotStep.Get_Number() != m_pSheetEditParam->GetRotateStep())
	{
		m_pSheetEditParam->SetRotateStep(m_editRotStep.Get_Number());
		m_pSheetEditParam->SetModified(TRUE);
	}

	if (m_bDetectPartDis != m_pSheetEditParam->EnableDisDetect())
	{
		m_pSheetEditParam->EnableDisDetect(m_bDetectPartDis);
		m_pSheetEditParam->SetModified(TRUE);
	}
	if (m_bDetectCommon != m_pSheetEditParam->EnableComDetect())
	{
		m_pSheetEditParam->EnableComDetect(m_bDetectCommon);
		m_pSheetEditParam->SetModified(TRUE);
	}
	if (m_editSheetMargin.Get_Number() != m_pSheetEditParam->GetSheetMargin())
	{
		m_pSheetEditParam->SetSheetMargin(m_editSheetMargin.Get_Number());
		m_pSheetEditParam->SetModified(TRUE);
	}
	if (m_editPartDis.Get_Number() != m_pSheetEditParam->GetPartDis())
	{
		m_pSheetEditParam->SetPartDis(m_editPartDis.Get_Number());
		m_pSheetEditParam->SetModified(TRUE);
	}
	if (m_editStickDis.Get_Number() != m_pSheetEditParam->GetStickDis())
	{
		m_pSheetEditParam->SetStickDis(m_editStickDis.Get_Number());
		m_pSheetEditParam->SetModified(TRUE);
	}
	if (m_editEscapeDis.Get_Number() != m_pSheetEditParam->GetEscapeDis())
	{
		m_pSheetEditParam->SetEscapeDis(m_editEscapeDis.Get_Number());
		m_pSheetEditParam->SetModified(TRUE);
	}

	if (m_bSleekCtrl != m_pSheetEditParam->EnableSleekCtrl())
	{
		m_pSheetEditParam->EnableSleekCtrl(m_bSleekCtrl);
		m_pSheetEditParam->SetModified(TRUE);
	}
	if (m_editSleekRate.Get_Number() != m_pSheetEditParam->GetSleekRate())
	{
		m_pSheetEditParam->SetSleekRate(m_editSleekRate.Get_Number());
		m_pSheetEditParam->SetModified(TRUE);
	}

	if (m_editLoopDetectDis.Get_Number() != m_pSheetEditParam->GetLoopDetectDis())
	{
		m_pSheetEditParam->SetLoopDetectDis(m_editLoopDetectDis.Get_Number());
		m_pSheetEditParam->SetModified(TRUE);
	}

	return m_pSheetEditParam;
}

END_CUTLEADER_NAMESPACE()
