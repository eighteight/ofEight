#pragma once

#include "ofMain.h"
#include "ofxOpticalFlowFarneback.h"
#include "ofxKinect.h"
#include "ofxSyphon.h"
#include "ofxCvContourFinder.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void updateContours();
        void drawBoundingRects();
		
    ofxKinect kinect;
    ofxSyphonServer syphonServer;
    ofxOpticalFlowLK flowSolver;
    
    //contour finder

    ofxCvContourFinder contourFinder;
    ofxCvGrayscaleImage grayDiff;
    std::vector<ofPolyline> contourPoly;
    std::vector<ofRectangle> rectangles;
};
