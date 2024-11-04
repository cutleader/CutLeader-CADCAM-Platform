#include "StdAfx.h"
#include "CamDataAddDlg.h"

#include "baseConst.h"
#include "IntegerList.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"

#include "clPartResource.h"
#include "PartManager.h"

#include "MachineItemLoader.h"
#include "clExpertLibResource.h"
#include "ExpertLibItemLoader.h"
#include "ParamConfigLoader.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(CamDataAddDlg, CDialogEx)

CamDataAddDlg::CamDataAddDlg()
				: CLDialog(CamDataAddDlg::IDD)
{
	m_iPartID = INVALID_ID;
	m_strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SCHEME_GEN_CAMDATA);
}

CamDataAddDlg::~CamDataAddDlg(void)
{
}

void CamDataAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);
	DDX_Control(pDX, IDC_COMBO_EXPLIB_ITEM, m_expLibItemCBX);
	DDX_Control(pDX, IDC_COMBO_CONFIG, m_partSchemeCBX);
}

BEGIN_MESSAGE_MAP(CamDataAddDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_EXPLIB_ITEM, OnSelchangeExpLibItem)

	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CamDataAddDlg::OnDestroy()
{
	RlsExpLibItemData();
	RlsPartSchemeItemData();
}

BOOL CamDataAddDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH CamDataAddDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CamDataAddDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// set style
	m_expLibItemCBX.SetColumnCount(1);
	m_partSchemeCBX.SetColumnCount(1);

	// init the dialog
	Init();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void CamDataAddDlg::OnSelchangeExpLibItem()
{
	int index = m_expLibItemCBX.GetCurSel();
	if (index != -1)
	{
		InitParamConfigCBX();
	}
}

void CamDataAddDlg::OnBnClickedOk()
{
	CString strProductName, str;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// �õ�ѡ�еġ�ר�ҿ����
	int iIndex = m_expLibItemCBX.GetCurSel();
	if (iIndex == -1)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SEL_EXPLIB_ITEM);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}
	else
	{
		LONGLONG* pData = (LONGLONG*)m_expLibItemCBX.GetItemData(iIndex);
		m_iExpLibItemID = *pData;
	}

	// �õ�ѡ�еġ��������á���
	iIndex = m_partSchemeCBX.GetCurSel();
	if (iIndex == -1)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SEL_PARAM_CONFIG);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}
	else
	{
		LONGLONG* pData = (LONGLONG*)m_partSchemeCBX.GetItemData(iIndex);
		m_iParamConfigID = *pData;
	}

	// ��������ĳ�ס��������ݡ��Ƿ��Ѿ�����������������á���
	LONGLONG iTmpCamDataID, iTmpExpLibItemID;
	if (PartManager::HasCamDatasetA(m_iPartID, m_iParamConfigID, iTmpCamDataID, iTmpExpLibItemID))
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_CONFIG_PARAM_USED);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	OnOK();
}

void CamDataAddDlg::InitParamConfigCBX()
{
	// at first, clear the combo box.
	m_partSchemeCBX.ResetContent();
	RlsPartSchemeItemData();
	m_paramConfigList.clear();

	// ѡ�еġ�ר�ҿ����
	int iIndex = m_expLibItemCBX.GetCurSel();
	LONGLONG* pExpLibItemID = (LONGLONG*)m_expLibItemCBX.GetItemData(iIndex);

	// �õ���ר�ҿ�������С��������á���Ϣ��
	DBConnectPtr pDBConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	m_paramConfigList = ParamConfigLoader::GetParamConfigList(pDBConnect, *pExpLibItemID);
	for (unsigned int i = 0; i < m_paramConfigList.size(); i++)
	{
		DataProperty prop = m_paramConfigList[i];

		// the ������������.
		DataPropItem propItem = prop.GetItem(2);
		VARIANT v = propItem.GetValue();
		m_partSchemeCBX.AddString((LPCTSTR)v.bstrVal);

		// set the item data.
		LONGLONG iParamConfigID = prop.GetItem(0).GetValue().llVal;
		m_partSchemeCBX.SetItemData(i, (DWORD_PTR)(new LONGLONG(iParamConfigID)));

		// whether set it to red color.
		LONGLONG iCamDataID, iExpLibItemID;
		if (PartManager::HasCamDatasetA(m_iPartID, iParamConfigID, iCamDataID, iExpLibItemID))
			m_partSchemeCBX.SetItemColor(i, RGB(255,0,0));
	}
}

void CamDataAddDlg::Init()
{
	DBConnectPtr pDBConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	m_expLibItemList = ExpertLibItemLoader::GetExpLibItemInfoList(pDBConnect);

	// �ɼ��Ļ�����
	DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	IntegerListPtr pVisibleMacList = MachineItemLoader::GetVisibleMacList(pMacLibDBConnect);

	// ��ʾ���С�ר�ҿ����
	for (unsigned int i = 0; i < m_expLibItemList.size(); i++)
	{
		DataProperty prop = m_expLibItemList[i];

		// ����Ӧ�Ļ����Ƿ�ɼ���
		if (!pVisibleMacList->Include(prop.GetItem(3).GetValue().llVal))
			continue;

		m_expLibItemCBX.AddString((LPCTSTR)prop.GetItem(1).GetValue().bstrVal);
		LONGLONG iExpLibItemID = prop.GetItem(0).GetValue().llVal;
		m_expLibItemCBX.SetItemData(m_expLibItemCBX.GetCount()-1, (DWORD_PTR)(new LONGLONG(iExpLibItemID)));
	}
}

void CamDataAddDlg::RlsExpLibItemData()
{
	for (int i = 0; i < m_expLibItemCBX.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_expLibItemCBX.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void CamDataAddDlg::RlsPartSchemeItemData()
{
	for (int i = 0; i < m_partSchemeCBX.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_partSchemeCBX.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

END_CUTLEADER_NAMESPACE()
