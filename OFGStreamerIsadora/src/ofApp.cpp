#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    
    fingerMovie.load("rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov");
	fingerMovie.play();
}


void ofApp::exit(){
}


//--------------------------------------------------------------
void ofApp::update(){
    
    fingerMovie.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetHexColor(0xFFFFFF);
    
    fingerMovie.draw(20,20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
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
