#include "stdafx.h"
#include "FolderInfoDlg.h"

#include "baseConst.h"
#include "clBaseDataResource.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(FolderInfoDlg, CDialog)

FolderInfoDlg::FolderInfoDlg(CWnd* pParent /*=NULL*/)
								: CLDialog(FolderInfoDlg::IDD, pParent)
{
	m_strName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NEW_FOLDER);
}

FolderInfoDlg::~FolderInfoDlg()
{
}

BOOL FolderInfoDlg::OnInitDialog()
{
	__super::OnInitDialog();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

void FolderInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_FOLDER_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_FOLDER_COMMENT, m_strComment);
}

void FolderInfoDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	OnOK();
}

BEGIN_MESSAGE_MAP(FolderInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)

END_MESSAGE_MAP()

void FolderInfoDlg::GetMatNameInfo(CString& strName, CString& strComment)
{
	strName = m_strName;
	strComment = m_strComment;
}

END_CUTLEADER_NAMESPACE()
