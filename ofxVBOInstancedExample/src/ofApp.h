#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxVboMeshInstanced.h"

class ofApp : public ofBaseApp{

	public:
	bool	isShaderDirty;
	
	ofFbo	mFboForce[2];
	ofFbo	mFboLength[2];
    
	ofShader* mShdForce;
	ofShader* mShdLength;
	ofShader* mShdInstanced;
    
	bool	mPingPong;
    
	ofxVboMeshInstanced mMshCube;
	
	ofEasyCam	mCamMain;
	
	ofLight	mLigDirectional;
	ofMaterial mMatMainMaterial;
	
	ofxPanel	mPnlMain;
	map<string, ofParameter<float> > mPrmFloat;
	map<string, ofParameter<bool> > mPrmBool;
	
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
		
};
