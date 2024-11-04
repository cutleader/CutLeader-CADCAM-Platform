#include "StdAfx.h"
#include "NestJobInfoDlg.h"

#include "baseConst.h"
#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "clUtilityResource.h"

#include "baseConst.h"
#include "clUILibResource.h"
#include "DataBaseManager.h"

#include "ParamConfig.h"
#include "ParamConfigLoader.h"
#include "ExpertLibItemLoader.h"

#include "clMaterialLibResource.h"
#include "IMaterialSize.h"

#include "clGeometryFeatureResource.h"
#include "clPartResource.h"
#include "PartItem.h"
#include "Part.h"
#include "PartCadData.h"

#include "clNestResource.h"
#include "NestTask.h"
#include "NestPart.h"
#include "NestPartList.h"
#include "NestMaterial.h"
#include "NestMaterialList.h"
#include "NestPriority.h"
#include "NestJob.h"
#include "NestJobMgr.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(NestJobInfoDlg, CDialogEx)

BEGIN_MESSAGE_MAP(NestJobInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_PAINT()

	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

END_MESSAGE_MAP()

NestJobInfoDlg::NestJobInfoDlg(CWnd* pParent /*=NULL*/)
					: CLDialog(NestJobInfoDlg::IDD, pParent)
{
}

NestJobInfoDlg::~NestJobInfoDlg(void)
{
}

void NestJobInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_EXPLIB_NAME, m_strExpLibName);
	DDX_Text(pDX, IDC_EDIT_CONFIG, m_strParamConfig);

	DDX_Control(pDX, IDC_LIST_NESTPART, m_nestPartList);
	DDX_Control(pDX, IDC_LIST_NESTMAT, m_nestMatList);

	DDX_Text(pDX, IDC_EDIT_MAT_UTIL, m_strUtilization);
}

BOOL NestJobInfoDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

	// init the style of the list ctrl
	InitListStyle();

	// init the data
	InitListData();

	// calc the utilization.
	map<LONGLONG, map<LONGLONG, int>> partCountDataInSheets; // 板材上不同零件的数量信息。map<板材id, map<零件id, 零件数量>>
	double dUtilization = NestJobMgr::CalcMatUtilization_fromDB(m_pNestJob.get(), partCountDataInSheets);
	m_strUtilization.Format(_T("%.1f"), dUtilization*100);

	SetBackgroundColor(UI_COLOR);
	UpdateData(FALSE);

	return TRUE;
}

void NestJobInfoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

}

void NestJobInfoDlg::OnBnClickedOk()
{

	OnOK();
}

BOOL NestJobInfoDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH NestJobInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("EDIT")) == 0)
	{
		// for "Invalid Data" edit box, show red bk color.
		CString strTmp;
		strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		CString strText;
		pWnd->GetWindowText(strText);
		if (strText == strTmp)
		{
			pDC->SetBkMode(TRANSPARENT);
			return ::CreateSolidBrush(RGB(255, 0, 0)); 
		}
		else
			return hbr;
	}

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

void NestJobInfoDlg::InitListStyle()
{
	CString str;

	/************************************************************************/
	// init "nest part" grid

	// Requires one never uses column 0
	m_nestPartList.InsertHiddenLabelColumn();

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol1 = new CGridColumnTraitText;
	m_nestPartList.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol1);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTNAME);
	CGridColumnTrait* pNameCo2 = new CGridColumnTraitText;
	m_nestPartList.InsertColumnTrait(2, str, LVCFMT_LEFT, 160, 0, pNameCo2);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NESTMGR_NEST_PRIORITY);
	CGridColumnTrait* pNameCo3 = new CGridColumnTraitText;
	m_nestPartList.InsertColumnTrait(3, str, LVCFMT_LEFT, 120, 0, pNameCo3);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_SUBMITTED_NUM);
	CGridColumnTrait* pNameCo4 = new CGridColumnTraitText;
	m_nestPartList.InsertColumnTrait(4, str, LVCFMT_LEFT, 140, 0, pNameCo4);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_NESTED_NUM);
	CGridColumnTrait* pNameCo5 = new CGridColumnTraitText;
	m_nestPartList.InsertColumnTrait(5, str, LVCFMT_LEFT, 140, 0, pNameCo5);
	/************************************************************************/

	/************************************************************************/
	// init "material size" grid

	// Requires one never uses column 0
	m_nestMatList.InsertHiddenLabelColumn();

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	pNUMCol1 = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(1, str, LVCFMT_LEFT, 60, 0, pNUMCol1);

	// "length"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_LENGTH);
	CGridColumnTrait* pLengthCol = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(2, str, LVCFMT_LEFT, 100, 0, pLengthCol);

	// "width"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_WIDTH);
	CGridColumnTrait* pWidthCol = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(3, str, LVCFMT_LEFT, 100, 0, pWidthCol);

	// "matsize type" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_TYPE);
	CGridColumnTrait* pMatSizeTypeCol = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(4, str, LVCFMT_LEFT, 160, 0, pMatSizeTypeCol);

	// set other style.
	m_nestMatList.SetExtendedStyle(LVS_EX_CHECKBOXES | m_nestMatList.GetExtendedStyle());
	m_nestMatList.EnableVisualStyles(TRUE);
	/************************************************************************/
}

void NestJobInfoDlg::InitListData()
{
	NestTaskPtr pNestTask = m_pNestJob->GetNestTask();

	/************************************************************************/
	// init the "scheme info" group.

	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();

	// get the machine name.
	LONGLONG iParamConfigID = pNestTask->GetParamConfigID();
	LONGLONG iExpLibItemID = ParamConfigLoader::GetExpLibItemID(pExpLibDBCon, iParamConfigID);
	m_strExpLibName = ExpertLibItemLoader::GetExpLibItemName(pExpLibDBCon, iExpLibItemID);
	if (m_strExpLibName == EMPTY_STRING)
		m_strExpLibName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);

	m_strParamConfig = ParamConfigLoader::GetConfigName(pExpLibDBCon, iParamConfigID);
	if (m_strParamConfig == EMPTY_STRING)
		m_strParamConfig = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
	/************************************************************************/

	// update the "nest part" grid.
    map<LONGLONG, map<LONGLONG, int>> partCountDataInSheets; // 板材上不同零件的数量信息。map<板材id, map<零件id, 零件数量>>
	NestPartListPtr pNestPartList = pNestTask->GetNestPartList();
	for (unsigned int i = 0; i < pNestPartList->size(); i++)
	{
		NestPartPtr pNestPart = pNestPartList->at(i);

		// insert a new row.
		int iCount = m_nestPartList.GetItemCount();
		m_nestPartList.InsertItem(iCount, _T(""));

		// "num" column
		CString strNum;
		strNum.Format(INT_FORMAT, iCount+1);
		m_nestPartList.SetItemText(iCount, 1, strNum);

		// the part name column
		CString strPartName;
		strPartName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		const PartItem* pPartItem = pNestPart->GetPartItem();
		if (pPartItem)
			strPartName = pPartItem->GetPartName();
		m_nestPartList.SetItemText(iCount, 2, strPartName);

		// the part priority column
		CString strPartPriority;
		strPartPriority.Format(INT_FORMAT, pNestPart->GetPriority().m_iNestPriority);
		m_nestPartList.SetItemText(iCount, 3, strPartPriority);

		// the part count column
		CString strPartCount;
		strPartCount.Format(INT_FORMAT, pNestPart->GetNestCount());
		m_nestPartList.SetItemText(iCount, 4, strPartCount);

		// the nested count.
		CString strNestedCount;
		strNestedCount = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		if (pPartItem)
        {
			strNestedCount.Format(INT_FORMAT, NestJobMgr::GetNestedPartCount_fromDB(m_pNestJob.get(), pPartItem->GetPartID(), partCountDataInSheets));
        }
		m_nestPartList.SetItemText(iCount, 5, strNestedCount);
	}

	// update "m_nestMatList" grid.
	NestMaterialListPtr pNestMatList = pNestTask->GetNestMatList();
	for (unsigned int k = 0; k < pNestMatList->size(); k++)
	{
		NestMaterialPtr pNestMat = pNestMatList->at(k);

		// insert a new row.
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = k;
		lvItem.iSubItem = 0;
		lvItem.pszText = _T("");
		m_nestMatList.InsertItem(&lvItem);

		// "num" column
		CString strNum;
		strNum.Format(INT_FORMAT, k+1);
		m_nestMatList.SetItemText(k, 1, strNum);

		if (pNestMat)
		{
			IMaterialSizePtr pMaterialSize = pNestMat->GetMatSize();

			// "material width" column
			CString strMatLen;
			strMatLen.Format(FLOAT_FORMAT_2, pMaterialSize->GetWidth());
			m_nestMatList.SetItemText(k, 2, strMatLen);

			// "material height" column
			CString strMatWidth;
			strMatWidth.Format(FLOAT_FORMAT_2, pMaterialSize->GetHeight());
			m_nestMatList.SetItemText(k, 3, strMatWidth);

			// "matsize type" column
			CString strMatSizeType;
			if (pMaterialSize->GetType() == MAT_SIZE_RECT)
				strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_RECT);
			else if (pMaterialSize->GetType() == MAT_SIZE_POLY)
				strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_POLY);
			m_nestMatList.SetItemText(k, 4, strMatSizeType);
		}
		else
		{
			// "material width" column
			CString str;
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
			m_nestMatList.SetItemText(k, 2, str);
			m_nestMatList.SetItemText(k, 3, str);
			m_nestMatList.SetItemText(k, 4, str);
		}
	}
}

END_CUTLEADER_NAMESPACE()
