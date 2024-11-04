#include "stdafx.h"
#include "MaterialThicknessInfoPage.h"

#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "baseConst.h"
#include "baseConst.h"
#include "clUtilityResource.h"
#include "MathEx.h"

#include "clGeometryFeatureResource.h"
#include "clMaterialLibResource.h"
#include "MaterialThickness.h"
#include "IMaterialSize.h"
#include "MaterialSizeList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

const size_t MAX_TIP_TEXT_LENGTH = 1024;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(MaterialThicknessInfoPage, CResizableDialog)

MaterialThicknessInfoPage::MaterialThicknessInfoPage() : CLResizableDialog(MaterialThicknessInfoPage::IDD)
{
}

MaterialThicknessInfoPage::~MaterialThicknessInfoPage()
{
}

void MaterialThicknessInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MATTHICK_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_MATNAME_COMMENT, m_strComment);
	DDX_Control(pDX, IDC_EDIT_THICKNESS, m_dThickness);

	DDX_Control(pDX, IDC_LIST_MATSIZE, m_ctrlMatSizeList);
	DDX_Control(pDX, IDC_EDIT_DUMMY, m_dDummy);
}

BEGIN_MESSAGE_MAP(MaterialThicknessInfoPage, CResizableDialog)
	ON_EN_KILLFOCUS(IDC_EDIT_MATTHICK_NAME, OnKillName)
	ON_EN_KILLFOCUS(IDC_EDIT_MATNAME_COMMENT, OnKillComment)
	ON_EN_KILLFOCUS(IDC_EDIT_THICKNESS, OnKillThickness)

	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MATSIZE, OnDblclkList)
END_MESSAGE_MAP()

BOOL MaterialThicknessInfoPage::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	/************************************************************************/
	// layout controls.

	// "basic info" group
	AddAnchor(IDC_STATIC_BASE_INFO, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_MATTHICK_NAME, TOP_LEFT);
	AddAnchor(IDC_EDIT_MATTHICK_NAME, TOP_LEFT);
	AddAnchor(IDC_EDIT_THICKNESS, TOP_LEFT);
	AddAnchor(IDC_STATIC_THICKNESS, TOP_LEFT);
	AddAnchor(IDC_EDIT_MATNAME_COMMENT, TOP_LEFT);
	AddAnchor(IDC_STATIC_COMMENT, TOP_LEFT);

	// "material size list" group
	AddAnchor(IDC_STATIC_MATSIZE, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_LIST_MATSIZE, TOP_LEFT, BOTTOM_RIGHT);
	/************************************************************************/

	InitGridStyle();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void MaterialThicknessInfoPage::OnDestroy()
{
	ReleaseGridItemData();

	CResizableDialog::OnDestroy();
}

void MaterialThicknessInfoPage::InitGridStyle()
{
	// Requires one never uses column 0
	m_ctrlMatSizeList.InsertHiddenLabelColumn();

	CString str;

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol1 = new CGridColumnTraitText;
	m_ctrlMatSizeList.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol1);

	// "matsize type" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_TYPE);
	CGridColumnTrait* pSkeleCol = new CGridColumnTraitText;
	m_ctrlMatSizeList.InsertColumnTrait(2, str, LVCFMT_LEFT, 90, 0, pSkeleCol);

	// �Ƿ����ϡ�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_IS_REMNANT);
	CGridColumnTrait* pRemnantCol = new CGridColumnTraitText;
	m_ctrlMatSizeList.InsertColumnTrait(3, str, LVCFMT_LEFT, 90, 0, pRemnantCol);

	// "width"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_WIDTH);
	CGridColumnTrait* pLengthCol = new CGridColumnTraitText;
	m_ctrlMatSizeList.InsertColumnTrait(4, str, LVCFMT_LEFT, 80, 0, pLengthCol);

	// "height"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_HEIGHT);
	CGridColumnTrait* pWidthCol = new CGridColumnTraitText;
	m_ctrlMatSizeList.InsertColumnTrait(5, str, LVCFMT_LEFT, 80, 0, pWidthCol);

	// "count"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MAT_TOTAL_NUM);
	CGridColumnTrait* pCountCol = new CGridColumnTraitText;
	m_ctrlMatSizeList.InsertColumnTrait(6, str, LVCFMT_LEFT, 80, 0, pCountCol);

	// "available number"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MAT_AVAILABLE_NUM);
	CGridColumnTrait* pAvailCol = new CGridColumnTraitText;
	m_ctrlMatSizeList.InsertColumnTrait(7, str, LVCFMT_LEFT, 80, 0, pAvailCol);

	m_ctrlMatSizeList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void MaterialThicknessInfoPage::DisplayMatThick(DataProperty matThickProp, vector<DataProperty> matSizeProps)
{
	m_matThickProp = matThickProp;
	m_bModified = FALSE;

	// "basic info" group.
	m_strName = matThickProp.GetItem(2).GetValue().bstrVal;
	m_dThickness.Init(matThickProp.GetItem(3).GetValue().dblVal, DIGITAL_NUM, FALSE);
	m_strComment = matThickProp.GetItem(4).GetValue().bstrVal;
	UpdateData(FALSE);

	// clear grid.
	ReleaseGridItemData();
	m_ctrlMatSizeList.DeleteAllItems();

	// ��ʼ�������ϳߴ硱�б�
	for (unsigned int i = 0; i < matSizeProps.size(); i++)
	{
		DataProperty dataProp = matSizeProps.at(i);

		// insert a new row.
		m_ctrlMatSizeList.InsertItem(i, _T(""));
		m_ctrlMatSizeList.SetItemData(i, (DWORD_PTR)(new LONGLONG(dataProp.GetItem(0).GetValue().llVal)));

		// "num" column
		CString strNum;
		strNum.Format(INT_FORMAT, i+1);
		m_ctrlMatSizeList.SetItemText(i, 1, strNum);

		// �����͡��С�
		MAT_SIZE_TYPE iType = (MAT_SIZE_TYPE)dataProp.GetItem(1).GetValue().llVal;
		CString strMatSizeType;
		if (iType == MAT_SIZE_RECT)
			strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_RECT);
		else if (iType == MAT_SIZE_POLY)
			strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_POLY);
		m_ctrlMatSizeList.SetItemText(i, 2, strMatSizeType);

		// �Ƿ�����
		BOOL bRemnant = (BOOL)dataProp.GetItem(2).GetValue().llVal;
		CString strIsRemnant;
		if (bRemnant)
			strIsRemnant = MultiLanguageMgr::GetInstance()->TranslateString(IDS_YES);
		else
			strIsRemnant = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NO);
		m_ctrlMatSizeList.SetItemText(i, 3, strIsRemnant);

		// "material width" column
		CString strMatWidth;
		strMatWidth.Format(FLOAT_FORMAT_4, dataProp.GetItem(3).GetValue().dblVal);
		m_ctrlMatSizeList.SetItemText(i, 4, strMatWidth);

		// "material height" column
		CString strMatHeight;
		strMatHeight.Format(FLOAT_FORMAT_4, dataProp.GetItem(4).GetValue().dblVal);
		m_ctrlMatSizeList.SetItemText(i, 5, strMatHeight);

		// "total" column
		CString strMatTotal;
		strMatTotal.Format(INT_FORMAT, dataProp.GetItem(5).GetValue().dblVal);
		m_ctrlMatSizeList.SetItemText(i, 6, strMatTotal);

		// "available num" column
		CString strAvailable;
		strAvailable.Format(INT_FORMAT, dataProp.GetItem(6).GetValue().dblVal);
		m_ctrlMatSizeList.SetItemText(i, 7, strAvailable);
	}

	m_dDummy.Init(0.001, DIGITAL_NUM, FALSE);
}

void MaterialThicknessInfoPage::OnKillName()
{
	UpdateData(TRUE);

	if (m_matThickProp.GetItem(2).GetValue().bstrVal != m_strName)
	{
		VARIANT v;
		v.vt = VT_BSTR;
		v.bstrVal = m_strName.AllocSysString();
		m_matThickProp.SetItem(2, DataPropItem(v));

		m_bModified = TRUE;
	}
}

void MaterialThicknessInfoPage::OnKillComment()
{
	UpdateData(TRUE);

	if (m_matThickProp.GetItem(4).GetValue().bstrVal != m_strComment)
	{
		VARIANT v;
		v.vt = VT_BSTR;
		v.bstrVal = m_strComment.AllocSysString();
		m_matThickProp.SetItem(4, DataPropItem(v));

		m_bModified = TRUE;
	}
}

void MaterialThicknessInfoPage::OnKillThickness()
{
	UpdateData(TRUE);

	if (!MathEx::Equal(m_matThickProp.GetItem(3).GetValue().dblVal, m_dThickness.Get_Number(), GEOM_TOLERANCE))
	{
		VARIANT v;
		v.vt = VT_R8;
		v.dblVal = m_dThickness.Get_Number();
		m_matThickProp.SetItem(3, DataPropItem(v));

		m_bModified = TRUE;
	}
}

void MaterialThicknessInfoPage::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{

}

void MaterialThicknessInfoPage::ReleaseGridItemData()
{
	for (int i = 0; i < m_ctrlMatSizeList.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_ctrlMatSizeList.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

END_CUTLEADER_NAMESPACE()
