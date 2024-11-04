#pragma once

#include "SequenceEditActionBase.h"
#include "ObserverBase.h"
#include "Point2D.h"
#include "Rect2D.h"
#include <map>
#include "GenerateRemCutLineDlg.h"

DECLARE_CUTLEADER_CLASS(Line2DList)

BEGIN_CUTLEADER_NAMESPACE()

// 自动生成余料切割线
// 注：
//  待完善：目前没有提供undo/redo功能。
class RemnantCutlineAutoAction : public SequenceEditActionBase, public ObserverBase
{
public:
	RemnantCutlineAutoAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, GenerateRemCutLineDlg* pGenerateRemCutLineDlg);
	~RemnantCutlineAutoAction(void);

public: // implements IInteract interface.
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_AUTO_REM_CUTLINE; }

public: // implement IObserver interface.
	virtual void Update(int iEventID, VARIANT varEventContent) override;

private:
	// 从光柱宽度算出光柱数量
	int BeamWidth_2_BeamNum(RECT_SIDE iRayDir, double dBeamWidth);

private:
	GenerateRemCutLineDlg* m_pGenerateRemCutLineDlg;

	// 光照产生的余料切割线，生成余料切割线工序时可能需要反向。
	Line2DListPtr m_pRemCutLines;
};

END_CUTLEADER_NAMESPACE()
