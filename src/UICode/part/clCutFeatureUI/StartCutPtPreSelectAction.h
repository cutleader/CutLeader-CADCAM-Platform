#pragma once

#include "CutFeatureEditBase.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(LoopStartCutData)

BEGIN_CUTLEADER_NAMESPACE()

// һЩ�����Ļ��࣬��Щ����֧�ֵ�ѡ�Ϳ�ѡ����·���С�������
// ע��
//   1) ͨ��ѡ�����е㡱��ѡ�񡰻�·���С�������
class StartCutPtPreSelectAction : public CutFeatureEditBase
{
protected:
	enum PRE_SELECT_STATE
	{
		// ��û�п�ʼѡ��
		PRE_SELECT_NO_INPUT			= 0,

		// ���������¡�
		PRE_SELECT_LEFT_BTN_DOWN	= 1,

		// ������̧��
		PRE_SELECT_LEFT_BTN_UP		= 2,
	};

public:
    StartCutPtPreSelectAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort);
	~StartCutPtPreSelectAction(void);

public:  // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	BOOL DrawBeforeFinish();

private:
	// �����ѡ�еġ���·���С�������
	std::vector<std::pair<LoopStartCutDataPtr, Point2D>> CalcSelLoopStaEndCut();

protected:
	// ������������е㡣
	Point2DPtr m_pActiveStartPt;

	// ѡ�е����е㼰��Ӧ�ġ���·������������
	std::vector<std::pair<LoopStartCutDataPtr, Point2D>> m_selStaEndPtPairs;

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
