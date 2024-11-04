#pragma once

#include "clBaseDataUICommon.h"
#include "XTreeCtrl.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ProductItemList)

BEGIN_CUTLEADER_NAMESPACE()

// the tree display the product tree.
class ClBaseDataUI_Export ProductTree : public XTreeCtrl
{
	DECLARE_DYNAMIC(ProductTree)

public:
	ProductTree(void);
	~ProductTree(void);

public: // implement functions from the basic class.
	virtual BOOL CanBeFolderNode(HTREEITEM hItem);

private:
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

public:
	void Init();

	// init the product tree.
	void InitTree(FolderNodePtr pWorkspace);

	// recursive to append "pProductItemList" under "hParentNode".
	// notes:
	//   1) return the appended tree items.
	void AppendSubNode(ProductItemListPtr pProductItemList, HTREEITEM hParentNode, std::vector<HTREEITEM>& newTreeItemList);

	// call this function to navigate to a node.
	// notes:
	//   1) if "m_iNaviNodeID" is INVALID_ID, navigate to the root node.
	void NavigateToNode(LONGLONG iNaviNodeID);

	// get the item data of selected tree nodes.
	/*         
			 folder1                       
			   ******** leaf1              
				   *                       
				   *                       
				   **** leaf2              
				   *                       
				   * folder2               
				   ************  leaf3     
						  *                
						  *                
						  *                
						  *****  leaf4     
						  *                
						  *                
						  *****  leaf5
	*/
	// notes:
	//   1) reference to the illustration, if we selected "folder2,leaf4,leaf5",
	//      we only keep "folder2" tree node because it's the parent node.
	//   2) we should not find parent/child relationship in the returned list.
	IntegerListPtr GetSelTreeItemData();

	// delete the node and release the node data.
	BOOL DeleteItem(HTREEITEM hItem);

private:
	// this is a recursive function.
	// if we find the node, we will return TRUE.
	BOOL LocateNode(HTREEITEM hItem, LONGLONG iNaviNodeID);

	// release the data which stored in the tree node.
	// notes:
	//   1) if other function will call this, provide the first root node.
	//   2) the recursive order:
	/*
					 **        **     
				  1 *  ****** *  * 5  
					 **   *    **     
						  *                   
						  *    **     
						  *** *  * 6  
						  *    **     
						  *                    
						  *           
						  *           
					 **   *    **     
				  2 *  ****** *  * 3  
					 **   *    **     
						  *                  
						  *    **     
						  *****  * 4  
							   **     
	*/
	void ReleaseAllNodeData(HTREEITEM hParentItem);

	// release the data which stored in "hParentItem" and its children.
	void ReleaseNodeData(HTREEITEM hParentItem);

private:
	FolderNodePtr m_pWorkspace;

	// the image list.
	CImageList* m_pImageList;
};

END_CUTLEADER_NAMESPACE()
