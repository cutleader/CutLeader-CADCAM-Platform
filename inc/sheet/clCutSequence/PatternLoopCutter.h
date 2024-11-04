#pragma once

#include "clCutSequenceCommon.h"

DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(CutNodeList)
DECLARE_CUTLEADER_CLASS(AvoidData)

BEGIN_CUTLEADER_NAMESPACE()

// cut pattern loop and generate cut nodes.
class PatternLoopCutter
{
public:
	// generate the cut node list of the polygon/standard loop instance.
	/* params:
	*  bLeadOutDisable: whether the lead out of the loop inst is disabled.
	*  bDropHead: whether need to drop the header before cut the loop.
	*  bRaiseHead: whether need to raise the header after cut the loop.
	*/
	static CutNodeListPtr CutPolyLoop(LoopInstancePtr pLoopInstance, BOOL bLeadOutDisable, BOOL bDropHead, BOOL bRaiseHead, BOOL bIgnoreMicroJoint);
	static CutNodeListPtr CutStdLoop(LoopInstancePtr pLoopInstance, BOOL bLeadOutDisable, BOOL bDropHead, BOOL bRaiseHead, BOOL bIgnoreMicroJoint);

	// 生成“多边形回路实例”的“切割节点”。
	// 注：
	//   1) 从“开始切割”的基本图形开始切割，不考虑刀具选择、引入引出线和切入切出点。
	static CutNodeListPtr CutPolyLoopA(LoopInstancePtr pLoopInstance);

	// generate the cut node list of the polygon/standard loop instance.
	/* params:
	*  bLeadOutDisable: whether the lead out of the loop inst is disabled.
	*  bDropHead: whether need to drop the header before cut the loop.
	*  bRaiseHead: whether need to raise the header after cut the loop.
	*  pAvoidData: if not NULL, process the avoid data within the array loop inst.
	*  pCutLoopInstList: the loop insts which will be avoided, DO NOT updated here.
	*/
	static CutNodeListPtr CutArrayLoop(LoopInstancePtr pLoopInstance, AvoidDataPtr pAvoidData, const LoopInstanceList* pCutLoopInstList, BOOL bLeadOutDisable = FALSE,
									   BOOL bDropHead = TRUE, BOOL bRaiseHead = TRUE);

private:
	// generate the cut node list of pattern.
	// notes: 
	//   1) we should cut the start pattern two times, because there is a "start point" on it.
	//   2) the cut node for the corner feature will be generated in the "first" pattern.
	static void CutPatList(LoopInstancePtr pLoopInstance, CutNodeListPtr pCutNodeList, BOOL bIgnoreMicroJoint);

    // 当轮廓的引线加在角上，并且过切为负值时，用这个函数，切割的流程有些不同。
    static void CutPatList_4_LeadAtCornerWithNegtiveOverCut(LoopInstancePtr pLoopInstance, CutNodeListPtr pCutNodeList, BOOL bIgnoreMicroJoint);
};

END_CUTLEADER_NAMESPACE()
