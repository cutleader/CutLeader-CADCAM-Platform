#pragma once

#include "clCutFeatureUICommon.h"
#include "ActionBase.h"
#include "IEditorInfo.h"

DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PatternLoopList)
DECLARE_CUTLEADER_CLASS(LoopCutFeatureList)

BEGIN_CUTLEADER_NAMESPACE()

class IPatternLoop;
typedef boost::shared_ptr<IPatternLoop> IPatternLoopPtr;

// “制造特征”编辑动作的基类。
class ClCutFeatureUI_Export CutFeatureEditBase : public ActionBase, public IEditorInfo
{
public:
    CutFeatureEditBase(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort);
	virtual ~CutFeatureEditBase(void);

public: // implement interface IEditorInfo.
	virtual EditorData GetEditorData() const;

protected:
	// the pattern list that the editor will use.
	PatternListPtr m_pPatList;

	// the pattern loop list that the editor will use.
	PatternLoopListPtr m_pPatternLoopList;

	// when we move the mouse, there is always a loop closest to the cursor.
	IPatternLoopPtr m_pActiveLoop;

	// the loop feature list that the editor will use.
	LoopCutFeatureListPtr m_pLoopFeatureList;
};

END_CUTLEADER_NAMESPACE()
