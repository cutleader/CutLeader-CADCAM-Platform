#pragma once

#include "clFrameCommon.h"
#include "taoResource.h"

DECLARE_CUTLEADER_CLASS(PatternPropertyBar)
DECLARE_CUTLEADER_CLASS(LoopToolEditPanel)
DECLARE_CUTLEADER_CLASS(LoopLeadPropertyBar)
DECLARE_CUTLEADER_CLASS(MicroJointPropertyBar)
DECLARE_CUTLEADER_CLASS(CornerPropertyBar)
DECLARE_CUTLEADER_CLASS(PartPlacementViewBar)
DECLARE_CUTLEADER_CLASS(PartPlacementPropertyBar)
DECLARE_CUTLEADER_CLASS(PartPlacementAddPanel)
DECLARE_CUTLEADER_CLASS(PartTopologyBar)
DECLARE_CUTLEADER_CLASS(CutSequencePane)
DECLARE_CUTLEADER_CLASS(CutNodeViewBar)
DECLARE_CUTLEADER_CLASS(NCViewBar)
DECLARE_CUTLEADER_CLASS(InteractBar)
DECLARE_CUTLEADER_CLASS(CommonFuncPane)

BEGIN_CUTLEADER_NAMESPACE()

// 面板管理器。
class CLFRAME_Export BarManager
{
public:
	BarManager(void);
	~BarManager(void);

public:
	// notes: 
	//  1) we should attach bar in same side by call "AttachToTabWnd", otherwise, the bar will be more and more narrow.
	void Init(CDockingManager* pDockingMgr, CWnd* pParent);

	CWnd* GetMainFrm() const { return m_pParent; }

	// hide all bars.
	void HideAllBar();
	void HideAllRightBar();

private:
	// for "geometry" module.
	BOOL CreatePatternPropBar();

	// for "cam feature" module.
    BOOL CreateLoopToolEditPane();
	BOOL CreateLoopLeadPropBar();
	BOOL CreateMicroJointPropBar();
	BOOL CreateCornerPropBar();

	// for "part placement" module.
	BOOL CreateAddPrtInstPane();
	BOOL CreatePartPmtPropBar();
	BOOL CreatePartPmtViewBar();
	BOOL CreatePartTopBar();

	// for "cutSeq" module.
	BOOL CreateSeqUnitPane();
	BOOL CreateCutNodeViewBar();

	// for "postProcessor" module.
	BOOL CreateNCViewBar();

	// the output bars.
	BOOL CreateOutputBar();

	// 常用功能面板。
	BOOL CreateCommonFuncPane();

private:
	CWnd* m_pParent;
	CDockingManager* m_pDockingMgr;

	// for "geometry" module.
	PatternPropertyBar* m_pPatPropBar;

	// for "CAM Feature" module.
    LoopToolEditPanel* m_pLoopToolEditPane;
	LoopLeadPropertyBar* m_pLoopLeadPropBar;
	MicroJointPropertyBar* m_pMicroJointPropBar;
	CornerPropertyBar* m_pCornerPropBar;

	// for "part Layout" module.
	PartPlacementAddPanel* m_pAddPrtPmtPane;
	PartPlacementPropertyBar* m_pPartPmtPropBar;
	PartPlacementViewBar* m_pPartPmtViewBar;
	PartTopologyBar* m_pPartTopBar;

	// for "cutSeq" module.
	CutSequencePane* m_pSeqUnitPane;
	CutNodeViewBar* m_pCutNodeViewBar;

	// for "postProcessor" module.
	NCViewBar* m_pNCViewBar;

	// the value input bar.
	InteractBar* m_pInteractBar;

	// 常用功能面板。
	CommonFuncPane* m_pCommonFuncPane;
};

END_CUTLEADER_NAMESPACE()
