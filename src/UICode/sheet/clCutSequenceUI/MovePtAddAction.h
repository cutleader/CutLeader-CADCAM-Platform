#pragma once

#include "SequenceEditActionBase.h"
#include "Point2D.h"
#include <map>

BEGIN_CUTLEADER_NAMESPACE()

// �����п����ƶ�������������ƶ��㡣
// ע��
//  �����ƣ�Ŀǰû���ṩundo/redo���ܡ�
class MovePtAddAction : public SequenceEditActionBase
{
public:
	MovePtAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~MovePtAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_ADD_MOV_PT; }

private:
	// �ҵ�����Ŀ����ƶ��㡣
	void FindClosestMovPt(const Point2D& pt);

private:
	// �������й���֮��Ŀ����ƶ��㡣
	std::map<LONGLONG, Point2DListPtr> m_movePtData;

	// ��ǰ�Ŀ����ƶ��㡣
	// ע��
	//  1) �����ƶ��ߵĽ����㲻��ѡ��
	LONGLONG m_iCurrentSeq;
	Point2D m_curMovPt;

	// ��Ϊ��׼�Ŀ����ƶ��㡣
	// ע��
	//  1) �����ƶ��ߵĽ����㲻��ѡ��
	LONGLONG m_iBaseSeq;
	Point2D m_baseMovPt;
};

END_CUTLEADER_NAMESPACE()
