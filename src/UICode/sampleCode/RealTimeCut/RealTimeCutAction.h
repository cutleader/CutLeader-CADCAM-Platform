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

    // 绘制“模拟形状”。
    // 注：
    //   1) CCP core也提供了类似的同名函数。
    void DrawSequenceSimulateShapes(const SequenceSimulateShapeList* pSequenceSimulateShapes);

    // 实时绘制切割头的位置。
    void DrawCutHeader(const Point2D& position);

private:
	RealTimeCutSimulator* m_pRealTimeCutSimulator;

	// “工序模拟形状”集合。
	SequenceSimulateShapeListPtr m_pSequenceSimulateShapes;
};

END_CUTLEADER_NAMESPACE()
