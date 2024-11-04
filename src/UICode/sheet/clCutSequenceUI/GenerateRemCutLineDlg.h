#pragma once

#include "clCutSequenceUICommon.h"
#include "taoresource.h"
#include "NumEdit.h"
#include "SubjectBase.h"
#include "Rect2D.h"

BEGIN_CUTLEADER_NAMESPACE()

// ·¢¸øactionµÄÃüÁî
enum GenerateRemCutLineDlg_Cmd
{
	GenerateRemCutLineDlg_noCmd			= -1,
	GenerateRemCutLineDlg_previewCmd	= 0,
	GenerateRemCutLineDlg_generateCmd	= 1,
	GenerateRemCutLineDlg_saveMatCmd	= 2,
	GenerateRemCutLineDlg_cancelCmd		= 3,
};

class ClCutSequenceUI_Export GenerateRemCutLineDlg : public CLDialog, public SubjectBase
{
	DECLARE_DYNAMIC(GenerateRemCutLineDlg)

public:
	GenerateRemCutLineDlg(CWnd* pParent = NULL);
	~GenerateRemCutLineDlg(void);

	enum { IDD = IDD_clCutSequenceUI_CreateRemnantCutLine };

protected: // inherit from the base class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

public: // message mapping.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnPreview();
	afx_msg void OnSaveRemMat();
	afx_msg void OnGenerate();
	afx_msg void OnQuit();

	DECLARE_MESSAGE_MAP()

public:
	void Init(RECT_SIDE iRemnantPosition, double dStepWidth);
	RECT_SIDE GetRemnantPosition() const;
	RECT_SIDE GetRayDir() const;

	double GetOffsetDistance() { return m_ctrlOffsetDistance.Get_Number(); }
	int GetStepWidth() { return (int)m_ctrlStepWidth.Get_Number(); }

	GenerateRemCutLineDlg_Cmd GetCmdType() const { return m_iCmdType; }

private: // for controls.
	CComboBox m_comboRemnantPosition;
	CNumEdit m_ctrlOffsetDistance;
	CNumEdit m_ctrlStepWidth;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data.
	GenerateRemCutLineDlg_Cmd m_iCmdType;
};

END_CUTLEADER_NAMESPACE()
