#pragma once

#include "clNestUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(Part)

BEGIN_CUTLEADER_NAMESPACE()

// �½���׼���
class CreateStdPartDlg : public CLDialog
{
	DECLARE_DYNAMIC(CreateStdPartDlg)

public:
	CreateStdPartDlg(CWnd* pParent = NULL);
	virtual ~CreateStdPartDlg(void);

	enum { IDD = IDD_clNestUI_Create_StdPart };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnSelectFolder();

	afx_msg void OnRectPart();
	afx_msg void OnRoundPart();

	afx_msg void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	void SetConfigParamID(LONGLONG iParamConfigID) { m_iParamConfigID = iParamConfigID; }

	PartPtr GetNewPart() { return m_pNewPart; }
	FolderNodePtr GetSaveFolder() { return m_pSaveFolder; }
	int GetPartCount() { return m_iPartNum; }

private: // controls.
	// ������ơ�
	CString m_strPartName;

	// ����·��
	CString m_ctrlSavePath;

	// ���������
	CNumEdit m_ctrlPartNum;

	// ������״��
	int m_iPartShape;
	CNumEdit m_ctrlWidth;
	CNumEdit m_ctrlHeight;
	CNumEdit m_ctrlRad;

private: // ����
	// �������á�
	LONGLONG m_iParamConfigID;

	// ����Ŀ¼��
	FolderNodePtr m_pSaveFolder;

	// �������������
	PartPtr m_pNewPart;

	// ���������
	int m_iPartNum;
};

END_CUTLEADER_NAMESPACE()
