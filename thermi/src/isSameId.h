#pragma once
#include "ofxThermitrackPoint.h"
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

	bool operator() (const ofxThermitrackPoint& point) {
		return (point.id==thisPoint.id);
    }
};
