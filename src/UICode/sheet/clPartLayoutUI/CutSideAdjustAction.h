#pragma once

#include <map>
#include <tuple>
#include "cLCutFeatureCommon.h"
#include "PartPlacementEditAction.h"
#include "SpreadSamePartDlg.h"

DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(ILead)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(LoopInstance)

BEGIN_CUTLEADER_NAMESPACE()

// ��������·ʵ�����������С�
class CutSideAdjustAction : public PartPlacementEditAction
{
	typedef enum tagSideAdjustState
	{
		CUTSIDE_ADJUST_NONE				= 0,
		CUTSIDE_ADJUST_SELECT_LOOPINST	= 1,
	} CUTSIDE_ADJUST_STATE;

public:
	CutSideAdjustAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
        LeadConfigItemPtr pLeadConfigItem, SpreadSamePartDlg* pSpreadSamePartDlg);
	~CutSideAdjustAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return CUTSIDE_ADJUST; }

public:
    void ChangeCutSide_OfSelectedLoopInstance(LoopInstancePtr pSelectedLoopInstance);

private:
	CUTSIDE_ADJUST_STATE m_state;

	// ���еġ���·ʵ������
	LoopInstanceListPtr m_pAllLoopInstanceList;

	// ��ǰ�ġ���·ʵ������
	LoopInstancePtr m_pActiveLoopInstance;

	// ��һ��ѡ��һ������ʱ����¼�����������Ϣ�����л��Ĺ�������Ҫ���֣�����������л�Ϊcuton�����л�������ʱ����Ҫ��������ʱ�����ߡ�
	// ע:
	//  1) ���ѡ������ɢ��������ͬ���������Ҫ�������¼��Щ�������Щ��Ϣ��
	//  2) <LONGLONG, LONGLONG>Ϊ<���ʵ��ID������ID>��������<����ʹ�õ������з�ʽ�������ߣ������ߣ������߳��ȣ������߳���>
	std::map<std::pair<LONGLONG, LONGLONG>, std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>> m_leadDataList_4_keepIt;

	// ���������߷������
	LeadConfigItemPtr m_pLeadConfigItem;

    // ��������ͬ������Ի���
    SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
