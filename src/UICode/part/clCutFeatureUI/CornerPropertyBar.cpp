#include "StdAfx.h"
#include "CornerPropertyBar.h"

#include "NumberProp.h"
#include "baseConst.h"
#include "CommandManager.h"
#include "clBaseDataResource.h"
#include "LogMgr.h"
#include "clCutFeatureResource.h"
#include "LoopCornerData.h"
#include "ICorner.h"
#include "CornerList.h"
#include "LoopCutFeatureList.h"
#include "CornerModifyCommand.h"
#include "CornerManager.h"

#include "CornerConfigItem.h"
#include "CornerRangeInfo.h"

BEGIN_CUTLEADER_NAMESPACE()

CornerPropertyBar* CornerPropertyBar::m_pCornerPropBar = NULL;

CornerPropertyBar::CornerPropertyBar(void)
{
	m_bIgnore_OnPropertyChangeMsg = false;
}

CornerPropertyBar::~CornerPropertyBar(void)
{
}

CornerPropertyBar* CornerPropertyBar::GetInstance(CWnd* pParent)
{
	if (m_pCornerPropBar == NULL)
	{
		CString str;
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROPBAR_CORNERPROP);

		m_pCornerPropBar = new CornerPropertyBar();
		m_pCornerPropBar->m_pMainWnd = pParent;
		if (!m_pCornerPropBar->Create(str, (CWnd*)pParent, CRect(0, 0, RIGHT_DOCK_PANE_WIDTH, 200), TRUE, IDC_PROPBAR_CORNERPROP,
									  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
        {
			return NULL;
        }
	}

	return m_pCornerPropBar;
}

BEGIN_MESSAGE_MAP(CornerPropertyBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChange)
END_MESSAGE_MAP()

void CornerPropertyBar::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
		return;

	CRect rectClient, rectCombo;
	GetClientRect(rectClient);

	rectCombo.SetRectEmpty();
	int cyCmb = rectCombo.Size().cy;
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), rectClient.Height() - cyCmb, SWP_NOACTIVATE | SWP_NOZORDER);
}

int CornerPropertyBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
		return -1;

	AdjustLayout();

	return 0;
}

void CornerPropertyBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CornerPropertyBar::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

LRESULT CornerPropertyBar::OnPropertyChange(WPARAM wParam, LPARAM lParam)
{
	if (m_bIgnore_OnPropertyChangeMsg)
	{
		return S_OK;
	}

	CMFCPropertyGridProperty* prop = (CMFCPropertyGridProperty *)lParam;
	COleVariant var = prop->GetValue();

	CString strType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_CORNER_TYPE);
	CString strValue = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_CORNER_VALUE);
	CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_bCreate) // �ڴ���ģʽ��
	{
		if (prop->GetName() == strType) // �޸��˽���������
        {
			m_iCornerType_4_create = CornerRangeInfo::GetCornerTypeByStr(var.bstrVal);

            // ˢ�½��档
            m_bIgnore_OnPropertyChangeMsg = true;
            DisplayCornerData_4_create(m_iCornerType_4_create, m_dCornerValue_4_create);
            m_bIgnore_OnPropertyChangeMsg = false;
        }
        else if (prop->GetName() == strValue)
        {
            m_dCornerValue_4_create = var.dblVal;
        }
	}
	else // �ڱ༭ģʽ��
	{
		if (prop->GetName() == strType) // �޸��˽���������
		{
            // ���������ͺ�ֵ��
            CornerType iCornerType = CornerRangeInfo::GetCornerTypeByStr(var.bstrVal);

			// ��ȡ������ֵ��
            double dCornerValue = -1;
            if (CornerManager::WhetherHaveCornerValue(iCornerType))
            {
                CMFCPropertyGridProperty* pGroup = m_wndPropList.GetProperty(0);
                if (pGroup->GetSubItemsCount() == 1) // �޸�ǰû�С�ֵ�����
                {
                    dCornerValue = m_pCornerConfigItem->GetDflCornerVal();
                }
                else
                {
                    COleVariant vVal = pGroup->GetSubItem(1)->GetValue();
                    dCornerValue = vVal.dblVal;
                }
            }

			// �����µĽ���������
			CornerListPtr pNewCornerList(new CornerList);
			for (unsigned int i = 0; i < m_pCornerList->size(); i++)
			{
				const ICorner* pOldCorner = m_pCornerList->at(i).get();
				ICornerPtr pNewCorner = CornerManager::BuildCorner(pOldCorner->GetParentID(), iCornerType, dCornerValue,
																  pOldCorner->GetFirstPatternID(), pOldCorner->GetSecondPatternID());
				pNewCornerList->push_back(pNewCorner);
			}

			// �����Щ�������ġ�������ֵ���ܲ��ܱ����ܡ�
			if (CornerManager::WhetherHaveCornerValue(iCornerType) && !CornerManager::CheckCornerValue(m_pLoopCornerList.get(), pNewCornerList.get()))
			{
				CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE_CORNER_WRONG_VAL);
				::MessageBox(m_pMainWnd->m_hWnd, str, strProductName, MB_OK | MB_ICONWARNING);

                // Ҫ���л�����ǰû�С�ֵ����򡱣�������Ҫ���������б�ؼ��Լ��ϡ�ֵ����򡱡�
                if (m_wndPropList.GetProperty(0)->GetSubItemsCount() == 1)
                {
                    m_bIgnore_OnPropertyChangeMsg = true;

                    // ���������б�ؼ�
                    {
                        m_wndPropList.RemoveAll();
                        CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE_CORNERINFO));
                        AddTypeControl(pGroup, iCornerType);
                        _variant_t v(m_pCornerConfigItem->GetDflCornerVal(), VT_R8);
                        NumberProp* pValProp = new NumberProp(MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_CORNER_VALUE), v, DIGITAL_NUM, NULL, 0, FALSE);
                        pGroup->AddSubItem(pValProp);
                        m_wndPropList.AddProperty(pGroup);
                    }

                    m_bIgnore_OnPropertyChangeMsg = false;
                }

				return S_OK;
			}

			// �޸Ľ�������
			ICommandPtr pCommand(new CornerModifyCommand(GetEditorData(), m_pLoopCornerList, m_pCornerList, pNewCornerList));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// ���µ�ǰ�༭�Ľ�������
			m_pCornerList = pNewCornerList;
		}
		else if (prop->GetName() == strValue) // �޸��˽�����ֵ
		{
            // ��ʱ������
            CMFCPropertyGridProperty* pGroup = m_wndPropList.GetProperty(0);
            COleVariant typeVal = pGroup->GetSubItem(0)->GetValue();
            CornerType iCornerType = CornerRangeInfo::GetCornerTypeByStr(typeVal.bstrVal);

			// �����µĽ���������
			CornerListPtr pNewCornerList(new CornerList);
			for (unsigned int i = 0; i < m_pCornerList->size(); i++)
			{
				const ICorner* pOldCorner = m_pCornerList->at(i).get();
                ICornerPtr pNewCorner;
                if (pOldCorner->HasCornerValue())
                {
                    pNewCorner = CornerManager::BuildCorner_byChangeCornerValue(pOldCorner, var.dblVal);
                }
				else // ��ʱ��ʵ����Ҳ�����ˡ�
                {
                    pNewCorner = CornerManager::BuildCorner(pOldCorner->GetParentID(), iCornerType, var.dblVal,
                        pOldCorner->GetFirstPatternID(), pOldCorner->GetSecondPatternID());
                }
				pNewCornerList->push_back(pNewCorner);
			}

			// �����Щ�������ġ�������ֵ���ܲ��ܱ����ܡ�
			if (!CornerManager::CheckCornerValue(m_pLoopCornerList.get(), pNewCornerList.get()))
			{
				CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE_CORNER_WRONG_VAL);
				::MessageBox(m_pMainWnd->m_hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
				return S_OK;
			}

			// �޸Ľ�������
			ICommandPtr pCommand(new CornerModifyCommand(GetEditorData(), m_pLoopCornerList, m_pCornerList, pNewCornerList));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// ���µ�ǰ�༭�Ľ�������
			m_pCornerList = pNewCornerList;
		}

		// ˢ�½��档
		GetRelativeView()->Invalidate();
		m_bIgnore_OnPropertyChangeMsg = true;
		DisplayCornerData_4_edit(m_editorData, m_pLoopCornerList, m_pCornerList, m_pCornerConfigItem);
		m_bIgnore_OnPropertyChangeMsg = false;
	}

	return S_OK;
}

XGridProperty* CornerPropertyBar::AddTypeControl(CMFCPropertyGridProperty* pGroup, CornerType iCurrentCornerType)
{
    StringListPtr pOptList = CornerRangeInfo::GetAllCornerType(FALSE);
    CString str1 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_CORNER_TYPE);;
    XGridProperty* pTypeProp = new XGridProperty(str1, CornerRangeInfo::GetNameByType(iCurrentCornerType), _T(""));
    for (unsigned int i = 0; i < pOptList->size(); i++)
    {
        pTypeProp->AddOption((*pOptList)[i]);
    }
    pTypeProp->AllowEdit(FALSE);
    pGroup->AddSubItem(pTypeProp);

    return pTypeProp;
}

void CornerPropertyBar::DisplayCornerData_4_create(CornerType iCornerType_4_create, double dCornerValue_4_create)
{
	// init value.
	m_bCreate = TRUE;
    m_iCornerType_4_create = iCornerType_4_create;
    m_dCornerValue_4_create = dCornerValue_4_create;

	m_wndPropList.RemoveAll();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CString strGroupName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE_CORNERINFO);
	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(strGroupName);

	// ��ӡ����͡������
    AddTypeControl(pGroup, iCornerType_4_create);

	// ��ӡ�ֵ�������
    if (CornerManager::WhetherHaveCornerValue(iCornerType_4_create))
    {
        _variant_t v(dCornerValue_4_create, VT_R8);
        NumberProp* pValProp = new NumberProp(MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_CORNER_VALUE), v, DIGITAL_NUM, NULL, 0, FALSE);
        pGroup->AddSubItem(pValProp);
    }

	m_wndPropList.AddProperty(pGroup);

	if (!IsVisible())
    {
		ShowPane(TRUE, FALSE, TRUE);
    }
}

void CornerPropertyBar::DisplayCornerData_4_edit(const EditorData& editorData, LoopCornerDataListPtr pLoopCornerList, CornerListPtr pCornerList,
                                                 CornerConfigItemPtr pCornerConfigItem)
{
	m_editorData = editorData;
	m_bCreate = FALSE;
	m_pLoopCornerList = pLoopCornerList;
	m_pCornerList = pCornerList;
    m_pCornerConfigItem = pCornerConfigItem;

	m_wndPropList.RemoveAll();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	// �������顣
	CString strGroupName;
	strGroupName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE_CORNERINFO);
	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(strGroupName);

	const ICorner* pCorner = m_pCornerList->at(0).get();

	// ��ӡ����͡������
    XGridProperty* pTypeProp = AddTypeControl(pGroup, pCorner->GetCornerType());
	if (m_pCornerList->size() > 1 && !m_pCornerList->SameCornerType())
	{
		pTypeProp->HighlightText(TRUE);
	}

    // ��ӡ�ֵ�������
    if (m_pCornerList->size() == 1) // ֻ��ʾһ������������Ϣ
    {
        if (pCorner->HasCornerValue()) // �����������ֵ��
        {
            _variant_t v(CornerManager::GetCornerValue(pCorner), VT_R8);
            NumberProp* pValProp = new NumberProp(MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_CORNER_VALUE), v, DIGITAL_NUM, NULL, 0, FALSE);
            pGroup->AddSubItem(pValProp);
        }
    }
    else // ��Ҫ��ʾ�������������Ϣ
    {
        if (m_pCornerList->SameCornerType()) // ������������ͬ��������
        {
            if (pCorner->HasCornerValue()) // ��Щ����������ֵ��
            {
                _variant_t v(CornerManager::GetCornerValue(pCorner), VT_R8);
                NumberProp* pValProp = new NumberProp(MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_CORNER_VALUE), v, DIGITAL_NUM, NULL, 0, FALSE);
                if (!CornerManager::SameCornerValue(m_pCornerList.get())) // ֵҪ�ǲ�ȫ��ͬ�������ֵ��
                {
                    pValProp->HighlightText(TRUE);
                }
                pGroup->AddSubItem(pValProp);
            }
        }
        else // ��������������ͬһ���ͣ�ֵ��λ�þ���ʾ��һ����������ֵ��Ҫ�ǵ�һ��������û��ֵ���ǾͲ���ֵ�����
        {
            if (pCorner->HasCornerValue()) // ��һ����������ֵ��
            {
                _variant_t v(CornerManager::GetCornerValue(pCorner), VT_R8);
                NumberProp* pValProp = new NumberProp(MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_CORNER_VALUE), v, DIGITAL_NUM, NULL, 0, FALSE);
                pValProp->Enable(FALSE);
                pGroup->AddSubItem(pValProp);
            }
        }
    }

	m_wndPropList.AddProperty(pGroup);

	if (!IsVisible())
    {
		ShowPane(TRUE, FALSE, TRUE);
    }
}

END_CUTLEADER_NAMESPACE()
