#pragma once

#include "clCutFeatureCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(MicroJointList)

BEGIN_CUTLEADER_NAMESPACE()

// �޸ġ�΢��������/�����߽Ƕȡ������
class ClCutFeature_Export MicroJointLeadAngleModifyCommand : public CommandBase
{
public:
	MicroJointLeadAngleModifyCommand(EditorData editorData, MicroJointListPtr pMicroJointList, BOOL bModLeadIn, double dNewAng);
	~MicroJointLeadAngleModifyCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	// Ҫ�޸ĵ�΢����������
	MicroJointListPtr m_pMicroJointList;

	// �޸����뻹��������
	BOOL m_bModLeadIn;

	// �µĽǶȡ�
	double m_dNewAng;

	// �ϵĽǶȡ�
	std::vector<double> m_dOldAngList;
};

END_CUTLEADER_NAMESPACE()
