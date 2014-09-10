#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxGui.h"
#include "ofxFog.h"

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
        void setNormals( ofMesh &mesh );
		ofLight areaLight;
		ofPlanePrimitive plane;
		ofMaterial materialPlane;
        ofVideoGrabber vidGrabber;
    
    
        ofEasyCam cam;
        ofVboMesh mesh;
        ofImage img;

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
};
