#pragma once

#include "clCutSequenceCommon.h"
#include "Point2D.h"

DECLARE_CUTLEADER_CLASS(MicroJoint)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(CutNodeList)

BEGIN_CUTLEADER_NAMESPACE()

// the class will generate the cut nodes of the micro joint.
class MicroJointCutter
{
public:
	// generate the cut nodes of the micro joint.
	static Point2D GenerateCutNodes(MicroJointPtr pMicroJoint, LoopInstancePtr pLoopInstance, CutNodeListPtr pCutNodeList_of_microJoint);

    // 得到微连接切割节点的开始点。
    // 注：
    //  1) 参数pCutNodeList_of_microJoint：切割一个微连接产生的切割节点，对于有引入引出线的微连接，节点数是5个(cutline->unpierce->traceline->pierce->cutline)。
    //     要是没有引入引出线，就不是5个。
    //  2) 有引入引出线时，开始点就是第一个cutline的起点，没有引入引出线时，开始点就是unpierce点。
    static Point2D GetStartPt_of_microjointCutNodes(CutNodeListPtr pCutNodeList_of_microJoint);

	// 反向微连接的切割节点。
	// 注：当实际的切割反向和微连接切割节点不一样时，需要反向。
	static CutNodeListPtr ReverseCutNodes(CutNodeListPtr pCutNodeList_of_microJoint);
};

END_CUTLEADER_NAMESPACE()
