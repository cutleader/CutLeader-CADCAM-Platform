#pragma once

#include "PartPlacementEditAction.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(CornerList)
DECLARE_CUTLEADER_CLASS(ICorner)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// һЩ�����Ļ��࣬��Щ����֧�ֵ�ѡ�Ϳ�ѡ��������
// ע��
//   1) ͨ��ѡ�񡰽ǵ㡱��ѡ�񡰽���������
class CornerPreSelectAction2 : public PartPlacementEditAction
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
	CornerPreSelectAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort);
	~CornerPreSelectAction2(void);

public:  // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	BOOL DrawBeforeFinish();

public:
	// �õ�ѡ�еĽ�������
	CornerListPtr GetSelectCornerList();

private:
	// �����ѡ�еġ�����������
	std::vector< std::pair<ICornerPtr, Point2D> > CalcSelectCornerPt();

protected:
	// ��������Ľ������㡣
	Point2DPtr m_pActiveCornerPt;

	// ѡ�еġ��������㡱����Ӧ�Ľ���������
	std::vector< std::pair<ICornerPtr, Point2D> > m_selCornerPtPairs;

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
