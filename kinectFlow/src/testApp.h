#pragma once

#include "ofMain.h"
#include "ofxOpticalFlowFarneback.h"
#include "ofxKinect.h"
#include "ofxSyphon.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"
#include "ofxOsc.h"

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

    void setUI();
    void sendMessage(string address, float val);
    void sendMessage(ofRectangle rect);
		
    ofxKinect kinect;
    ofxSyphonServer syphonServer;
    ofxOpticalFlowLK flowSolver;
    ofTexture videoTexture;
    
    //contour finder
    void updateContours();
    void drawShapes();
    ofxCvContourFinder contourFinder;
    ofxCvGrayscaleImage grayDiff;
    std::vector<ofPolyline> contourPoly;
    std::vector<ofRectangle> rectangles;
    float smoothSize, smoothShape, shapeScale, depthThreshold, circleScale;
    float trackedCircles;
    int trackedCirclesMax, trackedCirclesInt;
    
    void averageCircles();
    
    //ui
    ofxUICanvas *gui;
    void guiEvent(ofxUIEventArgs &e);
    float near, far;
    bool showFlow, showShape, showRect, showDepth, showVideo;
    bool takeBackground;
    
    //osc
    ofxOscSender oscSender;
    
    //mesh
    ofMesh mesh;
    void drawPointCloud();
    
    vector <vector<ofPoint> > circleHistory;
    vector <ofPoint> circleAverage;
    float historySize;

};
