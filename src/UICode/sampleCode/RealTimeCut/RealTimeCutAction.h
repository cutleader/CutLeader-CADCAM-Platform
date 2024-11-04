#pragma once

#include "clCutSequenceUICommon.h"
#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(RealTimeCutSimulator)
DECLARE_CUTLEADER_CLASS(SequenceSimulateShapeList)

BEGIN_CUTLEADER_NAMESPACE()

//
class ClCutSequenceUI_Export RealTimeCutAction : public SequenceEditActionBase
{
public:
	RealTimeCutAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~RealTimeCutAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_RealTime_Cut; }

public:
	void Start();

private:
    SequenceSimulateShapeListPtr SplitSequenceSimulateShapes(const SequenceSimulateShapeList* pSequenceSimulateShapes);

    // ���ơ�ģ����״����
    // ע��
    //   1) CCP coreҲ�ṩ�����Ƶ�ͬ��������
    void DrawSequenceSimulateShapes(const SequenceSimulateShapeList* pSequenceSimulateShapes);

    // ʵʱ�����и�ͷ��λ�á�
    void DrawCutHeader(const Point2D& position);

private:
	RealTimeCutSimulator* m_pRealTimeCutSimulator;

	// ������ģ����״�����ϡ�
	SequenceSimulateShapeListPtr m_pSequenceSimulateShapes;
};

END_CUTLEADER_NAMESPACE()
