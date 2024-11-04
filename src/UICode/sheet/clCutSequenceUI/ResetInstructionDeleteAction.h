#pragma once

#include "SequenceEditActionBase.h"
#include "Point2D.h"
#include <map>

DECLARE_CUTLEADER_CLASS(ICutNode)

BEGIN_CUTLEADER_NAMESPACE()

// ɾ����λ�㡣
// ע��
//  �����ƣ�Ŀǰû���ṩundo/redo���ܡ�
class ResetInstructionDeleteAction : public SequenceEditActionBase
{
public:
	ResetInstructionDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~ResetInstructionDeleteAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_DEL_RESET_INSTRUCT; }

private:
    // �����������и�·���͸�λ�㡣
    void Update_cutPath_and_resetPt();

    // �����и�·���Ͼ����������ĸ�λ��
    void Calc_closestResetPt_onCutPath(const Point2D& pt);

private:
    // ���е������
    CutSequenceListPtr m_pAllChainSeqs;

    // �����������и�·������·���Ǻ�ģ��ʱһ����·�������ܿ�����ƫ�Ƶġ�
    CutNodeListPtr m_pCutNode_4_chainSeq;

    // ��������еĸ�λ�㣬����������ʾ����Ҫʵʱά��
    Point2DListPtr m_pResetPtList;

    // ����ƶ�ʱ���и�·���Ͼ����������ĸ�λ�㣬Ҫ��û�и�λ���˾�Ϊ�ա�
    Point2DPtr m_pClosestResetPt_onCutPath;
};

END_CUTLEADER_NAMESPACE()
