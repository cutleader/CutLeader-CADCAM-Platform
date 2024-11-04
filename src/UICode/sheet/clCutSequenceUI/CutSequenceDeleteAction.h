#pragma once

#include "CutSequencePreSelectAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// ɾ������Ķ�����
class CutSequenceDeleteAction : public CutSequencePreSelectAction
{
	typedef enum tagStartCutEditState
	{
		SEQUENCE_DELETE_NONE				= 0,
		SEQUENCE_DELETE_SELECT_LOOPINST		= 1,
	} SEQUENCE_DELETE_STATE;

public:
	CutSequenceDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg);
	~CutSequenceDeleteAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_REMOVE; }
	virtual void UpdateForUndoRedo();

public:
	// ѡ����Щ����
	// ע: 
	//   1) ��������������ѡ�еĲ�����
	void SelectSeqUnits(CutSequenceListPtr pCutSequences);

private:
	SEQUENCE_DELETE_STATE m_state;
};

END_CUTLEADER_NAMESPACE()
