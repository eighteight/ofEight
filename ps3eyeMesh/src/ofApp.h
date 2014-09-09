#pragma once

#include "ofMain.h"
#include "ofxMacamPs3Eye.h"
#include "ofxSyphon.h"
#include "ofxObjLoader.h"
#include "ofxFog.h"
#include "ofxGui.h"

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
    
    ofEasyCam cam;
    ofVboMesh mesh;
    ofImage img;

    ofxMacamPs3Eye      ps3eye;
    ofxSyphonServer     faceServer;
    
    ofMesh sofa;
    
    ///////
    ofxFog fog;
    ofFloatColor fogColor;
    
    ofShader fogShader;
    ofLight light;
    ofFloatColor lightAmbient;
    ofFloatColor lightDiffuse;
    ofVec3f lightPosition;
    ofImage color;
    ofxPanel gui;
    bool bHide;
    
    ofxFloatSlider density;
    ofxFloatSlider fog_start;
    ofxFloatSlider fog_end;
    ofxFloatSlider r, g, b;
    ofxIntSlider fogFilter;
    
    
    ////area light
    ofLight areaLight;
    ofPlanePrimitive plane;
    ofEasyCam camera;
    ofMaterial materialPlane;

		
};
