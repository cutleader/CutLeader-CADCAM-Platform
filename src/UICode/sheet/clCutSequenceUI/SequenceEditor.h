#pragma once

#include "clCutSequenceUICommon.h"
#include "Rect2D.h"
#include "EditorBase.h"
#include "ApplyPartDlg.h"
#include "InsertPositionDlg.h"
#include "GenerateRemCutLineDlg.h"
#include "boost/enable_shared_from_this.hpp"

DECLARE_CUTLEADER_CLASS(IMaterialSize)
DECLARE_CUTLEADER_CLASS(ParamConfig)
DECLARE_CUTLEADER_CLASS(PartLayoutData)
DECLARE_CUTLEADER_CLASS(CutSequenceList)

BEGIN_CUTLEADER_NAMESPACE()

// 工序编辑器。
class ClCutSequenceUI_Export SequenceEditor : public EditorBase,
									 public boost::enable_shared_from_this<SequenceEditor>
{
public:
	SequenceEditor(GlViewPortPtr pViewPort);
	~SequenceEditor(void);

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
	void SetMatSize(IMaterialSizePtr pMaterialSize) { m_pMatSize = pMaterialSize; }
	IMaterialSizePtr GetMatSize() const { return m_pMatSize; }

	void SetPartLayoutData(PartLayoutDataPtr pPartLayoutData) { m_pPartLayoutData = pPartLayoutData; }
	PartLayoutDataPtr GetPartLayoutData() const { return m_pPartLayoutData; }

	void SetCutSequences(CutSequenceListPtr pCutSequences) { m_pCutSequences = pCutSequences; }
	CutSequenceListPtr GetCutSequences() const { return m_pCutSequences; }

	void SetParamConfig(ParamConfigPtr pParamConfig) { m_pParamConfig = pParamConfig; }
	ParamConfigPtr GetParamConfig() const { return m_pParamConfig; }

public:
	BOOL SetAction(SequenceActionType actionType);

	// “传播相同零件”对话框。
	void InitApplyPartDlg();
	void ShowApplyPartDlg();
	void HideApplyPartDlg();

	// “指定插入位置”对话框。
	void InitInsertPosDlg();
	void ShowInsertPosDlg();
	void HideInsertPosDlg();

	// “生成余料切割线”对话框。
	void Init_GenerateRemCutLineDlg();
	void Show_GenerateRemCutLineDlg();
	void Hide_GenerateRemCutLineDlg();

    // 销毁SheetView的子对话框
    void DestroyChildDialogOfSheetView();

private:
	// 板材用到的材料。
	IMaterialSizePtr m_pMatSize;

	// the layout info of the sheet.
	PartLayoutDataPtr m_pPartLayoutData;

	// the sequence unit list of the sheet.
	CutSequenceListPtr m_pCutSequences;

	// 板材用到的参数配置。
	ParamConfigPtr m_pParamConfig;

	// “以零件为单位”对话框。
	ApplyPartDlg* m_pApplyPartDlg;

	// “指定插入位置”对话框。
	InsertPositionDlg* m_pInsertPosDlg;

	// “生成余料切割线”对话框。
	GenerateRemCutLineDlg* m_pGenerateRemCutLineDlg;
};

END_CUTLEADER_NAMESPACE()
