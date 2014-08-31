#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"


#include "ofxDelaunay.h"
#include "ofxPostProcessing.h"
#include "ofxObjLoader.h"
#include "ofxSyphon.h"

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

        ofxPanel panel;
    	bool showGui;
        ofxSlider <int> minDistance;
        ofxSlider <int> maxDistance;
    	ofxSlider<int> pointSkip;
        ofxSlider<float> noiseAmount;
    	ofxToggle useRealColors;
    	ofxSlider<int> colorAlpha;
    
        ofxKinectV2 kinect;
        ofTexture texDepth;
        ofTexture texRGB;
    
    
        ofEasyCam cam;
        
        ofxPostProcessing postFx;
        ofMesh convertedMesh;
        ofMesh wireframeMesh;
        bool isSaving;
    
        ofxDelaunay del;
        
        ofImage blob;
    
        ofxSyphonServer syphonServerDepth;
        ofxSyphonServer syphonServerRGB;
};
