#pragma once

#include "clExpertLibCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(MicroJointRangeInfo)

BEGIN_CUTLEADER_NAMESPACE()

typedef enum tagMJ_POSITION
{
	MICRO_NONE		= 0,

	ONE_BOTTOM		= 1,
	ONE_RIGHT		= 2,
	ONE_TOP			= 3,
	ONE_LEFT		= 4,

	TWO_VERTICAL	= 5,
	TWO_HORIZONTAL	= 6,

	THREE_BOTTOM	= 7,
	THREE_RIGHT		= 8,
	THREE_TOP		= 9,
	THREE_LEFT		= 10,

	FOUR_FULL		= 11,

	SIX_DOWN		= 12,
	SIX_UP			= 13

} MJ_POSITION;

// a MicroJointRangeInfo object represent a half data of a tab.
// notes:
//   1) a MicroJointRangeInfo object occupy one row in table expLib_bmjrangeinfo.
class ClExpertLib_Export MicroJointRangeInfo : public ClData
{
public:
	MicroJointRangeInfo();
	~MicroJointRangeInfo();

public: // implements IData interface.
	IDataPtr Clone() const override;

public: // get/set functions.
	void IsHole(BOOL bHole) { m_bHole = bHole; }
	BOOL IsHole() const { return m_bHole; }

	void SetJointWidth(double dJointWidth) { m_dJointWidth = dJointWidth; }
	double GetJointWidth() const { return m_dJointWidth; }

	// in this function, we also should set "m_MicroJointPos".
	void SetQuantity(int iQuantity);
	int GetQuantity() const { return m_iQuantity; }

	void SetMicroJointPos(MJ_POSITION iMicroJointPos) { m_iMicroJointPos = iMicroJointPos; }
	MJ_POSITION GetMicroJointPos() const { return m_iMicroJointPos; }

	void SetRangeX(double dRangeX) { m_dRangeX = dRangeX; }
	double GetRangeX() const { return m_dRangeX; }

	void SetRangeY(double dRangeY) { m_dRangeY = dRangeY; }
	double GetRangeY() const { return m_dRangeY; }

public: // other functions.
	// get the angles by the "m_iMicroJointPos"
	// notes: 
	//   1) this function return a list of angle, actually the angles follow the CCW direction.
	std::vector<double> GetAngles();

private:
	// 1: hole, 0: perimeter
	BOOL m_bHole;

	// the width of the micro joint.
	double m_dJointWidth;

	// notes:
	//   1) Max value is 4 for hole range.
	//   2) Max value is 6 for perimeter range.
	int m_iQuantity;

	// the positions of the micro joint.
	MJ_POSITION m_iMicroJointPos;

	// the range size.
	double m_dRangeX;
	double m_dRangeY;
};

END_CUTLEADER_NAMESPACE()
