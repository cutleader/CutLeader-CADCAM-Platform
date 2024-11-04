#pragma once

#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(LineArc2DList)
DECLARE_CUTLEADER_CLASS(BridgeNode)

BEGIN_CUTLEADER_NAMESPACE()

// �Ͽ��ŵĶ�����
class BridgeSequenceBreakAction : public SequenceEditActionBase
{
public:
	BridgeSequenceBreakAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~BridgeSequenceBreakAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_BREAK_BDGESEQ; }
	virtual void UpdateForUndoRedo();

private:
	// �������������ġ�����š�����ͼ�μ���Ӧ�ġ��Žӽڵ㡱��
	void CalcCurBdgeGeomInfo(const Point2D& pt);

private:
	// ����������ġ�����š�����ͼ�μ���Ӧ�ġ��Žӽڵ㡱��
	LineArc2DListPtr m_pCurBdgeGeom;
	BridgeNodePtr m_pCurBdgeNode;
};

END_CUTLEADER_NAMESPACE()
