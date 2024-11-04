#pragma once

#include "clGeometryFeatureUICommon.h"
#include "EditorBase.h"
#include "boost/enable_shared_from_this.hpp"

DECLARE_CUTLEADER_CLASS(DrawAssistor)
DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PatternLoopList)

BEGIN_CUTLEADER_NAMESPACE()

// Í¼ÐÎ±à¼­Æ÷¡£
class ClGeometryFeatureUI_Export GeometryFeatureEditor : public EditorBase,
								   public boost::enable_shared_from_this<GeometryFeatureEditor>
{
public:
	GeometryFeatureEditor(GlViewPortPtr pViewPort, DrawAssistorPtr pDrawAssistor);
	~GeometryFeatureEditor(void);

public: // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL LButtonDBClk(double& dPtX, double& dPtY);
	BOOL RButtonUp(double& dPtX, double& dPtY);

	BOOL MovePoint(double& dPtX, double& dPtY);

	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IEditor interface.
	virtual CString GetEditorName();

public: // get/set functions.
	// set the pattern list that the editor will use.
	void SetPatternList(PatternListPtr pPatList) { m_pPatList = pPatList; }

	// set the pattern loop list that the editor will use.
	void SetPatternLoopList(PatternLoopListPtr pPatternLoopList) { m_pPatternLoopList = pPatternLoopList; }

public:
	BOOL SetDrawAction(DRAW_ACTION_TYPE drawType);
	BOOL SetViewAction(VIEW_ACTION_TYPE viewType);
	BOOL SetEditAction(EDIT_ACTION_TYPE editType);

private:
	// the pattern list that the editor will use.
	PatternListPtr m_pPatList;

	// the pattern loop list that the editor will use.
	PatternLoopListPtr m_pPatternLoopList;

	DrawAssistorPtr m_pDrawAssistor;
};

END_CUTLEADER_NAMESPACE()
