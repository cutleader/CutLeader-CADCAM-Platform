#pragma once

#include "clMaterialLibUICommon.h"
#include "LineArc2DStaticControl.h"
#include "CGridListCtrlGroups.h"
#include "CLResizableDialog.h"
#include "taoResource.h"
#include "NumEdit.h"
#include "IMaterialSize.h"

BEGIN_CUTLEADER_NAMESPACE()

// �����ϳߴ硱ҳ�档
class ClMaterialLibUI_Export MaterialSizeInfoPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(MaterialSizeInfoPage)

public:
	MaterialSizeInfoPage();
	~MaterialSizeInfoPage();

	enum { IDD = IDD_clMaterialLibUI_MaterialSize_Page };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected:
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnKillName();
	afx_msg void OnKillComment();
	afx_msg void OnSelChangeGrainDir();
	afx_msg void OnKillWidth();
	afx_msg void OnKillHeight();
	afx_msg void OnKillCount();

	afx_msg void OnCCWRot();

	DECLARE_MESSAGE_MAP()

public:
	// display the info of the material size.
	void DisplayMatSize(IMaterialSizePtr pMaterialSize);

	IMaterialSizePtr GetMatSize() { return m_pMatSize; }

private:
	void PreviewMatSize();

private: // controls
	// "basic info" group.
	CString m_strName;
	CString m_strComment;
	CComboBox m_ctrlIsRemnant;
	CNumEdit m_dWidth;
	CNumEdit m_dHeight;

	// "stock info" group.
	CNumEdit m_iCount;
	CNumEdit m_iUsed;

	// ͼ��Ԥ�����档
	LineArc2DStaticControl m_matPreviewWnd;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data member.
	IMaterialSizePtr m_pMatSize;
};

END_CUTLEADER_NAMESPACE()
