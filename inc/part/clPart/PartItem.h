#pragma once

#include "clPartCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(PartCadData)
DECLARE_CUTLEADER_CLASS(PartCamData)
DECLARE_CUTLEADER_CLASS(PartItem)

BEGIN_CUTLEADER_NAMESPACE()

// we keep PartCadData object and the relative CAM data in this class.
// notes:
//   1) for an Expert Item, we can only create one CAM data set.
//   2) we call "part1[scheme1]" and "part1[scheme2]" "brother" PartItem.

class ClPart_Export PartItem : public ClData
{
public:
	PartItem(void);
	PartItem(LONGLONG iPartID, CString strPartName, PartCadDataPtr pPartCadData, PartCamDataPtr pPartCamData);
	~PartItem(void);

public: // implement IData interface.
	virtual BOOL IsModified() const override;
    virtual LONGLONG GetLatestModifyTimeStamp() const override;

public: // get/set functions.
	void SetPartID(LONGLONG iPartID) { m_iPartID = iPartID; }
	LONGLONG GetPartID() const { return m_iPartID; }

	void SetPartName(CString strName) { m_strPartName = strName; }
	CString GetPartName() const { return m_strPartName; }

	void SetPartComment(CString strComment) { m_strPartComment = strComment; }
	CString GetPartComment() const { return m_strPartComment; }

	void SetCadData(PartCadDataPtr pPartCadData) { m_pPartCadData = pPartCadData; }
	PartCadDataPtr GetCadData() const { return m_pPartCadData; }

	void SetCamData(PartCamDataPtr pPartCamData) { m_pPartCamData = pPartCamData; }
	PartCamDataPtr GetCamData() const { return m_pPartCamData; }

public:
	// if two PartItems have same cad and cam data, we think they are same.
	BOOL SameAs(PartItemPtr pPartItem) const;

private:
	// part ID.
	LONGLONG m_iPartID;

	// part name.
	CString m_strPartName;

	// part comment.
	CString m_strPartComment;

	// the part cad data object.
	PartCadDataPtr m_pPartCadData;

	// the cam data of the part
	PartCamDataPtr m_pPartCamData;
};

END_CUTLEADER_NAMESPACE()
