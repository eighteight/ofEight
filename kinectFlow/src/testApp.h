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
		
    ofxKinect kinect;
    ofxSyphonServer syphon;
    ofxOpticalFlowLK flowSolver;
    
    //contour finder
    ofxCvColorImage colorImg;
    ofxCvContourFinder contourFinder;
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayBg;
    ofxCvGrayscaleImage grayDiff;
    
    std::vector<ofPolyline> contourPoly;
};
