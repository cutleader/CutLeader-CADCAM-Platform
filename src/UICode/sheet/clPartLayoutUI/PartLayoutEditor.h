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


// ������ֱ༭����
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
	// ����action���ڰ���������������е������
	void AddPartPmt(LONGLONG iPartID, LONGLONG iShtParamConfigID);

	// ����action���ڰ��������ӡ��������ϡ�
	void AddPartPmt(PartItemListPtr pPartItemList, const std::map<LONGLONG, std::vector<Matrix2D>>& matrixData_of_sameShapePart, FolderNodePtr pSaveFolder);

    // ����������ö�������Ԥ��ѡ��ָ���������
    void StartPartPlacementSelectAction_andPreSelect(PartPlacementListPtr pPartPlacements_preSelect);

	BOOL SetEditAction(EDIT_SHEET_ACTION_TYPE actionType);
	BOOL SetViewAction(VIEW_ACTION_TYPE viewType);


	/************************************************************************/
	// һЩ�����Ի���

	// ��ʼ�������Ի���
	void InitFloatDlg();

	// �����ܿ����������Ի���
	void ShowDisDetectDlg(PartPlacementMoveBase* pPartPmtMoveBase);
	void HideDisDetectDlg();

	// ��������ͬ������Ի���
	void ShowSpreadSamePartDlg();
	void HideSpreadSamePartDlg();

	// ������������С��Ի���
	void HidePartGridDlg();

    //
    void ShowSelectPartOrLoopActionBar();
    void HideSelectPartOrLoopActionBar();

    // ����SheetView���ӶԻ���
    void DestroyChildDialogOfSheetView();
	/************************************************************************/

private:
    // �õ���ǰѡ�е�����������ǰѡ�����������Ǿͷ����������ڵ��������Ҫȥ�ظ�����
    PartPlacementListPtr GetSelectedPartPlacements();

    // �õ���ǰѡ�е������������ǰѡ����������Ǿͷ��������������
    LoopInstanceListPtr GetSelectedLoopInstances();

private:
	PartPlacementListPtr m_pPartPlacementList;
	PartInstanceListPtr m_pPartInstanceList;
    PartGroupListPtr m_pPartGroups;

	// ����õ��Ĳ��ϡ�
	IMaterialSizePtr m_pMatSize;

	// ����õ��Ĳ������á�
	ParamConfigPtr m_pParamConfig;


	/************************************************************************/
	// һЩ�����Ի���

	// �����ܿ����������Ի���
	// ע��
	//  1) Ư������ͼ�����Ͻǡ�
	DistanceDetectDlg* m_pDisDetectDlg;

	// ��������ͬ������Ի���
	// ע��
	//  1) Ư������ͼ�����Ͻǡ�
	SpreadSamePartDlg* m_pSpreadSamePartDlg;

	// ������������С��Ի���
	// ע��
	//  1) Ư������ͼ�����Ͻǡ�
	CreatePartGridDlg* m_pCreatePartGridDlg;

    // ѡ�������ѡ������������action��������
    SelectPartOrLoopActionBar* m_pSelectPartOrLoopActionBar;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
