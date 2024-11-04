#include "StdAfx.h"
#include "MicroJointPropertyBar.h"

#include <set>
#include "clUtilityResource.h"
#include "DataHelper.h"
#include "NumberProp.h"
#include "CheckBoxProp.h"
#include "Point2DList.h"
#include "baseConst.h"
#include "CommandManager.h"
#include "clUILibResource.h"
#include "clBaseDataResource.h"

#include "PatternPosition.h"
#include "IPattern.h"
#include "IPatternLoop.h"
#include "clGeometryFeatureResource.h"

#include "ConditionType.h"
#include "ConditionTypeList.h"
#include "ConditionNodeList.h"
#include "ConditionNode.h"

#include "MicroJoint.h"
#include "MicroJointList.h"
#include "MicroJointPtModifyCommand.h"
#include "MicroJointLeadModifyCommand.h"
#include "MicroJointWidthModifyCommand.h"
#include "MicroJointLeadLengthModifyCommand.h"
#include "MicroJointLeadAngleModifyCommand.h"
#include "LoopMicroJoint.h"
#include "LoopMicroJointList.h"
#include "LoopCutFeatureList.h"
#include "clCutFeatureResource.h"
#include "MicroJointMgr.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

MicroJointPropertyBar* MicroJointPropertyBar::m_pMicroJointPropBar = NULL;

MicroJointPropertyBar::MicroJointPropertyBar(void)
{
	m_pLeadInCondGroup = NULL;
	m_pLeadOutCondGroup = NULL;
	m_bChangingMJPt = FALSE;
	m_bIgnore_OnPropertyChangeMsg = false;
}

MicroJointPropertyBar::~MicroJointPropertyBar(void)
{
}

MicroJointPropertyBar* MicroJointPropertyBar::GetInstance(CWnd* pParent)
{
	if (m_pMicroJointPropBar == NULL)
	{
		CString str;
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROPBAR_MicroJointPROP);

		m_pMicroJointPropBar = new MicroJointPropertyBar();
		m_pMicroJointPropBar->m_pMainWnd = pParent;
		if (!m_pMicroJointPropBar->Create(str, (CWnd*)pParent, CRect(0, 0, RIGHT_DOCK_PANE_WIDTH, 200), TRUE, IDC_PROPBAR_MicroJointPROP,
								   WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 
								   CBRS_RIGHT | CBRS_FLOAT_MULTI))
		{
			return NULL;
		}
	}

	return m_pMicroJointPropBar;
}

BEGIN_MESSAGE_MAP(MicroJointPropertyBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChange)

	ON_BN_CLICKED(IDC_SET_MJ_POINT, OnChangeMJPt)
	ON_UPDATE_COMMAND_UI(IDC_SET_MJ_POINT, OnUpdateClickBtn)
END_MESSAGE_MAP()

int MicroJointPropertyBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// create the list.
	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
		return -1;

	// create the button.
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SET_MJ_POINT);
	m_setMicroJointPtBtn.Create(str, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, rectDummy, this, IDC_SET_MJ_POINT);

	AdjustLayout();

	return 0;
}

void MicroJointPropertyBar::OnDestroy()
{
	if (m_pLeadInCondGroup && m_pLeadOutCondGroup)
		ReleaseData();

	CutFeaturePropertyBar::OnDestroy();
}

void MicroJointPropertyBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void MicroJointPropertyBar::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

LRESULT MicroJointPropertyBar::OnPropertyChange(WPARAM wParam, LPARAM lParam)
{
	if (m_bIgnore_OnPropertyChangeMsg)
	{
		return S_OK;
	}

	CMFCPropertyGridProperty* prop = (CMFCPropertyGridProperty *)lParam;
	COleVariant var = prop->GetValue();
	int iPropItemName = m_propItemInfo[(CObject*)prop];

	// �޸�΢���ӿ�ȡ�
	if (iPropItemName == IDS_PROP_MicroJointWIDTH)
	{
		// �޸�΢���ӿ��ʱ������ܲ����޸ġ�
		if (!MicroJointMgr::CanChangeMJWidth(m_pLoopMicroJointList, m_pMicroJointList, var.dblVal))
		{
			CString str, strProductName;
			strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE_GEOM_NOT_HOLD_MicroJoint);
			::MessageBox(m_pMainWnd->m_hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
			return S_OK;
		}

		ICommandPtr pCommand(new MicroJointWidthModifyCommand(GetEditorData(), m_pMicroJointList, var.dblVal));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}

	// ����/����΢��������/�����ߡ�
	CString strEanble, strDisable;
	strEanble = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ENABLE);
	strDisable = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DISABLE);
	if (iPropItemName == IDS_PROP_LEADIN_ENABLE)
	{
		ICommandPtr pCommand;
		if (var.bstrVal == strEanble)
			pCommand.reset(new MicroJointLeadModifyCommand(GetEditorData(), m_pMicroJointList, TRUE, TRUE));
		else if (var.bstrVal == strDisable)
			pCommand.reset(new MicroJointLeadModifyCommand(GetEditorData(), m_pMicroJointList, TRUE, FALSE));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}
	else if (iPropItemName == IDS_PROP_LEADOUT_ENABLE)
	{
		ICommandPtr pCommand;
		if (var.bstrVal == strEanble)
			pCommand.reset(new MicroJointLeadModifyCommand(GetEditorData(), m_pMicroJointList, FALSE, TRUE));
		else if (var.bstrVal == strDisable)
			pCommand.reset(new MicroJointLeadModifyCommand(GetEditorData(), m_pMicroJointList, FALSE, FALSE));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}

	// �޸ġ�΢��������/�����߳��ȡ���
	if (iPropItemName == IDS_PROP_LEADIN_LENGTH)
	{
		ICommandPtr pCommand(new MicroJointLeadLengthModifyCommand(GetEditorData(), m_pMicroJointList, TRUE, var.dblVal));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}
	else if (iPropItemName == IDS_PROP_LEADOUT_LENGTH)
	{
		ICommandPtr pCommand(new MicroJointLeadLengthModifyCommand(GetEditorData(), m_pMicroJointList, FALSE, var.dblVal));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}

	// �޸ġ�΢��������/�����߽Ƕȡ���
	if (iPropItemName == IDS_PROP_LEADIN_ANGLE)
	{
		ICommandPtr pCommand(new MicroJointLeadAngleModifyCommand(GetEditorData(), m_pMicroJointList, TRUE, var.dblVal));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}
	else if (iPropItemName == IDS_PROP_LEADOUT_ANGLE)
	{
		ICommandPtr pCommand(new MicroJointLeadAngleModifyCommand(GetEditorData(), m_pMicroJointList, FALSE, var.dblVal));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}

	// ˢ�½��档
	GetRelativeView()->Invalidate();
	m_bIgnore_OnPropertyChangeMsg = true;
	DisplayMJData(GetEditorData(), m_pLoopMicroJointList, m_pMicroJointList, m_pMJPtList);
	m_bIgnore_OnPropertyChangeMsg = false;

	return S_OK;
}

void MicroJointPropertyBar::OnChangeMJPt()
{
	m_bChangingMJPt = TRUE;
}

void MicroJointPropertyBar::OnUpdateClickBtn(CCmdUI* pCmdUI)
{
	if (m_pMicroJointList->HasSamePosition())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void MicroJointPropertyBar::DisplayMJData(const EditorData& editorData, LoopMicroJointListPtr pLoopMicroJointList, MicroJointListPtr pMicroJointList, Point2DListPtr pMJPtList)
{
	m_editorData = editorData;
	m_pLoopMicroJointList = pLoopMicroJointList;
	m_pMicroJointList = pMicroJointList;
	m_pMJPtList = pMJPtList;

	// �ͷ����ݡ�
	if (m_pLeadInCondGroup && m_pLeadOutCondGroup)
		ReleaseData();

	// �������֡�
	m_wndPropList.RemoveAll();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	/************************************************************************/
	// ��ʾ�������顣

	// ��ʾ΢����λ�úͿ�ȡ�
	DisplayPosWidth();

	// ��ʾ�������������ݡ�
	DisplayLeadInData();
	DisplayLeadOutData();
	/************************************************************************/

	if (!IsVisible())
		ShowPane(TRUE, FALSE, TRUE);

	// now we do not want to change micro joint pt.
	m_bChangingMJPt = FALSE;
}

void MicroJointPropertyBar::DisplayPosWidth()
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CATEGORY_MJ_POS_WIDTH);
	CMFCPropertyGridProperty* pMicroJointDataGroup = new CMFCPropertyGridProperty(str);

	MicroJointPtr pMJ = m_pMicroJointList->at(0);
	Point2D microjointPt = m_pMJPtList->at(0);

	// ��ʾ΢���ӵ�λ�á�
	BOOL bHightlight = FALSE;
	if (m_pMicroJointList->size() > 1)
		bHightlight = TRUE;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_LOCATION_X);
	AddDoublePropItem(pMicroJointDataGroup, str, microjointPt.X(), DIGITAL_NUM, FALSE, bHightlight);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_LOCATION_Y);
	AddDoublePropItem(pMicroJointDataGroup, str, microjointPt.Y(), DIGITAL_NUM, FALSE, bHightlight);

	// ��ʾ΢���ӿ�ȡ�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_MicroJointWIDTH);
	XGridProperty* pWidthProp = AddDoublePropItem(pMicroJointDataGroup, str, pMJ->GetWidth(), DIGITAL_NUM);
	m_propItemInfo[(CObject*)pWidthProp] = IDS_PROP_MicroJointWIDTH;
	if (m_pMicroJointList->size() > 1 && !m_pMicroJointList->HasSameWidth())
		pWidthProp->HighlightText(TRUE);

	m_wndPropList.AddProperty(pMicroJointDataGroup);
}

void MicroJointPropertyBar::DisplayLeadInData()
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CATEGORY_MJ_LEADIN);
	CMFCPropertyGridProperty* pLeadInDataGroup = new CMFCPropertyGridProperty(str);

	MicroJointPtr pMJ = m_pMicroJointList->at(0);

	/************************************************************************/	
	// ��ʾ���Ƿ����������ߡ���

	// �½������
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_LEADIN_ENABLE);
	XGridProperty* pLeadInCtrlProp = AddComboPropItemA(pLeadInDataGroup, str, pMJ->EnableLeadIn());
	m_propItemInfo[(CObject*)pLeadInCtrlProp] = IDS_PROP_LEADIN_ENABLE;

	// Ҫ��Ҫͻ����ʾ��
	if (m_pMicroJointList->size() > 1 && !m_pMicroJointList->EanbleLead(LeadInOut_In))
		pLeadInCtrlProp->HighlightText(TRUE);
	/************************************************************************/

	/************************************************************************/
	// ��ʾ�������߳��ȡ���

	// �½������
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_LEADIN_LENGTH);
	XGridProperty* pLeadInLenProp = AddDoublePropItem(pLeadInDataGroup, str, pMJ->GetLeadInLen(), DIGITAL_NUM);
	m_propItemInfo[(CObject*)pLeadInLenProp] = IDS_PROP_LEADIN_LENGTH;

	// ����״̬��
	if (m_pMicroJointList->size() > 1 && !m_pMicroJointList->HasSameLeadLen(LeadInOut_In))
		pLeadInLenProp->HighlightText(TRUE);
	/************************************************************************/

	/************************************************************************/
	// ��ʾ�������߽Ƕȡ���

	// �½������
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_LEADIN_ANGLE);
	XGridProperty* pLeadInAngProp = AddDoublePropItem(pLeadInDataGroup, str, pMJ->GetLeadInAngle(), DIGITAL_NUM);
	m_propItemInfo[(CObject*)pLeadInAngProp] = IDS_PROP_LEADIN_ANGLE;

	// ����״̬��
	if (m_pMicroJointList->size() > 1 && !m_pMicroJointList->HasSameLeadAng(LeadInOut_In))
		pLeadInAngProp->HighlightText(TRUE);
	/************************************************************************/

	m_wndPropList.AddProperty(pLeadInDataGroup);
}

void MicroJointPropertyBar::DisplayLeadOutData()
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CATEGORY_MJ_LEADOUT);
	CMFCPropertyGridProperty* pLeadOutDataGroup = new CMFCPropertyGridProperty(str);

	MicroJointPtr pMJ = m_pMicroJointList->at(0);

	/************************************************************************/
	// ��ʾ���Ƿ����������ߡ���

	// �Ƿ����������ߡ�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_LEADOUT_ENABLE);
	XGridProperty* pLeadCtrlProp = AddComboPropItemA(pLeadOutDataGroup, str, pMJ->EnableLeadOut());
	m_propItemInfo[(CObject*)pLeadCtrlProp] = IDS_PROP_LEADOUT_ENABLE;

	// Ҫ��Ҫͻ����ʾ��
	if (m_pMicroJointList->size() > 1 && !m_pMicroJointList->EanbleLead(LeadInOut_Out))
		pLeadCtrlProp->HighlightText(TRUE);
	/************************************************************************/

	/************************************************************************/
	// ��ʾ�������߳��ȡ���

	// �½�������
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_LEADOUT_LENGTH);
	XGridProperty* pLeadOutLenProp = AddDoublePropItem(pLeadOutDataGroup, str, pMJ->GetLeadOutLen(), DIGITAL_NUM);
	m_propItemInfo[(CObject*)pLeadOutLenProp] = IDS_PROP_LEADOUT_LENGTH;

	// ����״̬��
	if (m_pMicroJointList->size() > 1 && !m_pMicroJointList->HasSameLeadLen(LeadInOut_Out))
		pLeadOutLenProp->HighlightText(TRUE);
	/************************************************************************/

	/************************************************************************/
	// ��ʾ�������߽Ƕȡ���

	// �½������
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_LEADOUT_ANGLE);
	XGridProperty* pLeadOutAngProp = AddDoublePropItem(pLeadOutDataGroup, str, pMJ->GetLeadOutAngle(), DIGITAL_NUM);
	m_propItemInfo[(CObject*)pLeadOutAngProp] = IDS_PROP_LEADOUT_ANGLE;

	// ����״̬��
	if (m_pMicroJointList->size() > 1 && !m_pMicroJointList->HasSameLeadAng(LeadInOut_Out))
		pLeadOutAngProp->HighlightText(TRUE);
	/************************************************************************/

	m_wndPropList.AddProperty(pLeadOutDataGroup);
}

void MicroJointPropertyBar::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
		return;
	if (!m_pMicroJointList)
		return;

	CRect rectClient;
	GetClientRect(rectClient);

	// the height of the two windows.
	int iHeight1 = rectClient.Height() * 19 / 20;
	int iHeight2 = rectClient.Height() * 1 / 20;
	int iDeduction = 0; // the space between two windows.

	m_setMicroJointPtBtn.ShowWindow(SW_SHOW);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), iHeight1-iDeduction, SWP_NOACTIVATE | SWP_NOZORDER);
	m_setMicroJointPtBtn.SetWindowPos(NULL, rectClient.left, iHeight1, rectClient.Width(), iHeight2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void MicroJointPropertyBar::ReleaseData()
{
    return; // �����ƣ�debug�汾���ͷ���Դ��ʱ���������δ�ҵ����⡣

	for (int i = 0; i < m_pLeadInCondGroup->GetSubItemsCount(); i++)
	{
		CMFCPropertyGridProperty* pProp = m_pLeadInCondGroup->GetSubItem(i);
		delete (LONGLONG*)pProp->GetData();
	}

	for (int i = 0; i < m_pLeadOutCondGroup->GetSubItemsCount(); i++)
	{
		CMFCPropertyGridProperty* pProp = m_pLeadOutCondGroup->GetSubItem(i);
		delete (LONGLONG*)pProp->GetData();
	}
}

END_CUTLEADER_NAMESPACE()
