#include "StdAfx.h"
#include "AboutDlg.h"

#include "baseConst.h"
#include "clUtilityResource.h"
#include "DBConnect.h"
#include "versionInfo.h"

#include "clBaseDataResource.h"
#include "LicenseManager.h"

BEGIN_CUTLEADER_NAMESPACE()

AboutDlg::AboutDlg() : CLDialog(AboutDlg::IDD)
{
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTN_PRODUCT_NAME, m_btnProductName);

	DDX_Text(pDX, IDC_EDIT_PRODUCT_ID, m_strProductID);
	DDX_Text(pDX, IDC_EDIT_CUSTOMER_ID, m_strCustomerID);
	DDX_Text(pDX, IDC_EDIT_START_DATE, m_strStartDate);
	DDX_Text(pDX, IDC_EDIT_END_DATE, m_strEndDate);

	DDX_Control(pDX, IDC_STATIC_APP_VER, m_appVersion);
	DDX_Control(pDX, IDC_STATIC_DB_VER, m_dbVersion);
	DDX_Control(pDX, IDC_STATIC_PART_VER, m_partVersion);
	DDX_Control(pDX, IDC_STATIC_SHT_VER, m_shtVersion);
	DDX_Control(pDX, IDC_STATIC_DRI_VER, m_driVersion);
	DDX_Control(pDX, IDC_STATIC_EXPLIB_VER, m_expLibVersion);
	DDX_Control(pDX, IDC_STATIC_MATLIB_VER, m_matLibVersion);
	DDX_Control(pDX, IDC_STATIC_RPTLIB_VER, m_rptLibVersion);

	DDX_Control(pDX, IDC_STATIC_WEBSITE, m_websiteLink);
}

BEGIN_MESSAGE_MAP(AboutDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL AboutDlg::OnInitDialog()
{
	__super::OnInitDialog();


    // the license info.
    int iProductID = -1, iCustomerID = -1;
    CTime startTime, endTime;
    LicenseCheckResultType iLicenseCheckResult = LicenseManager::CheckLicense_2(iProductID, iCustomerID, startTime, endTime);
	if (iLicenseCheckResult == LicenseCheckResult_stdEdition)
    {
		m_btnProductName.LoadBitmaps(IDB_PRODUCT_NAME_STD);
    }
	else if (iLicenseCheckResult == LicenseCheckResult_professionalEdition)
    {
		m_btnProductName.LoadBitmaps(IDB_PRODUCT_NAME_PROF);
    }
	else if (iLicenseCheckResult == LicenseCheckResult_ultimateEdition)
    {
		m_btnProductName.LoadBitmaps(IDB_PRODUCT_NAME_ULTI);
    }
    else if (iLicenseCheckResult == LicenseCheckResult_noLicense)
    {
        m_btnProductName.LoadBitmaps(IDB_PRODUCT_NAME_TRIAL);
    }
	m_strProductID.Format(_T("%d"), iProductID);
	m_strCustomerID.Format(_T("%d"), iCustomerID);
	m_strStartDate.Format(_T("%d-%d-%d"), startTime.GetYear(), startTime.GetMonth(), startTime.GetDay());
	m_strEndDate.Format(_T("%d-%d-%d"), endTime.GetYear(), endTime.GetMonth(), endTime.GetDay());


	// app version.
	CString strAppVer;
	strAppVer = MultiLanguageMgr::GetInstance()->TranslateString(IDS_APP_VERSION);
	strAppVer.Replace(_T("WILLBEREPLACED1"), _T("<font size=\"+5\">"));
	strAppVer.Replace(_T("WILLBEREPLACED2"), APP_VERSION);
	m_appVersion.SetWindowText(strAppVer);

	// database version.
	CString strDBVer;
	strDBVer = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DB_VERSION);
	strDBVer.Replace(_T("WILLBEREPLACED1"), DB_VERSION);
	m_dbVersion.SetWindowText(strDBVer);

	// 零件版本。
	CString strPartVer;
	strPartVer = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_VERSION);
	strPartVer.Replace(_T("WILLBEREPLACED1"), PART_VERSION);
	m_partVersion.SetWindowText(strPartVer);

	// 板材版本。
	CString strShtVer;
	strShtVer = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_VERSION);
	strShtVer.Replace(_T("WILLBEREPLACED1"), SHEET_VERSION);
	m_shtVersion.SetWindowText(strShtVer);

	// driver version.
	CString strDriVer;
	strDriVer = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DRI_VERSION);
	strDriVer.Replace(_T("WILLBEREPLACED1"), DRIVER_VERSION);
	m_driVersion.SetWindowText(strDriVer);

	// 专家库版本。
	CString strExpLibVer;
	strExpLibVer = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXPLIB_VERSION);
	strExpLibVer.Replace(_T("WILLBEREPLACED1"), EXPLIB_VERSION);
	m_expLibVersion.SetWindowText(strExpLibVer);

	// 材料库版本。
	CString strMatLibVer;
	strMatLibVer = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATLIB_VERSION);
	strMatLibVer.Replace(_T("WILLBEREPLACED1"), MATLIB_VERSION);
	m_matLibVersion.SetWindowText(strMatLibVer);

	// 报表库版本。
	CString strRptLibVer;
	strRptLibVer = MultiLanguageMgr::GetInstance()->TranslateString(IDS_RPTLIB_VERSION);
	strRptLibVer.Replace(_T("WILLBEREPLACED1"), RPTLIB_VERSION);
	m_rptLibVersion.SetWindowText(strRptLibVer);

	// the web site link.
	CString strWebSite = PRODUCT_WEBSITE;
	m_websiteLink.SetWindowText(strWebSite);
	m_websiteLink.SetURL(strWebSite);
	m_websiteLink.SetUnderline(XHyperLink::ulAlways);


	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BOOL AboutDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH AboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
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

END_CUTLEADER_NAMESPACE()
