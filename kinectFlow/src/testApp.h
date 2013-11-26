#pragma once

#include "ofMain.h"
#include "ofxOpticalFlowFarneback.h"
#include "ofxKinect.h"
#include "ofxSyphon.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"
#include "ofxOpenCv2.h"

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
    void setUI();
		
    ofxKinect kinect;
    ofxSyphonServer syphonServer;
    ofxOpticalFlowLK flowSolver;
    
    //contour finder
    ofxCvContourFinder contourFinder;
    ofxCvGrayscaleImage grayDiff;
    std::vector<ofPolyline> contourPoly;
    std::vector<ofRectangle> rectangles;
    
    //ui
    ofxUICanvas *gui;
    void guiEvent(ofxUIEventArgs &e);
    float near, far;
    bool showFlow, showShape, showRect, showDepth;
};
