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
    // 发送消息给视图，要求加入零件或轮廓选择动作。
    static void SendMessage2EnterPartPlacementSelectAction(); // 进入零件放置选择动作，不预选择零件放置。
    static void SendMessage2EnterLoopInstanceSelectAction(); // 进入轮廓实例选择动作，不预选择轮廓实例。
    static void SendMessage2EnterPartPlacementOrLoopInstanceSelectAction(); // 顺从上一次的选择状态，不预选择对象。
    static void SendMessage2EnterPartPlacementSelectAction(const PartPlacementList* pPartPlacements_preSelect); // 进入零件放置选择动作，会预选择零件放置。
    static void SendMessage2EnterPartPlacementSelectAction(const PartPlacement* pPartPlacement_preSelect); // 进入零件放置选择动作，会预选择零件放置。

protected:
	// 板材用的材料。
	IMaterialSizePtr m_pMatSize;

	// part placements and instances of the sheet.
	// notes: 
	//   1) not all sub-class will use these two variables.
	PartPlacementListPtr m_pPartPlacementList;
	PartInstanceListPtr m_pPartInstanceList;

    // 所有零件分组
    PartGroupListPtr m_pPartGroupList;
};

END_CUTLEADER_NAMESPACE()
