#pragma once

#include "clMaterialLibUICommon.h"
#include "DataProperty.h"
#include "CGridListCtrlGroups.h"
#include "CLResizableDialog.h"
#include "taoResource.h"
#include "NumEdit.h"

BEGIN_CUTLEADER_NAMESPACE()

// ������Ϣҳ�档
class ClMaterialLibUI_Export MaterialInfoPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(MaterialInfoPage)

public:
	MaterialInfoPage();
	~MaterialInfoPage();

	enum { IDD = IDD_clMaterialLibUI_MaterialPage };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected:
	afx_msg void OnKillName();
	afx_msg void OnKillComment();
	afx_msg void OnKillGravity();

	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:
	// display the info of the material.
	void DisplayMatType(DataProperty matProp, std::vector<DataProperty> matThickProps);

	BOOL IsModified() { return m_bModified; }
	DataProperty GetMatProp() { return m_matProp; }

private:
	// init the column header of the list ctrl.
	void InitGridStyle();

private: // controls
	CString m_strName;
	CNumEdit m_dGravity;
	CString m_strComment;
	CString m_strCreateDate;
	CString m_strModifyDate;

	// material name list.
	CGridListCtrlGroups m_ctrlMatNameList;

	CNumEdit m_dDummy;

private: // data member.
	// �����ϡ������ԡ�
	// ע��
	//  1) �����ϵ��޸Ļ�������������
	DataProperty m_matProp;

	// �Ƿ��޸��ˡ����ϡ���
	BOOL m_bModified;
};

END_CUTLEADER_NAMESPACE()
