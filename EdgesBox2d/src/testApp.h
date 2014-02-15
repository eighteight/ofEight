
#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"

// ------------------------------------------------- App
class testApp : public ofBaseApp {
	
public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	vector <ofPolyline>                 lines;
	ofxBox2d                            box2d;
	vector <ofPtr<ofxBox2dRect> >		circles;
	vector <ofPtr<ofxBox2dEdge> >       edges;
    int boxSize;
    
    void setupAudio();
    void audioIn(float * input, int bufferSize, int nChannels);
    ofSoundStream soundStream;
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int 	bufferCounter;
    int 	drawCounter;
    
    float smoothedVol;
    float scaledVol;
};
