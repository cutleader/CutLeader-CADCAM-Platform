#include "StdAfx.h"
#include "SequenceSetPage.h"

#include "baseConst.h"
#include "clBaseDataResource.h"

#include "SequenceConfigItem.h"
#include "ComCutParam.h"
#include "BridgeCutParam.h"
#include "CollisionAvoidParam.h"
#include "clExpertLibResource.h"
#include "ComCutParamDlg.h"
#include "BridgeCutParamDlg.h"
#include "CollisionAvoidParamDlg.h"
#include "GridCutParamDlg.h"
#include "GridCutParam.h"
#include "FlyCutParam.h"
#include "FlyCutParamDlg.h"
#include "ChainCutParam.h"
#include "ChainCutParamDlg.h"


BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SequenceSetPage, CResizableDialog)

SequenceSetPage::SequenceSetPage(void)
			: CLResizableDialog(SequenceSetPage::IDD)
{
}

SequenceSetPage::~SequenceSetPage(void)
{
}

void SequenceSetPage::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO_XLINE, m_nPartDir);
	DDX_Radio(pDX, IDC_RADIO_SHORTEST, m_nLoopDir);
	DDX_Radio(pDX, IDC_RADIO_UPPER_LEFT, m_nCorner);

	DDX_Check(pDX, IDC_CHECK_COMCUT, m_bComCut);
	DDX_Check(pDX, IDC_CHECK_CHAINCUT, m_bChainCut);

	DDX_Check(pDX, IDC_CHECK_OFFSET, m_bSoftwareOffset);
	DDX_Check(pDX, IDC_CHECK_INTF, m_bCheckIntf);
    DDX_Check(pDX, IDC_CHECK_INNER_PART_RULE, m_bInnerPartCutRule);
}

BEGIN_MESSAGE_MAP(SequenceSetPage, CResizableDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	// Sequence Direction Group.
	ON_BN_CLICKED(IDC_RADIO_XLINE, OnPartXLine)
	ON_BN_CLICKED(IDC_RADIO_YLINE, OnPartYLine)
	ON_BN_CLICKED(IDC_RADIO_SHORTEST, OnLoopShortest)
	ON_BN_CLICKED(IDC_RADIO_XLINE_1, OnLoopXLine)
	ON_BN_CLICKED(IDC_RADIO_YLINE_1, OnLoopYLine)

	// Start Corner Group.
	ON_BN_CLICKED(IDC_RADIO_LOWER_LEFT, OnLeftBottom)
	ON_BN_CLICKED(IDC_RADIO_LOWER_RIGHT, OnRightBottom)
	ON_BN_CLICKED(IDC_RADIO_UPPER_LEFT, OnLeftTop)
	ON_BN_CLICKED(IDC_RADIO_UPPER_RIGHT, OnRightTop)

	// for common cutting.
	ON_BN_CLICKED(IDC_CHECK_COMCUT, OnCheckComCut)
	ON_BN_CLICKED(IDC_BTN_COMCUT, OnSetComCut)

	ON_BN_CLICKED(IDC_BTN_COLLISION_AVOID, OnSetCollisionAvoid)

	ON_BN_CLICKED(IDC_BTN_BDGECUT, OnSetBdgeCut)
	ON_BN_CLICKED(IDC_BTN_GRIDCUT, OnSetGridCut)
	ON_BN_CLICKED(IDC_BTN_FLYCUT, OnSetFlyCut)

	ON_BN_CLICKED(IDC_CHECK_CHAINCUT, OnCheckChainCut)
	ON_BN_CLICKED(IDC_BTN_CHAINCUT, OnSetChainCut)
END_MESSAGE_MAP()

BOOL SequenceSetPage::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	// init something
	LayoutCtrl();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void SequenceSetPage::LayoutCtrl()
{
	// for "Sequence Direction" group.
	AddAnchor(IDC_GROUP_SEQ_DIR, TOP_LEFT);
	AddAnchor(IDC_RADIO_XLINE, TOP_LEFT);
	AddAnchor(IDC_RADIO_YLINE, TOP_LEFT);
	AddAnchor(IDC_GROUP_SEQ_DIR_1, TOP_LEFT);
	AddAnchor(IDC_RADIO_SHORTEST, TOP_LEFT);
	AddAnchor(IDC_RADIO_XLINE_1, TOP_LEFT);
	AddAnchor(IDC_RADIO_YLINE_1, TOP_LEFT);

	// for "Start Corner" group.
	AddAnchor(IDC_GROUP_START_CORNER, TOP_LEFT);
	AddAnchor(IDC_RADIO_UPPER_LEFT, TOP_LEFT);
	AddAnchor(IDC_RADIO_UPPER_RIGHT, TOP_LEFT);
	AddAnchor(IDC_RADIO_LOWER_LEFT, TOP_LEFT);
	AddAnchor(IDC_RADIO_LOWER_RIGHT, TOP_LEFT);

	// for "Advanced Settings" group.
	AddAnchor(IDC_STATIC_ADV_SET, TOP_LEFT);
	AddAnchor(IDC_CHECK_INTF, TOP_LEFT);
    AddAnchor(IDC_CHECK_INNER_PART_RULE, TOP_LEFT);
	AddAnchor(IDC_CHECK_COMCUT, TOP_LEFT);
	AddAnchor(IDC_BTN_COMCUT, TOP_LEFT);
	AddAnchor(IDC_BTN_CUTAVOID, TOP_LEFT);
	AddAnchor(IDC_BTN_BDGECUT, TOP_LEFT);
	AddAnchor(IDC_BTN_GRIDCUT, TOP_LEFT);
	AddAnchor(IDC_BTN_FLYCUT, TOP_LEFT);
	AddAnchor(IDC_CHECK_CHAINCUT, TOP_LEFT);
	AddAnchor(IDC_BTN_CHAINCUT, TOP_LEFT);

	AddAnchor(IDC_CHECK_OFFSET, TOP_LEFT);
}

void SequenceSetPage::DisplaySeqConfigItem(SequenceConfigItemPtr seqSetting)
{
	m_pSeqSetting = seqSetting;

	// display sequence direction
	if (m_pSeqSetting->GetPartSeqDir() == SequencePolicy_DirectionX)
		m_nPartDir = 0;
	else if (m_pSeqSetting->GetPartSeqDir() == SequencePolicy_DirectionY)
		m_nPartDir = 1;
	if (m_pSeqSetting->GetLoopSeqDir() == SequencePolicy_ShortestPath)
		m_nLoopDir = 0;
	else if (m_pSeqSetting->GetLoopSeqDir() == SequencePolicy_DirectionX)
		m_nLoopDir = 1;
	else if (m_pSeqSetting->GetLoopSeqDir() == SequencePolicy_DirectionY)
		m_nLoopDir = 2;

	// display the sequence start corner
	if (m_pSeqSetting->GetStartCorner() == LEFT_TOP)
		m_nCorner = 0;
	else if (m_pSeqSetting->GetStartCorner() == LEFT_BOTTOM)
		m_nCorner = 2;
	else if (m_pSeqSetting->GetStartCorner() == RIGHT_TOP)
		m_nCorner = 1;
	else if (m_pSeqSetting->GetStartCorner() == RIGHT_BOTTOM)
		m_nCorner = 3;

	// show the advanced setting.
	m_bCheckIntf = m_pSeqSetting->IsCheckIntf();
    m_bInnerPartCutRule = m_pSeqSetting->GetIsCutEmbededPartsByInnerLoopCutOrder();
	m_bComCut = m_pSeqSetting->IsUseComCut();
	m_bChainCut = m_pSeqSetting->IsUseAutoChainCut();

	// �Ƿ����ƫ�ơ�
	if (m_pSeqSetting->IsUseComCut())
	{
		// ǿ������������ƫ�ơ�
		m_bSoftwareOffset = TRUE;
		GetDlgItem(IDC_CHECK_OFFSET)->EnableWindow(FALSE);
	}
	else
	{
		m_bSoftwareOffset = m_pSeqSetting->IsSoftwareOffset();
		GetDlgItem(IDC_CHECK_OFFSET)->EnableWindow(TRUE);
	}

	UpdateData(FALSE);
}

SequenceConfigItemPtr SequenceSetPage::GetSeqConfigItem() 
{
	UpdateData(TRUE);

	/************************************************************************/
	// set sequence direction.

	if (m_nPartDir == 0)
	{
		if (m_pSeqSetting->GetPartSeqDir() != SequencePolicy_DirectionX)
		{
			m_pSeqSetting->SetPartSeqDir(SequencePolicy_DirectionX);
			m_pSeqSetting->SetModified(TRUE);
		}
	}
	else if (m_nPartDir == 1)
	{
		if (m_pSeqSetting->GetPartSeqDir() != SequencePolicy_DirectionY)
		{
			m_pSeqSetting->SetPartSeqDir(SequencePolicy_DirectionY);
			m_pSeqSetting->SetModified(TRUE);
		}
	}

	if (m_nLoopDir == 0)
	{
		if (m_pSeqSetting->GetLoopSeqDir() != SequencePolicy_ShortestPath)
		{
			m_pSeqSetting->SetLoopSeqDir(SequencePolicy_ShortestPath);
			m_pSeqSetting->SetModified(TRUE);
		}
	}
	else if (m_nLoopDir == 1)
	{
		if (m_pSeqSetting->GetLoopSeqDir() != SequencePolicy_DirectionX)
		{
			m_pSeqSetting->SetLoopSeqDir(SequencePolicy_DirectionX);
			m_pSeqSetting->SetModified(TRUE);
		}
	}
	else if (m_nLoopDir == 2)
	{
		if (m_pSeqSetting->GetLoopSeqDir() != SequencePolicy_DirectionY)
		{
			m_pSeqSetting->SetLoopSeqDir(SequencePolicy_DirectionY);
			m_pSeqSetting->SetModified(TRUE);
		}
	}
	/************************************************************************/

	/************************************************************************/
	// set the sequence start corner.

	if (m_nCorner == 0)
	{
		if (m_pSeqSetting->GetStartCorner() != LEFT_TOP)
		{
			m_pSeqSetting->SetStartCorner(LEFT_TOP);
			m_pSeqSetting->SetModified(TRUE);
		}
	}
	else if (m_nCorner == 1)
	{
		if (m_pSeqSetting->GetStartCorner() != RIGHT_TOP)
		{
			m_pSeqSetting->SetStartCorner(RIGHT_TOP);
			m_pSeqSetting->SetModified(TRUE);
		}
	}
	else if (m_nCorner == 2)
	{
		if (m_pSeqSetting->GetStartCorner() != LEFT_BOTTOM)
		{
			m_pSeqSetting->SetStartCorner(LEFT_BOTTOM);
			m_pSeqSetting->SetModified(TRUE);
		}
	}
	else if (m_nCorner == 3)
	{
		if (m_pSeqSetting->GetStartCorner() != RIGHT_BOTTOM)
		{
			m_pSeqSetting->SetStartCorner(RIGHT_BOTTOM);
			m_pSeqSetting->SetModified(TRUE);
		}
	}
	/************************************************************************/

	// �����顣
	if (m_pSeqSetting->IsCheckIntf() != m_bCheckIntf)
	{
		m_pSeqSetting->IsCheckIntf(m_bCheckIntf);
		m_pSeqSetting->SetModified(TRUE);
	}

    if (m_pSeqSetting->GetIsCutEmbededPartsByInnerLoopCutOrder() != m_bInnerPartCutRule)
    {
        m_pSeqSetting->SetIsCutEmbededPartsByInnerLoopCutOrder(m_bInnerPartCutRule);
        m_pSeqSetting->SetModified(TRUE);
    }

	// common cut
	if (m_pSeqSetting->IsUseComCut() != m_bComCut)
	{
		m_pSeqSetting->IsUseComCut(m_bComCut);
		m_pSeqSetting->SetModified(TRUE);
	}

	// ��������ƫ�ơ�
	if (m_pSeqSetting->IsSoftwareOffset() != m_bSoftwareOffset)
	{
		m_pSeqSetting->IsSoftwareOffset(m_bSoftwareOffset);
		m_pSeqSetting->SetModified(TRUE);
	}

	if (m_pSeqSetting->IsUseAutoChainCut() != m_bChainCut)
	{
		m_pSeqSetting->IsUseAutoChainCut(m_bChainCut);
		m_pSeqSetting->SetModified(TRUE);
	}

	return m_pSeqSetting; 
}

void SequenceSetPage::OnPartXLine()
{
	UpdateData(TRUE);
}

void SequenceSetPage::OnPartYLine()
{
	UpdateData(TRUE);
}

void SequenceSetPage::OnLoopShortest()
{
	UpdateData(TRUE);
}

void SequenceSetPage::OnLoopXLine()
{
	UpdateData(TRUE);
}

void SequenceSetPage::OnLoopYLine()
{
	UpdateData(TRUE);
}

void SequenceSetPage::OnLeftTop()
{
	UpdateData(TRUE);
}

void SequenceSetPage::OnRightTop()
{
	UpdateData(TRUE);
}

void SequenceSetPage::OnLeftBottom()
{
	UpdateData(TRUE);
}

void SequenceSetPage::OnRightBottom()
{
	UpdateData(TRUE);
}

void SequenceSetPage::OnCheckComCut()
{
	UpdateData(TRUE);

	if (m_bComCut)
	{
		// ǿ������������ƫ�ơ�
		CButton* pOffsetBtn = (CButton*)GetDlgItem(IDC_CHECK_OFFSET);
		pOffsetBtn->SetCheck(1);
		GetDlgItem(IDC_CHECK_OFFSET)->EnableWindow(FALSE);

		// ��������
		((CButton*)GetDlgItem(IDC_CHECK_CHAINCUT))->SetCheck(0);
	}
	else
	{
		GetDlgItem(IDC_CHECK_OFFSET)->EnableWindow(TRUE);
	}
}

void SequenceSetPage::OnSetComCut()
{
	ComCutParamPtr pComCutParam = m_pSeqSetting->GetComCutParam();

	ComCutParamDlg dlg(pComCutParam);
	if (dlg.DoModal() == IDOK)
	{
		if (pComCutParam->IsModified())
		{
			m_pSeqSetting->SetModified(TRUE);
		}
	}
}

void SequenceSetPage::OnSetBdgeCut()
{
	BridgeCutParamPtr pBdgeCutParam = m_pSeqSetting->GetBdgeCutParam();

	BridgeCutParamDlg dlg(pBdgeCutParam);
	if (dlg.DoModal() == IDOK)
	{
		if (pBdgeCutParam->IsModified())
		{
			m_pSeqSetting->SetModified(TRUE);
		}
	}
}

void SequenceSetPage::OnSetGridCut()
{
	GridCutParamPtr pGridCutParam = m_pSeqSetting->GetGridCutParam();

	GridCutParamDlg dlg(pGridCutParam);
	if (dlg.DoModal() == IDOK)
	{
		if (pGridCutParam->IsModified())
		{
			m_pSeqSetting->SetModified(TRUE);
		}
	}
}

void SequenceSetPage::OnSetFlyCut()
{
	FlyCutParamPtr pFlyCutParam = m_pSeqSetting->GetFlyCutParam();

	FlyCutParamDlg dlg(pFlyCutParam);
	if (dlg.DoModal() == IDOK)
	{
		if (pFlyCutParam->IsModified())
		{
			m_pSeqSetting->SetModified(TRUE);
		}
	}
}

void SequenceSetPage::OnSetCollisionAvoid()
{
	CollisionAvoidParamPtr pCollisionAvoidParam = m_pSeqSetting->GetCollisionAvoidParam();

	CollisionAvoidParamDlg dlg(pCollisionAvoidParam);
	if (dlg.DoModal() == IDOK)
	{
		if (pCollisionAvoidParam->IsModified())
		{
			m_pSeqSetting->SetModified(TRUE);

            // ���ɱ���·��������ܹ���
            if (pCollisionAvoidParam->GetIsGenerateAvoidPath())
            {
                ((CButton*)GetDlgItem(IDC_CHECK_CHAINCUT))->SetCheck(0);
            }
		}
	}
}

void SequenceSetPage::OnCheckChainCut()
{
	UpdateData(TRUE);

	if (m_bChainCut)
	{
		// ���ù���
		((CButton*)GetDlgItem(IDC_CHECK_COMCUT))->SetCheck(0);

        // ���á����ɱ���·����
		m_pSeqSetting->GetCollisionAvoidParam()->SetIsGenerateAvoidPath(FALSE);
	}
}

void SequenceSetPage::OnSetChainCut()
{
	ChainCutParamPtr pChainCutParam = m_pSeqSetting->GetChainCutParam();

	ChainCutParamDlg dlg(pChainCutParam);
	if (dlg.DoModal() == IDOK)
	{
		if (pChainCutParam->IsModified())
		{
			m_pSeqSetting->SetModified(TRUE);
		}
	}
}

BOOL SequenceSetPage::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH SequenceSetPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("EDIT")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("LISTBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, WC_TREEVIEW) == 0)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

END_CUTLEADER_NAMESPACE()