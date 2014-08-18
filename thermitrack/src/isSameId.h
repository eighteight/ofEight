#pragma once
#include "ofxThermitrackPoints.h"
class isSameId
{
public:
	ofxThermitrackPoint thisPoint;
	isSameId(ofxThermitrackPoint point)
	{
		thisPoint = point;
	}
	isSameId(void)
	{
	}

	~isSameId(void)
	{
	}

	bool operator() (const ThermitrackPoint& point) {
		return (point.id==thisPoint.id);
    }
};
