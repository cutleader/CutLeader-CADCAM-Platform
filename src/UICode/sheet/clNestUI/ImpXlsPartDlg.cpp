#include "StdAfx.h"
#include "ImpXlsPartDlg.h"

#include "libxl.h"
using namespace libxl;

#include <map>
#include "clUtilityResource.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "PathHelper.h"

#include "baseConst.h"
#include "ProductFolderManager.h"
#include "ProductFileDefine.h"
#include "DataBaseManager.h"
#include "clBaseDataResource.h"
#include "ProductFolderDlg.h"
#include "ProductItemLoader.h"
#include "FolderItem.h"
#include "ExtPropType.h"
#include "ExtPropTypeList.h"
#include "ExtPropVal.h"
#include "ExtPropValList.h"

#include "PatternLoopList.h"
#include "PartCadData.h"
#include "PartItem.h"
#include "PartCamDataList.h"
#include "Part.h"
#include "PartBuilder.h"
#include "PartManager.h"
#include "clDataImportExportResource.h"
#include "NestPart.h"
#include "NestPartList.h"
#include "clNestResource.h"
#include "ImportManager.h"

#include "CLOptionInfo.h"
#include "ImpExpConfig.h"
#include "PartEditParam.h"
#include "CLOptionWriter.h"
#include "CLOptionSheet.h"
#include "LibXlHelper.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(ImpXlsPartDlg, CDialogEx)

ImpXlsPartDlg::ImpXlsPartDlg(CWnd* pParent /*=NULL*/)
		: CLDialog(ImpXlsPartDlg::IDD, pParent)
{
	m_pNestPartList.reset(new NestPartList);
}

ImpXlsPartDlg::~ImpXlsPartDlg(void)
{
}

BEGIN_MESSAGE_MAP(ImpXlsPartDlg, CDialogEx)
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_BTN_SAVE_PATH, OnSelectFolder)
	ON_BN_CLICKED(IDC_BTN_SEL_XLS, OnSelectXlsFile)
	ON_BN_CLICKED(IDC_BTN_DEL_ROW, OnDeleteRow)
	ON_BN_CLICKED(IDC_CHECK_LOAD_FROM_PATH, OnCheckFromFile)
	ON_BN_CLICKED(IDC_BTN_MGR_EXTPROP, OnMgrExtProp)
	ON_BN_CLICKED(IDOK, OnOK)

	ON_MESSAGE(WM_GET_CELL_EDITOR_DATA, GetCellEditorData)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_EXTPROP_MAP, OnEndLabelEdit)
END_MESSAGE_MAP()

void ImpXlsPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_XLS_PATH, m_editXLSPath);
	DDX_Control(pDX, IDC_EDIT_START_ROW, m_editStartRow);
	DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_editSavePath);
	DDX_Control(pDX, IDC_LIST_XLS_DATA, m_listXlsData);
	DDX_Control(pDX, IDC_COMBO_ROT_ANG, m_ctrlRotateAng);

	DDX_Control(pDX, IDC_COMBO_PART_NAME, m_comboPartName);
	DDX_Control(pDX, IDC_COMBO_PART_WIDTH, m_comboPartWidth);
	DDX_Control(pDX, IDC_COMBO_PART_HEIGHT, m_comboPartHeight);
	DDX_Control(pDX, IDC_COMBO_PART_COUNT, m_comboPartCount);
	DDX_Control(pDX, IDC_COMBO_PART_PATH, m_comboPartPath);
	DDX_Check(pDX, IDC_CHECK_LOAD_FROM_PATH, m_bLoadPartFromPath);

	DDX_Control(pDX, IDC_LIST_EXTPROP_MAP, m_listExtProp);
}

BOOL ImpXlsPartDlg::OnInitDialog()
{
	__super::OnInitDialog();

	ImpExpConfigPtr pImpExpConfig = CLOptionInfo::GetInstance()->GetImpExpConfig();
	m_editStartRow.Init(pImpExpConfig->GetStartRow(), 0, false);
	
	// ��ת�Ƕȡ�
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_FREE);
	m_ctrlRotateAng.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_90_INCREMENT);
	m_ctrlRotateAng.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_0_FIXED);
	m_ctrlRotateAng.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_90_FIXED);
	m_ctrlRotateAng.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_180_FIXED);
	m_ctrlRotateAng.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_270_FIXED);
	m_ctrlRotateAng.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_HORIZONTAL);
	m_ctrlRotateAng.AddString(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_VERTICAL);
	m_ctrlRotateAng.AddString(str);
	m_ctrlRotateAng.SetCurSel(2);

	// ��ʼ������Ŀ¼��
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	m_pSaveFolder = ProductItemLoader::LoadProductTree(pMainDBCon);
	m_editSavePath = m_pSaveFolder->GetName();

	// �Ƿ���ļ����������
	m_bLoadPartFromPath = pImpExpConfig->IsLoadPartFromPath();
	if (m_bLoadPartFromPath)
	{
		GetDlgItem(IDC_COMBO_PART_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_PART_WIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_PART_HEIGHT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_PART_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_PART_WIDTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_PART_HEIGHT)->EnableWindow(TRUE);
	}

	/************************************************************************/
	// ��ʼ������չ���ԡ��б�

	// Requires one never uses column 0
	m_listExtProp.InsertHiddenLabelColumn();

	// ���������С�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_listExtProp.InsertColumnTrait(1, str, LVCFMT_LEFT, 90, 0, pNameCol);

	// ���������С�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DATATYPE);
	CGridColumnTrait* pDataTypeCol = new CGridColumnTraitText;
	m_listExtProp.InsertColumnTrait(2, str, LVCFMT_LEFT, 90, 0, pDataTypeCol);

	// excel�С�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COLUMN);
	CGridColumnTrait* pExcelColCol = new CGridColumnTraitText;
	m_listExtProp.InsertColumnTrait(3, str, LVCFMT_LEFT, 80, 0, pExcelColCol);

	// we should decide the cell editor for this column at run-time.
	m_listExtProp.AddDynamicCol(3);

	// ��ʾ��չ���ԡ�
	int nItem = 0;
	PartEditParamPtr pPartEditParam = CLOptionInfo::GetInstance()->GetPartEditParam();
	ExtPropTypeListPtr pExtPropTypes = pPartEditParam->GetExtPropTypeList();
	m_listExtProp.DeleteAllItems();
	for (unsigned int i = 0; i < pExtPropTypes->size(); i++)
	{
		ExtPropTypePtr pExtPropType = pExtPropTypes->at(i);
		nItem = m_listExtProp.InsertItem(++nItem, _T(""));
		m_listExtProp.SetItemText(nItem, 1, pExtPropType->GetName());
		m_listExtProp.SetItemText(nItem, 2, pExtPropType->GetDataTypeName());
		m_listExtProp.SetItemText(nItem, 3, _T(""));
	}
	/************************************************************************/

	UpdateData(FALSE);
	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

void ImpXlsPartDlg::OnSelectFolder()
{
	ProductFolderDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		m_pSaveFolder = dlg.GetSelectFolder();

		// get the path of the folder.
		CString strPath = m_pSaveFolder->GetItemPath();
		strPath += _T("\\");
		strPath += m_pSaveFolder->GetName();

		m_editSavePath = strPath;
		UpdateData(FALSE);
	}
}

HBRUSH ImpXlsPartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_SHT)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void ImpXlsPartDlg::OnSelectXlsFile()
{
	CString strFilter;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPORT_XLS_EXT);
	CFileDialog openFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
	if (openFileDlg.DoModal() == IDOK)
	{
		CString strFilePath = openFileDlg.GetPathName();
		m_editXLSPath = strFilePath;
		UpdateData(FALSE);

		// ��excel�ļ���������ʾ���б��С�
		InitXlsDataList(strFilePath);

		// ��ʼ��ӳ���ϵ��
		if (m_listXlsData.GetItemCount() > 0)
		{
			// ��ʼ�����õ�������
			m_comboPartName.ResetContent();
			m_comboPartWidth.ResetContent();
			m_comboPartHeight.ResetContent();
			m_comboPartCount.ResetContent();
			m_comboPartPath.ResetContent();
			vector<CString> columeNames = GetColumnNames();
			for (unsigned int i = 0; i < columeNames.size(); i++)
			{
				m_comboPartName.AddString(columeNames.at(i));
				m_comboPartWidth.AddString(columeNames.at(i));
				m_comboPartHeight.AddString(columeNames.at(i));
				m_comboPartCount.AddString(columeNames.at(i));
				m_comboPartPath.AddString(columeNames.at(i));
			}

			// ����ѡ�е�������
			ImpExpConfigPtr pImpExpConfig = CLOptionInfo::GetInstance()->GetImpExpConfig();
			if (m_comboPartName.GetCount() >= pImpExpConfig->GetPartNameCol())
				m_comboPartName.SetCurSel(pImpExpConfig->GetPartNameCol()-1);
			else
				m_comboPartName.SetCurSel(0);
			if (m_comboPartWidth.GetCount() >= pImpExpConfig->GetPartWidthCol())
				m_comboPartWidth.SetCurSel(pImpExpConfig->GetPartWidthCol()-1);
			else
				m_comboPartWidth.SetCurSel(0);
			if (m_comboPartHeight.GetCount() >= pImpExpConfig->GetPartHeightCol())
				m_comboPartHeight.SetCurSel(pImpExpConfig->GetPartHeightCol()-1);
			else
				m_comboPartHeight.SetCurSel(0);
			if (m_comboPartCount.GetCount() >= pImpExpConfig->GetPartCountCol())
				m_comboPartCount.SetCurSel(pImpExpConfig->GetPartCountCol()-1);
			else
				m_comboPartCount.SetCurSel(0);
			if (m_comboPartPath.GetCount() >= pImpExpConfig->GetPartPathCol())
				m_comboPartPath.SetCurSel(pImpExpConfig->GetPartPathCol()-1);
			else
				m_comboPartPath.SetCurSel(0);
		}
	}
}

void ImpXlsPartDlg::OnDeleteRow()
{
	for (int i = 0; i < m_listXlsData.GetItemCount(); i++)
	{
		if (m_listXlsData.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			// delete the item.
			m_listXlsData.DeleteItem(i);
			i--;
		}
	}

	// ���������кš�
	for (int i = 0; i < m_listXlsData.GetItemCount(); i++)
	{
		CString strNum;
		if ((i+1) < 10)
			strNum.Format(_T("0%d"), i+1);
		else
			strNum.Format(_T("%d"), i+1);
		m_listXlsData.SetItemText(i, 0, strNum);
	}
}

void ImpXlsPartDlg::OnCheckFromFile()
{
	UpdateData(TRUE);

	if (m_bLoadPartFromPath)
	{
		GetDlgItem(IDC_COMBO_PART_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_PART_WIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_PART_HEIGHT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_PART_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_PART_WIDTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_PART_HEIGHT)->EnableWindow(TRUE);
	}
}

void ImpXlsPartDlg::OnMgrExtProp()
{
	CLOptionSheet clOptionSheet;
	clOptionSheet.InitStartPage(CONFIG_PART_EDIT);
	clOptionSheet.DoModal();

	// ���б�ɾ��һЩ��չ���ԡ�
	PartEditParamPtr pPartEditParam = CLOptionInfo::GetInstance()->GetPartEditParam();
	ExtPropTypeListPtr pExtPropTypes = pPartEditParam->GetExtPropTypeList();
	for (int i = 0; i < m_listExtProp.GetItemCount(); i++)
	{
		CString strExtPropName = m_listExtProp.GetItemText(i, 1);
		if (!pExtPropTypes->GetItemByName(strExtPropName))
		{
			m_listExtProp.DeleteItem(i);
			i--;
		}
	}

	// �����µ���չ���Ե��б�
	for (unsigned int i = 0; i < pExtPropTypes->size(); i++)
	{
		ExtPropTypePtr pExtPropType = pExtPropTypes->at(i);

		// �����չ�����ǲ����¼ӵġ�
		BOOL bExist = FALSE;
		for (int j = 0; j < m_listExtProp.GetItemCount(); j++)
		{
			CString strExtPropName = m_listExtProp.GetItemText(i, 1);
			if (pExtPropType->GetName() == strExtPropName)
			{
				bExist = TRUE;
				break;
			}
		}

		// ������С�
		if (!bExist)
		{
			int nItem = m_listExtProp.GetItemCount();
			nItem = m_listExtProp.InsertItem(++nItem, _T(""));
			m_listExtProp.SetItemText(nItem, 1, pExtPropType->GetName());
			m_listExtProp.SetItemText(nItem, 2, pExtPropType->GetDataTypeName());
			m_listExtProp.SetItemText(nItem, 3, _T(""));
		}
	}
}

void ImpXlsPartDlg::OnOK()
{
	UpdateData(TRUE);
	CString str, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// �б�Ϊ��ʱ��
	if (m_listXlsData.GetItemCount() == 0)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SEL_VALID_XLS);
		MessageBox(str, strProductName, MB_ICONWARNING);
		return;
	}

	// ��ʼ�С�
	int iStartRow = 0;
	if ((int)m_editStartRow.Get_Number() > 0)
		iStartRow = (int)m_editStartRow.Get_Number()-1;

	// �õ�����������ƣ���ȣ��߶ȣ�������
	vector<CString> partNames;
	vector<CString> partPaths;
	vector<double> partWidths, partHeights;
	vector<int> partCounts;
	int iPartNameCol = m_comboPartName.GetCurSel();
	int iPartWidthCol = m_comboPartWidth.GetCurSel();
	int iPartHeightCol = m_comboPartHeight.GetCurSel();
	int iPartCountCol = m_comboPartCount.GetCurSel();
	int iPartPathCol = m_comboPartPath.GetCurSel();
	for (int i = iStartRow; i < m_listXlsData.GetItemCount(); i++)
	{
		CString strRow;
		strRow.Format(_T("%d"), i+1);

		// ������ơ�
		CString strData = m_listXlsData.GetItemText(i, iPartNameCol+1);
		if (strData.GetLength() > 0)
			partNames.push_back(strData);
		else
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_NAME_COL_ERROR);
			str.Replace(_T("WILLBEREPLACED1"), strRow);
			MessageBox(str, strProductName, MB_ICONWARNING);
			return;
		}

		if (m_bLoadPartFromPath)
		{
			// ���·����
			CString strData = m_listXlsData.GetItemText(i, iPartPathCol+1);
			CFileFind fileFind;
			if (fileFind.FindFile(strData))
				partPaths.push_back(strData);
			else
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_PATH_COL_ERROR);
				str.Replace(_T("WILLBEREPLACED1"), strRow);
				MessageBox(str, strProductName, MB_ICONWARNING);
				return;
			}
		}
		else
		{
			// �����ȡ�
			strData = m_listXlsData.GetItemText(i, iPartWidthCol+1);
			double dWidth = _wtof(strData);
			if (dWidth > .0)
				partWidths.push_back(dWidth);
			else
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_WIDTH_COL_ERROR);
				str.Replace(_T("WILLBEREPLACED1"), strRow);
				MessageBox(str, strProductName, MB_ICONWARNING);
				return;
			}

			// ����߶ȡ�
			strData = m_listXlsData.GetItemText(i, iPartHeightCol+1);
			double dHeight = _wtof(strData);
			if (dHeight > .0)
				partHeights.push_back(dHeight);
			else
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_HEIGHT_COL_ERROR);
				str.Replace(_T("WILLBEREPLACED1"), strRow);
				MessageBox(str, strProductName, MB_ICONWARNING);
				return;
			}
		}

		// ���������
		strData = m_listXlsData.GetItemText(i, iPartCountCol+1);
		int iCount = _wtoi(strData);
		if (iCount > 0)
			partCounts.push_back(iCount);
		else
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_COUNT_COL_ERROR);
			str.Replace(_T("WILLBEREPLACED1"), strRow);
			MessageBox(str, strProductName, MB_ICONWARNING);
			return;
		}
	}

	// �����û�пɵ�������ݡ�
	if (partNames.size() == 0)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NO_DATA_IMPORT);
		MessageBox(str, strProductName, MB_ICONWARNING);
		return;
	}

	// ����ÿ��������������������������
	vector<ExtPropValListPtr> extPropVals = GetPartExtProps(iStartRow); // �õ������������չ���ԡ�
	for (unsigned int i = 0; i < partNames.size(); i++)
	{
		// ������������档
		PartPtr pPart;
		if (m_bLoadPartFromPath)
		{
			// dxf����dwg�ļ���
			BOOL bDwg = TRUE;
			CString strExt = PathHelper::GetFileExt(partPaths.at(i));
			CString strTmp;
			strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_DXF_EXT);
			if (strExt.CompareNoCase(strTmp) == 0)
				bDwg = FALSE;

			// ���������
			PartCadDataPtr pPartCadData = ImportManager::DxfDwg2CadData(partPaths.at(i), bDwg, FALSE);
			if (pPartCadData->GetPatternLoopList()->GetBoundaryLoop() != INVALID_IDX)
			{
				pPart = PartBuilder::BuildPart(partNames.at(i), pPartCadData, m_iParamConfigID, TRUE);
			}
			else
			{
				continue;
			}
		}
		else
		{
			double dWidth = partWidths.at(i);
			double dHeight = partHeights.at(i);
			pPart = PartBuilder::BuildRectPart(partNames.at(i), dWidth, dHeight, m_iParamConfigID);
		}
		pPart->SetExtPropValList(extPropVals[i]);
		PartManager::SaveNewPart(pPart.get(), m_pSaveFolder, FALSE);

		// �������������
		PartItemPtr pPartItem(new PartItem(pPart->GetID(), partNames.at(i), pPart->GetCadData(), pPart->GetCamDataList()->at(0)));
		NestPartPtr pNestPart(new NestPart(pPartItem));
		pNestPart->SetNestCount(partCounts.at(i));
		pNestPart->SetPriority(NestPriority::MaxVal());
		pNestPart->SetRotStyle((PART_ROT_STYLE)(m_ctrlRotateAng.GetCurSel()));
		m_pNestPartList->push_back(pNestPart);
	}

	// ����ӳ���ϵ��
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	ImpExpConfigPtr pImpExpConfig = pCLOptionInfo->GetImpExpConfig();
	pImpExpConfig->SetStartRow((int)m_editStartRow.Get_Number());
	pImpExpConfig->SetPartNameCol(m_comboPartName.GetCurSel()+1);
	pImpExpConfig->SetPartWidthCol(m_comboPartWidth.GetCurSel()+1);
	pImpExpConfig->SetPartHeightCol(m_comboPartHeight.GetCurSel()+1);
	pImpExpConfig->SetPartCountCol(m_comboPartCount.GetCurSel()+1);
	pImpExpConfig->SetPartPathCol(m_comboPartPath.GetCurSel()+1);
	pImpExpConfig->IsLoadPartFromPath(m_bLoadPartFromPath);
	CString strPath = ProductFolderManager::GetOptionPath();
	strPath += USER_OPTION_FILE;
	CLOptionWriter::SaveCLOption(pCLOptionInfo, strPath);

	CDialogEx::OnOK();
}

LRESULT ImpXlsPartDlg::GetCellEditorData(WPARAM wParam, LPARAM lParam)
{
	CellEditorData* pCellEditorData = reinterpret_cast<CellEditorData*>(wParam);
	if (pCellEditorData->hwnd == m_listExtProp.m_hWnd)
	{
		pCellEditorData->m_cellEditorType = COMBOBOX;
		pCellEditorData->m_optionValList.clear();
		pCellEditorData->m_optionValList.push_back(EMPTY_STRING);
		vector<CString> columeNames = GetColumnNames();
		for (unsigned int i = 0; i < columeNames.size(); i++)
			pCellEditorData->m_optionValList.push_back(columeNames.at(i));
	}

	return true;
}

void ImpXlsPartDlg::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	
	// �ݲ���Ҫ��ʲô��
}

void ImpXlsPartDlg::InitXlsDataList(CString strXlsPath)
{
	// ����б�
	m_listXlsData.DeleteAllItems();
	int iCurColNum = m_listXlsData.GetColumnCount();
	for (int i = 0; i < iCurColNum; i++)
		m_listXlsData.DeleteColumn(0);

	// ����book����
	Book* pBook;
	CString strExt = PathHelper::GetFileExt(strXlsPath);
	if (strExt.CompareNoCase(_T(".xls")) == 0)
		pBook = xlCreateBook();
	else if (strExt.CompareNoCase(_T(".xlsx")) == 0)
		pBook = xlCreateXMLBook();
    LibXlHelper::Init(pBook);

	// ���ز���ʾexcel�ļ������ݡ�
	if (pBook->load(strXlsPath))
	{
		libxl::Sheet* pSheet = pBook->getSheet(0);
		if (pSheet)
		{
			// ���������������
			int iStartColumn = pSheet->firstCol();
			int iColumnCount = pSheet->lastCol();
			int iStartRow = pSheet->firstRow();
			int iRowCount = pSheet->lastRow();

			// ���Num�С�
			int iColIdx = 0;
			CString strNum;
			strNum = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
			m_listXlsData.InsertColumn(iColIdx, strNum, LVCFMT_LEFT, 50);
			iColIdx++;

			// ���ÿ����ͷ��
			for (int i = iStartColumn; i < iColumnCount; i++)
			{
				CString str, strColumnName;
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COLUMN);
				strColumnName.Format(_T("%s %d"), str, i+1);
				m_listXlsData.InsertColumn(iColIdx, strColumnName, LVCFMT_LEFT, 100);
				iColIdx++;
			}

			// ����ÿ�����ݡ�
			int iRow = 0;
			for (int i = iStartRow; i < iRowCount; i++)
			{
				m_listXlsData.InsertItem(iRow, _T(""));

				// �����кš�
				int iCol = 0;
				CString strNum;
				if ((iRow+1) < 10)
					strNum.Format(_T("0%d"), iRow+1);
				else
					strNum.Format(_T("%d"), iRow+1);
				m_listXlsData.SetItemText(iRow, iCol, strNum);
				iCol++;

				// ��һ�е����ݡ�
				for (int j = iStartColumn; j < iColumnCount; j++)
				{
					CString strData;
					CellType iCellType = pSheet->cellType(i, j);
					if (iCellType == CELLTYPE_NUMBER)
						strData.Format(FLOAT_FORMAT_2, pSheet->readNum(i, j));
					else if (iCellType == CELLTYPE_STRING)
						strData = pSheet->readStr(i, j);
					m_listXlsData.SetItemText(iRow, iCol, strData);
					iCol++;
				}

				iRow++;
			}
		}
	}

	pBook->release();
}

vector<CString> ImpXlsPartDlg::GetColumnNames()
{
	vector<CString> columeNames;

	for (int i = 1; i < m_listXlsData.GetColumnCount(); i++)
	{
		CString strColumnName = m_listXlsData.GetColumnHeading(i);
		columeNames.push_back(strColumnName);
	}

	return columeNames;
}

vector<ExtPropValListPtr> ImpXlsPartDlg::GetPartExtProps(int iStartRow)
{
	vector<ExtPropValListPtr> partExtProps;

	// �������еĶ�Ӧ<�������ڼ���>��
	map<CString, int> colNameMap;
	vector<CString> columeNames = GetColumnNames();
	for (unsigned int i = 0; i < columeNames.size(); i++)
		colNameMap[columeNames.at(i)] = i+1; // ��һ���ò��š�

	// ��չ���Ժ��еĶ�Ӧ<��������չ����>��
	map<CString, ExtPropTypePtr> extPropMap;
	PartEditParamPtr pPartEditParam = CLOptionInfo::GetInstance()->GetPartEditParam();
	ExtPropTypeListPtr pExtPropTypes = pPartEditParam->GetExtPropTypeList();
	for (int i = 0; i < m_listExtProp.GetItemCount(); i++)
	{
		CString strCol = m_listExtProp.GetItemText(i, 3);
		if (strCol.GetLength() > 0)
		{
			CString strExtPropName = m_listExtProp.GetItemText(i, 1);
			ExtPropTypePtr pExtPropType = pExtPropTypes->GetItemByName(strExtPropName);
			extPropMap[strCol] = pExtPropType;
		}
	}

	// ÿ���������չ����ֵ��
	for (int i = iStartRow; i < m_listXlsData.GetItemCount(); i++)
	{
		// �������ĸ�����չ���ԡ�
		ExtPropValListPtr pExtPropVals(new ExtPropValList);
		for (map<CString, ExtPropTypePtr>::iterator iter = extPropMap.begin(); iter != extPropMap.end(); iter++)
		{
			CString strCol = iter->first;
			int iColIndex = colNameMap[strCol];

			// ��������ֵ��
			ExtPropValPtr pExtPropVal(new ExtPropVal);
			pExtPropVal->SetExtPropType(iter->second);
			_bstr_t strVal = m_listXlsData.GetItemText(i, iColIndex);
			pExtPropVal->SetVal(strVal);
			pExtPropVals->push_back(pExtPropVal);
		}
		partExtProps.push_back(pExtPropVals);
	}

	return partExtProps;
}

END_CUTLEADER_NAMESPACE()
