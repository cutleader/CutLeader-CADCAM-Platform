#pragma once

#include "boost/enable_shared_from_this.hpp"
#include "clPartLayoutCommon.h"
#include "ClData.h"
#include "Rect2D.h"
#include "SubjectBase.h"

DECLARE_CUTLEADER_CLASS(LineArc2DList)
DECLARE_CUTLEADER_CLASS(RepeatData)
DECLARE_CUTLEADER_CLASS(PartItem)
DECLARE_CUTLEADER_CLASS(PartPlacement)

BEGIN_CUTLEADER_NAMESPACE()

// “零件放置”对象。
// 注意：
//   1) “零件放置”对象中有一套完整的“加工特征数据”。
class ClPartLayout_Export PartPlacement : public ClData,
										public boost::enable_shared_from_this<PartPlacement>,
										public SubjectBase
{
public:
	PartPlacement(void);
	PartPlacement(PartItemPtr pPartItem, Matrix2D matrix = Matrix2D());
	PartPlacement(LONGLONG iPartPmtId, PartItemPtr pPartItem, const Matrix2D& matrix);

	~PartPlacement(void);

public: // implement IData interface.
	virtual BOOL IsModified() const override;
    virtual LONGLONG GetLatestModifyTimeStamp() const override;
	virtual DataProperty GetProperty() const override;
	virtual void UpdateProperty(DataPropItem& propItem) override;

	// clone the part pmt.
	// notes:
	//   1) 不克隆零件项，需要在这个函数外面处理。
	virtual IDataPtr Clone() const override;

public: // get/set functions
	PartItemPtr GetPartItem() const { return m_pPartItem; }
	void SetPartItem(PartItemPtr pPartItem) { m_pPartItem = pPartItem; }

	Matrix2D GetMatrix() const { return m_matrix; }
	void SetMatrix(const Matrix2D& mat) { m_matrix = mat; }

	BOOL IsGrid() const { return m_bGrid; }
	void IsGrid(BOOL bGrid) { m_bGrid = bGrid; }

	int GetRowCount() const { return m_iRowCount; }
	void SetRowCount(int iCount) { m_iRowCount = iCount; }

	int GetColCount() const { return m_iColCount; }
	void SetColCount(int iCount) { m_iColCount = iCount; }

	double GetSpacingX() const { return m_dSpacingX; }
	void SetSpacingX(double dSpacing) { m_dSpacingX = dSpacing; }

	double GetSpacingY() const { return m_dSpacingY; }
	void SetSpacingY(double dSpacing) { m_dSpacingY = dSpacing; }

	// get the base point of the part placement, actually this is the mouse-click point by user.
	// notes:
	//   1) the base point is the left-bottom point of the min-rect.
	Point2D GetBasePt() const;
	void SetBasePt(const Point2D& pt);

public: // other functions.
	// calc the base geoms of part pmt.
	// notes: 
	//  1) do not calc cut feature.
	//  2) if part pmt is a "huge grid", this function will cost some time.
	LineArc2DListPtr Calculate2DLineArcs() const;

	// get the distance between the part placement and the point.
	double DistanceTo(const Point2D& pt) const;

	// get the rectangle range of this part placement.
	// notes:
	//   1) do not include the cam features.
	Rect2D GetRectBox() const;

	// get the angle of the part placement.
	double GetAngle() const;

	// transform the part placement.
	void Transform(const Matrix2D& mat);

	/************************************************************************/
	// when move/rotate part pmt, call "CopyTo", when copy part pmt, call "Clone".
	
	// notes: we should not clone the pierce data.
	void CopyTo(PartPlacementPtr& pPartPlacement);
	void UpdateFrom(PartPlacementPtr pPartPlacement);
	/************************************************************************/

	/************************************************************************/
	// for part grid.

	// get the repeat data.
	// notes: 
	//   1) the matrix list do not have special order.
	//   2) the matrix list referred to the first instance.
	//   3) the count of matrix list equals (m_iRowCount*m_iColCount - 1)
	RepeatDataPtr GetRepeatData() const;

	// get the transform matrix of specified part instance.
	// notes:
	//   1) this matrix is NOT based on "m_matrix".
	Matrix2D GetInstMat(int iRow, int iCol) const;
	/************************************************************************/

	// whether the pattern loop exist.
	BOOL PatternLoopExist(LONGLONG iPatternLoopID) const;

	// get all repeat data.
	// notes: 
	//   1) the matrix list do not have special order.
	//   2) the matrix list do not referred to the first instance.
	//   3) the count of matrix list equals the count of part inst.
	RepeatDataPtr GetAllRepeatData() const;

	// 判断这个零件放置是不是在“pPartPlacement”前面。
	// 注：
	//  1) 用于排序中，当方向是向下时，越矮的排在前面。
	//  2) 用外接矩形来比较位置。
	BOOL BeforePartPmt(PartPlacementPtr pPartPlacement, POSITION_INFO iDir);

private:
	// reset some variables.
	void Reset();

private:
	// “零件项”。
	PartItemPtr m_pPartItem;

	// the transform matrix.
	// notes: 
	//   1) we adjust the part in this matrix, so the "[2][0], [2][1]" is NOT the base point.
	//   2) for Grid, we can use this Matrix to get the first instance.
	Matrix2D m_matrix;

	/************************************************************************/
	// these data are used for grid
	// notes:
	//   1) the grid is always X-axis and Y-axis direction.
	//   2) for grid, the angle in "m_matrix" will only affect the angle of each part inst, NOT the whole grid.

	// whether this part pmt is a grid.
	BOOL m_bGrid;

	// the column/row number.
	int	m_iRowCount;
	int	m_iColCount;

	// 阵列的行/列间距。
	double m_dSpacingX;
	double m_dSpacingY;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
