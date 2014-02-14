#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"
#include "ofxOsc.h"
#include "ofxTuioServer.h"

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

    ofTexture videoTexture;
    
    //contour finder
    void updateContours();
    void drawShapes();
    ofxCvContourFinder contourFinder;
    ofxCvGrayscaleImage grayDiff;
    std::vector<ofPolyline> contourPoly;
    std::vector<ofRectangle> rectangles;
    float smoothSize, smoothShape, shapeScale, depthThreshold;
    
    //ui
    ofxUICanvas *gui;
    void guiEvent(ofxUIEventArgs &e);
    float near, far;
    bool showRect, showDepth, showVideo;
    
    //osc
    ofxOscSender oscSender;
    
    //mesh
    ofMesh mesh;
    void drawPointCloud();
};
