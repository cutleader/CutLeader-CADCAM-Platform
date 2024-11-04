#pragma once

#include "clDataManagerCommon.h"
#include "taoresource.h"
#include "CLResizableDialog.h"
#include "InvCellGrid.h"

DECLARE_CUTLEADER_CLASS(Sheet)

BEGIN_CUTLEADER_NAMESPACE()

// use this page to display the sheet info.
// notes:
//   1) if we changed some props in "Basic info" group, we can get the changes from "m_pSheet", so we can save these props outside the dialog.
//   2) we cannot repair sheet in this page, if you want to repair sheet, open SheetRepairDlg.
class ClDataManager_Export SheetInfoPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(SheetInfoPage)

public:
	SheetInfoPage();
	~SheetInfoPage();

	enum { IDD = IDD_clDataManagerUI_SheetInfo };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected: // message mapping.
	afx_msg void OnKillSheetName();
	afx_msg void OnKillSheetComment();

	afx_msg void OnChangeMat();

	// change the scheme of the sheet.
	// notes:
	//   1) if current scheme is invalid, cannot enter this function. so if you 
	//      want to repair it, open SheetRepairDlg.
	//   2) if current scheme is valid, only can change scheme in "SheetSchmSelDlg" dialog.
	afx_msg void OnChangeScheme();

	afx_msg void OnRepairSheet();

	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

public:
	// display the sheet that will be edited.
	void DisplaySheetInfo(SheetPtr pSheet);

	SheetPtr GetSheet() { return m_pSheet; }

	// update the sheet info after repair it.
	void UpdateAfterRepair();

	// update the control status.
	void UpdateCtrlStatus();

	// update the modify date.
	// notes:
	//   1) get the modify date from sheet object.
	void UpdateModifyDate();

private:
	// init the column header of the list ctrl.
	void InitGridStyle();

	/************************************************************************/
	// init the data.

	// init Material Info group.
	void InitMatGroup();

	// init the "scheme info" group.
	void InitSchemeGroup();

	// init the content of the list ctrl.
	void InitPartGridData();
	void InitNestGridData();
	/************************************************************************/

	// preview sheet
	void PreviewSheet();

	// release the data which stored in grid.
	void ReleaseGridItemData();

private: // controls
	// basic info group.
	CString m_strName;
	CString m_strComment;
	CString m_strCreateDate;
	CString m_strModifyDate;

	// Material Info group.
	CString m_strMat;
	CString m_strMatThick;
	CString m_strMatSize;

	// scheme info group.
	CString m_strExpLibItem;
	CString m_strParamConfig;

	// wnd for preview.
	CStatic m_previewWnd;

	//
	InvCellGrid m_partItemGrid;
	CGridListCtrlGroups m_nestGrid;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data member
	// the sheet which will be displayed in this dialog.
	// notes:
	//   1) this may be used outside this dialog.
	//   2) 板材对象是以预览的方式从数据库加载的。
	SheetPtr m_pSheet;
};

END_CUTLEADER_NAMESPACE()
