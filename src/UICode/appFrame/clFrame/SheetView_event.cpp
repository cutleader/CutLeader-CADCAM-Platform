#include "StdAfx.h"
#include "SheetView.h"

#include "XMenu.h"
#include "clUtilityResource.h"
#include "ValueInputManager.h"
#include "UnitHelper.h"

#include "clGeometryBaseResource.h"
#include "CommandList.h"
#include "CommandManager.h"
#include "ICommand.h"
#include "UIManager.h"
#include "InteractBar.h"
#include "IAction.h"
#include "DataBaseManager.h"
#include "clBaseDataResource.h"
#include "LogMgr.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"
#include "ImpExpConfig.h"
#include "clOptionResource.h"
#include "clExpertLibResource.h"
#include "ParamConfig.h"
#include "ParamConfigLoader.h"
#include "SequenceConfigItem.h"
#include "ParamConfigDlg.h"
#include "IMaterialSize.h"
#include "MaterialLibLoader.h"
#include "ConfigData.h"
#include "ReportData.h"
#include "ReportTplList.h"
#include "ReportTpl.h"
#include "ReportPageList.h"
#include "ReportPageList.h"
#include "RptPageDrawer.h"
#include "ReportTplLoader.h"
#include "clReportResource.h"

#include "DrawAssistor.h"
#include "LoopStartCutDataList.h"
#include "LoopStartCutData.h"
#include "clCutFeatureResource.h"
#include "CornerPropertyBar.h"
#include "MicroJointPropertyBar.h"
#include "LoopLeadPropertyBar.h"
#include "LoopToolEditPanel.h"
#include "CornerPropertyBar.h"
#include "PartPlacement.h"
#include "PartLayoutEditor.h"
#include "clPartLayoutResource.h"
#include "PartPlacementSelectAction.h"
#include "PartPlacementList.h"
#include "PartPlacementViewBar.h"
#include "PartPlacementDrawer.h"
#include "PartPlacementPropertyBar.h"
#include "PartPlacementAddPanel.h"
#include "CutSequenceList.h"
#include "clCutSequenceResource.h"
#include "SequenceEditor.h"
#include "CutSequencePane.h"
#include "PartLayoutData.h"
#include "CutSequenceManager.h"
#include "SequenceSimulateShapeList.h"
#include "CutNodeList.h"
#include "clPostProcessorResource.h"
#include "MacRouterEditor.h"
#include "NCViewBar.h"
#include "CutNodeManager.h"
#include "Sheet.h"
#include "SheetSequenceData.h"
#include "SheetProcessor.h"
#include "SheetDrawer.h"
#include "CutSequenceDrawer.h"
#include "SheetDocument.h"
#include "clFrameResource.h"
#include "sampleCodeResource.h"
#include "CutSequenceDeleteCommand.h"
#include "LoopToolDataList.h"
#include "ICutPartSequence.h"
#include "LoopInstanceList.h"
#include "LoopToolData.h"


using namespace std;


BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SheetView, CLeaderView)

BEGIN_MESSAGE_MAP(SheetView, CLeaderView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
    ON_WM_DESTROY()

	// mouse event handler
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()

	// keyboard event handler
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()


	/************************************************************************/
	// Commands under "Part Placement" category.

	// “添加零件”面板。
	ON_COMMAND(IDC_PARTPMT_FROM_DB, OnPartPmtAddPart)
	ON_COMMAND(IDC_PARTPMT_FROM_SGL_DXF, OnPartPmtAddSglDxf)
	ON_COMMAND(IDC_PARTPMT_FROM_ASM_DXF, OnPartPmtAddAsmDxf)

	// “编辑零件布局”面板。
	ON_COMMAND(IDC_PARTPMT_SELECT, OnPartPmtSelect)
	ON_COMMAND(IDC_PARTPMT_EDIT_COPY, OnPartPmtCopy)
	ON_COMMAND(IDC_PARTPMT_EDIT_DELETE, OnPartPmtDelete)
	ON_COMMAND(IDC_PARTPMT_EDIT_ROTATE, OnPartPmtRotate)
	ON_COMMAND(IDC_PARTPMT_EDIT_MOVE, OnPartPmtMove)
	ON_COMMAND(IDC_PARTPMT_EDIT_GRID, OnPartPmtGrid)
	ON_COMMAND(IDC_PARTPMT_EXPLODE_GRID, OnExplodeGrid)
    ON_COMMAND(IDC_PARTPMT_CreatePartGroup, OnCreatePartGroup)
    ON_COMMAND(IDC_PARTPMT_ExplodePartGroup, OnExplodePartGroup)

	ON_COMMAND(IDC_GEN_SKELE_REMNANT, OnGenRemnantMat)
	ON_COMMAND(IDC_MEASURE_DISTANCE, OnMeasureDis)

	// “显示比例”面板。
	ON_COMMAND(IDC_PARTPMT_VIEW_ZOOMALL, OnZoomAll)
	ON_COMMAND(IDC_PARTPMT_VIEW_ZOOMIN, OnZoomIn)
	ON_COMMAND(IDC_PARTPMT_VIEW_ZOOMOUT, OnZoomOut)

	// “显示/隐藏”面板。
	ON_COMMAND(IDC_PARTPMT_VIEWBAR_PARTPMT, OnShowPartPmtTreeView)
	ON_UPDATE_COMMAND_UI(IDC_PARTPMT_VIEWBAR_PARTPMT, OnUpdatePartPmtTreeView)

	// 用于调试。
	ON_COMMAND(IDC_PARTPMT_VIEW_OFFSET, OnViewOffsetLoop)
	ON_UPDATE_COMMAND_UI(IDC_PARTPMT_VIEW_OFFSET, OnUpdateOffsetLoop)
	ON_COMMAND(IDC_PARTPMT_VIEWBAR_PARTTOP, OnShowPartTopTreeView)
	ON_UPDATE_COMMAND_UI(IDC_PARTPMT_VIEWBAR_PARTTOP, OnUpdatePartTopTreeView)
	/************************************************************************/


	/************************************************************************/
	// “制造特征”category下的命令。

	ON_COMMAND(IDC_CUTFEATURE_ASSIGN, OnAssignCutFeature)
	ON_COMMAND(IDC_CUTFEATURE_RESET, OnResetCutFeature)
	ON_COMMAND(IDC_CHECK_INTERFERE, OnCheckInterfere)

    // 轮廓刀具
    ON_COMMAND(IDC_CUTFEATURE_LOOPTOOL_EDIT, OnEditLoopTool)

	ON_COMMAND(IDC_CUTFEATURE_LOOP_LEAD_EDIT, OnEditLoopLead)
	ON_COMMAND(IDC_CUTFEATURE_LOOP_START_PT, OnEditLoopStartPt)
	ON_COMMAND(IDC_SWITCH_CUTSIDE, OnSwitchCutSide)
	ON_COMMAND(IDC_CHANGE_CUTDIR, OnChangeDirection)

	ON_COMMAND(IDC_CUTFEATURE_MJ_ADD, OnAddMicrojoint)
	ON_COMMAND(IDC_CUTFEATURE_MJ_EDIT, OnEditMicrojoint)
	ON_COMMAND(IDC_CUTFEATURE_MJ_DELETE, OnDelMicrojoint)

	ON_COMMAND(IDC_CUTFEATURE_CORNER_ADD, OnAddCorner)
	ON_COMMAND(IDC_CUTFEATURE_CORNER_EDIT, OnEditCorner)
	ON_COMMAND(IDC_CUTFEATURE_CORNER_DELETE, OnDelCorner)
	/************************************************************************/


	/************************************************************************/
	// Commands under "Sequence" category.

	// "Auto" panel
	ON_COMMAND(IDC_SEQ_AUTOSEQUENCE, OnAutoSequence)
	ON_COMMAND(IDC_SEQ_CLEARALLSEQUENCE, OnClearAllSequence)

	ON_COMMAND(IDC_SEQ_SELECTSEQUENCE, OnSelectSequence)
	ON_COMMAND(IDC_SEQ_REMOVESEQUENCE, OnRemoveSequence)
	ON_COMMAND(IDC_SEQ_SORTSEQUENCE, OnSortSequence)
	ON_COMMAND(IDC_SEQ_REPLACE_HOLE_SEQ, OnReplaceHoleSeq)

	ON_COMMAND(IDC_SEQ_ADD_LOOPSEQ, OnAddLoopSeq)
	ON_COMMAND(IDC_SEQ_INSERT_LOOPSEQ, OnInsertLoopSeq)

	ON_COMMAND(IDC_SEQ_ADD_BDGESEQ, OnAddBdgeSeq)
	ON_COMMAND(IDC_SEQ_BREAK_BDGESEQ, OnBreakBdgeSeq)
	ON_COMMAND(IDC_SEQ_BDGE_START_LOOP, OnBdgeStartLoop)

	ON_COMMAND(IDC_SEQ_ADD_CHAINSEQ, OnAddChainSeq)
	ON_COMMAND(IDC_SEQ_INSERT_CHAIN_NODE, OnInsertChainNode)
	ON_COMMAND(IDC_SEQ_DEL_CHAIN_NODE, OnDelChainNode)
    ON_COMMAND(IDC_SEQ_INSERT_RESET_INSTRUCTION, OnInsertResetInstruction)
    ON_COMMAND(IDC_SEQ_DEL_RESET_INSTRUCTION, OnDelResetInstruction)

	ON_COMMAND(IDC_SEQ_ADD_GRIDSEQ, OnAddGridSeq)
	ON_COMMAND(IDC_SEQ_ADD_FLYCUTSEQ, OnAddFlyCutSeq)

	ON_COMMAND(IDC_SEQ_ADD_MOV_PT, OnAddMovePoint)
	ON_COMMAND(IDC_SEQ_DEL_MOV_PT, OnDelMovePoint)

	ON_COMMAND(IDC_SEQ_AUTO_REM_CUTLINE, OnAutoRemCutline)
	ON_COMMAND(IDC_SEQ_CREATE_REM_CUTLINE, OnAddRemCutline)
	ON_COMMAND(IDC_SEQ_DELETE_REM_CUTLINE, OnDelRemCutline)

	ON_COMMAND(IDC_SEQ_VIEWSEQUENCE, OnViewSequence)
	ON_COMMAND(IDC_MACROUTER_VIEW_NC, OnViewNC)

	// “显示/隐藏”面板。
	ON_COMMAND(IDC_SEQ_VIEWBAR_SEQUNIT, OnShowSeqUnitPane)
	ON_UPDATE_COMMAND_UI(IDC_SEQ_VIEWBAR_SEQUNIT, OnUpdateSeqUnitPane)
	/************************************************************************/


    /************************************************************************/
    // commands for sample functions.
    ON_COMMAND(IDC_PartInstance_Quantity, OnPartInstanceQuantity)
    ON_COMMAND(IDC_Delete_LoopSequence, OnDeleteLoopSequence)
    ON_COMMAND(IDC_RealTimeCut, OnRealTimeCut)
    /************************************************************************/


	ON_MESSAGE(WM_SHT_GUIDE_QUIT, OnSheetGrideQuit)
	ON_COMMAND(IDC_PREVIEW_REPORT, OnPreviewReport)
	ON_COMMAND(IDC_PRINT_REPORT, OnPrintReport)
	ON_COMMAND(IDC_PREVIEW_LABEL, OnPreviewLabel)
	ON_COMMAND(IDC_PRINT_LABEL, OnPrintLabel)
	ON_COMMAND(IDC_PARAM_CONFIG, SetParamConfig)

    ON_MESSAGE(WM_HideOrShowCutPath, OnHideOrShowCutPath)
    ON_MESSAGE(WM_EnterSelectPartPlacementOrLoopInstanceAction, OnEnterSelectPartPlacementOrLoopInstanceAction)
    ON_MESSAGE(WM_EnterSelectPartPlacementAction, OnEnterSelectPartPlacementAction)
    ON_MESSAGE(WM_QueryOtherLoopTools, OnQueryOtherLoopTools)
END_MESSAGE_MAP()


SheetView::SheetView(void)
{
	m_pPartLayoutEditor.reset(new PartLayoutEditor(m_pViewPort));
	m_pSequenceEditor.reset(new SequenceEditor(m_pViewPort));
	m_pMacRouterEditor.reset(new MacRouterEditor(m_pViewPort));

	m_pSheetDrawer.reset(new SheetDrawer(m_pViewPort));

	m_bGuideMode = false;
	m_pSheetGuideDlg = new SheetGuideDlg();

	m_pOffPartPlacementList.reset();
    m_pSequenceSimulateShapes.reset(new SequenceSimulateShapeList);
    m_bHideSequenceSimulateShapes = false;
    m_iLatestModifyTimeStamp_ofPartPlacement = 0;
    m_iLatestModifyTimeStamp_ofSequence = 0;
}

SheetView::~SheetView(void)
{
}

void SheetView::OnDraw(CDC* pDC)
{
	// call base class to draw something.
	CLeaderView::OnDraw(pDC);

	if (!GetCurrentSheet())
		return;

    if (!m_pCurEditor)
        return;


	/************************************************************************/
	// 绘制板材。

	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();

	// 在“模拟工序动作”时，需要禁止显示“切割特征”。
	IActionPtr pAction = m_pCurEditor->GetActionObject();
	if (pAction && (pAction->GetActionType() == SEQUENCE_VIEW || pAction->GetActionType() == MACROUTER_NC_SIMULATE || pAction->GetActionType() == SEQUENCE_ADD_RESET_INSTRUCT))
	{
		// 保存原有的显示选项。
		BOOL bShowLead = pDisplayParam->GetShowLead();
		BOOL bShowMicroJoint = pDisplayParam->GetShowMicroJoint();
		BOOL bShowCorner = pDisplayParam->GetShowCorner();

		// 禁止显示“切割特征”。
		pDisplayParam->SetShowLead(FALSE);
		pDisplayParam->SetShowMicroJoint(FALSE);
		pDisplayParam->SetShowCorner(FALSE);

		// 绘制。
		COLORREF dwOldColor;
		m_pViewPort->GetDrawColor(dwOldColor);
		m_pViewPort->SetDrawColor(*pDisplayParam->GetPatColor());
		m_pSheetDrawer->DrawSheet();
		m_pViewPort->SetDrawColor(dwOldColor);

		// 恢复原有的显示选项。
		pDisplayParam->SetShowLead(bShowLead);
		pDisplayParam->SetShowMicroJoint(bShowMicroJoint);
		pDisplayParam->SetShowCorner(bShowCorner);
	}
	else
	{
		// 绘制。
		COLORREF dwOldColor;
		m_pViewPort->GetDrawColor(dwOldColor);
		m_pViewPort->SetDrawColor(*pDisplayParam->GetPatColor());
		m_pSheetDrawer->DrawSheet();
		m_pViewPort->SetDrawColor(dwOldColor);
	}

    // 更新工序并绘制切割路径。
    UpdateSequenceAndDrawCutPath();
	/************************************************************************/


	// 绘制偏移后的“零件放置”对象。
	if (m_pOffPartPlacementList)
	{
		PartPlacementDrawer partPlacementDrawer(m_pViewPort);
		partPlacementDrawer.DrawPartPlacements(m_pOffPartPlacementList.get());
	}

	// draw something based on different action.
	if (pAction)
		pAction->DrawBeforeFinish();

	// update the output bar.
	m_pCurEditor->UpdateOutputBar();

	// swap buffer
	m_pViewPort->SwapBuffers();
}

void SheetView::UpdateSequenceAndDrawCutPath()
{
    if (m_bHideSequenceSimulateShapes)
    {
        return;
    }

    SheetPtr pSheet = GetCurrentSheet();
    if (!pSheet)
    {
        LogMgr::DebugWarn(_T("576901"), this);
        return;
    }

    // 检查零件放置信息是不是变化了。
    bool bPartPlacementChanged = false;
    LONGLONG iLatestModifyTimeStamp_ofPartPlacement = pSheet->GetPartPlacements()->GetLatestModifyTimeStamp();
    if (iLatestModifyTimeStamp_ofPartPlacement > m_iLatestModifyTimeStamp_ofPartPlacement)
    {
        m_iLatestModifyTimeStamp_ofPartPlacement = iLatestModifyTimeStamp_ofPartPlacement;
        bPartPlacementChanged = true;
    }

    // 看看是否需要检查无效工序。
    if (bPartPlacementChanged)
    {
        // 得到无效工序
        PartPlacementListPtr pPartPlacements = pSheet->GetPartPlacements();
        PartInstanceListPtr pPartInstances = pSheet->GetPartInstances();
        CutSequenceListPtr pCutSequences = pSheet->GetSheetSequenceData()->GetCutSequences();
        CutSequenceListPtr pInvalidSequences = CutSequenceManager::GetInvalidSequences(pCutSequences, pPartPlacements, pPartInstances);

        // 执行命令删除无效工序。
        if (pInvalidSequences->size() > 0)
        {
            // 得到最后一个命令，它是“零件布局编辑器”中执行的命令。
            ICommandPtr pCommand;
            {
                if (CommandManager::GetInstance()->GetUndoCommands()->size() == 0)
                {
                    LogMgr::DebugWarn(_T("631027"), this);
                    return;
                }

                pCommand = CommandManager::GetInstance()->GetUndoCommands()->back();
                const CString& strEditorName = pCommand->GetEditorData().GetEditorName();
                if (strEditorName != MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_LAYOUT))
                {
                    LogMgr::DebugWarn(_T("037968"), this);
                    return;
                }
            }

            EditorData editorData(CWnd::FromHandle(m_pViewPort->GetWnd()), MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ));
            LeadConfigItemPtr pLeadConfigItem = boost::dynamic_pointer_cast<LeadConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_LEAD_ASSIGN));
            SequenceConfigItemPtr pSeqConfigItem = boost::dynamic_pointer_cast<SequenceConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_GENERATE_SEQ));
            ConfigDataPtr pConfigData(new ConfigData(pLeadConfigItem, pSeqConfigItem, m_pMaterialSize));
            CutSequenceDeleteCommandPtr pCutSequenceDeleteCommand(new CutSequenceDeleteCommand(editorData, pCutSequences, pConfigData, pInvalidSequences));
            pCutSequenceDeleteCommand->Do();
            m_sequenceDeleteCommandData[pCommand->GetID()] = pCutSequenceDeleteCommand;
        }
    }

    // 看看是否要更新切割路径模拟信息。
    bool bSequenceDataChanged = false;
    LONGLONG iLatestModifyTimeStamp_ofSequence = pSheet->GetSheetSequenceData()->GetLatestModifyTimeStamp();
    if (iLatestModifyTimeStamp_ofSequence > m_iLatestModifyTimeStamp_ofSequence)
    {
        m_iLatestModifyTimeStamp_ofSequence = iLatestModifyTimeStamp_ofSequence;
        bSequenceDataChanged = true;
    }

    // 重新生成切割路径模拟信息。
    if (bPartPlacementChanged || bSequenceDataChanged)
    {
        const SequenceConfigItem* pSequenceConfigItem = dynamic_cast<const SequenceConfigItem*>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_GENERATE_SEQ).get());
        Point2D startSeqPt = m_pMaterialSize->GetOuterRect().GetCornerPt(pSequenceConfigItem->GetStartCorner());
        CutSequenceListPtr pCutSequences = pSheet->GetSheetSequenceData()->GetCutSequences();
        CutNodeListPtr pCutNodeList = CutNodeManager::CutSequences2CutNodes(startSeqPt, pCutSequences.get());
        m_pSequenceSimulateShapes = CutNodeManager::CutNode2SimShape(pCutNodeList.get());
    }

    // 绘制切割路径
    {
        const DisplayParam* pDisplayParam = CLOptionInfo::GetInstance()->GetDisplayParam().get();

        // 老的设置。
        COLORREF dwOldColor;
        m_pViewPort->GetDrawColor(dwOldColor);

        CutSequenceDrawer cutSequenceDrawer(m_pViewPort);
        m_pViewPort->SetDrawColor(*pDisplayParam->GetSequenceColor());
        cutSequenceDrawer.DrawSequenceSimulateShapes(m_pSequenceSimulateShapes.get());

        // 恢复设置。
        m_pViewPort->SetDrawColor(dwOldColor);
    }
}

void SheetView::ProcessAfterUndo(const ICommand* pCommand)
{
    const CString& strEditorName = pCommand->GetEditorData().GetEditorName();
    if (strEditorName == MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_LAYOUT)) // 这个命令是“零件布局编辑器”中执行的。
    {
        // “零件布局编辑器”中执行的命令可能导致无效工序的删除，这里需要恢复这些工序。
        map<LONGLONG, CutSequenceDeleteCommandPtr>::iterator iter = m_sequenceDeleteCommandData.find(pCommand->GetID());
        if (iter != m_sequenceDeleteCommandData.end())
        {
            iter->second->UnDo();
            m_sequenceDeleteCommandData.erase(iter);
        }
    }
}

void SheetView::UpdateUndoRedoItem(CMFCRibbonBaseElement* pElem)
{
	// 当前的编辑器信息。
	EditorData editorData(this, m_pCurEditor->GetEditorName());

	switch (pElem->GetID())
	{
	case IDC_TAO_UNDO:
		{
			CMFCRibbonUndoButton* pUndo = DYNAMIC_DOWNCAST(CMFCRibbonUndoButton, pElem);
			pUndo->CleanUpUndoList();
            m_pUndoCmdList->clear();

			// get command names from end to begin.
			const CommandList* pUndoCmdList = CommandManager::GetInstance()->GetUndoCommands().get();
            m_pUndoCmdList->insert(m_pUndoCmdList->end(), pUndoCmdList->begin(), pUndoCmdList->end());
			for (unsigned int i = 0; i < m_pUndoCmdList->size(); i++)
			{
				ICommandPtr pCommand = m_pUndoCmdList->at(m_pUndoCmdList->size()-i-1);
				pUndo->AddUndoAction(pCommand->GetCommandName());
			}
		}
		break;
	case IDC_TAO_REDO:
		{
			CMFCRibbonUndoButton* pRedo = DYNAMIC_DOWNCAST(CMFCRibbonUndoButton, pElem);
			pRedo->CleanUpUndoList();
            m_pRedoCmdList->clear();

			// get command names from end to begin.
            const CommandList* pRedoCmdList = CommandManager::GetInstance()->GetRedoCommands().get();
            m_pRedoCmdList->insert(m_pRedoCmdList->end(), pRedoCmdList->begin(), pRedoCmdList->end());
			for (unsigned int i = 0; i < m_pRedoCmdList->size(); i++)
			{
				ICommandPtr pCommand = m_pRedoCmdList->at(m_pRedoCmdList->size()-i-1);
				pRedo->AddUndoAction(pCommand->GetCommandName());
			}
		}
		break;
	}
}

void SheetView::Print(CDC* pDC, CPrintInfo* pInfo) 
{
	// the report page which will display.
	UINT iCurPageIndex = pInfo->m_nCurPage;
	ReportPageListPtr pReportPages = m_pReportData->GetReportPageList();
	ReportPagePtr pReportPage = pReportPages->at(iCurPageIndex-1);

	// draw the current report page.
	RptPageDrawer::DrawRptPage(pReportPage, pDC);
}

void SheetView::PreparePrinting(CPrintInfo* pInfo)
{
	// set the report page count.
	ReportPageListPtr pReportPages = m_pReportData->GetReportPageList();
	pInfo->SetMaxPage(pReportPages->size());
}

CString SheetView::GetWndText()
{
	CString str;
	GetWindowText(str);
	return str;
}

void SheetView::SetWndFocus()
{
	SetFocus();
}

int SheetView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// init the OpenGL drawer.
	m_pViewPort->InitEnv(m_hWnd, 0.00001, 10000);

	// the drawing area.
	CRect wndRect;
	GetClientRect(&wndRect);

	// the rect of the geometry items.
	Rect2D geomRect(.0, 60.0, .0, 60.0);

	// set the drawing area.
	double dLeftBottomPtX = .0, dLeftBottomPtY = .0;
	double dXDirRange = m_pViewPort->GetFitAllParam(wndRect.Width(), wndRect.Height(), geomRect.GetXMin(), geomRect.GetXMax(),
													geomRect.GetYMin(), geomRect.GetYMax(), 1.1, dLeftBottomPtX, dLeftBottomPtY);
	m_pViewPort->SetDrawingArea(1.1 * dXDirRange, wndRect.Width(), wndRect.Height(), dLeftBottomPtX, dLeftBottomPtY);

	// 初始化“引导模式对话框”。
	m_pSheetGuideDlg->Create(IDD_CLFRAME_GUIDE);
	m_pSheetGuideDlg->SetParent(this);
	m_pSheetGuideDlg->ShowWindow(SW_HIDE);

	return CView::OnCreate(lpCreateStruct);
}

BOOL SheetView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bPopupShow)
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	else
		SetCursor(NULL);

	return TRUE;
}

void SheetView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_OffSetPoint = point;

	CLeaderView::OnMButtonDown(nFlags, point);
}

BOOL SheetView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void SheetView::OnSize(UINT nType, int cx, int cy)
{
	CLeaderView::OnSize(nType, cx, cy);

	m_pViewPort->SetClientSize(cx, cy);
}

void SheetView::OnDestroy()
{
    m_pPartLayoutEditor->DestroyChildDialogOfSheetView();
    m_pSequenceEditor->DestroyChildDialogOfSheetView();
    m_pSheetGuideDlg->DestroyWindow();

    CLeaderView::OnDestroy();
}

BOOL SheetView::OnMouseWheel(UINT nFlags, short iDelta, CPoint pt)
{
	// figure out the zoom rate.
	double rate = 1.25;
	if (iDelta > 0)	
		rate = 1/rate;

	// adjust the drawer in the main thread.
	CPoint PtClient = pt;
	ScreenToClient(&PtClient);
	double PointX = PtClient.x;
	double PointY = PtClient.y;
	m_pViewPort->ZoomViewPort(rate, PointX, PointY);

	Invalidate();

	return TRUE;
}

void SheetView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// get the action object to perform left button down event.
	double dx = point.x;
	double dy = point.y;
	m_pViewPort->DToW(dx, dy);

	// delegate to action object
	m_pCurEditor->LButtonDown(dx, dy);

	Invalidate();

	CLeaderView::OnLButtonDown(nFlags, point);
}

void SheetView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// get the action object to perform left button up event.
	double dx = point.x;
	double dy = point.y;
	m_pViewPort->DToW(dx, dy);

	// delegate to action object
	m_pCurEditor->LButtonUp(dx, dy);

	Invalidate();

	CLeaderView::OnLButtonUp(nFlags, point);
}

void SheetView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	double dx = point.x;
	double dy = point.y;
	m_pViewPort->DToW(dx, dy);

	// delegate to action object
	m_pCurEditor->LButtonDBClk(dx, dy);

	Invalidate();

	CLeaderView::OnLButtonDblClk(nFlags, point);
}

void SheetView::OnContextMenu(CWnd*, CPoint point)
{
	// 当前动作要是想处理右键消息，不弹出右键菜单。
	if (m_pCurEditor->GetActionObject()->IsProcessRightClickEvent())
	{
		return;
	}

	m_bPopupShow = TRUE;

	// 创建菜单。
	XMenu xMenu;
	xMenu.CreatePopupMenu();

	/************************************************************************/	
	// 创建各个菜单项。

	CString str;

	// 添加零件。
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ADDPART_FROM_DXF);
	xMenu.AppendMenu(MF_STRING, IDC_PARTPMT_FROM_SGL_DXF, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ADDPART_FROM_ASM_DXF);
	xMenu.AppendMenu(MF_STRING, IDC_PARTPMT_FROM_ASM_DXF, str.AllocSysString());

	// 分隔符
	xMenu.AppendMenu(MF_SEPARATOR);

	// 零件布局编辑。
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTPMT_SELECT);
	xMenu.AppendMenu(MF_STRING, IDC_PARTPMT_SELECT, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMAND_DEL_PARTPMT);
	xMenu.AppendMenu(MF_STRING, IDC_PARTPMT_EDIT_DELETE, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COPY_PART);
	xMenu.AppendMenu(MF_STRING, IDC_PARTPMT_EDIT_COPY, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MOVE_PART);
	xMenu.AppendMenu(MF_STRING, IDC_PARTPMT_EDIT_MOVE, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ROTATE_PART);
	xMenu.AppendMenu(MF_STRING, IDC_PARTPMT_EDIT_ROTATE, str.AllocSysString());

	// 分隔符
	xMenu.AppendMenu(MF_SEPARATOR);

	// 编辑工艺。
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CHECK_INTERFERE);
	xMenu.AppendMenu(MF_STRING, IDC_CHECK_INTERFERE, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EDIT_LOOP_LEAD);
	xMenu.AppendMenu(MF_STRING, IDC_CUTFEATURE_LOOP_LEAD_EDIT, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EDIT_LOOP_START_PT);
	xMenu.AppendMenu(MF_STRING, IDC_CUTFEATURE_LOOP_START_PT, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CHANGE_CUTDIR);
	xMenu.AppendMenu(MF_STRING, IDC_CHANGE_CUTDIR, str.AllocSysString());

	// 分隔符
	xMenu.AppendMenu(MF_SEPARATOR);

	// 编辑工序。
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_AUTOSEQUENCE);
	xMenu.AppendMenu(MF_STRING, IDC_SEQ_AUTOSEQUENCE, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_ADD_LOOP_SEQ);
	xMenu.AppendMenu(MF_STRING, IDC_SEQ_ADD_LOOPSEQ, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REMOVESEQUENCE);
	xMenu.AppendMenu(MF_STRING, IDC_SEQ_REMOVESEQUENCE, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_VIEWSEQUENCE);
	xMenu.AppendMenu(MF_STRING, IDC_SEQ_VIEWSEQUENCE, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MACROUTER_VIEW_NC);
	xMenu.AppendMenu(MF_STRING, IDC_MACROUTER_VIEW_NC, str.AllocSysString());

	// 分隔符
	xMenu.AppendMenu(MF_SEPARATOR);

	// 输出报表。
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PREVIEW_REPORT);
	xMenu.AppendMenu(MF_STRING, IDC_PREVIEW_REPORT, str.AllocSysString());
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRINT_REPORT);
	xMenu.AppendMenu(MF_STRING, IDC_PRINT_REPORT, str.AllocSysString());

	// 分隔符
	xMenu.AppendMenu(MF_SEPARATOR);

	// 设置。
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CAM_PARAM);
	xMenu.AppendMenu(MF_STRING, IDC_PARAM_CONFIG, str.AllocSysString());
	/************************************************************************/

	// 显示。
	xMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, point.x, point.y, this);

	m_bPopupShow = FALSE;
}

void SheetView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// get the action object to perform left button down event.
	double dx = point.x;
	double dy = point.y;
	m_pViewPort->DToW(dx, dy);

	// delegate to action object
	m_pCurEditor->RButtonUp(dx, dy);

	Invalidate();

	CLeaderView::OnRButtonUp(nFlags, point);
}

void SheetView::OnMouseMove(UINT nFlags, CPoint point)
{
	CLeaderView::OnMouseMove(nFlags, point);

	// when move cursor upon the view, set the focus.
	SetFocus();

	// middle button down.
	if (nFlags & MK_MBUTTON)
	{
		CPoint ptOffSet = point - m_OffSetPoint;
		m_pViewPort->PanViewPort(ptOffSet.x, ptOffSet.y);
		m_OffSetPoint = point;
	}
	else
	{
		/************************************************************************/
		// delegate the "MouseMove" event to the action.

		double dx = point.x;
		double dy = point.y;
		m_pViewPort->DToW(dx, dy);

		// delegate to action object
		double dTmpX = dx, dTmpY = dy;
		m_pCurEditor->MovePoint(dTmpX, dTmpY);
		/************************************************************************/	

		/************************************************************************/
		// update the status bar.

		// the coordinate of the cursor.
		CString strCoordinate;
		strCoordinate.Format(_T("%.4f , %.4f"), dTmpX, dTmpY);

		// the string will be displayed.
		CString str;
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CURSOR_POSITION);
		str.Replace(_T("WILLBEREPLACED1"), strCoordinate);

		// display.
		UIManager::GetInstance()->GetStatusBar()->UpdateStatusBar(str, ID_SECOND_PANE);
		/************************************************************************/
	}

	Invalidate();
}

void SheetView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CLeaderView::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputS(nChar) && (::GetKeyState(VK_CONTROL) & 0x8000))
	{
		CLeaderDoc* pDoc = GetDocument();
		pDoc->OnSaveDocument();
	}
	else if (ValueInputManager::GetInstance()->IsInputZ(nChar) && (::GetKeyState(VK_CONTROL) & 0x8000))
	{
		OnEditUndo();
	}
	else
	{
		// delegate to action object
		m_pCurEditor->OnKeyDown(nChar, nRepCnt, nFlags);
	}

	Invalidate();
}

void SheetView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CLeaderView::OnKeyUp(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputS(nChar) && (::GetKeyState(VK_CONTROL) & 0x8000))
	{
		;
	}
	else if (ValueInputManager::GetInstance()->IsInputZ(nChar) && (::GetKeyState(VK_CONTROL) & 0x8000))
	{
		;
	}
	else
	{
		// delegate to action object
		m_pCurEditor->OnKeyUp( nChar, nRepCnt, nFlags);
	}
}

LRESULT SheetView::OnSheetGrideQuit(WPARAM wParam, LPARAM lParam)
{
	m_bGuideMode = false;
	return 0;
}

void SheetView::OnPreviewReport()
{
	if (GenerateShtRpt())
		PreviewData();
}

void SheetView::OnPrintReport()
{
	if (GenerateShtRpt())
		PrintData();
}

void SheetView::OnPreviewLabel()
{
	if (GenerateLabelRpt())
		PreviewData();
}

void SheetView::OnPrintLabel()
{
	if (GenerateLabelRpt())
		PrintData();
}

void SheetView::SetParamConfig()
{
	// 得到这个板材相关的“参数配置”。
	ParamConfigPtr pParamConfig = m_pPartLayoutEditor->GetParamConfig();
	LONGLONG iParamConfigID = pParamConfig->GetID();

	ParamConfigDlg dlg(this);
	dlg.Init(iParamConfigID);
	if (dlg.DoModal() == IDOK)
	{
		// 更新板材用到的参数配置。
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, iParamConfigID);
		m_pPartLayoutEditor->SetParamConfig(pParamConfig);
		m_pSequenceEditor->SetParamConfig(pParamConfig);
	}
}

void SheetView::Init(bool bGuideMode)
{
	m_bGuideMode = bGuideMode;

	// get the sheet opened.
	SheetPtr pSheet = GetCurrentSheet();


	/************************************************************************/
	// prepare some data.

	// 参数配置。
	LONGLONG iParamConfigID = pSheet->GetParamConfigID();
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, iParamConfigID);
    m_pParamConfig = pParamConfig;

	// get mat info.
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, pSheet->GetMaterialSizeID());
    m_pMaterialSize = pMaterialSize;

	//
	PartPlacementListPtr pPartPlacements = pSheet->GetPartPlacements();
	PartInstanceListPtr pPartInstances = pSheet->GetPartInstances();
    PartGroupListPtr pPartGroups = pSheet->GetPartGroupList();
	SheetSequenceDataPtr pSheetSequenceData = pSheet->GetSheetSequenceData();
	CutSequenceListPtr pSeqUnitList = pSheetSequenceData->GetCutSequences();
	/************************************************************************/


	/************************************************************************/
	// init the editors.

	// init the sheet layout editor.
	m_pPartLayoutEditor->SetPartPlacements(pPartPlacements);
	m_pPartLayoutEditor->SetPartInsts(pPartInstances);
    m_pPartLayoutEditor->SetPartGroups(pPartGroups);
	m_pPartLayoutEditor->SetMatSize(pMaterialSize);
	m_pPartLayoutEditor->SetParamConfig(pParamConfig);
	m_pPartLayoutEditor->InitFloatDlg();

	// init the sequence editor.
	PartLayoutDataPtr pPartLayoutData(new PartLayoutData(pPartPlacements, pPartInstances));
	m_pSequenceEditor->SetMatSize(pMaterialSize);
	m_pSequenceEditor->SetPartLayoutData(pPartLayoutData);
	m_pSequenceEditor->SetCutSequences(pSeqUnitList);
	m_pSequenceEditor->SetParamConfig(pParamConfig);
	m_pSequenceEditor->InitApplyPartDlg();
	m_pSequenceEditor->InitInsertPosDlg();
	m_pSequenceEditor->Init_GenerateRemCutLineDlg();

	// set the current editor.
	m_pPartLayoutEditor->SetEditAction(EDIT_SELECTPRTPMT);
	m_pCurEditor = m_pPartLayoutEditor;
	/************************************************************************/


	// init the sheet drawer.
	m_pSheetDrawer->Init(pSheet, pMaterialSize);

	// 适配视图区域的显示。
	OnZoomAll();

	// 是否显示引导对话框。
	if (m_bGuideMode)
	{
		// 定位对话框。
		m_pSheetGuideDlg->ShowWindow(SW_SHOW);
		CRect winRect;
		m_pSheetGuideDlg->GetWindowRect(winRect);
		m_pSheetGuideDlg->MoveWindow(5, 5, winRect.Width(), winRect.Height());
	}
}

SheetPtr SheetView::GetCurrentSheet()
{
	CLeaderDoc* pDoc = GetDocument();
	SheetDocument* pSheetDoc = dynamic_cast<SheetDocument*>(pDoc);

	return pSheetDoc->GetSheet();
}

void SheetView::SwitchEditor(IEditorPtr pNextEditor)
{
	CString strCurEditorName = m_pCurEditor->GetEditorName();
	CString strNextEditorName = pNextEditor->GetEditorName();

	// 不需要切换编辑器。
	if (strCurEditorName == strNextEditorName)
    {
		return;
    }

    CString strPartPmtEditor = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_LAYOUT);
	CString strSeqEditor = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ);
	CString strNCEditor = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NC);
	if (strNextEditorName == strPartPmtEditor) // 切换到了“零件布局编辑器”。
	{
		m_pCurEditor = m_pPartLayoutEditor;
		m_pCurEditor->SetDummyAction();

		// 隐藏相关的面板。
		CutSequencePane::GetInstance()->ShowBar(FALSE);
		NCViewBar::GetInstance()->ShowBar(FALSE);

		// 隐藏对话框。
		m_pSequenceEditor->HideApplyPartDlg();
		m_pSequenceEditor->HideInsertPosDlg();
		m_pSequenceEditor->Hide_GenerateRemCutLineDlg();
	}
	else if (strNextEditorName == strSeqEditor) // 切换到了“工序编辑器”。
	{
		m_pCurEditor = m_pSequenceEditor;
		m_pCurEditor->SetDummyAction();

		// 隐藏相关的面板。
		PartPlacementViewBar::GetInstance()->ShowBar(FALSE);
		PartPlacementPropertyBar::GetInstance()->ShowBar(FALSE);
		PartPlacementAddPanel::GetInstance()->ShowBar(FALSE);
		CornerPropertyBar::GetInstance()->ShowBar(FALSE);
        LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
		LoopLeadPropertyBar::GetInstance()->ShowBar(FALSE);
		MicroJointPropertyBar::GetInstance()->ShowBar(FALSE);
		NCViewBar::GetInstance()->ShowBar(FALSE);

		// 隐藏对话框。
		m_pPartLayoutEditor->HideDisDetectDlg();
		m_pPartLayoutEditor->HideSpreadSamePartDlg();
		m_pPartLayoutEditor->HidePartGridDlg();
        m_pPartLayoutEditor->HideSelectPartOrLoopActionBar();
	}
	else if (strNextEditorName == strNCEditor) // 切换到了“NC编辑器”。
	{
		m_pCurEditor = m_pMacRouterEditor;

		// 隐藏相关的面板。
		PartPlacementViewBar::GetInstance()->ShowBar(FALSE);
		PartPlacementPropertyBar::GetInstance()->ShowBar(FALSE);
		PartPlacementAddPanel::GetInstance()->ShowBar(FALSE);
		CornerPropertyBar::GetInstance()->ShowBar(FALSE);
        LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
		LoopLeadPropertyBar::GetInstance()->ShowBar(FALSE);
		MicroJointPropertyBar::GetInstance()->ShowBar(FALSE);
		CutSequencePane::GetInstance()->ShowBar(FALSE);

		// 隐藏对话框。
		m_pPartLayoutEditor->HideDisDetectDlg();
		m_pPartLayoutEditor->HideSpreadSamePartDlg();
		m_pPartLayoutEditor->HidePartGridDlg();
        m_pPartLayoutEditor->HideSelectPartOrLoopActionBar();
		m_pSequenceEditor->HideApplyPartDlg();
		m_pSequenceEditor->HideInsertPosDlg();
		m_pSequenceEditor->Hide_GenerateRemCutLineDlg();
	}
}

void SheetView::HideLayoutEditorPane()
{
	PartPlacementViewBar::GetInstance()->ShowBar(FALSE);
	PartPlacementPropertyBar::GetInstance()->ShowBar(FALSE);
	PartPlacementAddPanel::GetInstance()->ShowBar(FALSE);

	CornerPropertyBar::GetInstance()->ShowBar(FALSE);
    LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
	LoopLeadPropertyBar::GetInstance()->ShowBar(FALSE);
	MicroJointPropertyBar::GetInstance()->ShowBar(FALSE);
}

BOOL SheetView::GenerateShtRpt()
{
	// 加载报表模板。
	ImpExpConfigPtr pImpExpConfig = CLOptionInfo::GetInstance()->GetImpExpConfig();
	LONGLONG iShtRptTplID = pImpExpConfig->GetShtRptTplID();
	ReportTplPtr pReportTpl = ReportTplLoader::LoadReportTpl(iShtRptTplID);
	if (pReportTpl)
	{
		// generate report data.
		CClientDC dc(this);
		UnitHelper::Init(dc.GetDeviceCaps(LOGPIXELSX));
		m_pReportData = SheetProcessor::GenerateSheetRpt(GetCurrentSheet(), pReportTpl);
	}
	else
	{
		CString str, strProductName;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_WRONG_DFL_TPL);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	return TRUE;
}

BOOL SheetView::GenerateLabelRpt()
{
	// 加载报表模板。
	ImpExpConfigPtr pImpExpConfig = CLOptionInfo::GetInstance()->GetImpExpConfig();
	LONGLONG iLabelRptTplID = pImpExpConfig->GetLabelRptTplID();
	ReportTplPtr pReportTpl = ReportTplLoader::LoadReportTpl(iLabelRptTplID);
	if (pReportTpl)
	{
		// generate report data.
		CClientDC dc(this);
		UnitHelper::Init(dc.GetDeviceCaps(LOGPIXELSX));
		m_pReportData = SheetProcessor::GenerateSheetRpt(GetCurrentSheet(), pReportTpl);
	}
	else
	{
		CString str, strProductName;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_WRONG_DFL_TPL);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	return TRUE;
}

LRESULT SheetView::OnHideOrShowCutPath(WPARAM wParam, LPARAM lParam)
{
    if (wParam == 0)
    {
        m_bHideSequenceSimulateShapes = true;
    }
    else if (wParam == 1)
    {
        m_bHideSequenceSimulateShapes = false;
    }

    Invalidate();

    return 0;
}

LRESULT SheetView::OnEnterSelectPartPlacementOrLoopInstanceAction(WPARAM wParam, LPARAM lParam)
{
    if (wParam == 1)
    {
        SwitchEditor(m_pPartLayoutEditor);
        HideLayoutEditorPane();
        m_pPartLayoutEditor->SetEditAction(EDIT_SelectLoopInstance);
    }
    else if (wParam == 2)
    {
        SwitchEditor(m_pPartLayoutEditor);
        HideLayoutEditorPane();
        m_pPartLayoutEditor->SetEditAction(EDIT_SELECTPRTPMT);
    }
    else if (wParam == 3)
    {
        SwitchEditor(m_pPartLayoutEditor);
        HideLayoutEditorPane();
        m_pPartLayoutEditor->SetEditAction(EDIT_SelectPartOrLoopInstance);
    }

    return 0;
}

LRESULT SheetView::OnEnterSelectPartPlacementAction(WPARAM wParam, LPARAM lParam)
{
    if (wParam == 0)
    {
        LogMgr::DebugWarn(_T("246845"), this);
        return 0;
    }

    // 当前板材
    SheetPtr pSheet = GetCurrentSheet();
    if (!pSheet)
    {
        LogMgr::DebugWarn(_T("364850"), this);
        return 0;
    }

    SwitchEditor(m_pPartLayoutEditor);
    HideLayoutEditorPane();

    // 得到预选择的零件放置。
    PartPlacementListPtr pPartPlacements_preSelect(new PartPlacementList);
    vector<LONGLONG>* pPartPlacementIDs_preSelect = (vector<LONGLONG>*)wParam;
    for (unsigned int i = 0; i < pPartPlacementIDs_preSelect->size(); i++)
    {
        LONGLONG iPartPlacementID = pPartPlacementIDs_preSelect->operator [](i);
        PartPlacementPtr pPartPlacement = pSheet->GetPartPlacements()->GetPartPlacementByID(iPartPlacementID);
        pPartPlacements_preSelect->push_back(pPartPlacement);
    }
    if (pPartPlacements_preSelect->size() == 0)
    {
        LogMgr::DebugWarn(_T("789642"), this);
        return 0;
    }
    delete pPartPlacementIDs_preSelect; // 这个资源由消息接收者释放。

    // 启动零件放置动作，并预先选择指定的零件。
    m_pPartLayoutEditor->StartPartPlacementSelectAction_andPreSelect(pPartPlacements_preSelect);

    return 0;
}

LRESULT SheetView::OnQueryOtherLoopTools(WPARAM wParam, LPARAM lParam)
{
    LoopToolDataList* pLoopToolDataList = (LoopToolDataList*)wParam;
    LoopToolDataList* pLoopToolDataList_other = (LoopToolDataList*)lParam;

    SheetPtr pSheet = GetCurrentSheet();
    const CutSequenceList* pCutSequenceList = pSheet->GetSheetSequenceData()->GetCutSequences().get();
    for (unsigned int i = 0; i < pLoopToolDataList->size(); i++) // 遍历每个轮廓刀具
    {
        const LoopToolData* pLoopToolData = pLoopToolDataList->operator [](i).get();
        for (unsigned int j = 0; j < pCutSequenceList->size(); j++) // 遍历每个工序
        {
            if (const ICutPartSequence* pCutPartSequence = dynamic_cast<const ICutPartSequence*>(pCutSequenceList->operator [](j).get()))
            {
                LoopInstanceListPtr pLoopInstances_inOneSequence = pCutPartSequence->GetLoopInstanceList(); // 工序加工的轮廓实例
                if (pLoopInstances_inOneSequence->GetLoopInstanceByLoopTool(pLoopToolData))
                {
                    LoopToolDataListPtr pLoopToolDataList_tmp = pLoopInstances_inOneSequence->GetLoopToolDataList();
                    pLoopToolDataList_tmp->DeleteItemByID(pLoopToolData->GetID());
                    for (unsigned int k = 0; k < pLoopToolDataList_tmp->size(); k++)
                    {
                        LoopToolDataPtr pLoopToolData_tmp = pLoopToolDataList_tmp->operator [](k);
                        if (!pLoopToolDataList_other->GetItemByID(pLoopToolData_tmp->GetID()) && !pLoopToolDataList->GetItemByID(pLoopToolData_tmp->GetID()))
                        {
                            pLoopToolDataList_other->push_back(pLoopToolData_tmp);
                        }
                    }
                }
            }
        }
    }

    return 0;
}

END_CUTLEADER_NAMESPACE()
