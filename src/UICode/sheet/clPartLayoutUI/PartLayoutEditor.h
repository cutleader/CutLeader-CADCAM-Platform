#pragma once

#include "clCutFeatureUICommon.h"
#include "EditorBase.h"
#include "DistanceDetectDlg.h"
#include "SpreadSamePartDlg.h"
#include "CreatePartGridDlg.h"
#include "SelectPartOrLoopActionBar.h"
#include "boost/enable_shared_from_this.hpp"


DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ParamConfig)
DECLARE_CUTLEADER_CLASS(PartItemList)
DECLARE_CUTLEADER_CLASS(PartGroupList)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)


BEGIN_CUTLEADER_NAMESPACE()


// 零件布局编辑器。
class ClPartLayoutUI_Export PartLayoutEditor : public EditorBase, public boost::enable_shared_from_this<PartLayoutEditor>
{
public:
	PartLayoutEditor(GlViewPortPtr pViewPort);
	~PartLayoutEditor(void);

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

public: // set/get functions
	void SetPartPlacements(PartPlacementListPtr pPartPlacements) { m_pPartPlacementList = pPartPlacements; }
	void SetPartInsts(PartInstanceListPtr pPartInstances) { m_pPartInstanceList = pPartInstances; }
    void SetPartGroups(PartGroupListPtr pPartGroups) { m_pPartGroups = pPartGroups; }

	void SetMatSize(IMaterialSizePtr pMaterialSize) { m_pMatSize = pMaterialSize; }
	IMaterialSizePtr GetMatSize() { return m_pMatSize; }

	void SetParamConfig(ParamConfigPtr pParamConfig) { m_pParamConfig = pParamConfig; }
	ParamConfigPtr GetParamConfig() { return m_pParamConfig; }

public:
	// 启动action，在板材上面添加零件库中的零件。
	void AddPartPmt(LONGLONG iPartID, LONGLONG iShtParamConfigID);

	// 启动action，在板材上面添加“零件项”集合。
	void AddPartPmt(PartItemListPtr pPartItemList, const std::map<LONGLONG, std::vector<Matrix2D>>& matrixData_of_sameShapePart, FolderNodePtr pSaveFolder);

    // 启动零件放置动作，并预先选择指定的零件。
    void StartPartPlacementSelectAction_andPreSelect(PartPlacementListPtr pPartPlacements_preSelect);

	BOOL SetEditAction(EDIT_SHEET_ACTION_TYPE actionType);
	BOOL SetViewAction(VIEW_ACTION_TYPE viewType);


	/************************************************************************/
	// 一些浮动对话框。

	// 初始化浮动对话框。
	void InitFloatDlg();

	// “智能靠碰参数”对话框。
	void ShowDisDetectDlg(PartPlacementMoveBase* pPartPmtMoveBase);
	void HideDisDetectDlg();

	// “传播相同零件”对话框。
	void ShowSpreadSamePartDlg();
	void HideSpreadSamePartDlg();

	// “定义零件阵列”对话框。
	void HidePartGridDlg();

    //
    void ShowSelectPartOrLoopActionBar();
    void HideSelectPartOrLoopActionBar();

    // 销毁SheetView的子对话框
    void DestroyChildDialogOfSheetView();
	/************************************************************************/

private:
    // 得到当前选中的零件，如果当前选的是轮廓，那就返回轮廓所在的零件（需要去重复）。
    PartPlacementListPtr GetSelectedPartPlacements();

    // 得到当前选中的轮廓，如果当前选的是零件，那就返回零件的轮廓。
    LoopInstanceListPtr GetSelectedLoopInstances();

private:
	PartPlacementListPtr m_pPartPlacementList;
	PartInstanceListPtr m_pPartInstanceList;
    PartGroupListPtr m_pPartGroups;

	// 板材用到的材料。
	IMaterialSizePtr m_pMatSize;

	// 板材用到的参数配置。
	ParamConfigPtr m_pParamConfig;


	/************************************************************************/
	// 一些浮动对话框。

	// “智能靠碰参数”对话框。
	// 注：
	//  1) 漂浮在视图的右上角。
	DistanceDetectDlg* m_pDisDetectDlg;

	// “传播相同零件”对话框。
	// 注：
	//  1) 漂浮在视图的右上角。
	SpreadSamePartDlg* m_pSpreadSamePartDlg;

	// “定义零件阵列”对话框。
	// 注：
	//  1) 漂浮在视图的右上角。
	CreatePartGridDlg* m_pCreatePartGridDlg;

    // 选择零件和选择轮廓动作的action工具条。
    SelectPartOrLoopActionBar* m_pSelectPartOrLoopActionBar;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
