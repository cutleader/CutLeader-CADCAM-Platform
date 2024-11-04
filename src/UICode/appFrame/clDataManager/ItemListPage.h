#pragma once

#include "clDataManagerCommon.h"
#include "DataProperty.h"
#include "taoresource.h"
#include "resizableDialog.h"
#include "CGridListCtrlGroups.h"

DECLARE_CUTLEADER_CLASS(ProductItemList)

BEGIN_CUTLEADER_NAMESPACE()

// this page will display the items.
class ClDataManager_Export ItemListPage : public CResizableDialog
{
	DECLARE_DYNCREATE(ItemListPage)

public:
	ItemListPage();
	~ItemListPage();

	enum { IDD = IDD_clDataManager_ItemList };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected: // message mapping.
	DECLARE_MESSAGE_MAP()

public:
	// display node list.
	void DisplayItemList(ProductItemListPtr pProductItemList);

private:
	// init the column header of the list ctrl.
	void InitGridStyle();

	// init the content of the list ctrl.
	void InitGridData();

private: // controls
	CGridListCtrlGroups m_productItemGrid;

private: // data member.
	ProductItemListPtr m_pProductItemList;
};

END_CUTLEADER_NAMESPACE()
