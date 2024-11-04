#pragma once

#include "XBarBase.h"
#include "PartAddDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

// use this pane to load part to sheet.
// notes:
//   1) in "OnDblclkList" function, we should check whether the selected part use the same expert library item with the sheet.
class ClPartLayoutUI_Export PartPlacementAddPanel : public XBarBase
{
private:
	PartPlacementAddPanel();

public:
	virtual ~PartPlacementAddPanel();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public:
	static PartPlacementAddPanel* GetInstance(CWnd* pParent = NULL);

	// call this to update this pane.
	void DisplayPartList(PartLayoutEditorPtr pSheetEditor, ParamConfigPtr pParamConfig, FolderNodePtr pWorkspace);

private: // for controls
	// the singleton object
	static PartPlacementAddPanel* m_pAddPrtPmtPane;

private: // data members.
	PartAddDlg m_partAddDlg;
};

END_CUTLEADER_NAMESPACE()
