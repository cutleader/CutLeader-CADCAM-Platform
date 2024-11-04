#include "StdAfx.h"
#include "NestMatSelectDlg.h"

#include "baseConst.h"
#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "clUtilityResource.h"
#include "PathHelper.h"

#include "Rect2D.h"
#include "LineArc2DList.h"
#include "LineArc2DLoop.h"
#include "LineArc2DLoopList.h"
#include "LineArc2DLoopManager.h"
#include "baseConst.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"
#include "CLOptionInfo.h"
#include "ImpExpConfig.h"

#include "clPartResource.h"
#include "clGeometryFeatureResource.h"
#include "PatternList.h"

#include "DxfData.h"
#include "DxfDwgConvertor.h"

#include "clNestResource.h"
#include "NestMaterialList.h"
#include "NestMaterial.h"
#include "clNestResource.h"

#include "ImportMaterialDlg.h"
#include "clMaterialLibResource.h"
#include "MaterialLibLoader.h"
#include "Material.h"
#include "MaterialThickness.h"
#include "IMaterialSize.h"
#include "MaterialSizeList.h"
#include "MaterialSizeInfoDlg.h"
#include "MaterialLibWriter.h"
#include "MaterialProcessor.h"
#include "LicenseManager.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(NestMatSelectDlg, CDialogEx)

BEGIN_MESSAGE_MAP(NestMatSelectDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()

	ON_CBN_SELCHANGE(IDC_COMBO_MAT_TYPE, OnSelchangeMatType)
	ON_CBN_SELCHANGE(IDC_COMBO_MAT_NAME, OnSelchangeMatName)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAT, OnMatListchanged)
	ON_BN_CLICKED(IDC_CHECK_HIDE_SKELE, OnCheckHideSkele)

	ON_BN_CLICKED(IDC_BTN_NEW_MATSIZE, OnNewMatSize)
	ON_BN_CLICKED(IDC_BTN_IMP_MATSIZE, OnImpMatSize)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

NestMatSelectDlg::NestMatSelectDlg(CWnd* pParent /*=NULL*/)
				: CLDialog(NestMatSelectDlg::IDD, pParent)
{
	m_pMatSizeList.reset(new MaterialSizeList);
	m_bListInited = FALSE;
	m_bHideSkele = FALSE;
}

NestMatSelectDlg::~NestMatSelectDlg()
{
}

void NestMatSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_MAT_TYPE, m_mat);
	DDX_Control(pDX, IDC_COMBO_MAT_NAME, m_matThick);

	DDX_Control(pDX, IDC_LIST_MAT, m_matList);
	DDX_Check(pDX, IDC_CHECK_HIDE_SKELE, m_bHideSkele);
	DDX_Control(pDX, IDC_STATIC_PREVIEW_MAT, m_matPreviewWnd);
}

BOOL NestMatSelectDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// ��ʼ���ӿڡ�
	Rect2D geomRect(.0, 10.0, .0, 10.0);
	m_matPreviewWnd.InitViewPort(geomRect);

	// load all MatType.
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	m_pMatList = MaterialLibLoader::LoadAllMat(pMatLibDBCon);

	InitListStyle();
	InitData();
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

void NestMatSelectDlg::OnDestroy()
{
	// release the item data.
	RlsMatTypeItemData();
	RlsMatNameItemData();
	RlsMatSizeItemData();

	// �ͷ��ӿڡ�
	m_matPreviewWnd.ReleaseViewPort();

	CDialogEx::OnDestroy();
}

void NestMatSelectDlg::OnPaint()
{
	CDialogEx::OnPaint();
	PreviewMatSize();
}

HBRUSH NestMatSelectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void NestMatSelectDlg::OnSelchangeMatType()
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
		m_matList.DeleteAllItems();
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

void NestMatSelectDlg::OnSelchangeMatName()
{
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

	// init material size grid.
	if (pMatThick)
		InitMatSizeList(pMatThick, FALSE);
}

void NestMatSelectDlg::OnMatListchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!m_bListInited)
		return;

	// �����ǲ��ǵ���checkbox��
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(1)) && (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(2))) ||
		((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(2)) && (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(1)))) 
	{
		m_matList.SetItemState(pNMLV->iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}

	// Ԥ�����ϡ�
	PreviewMatSize();

	*pResult = 0;
}

void NestMatSelectDlg::OnCheckHideSkele()
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
		InitMatSizeList(pMatThick, FALSE);
}

void NestMatSelectDlg::OnNewMatSize()
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
		AddMatSizeToList(pMaterialSize, TRUE);
	}
}

void NestMatSelectDlg::OnImpMatSize()
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
				AddMatSizeToList(pMaterialSize, TRUE);
			}
		}
	}
}

void NestMatSelectDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	/************************************************************************/
	// check something.

	int iMatTypeIndex = m_mat.GetCurSel();
	if (iMatTypeIndex == -1)
		return;

	int iMatNameIndex = m_matThick.GetCurSel();
	if (iMatNameIndex == -1)
		return;
	/************************************************************************/

	/************************************************************************/
	// get the selected material name.

	LONGLONG* pMatID = (LONGLONG*)m_mat.GetItemData(iMatTypeIndex);
	MaterialPtr pMaterial = m_pMatList->GetItemByID(*pMatID);

	LONGLONG* pMatThickID = (LONGLONG*)m_matThick.GetItemData(iMatNameIndex);
	MaterialThicknessListPtr pMatThickList = pMaterial->GetMatThickList();
	MaterialThicknessPtr pMatThick = pMatThickList->GetItemByID(*pMatThickID);
	/************************************************************************/

	// get the selected material size.
	MaterialSizeListPtr pMaterialSizeList = pMatThick->GetMatSizeList();
	for (int i = 0; i < m_matList.GetItemCount(); i++)
	{
		BOOL bChecked = m_matList.GetCheck(i);
		if (bChecked)
		{
			LONGLONG* pMatSizeID = (LONGLONG*)m_matList.GetItemData(i);
			IMaterialSizePtr pMaterialSize = pMaterialSizeList->GetItemByID(*pMatSizeID);
			m_pMatSizeList->push_back(pMaterialSize);
		}
	}

	// ����ǲ���ѡ���˲��ϡ�
	if (m_pMatSizeList->size() > 0)
		OnOK();
}

void NestMatSelectDlg::RlsMatTypeItemData()
{
	for (int i = 0; i < m_mat.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_mat.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void NestMatSelectDlg::RlsMatNameItemData()
{
	for (int i = 0; i < m_matThick.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_matThick.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void NestMatSelectDlg::RlsMatSizeItemData()
{
	for (int i = 0; i < m_matList.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_matList.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void NestMatSelectDlg::InitListStyle()
{
	CString str;

	// place check box at first column.
	CGridColumnTrait* pStatusCol = new CGridColumnTraitText;
	m_matList.InsertColumnTrait(0, _T(""), LVCFMT_LEFT, 25, 0, pStatusCol);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol1 = new CGridColumnTraitText;
	m_matList.InsertColumnTrait(1, str, LVCFMT_LEFT, 34, 0, pNUMCol1);

	// �����ơ��С�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_matList.InsertColumnTrait(2, str, LVCFMT_LEFT, 90, 0, pNameCol);

	// "matsize type" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_TYPE);
	CGridColumnTrait* pMatSizeTypeCol = new CGridColumnTraitText;
	m_matList.InsertColumnTrait(3, str, LVCFMT_LEFT, 60, 0, pMatSizeTypeCol);

	// �Ƿ����ϡ�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_IS_REMNANT);
	CGridColumnTrait* pRemnantCol = new CGridColumnTraitText;
	m_matList.InsertColumnTrait(4, str, LVCFMT_LEFT, 55, 0, pRemnantCol);

	// "width"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_WIDTH);
	CGridColumnTrait* pWidthCol = new CGridColumnTraitText;
	m_matList.InsertColumnTrait(5, str, LVCFMT_LEFT, 55, 0, pWidthCol);

	// "height"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_HEIGHT);
	CGridColumnTrait* pHeightCol = new CGridColumnTraitText;
	m_matList.InsertColumnTrait(6, str, LVCFMT_LEFT, 55, 0, pHeightCol);

	// "count"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MAT_TOTAL_NUM);
	CGridColumnTrait* pCountCol = new CGridColumnTraitText;
	m_matList.InsertColumnTrait(7, str, LVCFMT_LEFT, 50, 0, pCountCol);

	// "available number"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MAT_AVAILABLE_NUM);
	CGridColumnTrait* pAvailCol = new CGridColumnTraitText;
	m_matList.InsertColumnTrait(8, str, LVCFMT_LEFT, 70, 0, pAvailCol);

	// set other style.
	m_matList.SetExtendedStyle(LVS_EX_CHECKBOXES | m_matList.GetExtendedStyle());
	m_matList.EnableVisualStyles(TRUE);
}

void NestMatSelectDlg::InitData()
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
		InitMatSizeList(pMatThick, FALSE);
	}
}

void NestMatSelectDlg::InitMatSizeList(MaterialThicknessPtr pMatThick, BOOL bCheck)
{
	m_bListInited = FALSE;

	m_matList.DeleteAllItems();
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

            // ��������Ҫ����0
            if (pMaterialSize->GetAvailNum() < 1)
            {
                continue;
            }

			// ���Ӳ��ϳߴ絽�б�
			AddMatSizeToList(pMaterialSize, bCheck);

			iRowCount++;
		}
	}

	m_bListInited = TRUE;

	// ѡ���һ�С�
	int iItemCount = m_matList.GetItemCount();
	if (iItemCount > 0)
	{
		m_matList.EnsureVisible(0, FALSE);
		m_matList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_matList.SetFocus();
	}
}

void NestMatSelectDlg::PreviewMatSize()
{
	// �õ�ѡ���
	int iIndex = INVALID_IDX;
	for (int i = 0; i < m_matList.GetItemCount(); i++)
	{
		if (m_matList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			iIndex = i;
	}

	if (iIndex != INVALID_IDX)
	{
		LONGLONG* pMatSizeID = (LONGLONG*)m_matList.GetItemData(iIndex);
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

void NestMatSelectDlg::AddMatSizeToList(IMaterialSizePtr pMaterialSize, BOOL bCheck)
{
	int iRowCount = m_matList.GetItemCount();

	// ����һ�С�
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_PARAM;
	lvItem.iItem = iRowCount;
	lvItem.iSubItem = 0;
	lvItem.pszText = _T("");
	lvItem.lParam = (DWORD_PTR)(new LONGLONG(pMaterialSize->GetID()));
	m_matList.InsertItem(&lvItem);

	// "num" column
	CString strNum;
	strNum.Format(INT_FORMAT, iRowCount+1);
	m_matList.SetItemText(iRowCount, 1, strNum);

	// ���ơ�
	CString strName = pMaterialSize->GetName();
	m_matList.SetItemText(iRowCount, 2, strName);

	// ���������͡��С�
	CString strMatSizeType;
	if (pMaterialSize->GetType() == MAT_SIZE_RECT)
		strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_RECT);
	else if (pMaterialSize->GetType() == MAT_SIZE_POLY)
		strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_POLY);
	m_matList.SetItemText(iRowCount, 3, strMatSizeType);

	// �Ƿ�����
	CString strIsRemnant;
	if (pMaterialSize->IsRemnant())
		strIsRemnant = MultiLanguageMgr::GetInstance()->TranslateString(IDS_YES);
	else
		strIsRemnant = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NO);
	m_matList.SetItemText(iRowCount, 4, strIsRemnant);

	// "material width" column
	CString strMatLen;
	strMatLen.Format(FLOAT_FORMAT_1, pMaterialSize->GetWidth());
	m_matList.SetItemText(iRowCount, 5, strMatLen);

	// "material height" column
	CString strMatWidth;
	strMatWidth.Format(FLOAT_FORMAT_1, pMaterialSize->GetHeight());
	m_matList.SetItemText(iRowCount, 6, strMatWidth);

	// "total" column
	CString strMatTotal;
	strMatTotal.Format(INT_FORMAT, pMaterialSize->GetTotalNum());
	m_matList.SetItemText(iRowCount, 7, strMatTotal);

	// "available num" column
	CString strAvailable;
	strAvailable.Format(INT_FORMAT, pMaterialSize->GetAvailNum());
	m_matList.SetItemText(iRowCount, 8, strAvailable);

	m_matList.SetCheck(iRowCount, bCheck);
}

END_CUTLEADER_NAMESPACE()
