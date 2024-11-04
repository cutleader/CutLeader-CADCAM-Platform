#pragma once

#include "PartPlacementEditAction.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(LoopStartCutData)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// һЩ�����Ļ��࣬��Щ����֧���ڰ���ϵ�ѡ�Ϳ�ѡ����·������������
// ע��
//   1) ͨ��ѡ�����е㡱��ѡ�񡰻�·������������
class StartCutPtPreSelectAction2 : public PartPlacementEditAction
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
	StartCutPtPreSelectAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort);
	~StartCutPtPreSelectAction2(void);

public:  // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	BOOL DrawBeforeFinish();

private:
	// �����ѡ�еġ���·���С�������
	std::vector< std::pair<LoopStartCutDataPtr, Point2D> > CalcSelLoopStaEndCut();

protected:
	// ������������е㡣
	Point2DPtr m_pActiveStartPt;

	// ѡ�е����е㼰��Ӧ�ġ���·������������
	std::vector< std::pair<LoopStartCutDataPtr, Point2D> > m_selStaEndPtPairs;

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

	// �������еĻ�·ʵ����
	LoopInstanceListPtr m_pAllLoopInstanceList;
};

END_CUTLEADER_NAMESPACE()
