#pragma once

#include "clNestJobCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(IMaterialSize)

BEGIN_CUTLEADER_NAMESPACE()

// “排版材料”对象。
class ClNestJob_Export NestMaterial : public ClData
{
public:
	NestMaterial(void);
	NestMaterial(IMaterialSizePtr pMaterialSize);

	~NestMaterial(void);

public: // set/get functions.
	void SetMatSize(IMaterialSizePtr pMaterialSize) { m_pMatSize = pMaterialSize; }
	IMaterialSizePtr GetMatSize() const { return m_pMatSize; }

private:
	IMaterialSizePtr m_pMatSize;
};

END_CUTLEADER_NAMESPACE()
