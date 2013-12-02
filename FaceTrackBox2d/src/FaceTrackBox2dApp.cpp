#include "FaceTrackBox2dApp.h"

using namespace ofxCv;
using namespace cv;


void FaceTrackBox2dApp::setup() {
	ofSetVerticalSync(true);
    bPaused = false;
	movieWidth = 1024;
    movieHeight = 768;
    
    cam.initGrabber(movieWidth, movieHeight);
    
    ofSetWindowTitle("FaceMetMuseum");
    ofSetWindowShape(movieWidth, ofGetScreenHeight());
    
	tracker.setup();
    
	host = "localhost";
	port = 8338;
	osc.setup(host, port);
    
	osc.setup(host, port);

    bDrawMesh = true;
}

void FaceTrackBox2dApp::clearBundle() {
	bundle.clear();
}

template <>
void FaceTrackBox2dApp::addMessage(string address, ofVec3f data) {
	ofxOscMessage msg;
	msg.setAddress(address);
	msg.addFloatArg(data.x);
	msg.addFloatArg(data.y);
	msg.addFloatArg(data.z);
	bundle.addMessage(msg);
}

template <>
void FaceTrackBox2dApp::addMessage(string address, ofVec2f data) {
	ofxOscMessage msg;
	msg.setAddress(address);
	msg.addFloatArg(data.x);
	msg.addFloatArg(data.y);
	bundle.addMessage(msg);
}

template <>
void FaceTrackBox2dApp::addMessage(string address, float data) {
	ofxOscMessage msg;
	msg.setAddress(address);
	msg.addFloatArg(data);
	bundle.addMessage(msg);
}

template <>
void FaceTrackBox2dApp::addMessage(string address, int data) {
	ofxOscMessage msg;
	msg.setAddress(address);
	msg.addIntArg(data);
	bundle.addMessage(msg);
}

void FaceTrackBox2dApp::sendBundle() {
	osc.sendBundle(bundle);
}

void FaceTrackBox2dApp::update() {
    cam.update();
	if(cam.isFrameNew()) {
        
        try{
            tracker.update(toCv(cam));
            
            if(tracker.getFound()) {
                addMessage("/found", 1);
                
                ofVec2f position = tracker.getPosition();
                addMessage("/pose/position", position);
                scale = tracker.getScale();
                addMessage("/pose/scale", scale);
                ofVec3f orientation = tracker.getOrientation();
                addMessage("/pose/orientation", orientation);
                
                addMessage("/gesture/mouth/width", tracker.getGesture(ofxFaceTracker::MOUTH_WIDTH));
                addMessage("/gesture/mouth/height", tracker.getGesture(ofxFaceTracker::MOUTH_HEIGHT));
                addMessage("/gesture/eyebrow/left", tracker.getGesture(ofxFaceTracker::LEFT_EYEBROW_HEIGHT));
                addMessage("/gesture/eyebrow/right", tracker.getGesture(ofxFaceTracker::RIGHT_EYEBROW_HEIGHT));
                addMessage("/gesture/eye/left", tracker.getGesture(ofxFaceTracker::LEFT_EYE_OPENNESS));
                addMessage("/gesture/eye/right", tracker.getGesture(ofxFaceTracker::RIGHT_EYE_OPENNESS));
                addMessage("/gesture/jaw", tracker.getGesture(ofxFaceTracker::JAW_OPENNESS));
                addMessage("/gesture/nostrils", tracker.getGesture(ofxFaceTracker::NOSTRIL_FLARE));
                
            } else {
                addMessage("/found", 0);
            }
        } catch(...)   {
            return;
        }
    }
		rotationMatrix = tracker.getRotationMatrix();
}

void FaceTrackBox2dApp::draw() {
	ofSetColor(255);
	cam.draw(0, 0);

	if(tracker.getFound()) {

		if(bDrawMesh) {
			ofSetLineWidth(1);
            
			tracker.getImageMesh().drawWireframe();
		
			ofPushView();
			ofSetupScreenOrtho(sourceWidth, sourceHeight, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
			ofVec2f pos = tracker.getPosition();
			ofTranslate(pos.x, pos.y);
			applyMatrix(rotationMatrix);
			ofScale(10,10,10);
			ofDrawAxis(scale);
			ofPopView();
            ofDrawBitmapStringHighlight("beautiful face", 10, 20);
		}
	} else {
		ofDrawBitmapStringHighlight("no face here", 10, 20);
	}
    
	if(bPaused) {
		ofSetColor(255, 0, 0);
		ofDrawBitmapStringHighlight( "paused", 10, 32);
	}
}

void FaceTrackBox2dApp::keyPressed(int key) {
	switch(key) {
		case 'r':
			tracker.reset();
			break;
		case 'm':
			bDrawMesh = !bDrawMesh;
			break;
		case 'p':
			bPaused = !bPaused;
			break;
		case OF_KEY_UP:
			break;
		case OF_KEY_DOWN:
			break;
        case 358: //left
            frameCount++;
            frameCount = frameCount >= numImages ? numImages - 1: frameCount;
            break;
        case 356: //right
            frameCount--;
            frameCount = frameCount < 0 ? 0 : frameCount;
            break;
	}
}
