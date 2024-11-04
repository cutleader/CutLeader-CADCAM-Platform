#include "stdafx.h"
#include "SearchPartDlg.h"

#include "baseConst.h"
#include "FolderItem.h"
#include "ProductItemList.h"
#include "ProductFolderDlg.h"
#include "ProductStructProcessor.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(SearchPartDlg, CDialog)

BEGIN_MESSAGE_MAP(SearchPartDlg, CDialogEx)
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_CHECK_FOLDER, OnCheckFolderFilter)
	ON_BN_CLICKED(IDC_BTN_FOLDER, OnSelectFilterFolder)
	ON_BN_CLICKED(IDC_CHECK_NAME, OnCheckNameFilter)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

SearchPartDlg::SearchPartDlg(CWnd* pParent /*=NULL*/)
	: CLDialog(SearchPartDlg::IDD, pParent)
{
	m_bFolderFilter = FALSE;
	m_bNameFilter = FALSE;
}

SearchPartDlg::~SearchPartDlg()
{
}

void SearchPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_FOLDER, m_bFolderFilter);
	DDX_Check(pDX, IDC_CHECK_NAME, m_bNameFilter);
	DDX_Text(pDX, IDC_EDIT_PART_NAME, m_strNameFilter);
}

BOOL SearchPartDlg::OnInitDialog()
{
	__super::OnInitDialog();

	((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->SetWindowText(m_pFilterFolder->GetFullPath());
	((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->EnableWindow(FALSE);
	((CWnd*)GetDlgItem(IDC_EDIT_PART_NAME))->EnableWindow(FALSE);

	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

HBRUSH SearchPartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void SearchPartDlg::OnCheckFolderFilter()
{
	UpdateData(TRUE);

	// init the folder filter.
	if (m_bFolderFilter)
		((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->EnableWindow(TRUE);
	else
		((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->EnableWindow(FALSE);
}

void SearchPartDlg::OnCheckNameFilter()
{
	UpdateData(TRUE);

	// init the part name filter.
	if (m_bNameFilter)
		((CWnd*)GetDlgItem(IDC_EDIT_PART_NAME))->EnableWindow(TRUE);
	else
		((CWnd*)GetDlgItem(IDC_EDIT_PART_NAME))->EnableWindow(FALSE);
}

void SearchPartDlg::OnSelectFilterFolder()
{
	ProductFolderDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_pFilterFolder = dlg.GetSelectFolder();

		// get the path of the folder.
		CString strPath = m_pFilterFolder->GetItemPath();
		strPath += _T("\\");
		strPath += m_pFilterFolder->GetName();

		// put the folder path to the button.
		((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->SetWindowText(strPath);
	}
}

void SearchPartDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	// name filter
	m_strNameFilter.Trim();
	if (m_bNameFilter && m_strNameFilter.GetLength() < 1)
		m_bNameFilter = FALSE;

	// search parts using the conditions.
	m_pProductItemList = ProductStructProcessor::SearchDataItem(m_pFilterFolder, TRUE, DATANODE_PART, m_bNameFilter, m_strNameFilter);

	OnOK();
}

END_CUTLEADER_NAMESPACE()
