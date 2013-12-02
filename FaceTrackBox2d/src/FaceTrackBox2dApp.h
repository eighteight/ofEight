#pragma once

/*
 
 THIS IS A QUICK HACK BASED ON KYLE MCDONALD'S FACEOSC:
 
 CAMERA AND MOVIE PLAYER REMOVED
 
 LOADING IMAGES THROUGH A JSON FILE ARE PUT IN
 
 CREATED BY VLADIMIR GUSEV (EIGHTEIGHT@GITHUB, EIGHT_IO@TWITTER)
 
 DURING THE METROPOLITAN MUSEUM OF ART MEDIA LAB HACKATON ON NOV 16 2013
 
 THANKS TO CAMERON FOR LENDING HIS JSON PARSER CODE
 
 VERSION 0.1
 
 */


#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxOsc.h"
#include "ofxBox2d.h"

#define OBJECT_CROWD_SOURCER_DIR "/Users/eight/repos/objectcrowdsourcer/"

class FaceTrackBox2dApp : public ofBaseApp {
public:

	void clearBundle();
	template <class T>
	void addMessage(string address, T data);
	void sendBundle();

	void setup();
	void update();

	void draw();
	void keyPressed(int key);

	bool bPaused;

	int movieWidth, movieHeight;
	int sourceWidth, sourceHeight;

	string host;
	int port;
	ofxOscSender osc;
	ofxOscBundle bundle;
    
	ofxFaceTracker tracker;
	float scale;
	ofMatrix4x4 rotationMatrix;
	
	bool bDrawMesh;

    void reset();
    
    ofVideoGrabber cam;

    vector<string> urls;
    int numImages;
    int frameCount, oldFrameCount;
    
    //osc
    void updateOsc(ofxFaceTracker& tracker);
    //box2d
    ofxBox2d                            box2d;
    vector <ofPtr<ofxBox2dCircle> >     circles;
	vector <ofPtr<ofxBox2dPolygon> >	polyShapes;
    ofPolyline                          shape;
    bool                                breakupIntoTriangles;
    void updateBox2d();
    void drawBox2d();

};
