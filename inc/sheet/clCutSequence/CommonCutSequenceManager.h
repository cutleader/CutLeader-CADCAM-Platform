#pragma once

#include "clCutSequenceCommon.h"
#include "Point2D.h"
#include <map>

DECLARE_CUTLEADER_CLASS(LineArc2DList)
DECLARE_CUTLEADER_CLASS(SequenceConfigItem)
DECLARE_CUTLEADER_CLASS(PartInstanceTopologyItem)
DECLARE_CUTLEADER_CLASS(PartInstanceTopologyItemList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(AvoidData)
DECLARE_CUTLEADER_CLASS(ComCutGroupList)
DECLARE_CUTLEADER_CLASS(ComCutGroup)
DECLARE_CUTLEADER_CLASS(CommonCutSequence)
DECLARE_CUTLEADER_CLASS(CutSequenceList)
DECLARE_CUTLEADER_CLASS(IEdge)
DECLARE_CUTLEADER_CLASS(EdgeLoop)
DECLARE_CUTLEADER_CLASS(EdgeLoopList)
DECLARE_CUTLEADER_CLASS(ConfigData)
DECLARE_CUTLEADER_CLASS(MicroJointDatasOfComCutGroup)

BEGIN_CUTLEADER_NAMESPACE()

// “共边工序”管理器。
class ClCutSequence_Export CommonCutSequenceManager
{
public:
	// 找到一个零件拓扑层中的“共边组”。
	// 注：
	//  1) “共边组”中零件实例的外轮廓上面不能有其他工序。
	static ComCutGroupListPtr FindComCutGroups(PartInstanceListPtr pPartInstances, LoopInstanceListPtr pSeqLoopInsts, const SequenceConfigItem* pSeqConfigItem, AvoidDataPtr pAvoidData);

	// 生成“共边工序”。
	static CommonCutSequencePtr GenerateCommonCutSequence(Point2D& lastCutPt, const PartInstanceTopologyItemList* pPartTopItems, const PartInstanceTopologyItem* pPartTopItem, ComCutGroupPtr pComCutGroup,
									 EdgeLoopListPtr pEdgeLoops, LoopInstanceListPtr pSeqLoopInsts, const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, const ConfigData* pConfigData);

	// 偏移“共边工序”。
	static CommonCutSequencePtr ProcessOffset(CommonCutSequencePtr pComCutSequenceUnit, ComCutGroupPtr pComCutGroup);

	// check whether comcut sequence is invalid.
	/* 参数：
	*  pValidGroups: 所有有效的共边组。
	*/
	// 注：
	//  1) 要是共边工序所在的零件实例没有了或者共边组无效了，共边工序也就无效了。
	static BOOL IsComcutInvalid(CommonCutSequencePtr pComCutSequence, PartInstanceListPtr pAllPartInsts);

	// 根据新的“零件实例”更新工序中的一些数据。
	// 注：
	//  1) 更新共边工序中的零件实例ID。
	static void UpdateComcutSeq(CommonCutSequencePtr pComCutSequence, PartInstanceListPtr pOldPartInsts, PartInstanceListPtr pNewPartInsts);

	// 计算“边回路”的起始切割边。
	// 注：
	//  1) 调用前确认这个“边回路”上面有边被切过（哪怕只是过切）。
	static IEdgePtr CalcStartEdge(const Point2D& lastCutPt, EdgeLoopPtr pEdgeLoop);

    // 得到一个共边组上面的微连接信息。
    static MicroJointDatasOfComCutGroupPtr GetMicroJointDatas_of_comCutGroup(ComCutGroupPtr pComCutGroup);
};

END_CUTLEADER_NAMESPACE()
