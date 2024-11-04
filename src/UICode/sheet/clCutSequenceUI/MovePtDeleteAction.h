#pragma once

#include "SequenceEditActionBase.h"
#include "Point2D.h"
#include <map>

BEGIN_CUTLEADER_NAMESPACE()

// ɾ��ѡ�еĿ����ƶ��㡣
// ע��
//  �����ƣ�Ŀǰû���ṩundo/redo���ܡ�
class MovePtDeleteAction : public SequenceEditActionBase
{
public:
	MovePtDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~MovePtDeleteAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_DEL_MOV_PT; }

private:
	// �õ���������Ŀ����ƶ��㡣
	void FindClosestMovPt(const Point2D& pt);

private:
	// �������п����ƶ��㣬������ҡ�
	std::map<LONGLONG, Point2DListPtr> m_movePtData;

	// ��������Ŀ����ƶ��㡣
	// ע��
	//  1) �����ƶ��������˵㲻��ѡ��
	LONGLONG m_iCurrentSeq;
	Point2D m_curMovPt;
};

END_CUTLEADER_NAMESPACE()
