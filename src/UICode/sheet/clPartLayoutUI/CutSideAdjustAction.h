#pragma once

#include <map>
#include <tuple>
#include "cLCutFeatureCommon.h"
#include "PartPlacementEditAction.h"
#include "SpreadSamePartDlg.h"

DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(ILead)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(LoopInstance)

BEGIN_CUTLEADER_NAMESPACE()

// 调整“回路实例”的内外切。
class CutSideAdjustAction : public PartPlacementEditAction
{
	typedef enum tagSideAdjustState
	{
		CUTSIDE_ADJUST_NONE				= 0,
		CUTSIDE_ADJUST_SELECT_LOOPINST	= 1,
	} CUTSIDE_ADJUST_STATE;

public:
	CutSideAdjustAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
        LeadConfigItemPtr pLeadConfigItem, SpreadSamePartDlg* pSpreadSamePartDlg);
	~CutSideAdjustAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return CUTSIDE_ADJUST; }

public:
    void ChangeCutSide_OfSelectedLoopInstance(LoopInstancePtr pSelectedLoopInstance);

private:
	CUTSIDE_ADJUST_STATE m_state;

	// 所有的“回路实例”。
	LoopInstanceListPtr m_pAllLoopInstanceList;

	// 当前的“回路实例”。
	LoopInstancePtr m_pActiveLoopInstance;

	// 第一次选中一个轮廓时，记录上面的引线信息。在切换的过程中需要保持，比如把外切切换为cuton，再切换到内切时，需要保留外切时的引线。
	// 注:
	//  1) 如果选中了扩散到其他相同零件，还需要在这里记录那些零件的这些信息。
	//  2) <LONGLONG, LONGLONG>为<零件实例ID，轮廓ID>，后面是<现在使用的内外切方式，引入线，引出线，引入线长度，引出线长度>
	std::map<std::pair<LONGLONG, LONGLONG>, std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>> m_leadDataList_4_keepIt;

	// 引入引出线分配规则。
	LeadConfigItemPtr m_pLeadConfigItem;

    // “传播相同零件”对话框。
    SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
