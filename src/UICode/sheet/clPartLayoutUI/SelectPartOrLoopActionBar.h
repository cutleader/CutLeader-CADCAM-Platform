#pragma once

#include "clPartLayoutUICommon.h"
#include "taoresource.h"


BEGIN_CUTLEADER_NAMESPACE()

// 选择零件和选择轮廓动作的action工具条。
class ClPartLayoutUI_Export SelectPartOrLoopActionBar : public CLDialog
{
	DECLARE_DYNCREATE(SelectPartOrLoopActionBar)

public:
	SelectPartOrLoopActionBar(CWnd* pParent = NULL);
	~SelectPartOrLoopActionBar(void);

	enum { IDD = IDD_clPartLayoutUI_SelectPartOrLoopActionBar };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelectLoopInstance();
    afx_msg void OnAutoSelectSamePartsOrLoops();
    afx_msg void OnHideCutSequence();

	DECLARE_MESSAGE_MAP()

public:
    BOOL GetIsSelectingLoopInstance() const { return m_bSelectLoopInstance; }
    BOOL GetIsAutoSelectSamePartsOrLoops() const { return m_bAutoSelectSamePartsOrLoops; }

private:
	CBrush m_brush;

    BOOL m_bSelectLoopInstance;
    BOOL m_bAutoSelectSamePartsOrLoops;
	BOOL m_bHideCutSequence;
};

END_CUTLEADER_NAMESPACE()
