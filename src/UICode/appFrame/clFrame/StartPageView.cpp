#include "stdafx.h"
#include "StartPageView.h"

#include "baseConst.h"
#include "CLOptionInfo.h"
#include "ProductParam.h"
#include "MultiLanguageMgr.h"
#include "clFrameResource.h"
#include "LicenseManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(StartPageView, CResizableFormView)

BEGIN_MESSAGE_MAP(StartPageView, CResizableFormView)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_BTN_GUIDE_MODE_1, OnGuideMode1)
	ON_BN_CLICKED(IDC_BTN_GUIDE_MODE_2, OnGuideMode2)
END_MESSAGE_MAP()

StartPageView::StartPageView()
	: CResizableFormView(StartPageView::IDD)
{
	m_buttonFont.CreatePointFont(140, _T("Arial"));
}

StartPageView::~StartPageView()
{
	m_buttonFont.DeleteObject();
}

void StartPageView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTN_GUIDE_MODE_1, m_ctrlMode1Btn);
	DDX_Control(pDX, IDC_BTN_GUIDE_MODE_2, m_ctrlMode2Btn);
}

void StartPageView::OnInitialUpdate()
{
	CResizableFormView::OnInitialUpdate();
	ResizeParentToFit();
}

BOOL StartPageView::OnInitDialog() 
{
	CResizableFormView::OnInitDialog();

	// 翻译
	MultiLanguageMgr::GetInstance()->TranslateDialog(this);

	// 设置按钮字体。
	GetDlgItem(IDC_BTN_GUIDE_MODE_1)->SetFont(&m_buttonFont);
	GetDlgItem(IDC_BTN_GUIDE_MODE_2)->SetFont(&m_buttonFont);

	// 控件布局。
	AddAnchor(IDC_STATIC_GUIDE_MODE, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_BTN_GUIDE_MODE_1, TOP_CENTER);
	AddAnchor(IDC_PIC_CREATE_SHEET_1_ENG, TOP_LEFT);
	AddAnchor(IDC_PIC_CREATE_SHEET_1_CN, TOP_LEFT);
	AddAnchor(IDC_BTN_GUIDE_MODE_2, TOP_CENTER);
	AddAnchor(IDC_PIC_CREATE_SHEET_2_ENG, TOP_LEFT);
	AddAnchor(IDC_PIC_CREATE_SHEET_2_CN, TOP_LEFT);

	// 控制图片的显示
	if (CLOptionInfo::GetInstance()->GetProductParam()->GetLang() == LANG_CHS)
	{
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_1_ENG))->ShowWindow(SW_HIDE);
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_2_ENG))->ShowWindow(SW_HIDE);
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_1_CN))->ShowWindow(SW_SHOW);
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_2_CN))->ShowWindow(SW_SHOW);
	}
	else
	{
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_1_ENG))->ShowWindow(SW_SHOW);
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_2_ENG))->ShowWindow(SW_SHOW);
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_1_CN))->ShowWindow(SW_HIDE);
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_2_CN))->ShowWindow(SW_HIDE);
	}

	// 检查许可证, don't disable this because the core dlls also did such check.
    LicenseCheckResultType iLicenseCheckResult = LicenseManager::CheckLicense();
	if (iLicenseCheckResult == LicenseCheckResult_professionalEdition || iLicenseCheckResult == LicenseCheckResult_ultimateEdition ||
        iLicenseCheckResult == LicenseCheckResult_noLicense)
	{
		((CWnd*)GetDlgItem(IDC_BTN_GUIDE_MODE_2))->ShowWindow(SW_SHOW);
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_2_ENG))->ShowWindow(SW_SHOW);
		((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_2_CN))->ShowWindow(SW_SHOW);
	}
    else
    {
        ((CWnd*)GetDlgItem(IDC_BTN_GUIDE_MODE_2))->ShowWindow(SW_HIDE);
        ((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_2_ENG))->ShowWindow(SW_HIDE);
        ((CWnd*)GetDlgItem(IDC_PIC_CREATE_SHEET_2_CN))->ShowWindow(SW_HIDE);
    }

	return TRUE;
}

BOOL StartPageView::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH StartPageView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

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

void StartPageView::OnGuideMode1()
{
	AfxGetApp()->PostThreadMessage(WM_GUIDE_MODE_1, NULL, NULL);
}

void StartPageView::OnGuideMode2()
{
	AfxGetApp()->PostThreadMessage(WM_GUIDE_MODE_2, NULL, NULL);
}

END_CUTLEADER_NAMESPACE()
