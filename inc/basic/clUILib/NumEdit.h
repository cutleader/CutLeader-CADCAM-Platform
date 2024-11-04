#ifndef __NUMEDIT_H__
#define __NUMEDIT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "clUILibCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// the edit box that only accept number value.
// notes: 
//   1) call Init(123, 0, FALSE) to make CNumEdit only accept integer.
//   2) if the number edit is the last focused control on the page, it maybe not updated correctly, so we can add a dummy number edit behind it.
class ClUILib_Export CNumEdit : public CEdit
{
public:
	CNumEdit();
	virtual ~CNumEdit();

public:
	void Init(double fNumber = 0.00, int dPrecision = 3, bool bNegValues = true);
	void Set_Data(double fNumber, int dPrecision, bool bNegValues);

public:	
	CString Get_Number_Text();

	double Get_Number();
	void Set_Number(double fNumber);

	// 从界面上把值读到numEdit中，类似cedit::UpdateData(true)。
	void UpdateNumEdit_fromUI();

protected:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus();
	afx_msg BOOL OnKillFocus();	

	DECLARE_MESSAGE_MAP()

protected:
	void Get_EditCtrl_Text();
	void Set_EditCtrl_Text(CString strText);
	bool Validate_Char(UINT nChar);
	double Text_To_Number(CString strText);
	CString Number_To_Text(double fNumber);

private:
	void Update();

protected:
	double m_fNumber;
	int	m_dPrecision;
	bool m_bNegValues;
	CString m_strText;

private:
	CToolTipCtrl* m_pToolTip;
};

#endif

END_CUTLEADER_NAMESPACE()
