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

    // �õ�΢�����и�ڵ�Ŀ�ʼ�㡣
    // ע��
    //  1) ����pCutNodeList_of_microJoint���и�һ��΢���Ӳ������и�ڵ㣬���������������ߵ�΢���ӣ��ڵ�����5��(cutline->unpierce->traceline->pierce->cutline)��
    //     Ҫ��û�����������ߣ��Ͳ���5����
    //  2) ������������ʱ����ʼ����ǵ�һ��cutline����㣬û������������ʱ����ʼ�����unpierce�㡣
    static Point2D GetStartPt_of_microjointCutNodes(CutNodeListPtr pCutNodeList_of_microJoint);

	// ����΢���ӵ��и�ڵ㡣
	// ע����ʵ�ʵ��и���΢�����и�ڵ㲻һ��ʱ����Ҫ����
	static CutNodeListPtr ReverseCutNodes(CutNodeListPtr pCutNodeList_of_microJoint);
};

END_CUTLEADER_NAMESPACE()
