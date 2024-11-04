#include "StdAfx.h"
#include "StatusBar.h"

BEGIN_CUTLEADER_NAMESPACE()

StatusBar::StatusBar(void)
{
}

StatusBar::~StatusBar(void)
{
}

void StatusBar::Init(CWnd* pParentWnd)
{
	m_statusBar.Create(pParentWnd);
	m_statusBar.AddElement(new CMFCRibbonStatusBarPane(ID_FIRST_PANE, _T(""), TRUE), _T(""));
	m_statusBar.AddSeparator();
	m_statusBar.AddSeparator();
	m_statusBar.AddElement(new CMFCRibbonStatusBarPane(ID_SECOND_PANE, _T(""), TRUE), _T(""));;
}

void StatusBar::UpdateStatusBar(LPCTSTR str, int iID)
{
	CMFCRibbonBaseElement* pPane = m_statusBar.FindByID(iID);
	if (pPane)
		pPane->SetText(str);

	m_statusBar.Invalidate();
	m_statusBar.UpdateWindow();
	m_statusBar.RecalcLayout();
}

void StatusBar::Show()
{
	if (!m_statusBar.IsVisible())
		m_statusBar.ShowPane(TRUE, FALSE, TRUE);
}

void StatusBar::Hide()
{
	if (m_statusBar.IsVisible())
		m_statusBar.ShowPane(FALSE, FALSE, FALSE);
}

END_CUTLEADER_NAMESPACE()
