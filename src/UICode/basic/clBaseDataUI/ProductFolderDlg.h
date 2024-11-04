#pragma once

#include "clBaseDataUICommon.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ProductItemList)

BEGIN_CUTLEADER_NAMESPACE()

class IProductItem;
typedef boost::shared_ptr<IProductItem> ProductItemPtr;

// 显示产品树的目录结构。
class ClBaseDataUI_Export ProductFolderDlg : public CLDialog
{
	DECLARE_DYNAMIC(ProductFolderDlg)

public:
	ProductFolderDlg(CWnd* pParent = NULL);
	virtual ~ProductFolderDlg();

	// Dialog Data
	enum { IDD = IDD_clBaseDataUI_Folder };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnDestroy();
	afx_msg void OnCreateFolder();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	FolderNodePtr GetSelectFolder() { return m_pSelFolderNode; }

private:
	// init the product tree.
	void Init();

	// recursive to append "pProductItemList" under "hParentNode".
	void AppendSubNode(ProductItemListPtr pProductItemList, HTREEITEM hParentNode);

	// whether there are folders under this folder node.
	BOOL HasFolder(ProductItemPtr pFolderItem);

	// release item data in tree.
	void ReleaseNodeData(HTREEITEM hParentItem);

private: // for control.
	// the tree control.
	CTreeCtrl m_productTree;

	// the image list.
	CImageList* m_pImageList;

private: // for data
	FolderNodePtr m_pWorkspace;

	// the selected node.
	FolderNodePtr m_pSelFolderNode;
};

END_CUTLEADER_NAMESPACE()
