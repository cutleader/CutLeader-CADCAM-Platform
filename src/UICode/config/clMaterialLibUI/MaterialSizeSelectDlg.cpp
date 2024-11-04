#include "StdAfx.h"
#include "MaterialSizeSelectDlg.h"

#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "clUtilityResource.h"
#include "baseconst.h"
#include "PathHelper.h"

#include "clBaseDataResource.h"
#include "baseConst.h"
#include "clUILibResource.h"
#include "DataBaseManager.h"
#include "Rect2D.h"
#include "LineArc2DList.h"
#include "LineArc2DLoop.h"
#include "LineArc2DLoopList.h"
#include "LineArc2DLoopManager.h"
#include "CLOptionInfo.h"
#include "ImpExpConfig.h"

#include "clPartResource.h"
#include "clGeometryFeatureResource.h"

#include "DxfData.h"
#include "DxfDwgConvertor.h"

#include "clMaterialLibResource.h"
#include "Material.h"
#include "MaterialThickness.h"
#include "IMaterialSize.h"
#include "MaterialSizeList.h"
#include "MaterialSizeInfoDlg.h"
#include "MaterialLibWriter.h"
#include "MaterialLibLoader.h"
#include "ImportMaterialDlg.h"
#include "MaterialProcessor.h"
#include "LicenseManager.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(MaterialSizeSelectDlg, CDialogEx)

MaterialSizeSelectDlg::MaterialSizeSelectDlg()
				: CLDialog(MaterialSizeSelectDlg::IDD)
{
	m_pMatSize.reset();
	m_bListInited = FALSE;
	m_bHideSkele = FALSE;
}

MaterialSizeSelectDlg::~MaterialSizeSelectDlg(void)
{
}

void MaterialSizeSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_MAT_TYPE, m_mat);
	DDX_Control(pDX, IDC_COMBO_MAT_NAME, m_matThick);

	DDX_Control(pDX, IDC_LIST_MATSIZE, m_matSizeList);
	DDX_Check(pDX, IDC_CHECK_HIDE_SKELE, m_bHideSkele);
	DDX_Control(pDX, IDC_STATIC_PREVIEW_MAT, m_matPreviewWnd);
}

BEGIN_MESSAGE_MAP(MaterialSizeSelectDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()

	ON_CBN_SELCHANGE(IDC_COMBO_MAT_TYPE, OnSelchangeMatType)
	ON_CBN_SELCHANGE(IDC_COMBO_MAT_NAME, OnSelchangeMatName)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MATSIZE, OnMatListchanged)
	ON_BN_CLICKED(IDC_CHECK_HIDE_SKELE, OnCheckHideSkele)

	ON_BN_CLICKED(IDC_BTN_NEW_MATSIZE, OnNewMatSize)
	ON_BN_CLICKED(IDC_BTN_IMP_MATSIZE, OnImpMatSize)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL MaterialSizeSelectDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// ��ʼ���ӿڡ�
	Rect2D geomRect(.0, 10.0, .0, 10.0);
	m_matPreviewWnd.InitViewPort(geomRect);

	// load all MatType.
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	m_pMatList = MaterialLibLoader::LoadAllMat(pMatLibDBCon);

	InitListStyle();
	InitListData();
	UpdateData(FALSE);

	// ������֤��
    LicenseCheckResultType iLicenseCheckResult = LicenseManager::CheckLicense();
	if (iLicenseCheckResult != LicenseCheckResult_ultimateEdition)
	{
		((CWnd*)GetDlgItem(IDC_BTN_IMP_MATSIZE))->ShowWindow(SW_HIDE);
	}

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void MaterialSizeSelectDlg::OnDestroy()
{
	// release the item data.
	RlsMatTypeItemData();
	RlsMatNameItemData();
	RlsMatSizeItemData();

	// �ͷ��ӿڡ�
	m_matPreviewWnd.ReleaseViewPort();

	CDialogEx::OnDestroy();
}

void MaterialSizeSelectDlg::OnPaint()
{
	CDialogEx::OnPaint();
	PreviewMatSize();
}

HBRUSH MaterialSizeSelectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_MAT)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void MaterialSizeSelectDlg::OnSelchangeMatType()
{
	int iIndex = m_mat.GetCurSel();
	if (iIndex == -1)
		return;

	// get the selected material type.
	LONGLONG* pMatID = (LONGLONG*)m_mat.GetItemData(iIndex);
	MaterialPtr pMaterial = m_pMatList->GetItemByID(*pMatID);

	/************************************************************************/
	// init material name list.

	m_matThick.ResetContent();
	RlsMatNameItemData();

	MaterialThicknessListPtr pMatThickList = pMaterial->GetMatThickList();
	if (pMatThickList->size() == 0)
	{
		m_matSizeList.DeleteAllItems();
		RlsMatSizeItemData();
		return;
	}

	for (unsigned int j = 0; j < pMatThickList->size(); j++)
	{
		MaterialThicknessPtr pMatThick = pMatThickList->at(j);
		m_matThick.AddString(pMatThick->GetName());
		m_matThick.SetItemData(j, (DWORD_PTR)(new LONGLONG(pMatThick->GetID())));
	}
	m_matThick.SetCurSel(0);

	OnSelchangeMatName();
	/************************************************************************/
}

void MaterialSizeSelectDlg::OnSelchangeMatName()
{
	int iMatTypeIndex = m_mat.GetCurSel();
	if (iMatTypeIndex == -1)
		return;

	int iMatNameIndex = m_matThick.GetCurSel();
	if (iMatNameIndex == -1)
		return;


	/************************************************************************/
	// get the selected material name.

	LONGLONG* pMatID = (LONGLONG*)m_mat.GetItemData(iMatTypeIndex);
	MaterialPtr pMaterial = m_pMatList->GetItemByID(*pMatID);

	LONGLONG* pMatThickID = (LONGLONG*)m_matThick.GetItemData(iMatNameIndex);
	MaterialThicknessListPtr pMatThickList = pMaterial->GetMatThickList();
	MaterialThicknessPtr pMatThick = pMatThickList->GetItemByID(*pMatThickID);
	/************************************************************************/


	/************************************************************************/
	// init material size grid.

	m_matSizeList.DeleteAllItems();
	RlsMatSizeItemData();

	MaterialSizeListPtr pMaterialSizeList = pMatThick->GetMatSizeList();
	if (pMaterialSizeList->size() == 0)
		return;

	for (unsigned int k = 0; k < pMaterialSizeList->size(); k++)
	{
		IMaterialSizePtr pMaterialSize = pMaterialSizeList->at(k);

		// insert a new row.
		m_matSizeList.InsertItem(k, _T(""));
		m_matSizeList.SetItemData(k, (DWORD_PTR)(new LONGLONG(pMaterialSize->GetID())));

		// "num" column
		CString strNum;
		strNum.Format(INT_FORMAT, k+1);
		m_matSizeList.SetItemText(k, 1, strNum);

		// ���ơ�
		CString strName = pMaterialSize->GetName();
		m_matSizeList.SetItemText(k, 2, strName);

		// ���������͡��С�
		CString strMatSizeType;
		if (pMaterialSize->GetType() == MAT_SIZE_RECT)
			strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_RECT);
		else if (pMaterialSize->GetType() == MAT_SIZE_POLY)
			strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_POLY);
		m_matSizeList.SetItemText(k, 3, strMatSizeType);

		// �Ƿ�����
		CString strIsRemnant;
		if (pMaterialSize->IsRemnant())
			strIsRemnant = MultiLanguageMgr::GetInstance()->TranslateString(IDS_YES);
		else
			strIsRemnant = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NO);
		m_matSizeList.SetItemText(k, 4, strIsRemnant);

		// "material width" column
		CString strMatLen;
		strMatLen.Format(FLOAT_FORMAT_1, pMaterialSize->GetWidth());
		m_matSizeList.SetItemText(k, 5, strMatLen);

		// "material height" column
		CString strMatWidth;
		strMatWidth.Format(FLOAT_FORMAT_1, pMaterialSize->GetHeight());
		m_matSizeList.SetItemText(k, 6, strMatWidth);

		// "total" column
		CString strMatTotal;
		strMatTotal.Format(INT_FORMAT, pMaterialSize->GetTotalNum());
		m_matSizeList.SetItemText(k, 7, strMatTotal);

		// "available num" column
		CString strAvailable;
		strAvailable.Format(INT_FORMAT, pMaterialSize->GetAvailNum());
		m_matSizeList.SetItemText(k, 8, strAvailable);
	}
	/************************************************************************/
}

void MaterialSizeSelectDlg::OnMatListchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!m_bListInited)
		return;

	// Ԥ�����ϡ�
	PreviewMatSize();

	*pResult = 0;
}

void MaterialSizeSelectDlg::OnCheckHideSkele()
{
	UpdateData(TRUE);

	/************************************************************************/
	// get the selected material name.

	// current mat type.
	int iMatTypeIndex = m_mat.GetCurSel();
	LONGLONG* pMatID = (LONGLONG*)m_mat.GetItemData(iMatTypeIndex);
	MaterialPtr pMaterial = m_pMatList->GetItemByID(*pMatID);

	// selected mat name.
	int iMatNameIndex = m_matThick.GetCurSel();
	LONGLONG* pMatThickID = (LONGLONG*)m_matThick.GetItemData(iMatNameIndex);
	MaterialThicknessListPtr pMatThickList = pMaterial->GetMatThickList();
	MaterialThicknessPtr pMatThick = pMatThickList->GetItemByID(*pMatThickID);
	/************************************************************************/

	// init the matSize list by the material name object.
	if (pMatThick)
		InitMatSizeList(pMatThick);
}

void MaterialSizeSelectDlg::OnNewMatSize()
{
	// ѡ�еĲ��ϡ�
	int iMatTypeIndex = m_mat.GetCurSel();
	if (iMatTypeIndex == -1)
		return;
	LONGLONG* pMatID = (LONGLONG*)m_mat.GetItemData(iMatTypeIndex);
	MaterialPtr pMaterial = m_pMatList->GetItemByID(*pMatID);

	// ѡ�еĲ��Ϻ�ȡ�
	int iMatNameIndex = m_matThick.GetCurSel();
	if (iMatNameIndex == -1)
		return;
	LONGLONG* pMatThickID = (LONGLONG*)m_matThick.GetItemData(iMatNameIndex);
	MaterialThicknessListPtr pMatThickList = pMaterial->GetMatThickList();
	MaterialThicknessPtr pMatThick = pMatThickList->GetItemByID(*pMatThickID);

	MaterialSizeInfoDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		// �½��Ĳ��ϳߴ硣
		IMaterialSizePtr pMaterialSize = dlg.GetMatSize();
		pMaterialSize->SetParentID(pMatThick->GetID());
		pMatThick->GetMatSizeList()->push_back(pMaterialSize);

		// ������ϳߴ硣
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		MaterialLibWriter::SaveMatSize(pMatLibDBCon, pMaterialSize, TRUE);

		// ���Ӳ��ϳߴ絽�б�
		AddMatSizeToList(pMaterialSize);
	}
}

void MaterialSizeSelectDlg::OnImpMatSize()
{
	// ѡ�еĲ��ϡ�
	int iMatTypeIndex = m_mat.GetCurSel();
	if (iMatTypeIndex == -1)
		return;
	LONGLONG* pMatID = (LONGLONG*)m_mat.GetItemData(iMatTypeIndex);
	MaterialPtr pMaterial = m_pMatList->GetItemByID(*pMatID);

	// ѡ�еĲ��Ϻ�ȡ�
	int iMatNameIndex = m_matThick.GetCurSel();
	if (iMatNameIndex == -1)
		return;
	LONGLONG* pMatThickID = (LONGLONG*)m_matThick.GetItemData(iMatNameIndex);
	MaterialThicknessListPtr pMatThickList = pMaterial->GetMatThickList();
	MaterialThicknessPtr pMatThick = pMatThickList->GetItemByID(*pMatThickID);

	ImportMaterialDlg impFileDlg(OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT);
	if (impFileDlg.DoModal() == IDOK)
	{
		POSITION pos = impFileDlg.GetStartPosition();
		while (pos != NULL)
		{
			CString strFilePath = impFileDlg.GetNextPathName(pos);

			// ��dxf/dwg�ļ��õ�����ͼ�μ��ϡ�
			DxfDataPtr pDxfData;
			if (PathHelper::IsDxfFile(strFilePath))
				pDxfData = DxfDwgConvertor::ExtractPatList(strFilePath, FALSE);
			else if (PathHelper::IsDwgFile(strFilePath))
				pDxfData = DxfDwgConvertor::ExtractPatList(strFilePath, TRUE);
			LineArc2DListPtr pGeom2DList = pDxfData->Calculate2DLineArcs();

			// �õ�����ͼ�λ�·��
            double dImpTol = CLOptionInfo::GetInstance()->GetImpExpConfig()->GetImpTol(); // �����ݲ�����
			LineArc2DLoopListPtr pGeomLoopList = LineArc2DLoopManager::BuildGeomLoop(pGeom2DList, dImpTol);
			if (pGeomLoopList->size() == 0)
			{
				CString strProductName, str;
				strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_IMPORT_MATSIZE_ERROR1);
				MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
			}
			else
			{
				// �ӡ�����ͼ�λ�·���������ɡ����ϳߴ硱����
				IMaterialSizePtr pMaterialSize = MaterialProcessor::BuildMatSize(PathHelper::GetFileName(strFilePath), pGeomLoopList.get());
				pMaterialSize->SetParentID(pMatThick->GetID());
				pMatThick->GetMatSizeList()->push_back(pMaterialSize);

				// save the MatSize.
				DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
				MaterialLibWriter::SaveMatSize(pMatLibDBCon, pMaterialSize, TRUE);

				// ���Ӳ��ϳߴ絽�б�
				AddMatSizeToList(pMaterialSize);
			}
		}
	}
}

void MaterialSizeSelectDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	// ѡ�еĲ��ϡ�
	int iMatTypeIndex = m_mat.GetCurSel();
	if (iMatTypeIndex == -1)
		return;
	LONGLONG* pMatID = (LONGLONG*)m_mat.GetItemData(iMatTypeIndex);
	MaterialPtr pMaterial = m_pMatList->GetItemByID(*pMatID);

	// ѡ�еĲ��Ϻ�ȡ�
	int iMatNameIndex = m_matThick.GetCurSel();
	if (iMatNameIndex == -1)
		return;
	LONGLONG* pMatThickID = (LONGLONG*)m_matThick.GetItemData(iMatNameIndex);
	MaterialThicknessListPtr pMatThickList = pMaterial->GetMatThickList();
	MaterialThicknessPtr pMatThick = pMatThickList->GetItemByID(*pMatThickID);

	// ѡ�еĲ��ϳߴ硣
	int iMatSizeIndex = INVALID_IDX;
	for (int i = 0; i < m_matSizeList.GetItemCount(); i++)
	{
		if (m_matSizeList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			iMatSizeIndex = i;
	}
	if (iMatSizeIndex == -1)
		return;
	LONGLONG* pMatSizeID = (LONGLONG*)m_matSizeList.GetItemData(iMatSizeIndex);
	MaterialSizeListPtr pMaterialSizeList = pMatThick->GetMatSizeList();
	m_pMatSize = pMaterialSizeList->GetItemByID(*pMatSizeID);

	OnOK();
}

void MaterialSizeSelectDlg::RlsMatTypeItemData()
{
	for (int i = 0; i < m_mat.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_mat.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void MaterialSizeSelectDlg::RlsMatNameItemData()
{
	for (int i = 0; i < m_matThick.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_matThick.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void MaterialSizeSelectDlg::RlsMatSizeItemData()
{
	for (int i = 0; i < m_matSizeList.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_matSizeList.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void MaterialSizeSelectDlg::InitListStyle()
{
	CString str;

	// Requires one never uses column 0
	m_matSizeList.InsertHiddenLabelColumn();

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol1 = new CGridColumnTraitText;
	m_matSizeList.InsertColumnTrait(1, str, LVCFMT_LEFT, 34, 0, pNUMCol1);

	// �����ơ��С�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_matSizeList.InsertColumnTrait(2, str, LVCFMT_LEFT, 90, 0, pNameCol);

	// "matsize type" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_TYPE);
	CGridColumnTrait* pMatSizeTypeCol = new CGridColumnTraitText;
	m_matSizeList.InsertColumnTrait(3, str, LVCFMT_LEFT, 60, 0, pMatSizeTypeCol);

	// �Ƿ����ϡ�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_IS_REMNANT);
	CGridColumnTrait* pRemnantCol = new CGridColumnTraitText;
	m_matSizeList.InsertColumnTrait(4, str, LVCFMT_LEFT, 60, 0, pRemnantCol);

	// "width"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_WIDTH);
	CGridColumnTrait* pLengthCol = new CGridColumnTraitText;
	m_matSizeList.InsertColumnTrait(5, str, LVCFMT_LEFT, 60, 0, pLengthCol);

	// "height"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_HEIGHT);
	CGridColumnTrait* pWidthCol = new CGridColumnTraitText;
	m_matSizeList.InsertColumnTrait(6, str, LVCFMT_LEFT, 60, 0, pWidthCol);

	// "count"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COUNT);
	CGridColumnTrait* pCountCol = new CGridColumnTraitText;
	m_matSizeList.InsertColumnTrait(7, str, LVCFMT_LEFT, 50, 0, pCountCol);

	// "available number"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MAT_AVAILABLE_NUM);
	CGridColumnTrait* pAvailCol = new CGridColumnTraitText;
	m_matSizeList.InsertColumnTrait(8, str, LVCFMT_LEFT, 70, 0, pAvailCol);

	// set other style.
	m_matSizeList.EnableVisualStyles(TRUE);
}

void MaterialSizeSelectDlg::InitListData()
{
	// init material type list.
	for (unsigned int i = 0; i < m_pMatList->size(); i++)
	{
		MaterialPtr pMaterial = m_pMatList->at(i);
		m_mat.AddString(pMaterial->GetName());
		m_mat.SetItemData(i, (DWORD_PTR)(new LONGLONG(pMaterial->GetID())));
	}
	m_mat.SetCurSel(0);

	// init material name list.
	MaterialPtr pMaterial = m_pMatList->at(0);
	MaterialThicknessListPtr pMatThickList = pMaterial->GetMatThickList();
	if (pMatThickList->size() > 0)
	{
		for (unsigned int j = 0; j < pMatThickList->size(); j++)
		{
			MaterialThicknessPtr pMatThick = pMatThickList->at(j);
			m_matThick.AddString(pMatThick->GetName());
			m_matThick.SetItemData(j, (DWORD_PTR)(new LONGLONG(pMatThick->GetID())));
		}
		m_matThick.SetCurSel(0);
	}

	// init material size grid.
	if (pMatThickList->size() > 0)
	{
		MaterialThicknessPtr pMatThick = pMatThickList->at(0);
		InitMatSizeList(pMatThick);
	}
}

void MaterialSizeSelectDlg::InitMatSizeList(MaterialThicknessPtr pMatThick)
{
	m_bListInited = FALSE;

	m_matSizeList.DeleteAllItems();
	RlsMatSizeItemData();

	MaterialSizeListPtr pMaterialSizeList = pMatThick->GetMatSizeList();
	if (pMaterialSizeList->size() > 0)
	{
		int iRowCount = 0;
		for (unsigned int k = 0; k < pMaterialSizeList->size(); k++)
		{
			IMaterialSizePtr pMaterialSize = pMaterialSizeList->at(k);

			// check whether display remnant material.
			if (m_bHideSkele && pMaterialSize->IsRemnant())
				continue;

			// ���Ӳ��ϳߴ絽�б�
			AddMatSizeToList(pMaterialSize);

			iRowCount++;
		}
	}

	m_bListInited = TRUE;

	// ѡ���һ�С�
	int iItemCount = m_matSizeList.GetItemCount();
	if (iItemCount > 0)
	{
		m_matSizeList.EnsureVisible(0, FALSE);
		m_matSizeList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_matSizeList.SetFocus();
	}
}

void MaterialSizeSelectDlg::PreviewMatSize()
{
	// �õ�ѡ���
	int iIndex = INVALID_IDX;
	for (int i = 0; i < m_matSizeList.GetItemCount(); i++)
	{
		if (m_matSizeList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			iIndex = i;
	}

	if (iIndex != INVALID_IDX)
	{
		LONGLONG* pMatSizeID = (LONGLONG*)m_matSizeList.GetItemData(iIndex);
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, *pMatSizeID);
		if (pMaterialSize)
		{
			// ��ʼ���ӿڡ�
			Rect2DPtr pOutRect(new Rect2D(0, pMaterialSize->GetWidth(), 0, pMaterialSize->GetHeight()));
			LineArc2DListPtr pGeom2DList = pOutRect->GetLineS();
			Rect2D geomRect = pGeom2DList->GetRectBox();
			m_matPreviewWnd.FitRect(geomRect);

			// ���ò������
			m_matPreviewWnd.SetSolidGeom2DList(MaterialProcessor::GetBorderLineArcs(pMaterialSize.get()));

			// ���Ʋ��ϵ���������
			m_matPreviewWnd.SetDashGeom2DList(MaterialProcessor::GetUselessRegions(pMaterialSize.get()));

			// ���ơ�
			m_matPreviewWnd.OnPaint();
		}
	}
}

void MaterialSizeSelectDlg::AddMatSizeToList(IMaterialSizePtr pMaterialSize)
{
	int iRowCount = m_matSizeList.GetItemCount();

	// ����һ�С�
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_PARAM;
	lvItem.iItem = iRowCount;
	lvItem.iSubItem = 0;
	lvItem.pszText = _T("");
	lvItem.lParam = (DWORD_PTR)(new LONGLONG(pMaterialSize->GetID()));
	m_matSizeList.InsertItem(&lvItem);

	// "num" column
	CString strNum;
	strNum.Format(INT_FORMAT, iRowCount+1);
	m_matSizeList.SetItemText(iRowCount, 1, strNum);

	// ���ơ�
	CString strName = pMaterialSize->GetName();
	m_matSizeList.SetItemText(iRowCount, 2, strName);

	// ���������͡��С�
	CString strMatSizeType;
	if (pMaterialSize->GetType() == MAT_SIZE_RECT)
		strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_RECT);
	else if (pMaterialSize->GetType() == MAT_SIZE_POLY)
		strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_POLY);
	m_matSizeList.SetItemText(iRowCount, 3, strMatSizeType);

	// �Ƿ�����
	CString strIsRemnant;
	if (pMaterialSize->IsRemnant())
		strIsRemnant = MultiLanguageMgr::GetInstance()->TranslateString(IDS_YES);
	else
		strIsRemnant = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NO);
	m_matSizeList.SetItemText(iRowCount, 4, strIsRemnant);

	// "material width" column
	CString strMatLen;
	strMatLen.Format(FLOAT_FORMAT_1, pMaterialSize->GetWidth());
	m_matSizeList.SetItemText(iRowCount, 5, strMatLen);

	// "material height" column
	CString strMatWidth;
	strMatWidth.Format(FLOAT_FORMAT_1, pMaterialSize->GetHeight());
	m_matSizeList.SetItemText(iRowCount, 6, strMatWidth);

	// "total" column
	CString strMatTotal;
	strMatTotal.Format(INT_FORMAT, pMaterialSize->GetTotalNum());
	m_matSizeList.SetItemText(iRowCount, 7, strMatTotal);

	// "available num" column
	CString strAvailable;
	strAvailable.Format(INT_FORMAT, pMaterialSize->GetAvailNum());
	m_matSizeList.SetItemText(iRowCount, 8, strAvailable);

	m_matSizeList.SetItemState(iRowCount, LVIS_SELECTED, LVIS_SELECTED);
}

END_CUTLEADER_NAMESPACE()
