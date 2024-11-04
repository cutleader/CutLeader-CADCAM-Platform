#pragma once

#include "SequenceEditActionBase.h"
#include "Point2D.h"
#include <map>

DECLARE_CUTLEADER_CLASS(ICutNode)

BEGIN_CUTLEADER_NAMESPACE()

// ����������Ӹ�λ�㡣
// ע��
//  �����ƣ�Ŀǰû���ṩundo/redo���ܡ�
class ResetInstructionAddAction : public SequenceEditActionBase
{
public:
	ResetInstructionAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~ResetInstructionAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_ADD_RESET_INSTRUCT; }

private:
    // �����������и�·���͸�λ�㡣
    void Update_cutPath_and_resetPt();

private:
    // ���е������
    CutSequenceListPtr m_pAllChainSeqs;

    // �����������и�·������·���Ǻ�ģ��ʱһ����·�������ܿ�����ƫ�Ƶġ�
    CutNodeListPtr m_pCutNode_4_chainSeq;

    // ��������еĸ�λ�㣬����������ʾ����Ҫʵʱά��
    Point2DListPtr m_pResetPtList;

    // ����ƶ�ʱ���и�·���Ͼ����������ĵ�
    Point2D m_closestPt_onCutPath;
    ICutNodePtr m_pCutNode_hasClosestPt; // ��������ڵ��и�ڵ�
};

END_CUTLEADER_NAMESPACE()
