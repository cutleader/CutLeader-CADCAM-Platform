#include "StdAfx.h"
#include "DefaultMaterialSelectDlg.h"

#include "baseConst.h"
#include "DataBaseManager.h"

#include "IMaterialSize.h"
#include "MaterialLibLoader.h"
#include "MaterialSizeSelectDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(DefaultMaterialSelectDlg, CDialogEx)

DefaultMaterialSelectDlg::DefaultMaterialSelectDlg()
				: CLDialog(DefaultMaterialSelectDlg::IDD)
{
}

DefaultMaterialSelectDlg::~DefaultMaterialSelectDlg(void)
{
}

void DefaultMaterialSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MAT_TYPE, m_strMat);
	DDX_Text(pDX, IDC_EDIT_MAT_NAME, m_strMatThick);
	DDX_Text(pDX, IDC_EDIT_MAT_SIZE, m_strMatSize);
}

BEGIN_MESSAGE_MAP(DefaultMaterialSelectDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SEL_MAT, OnSelectMat)
END_MESSAGE_MAP()

BOOL DefaultMaterialSelectDlg::OnInitDialog()
{
	__super::OnInitDialog();

	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();

	// ²ÄÁÏ³ß´çµÄÊôÐÔ¡£
	DataProperty dataProp;
	MaterialLibLoader::GetMatSizeProp(pMatLibDBCon, m_iDflMatSizeID, dataProp);
	m_strMat = dataProp.GetItem(0).GetValue().bstrVal;
	m_strMatThick.Format(FLOAT_FORMAT_4, dataProp.GetItem(1).GetValue().dblVal);
	m_strMatSize = dataProp.GetItem(6).GetValue().bstrVal;
	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void DefaultMaterialSelectDlg::OnSelectMat()
{
	MaterialSizeSelectDlg matSizeSelDlg;
	if (matSizeSelDlg.DoModal() == IDOK)
	{
		IMaterialSizePtr pMaterialSize = matSizeSelDlg.GetSelMatSize();
		m_iDflMatSizeID = pMaterialSize->GetID();

		DataProperty dataProp;
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		MaterialLibLoader::GetMatSizeProp(pMatLibDBCon, m_iDflMatSizeID, dataProp);
		m_strMat = dataProp.GetItem(0).GetValue().bstrVal;
		m_strMatThick.Format(FLOAT_FORMAT_4, dataProp.GetItem(1).GetValue().dblVal);
		m_strMatSize = dataProp.GetItem(6).GetValue().bstrVal;

		UpdateData(FALSE);
	}
}

void DefaultMaterialSelectDlg::OnBnClickedOk()
{
	OnOK();
}

END_CUTLEADER_NAMESPACE()
