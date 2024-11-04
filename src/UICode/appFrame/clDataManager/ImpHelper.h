#pragma once

#include "clDataManagerCommon.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ProductItemList)

BEGIN_CUTLEADER_NAMESPACE()

// �������ݵİ����ࡣ
class ClDataManager_Export ImpHelper
{
public:
	// �������ݡ�
	/* ������
	*  pFolderNode: ���뵽�δ���
	*  bCanChangeFolder: �Ƿ���Ըı�Ŀ¼��
	*/
	// ע��
	//   1) �����������ѡ������ļ����Լ�������⡣
	//   2) ����ɹ�������Ҫ���ء���Ʒ����ϡ�
	static ProductItemListPtr ImportData(FolderNodePtr pFolderNode, BOOL bCanChangeFolder);

	// ��ֵ��������
	/* ������
	*  pFolderNode: ���뵽�δ���
	*  bCanChangeFolder: �Ƿ���Ըı�Ŀ¼��
	*/
	// ע��
	//   1) �����������ѡ������ļ����Լ�������⡣
	//   2) ����ļ��а�������������Ҫ��֡�
	//   3) ����ɹ�������Ҫ���ء���Ʒ����ϡ�
	static ProductItemListPtr SplitImportPart(FolderNodePtr pFolderNode, BOOL bCanChangeFolder);
};

END_CUTLEADER_NAMESPACE()
