#include "stdafx.h"
#include "NestScheduleDlg.h"

#include <tuple>
#include "IntegerList.h"
#include "clUtilityResource.h"
#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "DateHelper.h"
#include "PathHelper.h"
#include "clSecureResource.h"
#include "RandomGen.h"

#include "ProgressData.h"
#include "ProgressDlg.h"
#include "ImgData.h"
#include "baseConst.h"
#include "clUILibResource.h"
#include "Rect2D.h"
#include "LineArc2DList.h"
#include "clBaseDataResource.h"
#include "ProductFolderManager.h"
#include "DataBaseManager.h"
#include "LogMgr.h"
#include "MultiLanguageMgr.h"
#include "ProductFileManager.h"

#include "ProductFolderDlg.h"
#include "FolderItem.h"
#include "ProductStructProcessor.h"
#include "ProductItemList.h"
#include "ProductItemLoader.h"
#include "NestPartSelectDlg.h"

#include "clGeometryFeatureResource.h"
#include "PatternList.h"
#include "PatternLoopList.h"
#include "LoopToolData.h"
#include "LoopCutFeature.h"
#include "LoopProcessor.h"
#include "clPartResource.h"
#include "PartCadData.h"
#include "PartCamData.h"
#include "PartCamDataList.h"
#include "LoopCutFeatureLoader.h"
#include "PartItem.h"
#include "Part.h"
#include "PartItemList.h"
#include "PartManager.h"
#include "PartBuilder.h"
#include "PartLoader.h"
#include "PartWriter.h"
#include "Loop2PartDlg.h"

#include "NestTask.h"
#include "NestPart.h"
#include "NestMaterial.h"
#include "NestMaterialList.h"
#include "NestPartList.h"
#include "NestMatSelectDlg.h"
#include "clNestResource.h"
#include "NestResultDlg.h"
#include "CreateStdPartDlg.h"
#include "NestInfoDlg.h"
#include "ImpXlsPartDlg.h"
#include "NestJobSelDlg.h"
#include "NestJob.h"
#include "NestProcessor.h"
#include "LibraryData.h"
#include "AreaInfo.h"

#include "clDataImportExportResource.h"
#include "ExtImpDataSelDlg.h"
#include "ImportManager.h"

#include "IMaterialSize.h"
#include "MaterialSizeList.h"
#include "MaterialLibLoader.h"
#include "clMaterialLibResource.h"
#include "MaterialProcessor.h"

#include "MachineItemLoader.h"
#include "clMachineLibResource.h"

#include "ParamConfig.h"
#include "ParamConfigSelectDlg.h"
#include "ParamConfigLoader.h"
#include "DefaultParamConfig.h"
#include "ExpertLibItemLoader.h"
#include "clExpertLibResource.h"
#include "ParamConfigDlg.h"
#include "NestConfigItem.h"
#include "ConfigItemLoader.h"

using namespace std;
using namespace std::tr1;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(NestScheduleDlg, CDialogEx)

BEGIN_MESSAGE_MAP(NestScheduleDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()

	ON_CBN_SELCHANGE(IDC_COMBO_EXPLIB_ITEM, OnSelchangeExpLib)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAM_CONFIG, OnSelchangeParam)
	ON_BN_CLICKED(IDC_BTN_EDIT_PARAM, OnEditParam)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_NEST_PART, OnNestPartListchanged)
	ON_BN_CLICKED(IDC_BTN_ADD_FROM_DB, OnAddPartFromDB)
	ON_BN_CLICKED(IDC_BTN_IMP_EXT_PART, OnImpExtPart)
	ON_BN_CLICKED(IDC_BTN_ADD_STD_PART_2, OnAddStdPart)
	ON_BN_CLICKED(IDC_BTN_NEST_INFO, OnSetNestInfo)
	ON_BN_CLICKED(IDC_BTN_DELETE_PART, OnRemoveFromNestList)

	ON_BN_CLICKED(IDC_BTN_ADD_MAT, OnAddMat)
	ON_BN_CLICKED(IDC_BTN_DELETE_MAT, OnDeleteMat)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_NEST_MAT, OnNestMatListchanged)

	ON_BN_CLICKED(IDC_BTN_LOAD_NEST_JOB, OnLoadNestJob)
	ON_BN_CLICKED(IDC_BTN_EXECUTE_NEST, OnExecuteNest)
END_MESSAGE_MAP()

NestScheduleDlg::NestScheduleDlg(CWnd* pParent /*=NULL*/)
		: CLDialog(NestScheduleDlg::IDD, pParent)
{
	m_iExpLibItemID = INVALID_ID;
	m_iParamConfigID = INVALID_ID;
	m_bFinishAddNestPart = TRUE;
}

NestScheduleDlg::~NestScheduleDlg()
{
}

void NestScheduleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// “参数配置”分组。
	DDX_Control(pDX, IDC_COMBO_EXPLIB_ITEM, m_comboExpLibItem);
	DDX_Control(pDX, IDC_COMBO_PARAM_CONFIG, m_comboParamConfig);

	// 零件
	DDX_Control(pDX, IDC_LIST_NEST_PART, m_nestPartList);
	DDX_Control(pDX, IDC_STATIC_PREVIEW_NESTPART, m_partPreviewWnd);
	DDX_Control(pDX, IDC_BTN_IMP_EXT_PART, m_ctrlImpExtPart);
	DDX_Control(pDX, IDC_BTN_ADD_FROM_DB, m_ctrlAddFromDB);
	DDX_Control(pDX, IDC_BTN_ADD_STD_PART_2, m_ctrlAddStdPart);
	DDX_Control(pDX, IDC_BTN_NEST_INFO, m_ctrlNestInfo);

	// 材料
	DDX_Control(pDX, IDC_LIST_NEST_MAT, m_nestMatList);
	DDX_Control(pDX, IDC_STATIC_PREVIEW_MAT, m_matPreviewWnd);
	DDX_Control(pDX, IDC_BTN_ADD_MAT, m_ctrlAddMat);

	// “加载排版作业”按钮。
	DDX_Control(pDX, IDC_BTN_LOAD_NEST_JOB, m_ctrlLoadNestJob);

	// 排版按钮。
	DDX_Control(pDX, IDC_BTN_EXECUTE_NEST, m_ctrlExecuteNest);
}

BOOL NestScheduleDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// 按钮提示。
	CString strTips;
	strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ADD_PART_FROM_DXF);
	m_ctrlImpExtPart.SetTooltip(strTips);
	strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ADD_PART_FROM_DB);
	m_ctrlAddFromDB.SetTooltip(strTips);
	strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ADD_MAT_FROM_DB);
	m_ctrlAddMat.SetTooltip(strTips);
	strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SET_NEST_INFO_TIP);
	m_ctrlNestInfo.SetTooltip(strTips);
	strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_RENEST_NEST_JOB);
	m_ctrlLoadNestJob.SetTooltip(strTips);

	// 初始化按钮菜单。
	m_btnMenu.CreatePopupMenu();
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ADD_PART_FROM_ASM_DXF);
	m_btnMenu.AppendMenu(MF_STRING, IDC_BTN_ADD_ASM_DXF, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMP_RECT_PART_FROM_XLS);
	m_btnMenu.AppendMenu(MF_STRING, IDC_BTN_IMP_STD_XLS, str.AllocSysString());
	m_ctrlImpExtPart.m_hMenu = m_btnMenu.GetSafeHmenu();
	m_ctrlImpExtPart.m_bStayPressed = TRUE;
	m_ctrlImpExtPart.m_bDefaultClick = TRUE;

	// 初始化“参数配置”分组。
	InitParamGroup();

	// 初始化表格样式。
	InitPartGridStyle();
	InitMatGridStyle();

	// 初始化视口。
	Rect2D geomRect(.0, 10.0, .0, 10.0);
	m_matPreviewWnd.InitViewPort(geomRect);

	// 添加默认的材料。
	AddDflMat();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void NestScheduleDlg::OnDestroy()
{
	RlsExpLibData();
	RlsParamData();
	RlsPartGridData();
	RlsMatGridData();

	// 释放视口。
	m_matPreviewWnd.ReleaseViewPort();

	CDialogEx::OnDestroy();
}

void NestScheduleDlg::OnPaint()
{
	CDialogEx::OnPaint();

	PreviewNestPart();
	PreviewNestMat();
}

HBRUSH NestScheduleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_NESTPART || pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_MAT)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void NestScheduleDlg::OnSelchangeExpLib()
{
	int iIndex = m_comboExpLibItem.GetCurSel();
	if (iIndex != INVALID_IDX)
	{
		LONGLONG* pExpLibID = (LONGLONG*)m_comboExpLibItem.GetItemData(iIndex);
		m_iExpLibItemID = *pExpLibID;

		// 初始化“参数配置”组合框。
		RlsParamData();
		m_comboParamConfig.ResetContent();
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		vector<DataProperty> paramConfigList = ParamConfigLoader::GetParamConfigList(pExpLibDBCon, m_iExpLibItemID);
		for (unsigned int i = 0; i < paramConfigList.size(); i++)
		{
			DataProperty prop = paramConfigList[i];
			LONGLONG iParamConfigID = prop.GetItem(0).GetValue().llVal;
			DataPropItem nameProp = prop.GetItem(2);
			m_comboParamConfig.AddString((LPCTSTR)nameProp.GetValue().bstrVal);
			m_comboParamConfig.SetItemData(i, (DWORD_PTR)(new LONGLONG(iParamConfigID)));
		}
		m_comboParamConfig.SetCurSel(0);
		m_iParamConfigID = paramConfigList[0].GetItem(0).GetValue().llVal;
	}
}

void NestScheduleDlg::OnSelchangeParam()
{
	int iIndex = m_comboParamConfig.GetCurSel();
	if (iIndex != INVALID_IDX)
	{
		LONGLONG* pParamConfigID = (LONGLONG*)m_comboParamConfig.GetItemData(iIndex);
		m_iParamConfigID = *pParamConfigID;
	}
}

void NestScheduleDlg::OnEditParam()
{
	ParamConfigDlg dlg(this);
	dlg.Init(m_iParamConfigID, 7);
	dlg.DoModal();
}

void NestScheduleDlg::OnAddPartFromDB()
{
	NestPartSelectDlg dlg;
	dlg.Init(m_pWorkspace);
	if (dlg.DoModal() == IDOK)
	{
		// 得到选中的零件。
		IntegerListPtr pSelPartIDList = dlg.GetSelectedPart();

		// 禁止消息。
		m_bFinishAddNestPart = FALSE;

		// 逐个加入选中的零件。
		for (unsigned int i = 0; i < pSelPartIDList->size(); i++)
		{
			LONGLONG iPartID = pSelPartIDList->at(i);

			// 检查是否添加了这个零件。
			if (PartAdded(iPartID))
				continue;

            // 检查这个零件有没有和当前参数配置对应的工艺数据。
            LONGLONG iCamDataID = CheckPart(m_iParamConfigID, iPartID);

			// 插入一行。
			int iRowCount = m_nestPartList.GetItemCount();
			m_nestPartList.InsertItem(iRowCount, _T(""));
			m_nestPartList.SetItemData(iRowCount, (DWORD_PTR)(new LONGLONG(iPartID)));

			// 序号。
			CString strNum;
			if ((iRowCount+1) < 10)
				strNum.Format(_T("0%d"), iRowCount+1);
			else
				strNum.Format(_T("%d"), iRowCount+1);
			m_nestPartList.SetItemText(iRowCount, 1, strNum);

			// 零件名称。
			DataProperty partProp;
			DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
			ProductItemLoader::GetDataProp(pMainDBCon, iPartID, partProp);
			CString strPartName = (LPCTSTR)partProp.GetItem(0).GetValue().bstrVal;
			m_nestPartList.SetItemText(iRowCount, 2, strPartName);

			// 零件尺寸。
			DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(iPartID);
			PartCadDataPtr pPartCadData = PartLoader::LoadPartCadData(pPartDBCon);
			Rect2D outerRect = pPartCadData->GetPatternLoopList()->GetRectBox();
			CString strPartSize;
			strPartSize.Format(_T("%.3f * %.3f"), outerRect.GetWidth(), outerRect.GetHeight());
			m_nestPartList.SetItemText(iRowCount, 3, strPartSize);

			// 排版优先级。
			CString strPartPriority;
			strPartPriority.Format(INT_FORMAT, NestPriority::MaxVal());
			m_nestPartList.SetItemText(iRowCount, 4, strPartPriority);

			// 排版个数。
			CString strPartCount;
			strPartCount.Format(INT_FORMAT, 1);
			m_nestPartList.SetItemText(iRowCount, 5, strPartCount);

			// 旋转角度。
			CString strRotAng;
			strRotAng = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_FREE);
			m_nestPartList.SetItemText(iRowCount, 6, strRotAng);

            // 外轮廓刀具信息
            IPatternLoopPtr pBoundaryPatternLoop = pPartCadData->GetPatternLoopList()->GetBoundaryLoopEx();
            if (!pBoundaryPatternLoop)
            {
                LogMgr::DebugWarn(_T("NestScheduleDlg::OnAddPartFromDB want add a part without the closed boundary loop!"));
            }
            tuple<CString, double> toolInfo = LoopCutFeatureLoader::LoadToolInfo_of_loop(pPartDBCon, iCamDataID, pBoundaryPatternLoop->GetID());
            CString strToolInfo;
            strToolInfo.Format(_T("%.3f(%s)"), get<1>(toolInfo), get<0>(toolInfo));
            m_nestPartList.SetItemText(iRowCount, 7, strToolInfo);
		}

		// 打开消息。
		m_bFinishAddNestPart = TRUE;

		/************************************************************************/
		// select the last row.

		int iCount = m_nestPartList.GetItemCount();

		// de-select the selected rows.
		for (int i = 0; i < iCount; i++)
		{
			if (m_nestPartList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
				m_nestPartList.SetItemState(i, 0, LVIS_SELECTED);
		}

		// select the last row.
		m_nestPartList.EnsureVisible(iCount-1, FALSE);
		m_nestPartList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
		m_nestPartList.SetFocus();
		/************************************************************************/
	}
}

void NestScheduleDlg::OnImpExtPart()
{
	// 准备数据。
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	FolderNodePtr pWorksapce = ProductItemLoader::LoadProductTree(pMainDBCon);

	switch (m_ctrlImpExtPart.m_nMenuResult)
	{
	case IDC_BTN_ADD_ASM_DXF:
		{
			// 选择要导入的dxf/dwg文件。
			CString strFilter, strDflExt;
			strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPORT_DXFDWG_FILEEXT);
			ExtImpDataSelDlg impFileDlg(strDflExt, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, strFilter);
			impFileDlg.SetSavePath(pWorksapce, TRUE);
			impFileDlg.SetExpLibItem(m_iExpLibItemID, FALSE);
            impFileDlg.SetParamConfig(m_iParamConfigID, FALSE);
			if (impFileDlg.DoModal() == IDOK)
			{
				CString strFilePath = impFileDlg.GetPathName();
				CString strName = PathHelper::GetFileName(strFilePath);
				IMPEXP_DATA_TYPE impFileType = impFileDlg.GetImpType(strFilePath);
				LONGLONG iParamConfigID = impFileDlg.GetPartScheme();

                // 得到dxf data
                DxfDataPtr pDxfData = impFileDlg.GetDxfData();
                if (!pDxfData)
                {
                    LogMgr::DebugWarn(_T("Debug warning 1 in OnImpExtPart function!"), this);
					return;
                }

				// 所有导入的零件项。
				PartItemListPtr pPartItemList(new PartItemList);

				// 导入dxf/dwg文件为PartCadData对象，此时不要生成阵列回路，否则会影响拓扑关系识别。
				PartCadDataPtr pPartCadData = ImportManager::DxfDwg2CadData(strFilePath, pDxfData.get(), FALSE);

				LoopTopologyItemListPtr pLoopTopItemList = LoopProcessor::CalcLoopTopStruct(pPartCadData->GetPatternLoopList().get());
                if (!LoopProcessor::NeedToSeperate(pLoopTopItemList.get()))
				{
					// 利用“零件cad数据”和零件方案来构建零件对象。
					PartPtr pPart = PartBuilder::BuildPart(strName, pPartCadData, iParamConfigID, TRUE);
					PartItemPtr pPartItem(new PartItem(pPart->GetID(), strName, pPartCadData, pPart->GetCamDataList()->at(0)));
					pPartItemList->push_back(pPartItem);
				}
				else
				{
					// 这个对话框用于从回路识别零件。
					Loop2PartDlg dlg;
					dlg.Init(pLoopTopItemList, strName, FALSE);
					if (dlg.DoModal() == IDC_BTN_ACCEPT)
					{
						// 得到识别出的零件集合。
						BOOL bRecognizeSamePart = false;
						vector<pair<CString, PatternLoopListPtr> > pPartList = dlg.GetRecognizedParts(bRecognizeSamePart);

						// 创建所有“零件项”。
						for (unsigned int i = 0; i < pPartList.size(); i++)
						{
							PatternListPtr pPatList = pPartList.at(i).second->GetPatList();
							PartCadDataPtr pPartCadData(new PartCadData(pPatList, pPartList.at(i).second));
							PartPtr pPart = PartBuilder::BuildPart(pPartList.at(i).first, pPartCadData, iParamConfigID, TRUE);
							PartItemPtr pPartItem(new PartItem(pPart->GetID(), pPart->GetName(), pPartCadData, pPart->GetCamDataList()->at(0)));
							pPartItemList->push_back(pPartItem);
						}
					}
					else
						return;
				}

				// 保存零件到数据库。
				for (unsigned int i = 0; i < pPartItemList->size(); i++)
				{
					PartItemPtr pPartItem = pPartItemList->at(i);
					PartPtr pPart(new Part(pPartItem->GetPartID(), pPartItem->GetPartName(), pPartItem->GetPartComment(), pPartItem->GetCadData(), pPartItem->GetCamData()));
					pPart->SetCreateTime(DateHelper::GetCurrentTime());
					pPart->SetModifyTime(DateHelper::GetCurrentTime());
					PartManager::SaveNewPart(pPart.get(), impFileDlg.GetSavePath());
				}

				// 禁止消息。
				m_bFinishAddNestPart = FALSE;

				// 逐个加入选中的零件。
				for (unsigned int i = 0; i < pPartItemList->size(); i++)
				{
					PartItemPtr pPartItem = pPartItemList->at(i);

					// 检查是否添加了这个零件。
					if (PartAdded(pPartItem->GetPartID()))
						continue;

					// 插入一行。
					AddNestPartToGrid(pPartItem.get(), NestPriority::MaxVal(), 1, PART_ROT_FREE);
				}

				// 打开消息。
				m_bFinishAddNestPart = TRUE;

				/************************************************************************/
				// select the last row.

				int iCount = m_nestPartList.GetItemCount();

				// de-select the selected rows.
				for (int i = 0; i < iCount; i++)
				{
					if (m_nestPartList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
						m_nestPartList.SetItemState(i, 0, LVIS_SELECTED);
				}

				// select the last row.
				m_nestPartList.EnsureVisible(iCount-1, FALSE);
				m_nestPartList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
				m_nestPartList.SetFocus();
				/************************************************************************/
			}
		}
		break;

	case IDC_BTN_IMP_STD_XLS:
		{
			ImportRectPart();
		}
		break;

	default:
		{
			CString strFileName, strFilter, strDflExt;
			strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPORT_DXFDWG_FILEEXT);
			ExtImpDataSelDlg impFileDlg(strDflExt, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT, strFilter);
			int iBufferSize = 100*1000*MAX_PATH;
			impFileDlg.m_ofn.lpstrFile = strFileName.GetBuffer(iBufferSize);
			impFileDlg.m_ofn.nMaxFile = iBufferSize;
			impFileDlg.SetSavePath(pWorksapce, TRUE);
			impFileDlg.SetExpLibItem(m_iExpLibItemID, FALSE);
            impFileDlg.SetParamConfig(m_iParamConfigID, FALSE);
			if (impFileDlg.DoModal() == IDOK)
			{
				// 所有导入的零件项。
				PartItemListPtr pPartItemList(new PartItemList);

                // 得到选中的文件数
                int iSelectedFileCount = 0;
                {
                    POSITION pos = impFileDlg.GetStartPosition();
                    while (pos != NULL)
                    {
                        iSelectedFileCount++;
                        impFileDlg.GetNextPathName(pos);
                    }
                }

                // 加载参数配置
                LONGLONG iParamConfigID = impFileDlg.GetPartScheme();
                ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(DataBaseManager::GetInstance()->GetExpLibDBConnect(), iParamConfigID);

                // 遍历每个文件
				POSITION pos = impFileDlg.GetStartPosition();
				while (pos != NULL)
				{
					CString strFilePath = impFileDlg.GetNextPathName(pos);
					IMPEXP_DATA_TYPE impType = impFileDlg.GetImpType(strFilePath);

					// 导入dxf/dwg文件为PartCadData对象，检查是否有封闭的外边框。
					PartCadDataPtr pPartCadData;
                    if (iSelectedFileCount == 1)
                    {
                        pPartCadData = ImportManager::DxfDwg2CadData(strFilePath, impFileDlg.GetDxfData().get(), FALSE);
                    }
                    else
                    {
                        pPartCadData = ImportManager::DxfDwg2CadData(strFilePath, impType == IMPEXP_DATA_DWG, FALSE);
                    }
					if (pPartCadData->GetPatternLoopList()->GetBoundaryLoop() == INVALID_IDX)
					{
						CString strProductName, str;
						strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
						str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NEST_OPEN_BOUNDARY);
						str.Replace(_T("WILLBEREPLACED1"), strFilePath);
						MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
						continue;
					}
					else
					{
						PartPtr pPart = PartBuilder::BuildPart(PathHelper::GetFileName(strFilePath), pPartCadData, pParamConfig.get(), TRUE);
						PartItemPtr pPartItem(new PartItem(pPart->GetID(), pPart->GetName(), pPartCadData, pPart->GetCamDataList()->at(0)));
						pPartItemList->push_back(pPartItem);
					}
				}

				// 保存零件到数据库。
                vector<PartPtr> newPartList;
				for (unsigned int i = 0; i < pPartItemList->size(); i++)
				{
					PartItemPtr pPartItem = pPartItemList->at(i);
					PartPtr pPart(new Part(pPartItem->GetPartID(), pPartItem->GetPartName(), pPartItem->GetPartComment(), pPartItem->GetCadData(), pPartItem->GetCamData()));
					pPart->SetCreateTime(DateHelper::GetCurrentTime());
					pPart->SetModifyTime(DateHelper::GetCurrentTime());
                    newPartList.push_back(pPart);
				}
                PartManager::SaveNewPartList(newPartList, impFileDlg.GetSavePath());

				// 禁止消息。
				m_bFinishAddNestPart = FALSE;

				// 逐个加入选中的零件。
				for (unsigned int i = 0; i < pPartItemList->size(); i++)
				{
					PartItemPtr pPartItem = pPartItemList->at(i);
					AddNestPartToGrid(pPartItem.get(), NestPriority::MaxVal(), 1, PART_ROT_FREE);
				}

				// 打开消息。
				m_bFinishAddNestPart = TRUE;

				/************************************************************************/
				// select the last row.

				int iCount = m_nestPartList.GetItemCount();

				// de-select the selected rows.
				for (int i = 0; i < iCount; i++)
				{
					if (m_nestPartList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
						m_nestPartList.SetItemState(i, 0, LVIS_SELECTED);
				}

				// select the last row.
				m_nestPartList.EnsureVisible(iCount-1, FALSE);
				m_nestPartList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
				m_nestPartList.SetFocus();
				/************************************************************************/
			}
			strFileName.ReleaseBuffer();
		}
		break;
	}
}

void NestScheduleDlg::OnAddStdPart()
{
	CreateStdPart();
}

void NestScheduleDlg::OnSetNestInfo()
{
	// 检查有没有选中零件。
	int iSelCount = 0;
	for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
	{
		if (m_nestPartList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
			iSelCount++;
	}
	if (iSelCount == 0)
	{
		CString strProductName, str;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NEST_INFO);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	// 设置排版数量。
	NestInfoDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		// 排版数量。
		if (dlg.IsEnablePartCount())
		{
			int iNestCount = dlg.GetNestCount();
			for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
			{
				if (m_nestPartList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
				{
					CString strPartCount;
					strPartCount.Format(INT_FORMAT, iNestCount);
					m_nestPartList.SetItemText(i, 5, strPartCount);
				}
			}
		}

		// 旋转角度。
		if (dlg.IsEnableRotAng())
		{
			PART_ROT_STYLE iRotAng = dlg.GetRotAng();
			CString strRotAng = NestPart::GetRotStyleName(iRotAng);
			for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
			{
				if (m_nestPartList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
					m_nestPartList.SetItemText(i, 6, strRotAng);
			}
		}
	}
}

void NestScheduleDlg::OnRemoveFromNestList()
{
	// 禁止消息。
	m_bFinishAddNestPart = FALSE;

	for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
	{
		if (m_nestPartList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			// release item data.
			DWORD_PTR pItemData = m_nestPartList.GetItemData(i);
			delete (LONGLONG*)pItemData;

			// delete the item.
			m_nestPartList.DeleteItem(i);
			i--;
		}
	}

	// re-index the first column.
	for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
	{
		CString strNum;
		if ((i+1) < 10)
			strNum.Format(_T("0%d"), i+1);
		else
			strNum.Format(_T("%d"), i+1);
		m_nestPartList.SetItemText(i, 1, strNum);
	}

	// 打开消息。
	m_bFinishAddNestPart = TRUE;

	// select the last row.
	int iCount = m_nestPartList.GetItemCount();
	if (iCount > 0)
	{
		m_nestPartList.EnsureVisible(iCount-1, FALSE);
		m_nestPartList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
		m_nestPartList.SetFocus();
	}
	else
	{
		PreviewNestPart();
	}
}

void NestScheduleDlg::OnNestPartListchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!m_bFinishAddNestPart)
		return;

	// preview
	PreviewNestPart();

	*pResult = 0;
}

void NestScheduleDlg::OnAddMat()
{
	NestMatSelectDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		// 添加到材料表格。
		MaterialSizeListPtr pMaterialSizeList = dlg.GetSelectedMaterialSizes();
		for (unsigned int i = 0; i < pMaterialSizeList->size(); i++)
		{
			// check whether this material is added.
			IMaterialSizePtr pMaterialSize = pMaterialSizeList->at(i);
			if (MatAdded(pMaterialSize->GetID()))
				continue;

			// insert a new row.
			AddNestMat(pMaterialSize);
		}

		/************************************************************************/
		// select the last row.

		int iCount = m_nestMatList.GetItemCount();

		// de-select the selected rows.
		for (int i = 0; i < iCount; i++)
		{
			if (m_nestMatList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
				m_nestMatList.SetItemState(i, 0, LVIS_SELECTED);
		}

		// select the last row.
		m_nestMatList.EnsureVisible(iCount-1, FALSE);
		m_nestMatList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
		m_nestMatList.SetFocus();
		/************************************************************************/
	}
}

void NestScheduleDlg::OnDeleteMat()
{
	for (int i = 0; i < m_nestMatList.GetItemCount(); i++)
	{
		if (m_nestMatList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			// release item data.
			DWORD_PTR pItemData = m_nestMatList.GetItemData(i);
			delete (LONGLONG*)pItemData;

			// delete the item.
			m_nestMatList.DeleteItem(i);
			i--;
		}
	}

	// re-index the first column.
	for (int i = 0; i < m_nestMatList.GetItemCount(); i++)
	{
		CString strNum;
		if ((i+1) < 10)
			strNum.Format(_T("0%d"), i+1);
		else
			strNum.Format(_T("%d"), i+1);
		m_nestMatList.SetItemText(i, 1, strNum);
	}

	// select the last row.
	int iCount = m_nestMatList.GetItemCount();
	if (iCount > 0)
	{
		m_nestMatList.EnsureVisible(iCount-1, FALSE);
		m_nestMatList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
		m_nestMatList.SetFocus();
	}
	else
	{
		// 清空预览。
		LineArc2DListPtr pGeom2Ds;
		pGeom2Ds.reset();
		m_matPreviewWnd.SetSolidGeom2DList(pGeom2Ds);
		m_matPreviewWnd.SetDashGeom2DList(pGeom2Ds);
		m_matPreviewWnd.OnPaint();
	}
}

void NestScheduleDlg::OnNestMatListchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	// preview
	PreviewNestMat();

	*pResult = 0;
}

void NestScheduleDlg::OnLoadNestJob()
{
	vector<DataProperty> nestJobInfoList;
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	ProductItemLoader::GetDataPropList(pMainDBCon, nestJobInfoList, DATANODE_NESTJOB);
	if (nestJobInfoList.size() > 0)
	{
		NestJobSelDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			NestJobPtr pNestJob = dlg.GetSelectNestJob();
			NestTaskPtr pNestTask = pNestJob->GetNestTask();

			// 禁止消息。
			m_bFinishAddNestPart = FALSE;

			// 初始化零件列表。
			RlsPartGridData();
			m_nestPartList.DeleteAllItems();
			NestPartListPtr pNestParts = pNestTask->GetNestPartList();
			for (unsigned int i = 0; i < pNestParts->size(); i++)
			{
				NestPartPtr pNestPart = pNestParts->at(i);
				AddNestPartToGrid(pNestPart->GetPartItem(), pNestPart->GetPriority(), pNestPart->GetNestCount(), pNestPart->GetRotStyle());
			}

			// 初始化材料列表。
			RlsMatGridData();
			m_nestMatList.DeleteAllItems();
			NestMaterialListPtr pNestMats = pNestTask->GetNestMatList();
			for (unsigned int i = 0; i < pNestMats->size(); i++)
			{
				// 添加一行。
				NestMaterialPtr pNestMat = pNestMats->at(i);
				AddNestMat(pNestMat->GetMatSize());
			}

			// 打开消息。
			m_bFinishAddNestPart = TRUE;

			/************************************************************************/
			// select the last row.

			// 零件列表。
			int iCount = m_nestPartList.GetItemCount();
			m_nestPartList.EnsureVisible(iCount-1, FALSE);
			m_nestPartList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);

			// 材料列表。
			iCount = m_nestMatList.GetItemCount();
			m_nestMatList.EnsureVisible(iCount-1, FALSE);
			m_nestMatList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
			m_nestMatList.SetFocus();
			/************************************************************************/
		}
	}
	else
	{
		CString strProductName, str;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NO_NEST_JOB);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
	}
}

void NestScheduleDlg::OnExecuteNest()
{
	CString strTitle, strMsg;
	strTitle = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// 创建排版任务。
	NestTaskPtr pNestTask(new NestTask);
	{
		pNestTask->SetParamConfigID(m_iParamConfigID);

		// 从列表得到排版零件信息。
		NestPartListPtr pNestPartList = GetNestPartList();
		if (pNestPartList->size() == 0)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NO_NESTPART);
			MessageBox(strMsg, strTitle, MB_ICONWARNING);
			return;
		}
		pNestTask->SetNestPartList(pNestPartList);

		// 从列表得到排版材料信息。
		NestMaterialListPtr pNestMatList = GetNestMatList();
		if (pNestMatList->size() == 0)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NO_NESTMAT);
			MessageBox(strMsg, strTitle, MB_ICONWARNING);
			return;
		}
		pNestTask->SetNestMatList(pNestMatList);
	}

	// 计算零件的面积信息。
	AreaInfo areaInfo;
	{
		map<__int64, double> areaData;
		map<__int64, Polygon2DPtr> partOutPolyData;
		double dAllArea = .0;
		NestPartListPtr pNestPartList = pNestTask->GetNestPartList();
		for (unsigned int i = 0; i < pNestPartList->size(); i++)
		{
			NestPartPtr pNestPart = pNestPartList->at(i);
			pair<double, Polygon2DPtr> area = PartManager::GetPartAreaEx(pNestPart->GetPartItem()->GetCadData());
			areaData[pNestPart->GetPartItem()->GetPartID()] = area.first;
			partOutPolyData[pNestPart->GetPartItem()->GetPartID()] = area.second;
			dAllArea += area.first*pNestPart->GetNestCount();
		}
		areaInfo.partAreaData = areaData;
		areaInfo.partOutPolyData = partOutPolyData;
		areaInfo.dAllPartArea = dAllArea;
	}

	// 准备库信息。
	LibraryDataPtr pLibraryData;
	{
		// 机器信息
		DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		LONGLONG iExpLibItemID = ParamConfigLoader::GetExpLibItemID(pExpLibDBCon, pNestTask->GetParamConfigID());
		LONGLONG iMacItemID = ExpertLibItemLoader::GetMacIDByExpLibItem(pExpLibDBCon, iExpLibItemID);
		MachineParamPtr pMacParam = MachineItemLoader::LoadMacParam(pMacLibDBConnect, iMacItemID);

		// 参数配置。
		NestConfigItemPtr pNestConfigItem = ConfigItemLoader::LoadNestConfigItem(pExpLibDBCon, pNestTask->GetParamConfigID());
		SequenceConfigItemPtr pSeqConfigItem = ConfigItemLoader::LoadSeqConfigItem(pExpLibDBCon, pNestTask->GetParamConfigID());

		// 材料
		MaterialSizeListPtr pMaterialSizeList(new MaterialSizeList);
		for (unsigned int i = 0; i < pNestTask->GetNestMatList()->size(); i++)
		{
			pMaterialSizeList->push_back(pNestTask->GetNestMatList()->at(i)->GetMatSize());
		}

		pLibraryData.reset(new LibraryData(pMacParam, pMaterialSizeList, pNestConfigItem, pSeqConfigItem));
	}

    // 要是启动共边排版，检查参与排版的各个零件的刀具是否一样。
    if (pLibraryData->m_pNestConfigItem->IsEnableComcut() && m_nestPartList.GetItemCount() > 1)
    {
        CString strToolInfo = m_nestPartList.GetItemText(0, 7);
        for (int i = 1; i < m_nestPartList.GetItemCount(); i++)
        {
            CString strTmpToolInfo = m_nestPartList.GetItemText(i, 7);
            if (strTmpToolInfo != strToolInfo)
            {
                strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_TOOLSIZE_WARN);
                MessageBox(strMsg, strTitle, MB_ICONWARNING);
                break;
            }
        }
        
    }

	// 启动排版。
	NestProcessorPtr pNestProcessor(new NestProcessor(pNestTask, areaInfo, pLibraryData));
	pNestProcessor->StartNest();

	// 显示排版过程。
	NestResultDlg nestResultDlg;
	nestResultDlg.Init(pNestTask, pNestProcessor, m_pWorkspace, areaInfo, pLibraryData);
	if (nestResultDlg.DoModal() == IDC_BTN_SAVE)
	{
		m_pNestJob = nestResultDlg.GetNestJob();
		m_pParentFolder = nestResultDlg.GetSaveFolder(m_strName);
		m_bOpenSheet = nestResultDlg.IsOpenSheet();
		EndDialog(IDC_BTN_EXECUTE_NEST);
	}
}

FolderNodePtr NestScheduleDlg::GetSaveFolder(CString& strJobName) const
{
	strJobName = m_strName;
	return m_pParentFolder;
}

void NestScheduleDlg::InitParamGroup()
{
	// 默认的参数配置。
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	DefaultParamConfigPtr pDflParamConfig = ParamConfigLoader::LoadDflParamConfig(pExpLibDBCon);
	m_iExpLibItemID = pDflParamConfig->GetExpLibItemID();
	m_iParamConfigID = pDflParamConfig->GetParamConfigID();

	// 可见的机器。
	DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	IntegerListPtr pVisibleMacList = MachineItemLoader::GetVisibleMacList(pMacLibDBConnect);

	// 初始化“专家库”组合框。
	vector<DataProperty> expLibItemList = ExpertLibItemLoader::GetExpLibItemInfoList(pExpLibDBCon);
	int iCurrentIndex = 0;
	for (unsigned int i = 0; i < expLibItemList.size(); i++)
	{
		DataProperty prop = expLibItemList[i];

		// 检查对应的机器是否可见。
		if (!pVisibleMacList->Include(prop.GetItem(3).GetValue().llVal))
			continue;

		LONGLONG iExpLibItemID = prop.GetItem(0).GetValue().llVal;
		m_comboExpLibItem.AddString((LPCTSTR)prop.GetItem(1).GetValue().bstrVal);
		m_comboExpLibItem.SetItemData(m_comboExpLibItem.GetCount()-1, (DWORD_PTR)(new LONGLONG(iExpLibItemID)));
		if (iExpLibItemID == m_iExpLibItemID)
			iCurrentIndex = m_comboExpLibItem.GetCount()-1;
	}
	m_comboExpLibItem.SetCurSel(iCurrentIndex);

	// 初始化“参数配置”组合框。
	vector<DataProperty> paramConfigList = ParamConfigLoader::GetParamConfigList(pExpLibDBCon, m_iExpLibItemID);
	for (unsigned int i = 0; i < paramConfigList.size(); i++)
	{
		DataProperty prop = paramConfigList[i];
		LONGLONG iParamConfigID = prop.GetItem(0).GetValue().llVal;
		DataPropItem nameProp = prop.GetItem(2);
		m_comboParamConfig.AddString((LPCTSTR)nameProp.GetValue().bstrVal);
		m_comboParamConfig.SetItemData(i, (DWORD_PTR)(new LONGLONG(iParamConfigID)));
		if (iParamConfigID == m_iParamConfigID)
			iCurrentIndex = i;
	}
	m_comboParamConfig.SetCurSel(iCurrentIndex);
}

void NestScheduleDlg::RlsExpLibData()
{
	for (int i = 0; i < m_comboExpLibItem.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_comboExpLibItem.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void NestScheduleDlg::RlsParamData()
{
	for (int i = 0; i < m_comboParamConfig.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_comboParamConfig.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void NestScheduleDlg::InitPartGridStyle()
{
	CString str;

	// Requires one never uses column 0
	m_nestPartList.InsertHiddenLabelColumn();

	// the num column.
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol1 = new CGridColumnTraitText;
	m_nestPartList.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol1);

	// the part name column.
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTNAME);
	CGridColumnTrait* pNameCo2 = new CGridColumnTraitText;
	m_nestPartList.InsertColumnTrait(2, str, LVCFMT_LEFT, 120, 0, pNameCo2);

	// “零件尺寸”列。
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_SIZE);
	CGridColumnTrait* pPartSizeCol = new CGridColumnTraitText;
	m_nestPartList.InsertColumnTrait(3, str, LVCFMT_LEFT, 150, 0, pPartSizeCol);

	// the nest priority column.
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NESTMGR_NEST_PRIORITY);
	CGridColumnTraitCombo* pNameCo3 = new CGridColumnTraitCombo;
	pNameCo3->SetStyle(pNameCo3->GetStyle() | CBS_DROPDOWNLIST);
	StringListPtr pList1 = NestPriority::GetAllPriority();
	for (unsigned int i = 0; i < pList1->size(); i++)
		pNameCo3->AddItem(i, pList1->at(i));
	m_nestPartList.InsertColumnTrait(4, str, LVCFMT_LEFT, 70, 0, pNameCo3);

	// the nest count column.
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COUNT);
	CGridColumnTrait* pNameCo4 = new CGridColumnTraitEdit(INTEDITBOX);
	m_nestPartList.InsertColumnTrait(5, str, LVCFMT_LEFT, 50, 0, pNameCo4);

	// "Rotate Angle" column.
	CGridColumnTraitCombo* pRotAngCol = new CGridColumnTraitCombo;
	pRotAngCol->SetStyle(pRotAngCol->GetStyle() | CBS_DROPDOWNLIST);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_FREE);
	pRotAngCol->AddItem(0, str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_90_INCREMENT);
	pRotAngCol->AddItem(1, str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_0_FIXED);
	pRotAngCol->AddItem(2, str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_90_FIXED);
	pRotAngCol->AddItem(3, str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_180_FIXED);
	pRotAngCol->AddItem(4, str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_270_FIXED);
	pRotAngCol->AddItem(5, str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_HORIZONTAL);
	pRotAngCol->AddItem(6, str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_VERTICAL);
	pRotAngCol->AddItem(7, str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NEST_ROT_ANG);
	m_nestPartList.InsertColumnTrait(6, str, LVCFMT_LEFT, 130, 0, pRotAngCol);

    // “外轮廓刀具信息”列。
    str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NESTPART_OUTER_LOOPTOOL);
    CGridColumnTrait* pBoundaryLoopToolCol = new CGridColumnTraitText;
    m_nestPartList.InsertColumnTrait(7, str, LVCFMT_LEFT, 130, 0, pBoundaryLoopToolCol);
}

void NestScheduleDlg::PreviewNestPart()
{
	// 得到选中项。
	int iIndex = INVALID_IDX;
	for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
	{
		if (m_nestPartList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			iIndex = i;
	}

	// 得到预览数据。
	ImgDataPtr pImgData;
	pImgData.reset();
	if (iIndex != INVALID_IDX)
	{
		LONGLONG* pPartID = (LONGLONG*)m_nestPartList.GetItemData(iIndex);
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(*pPartID);
		pImgData = PartLoader::LoadPreviewData(pPartDBCon);
	}

	// 预览。
	ImgData::DrawImgData(pImgData, &m_partPreviewWnd);
}

LONGLONG NestScheduleDlg::CheckPart(LONGLONG iParamConfigID, LONGLONG iPartID)
{
	// 检查零件是不是有和当前选中的“参数配置”配套的工艺数据。
	LONGLONG iCamDataID, iExpLibItemID;
	if (!PartManager::HasCamDatasetA(iPartID, iParamConfigID, iCamDataID, iExpLibItemID))
	{
		// 创建新的工艺数据并保存。
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(iPartID);
		PartCadDataPtr pPartCadData = PartLoader::LoadPartCadData(pPartDBCon);
		PartCamDataPtr pNewPartCamData = PartBuilder::BuildPartCamData(pPartCadData, iParamConfigID);
		PartWriter::SaveCamData(pPartDBCon, pNewPartCamData);
		iCamDataID = pNewPartCamData->GetID();
	}

	return iCamDataID;
}

void NestScheduleDlg::RlsPartGridData()
{
	for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_nestPartList.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

BOOL NestScheduleDlg::PartAdded(LONGLONG iPartID)
{
	for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
	{
		LONGLONG* pPartID = (LONGLONG*)m_nestPartList.GetItemData(i);
		if (*pPartID == iPartID)
			return TRUE;
	}

	return FALSE;
}

NestPartListPtr NestScheduleDlg::GetNestPartList()
{
	NestPartListPtr pNestPartList(new NestPartList);

	for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
	{
		LONGLONG* pPartID = (LONGLONG*)m_nestPartList.GetItemData(i);

		// 加载零件项。
		LONGLONG iCamDataID = CheckPart(m_iParamConfigID, *pPartID);
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(*pPartID);
		PartItemPtr pPartItem = PartLoader::LoadPartItem(pPartDBCon, *pPartID, iCamDataID, true);

		// 新建“排版零件”。
		NestPartPtr pNestPart(new NestPart(pPartItem));
		pNestPartList->push_back(pNestPart);

		// 排版优先级。
		int iNestPriority = _wtoi(m_nestPartList.GetItemText(i, 4));
		NestPriority nestPriority(iNestPriority);
		pNestPart->SetPriority(nestPriority);

		// 排版个数。
		int iNestCount = _wtoi(m_nestPartList.GetItemText(i, 5));
		pNestPart->SetNestCount(iNestCount);

		/************************************************************************/		
		// 旋转角度。

		CString str1, str2, str3, str4, str5, str6, str7, str8;
		str1 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_FREE);
		str2 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_90_INCREMENT);
		str3 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_0_FIXED);
		str4 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_90_FIXED);
		str5 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_180_FIXED);
		str6 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_270_FIXED);
		str7 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_HORIZONTAL);
		str8 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_VERTICAL);

		PART_ROT_STYLE iPartRotAng = PART_ROT_FREE;
		CString str = m_nestPartList.GetItemText(i, 6);
		if (str == str1)
			iPartRotAng = PART_ROT_FREE;
		else if (str == str2)
			iPartRotAng = PART_ROT_PID2_INCREMENT;
		else if (str == str3)
			iPartRotAng = PART_ROT_ZERO_FIXED;
		else if (str == str4)
			iPartRotAng = PART_ROT_PID2_FIXED;
		else if (str == str5)
			iPartRotAng = PART_ROT_PI_FIXED;
		else if (str == str6)
			iPartRotAng = PART_ROT_PID23_FIXED;
		else if (str == str7)
		{
			iPartRotAng = PART_ROT_HORIZONTAL;
		}
		else if (str == str8)
		{
			iPartRotAng = PART_ROT_VERTICAL;
		}
		pNestPart->SetRotStyle(iPartRotAng);
		/************************************************************************/
	}

	return pNestPartList;
}

void NestScheduleDlg::CreateStdPart()
{
	CreateStdPartDlg dlg;
	dlg.SetConfigParamID(m_iParamConfigID);
	if (dlg.DoModal() == IDOK)
	{
		PartPtr pPart = dlg.GetNewPart();
		FolderNodePtr pFolderNode = dlg.GetSaveFolder();
		int iPartCount = dlg.GetPartCount();

		// 保存零件到数据库。
		PartManager::SaveNewPart(pPart.get(), pFolderNode);

		/************************************************************************/
		// 插入一行。

		int iRowCount = m_nestPartList.GetItemCount();
		m_nestPartList.InsertItem(iRowCount, _T(""));
		m_nestPartList.SetItemData(iRowCount, (DWORD_PTR)(new LONGLONG(pPart->GetID())));

		// 序号。
		CString strNum;
		if ((iRowCount+1) < 10)
			strNum.Format(_T("0%d"), iRowCount+1);
		else
			strNum.Format(_T("%d"), iRowCount+1);
		m_nestPartList.SetItemText(iRowCount, 1, strNum);

		// 零件名称。
		m_nestPartList.SetItemText(iRowCount, 2, pPart->GetName());

		// 零件尺寸。
		PartCadDataPtr pPartCadData = pPart->GetCadData();
		Rect2D outerRect = pPartCadData->GetPatternLoopList()->GetRectBox();
		CString strPartSize;
		strPartSize.Format(_T("%.3f * %.3f"), outerRect.GetWidth(), outerRect.GetHeight());
		m_nestPartList.SetItemText(iRowCount, 3, strPartSize);

		// 排版优先级。
		CString strPartPriority;
		strPartPriority.Format(INT_FORMAT, NestPriority::MaxVal());
		m_nestPartList.SetItemText(iRowCount, 4, strPartPriority);

		// 排版个数。
		CString strPartCount;
		strPartCount.Format(INT_FORMAT, iPartCount);
		m_nestPartList.SetItemText(iRowCount, 5, strPartCount);

		// 旋转角度。
		CString strRotAng;
		strRotAng = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_FREE);
		m_nestPartList.SetItemText(iRowCount, 6, strRotAng);

		// 外轮廓刀具信息
		LoopCutFeaturePtr pBoundaryLoopFeature = pPart->GetCamDataList()->at(0)->GetBoundaryLoopFeature();
		LoopToolDataPtr pLoopTool = pBoundaryLoopFeature->GetLoopTool();
		CString strToolInfo;
		strToolInfo.Format(_T("%.3f(%s)"), pLoopTool->GetToolWidth(), pLoopTool->GetToolName());
		m_nestPartList.SetItemText(iRowCount, 7, strToolInfo);
		/************************************************************************/

		/************************************************************************/
		// select the last row.

		int iCount = m_nestPartList.GetItemCount();

		// de-select the selected rows.
		for (int i = 0; i < iCount; i++)
		{
			if (m_nestPartList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
				m_nestPartList.SetItemState(i, 0, LVIS_SELECTED);
		}

		// select the last row.
		m_nestPartList.EnsureVisible(iCount-1, FALSE);
		m_nestPartList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
		m_nestPartList.SetFocus();
		/************************************************************************/
	}
}

void NestScheduleDlg::ImportRectPart()
{
	ImpXlsPartDlg dlg;
	dlg.SetConfigParamID(m_iParamConfigID);
	if (dlg.DoModal() == IDOK)
	{
		NestPartListPtr pNestParts = dlg.GetNestParts();

		// 禁止消息。
		m_bFinishAddNestPart = FALSE;

		// 插入每行。
		for (unsigned int i = 0; i < pNestParts->size(); i++)
		{
			NestPartPtr pNestPart = pNestParts->at(i);
			AddNestPartToGrid(pNestPart->GetPartItem(), pNestPart->GetPriority(), pNestPart->GetNestCount(), pNestPart->GetRotStyle());
		}

		// 打开消息。
		m_bFinishAddNestPart = TRUE;

		/************************************************************************/
		// select the last row.

		int iCount = m_nestPartList.GetItemCount();

		// de-select the selected rows.
		for (int i = 0; i < iCount; i++)
		{
			if (m_nestPartList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
				m_nestPartList.SetItemState(i, 0, LVIS_SELECTED);
		}

		// select the last row.
		m_nestPartList.EnsureVisible(iCount-1, FALSE);
		m_nestPartList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
		m_nestPartList.SetFocus();
		/************************************************************************/
	}
}

void NestScheduleDlg::AddNestPartToGrid(const PartItem* pPartItem, const NestPriority& iNestPriority, int iCount, PART_ROT_STYLE iRotAng)
{
	int iRowCount = m_nestPartList.GetItemCount();
	m_nestPartList.InsertItem(iRowCount, _T(""));
	m_nestPartList.SetItemData(iRowCount, (DWORD_PTR)(new LONGLONG(pPartItem->GetPartID())));

	// 序号。
	CString strNum;
	if ((iRowCount+1) < 10)
		strNum.Format(_T("0%d"), iRowCount+1);
	else
		strNum.Format(_T("%d"), iRowCount+1);
	m_nestPartList.SetItemText(iRowCount, 1, strNum);

	// 零件名称。
	m_nestPartList.SetItemText(iRowCount, 2, pPartItem->GetPartName());

	// 零件尺寸。
	PartCadDataPtr pPartCadData = pPartItem->GetCadData();
	Rect2D outerRect = pPartCadData->GetPatternLoopList()->GetRectBox();
	CString strPartSize;
	strPartSize.Format(_T("%.3f * %.3f"), outerRect.GetWidth(), outerRect.GetHeight());
	m_nestPartList.SetItemText(iRowCount, 3, strPartSize);

	// 排版优先级。
	CString strPartPriority;
	strPartPriority.Format(INT_FORMAT, iNestPriority);
	m_nestPartList.SetItemText(iRowCount, 4, strPartPriority);

	// 排版个数。
	CString strPartCount;
	strPartCount.Format(INT_FORMAT, iCount);
	m_nestPartList.SetItemText(iRowCount, 5, strPartCount);

	// 旋转角度。
	CString strRotAng = NestPart::GetRotStyleName(iRotAng);
	m_nestPartList.SetItemText(iRowCount, 6, strRotAng);

	// 外轮廓刀具信息
	LoopCutFeaturePtr pBoundaryLoopFeature = pPartItem->GetCamData()->GetBoundaryLoopFeature();
	LoopToolDataPtr pLoopTool = pBoundaryLoopFeature->GetLoopTool();
	CString strToolInfo;
	strToolInfo.Format(_T("%.3f(%s)"), pLoopTool->GetToolWidth(), pLoopTool->GetToolName());
	m_nestPartList.SetItemText(iRowCount, 7, strToolInfo);
}

void NestScheduleDlg::InitMatGridStyle()
{
	CString str;

	// Requires one never uses column 0
	m_nestMatList.InsertHiddenLabelColumn();

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol1 = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol1);

	// "material name" column
	CGridColumnTrait* pMatThickCol = new CGridColumnTraitText;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	m_nestMatList.InsertColumnTrait(2, str, LVCFMT_LEFT, 130, 0, pMatThickCol);

	// "width"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_WIDTH);
	CGridColumnTrait* pWidthCol = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(3, str, LVCFMT_LEFT, 90, 0, pWidthCol);

	// "height"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_HEIGHT);
	CGridColumnTrait* pHeightCol = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(4, str, LVCFMT_LEFT, 90, 0, pHeightCol);

	// "count"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MAT_TOTAL_NUM);
	CGridColumnTrait* pCountCol = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(5, str, LVCFMT_LEFT, 80, 0, pCountCol);

	// "available number"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MAT_AVAILABLE_NUM);
	CGridColumnTrait* pAvailCol = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(6, str, LVCFMT_LEFT, 80, 0, pAvailCol);

	// "matsize type" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_TYPE);
	CGridColumnTrait* pMatSizeTypeCol = new CGridColumnTraitText;
	m_nestMatList.InsertColumnTrait(7, str, LVCFMT_LEFT, 90, 0, pMatSizeTypeCol);
}

void NestScheduleDlg::PreviewNestMat()
{
	// 得到选中项。
	int iIndex = INVALID_IDX;
	for (int i = 0; i < m_nestMatList.GetItemCount(); i++)
	{
		if (m_nestMatList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			iIndex = i;
	}

	if (iIndex != INVALID_IDX)
	{
		LONGLONG* pMatSizeID = (LONGLONG*)m_nestMatList.GetItemData(iIndex);
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, *pMatSizeID);
		if (pMaterialSize)
		{
			// 初始化视口。
			Rect2DPtr pOutRect(new Rect2D(0, pMaterialSize->GetWidth(), 0, pMaterialSize->GetHeight()));
			LineArc2DListPtr pGeom2DList = pOutRect->GetLineS();
			Rect2D geomRect = pGeom2DList->GetRectBox();
			m_matPreviewWnd.FitRect(geomRect);

			// 设置材料外框。
			m_matPreviewWnd.SetSolidGeom2DList(MaterialProcessor::GetBorderLineArcs(pMaterialSize.get()));

			// 绘制材料的已用区域。
			LineArc2DListPtr pUselessRegGeo2D = MaterialProcessor::GetUselessRegions(pMaterialSize.get());
			m_matPreviewWnd.SetDashGeom2DList(pUselessRegGeo2D);

			// 绘制。
			m_matPreviewWnd.OnPaint();
		}
	}
}

void NestScheduleDlg::RlsMatGridData()
{
	for (int i = 0; i < m_nestMatList.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_nestMatList.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

BOOL NestScheduleDlg::MatAdded(LONGLONG iMaterialSizeID)
{
	for (int i = 0; i < m_nestMatList.GetItemCount(); i++)
	{
		LONGLONG* pMatSizeID = (LONGLONG*)m_nestMatList.GetItemData(i);
		if (*pMatSizeID == iMaterialSizeID)
			return TRUE;
	}

	return FALSE;
}

NestMaterialListPtr NestScheduleDlg::GetNestMatList()
{
	NestMaterialListPtr pNestMatList(new NestMaterialList);

	// 遍历材料表格。
	for (int i = 0; i < m_nestMatList.GetItemCount(); i++)
	{
		LONGLONG* pMatSizeID = (LONGLONG*)m_nestMatList.GetItemData(i);

		// 从数据库加载材料。
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, *pMatSizeID);

		// 创建“排版材料”。
		NestMaterialPtr pNestMat(new NestMaterial(pMaterialSize));
		pNestMatList->push_back(pNestMat);
	}

	return pNestMatList;
}

void NestScheduleDlg::AddDflMat()
{
	// 默认的材料。
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, MaterialLibLoader::GetDflMatSize(pMatLibDBCon));

	// insert a new row.
	AddNestMat(pMaterialSize);
	m_nestMatList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}

void NestScheduleDlg::AddNestMat(IMaterialSizePtr pMaterialSize)
{
	int iRowCount = m_nestMatList.GetItemCount();
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_PARAM;
	lvItem.iItem = iRowCount;
	lvItem.iSubItem = 0;
	lvItem.pszText = _T("");
	lvItem.lParam = (DWORD_PTR)(new LONGLONG(pMaterialSize->GetID()));
	m_nestMatList.InsertItem(&lvItem);

	// "num" column
	CString strNum;
	if ((iRowCount+1) < 10)
		strNum.Format(_T("0%d"), iRowCount+1);
	else
		strNum.Format(_T("%d"), iRowCount+1);
	m_nestMatList.SetItemText(iRowCount, 1, strNum);

	// "material name" column
	m_nestMatList.SetItemText(iRowCount, 2, pMaterialSize->GetName());

	// "material width" column
	CString strMatWid;
	strMatWid.Format(FLOAT_FORMAT_4, pMaterialSize->GetWidth());
	m_nestMatList.SetItemText(iRowCount, 3, strMatWid);

	// "material height" column
	CString strMatHeight;
	strMatHeight.Format(FLOAT_FORMAT_4, pMaterialSize->GetHeight());
	m_nestMatList.SetItemText(iRowCount, 4, strMatHeight);

	// "total" column
	CString strMatTotal;
	strMatTotal.Format(INT_FORMAT, pMaterialSize->GetTotalNum());
	m_nestMatList.SetItemText(iRowCount, 5, strMatTotal);

	// "available num" column
	CString strAvailable;
	strAvailable.Format(INT_FORMAT, pMaterialSize->GetAvailNum());
	m_nestMatList.SetItemText(iRowCount, 6, strAvailable);

	// “材料类型”列。
	CString strMatSizeType;
	if (pMaterialSize->GetType() == MAT_SIZE_RECT)
		strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_RECT);
	else if (pMaterialSize->GetType() == MAT_SIZE_POLY)
		strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_POLY);
	m_nestMatList.SetItemText(iRowCount, 7, strMatSizeType);
}

END_CUTLEADER_NAMESPACE()
