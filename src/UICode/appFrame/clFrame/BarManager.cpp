#include "StdAfx.h"
#include "BarManager.h"

#include "PatternPropertyBar.h"
#include "LoopToolEditPanel.h"
#include "CornerPropertyBar.h"
#include "LoopLeadPropertyBar.h"
#include "MicroJointPropertyBar.h"
#include "PartPlacementPropertyBar.h"
#include "PartPlacementViewBar.h"
#include "PartPlacementAddPanel.h"
#include "PartTopologyBar.h"
#include "CutSequencePane.h"
#include "CutNodeViewBar.h"
#include "NCViewBar.h"
#include "InteractBar.h"
#include "CommonFuncPane.h"

BEGIN_CUTLEADER_NAMESPACE()

BarManager::BarManager(void)
{
	m_pPatPropBar = NULL;
    m_pLoopToolEditPane = NULL;
    m_pLoopLeadPropBar = NULL;
	m_pMicroJointPropBar = NULL;
	m_pCornerPropBar = NULL;
	m_pAddPrtPmtPane = NULL;
	m_pPartPmtPropBar = NULL;
	m_pPartPmtViewBar = NULL;
	m_pPartTopBar = NULL;
	m_pSeqUnitPane = NULL;
	m_pCutNodeViewBar = NULL;
	m_pNCViewBar = NULL;
	m_pInteractBar = NULL;
	m_pCommonFuncPane = NULL;
}

BarManager::~BarManager(void)
{
	if (m_pPatPropBar)
	{
		delete m_pPatPropBar;
		m_pPatPropBar = NULL;
	}

    if (m_pLoopToolEditPane)
    {
        delete m_pLoopToolEditPane;
        m_pLoopToolEditPane = NULL;
    }

    if (m_pLoopLeadPropBar)
    {
        delete m_pLoopLeadPropBar;
        m_pLoopLeadPropBar = NULL;
    }

	if (m_pMicroJointPropBar)
	{
		delete m_pMicroJointPropBar;
		m_pMicroJointPropBar = NULL;
	}

	if (m_pCornerPropBar)
	{
		delete m_pCornerPropBar;
		m_pCornerPropBar = NULL;
	}

	if (m_pAddPrtPmtPane)
	{
		delete m_pAddPrtPmtPane;
		m_pAddPrtPmtPane = NULL;
	}

	if (m_pPartPmtPropBar)
	{
		delete m_pPartPmtPropBar;
		m_pPartPmtPropBar = NULL;
	}

	if (m_pPartTopBar)
	{
		delete m_pPartTopBar;
		m_pPartTopBar = NULL;
	}

	if (m_pPartPmtViewBar)
	{
		delete m_pPartPmtViewBar;
		m_pPartPmtViewBar = NULL;
	}

	if (m_pSeqUnitPane)
	{
		delete m_pSeqUnitPane;
		m_pSeqUnitPane = NULL;
	}

	if (m_pCutNodeViewBar)
	{
		delete m_pCutNodeViewBar;
		m_pCutNodeViewBar = NULL;
	}

	if (m_pNCViewBar)
	{
		delete m_pNCViewBar;
		m_pNCViewBar = NULL;
	}

	if (m_pInteractBar)
	{
		delete m_pInteractBar;
		m_pInteractBar = NULL;
	}

	if (m_pCommonFuncPane)
	{
		delete m_pCommonFuncPane;
		m_pCommonFuncPane = NULL;
	}
}

void BarManager::Init(CDockingManager* pDockingMgr, CWnd* pParent)
{ 
	m_pDockingMgr = pDockingMgr;
	m_pParent = pParent;

	// "geometry" module
	CreatePatternPropBar();

	// "cam feature" module
	CreateMicroJointPropBar();
	m_pMicroJointPropBar->AttachToTabWnd(m_pPatPropBar, DM_STANDARD, FALSE, NULL);
	CreateCornerPropBar();
	m_pCornerPropBar->AttachToTabWnd(m_pMicroJointPropBar, DM_STANDARD, FALSE, NULL);
	CreateLoopLeadPropBar();
	m_pLoopLeadPropBar->AttachToTabWnd(m_pCornerPropBar, DM_STANDARD, FALSE, NULL);
    CreateLoopToolEditPane();
    m_pLoopToolEditPane->AttachToTabWnd(m_pLoopLeadPropBar, DM_STANDARD, FALSE, NULL);

	// "part layout" module.
	CreatePartPmtPropBar();
	m_pPartPmtPropBar->AttachToTabWnd(m_pCornerPropBar, DM_STANDARD, FALSE, NULL);
	CreateAddPrtInstPane();
	m_pAddPrtPmtPane->AttachToTabWnd(m_pPartPmtPropBar, DM_STANDARD, FALSE, NULL);
	CreatePartPmtViewBar();
	m_pPartPmtViewBar->AttachToTabWnd(m_pAddPrtPmtPane, DM_STANDARD, FALSE, NULL);
	CreatePartTopBar();
	m_pPartTopBar->AttachToTabWnd(m_pPartPmtViewBar, DM_STANDARD, FALSE, NULL);

	// "cutSeq" module.
	CreateSeqUnitPane();
	m_pSeqUnitPane->AttachToTabWnd(m_pPartPmtViewBar, DM_STANDARD, FALSE, NULL);
	CreateCutNodeViewBar();
	m_pCutNodeViewBar->AttachToTabWnd(m_pSeqUnitPane, DM_STANDARD, FALSE, NULL);

	// "postProcessor" module.
	CreateNCViewBar();
	m_pNCViewBar->AttachToTabWnd(m_pCutNodeViewBar, DM_STANDARD, FALSE, NULL);

	// 常用功能面板。
	CreateCommonFuncPane();

	// create bars on the bottom side.
	CreateOutputBar();
}

void BarManager::HideAllBar()
{
	// geometry category.
	m_pPatPropBar->ShowBar(FALSE);

	// cam feature category.
    m_pLoopToolEditPane->ShowBar(FALSE);
	m_pLoopLeadPropBar->ShowBar(FALSE);
	m_pCornerPropBar->ShowBar(FALSE);
	m_pMicroJointPropBar->ShowBar(FALSE);

	// part pmt category.
	m_pPartPmtViewBar->ShowBar(FALSE);
	m_pAddPrtPmtPane->ShowBar(FALSE);
	m_pPartPmtPropBar->ShowBar(FALSE);
	m_pPartTopBar->ShowBar(FALSE);

	// sequence category.
	m_pSeqUnitPane->ShowBar(FALSE);
	m_pCutNodeViewBar->ShowBar(FALSE);
	m_pNCViewBar->ShowBar(FALSE);

	// output bars.
	m_pInteractBar->ShowBar(FALSE);

	m_pCommonFuncPane->ShowBar(FALSE);
}

void BarManager::HideAllRightBar()
{
	// geometry category.
	m_pPatPropBar->ShowBar(FALSE);

	// cam feature category.
    m_pLoopToolEditPane->ShowBar(FALSE);
	m_pLoopLeadPropBar->ShowBar(FALSE);
	m_pCornerPropBar->ShowBar(FALSE);
	m_pMicroJointPropBar->ShowBar(FALSE);

	// part pmt category.
	m_pPartPmtViewBar->ShowBar(FALSE);
	m_pAddPrtPmtPane->ShowBar(FALSE);
	m_pPartPmtPropBar->ShowBar(FALSE);
	m_pPartTopBar->ShowBar(FALSE);

	// sequence category.
	m_pSeqUnitPane->ShowBar(FALSE);
	m_pCutNodeViewBar->ShowBar(FALSE);
	m_pNCViewBar->ShowBar(FALSE);
}

BOOL BarManager::CreatePatternPropBar()
{
	m_pPatPropBar = PatternPropertyBar::GetInstance(m_pParent);
	if (!m_pPatPropBar)
		return FALSE;

	m_pPatPropBar->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pPatPropBar);

	return TRUE;
}

BOOL BarManager::CreateLoopToolEditPane()
{
    m_pLoopToolEditPane = LoopToolEditPanel::GetInstance(m_pParent);
    if (!m_pLoopToolEditPane)
        return FALSE;

    m_pLoopToolEditPane->EnableDocking(CBRS_ALIGN_RIGHT);
    m_pDockingMgr->DockPane(m_pLoopToolEditPane);

    return TRUE;
}

BOOL BarManager::CreateLoopLeadPropBar()
{
	m_pLoopLeadPropBar = LoopLeadPropertyBar::GetInstance(m_pParent);
	if (!m_pLoopLeadPropBar)
		return FALSE;

	m_pLoopLeadPropBar->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pLoopLeadPropBar);

	return TRUE;
}

BOOL BarManager::CreateMicroJointPropBar()
{
	m_pMicroJointPropBar = MicroJointPropertyBar::GetInstance(m_pParent);
	if (!m_pMicroJointPropBar)
		return FALSE;

	m_pMicroJointPropBar->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pMicroJointPropBar);

	return TRUE;
}

BOOL BarManager::CreatePartPmtViewBar()
{
	m_pPartPmtViewBar = PartPlacementViewBar::GetInstance(m_pParent);
	if (!m_pPartPmtViewBar)
		return FALSE;

	m_pPartPmtViewBar->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pPartPmtViewBar);

	return TRUE;
}

BOOL BarManager::CreatePartTopBar()
{
	m_pPartTopBar = PartTopologyBar::GetInstance(m_pParent);
	if (!m_pPartTopBar)
		return FALSE;

	m_pPartTopBar->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pPartTopBar);

	return TRUE;
}

BOOL BarManager::CreateSeqUnitPane()
{
	m_pSeqUnitPane = CutSequencePane::GetInstance(m_pParent);
	if (!m_pSeqUnitPane)
		return FALSE;

	m_pSeqUnitPane->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pSeqUnitPane);

	return TRUE;
}

BOOL BarManager::CreateCutNodeViewBar()
{
	m_pCutNodeViewBar = CutNodeViewBar::GetInstance(m_pParent);
	if (!m_pCutNodeViewBar)
		return FALSE;

	m_pCutNodeViewBar->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pCutNodeViewBar);

	return TRUE;
}

BOOL BarManager::CreateNCViewBar()
{
	m_pNCViewBar = NCViewBar::GetInstance(m_pParent);
	if (!m_pNCViewBar)
		return FALSE;

	m_pNCViewBar->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pNCViewBar);

	return TRUE;
}

BOOL BarManager::CreateOutputBar()
{
	m_pInteractBar = InteractBar::GetInstance(m_pParent);
	if (!m_pInteractBar)
		return FALSE;

	m_pInteractBar->EnableDocking(CBRS_ALIGN_ANY);
	m_pDockingMgr->DockPane(m_pInteractBar);

	return TRUE;
}

BOOL BarManager::CreateCommonFuncPane()
{
	m_pCommonFuncPane = CommonFuncPane::GetInstance(m_pParent);
	if (!m_pCommonFuncPane)
		return FALSE;

	m_pAddPrtPmtPane->EnableDocking(CBRS_ALIGN_LEFT);
	m_pDockingMgr->DockPane(m_pCommonFuncPane);

	return TRUE;
}

BOOL BarManager::CreateAddPrtInstPane()
{
	m_pAddPrtPmtPane = PartPlacementAddPanel::GetInstance(m_pParent);
	if (!m_pAddPrtPmtPane)
		return FALSE;

	m_pAddPrtPmtPane->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pAddPrtPmtPane);

	return TRUE;
}

BOOL BarManager::CreatePartPmtPropBar()
{
	m_pPartPmtPropBar = PartPlacementPropertyBar::GetInstance(m_pParent);
	if (!m_pPartPmtPropBar)
		return FALSE;

	m_pPartPmtPropBar->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pPartPmtPropBar);

	return TRUE;
}

BOOL BarManager::CreateCornerPropBar()
{
	m_pCornerPropBar = CornerPropertyBar::GetInstance(m_pParent);
	if (!m_pCornerPropBar)
		return FALSE;

	m_pCornerPropBar->EnableDocking(CBRS_ALIGN_RIGHT);
	m_pDockingMgr->DockPane(m_pCornerPropBar);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
