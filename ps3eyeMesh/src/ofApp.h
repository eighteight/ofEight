#pragma once

#include "ofMain.h"
#include "ofxMacamPs3Eye.h"
#include "ofxSyphon.h"
#include "ofxObjLoader.h"
#include "ofxGui.h"
#include "ofxAnimatableOfPoint.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void drawCam(float factor);
        void updateCamPosition();
        void setCamLocations();
        void setPS3EyeGUI();
    
        void onAutoGainAndShutterChange(bool & value);
        void onGainChange(float & value);
        void onShutterChange(float & value);
        void onGammaChange(float & value);
        void onBrightnessChange(float & value);
        void onContrastChange(float & value);
        void onHueChange(float & value);
        void onLedChange(bool & value);
        void onFlickerChange(int & value);
        void onWhiteBalanceChange(int & value);
    
    ofEasyCam cam;
    ofVboMesh mesh;
    ofImage img;
    ofMesh sofa;
    
    ofxMacamPs3Eye      ps3eye;
    
    ofxSyphonServer     faceServer;
    ofxSyphonServer     camServer;

	ofxPanel gui, ps3EyeGui;
	ofxSlider<int> colorAlpha;
	ofxSlider<int> pointSize;
	ofxToggle isEasyCamMoving;
	ofxToggle roll;
    
    vector<ofVec3f> myLocations;
    float startTime;
    bool isCameraGrabbing;

    ofxAnimatableOfPoint animatable;
    int currentAnim;
    float currentRotation;
		
};
