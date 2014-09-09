#pragma once

#include "ofMain.h"
#include "ofxMacamPs3Eye.h"
#include "ofxSyphon.h"
#include "ofxObjLoader.h"

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

		
};
