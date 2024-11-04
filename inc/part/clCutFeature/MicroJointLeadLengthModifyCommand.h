#pragma once

#include "clCutFeatureCommon.h"
#include "CommandBase.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(MicroJointList)

BEGIN_CUTLEADER_NAMESPACE()

// �޸ġ�΢��������/�����߳��ȡ������
class ClCutFeature_Export MicroJointLeadLengthModifyCommand : public CommandBase
{
public:
	MicroJointLeadLengthModifyCommand(EditorData editorData, MicroJointListPtr pMicroJointList, BOOL bModLeadIn, double dNewLen);
	~MicroJointLeadLengthModifyCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	// Ҫ�޸ĵ�΢����������
	MicroJointListPtr m_pMicroJointList;

	// �޸����뻹��������
	BOOL m_bModLeadIn;

	// �µĳ��ȡ�
	double m_dNewLen;

	// �ϵĳ��ȡ�
	std::vector<double> m_dOldLenList;
};

END_CUTLEADER_NAMESPACE()
