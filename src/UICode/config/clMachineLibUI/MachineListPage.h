#pragma once

#include "clMachineLibUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"
#include "DataProperty.h"
#include "CGridListCtrlGroups.h"
#include "CLResizableDialog.h"
#include <vector>

BEGIN_CUTLEADER_NAMESPACE()

// we can display all machine info in this page.
class ClMachineLibUI_Export MachineListPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(MachineListPage)

public:
	MachineListPage(void);
	~MachineListPage(void);

	enum { IDD = IDD_clMachineLibUI_MachineList };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

	DECLARE_MESSAGE_MAP()

public:
	void DisplayMacList(std::vector<DataProperty> propList);

private:
	// init the column header of the list ctrl.
	void InitGridStyle();

	// init the content of the list ctrl.
	void InitGridData();

private: // controls
	CGridListCtrlGroups m_macGrid;

private: // data member
	std::vector<DataProperty> m_propList;
};

END_CUTLEADER_NAMESPACE()