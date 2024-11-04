#pragma once

#include "clCutSequenceUICommon.h"
#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(CutNodeSimulator)
DECLARE_CUTLEADER_CLASS(SequenceSimulateShapeList)

BEGIN_CUTLEADER_NAMESPACE()

// ����ģ�⶯����
class ClCutSequenceUI_Export SequenceViewAction : public SequenceEditActionBase
{
public:
	SequenceViewAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~SequenceViewAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_VIEW; }

public: // other functions.
	void Start();

private:
	// get the message by the simulation speed.
	int GetMsgBySpeed(int iSpeed);

private:
	CutNodeSimulator* m_pCutNodeSimulator;

	// ������ģ����״�����ϡ�
	SequenceSimulateShapeListPtr m_pSequenceSimulateShapeList;
};

END_CUTLEADER_NAMESPACE()
