#pragma once

#include "SequenceEditActionBase.h"
#include "ObserverBase.h"
#include "Point2D.h"
#include "Rect2D.h"
#include <map>
#include "GenerateRemCutLineDlg.h"

DECLARE_CUTLEADER_CLASS(Line2DList)

BEGIN_CUTLEADER_NAMESPACE()

// �Զ����������и���
// ע��
//  �����ƣ�Ŀǰû���ṩundo/redo���ܡ�
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
	// �ӹ�����������������
	int BeamWidth_2_BeamNum(RECT_SIDE iRayDir, double dBeamWidth);

private:
	GenerateRemCutLineDlg* m_pGenerateRemCutLineDlg;

	// ���ղ����������и��ߣ����������и��߹���ʱ������Ҫ����
	Line2DListPtr m_pRemCutLines;
};

END_CUTLEADER_NAMESPACE()
