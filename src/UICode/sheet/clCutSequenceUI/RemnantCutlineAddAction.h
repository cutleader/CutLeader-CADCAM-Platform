#pragma once

#include "SequenceEditActionBase.h"
#include "Point2D.h"
#include <map>

BEGIN_CUTLEADER_NAMESPACE()

// ���������и���
// ע��
//  �����ƣ�Ŀǰû���ṩundo/redo���ܡ�
class RemnantCutlineAddAction : public SequenceEditActionBase
{
public:
	RemnantCutlineAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~RemnantCutlineAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL RButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_ADD_REM_CUTLINE; }
	virtual bool IsProcessRightClickEvent() const override { return true; }

private:
	// �и��ߵĶ˵㡣
	Point2DListPtr m_pEndPts_of_remCutLine;

	// ��ǰ���λ�á�
	Point2D m_currentPos;
};

END_CUTLEADER_NAMESPACE()
