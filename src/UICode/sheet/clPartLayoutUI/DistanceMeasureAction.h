#pragma once

#include "PartPlacementEditAction.h"

DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// ������������롱�Ķ�����
class DistanceMeasureAction : public PartPlacementEditAction
{
	typedef enum tagMeasureDisState
	{
		NONE					= 0,
		FIRST_LOOP_SELECTED		= 1,
		FIRST_PT_SELECTED		= 2,
		SECOND_LOOP_SELECTED	= 3,
		SECOND_PT_SELECTED		= 4,
	} MEASURE_DIS_STATE;

public:
	DistanceMeasureAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort);
	virtual ~DistanceMeasureAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return MEASURE_PT_DISTANCE; }

private:
	// ���������Ļ�·��
	// ע��
	//  1) �����߿�Ҳ�㡰��·����
	LineArc2DListPtr CalcClosestLoop(const Point2D& pt);

private:
	MEASURE_DIS_STATE m_iState;

	// ����������Ļ�·��
	LineArc2DListPtr m_pCloseLoop;

	// �����㡣
	Point2D m_fstPt;
	Point2D m_sndPt;

	// �������е���Χ����·ʵ������
	LoopInstanceListPtr m_pAllLoopInstances;

	// �����ı߿�Ļ���ͼ�Ρ�
	LineArc2DListPtr m_pSheetBorderGeoms;
};

END_CUTLEADER_NAMESPACE()
