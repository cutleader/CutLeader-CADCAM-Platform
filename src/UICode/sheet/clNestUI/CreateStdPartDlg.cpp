#include "StdAfx.h"
#include "CreateStdPartDlg.h"

#include "baseConst.h"
#include "DataBaseManager.h"

#include "ProductFolderDlg.h"
#include "ProductItemLoader.h"
#include "FolderItem.h"

#include "clPartResource.h"
#include "Part.h"
#include "PartBuilder.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(CreateStdPartDlg, CDialogEx)

CreateStdPartDlg::CreateStdPartDlg(CWnd* pParent)
	: CLDialog(CreateStdPartDlg::IDD, pParent)
{
	m_strPartName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NEW_PART);
}

CreateStdPartDlg::~CreateStdPartDlg(void)
{
}

BEGIN_MESSAGE_MAP(CreateStdPartDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SAVE_PATH, OnSelectFolder)

	ON_BN_CLICKED(IDC_RADIO_RECT, OnRectPart)
	ON_BN_CLICKED(IDC_RADIO_ROUND, OnRoundPart)

	ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()

void CreateStdPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_PART_NAME, m_strPartName);
	DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_ctrlSavePath);
	DDX_Control(pDX, IDC_EDIT_PART_COUNT, m_ctrlPartNum);

	DDX_Radio(pDX, IDC_RADIO_RECT, m_iPartShape);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_ctrlWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_ctrlHeight);
	DDX_Control(pDX, IDC_EDIT_RAD, m_ctrlRad);
}

BOOL CreateStdPartDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// 初始化保存目录。
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	m_pSaveFolder = ProductItemLoader::LoadProductTree(pMainDBCon);
	m_ctrlSavePath = m_pSaveFolder->GetName();

	// 零件数量。
	m_ctrlPartNum.Init(10, 0, false);

	// 零件形状。
	m_iPartShape = 0;
	m_ctrlWidth.Init(50.0, DIGITAL_NUM, false);
	m_ctrlHeight.Init(50.0, DIGITAL_NUM, false);
	m_ctrlRad.Init(50.0, DIGITAL_NUM, false);
	((CWnd*)GetDlgItem(IDC_EDIT_RAD))->EnableWindow(FALSE);

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void CreateStdPartDlg::OnSelectFolder()
{
	ProductFolderDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		m_pSaveFolder = dlg.GetSelectFolder();

		// get the path of the folder.
		CString strPath = m_pSaveFolder->GetItemPath();
		strPath += _T("\\");
		strPath += m_pSaveFolder->GetName();

		m_ctrlSavePath = strPath;
		UpdateData(FALSE);
	}
}

void CreateStdPartDlg::OnRectPart()
{
	UpdateData(TRUE);

	if (m_iPartShape == 0)
	{
		((CWnd*)GetDlgItem(IDC_EDIT_WIDTH))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_EDIT_HEIGHT))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_EDIT_RAD))->EnableWindow(FALSE);
	}
}

void CreateStdPartDlg::OnRoundPart()
{
	UpdateData(TRUE);

	if (m_iPartShape == 1)
	{
		((CWnd*)GetDlgItem(IDC_EDIT_WIDTH))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_EDIT_HEIGHT))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_EDIT_RAD))->EnableWindow(TRUE);
	}
}

void CreateStdPartDlg::OnOK()
{
	UpdateData(TRUE);

	// 创建的零件对象。
	if (m_iPartShape == 0)
	{
		m_pNewPart = PartBuilder::BuildRectPart(m_strPartName, m_ctrlWidth.Get_Number(), m_ctrlHeight.Get_Number(), m_iParamConfigID);
	}
	else if (m_iPartShape == 1)
	{
		m_pNewPart = PartBuilder::BuildRoundPart(m_strPartName, m_ctrlRad.Get_Number(), m_iParamConfigID);
	}

	// 零件数量。
	m_iPartNum = (int)m_ctrlPartNum.Get_Number();

	CDialogEx::OnOK();
}

END_CUTLEADER_NAMESPACE()
