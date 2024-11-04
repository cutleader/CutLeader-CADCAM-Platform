#include "StdAfx.h"
#include "CamDataSelDlg.h"

#include "baseConst.h"
#include "ProductFolderManager.h"
#include "DataBaseManager.h"

#include "PartLoader.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(CamDataSelDlg, CDialogEx)

CamDataSelDlg::CamDataSelDlg()
				: CLDialog(CamDataSelDlg::IDD)
{
}

CamDataSelDlg::~CamDataSelDlg(void)
{
}

void CamDataSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_NAME, m_schemeCBX);
}

BEGIN_MESSAGE_MAP(CamDataSelDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)

END_MESSAGE_MAP()

BOOL CamDataSelDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// init the dialog
	Init();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

void CamDataSelDlg::OnDestroy()
{
	for (int i = 0; i < m_schemeCBX.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_schemeCBX.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}

	CDialogEx::OnDestroy();
}

void CamDataSelDlg::OnBnClickedOk()
{
	int index = m_schemeCBX.GetCurSel();
	if (index != -1)
	{
		m_iSelCamDataID = *((LONGLONG*)m_schemeCBX.GetItemData(index));
		OnOK();
	}
}

void CamDataSelDlg::Init()
{
	// init connection to the part db file.
	DBConnectPtr pPartConnect = DataBaseManager::ConnectPartDB(m_iPartID);

	// �õ�������ӹ����ա������Ϣ��
	vector<DataProperty> camDataList = PartLoader::GetPartCamDataProp(pPartConnect);
	for (unsigned int i = 0; i < camDataList.size(); i++)
	{
		DataProperty prop = camDataList[i];

		DataPropItem propItem = prop.GetItem(0);
		LONGLONG iCamDataID = propItem.GetValue().llVal;

		propItem = prop.GetItem(2);
		CString strConfigName = propItem.GetValue().bstrVal;

		// add the cam data info into the combo box.
		m_schemeCBX.AddString(strConfigName);
		m_schemeCBX.SetItemData(i, (DWORD_PTR)(new LONGLONG(iCamDataID)));
	}

	m_schemeCBX.SetCurSel(0);
}

END_CUTLEADER_NAMESPACE()
