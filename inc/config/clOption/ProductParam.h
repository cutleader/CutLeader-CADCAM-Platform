#pragma once

#include "clUILibCommon.h"
#include "clOptionCommon.h"
#include "ClData.h"
#include "MultiLanguageMgr.h"


BEGIN_CUTLEADER_NAMESPACE()


// 产品基本设置。
class ClOption_Export ProductParam : public ClData
{
public:
	ProductParam(void);
	~ProductParam(void);

public: // get/set functions.
	void SetDimUnitType(DIM_UNIT_TYPE dimUnit) { m_dimUnit = dimUnit; }
	DIM_UNIT_TYPE GetDimUnitType() const { return m_dimUnit; }

	void SetLang(LANG_TYPE lang) { m_lang = lang; }
	LANG_TYPE GetLang() const { return m_lang; }

	void SetBkLocation(CString strBkLocation) { m_strBkLocation = strBkLocation; }
	CString GetBkLocation() const { return m_strBkLocation; }

	void SetDBBackupDay(int iDBBackupDay) { m_iDBBackupDay = iDBBackupDay; }
	int GetDBBackupDay() const { return m_iDBBackupDay; }

	void SetLogBackupSize(int iLogBackupSize) { m_iLogBackupSize = iLogBackupSize; }
	int GetLogBackupSize() const { return m_iLogBackupSize; }

	void IsUseInside(BOOL b) { m_bInside = b; }
	BOOL IsUseInside() const { return m_bInside; }

	void IsShowHelpVideo(BOOL b) { m_bShowHelpVideo = b; }
	BOOL IsShowHelpVideo() const { return m_bShowHelpVideo; }

	void IsShowStartPage(BOOL b) { m_bShowStartPage = b; }
	BOOL IsShowStartPage() const { return m_bShowStartPage; }

    void IsEnableReset4ChainSeq(BOOL bEnableReset4ChainSeq) { m_bEnableReset4ChainSeq = bEnableReset4ChainSeq; }
    BOOL IsEnableReset4ChainSeq() const { return m_bEnableReset4ChainSeq; }

private:
	// the dimension unit.
	DIM_UNIT_TYPE m_dimUnit;

	// the language.
	LANG_TYPE m_lang;

	// the backup location.
	CString m_strBkLocation;

	// days interval for backup database.
	int m_iDBBackupDay;

	// size limitation for backup log file.
	int m_iLogBackupSize;

	// whether the product is used internal.
	BOOL m_bInside;

	// whether show the "help video" dialog.
	BOOL m_bShowHelpVideo;

	// whether show start page.
	BOOL m_bShowStartPage;

    // 定制功能，是否启用连割工序的复位功能
    BOOL m_bEnableReset4ChainSeq;
};

END_CUTLEADER_NAMESPACE()
