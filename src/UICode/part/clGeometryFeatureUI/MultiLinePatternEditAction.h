#pragma once

#include "LinePatternEditAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// this class can draw polygon-line.
// notes:
//   1) this class delegate the operations to class "LinePatternEditAction".
class MultiLinePatternEditAction : public LinePatternEditAction
{
public:
	MultiLinePatternEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort);
	~MultiLinePatternEditAction(void);

public: // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);

public: // implement IAction interface.
	virtual int GetActionType() const override { return DRAW_MULTILINE; }

private:
	PatternListPtr m_pNewPatList;
};

END_CUTLEADER_NAMESPACE()
