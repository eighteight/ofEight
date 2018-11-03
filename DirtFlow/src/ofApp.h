#pragma once

#include "ofMain.h"
#include "ofxInkSim.h"
#include "GUI.h"
#include "ofxSyphon.h"
#include "ofxPSBlend.h"
#include "SampleBrush.h"


class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
private:
    void setBrush(int);
    GUI gui;
    
    ofxInkSim dirtSim;
    bool isControlKey;
    vector<ofFile> brushList;
    string currDrawMode;
    int currBrush;
    ofImage base;
    ofxPSBlend psBlend;
    int blendMode;
    
    bool isNegative = false;
    
    ofFbo fbo;
    vector<ofTexture> brushes;
    
    ofxSyphonServer syphonServer;
    
    SampleBrush brush;
};
