#pragma once

#include "PatternEditActionBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// һЩ�����Ļ��࣬��Щ����֧�ֵ�ѡ�Ϳ�ѡͼ�Ρ�
class PatternPreSelectAction : public PatternEditActionBase
{
protected:
	typedef enum tagPreSelectState
	{
		// the pre-select mode is inactive, mean every input cannot go into this action.
		PRE_SELECT_INACTIVE			= -1,

		// we can begin to select.
		PRE_SELECT_ACTIVE			= 0,

		PRE_SELECT_FIRST_INPUT		= 1,
		PRE_SELECT_SECOND_INPUT		= 2,
	} PRE_SELECT_STATE;

public:
	PatternPreSelectAction(void);
	~PatternPreSelectAction(void);

public:  // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	BOOL DrawBeforeFinish();

private:
	// calculate which pattern are selected using "m_data".
	PatternListPtr CalcSelectPat();

protected:
	// ���������ͼ�Ρ�
	IPatternPtr m_pActivePat;

	// ѡ�е�ͼ�Ρ�
	PatternListPtr m_pSelectedPats;

	// ѡ��״̬��
	PRE_SELECT_STATE m_preSelectState;

	// ѡ�������ݡ�
	/*
		LeftTopPointX;
		LeftTopPointY;
		RightBottomPointX;
		RightBottomPointY;
	*/
	double m_data[4];
};

END_CUTLEADER_NAMESPACE()
