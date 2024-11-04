#pragma once

#include "Rect2D.h"
#include "ActionBase.h"
#include "IEditorInfo.h"
#include "DistanceDetectDlg.h"


DECLARE_CUTLEADER_CLASS(PartPlacement)
DECLARE_CUTLEADER_CLASS(PartGroupList)


BEGIN_CUTLEADER_NAMESPACE()

class ClPartLayoutUI_Export PartPlacementEditAction : public ActionBase, public IEditorInfo
{
public:
	PartPlacementEditAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort);
	virtual ~PartPlacementEditAction(void);

public: // implement interface IEditorInfo.
	virtual EditorData GetEditorData() const;

public: // implement IInteract interface.
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
    // ������Ϣ����ͼ��Ҫ��������������ѡ������
    static void SendMessage2EnterPartPlacementSelectAction(); // �����������ѡ��������Ԥѡ��������á�
    static void SendMessage2EnterLoopInstanceSelectAction(); // ��������ʵ��ѡ��������Ԥѡ������ʵ����
    static void SendMessage2EnterPartPlacementOrLoopInstanceSelectAction(); // ˳����һ�ε�ѡ��״̬����Ԥѡ�����
    static void SendMessage2EnterPartPlacementSelectAction(const PartPlacementList* pPartPlacements_preSelect); // �����������ѡ��������Ԥѡ��������á�
    static void SendMessage2EnterPartPlacementSelectAction(const PartPlacement* pPartPlacement_preSelect); // �����������ѡ��������Ԥѡ��������á�

protected:
	// ����õĲ��ϡ�
	IMaterialSizePtr m_pMatSize;

	// part placements and instances of the sheet.
	// notes: 
	//   1) not all sub-class will use these two variables.
	PartPlacementListPtr m_pPartPlacementList;
	PartInstanceListPtr m_pPartInstanceList;

    // �����������
    PartGroupListPtr m_pPartGroupList;
};

END_CUTLEADER_NAMESPACE()
