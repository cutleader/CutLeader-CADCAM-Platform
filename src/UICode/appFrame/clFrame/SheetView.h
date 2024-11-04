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

// �����ͼ��
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
	// ��������֡�category�µ����

	// ������������塣
	afx_msg void OnPartPmtAddPart();
	afx_msg void OnPartPmtAddSglDxf();
	afx_msg void OnPartPmtAddAsmDxf();

	// ���༭������֡���塣
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

	// ����ʾ���������
	afx_msg void OnZoomAll();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();

	// ����ʾ/���ء���塣
	afx_msg void OnShowPartPmtTreeView();
	afx_msg void OnUpdatePartPmtTreeView(CCmdUI* pCmdUI);

	// ���ڵ��ԡ�
	afx_msg void OnViewOffsetLoop();
	afx_msg void OnUpdateOffsetLoop(CCmdUI* pCmdUI);
	afx_msg void OnShowPartTopTreeView();
	afx_msg void OnUpdatePartTopTreeView(CCmdUI* pCmdUI);
	/************************************************************************/


	/************************************************************************/
	// ������������category�µ����

	afx_msg void OnAssignCutFeature();
	afx_msg void OnResetCutFeature();
	afx_msg void OnCheckInterfere();

    afx_msg void OnEditLoopTool();

	// start/end cut panel.
	afx_msg void OnEditLoopLead();
	afx_msg void OnEditLoopStartPt();
	afx_msg void OnSwitchCutSide();
	afx_msg void OnChangeDirection();

	// ΢����
	afx_msg void OnAddMicrojoint();
	afx_msg void OnEditMicrojoint();
	afx_msg void OnDelMicrojoint();

	// ��������
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

	// �Žӡ�
	afx_msg void OnAddBdgeSeq();
	afx_msg void OnBreakBdgeSeq();
	afx_msg void OnBdgeStartLoop();

	// ���
	afx_msg void OnAddChainSeq();
	afx_msg void OnInsertChainNode();
	afx_msg void OnDelChainNode();
    afx_msg void OnInsertResetInstruction();
    afx_msg void OnDelResetInstruction();

	// �����и
	afx_msg void OnAddGridSeq();

	// �����и
	afx_msg void OnAddFlyCutSeq();

	afx_msg void OnAddMovePoint();
	afx_msg void OnDelMovePoint();

	// �����и���
	afx_msg void OnAutoRemCutline();
	afx_msg void OnAddRemCutline();
	afx_msg void OnDelRemCutline();

	afx_msg void OnViewSequence();
	afx_msg void OnViewNC();

	// ����ʾ/���ء���塣
	afx_msg void OnShowSeqUnitPane();
	afx_msg void OnUpdateSeqUnitPane(CCmdUI* pCmdUI);
	/************************************************************************/


    /************************************************************************/
    // commands for sample functions.
    afx_msg void OnPartInstanceQuantity();
    afx_msg void OnDeleteLoopSequence();
    afx_msg void OnRealTimeCut();
    /************************************************************************/


	// �������Ի��򡱷�������Ϣ��
	afx_msg LRESULT OnSheetGrideQuit(WPARAM wParam, LPARAM lParam);

	// ����
	afx_msg void OnPreviewReport();
	afx_msg void OnPrintReport();
	afx_msg void OnPreviewLabel();
	afx_msg void OnPrintLabel();

	// �޸���������صġ��������á���
	// ע��
	//  1) ��Ҫ���浽���ݿ⼰���¡����ձ༭�����еġ��������á���
	afx_msg void SetParamConfig();

	afx_msg LRESULT OnHideOrShowCutPath(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEnterSelectPartPlacementOrLoopInstanceAction(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEnterSelectPartPlacementAction(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnQueryOtherLoopTools(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	// ��ʼ�������ͼ��
	void Init(bool bGuideMode = false);

private:
	// get the part associated with this "PartView"
	SheetPtr GetCurrentSheet();

	// �л��༭����
	/* ������
	*  pNextEditor: ִ�еĶ������ڵġ��༭������
	*/
	// ע��
	//  1) ִ�ж���ʱ��Ҫ�������ļ�飬��Ϊ�༭���ı仯�ᵼ��һЩ��̨�Ĵ���
	void SwitchEditor(IEditorPtr pNextEditor);

	// ���ء�������ֱ༭�����õ�����塣
	void HideLayoutEditorPane();

	// ���ɱ���
	BOOL GenerateShtRpt();
	BOOL GenerateLabelRpt();

    // ���¹��򲢻����и�·�����ڰ�����ݷ����仯ʱ���á�
    void UpdateSequenceAndDrawCutPath();

private:
	// the editors.
	PartLayoutEditorPtr m_pPartLayoutEditor;
	SequenceEditorPtr m_pSequenceEditor;
	MacRouterEditorPtr m_pMacRouterEditor;

	// the sheet drawer.
	SheetDrawerPtr m_pSheetDrawer;

	// �򵼶Ի���
	bool m_bGuideMode;
	SheetGuideDlg* m_pSheetGuideDlg;

	// ƫ�ƺ�ġ�������á�����
	PartPlacementListPtr m_pOffPartPlacementList;

	// the report data.
	ReportDataPtr m_pReportData;

    // �и�·��ģ����Ϣ��
    bool m_bHideSequenceSimulateShapes; // �Ƿ�����
    SequenceSimulateShapeListPtr m_pSequenceSimulateShapes;

    // �ð���õ���֪ʶ�⡣
    ParamConfigPtr m_pParamConfig;
    IMaterialSizePtr m_pMaterialSize;

    // ʱ�������Ҫ��OnDraw�и��������޸�ʱ���ʵʱ���¡�
    LONGLONG m_iLatestModifyTimeStamp_ofPartPlacement; // �����������õ��޸�ʱ���
    LONGLONG m_iLatestModifyTimeStamp_ofSequence; // ����й�����޸�ʱ���

    // ��������ֱ༭������ִ�е�������ܵ�����Ч�����ɾ�������������¼����Щ��Ϣ��
    //  ע��
    //  1����������ֱ༭�����е�����ִ�к󣬻ᴥ��OnDraw�����žͻ���᲻�ᵼ����Ч����
    //  2�����������¼�ˡ�������ֱ༭�����е��ĸ��������ĳ�ι���ɾ�����ñ����е���������undo/redo�˵��У�undo����������Ҫ����Ϊ�ա�
    std::map<LONGLONG, CutSequenceDeleteCommandPtr> m_sequenceDeleteCommandData;
};

END_CUTLEADER_NAMESPACE()
