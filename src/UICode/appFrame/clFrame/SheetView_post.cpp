#include "StdAfx.h"
#include "SheetView.h"

#include "clBaseDataResource.h"
#include "versionInfo.h"
#include "clSecureResource.h"
#include "ProgressDlg.h"
#include "DataBaseManager.h"

#include "MachineParam.h"
#include "ParamConfigLoader.h"
#include "ExpertLibItemLoader.h"
#include "MachineItemLoader.h"

#include "CutSequencePane.h"
#include "clSheetResource.h"
#include "MacRouterEditor.h"
#include "SheetProcessor.h"
#include "CutSequenceList.h"
#include "SheetSequenceData.h"
#include "Sheet.h"
#include "GenerateNCTask.h"
#include "NCViewBar.h"
#include "CutNodeManager.h"
#include "LoopInstanceManager.h"
#include "clCutFeatureResource.h"
#include "LoopInstanceList.h"


BEGIN_CUTLEADER_NAMESPACE()


void SheetView::OnViewNC()
{
	// 检查板材中有没有工序。
	SheetPtr pSheet = GetCurrentSheet();
	if (pSheet->GetSheetSequenceData()->GetCutSequences()->size() == 0)
	{
		CString str, strProductName;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_GEN_NC_NO_SEQ);
		if (MessageBox(str, strProductName, MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
	}

	// 切换编辑器。
	SwitchEditor(m_pMacRouterEditor);

	// generate the NC code of the sheet and display them.
    {
        /************************************************************************/
        // create a task to generate NC.

        // init the task.
        GenerateNCTaskPtr pGenNCTask(new GenerateNCTask(pSheet));

        // display the progress of the task.
        CString str;
        str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_GEN_NC);
        ProgressDlg progressDlg;
        progressDlg.Init(str, pGenNCTask);
        progressDlg.DoModal();

        std::set<LONGLONG> patternLoops_failedToOffset = pGenNCTask->GetPatternLoops_failedToOffset(); // 此处记录偏移失败的几何轮廓。
        if (patternLoops_failedToOffset.size() > 0) // 如果某些轮廓被分配了不合适的刀具尺寸，这里提示用户。
        {
            CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_LoopHaveWrongToolSize);
            MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
        }
        /************************************************************************/


        /************************************************************************/
        // display NC.

        // prepare some data.
        DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
        DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
        LONGLONG iParamConfigID = pSheet->GetParamConfigID();
        LONGLONG iExpLibItemID = ParamConfigLoader::GetExpLibItemID(pExpLibDBCon, iParamConfigID);
        LONGLONG iMacItemID = ExpertLibItemLoader::GetMacIDByExpLibItem(pExpLibDBCon, iExpLibItemID);
        MachineParamPtr pMacParam = MachineItemLoader::LoadMacParam(pMacLibDBConnect, iMacItemID);

        // 生成“模拟形状”集合。
        CutNodeListPtr pCutNodeList = pGenNCTask->GetCutNodeList();
        SequenceSimulateShapeListPtr pSeqSimShapeList = CutNodeManager::CutNode2SimShape(pCutNodeList.get());

        // view NC.
        PartInstanceListPtr pPartInstances = pSheet->GetPartInstances();
        LoopInstanceListPtr pAllLoopInstances = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
        NCViewBar::GetInstance()->InitBar(pSheet->GetName(), pSeqSimShapeList, pGenNCTask->GetNCCodeData(), m_pMacRouterEditor, pMacParam->GetNCPostFix(),
            pAllLoopInstances, patternLoops_failedToOffset);
        NCViewBar::GetInstance()->ShowBar(TRUE);
        /************************************************************************/
    }
}

END_CUTLEADER_NAMESPACE()
