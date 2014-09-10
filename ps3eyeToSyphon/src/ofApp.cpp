#include "ofApp.h"

#define WIDTH 640
#define HEIGHT 480
//--------------------------------------------------------------
void ofApp::setup() {
    ofSetVerticalSync(true);

	ps3eye.listDevices();
	if (!ps3eye.initGrabber(WIDTH,HEIGHT, false)){
        cout<<"CAM not initialized"<<endl;
    };

    faceServer.setName("PS3 EYE");
    ofSetWindowShape(WIDTH,HEIGHT);
    
    
}

//--------------------------------------------------------------
void ofApp::update() {
    ps3eye.update();
	if(ps3eye.isFrameNew()) {
        //faceServer.publishTexture(ps3eye.getTexture());
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
    ps3eye.draw(0,0,WIDTH,HEIGHT);
    faceServer.publishScreen();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
   	switch(key){
        case OF_KEY_UP:
            //areaLight.move(0,10,0);
            break;
        case OF_KEY_DOWN:
            //areaLight.move(0,-10,0);
            break;
        case OF_KEY_LEFT:
            //areaLight.rotate(1,0,0,1);
            break;
        case OF_KEY_RIGHT:
            //areaLight.rotate(-1,0,0,1);
            break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}