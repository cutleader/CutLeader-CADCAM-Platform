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

// ����༭����
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

	// ��������ͬ������Ի���
	void InitApplyPartDlg();
	void ShowApplyPartDlg();
	void HideApplyPartDlg();

	// ��ָ������λ�á��Ի���
	void InitInsertPosDlg();
	void ShowInsertPosDlg();
	void HideInsertPosDlg();

	// �����������и��ߡ��Ի���
	void Init_GenerateRemCutLineDlg();
	void Show_GenerateRemCutLineDlg();
	void Hide_GenerateRemCutLineDlg();

    // ����SheetView���ӶԻ���
    void DestroyChildDialogOfSheetView();

private:
	// ����õ��Ĳ��ϡ�
	IMaterialSizePtr m_pMatSize;

	// the layout info of the sheet.
	PartLayoutDataPtr m_pPartLayoutData;

	// the sequence unit list of the sheet.
	CutSequenceListPtr m_pCutSequences;

	// ����õ��Ĳ������á�
	ParamConfigPtr m_pParamConfig;

	// �������Ϊ��λ���Ի���
	ApplyPartDlg* m_pApplyPartDlg;

	// ��ָ������λ�á��Ի���
	InsertPositionDlg* m_pInsertPosDlg;

	// �����������и��ߡ��Ի���
	GenerateRemCutLineDlg* m_pGenerateRemCutLineDlg;
};

END_CUTLEADER_NAMESPACE()
