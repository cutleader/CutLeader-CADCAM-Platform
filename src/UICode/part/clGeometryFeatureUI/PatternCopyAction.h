#pragma once

#include "clGeometryFeatureUICommon.h"
#include "Matrix2D.h"
#include "PatternPreSelectAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// ¡°¿½±´Í¼ÐÎ¡±¶¯×÷¡£
class PatternCopyAction : public PatternPreSelectAction
{
	typedef enum tagModifyState
	{
		COPY_PATTERN_NOTSTART		= 0,
		COPY_PATTERN_BASE_POINT		= 1,
		COPY_PATTERN_DEST_POINT		= 2,
	} COPY_PATTERN_STATE;

public:
	PatternCopyAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort, PatternListPtr pSelPats);
	~PatternCopyAction(void);

public:  // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL RButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);

	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

public:  // implement IAction interface.
	virtual int GetActionType() const override { return EDIT_COPY; }
	virtual BOOL DrawBeforeFinish();
	virtual void UpdateForUndoRedo();

private:
	void SetMatrixValue();

private:
	COPY_PATTERN_STATE m_actionState;

	// use these data to decide the transform matrix.
	double m_dBasePtX, m_dBasePtY;
	double m_dFirstPtX, m_dFirstPtY;
	double m_dSecondPtX, m_dSecondPtY;

	Matrix2D m_modifyPatMat;

	// a new copy of selected patterns.
	// these patterns will be added to part.
	PatternListPtr m_pNewPats;
};

END_CUTLEADER_NAMESPACE()
