#include "FaceTrackBox2dApp.h"

static bool shouldRemove(ofPtr<ofxBox2dBaseShape>shape) {
    return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape.get()->getPosition());
}

using namespace ofxCv;
using namespace cv;


void FaceTrackBox2dApp::setup() {
	ofSetVerticalSync(true);
    bPaused = false;
	movieWidth = 1024;
    movieHeight = 768;
    
    cam.initGrabber(movieWidth, movieHeight);
    
    ofSetWindowTitle("FaceTrackBox2d");
    ofSetWindowShape(movieWidth, ofGetScreenHeight());
    
	tracker.setup();
    
	host = "localhost";
	port = 8338;
	osc.setup(host, port);
    
	osc.setup(host, port);

    bDrawMesh = true;

    // Box2d
	box2d.init();
	box2d.setGravity(0, 20);
	box2d.createGround();
	box2d.setFPS(30.0);
    box2d.setBounds(ofPoint(0,0), ofPoint(movieWidth,movieHeight));
}

void FaceTrackBox2dApp::updateBox2d(){
    // add some circles every so often
	if((int)ofRandom(0, 10) == 0) {
        ofPtr<ofxBox2dCircle> circle = ofPtr<ofxBox2dCircle>(new ofxBox2dCircle);
        circle->setPhysics(0.3, 0.5, 0.1);
		circle->setup(box2d.getWorld(), (ofGetWidth()/2)+ofRandom(-20, 20), -20, ofRandom(5, 10));
		circles.push_back(circle);
	}
	
    // remove shapes offscreen
    ofRemove(circles, shouldRemove);
    // ofRemove(polyShapes, shouldRemove);
    
    
	box2d.update();
}

void FaceTrackBox2dApp::drawBox2d() {
	// some circles :)
	for (int i=0; i<circles.size(); i++) {
		ofFill();
		ofSetHexColor(0xc0dd3b);
		circles[i].get()->draw();
	}
	
	ofSetHexColor(0x444342);
	ofFill();
	shape.draw();
	
	ofSetHexColor(0x444342);
	ofNoFill();
	for (int i=0; i<polyShapes.size(); i++) {
		polyShapes[i].get()->draw();
        
        ofCircle(polyShapes[i].get()->getPosition(), 3);
	}

	// some debug information
	string info = "Draw a shape with the mouse\n";
	info += "Press 1 to add some circles\n";
	info += "Press c to clear everything\n";
	info += "Press t to break object up into triangles/convex poly: "+string(breakupIntoTriangles?"true":"false")+"\n";
    info += "Total Bodies: "+ofToString(box2d.getBodyCount())+"\n";
	info += "Total Joints: "+ofToString(box2d.getJointCount())+"\n\n";
	info += "FPS: "+ofToString(ofGetFrameRate())+"\n";
    ofSetHexColor(0x444342);
	ofDrawBitmapString(info, 10, 15);
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
    
    drawBox2d();
    
    
	if(bPaused) {
		ofSetColor(255, 0, 0);
		ofDrawBitmapStringHighlight( "paused", 10, 32);
	}
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
            polyShapes.clear();
            if(tracker.getFound()) {
                ofPolyline outline = tracker.getImageFeature(ofxFaceTracker::JAW);
                ofPtr<ofxBox2dPolygon> poly = ofPtr<ofxBox2dPolygon>(new ofxBox2dPolygon);
                poly.get()->addVertices(outline.getVertices());
                poly.get()->setPhysics(1.0, 0.3, 0.3);
                poly.get()->create(box2d.getWorld());
                polyShapes.push_back(poly);
                
                updateOsc(tracker);

                
            } else {
                addMessage("/found", 0);
            }
        } catch(...)   {
            return;
        }
    }
    rotationMatrix = tracker.getRotationMatrix();
    
    updateBox2d();
}

void FaceTrackBox2dApp::updateOsc(ofxFaceTracker& tracker){
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
