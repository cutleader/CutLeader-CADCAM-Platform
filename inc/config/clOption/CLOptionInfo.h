#pragma once

#include "clOptionCommon.h"

DECLARE_CUTLEADER_CLASS(ProductParam)
DECLARE_CUTLEADER_CLASS(ImpExpConfig)
DECLARE_CUTLEADER_CLASS(DisplayParam)
DECLARE_CUTLEADER_CLASS(PartEditParam)
DECLARE_CUTLEADER_CLASS(SheetEditParam)
DECLARE_CUTLEADER_CLASS(ConcealConfig)
DECLARE_CUTLEADER_CLASS(CLOptionInfo)

BEGIN_CUTLEADER_NAMESPACE()

class ClOption_Export CLOptionInfo
{
public:
	CLOptionInfo(void);
	~CLOptionInfo(void);

public: // get/set functions.
	// the data which can config on the UI.
	ProductParamPtr GetProductParam() const { return m_pProductParam; }
	ImpExpConfigPtr GetImpExpConfig() const { return m_pImpExpConfig; }
	DisplayParamPtr GetDisplayParam() const { return m_colorStyleInfoPtr; }
	PartEditParamPtr GetPartEditParam() const { return m_pPartEditParam; }
	SheetEditParamPtr GetSheetEditParam() const { return m_displayInfoPtr; }

	// the data which can NOT be set on the UI.
	void SetConcealConfig(ConcealConfigPtr pConcealConfig) { m_pConcealConfig = pConcealConfig; }
	ConcealConfigPtr GetConcealConfig() const { return m_pConcealConfig; }

public:
	static void Init(CLOptionInfoPtr pTmpCLOptionInfo) { pCLOptionInfo = pTmpCLOptionInfo; }
	static CLOptionInfoPtr GetInstance() { return pCLOptionInfo; }

private:
	// the singleton object
	static CLOptionInfoPtr pCLOptionInfo;

	// the data which can config on the UI.
	ProductParamPtr m_pProductParam;
	ImpExpConfigPtr m_pImpExpConfig;
	DisplayParamPtr m_colorStyleInfoPtr;
	PartEditParamPtr m_pPartEditParam;
	SheetEditParamPtr m_displayInfoPtr;

	// the data which can NOT be configed on the UI.
	ConcealConfigPtr m_pConcealConfig;
};

END_CUTLEADER_NAMESPACE()
