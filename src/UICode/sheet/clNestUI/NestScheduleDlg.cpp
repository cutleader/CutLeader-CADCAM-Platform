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

	// ���������á����顣
	DDX_Control(pDX, IDC_COMBO_EXPLIB_ITEM, m_comboExpLibItem);
	DDX_Control(pDX, IDC_COMBO_PARAM_CONFIG, m_comboParamConfig);

	// ���
	DDX_Control(pDX, IDC_LIST_NEST_PART, m_nestPartList);
	DDX_Control(pDX, IDC_STATIC_PREVIEW_NESTPART, m_partPreviewWnd);
	DDX_Control(pDX, IDC_BTN_IMP_EXT_PART, m_ctrlImpExtPart);
	DDX_Control(pDX, IDC_BTN_ADD_FROM_DB, m_ctrlAddFromDB);
	DDX_Control(pDX, IDC_BTN_ADD_STD_PART_2, m_ctrlAddStdPart);
	DDX_Control(pDX, IDC_BTN_NEST_INFO, m_ctrlNestInfo);

	// ����
	DDX_Control(pDX, IDC_LIST_NEST_MAT, m_nestMatList);
	DDX_Control(pDX, IDC_STATIC_PREVIEW_MAT, m_matPreviewWnd);
	DDX_Control(pDX, IDC_BTN_ADD_MAT, m_ctrlAddMat);

	// �������Ű���ҵ����ť��
	DDX_Control(pDX, IDC_BTN_LOAD_NEST_JOB, m_ctrlLoadNestJob);

	// �Ű水ť��
	DDX_Control(pDX, IDC_BTN_EXECUTE_NEST, m_ctrlExecuteNest);
}

BOOL NestScheduleDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// ��ť��ʾ��
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

	// ��ʼ����ť�˵���
	m_btnMenu.CreatePopupMenu();
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ADD_PART_FROM_ASM_DXF);
	m_btnMenu.AppendMenu(MF_STRING, IDC_BTN_ADD_ASM_DXF, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMP_RECT_PART_FROM_XLS);
	m_btnMenu.AppendMenu(MF_STRING, IDC_BTN_IMP_STD_XLS, str.AllocSysString());
	m_ctrlImpExtPart.m_hMenu = m_btnMenu.GetSafeHmenu();
	m_ctrlImpExtPart.m_bStayPressed = TRUE;
	m_ctrlImpExtPart.m_bDefaultClick = TRUE;

	// ��ʼ�����������á����顣
	InitParamGroup();

	// ��ʼ�������ʽ��
	InitPartGridStyle();
	InitMatGridStyle();

	// ��ʼ���ӿڡ�
	Rect2D geomRect(.0, 10.0, .0, 10.0);
	m_matPreviewWnd.InitViewPort(geomRect);

	// ���Ĭ�ϵĲ��ϡ�
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

	// �ͷ��ӿڡ�
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

		// ��ʼ�����������á���Ͽ�
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
		// �õ�ѡ�е������
		IntegerListPtr pSelPartIDList = dlg.GetSelectedPart();

		// ��ֹ��Ϣ��
		m_bFinishAddNestPart = FALSE;

		// �������ѡ�е������
		for (unsigned int i = 0; i < pSelPartIDList->size(); i++)
		{
			LONGLONG iPartID = pSelPartIDList->at(i);

			// ����Ƿ��������������
			if (PartAdded(iPartID))
				continue;

            // �����������û�к͵�ǰ�������ö�Ӧ�Ĺ������ݡ�
            LONGLONG iCamDataID = CheckPart(m_iParamConfigID, iPartID);

			// ����һ�С�
			int iRowCount = m_nestPartList.GetItemCount();
			m_nestPartList.InsertItem(iRowCount, _T(""));
			m_nestPartList.SetItemData(iRowCount, (DWORD_PTR)(new LONGLONG(iPartID)));

			// ��š�
			CString strNum;
			if ((iRowCount+1) < 10)
				strNum.Format(_T("0%d"), iRowCount+1);
			else
				strNum.Format(_T("%d"), iRowCount+1);
			m_nestPartList.SetItemText(iRowCount, 1, strNum);

			// ������ơ�
			DataProperty partProp;
			DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
			ProductItemLoader::GetDataProp(pMainDBCon, iPartID, partProp);
			CString strPartName = (LPCTSTR)partProp.GetItem(0).GetValue().bstrVal;
			m_nestPartList.SetItemText(iRowCount, 2, strPartName);

			// ����ߴ硣
			DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(iPartID);
			PartCadDataPtr pPartCadData = PartLoader::LoadPartCadData(pPartDBCon);
			Rect2D outerRect = pPartCadData->GetPatternLoopList()->GetRectBox();
			CString strPartSize;
			strPartSize.Format(_T("%.3f * %.3f"), outerRect.GetWidth(), outerRect.GetHeight());
			m_nestPartList.SetItemText(iRowCount, 3, strPartSize);

			// �Ű����ȼ���
			CString strPartPriority;
			strPartPriority.Format(INT_FORMAT, NestPriority::MaxVal());
			m_nestPartList.SetItemText(iRowCount, 4, strPartPriority);

			// �Ű������
			CString strPartCount;
			strPartCount.Format(INT_FORMAT, 1);
			m_nestPartList.SetItemText(iRowCount, 5, strPartCount);

			// ��ת�Ƕȡ�
			CString strRotAng;
			strRotAng = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_FREE);
			m_nestPartList.SetItemText(iRowCount, 6, strRotAng);

            // ������������Ϣ
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

		// ����Ϣ��
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
	// ׼�����ݡ�
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	FolderNodePtr pWorksapce = ProductItemLoader::LoadProductTree(pMainDBCon);

	switch (m_ctrlImpExtPart.m_nMenuResult)
	{
	case IDC_BTN_ADD_ASM_DXF:
		{
			// ѡ��Ҫ�����dxf/dwg�ļ���
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

                // �õ�dxf data
                DxfDataPtr pDxfData = impFileDlg.GetDxfData();
                if (!pDxfData)
                {
                    LogMgr::DebugWarn(_T("Debug warning 1 in OnImpExtPart function!"), this);
					return;
                }

				// ���е��������
				PartItemListPtr pPartItemList(new PartItemList);

				// ����dxf/dwg�ļ�ΪPartCadData���󣬴�ʱ��Ҫ�������л�·�������Ӱ�����˹�ϵʶ��
				PartCadDataPtr pPartCadData = ImportManager::DxfDwg2CadData(strFilePath, pDxfData.get(), FALSE);

				LoopTopologyItemListPtr pLoopTopItemList = LoopProcessor::CalcLoopTopStruct(pPartCadData->GetPatternLoopList().get());
                if (!LoopProcessor::NeedToSeperate(pLoopTopItemList.get()))
				{
					// ���á����cad���ݡ�����������������������
					PartPtr pPart = PartBuilder::BuildPart(strName, pPartCadData, iParamConfigID, TRUE);
					PartItemPtr pPartItem(new PartItem(pPart->GetID(), strName, pPartCadData, pPart->GetCamDataList()->at(0)));
					pPartItemList->push_back(pPartItem);
				}
				else
				{
					// ����Ի������ڴӻ�·ʶ�������
					Loop2PartDlg dlg;
					dlg.Init(pLoopTopItemList, strName, FALSE);
					if (dlg.DoModal() == IDC_BTN_ACCEPT)
					{
						// �õ�ʶ�����������ϡ�
						BOOL bRecognizeSamePart = false;
						vector<pair<CString, PatternLoopListPtr> > pPartList = dlg.GetRecognizedParts(bRecognizeSamePart);

						// �������С�������
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

				// ������������ݿ⡣
				for (unsigned int i = 0; i < pPartItemList->size(); i++)
				{
					PartItemPtr pPartItem = pPartItemList->at(i);
					PartPtr pPart(new Part(pPartItem->GetPartID(), pPartItem->GetPartName(), pPartItem->GetPartComment(), pPartItem->GetCadData(), pPartItem->GetCamData()));
					pPart->SetCreateTime(DateHelper::GetCurrentTime());
					pPart->SetModifyTime(DateHelper::GetCurrentTime());
					PartManager::SaveNewPart(pPart.get(), impFileDlg.GetSavePath());
				}

				// ��ֹ��Ϣ��
				m_bFinishAddNestPart = FALSE;

				// �������ѡ�е������
				for (unsigned int i = 0; i < pPartItemList->size(); i++)
				{
					PartItemPtr pPartItem = pPartItemList->at(i);

					// ����Ƿ��������������
					if (PartAdded(pPartItem->GetPartID()))
						continue;

					// ����һ�С�
					AddNestPartToGrid(pPartItem.get(), NestPriority::MaxVal(), 1, PART_ROT_FREE);
				}

				// ����Ϣ��
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
				// ���е��������
				PartItemListPtr pPartItemList(new PartItemList);

                // �õ�ѡ�е��ļ���
                int iSelectedFileCount = 0;
                {
                    POSITION pos = impFileDlg.GetStartPosition();
                    while (pos != NULL)
                    {
                        iSelectedFileCount++;
                        impFileDlg.GetNextPathName(pos);
                    }
                }

                // ���ز�������
                LONGLONG iParamConfigID = impFileDlg.GetPartScheme();
                ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(DataBaseManager::GetInstance()->GetExpLibDBConnect(), iParamConfigID);

                // ����ÿ���ļ�
				POSITION pos = impFileDlg.GetStartPosition();
				while (pos != NULL)
				{
					CString strFilePath = impFileDlg.GetNextPathName(pos);
					IMPEXP_DATA_TYPE impType = impFileDlg.GetImpType(strFilePath);

					// ����dxf/dwg�ļ�ΪPartCadData���󣬼���Ƿ��з�յ���߿�
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

				// ������������ݿ⡣
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

				// ��ֹ��Ϣ��
				m_bFinishAddNestPart = FALSE;

				// �������ѡ�е������
				for (unsigned int i = 0; i < pPartItemList->size(); i++)
				{
					PartItemPtr pPartItem = pPartItemList->at(i);
					AddNestPartToGrid(pPartItem.get(), NestPriority::MaxVal(), 1, PART_ROT_FREE);
				}

				// ����Ϣ��
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
	// �����û��ѡ�������
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

	// �����Ű�������
	NestInfoDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		// �Ű�������
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

		// ��ת�Ƕȡ�
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
	// ��ֹ��Ϣ��
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

	// ����Ϣ��
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
		// ��ӵ����ϱ��
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
		// ���Ԥ����
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

			// ��ֹ��Ϣ��
			m_bFinishAddNestPart = FALSE;

			// ��ʼ������б�
			RlsPartGridData();
			m_nestPartList.DeleteAllItems();
			NestPartListPtr pNestParts = pNestTask->GetNestPartList();
			for (unsigned int i = 0; i < pNestParts->size(); i++)
			{
				NestPartPtr pNestPart = pNestParts->at(i);
				AddNestPartToGrid(pNestPart->GetPartItem(), pNestPart->GetPriority(), pNestPart->GetNestCount(), pNestPart->GetRotStyle());
			}

			// ��ʼ�������б�
			RlsMatGridData();
			m_nestMatList.DeleteAllItems();
			NestMaterialListPtr pNestMats = pNestTask->GetNestMatList();
			for (unsigned int i = 0; i < pNestMats->size(); i++)
			{
				// ���һ�С�
				NestMaterialPtr pNestMat = pNestMats->at(i);
				AddNestMat(pNestMat->GetMatSize());
			}

			// ����Ϣ��
			m_bFinishAddNestPart = TRUE;

			/************************************************************************/
			// select the last row.

			// ����б�
			int iCount = m_nestPartList.GetItemCount();
			m_nestPartList.EnsureVisible(iCount-1, FALSE);
			m_nestPartList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);

			// �����б�
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

	// �����Ű�����
	NestTaskPtr pNestTask(new NestTask);
	{
		pNestTask->SetParamConfigID(m_iParamConfigID);

		// ���б�õ��Ű������Ϣ��
		NestPartListPtr pNestPartList = GetNestPartList();
		if (pNestPartList->size() == 0)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NO_NESTPART);
			MessageBox(strMsg, strTitle, MB_ICONWARNING);
			return;
		}
		pNestTask->SetNestPartList(pNestPartList);

		// ���б�õ��Ű������Ϣ��
		NestMaterialListPtr pNestMatList = GetNestMatList();
		if (pNestMatList->size() == 0)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NO_NESTMAT);
			MessageBox(strMsg, strTitle, MB_ICONWARNING);
			return;
		}
		pNestTask->SetNestMatList(pNestMatList);
	}

	// ��������������Ϣ��
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

	// ׼������Ϣ��
	LibraryDataPtr pLibraryData;
	{
		// ������Ϣ
		DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		LONGLONG iExpLibItemID = ParamConfigLoader::GetExpLibItemID(pExpLibDBCon, pNestTask->GetParamConfigID());
		LONGLONG iMacItemID = ExpertLibItemLoader::GetMacIDByExpLibItem(pExpLibDBCon, iExpLibItemID);
		MachineParamPtr pMacParam = MachineItemLoader::LoadMacParam(pMacLibDBConnect, iMacItemID);

		// �������á�
		NestConfigItemPtr pNestConfigItem = ConfigItemLoader::LoadNestConfigItem(pExpLibDBCon, pNestTask->GetParamConfigID());
		SequenceConfigItemPtr pSeqConfigItem = ConfigItemLoader::LoadSeqConfigItem(pExpLibDBCon, pNestTask->GetParamConfigID());

		// ����
		MaterialSizeListPtr pMaterialSizeList(new MaterialSizeList);
		for (unsigned int i = 0; i < pNestTask->GetNestMatList()->size(); i++)
		{
			pMaterialSizeList->push_back(pNestTask->GetNestMatList()->at(i)->GetMatSize());
		}

		pLibraryData.reset(new LibraryData(pMacParam, pMaterialSizeList, pNestConfigItem, pSeqConfigItem));
	}

    // Ҫ�����������Ű棬�������Ű�ĸ�������ĵ����Ƿ�һ����
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

	// �����Ű档
	NestProcessorPtr pNestProcessor(new NestProcessor(pNestTask, areaInfo, pLibraryData));
	pNestProcessor->StartNest();

	// ��ʾ�Ű���̡�
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
	// Ĭ�ϵĲ������á�
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	DefaultParamConfigPtr pDflParamConfig = ParamConfigLoader::LoadDflParamConfig(pExpLibDBCon);
	m_iExpLibItemID = pDflParamConfig->GetExpLibItemID();
	m_iParamConfigID = pDflParamConfig->GetParamConfigID();

	// �ɼ��Ļ�����
	DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	IntegerListPtr pVisibleMacList = MachineItemLoader::GetVisibleMacList(pMacLibDBConnect);

	// ��ʼ����ר�ҿ⡱��Ͽ�
	vector<DataProperty> expLibItemList = ExpertLibItemLoader::GetExpLibItemInfoList(pExpLibDBCon);
	int iCurrentIndex = 0;
	for (unsigned int i = 0; i < expLibItemList.size(); i++)
	{
		DataProperty prop = expLibItemList[i];

		// ����Ӧ�Ļ����Ƿ�ɼ���
		if (!pVisibleMacList->Include(prop.GetItem(3).GetValue().llVal))
			continue;

		LONGLONG iExpLibItemID = prop.GetItem(0).GetValue().llVal;
		m_comboExpLibItem.AddString((LPCTSTR)prop.GetItem(1).GetValue().bstrVal);
		m_comboExpLibItem.SetItemData(m_comboExpLibItem.GetCount()-1, (DWORD_PTR)(new LONGLONG(iExpLibItemID)));
		if (iExpLibItemID == m_iExpLibItemID)
			iCurrentIndex = m_comboExpLibItem.GetCount()-1;
	}
	m_comboExpLibItem.SetCurSel(iCurrentIndex);

	// ��ʼ�����������á���Ͽ�
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

	// ������ߴ硱�С�
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

    // ��������������Ϣ���С�
    str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NESTPART_OUTER_LOOPTOOL);
    CGridColumnTrait* pBoundaryLoopToolCol = new CGridColumnTraitText;
    m_nestPartList.InsertColumnTrait(7, str, LVCFMT_LEFT, 130, 0, pBoundaryLoopToolCol);
}

void NestScheduleDlg::PreviewNestPart()
{
	// �õ�ѡ���
	int iIndex = INVALID_IDX;
	for (int i = 0; i < m_nestPartList.GetItemCount(); i++)
	{
		if (m_nestPartList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			iIndex = i;
	}

	// �õ�Ԥ�����ݡ�
	ImgDataPtr pImgData;
	pImgData.reset();
	if (iIndex != INVALID_IDX)
	{
		LONGLONG* pPartID = (LONGLONG*)m_nestPartList.GetItemData(iIndex);
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(*pPartID);
		pImgData = PartLoader::LoadPreviewData(pPartDBCon);
	}

	// Ԥ����
	ImgData::DrawImgData(pImgData, &m_partPreviewWnd);
}

LONGLONG NestScheduleDlg::CheckPart(LONGLONG iParamConfigID, LONGLONG iPartID)
{
	// �������ǲ����к͵�ǰѡ�еġ��������á����׵Ĺ������ݡ�
	LONGLONG iCamDataID, iExpLibItemID;
	if (!PartManager::HasCamDatasetA(iPartID, iParamConfigID, iCamDataID, iExpLibItemID))
	{
		// �����µĹ������ݲ����档
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

		// ��������
		LONGLONG iCamDataID = CheckPart(m_iParamConfigID, *pPartID);
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(*pPartID);
		PartItemPtr pPartItem = PartLoader::LoadPartItem(pPartDBCon, *pPartID, iCamDataID, true);

		// �½����Ű��������
		NestPartPtr pNestPart(new NestPart(pPartItem));
		pNestPartList->push_back(pNestPart);

		// �Ű����ȼ���
		int iNestPriority = _wtoi(m_nestPartList.GetItemText(i, 4));
		NestPriority nestPriority(iNestPriority);
		pNestPart->SetPriority(nestPriority);

		// �Ű������
		int iNestCount = _wtoi(m_nestPartList.GetItemText(i, 5));
		pNestPart->SetNestCount(iNestCount);

		/************************************************************************/		
		// ��ת�Ƕȡ�

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

		// ������������ݿ⡣
		PartManager::SaveNewPart(pPart.get(), pFolderNode);

		/************************************************************************/
		// ����һ�С�

		int iRowCount = m_nestPartList.GetItemCount();
		m_nestPartList.InsertItem(iRowCount, _T(""));
		m_nestPartList.SetItemData(iRowCount, (DWORD_PTR)(new LONGLONG(pPart->GetID())));

		// ��š�
		CString strNum;
		if ((iRowCount+1) < 10)
			strNum.Format(_T("0%d"), iRowCount+1);
		else
			strNum.Format(_T("%d"), iRowCount+1);
		m_nestPartList.SetItemText(iRowCount, 1, strNum);

		// ������ơ�
		m_nestPartList.SetItemText(iRowCount, 2, pPart->GetName());

		// ����ߴ硣
		PartCadDataPtr pPartCadData = pPart->GetCadData();
		Rect2D outerRect = pPartCadData->GetPatternLoopList()->GetRectBox();
		CString strPartSize;
		strPartSize.Format(_T("%.3f * %.3f"), outerRect.GetWidth(), outerRect.GetHeight());
		m_nestPartList.SetItemText(iRowCount, 3, strPartSize);

		// �Ű����ȼ���
		CString strPartPriority;
		strPartPriority.Format(INT_FORMAT, NestPriority::MaxVal());
		m_nestPartList.SetItemText(iRowCount, 4, strPartPriority);

		// �Ű������
		CString strPartCount;
		strPartCount.Format(INT_FORMAT, iPartCount);
		m_nestPartList.SetItemText(iRowCount, 5, strPartCount);

		// ��ת�Ƕȡ�
		CString strRotAng;
		strRotAng = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_ROTATE_FREE);
		m_nestPartList.SetItemText(iRowCount, 6, strRotAng);

		// ������������Ϣ
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

		// ��ֹ��Ϣ��
		m_bFinishAddNestPart = FALSE;

		// ����ÿ�С�
		for (unsigned int i = 0; i < pNestParts->size(); i++)
		{
			NestPartPtr pNestPart = pNestParts->at(i);
			AddNestPartToGrid(pNestPart->GetPartItem(), pNestPart->GetPriority(), pNestPart->GetNestCount(), pNestPart->GetRotStyle());
		}

		// ����Ϣ��
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

	// ��š�
	CString strNum;
	if ((iRowCount+1) < 10)
		strNum.Format(_T("0%d"), iRowCount+1);
	else
		strNum.Format(_T("%d"), iRowCount+1);
	m_nestPartList.SetItemText(iRowCount, 1, strNum);

	// ������ơ�
	m_nestPartList.SetItemText(iRowCount, 2, pPartItem->GetPartName());

	// ����ߴ硣
	PartCadDataPtr pPartCadData = pPartItem->GetCadData();
	Rect2D outerRect = pPartCadData->GetPatternLoopList()->GetRectBox();
	CString strPartSize;
	strPartSize.Format(_T("%.3f * %.3f"), outerRect.GetWidth(), outerRect.GetHeight());
	m_nestPartList.SetItemText(iRowCount, 3, strPartSize);

	// �Ű����ȼ���
	CString strPartPriority;
	strPartPriority.Format(INT_FORMAT, iNestPriority);
	m_nestPartList.SetItemText(iRowCount, 4, strPartPriority);

	// �Ű������
	CString strPartCount;
	strPartCount.Format(INT_FORMAT, iCount);
	m_nestPartList.SetItemText(iRowCount, 5, strPartCount);

	// ��ת�Ƕȡ�
	CString strRotAng = NestPart::GetRotStyleName(iRotAng);
	m_nestPartList.SetItemText(iRowCount, 6, strRotAng);

	// ������������Ϣ
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
	// �õ�ѡ���
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
			// ��ʼ���ӿڡ�
			Rect2DPtr pOutRect(new Rect2D(0, pMaterialSize->GetWidth(), 0, pMaterialSize->GetHeight()));
			LineArc2DListPtr pGeom2DList = pOutRect->GetLineS();
			Rect2D geomRect = pGeom2DList->GetRectBox();
			m_matPreviewWnd.FitRect(geomRect);

			// ���ò������
			m_matPreviewWnd.SetSolidGeom2DList(MaterialProcessor::GetBorderLineArcs(pMaterialSize.get()));

			// ���Ʋ��ϵ���������
			LineArc2DListPtr pUselessRegGeo2D = MaterialProcessor::GetUselessRegions(pMaterialSize.get());
			m_matPreviewWnd.SetDashGeom2DList(pUselessRegGeo2D);

			// ���ơ�
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

	// �������ϱ��
	for (int i = 0; i < m_nestMatList.GetItemCount(); i++)
	{
		LONGLONG* pMatSizeID = (LONGLONG*)m_nestMatList.GetItemData(i);

		// �����ݿ���ز��ϡ�
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, *pMatSizeID);

		// �������Ű���ϡ���
		NestMaterialPtr pNestMat(new NestMaterial(pMaterialSize));
		pNestMatList->push_back(pNestMat);
	}

	return pNestMatList;
}

void NestScheduleDlg::AddDflMat()
{
	// Ĭ�ϵĲ��ϡ�
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

	// ���������͡��С�
	CString strMatSizeType;
	if (pMaterialSize->GetType() == MAT_SIZE_RECT)
		strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_RECT);
	else if (pMaterialSize->GetType() == MAT_SIZE_POLY)
		strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_POLY);
	m_nestMatList.SetItemText(iRowCount, 7, strMatSizeType);
}

END_CUTLEADER_NAMESPACE()
