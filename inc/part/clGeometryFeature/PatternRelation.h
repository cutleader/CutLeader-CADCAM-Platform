#pragma once

#include "clGeometryFeatureCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(IPattern)

BEGIN_CUTLEADER_NAMESPACE()

// the relation types between the patterns.
enum PatternRelationType
{
	// e.g. the control pattern within the array pattern
	PatternRelation_Control	= 100,

	// e.g. the array pattern.
	PatternRelation_BeControl	= 101,

};

// this class records the relations between patterns. e.g. LinePattern 1 works as the controller of LineArrayPattern 2, in database, we have such structure:
//       *****(the unique ID of the relation)        ID of pattern1   ID of pattern 2       relation type
//       *****(the unique ID of the relation)        ID of pattern2   ID of pattern 1       relation type
// we also have the corresponding structure in memory:
//       m_iID                                       m_iParentID	  m_pTargetPattern           m_iPatternRelationType   
//       m_iID                                       m_iParentID	  m_pTargetPattern           m_iPatternRelationType
class ClGeometryFeature_Export PatternRelation : public ClData
{
public:
	PatternRelation(LONGLONG iParentID, IPatternPtr targetPat, PatternRelationType iRelationType);
	PatternRelation(LONGLONG iID, LONGLONG iParentID, IPatternPtr targetPat, PatternRelationType iRelationType); // use this function to load relations from the database.
	virtual ~PatternRelation(void);

public: // get/set functions.
	IPatternPtr GetTargetPattern() const { return m_pTargetPattern; }
	PatternRelationType GetPatternRelationType() const { return m_iPatternRelationType; }

private:
	// the target pattern of the relation.
	IPatternPtr m_pTargetPattern;

	// the type of the relation
	// notes:
	//   1) in "grid" pattern, this will be "PatternRelation_Control", that means "m_pTargetPattern" controls the "grid" pattern.
	PatternRelationType m_iPatternRelationType;
};

END_CUTLEADER_NAMESPACE()
