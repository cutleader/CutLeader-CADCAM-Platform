#include "stdafx.h"
#include "NewExtPropDlg.h"

#include "baseConst.h"
#include "clUILibResource.h"
#include "clBaseDataResource.h"
#include "ExtPropType.h"
#include "ExtPropTypeList.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(NewExtPropDlg, CDialogEx)

NewExtPropDlg::NewExtPropDlg()
						 : CLDialog(NewExtPropDlg::IDD)
{
	m_strName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NEW_EXT_PROP);
}

NewExtPropDlg::~NewExtPropDlg()
{
}

void NewExtPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_PROP_NAME, m_strName);
	DDX_Control(pDX, IDC_COMBO_DATA_TYPE, m_ctrlDataType);
}

BEGIN_MESSAGE_MAP(NewExtPropDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL NewExtPropDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// ��ʼ���������͡�
	StringListPtr pDataTypes = ExtPropType::GetAllDataType();
	for (unsigned int i = 0; i < pDataTypes->size(); i++)
		m_ctrlDataType.InsertString(i, pDataTypes->at(i));
	m_ctrlDataType.SetCurSel(0);

	UpdateData(FALSE);
	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

void NewExtPropDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	// �ȼ������ͬ������չ���ԡ�
	if (m_pExtPropTypes->GetItemByName(m_strName))
	{
		CString strProductName, str;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXT_PROP_SAME_NAME);
		MessageBox(str, strProductName, MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		m_pExtPropType.reset(new ExtPropType(m_strName, (BASIC_DATA_TYPE)m_ctrlDataType.GetCurSel()));
		OnOK();
	}
}

END_CUTLEADER_NAMESPACE()