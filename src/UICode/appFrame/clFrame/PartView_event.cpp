#include "StdAfx.h"
#include "PartView.h"

#include "ValueInputManager.h"
#include "LineArc2DList.h"
#include "clUILibResource.h"
#include "InteractBar.h"
#include "IAction.h"
#include "CommandList.h"
#include "ICommand.h"
#include "CommandManager.h"
#include "InteractBar.h"
#include "EditorData.h"
#include "DataBaseManager.h"

#include "DrawAssistor.h"
#include "PatternList.h"
#include "PatternLoopList.h"
#include "clGeometryFeatureResource.h"
#include "GeometryFeatureEditor.h"
#include "PatternPropertyBar.h"

#include "clCutFeatureResource.h"
#include "CutFeatureEditor.h"
#include "LoopCutFeature.h"
#include "LoopCutFeatureList.h"
#include "LoopStartCutDataList.h"
#include "LoopStartCutData.h"
#include "MicroJointList.h"
#include "MicroJoint.h"
#include "LoopMicroJoint.h"
#include "LoopMicroJointList.h"
#include "LoopCornerData.h"
#include "LoopCornerDataList.h"
#include "LoopCutFeatureManager.h"
#include "LoopStartCutProcessor.h"
#include "CornerPropertyBar.h"
#include "MicroJointPropertyBar.h"
#include "LoopLeadPropertyBar.h"
#include "LoopToolEditPanel.h"

#include "PartCadData.h"
#include "PartItem.h"
#include "PartCamData.h"
#include "PartDocument.h"
#include "PartDrawer.h"

#include "UIManager.h"
#include "clFrameResource.h"

#include "clExpertLibResource.h"
#include "ParamConfig.h"
#include "ParamConfigLoader.h"
#include "ParamConfigDlg.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

IMPLEMENT_DYNCREATE(PartView, CLeaderView)

BEGIN_MESSAGE_MAP(PartView, CLeaderView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()

	// mouse event handler
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()

	// keyboard event handler
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()

	/************************************************************************/
	// draw panel within geometry category.

	// line.
	ON_COMMAND(IDC_GEOM_DRAW_STDLINE, OnDrawStandardline)
	ON_COMMAND(IDC_GEOM_DRAW_MULTILINE, OnDrawMultiplelines)
	ON_COMMAND(IDC_GEOM_DRAW_RECTLINE, OnDrawRectanglelines)

	// hole
	ON_COMMAND(IDC_GEOM_DRAW_RECTHOLE, OnDrawRectangleHole)
	ON_COMMAND(IDC_GEOM_DRAW_TRIHOLE, OnDrawTriangleHole)

	// round hole
	ON_COMMAND(IDC_GEOM_DRAW_ROUNDHOLE, OnDrawRoundHole)
	ON_COMMAND(IDC_GEOM_DRAW_OBROUND, OnDrawOBround)
	ON_COMMAND(IDC_GEOM_DRAW_ELLIPSE, OnDrawEllipse)

	// arc
	ON_COMMAND(IDC_GEOM_DRAW_STDARC, OnDrawArcStandardarc)
	ON_COMMAND(IDC_GEOM_DRAW_ARC3PT, OnDrawArc3pointsarc)

	// D shape
	ON_COMMAND(IDC_GEOM_DRAW_SINGLED, OnDrawSingleD)
	ON_COMMAND(IDC_GEOM_DRAW_DOUBLED, OnDrawDoubleD)

	// Round Corner
	ON_COMMAND(IDC_GEOM_DRAW_RCHOLE, OnDrawRCHole)
	ON_COMMAND(IDC_GEOM_DRAW_RCLINEARC, OnDrawRCLines)

	// radius slot
	ON_COMMAND(IDC_GEOM_DRAW_RADSLOT, OnDrawRadSlot)

	// grid
	ON_COMMAND(IDC_GEOM_DRAW_PATLINE, OnDrawPatternLine)
	ON_COMMAND(IDC_GEOM_DRAW_PATGRID, OnDrawPatternGrid)
	ON_COMMAND(IDC_GEOM_DRAW_PATCIRCLE, OnDrawPatternCircle)
	ON_COMMAND(IDC_GEOM_DRAW_PATARC, OnDrawPatternArc)
	/************************************************************************/

	// "View" panel within geometry category.
	ON_COMMAND(IDC_GEOM_VIEW_ZOOMALL, OnZoomAll)
	ON_COMMAND(IDC_GEOM_VIEW_ZOOMIN, OnZoomIn)
	ON_COMMAND(IDC_GEOM_VIEW_ZOOMOUT, OnZoomOut)

	/************************************************************************/
	// "Edit" panel within geometry category.

	ON_COMMAND(IDC_GEOM_EDIT_SELECTPAT, OnSelectPattern)
	ON_COMMAND(IDC_GEOM_EDIT_COPY, OnCopyPattern)
	ON_COMMAND(IDC_GEOM_EDIT_DELETE, OnDeletePattern)
	ON_COMMAND(IDC_GEOM_EDIT_EXPLODE, OnExplodeGridPat)

	ON_COMMAND(IDC_GEOM_EDIT_MOVE, OnMovePattern)
	ON_COMMAND(IDC_GEOM_EDIT_ROTATE, OnRotatePattern)
	ON_COMMAND(IDC_GEOM_EDIT_OPTIMIZE, OnOptimizePattern)
	ON_COMMAND(IDC_GEOM_OFFSET_LOOP, OnOffsetLoop)
	/************************************************************************/

	/************************************************************************/
	// CAM Feature category.

	ON_COMMAND(IDC_CUTFEATURE_ASSIGN, OnAssignCutFeature)
	ON_COMMAND(IDC_CUTFEATURE_RESET, OnResetCutFeature)
	ON_COMMAND(IDC_CHECK_INTERFERE, OnCheckInterfere)

    // 轮廓刀具
    ON_COMMAND(IDC_CUTFEATURE_LOOPTOOL_EDIT, OnEditLoopTool)

	// start/end cut panel
	ON_COMMAND(IDC_CUTFEATURE_LOOP_LEAD_EDIT, OnEditLoopLead)
	ON_COMMAND(IDC_CUTFEATURE_LOOP_START_PT, OnEditLoopStartPt)
	ON_COMMAND(IDC_SWITCH_CUTSIDE, OnSwitchCutSide)
	ON_COMMAND(IDC_CHANGE_CUTDIR, OnChangeDirection)

	// micro joint panel
	ON_COMMAND(IDC_CUTFEATURE_MJ_ADD, OnAddMicrojoint)
	ON_COMMAND(IDC_CUTFEATURE_MJ_EDIT, OnEditMicrojoint)
	ON_COMMAND(IDC_CUTFEATURE_MJ_DELETE, OnDeleteMicrojoint)

	// corner panel
	ON_COMMAND(IDC_CUTFEATURE_CORNER_ADD, OnAddCornerFeature)
	ON_COMMAND(IDC_CUTFEATURE_CORNER_EDIT, OnEditCornerFeature)
	ON_COMMAND(IDC_CUTFEATURE_CORNER_DELETE, OnDeleteCornerFeature)
	/************************************************************************/

	ON_COMMAND(IDC_PARAM_CONFIG, SetParamConfig)
END_MESSAGE_MAP()

PartView::PartView(void)
{
	m_pDrawAssistor.reset(new DrawAssistor(m_pViewPort));
	m_pGeometryEditor.reset(new GeometryFeatureEditor(m_pViewPort, m_pDrawAssistor));
	m_pCutFeatureEditor.reset(new CutFeatureEditor(m_pViewPort));
}

PartView::~PartView(void)
{
}

void PartView::OnDraw(CDC* pDC)
{
	// call base class to draw something.
	CLeaderView::OnDraw(pDC);

	/************************************************************************/
	// 绘制零件。

	// if still no part item associate with the document, do not draw.
	PartItemPtr pPartItem = GetCurrentPart();
	if (!pPartItem)
		return;

	// if no cad data in part item, do not draw.
	PartCadDataPtr pPartCadData = pPartItem->GetCadData();
	if (!pPartCadData)
		return;

	// 是否初始化了编辑器。
	if (!m_pCurEditor)
		return;

	// 要不要画工艺。
	BOOL bDrawCamFeature = FALSE;
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE);
	if (m_pCurEditor->GetEditorName() == str)
		bDrawCamFeature = TRUE;

	// 绘制零件。
	PartDrawer partDrawer(m_pViewPort);
	partDrawer.DrawPart(pPartItem.get(), bDrawCamFeature);
	/************************************************************************/

	// draw something based on different action.
	IActionPtr pAction = m_pCurEditor->GetActionObject();
	pAction->DrawBeforeFinish();

	// update the output bar.
	m_pCurEditor->UpdateOutputBar();

	// redraw the draw assist information.
	m_pDrawAssistor->DrawAssistInfo();

	// swap buffer
	m_pViewPort->SwapBuffers();
}

void PartView::UpdateUndoRedoItem(CMFCRibbonBaseElement* pElem)
{
	// 当前的编辑器信息。
	EditorData editorData(this, m_pCurEditor->GetEditorName());

	switch (pElem->GetID())
	{
	case IDC_TAO_UNDO:
		{
			CMFCRibbonUndoButton* pUndo = DYNAMIC_DOWNCAST(CMFCRibbonUndoButton, pElem);
			pUndo->CleanUpUndoList();

			// get all undo items for this view.
			CommandListPtr pUndoCommands = CommandManager::GetInstance()->GetUndoCommands();
			m_pUndoCmdList = pUndoCommands->GetCmdByEditor(editorData);

			// get command names from end to begin.
			int iCount = m_pUndoCmdList->size();
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

			// get all redo items for this view.
			CommandListPtr pRedoCommands = CommandManager::GetInstance()->GetRedoCommands();
			m_pRedoCmdList = pRedoCommands->GetCmdByEditor(editorData);

			// get command names from end to begin.
			for (unsigned int i = 0; i < m_pRedoCmdList->size(); i++)
			{
				ICommandPtr pCommand = m_pRedoCmdList->at(m_pRedoCmdList->size()-i-1);
				pRedo->AddUndoAction(pCommand->GetCommandName());
			}
		}
		break;
	}
}

int PartView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// init the OpenGL drawer.
	m_pViewPort->InitEnv(m_hWnd, 0.00001, 10000);

	// the drawing area.
	CRect wndRect;
	GetClientRect(&wndRect);

	// the rect of the geometry items.
	Rect2D geomRect(.0, 10.0, .0, 10.0);

	// set the drawing area.
	double dLeftBottomPtX = .0, dLeftBottomPtY = .0;
	double dXDirRange = m_pViewPort->GetFitAllParam(wndRect.Width(), wndRect.Height(), geomRect.GetXMin(), geomRect.GetXMax(),
													geomRect.GetYMin(), geomRect.GetYMax(), 1.2, dLeftBottomPtX, dLeftBottomPtY);
	m_pViewPort->SetDrawingArea(1.1 * dXDirRange, wndRect.Width(), wndRect.Height(), dLeftBottomPtX, dLeftBottomPtY);

	return CView::OnCreate(lpCreateStruct);
}

BOOL PartView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bPopupShow)
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	else
		SetCursor(NULL);

	return TRUE;
}

void PartView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_OffSetPoint = point;

	CLeaderView::OnMButtonDown(nFlags, point);
}

BOOL PartView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void PartView::OnSize(UINT nType, int cx, int cy)
{
	CLeaderView::OnSize(nType, cx, cy);

	m_pViewPort->SetClientSize(cx, cy);
}

BOOL PartView::OnMouseWheel(UINT nFlags, short iDelta, CPoint pt)
{
	double dRate = 1.25;
	if (iDelta > 0)
		dRate = 1/dRate;

	CPoint PtClient = pt;
	ScreenToClient(&PtClient);
	double dPointX = PtClient.x;
	double dPointY = PtClient.y;
	m_pViewPort->ZoomViewPort(dRate, dPointX, dPointY);

	// reset the "draw assist" information.
	m_pDrawAssistor->Reset();

	Invalidate();

	return TRUE;
}

void PartView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// get the action object to perform left button down event.
	double dx = point.x;
	double dy = point.y;
	m_pViewPort->DToW(dx, dy);

	// 检查是不是点中了一个捕获点。
	Point2DPtr pSnapPt = m_pDrawAssistor->GetSnapPt();
	if (pSnapPt)
	{
		dx = pSnapPt->X();
		dy = pSnapPt->Y();
	}

	// delegate to action object
	m_pCurEditor->LButtonDown(dx, dy);

	Invalidate();

	CLeaderView::OnLButtonDown(nFlags, point);
}

void PartView::OnLButtonUp(UINT nFlags, CPoint point)
{
	/************************************************************************/
	// get the action object to perform left button up event.

	double dx = point.x;
	double dy = point.y;
	m_pViewPort->DToW(dx, dy);

	// delegate to action object
	m_pCurEditor->LButtonUp(dx, dy);
	/************************************************************************/

	Invalidate();

	CLeaderView::OnLButtonUp(nFlags, point);
}

void PartView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	/************************************************************************/
	// get the action object to perform left button db-click event.

	//
	double dx = point.x;
	double dy = point.y;
	m_pViewPort->DToW(dx, dy);

	// delegate to action object
	m_pCurEditor->LButtonDBClk(dx, dy);
	/************************************************************************/

	Invalidate();

	CLeaderView::OnLButtonDblClk(nFlags, point);
}

void PartView::OnRButtonUp(UINT nFlags, CPoint point)
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

void PartView::OnMouseMove(UINT nFlags, CPoint point)
{
	CLeaderView::OnMouseMove(nFlags, point);

	// when move cursor upon the view, set the focus.
	SetFocus();

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

void PartView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

void PartView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
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

void PartView::SetParamConfig()
{
	// 得到这个零件相关的“参数配置”。
	ParamConfigPtr pParamConfig = m_pCutFeatureEditor->GetParamConfig();
	LONGLONG iParamConfigID = pParamConfig->GetID();

	ParamConfigDlg dlg(this);
	dlg.Init(iParamConfigID);
	if (dlg.DoModal() == IDOK)
	{
		// 更新零件用到的参数配置。
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, iParamConfigID);
		m_pCutFeatureEditor->SetParamConfig(pParamConfig);
	}
}

void PartView::Init()
{
	// prepare some data.
	PartItemPtr pPartItem = GetCurrentPart();
	PartCadDataPtr pPartCadData = pPartItem->GetCadData();
	PatternListPtr pPatList = pPartCadData->GetPatternList();
	PatternLoopListPtr pPatternLoopList = pPartCadData->GetPatternLoopList();
	PartCamDataPtr pPartCamData = pPartItem->GetCamData();
	LoopCutFeatureListPtr pLoopFeatureList = pPartCamData->GetLoopFeatureList();
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pPartCamData->GetParamConfigID());

	/************************************************************************/
	// 初始化各个“编辑器”。

	// init Geometry Editor.
	m_pGeometryEditor->SetPatternList(pPatList);
	m_pGeometryEditor->SetPatternLoopList(pPatternLoopList);

	// init cam feature editor
	m_pCutFeatureEditor->SetPatternList(pPatList);
	m_pCutFeatureEditor->SetPatternLoopList(pPatternLoopList);
	m_pCutFeatureEditor->SetLoopFeatureList(pLoopFeatureList);
	m_pCutFeatureEditor->SetParamConfig(pParamConfig);

	// use the GeometryFeatureEditor as the current editor.
	m_pGeometryEditor->SetEditAction(EDIT_SELECTPATTERN);
	m_pCurEditor = m_pGeometryEditor;
	/************************************************************************/

	// 适配视图区域的显示。
	OnZoomAll();
}

PartItemPtr PartView::GetCurrentPart()
{
	CLeaderDoc* pDoc = GetDocument();
	PartDocument* pPartDoc = dynamic_cast<PartDocument*>(pDoc);
	return pPartDoc->GetPartItem();
}

void PartView::SwitchEditor(IEditorPtr pNextEditor)
{
	CString strCurEditorName = m_pCurEditor->GetEditorName();
	CString strNextEditorName = pNextEditor->GetEditorName();

	// 不需要切换编辑器。
	if (strCurEditorName == strNextEditorName)
		return;

	// 几个编辑器名称。
	CString strCadEditor, strCamEditor;
	strCadEditor = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEOMETRY);
	strCamEditor = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE);

	// 切换到了“图形编辑器”。
	if (strCurEditorName == strCamEditor && strNextEditorName == strCadEditor)
	{
		m_pCurEditor = m_pGeometryEditor;

		// 隐藏相关的面板。
		CornerPropertyBar::GetInstance()->ShowBar(FALSE);
		LoopLeadPropertyBar::GetInstance()->ShowBar(FALSE);
        LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
		MicroJointPropertyBar::GetInstance()->ShowBar(FALSE);
	}

	// 切换到了“加工工艺编辑器”。
	if (strCurEditorName == strCadEditor && strNextEditorName == strCamEditor)
	{
		m_pCurEditor = m_pCutFeatureEditor;

		// 隐藏相关的面板。
		PatternPropertyBar::GetInstance()->ShowBar(FALSE);

		// 编辑器从“图形编辑器”切换到了“加工工艺编辑器”。
		if (strCurEditorName == strCadEditor)
		{
			// 清理对应“图形编辑器”的undo/redo列表。
			EditorData editorData(this, strCadEditor);
			CommandListPtr pUndoCommands = CommandManager::GetInstance()->GetUndoCommands();
			CommandListPtr pTmpUndoCommands = pUndoCommands->GetCmdByEditor(editorData);
			pUndoCommands->RemoveCommand(pTmpUndoCommands);
			CommandListPtr pRedoCommands = CommandManager::GetInstance()->GetRedoCommands();
			CommandListPtr pTmpRedoCommands = pRedoCommands->GetCmdByEditor(editorData);
			pRedoCommands->RemoveCommand(pTmpRedoCommands);

			// 处理“加工工艺”。
			PartItemPtr pPartItem = GetCurrentPart();
			PartCadDataPtr pPartCadData = pPartItem->GetCadData();
			PartCamDataPtr pPartCamData = pPartItem->GetCamData();
			LoopCutFeatureListPtr pLoopFeatureList = pPartCamData->GetLoopFeatureList();
			DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
			ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pPartCamData->GetParamConfigID());
			LoopCutFeatureManager::CheckIntegrity(pPartCadData->GetPatternLoopList().get(), pLoopFeatureList, pParamConfig.get(), TRUE, FALSE, FALSE, Matrix2D());
			for (unsigned int i = 0; i < pLoopFeatureList->size(); i++)
				LoopStartCutProcessor::UpdateCache(pLoopFeatureList->at(i)->GetLoopStartCutData());
		}
	}

	// 刷新视图。
	Invalidate();
}
