#include "StdAfx.h"
#include "ParamConfigSelectDlg.h"

#include "baseConst.h"
#include "IntegerList.h"
#include "DataBaseManager.h"

#include "MachineItemLoader.h"
#include "ExpertLibItemLoader.h"
#include "ParamConfigLoader.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(ParamConfigSelectDlg, CDialogEx)

ParamConfigSelectDlg::ParamConfigSelectDlg()
				: CLDialog(ParamConfigSelectDlg::IDD)
{
}

ParamConfigSelectDlg::~ParamConfigSelectDlg(void)
{
}

void ParamConfigSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_EXPLIB_ITEM, m_expLibItemCBX);
	DDX_Control(pDX, IDC_COMBO_PARAM_CONFIG, m_paramConfigCBX);
}

BEGIN_MESSAGE_MAP(ParamConfigSelectDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_EXPLIB_ITEM, OnSelchangeExpLibItem)
END_MESSAGE_MAP()

BOOL ParamConfigSelectDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// init the dialog
	Init();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void ParamConfigSelectDlg::OnSelchangeExpLibItem()
{
	int iIndex = m_expLibItemCBX.GetCurSel();
	if (iIndex != -1)
	{
		LONGLONG iExpLibItemID = *((LONGLONG*)m_expLibItemCBX.GetItemData(iIndex));
		InitConfigCBX(iExpLibItemID);
	}
}

void ParamConfigSelectDlg::SetCurData(LONGLONG iExpLibItemID, LONGLONG iCurConfigID) 
{ 
	m_iExpLibItemID = iExpLibItemID;
	m_iCurConfigID = iCurConfigID;
}

void ParamConfigSelectDlg::OnBnClickedOk()
{
	DataProperty prop;
	DataPropItem propItem;

	// get current mac ID
	int iIndex = m_expLibItemCBX.GetCurSel();
	if (iIndex != -1)
	{
		m_iExpLibItemID = *((LONGLONG*)m_expLibItemCBX.GetItemData(iIndex));
	}

	// ѡ�еġ��������á���
	iIndex = m_paramConfigCBX.GetCurSel();
	if (iIndex != -1)
	{
		m_iCurConfigID = *((LONGLONG*)m_paramConfigCBX.GetItemData(iIndex));
	}

	OnOK();
}

void ParamConfigSelectDlg::InitConfigCBX(LONGLONG iExpLibItemID)
{
	m_paramConfigCBX.ResetContent();

	// ר�ҿ������еġ��������á���
	DBConnectPtr pDBConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	vector<DataProperty> paramConfigList = ParamConfigLoader::GetParamConfigList(pDBConnect, iExpLibItemID);

	int iCurIdx = 0;
	for (unsigned int i = 0; i < paramConfigList.size(); i++)
	{
		DataProperty prop = paramConfigList[i];

		m_paramConfigCBX.AddString((LPCTSTR)prop.GetItem(2).GetValue().bstrVal);
		LONGLONG iParamConfigID = prop.GetItem(0).GetValue().llVal;
		m_paramConfigCBX.SetItemData(i, (DWORD_PTR)(new LONGLONG(iParamConfigID)));
		if (iParamConfigID == m_iCurConfigID)
			iCurIdx = i;
	}
	m_paramConfigCBX.SetCurSel(iCurIdx);
}

void ParamConfigSelectDlg::Init()
{
	// ���е�ר�ҿ���Ϣ��
	DBConnectPtr pDBConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	vector<DataProperty> expLibItemList = ExpertLibItemLoader::GetExpLibItemInfoList(pDBConnect);

	// �ɼ��Ļ�����
	DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	IntegerListPtr pVisibleMacList = MachineItemLoader::GetVisibleMacList(pMacLibDBConnect);

	// ����ÿ��ר�ҿ��
	int iCurIdx = 0;
	for (unsigned int i = 0; i < expLibItemList.size(); i++)
	{
		DataProperty prop = expLibItemList[i];

		// ����Ӧ�Ļ����Ƿ�ɼ���
		if (!pVisibleMacList->Include(prop.GetItem(3).GetValue().llVal))
			continue;

		m_expLibItemCBX.AddString((LPCTSTR)prop.GetItem(1).GetValue().bstrVal);
		LONGLONG iExpLibItemID = prop.GetItem(0).GetValue().llVal;
		m_expLibItemCBX.SetItemData(m_expLibItemCBX.GetCount()-1, (DWORD_PTR)(new LONGLONG(iExpLibItemID)));
		if (iExpLibItemID == m_iExpLibItemID)
			iCurIdx = m_expLibItemCBX.GetCount()-1;
	}
	m_expLibItemCBX.SetCurSel(iCurIdx);

	// init the scheme combo box.
	LONGLONG iExpLibItemID = *((LONGLONG*)m_expLibItemCBX.GetItemData(iCurIdx));
	InitConfigCBX(iExpLibItemID);
}

END_CUTLEADER_NAMESPACE()
