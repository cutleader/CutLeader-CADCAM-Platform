#pragma once

#include "clGeometryBaseCommon.h"
#include "ClData.h"
#include "ParamConfigItem.h"

DECLARE_CUTLEADER_CLASS(ComCutParam)
DECLARE_CUTLEADER_CLASS(BridgeCutParam)
DECLARE_CUTLEADER_CLASS(GridCutParam)
DECLARE_CUTLEADER_CLASS(FlyCutParam)
DECLARE_CUTLEADER_CLASS(CollisionAvoidParam)
DECLARE_CUTLEADER_CLASS(SequenceConfigItem)
DECLARE_CUTLEADER_CLASS(ChainCutParam)

BEGIN_CUTLEADER_NAMESPACE()

// “工序分配规则”。
class ClExpertLib_Export SequenceConfigItem : public ClData,
								   virtual public ParamConfigItem
{
public:
	SequenceConfigItem(void);
	virtual ~SequenceConfigItem(void);

public: // implements IData interface.
	virtual IDataPtr Clone() const override;

public: // implements ParamConfigItem interface.
	PARAMCONFIG_ITEM_TYPE GetParamConfigItemType() const override { return PARAMCONFIG_ITEM_GENERATE_SEQ; }

public: // get/set functions.
	void SetPartSeqDir(SequencePolicyType seqDir) { m_partSeqDir = seqDir; }
	SequencePolicyType GetPartSeqDir() const { return m_partSeqDir; }

	void SetLoopSeqDir(SequencePolicyType seqDir) { m_loopSeqDir = seqDir; }
	SequencePolicyType GetLoopSeqDir() const { return m_loopSeqDir; }

	void SetStartCorner(RECT_CORNER startCorner) { m_startCorner = startCorner; }
	RECT_CORNER GetStartCorner() const { return m_startCorner; }

	void IsUseComCut(BOOL bUseComCut) { m_bUseComCut = bUseComCut; }
	BOOL IsUseComCut() const { return m_bUseComCut; }

	void SetComCutParam(ComCutParamPtr pComCutParam) { m_pComCutParam = pComCutParam; }
	ComCutParamPtr GetComCutParam() const { return m_pComCutParam; }

	void SetBdgeCutParam(BridgeCutParamPtr pBdgeCutParam) { m_pBdgeCutParam = pBdgeCutParam; }
	BridgeCutParamPtr GetBdgeCutParam() const { return m_pBdgeCutParam; }

	void SetGridCutParam(GridCutParamPtr pGridCutParam) { m_pGridCutParam = pGridCutParam; }
	GridCutParamPtr GetGridCutParam() const { return m_pGridCutParam; }

	void SetFlyCutParam(FlyCutParamPtr pFlyCutParam) { m_pFlyCutParam = pFlyCutParam; }
	FlyCutParamPtr GetFlyCutParam() const { return m_pFlyCutParam; }

	void SetCollisionAvoidParam(CollisionAvoidParamPtr pCollisionAvoidParam) { m_pCollisionAvoidParam = pCollisionAvoidParam; }
	CollisionAvoidParamPtr GetCollisionAvoidParam() const { return m_pCollisionAvoidParam; }

	void IsSoftwareOffset(BOOL bSoftwareOffset) { m_bSoftwareOffset = bSoftwareOffset; }
	BOOL IsSoftwareOffset() const { return m_bSoftwareOffset; }

	void IsCheckIntf(BOOL bCheckIntf) { m_bCheckIntf = bCheckIntf; }
	BOOL IsCheckIntf() const { return m_bCheckIntf; }

	void IsUseAutoChainCut(BOOL bUseAutoChainCut) { m_bUseAutoChainCut = bUseAutoChainCut; }
	BOOL IsUseAutoChainCut() const { return m_bUseAutoChainCut; }

	void SetChainCutParam(ChainCutParamPtr pChainCutParam) { m_pChainCutParam = pChainCutParam; }
	ChainCutParamPtr GetChainCutParam() const { return m_pChainCutParam; }

    void SetIsCutEmbededPartsByInnerLoopCutOrder(BOOL bCutEmbededPartsByInnerLoopCutOrder) { m_bCutEmbededPartsByInnerLoopCutOrder = bCutEmbededPartsByInnerLoopCutOrder; }
    BOOL GetIsCutEmbededPartsByInnerLoopCutOrder() const { return m_bCutEmbededPartsByInnerLoopCutOrder; }

private:
	// the sequence direction policy
	SequencePolicyType m_partSeqDir;
	SequencePolicyType m_loopSeqDir;

	// the sequence start corner
	RECT_CORNER m_startCorner;

	// for common cutting.
	BOOL m_bUseComCut;
	ComCutParamPtr m_pComCutParam;

	// 关于桥接参数。
	BridgeCutParamPtr m_pBdgeCutParam;

	// 田字切割参数。
	GridCutParamPtr m_pGridCutParam;

	// 飞行切割参数。
	FlyCutParamPtr m_pFlyCutParam;

	// 碰撞避免参数
	CollisionAvoidParamPtr m_pCollisionAvoidParam;

	// 是否程序偏移。
	BOOL m_bSoftwareOffset;

	// 自动分配工序时是否执行干涉检查。
	BOOL m_bCheckIntf;

	// 连割参数
	BOOL m_bUseAutoChainCut;
	ChainCutParamPtr m_pChainCutParam;

    // 按照内孔的切割顺序来切割内嵌零件，即切完一个内孔中的内嵌零件后马上去切内孔，而不是切完所有的内嵌零件后才去切内孔。
    BOOL m_bCutEmbededPartsByInnerLoopCutOrder;
};

END_CUTLEADER_NAMESPACE()
