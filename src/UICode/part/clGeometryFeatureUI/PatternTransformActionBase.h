#pragma once

#include "Matrix2D.h"
#include "IPatternTransformAction.h"
#include "PatternPreSelectAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// The base class for actions that will transform the pattern, e.g., move/rotate/flip the pattern.
class PatternTransformActionBase : public IPatternTransformAction,
							   public PatternPreSelectAction
{
public:
	PatternTransformActionBase(void);
	~PatternTransformActionBase(void);

public: // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL RButtonUp(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	// update the action for undo/redo operation.
	// notes:
	//   1) reset the status of the action:
	//      a) if pattern is lost.
	virtual void UpdateForUndoRedo();

protected:
	TRANSFORM_PATTERN_STATE m_actionState;

	// use these data to decide the transform matrix.
	double m_dBasePtX, m_dBasePtY;
	double m_dFirstPtX, m_dFirstPtY;
	double m_dSecondPtX, m_dSecondPtY;

	Matrix2D m_modifyPatMat;

	// a new copy of selected patterns.
	PatternListPtr m_pNewPats;
};

END_CUTLEADER_NAMESPACE()
