#include "StdAfx.h"
#include "MaterialLibTree.h"

#include "baseConst.h"

#include "taoresource.h"

#include "MaterialLibNodeList.h"
#include "MaterialLibNode.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(MaterialLibTree, XTreeCtrl)

MaterialLibTree::MaterialLibTree(void)
				: XTreeCtrl(IDC_CURSOR_TREE_COPYDROP, IDC_CURSOR_TREE_MOVEDROP, IDC_CURSOR_TREE_NODROP)
{
	// disable drag/drop.
	EnableDragDrop(FALSE);
}

MaterialLibTree::~MaterialLibTree(void)
{
	delete m_pImageList;
}

void MaterialLibTree::Init(IMaterialLibNodePtr pMatLibNode)
{
	// init the icons for the tree node.
	m_pImageList = new CImageList();
	m_pImageList->Create(IDB_MATLIB_TREE, 16, 10, RGB(192, 192, 192));
	SetImageList(m_pImageList, TVSIL_NORMAL);

	MaterialLibNodeListPtr pMatNodeList = pMatLibNode->GetSubNodeList();
	for (unsigned int i = 0; i < pMatNodeList->size(); i++)
	{
		IMaterialLibNodePtr pMatNode = pMatNodeList->at(i);

		/************************************************************************/
		// append the first layer(matType).

		// create tree node.
		TVINSERTSTRUCT matTypeNode;
		matTypeNode.hParent = NULL;
		matTypeNode.item.iImage = 0;
		matTypeNode.item.iSelectedImage = 0;
		matTypeNode.item.pszText = pMatNode->GetNodeName().AllocSysString();
		matTypeNode.item.cchTextMax = MAXLENGTH_STR;
		matTypeNode.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_CHILDREN;
		matTypeNode.item.cChildren = 1;
		matTypeNode.hInsertAfter = TVI_LAST;

		// set the node data.
		IMaterialLibNode* pTmpNode = pMatNode.get();
		MaterialLibNode* pMatLibNode = new MaterialLibNode(pTmpNode->GetNodeName(), pTmpNode->GetMatLibNodeType(), pTmpNode->GetDataID());
		matTypeNode.item.lParam = (LPARAM)pMatLibNode;

		// ������Ͻڵ㡣
		HTREEITEM hMatTypeNode = InsertItem(&matTypeNode);
		/************************************************************************/

		// ��ʼ�����Ͻڵ���ӽڵ㡣
		InitMatTypeNode(pMatNode, hMatTypeNode);
	}
}

HTREEITEM MaterialLibTree::AppendMatNode(IMaterialLibNodePtr pMatNode)
{
	HTREEITEM hMatNode;

	/************************************************************************/
	// ��Ӳ��Ͻڵ㡣

	// create tree node.
	TVINSERTSTRUCT matTypeNode;
	matTypeNode.hParent = NULL;
	matTypeNode.item.iImage = 0;
	matTypeNode.item.iSelectedImage = 0;
	matTypeNode.item.pszText = pMatNode->GetNodeName().AllocSysString();
	matTypeNode.item.cchTextMax = MAXLENGTH_STR;
	matTypeNode.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_CHILDREN;
	matTypeNode.hInsertAfter = TVI_LAST;

	// set the node data.
	IMaterialLibNode* pTmpNode = pMatNode.get();
	MaterialLibNode* pMatLibNode = new MaterialLibNode(pTmpNode->GetNodeName(), pTmpNode->GetMatLibNodeType(), pTmpNode->GetDataID());
	matTypeNode.item.lParam = (LPARAM)pMatLibNode;

	// ������Ͻڵ㡣
	hMatNode = InsertItem(&matTypeNode);
	/************************************************************************/

	// ��ʼ�����Ͻڵ���ӽڵ㡣
	InitMatTypeNode(pMatNode, hMatNode);

	return hMatNode;
}

void MaterialLibTree::InitMatTypeNode(IMaterialLibNodePtr pMatNode, HTREEITEM hMatTypeNode)
{
	MaterialLibNodeListPtr pMatThickNodeList = pMatNode->GetSubNodeList();
	if (!pMatThickNodeList)
		return;
	for (unsigned int i = 0; i < pMatThickNodeList->size(); i++)
	{
		IMaterialLibNodePtr pMatThickNode = pMatThickNodeList->at(i);

		// ���ϡ����Ϻ�ȡ��ڵ㡣
		TVINSERTSTRUCT matNameNode;
		matNameNode.hParent = hMatTypeNode;
		matNameNode.item.iImage = 1;
		matNameNode.item.iSelectedImage = 1;
		matNameNode.item.pszText = pMatThickNode->GetNodeName().AllocSysString();
		matNameNode.item.cchTextMax = MAXLENGTH_STR;
		matNameNode.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_CHILDREN;
		matNameNode.hInsertAfter = TVI_LAST;
		IMaterialLibNode* pTmpNode = pMatThickNode.get();
		MaterialLibNode* pMatLibNode = new MaterialLibNode(pTmpNode->GetNodeName(), pTmpNode->GetMatLibNodeType(), pTmpNode->GetDataID());
		matNameNode.item.lParam = (LPARAM)pMatLibNode;
		HTREEITEM hMatNameNode = InsertItem(&matNameNode);

		// ��ʼ�������Ϻ�ȡ��ڵ���ӽڵ㡣
		InitMatNameNode(pMatThickNode, hMatNameNode);
	}
}

void MaterialLibTree::InitMatNameNode(IMaterialLibNodePtr pMatThickNode, HTREEITEM hMatNameNode)
{
	MaterialLibNodeListPtr pMatSizeNodeList = pMatThickNode->GetSubNodeList();
	if (!pMatSizeNodeList)
		return;
	for (unsigned int i = 0; i < pMatSizeNodeList->size(); i++)
	{
		IMaterialLibNodePtr pMatSizeNode = pMatSizeNodeList->at(i);

		// ���ϡ����ϳߴ硱�ڵ㡣
		TVINSERTSTRUCT matSizeNode;
		matSizeNode.hParent = hMatNameNode;
		matSizeNode.item.iImage = 2;
		matSizeNode.item.iSelectedImage = 2;
		matSizeNode.item.pszText = pMatSizeNode->GetNodeName().AllocSysString();
		matSizeNode.item.cchTextMax = MAXLENGTH_STR;
		matSizeNode.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
		matSizeNode.hInsertAfter = TVI_LAST;
		IMaterialLibNode* pTmpNode = pMatSizeNode.get();
		MaterialLibNode* pMatLibNode = new MaterialLibNode(pTmpNode->GetNodeName(), pTmpNode->GetMatLibNodeType(), pTmpNode->GetDataID());
		matSizeNode.item.lParam = (LPARAM)pMatLibNode;
		HTREEITEM hMatSizeNode = InsertItem(&matSizeNode);
	}
}

END_CUTLEADER_NAMESPACE()
