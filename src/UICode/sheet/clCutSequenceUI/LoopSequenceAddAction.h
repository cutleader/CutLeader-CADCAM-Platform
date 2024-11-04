#pragma once

#include "SequenceEditActionBase.h"
#include "Rect2D.h"
#include "ApplyPartDlg.h"
#include <map>

DECLARE_CUTLEADER_CLASS(PartInstance)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// 添加“轮廓工序”的动作。
class LoopSequenceAddAction : public SequenceEditActionBase
{
public:
	LoopSequenceAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg);
	~LoopSequenceAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_ADD_LOOPSEQ; }
	virtual void UpdateForUndoRedo();

private:
	// 给零件实例分配工序。
	void AssignPartSeq(PartInstancePtr pPartInstance);

private:
	// 当前的“回路实例”。
	LoopInstancePtr m_pActiveLoopInstance;

	// 当前的零件实例。
	// 注：
	//  1) 以零件为单位时用到。
	PartInstancePtr m_pActivePartInst;

	// 缓存所有的“回路实例”。
	LoopInstanceListPtr m_pLoopInstanceList;

	// “以零件为单位”对话框。
	ApplyPartDlg* m_pApplyPartDlg;

	// 缓存零件实例中各个回路实例的外接矩形。
	std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG, Rect2D>>> m_partRectMap;
};

END_CUTLEADER_NAMESPACE()
