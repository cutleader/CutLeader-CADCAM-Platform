#pragma once

#include "PartLayoutPropertyBar.h"
#include "DataPropItem.h"
#include <vector>
#include <map>

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// this bar will display the properties of the part placement.
class ClPartLayoutUI_Export PartPlacementPropertyBar : public PartLayoutPropertyBar
{
private:
	PartPlacementPropertyBar(void);

public:
	virtual ~PartPlacementPropertyBar(void);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnPropertyChange(WPARAM wParam, LPARAM lParam);

	afx_msg void OnSetGridGap();
	afx_msg void OnUpdateClickBtn(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

public:
	static PartPlacementPropertyBar* GetInstance(CWnd* pParent = NULL);

	// display the props of part pmt.
	// notes:
	//   1) if more than one part pmt in "pPartPlacements", we only display some basic data for "pPartPlacements" and cannot modify them.
	void DisplayPartPmt(PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, GlViewPortPtr pViewPort);

	// 更新“零件放置”的属性。
	// 注：
	//  1) 显示一个“零件放置”属性时调用这个函数。
	void UpdatePmtProp();

private:
	void AdjustLayout();

	BOOL CreatePropWnd();
	BOOL CreateSetGapBtn();

	// init a prop group with Prop items
	void InitPropGroup(int iGroupName, std::vector<DataPropItem>& propItems);

private: // for UI.
	// the singleton object
	static PartPlacementPropertyBar *m_pPartPmtPropBar;

	CMFCPropertyGridCtrl m_wndPropList;

	// the button to set the gap of grid.
	CButton m_setGridGapBtn;

	// the button to edit part.
	CButton m_editPartBtn;

	std::map<CObject*, DataPropItem> m_propInfo;

private: // for data member.
	GlViewPortPtr m_pViewPort;

	// the part pmt which will be edited.
	PartPlacementListPtr m_pPartPlacementList;

	PartInstanceListPtr m_pPartInstanceList;
};

END_CUTLEADER_NAMESPACE()
