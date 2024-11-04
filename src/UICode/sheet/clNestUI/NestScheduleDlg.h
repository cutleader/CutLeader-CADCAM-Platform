#pragma once

#include "clNestUICommon.h"
#include "LineArc2DStaticControl.h"
#include "taoresource.h"
#include "CGridListCtrlGroups.h"
#include "NestPriority.h"
#include "CLDialog.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(IMaterialSize)
DECLARE_CUTLEADER_CLASS(PartItem)
DECLARE_CUTLEADER_CLASS(NestPartList)
DECLARE_CUTLEADER_CLASS(NestMaterialList)
DECLARE_CUTLEADER_CLASS(NestJob)

BEGIN_CUTLEADER_NAMESPACE()

class ClNestUI_Export NestScheduleDlg : public CLDialog
{
	DECLARE_DYNAMIC(NestScheduleDlg)

public:
	NestScheduleDlg(CWnd* pParent = NULL);
	virtual ~NestScheduleDlg();

	enum { IDD = IDD_clNestUI_NestSchedule };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelchangeExpLib();
	afx_msg void OnSelchangeParam();
	afx_msg void OnEditParam();

	afx_msg void OnAddPartFromDB();
	afx_msg void OnImpExtPart();
	afx_msg void OnAddStdPart();
	afx_msg void OnSetNestInfo();
	afx_msg void OnRemoveFromNestList();
	afx_msg void OnNestPartListchanged(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnAddMat();
	afx_msg void OnDeleteMat();
	afx_msg void OnNestMatListchanged(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnLoadNestJob();
	afx_msg void OnExecuteNest();

	DECLARE_MESSAGE_MAP()

public:
	void SetProductStruct(FolderNodePtr pWorkspace) { m_pWorkspace = pWorkspace; }

	// client will call this to get the nest job.
	NestJobPtr GetNestJob() const { return m_pNestJob; }

	FolderNodePtr GetSaveFolder(CString& strJobName) const;
	BOOL IsOpenSheet() const { return m_bOpenSheet; }

private:
	/************************************************************************/
	// ����������ء�

	// ��ʼ�����������á����顣
	void InitParamGroup();

	// �ͷš�ר�ҿ���Ͽ��е����ݡ�
	void RlsExpLibData();

	// �ͷš�����������Ͽ��е����ݡ�
	void RlsParamData();
	/************************************************************************/

	/************************************************************************/
	// �����ء�

	// ��ʼ���������񡱵���ʽ��
	void InitPartGridStyle();

	// Ԥ���б���ѡ�е������
	void PreviewNestPart();

	// �������ǲ����к͡��������á����׵Ĺ������ݡ�
	// ע��
	//   1) Ҫ��û�У�����һ���µĹ������ݡ�
	//   2) ���ع�������ID��
	LONGLONG CheckPart(LONGLONG iParamConfigID, LONGLONG iPartID);

	// �ͷš��������е����ݡ�
	void RlsPartGridData();

	// �Ƿ��������������
	BOOL PartAdded(LONGLONG iPartID);

	// ���б�õ��Ű������Ϣ��
	NestPartListPtr GetNestPartList();

	// �йر�׼�����
	void CreateStdPart();
	void ImportRectPart();

	// ���б������һ�С�
	void AddNestPartToGrid(const PartItem* pPartItem, const NestPriority& iNestPriority, int iCount, PART_ROT_STYLE iRotAng);
	/************************************************************************/

	/************************************************************************/
	// ������ء�

	// ��ʼ�������ϱ�񡱵���ʽ��
	void InitMatGridStyle();

	// Ԥ���б���ѡ�еİ�ġ�
	void PreviewNestMat();

	// �ͷš����ϱ���е����ݡ���
	void RlsMatGridData();

	// �Ƿ������������
	BOOL MatAdded(LONGLONG iMaterialSizeID);

	// ���б�õ��Ű������Ϣ��
	NestMaterialListPtr GetNestMatList();

	// ���Ĭ�ϵĲ��ϡ�
	void AddDflMat();

	// ���б������һ�С�
	void AddNestMat(IMaterialSizePtr pMaterialSize);
	/************************************************************************/

private: // for controls
	// ���������á����顣
	CComboBox m_comboExpLibItem;
	CComboBox m_comboParamConfig;

	// ���Ű���������顣
	CGridListCtrlGroups m_nestPartList;
	CStatic m_partPreviewWnd;
	CMFCMenuButton m_ctrlImpExtPart;
	CMenu m_btnMenu;
	CMFCButton m_ctrlAddFromDB;
	CMFCButton m_ctrlAddStdPart;
	CMenu m_btnStdPartMenu;
	CMFCButton m_ctrlNestInfo;

	// ���Ű���ϡ����顣
	CGridListCtrlGroups m_nestMatList;
	LineArc2DStaticControl m_matPreviewWnd;
	CMFCButton m_ctrlAddMat;

	// �������Ű���ҵ����ť��
	CMFCButton m_ctrlLoadNestJob;

	// �Ű水ť��
	CMFCButton m_ctrlExecuteNest;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data member
	// about selected scheme info.
	LONGLONG m_iExpLibItemID;
	LONGLONG m_iParamConfigID;

	/************************************************************************/
	// about some folder node.

	// the root node of the product structure.
	FolderNodePtr m_pWorkspace;

	// whether to save.
	FolderNodePtr m_pParentFolder;
	CString m_strName;
	/************************************************************************/

	// if we did a nest, we should generate such a job.
	NestJobPtr m_pNestJob;

	// �����Ű�������б��Ƿ��ʼ���ꡣ
	BOOL m_bFinishAddNestPart;

	BOOL m_bOpenSheet;
};

END_CUTLEADER_NAMESPACE()
