#pragma once

#include "CLeaderView.h"

DECLARE_CUTLEADER_CLASS(GeometryFeatureEditor)
DECLARE_CUTLEADER_CLASS(DrawAssistor)
DECLARE_CUTLEADER_CLASS(CutFeatureEditor)
DECLARE_CUTLEADER_CLASS(PartItem)

BEGIN_CUTLEADER_NAMESPACE()

class CLFRAME_Export PartView : public CLeaderView
{
	DECLARE_DYNCREATE(PartView)

public:
	PartView(void);
	~PartView(void);

public: // inherit from the super class.
	virtual void OnDraw(CDC* pDC);
	virtual void UpdateUndoRedoItem(CMFCRibbonBaseElement* pElem);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// mouse event handler
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short iDelta, CPoint pt);

	// keyboard event handler
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	/************************************************************************/
	// draw panel within geometry category.

	// line.
	afx_msg void OnDrawStandardline();
	afx_msg void OnDrawMultiplelines();
	afx_msg void OnDrawRectanglelines();

	// hole
	afx_msg void OnDrawRectangleHole();
	afx_msg void OnDrawTriangleHole();

	// round hole
	afx_msg void OnDrawRoundHole();
	afx_msg void OnDrawOBround();
	afx_msg void OnDrawEllipse();

	// arc
	afx_msg void OnDrawArcStandardarc();
	afx_msg void OnDrawArc3pointsarc();

	// D shape
	afx_msg void OnDrawSingleD();
	afx_msg void OnDrawDoubleD();

	// Round Corner
	afx_msg void OnDrawRCHole();
	afx_msg void OnDrawRCLines();

	// radius slot
	afx_msg void OnDrawRadSlot();

	// grid
	afx_msg void OnDrawPatternLine();
	afx_msg void OnDrawPatternArc();
	afx_msg void OnDrawPatternCircle();
	afx_msg void OnDrawPatternGrid();
	/************************************************************************/

	// "edit" panel within geometry category.
	afx_msg void OnSelectPattern();
	afx_msg void OnDeletePattern();
	afx_msg void OnExplodeGridPat();
	afx_msg void OnCopyPattern();
	afx_msg void OnMovePattern();
	afx_msg void OnRotatePattern();
	afx_msg void OnOptimizePattern();
	afx_msg void OnOffsetLoop();

	// "view" panel within geometry category.
	afx_msg void OnZoomAll();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();

	/************************************************************************/
	// CAM Feature category.

	afx_msg void OnAssignCutFeature();
	afx_msg void OnResetCutFeature();
	afx_msg void OnCheckInterfere();

	afx_msg void OnEditLoopTool();

	// start/end cut panel
	afx_msg void OnEditLoopLead();
	afx_msg void OnEditLoopStartPt();
	afx_msg void OnSwitchCutSide();
	afx_msg void OnChangeDirection();

	// micro joint panel
	afx_msg void OnAddMicrojoint();
	afx_msg void OnEditMicrojoint();
	afx_msg void OnDeleteMicrojoint();

	// Corner panel
	afx_msg void OnAddCornerFeature();
	afx_msg void OnEditCornerFeature();
	afx_msg void OnDeleteCornerFeature();
	/************************************************************************/

	// �޸���������صġ��������á���
	// ע��
	//  1) ��Ҫ���浽���ݿ⼰���¡����ձ༭�����еġ��������á���
	afx_msg void SetParamConfig();

	DECLARE_MESSAGE_MAP()

public:
	// ��ʼ����ͼ��
	void Init();

private:
	// get the part associated with this "PartView"
	PartItemPtr GetCurrentPart();

	// �л��༭����
	/* ������
	*  pNextEditor: ִ�еĶ������ڵġ��༭������
	*/
	// ע��
	//  1) ִ�ж���ʱ��Ҫ�������ļ�飬��Ϊ�༭���ı仯�ᵼ��һЩ��̨�Ĵ���
	void SwitchEditor(IEditorPtr pNextEditor);

private:
	// the geometry editor
	GeometryFeatureEditorPtr m_pGeometryEditor;

	// we will only use this under Part mode.
	CutFeatureEditorPtr m_pCutFeatureEditor;

	// the draw assistor.
	DrawAssistorPtr m_pDrawAssistor;
};

END_CUTLEADER_NAMESPACE()
