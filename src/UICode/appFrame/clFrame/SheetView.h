#pragma once

#include "CLeaderView.h"
#include "SheetGuideDlg.h"
#include "PrintWndBase.h"

DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(PartLayoutEditor)
DECLARE_CUTLEADER_CLASS(SequenceEditor)
DECLARE_CUTLEADER_CLASS(MacRouterEditor)
DECLARE_CUTLEADER_CLASS(Sheet)
DECLARE_CUTLEADER_CLASS(SheetDrawer)
DECLARE_CUTLEADER_CLASS(ReportData)
DECLARE_CUTLEADER_CLASS(ParamConfig)
DECLARE_CUTLEADER_CLASS(IMaterialSize)
DECLARE_CUTLEADER_CLASS(SequenceSimulateShapeList)
DECLARE_CUTLEADER_CLASS(CutSequenceDeleteCommand)
DECLARE_CUTLEADER_CLASS(ICommand)


BEGIN_CUTLEADER_NAMESPACE()

// 板材视图。
class CLFRAME_Export SheetView : public CLeaderView, public PrintWndBase
{
	DECLARE_DYNCREATE(SheetView)

public:
	SheetView(void);
	~SheetView(void);

public: // inherit from the super class.
	virtual void OnDraw(CDC* pDC);
	virtual void UpdateUndoRedoItem(CMFCRibbonBaseElement* pElem);
    virtual void ProcessAfterUndo(const ICommand* pCommand);

	virtual void PreparePrinting(CPrintInfo* pInfo);
	virtual void Print(CDC* pDC, CPrintInfo* pInfo);
	virtual CString GetWndText();
	virtual void SetWndFocus();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();

	// mouse event handler
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short iDelta, CPoint pt);

	// keyboard event handler
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);


	/************************************************************************/
	// “零件布局”category下的命令。

	// “添加零件”面板。
	afx_msg void OnPartPmtAddPart();
	afx_msg void OnPartPmtAddSglDxf();
	afx_msg void OnPartPmtAddAsmDxf();

	// “编辑零件布局”面板。
	afx_msg void OnPartPmtSelect();
	afx_msg void OnPartPmtCopy();
	afx_msg void OnPartPmtDelete();
	afx_msg void OnPartPmtRotate();
	afx_msg void OnPartPmtMove();
	afx_msg void OnPartPmtGrid();
	afx_msg void OnExplodeGrid();
    afx_msg void OnCreatePartGroup();
    afx_msg void OnExplodePartGroup();

	afx_msg void OnGenRemnantMat();
	afx_msg void OnMeasureDis();

	// “显示比例”面板
	afx_msg void OnZoomAll();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();

	// “显示/隐藏”面板。
	afx_msg void OnShowPartPmtTreeView();
	afx_msg void OnUpdatePartPmtTreeView(CCmdUI* pCmdUI);

	// 用于调试。
	afx_msg void OnViewOffsetLoop();
	afx_msg void OnUpdateOffsetLoop(CCmdUI* pCmdUI);
	afx_msg void OnShowPartTopTreeView();
	afx_msg void OnUpdatePartTopTreeView(CCmdUI* pCmdUI);
	/************************************************************************/


	/************************************************************************/
	// “制造特征”category下的命令。

	afx_msg void OnAssignCutFeature();
	afx_msg void OnResetCutFeature();
	afx_msg void OnCheckInterfere();

    afx_msg void OnEditLoopTool();

	// start/end cut panel.
	afx_msg void OnEditLoopLead();
	afx_msg void OnEditLoopStartPt();
	afx_msg void OnSwitchCutSide();
	afx_msg void OnChangeDirection();

	// 微连接
	afx_msg void OnAddMicrojoint();
	afx_msg void OnEditMicrojoint();
	afx_msg void OnDelMicrojoint();

	// 角特征。
	afx_msg void OnAddCorner();
	afx_msg void OnEditCorner();
	afx_msg void OnDelCorner();
	/************************************************************************/


	/************************************************************************/
	// Commands under "Sequence and NC" category.

	// "Auto" panel
	afx_msg void OnAutoSequence();
	afx_msg void OnClearAllSequence();
	
	afx_msg void OnSelectSequence();
	afx_msg void OnRemoveSequence();
	afx_msg void OnSortSequence();
	afx_msg void OnReplaceHoleSeq();

	afx_msg void OnAddLoopSeq();
	afx_msg void OnInsertLoopSeq();

	// 桥接。
	afx_msg void OnAddBdgeSeq();
	afx_msg void OnBreakBdgeSeq();
	afx_msg void OnBdgeStartLoop();

	// 连割。
	afx_msg void OnAddChainSeq();
	afx_msg void OnInsertChainNode();
	afx_msg void OnDelChainNode();
    afx_msg void OnInsertResetInstruction();
    afx_msg void OnDelResetInstruction();

	// 田字切割。
	afx_msg void OnAddGridSeq();

	// 飞行切割。
	afx_msg void OnAddFlyCutSeq();

	afx_msg void OnAddMovePoint();
	afx_msg void OnDelMovePoint();

	// 余料切割线
	afx_msg void OnAutoRemCutline();
	afx_msg void OnAddRemCutline();
	afx_msg void OnDelRemCutline();

	afx_msg void OnViewSequence();
	afx_msg void OnViewNC();

	// “显示/隐藏”面板。
	afx_msg void OnShowSeqUnitPane();
	afx_msg void OnUpdateSeqUnitPane(CCmdUI* pCmdUI);
	/************************************************************************/


    /************************************************************************/
    // commands for sample functions.
    afx_msg void OnPartInstanceQuantity();
    afx_msg void OnDeleteLoopSequence();
    afx_msg void OnRealTimeCut();
    /************************************************************************/


	// “引导对话框”发来的消息。
	afx_msg LRESULT OnSheetGrideQuit(WPARAM wParam, LPARAM lParam);

	// 报表。
	afx_msg void OnPreviewReport();
	afx_msg void OnPrintReport();
	afx_msg void OnPreviewLabel();
	afx_msg void OnPrintLabel();

	// 修改这个板材相关的“参数配置”。
	// 注：
	//  1) 需要保存到数据库及更新“工艺编辑器”中的“参数配置”。
	afx_msg void SetParamConfig();

	afx_msg LRESULT OnHideOrShowCutPath(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEnterSelectPartPlacementOrLoopInstanceAction(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEnterSelectPartPlacementAction(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnQueryOtherLoopTools(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	// 初始化板材视图。
	void Init(bool bGuideMode = false);

private:
	// get the part associated with this "PartView"
	SheetPtr GetCurrentSheet();

	// 切换编辑器。
	/* 参数：
	*  pNextEditor: 执行的动作所在的“编辑器”。
	*/
	// 注：
	//  1) 执行动作时需要做这样的检查，因为编辑器的变化会导致一些后台的处理。
	void SwitchEditor(IEditorPtr pNextEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	void HideLayoutEditorPane();

	// 生成报表。
	BOOL GenerateShtRpt();
	BOOL GenerateLabelRpt();

    // 更新工序并绘制切割路径，在板材数据发生变化时调用。
    void UpdateSequenceAndDrawCutPath();

private:
	// the editors.
	PartLayoutEditorPtr m_pPartLayoutEditor;
	SequenceEditorPtr m_pSequenceEditor;
	MacRouterEditorPtr m_pMacRouterEditor;

	// the sheet drawer.
	SheetDrawerPtr m_pSheetDrawer;

	// 向导对话框。
	bool m_bGuideMode;
	SheetGuideDlg* m_pSheetGuideDlg;

	// 偏移后的“零件放置”对象。
	PartPlacementListPtr m_pOffPartPlacementList;

	// the report data.
	ReportDataPtr m_pReportData;

    // 切割路径模拟信息。
    bool m_bHideSequenceSimulateShapes; // 是否隐藏
    SequenceSimulateShapeListPtr m_pSequenceSimulateShapes;

    // 该板材用到的知识库。
    ParamConfigPtr m_pParamConfig;
    IMaterialSizePtr m_pMaterialSize;

    // 时间戳，需要在OnDraw中根据最新修改时间戳实时更新。
    LONGLONG m_iLatestModifyTimeStamp_ofPartPlacement; // 板材中零件放置的修改时间戳
    LONGLONG m_iLatestModifyTimeStamp_ofSequence; // 板材中工序的修改时间戳

    // “零件布局编辑器”中执行的命令可能导致无效工序的删除，这个变量记录了这些信息。
    //  注：
    //  1）“零件布局编辑器”中的命令执行后，会触发OnDraw，接着就会检查会不会导致无效工序。
    //  2）这个变量记录了“零件布局编辑器”中的哪个命令导致了某次工序删除，该变量中的命令不会放入undo/redo菜单中，undo完该命令后需要设置为空。
    std::map<LONGLONG, CutSequenceDeleteCommandPtr> m_sequenceDeleteCommandData;
};

END_CUTLEADER_NAMESPACE()
