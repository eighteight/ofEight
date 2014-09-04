#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"


#include "ofxDelaunay.h"
#include "ofxPostProcessing.h"
#include "ofxObjLoader.h"
#include "ofxSyphon.h"

#include "ofxBullet.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void drawPointCloud();
        void addPoint(float x, float y, float z);

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
        
        ofxPostProcessing postFx;
        bool isSaving;
    
        ofxSyphonServer syphonServerDepth;
        ofxSyphonServer syphonServerRGB;
        ofxSyphonServer syphonServerScreen;
    
        ofEasyCam easyCam;
        int near;
        int step;
        int nearThresh;
        int farThresh;
        int pointSize;
        bool wall;
        
        ofVbo vbo;
        vector <ofVec3f> sizes;
        vector <ofVec3f> points;
		ofShader shader;
        ofTexture pointImg;
    
    
        ofxBulletWorldRigid			world;
        ofxBulletBox				ground;
        
        ofxBulletSphere*			sphere;
        ofxBulletBox*				box;
        ofxBulletCone*				cone;
        ofxBulletCapsule*			capsule;
        ofxBulletCylinder*			cylinder;
    
};
