#pragma once

#include "clBaseDataUICommon.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ProductItemList)

BEGIN_CUTLEADER_NAMESPACE()

class IProductItem;
typedef boost::shared_ptr<IProductItem> ProductItemPtr;

// use this dialog to decide where to save the data.
class ClBaseDataUI_Export ProductSaveInfoDlg : public CLDialog
{
	DECLARE_DYNAMIC(ProductSaveInfoDlg)

public:
	ProductSaveInfoDlg(CWnd* pParent = NULL);
	virtual ~ProductSaveInfoDlg();

	// Dialog Data
	enum { IDD = IDD_clBaseDataUI_SaveData };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	void Init(CString strTitle, FolderNodePtr pWorkspace, CString strName);

	FolderNodePtr GetSelectFolder() { return m_pSelFolderNode; }
	CString GetName() { return m_strName; }

private:
	// init the product tree.
	void InitTree();

	// recursive to append "pProductItemList" under "hParentNode".
	void AppendSubNode(ProductItemListPtr pProductItemList, HTREEITEM hParentNode);

	// whether there are folders under this folder node.
	BOOL HasFolder(ProductItemPtr pFolderItem);

	// release item data in tree.
	void ReleaseNodeData(HTREEITEM hParentItem);

private: // for control.
	CString m_strName;

	// the tree control.
	CTreeCtrl m_productTree;

	// the image list.
	CImageList* m_pImageList;

private: // for data
	CString m_strTitle;
	FolderNodePtr m_pWorkspace;

	// the selected node.
	FolderNodePtr m_pSelFolderNode;
};

END_CUTLEADER_NAMESPACE()
