#include "stdafx.h"
#include "cLeaderView.h"

#include "XMenu.h"
#include "IEditor.h"
#include "CommandList.h"
#include "clUILibResource.h"
#include "CommandManager.h"

#include "clFrameResource.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

#include "cLeaderDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(CLeaderView, CView)

BEGIN_MESSAGE_MAP(CLeaderView, CView)
	ON_WM_DESTROY()

	ON_COMMAND(IDC_TAO_UNDO, OnEditUndo)
	ON_COMMAND(IDC_TAO_REDO, OnEditRedo)
END_MESSAGE_MAP()


CLeaderView::CLeaderView()
{
	m_pViewPort.reset(new GlViewPort);
	m_bPopupShow = FALSE;
    m_pUndoCmdList.reset(new CommandList);
    m_pRedoCmdList.reset(new CommandList);
}

CLeaderView::~CLeaderView()
{
}

void CLeaderView::OnDraw(CDC*)
{
	CLeaderDoc* pDoc = GetDocument();
	if (!pDoc)
		return;

	// bind the OpenGL rendering context with device context.
	m_pViewPort->BindRendContext();

	// clear the current screen.
	m_pViewPort->ClearScreen();

	// set background color.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();
	m_pViewPort->SetBackgroundColor(*pColorInfo->GetBackgroundColor());

	// 绘制坐标轴。
	m_pViewPort->DrawCoordinate(0, 0, 0, FALSE);

	if (!m_pCurEditor)
		return;
}

void CLeaderView::OnDestroy()
{
	// 从“命令管理器”清除掉这个视图的undo/redo项。
	CommandListPtr pUndoCommands = CommandManager::GetInstance()->GetUndoCommands();
	CommandListPtr pTmpUndoCommands = pUndoCommands->GetCommandByView(this);
	pUndoCommands->RemoveCommand(pTmpUndoCommands);
	CommandListPtr pRedoCommands = CommandManager::GetInstance()->GetRedoCommands();
	CommandListPtr pTmpRedoCommands = pRedoCommands->GetCommandByView(this);
	pRedoCommands->RemoveCommand(pTmpRedoCommands);

	if (m_pViewPort)
		m_pViewPort->ReleaseResource();

	CView::OnDestroy();
}

void CLeaderView::OnEditUndo()
{
	CMFCRibbonBar* pRibbon = ((CMDIFrameWndEx*)GetTopLevelFrame())->GetRibbonBar();
	CMFCRibbonUndoButton* pUndoBtn = DYNAMIC_DOWNCAST(CMFCRibbonUndoButton, pRibbon->FindByID(IDC_TAO_UNDO));

	int iCount = pUndoBtn->GetActionNumber();
	if (iCount == -1)
	{
		UpdateUndoRedoItem(pUndoBtn);

		if (m_pUndoCmdList->size() > 0)
		{
			ICommandPtr pCommand = m_pUndoCmdList->at(m_pUndoCmdList->size()-1);
			CommandManager::GetInstance()->Undo(pCommand);
            ProcessAfterUndo(pCommand.get());
			m_pCurEditor->UpdateActionForUndoRedo();
		}
	}
	else
	{
		if (pUndoBtn != NULL && iCount > 0)
		{
			for (int i = 0; i < iCount; i++)
			{
				ICommandPtr pCommand = m_pUndoCmdList->at(m_pUndoCmdList->size()-i-1);
				CommandManager::GetInstance()->Undo(pCommand);
                ProcessAfterUndo(pCommand.get());
			}

			m_pCurEditor->UpdateActionForUndoRedo();
		}
	}

	// 刷新视图。
	Invalidate();
}

void CLeaderView::OnEditRedo()
{
	CMFCRibbonBar* pRibbon = ((CMDIFrameWndEx*)GetTopLevelFrame())->GetRibbonBar();
	CMFCRibbonUndoButton* pRedoBtn = DYNAMIC_DOWNCAST(CMFCRibbonUndoButton, pRibbon->FindByID(IDC_TAO_REDO));

	int iCount = pRedoBtn->GetActionNumber();
	if (iCount == -1)
	{
		UpdateUndoRedoItem(pRedoBtn);

		if (m_pRedoCmdList->size() > 0)
		{
			ICommandPtr pCommand = m_pRedoCmdList->at(m_pRedoCmdList->size()-1);
			CommandManager::GetInstance()->Redo(pCommand);

			// update the action after the undo/redo operation.
			m_pCurEditor->UpdateActionForUndoRedo();
		}
	}
	else
	{
		if (pRedoBtn != NULL && iCount > 0)
		{
			for (int i = 0; i < iCount; i++)
			{
				ICommandPtr pCommand = m_pRedoCmdList->at(m_pRedoCmdList->size()-i-1);
				CommandManager::GetInstance()->Redo(pCommand);
			}

			// update the action after the undo/redo operation.
			m_pCurEditor->UpdateActionForUndoRedo();
		}
	}

	// 刷新视图。
	Invalidate();
}

END_CUTLEADER_NAMESPACE()
