#pragma once

#include "clCutFeatureUICommon.h"
#include "EditorBase.h"
#include "boost/enable_shared_from_this.hpp"

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PatternLoopList)
DECLARE_CUTLEADER_CLASS(LoopCutFeatureList)
DECLARE_CUTLEADER_CLASS(ParamConfig)

BEGIN_CUTLEADER_NAMESPACE()

// 加工工艺编辑器。
class ClCutFeatureUI_Export CutFeatureEditor : public EditorBase,
						public boost::enable_shared_from_this<CutFeatureEditor>
{
public:
	CutFeatureEditor(GlViewPortPtr pViewPort);
	~CutFeatureEditor(void);

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
	void SetPatternList(PatternListPtr pPatList) { m_pPatList = pPatList; }
	void SetPatternLoopList(PatternLoopListPtr pPatternLoopList) { m_pPatternLoopList = pPatternLoopList; }
	void SetLoopFeatureList(LoopCutFeatureListPtr pLoopFeatureList) { m_pLoopFeatureList = pLoopFeatureList; }

	void SetParamConfig(ParamConfigPtr pParamConfig) { m_pParamConfig = pParamConfig; }
	ParamConfigPtr GetParamConfig() { return m_pParamConfig; }

public: // other functions.
	BOOL SetAction(CUTFEATURE_ACTION_TYPE actionType);

private:
	// the pattern list that the editor will use.
	PatternListPtr m_pPatList;

	// the pattern loop list that the editor will use.
	PatternLoopListPtr m_pPatternLoopList;

	// the loop feature list that the editor will use.
	LoopCutFeatureListPtr m_pLoopFeatureList;

	// the param config used by the part.
	ParamConfigPtr m_pParamConfig;
};

END_CUTLEADER_NAMESPACE()
