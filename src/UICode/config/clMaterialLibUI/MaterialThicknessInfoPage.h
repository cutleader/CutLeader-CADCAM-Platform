#pragma once

#include "clMaterialLibUICommon.h"
#include "DataProperty.h"
#include "CGridListCtrlGroups.h"
#include "CLResizableDialog.h"
#include "taoResource.h"
#include "NumEdit.h"

BEGIN_CUTLEADER_NAMESPACE()

// �����Ϻ�ȡ�ҳ�档
class ClMaterialLibUI_Export MaterialThicknessInfoPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(MaterialThicknessInfoPage)

public:
	MaterialThicknessInfoPage();
	~MaterialThicknessInfoPage();

	enum { IDD = IDD_clMaterialLibUI_MaterialThickness_Page };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected:
	afx_msg void OnKillName();
	afx_msg void OnKillComment();
	afx_msg void OnKillThickness();

	afx_msg void OnDestroy();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:
	// ��ʾ�����Ϻ�ȡ���Ϣ��
	void DisplayMatThick(DataProperty matThickProp, std::vector<DataProperty> matSizeProps);

	BOOL IsModified() { return m_bModified; }
	DataProperty GetMatThickProp() { return m_matThickProp; }

private:
	// init the column header of the list ctrl.
	void InitGridStyle();

	// release the data which stored in grid.
	void ReleaseGridItemData();

private: // controls
	CString m_strName;
	CString m_strComment;
	CNumEdit m_dThickness;

	// material size list.
	CGridListCtrlGroups m_ctrlMatSizeList;

	CNumEdit m_dDummy;

private: // data member.
	// �����Ϻ�ȡ������ԡ�
	// ע��
	//  1) �����ϵ��޸Ļ�������������
	DataProperty m_matThickProp;

	// �Ƿ��޸��ˡ����Ϻ�ȡ���
	BOOL m_bModified;
};

END_CUTLEADER_NAMESPACE()
