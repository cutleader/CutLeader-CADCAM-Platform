#pragma once

#include "CutFeatureEditBase.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(MicroJoint)
DECLARE_CUTLEADER_CLASS(MicroJointList)

BEGIN_CUTLEADER_NAMESPACE()

// һЩ�����Ļ��࣬��Щ����֧�ֵ�ѡ�Ϳ�ѡ΢���ӡ�
// ע��
//   1) ͨ��ѡ��΢���ӵ㡱��ѡ��΢���ӡ�������
class ClCutFeatureUI_Export MicroJointPreSelectAction : public CutFeatureEditBase
{
protected:
	typedef enum tagPreSelectState
	{
		// ��û�п�ʼѡ��
		PRE_SELECT_NO_INPUT			= 0,

		// ���������¡�
		PRE_SELECT_LEFT_BTN_DOWN	= 1,

		// ������̧��
		PRE_SELECT_LEFT_BTN_UP		= 2,
	} PRE_SELECT_STATE;

public:
    MicroJointPreSelectAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort);
	virtual ~MicroJointPreSelectAction(void);

public:  // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	BOOL DrawBeforeFinish();

public:
	// �õ�ѡ�е�΢����������
	MicroJointListPtr GetSelectMJList();

private:
	// �����ѡ�еġ�΢������������
	std::vector<std::pair<MicroJointPtr, Point2D>> CalcSelectMJPt();

protected:
	// ���������΢���ӵ㡣
	Point2DPtr m_pActiveMJPt;

	// ѡ�е�΢���ӵ㼰��Ӧ�ġ�΢������������
	std::vector<std::pair<MicroJointPtr, Point2D>> m_selMjPtPairs;

	// ѡ��״̬��
	PRE_SELECT_STATE m_preSelectState;

	// ѡ�������ݡ�
	/*
		LeftTopPointX;
		LeftTopPointY;
		RightBottomPointX;
		RightBottomPointY;
	*/
	double m_data[4];
};

END_CUTLEADER_NAMESPACE()
